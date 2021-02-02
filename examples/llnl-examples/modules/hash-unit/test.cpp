
//#define __SYNTHESIS__

#define RVD
#if defined(RVD)
#include "shared2/sc_rvd.h"
#include "shared2/rvfifo_cc.h"
template <typename T> using sc_stream = sc_rvd<T>;
template <typename T> using sc_stream_in = sc_rvd_in<T>;
template <typename T> using sc_stream_out = sc_rvd_out<T>;
template <typename T, int IW,  bool RLEV> using sfifo_cc = rvfifo_cc<T, IW, RLEV>;
#else
#include "shared2/sc_stream_ports.h"
#include "shared2/sc_stream.h"
#include "shared2/sfifo_cc.h"
#endif



#if !defined(RLEVEL)
#define RLEVEL false
#endif

#include "shared2/pulse.h"
#include "shash.h"

#define BLOCK_LEN 256
#define C_INIT 0xDEADBEEFDEADBEEFULL
#define SEED 24409ULL

SC_MODULE(tb_driver)
{
  /*-------- ports --------*/
  sc_in<bool> clk;
  sc_in<bool> reset;

  sc_stream_out<sc_biguint<KEYLW+KEYW> > hdatatoshash;
  sc_stream_out<sc_biguint<TAPW> > htaptoshash;
  sc_stream_in<sc_biguint<TAPW> > htapfromshash;

  sc_out<bool> error;

  /*-------- registers --------*/
  sc_signal<int> scount;
  sc_signal<int> rcount;

  sc_biguint<KEYLW+KEYW> block[BLOCK_LEN];

    /*-------- channels --------*/
  sc_signal<bool> c_sync_send;
  sc_signal<bool> c_sync_recv;
  
  /*-------- send processes --------*/
  void mc_send()
  {
    bool valid = scount.read() < BLOCK_LEN;
    bool sync = htapfromshash.ready_r() && valid;
    sc_biguint<KEYLW+KEYW> flit(block[scount%BLOCK_LEN]);

    hdatatoshash.data_w(flit);
    hdatatoshash.valid_w(valid);
    htaptoshash.data_w((sc_uint<64>(C_INIT),sc_uint<64>(C_INIT),sc_uint<64>(SEED),sc_uint<64>(SEED)));
    htaptoshash.valid_w(valid);
    
    c_sync_send = sync;
#ifndef __SYNTHESIS__
    if (sync) {
      cout << name() << "::mc_send ts: " << sc_time_stamp()
	   << ", flit: " << flit << ", sync:" << sync
	   << ", c_sync_send: " << c_sync_send
	   << ", htapfromhash.ready_r: " << htapfromshash.ready_r() << endl;
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
    bool sync = htapfromshash.valid_r(); // always ready
    sc_biguint<TAPW> tapout = htapfromshash.data_r();

    htapfromshash.ready_w(true);
    //s_port.ready_w(true);
    error = rcount.read() != BLOCK_LEN;

    c_sync_recv = sync;
#ifndef __SYNTHESIS__
    if (sync) {
      cout << name() << "::mc_recv ts: " << sc_time_stamp() <<
	", htapfromshash: " << htapfromshash.data_r() <<endl;
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
    sc_trace(tf,hdatatoshash,   (std::string(name())+".datastrm").c_str());
    sc_trace(tf,htaptoshash,   (std::string(name())+".datastrm").c_str());
    sc_trace(tf,htapfromshash,   (std::string(name())+".htapfromshash").c_str());
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
    sensitive << hdatatoshash.ready_chg() << clk.pos();
    sensitive << htaptoshash.ready_chg();
    sensitive << scount;
    SC_METHOD(ms_send);
    sensitive << clk.pos();
    dont_initialize();

    SC_METHOD(mc_recv);
    sensitive << clk.pos();
    sensitive << htapfromshash.valid_chg() <<htapfromshash.data_chg();
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
	
  sc_stream<sc_biguint<KEYLW+KEYW> > hdata;
  sc_stream<sc_biguint<TAPW> > htaptoshash;
  sc_stream<sc_biguint<TAPW> > htapfromshash;


  sc_signal<bool> c_error("c_error");
  
  pulse<0,2,RLEVEL> u_pulse("u_pulse");
  tb_driver u_tb_driver("u_tb_driver");
  short_hash u_dut("u_dut"); // hash unit dut


  // connect reset
  u_pulse.clk(clk);
  u_pulse.sig(reset);

  // connect test bench driver
  u_tb_driver.clk(clk);
  u_tb_driver.reset(reset);
  u_tb_driver.hdatatoshash(hdata);
  u_tb_driver.htaptoshash(htaptoshash);
  u_tb_driver.htapfromshash(htapfromshash);
  u_tb_driver.error(c_error);

  // connect DUT
  u_dut.clk(clk);
  u_dut.reset(reset);
  u_dut.s_dat(hdata);
  u_dut.s_tap(htaptoshash);
  u_dut.m_tap(htapfromshash);

#if defined(VCD)
  tf = sc_create_vcd_trace_file("test");
  tf->set_time_unit(1, SC_NS);
  sc_trace(tf, clk, clk.name());
  sc_trace(tf, reset, reset.name());
  sc_trace(tf, hdata, "hdata");//driv_data.name());
  sc_trace(tf,htaptoshash, "htaptoshash"); //dut_min.name());
  sc_trace(tf,htapfromshash, "htapfromshash"); //dut_max.name());
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
