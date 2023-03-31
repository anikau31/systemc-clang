
#define __SYNTHESIS__

#if defined(__SYNTHESIS__)
#define RVD
#endif

#if !defined(RLEVEL)
#define RLEVEL false
#endif

#ifndef DATAW
#define DATAW 32 // bitstream data width
#endif

#include "divider.h"

sc_trace_file *tf;

// The module wrapper for divider
SC_MODULE(mymodule) 
{

  sc_clock clk; 
  sc_signal<bool> areset;

  sc_stream_in<data_t> dividend;
  sc_stream_in<data_t> divisor;

  sc_stream_out<data_t> quotient;


 
  divider<DATAW> u_dut;

  SC_CTOR(mymodule) : u_dut("u_dut")
        {
                // connect DUT
                u_dut.clk(clk);
                u_dut.arst(areset);

                u_dut.dividend(dividend);
                u_dut.divisor(divisor);
                u_dut.quotient(quotient);
        }
};

int sc_main(int argc , char *argv[])
{
  mymodule mymod("mymod");
}
