#include "systemc.h"
SC_MODULE(test) {
  sc_in_clk clk1;
  sc_in_clk clk2;
  void entry_function_1() {
    while (true) {
      // do nothing
    }
  }

  SC_CTOR(test) {
    SC_METHOD(entry_function_1);
    sensitive << clk1.neg();
    sensitive << clk2.pos();
  }
};

int sc_main(int argc, char *argv[]) {
  sc_clock clk;
  test test_instance("testing");
  test_instance.clk1(clk);
  test_instance.clk2(clk);

  return 0;
}
