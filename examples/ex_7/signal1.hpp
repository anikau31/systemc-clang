#include <systemc.h>

SC_MODULE(top) {
  sc_in_clk clk;
  sc_in<int> int_port;

  sc_signal<int> SIGNAL1;
  sc_signal<sc_int<16> > SIGNAL2;
  
  SC_CTOR(top) {
    SC_THREAD(topEntry);
  }

  void topEntry() {
    while(true) {
      SIGNAL1.write(10);
      SIGNAL2.write(5);
      x = SIGNAL1.read();
      x = SIGNAL2.read();
      wait(1, SC_NS);
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

