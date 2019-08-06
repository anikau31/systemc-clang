#include "systemc.h"
#include <iostream>

template<typename FP, int DIM>
SC_MODULE( test ){
    sc_in<int> in;
    sc_out<int> out;
    sc_signal<int> test_signal;
    void entry_function_1() {
      std::cout << "value: " << DIM << std::endl;
    }

    SC_CTOR( test ) {
        SC_METHOD(entry_function_1);
    }
};

int sc_main(int argc, char *argv[]) {
    sc_signal<int> sig1;
    test<int,5> test_instance("testing");
    test_instance.in(sig1);
    test_instance.out(sig1);

    sc_start(100, SC_NS);
    return 0;
}
