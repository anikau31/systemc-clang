#include "systemc.h"
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

typedef sc_uint<DATAW> data_t;

extern sc_trace_file *tf;

template<int WINDOW_SIZE>
SC_MODULE(moving_average)
{
  sc_in<bool> clk;
  sc_in<bool> reset;

  /*-------- ports --------*/
  sc_stream_in <data_t> datastrm;

  sc_stream_out<data_t> min_out;
  sc_stream_out<data_t> max_out;
  sc_stream_out<data_t> avg_out;
 

  /*-------- registers --------*/
  sc_signal <data_t> window[WINDOW_SIZE];
  
  sc_signal<sc_uint<8>> n, insert;
  sc_signal<data_t> sum;
  sc_signal<data_t> cur_min, cur_max, cur_avg;
  sc_signal<bool> datardy;
  void mc_proc ()
  {
    min_out.valid_w(datardy);
    min_out.data_w(cur_min);
    max_out.valid_w(datardy);
    max_out.data_w(cur_max);
    avg_out.valid_w(datardy);
    avg_out.data_w(cur_avg);
    datastrm.ready_w(true);

#ifndef __SYNTHESIS__
      cout << name() << "::mc_proc ts: " << sc_time_stamp()
	   << ", datardy: " << datardy << ", datastrm.ready: " << datastrm.ready
	    << endl;
    
#endif
  }
  
  void ms_proc ()
  {
    if (reset == RLEVEL) {
      n = 0; insert = 0; cur_min = cur_max = cur_avg = 0;
      sum = 0;
      for (int i=0; i<WINDOW_SIZE; i++)
	window[i] = 0; datardy = true;
    } else {
      if (datastrm.valid_r()) { // new data
	if (cur_min > datastrm.data) cur_min = datastrm.data;
	if (cur_max < datastrm.data) cur_max = datastrm.data;
	window[n.read().to_uint()] = datastrm.data;
	if (n.read().to_uint() < WINDOW_SIZE) n.write(n.read().to_uint() +1);
	cur_avg.write((sum.read().to_uint() + datastrm.data.read().to_int() )/ (n.read().to_uint()+1));
	sum.write(sum.read().to_uint() + datastrm.data.read().to_uint() - window[insert.read().to_uint()].read().to_uint());
	if ((int) insert.read() >= WINDOW_SIZE-1) insert.write(0);
	else insert.write(insert.read() + 1);
	datardy = true;
      }
    }
  }
#if defined(VCD)
  void start_of_simulation()
  {
    sc_trace(tf, datastrm,   (std::string(name())+".datastrm").c_str());
    sc_trace(tf, min_out,   (std::string(name())+".min_out").c_str());
    sc_trace(tf, max_out,   (std::string(name())+".max_out").c_str());
    sc_trace(tf, avg_out,   (std::string(name())+".avg_out").c_str());
    sc_trace(tf, sum,  (std::string(name())+".sum").c_str());
    sc_trace(tf, cur_min,   (std::string(name())+".cur_min").c_str());
    sc_trace(tf, cur_max,   (std::string(name())+".cur_max").c_str());
    sc_trace(tf, cur_avg,   (std::string(name())+".cur_avg").c_str());
    sc_trace(tf, insert, (std::string(name())+".insert").c_str());
  }
#endif

 SC_CTOR(moving_average) 
      {

	SC_METHOD(mc_proc);
	sensitive << clk.pos();
	dont_initialize();
	/* sensitive << datastrm.valid; */
	/* //sensitive << min_out.ready_event(); */
	/* sensitive << min_out.ready; */
	/* sensitive << max_out.ready; */
	/* sensitive << avg_out.ready; */
	SC_METHOD(ms_proc);
	sensitive << clk.pos();
	dont_initialize();
      }


};
	  
	  
      
