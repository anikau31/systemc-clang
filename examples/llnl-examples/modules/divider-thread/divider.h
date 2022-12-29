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
  typedef sc_biguint<2 * BW> tmp_t;
  sc_in<bool> clk;
  sc_in<bool> arst;

  sc_stream_in<val_t> dividend;
  sc_stream_in<val_t> divisor;

  sc_stream_out<val_t>  quotient;

  sc_signal<val_t> _res;
  sc_signal<val_t> _op1;
  sc_signal<bool>  _op1_vld;
  sc_signal<tmp_t> _op2;
  sc_signal<bool>  _op2_vld;
  sc_signal<val_t> _quotient;
  sc_signal<val_t> _temp;
  sc_signal<bool>  _vld;
  sc_signal<bool>  _busy;
  sc_signal<bool>  _sync;
  sc_signal<bool>  _sync_out;

  void mc_proc() {
    _sync.write(dividend.valid_r() && divisor.valid_r() && !_vld.read());
    dividend.ready_w(!_op1_vld);
    divisor.ready_w(!_op2_vld);
    quotient.valid_w(_vld.read());
    quotient.data_w(_quotient.read());
    _sync_out.write(_vld.read() && quotient.ready_r());
  }

  void thread_proc_1() {
    // sc_signal<bool> _sync;

    _vld.write(false);
    _op1_vld.write(false);
    _op2_vld.write(false);

    wait();

    while(true) {
      // wait for input values
      // _sync.write(dividend.valid_r() && divisor.valid_r() && !_vld.read());
      if(_sync.read()) {
        // avoids combinational path between valid/ready
        _op1.write(dividend.data_r());
        _op2.write(tmp_t(divisor.data_r()));
        _op1_vld.write(true);
        _op2_vld.write(true);

        wait();

        _op1_vld.write(false);
        _op2_vld.write(false);

        wait();


        // perform div-sub division

        _quotient = 0;
        _temp = 0;
        _op1 = dividend.read();
        _op2.write(tmp_t(divisor.read()));
        _vld.write(false);
        wait();
        for(int i = BW - 1; i >= 0; i--) {
          if(_temp.read() + (_op2.read() << i) <= _op1.read()) {
            _temp.write(_temp.read() + (_op2.read() << 1));
            _quotient.write(_quotient.read() | (1LL << i));
          }
          wait();
        }
        _vld.write(true);
      }
      if(_sync_out.read()) {
        _vld.write(false):
      }
      wait();

    }
  }

  SC_CTOR(divider) 
  {
    SC_METHOD(mc_proc);
    sensitive << _op1_vld << _op2_vld;
    SC_CTHREAD(thread_proc_1, clk.pos());
    async_reset_signal_is(arst, RLEVEL);
  }
};

