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
