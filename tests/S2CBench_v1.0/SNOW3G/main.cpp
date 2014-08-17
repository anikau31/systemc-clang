//===========================================================================
//
//
// File Name    : main.cpp
// Description  : SNOW 3G  testbench
// Release Date : 24/07/2013 
//
// Revision History
//---------------------------------------------------------------------------
// Date             Author      Version     Description
//---------------------------------------------------------------------------
//14/02/2013        PolyU            01
//
//// g++ -o snow_3G.exe main.cpp tb_snow_3G.cpp fir.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -lm
//===========================================================================

#include "snow_3G.h"
#include "tb_snow_3G.h"


int sc_main(int argc, char** argv)
{
  sc_clock                clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
  sc_signal<bool>         rst;
  sc_signal<sc_uint<32> >  in_data[SIZE];
  sc_signal<sc_uint<32> >  out_data[SIZE];

  int x;
  char name[10];


  snow_3G u_SNOW("SNOW");
  test_snow_3G test("test_SNOW");

  //connect to SNOW 3G
  u_SNOW.clk( clk );
  u_SNOW.rst( rst );

  for(x=0; x < SIZE; x++){
    u_SNOW.ks_in[x]( in_data[x] );
    u_SNOW.ks_out[x]( out_data[x] );
  }


  // connect to test bench
  test.clk( clk );
  test.rst( rst );
  
  for(x=0; x < SIZE; x++){
    test.ks_in[x]( in_data[x] );
    test.ks_out[x]( out_data[x] );
  }


  sc_start();

    return 0;

};





