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

    while (1) {
        k = 1;
        wait(4);
        k = 2;
    }
  }

  SC_CTOR(test) {
    SC_THREAD(single_wait); //{
     sensitive << clk.pos();
     async_reset_signal_is(reset, false); // active low reset async
     
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
