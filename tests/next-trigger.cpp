#include "systemc.h"
#include <iostream>

SC_MODULE( test ){
    sc_in<int> in;
    sc_out<int> out;
    sc_signal<int> test_signal;

    void hello() {
      std::cout << sc_time_stamp() << ": Hello" << std::endl;
      next_trigger(sc_time(50, SC_NS));
      std::cout << sc_time_stamp() << ": World" << std::endl;
      next_trigger(sc_time(1000, SC_NS));
      std::cout << sc_time_stamp() << ": !!" << std::endl;
    }

    SC_CTOR( test ) {
        SC_METHOD( hello );
    }
};

int sc_main(int argc, char *argv[]) {
    std::cout << "start of test" << std::endl;
    sc_signal<int> sig1;
    test test_instance("testing");
    test_instance.in(sig1);
    test_instance.out(sig1);

    sc_start();

    std::cout << "End of test" << std::endl;
    return 0;
}
