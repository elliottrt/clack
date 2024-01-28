#pragma once

#include <string>
#include <vector>

namespace Clack {

class Solver;

class Expression {

	char c = '\0';
	int i = -1;
	std::string expr;
	Solver *solver;

	void next(void);
	char peek(void) const;
	bool check(const char _c);
	void to(int pos);

	double expression(void);
	double term(void);
	double factor(bool explore = true);

	int depthFindClose(void);

	std::vector<std::string> resolveArgs(std::string &args);

public:

	Expression(const std::string &expr, Solver *solver);

	double solve(void);

};

}
