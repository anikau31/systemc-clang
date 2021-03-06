#define RVD

#include "pulse.h"
#include "zfp.h"

#define E 11
#define F 52
// #define E  8
// #define F 23
#define DIM 2
#define BLOCK_SIZE fpblk_sz(DIM) /* values per block */
// #define __SYNTHESIS__

typedef fp_t<E,F> real_t;
typedef real_t flit_t;
typedef typename flit_t::si_t si_t;


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

SC_MODULE(tb_driver)
{
	sc_in<bool> clk;
	sc_in<bool> reset;

	sc_stream_out<flit_t> m_fp;
	sc_stream_in <si_t> s_int;

	int retval;

	void ct_send()
	{
		flit_t flit;
		m_fp.reset();
		wait();
		for (int j = 0; j < 2; j++) {
		for (int i = 0; i < BLOCK_SIZE; i++) {
			flit = block[i];
			// wait(); // write also has a call to wait();
#ifndef __SYNTHESIS__
			cout << "tb_driver::ct_send ts: " << sc_time_stamp() << ", flit: " << flit << endl;
#endif
			m_fp.write(flit);
		}
		}
	}

	void ct_recv()
	{
		si_t flit;
		s_int.reset();
		wait();
		for (int j = 0; j < 2; j++) {
		for (int i = 0; i < BLOCK_SIZE; i++)  {
			// wait(); // read also has a call to wait();
			flit = s_int.read();
#ifndef __SYNTHESIS__
			cout << "tb_driver::ct_recv ts: " << sc_time_stamp() << ", flit: " << flit << endl;
#endif
		}
		}
		retval = 0;
	}

	SC_CTOR(tb_driver) :
		clk("clk"),
		reset("reset"),
		retval(1)
	{
		SC_CTHREAD(ct_send, clk.pos());
			reset_signal_is(reset, false);

		SC_CTHREAD(ct_recv, clk.pos());
			reset_signal_is(reset, false);
	}
};

sc_trace_file *tf;

int sc_main(int argc , char *argv[])
{
	sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
	// sc_report_handler::set_actions(SC_ID_LOGIC_X_TO_BOOL_, SC_LOG);
	// sc_report_handler::set_actions(SC_ID_VECTOR_CONTAINS_LOGIC_VALUE_, SC_LOG);
	// sc_report_handler::set_actions(SC_ID_OBJECT_EXISTS_, SC_LOG);

	sc_clock clk("clk", 10, SC_NS); // create a 10ns period clock signal
	sc_signal<bool> reset("reset");
	sc_stream<flit_t> c_driver_fp("c_driver_fp");
	sc_stream<si_t> c_dut_int("c_dut_int");

	pulse<0,2,false> u_pulse("u_pulse");
	tb_driver u_tb_driver("u_tb_driver");
	zfp_encode<flit_t, DIM> u_dut("u_dut");

	// HP: work around
	find_emax<flit_t, DIM> u_find_emax("u_find_emax");
	u_find_emax.clk(clk);
	u_dut.u_find_emax = &u_find_emax;

	// connect reset
	u_pulse.clk(clk);
	u_pulse.sig(reset);

	// connect driver
	u_tb_driver.clk(clk);
	u_tb_driver.reset(reset);
	u_tb_driver.m_fp(c_driver_fp);
	u_tb_driver.s_int(c_dut_int);

	// connect DUT
	u_dut.clk(clk);
	u_dut.reset(reset);
	u_dut.s_fp(c_driver_fp);
	u_dut.m_int(c_dut_int);

#if defined(VCD)
	extern sc_trace_file *tf;
	tf = sc_create_vcd_trace_file("test");
	tf->set_time_unit(1, SC_NS);
	sc_trace(tf, clk, clk.name());
	sc_trace(tf, reset, reset.name());
	sc_trace(tf, c_driver_fp, c_driver_fp.name());
	sc_trace(tf, c_dut_int, c_dut_int.name());
#endif

	cout << "INFO: Simulating " << endl;

	// start simulation 
	sc_start(1100, SC_NS);

	if (u_tb_driver.retval != 0) {
		printf("Test failed!\n");
	} else {
		printf("Test passed!\n");
	}
	return u_tb_driver.retval;
}
