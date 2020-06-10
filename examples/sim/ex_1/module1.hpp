#include "systemc.h"
sc_event zen;
sc_event jjk;
SC_MODULE(top) {

  sc_in_clk clk;
  sc_in<int> in_port;	
  sc_out<int> out_port;

  void g(int h) {
    h++;
  }
  void f(int r, int q){
    for (int j = 0; j<5;j++) {
      j = 0;
    }	
    while (r > 0) {	
      r = y*2;
    }
  }

  SC_CTOR(top) {
    SC_THREAD(topEntry);
    sensitive<<clk.pos();
  }

  void topEntry() {
    while(true) {
      x = x*2;
      if (x>5) {
	if(x > 2) {
	  wait(1, SC_NS);
	}
	y++;
      }	
			
      var++;
      if ( x < 3) {
	var ++;
	x = 2;	
      }
      else {
	var--;
	y = 8;
      }
      wait(1, SC_NS);
      --var;
      g(var);
      y = y + 1;
      for (int i = 0; i<10; i++) {
	i ++;	
	wait(1, SC_NS);
      }
      var = var *2;			
      wait(1, SC_NS);
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
  sc_signal<int> output;
  top t1 ("t1");

  t1.clk(CLOCK);
  t1.in_port(input);
  t1.out_port(output);

  sc_start(10, SC_NS);

  return 0;	
}

