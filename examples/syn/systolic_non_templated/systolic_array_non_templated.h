#pragma once

#include "systemc.h"

#include <iostream>

constexpr int clog2(int a) {
  return a == 1 ? 1 : 1 + clog2(a  / 2);
}


// mult := _Tin, _Tin -> _Tmult
// add  := _Tacc, _Tmult -> _Tacc
// TOOD: add operator overloading?
template<typename _Tin, typename _Tmult>
_Tmult mult(_Tin, _Tin);

template<typename _Tacc, typename _Tmult>
_Tacc add(_Tacc, _Tmult);


SC_MODULE(pe_non_templated) {
  typedef sc_uint<8> _Tin;
  typedef sc_uint<16> _Tmult;
  typedef sc_uint<16> _Tacc;
  sc_in<bool> clk;
  sc_in<bool> rst;

  sc_in<bool> init;

  sc_in<_Tin>  in_a;
  sc_in<_Tin>  in_b;
  sc_out<_Tin>  out_a;
  sc_out<_Tin>  out_b;

  sc_in<_Tacc>  in_data;
  sc_in<bool>   in_valid;
  sc_out<_Tacc> out_data;
  sc_out<bool>  out_valid;

  sc_signal<_Tmult> mult_op;
  sc_signal<_Tacc> out_sum;
  sc_signal<_Tacc> out_sum_d;

  sc_signal<_Tacc> in_data_r;
  sc_signal<bool> in_valid_r;



#if defined(VCD)
  void start_of_simulation() {
  }
#endif

  void ms_proc() {
    if(rst.read()) {
      out_a.write(0);
      out_b.write(0);
      in_valid_r.write(0);
      in_data_r.write(0);
      out_valid.write(0);
      out_data.write(0);
        out_sum.write(0);
    } else {
      out_a.write(in_a);
      out_b.write(in_b);

      in_valid_r.write(in_valid.read());
      in_data_r.write(in_data);

      if(init.read()) {
        out_sum.write(mult_op.read());
        out_valid.write(1);
        out_data.write(out_sum);
      } else {
        out_sum.write(out_sum_d.read());
        out_valid.write(in_valid_r.read());
        out_data.write(in_data_r.read());
      }
    }
  }
  void mc_proc() {
    // mult_op = mult<_Tin, _Tmult>(in_a, in_b);
    // out_sum_d = add<_Tacc, _Tmult>(out_sum.read(), mult_op.read());
    mult_op = in_a.read() * in_b.read();
    out_sum_d = out_sum.read() + mult_op.read();
  }


  SC_HAS_PROCESS(pe_non_templated);
  pe_non_templated() : ::sc_core::sc_module { "pe_nt_inst" } {
    SC_METHOD(ms_proc);
    sensitive << clk.pos();
    SC_METHOD(mc_proc);
    sensitive << in_a << in_b << out_sum << mult_op;
    dont_initialize();
  }
};

SC_MODULE(systolic_non_templated) {


    const static int N1 = 4;
    const static int N2 = 4;
    const static int M = 8;

    typedef sc_uint<8> _Tin;
    typedef sc_uint<16> _Tmult;
    typedef sc_uint<16> _Tacc;

    pe_non_templated pe_inst[N1][N2];

    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_in<bool> enable_row_count_A;


    sc_in<_Tin>   A[N1];
    sc_in<_Tin>   B[N2];
    sc_out<_Tacc> D[N1];
    sc_out<sc_bv<N1>> valid_D;

    sc_signal<_Tin>   a_wire[N1][N2];
    sc_signal<_Tin>   b_wire[N1][N2];
    sc_signal<bool>   valid_wire[N1][N2 + 1];
    sc_signal<_Tacc>  data_wire[N1][N2 + 1];

    sc_signal<bool> shift[2*N1];
    sc_signal<sc_uint<clog2((M * M) / (N1 * N2))>> patch;

    void ms_proc() {
        if(rst.read()) {
            for(int i = 0; i < 2 * N1; i++) {
                shift[i].write(0);
            }
            patch.write(0);
        } else {
            // if(pixel_cntr_A.read() == M - 1 && patch.read() < (M * M) / (N1 * N2) ) {
            //     patch.write(patch.read() + 1);
            //     shift[0].write(1);
            // } else {
            //     shift[0].write(0);
            // }
        }
    }

    void sa_mc_proc() {
        sc_bv<N1> valid_D_d;
        for(int i =0; i < N1; i++) {
            valid_D_d[i] = valid_wire[i][N2];
            // valid_D[i].write(valid_wire[i][N2]);
            D[i].write(data_wire[i][N2]);
            a_wire[i][0].write(A[i]);
            valid_wire[i][0] = 0;
            data_wire[i][0] = 0;
        }
        for(int j = 0; j < N2; j++) {
            b_wire[0][j].write(B[j]);
        }
        valid_D.write(valid_D_d);
    }

    // this module is only used for forcing the generation of pe

    SC_CTOR(systolic_non_templated) /*: pe_inst {"pe_inst", N1 * N2} */  {

        SC_METHOD(ms_proc);
        sensitive << clk.pos();

        SC_METHOD(sa_mc_proc);
        for(int i = 0; i < N1; i++) {
            for(int j = 0; j < N2 + 1; j++) {
                sensitive << valid_wire[i][j] << data_wire[i][j];
            }
        }
        for(int i = 0; i < N1; i++) sensitive << A[i];
        for(int i = 0; i < N2; i++) sensitive << B[i];


        for(int i = 0; i < N1; i++) {
            for(int j = 0; j < N2; j++) {
                pe_inst[i][j].clk(clk);
                pe_inst[i][j].rst(rst);
                pe_inst[i][j].init(shift[i + j + 1]);
                pe_inst[i][j].in_a(a_wire[i][j]);
                pe_inst[i][j].in_b(b_wire[i][j]);
                pe_inst[i][j].out_a(a_wire[i][j + 1]);
                pe_inst[i][j].out_b(b_wire[i + 1][j]);
                pe_inst[i][j].in_valid(valid_wire[i][j]);
                pe_inst[i][j].in_data(data_wire[i][j]);
                pe_inst[i][j].out_valid(valid_wire[i][j + 1]);
                pe_inst[i][j].out_data(data_wire[i][j + 1]);
            }
        }

    }

};