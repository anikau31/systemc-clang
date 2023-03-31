#include <hls_stream.h>
#include <ap_int.h>
#include<ap_axi_sdata.h>

// used for axis.last signal
typedef ap_axiu<32,0,0,0> flit_t;

void strm_in(
    ap_uint<32>* mem_in,
    int in_cnt,
    hls::stream<flit_t>& x
    ) {
  for(int i = 0; i < in_cnt; i++) {
    auto d = mem_in[i];
    flit_t f;
    f.data = d;
    f.last = i == (in_cnt - 1);
    x.write(f);
  }
}

void strm_out(
    ap_uint<32>* mem_out,
    int out_cnt,
    hls::stream<flit_t>& y
    ) {
  for(int i = 0; i < out_cnt; i++) {
    auto d = y.read();
    mem_out[i] = d.data;
  }
}

void mms(
    ap_uint<32>* mem_in,
    ap_uint<32>* mem_out,
    int in_cnt,
    int out_cnt,
    hls::stream<flit_t>& x,
    hls::stream<flit_t>& y
    ) {
  strm_in(mem_in, in_cnt, x);
  strm_out(mem_out, out_cnt, y);
}
