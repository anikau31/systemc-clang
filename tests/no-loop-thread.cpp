#include "systemc.h"
#include <iostream>

SC_MODULE( test ){
    sc_in<int> in;
    sc_out<int> out;
    sc_signal<int> test_signal;

    void hello() {
      std::cout << "write 1" << std::endl;
      out.write(1);
      wait();
      /*
      std::cout << "write 2" << std::endl;
      out.write(2);
      wait(sc_time(100, SC_NS));
      */
    }

    SC_CTOR( test ) {
        SC_THREAD(hello);
    }
};

int sc_main(int argc, char *argv[]) {
    sc_signal<int> sig1;
    test test_instance("testing");
    test_instance.in(sig1);
    test_instance.out(sig1);

    sc_start();

    return 0;
}
