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

  void for_if_stmt_wait0() {
    k = 0;
    while (true) {
      k = 1;
      wait();

      for (int i = 0; i < 2; i++) {
        k = 2;
        if ( k == i%2 ) {
          wait();
        } else {
          wait(2);
        }
      }
      k = 3;
    }
  }
  SC_CTOR(test) {
    int x{2};
    SC_THREAD(for_if_stmt_wait0);
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
