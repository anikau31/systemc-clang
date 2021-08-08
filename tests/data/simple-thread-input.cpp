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

  void simple_wait() {
    while (true) {
      if (x == 1) {
        k = 1;
        wait();

        if (x == 3) {
          k = 2;
          wait();
        } else {
          k = 3;
        }
      } else {
        if (x == 5) {
          k = 4;
          wait();
        }
      }
      wait();
      k = 5;
    }
  }

  void func_call() {
    int k{0};
    k = k + 1;
  }

  void test_thread() {
    while (true) {
      func_call();
      x = x + 1;
      x = x + 2;
      wait();
      out1.write(x);
      x = x + 3;
      wait(4);
      wait(4, SC_NS);
      x = x + 6;
    }
  }

  SC_CTOR(test) {
    int x{2};
    SC_THREAD(test_thread);
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
