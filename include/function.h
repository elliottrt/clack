#pragma once

#include <functional>
#include <vector>
#include <iostream>
#include <sstream>

namespace Clack {

std::string toSafeName(const std::string &var);

std::string unSafeName(const std::string &var);

class Function {

	std::function<void(double)> voidfunction1;
	std::function<double(void)> function0;
	std::function<double(double)> function1;
	std::function<double(double, double)> function2;

	std::string functionExpression;
	std::string safeExpression;

	std::vector<std::string> argNames;
	bool system = true;

	void makeReplaceSafe(void);

public:

	static const char safePrefix = '$';
	static const char safeSuffix = '$';

	Function(std::function<void(double)> function);

	Function(std::function<double(void)> function);

	Function(std::function<double(double)> function);

	Function(std::function<double(double, double)> function);

	Function(const std::string &sig, const std::string &funcexpr);

	std::string call(const std::vector<double> &args);

	bool getSystem(void) const { return this->system; }

	std::string getExpression(void) const { return this->functionExpression; }

	size_t getArgCount(void) const { return this->argNames.size(); }

	std::string getArg(const size_t i) const { return unSafeName(this->argNames[i]); }

};

}
