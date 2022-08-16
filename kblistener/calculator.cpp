#include"calculator.h"

bool calculator::validatename(const char * in, int length)
{
	if (!length)length = strlen(in);
	for (int i = 0; i < length; i++) {
		auto&c = in[i];
		if (c >= 'A'&&c <= 'Z')continue;
		if (c >= 'a'&&c <= 'z')continue;
		if (c == '_')continue;
		
		if (i > 0 && c >= '0'&&c <= '9')continue;

		return false;
	}
	return true;
}

calculator::udfunction * calculator::isUdf(const char * in) {
	for (auto& a : udf)
		if (std::string(in, in + a.name.size()) == a.name && (in[a.name.size()] == '.' || (in[a.name.size()] == '(')))
			return &a;
	return nullptr;
}

calculator::udoperator * calculator::isUdo(const char * in) {
restart:
	for (auto& a : udo)
		if (!a.name.size()) { udo.erase(udo.begin() + (&a - udo.begin()._Ptr)); goto restart; }
		else if (std::string(in, in + a.name.size()) == a.name && (*(in + a.name.size()) == '.' || isValue(in + a.name.size()))) {
			return &a;
		}
		return nullptr;
}

int calculator::isNum(const char * in) {
	const char* x = in;
	for (;*in; in++)
		if (!(*in == '.' || (*in >= '0'&&*in <= '9'))) {
			break;
		}
	return in-x;
}

std::pair<std::string, long double>* calculator::isVar(const char * in, bool * is_const, int*len) {
	
	{
		int i = 0;
		std::shared_ptr<calculator>pC;
		udoperator* a = 0; udfunction *b = 0;
		if ((a = isUdo(in + i)) || (b = isUdf(in + i))) {
			if (a) { i += a->name.size(); pC = a->calc; }
			else { i += b->name.size(); pC = b->calc; }
			if (in[i] == '.') {
				i++;
				if (pC) {
					auto a = pC->isVar(in + i, is_const, &i);
					if (len)*len += i + (a ? a->first.length():0);
					return a;
				}
			}
			else { a = 0; b = 0; pC = 0; }
		}
	}
	
	std::pair<std::string, long double>*buffer = 0;
	int i = 0;
	for (; i < vars.size(); i++)
		if (vars[i].first == std::string(in, in + vars[i].first.size())) {
			if (!buffer)buffer = &vars[i];
			else if (buffer->first.size()<vars[i].first.size())buffer = &vars[i];
			break;
		}
	if (is_const)
		*is_const = i < constcount;
	if (len)*len += (buffer ? buffer->first.length() : 0);
	return buffer;
}

std::pair<std::string, std::unordered_map<long long, long double>>* calculator::isList(const char * in, int * len)
{
	{
		int i = 0;
		std::shared_ptr<calculator>pC;
		udoperator* a = 0; udfunction *b = 0;
		if ((a = isUdo(in + i)) || (b = isUdf(in + i))) {
			if (a) { i += a->name.size(); pC = a->calc; }
			else { i += b->name.size(); pC = b->calc; }
			if (in[i] == '.') {
				i++;
				if (pC) {
					auto a = pC->isList(in + i, &i);
					if (len)*len += i + (a ? a->first.length() : 0);
					return a;
				}
			}
			else { a = 0; b = 0; pC = 0; }
		}
	}

	std::pair<std::string, std::unordered_map<long long, long double>>*buffer = 0;
	int i = 0;
	for (; i < lists.size(); i++)
		if (lists[i].first == std::string(in, in + lists[i].first.size())) {
			if (!buffer)buffer = &lists[i];
			else if (buffer->first.size() < lists[i].first.size())buffer = &lists[i];
			break;
		}
	if (len)*len += (buffer ? buffer->first.length() : 0);
	return buffer;
}

int calculator::isFunction(const char * in) {
	for (int i = 0; i < functions.size(); i++)
		if (functions[i] == std::string(in, in + functions[i].size())) return i + 1;
	return 0;
}

