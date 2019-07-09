#include "systemc.h"
sc_event e1;

SC_MODULE(producer){
	int global;
	sc_out <int*> out;
	void f1() {
   int *i = new int [2];
   i[0] = 0;
		while(true) {
			wait(2, SC_NS);	
   cout<<"\n Writing i : " <<i[0];
			out.write(i);
   i[0]++;
			wait(10, SC_NS);			
			global = 0;
	
		}	
	}
	void f3() {
		while(true) {
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
	sc_in<int*> in;
	void f2() {
		int *x;
		while(true) {

			wait(5, SC_NS);
			x = in.read();
			wait(2, SC_NS);
   cout<<"\n Value of x : " <<x[0];
   x[0] = x[0] * 5;
		}
	}
	SC_CTOR(consumer) {
		SC_THREAD(f2);
	}
};

int sc_main(int argc, char *argv[]) {
	
	sc_signal<int*> s1;
	
	producer p1("producer");
	consumer c1("consumer");

	p1.out(s1);
	c1.in(s1);
 sc_start();
	return 0;
}
