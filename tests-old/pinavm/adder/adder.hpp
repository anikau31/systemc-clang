//#include "systemc.h"
#include <systemc.h>

SC_MODULE(Adder) {
	
	
	sc_in<sc_uint<22> > XYZ;

	sc_in<sc_uint<8> > a, b;
	sc_out<sc_uint<8> > c;
	
	SC_CTOR(Adder);
	
	void compute();
	
};

