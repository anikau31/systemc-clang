/* Placeholder for further modules */
// TODO: include example directory
#define RVD 1
#include "systemc.h"

#if !defined(RLEVEL)
#define RLEVEL false
#endif

#include "pulse.h"
#include "zfp.h"

#define EXPOW 11
#define FRACW 52
// #define EXPOW 8
// #define FRACW 23
#ifndef DIMS
#define DIMS 1
#endif
#define BLOCK_CNT 2
#define BLOCK_LEN fpblk_sz(DIMS) /* values per block */

typedef fp_t<EXPOW, FRACW> fpn_t;
typedef fpn_t enc_t;
typedef typename fpn_t::expo_t expo_t;

SC_MODULE(submodule) {
  sc_in<int> input;
  sc_out<int> output;
  sc_stream_in<fpn_t> stream_in_port;
  sc_stream_out<fpn_t> stream_out_port;

  void entry() {}
  SC_CTOR(submodule) {
    SC_METHOD(entry) ;

  }
};

SC_MODULE(test) {
  sc_in_clk clk;

  sc_in<int> inS;
  sc_out<int> outS;
  sc_in<double> inT;
  sc_out<double> outT;

  sc_stream_in<fpn_t> test_stream_in_port;
  sc_stream_out<fpn_t> test_stream_out_port;

  // sc_stream<32> input_stream;
  // sc_stream<32> output_stream;
//
  submodule sub_module_member;

  void entry() {
    while (true) {}
  }

  SC_CTOR(test) : sub_module_member("SUBMODULE") {
    sub_module_member.input(inS);
    sub_module_member.output(outS);
    sub_module_member.stream_in_port(test_stream_in_port);
    sub_module_member.stream_out_port(test_stream_out_port);

    SC_METHOD(entry);
    sensitive << clk.pos();
  }
};

int sc_main(int argc, char *argv[]) {
  sc_clock clk;
  sc_signal<int> sig1;

  sc_stream<fpn_t> input_stream;
  sc_stream<fpn_t> output_stream;

  test test_instance("mymodule_instance");
  test_instance.clk(clk);
  test_instance.inS(sig1);
  test_instance.outS(sig1);
  test_instance.test_stream_in_port(input_stream);
  test_instance.test_stream_out_port(output_stream);

  return 0;
}
