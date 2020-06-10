#include <systemc.h>

sc_event globalVar;

SC_MODULE(top) {
  sc_in_clk clk;
  sc_event inclass;

  SC_CTOR(top) {
    SC_THREAD(topEntry);
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
      globalVar.notify();
      inclass.notify();
    }

  }

 public:
  int var;
  int x;
  int y;
};

