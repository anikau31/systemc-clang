
/**
 * Streaming hash unit
 */

#define __SYNTHESIS__

#if defined(__SYNTHESIS__)
#define RVD
#endif

#if defined(RVD)
#include "../../zfpsynth/shared2/sc_rvd.h"
#include "../../zfpsynth/shared2/rvfifo_cc.h"
template <typename T> using sc_stream = sc_rvd<T>;
template <typename T> using sc_stream_in = sc_rvd_in<T>;
template <typename T> using sc_stream_out = sc_rvd_out<T>;
template <typename T, int IW,  bool RLEV> using sfifo_cc = rvfifo_cc<T, IW, RLEV>;

#else
#include "../../zfpsynth/shared2/sc_stream_ports.h"
#include "../../zfpsynth/shared2/sc_stream.h"
#include "../../zfpsynth/shared2/sfifo_cc.h"
#endif

#if !defined(RLEVEL)
#define RLEVEL false
#endif

#include "systemc.h"

#include "shash.h"

sc_trace_file *tf;

#define SEED 24409

// The module wrapper for moving_average
SC_MODULE(mymodule) 
{

  sc_clock clk; 
  sc_signal<bool> reset;

  sc_stream<sc_biguint<KEYLW+KEYW> > hdata;
  sc_stream<sc_biguint<TAPW> > htaptoshash;
  sc_stream<sc_biguint<TAPW> > htapfromshash;

  short_hash u_dut; // hash unit dut

  SC_CTOR(mymodule) : u_dut("u_dut")
  {
    // connect hsu
    u_dut.clk(clk);
    u_dut.reset(reset);
    u_dut.s_dat(hdata);
    u_dut.s_tap(htaptoshash);
    u_dut.m_tap(htapfromshash);

  }
};

int sc_main(int argc , char *argv[])
{
  mymodule mymod("mymod");
}