bool calculator::isValue(const char * in) {
	int b = 0;
	if (!in)return 0;
	if (isNum(in))return 1;
	if (isFunction(in))return 1;
	if (isUdf(in))return 1;
	if (isVar(in,0,&b))return b;
	if (isList(in, &b))return b;
	if (in[0] == '(')return 1;
	if (in[0] == '[')return 1;
	return 0;
}

long double calculator::faculty(int in) {
	long double buff = 1;
	if (buff < 0)return NAN;
	for (int i = in; i > 0; i--) {
		buff *= i;
	}
	return buff;
}

long double calculator::strToNum(const char * in) {
	long double buff = 0; double point = 0;
	for (int i = 0; in[i]; i++) {
		if (!point)
			if (in[i] >= '0'&&in[i] <= '9') { buff *= 10; buff += in[i] - '0'; }
			else if (in[i] == '.') point = 1;
			else return buff;
		else {
			point *= 10;
			if (in[i] >= '0'&&in[i] <= '9')  buff += (in[i] - '0') / point;
			else return buff;
		}
	}
	return buff;
}

long double calculator::loadValue(const char * in, int & i) {
	long double b = 0;
	int buff = 0;
	std::pair<std::string, long double>*var = 0;
	std::pair<std::string, std::unordered_map<long long, long double>>*list = 0;
	udfunction*pudf = 0; 

	if ((var = isVar(in+i,0,&buff))&& in[i+buff]!='<' && buff) {
		b = var->second; i += buff;
	}
	else if ((list = isList(in + i, &buff)) && in[i + buff] == '<'&&buff) {
		i += buff + 1;
		b = list->second[std::lroundl(add(in, i))];
		if (in[i] == '>')i++;
	}
	else if (buff= isNum(in + i)) { b = strToNum(in + i); i += buff; }
	else if (isFunction(in + i))b = function(in, i);
	else if (pudf = isUdf(in + i))b = cudf(in, i, pudf);
	else if (in[i] == '(') { b = add(in, ++i); i++; }
	else if (in[i] == '[') {
		b = calc(in + (++i));
		int x = 1;
		for (; in[i] && x; i++) {
			if (in[i] == '[')x++;
			else if (in[i] == ']')x--;
		}
	}

	for (; in[i] == '!'; i++) { b = faculty(b); }
	return b;
}

long double calculator::function(const char * in, int & i) {
	int buffer = isFunction(in + i);
	if (!buffer)return NAN;
	i += functions[buffer - 1].size();
	switch (buffer) {
	case 1: return cos(loadValue(in, i) * M_PI / 180.0);
	case 2: return sin(loadValue(in, i) * M_PI / 180.0);
	case 3: return tan(loadValue(in, i) * M_PI / 180.0);
	case 4: return acos(loadValue(in, i)) * 180 / M_PI;
	case 5: return asin(loadValue(in, i)) * 180 / M_PI;
	case 6: return atan(loadValue(in, i)) * 180 / M_PI;
	case 7: return log(loadValue(in, i)) / log(M_E);
	default:return 0;
	}
}

long double calculator::cudf(const char * in, int & i, udfunction * p) {//calculate user defined function
begin:
	i += p->name.size();
	//if (!p->calc) { errors.push_back("parent function deleted"); return 0; }//unreachable
	if (in[i] == '.') {
		i++;
		p = p->calc->isUdf(in + i);//idk why but my debugger complains that it cant convert "udfunction*" to "udfunction*", compiles fine though
		if (p)goto begin;
		return 0;
	}
	if (in[i] == '(') {
		i++;
		if (p->paralist.size() == 0) { i++; goto paralist_finished; }
		for (auto&para : p->paralist) {
			p->calc->addVar(para.c_str(), add(in, i));
			if (in[i] == ',')i++;
			else if (in[i] == ')'&&&para == p->paralist.end()._Ptr - 1) { i++; goto paralist_finished; }
			else break;
		}
	}
	return 0;
paralist_finished:
	long double a = p->calc->calc(p->function.c_str());
	return a;

}

long double calculator::cudo(const char * in, int & i) {//calculate user defined operator
	long double ANS = loadValue(in, i);
	udoperator*p = 0;
	while (1) {
		if (p = isUdo(in + i)) {
			i += p->name.size();
			p->calc->addVar(p->paraFirst.c_str(), ANS);
			p->calc->addVar(p->paraSecond.c_str(), loadValue(in, i));
			ANS = p->calc->calc(p->function.c_str());
			}
		else return ANS;
	}
}

