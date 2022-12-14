#include "clack.h"
#include "expression.h"
#include "function-defs.h"

#include <sstream>
#include <fstream>
#include <random>

std::minstd_rand random_;

void seed_(double s) { random_.seed(s); }

double rand_(void) { return random_() / (double) std::minstd_rand::max(); }


void Clack::Solver::loadBuiltins(void) {

	setVarSystem("pi", 3.14159265358979323846);
	setVarSystem("e",  2.71828182845904523536);

	setFunctionSystemVoid("seed", &seed_);

	setFunctionSystem("rand", &rand_);

	setFunctionSystem("sqrt", (double(*)(double))&std::sqrt);
	setFunctionSystem("sin",  (double(*)(double))&std::sin);
	setFunctionSystem("cos",  (double(*)(double))&std::cos);
	setFunctionSystem("tan",  (double(*)(double))&std::tan);
	setFunctionSystem("asin",  (double(*)(double))&std::asin);
	setFunctionSystem("acos",  (double(*)(double))&std::acos);
	setFunctionSystem("atan",  (double(*)(double))&std::atan);
	setFunctionSystem("abs",  (double(*)(double))&std::abs);
	setFunctionSystem("ln",  (double(*)(double))&std::log);
	setFunctionSystem("ceil",  (double(*)(double))&std::ceil);
	setFunctionSystem("floor",  (double(*)(double))&std::floor);
	setFunctionSystem("round",  (double(*)(double))&std::round);
	setFunctionSystem("sign", (double(*)(double))&sign);

	setFunctionSystem("mod", (double(*)(double, double))&std::fmod);
	setFunctionSystem("max", (double(*)(double, double))&std::fmax);
	setFunctionSystem("min", (double(*)(double, double))&std::fmin);
	setFunctionSystem("log", &logbase);
	setFunctionSystem("equ", &equ);
	setFunctionSystem("nequ", &nequ);
	setFunctionSystem("less", &less);
	setFunctionSystem("lessequ", &lessequ);
	setFunctionSystem("greater", &greater);
	setFunctionSystem("greaterequ", &greaterequ);
}

void Clack::Solver::reset(void) { 
	auto viter = this->variables.begin();
	auto vendIter = this->variables.end();

	for(; viter != vendIter; ) {
		if (!viter->second.getSystem()) viter = this->variables.erase(viter);
		else ++viter;
	}

	auto fiter = this->functions.begin();
	auto fendIter = this->functions.end();

	for(; fiter != fendIter; ) {
		if (!fiter->second.getSystem()) fiter = this->functions.erase(fiter);
		else ++fiter;
	}
}

void Clack::Solver::setVar(const std::string var, const double val, const bool system) {
	if (this->variables.count(var) && this->variables.at(var).getSystem()) {
		std::cerr << "ERROR: Attempted to set system variable " << var << std::endl;
	} else {
		this->variables.erase(var);
		this->variables.insert({var, Variable(val, system)});
	}
}

double Clack::Solver::getVar(const std::string &name) const {
	if (this->varExists(name)) return this->variables.at(name).getValue();
	else {
		std::cerr << "ERROR: No variable named " << name << std::endl;
		return 0;
	}
}

bool Clack::Solver::varExists(const std::string &name) const {
	return this->variables.count(name);
}

size_t Clack::Solver::getVarCount(void) const {
	return this->variables.size();
}

