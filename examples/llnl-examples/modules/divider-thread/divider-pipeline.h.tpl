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

#define STAGE_NAME(i) stage_thread_##i
#define STAGE_THREAD(t) SC_THREAD(t)
#define STAGE(i) \
  void STAGE_NAME(i)() { \
    val_t one = 1; \
    _vld[i].write(false); \
    wait(); \
    while(true) { \
        if(_temp[i + 1].read() + _op2[i + 1].read() <= _op1[i + 1].read()) { \
          _temp[i] = _temp[i + 1].read() + _op2[i + 1].read(); \
          _quotient[i] = _quotient[i + 1].read() | (one << i); \
        } else { \
          _temp[i] = _temp[i + 1].read(); \
          _quotient[i] = _quotient[i + 1].read(); \
        } \
        _op2[i].write(_op2[i + 1].read() >> 1); \
        _op1[i].write(_op1[i + 1].read()); \
        _vld[i].write(_vld[i + 1].read()); \
        wait(); \
      } \
    } \

#define STAGE(i, j) \
  void STAGE_NAME(i)() { \
    val_t one = 1; \
    _vld##i.write(false); \
    wait(); \
    while(true) { \
        if(_temp##j.read() + _op2##j.read() <= _op1##j.read()) { \
          _temp##i = _temp##j.read() + _op2##j.read(); \
          _quotient##i = _quotient##j.read() | (one << i); \
        } else { \
          _temp##i = _temp##j.read(); \
          _quotient##i = _quotient##j.read(); \
        } \
        _op2##i.write(_op2##j.read() >> 1); \
        _op1##i.write(_op1##j.read()); \
        _vld##i.write(_vld##j.read()); \
        wait(); \
      } \
    } \



typedef sc_uint<DATAW> data_t;

extern sc_trace_file *tf;

// a simple divider that calculate one bit at a time
// this is used for fixing the timing issue
template<int BW>
SC_MODULE(divider) {
  typedef sc_uint<BW> val_t;
  typedef sc_biguint<2 * BW> tmp_t;
  sc_in<bool> clk;
  sc_in<bool> arst;

  sc_stream_in<val_t> dividend;
  sc_stream_in<val_t> divisor;

  sc_stream_out<val_t>  quotient;

  sc_signal<bool>  _sync;

  // original pipelined signals
  sc_signal<val_t> _res;
  {% for i in range(9) %}
    sc_signal<val_t> _op1{{i}};
    sc_signal<tmp_t> _op2{{i}};
    sc_signal<val_t> _quotient{{i}};
    sc_signal<val_t> _temp{{i}};
    sc_signal<bool>  _vld{{i}};
  {% endfor %}
  // sc_signal<val_t> _op1[BW + 1];
  // sc_signal<tmp_t> _op2[BW + 1];
  // sc_signal<val_t> _quotient[BW + 1];
  // sc_signal<val_t> _temp[BW + 1];
  // sc_signal<bool>  _vld[BW + 1];

  void mc_proc() {
    _sync.write(dividend.valid_r() && divisor.valid_r());
    dividend.ready_w(true);
    divisor.ready_w(true);
    quotient.valid_w(_vld0.read());
    quotient.data_w(_quotient0.read());
  }

  {% for i in range(8) %}
    // STAGE({{i}});
    STAGE({{i}}, {{i+1}});
  {% endfor %}

  void thread_proc_in() {
    while(true) {
      if(_sync.read()) {
        _op1{{8}}.write(dividend.data_r());
        _op2{{8}}.write(tmp_t(divisor.data_r()) << ({{8}}- 1));
        _quotient{{8}}.write(0);
        _temp{{8}}.write(0);
        _vld{{8}}.write(true);
      } else {
        _vld{{8}}.write(true);
      }
      wait();
    }
  }

  SC_CTOR(divider) 
  {
    SC_METHOD(mc_proc);
    sensitive << dividend.valid_chg() << divisor.valid_chg() << _vld0 << _quotient0;

    SC_THREAD(thread_proc_in);
    sensitive << clk.pos();
    async_reset_signal_is(arst, RLEVEL);

    // pipeline stages

    // STAGE_THREAD(STAGE_NAME(1));
    // sensitive << clk.pos();
    // async_reset_signal_is(arst, RLEVEL);

    //   STAGE_THREAD(STAGE_NAME(0));
    //   sensitive << clk.pos();
    //   async_reset_signal_is(arst, RLEVEL);
    {% for i in range(8) %}
      STAGE_THREAD(STAGE_NAME({{i}}));
      sensitive << clk.pos();
      async_reset_signal_is(arst, RLEVEL);
    {% endfor %}

  }
};