long double calculator::pot(const char * in, int & i) {
	long double ANS = cudo(in, i);
	while (1) {
		if (std::string(in+i,in+i+3)=="exp") {
			if (!isValue(in + (i += 3))) {
				ANS *= ANS;
			}
			else {
				long double buffer = loadValue(in, i);
				if ((!buffer) && (!ANS))ANS = NAN;
				else ANS = pow(ANS, buffer);
			}
		}
		else if (std::string(in + i, in + i + 4) == "root") {
			if (!isValue(in + (i += 4))) {
				ANS = sqrt(ANS);
			}
			else ANS = pow(ANS, 1 / loadValue(in, i));
		}
		else if (std::string(in + i, in + i + 3) == "log") {
			if (!isValue(in + (i += 3))) {
				ANS = log10(ANS);
			}
			else ANS = log(ANS) / log(loadValue(in, i));
		}
		else return ANS;
	}
}

long double calculator::mul(const char * in, int & i) {
	long double ANS = pot(in, i);
	while (1) {
		if (in[i] == '*') {
			if (!isValue(in + i + 1)) {
				ANS *= 2;
				i++;
			}
			else ANS *= pot(in, ++i);
		}
		else if (isValue(in + i)) {
			ANS *= pot(in, i);
		}
		else if (in[i] == '/') {
			if (!isValue(in + i + 1)) {
				ANS *= 0.5;
				i++;
			}
			else {
				long double buffer = pot(in, ++i);
				if (!buffer)ANS = NAN;
				else ANS /= buffer;
			}
		}
		else if (in[i] == '%') {
			if (isValue(in + i + 1)) {
				long double buffer = pot(in, ++i);
				if (!buffer)ANS = NAN;
				else ANS = ANS - floor(ANS / buffer)*buffer;
			}
		}
		else return ANS;
	}
}

long double calculator::add(const char * in, int & i) {
	long double ANS = mul(in, i);
	while (1) {
		if (in[i] == '+') {
			if (!isValue(in + i + 1)) {
				ANS += 1;
				i++;
			}
			else ANS += mul(in, ++i);
		}
		else if (in[i] == '-') {
			if (!isValue(in + i + 1)) {
				ANS -= 1;
				i++;
			}
			else ANS -= mul(in, ++i);
		}
		else return ANS;
	}
}

bool calculator::addVar(const char * in, long double val) {
	auto a=addVar(in);
	if (a) {
		a->second = val;
	}
	return a;
}

std::pair<std::string,long double>*calculator::addVar(const char * in) {
	if (!validatename(in))return 0;
	auto a = isVar(in);
	if (a) {
		return a;
	}
	vars.push_back(std::make_pair<std::string, long double>(std::string(in), 0));
	return &vars.back();
}
std::pair<std::string,std::unordered_map<long long,long double>>*calculator::addList(const char * in,int*lenght) {
	auto len = std::string(in).find('<');
	if (lenght)*lenght = len;
	if (len <= 0)return 0;
	if (!validatename(in,len))return 0;
	auto a = isList(in);
	if (a) {
		return a;
	}
	lists.push_back(std::make_pair<std::string, std::unordered_map<long long, long double>>(std::string(in,in+len), {}));
	return &lists.back();
}

void calculator::undefine(const char * in) {
	delFunction(in);
	delOperator(in);
	for (int i = constcount; i < vars.size(); i++)
		if (vars[i].first == std::string(in)) {
			vars.erase(vars.begin() + i--);
		}
	for (int i = 0; i < lists.size(); i++)
		if (lists[i].first == std::string(in)) {
			lists.erase(lists.begin() + i--);
		}
}

calculator::calculator() {
	if (!functions.size()) {
		functions.push_back("cos");
		functions.push_back("sin");
		functions.push_back("tan");
		functions.push_back("acos");
		functions.push_back("asin");
		functions.push_back("atan");
		functions.push_back("ln");
	}
	vars.push_back(std::make_pair(std::string("PI"), M_PI));
	vars.push_back(std::make_pair(std::string("e"), M_E));
	vars.push_back(std::make_pair(std::string("TAU"), 2 * M_PI));
	constcount = vars.size();
}


