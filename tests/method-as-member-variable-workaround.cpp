#include "systemc.h"
#include <iostream>

SC_MODULE( MEMBERVAR ) {
  sc_in<int> dummy;

  void print() {
    std::cout << "This is an SC object as member variable" << std::endl;
  }

  void var() {
    std::cout << "I'm var?" << std::endl;
  }

  SC_CTOR( MEMBERVAR ) {
  SC_METHOD(var);
  }
};

SC_MODULE( test ){
    sc_in<int> in;
    sc_out<int> out;
    sc_signal<int> test_signal;
    MEMBERVAR* mv;

    void entry_function_1() {
      std::cout << "ef1: " << std::endl;
      mv->print();
    }

    SC_CTOR( test ): mv{nullptr} {
        SC_METHOD(entry_function_1);
    }
};

int sc_main(int argc, char *argv[]) {
    sc_signal<int> sig1;
    test test_instance("testing");
    test test_instance2("testing2");
    MEMBERVAR mv_instance("mv_testing");
    mv_instance.dummy(sig1);

    test_instance.in(sig1);
    test_instance.out(sig1);
    test_instance.mv = &mv_instance;

    sc_start(100, SC_NS);
    return 0;
}
