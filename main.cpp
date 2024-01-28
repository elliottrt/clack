#include <iostream>
#include <string>

#include "include/clack.h"

/*
TODO: a way to change variables programmatically
TODO: character literal ' ' (space) won't work
TODO: error messages should show the expression they're evaluating
*/

int main(void) {

	Clack::Solver solver = Clack::Solver();

	while (1) {
		std::string in;
		std::cout << "$ ";
		std::getline(std::cin, in);
		if (in.size() == 0) break;

		try { solver.runCommand(in); }
		catch (const Clack::CommandError &cmdError) {
			std::cerr << cmdError.what() << std::endl;
		}

	}

	return 0;
}