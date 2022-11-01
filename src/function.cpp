#include "function.h"

std::string Clack::toSafeName(const std::string &var) {
	return Clack::Function::safePrefix + var + Clack::Function::safeSuffix;
}

std::string Clack::unSafeName(const std::string &var) {
	std::string undone = var;
	//undone.erase(0, Function::safePrefix.length());
	//undone.erase(undone.size() - Function::safeSuffix.length(), Function::safeSuffix.length());
	undone.erase(0, 1);
	undone.erase(undone.size() - 1, 1);
	return undone;
}

void Clack::Function::makeReplaceSafe(void) {
	this->safeExpression = functionExpression;
	for (std::string varName: this->argNames) {
		size_t start_pos = 0;
		std::string safeVarName = Clack::toSafeName(varName);
	    while ((start_pos = this->safeExpression.find(varName, start_pos)) != std::string::npos) {

	    	size_t nextComma = this->safeExpression.find(",", start_pos);
	    	size_t nextOpenParen = this->safeExpression.find("(", start_pos);
	    	size_t nextCloseParen = this->safeExpression.find(")", start_pos);
	    	char nextChar = this->safeExpression[start_pos + varName.size()];
	    	bool shouldReplace = nextComma < nextOpenParen || 
	    						 nextCloseParen < nextOpenParen || 
	    						 !std::isalpha(nextChar); // || 
	    						 //!std::isspace(nextChar);
	    	//std::cout << nextChar << std::endl;

	    	//std::cout << this->safeExpression.substr(start_pos) << std::endl;
	    	//std::cout << shouldReplace << std::endl;

	        if (shouldReplace) {
	        	this->safeExpression.replace(start_pos, varName.length(), safeVarName);
	        	start_pos += safeVarName.length();
	        } else start_pos += varName.length();
	    }
	}
	for (size_t i = 0; i < this->argNames.size(); ++i) {
		this->argNames[i] = Clack::toSafeName(this->argNames[i]);
	}
}

Clack::Function::Function(std::function<void(double)> function) {
	this->voidfunction1 = function;
	this->argNames.push_back("x");
}

Clack::Function::Function(std::function<double(void)> function) {
	this->function0 = function;
}

Clack::Function::Function(std::function<double(double)> function) {
	this->function1 = function;
	this->argNames.push_back("x");
}

Clack::Function::Function(std::function<double(double, double)> function) {
	this->function2 = function;
	this->argNames.push_back("x");
	this->argNames.push_back("y");
}

Clack::Function::Function(const std::string &sig, const std::string &funcexpr) {
	this->functionExpression = funcexpr;
	this->system = false;

	std::string signature = sig;

	// remove open paren
	signature.erase(0, 1);
	// remove close paren
	signature.erase(signature.size()-1, 1);

	// remove whitespace
	signature.erase(std::remove_if(signature.begin(), 
								   signature.end(), 
								   (int(*)(int))std::isspace), 
								   signature.end());

	std::stringstream argStream(signature);

	std::string arg;
	while (std::getline(argStream, arg, ',')) {
		this->argNames.push_back(arg);
	}

	this->makeReplaceSafe();

	//std::cout << this->safeExpression << std::endl;

	//std::cout << this->functionExpression << std::endl;

	//for (auto v: this->argNames) std::cout << v << std::endl;
}

std::string Clack::Function::call(const std::vector<double> &args) {
	if (args.size() != this->argNames.size()) {
		std::cout << "Expected " << this->argNames.size() << " arguments, got " << args.size() << std::endl;
	}
	if (this->system) {
		switch (this->argNames.size()) {
			case 0: return std::to_string(this->function0());
			case 1: {
				if (this->function1)
					return std::to_string(this->function1(args[0]));
				else
					return std::to_string(0.0);
			}
			case 2: return std::to_string(this->function2(args[0], args[1]));
			default: std::cout << "unreachable: system function with 3+ args" << std::endl; return "0";
		}
	} else {

		std::string expr = "(" + this->safeExpression + ")";

		for (int arg = 0; arg < this->argNames.size(); arg++) {
			std::string var = argNames[arg];
			std::string val = std::to_string(args[arg]);

			// Code from https://gist.github.com/GenesisFR/cceaf433d5b42dcdddecdddee0657292
			size_t start_pos = 0;
		    while ((start_pos = expr.find(var, start_pos)) != std::string::npos)
		    {
		        expr.replace(start_pos, var.length(), val);
		        start_pos += val.length(); // Handles case where 'to' is a substring of 'from'
		    }
		}

	 	return expr;
	}
}
