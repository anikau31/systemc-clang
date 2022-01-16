#include "systemc.h"

SC_MODULE(test) {
  // input ports
  sc_in_clk clk;
  sc_in<int> in1;
  sc_in<bool> reset;
  // output ports
  sc_out<int> out1;

  int k;

  void simple_for_wait() {

    while (true) {                    // 7
        k = 1;                        // 6
        for (int i{0}; i<10; i++) {   // 5
          k = 2;                      // 4
          //wait(4);
          wait();                     // 41
          k = 3;                      //42
        }
        k = 4;                        // 2
                     // 3 ; ++1
    //    wait();
     //   k = 5;

    }
  }

  SC_CTOR(test) {
    SC_THREAD(simple_for_wait); //{
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
