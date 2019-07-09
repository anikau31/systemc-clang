//===========================================================================
//
//
// File Name    : main.cpp
// Description  : AES CIPJER  main
// Release Date : 24/07/2013 
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
//
// Revision History
//---------------------------------------------------------------------------
// Date             Author      Version     Description
//---------------------------------------------------------------------------
//14/02/2013        PolyU           1.0     Main system description
//
// g++ -o aes_cipher.exe main.cpp tb_aes_cipher_3G.cpp aes_cipher.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -lm
//===========================================================================

#include "aes_cipher.h"
#include "tb_aes_cipher.h"


int sc_main(int argc, char** argv)
{
  sc_clock                clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
  sc_signal<bool>         rst;
  sc_signal<sc_uint<8> >  in_data[SIZE];
  sc_signal<sc_uint<8> >  in_key[SIZE];
  sc_signal<sc_uint<8> >  out_data[SIZE];


  int x;
  char name[10];


  aes_cipher u_AES("AES_CIPHER");
  test_aes_cipher test("test_AES_CIPHER");

  //connect to AES CIPHER
  u_AES.clk( clk );
  u_AES.rst( rst );

  for(x=0; x < SIZE; x++){
    u_AES.idata[x]( in_data[x] );
    u_AES.ikey[x]( in_key[x] );
    u_AES.odata[x]( out_data[x] );
  }


  // connect to Test Bench
  test.clk( clk );
  test.rst( rst );

  for(x=0; x < SIZE; x++){
    test.idata[x]( in_data[x] );
    test.ikey[x]( in_key[x] );
    test.odata[x]( out_data[x] );
  }




  sc_start();

    return 0;

};





