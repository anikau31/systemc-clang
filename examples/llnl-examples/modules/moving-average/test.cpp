
//#define __SYNTHESIS__
#if defined(__SYNTHESIS__)
#define RVD
#endif

#if !defined(RLEVEL)
#define RLEVEL false
#endif

#include "shared2/pulse.h"
#include "moving_average.h"

#define BLOCK_LEN 256

template<typename I, typename O>
SC_MODULE(tb_driver)
{
  /*-------- ports --------*/
  sc_in<bool> clk;
  sc_in<bool> reset;

  sc_stream_in <I> min_port, max_port, avg_port; //s_port;
  sc_stream_out<O> m_port;

  sc_out<bool> error;

  /*-------- registers --------*/
  sc_signal<int> scount;
  sc_signal<int> rcount;
  /*-------- data ---------*/
  O block[BLOCK_LEN];
  /*-------- channels --------*/
  sc_signal<bool> c_sync_send;
  sc_signal<bool> c_sync_recv;

  /*-------- send processes --------*/
  void mc_send()
  {
    bool valid = scount.read() < BLOCK_LEN;
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
    bool sync = min_port.valid_r() & max_port.valid_r() & avg_port.valid_r(); // always ready
    //s_port.valid_r(); // always ready
    //I flit = s_port.data_r();
    I minval = min_port.data_r();
    I maxval = max_port.data_r();
    I avgval = avg_port.data_r();

    min_port.ready_w(true);
    max_port.ready_w(true);
    avg_port.ready_w(true);
    //s_port.ready_w(true);
    error = rcount.read() != BLOCK_LEN;

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
    for (int i = 0; i < 100; i++)
      block[i] = i;
    SC_METHOD(mc_send);
    sensitive << m_port.ready_chg();
    sensitive << scount;
    SC_METHOD(ms_send);
    sensitive << clk.pos();
    dont_initialize();

    SC_METHOD(mc_recv);
    sensitive << min_port.valid_chg() <<min_port.data_chg();
    sensitive << max_port.valid_chg() <<max_port.data_chg();
    sensitive << avg_port.valid_chg() <<avg_port.data_chg();
    //sensitive << s_port.valid_chg() << s_port.data_chg();
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
	
  sc_stream <data_t> driv_data("driv_data");
  sc_stream <data_t>  dut_min("dut_min");
  sc_stream <data_t>  dut_max("dut_max");
  sc_stream <data_t>  dut_avg("dut_avg");

  pulse<0,2,RLEVEL> u_pulse("u_pulse");
  tb_driver<enc_t, fpn_t> u_tb_driver("u_tb_driver");
  moving_average<DATAW> u_dut;


  // connect reset
  u_pulse.clk(clk);
  u_pulse.sig(reset);

  // connect test bench driver
  u_tb_driver.clk(clk);
  u_tb_driver.reset(reset);
  u_tb_driver.m_port(driv_data);
  u_tb_driver.min_port(dut_min);
  u_tb_driver.max_port(dut_max);
  u_tb_driver.avg_port(dut_avg);
  u_tb_driver.error(c_error);

  // connect DUT
  u_dut.clk(clk);
  u_dut.reset(reset);
  u_dut.s_fp(driv_data);
  u_dut.min_out(dut_min);
  u_dut.max_out(dut_max);
  u_dut.avg_out(dut_avg);

#if defined(VCD)
  tf = sc_create_vcd_trace_file("test");
  tf->set_time_unit(1, SC_NS);
  sc_trace(tf, clk, clk.name());
  sc_trace(tf, reset, reset.name());
  sc_trace(tf, driv_data, driv_data.name());
  sc_trace(tf,dut_min, dut_min.name());
  sc_trace(tf,dut_max, dut_max.name());
  sc_trace(tf,dut_avg, dut_avg.name());
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
