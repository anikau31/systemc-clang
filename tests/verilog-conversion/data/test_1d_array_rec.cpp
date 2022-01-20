/******************************************************************************
* Copyright (c) 2020, Intel Corporation. All rights reserved.
* 
* SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception.
* 
*****************************************************************************/

#include "systemc.h"

using namespace sc_core;

// Local an member array of data types, signals and ports
class A : public sc_module {
public:
    

    sc_signal<sc_uint<5> > sig1[3];
    sc_signal<bool>     sig1b[3];
    sc_signal<int>      sig2[3][3];
    sc_signal<sc_uint<5> > sig2a[3][3];
    sc_signal<bool>     sig2b[3][3];
    
    sc_in<sc_uint<5> >* in_ports_p1[3];
    sc_out<sc_uint<5> >* out_ports_p1[3];
    sc_signal<int>*     sig_p1[3];
    sc_in<int>*         in_ports_p2[3][3];
    sc_out<int>*        out_ports_p2[3][3];
    sc_signal<int>*     sig_p2[3][3];
    sc_signal<sc_int<4> >*  sig_p2a[3][3];

    sc_signal<bool> dummy;

    SC_CTOR(A) {
        for (int i = 0; i < 3; i++) {
            in_ports_p1[i] = new sc_in<sc_uint<5> >("in_ports_p1");
            out_ports_p1[i] = new sc_out<sc_uint<5> >("out_ports_p1");
            sig_p1[i] = new sc_signal<int>("sig_p1");
            
            for (int j = 0; j < 3; j++) {
                in_ports_p2[i][j] = new sc_in<int>("in_ports_p2");
                out_ports_p2[i][j] = new sc_out<int>("out_ports_p2");
                sig_p2[i][j] = new sc_signal<int>("sig_p2");
                sig_p2a[i][j] = new sc_signal<sc_int<4> >("sig_p2a");
            }    
        }    


        SC_METHOD(array_record_1d); sensitive << sig;
        SC_METHOD(array_record_2d); sensitive << sig;
    }
    

    
    // Array of records
    struct Pair {int x; int y;};


    Pair pa_1d[2];

    void array_record_1d() 
    {
        pa_1d[sig].x = 0;
    }
    

    Pair pa[2][2];
    sc_signal<int> sig;

    void array_record_2d() 
    {
        pa[sig][sig].x = 0;
    }

};

class B_top : public sc_module {
public:
    sc_signal<int>          in_ports1[3];
    sc_signal<int>          in_ports1b[3];
    sc_signal<int>          in_ports2[3][3];
    sc_signal<int>          in_ports2b[3][3];
    
    sc_signal<bool>         out_ports1b[3];
    sc_signal<int>          out_ports1[3];
    sc_signal<int>          out_ports2[3][3];
    sc_signal<bool>         out_ports2b[3][3];

    sc_signal<sc_uint<5> >  in_ports_p1[3];
    sc_signal<int>          in_ports_p2[3][3];
    sc_signal<int>          out_ports_p2[3][3];
    
    A a_mod{"a_mod"};

    SC_CTOR(B_top) {
        for (int i = 0; i < 3; i++) {
            a_mod.in_ports1[i](in_ports1[i]);
            a_mod.in_ports1b[i](in_ports1b[i]);
            a_mod.out_ports1[i](out_ports1[i]);
            a_mod.out_ports1b[i](out_ports1b[i]);

            a_mod.in_ports_p1[i]->bind(in_ports_p1[i]);
            a_mod.out_ports_p1[i]->bind(in_ports_p1[i]);
            
            for (int j = 0; j < 3; j++) {
                a_mod.in_ports2[i][j](in_ports2[i][j]);
                a_mod.in_ports2b[i][j](in_ports2b[i][j]);
                a_mod.out_ports2[i][j](out_ports2[i][j]);
                a_mod.out_ports2b[i][j](out_ports2b[i][j]);
                
                a_mod.in_ports_p2[i][j]->bind(in_ports_p2[i][j]);
                a_mod.out_ports_p2[i][j]->bind(out_ports_p2[i][j]);
            }
        }
    }
};

int sc_main(int argc, char *argv[]) {
    B_top b_mod{"b_mod"};
    sc_start();
    return 0;
}

