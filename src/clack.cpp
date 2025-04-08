#include "clack.h"
#include "expression.h"
#include "function-defs.h"
#include "expression_error.h"
#include "command_error.h"
#include "function.h"

#include <sstream>
#include <fstream>

void Clack::Solver::setVarSystem(const std::string &var, mathtype_t val) { 
	this->setVar(var, val, true); 
}

Clack::Solver::Solver() {
	this->loadBuiltins();
	this->reset();
	this->fileSilent = false;
}

void Clack::Solver::loadBuiltins(void) {

	setVarSystem("pi",  3.14159265358979323846);
	setVarSystem("e",   2.71828182845904523536);
	setVarSystem("phi", 1.61803398874989484820);

	setFunctionSystem("seed", &seed_);
	setFunctionSystem("rand", &rand_);

	setFunctionSystem("sqrt", (mathtype_t(*)(mathtype_t))&std::sqrt);
	setFunctionSystem("sin",  (mathtype_t(*)(mathtype_t))&std::sin);
	setFunctionSystem("cos",  (mathtype_t(*)(mathtype_t))&std::cos);
	setFunctionSystem("tan",  (mathtype_t(*)(mathtype_t))&std::tan);
	setFunctionSystem("asin",  (mathtype_t(*)(mathtype_t))&std::asin);
	setFunctionSystem("acos",  (mathtype_t(*)(mathtype_t))&std::acos);
	setFunctionSystem("atan",  (mathtype_t(*)(mathtype_t))&std::atan);
	setFunctionSystem("ln",  (mathtype_t(*)(mathtype_t))&std::log);
	setFunctionSystem("ceil",  (mathtype_t(*)(mathtype_t))&std::ceil);
	setFunctionSystem("floor",  (mathtype_t(*)(mathtype_t))&std::floor);
	setFunctionSystem("round",  (mathtype_t(*)(mathtype_t))&std::round);

	setFunctionSystem("print", &print);
	setFunctionSystem("printa", &printa);

	setFunctionSystem("mod", (mathtype_t(*)(mathtype_t, mathtype_t))&std::fmod);
	setFunctionSystem("pow", (mathtype_t(*)(mathtype_t, mathtype_t))&std::pow);
	setFunctionSystem("log", &logbase);
}

void Clack::Solver::reset(void) { 

	auto viter = this->variables.begin();
	auto vendIter = this->variables.end();

	for(; viter != vendIter; ) {
		if (!viter->second.system) viter = this->variables.erase(viter);
		else ++viter;
	}

	auto fiter = this->functions.begin();
	auto fendIter = this->functions.end();

	for(; fiter != fendIter; ) {
		if (!fiter->second.getSystem()) fiter = this->functions.erase(fiter);
		else ++fiter;
	}
}

void Clack::Solver::setVar(const std::string var, const mathtype_t val, const bool system) {
	if (this->variables.count(var) && this->variables.at(var).system) {
		throw Clack::CommandError("Attempted to set system variable " + var);
	} else {
		this->variables.erase(var);
		this->variables.insert({var, Variable(val, system)});
	}
}

Clack::mathtype_t Clack::Solver::getVar(const std::string &name) {
	if (this->varExists(name)) 
		return this->variables.at(name).value;
	else 
		throw Clack::CommandError("No variable named " + name);
}

bool Clack::Solver::varExists(const std::string &name) const {
	return this->variables.count(name);
}

size_t Clack::Solver::getVarCount(void) const {
	return this->variables.size();
}

std::string Clack::Solver::callFunction(const std::string &name, const std::vector<std::string> &args) {

	if (this->functionExists(name, args.size())) {

		std::pair<std::string, int> funcPair = std::make_pair(name, args.size());
		return this->functions.at(funcPair).call(args);

	} else if (this->functionExists(name)) {

		throw Clack::ExpressionError("Wrong number of arguments for function " + name);

	} else {

		throw Clack::ExpressionError("No function named " + name);
		
	}

}

bool Clack::Solver::functionExists(const std::string &name, const int args) const {
	return this->functions.count(std::make_pair(name, args)) > 0;
}

