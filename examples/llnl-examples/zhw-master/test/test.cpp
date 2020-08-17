
#include <cmath> // floor
#include <cstdlib> // exit, atoi, atof
#include <unistd.h> // getopt, optarg, optind
#include <limits> // numeric_limits

#include "pulse.h"
#include "zhw_encode.h"

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

#if defined(VCD)
namespace zhw {
	sc_trace_file *tf;
}
#endif

int sc_main(int argc , char *argv[])
{
	/* * * * * * * * * * get arguments * * * * * * * * * */
	int opt;
	bool nok = false;

	while ((opt = getopt(argc, argv, "b:r:t:")) != -1) {
		switch (opt) {
		case 'b':
			blocks = atoi(optarg);
			if (blocks < 1) {
				cerr << " -- error: number of blocks must be 1 or greater" << endl;
				nok = true;
			}
			break;
		case 'r':
			rate = atof(optarg);
			if (rate <= 0 || rate > fpn_t::bits) {
				cerr << " -- error: range must be 0 < rate <= " << fpn_t::bits << endl;
				nok = true;
			}
			break;
		case 't':
			tcarg = optarg;
			/* param check below */
			break;
		default: /* unknown option */
			nok = true;
		}
	}
	if (nok || optind < argc) {
		cerr << "Usage: test -b<int> -r<fp> -t<int>" << endl;
		cerr << "  -b  number of blocks, default: " << DEFAULT_BLOCKS << endl;
		cerr << "  -r  rate, default: " << DEFAULT_RATE << endl;
		cerr << "  -t  test case, default: " << DEFAULT_TCARG << endl;
		/* TODO: list test cases and args */
		cerr << endl;
		return EXIT_FAILURE;
	}
	cout << "dims: "   << DIMS   << endl;
	cout << "tcase: "  << tcarg  << endl;
	cout << "rate: "   << rate   << ", ";
	cout << "blocks: " << blocks << endl;

	/* * * * * * * * * * setup * * * * * * * * * */
	static_assert(sizeof(real_t)*8 == fpn_t::bits, "real_t must match fpn_t size");

	unsigned bits;
	// maximum possible
	// bits = bp_w(DIMS)*fpn_t::bits+fpn_t::ebits+1;

	// TODO: add rate function to zbatch that sets params? would need fpn_t?
	// based on rate (see zfp/src/zfp.c:zfp_stream_set_rate)
	bits = sc_max((unsigned)std::floor(BLOCK_LEN * rate + 0.5), (unsigned)(fpn_t::ebits+1));
	bits += enc_t::dbits - 1;
	bits &= ~(enc_t::dbits - 1);

	zpar par;
	par.minbits = bits;
	par.maxbits = bits;
	par.maxprec = fpn_t::bits; // ZFP_MAX_PREC
	par.minexp = 1-fpn_t::ebias-fpn_t::fbits; // ZFP_MIN_EXP (unbiased)

	switch (tcarg[0]) {
	case '1':
		ptcase = new tcase1<real_t,enc_t::uic_t,DIMS>(par, blocks);
		break;
	case '2':
		ptcase = new tcase2<real_t,enc_t::uic_t,DIMS>(par, blocks);
		break;
	case '3':
		ptcase = new tcase3<real_t,enc_t::uic_t,DIMS>(par, blocks);
		break;
	case '4':
		ptcase = new tcase4<real_t,enc_t::uic_t,DIMS>(par, blocks);
		break;
	case '5':
		ptcase = new tcase5<real_t,enc_t::uic_t,DIMS>(par, blocks);
		break;
	case '6':
		ptcase = new tcase6<real_t,enc_t::uic_t,DIMS>(par, blocks);
		break;
	default:
		cout << " -- error: unknown test case" << endl;
		return 1;
	}

	/* * * * * * * * * * SystemC start * * * * * * * * * */
	sc_report_handler::set_actions(SC_ID_IEEE_1666_DEPRECATION_, SC_DO_NOTHING);
	// sc_report_handler::set_actions(SC_ID_LOGIC_X_TO_BOOL_, SC_LOG);
	// sc_report_handler::set_actions(SC_ID_VECTOR_CONTAINS_LOGIC_VALUE_, SC_LOG);
	// sc_report_handler::set_actions(SC_ID_OBJECT_EXISTS_, SC_LOG);

	sc_clock clk("clk", 10, SC_NS); // create a 10ns period clock signal
	sc_signal<bool> reset("reset");

	sc_signal<uconfig_t> minbits("minbits");
	sc_signal<uconfig_t> maxbits("maxbits");
	sc_signal<uconfig_t> maxprec("maxprec");
	sc_signal<sconfig_t> minexp("minexp");

	minbits = par.minbits;
	maxbits = par.maxbits;
	maxprec = par.maxprec;
	minexp = par.minexp + fpn_t::ebias; // (biased)

	cout << "minbits: " << minbits.get_new_value() << ", ";
	cout << "maxbits: " << maxbits.get_new_value() << ", ";
	cout << "maxprec: " << maxprec.get_new_value() << ", ";
	cout << "minexp: "  << minexp.get_new_value()  << endl;

	sc_stream<fpn_t> c_driver_fp("c_driver_fp");
	sc_stream<enc_t> c_dut_enc("c_dut_enc");

	pulse<0,2,RLEVEL> u_pulse("u_pulse");
	tb_driver<enc_t, fpn_t> u_tb_driver("u_tb_driver");
	zhw::encode<fpn_t, DIMS, enc_t> u_dut("u_dut");

	// connect reset
	u_pulse.clk(clk);
	u_pulse.sig(reset);

	// connect driver
	u_tb_driver.clk(clk);
	u_tb_driver.reset(reset);
	u_tb_driver.m_port(c_driver_fp);
	u_tb_driver.s_port(c_dut_enc);

	// connect DUT
	u_dut.clk(clk);
	u_dut.reset(reset);
	u_dut.minbits(minbits);
	u_dut.maxbits(maxbits);
	u_dut.maxprec(maxprec);
	u_dut.minexp(minexp);
	u_dut.s_fp(c_driver_fp);
	u_dut.m_bits(c_dut_enc);

#if defined(VCD)
	using zhw::tf;
	tf = sc_create_vcd_trace_file("test");
	tf->set_time_unit(1, SC_NS);
	sc_trace(tf, clk, clk.name());
	sc_trace(tf, reset, reset.name());
	sc_trace(tf, minbits, minbits.name());
	sc_trace(tf, maxbits, maxbits.name());
	sc_trace(tf, maxprec, maxprec.name());
	sc_trace(tf, minexp, minexp.name());
	sc_trace(tf, c_driver_fp, c_driver_fp.name());
	sc_trace(tf, c_dut_enc, c_dut_enc.name());
#endif

	cout << "INFO: Simulating " << endl;

	// start simulation 
	// sc_start(600+(900*(int)blocks), SC_NS);
	sc_start(600+(2400*(int)blocks), SC_NS);

	if (u_tb_driver.retval != 0) {
		cout << "Test failed!" << endl;
	} else {
		cout << "Test passed!" << endl;
	}
	cout << "zfphw compression cycles: " <<
		(u_tb_driver.t1 - u_tb_driver.t0) / clk.period() << endl;
	cout << "libzfp compression cycles: " << u_tb_driver.eticks << endl;
	return u_tb_driver.retval;
}

/* TODO:
 * Generate test data and compare results with software ZFP
   - Runtime option to select generator function and exponent range
   - Support various IEEE FP precisions half, single, double, quad
 * Support other ZFP compression modes besides fixed-rate?
 */
/* DONE:
 * Generate test data and compare results with software ZFP
   - Include subnormals, and other extreme values,
   - Support checking with static blocks without linking with ZFP
   - Provide a compile option to check results with software ZFP
 */
