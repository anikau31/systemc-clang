
/**
 * Streaming moving average calculation
 */

#define __SYNTHESIS__

#if defined(__SYNTHESIS__)
#define RVD
#endif

#if !defined(RLEVEL)
#define RLEVEL false
#endif

#ifndef DATAW
#define DATAW 16 // bitstream data width
#endif

#include "moving-average.h"

sc_trace_file *tf;

// The module wrapper for moving_average
SC_MODULE(mymodule) 
{

  sc_clock clk; 
  sc_signal<bool> reset;

  sc_stream <data_t> driv_data;

  sc_stream <data_t>  dut_min;
  sc_stream <data_t>  dut_max;
  sc_stream <data_t>  dut_avg;

 
  moving_average<DATAW> u_dut;

  SC_CTOR(mymodule) : u_dut("u_dut")
        {
                // connect DUT
                u_dut.clk(clk);
                u_dut.reset(reset);

                u_dut.datastrm   (driv_data);
                u_dut.min_out   (dut_min);
                u_dut.max_out   (dut_max);
                u_dut.avg_out   (dut_avg);
        }
};

int sc_main(int argc , char *argv[])
{
  mymodule mymod("mymod");
}
