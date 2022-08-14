#pragma once
#define _USE_MATH_DEFINES
#include<math.h>
#include<string>
#include<vector>
#include<memory>


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

	std::vector<udfunction>udf;
	std::vector<udoperator>udo;
	int constcount = 0;//the first Variables are constant and should not be overwritten
	std::vector<std::string>functions;
	std::vector<std::pair<std::string, long double>>Var;//variables local to this class
	std::vector<std::pair<std::string,std::vector<long double>>>lists;//arrays of variables local to this class

	udfunction*is_udf(const char*in);
	udoperator*is_udo(const char*in);

	int is_Num(const char*in);
	std::pair<std::string, long double>*is_Var(const char*in, bool*is_const = 0,int*len=0);

	int is_Function(const char*in);
	bool is_value(const char*in);

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
		Var = rhs.Var;
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
	void define(const char*in, long double val);
	std::pair<std::string, long double>*define(const char*in);
	void undefine(const char*in);
};