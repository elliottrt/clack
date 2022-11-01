#pragma once

namespace Clack {

class Variable {

	double value = 0.0;
	bool system = false;

public:
	
	Variable(const double value, const bool system = false): value(value), system(system) {}

	void setValue(const double value) { this->value = value; }
	double getValue(void) const { return this->value; }

	bool getSystem(void) const { return this->system; }
	
};

}