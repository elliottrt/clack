#pragma once

#include "clack.h"

#include <cmath>
#include <random>

// printing

Clack::mathtype_t print(Clack::mathtype_t x) {
	std::cout << x;
	return x;
}

Clack::mathtype_t printa(Clack::mathtype_t a) {
	char c = (char) a;
	std::cout << c;
	return a;
}

// random numbers

std::minstd_rand random_;

Clack::mathtype_t seed_(Clack::mathtype_t s) { 
	random_.seed(s); 
	return s;
}

Clack::mathtype_t rand_(void) { return random_() / (Clack::mathtype_t) std::minstd_rand::max(); }

// math

Clack::mathtype_t logbase(Clack::mathtype_t x, Clack::mathtype_t base) {
	return std::log2(x) / std::log2(base);
}
