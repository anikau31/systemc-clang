/******************************************************************************
* Copyright (c) 2020, Intel Corporation. All rights reserved.
* 
* SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception.
* 
*****************************************************************************/

//#include "//assert.h"
#include <systemc.h>

// for loop with and w/o wait() general cases
class top : sc_module
{
public:
    sc_in_clk clk;
    sc_signal<bool> arstn{"arstn", 1};
    sc_signal<int> in{"in"};
    sc_signal<int> out{"out"};

    SC_HAS_PROCESS(top);
    top(sc_module_name)
    {
                
        SC_CTHREAD(nested_for_with_wait, clk.pos());
        async_reset_signal_is(arstn, false);
        // SC_CTHREAD(for_stmt_wait3, clk.pos());
        // async_reset_signal_is(arstn, false);
//
        /*
        SC_CTHREAD(for_stmt_no_wait4, clk.pos());
        async_reset_signal_is(arstn, false);

        
        SC_CTHREAD(for_stmt_wait0, clk.pos());
        async_reset_signal_is(arstn, false);
        
        SC_CTHREAD(for_stmt_wait1, clk.pos());
        async_reset_signal_is(arstn, false);
        
        SC_CTHREAD(for_stmt_wait2, clk.pos());
        async_reset_signal_is(arstn, false);
        
        SC_CTHREAD(for_stmt_wait3, clk.pos());
        async_reset_signal_is(arstn, false);

        SC_CTHREAD(for_stmt_wait4, clk.pos());
        async_reset_signal_is(arstn, false);

        SC_CTHREAD(for_stmt_wait_noiter, clk.pos());
        async_reset_signal_is(arstn, false);
        
        SC_CTHREAD(for_multi_wait1, clk.pos());
        async_reset_signal_is(arstn, false);
        
        SC_CTHREAD(for_multi_wait2, clk.pos());
        async_reset_signal_is(arstn, false);

        SC_CTHREAD(for_multi_wait3, clk.pos());
        async_reset_signal_is(arstn, false);
        */
    }

    void nested_for_with_wait()
    {
        int k = 0;
        wait();
        
        while (true) {
            for (int i = 0; i < 2; i++) {
                k = 1;
                wait();
                  k = 2;
                for (int j = 0; j < 3; j++) {
                    k = 3;
                    wait();     
                    k = 4;
                    wait();
                    k = 5;
                }
                k = 6;
                wait();
            }
            k = 7;
            wait();             
        }
    }
    

    // Double loops with break
    void for_stmt_wait3()
    {
        int k = 0;
        wait();
        
        while (true) {
            for (int i = 0; i < 2; i++) {
                k = 1;
                for (int j = 0; j < 3; j++) {
                    k = 2;
                    wait();     // 1
                }
       //         if (in) break;
            }
            k = 3;
            wait();             // 2
        }
    }
    
    /*
    // Double loops with continue
    void for_stmt_wait4()
    {
        int k = 0;
        wait();
        
        while (true) {
            for (int i = 0; i < 2; i++) {
                wait();
                
                if (in.read() == 42) continue;

                for (int j = 0; j < 3; j++) {
                    k++;
                }
            }
        }
    }
    
    // For with wait() no iteration
    void for_stmt_wait_noiter()
    {
        int k = 0;
        wait();
        
        while (true) {
            k = 1;
            for (int i = 0; i < 0; i++) {
                k = 2;
                wait();
            }
            k = 3;
            wait();
        }
    }
    
// ---------------------------------------------------------------------------
    
    void for_multi_wait1()
    {
        int k = 0;
        wait();
        
        while (true) {
            k = 1;
            for (int i = 0; i < 10; i++) {
                k = 2;
                wait();         // 1
                
                k = 3;
                wait();         // 2
                
                if (in) {
                    k = 4;
                    wait();     // 3
                }
            }
            wait();             // 4
        }
    }
    
    void for_multi_wait2()
    {
        int k = 0;
        wait();
        
        while (true) {
            k = 1;
            wait();             // 1
            
            while (!in) {
                wait();         // 2

                for (int i = 0; i <3; ++i) {
                    k = 2;
                    wait();     // 3
                }
            }
        }
    }    

    void for_multi_wait3()
    {
        int k = 0;
        wait();
        
        while (true) {
            k = 1;
            wait();             // 1
            
            do {
                for (int i = 0; i <3; ++i) {
                    k = 2;
                    wait();     // 2
                }

                wait();         // 3
                
                if (out) {
                    wait();     // 4
                    k = 3;
                }

            } while (in.read() != 42);
        }
    }     
    */
};

int sc_main(int argc, char *argv[])
{
    sc_clock clk{"clk", 10, SC_NS};
    top top_inst{"top_inst"};
    top_inst.clk(clk);
    
    sc_start(100, SC_NS);
    return 0;
}

