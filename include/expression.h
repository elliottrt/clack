#pragma once

#include <string>
#include <vector>

namespace Clack {

class Solver;

class Expression {

	char c = '\0';
	ssize_t i = -1;
	std::string expr;
	Solver *solver;

	void next(void);
	char peek(void) const;
	bool check(const char _c);
	void to(ssize_t pos);

	double expression(bool explore = true);
	double term(bool explore = true);
	double factor(bool explore = true);

	int depthFindClose(void);

	std::vector<std::string> resolveArgs(std::string &args);

public:

	Expression(const std::string &expr, Solver *solver);

	double solve(void);

};

}
