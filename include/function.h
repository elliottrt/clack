#pragma once

#include "defs.h"

#include <functional>
#include <vector>
#include <iostream>
#include <sstream>

namespace Clack {

class Function {

	// TODO: try to not do this
	std::function<mathtype_t(void)> function0;
	std::function<mathtype_t(mathtype_t)> function1;
	std::function<mathtype_t(mathtype_t, mathtype_t)> function2;

	std::string functionExpression;
	std::string safeExpression;

	std::vector<std::string> argNames;
	bool system = true;

	void makeReplaceSafe(void);

public:

	static std::vector<std::string> parseArgNames(const std::string &args);

	static std::string toSafeName(const std::string &var);
	static std::string unSafeName(const std::string &var);

	Function(std::function<mathtype_t(void)> function);
	Function(std::function<mathtype_t(mathtype_t)> function);
	Function(std::function<mathtype_t(mathtype_t, mathtype_t)> function);

	Function(const std::string &sig, const std::string &funcexpr);

	std::string call(const std::vector<std::string> &args);

	bool getSystem(void) const { return this->system; }

	std::string getExpression(void) const { return this->functionExpression; }

	size_t getArgCount(void) const { return this->argNames.size(); }

	std::string getArg(const size_t i) const { return unSafeName(this->argNames[i]); }

};

}
