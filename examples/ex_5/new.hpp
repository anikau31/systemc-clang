#include "systemc.h"

sc_event ev;
sc_event oev;

SC_MODULE(top) {

  sc_in_clk clk;
  sc_in<int> in_port;
  sc_out<int> out_port;
  sc_event aev;	
  void f(int r, int q){
    r = y*2;
  }

  SC_CTOR(top) {
    SC_THREAD(topEntry);
  }

  void topEntry() {
    while(true) {
      x = x*2;

      if (x>5){
				
	wait(1, SC_NS);
					
      }
      v++;
      wait(1, SC_NS);
      w++;
      y = y + 1;
      for (int i = 0; i<10; i++) {
	x++;
	if (x>3){
	  x = y-2;
	  wait(1, SC_NS);
	}
      }
      x = x *2;			
      oev.notify(1, SC_NS);
      wait(ev);
      y = x-2;
      f(x, y);
      out_port.write(y);
      wait(4, SC_NS);
    }	
  }

 public:
  int v;
  int x;
  int y;
  int w;
};

int sc_main(int argc, char *argv[]){
	
  sc_clock CLOCK("clock", 5);
  sc_signal<int> in;
  sc_signal<int> out;
  sc_signal<int> f;

  top t1 ("t1");
  t1.clk(CLOCK);
  t1.in_port(in);
  t1.out_port(f);

  top t2 ("t2");
  t2.clk(CLOCK);
  t2.in_port(f);
  t2.out_port(out);

  sc_start(10, SC_NS);

  return 0;	
}

