#include "systemc.h"
SC_MODULE( test ){
    sc_in_clk clk;
    sc_in<int> in;
    sc_out<int> out;
    sc_signal<int> test_signal;
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
        SC_METHOD(entry_function_1);
        sensitive << clk.pos();
        SC_METHOD(entry_function_2);
    }
};

SC_MODULE(DUT) {
    sc_clock clock;
    sc_signal<int> sig1;
    test test_instance;

    SC_CTOR(DUT): test_instance("testing_pb") {
    test_instance.clk(clock);
    test_instance.in(sig1);
    test_instance.out(sig1);

    }


};

int sc_main(int argc, char *argv[]) {
  DUT d("d");
    return 0;
}
