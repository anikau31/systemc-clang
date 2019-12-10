#include "systemc.h"

SC_MODULE( non_template ) {
  SC_CTOR(non_template) {
  };
};

template <typename S, typename T>
SC_MODULE( test ){
    sc_in_clk clk;
    sc_in<S> inS;
    sc_out<S> outS;
    sc_signal<S> test_signal;

    sc_in<T> inT;
    sc_in<T> outT;
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
    sc_signal<int> sig1;
    test<int,double> test_instance("testing");
    test_instance.clk(clk);
    test_instance.inS(sig1);
    test_instance.outS(sig1);

    return 0;
}
