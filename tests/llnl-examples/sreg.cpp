/* Placeholder for further modules */
// TODO: include example directory
#include "../systemc-clang/examples/llnl-examples/sreg.h"

#include <systemc.h>

SC_MODULE(test) {
  sc_in_clk clk;
  

  void mc() {
  }

  SC_CTOR(test) {
    SC_METHOD(mc);
  }
};

int sc_main(int argc, char *argv[]) {
  sc_clock clk;
  test test_instance("testing");
  test_instance.clk(clk);

  sreg<int, BYPASS> sreg_bypass("sreg_bypass");
  sreg<int, FWD>    sreg_fwd("sreg_fwd");
  sreg<int, FWD_REV> sreg_fwd_rev("sreg_fwd_rev");

  sreg_bypass.clk(clk);
  sreg_fwd.clk(clk);
  sreg_fwd_rev.clk(clk);

  return 0;
}
