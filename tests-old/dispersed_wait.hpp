#include <systemc.h>
using namespace std;
sc_event e1, e2, e3;
SC_MODULE(producer) {
	sc_out<int> out;
	SC_CTOR(producer) {
		SC_THREAD(f1);
		x = new int[512];
	}
	void f1() {
		while(true) {
			wait(SC_ZERO_TIME);
			for (int i = 0; i<512; i++) {
				x[i] = i;
			}
			wait(1, SC_NS);
			e1.notify();
			out.write(x[9]);
			sc_stop();
		}
	}
	~producer() {
		delete [] x;
	}
	private:
		int *x;
};

SC_MODULE(consumer) {
	sc_in<int> in;
	SC_CTOR(consumer) {
		SC_THREAD(f2);	
	}
	void f2() {
		int a[400];
		while(true){
			for (int i = 0; i<400; i++) {
				a[i] = i*2;
				wait(1, SC_NS);
			}
			wait(e2);
			a[256] = in.read();
			
			wait(1, SC_NS);
		}
	}
};

SC_MODULE(other) {
	SC_CTOR(other) {
		SC_THREAD(f3);
	}
	void f3() {
		while (true) {
			wait(e1);
			wait(20, SC_NS);
			for (int i = 0; i< 450; i++) {
			}
			e2.notify();
		}	
	}
};

int sc_main(int argc, char *argv[]) {
	sc_signal<int> sig1;
	producer p1("p1");
	p1.out(sig1);

	consumer c1("c1");
	c1.in(sig1);

	other o1 ("o1");

	sc_start();
	return 0;
}
