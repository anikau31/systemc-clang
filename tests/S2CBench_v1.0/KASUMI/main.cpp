//===========================================================================
//
// File Name    : main.cpp
// Description  : Kasumi  Top module
// Release Date : 23/07/2013 
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra
//
// Revision History
//---------------------------------------------------------------------------
// Date           Version   Author          Description
//---------------------------------------------------------------------------
//23/07/2013       1.0      PolyU DARC lab   Top system description for Kasumi
//
// g++ -o kasumi.exe main.cpp tb_kasumi.cpp kasumi.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -lm     
//===========================================================================

#include "kasumi.h"
#include "tb_kasumi.h"


int sc_main(int argc, char** argv)
{
  sc_clock                clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
  sc_signal<bool>         rst;
  sc_signal<sc_uint<8> >  indata[DEPTH][WIDTH];
  sc_signal<sc_uint<8> >  odata[DEPTH][WIDTH];
  sc_signal<sc_uint<8> >  k[8][2];

  int x, y;
  char name[15];


  kasumi u_KASUMI("KASUMI");
  test_kasumi u_TEST("test_KASUMI");

  //connect to Kasumi
  u_KASUMI.clk( clk );
  u_KASUMI.rst( rst );


  for(x=0; x < DEPTH; x++){
    for(y=0; y < WIDTH; y++){
      u_KASUMI.indata[x][y]( indata[x][y] );
      u_KASUMI.odata[x][y]( odata[x][y] );
    }
  }

  for(x=0; x < 8; x++){
    for(y=0; y < 2; y++){
      u_KASUMI.k[x][y]( k[x][y] );
    }
  }


  // connect to test bench
  u_TEST.clk( clk );
  u_TEST.rst( rst );
  
  for(x=0; x < DEPTH; x++){
    for(y=0; y < WIDTH; y++){
      u_TEST.indata[x][y]( indata[x][y] );
      u_TEST.odata[x][y]( odata[x][y] );
    }
  }

  for(x=0; x < 8; x++){
    for(y=0; y < 2; y++){
      u_TEST.k[x][y]( k[x][y] );
    }
  }
  sc_start();
    return 0;

};
