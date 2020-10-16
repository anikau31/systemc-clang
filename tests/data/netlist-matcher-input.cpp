#include "systemc.h"

SC_MODULE(submodule) {
  sc_in<int> input;
  sc_out<int> output;

  void entry() {}
  SC_CTOR(submodule) {
    SC_METHOD(entry) ;

  }
};

SC_MODULE(test) {
  sc_in_clk clk;

  sc_in<int> inS;
  sc_out<int> outS;
  sc_in<double> inT;
  sc_out<double> outT;

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

SC_MODULE(DUT) {
  sc_clock clk;
  sc_signal<int> sig1;

  test test_instance;
  SC_CTOR(DUT) : test_instance("testing") {
    test_instance.clk(clk);
    test_instance.inS(sig1);
    test_instance.outS(sig1);
  }
};

int sc_main(int argc, char *argv[]) {
  DUT d("dut");
  return 0;
}
