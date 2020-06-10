#include <systemc.h>
#include "avg.cpp"

SC_MODULE(DUT) {

  sc_signal<sc_bv<8> > input;
  sc_signal<sc_bv<8> > output;
	sc_signal<bool> clk;

	avg avg_unit;

  SC_CTOR(DUT) : avg_unit{"avger"} {

	avg_unit.clk(clk); 
  avg_unit.i_data(input); 
  avg_unit.o_data(output);
  }

};


int sc_main(int argc, char* argv[])	{
  DUT dut("syn_dut");

	return 0;
}
