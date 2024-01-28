#include <iostream>
#include <string>
#include "include/clack.h"

/*
TODO: print(f) and printa(a) -> print double or ascii, return argument
TODO: characters as numbers -> 'A' = 65
*/

int main(void) {

	Clack::Solver solver = Clack::Solver();

	while (1) {
		std::string in;
		std::cout << "$ ";
		std::getline(std::cin, in);
		if (in.size() == 0) break;
		solver.runCommand(in);
	}

	return 0;
}