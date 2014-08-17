//===========================================================================
//
// File Name    : disparity.h
// Description  : Disparity Estimator Top Module
// Release Date : 16/07/13
//
//
// Revision History
//---------------------------------------------------------------------------
// Date        Version   Author     Description
//---------------------------------------------------------------------------
//16/07/13          1.0             Top module of disparity estimator
//===========================================================================
#ifndef DISPARITY_TOP_H
#define DISPARITY_TOP_H

#include "pre_process.h"
#include "disparity.h"


SC_MODULE (disparity_top){
 
  // Inputs
  sc_in_clk clk;
  sc_in<bool>             rst;

  sc_in<bool>             VSynch;
  sc_in<bool>             HSynch;
  sc_in<sc_uint<16> >     Height;
  sc_in<sc_uint<16> >     Width;
  sc_in<sc_uint<8> >      Left_pixel_R;
  sc_in<sc_uint<8> >      Left_pixel_G;
  sc_in<sc_uint<8> >      Left_pixel_B;
  sc_in<sc_uint<8> >      Right_pixel_R;
  sc_in<sc_uint<8> >      Right_pixel_G;
  sc_in<sc_uint<8> >      Right_pixel_B;
  sc_in<bool>             valid_in;

  // Outputs
  sc_out<bool>            valid_out;
  sc_out<sc_uint<8> >     depth_out;

  sc_signal<bool>         VSynch_load;
  sc_signal<bool>         HSynch_load;
  sc_signal<sc_uint<16> > Height_load;
  sc_signal<sc_uint<16> > Width_load;
  sc_signal<sc_uint<8> >  Left_pixel_Y_load;
  sc_signal<sc_uint<8> >  Right_pixel_Y_load;
  sc_signal<bool>         valid_in_load;

  sc_signal<sc_uint<10> > hsize;
  sc_signal<sc_uint<10> > wsize;
  sc_signal<sc_uint<7> >  hd;
  sc_signal<sc_uint<7> >  wd;
  sc_signal<sc_uint<10> > hbnd;
  sc_signal<sc_uint<10> > wbnd;

  //Instance declaration
  pre_process *u_PRE;
  disparity *u_DISPARITY;

  SC_CTOR (disparity_top){

    u_PRE = new pre_process("PRE_PROCESS");
    u_DISPARITY = new disparity("DISPARITY");
    
    //connect to Instancies
    u_PRE->clk( clk );
    u_PRE->rst( rst );
    u_PRE->VSynch( VSynch );
    u_PRE->HSynch( HSynch );
    u_PRE->Height( Height );
    u_PRE->Width( Width );
    u_PRE->Left_pixel_R( Left_pixel_B );
    u_PRE->Left_pixel_G( Left_pixel_G );
    u_PRE->Left_pixel_B( Left_pixel_R );
    u_PRE->Right_pixel_R( Right_pixel_B );
    u_PRE->Right_pixel_G( Right_pixel_G );
    u_PRE->Right_pixel_B( Right_pixel_R );
    u_PRE->valid_in( valid_in );

    u_PRE->VSynch_load( VSynch_load );
    u_PRE->HSynch_load( HSynch_load );
    u_PRE->Height_load( Height_load );
    u_PRE->Width_load( Width_load );
    u_PRE->Left_pixel_Y_load( Left_pixel_Y_load );
    u_PRE->Right_pixel_Y_load( Right_pixel_Y_load );
    u_PRE->valid_in_load( valid_in_load );

    u_PRE->hsize(hsize);
    u_PRE->wsize(wsize);
    u_PRE->hd(hd);
    u_PRE->wd(wd);
    u_PRE->hbnd(hbnd);
    u_PRE->wbnd(wbnd);

    //connect to Dipsarity Estimator main kernel
    u_DISPARITY->clk( clk );
    u_DISPARITY->rst( rst );
    u_DISPARITY->VSynch( VSynch_load );
    u_DISPARITY->HSynch( HSynch_load );
    u_DISPARITY->Height( Height_load );
    u_DISPARITY->Width( Width_load );
    u_DISPARITY->Left_pixel_Y( Left_pixel_Y_load );
    u_DISPARITY->Right_pixel_Y( Right_pixel_Y_load );
    u_DISPARITY->valid_in( valid_in_load );
    u_DISPARITY->hsize(hsize);
    u_DISPARITY->wsize(wsize);
    u_DISPARITY->hd(hd);
    u_DISPARITY->wd(wd);
    u_DISPARITY->hbnd(hbnd);
    u_DISPARITY->wbnd(wbnd);
    u_DISPARITY->valid_out( valid_out );
    u_DISPARITY->depth_out( depth_out );
  }
};

#endif   // END DISPARITY_TOP_H
