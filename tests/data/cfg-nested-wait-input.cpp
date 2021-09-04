#include "systemc.h"
//#define wait(...) if (true) {wait(__VA_ARGS__);}

SC_MODULE(test) {
  // input ports
  sc_in_clk clk;
  sc_in<int> in1;
  // output ports
  sc_out<int> out1;

  // others
  int x;
  int k;

  void nested_for_with_wait() {
    k = 0;
    wait();

    while (true) {
      for (int i = 0; i < 2; i++) {
        k = 1;
        wait();
        k = 2;
        for (int j = 0; j < 3; j++) {
          k = 3;
          wait();
          k = 4;
          wait();
          k = 5;
        }
        k = 6;
        wait();
      }
      k = 7;
      wait();
    }
  }

  SC_CTOR(test) {
    int x{2};
    SC_THREAD(nested_for_with_wait);
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
