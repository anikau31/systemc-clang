#include <hls_stream.h>
#include <ap_int.h>
#include<ap_axi_sdata.h>

// used for axis.last signal
// typedef ap_axiu<64,0,0,0> flit_t;
typedef ap_uint<64> flit_t;

void strm_in(
    ap_uint<64>* mem_in_x,
    ap_uint<64>* mem_in_y,
    int in_cnt,
    hls::stream<flit_t>& x,
    hls::stream<flit_t>& y
    ) {
  for(int i = 0; i < in_cnt; i++) {
    auto dividend = mem_in_x[i];
    auto divisor = mem_in_y[i];
    flit_t f;
    f = dividend;
    // f.data = dividend;
    // f.last = i == (in_cnt - 1);
    x.write(f);

    // f.data = divisor;
    f = divisor;
    y.write(f);
  }
}

void strm_out(
    ap_uint<64>* mem_out,
    int out_cnt,
    hls::stream<flit_t>& y
    ) {
  for(int i = 0; i < out_cnt; i++) {
    auto d = y.read();
    // mem_out[i] = d.data;
    mem_out[i] = d;
  }
}

void mms(
    ap_uint<64>* mem_in_x,
    ap_uint<64>* mem_in_y,
    ap_uint<64>* mem_out,
    int in_cnt,
    int out_cnt,
    hls::stream<flit_t>& x,
    hls::stream<flit_t>& y,
    hls::stream<flit_t>& z
    ) {
  strm_in(mem_in_x, mem_in_y, in_cnt, x, y);
  strm_out(mem_out, out_cnt, z);
}