bool Clack::Solver::functionExists(const std::string &name) const {
	for(auto pair : this->functions)
		if (pair.first.first == name) return true;
	return false;
}

size_t Clack::Solver::getFunctionCount(void) const {
	return this->functions.size();
}

void Clack::Solver::setFunctionSystem(const std::string &funcName, std::function<mathtype_t(void)> func) {
	if (func != nullptr) {
		std::pair<std::string, int> funcPair = std::make_pair(funcName, 0);
		this->functions.erase(funcPair);
		this->functions.insert({funcPair, Clack::Function(func)});
	}
}

void Clack::Solver::setFunctionSystem(const std::string &funcName, std::function<mathtype_t(mathtype_t)> func) {
	if (func != nullptr) {
		std::pair<std::string, int> funcPair = std::make_pair(funcName, 1);
		this->functions.erase(funcPair);
		this->functions.insert({funcPair, Clack::Function(func)});
	}
}

void Clack::Solver::setFunctionSystem(const std::string &funcName, std::function<mathtype_t(mathtype_t, mathtype_t)> func) {
	if (func != nullptr) {
		std::pair<std::string, int> funcPair = std::make_pair(funcName, 2);
		this->functions.erase(funcPair);
		this->functions.insert({funcPair, Clack::Function(func)});
	}
}

void Clack::Solver::setFunction(const std::string &funcName, const std::string &args, const std::string &funcexpr) {
	std::pair<std::string, int> funcPair = std::make_pair(funcName, Clack::Function::parseArgNames(args).size());

	if (this->functions.count(funcPair) && this->functions.at(funcPair).getSystem()) {
		throw Clack::CommandError("Attempted to set system function " + funcName);
	} else {
		this->functions.erase(funcPair);
		this->functions.insert({funcPair, Clack::Function(args, funcexpr)});
	}
}

Clack::mathtype_t Clack::Solver::solve(std::string expr) {

	Clack::Expression clackExpression = Clack::Expression(expr, this);
	this->lastResult = clackExpression.solve();

	return this->lastResult;
}

