#include "systemc.h"
SC_MODULE( test ){
   sc_in<int> in;
	sc_out<int> out;
	void entry_function_1() {
		while(true) {
      // do nothing
		}
	}

	void entry_function_2() {
		while(true) {
      // do nothing
		}
	}

	SC_CTOR( test ) {
		SC_THREAD(entry_function_1);
		SC_THREAD(entry_function_2);
	}
};

int sc_main(int argc, char *argv[]) {

	sc_signal<int> sig1;
    test test_instance("testing");
    test_instance.in(sig1);
    test_instance.out(sig1);

	return 0;
}
