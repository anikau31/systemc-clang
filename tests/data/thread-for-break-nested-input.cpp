#include "systemc.h"

SC_MODULE(test) {
  // input ports
  sc_in_clk clk;
  sc_in<int> in1;
  // output ports
  sc_out<int> out1;

  // others
  int x;
  int k;

 void break0() {
    k = 0;
    while (true) {
      k = 1;
      for (int i = 0; i < 2; i++) {
        if (i == 1) {
          k = 2;
          for (int j = 0; j < 3; j++) {
            if (j == 1) {
              k = 4;
              break;
            } else {
              k = 6;
            }
          }
        //  break;
        }
        else k = 5;
          k++;
      }
      k = 3;
      wait();
      k = 4;
    }
  }

  SC_CTOR(test) {
    int x{2};
    SC_THREAD(break0);
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
  DUT d("d");
  return 0;
}
