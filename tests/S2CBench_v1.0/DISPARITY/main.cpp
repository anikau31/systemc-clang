//===========================================================================
// 
//
// File Name    : main.cpp
// Description  : Real 3D disparity estimation
// Release Date : 16/07/13
// Author       : 
//
// Revision History
//---------------------------------------------------------------------------
// Date         Version   Author    Description
//---------------------------------------------------------------------------
//16/07/13         1.0              top level main
///===========================================================================

#include "disparity_top.h"
#include "tb_disparity.h"
#include "pre_process.h"
#include "disparity.h"
#include "define.h"

int sc_main(int argc, char** argv)
{
  sc_clock                clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
  sc_signal<bool>         rst;
  sc_signal<bool>         VSynch;
  sc_signal<bool>         HSynch;
  sc_signal<sc_uint<16> > Height;
  sc_signal<sc_uint<16> > Width;
  sc_signal<sc_uint<8> >  Left_pixel_R;
  sc_signal<sc_uint<8> >  Left_pixel_G;
  sc_signal<sc_uint<8> >  Left_pixel_B;
  sc_signal<sc_uint<8> >  Right_pixel_R;
  sc_signal<sc_uint<8> >  Right_pixel_G;
  sc_signal<sc_uint<8> >  Right_pixel_B;
  sc_signal<bool>         valid_in;
  sc_signal<bool>         valid_out;

  sc_signal<sc_uint<8> >  depth_out;  
  disparity_top u_DISPARITY_TOP("DISPARITY");
  test_disparity u_test("test_DE");

  // Instantiate Disparity Estimator unit
  u_DISPARITY_TOP.clk( clk );
  u_DISPARITY_TOP.rst( rst );
  u_DISPARITY_TOP.VSynch( VSynch );
  u_DISPARITY_TOP.HSynch( HSynch );
  u_DISPARITY_TOP.Height( Height );
  u_DISPARITY_TOP.Width( Width );
  u_DISPARITY_TOP.Left_pixel_R( Left_pixel_B );
  u_DISPARITY_TOP.Left_pixel_G( Left_pixel_G );
  u_DISPARITY_TOP.Left_pixel_B( Left_pixel_R );
  u_DISPARITY_TOP.Right_pixel_R( Right_pixel_B );
  u_DISPARITY_TOP.Right_pixel_G( Right_pixel_G );
  u_DISPARITY_TOP.Right_pixel_B( Right_pixel_R );
  u_DISPARITY_TOP.valid_in( valid_in );
  u_DISPARITY_TOP.valid_out( valid_out );
  u_DISPARITY_TOP.depth_out( depth_out );

  // Instantiate TB
  u_test.clk( clk );
  u_test.rst( rst );
  u_test.VSynch( VSynch );
  u_test.HSynch( HSynch );
  u_test.Height( Height );
  u_test.Width( Width );
  u_test.Left_pixel_R( Left_pixel_B );
  u_test.Left_pixel_G( Left_pixel_G );
  u_test.Left_pixel_B( Left_pixel_R );
  u_test.Right_pixel_R( Right_pixel_B );
  u_test.Right_pixel_G( Right_pixel_G );
  u_test.Right_pixel_B( Right_pixel_R );
  u_test.valid_in( valid_in );
  u_test.valid_out( valid_out );
  u_test.depth_out( depth_out );


  sc_start();
  return 0;
};
