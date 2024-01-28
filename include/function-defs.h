#pragma once

#include <cmath>
#include <random>

// printing

double print(double x) {
	std::cout << x;
	return x;
}

double printa(double a) {
	char c = (char) a;
	std::cout << c;
	return a;
}

// random numbers

std::minstd_rand random_;

void seed_(double s) { random_.seed(s); }

double rand_(void) { return random_() / (double) std::minstd_rand::max(); }

// math

double logbase(double x, double base) {
	return std::log2(x) / std::log2(base);
}

// TODO: sign should be possible in std.clk with short circuiting (which is implemented already)

double sign(double x) {
	if (x == 0) return 0;
	return x / std::abs(x);
}

// TODO: add <, <=, >, >=, == (or =), !=
// as binary operators - greater and less are below addition and subtraction
// and equals and not equals are below that

double equ(double x, double y) { return x == y; }

double nequ(double x, double y) { return x != y; }

double less(double x, double y) { return x < y; }

double lessequ(double x, double y) { return x <= y; }

double greater(double x, double y) { return x > y; }

double greaterequ(double x, double y) { return x >= y; }
