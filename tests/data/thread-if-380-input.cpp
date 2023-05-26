#include "systemc.h"

  static const bool   CONST_A = 1;
SC_MODULE(test) {
  sc_in_clk clk;
  sc_in<int> in1;
  sc_out<int> out1;

  bool b;
  
  void smem_if_binary_const() {
    bool b = 0;
    wait();

    while (true) {          // B5
        if (CONST_A) {      // B4
            b = !b;         // B3
        }

        wait();             // B2
    }                       // B1
}

  SC_CTOR(test) {
    int x{2};
    SC_THREAD(smem_if_binary_const);
    sensitive << clk.pos();
  }
};

SC_MODULE(DUT) {
  sc_signal<int> sig1;
  sc_signal<double> double_sig;

  test test_instance;

  int others;
  SC_CTOR(DUT) : test_instance("testing") {
    test_instance.in1(sig1);
    test_instance.out1(sig1);
  }
};

int sc_main(int argc, char *argv[]) {
  //DUT d("top");
  test test_instance("top");
  return 0;
}
