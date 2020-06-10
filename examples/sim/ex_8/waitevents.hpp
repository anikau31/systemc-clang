#include <systemc.h>

sc_event globalVar;
extern sc_event extglobalEvent;
bool globalBool;

SC_MODULE(waitevents) {
  sc_in_clk clk;
  sc_event inclass;

  SC_CTOR(waitevents) {
    SC_THREAD(topEntry);
  }

  void topEntry() {
    int test;
    int b;
    test = test + 1;
    b = test;
    globalBool = false;
    while(true) {
      wait(1, SC_NS);
      wait();
      var++;
      wait(inclass);
      --var;
      y = y + 1;
      wait(globalVar);
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

