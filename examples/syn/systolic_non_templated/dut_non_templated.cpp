#include "systolic_array_non_templated.h"

// non-templated
SC_MODULE(mymodule) {

  typedef sc_uint<8> T;
  typedef sc_uint<16> Tmult;
  typedef sc_uint<16> Tacc;

  systolic_non_templated u_dut;

    sc_clock clk; 
    sc_signal<bool> rst;
    sc_signal<bool> enable_row_count_A;


    sc_signal<T>   A[N1];
    sc_signal<T>   B[N2];
    sc_signal<Tacc> D[N1];
    sc_signal<sc_bv<N1>> valid_D;

  

  SC_CTOR(mymodule) : u_dut { "systolic" } {

    u_dut.clk(clk);
    u_dut.rst(rst);


    // u_dut.in_a(in_a);
    // u_dut.in_b(in_b);
    // u_dut.out_a(out_a);
    // u_dut.out_b(out_b);

    // u_dut.in_data(in_data);
    // u_dut.in_valid(in_valid);
    // u_dut.out_data(out_data);
    // u_dut.out_valid(out_valid);



  }

};


int sc_main(int argc, char** argvj) {
    mymodule mymod { "mymod" };
}