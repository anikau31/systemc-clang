#include <systemc.h>

SC_MODULE(top) {
  sc_in_clk clk;
  SC_CTOR(top) {
    SC_THREAD(topEntry);
    SC_THREAD(secondEntry);
  }

  void secondEntry() {
    while(true) {
      wait(2, SC_NS);
      wait();
    }
  }

  void topEntry() {
    while(true) {
      wait();
      var++;
      wait();
      --var;
      y = y + 1;
      wait();
      var = var *2;
    }
  }

 public:
  int var;
  int x;
  int y;
};

