//========================================================================================
// 
// File Name    : main.cpp
// Description  : Sobel filter top design implementation
// Release Date : 23/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version    Author            Description
//---------------------------------------------------------------------------------------
//23/07/2013      1.0     PolyU DARC Lab     Top system          
//
//// g++ -o sobel.exe main.cpp tb_sobel.cpp sobel.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -lm 
//=======================================================================================


#include "sobel.h"
#include "tb_sobel.h"

int sc_main(int argc, char* argv[]){

  sc_clock                clk("clk", 25, SC_NS, 0.5, 0, SC_NS, true);
  sc_signal<bool> rst;

  sc_signal<sc_uint<8> > input_row[3];
  sc_signal<sc_uint<8> > output_row;
  int i;
  char name[15];
  

  sobel s_obj("sobel");
  tb_sobel tb_obj("tb_sobel");
  s_obj.clk(clk);
  s_obj.rst(rst);


  
  for(i=0;i< 3; i++){
    s_obj.input_row[i](input_row[i]);
    }
 
  s_obj.output_row(output_row);

  

  tb_obj.clk(clk);
  tb_obj.rst(rst);

    for(i=0;i< 3; i++){
	tb_obj.input_row[i](input_row[i]);
	}

  tb_obj.output_row(output_row);
   sc_start();

  return 0;
}
