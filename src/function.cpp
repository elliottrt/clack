#include "function.h"
#include "clack.h"
#include "expression_error.h"

void Clack::Function::makeReplaceSafe(void) {
	this->safeExpression = functionExpression;
	for (std::string varName: this->argNames) {
		size_t start_pos = 0;
		std::string safeVarName = Clack::Function::toSafeName(varName);
		bool functionPointer = varName[0] == Clack::functionPointerPrefix;
	    while ((start_pos = this->safeExpression.find(varName, start_pos)) != std::string::npos) {

	    	bool shouldReplace = false;

	    	if (functionPointer) {
	    		size_t tempPos = start_pos + varName.size();
	    		while (tempPos < this->safeExpression.size() && std::isspace(this->safeExpression[tempPos])) 
	    			++tempPos;
	    		shouldReplace = this->safeExpression[tempPos] == '(' || this->safeExpression[tempPos] == ',';
	    	} else {
		    	size_t nextComma = this->safeExpression.find(",", start_pos);
		    	size_t nextOpenParen = this->safeExpression.find("(", start_pos);
		    	size_t nextCloseParen = this->safeExpression.find(")", start_pos);
		    	char nextChar = this->safeExpression[start_pos + varName.size()];
		    	bool notFunction = !std::isalpha(nextChar) && nextChar != '(';
		    	shouldReplace = nextComma < nextOpenParen || 
		    					nextCloseParen < nextOpenParen || 
		    					notFunction; // || 
		    						 //!std::isspace(nextChar);
		    	//std::cout << nextChar << std::endl;

		    	//std::cout << this->safeExpression.substr(start_pos) << std::endl;
		    	//std::cout << shouldReplace << std::endl;
	    	}

	        if (shouldReplace) {
	        	this->safeExpression.replace(start_pos, varName.length(), safeVarName);
	        	start_pos += safeVarName.length();
	        } else start_pos += varName.length();
	    }
	}
	for (size_t i = 0; i < this->argNames.size(); ++i) {
		this->argNames[i] = Clack::Function::toSafeName(this->argNames[i]);
	}
}

std::string Clack::Function::toSafeName(const std::string &var) {
	return Clack::Function::safePrefix + var + Clack::Function::safeSuffix;
}

std::string Clack::Function::unSafeName(const std::string &var) {
	std::string undone = var;
	//undone.erase(0, Function::safePrefix.length());
	//undone.erase(undone.size() - Function::safeSuffix.length(), Function::safeSuffix.length());
	undone.erase(0, 1);
	undone.erase(undone.size() - 1, 1);
	return undone;
}

std::vector<std::string> Clack::Function::parseArgNames(const std::string &args) {

	std::string signature = args;
	std::vector<std::string> argNames;

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
		argNames.push_back(arg);
	}

	return argNames;
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

	this->argNames = Clack::Function::parseArgNames(sig);

	this->makeReplaceSafe();

	//std::cout << this->safeExpression << std::endl;
}

std::string Clack::Function::call(const std::vector<std::string> &args) {

	if (args.size() != this->argNames.size()) {
		throw Clack::ExpressionError("Expected " + std::to_string(this->argNames.size()) + " arguments, got " + std::to_string(args.size()));
	}

	if (this->system) {
		switch (this->argNames.size()) {
			case 0: return std::to_string(this->function0());
			case 1: {
				if (this->function1)
					return std::to_string(this->function1(std::stod(args[0])));
				else
					return std::to_string(0.0);
			}
			case 2: return std::to_string(this->function2(std::stod(args[0]), std::stod(args[1])));
			default: std::cout << "unreachable: system function with 3+ args" << std::endl; return "0";
		}
	} else {

		std::string expr = "(" + this->safeExpression + ")";

		for (int arg = 0; arg < this->argNames.size(); arg++) {
			// check if the expected argument is a function pointer
			std::string var = argNames[arg];
			// check char 1 to skip safe prefix
			bool functionPointer = var[1] == Clack::functionPointerPrefix;

			if (functionPointer && args[arg][0] != Clack::functionPointerPrefix) {
				throw Clack::ExpressionError("Expected function pointer for argument " + std::to_string(arg));
			}

			if (!functionPointer && args[arg][0] == Clack::functionPointerPrefix) {
				throw Clack::ExpressionError("Unexpected function pointer for argument " + std::to_string(arg));
			}

			// Code from https://gist.github.com/GenesisFR/cceaf433d5b42dcdddecdddee0657292
			size_t start_pos = 0;
		    while ((start_pos = expr.find(var, start_pos)) != std::string::npos)
		    {
		    	std::string replacement = args[arg];

		    	if (functionPointer) {
		    		size_t tempPos = start_pos + var.size();
		    		while (tempPos < expr.size() && std::isspace(expr[tempPos])) 
		    			++tempPos;
		    		bool functionCall = expr[tempPos] == '(';
			    	if (functionCall) {
			    		replacement.erase(0, 1);
			    	}
		    	}

		        expr.replace(start_pos, var.length(), replacement);
		        start_pos += args[arg].length(); // Handles case where 'to' is a substring of 'from'
		    }
		}

	 	return expr;
	}
}
