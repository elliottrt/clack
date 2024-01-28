#pragma once
#include <string>
#include <iostream>
#include <map>
#include <cctype>

#include "function.h"
#include "command_error.h"

/*

The original math equation solver code was taken from here:
https://stackoverflow.com/questions/3422673/how-to-evaluate-a-math-expression-given-in-string-form

and expanded upon by me.

*/

// math

namespace Clack {

static inline bool varValidFirst(char const f) {
	return std::isalpha(f) || f == '_';
}

static inline bool varValid(char const f) {
	return std::isalnum(f) || f == '_';
}

static const char functionPointerPrefix = '&';

struct Variable {

	double value;
	const bool system;
	
	Variable(const double value, const bool system = false): value(value), system(system) {}

};

class Solver {

	std::map<std::pair<std::string, int>, Function> functions {};
	std::map<std::string, Variable> variables {};

	double lastResult;

	bool fileSilent;

    void loadBuiltins(void);

    void reset(void);

    inline void setVarSystem(std::string var, double val) { this->setVar(var, val, true); }

public:

	Solver(void) {
		this->loadBuiltins();
		this->reset();
		this->fileSilent = false;
	}

	void setVar(const std::string var, const double val, const bool system = false);

    double getVar(const std::string &name);

	bool varExists(const std::string &name) const;

	size_t getVarCount(void) const;

	std::string callFunction(const std::string &name, const std::vector<std::string> &args);

	bool functionExists(const std::string &name, const int args) const;

	bool functionExists(const std::string &name) const;

	size_t getFunctionCount(void) const;

	void setFunctionSystemVoid(std::string funcName, std::function<void(double)> func) {
		if (func != nullptr) {
			std::pair<std::string, int> funcPair = std::make_pair(funcName, 1);
			this->functions.erase(funcPair);
			this->functions.insert({funcPair, Function(func)});
		}
	}

	void setFunctionSystem(std::string funcName, std::function<double(void)> func) {
		if (func != nullptr) {
			std::pair<std::string, int> funcPair = std::make_pair(funcName, 0);
			this->functions.erase(funcPair);
			this->functions.insert({funcPair, Function(func)});
		}
	}

	void setFunctionSystem(std::string funcName, std::function<double(double)> func) {
		if (func != nullptr) {
			std::pair<std::string, int> funcPair = std::make_pair(funcName, 1);
			this->functions.erase(funcPair);
			this->functions.insert({funcPair, Function(func)});
		}
	}

	void setFunctionSystem(std::string funcName, std::function<double(double, double)> func) {
		if (func != nullptr) {
			std::pair<std::string, int> funcPair = std::make_pair(funcName, 2);
			this->functions.erase(funcPair);
			this->functions.insert({funcPair, Function(func)});
		}
	}

	void setFunction(std::string funcName, std::string args, std::string funcexpr) {
		std::pair<std::string, int> funcPair = std::make_pair(funcName, Function::parseArgNames(args).size());

		if (this->functions.count(funcPair) && this->functions.at(funcPair).getSystem()) {
			throw Clack::CommandError("Attempted to set system function " + funcName);
		} else {
			this->functions.erase(funcPair);
			this->functions.insert({funcPair, Function(args, funcexpr)});
		}
	}

	double solve(std::string expr);

	void runCommand(std::string cmd);

	void dumpState(std::string toFile);
};

}
