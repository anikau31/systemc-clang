#include <systemc.h>

SC_MODULE(producer) {

  sc_in <bool> clock;
  sc_in <int*> in1;
  sc_out <bool> ready;
  sc_out <int*> out;

  void p(){
 
    int x, y;
    int i, j;

    int *a1;
    int *a2;
    int *sum_array;
    int final_sum = 0;
    j  = 10;
    while (true) {

      for (i = 0; i< 10; i++) {
	a1[i] = j * 2;
      }
      if (j  > 10) {
	wait (1, SC_NS);
	for (i = 0; i<10; i++) {
	  a2[i] = i*8;
	}
      }
      else {
	for (i = 0; i<10; i++) {
	  a2[i] = i*2;
	}

      }
      j = 20;
      wait(1, SC_NS);
      if (final_sum > 50) {
	j--;
	wait (1, SC_NS);
	continue;

      }

      ready.write(true);
      wait(1, SC_NS);

    }
  }
  SC_CTOR(producer) {
    SC_THREAD(p);
    sensitive << clock.pos();
  }
};

int sc_main(int argc, char *argv[]){
	
  sc_clock CLOCK("clock", 5);
  sc_signal<int*> in;
  sc_signal<bool> ready;
  sc_signal<int*> out;

  producer p1 ("p1");
  p1.clock(CLOCK);
  p1.in1(in);
  p1.ready(ready);
  p1.out(out);	
}
