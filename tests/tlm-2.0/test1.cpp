#include "systemc.h"


SC_MODULE(producer){
	int global;
	sc_out <int> out;
	void f1() {
		int i, j;
		while(true) {
			wait(2, SC_NS);
			cout<<"f1 "<<sc_time_stamp()<<endl;
			for (int x = 0; x<5; x++) {
				i = i + x*8;
			}
			out.write(i);
			wait(5, SC_NS);
			cout<<"f1 "<<sc_time_stamp()<<endl;
			i++;	
			wait(10, SC_NS);			
			cout<<"f1 "<<sc_time_stamp()<<endl;
			global = 0;
	
		}	
	}
	void f3() {
		while(true) {
			global = 4;
			wait(15, SC_NS);
			cout<<"f3 "<<sc_time_stamp()<<endl;
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
		int y;
		while(true) {
			for (int i = 0; i<4; i++ ) {
				x = i*7;
			}
			wait(5, SC_NS);
			cout<<"f2 "<<sc_time_stamp()<<endl;
			if(x>5) {
				wait(10, SC_NS);
				cout<<"f2 "<<sc_time_stamp()<<endl;
			}
			x = in.read();
			wait(2, SC_NS);
			cout<<"f2 "<<sc_time_stamp()<<endl;
		}
	}
	SC_CTOR(consumer) {
		SC_THREAD(f2);
	}
};

int sc_main(int argc, char *argv[]) {
	
	sc_signal<int> s1;
	
	producer p1("producer");
	consumer c1("consumer");

	p1.out(s1);
	c1.in(s1);
	
	sc_start(100, SC_NS);
	return 0;
}
