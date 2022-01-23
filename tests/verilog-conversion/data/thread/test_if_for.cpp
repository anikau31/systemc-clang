#include <systemc.h>

// if general cases
class A : sc_module
{
public:
    // sc_clock clk{"clk", 10, SC_NS};
    sc_in<bool> clk;
    sc_in<bool> arstn{"arstn"};
    sc_in<int> in{"in"};
    sc_out<int> out{"out"};

    static const bool   CONST_A = 1;
    
    enum {enumVal0 = 0, enumVal1 = 1};
    
    SC_HAS_PROCESS(A);
    A(sc_module_name)
    {
        // infinite loop
        SC_THREAD(if_stmt_wait_for2);
        sensitive << clk.posedge_event();
        async_reset_signal_is(arstn, false);
        
        // infinite loop
        SC_THREAD(if_stmt_wait_for2a);
        sensitive << clk.posedge_event();
        async_reset_signal_is(arstn, false);

        // infinite loop
        SC_THREAD(if_stmt_wait_for2b);
        sensitive << clk.posedge_event();
        async_reset_signal_is(arstn, false);

    }


    // IF statement with wait() inside of FOR without wait()
    void if_stmt_wait_for2()
    {
        int k = 0;
        wait();
        
        while (true) {                      // B8
                                            // B7
            for (int i = 0; i < 3; i++) {   // B6
                if (in.read()) {            // B5
                    k = 1;                  // B4
                    wait();  // 1
                }
                
                wait(); // 2                // B3
            }                               // B2
        }                                   // B1
    }
    
    void if_stmt_wait_for2a()
    {
        int k = 0;
        wait();
        
        while (true) {
        
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (in.read()) {
                        k = 1;
                        wait();  // 1
                    }

                    wait(); // 2
                }
            }
            
            wait();  // 3
        }
    }
    
    void if_stmt_wait_for2b()
    {
        int k = 0;
        wait();
        
        while (true) {
            
            for (int i = 0; i < 3; i++) {
                if (in.read()) {
                    k = 1;
                    wait();  // 1
                }
            
                for (int j = 0; j < 3; j++) {
                    if (out.read()) {
                        k = 2;
                    } else {
                        k = 3;
                        wait(); // 2
                    }
                    wait(); // 3
                }
                
                wait(); // 4
            }
        }
    }
    
};

class B_top : sc_module
{
public:
    sc_signal<bool> clk;
    sc_signal<bool> arstn{"arstn"};
    sc_signal<int> in{"in"};
    sc_signal<int> out{"out"};

    A a_mod{"a_mod"};

    SC_CTOR(B_top) {
      a_mod.in(in);
      a_mod.out(out);
      a_mod.arstn(arstn);
      a_mod.clk(clk);
    }
};

int sc_main(int argc, char *argv[])
{
    B_top top_inst{"top_inst"};
    sc_start(100, SC_NS);
    return 0;
}



