#include "systemc.h"
#include "gpumacro.h"
sc_event e1;

SC_MODULE(producer){
	int global;
	sc_out <int> out;
	void f1() {
		int i, j;
		while(true) {
			wait(2, SC_NS);
			for (int x = 0; x<5; x++) {
				i = i + x*8;
				wait(3, SC_NS);
			}
			out.write(i);
			wait(e1);
			i++;	
			wait(10, SC_NS);			
			global = 0;
	
		}	
	}
	void f3() {
		while(true) {			
			for (int i = 0; i<20; i++ ) {
			PROFILE_TIME(p1, 0, 12, 30);		
			PROFILE_TIME(p2, 1, 42, 90);		
				global = global + i;
			}
			global = 4;
			wait(15, SC_NS);
   e1.notify();
		}
	}
	SC_CTOR(producer) {
		SC_THREAD(f1);
		SC_THREAD(f3);
	}
};

SC_MODULE(consumer) {
	sc_in<int> in;
	void f2() {
		int x;
		while(true) {

			for (int i = 0; i<4; i++ ) {
			PROFILE_TIME(c1, 0, 100, 50);		
			PROFILE_TIME(c2, 1, 200, 10);		
				x = i*7;
			}

			wait(5, SC_NS);
			if(x>5) {
				wait(10, SC_NS);
			}
			x = in.read();
			wait(2, SC_NS);
		}
	}
	SC_CTOR(consumer) {
		SC_THREAD(f2);
	}
};

int sc_main(int argc, char *argv[]) {
	
	sc_signal<int> s1;
	sc_signal<int> s2;	

	producer p1("producer1");
	consumer c1("consumer1");
			
	producer p2("producer2");
	consumer c2("consumer2");

	p1.out(s1);
	c1.in(s1);
	
	p2.out(s2);
	c2.in(s2);
	
	sc_start();
	return 0;
}
