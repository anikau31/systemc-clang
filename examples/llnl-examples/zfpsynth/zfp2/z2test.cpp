#define __SYNTHESIS__

#if defined(__SYNTHESIS__)
#define RVD
#endif

#if !defined(RLEVEL)
#define RLEVEL false
#endif

#include "../shared/pulse.h"
#include "../shared/zfp.h"

#define EXPOW 11
#define FRACW 52
// #define EXPOW  8
// #define FRACW 23
#ifndef DIMS
#define DIMS 1
#endif
#define BLOCK_CNT 2
#define BLOCK_LEN fpblk_sz(DIMS) /* values per block */

typedef fp_t<EXPOW,FRACW> fpn_t;
typedef sc_bv<fpn_t::bits*BLOCK_LEN> enc_t;

#if DIMS > 2
#error "block array only supports 1 or 2 dimensions"
#endif

SC_MODULE(mymodule)
{
  sc_clock clk; 
  sc_signal<bool> reset;

  sc_stream<fpn_t> c_driver_fp;
  sc_stream<enc_t> c_dut_enc;
  zfp_encode<fpn_t, DIMS, enc_t> u_dut;

  SC_CTOR(mymodule) : u_dut("u_dut")
  {
    // connect DUT
    u_dut.clk(clk);
    u_dut.reset(reset);
    u_dut.s_fp(c_driver_fp);
    u_dut.m_enc(c_dut_enc);
  }
};

int sc_main(int argc , char *argv[])
{

  mymodule mymod("mymod");

}
