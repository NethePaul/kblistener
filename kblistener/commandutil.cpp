#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include<assert.h>
#include "commandutil.h"
#include<locale>
#include<codecvt>
#include<stack>
#include<array>
#include"calculator.h"
#include"repeatablequeue.h"

extern std::stack<rpqueue<INPUT>> sendInputQueue;

void send(const INPUT&in) {
	sendInputQueue.push(in);
}
int alreadyawaitinganswer=false;
INPUT lastsent;
void repnext() {
	if (alreadyawaitinganswer)return;
	
	if (sendInputQueue.empty())return;

	alreadyawaitinganswer++;
	auto&d = sendInputQueue.top();
	auto&s = d.pop();
	bool siqpop = d.empty();
	INPUT out[2]{};
	out[0] = lastsent = s;
	out[1].type = INPUT_KEYBOARD;
	out[1].ki.wVk = VK_SEND;
	SendInput(2, out, sizeof(INPUT));
	if(siqpop)sendInputQueue.pop();
}
void cont()
{
	alreadyawaitinganswer = 0;
	if (lastsent.type)
		send(lastsent);//resend last
	repnext();
}
void interrupt() {
	alreadyawaitinganswer++;
	lastsent.type = 0;
}
void discard() {
	sendInputQueue = decltype(sendInputQueue)();
	alreadyawaitinganswer = 0;
}
void receivedanswer() {
	if(alreadyawaitinganswer)
		alreadyawaitinganswer--;
	repnext();
}
void del(int i) {
	static INPUT rm[5]{};
	if (!rm->type) {
		for (int i = 0; i < 5; i++) {
			rm[i].type = INPUT_KEYBOARD;
		}

		rm[1].ki.wVk = rm[2].ki.wVk = VK_BACK;
		rm[0].ki.wVk = rm[3].ki.wVk = VK_IGNORE;

		rm[2].ki.dwFlags = KEYEVENTF_KEYUP;
		rm[3].ki.dwFlags = KEYEVENTF_KEYUP;

		rm[4].ki.wVk = VK_SEND;
	}
	auto r = new INPUT[3 + i * 2];
	r[0] = rm[0];
	for (int j = i; j--;) {
		r[1 + j * 2] = rm[1];
		r[2 + j * 2] = rm[2];
	}
	r[1 + i * 2] = rm[3];
	r[2 + i * 2] = rm[4];
	int n = 3 + i * 2;
	alreadyawaitinganswer++;
	SendInput(n, r, sizeof(INPUT));
	delete[]r;
}

calculator calc;
long double loadnumparam(CircularBuffer<wchar_t, CMDLEN>&command)
{
	return calc(loadstrparam(command, textfilter).c_str());
}

bool singlewordfilter(wchar_t b)
{
	if (
		(b >= L'0'&&b <= L'9') ||
		(b >= L'a'&&b <= L'z') ||
		(b >= L'A'&&b <= L'Z') ||
		(b == L'_')
		)
		return true;
	return false;
}

using convert_type = std::codecvt_utf8<wchar_t>;
std::wstring_convert<convert_type, wchar_t> wtoc;

bool textfilter(wchar_t b)
{
	switch (b) {
	case L':':case L'#':return false;
	}
	return b;
}

std::string loadstrparam(CircularBuffer<wchar_t, CMDLEN>&command, bool(*filter)(wchar_t))
{
	int max_digit = 0;
	wchar_t b;
	while (true) {
		b = command.get(max_digit + 1);
		if(filter(b))
			max_digit++;
		else if (b == L':')break;
		else return "";
	}
	command.set(0,0);
	const auto&str=wtoc.to_bytes(&command.get(max_digit));
	del(max_digit + 1);
	command.pop(max_digit + 1);
	return str;
}

void popparam(CircularBuffer<wchar_t, CMDLEN>&command) {
	int max_digit = 0;
	wchar_t b;
	while (true) {
		b = command.get(++max_digit);
		if (b == L':')break;
		if (!b)return;
	}
	del(max_digit);
	command.pop(max_digit);
}