calculator::calculator(calculator & rhs) {
	*this = rhs;
}

long double calculator::calc(const char * in) {
	int x = 0;
	for (int i = 0; in[i]; i++)
		if (in[i] == '(')x++;
		else if (in[i] == ')')x--;
		std::string l = in;

		if (std::string(in, in + 4) == "def "&&l.size()>4) {
			in += 4;
			if (addFunction(in))return true;
			if(addOperator(in))return true;
			if (addList(in))return true;
			if (std::string(in).find('(') == std::string::npos) {
				auto a=addVar(in);
				if (a)return a->second;
			}
			return false;
		}
		if (std::string(in, in + 6) == "undef " && l.size()>6) {
			undefine(in + 6);
			return 0;
		}
		int i = 0;
		long double buffer = 0;

		if (x >= 0)buffer = add(in, i);//calculate it
		else buffer = add((std::string(-x, '(') + in).c_str(), i);//insert x '(' at the begining of in , e.g in=="(1))" -> in=="((1))" , actual calculation



		if (in[i]=='=')
		{
			int assigncount = 0;
			std::string assign(in + i);
			for (int i = 0; i < assign.size(); i++) {
				if (assign[i] == '=') {
					assign[i] = 0;
					assigncount++;
				}
			}
			in = assign.c_str();
			i = 0;
			while (assigncount-- > 0&&in[++i]) {
				if (in[i] == '>') {
					i++;
					l = in + i; bool constant = false;

					auto v = isVar(in + i, &constant);
					if (v)
						if (!constant)
							v->second = buffer;
						else;
					else if (addVar(in + i, buffer));
					else {
						
						int len = 0;
						auto a = addList(in + i,&len);

						if (a) {
							i += len;
							a->second[std::lroundl(add(in,++i))] = buffer;
							if(in[i]=='>')i++;
						}
					}
				}
				else {
					double rhs = add(in, i);
					return (abs(rhs - buffer) < 0.0000001);
				}
				while (in[++i]);//advance buffer to next null character ('=' were converted to NULL)
			}
		}
		else if (in[i] == '>') {
			i++;
			bool oreq = false;
			if (in[i] == '=') {
				i++;
				oreq = true;
			}
			double rhs = add(in, i);
			return (buffer > rhs) || (oreq&& (abs(rhs - buffer) < 0.0000001));
		}
		else if (in[i] == '<') {
			i++;
			bool oreq = false;
			if (in[i] == '=') {
				i++;
				oreq = true;
			}
			double rhs = add(in, i);
			return (buffer < rhs) || (oreq && (abs(rhs - buffer) < 0.0000001));
		}
		else if (in[i] == '!'&&in[i+1]=='=') {
			i+=2;
			double rhs = add(in, i);
			return abs(rhs - buffer) >= 0.0000001;
		}
	ret:
		return buffer;
}

bool calculator::deleteVar(const char * name) {
	for (int i = 4; i < vars.size(); i++)
		if (vars[i].first == std::string(name)) {
			vars.erase(vars.begin() + i--);
			return true;
		}
	return false;
}

