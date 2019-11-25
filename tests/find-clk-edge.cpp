#include "systemc.h"
SC_MODULE( test ){
    sc_in_clk clk;
    void entry_function_1() {
        while(true) {
            // do nothing
        }
    }

    SC_CTOR( test ) {
        SC_METHOD(entry_function_1);
        sensitive << clk.pos();
    }
};

int sc_main(int argc, char *argv[]) {
    sc_clock clk;
    test test_instance("testing");
    test_instance.clk(clk);

    return 0;
}
