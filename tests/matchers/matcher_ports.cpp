#include "systemc.h"

SC_MODULE( test ){

  sc_in_clk clock;
  sc_in<int> in1;
  sc_in<int> in2;
  sc_inout<double> in_out;
  sc_out<int> out1;
  sc_out<int> out2;
  sc_signal<int> internal_signal;

  int x;

  void entry_function_1() {
    while(true) {
    }
  }
  SC_CTOR( test ) {
    SC_METHOD(entry_function_1);
    sensitive << clk.pos();
  }
};

int sc_main(int argc, char *argv[]) {
  sc_signal<int> sig1;
  sc_signal<double> double_sig;
  test test_instance("testing");
  test_instance.in1(sig1);
  test_instance.in_out(double_sig);
  test_instance.out1(sig1);

  return 0;
}
