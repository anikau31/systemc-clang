#include "systemc.h"

/*
SC_MODULE( test ){

  sc_in_clk clk;
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
*/

SC_MODULE( simple_module){

  sc_in_clk clk;
  sc_in<int> one;
  sc_out<int> out_one;
  int yx;

  void entry_function_1() {
    int x_var;
    double y_var;
    sc_int<4> z_var;
    while(true) {
    }
  }

  SC_CTOR( simple_module ) {
    SC_METHOD(entry_function_1);
    sensitive << clk.pos();
  }
};


int sc_main(int argc, char *argv[]) {
  sc_signal<int> sig1;
  sc_signal<double> double_sig;
  /*
  test test_instance("testing");
  test_instance.in1(sig1);
  test_instance.in_out(double_sig);
  test_instance.out1(sig1);
  */

  simple_module simple("simple_second_module");
  return 0;
}
