#pragma once

#include "systemc.h"

#include <iostream>

constexpr int clog2(int a) {
  return a == 1 ? 1 : 1 + clog2(a  / 2);
}

template<int WIDTH, int HEIGHT>
SC_MODULE(counter) {
  sc_in<bool> clk;
  sc_in<bool> reset;

  sc_in<bool> enable_row_count;
  sc_out<sc_uint<clog2(WIDTH)>> pixel_cntr;
  sc_out<sc_uint<clog2(HEIGHT)>> slice_cntr;

  void ms_proc_1() {
    if(reset.read()) {
        pixel_cntr.write(0);
    } else {
        if(pixel_cntr.read() == WIDTH - 1) {
            pixel_cntr.write(0);
        } else {
            pixel_cntr.write(pixel_cntr.read() + 1);
        }
    }
  }

  void ms_proc_2() {
    if(reset.read()) {
        slice_cntr = 0;
    } else {
        if(enable_row_count.read() && pixel_cntr.read() == WIDTH - 1) {
            if(slice_cntr.read() == HEIGHT - 1) {
                slice_cntr.write(0);
            } else {
                slice_cntr.write(slice_cntr.read() + 1);
            }
        }
    }
  }

#if defined(VCD)
  void start_of_simulation() {
  }
#endif

  SC_CTOR(counter) {

    SC_METHOD(ms_proc_1);
    sensitive << clk.pos();

    SC_METHOD(ms_proc_2);
    sensitive << clk.pos();
    dont_initialize();
  }

};

// mult := _Tin, _Tin -> _Tmult
// add  := _Tacc, _Tmult -> _Tacc
// TOOD: add operator overloading?
template<typename _Tin, typename _Tmult>
_Tmult mult(_Tin, _Tin);

template<typename _Tacc, typename _Tmult>
_Tacc add(_Tacc, _Tmult);


template<typename _Tin, typename _Tmult, typename _Tacc>
SC_MODULE(pe) {
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


  // SC_CTOR(pe) : ::sc_core::sc_module { "pe_inst" } {
  SC_HAS_PROCESS(pe);
  pe(const sc_core::sc_module_name& name) : ::sc_core::sc_module { name  } {
    SC_METHOD(ms_proc);
    sensitive << clk.pos();
    SC_METHOD(mc_proc);
    sensitive << in_a << in_b << out_sum << mult_op;
    dont_initialize();
  }
};

template<typename T, int pipes>
SC_MODULE(delay_pipe) {
  sc_in<bool> clk;
  sc_in<bool> rst;

  sc_in<T> in_p;
  sc_out<T> out_p;

  sc_signal<T> regs[pipes - 1];

  void ms_proc() {
    if(rst.read()) {
      regs[0].write(0);
      for(int i = 1; i < pipes; i++) {
        regs[i].write(0);
      }
    } else {
      regs[0].write(in_p.read());
      for(int i = 1; i < pipes; i++) {
        regs[i].write(regs[i - 1]);
      }
    }
  }


  SC_CTOR(delay_pipe) {
    SC_METHOD(ms_proc);
    sensitive << clk.pos();
    dont_initialize();
  }
};

template<typename T, typename Tacc, int N1, int N2, int M>
SC_MODULE(control) {

  sc_in<bool> clk;
  sc_in<bool> rst;

  sc_in<bool> enable_row_count;
  sc_out<sc_uint<clog2(M)>>          pixel_cntr_A;
  sc_out<sc_uint<clog2(M/N1)>>       slice_cntr_A;
  sc_out<sc_uint<clog2(M/N2)>>       pixel_cntr_B;
  sc_out<sc_uint<clog2(M)>>          slice_cntr_B;
  sc_out<sc_uint<clog2(M * M / N1)>> rd_addr_A;
  sc_out<sc_uint<clog2(M * M / N2)>> rd_addr_B;

  sc_signal<bool> const_true;

  void mc_proc() {
    rd_addr_A.write(slice_cntr_A.read() * M + pixel_cntr_A.read());
    rd_addr_B.write(pixel_cntr_B.read() * M + slice_cntr_B.read());
    const_true.write(true);
  }

  counter<M, M / N2> counter_B;
  counter<M, M / N1> counter_A;

  SC_CTOR(control) : counter_B { "counter_B" }, counter_A { "counter_A" } {
    SC_METHOD(mc_proc);
    sensitive << pixel_cntr_A << pixel_cntr_B << slice_cntr_A << slice_cntr_B;
    dont_initialize();

    counter_B.clk(clk);
    counter_B.reset(rst);
    counter_B.enable_row_count(const_true);
    counter_B.pixel_cntr(slice_cntr_B);
    counter_B.slice_cntr(pixel_cntr_B);

    counter_A.clk(clk);
    counter_A.reset(rst);
    counter_A.enable_row_count(enable_row_count);
    counter_A.pixel_cntr(pixel_cntr_A);
    counter_A.slice_cntr(slice_cntr_A);
  }
};

template<typename _Tin,  typename _Tacc, typename _Tmult, int N1, int N2, int M>
SC_MODULE(systolic) {

    control<_Tin, _Tacc, N1, N2, M> control_inst { "control_inst" };
    // pe<_Tin, _Tmult, _Tacc> pe_inst[N1][N2];
    // pe<_Tin, _Tmult, _Tacc>* pe_inst;
    sc_vector<pe<_Tin, _Tmult, _Tacc>> pe_inst {"pe_inst", N1 * N2} ;

    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_in<bool> enable_row_count_A;


    sc_out<typename decltype(control_inst.pixel_cntr_A)::data_type> pixel_cntr_A;
    sc_out<typename decltype(control_inst.slice_cntr_A)::data_type> slice_cntr_A;
    sc_out<typename decltype(control_inst.pixel_cntr_B)::data_type> pixel_cntr_B;
    sc_out<typename decltype(control_inst.slice_cntr_B)::data_type> slice_cntr_B;
    sc_out<typename decltype(control_inst.rd_addr_A)::data_type> rd_addr_A;
    sc_out<typename decltype(control_inst.rd_addr_B)::data_type> rd_addr_B;

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
            if(pixel_cntr_A.read() == M - 1 && patch.read() < (M * M) / (N1 * N2) ) {
                patch.write(patch.read() + 1);
                shift[0].write(1);
            } else {
                shift[0].write(0);
            }
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

    SC_CTOR(systolic) /* : pe_inst {"pe_inst", N1 * N2} */   {
        
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
        control_inst.clk(clk);
        control_inst.rst(rst);
        control_inst.enable_row_count(enable_row_count_A);
        control_inst.pixel_cntr_B(pixel_cntr_B);
        control_inst.slice_cntr_B(slice_cntr_B);
        control_inst.pixel_cntr_A(pixel_cntr_A);
        control_inst.slice_cntr_A(slice_cntr_A);
        control_inst.rd_addr_A(rd_addr_A);
        control_inst.rd_addr_B(rd_addr_B);

        // sc_vector
        for(int i = 0; i < N1 * N2; i++) {
            pe_inst[i].clk(clk);
            // remaining signals not supported due to complex index patterns
        }
        // 2D array
        /*
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
        */

    }

};