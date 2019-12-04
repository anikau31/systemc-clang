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

int sc_main(int argc, char *argv[]) {
    sc_clock clk;
    sc_signal<int> sig1;
    test test_instance("testing");
    test_instance.clk(clk);
    test_instance.in(sig1);
    test_instance.out(sig1);

    return 0;
}
