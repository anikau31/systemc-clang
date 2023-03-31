#include "systolic_array.h"

// SC_MODULE(mymodule) {
//   control<sc_uint<8>, sc_uint<16>, 4, 4, 8> u_dut;
// 
//   sc_clock clk; 
//   sc_signal<bool> rst;
//   
//   sc_signal<bool> enable_row_count;
//   sc_signal<decltype(u_dut.pixel_cntr_A)::data_type> pixel_cntr_A;
//   sc_signal<decltype(u_dut.slice_cntr_A)::data_type> slice_cntr_A;
//   sc_signal<decltype(u_dut.pixel_cntr_B)::data_type> pixel_cntr_B;
//   sc_signal<decltype(u_dut.slice_cntr_B)::data_type> slice_cntr_B;
//   sc_signal<decltype(u_dut.rd_addr_A)::data_type> rd_addr_A;
//   sc_signal<decltype(u_dut.rd_addr_B)::data_type> rd_addr_B;
// 
// 
//   SC_CTOR(mymodule) : u_dut { "control" } {
// 
//     u_dut.clk(clk);
//     u_dut.rst(rst);
// 
//     u_dut.enable_row_count(enable_row_count);
//     u_dut.pixel_cntr_A(pixel_cntr_A);
//     u_dut.slice_cntr_A(slice_cntr_A);
//     u_dut.pixel_cntr_B(pixel_cntr_B);
//     u_dut.slice_cntr_B(slice_cntr_B);
//     u_dut.rd_addr_A(rd_addr_A);
//     u_dut.rd_addr_B(rd_addr_B);
//   }
// 
// };

// // pe
// SC_MODULE(mymodule) {
//   pe<sc_uint<8>, sc_uint<16>, sc_uint<16>> u_dut;
// 
//   sc_clock clk; 
//   sc_signal<bool> rst;
//   
//   sc_signal<bool> init;
//   sc_signal<decltype(u_dut.in_a)::data_type> in_a;
//   sc_signal<decltype(u_dut.in_b)::data_type> in_b;
//   sc_signal<decltype(u_dut.out_a)::data_type> out_a;
//   sc_signal<decltype(u_dut.out_b)::data_type> out_b;
// 
//   sc_signal<decltype(u_dut.in_data)::data_type>   in_data;
//   sc_signal<decltype(u_dut.in_valid)::data_type>  in_valid;
//   sc_signal<decltype(u_dut.out_data)::data_type>  out_data;
//   sc_signal<decltype(u_dut.out_valid)::data_type> out_valid;
// 
// 
// 
//   SC_CTOR(mymodule) : u_dut { "control" } {
// 
//     u_dut.clk(clk);
//     u_dut.rst(rst);
// 
//     u_dut.in_a(in_a);
//     u_dut.in_b(in_b);
//     u_dut.out_a(out_a);
//     u_dut.out_b(out_b);
// 
//     u_dut.in_data(in_data);
//     u_dut.in_valid(in_valid);
//     u_dut.out_data(out_data);
//     u_dut.out_valid(out_valid);
// 
// 
// 
//   }
// 
// };
const int N1 = 8;
const int N2 = 8;
const int M = 8;
SC_MODULE(mymodule) {
  typedef sc_uint<8> T;
  typedef sc_uint<16> Tmult;
  typedef sc_uint<16> Tacc;

  systolic<T, Tmult, Tacc, N1, N2, M> u_dut;

  sc_clock clk;
  sc_signal<bool> rst;
  sc_signal<bool> enable_row_count_A;

  sc_signal<typename decltype(u_dut.pixel_cntr_A)::data_type> pixel_cntr_A;
  sc_signal<typename decltype(u_dut.slice_cntr_A)::data_type> slice_cntr_A;
  sc_signal<typename decltype(u_dut.pixel_cntr_B)::data_type> pixel_cntr_B;
  sc_signal<typename decltype(u_dut.slice_cntr_B)::data_type> slice_cntr_B;
  sc_signal<typename decltype(u_dut.rd_addr_A)::data_type> rd_addr_A;
  sc_signal<typename decltype(u_dut.rd_addr_B)::data_type> rd_addr_B;

  sc_signal<T> A[N1];
  sc_signal<T> B[N2];
  sc_signal<Tacc> D[N1];
  sc_signal<sc_bv<N1>> valid_D;

  SC_CTOR(mymodule) : u_dut{"systolic"} {
    u_dut.clk(clk);
    u_dut.rst(rst);

    u_dut.enable_row_count_A(enable_row_count_A);

    u_dut.pixel_cntr_A(pixel_cntr_A);
    u_dut.slice_cntr_A(slice_cntr_A);
    u_dut.pixel_cntr_B(pixel_cntr_B);
    u_dut.slice_cntr_B(slice_cntr_B);

    u_dut.rd_addr_A(rd_addr_A);
    u_dut.rd_addr_B(rd_addr_B);


  }

};

int sc_main(int argc, char** argvj) {
    mymodule mymod { "mymod" };
}