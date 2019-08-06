#include "systemc.h"
#include <iostream>

SC_MODULE( member_variable ) {
  void print() {
    std::cout << "This is an SC object as member variable" << std::endl;
  }

  void var() {
    std::cout << "I'm var?" << std::endl;
  }

  SC_CTOR( member_variable) {
  SC_METHOD(var);
  }
};

SC_MODULE( test ){
    sc_in<int> in;
    sc_out<int> out;
    sc_signal<int> test_signal;
    member_variable mv;

    void entry_function_1() {
      std::cout << "ef1: " << std::endl;
 //     mv.print();
    }

    SC_CTOR( test ): mv("mv") {
        SC_METHOD(entry_function_1);
    }
};

int sc_main(int argc, char *argv[]) {
    sc_signal<int> sig1;
    test test_instance("testing");
    test_instance.in(sig1);
    test_instance.out(sig1);

    sc_start(100, SC_NS);
    return 0;
}
