#include "systemc.h"

SC_MODULE( dummy ) {
  void entry() {}
  SC_CTOR( dummy ) {
    SC_METHOD( entry );
  }
};

SC_MODULE( test ){

  sc_in_clk clk;
  sc_in<int> in1;
  sc_in<int> in2;
  sc_inout<double> in_out;
  sc_out<int> out1;
  sc_out<int> out2;
  sc_signal<int> internal_signal;

  static constexpr int MAX_DEPTH = 4;
  sc_out<double> out_array_port[MAX_DEPTH+1];

  dummy dumdum;

  int x;

  void entry_function_1() {
    while(true) {
    }
  }
  SC_CTOR( test ) :dumdum{"dum"} {
    SC_METHOD(entry_function_1);
    sensitive << clk.pos();
    // Sensitivity
    for (int i{0}; i <= MAX_DEPTH; ++i) {
      sensitive << out_array_port[i];
      for (int j{0}; j <= MAX_DEPTH+1; ++j) {
        sensitive << out_array_port[j];
      }
    }

    for (int k{0}; k <= MAX_DEPTH+2; ++k) {
      sensitive << out_array_port[k];
    }

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
