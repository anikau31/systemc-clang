#include "systemc.h"

SC_MODULE( test ){

  sc_in_clk clk;
  sc_in<int> in;
  sc_inout<double> in_out;
  sc_out<int> out;
  sc_signal<int> internal_signal;

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
  test_instance.in(sig1);
  test_instance.in_out(double_sig);
  test_instance.out(sig1);

  return 0;
}
