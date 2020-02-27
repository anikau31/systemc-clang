#include "systemc.h"
SC_MODULE(topadd2) {
  sc_in_clk clk;
  sc_in<int> in_port_1;
  sc_in<int> in_port_2;
  sc_out<int> out_port;

  SC_CTOR(topadd2) {
    SC_METHOD(topEntry);
    sensitive<<clk.pos();
  }

  void topEntry() {
    out_port.write(in_port_1.read() ^ in_port_2.read());
	}

};

int sc_main(int argc, char *argv[]){
	
  sc_clock CLOCK("clock", 5);
  sc_signal<int> input_1;
  sc_signal<int> input_2;
  sc_signal<int> output;
  topadd2 t1 ("t1");

  t1.clk(CLOCK);
  t1.in_port_1(input_1);
  t1.in_port_2(input_2);
  t1.out_port(output);

  sc_start(10, SC_NS);

  return 0;	
}

