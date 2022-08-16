#include<Windows.h>
#include<fstream>
#include"CircularBuffer.h"
#include<sstream>
#include<thread>
#include<string>
#include<cstring>
#include"commandutil.h"
#include<stack>
#include<array>
#include"calculator.h"
#include"strtokeyh.h"
#include"vkex.h"
#include"repeatablequeue.h"
#include<Psapi.h>

HOOKPROC hook=0;
HHOOK hhook=0;
HINSTANCE dll=0;
HWND hwnd;
bool addNL = 0;
int lastcmdindex=0;
auto logfile = R"(c:\documents\kblog.txt)";
auto rawlogfile = R"(c:\documents\kblograw.bin)";
auto savedir = std::string(R"(c:\documents\saves\)");



CircularBuffer<wchar_t, CMDLEN>command;
using cfptr = void(*)(void);
struct Command {const wchar_t*id; cfptr f; };
void log_vk(WPARAM);
bool is_vk(WPARAM);
LRESULT CALLBACK proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
void log(const wchar_t*, bool ignoreAddNL = 0);
void lograw(const char*,UINT, WPARAM, LPARAM);
struct UWPLP { UINT u; WPARAM wp; LPARAM lp; };
UWPLP readraw(const char*,int index);
int getcurrrawindex(const char*);

void exit() { PostMessage(hwnd, WM_QUIT, 1, 0); };
void forceclose() {
	DWORD PID;
	HWND h = GetForegroundWindow();
	GetWindowThreadProcessId(h, &PID);
	char buffer[MAX_PATH]{};
	auto handle=OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, PID);
	if (GetModuleFileNameExA(handle, 0, buffer, MAX_PATH)) {
		for (int i = 0; buffer[i];i++)if (isupper(buffer[i]))buffer[i] += 'a' - 'A';
		if (std::string(buffer).find("explorer.exe") != std::string::npos)return;
	}

	std::ostringstream ss;
	ss << "Taskkill /PID " << PID << " /F";
	system(ss.str().c_str());
}
void close() {
	PostMessageA(GetForegroundWindow(), WM_CLOSE, 0, 0);
}
void cmd() {
	std::thread([]() {
		system("start \"cmd\" /D %windir%\\system32 cmd.exe");
	}).detach();
}
struct recorder {
	int start = -1;
	int end = -1;
	int scope = 0;
	const char*source = rawlogfile;
	~recorder() { if (source != rawlogfile)delete[]source; }
};
int repeatcount = 1;
int recording = 0;
int replaying = 0;
std::stack<recorder>rec_;
std::stack<rpqueue<INPUT>> sendInputQueue;
#define recend rec_.top().end
#define recstart rec_.top().start
#define recsource rec_.top().source
#define recscope rec_.top().scope
void end() {
	if (!rec_.size())return;
	int topscope = recscope;
	if(recording)
		while (rec_.size()&&recend > recstart) rec_.pop();
	else if (replaying) {
		while (rec_.size() && recscope >= topscope) rec_.pop();
		replaying--;
	}

	if (!rec_.size())return;
	if (rec_.size() && recend == -1) {
		if (topscope != recscope) {
			recend = getcurrrawindex(rawlogfile);
		}
		else {
			recend = lastcmdindex;
		}recording--;
	}
}
void prev() {
	if (!rec_.size())return;
	if (recend > recstart) rec_.pop();
}

