/**
 * This module tests for encode_ints module
 */
#define __SYNTHESIS__

#if defined(__SYNTHESIS__)
#define RVD
#endif

#include <cmath> // floor
#include <cstdlib> // exit, atoi, atof
#include <unistd.h> // getopt, optarg, optind
#include <limits> // numeric_limits

#include "../shared2/pulse.h"
#include "../shared2/zhw_encode.h"

#ifndef DATAW
#define DATAW 64   // bitstream data width
#endif

#ifndef PREC
#define PREC 64 // floating-point number bit width
#endif

// #if PREC ==     16 // half
// #define EXPOW    5 // exponent bit width
// #define FRACW   10 // fraction bit width
// #define USE_HALF
#if PREC ==     32 // single
#define EXPOW    8 // exponent bit width
#define FRACW   23 // fraction bit width
#define USE_FLOAT
#elif PREC ==   64 // double
#define EXPOW   11 // exponent bit width
#define FRACW   52 // fraction bit width
#define USE_DOUBLE
#elif PREC ==   80 // ldouble
#define EXPOW   15 // exponent bit width
#define FRACW   64 // fraction bit width
#define USE_LDOUBLE
#elif PREC ==  128 // quad
#define EXPOW   15 // exponent bit width
#define FRACW  112 // fraction bit width
#define USE_QUAD
#else
#error "unknown precision"
#define EXPOW    1 // exponent bit width
#define FRACW    1 // exponent bit width
#endif
#include "real.h"

#ifndef DIMS
#define DIMS 2
#endif

#define BLOCK_LEN zhw::fpblk_sz(DIMS) /* values per block */

typedef fp_t<EXPOW,FRACW> fpn_t;  // floating-point number type
typedef bits_t<DATAW> enc_t;      // encoded bitstream type
typedef zhw::sconfig_t sconfig_t; // signed configuration parameter type
typedef zhw::uconfig_t uconfig_t; // unsigned configuration parameter type

#include "tcase.h" // test cases

#define DEFAULT_BLOCKS 1
#define DEFAULT_RATE (fpn_t::bits)
#define DEFAULT_TCARG "1"
static unsigned blocks = DEFAULT_BLOCKS;
static double rate = DEFAULT_RATE;
static const char *tcarg = DEFAULT_TCARG;
static tcase<real_t,enc_t::uic_t,DIMS> *ptcase;

using namespace zhw;

// The module wrapper for encode_ints
SC_MODULE(mymodule) 
{

	typedef typename FP::ui_t ui_t;
  sc_clock clk; 
  sc_signal<bool> reset;

	/*-------- ports --------*/
	sc_signal <ui_t> c_driver_block[fpblk_sz(DIMS)];
	sc_signal <bool> c_driver_valid;
	sc_signal <bool> c_driver_ready;

	sc_signal <bool> c_driver_flush;

	sc_signal <sc_uint<bc_w(DIMS)> > c_dut_bc;
	sc_signal <sc_bv<bp_w(DIMS)> >   c_dut_bp;
	sc_signal <bool>                c_dut_last;
	sc_signal <bool>                c_dut_valid;
	sc_signal <bool>                c_dut_ready;

  zhw::encode_ints<fpn_t, DIMS> u_dut;

  SC_CTOR(mymodule) : u_dut("u_dut")
  {
    // connect DUT
    u_dut.clk(clk);
    u_dut.reset(reset);

    u_dut.s_block(c_driver_block);
    u_dut.s_valid(c_driver_valid);
    u_dut.s_ready(c_driver_ready);

    u_dut.s_flush(c_driver_flush);

    u_dut.m_bc   (c_dut_bc);
    u_dut.m_bp   (c_dut_bp);
    u_dut.m_last (c_dut_last);
    u_dut.m_valid(c_dut_valid);
    u_dut.m_ready(c_dut_ready);
  }
};

int sc_main(int argc , char *argv[])
{
	mymodule mymod("mymod");
}

