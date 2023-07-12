#include <systemc.h>

// Array general cases and some special cases
class top : sc_module {
 public:
  sc_in<bool> clk;
  sc_signal<bool> arstn;

  sc_signal<bool> a{"a"};
  sc_signal<bool> ps_repl;

  static const unsigned DOMAIN_NUM = 1;

  SC_HAS_PROCESS(top);
  top(sc_module_name) {
    SC_CTHREAD(bug_in_array_index2, clk.pos());
    async_reset_signal_is(arstn, false);
  }

  // BUG in real design -- in ConfigCtrl.h extra copy of @reqSource in Verilog
  // Shorten version
  
  sc_signal<sc_uint<2>> fifo_source;
  sc_signal<sc_uint<2>> master_resp_data[3];
  static const unsigned ECC_NUM = 0;

  // BUG in real design -- in ConfigCtrl.h extra copy of @reqSource in Verilog
  // Full version
  void bug_in_array_index2() {
    wait();

    while (1) {
      int x, y, z;
      z = (x > y) ? ( (y > z) ? x=x+1 : y=y+1) : ((z < 10)? z = z -1: z +4);
      // z = (x > y) ? ( y + 1) : ((z < 10)? z = z -1: z +4);
    }
  }
};

int sc_main(int argc, char *argv[]) {
  top top_inst{"top_inst"};
  sc_start(100, SC_NS);
  return 0;
}
