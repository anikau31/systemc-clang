#include "sreg.h"

int sc_main(int argc , char *argv[])
{
  sc_clock clk("clk", 10, SC_NS); // create a 10ns period clock signal
  sc_signal<bool> reset("reset");
  sreg<sc_int<8>, BYPASS>  my_sreg_driver("my_sreg_driver");
  my_sreg_driver.clk(clk);
  my_sreg_driver.reset(reset);
}
