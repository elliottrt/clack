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
	bool errored;

	void next(void);
	char peek(void) const;
	bool check(const char _c);

	double expression(void);
	double term(void);
	double factor(void);

	int depthFindClose(void);

	std::vector<std::string> resolveArgs(std::string &args);

public:

	Expression(std::string &e, Solver *solver);

	double solve(void);

};

}
