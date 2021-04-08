#include "systemc.h"

class NestedModule : public sc_module {
 public:
  sc_in_clk nested_clk;

  SC_CTOR(NestedModule) {}
};

class Base : public sc_module {
 public:
  sc_in_clk clk;
  sc_in<int> in1;

  NestedModule nested_module;

  void proc() { }

  SC_CTOR(Base) : nested_module("NestedModule") {
    SC_METHOD(proc);
    sensitive << clk.pos();

    nested_module.nested_clk(clk);

  }

};

class test : public Base {
 public:
  sc_in<int> in2;
  sc_out<int> out1;
  sc_signal<int> internal_signal;

  void entry_function_1() {
    while (true) {
    }
  }

  SC_HAS_PROCESS(test);

  test(const sc_module_name &name) : Base(name) {
    SC_METHOD(entry_function_1);
    sensitive << clk.pos();
  }
};

SC_MODULE(DUT) {
  sc_signal<int> sig1;

  test test_instance;

  int others;
  SC_CTOR(DUT) : test_instance("testing") {
    test_instance.in1(sig1);
    test_instance.in2(sig1);
    test_instance.out1(sig1);
  }
};

int sc_main(int argc, char *argv[]) {
  DUT d("d");
  return 0;
}