std::string Clack::Solver::callFunction(const std::string &name, const std::vector<std::string> &args) {
	std::pair<std::string, int> funcPair = std::make_pair(name, args.size());
	if (this->functionExists(name, args.size())) {
		return this->functions.at(funcPair).call(args);
	} else if (this->functionExists(name)) {
		std::cerr << "ERROR: Wrong number of arguments for function " << name << std::endl;
	} else {
		std::cerr << "ERROR: No function named '" << name << std::endl;
	}
	return "0";
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

double Clack::Solver::solve(std::string expr) {
	this->lastResult = Clack::Expression(expr, this).solve();
	return this->lastResult;
}

void Clack::Solver::runCommand(std::string cmd) {

	if (cmd.size() == 0 || cmd[0] == '#') return;

	std::stringstream cmdSS(cmd);
	std::string part;
	cmdSS >> part;

	// TODO: turn map into map of lambdas

	static std::map<std::string, int> commandList = {{
		{"var", 	0},
		{"file", 	1},
		{"reset", 	3},
		{"varlist", 4},
		{"clear", 	5},
		{"store", 	6},
		{"deflist",	7},
		{"def", 	8},
		{"save",	9},
		{"help",	10},
		{"fsilent", 11}
	}};

	if (commandList.count(part) == 0) {
		//std::cout << "Unknown command \'" + part + "\'\n";
		std::cout << this->solve(cmd) << std::endl;
		return;
	}

	switch(commandList[part]) {
		case 0: {
			cmdSS >> part;
			// We only allow the first character to be a letter,
			// and the rest can only be numbers and letters
			if (!std::isalpha(part[0])) break;
			for (int i = 1; i < part.size(); i++) if (!std::isalnum(part[i])) break;
			double variableValue = this->solve(cmd.substr(4 + part.size(), std::string::npos));
			this->setVar(part, variableValue);
		} break;
		case 1: {
			std::ifstream f(cmd.substr(5, std::string::npos));
			std::string line;
			int cmds_ran = 0;
			while (std::getline(f, line)) {
				if (line.length() > 0) {
					if (!this->fileSilent && line != "fsilent") std::cout << ">" + line + "\n";
					this->runCommand(line);
					cmds_ran++;
				}
			}
			std::cout << "Ran " << cmds_ran << " commands from \'" << cmd.substr(5, std::string::npos) << "\'\n";
		} break;
		case 3: { this->reset(); } break;
		case 4: {
			for (auto v: this->variables) {
			    std::cout << '\'' << v.first << '\'' << " = " << v.second.getValue() << '\n';
			}  
		} break;
		case 5: { std::cout << "\e[2J\e[H"; } break;
		case 6: {
			cmdSS >> part;
			// We only allow the first character to be a letter,
			// and the rest can only be numbers and letters
			if (!std::isalpha(part[0])) break;
			for (int i = 1; i < part.size(); i++) if (!std::isalnum(part[i])) break;
			this->setVar(part, this->lastResult);
		} break;
		case 7: {
			for (auto f: this->functions) {
				if (f.second.getSystem()) {
					std::cout << f.first.first + "(" << f.second.getArgCount() << ") = system function\n";
				} else {
					// TODO: instead of having to redo this each time,
					// we should have two expressions: safe and dangerous
					//f.second.makeReplaceDanger();
					std::cout << f.first.first + "(";
					for (size_t iArg = 0; iArg < f.second.getArgCount(); ++iArg) {
						std::cout << f.second.getArg(iArg) + (iArg == f.second.getArgCount() - 1 ? "" : ", ");
					}
					std::cout << ") = " + f.second.getExpression() << std::endl;
					//f.second.makeReplaceSafe();
				}
			} 
		} break;
		case 8: {
			//cmdSS >> part;

			std::getline(cmdSS, part, ')'); part += ")";

			int defStart = 4 + part.size();

			std::string functionExpr = cmd.substr(defStart - 1);
			// if there is no funciton body, we error
			if (functionExpr.size() == 0 || (functionExpr.size() == 1 && functionExpr == " ")) {
				std::cerr << "ERROR: No function definition for function " << part << std::endl;
				return;
			}

			int parenIndex = part.find("(");
			if (parenIndex == std::string::npos) {
				std::cerr << "ERROR: No argument list for function " << part << std::endl;
				return;
			}

			std::string arglist = part.substr(parenIndex);
			part = part.substr(0, parenIndex);

			// We only allow the first character to be a letter,
			// and the rest can only be numbers and letters
			if (part[0] == ' ') part.erase(0, 1);
			if (!Clack::varValidFirst(part[0])) break;
			for (int i = 1; i < part.size(); i++) if (!Clack::varValid(part[i])) break;
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
		default: {
			std::cerr << "ERROR: Unreachable: default command" << std::endl;
		}
	}
}

void Clack::Solver::dumpState(std::string toFile) {
	std::ofstream out(toFile);

	for (auto varpair: this->variables) {
		if (!varpair.second.getSystem())
			out << "var " << varpair.first << " " << varpair.second.getValue() << std::endl;
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

