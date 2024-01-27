#include "expression.h"
#include "clack.h"

#include <iostream>
#include <cmath>

#define EXPR_ERR(M) { std::cout << "ERROR: " << M << std::endl; this->errored = true; return 0; }

void Clack::Expression::next(void) { 
	this->c = ++this->i < this->expr.size() ? this->expr[i] : -1; 
}

char Clack::Expression::peek(void) const { 
	return this->expr[this->i]; 
}

bool Clack::Expression::check(const char _c) {
	while (this->c == ' ') next();
	if (this->c == _c) { 
		next(); 
		return true; 
	}
	return false;
}

Clack::Expression::Expression(std::string &e, Solver *solver) {
	this->expr = e;
	this->solver = solver;
	this->next();
	this->errored = false;
}

double Clack::Expression::solve(void) {
	this->errored = false;
	double result = this->expression();
	return this->errored ? 0 : result;
}

double Clack::Expression::expression(void) {
	double x = this->term();
    for (;;) {
        if (this->check('+')) x += this->term(); // addition
        else if (this->check('-')) x -= this->term(); // subtraction
        else return x;
    }
}

double Clack::Expression::term(void) {
	double x = this->factor();
    for (;;) {
        if (this->check('*')) x *= this->factor(); // multiplication
        else if (this->check('/')) x /= this->factor(); // division
        else return x;
    }
}

double Clack::Expression::factor(void) {
	if (this->check('+')) return this->factor(); // unary plus
    if (this->check('-')) return -this->factor(); // unary minus

    if (this->expr.length() == 0) return 0;
        
    double x;
    int startPos = this->i;
    std::string argList = "";
    if (this->check('(')) { // parentheses
    	std::cout << this->expr.substr(this->i) << " | " << this->c << std::endl;
        x = this->expression();
        std::cout << this->expr.substr(this->i) << " | " << this->c << std::endl;
        if (!this->check(')')) {
        	EXPR_ERR("Missing ')'");
        }
    } else if ((this->c >= '0' && this->c <= '9') || this->c == '.') { // numbers
        while ((this->c >= '0' && this->c <= '9') || this->c == '.') this->next();
        x = std::stod(this->expr.substr(startPos, this->i - startPos));
    } else if (Clack::varValidFirst(this->c)) { // functions and variables
        while (Clack::varValid(this->c)) this->next();
        std::string ident = this->expr.substr(startPos, this->i - startPos);
        if (this->check('(')) { // functions
        	int closingLocation = this->depthFindClose();
        	std::cout << this->expr.substr(startPos, closingLocation) << std::endl;
        	argList = this->expr.substr(this->i, closingLocation - this->i);
        	//std::cout << argList << std::endl;
            //x = this->expression();
            if (this->errored) return 0;
            this->i = closingLocation;
        } else if (this->peek() != '(') { // variables
        	if (this->solver->varExists(ident) == false) {
        		std::cout << "Unknown variable " << ident << std::endl;
        		this->errored = true;
        		return 0;
        	}
        	x = this->solver->getVar(ident);
        	return x;
        } else { // If we have more to do
    		x = this->factor();
    	}
    	if (this->solver->functionExists(ident) == false) {
    		EXPR_ERR("Unknown function " + ident);
    	}
    	std::vector<std::string> resolvedArgs = this->resolveArgs(argList);
    	std::string functionExpression = this->solver->callFunction(ident, resolvedArgs);
    	//std::cout << functionExpression << std::endl;
        x = this->solver->solve(functionExpression);
        //std::cout << "solved and got " << x << std::endl;
    } else {
    	if (this->c == -1) {
    		EXPR_ERR("Reached end of expression")
    	}
    	else 
    		EXPR_ERR("Unexpected " << this->c << " " << (int) this->c)
    }
  
    if (this->check('^')) x = std::pow(x, this->factor()); // exponentiation
    
    return x;
}

int Clack::Expression::depthFindClose(void) {
	int ci = this->i - 1;
	int depth = 1;
	while (depth > 0) {

		char c_ = this->expr[++ci];
		if (c_ == '(') ++depth;
		else if (c_ == ')') --depth;
		
		if (ci >= this->expr.length())
			EXPR_ERR("No closing parenthesis")

	}
	return ci - 1;
}

std::vector<std::string> Clack::Expression::resolveArgs(std::string &args) {
	// remove spaces
	args.erase(std::remove_if(args.begin(), args.end(), (int(*)(int))std::isspace), args.end());
	// put into stream
	std::stringstream argStream(args);

	std::string arg;
	std::vector<std::string> argVector;
	// TODO: getline will get commas inside of functions used as args
	//while (std::getline(argStream, arg, ',')) {
		//argVector.push_back(Expression(arg, this->solver).solve());
	//}
	while (std::getline(argStream, arg, ',')) {

		int depth = std::count(arg.begin(), arg.end(), '(') - std::count(arg.begin(), arg.end(), ')');

		if (depth != 0) arg += ',';

		while (depth != 0) { 
			char n = argStream.get();

			if (n == -1) break;

			if (n == ')') --depth;
			if (n == '(') ++depth;

			arg += n;
		}

		std::string resolvedArg = (arg[0] == Clack::functionPointerPrefix) ? 
											arg : 
											std::to_string(Expression(arg, this->solver).solve());

		//std::cout << resolvedArg << std::endl;

		if (arg.size() != 0) argVector.push_back(resolvedArg);
	}
	return argVector;
}

