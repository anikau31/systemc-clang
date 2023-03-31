#include <hls_stream.h>
#include <ap_int.h>

void strm_in(ap_uint<64>* mem_in, 
    int size,
    hls::stream<ap_uint<64>>& data_strm
    ) {
  for(int i = 0; i < size; i++) {
    ap_uint<64> a = mem_in[i];
    data_strm.write(a);
  }
}

void strm_out(ap_uint<64>* mem_out, 
    int size,
    hls::stream<ap_uint<64>>& max_out,
    hls::stream<ap_uint<64>>& min_out,
    hls::stream<ap_uint<64>>& avg_out
    ) {
  for(int i = 0; i < size; i++) {
    auto mx = max_out.read();
    auto mn = min_out.read();
    auto av = avg_out.read();
    // sequential write, good enough for our case
    mem_out[i] = mx;
    mem_out[i + size] = mn;
    mem_out[i + 2 * size] = av;
  }
}

void starter(
    ap_uint<64>* mem_in,
    ap_uint<64>* mem_out,
    int size,

    // inter-kernel connection
    hls::stream<ap_uint<64>>& data_strm,
    hls::stream<ap_uint<64>>& max_out,
    hls::stream<ap_uint<64>>& min_out,
    hls::stream<ap_uint<64>>& avg_out
    ) {
// #pragma HLS dataflow
  strm_in(mem_in, size, data_strm);
  strm_out(mem_out, size, max_out, min_out, avg_out);
}
