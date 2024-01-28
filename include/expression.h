#pragma once

#include "clack.h"

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

	mathtype_t expression(bool explore = true);
	mathtype_t term(bool explore = true);
	mathtype_t factor(bool explore = true);

	int depthFindClose(void);

	std::vector<std::string> resolveArgs(std::string &args);

public:

	Expression(const std::string &expr, Solver *solver);

	mathtype_t solve(void);

};

}