bool calculator::addFunction(const char * f) {//input "f(x,y)=x+y"
	if (*f == '[')return 0;
	int name;
	std::vector<std::pair<int, int>>para;
	int fb, fe;
	int i = 0;
	udoperator* a = 0; udfunction *b = 0;
	std::shared_ptr<calculator>pC;
	const char* c;
	if ((a = isUdo(f)) || (b = isUdf(f))) {
		if (a) { c = f + a->name.size(); pC = a->calc; }
		else { c = f + b->name.size(); pC = b->calc; }
		if (*c == '.')f = c + 1;
		else { a = 0; b = 0; pC = 0; }
	}
	for (; f[i]; i++) {
		if (f[i] == '(') { name = i; if (!name) break; goto next1; }
	}goto error;
next1:
	for (int first = 0; f[i]; i++) {
		if (f[i] == ',') { if (!first)continue; para.emplace_back(std::make_pair(first, i)); first = 0; }
		else if (!first&&f[i] != ' '&&f[i] != ')'&&f[i] != '('&&f[i] != ',') { first = i; }
		if (f[i] == ')') { if (first)para.emplace_back(std::make_pair(first, i)); goto next2; }
	}goto error;
next2:
	for (; f[i]; i++)if (f[i] == '=')goto next3;
	goto error;
next3:
	fb = i + 1;
	for (int x = 1; f[i] && x; i++)
		if (f[i] == '[')x++;
		else if (f[i] == ']')x--;
		fe = i;
		{
			auto n = std::string(f, f + name);
			for (const auto&a : n)if (a == '.')goto error;
			if (isUdf((n + '.').c_str()))delFunction(n.c_str());
			if (isUdo((n + '.').c_str()))delOperator(n.c_str());
			if (!pC) {
				pC = std::make_shared<calculator>();
			}
			if (!pC) { return false; }
			auto func = std::string(f + fb, f + fe);
			udf.emplace_back();
			udfunction&a = udf.back();
			a.calc = pC;
			a.function = func; 
			for (auto&p : para)
				a.paralist.emplace_back(f + p.first, f + p.second);
			for (auto&p : a.paralist)
				a.calc->addVar(p.c_str());
			a.name = n;
			return 1;
		}
	error:
		return 0;
}

bool calculator::addOperator(const char * f) {
	if (*f == '[')return 0;
	std::pair<int, int>para1, para2, name;
	int fb, fe;
	int i = 0;

	udoperator* a = 0; udfunction *b = 0;
	std::shared_ptr<calculator>pC;
	const char* c;
	if ((a = isUdo(f)) || (b = isUdf(f))) {
		if (a) { c = f + a->name.size(); pC = a->calc; }
		else { c = f + b->name.size(); pC = b->calc; }
		if (*c == '.')f = c + 1;
		else { a = 0; b = 0; pC = 0; }
	}

	for (; f[i]; i++) {
		if (f[i] == ' ') { para1 = std::make_pair(0, i); goto next1; }
	}goto error;
next1:
	for (; f[i] && f[i] == ' '; i++);
	for (int p = i; f[i]; i++) {
		if (f[i] == ' ') { name = std::make_pair(p, i); goto next2; }
	}goto error;
next2:
	for (; f[i] && f[i] == ' '; i++);
	for (int p = i; f[i]; i++) {
		if (f[i] == ' ' || f[i] == '=') { para2 = std::make_pair(p, i); goto next3; }
	}goto error;
next3:
	for (; f[i]; i++)if (f[i] == '=')goto next4;
	goto error;
next4:
	fb = i + 1;
	for (int x = 1; f[i] && x; i++)
		if (f[i] == '[')x++;
		else if (f[i] == ']')x--;
		fe = i;
		{
			if (!pC) {
				pC = std::make_shared<calculator>();
			}
			if (!pC) { return false; }
			auto n = std::string(f + name.first, f + name.second);
			for (const auto&a : n)if (a == '.')goto error;
			if (isUdo((n + '.').c_str()))delOperator(n.c_str());
			if (isUdf((n + '.').c_str()))delFunction(n.c_str());
			udo.push_back({});
			udoperator&a = udo.back();
			a.calc = pC;
			a.name = n;
			a.paraFirst = std::string(f + para1.first, f + para1.second);
			a.paraSecond = std::string(f + para2.first, f + para2.second);
			a.function = std::string(f + fb, f + fe);
			a.calc->addVar(a.paraFirst.c_str(), 0);
			a.calc->addVar(a.paraSecond.c_str(), 0);
			return 1;
		}
	error:;
		return 0;
}

bool calculator::delFunction(const char * in) {
	for (auto& a : udf)
		if (std::string(in) == a.name) {
			//delete a.calc.ptr();
			udf.erase(udf.begin() + (&a - udf.begin()._Ptr));
			return true;
		}
	return false;
}

bool calculator::delOperator(const char * in) {
	for (auto& a : udo)
		if (std::string(in) == a.name) {
			//delete a.calc.ptr();
			udo.erase(udo.begin() + (&a - udo.begin()._Ptr));
			return true;
		}
	return false;
}
