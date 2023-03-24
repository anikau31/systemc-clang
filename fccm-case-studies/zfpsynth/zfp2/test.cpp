
// #define __SYNTHESIS__
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
/* example 2D block of (reinterpreted) doubles */
static const unsigned block_emax = 0x3f7;
static const sc_dt::uint64 block[] = {
	0xbf7c3a7bb8495ca9ULL,
	0xbf79f9d9058ffdafULL,
	0xbf77c7abd0b61999ULL,
	0xbf75a42c806bd1daULL,
	0xbf738f8f740b8ea8ULL,
	0xbf718a050399fef8ULL,
	0xbf6f2772ff8c30feULL,
	0xbf6b59aa63d22f68ULL,
	0xbf67aaf8b80cff9eULL,
	0xbf641b9e71983592ULL,
	0xbf60abd3f723f2b7ULL,
	0xbf5ab7934169cc04ULL,
	0xbf54574f6f4897d3ULL,
	0xbf4c6e39da7fb99bULL,
	0xbf40ae5826a893d1ULL,
	0xbf25bce8e19d48e1ULL
};


template<typename I, typename O>
SC_MODULE(tb_driver)
{
	/*-------- ports --------*/
	sc_in<bool> clk;
	sc_in<bool> reset;

	sc_stream_in <I> s_port;
	sc_stream_out<O> m_port;

	sc_out<bool> error;

	/*-------- registers --------*/
	sc_signal<int> scount;
	sc_signal<int> rcount;

	/*-------- channels --------*/
	sc_signal<bool> c_sync_send;
	sc_signal<bool> c_sync_recv;

	/*-------- send processes --------*/
	void mc_send()
	{
		bool valid = scount.read() < BLOCK_CNT*BLOCK_LEN;
		bool sync = m_port.ready_r() && valid;
		O flit(block[scount%BLOCK_LEN]);

		m_port.data_w(flit);
		m_port.valid_w(valid);

		c_sync_send = sync;
#ifndef __SYNTHESIS__
		if (sync) {
			cout << name() << "::mc_send ts: " << sc_time_stamp() << ", flit: " << flit << endl;
		}
#endif
	}

	void ms_send()
	{
		if (reset == RLEVEL) {
			scount = 0;
		} else {
			if (c_sync_send) scount = scount.read() + 1;
		}
	}

	/*-------- recv processes --------*/
	void mc_recv()
	{
		bool sync = s_port.valid_r(); // always ready
		I flit = s_port.data_r();

		s_port.ready_w(true);
		error = rcount.read() != BLOCK_CNT; // * BLOCK_LEN;

		c_sync_recv = sync;
#ifndef __SYNTHESIS__
		if (sync) {
			cout << name() << "::mc_recv ts: " << sc_time_stamp() << ", flit: " << flit << endl;
		}
#endif
	}

	void ms_recv()
	{
		if (reset == RLEVEL) {
			rcount = 0;
		} else {
			if (c_sync_recv) rcount = rcount.read() + 1;
		}
	}

	SC_CTOR(tb_driver)
	{
		SC_METHOD(mc_send);
			sensitive << m_port.ready_chg();
			sensitive << scount;
		SC_METHOD(ms_send);
			sensitive << clk.pos();
			dont_initialize();

		SC_METHOD(mc_recv);
			sensitive << s_port.valid_chg() << s_port.data_chg();
			sensitive << rcount;
		SC_METHOD(ms_recv);
			sensitive << clk.pos();
			dont_initialize();
	}
};


sc_trace_file *tf;

int sc_main(int argc , char *argv[])
{
	sc_report_handler::set_actions(SC_ID_IEEE_1666_DEPRECATION_, SC_DO_NOTHING);
	// sc_report_handler::set_actions(SC_ID_LOGIC_X_TO_BOOL_, SC_LOG);
	// sc_report_handler::set_actions(SC_ID_VECTOR_CONTAINS_LOGIC_VALUE_, SC_LOG);
	// sc_report_handler::set_actions(SC_ID_OBJECT_EXISTS_, SC_LOG);

	sc_clock clk("clk", 10, SC_NS); // create a 10ns period clock signal
	sc_signal<bool> reset("reset");

	sc_stream<fpn_t> c_driver_fp("c_driver_fp");
	sc_stream<enc_t> c_dut_enc("c_dut_enc");
	sc_signal<bool> c_error("c_error");

	pulse<0,2,RLEVEL> u_pulse("u_pulse");
	tb_driver<enc_t, fpn_t> u_tb_driver("u_tb_driver");
	zfp_encode<fpn_t, DIMS, enc_t> u_dut("u_dut");

	// connect reset
	u_pulse.clk(clk);
	u_pulse.sig(reset);

	// connect test bench driver
	u_tb_driver.clk(clk);
	u_tb_driver.reset(reset);
	u_tb_driver.m_port(c_driver_fp);
	u_tb_driver.s_port(c_dut_enc);
	u_tb_driver.error(c_error);

	// connect DUT
	u_dut.clk(clk);
	u_dut.reset(reset);
	u_dut.s_fp(c_driver_fp);
	u_dut.m_enc(c_dut_enc);

#if defined(VCD)
	tf = sc_create_vcd_trace_file("test");
	tf->set_time_unit(1, SC_NS);
	sc_trace(tf, clk, clk.name());
	sc_trace(tf, reset, reset.name());
	sc_trace(tf, c_driver_fp, c_driver_fp.name());
	sc_trace(tf, c_dut_enc, c_dut_enc.name());
	sc_trace(tf, c_error, c_error.name());
#endif

#if !defined(__SYNTHESIS__)
	// start simulation 
	cout << "INFO: Simulating " << endl;
	sc_start(1100, SC_NS);

	if (c_error.read()) {
		printf("Test failed!\n");
	} else {
		printf("Test passed!\n");
	}
#endif

	return c_error.read();
}
