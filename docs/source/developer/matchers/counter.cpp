#include <systemc.h>

SC_MODULE(counter) {
  // clock
  sc_in_clk clk;

  // output port
  sc_out<sc_uint<32>> count_out;

  // member variable
  sc_uint<32> keep_count;

  SC_CTOR(counter) {
    SC_METHOD(count_up);
    sensitive << clk.pos();
  }

  void count_up() {
    keep_count = keep_count + 1;
    count_out.write( keep_count );
  }

};

// Top level module.
SC_MODULE(DUT) {
  counter count;
  sc_clock clock;
  sc_signal< sc_uint<32> > counter_out;

  SC_CTOR(DUT): count{"counter_instance"} {
    // port bindings
    count.clk(clock);
    count.count_out(counter_out);
  };
};

int sc_main() {
  DUT dut{"design_under_test"};
  return 0;
};