void load();
void rep() {
	int repeatcount = loadintparam(command);//optional parameter to set the repeatcount for this rep only
	if (repeatcount == 0)repeatcount = ::repeatcount;
	int loadsuccess = rec_.size();
	load();
	loadsuccess = (loadsuccess != rec_.size());
	if (!rec_.size())return;
	if (recend == -1)end();
	int m_ = recend;
	int m = m_ - recstart;
	if (m <= 0)return;
	replaying++;

	INPUT out{};
	out.type = INPUT_KEYBOARD;

	out.ki.wVk = VK_REPLAYING;
	out.ki.dwFlags = KEYEVENTF_KEYUP;
	sendInputQueue.push(out);

	INPUT*b = new INPUT[m];
	for (int i = 0; i < m; i++) {
		auto a = readraw(recsource, i + recstart);
		out.ki.wVk = a.wp;
		out.ki.wScan = (a.lp >> 16) & 0xFF;
		out.ki.dwFlags = a.u == WM_KEYUP ? KEYEVENTF_KEYUP : 0;
		b[i]=out;
	}
	sendInputQueue.push(rpqueue<INPUT>(m, repeatcount, b));

	out.ki.wVk = VK_REPLAYING;
	out.ki.dwFlags = 0;
	out.ki.wScan = 0;
	sendInputQueue.push(out);

	if (loadsuccess)prev();//unload
	repnext();
}
void rec() {
	recording++;
	rec_.emplace(recorder());
	recsource = rawlogfile;
	recstart = getcurrrawindex(rawlogfile);
	recend = -1;
	recscope = recording+replaying;
}
void test() {
	MessageBox(0, L"kblistener is running", L"running", MB_ICONINFORMATION | MB_OK);
}
void set() {
	repeatcount = loadintparam(command);
}

void load() {
	auto str = loadstrparam(command);
	if (str.empty())return;
	char*file = new char[savedir.size()+str.length() + 5]{};

	memcpy_s(file, savedir.size(), savedir.c_str(), savedir.size());//copy dir
	memcpy_s(file + savedir.size(), str.length(), str.c_str(), str.length());//copy file name
	memcpy_s(savedir.size() + file + str.length(), 4, ".bin",4);//copy file extension
	if (!std::ifstream(file))return;//save does not exist
	rec_.emplace(recorder());
	recsource = file;
	recstart = 0;
	recend = getcurrrawindex(recsource);
	recscope = recording;
}
void save() {
	if (recend==-1)end();
	if (recend <= recstart)return;
	auto file=loadstrparam(command);
	std::ifstream in(recsource, std::ios::binary);
	std::ofstream out(savedir+file+".bin", std::ios::trunc | std::ios::binary);
	in.seekg(recstart * 6);
	if (in&&out) {
		char buffer[6];
		for (int i = recend - recstart; i--;) {
			in.read(buffer, 6);
			out.write(buffer, 6);
		}
		in.close();
		out.close();
	}
}
void setcmdindex() {
	lastcmdindex = getcurrrawindex(rawlogfile)-1;
}
void reptxt(const std::string&str) {
	if (!str.length())return;

	INPUT out{};
	out.type = INPUT_KEYBOARD;

	out.ki.wVk = VK_REPLAYING;
	out.ki.dwFlags = KEYEVENTF_KEYUP;
	sendInputQueue.push(out);

	for (int i = str.length(); i--;) {
		char in = str[i];
		auto vklist = type(in);

		out.ki.dwFlags = KEYEVENTF_KEYUP;
		for (int vk : vklist) {
			out.ki.wVk = vk;
			sendInputQueue.push(out);
		}
		out.ki.dwFlags = 0;
		for (int i = vklist.size(); i--;) {
			out.ki.wVk = vklist[i];
			sendInputQueue.push(out);
		}
	}
	out.ki.wVk = VK_REPLAYING;
	out.ki.dwFlags = 0;
	out.ki.wScan = 0;
	sendInputQueue.push(out);
	
	repnext();
};
void num() {
	long double out=loadintparam(command);
	std::ostringstream ss;
	ss << out;
	std::string str = ss.str();
	reptxt(str);
}
void calc_() {
	loadintparam(command);
}
void call() {
	int loadsuccess = rec_.size();
	load();
	loadsuccess = (loadsuccess != rec_.size());
	if (!rec_.size())return;
	if (recend == -1)end();
	int m_ = recend;
	int m = m_ - recstart;
	if (m <= 0)return;

	replaying++;

	INPUT out{};
	out.type = INPUT_KEYBOARD;
	
	out.ki.wVk = VK_REPLAYING;
	out.ki.dwFlags = KEYEVENTF_KEYUP;
	sendInputQueue.push(out);
	
	INPUT*b = new INPUT[m];
	for (int i = 0; i < m; i++) {
		auto a = readraw(recsource, i + recstart);
		out.ki.wVk = a.wp;
		out.ki.wScan = (a.lp >> 16) & 0xFF;
		out.ki.dwFlags = a.u == WM_KEYUP ? KEYEVENTF_KEYUP : 0;
		b[i] = out;
	}
	sendInputQueue.push(rpqueue<INPUT>(m, 1, b));

	out.ki.wVk = VK_REPLAYING;
	out.ki.dwFlags = 0;
	out.ki.wScan = 0;
	sendInputQueue.push(out);
	
	if (loadsuccess)prev();//unload
	repnext();
}
void If() {
	int con=loadintparam(command);
	if (con)call();
	else popparam(command);
}
void inc() {
	auto var = loadstrparam(command);
	extern calculator calc;
	auto a=calc.addVar(var.c_str());
	if (a) {
		a->second++; 

		std::ostringstream ss;
		ss << a->second;
		std::string str = ss.str();
		reptxt(str);
	}
}
void dec() {
	auto var = loadstrparam(command);
	extern calculator calc;
	auto a = calc.addVar(var.c_str());
	if (a) {
		a->second--;

		std::ostringstream ss;
		ss << a->second;
		std::string str = ss.str();
		reptxt(str);
	}
}
//input for commands paused
bool paused = false;
int nonremovablecommands = 1;//first 1 commands will not be removed after entering 
int nonpauseablecommands = 2;//first 2 commands will not be ignored while paused
void pause() { paused = true; }
void resume() { paused = false; }

