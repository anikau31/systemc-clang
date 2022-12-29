
//#define __SYNTHESIS__
#if defined(__SYNTHESIS__)
#define RVD
#endif

#if !defined(RLEVEL)
#define RLEVEL false
#endif

#ifndef DATAW
#define DATAW 64   // bitstream data width
#endif

#include "shared2/pulse.h"
#include "divider.h"

#define BLOCK_LEN 256

template<typename I, typename O>
SC_MODULE(tb_driver)
{
  /*-------- ports --------*/
  sc_in<bool> clk;
  sc_in<bool> reset;

  sc_stream_in <I> quotient_port; //s_port;
  sc_stream_out<O> dividend_port, divisor_port;

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
    bool sync = dividend_port.ready_r() && divisor_port.ready_r() && valid;
    O flit(block[scount%BLOCK_LEN]);

    dividend_port.data_w(flit);
    dividend_port.valid_w(valid);
    divisor_port.data_w(flit);
    divisor_port.valid_w(valid);

    c_sync_send = sync;
#ifndef __SYNTHESIS__
    if (sync) {
      cout << name() << "::mc_send ts: " << sc_time_stamp()
	   << ", flit: " << flit << ", sync:" << sync
	   << ", c_sync_send: " << c_sync_send
	   << ", dividend.ready_r: " << dividend_port.ready_r() 
	   << ", divisor.ready_r: " << divisor_port.ready_r() << endl;
    }
#endif
  }

  void ms_send()
  {
    if (reset == RLEVEL) {
      scount = 0;
    } else {
      if (c_sync_send) scount = scount.read() + 1;
      //scount = scount.read() + 1;
    }
  }

  /*-------- recv processes --------*/
  void mc_recv()
  {
    bool sync = quotient_port.valid_r(); // always ready
    //s_port.valid_r(); // always ready
    //I flit = s_port.data_r();
    I quotient = quotient_port.data_r();

    quotient_port.ready_w(true);
    //s_port.ready_w(true);
    error = rcount.read() != BLOCK_LEN;

    c_sync_recv = sync;
#ifndef __SYNTHESIS__
    if (sync) {
      cout << name() << "::mc_recv ts: " << sc_time_stamp() <<
	", quotient: " << quotient << endl;
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

  #if defined(VCD)
  void start_of_simulation()
  {
    sc_trace(tf, dividend_port,   (std::string(name())+".dividend").c_str());
    sc_trace(tf, divisor_port,   (std::string(name())+".divisor").c_str());
    sc_trace(tf, quotient_port,   (std::string(name())+".quotient_port").c_str());
    sc_trace(tf, scount,  (std::string(name())+".scount").c_str());
    sc_trace(tf, rcount,  (std::string(name())+".rcount").c_str());
    sc_trace(tf, c_sync_send,   (std::string(name())+".c_sync_send").c_str());
    sc_trace(tf, c_sync_recv,   (std::string(name())+".c_sync_recv").c_str());
  }
#endif
  SC_CTOR(tb_driver)
  {
    for (int i = 0; i < BLOCK_LEN; i++)
      block[i] = i;
    SC_METHOD(mc_send);
    sensitive << dividend_port.ready_chg() << divisor_port.ready_chg() << clk.pos();
    sensitive << scount;
    SC_METHOD(ms_send);
    sensitive << clk.pos();
    dont_initialize();

    SC_METHOD(mc_recv);
    sensitive << clk.pos();
    sensitive << quotient_port.valid_chg() << quotient_port.data_chg();
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
	
  sc_stream <data_t> driv_dividend("dividend");
  sc_stream <data_t> driv_divisor("divisor");
  sc_stream <data_t> dut_quotient("quotient");

  sc_signal<bool> c_error("c_error");
  
  pulse<0,3,RLEVEL> u_pulse("u_pulse");
  tb_driver<data_t, data_t> u_tb_driver("u_tb_driver");
  divider<DATAW> u_dut("moving_average");


  // connect reset
  u_pulse.clk(clk);
  u_pulse.sig(reset);

  // connect test bench driver
  u_tb_driver.clk(clk);
  u_tb_driver.reset(reset);
  u_tb_driver.dividend_port(driv_dividend);
  u_tb_driver.divisor_port(driv_divisor);
  u_tb_driver.quotient_port(dut_quotient);
  u_tb_driver.error(c_error);

  // connect DUT
  u_dut.clk(clk);
  u_dut.arst(reset);
  u_dut.dividend(driv_dividend);
  u_dut.dividend(driv_divisor);
  u_dut.quotient(dut_quotient);

#if defined(VCD)
  tf = sc_create_vcd_trace_file("test");
  tf->set_time_unit(1, SC_NS);
  sc_trace(tf, clk, clk.name());
  sc_trace(tf, reset, reset.name());
  sc_trace(tf, driv_data, "driv_data");//driv_data.name());
  sc_trace(tf,dut_min, "min"); //dut_min.name());
  sc_trace(tf,dut_max, "max"); //dut_max.name());
  sc_trace(tf,dut_avg, "avg"); //dut_avg.name());
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
