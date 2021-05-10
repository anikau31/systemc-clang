#include "systemc.h"

class NestedModule : public sc_module {
 public:
  sc_in_clk nested_clk;

  SC_CTOR(NestedModule) {}
};

class A : public sc_module {
 public:
  sc_in_clk clk;
  sc_in<int> in1;

  NestedModule nested_module;

  void proc() { }

  SC_CTOR(A) : nested_module("NestedModule") {
    SC_METHOD(proc);
    sensitive << clk.pos();

    nested_module.nested_clk(clk);

  }

};

class B: public A {
 public:
  sc_in_clk b_clk;
  sc_out<double> b_out;

  void b_function() {
    while (true) {
    }
  }

  SC_HAS_PROCESS(B);

  B(const sc_module_name &name) : A(name) {
    SC_METHOD(b_function);
    sensitive << b_clk.pos();
  }
};


class C: public B {
 public:
  sc_in_clk c_clk;
  sc_in<double> c_out;

  void c_function() {
    while (true) {
    }
  }

  SC_HAS_PROCESS(C);

  C(const sc_module_name &name) : B(name) {
    SC_METHOD(c_function);
    sensitive << c_clk.pos();
  }
};


class test : public C {
 public:
  sc_in<int> in2;
  sc_out<int> out1;
  sc_signal<int> internal_signal;

  void entry_function_1() {
    while (true) {
    }
  }

  SC_HAS_PROCESS(test);

  test(const sc_module_name &name) : C(name) {
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
