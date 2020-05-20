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
typedef fpn_t enc_t;
typedef typename fpn_t::expo_t expo_t;

SC_MODULE(mymodule)
{
  sc_clock clk; 
  sc_signal<bool> reset;
  sc_stream<fpn_t> c_tb_send_fp;
  sc_stream<enc_t> c_dut_enc;
  sc_stream<expo_t> c_dut_expo;
  find_emax<fpn_t, DIMS> u_dut;
  SC_CTOR(mymodule) : u_dut("u_dut")
  {
    // connect DUT
    u_dut.clk(clk);
    u_dut.reset(reset);
    u_dut.s_fp(c_tb_send_fp);
    u_dut.m_fp(c_dut_enc);
    u_dut.m_ex(c_dut_expo);
  }
};

#if DIMS > 2
#error "block array only supports 1 or 2 dimensions"
#endif


int sc_main(int argc , char *argv[])
{

  mymodule mymod("mymod");

}
