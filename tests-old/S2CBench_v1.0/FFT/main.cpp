//========================================================================================
// 
// File Name    : main.cpp
// Description  : Testbench for FFT
// Release Date : 16/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date       Version    Author              Description
//----------------------------------------------------------------------------------------
//16/07/2013    1.0     PolyU DARC Lab        Main FFT testbench top module                 
//
// g++ -o fft.exe main.cpp tb_fft.cpp fft.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -lm
//=======================================================================================


#include "fft.h"
#include "tb_fft.h"


int sc_main(int argc, char** argv)
{
  sc_clock                clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
  sc_signal<bool>         rst;
 
  sc_signal<sc_fixed<32,16, SC_TRN, SC_WRAP> > in_real;   
  sc_signal<sc_fixed<32,16, SC_TRN, SC_WRAP> > in_imag;  
  sc_signal<sc_fixed<32,16, SC_TRN, SC_WRAP> > out_real;        
  sc_signal<sc_fixed<32,16, SC_TRN, SC_WRAP> > out_imag;  
  sc_signal<bool> data_valid; 
  sc_signal<bool> data_ack;   
  sc_signal<bool> data_req;         
  sc_signal<bool> data_ready;  
     

   fft u_FFT("FFT");
   test_fft test("test_FFT");

   //connect to bubble sort
   u_FFT.clk( clk );
   u_FFT.rst( rst );
   u_FFT.in_real( in_real);
   u_FFT.in_imag( in_imag);
   u_FFT.data_valid( data_valid );
   u_FFT.data_ack( data_ack );

   u_FFT.out_real( out_real);
   u_FFT.out_imag( out_imag);
   u_FFT.data_req( data_req);
   u_FFT.data_ready( data_ready);
 
  // connect to test bench
  test.clk( clk );
  test.rst( rst );
  test.in_real( in_real);
  test.in_imag( in_imag);
  test.data_valid( data_valid );
  test.data_ack( data_ack );

  test.out_real( out_real);
  test.out_imag( out_imag);
  test.data_req( data_req);
  test.data_ready( data_ready);  

  sc_start();
    return 0;

};
