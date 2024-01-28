#include "expression.h"
#include "clack.h"

#include <iostream>
#include <cmath>

#define EXPR_ERR_R(M, R) { std::cout << "ERROR: " << M << std::endl; this->errored = true; return R; }
#define EXPR_ERR(M) EXPR_ERR_R(M, 0)

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

void Clack::Expression::to(int pos) {
	this->i = pos;
	this->c = pos >= 0 && pos < this->expr.length() ? this->expr[pos] : -1 ;
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

// the "&& x != 0" allows short circuiting?
double Clack::Expression::term(void) {
	double x = this->factor();
	//std::cout << this->expr[this->i] << " rest of expr (term): " << this->expr.substr(this->i) << std::endl;
    for (;;) {
        if (this->check('*') && x != 0) x *= this->factor(); // multiplication
        else if (this->check('/') && x != 0) x /= this->factor(); // division
        else return x;
    }
}

double Clack::Expression::factor(void) {
	if (this->check('+')) return this->factor(); // unary plus
    if (this->check('-')) return -this->factor(); // unary minus

    if (this->expr.length() == 0) return 0;

	// std::cout << "rest of expr (start): " << this->expr.substr(this->i) << std::endl;
        
    double x;
    int startPos = this->i;
    std::string argList = "";

    if (this->peek() == '(') { // parentheses

		//std::cout << "rest of expr (paren): " << this->expr.substr(this->i) << std::endl;

		int closingLocation = this->depthFindClose();

		// also remove last paren
		std::string parenExpression = this->expr.substr(this->i + 1, closingLocation - this->i - 1);

		//std::cout << "paren expr: " << parenExpression << std::endl;

		// solve whatever's in the parenthesis
        x = this->solver->solve(parenExpression);

		// i = 1 past closing paren
        this->to(closingLocation + 1);

	// TODO: check for multiple decimal points in number
    } else if ((this->c >= '0' && this->c <= '9') || this->c == '.') { // numbers

        while ((this->c >= '0' && this->c <= '9') || this->c == '.') 
			this->next();

        x = std::stod(this->expr.substr(startPos, this->i - startPos));

    } else if (Clack::varValidFirst(this->c)) { // functions and variables

        while (Clack::varValid(this->c)) 
			this->next();

        std::string ident = this->expr.substr(startPos, this->i - startPos);

        if (this->peek() == '(') { // functions

			// position 1 past closing paren
        	int closingLocation = this->depthFindClose();

        	argList = this->expr.substr(this->i + 1, closingLocation - this->i - 1);

            if (this->errored) return 0;

            this->to(closingLocation + 1);

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

		/*
		for (auto s : resolvedArgs) {
			//std::cout << "arg of " << ident << ": " << s << std::endl;
		}
		*/

    	std::string functionExpression = this->solver->callFunction(ident, resolvedArgs);
        x = this->solver->solve(functionExpression);

        //std::cout << ident << "(" << argList << ") = " << functionExpression << " solved and got " << x << std::endl;
		//std::cout << "rest of expr (funct): " << this->expr.substr(this->i) << std::endl;

    } else {
		
    	if (this->c == -1) {
    		EXPR_ERR("Reached end of expression")
    	}
    	else 
    		EXPR_ERR("Unexpected " << this->c << " " << (int) this->c)

    }
    
    return x;
}

// should be on a paren when calling the function
int Clack::Expression::depthFindClose(void) {

	int ci = this->i;
	int depth = 1;
	while (depth > 0) {

		char c_ = this->expr[++ci];

		if (c_ == '(') 
			++depth;
		else if (c_ == ')') 
			--depth;
		
		if (ci >= this->expr.length())
			EXPR_ERR("No closing parenthesis")

	}
	return ci;

}

std::vector<std::string> Clack::Expression::resolveArgs(std::string &args) {

	// remove spaces
	args.erase(std::remove_if(args.begin(), args.end(), (int(*)(int))std::isspace), args.end());
	// put into stream
	std::stringstream argStream(args);

	std::string arg;
	std::vector<std::string> argVector;

	// TODO: getline will get commas inside of functions used as args

	// 4,equ(x,6),10
	while (std::getline(argStream, arg, ',')) {

		int depth = std::count(arg.begin(), arg.end(), '(') - std::count(arg.begin(), arg.end(), ')');

		if (depth != 0) arg += ',';

		while (depth != 0) { 
			char n = argStream.get();

			if (n == -1) break;
			else if (n == ')') --depth;
			else if (n == '(') ++depth;

			arg += n;
		}

		std::string resolvedArg = (arg[0] == Clack::functionPointerPrefix) ? 
											arg : 
											std::to_string(Expression(arg, this->solver).solve());

		if (arg.size() != 0) argVector.push_back(resolvedArg);
	}

	return argVector;

}

