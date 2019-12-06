/* Placeholder for further modules */
// TODO: include example directory
#include "sreg.h"

#include <systemc.h>

SC_MODULE(test) {
public:
  sc_in<bool> clk;
  sc_stream_out<int> m_port;

  void mc_test() {
  }

  SC_CTOR(test) {
    SC_METHOD(mc);
  }
};

int sc_main(int argc, char *argv[]) {
  sc_signal<bool> clk;
  sc_signal<bool> reset;
  sc_stream<int> in_s;
  test test_instance("testing");
  test_instance.clk(clk);
  test_instance.m_port(in_s);

  sreg<int, BYPASS> sreg_bypass("sreg_bypass");
  sreg<int, FWD>    sreg_fwd("sreg_fwd");
  sreg<int, FWD_REV> sreg_fwd_rev("sreg_fwd_rev");

  sreg_bypass.clk(clk);
  sreg_bypass.reset(reset);
  sreg_bypass.s_port(in_s);

  sreg_fwd.clk(clk);
  sreg_fwd.reset(reset);
  sreg_fwd.s_port(in_s);

  sreg_fwd_rev.clk(clk);
  sreg_fwd_rev.reset(reset);
  sreg_fwd_rev.s_port(in_s);

  return 0;
}
