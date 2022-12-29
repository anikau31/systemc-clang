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

// a simple divider that calculate one bit at a time
// this is used for fixing the timing issue
template<int BW>
SC_MODULE(divider) {
  typedef sc_uint<BW> val_t;
  typedef sc_bigint<2 * BW> tmp_t;
  sc_in<bool> clk;
  sc_in<bool> rst;

  sc_in<val_t> dividend;
  sc_in<val_t> divisor;
  sc_in<bool>  valid;

  sc_out<val_t>  quotient;
  sc_out<bool>  vld;

  sc_signal<val_t> _res;
  sc_signal<val_t> _op1[BW + 1];
  sc_signal<tmp_t> _op2[BW + 1];
  sc_signal<val_t> _quotient[BW + 1];
  sc_signal<val_t> _temp[BW + 1];
  sc_signal<bool>  _vld[BW + 1];

  void mc_proc() {
    // no output register
    vld.write(_vld[0]);
    quotient.write(_quotient[0]);
  }
  void ms_proc() {
    if(rst.read() == RLEVEL) {
      for(int i = 0; i <= BW; i++) {
        _vld[i].write(false);
      }
    } else {
      val_t one = 1;
      // initial pipeline
      _op1[BW].write(dividend.read());
      _op2[BW].write(divisor.read() << (BW - 1));
      _vld[BW].write(valid.read());
      _temp[BW].write(0);
      _quotient[BW].write(0);

      for(int i = BW - 1; i >= 0; i--) {
        if(_temp[i + 1].read() + _op2[i + 1].read() <= _op1[i + 1].read()) {
          _temp[i] = _temp[i + 1].read() + _op2[i + 1].read();
          _quotient[i] = _quotient[i + 1].read() | (one << i);
        } else {
          _temp[i] = _temp[i + 1].read();
          _quotient[i] = _quotient[i + 1].read();
        }
        _op2[i].write(_op2[i + 1].read() >> 1);
        _op1[i].write(_op1[i + 1].read());
        _vld[i].write(_vld[i + 1].read());
      }
    }
  }
  SC_CTOR(divider) 
  {

    SC_METHOD(mc_proc);
    sensitive << _vld[0] << _quotient[0];
    dont_initialize();
    
    SC_METHOD(ms_proc);
    sensitive << clk.pos();
    dont_initialize();
  }
};

template<typename T, int LATENCY>
SC_MODULE(delay_pipe) {
  sc_in<bool>   clk;
  sc_in<T>      d_in;
  sc_out<T>     d_out;
  sc_signal<T>  pipe[LATENCY];

  void mc_proc() {
    d_out.write(pipe[LATENCY-1]);
  }
  void ms_proc() {
    pipe[0].write(d_in);
    for(int i = 1; i < LATENCY; i++) {
      pipe[i] = pipe[i - 1];
    }
  }

  SC_CTOR(delay_pipe) {
    SC_METHOD(mc_proc);
    sensitive << pipe[LATENCY - 1];
    dont_initialize();

    SC_METHOD(ms_proc);
    sensitive << clk.pos();
    dont_initialize();
  }
};

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

  sc_signal<data_t> _cur_min, _cur_max;
  sc_signal<data_t> dividend;
  sc_signal<data_t> divisor;
  sc_signal<bool> div_vld;

  sc_signal<bool> div_out_vld;

  /*-------- submodules --------*/
  divider<DATAW> u_div { "u_div" };
  delay_pipe<data_t, DATAW> u_pipe_min { "u_min" };
  delay_pipe<data_t, DATAW> u_pipe_max { "u_max" };

  void mc_proc ()
  {
    min_out.valid_w(div_out_vld);
    min_out.data_w(cur_min);

    max_out.valid_w(div_out_vld);
    max_out.data_w(cur_max);

    avg_out.valid_w(div_out_vld);
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
      n = 0; insert = 0; /* cur_min = cur_max = cur_avg = 0; */
      _cur_min = -1;
      _cur_max = 0;
      sum = 0;
      for (int i=0; i<WINDOW_SIZE; i++) {
        window[i] = 0; 
      }
      datardy = true;
    } else {
      dividend.write((sum.read().to_uint() + datastrm.data.read().to_int() ));
      divisor.write((n.read().to_uint()+1));
      div_vld.write(datastrm.valid_r());

      if (datastrm.valid_r()) { // new data
        if (_cur_min > datastrm.data) _cur_min = datastrm.data;
        if (_cur_max < datastrm.data) _cur_max = datastrm.data;
        //window[n.read().to_uint()] = datastrm.data;
        window[insert.read().to_uint()] = datastrm.data;
        if (n.read().to_uint() < WINDOW_SIZE) n.write(n.read().to_uint() +1);


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

    u_div.clk(clk);
    u_div.rst(reset);
    u_div.dividend(dividend);
    u_div.divisor(divisor);
    u_div.valid(div_vld);

    u_div.quotient(cur_avg);
    u_div.vld(div_out_vld);


    u_pipe_min.clk(clk);
    u_pipe_min.d_in(_cur_min);
    u_pipe_min.d_out(cur_min);


    u_pipe_max.clk(clk);
    u_pipe_max.d_in(_cur_max);
    u_pipe_max.d_out(cur_max);

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
