#pragma once

#include <cmath>

double logbase(double x, double base) {
	return std::log2(x) / std::log2(base);
}

double sign(double x) {
	if (x == 0) return 0;
	return x / std::abs(x);
}

double equ(double x, double y) { return x == y; }

double nequ(double x, double y) { return x != y; }

double less(double x, double y) { return x < y; }

double lessequ(double x, double y) { return x <= y; }

double greater(double x, double y) { return x > y; }

double greaterequ(double x, double y) { return x >= y; }
