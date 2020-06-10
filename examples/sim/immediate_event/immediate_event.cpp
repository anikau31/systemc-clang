#include "systemc.h"
#include <iostream>
using namespace std;

sc_event pe, ce;

SC_MODULE(producer) {
  sc_in_clk clk;
SC_CTOR(producer) {
  SC_THREAD(prodEntry) {
    sensitive << clk.pos();
  }
}
  void prodEntry() {
    while(true) {
      ce.notify();
      cout << sc_time_stamp() << ": producer\n";
      wait();
    }
  }
};


SC_MODULE(consumer) {
  sc_in_clk clk;
SC_CTOR(consumer) {
  SC_THREAD(consumerEntry) {
    //sensitive << clk.pos();
  }
}
  void consumerEntry() {
    while(true) {
      wait(ce);
      cout << sc_time_stamp() << ": consumer\n";
    }
  }
};




int sc_main(int argc, char *argv[]) {
  sc_clock clk;
  consumer c("consumer");
  producer p("producer");

  p.clk(clk);
  c.clk(clk);

  sc_start(10, SC_NS);

  return 0;

};