bool Clack::Solver::runCommand(std::string cmd) {

	if (cmd.size() == 0 || cmd[0] == '#') return true;

	std::cout << std::setprecision(std::numeric_limits<Clack::mathtype_t>::digits10);

	std::stringstream cmdSS(cmd);
	std::string part;
	cmdSS >> part;

	// TODO: command descriptions

	static std::map<std::string, int> commandList = {{
		{"var", 		0},
		{"file", 		1},
		{"reset", 		3},
		{"varlist", 	4},
		{"clear", 		5},
		{"store", 		6},
		{"deflist",		7},
		{"def", 		8},
		{"save",		9},
		{"help",		10},
		{"fsilent", 	11},
		{"quit",		12},
		{"exit",		12}
	}};

	if (commandList.count(part) == 0) {

		try {

			mathtype_t result = this->solve(cmd);
			std::cout << "Result: " << result << std::endl;

		} catch (const Clack::ExpressionError &exprError) {
			std::cerr << exprError.what() << std::endl;
		}
		
		return true;
	}

	switch(commandList[part]) {
		case 0: {
			cmdSS >> part;
			// We only allow the first character to be a letter,
			// and the rest can only be numbers and letters
			if (!std::isalpha(part[0])) break;
			for (size_t i = 1; i < part.size(); i++) if (!std::isalnum(part[i])) break;
			mathtype_t variableValue = this->solve(cmd.substr(4 + part.size(), std::string::npos));
			this->setVar(part, variableValue);
		} break;
		case 1: {
			std::string fname = cmd.substr(5, std::string::npos);

			std::ifstream f(fname);

			if (!f.is_open())
				throw Clack::CommandError("Unable to open file " + fname);

			std::string line;
			size_t lineNumber = 1;
			while (std::getline(f, line)) {
				if (line.length() > 0) {
					if (!this->fileSilent && line != "fsilent") std::cout << "$ " + line + "\n";

					try {
						this->runCommand(line);
					} catch (const Clack::CommandError &cmdError) {
						std::cerr << cmdError.what() << " -- file " + fname + ", line " << lineNumber << std::endl;
						break;
					} catch (const Clack::ExpressionError &exprError) {
						std::cerr << exprError.what() << " -- file " + fname + ", line " << lineNumber << std::endl;
						break;
					}
				}
				lineNumber++;
			}
		} break;
		case 3: { 
			this->reset(); 
		} break;
		case 4: {
			for (auto v: this->variables) {
			    std::cout << '\'' << v.first << '\'' << " = " << v.second.value << '\n';
			}  
		} break;
		case 5: { 
			std::cout << "\e[2J\e[H"; 
		} break;
		case 6: {
			cmdSS >> part;
			// We only allow the first character to be a letter,
			// and the rest can only be numbers and letters
			if (!std::isalpha(part[0])) break;
			for (size_t i = 1; i < part.size(); i++) if (!std::isalnum(part[i])) break;
			this->setVar(part, this->lastResult);
		} break;
		case 7: {
			for (auto f: this->functions) {
				if (f.second.getSystem()) {
					std::cout << f.first.first + "(" << f.second.getArgCount() << ") = system function\n";
				} else {
					std::cout << f.first.first + "(";
					for (size_t iArg = 0; iArg < f.second.getArgCount(); ++iArg) {
						std::cout << f.second.getArg(iArg) + (iArg == f.second.getArgCount() - 1 ? "" : ", ");
					}
					std::cout << ") = " + f.second.getExpression() << std::endl;
				}
			}
		} break;
		case 8: {

			std::getline(cmdSS, part, ')'); part += ")";

			size_t defStart = std::string("def ").size() + part.size();

			// TODO: this is a bad way of checking for parenthesis
			if (defStart - 1 >= cmd.length())
				throw Clack::CommandError("Function definition requires parenthesis");

			std::string functionExpr = cmd.substr(defStart - 1);

			// if there is no function body, we error
			if (functionExpr.size() == 0 || functionExpr == " ")
				throw Clack::CommandError("No function definition for function " + part);

			size_t parenIndex = part.find("(");
			if (parenIndex == std::string::npos)
				throw Clack::CommandError("No argument list for function " + part);

			std::string arglist = part.substr(parenIndex);
			part = part.substr(0, parenIndex);

			// We only allow the first character to be a letter,
			// and the rest can only be numbers and letters
			if (part[0] == ' ') part.erase(0, 1);
			if (!Clack::varValidFirst(part[0])) break;
			for (size_t i = 1; i < part.size(); i++) if (!Clack::varValid(part[i])) break;
			if (functionExpr[0] == ' ') functionExpr.erase(0, 1);
			this->setFunction(part, arglist, functionExpr);
		} break;
		case 9: {
			cmdSS >> part;
			this->dumpState(part);
		} break;
		case 10: {
			std::cout << "Command List: " << std::endl;
			for(auto iter = commandList.begin(); iter != commandList.end(); ++iter) {
				std::cout << iter->first << std::endl;
			}
		} break;
		case 11: {
			this->fileSilent = !this->fileSilent;
		} break;
		case 12: {
			return false;
		} break;
		default: {
			std::cerr << "Unreachable: default command" << std::endl;
		}
	}

	return true;
}

void Clack::Solver::dumpState(std::string toFile) {
	std::ofstream out(toFile);

	for (auto varpair: this->variables) {
		if (!varpair.second.system)
			out << "var " << varpair.first << " " << varpair.second.value << std::endl;
	}

	for (auto funcpair: this->functions) {
		if (!funcpair.second.getSystem()) {
			out << "def " << funcpair.first.first << "(";
			for (size_t iArg = 0; iArg < funcpair.second.getArgCount(); ++iArg) {
				out << funcpair.second.getArg(iArg) + (iArg == funcpair.second.getArgCount() - 1 ? "" : ", ");
			}
			out << ") " << funcpair.second.getExpression() << std::endl;
		}
	}
}

