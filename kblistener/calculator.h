#pragma once
#define _USE_MATH_DEFINES
#include<math.h>
#include<string>
#include<vector>
#include<memory>
#include<unordered_map>


class calculator {
public:
	struct udfunction{//user defined functions
		//e.g.function def f(x,y,z)=x+y*z
		std::vector<std::string> paralist;//"x","y","z"
		std::string name;//"f("
		std::string function;//"x+y*z"
		std::shared_ptr<calculator> calc;
		udfunction(const udfunction&mov) = default;
		udfunction() = default;
		udfunction&operator=(const udfunction&mov) = default;
	};
	struct udoperator {//user defined operators
		//e.g.operator def x f y=x+y
		std::string paraFirst;//"x"
		std::string paraSecond;//"y"
		std::string name;//"f"
		std::string function;//"x+y"
		std::shared_ptr<calculator>calc;
		udoperator(const udoperator&mov) = default;
		udoperator&operator=(const udoperator&mov) = default;
		udoperator() = default;
	};

private:
	bool validatename(const char*in, int length=0);
	std::vector<udfunction>udf;
	std::vector<udoperator>udo;
	int constcount = 0;//the first Variables are constant and should not be overwritten
	std::vector<std::string>functions;
	std::vector<std::pair<std::string, long double>>vars;//variables (e.g. x=10)
	std::vector<std::pair<std::string, std::unordered_map<long long,long double>>>lists;//vectors (e.g. x<2>=10)

	udfunction*isUdf(const char*in);
	udoperator*isUdo(const char*in);

	int isNum(const char*in);
	std::pair<std::string, long double>*isVar(const char*in, bool*is_const = 0,int*len=0);
	std::pair<std::string, std::unordered_map<long long,long double>>*isList(const char*in, int*len = 0);

	int isFunction(const char*in);
	bool isValue(const char*in);

	long double faculty(int in);

	long double strToNum(const char*in);

	long double loadValue(const char*in, int&i);
	long double function(const char*in, int&i);
	long double cudf(const char*in, int &i, udfunction*p);
	long double cudo(const char*in, int &i);
	long double pot(const char*in, int&i);
	long double mul(const char*in, int&i);
	long double add(const char*in, int&i);
public:

	calculator();
	calculator(calculator&rhs);
	calculator&operator=(calculator&rhs) {
		constcount = rhs.constcount;
		vars = rhs.vars;
		udf = rhs.udf;
		udo = rhs.udo;
		return*this;
	}
	long double operator()(const char*in) { return calc(in); }
	long double calc(const char*in);


	bool deleteVar(const char*name);
	bool addFunction(const char*f);
	bool addOperator(const char*f);
	bool delFunction(const char*in);
	bool delOperator(const char*in);
	bool addVar(const char*in, long double val);
	std::pair<std::string, long double>*addVar(const char*in);
	std::pair<std::string,std::unordered_map<long long,long double>>* addList(const char * in, int*len=0);
	void undefine(const char*in);
};