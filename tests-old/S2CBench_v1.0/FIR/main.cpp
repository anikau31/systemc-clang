//===========================================================================
//
// File Name    : main.cpp
// Description  : FIR top system description
// Release Date : 14/02/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
//
// Revision History
//---------------------------------------------------------------------------
// Date            Version   Author    Description
//---------------------------------------------------------------------------
//14/02/2013         1.0      PolyU     Top system declaration
//
// g++ -o filter.exe main.cpp tb_fir.cpp fir.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -lm
//===========================================================================

#include "fir.h"
#include "tb_fir.h"


int sc_main(int argc, char** argv)
{
  sc_clock                clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
  sc_signal<bool>         rst;
  sc_signal<sc_uint<8> >  in_data[FILTER_TAPS];
  sc_signal<sc_uint<16> > coeff[FILTER_TAPS];
 
  sc_signal<sc_uint<8> > filter_output; 
  int x;
  char name[10];


  fir u_FIR("FIR");
  test_FIR test("test_FIR");

  //connect to FIR
  u_FIR.clk( clk );
  u_FIR.rst( rst );

  for(x=0; x < FILTER_TAPS; x++){
    u_FIR.in_data[x]( in_data[x] );
    u_FIR.coeff[x]( coeff[x] );
  }
  u_FIR.filter_output( filter_output );

  // connect to test bench
  test.clk( clk );
  test.rst( rst );
  
  for(x=0; x < FILTER_TAPS; x++){
    test.in_data[x]( in_data[x] );
    test.coeff[x]( coeff[x] );
  }

  test.filter_output( filter_output );
  sc_start();
    return 0;

};
