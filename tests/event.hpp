#include "systemc.h"

sc_event e1, e2, e3;
SC_MODULE(producer) {
	sc_out<int> out;
	SC_CTOR(producer) {
		SC_THREAD(f1);
		a = new int [5];
	}
	void f1() {
		while (true) {
			sc_core::wait(SC_ZERO_TIME);
			e1.notify();
			for (int i = 0; i<5; i++) {
				a[i] = i*2;
				out.write(a[i]);
				sc_core::wait(1, SC_NS);
			}
			sc_core::wait(e2);
			sc_stop();
		}
	}
	~producer() {
		delete [] a;
	}
	private:
		int *a;
};

SC_MODULE(consumer) {

	SC_CTOR(consumer) {
		SC_THREAD(f2);
	}
	void f2() {
		int x, y;
		sc_time delay = sc_time(10, SC_NS);
		sc_time t1(20, SC_NS);
		while(true) {
			sc_core::wait(e1);
			x++;
			y--;
			sc_core::wait(10, SC_NS);
			e2.notify();
		}
	}
};

SC_MODULE(other) {
	sc_in<int> in;
	SC_CTOR(other) {
		SC_THREAD(f3);
	}
	void f3() {
		int x;
		while(true) {
			for (int i = 0; i< 5; i++) {
				sc_core::wait(1, SC_NS);
				x = in.read();
			}	
		}
	}
};

int sc_main(int argc, char *argv[]) {
	
	sc_signal<int> sig1;
	
	producer p1("p1");
	p1.out(sig1);

	consumer c1 ("c1");

	other o1 ("o1");
	o1.in(sig1);

	return 0;
}
