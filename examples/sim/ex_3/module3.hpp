#include "systemc.h"
sc_event zen;
SC_MODULE(top) {
  sc_in_clk clk;
  sc_in<int> int_port;
  void f(int r, int q){
    r = y*2;
  }

  SC_CTOR(top) {
    SC_THREAD(topEntry);
  }

  void topEntry() {
    while(true) {
      if (x >5) {
	x++;
      }
      else {
	wait(1, SC_NS);
      }
      var++;
      wait();
      --var;
      y = y + 1;
      wait();
      var = var *2;
      f(var, y);
    }	
  }
 public:
  int var;
  int x;
  int y;
};

int sc_main(int argc, char *argv[]){
	
  sc_clock CLOCK("clock", 5);
  sc_signal<int> input;
  sc_signal<int> bogus;
  top t1 ("t1");
  t1.clk(CLOCK);
  t1.int_port(input);

  sc_start(10, SC_NS);

  return 0;	
}

