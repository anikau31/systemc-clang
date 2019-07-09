#include "adder.hpp"

Adder::Adder(sc_module_name name) : sc_module(name) {
	SC_METHOD(compute);
	sensitive << a << b;
}


void Adder::compute() {
	c.write(a.read() + b.read());
}
