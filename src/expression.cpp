#include "expression.h"
#include "clack.h"
#include "expression_error.h"

#include <iostream>
#include <cmath>

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

Clack::Expression::Expression(const std::string &expr, Solver *solver) {
	this->expr = expr;
	this->solver = solver;
	this->next();
}

double Clack::Expression::solve(void) {
	return this->expression();
}

double Clack::Expression::expression(void) {
	double x = this->term();
    for (;;) {
        if (this->check('+')) x += this->term(); // addition
        else if (this->check('-')) x -= this->term(); // subtraction
        else return x;
    }
}

// the "&& x != 0" allows short circuiting
double Clack::Expression::term(void) {
	double x = this->factor();
	//std::cout << this->expr[this->i] << " rest of expr (term): " << this->expr.substr(this->i) << std::endl;
    for (;;) {
        if (this->check('*')) {
			if (x != 0)
				x *= this->factor(); // multiplication
			else // short circuit by discarding next without function calls
				this->factor(false);
		}
        else if (this->check('/')) {
			if (x != 0)
				x /= this->factor(); // division
			else // short circuit by discarding next without function calls
				this->factor(false);
		}
		else if (this->peek() == '(') throw Clack::ExpressionError("Unexpected '(' at position " + std::to_string(this->i));
        else return x;
    }
}

// explore: do we evaluate variables and function calls?
// we don't want to when we short circuit
double Clack::Expression::factor(bool explore) {
	if (this->check('+')) return this->factor(); // unary plus
    if (this->check('-')) return -this->factor(); // unary minus

    if (this->expr.length() == 0) return 0;

	// std::cout << "rest of expr (start): " << this->expr.substr(this->i) << std::endl;
        
    double x;
    int startPos = this->i;
    std::string argList = "";

    if (this->peek() == '(') { // parentheses

		// std::cout << "rest of expr (paren): " << this->expr.substr(this->i) << std::endl;

		int closingLocation = this->depthFindClose();

		// also remove last paren
		std::string parenExpression = this->expr.substr(this->i + 1, closingLocation - this->i - 1);

		//std::cout << "paren expr: " << parenExpression << std::endl;

		// solve whatever's in the parenthesis
        if (explore) x = this->solver->solve(parenExpression);

		// i = 1 past closing paren
        this->to(closingLocation + 1);

    } else if ((this->c >= '0' && this->c <= '9') || this->c == '.') { // numbers

        while ((this->c >= '0' && this->c <= '9') || this->c == '.') 
			this->next();

		std::string number = this->expr.substr(startPos, this->i - startPos);

		// TODO: replace this with try/catch around std::stod?
		if (number == "." || std::count(number.cbegin(), number.cend(), '.') > 1)
			throw Clack::ExpressionError("Invalid number literal " + number);

        x = std::stod(number);

	} else if (this->check('\'')) {

		x = this->peek();

		this->next();

		if (!this->check('\''))
			throw Clack::ExpressionError("Expected \' following \'" + std::string(1, (char) x));

		return x;

    } else if (Clack::varValidFirst(this->c)) { // functions and variables

        while (Clack::varValid(this->c)) 
			this->next();

        std::string ident = this->expr.substr(startPos, this->i - startPos);

        if (this->peek() == '(') { // functions

			// position 1 past closing paren
        	int closingLocation = this->depthFindClose();

        	argList = this->expr.substr(this->i + 1, closingLocation - this->i - 1);

            this->to(closingLocation + 1);

        } else if (this->peek() != '(') { // variables

        	if (this->solver->varExists(ident) == false)
				throw Clack::ExpressionError("Unknown variable " + ident);

        	x = this->solver->getVar(ident);
        	return x;

        } else { // If we have more to do

    		if (explore) x = this->factor();
			
    	}

    	if (this->solver->functionExists(ident) == false)
    		throw Clack::ExpressionError("Unknown function " + ident);

		if (explore) {
			std::vector<std::string> resolvedArgs = this->resolveArgs(argList);

			/*
			for (auto s : resolvedArgs) {
				//std::cout << "arg of " << ident << ": " << s << std::endl;
			}
			*/

			std::string functionExpression = this->solver->callFunction(ident, resolvedArgs);

			x = this->solver->solve(functionExpression);
		}

        //std::cout << ident << "(" << argList << ") = " << functionExpression << " solved and got " << x << std::endl;
		//std::cout << "rest of expr (funct): " << this->expr.substr(this->i) << std::endl;

    } else {
		
    	if (this->c == -1)
    		throw Clack::ExpressionError("Reached end of expression");
    	else 
    		throw Clack::ExpressionError("Unexpected character '" + std::string(1, this->c ) + "' (" + std::to_string((int) this->c) + ") at position " + std::to_string(this->i));

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
			throw Clack::ExpressionError("No closing parenthesis to match position " + std::to_string(this->i));

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

