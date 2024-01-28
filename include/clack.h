#pragma once

#include "defs.h"
#include "function.h"
#include "command_error.h"

#include <string>
#include <iostream>
#include <map>
#include <cctype>

/*

The original math equation solver code was taken from here:
https://stackoverflow.com/questions/3422673/how-to-evaluate-a-math-expression-given-in-string-form

and expanded upon by me.

*/

// math

namespace Clack {

struct Variable {

	mathtype_t value;
	const bool system;
	
	Variable(const mathtype_t value, const bool system = false): value(value), system(system) {}

};

class Solver {

	std::map<std::pair<std::string, int>, Function> functions {};
	std::map<std::string, Variable> variables {};

	mathtype_t lastResult;

	bool fileSilent;

    void loadBuiltins(void);

    void reset(void);

    void setVarSystem(const std::string &var, mathtype_t val);

public:

	Solver(void);

	void setVar(const std::string var, const mathtype_t val, const bool system = false);

    mathtype_t getVar(const std::string &name);

	bool varExists(const std::string &name) const;

	size_t getVarCount(void) const;

	std::string callFunction(const std::string &name, const std::vector<std::string> &args);

	bool functionExists(const std::string &name, const int args) const;

	bool functionExists(const std::string &name) const;

	size_t getFunctionCount(void) const;

	void setFunctionSystem(const std::string &funcName, std::function<mathtype_t(void)> func);
	void setFunctionSystem(const std::string &funcName, std::function<mathtype_t(mathtype_t)> func);
	void setFunctionSystem(const std::string &funcName, std::function<mathtype_t(mathtype_t, mathtype_t)> func);
	void setFunction(const std::string &funcName, const std::string &args, const std::string &funcexpr);

	mathtype_t solve(std::string expr);

	void runCommand(std::string cmd);

	void dumpState(std::string toFile);
};

}