#define c(id) L"#"#id,id
Command commands[] = { 
	L"#",setcmdindex,
	c(resume),
	c(pause),
	L"#calc",calc_,
	L"#if",If,
	c(exit),
	c(forceclose),
	c(close),
	c(cmd),
	c(rep),
	c(test),
	c(end),
	c(rec),
	c(set),
	c(save),
	c(load),
	c(prev),
	c(num),
	c(call),
	c(inc),
	c(dec),
	c(cont),
	c(discard)
};
#undef c

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	//clear log file
	std::wofstream(logfile, std::ios::trunc).close();
	std::ofstream(rawlogfile, std::ios::trunc).close();
	//init
	auto className = "kblistener";
	WNDCLASSA wnd{};
	wnd.lpszClassName = className;
	wnd.lpfnWndProc = proc;
	RegisterClassA(&wnd);
	hwnd = CreateWindowExA(0,className, "none", WS_POPUPWINDOW, 0, 0, 0, 0, 0, 0, 0, 0);
	if (!hwnd) {
		MessageBoxA(0, "cannot load the keyboard hook dll", "error", MB_ICONERROR | MB_OK);
		return -1;
	}
	ShowWindow(hwnd, SW_HIDE);
	UpdateWindow(hwnd);
	
	
	//main message loop
	MSG msg;
	while (GetMessageA(&msg, hwnd, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
	
	return 0;
}

LRESULT CALLBACK proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	static int replaying = 0;
	wchar_t buffer[2]{};
	switch (msg)
	{
	case WM_SENDBACK:
		receivedanswer();
		return 0;
	case WM_KEYDOWN:
		
		if (wp == VK_REPLAYING) {//replays are not logged/recorded only the actual user imput is
			replaying++;
		}
		else if (!replaying) {
			lograw(rawlogfile, msg, wp, lp);
			if (is_vk(wp)) { log(L"[DOWN]:"); log_vk(wp); }
		}return 0;
	case WM_KEYUP:
		if (wp == VK_ESCAPE) {
			interrupt();
		}
		if (wp == VK_REPLAYING) {
			if(replaying)replaying--;
		}
		else if (!replaying) {
			lograw(rawlogfile, msg, wp, lp);
			if (is_vk(wp)) { log(L"[  UP]:"); log_vk(wp); }
		}return 0;
	case WM_CHAR:
		if (wp == L'\b') {
			command.pop();
		}
		if (iswprint(wp))
		{
			if (!addNL&&!replaying) {
				log(L"[TEXT]:");
			}
			addNL = true;
			command.push(wp);
			for (int i = 0; i < sizeof(commands)/sizeof(*commands);i++) {
				auto&c = commands[i];
				if ((!paused||i<nonpauseablecommands)
					&& command.cpr(c.id)) {
					
					//remove last command
					int l = lstrlenW(c.id);
					if (i >= nonremovablecommands) {
						del(l);
						command.pop(l);
					}
					c.f();
					
					break;
				}
			}
			buffer[0] = wp;
			if(!replaying)log(buffer, 1);
		}
		break;
		return 0;
	case WM_DESTROY:
		FreeLibrary(dll);
		UnhookWindowsHookEx(hhook);
		PostQuitMessage(0);
		return 0;
	case WM_CREATE:
		dll = LoadLibraryA("kbhook.dll");
		if (!dll) {
			MessageBoxA(0, "cannot load the keyboard hook dll", "error", MB_ICONERROR | MB_OK);
			DestroyWindow(hwnd);
			return -1;
		}
		else {
			hook = (HOOKPROC)GetProcAddress(dll, "_hook@12");
			if(!hook)hook = (HOOKPROC)GetProcAddress(dll, "hook");
			if (!hook) {
				MessageBoxA(0, "failed to load hook procedure", "error", MB_ICONERROR | MB_OK);
				DestroyWindow(hwnd);
			}
			else hhook = SetWindowsHookExA(WH_KEYBOARD, hook, dll, 0);
			using initptr = void(__stdcall*)(HHOOK, HWND);
			initptr init = (initptr)GetProcAddress(dll, "_init@8");
			if(!init)init = (initptr)GetProcAddress(dll, "init");
			if (init)init(hhook, hwnd);
		}
		return 0;
	}
	return DefWindowProcA(hwnd, msg, wp, lp);
}

