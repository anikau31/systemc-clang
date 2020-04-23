#include "systemc.h"

SC_MODULE(submodule) {
  sc_in<int> input;
  sc_out<int> output;

  void entry() {}
  SC_CTOR(submodule) {
    SC_METHOD(entry) ;

  }
};

template <typename S, typename T>
SC_MODULE(test) {
  sc_in_clk clk;

  sc_in<S> inS;
  sc_out<S> outS;
  sc_in<T> inT;
  sc_out<T> outT;

  submodule sub_module_member;

  void entry() {
    while (true) {}
  }

  SC_CTOR(test) : sub_module_member("SUBMODULE") {
    sub_module_member.input(inS);
    sub_module_member.output(outS);

    SC_METHOD(entry);
    sensitive << clk.pos();
  }
};

int sc_main(int argc, char *argv[]) {
  sc_clock clk;
  sc_signal<int> sig1;

  test<int, double> test_instance("testing");
  test_instance.clk(clk);
  test_instance.inS(sig1);
  test_instance.outS(sig1);

  return 0;
}
