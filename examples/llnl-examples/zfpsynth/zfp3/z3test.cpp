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


// Process Prefix
// --------------
// mc: method combinatorial logic
// ms: method sequential logic
// ct: clocked thread
// th: thread

// Port & Channel Prefix
// ---------------------
// m: port master
// s: port slave
// c: channel
// u: module instance (unit)

// Send/Recv data on a stream.
// Translate between the I/O buffer types and SystemC types.
// The tb_driver is not intended to be synthesizable.
template<typename I, typename O>
SC_MODULE(tb_driver)
{
	sc_in<bool> clk;
	sc_in<bool> reset;

	sc_stream_in <I> s_port;
	sc_stream_out<O> m_port;

	sc_time t0, t1;
	uint64_t eticks; // software encode time in cycles
	int retval;

	void ct_send()
	{
		real_t *buf = ptcase->get_buf_R1();
		O flit;
		m_port.reset();
		wait();
#if defined(P_DATA)
		cout << " Uncompressed real data (input to encode)" << endl;
		ptcase->dump(buf, blocks*BLOCK_LEN*sizeof(real_t), sizeof(real_t));
#endif
		t0 = sc_time_stamp();
		for (unsigned j = 0; j < blocks; j++) {
			for (unsigned i = 0; i < BLOCK_LEN; i++) {
				flit = buf[j*BLOCK_LEN+i];
				// wait();
#if defined(P_IO)
				cout << "tb_driver::ct_send ts: " << sc_time_stamp() << ", flit: " << flit << endl;
#endif
				m_port.write(flit); // write has call to wait();
			}
		}
	}

	void ct_recv()
	{
		enc_t::uic_t *buf = ptcase->get_buf_EN();
		unsigned len = 0;
		I flit;
		s_port.reset();
		wait();
		for (unsigned j = 0; j < blocks; j++) {
			for (unsigned i = 0; ; i++)  {
				// wait();
				flit = s_port.read(); // read has call to wait();
#if defined(P_IO)
				cout << "tb_driver::ct_recv ts: " << sc_time_stamp() << ", flit: " << flit << endl;
#endif
				buf[len++] = flit.tdata.to_uint64();
				if (flit.tlast) break;
			}
		}
		t1 = sc_time_stamp();
#if defined(P_DATA)
		cout << " Compressed data (output from encode, input to decode)" << endl;
		ptcase->dump(buf, len*sizeof(enc_t::uic_t), sizeof(enc_t::uic_t));
#endif
		retval = ptcase->check_EN(len, eticks);
		// sync after whole batch is done
	}

	// need threads for decode
	// enc_t::uic_t *buf = ptcase->get_buf_EN();
	// real_t *buf = ptcase->get_buf_R2();
	// retval = ptcase->check_R2();

	SC_CTOR(tb_driver) :
		clk("clk"),
		reset("reset"),
		retval(1)
	{
		SC_CTHREAD(ct_send, clk.pos());
			reset_signal_is(reset, RLEVEL);

		SC_CTHREAD(ct_recv, clk.pos());
			reset_signal_is(reset, RLEVEL);
	}
};

using namespace zhw;

SC_MODULE(mymodule)
{
  sc_clock clk; 
  sc_signal<bool> reset;

  sc_signal <uconfig_t> minbits;
  sc_signal <uconfig_t> maxbits;
  sc_signal <uconfig_t> maxprec;
  sc_signal <sconfig_t> minexp; // assume biased



  sc_stream<fpn_t> c_driver_fp;
  sc_stream<enc_t> c_dut_enc;
  zhw::encode<fpn_t, DIMS, enc_t> u_dut;

  SC_CTOR(mymodule) : u_dut("u_dut")
  {

    minbits = fpn_t::bits; /* minimum number of bits per 4^d block */
    maxbits = fpn_t::bits; /* maximum number of bits per 4^d block */
    maxprec = fpn_t::bits; /* maximum precision (# bit planes coded) */
    minexp = 1-fpn_t::ebias-fpn_t::fbits;

 // connect DUT
	u_dut.clk(clk);
	u_dut.reset(reset);
	u_dut.minbits(minbits);
	u_dut.maxbits(maxbits);
      
	u_dut.maxprec(maxprec);
	u_dut.minexp(minexp);
	u_dut.s_fp(c_driver_fp);
	u_dut.m_bits(c_dut_enc);
  }
};

int sc_main(int argc , char *argv[])
{
	mymodule mymod("mymod");
}

