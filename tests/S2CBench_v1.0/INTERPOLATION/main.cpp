//========================================================================================
// 
// File Name    : main.cpp
// Description  : Main interpolation filter Testbench 
// Release Date : 23/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version   Author       Description
//----------------------------------------------------------------------------------------
//23/07/2013      1.0       PolyU      main testbench for interpolation filter
//=======================================================================================

#include "tb_interp.h"
#include "filter_interp.h"

int sc_main(int argc, char* argv[]){

  sc_clock clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
  sc_signal<bool> rst;

   sc_signal< sc_fixed<16,1,SC_TRN,SC_WRAP> > indata;
   sc_signal< sc_ufixed<9,0,SC_TRN,SC_WRAP> > infactor;
  
   sc_signal< bool > odata_en;
   sc_signal< sc_fixed<16,1,SC_RND,SC_SAT> > odata;

   interp interp_obj("interp");
   tb_interp interp_tbobj("tb_interp");

   interp_obj.clk(clk);
   interp_tbobj.clk(clk);

   interp_obj.rst(rst);
   interp_tbobj.rst(rst);


   interp_obj.infactor(infactor);
   interp_obj.indata(indata);
   interp_obj.odata(odata);
   interp_obj.odata_en(odata_en);


   interp_tbobj.infactor(infactor);
   interp_tbobj.indata(indata);
   interp_tbobj.odata(odata);
   interp_tbobj.odata_en(odata_en);
  sc_start();

  return 0;
}
