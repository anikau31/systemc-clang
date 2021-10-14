#include "systemc.h"

SC_MODULE(test) {
  // input ports
  sc_in_clk clk;
  sc_in<int> in1;
  sc_in<bool> reset;
  // output ports
  sc_out<int> out1;

  int k;

  void single_wait() {
    while (true) {
      k = 1;
      wait();
    }
  }

  void reset_wait() {
    k = 1;
    wait();
    k = 2;
    while (true) {
      k = 3;
      wait();
      k = 4;
    }
  }

  void reset_conditional_wait() {
    /// This should not be allowed actually.  I wonder what the tool will do.
    if (k == 0) {
      k = 1;
      wait();
    } else {
      k = 2;
      wait();
    }
    k = 3;

    while (true) {
      k = 3;
      wait();
      k = 4;
    }
  }

  SC_CTOR(test) {
    SC_THREAD(single_wait);  //{
    sensitive << clk.pos();
    async_reset_signal_is(reset, false);  // active low reset async

    SC_THREAD(reset_wait);
    sensitive << clk.pos();
    async_reset_signal_is(reset, true);  // active low reset async

    SC_THREAD(reset_conditional_wait);
    sensitive << clk.pos();
    async_reset_signal_is(reset, false);  // active low reset async

  }
};

SC_MODULE(DUT) {
  sc_signal<int> sig1;
  sc_signal<sc_int<64>> double_sig;

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
