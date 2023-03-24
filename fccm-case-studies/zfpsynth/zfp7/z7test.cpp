#define __SYNTHESIS__

#if defined(__SYNTHESIS__)
#define RVD
#endif

#include <cmath> // floor
#include <cstdlib> // exit, atoi, atof
#include <unistd.h> // getopt, optarg, optind
#include <limits> // numeric_limits

#include "../shared3/pulse.h"
// zhw_encode.h is required to bring in block_buffer with proper template type
#include "../shared3/zhw_encode.h"
#include "../shared3/zhw_decode.h"

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
// #include "real.h"

#ifndef DIMS
#define DIMS 2
#endif

#define BLOCK_LEN zhw::fpblk_sz(DIMS) /* values per block */

typedef fp_t<EXPOW,FRACW> fpn_t;  // floating-point number type
typedef bits_t<DATAW> enc_t;      // encoded bitstream type
typedef zhw::sconfig_t sconfig_t; // signed configuration parameter type
typedef zhw::uconfig_t uconfig_t; // unsigned configuration parameter type

// #include "tcase.h" // test cases


// Instantiation of DUT, similar to test/decode_test/decode_tb.h

/*
+--------------------------------------------------+
|              zhw::decode<FP, DIM, B>             |
| sc_in<bool> clk       sc_stream_out<FP> m_stream |
| sc_in<bool> reset                                |
|                                                  |
|                                                  |
| sc_in <uconfig_t> maxbits                        |
| sc_in <uconfig_t> minbits                        |
| sc_in <uconfig_t> maxprec                        |
| sc_in <sconfig_t> minexp                         |
|                                                  |
| sc_stream_in <B> s_bits                          |
+--------------------------------------------------+
 */

using namespace zhw;

SC_MODULE(mymodule)
{
  sc_clock clk; 
  sc_signal<bool> reset;

  sc_signal <uconfig_t> minbits;
  sc_signal <uconfig_t> maxbits;
  sc_signal <uconfig_t> maxprec;
  sc_signal <sconfig_t> minexp; // assume biased



  sc_stream<fpn_t> c_dut_fp;
  sc_stream<enc_t> c_driver_enc;
  zhw::decode<fpn_t, DIMS, enc_t> u_dut;

  SC_CTOR(mymodule) : u_dut("u_dut") {
    // These values are encoded in test/decode_test/tcasestream.h
    minbits = 128;        /* minimum number of bits per 4^d block */
    maxbits = 128;        /* maximum number of bits per 4^d block */
    maxprec = fpn_t::bits; /* maximum precision (# bit planes coded) */
    // minexp = 1-fpn_t::ebias-fpn_t::fbits;
    minexp = -1074;

    // connect DUT
    u_dut.clk(clk);
    u_dut.reset(reset);
    u_dut.minbits(minbits);
    u_dut.maxbits(maxbits);
        
    u_dut.maxprec(maxprec);
    u_dut.minexp(minexp);
    u_dut.s_bits(c_driver_enc);
    u_dut.m_stream(c_dut_fp);
  }
};

int sc_main(int argc , char *argv[])
{
	mymodule mymod("mymod");
}