void log(const wchar_t *w, bool ignoreAddNL)
{
	std::wofstream out(logfile, std::ios::app);
	if (!out)return;
	if (addNL && !ignoreAddNL) {
		out << L"\n";
		addNL = false;
	}
	out << w;
}

void lograw(const char*dest,UINT u_, WPARAM w, LPARAM l) {
	std::ofstream out(dest, std::ios::app | std::ios::binary);
	if (!out)return;
	char u = u_ == WM_KEYUP;
	out.write(&u, sizeof(char));
	out.write((char*)&w, sizeof(char));
	out.write((char*)&l, sizeof(UINT32));
}
UWPLP readraw(const char*source,int index) {
	std::ifstream in(source, std::ios::binary);
	if (!in)return {};
	UWPLP r{};
	in.seekg(index*(sizeof(char) + sizeof(char) + sizeof(UINT32)));
	if (!in)return {};
	in.read((char*)&r.u, sizeof(char));
	r.u = r.u ? WM_KEYUP: WM_KEYDOWN;
	in.read((char*)&r.wp, sizeof(char));
	in.read((char*)&r.lp, sizeof(UINT32));
	if (in.fail())return {};
	return r;
}
int getcurrrawindex(const char*source) {
	std::ifstream in(source, std::ios::binary);
	if (!in)return -1;
	in.seekg(0, in.end);
	static constexpr int s = (sizeof(char) + sizeof(char) + sizeof(UINT32));
	int p = in.tellg();
	if (p%s)return -2;
	return p / s;
}

bool is_vk(WPARAM wp) {
#define l(a)case VK_##a:return true;
	switch (wp) {
#include"VKToLog.h"
	}
	return false;
}

void log_vk(WPARAM wp) {
#define l(a)case VK_##a:log(L"["#a"]\n");break;
	switch (wp) {
#include"VKToLog.h"
	}
}