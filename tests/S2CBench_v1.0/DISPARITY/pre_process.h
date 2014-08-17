//=================================================================================
//
// File Name    : pre_process.h
// Description  : RGB to YUV conversion and parameter computation
// Release Date : 24/07/13
// Author       : 
//
// Revision History
//---------------------------------------------------------------------------
// Date       Version    Author     Description
//---------------------------------------------------------------------------
//24/07/2013     1.0                 RGB to YUV conversion and parameter compuation
//================================================================================

#ifndef PRE_PROCESS_
#define PRE_PROCESS_

#include <systemc.h>
#include "define.h"
SC_MODULE (pre_process) {

  // inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in<bool>          VSynch;
  sc_in<bool>          HSynch;
  sc_in<sc_uint<16> >  Height;
  sc_in<sc_uint<16> >  Width;
  sc_in<sc_uint<8> >   Left_pixel_R;
  sc_in<sc_uint<8> >   Left_pixel_G;
  sc_in<sc_uint<8> >   Left_pixel_B;
  sc_in<sc_uint<8> >   Right_pixel_R;
  sc_in<sc_uint<8> >   Right_pixel_G;
  sc_in<sc_uint<8> >   Right_pixel_B;
  sc_in<bool>          valid_in;


  // outputs
  sc_out<bool>          VSynch_load;
  sc_out<bool>          HSynch_load;
  sc_out<sc_uint<16> >  Height_load;
  sc_out<sc_uint<16> >  Width_load;
  sc_out<sc_uint<8> >   Left_pixel_Y_load;
  sc_out<sc_uint<8> >   Right_pixel_Y_load;
  sc_out<bool>          valid_in_load;

  sc_out<sc_uint<10> > hsize;
  sc_out<sc_uint<10> > wsize;
  sc_out<sc_uint<7> >  hd;
  sc_out<sc_uint<7> >  wd;
  sc_out<sc_uint<10> > hbnd;
  sc_out<sc_uint<10> > wbnd;


  // Variables declaration
  sc_signal<bool>      VSynch_read;


  // Member functions declaration

void RGBtoYUV()
{

  // Variables declaration

  // Coefficients converted to integers (ref.:http://en.wikipedia.org/wiki/YUV)
  int rgb2yuv_coeff[3][3] = {{299, 587, 114},
			     {-147, -289, 436},
			     {615, -515, -100}};


 int rgb2yuv_offset[3] = {18, 132, 132};

 int Left_R;
 int Left_G;
 int Left_B;

 int Right_R;
 int Right_G;
 int Right_B;

 int Left_tmp;
 int Right_tmp;

  //Reset
  VSynch_load.write(true);
  HSynch_load.write(true);
  Height_load.write(0);
  Width_load.write(0);
  Left_pixel_Y_load.write(0);
  Right_pixel_Y_load.write(0);
  valid_in_load.write(false);

  wait();

  while(1){
 

    Left_R=Left_pixel_R.read();
    Left_B=Left_pixel_B.read();
    Left_G=Left_pixel_G.read();

    Right_R=Right_pixel_R.read();
    Right_G=Right_pixel_G.read();
    Right_B=Right_pixel_B.read();

    
    Left_tmp = (int)(( rgb2yuv_coeff[0][0]*(int)(Left_B) 
			+ rgb2yuv_coeff[0][1]*(int)(Left_G)
			+ rgb2yuv_coeff[0][2]*(int)(Left_R))/1024 
		        + rgb2yuv_offset[0]);

    Right_tmp = (int)(( rgb2yuv_coeff[0][0]*(int)(Right_B)
			+ rgb2yuv_coeff[0][1]*(int)(Right_G)
			+ rgb2yuv_coeff[0][2]*(int)(Right_R))/1024
		        + rgb2yuv_offset[0]);
   
 
    if(valid_in.read()==true){
      Left_pixel_Y_load.write(Left_tmp);
      Right_pixel_Y_load.write(Right_tmp);
      valid_in_load.write(true);
    }
    else{
      valid_in_load.write(false);
    }
    
    VSynch_load.write(VSynch.read());
    HSynch_load.write(HSynch.read());
    Height_load.write(Height);
    Width_load.write(Width);
    
    wait();
  }
}

/*
** Generate static parameters for the disparty estimator's main fumnction
*/
void parameters()
{

  
  // Variables declaration
  int hsize_tmp;    
  int hd_tmp;        
  int hbnd_tmp;     
  int wsize_tmp;    
  int wd_tmp;       
  int wbnd_tmp;    
  int height;
  int width;

  //Reset 
  VSynch_read=false;
  hsize.write(0);
  wsize.write(0);
  hd.write(0);
  wd.write(0);
  hbnd.write(0);
  wbnd.write(0);
 
  wait();

  while(true){
  
    height = Height.read();
    width  = Width.read();

    VSynch_read   = VSynch;


    if(height==0){
      hsize_tmp = 0;    
      hd_tmp    = 0;    
      hbnd_tmp  = 0;    
    }
    else{
      if(height%HEIGHT == 0)
	hsize_tmp =  height/HEIGHT;
      else
	hsize_tmp = (height/HEIGHT)+1;

      if(height%HEIGHT==0)
	hd_tmp = HEIGHT;
      else
	hd_tmp = HEIGHT-1;
             
      hbnd_tmp  = ((height - hd_tmp*hsize_tmp)/2) + (((height - hd_tmp*hsize_tmp)/2)%2);
    }
    


    if(width==0){
      wsize_tmp = 0;     
      wd_tmp    = 0;    
      wbnd_tmp  = 0;    
    }
    else{
      if(width%WIDTH==0)
	wsize_tmp = width/WIDTH;
      else
	wsize_tmp = (width/WIDTH)+1; 
 
      wd_tmp = WIDTH;
                   
      wbnd_tmp  = ((width  - wd_tmp*wsize_tmp)/2) + (((width  - wd_tmp*wsize_tmp)/2)%2);
    }
    
    if((VSynch.read()==false)&&(VSynch_read==true)){
      hsize.write(hsize_tmp);
      wsize.write(wsize_tmp);
      hd.write(hd_tmp);
      wd.write(wd_tmp);
      hbnd.write(hbnd_tmp);
      wbnd.write(wbnd_tmp);
    }

    wait();
  }   //end while

}

  SC_CTOR (pre_process){
    
    SC_CTHREAD(RGBtoYUV,clk.pos());     
    reset_signal_is(rst,false);
 
    SC_CTHREAD(parameters,clk.pos());     
    reset_signal_is(rst,false);

  };


  ~pre_process(){};


};





#endif // PRE_PROCESS_
