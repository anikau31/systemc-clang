//========================================================================================
// 
// File Name    : filter_decim.h
// Description  : Main 5 stages decimation filter component declaration
// Release Date : 31/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date       Version   Author      Description
//----------------------------------------------------------------------------------------
//31/07/2013      1.0    PolyU      Decimation filter module declaration
//=======================================================================================

#ifndef DECFILT_H_
#define DECFILT_H_

#define SC_INCLUDE_FX 
#include "define.h"


SC_MODULE(decfilt)
 {


public:
   
   // Inputs
   
   sc_in_clk clk;   
   sc_in<bool> rst;

   sc_in<bool> load_coeff;    
   sc_in < sc_fixed<19,2,SC_RND,SC_SAT> > indata;

   sc_in< sc_fixed<17,2,SC_RND,SC_SAT> > incoef1[TAPS_STAGE1] ;
   sc_in< sc_fixed<17,2,SC_RND,SC_SAT> > incoef2[TAPS_STAGE2] ;
   sc_in< sc_fixed<17,2,SC_RND,SC_SAT> > incoef3[TAPS_STAGE3] ;
   sc_in< sc_fixed<17,2,SC_RND,SC_SAT> > incoef4[TAPS_STAGE4] ;
   sc_in< sc_fixed<17,2,SC_RND,SC_SAT> > incoef5[TAPS_STAGE5] ;

   // Output   
   sc_out < sc_fixed<19, 2, SC_RND, SC_SAT> > odata;

   // Variables
   int stage_counter_1;   // decimation stages counter. HLS toold should 
   int stage_counter_2;   // adust bitwidth automatically
   int stage_counter_3;
   int stage_counter_4;
   int stage_counter_5;
   
   sc_fixed<19,2,SC_RND,SC_SAT> bufferline_1[7];
   sc_fixed<19,2,SC_RND,SC_SAT> bufferline_2[7];
   sc_fixed<19,2,SC_RND,SC_SAT> bufferline_3[7];
   sc_fixed<19,2,SC_RND,SC_SAT> bufferline_4[11];
   sc_fixed<19,2,SC_RND,SC_SAT> bufferline_5[24];


   // Functions declarations
   /*  R  */

void run() 
{

  // signals for STAGE 1
  sc_fixed<19,2,SC_TRN,SC_SAT> in_stage_1;
  sc_fixed<19,2,SC_TRN,SC_SAT> buffer_1;
  sc_fixed<36,4,SC_RND,SC_SAT> SoP1;
  sc_fixed<19, 2, SC_RND, SC_SAT> stage_1;
  bool stage_control_1 = false;

  // signals for STAGE 2
  sc_fixed<19,2,SC_RND,SC_SAT> in_stage_2;
  sc_fixed<19,2,SC_RND,SC_SAT> buffer_2;
  sc_fixed<36,4,SC_RND,SC_SAT> SoP2;
  sc_fixed<19, 2, SC_RND, SC_SAT> stage_2;
  bool stage_control_2 = false;

  // signals for STAGE 3
  sc_fixed<19,2,SC_RND,SC_SAT> in_stage_3;
  sc_fixed<19,2,SC_RND,SC_SAT> buffer_3;
  sc_fixed<36,4,SC_RND,SC_SAT> SoP3;
  sc_fixed<19, 2, SC_RND, SC_SAT> stage_3;
  bool stage_control_3 = false;

  // signals for STAGE 4
  sc_fixed<19,2,SC_RND,SC_SAT> in_stage_4;
  sc_fixed<19,2,SC_RND,SC_SAT> buffer_4;
  sc_fixed<36,4,SC_RND,SC_SAT> SoP4;
  sc_fixed<19, 2, SC_RND, SC_SAT> stage_4;
  bool stage_control_4 = false;

  // signals for STAGE 5
  sc_fixed<19,2,SC_RND,SC_SAT> in_stage_5;
  sc_fixed<36,4,SC_RND,SC_SAT> SoP5;
  sc_fixed<19, 2, SC_RND, SC_SAT> stage_5;
  bool stage_control_5 = false;

 stage_counter_1 = 0;
 stage_counter_2 = 0;
 stage_counter_3 = 0;
 stage_counter_4 = 0;
 stage_counter_5 = 0;

 unsigned int x=0;;

 sc_fixed<17,2,SC_RND,SC_SAT> incoef1_var[TAPS_STAGE1] ;
 sc_fixed<17,2,SC_RND,SC_SAT> incoef2_var[TAPS_STAGE2] ;
 sc_fixed<17,2,SC_RND,SC_SAT> incoef3_var[TAPS_STAGE3];
 sc_fixed<17,2,SC_RND,SC_SAT> incoef4_var[TAPS_STAGE4] ;
 sc_fixed<17,2,SC_RND,SC_SAT> incoef5_var[TAPS_STAGE5];

 wait();
 
 // Main stage filter body
 
 while( true )
 {

   if(load_coeff.read()){
     // Read coefficients into coef arrays
     for (x = 0; x < TAPS_STAGE1; x++) {
       incoef1_var[x] = incoef1[x].read();

     }
     for (x = 0; x < TAPS_STAGE2; x++) {
       incoef2_var[x] = incoef2[x].read();

     }
     for (x = 0; x < TAPS_STAGE3; x++) {
       incoef3_var[x] = incoef3[x].read();

     }
     for (x = 0; x < TAPS_STAGE4; x++) {
       incoef4_var[x] = incoef4[x].read();

     }
     for (x = 0; x < TAPS_STAGE5; x++) {
       incoef5_var[x] = incoef5[x].read();

     }
   }

  // Read filter in_stage data 
  in_stage_1 = indata.read();

  //================================
  // Stage 1
  //================================

      
  // Shift data and store new data
  for( x = BUFFER_SIZE_STAGE1-1; x > 0; x--) {
   bufferline_1[x] = bufferline_1[x-1];
  }
  bufferline_1[0] = in_stage_1;

  SoP1 = 0;
  stage_control_1 = false;

  for (x = 0; x < TAPS_STAGE1; x++) {
    if( x == TAPS_STAGE1-1 )
      buffer_1 = 0;
    else 
      buffer_1 = bufferline_1[ BUFFER_SIZE_STAGE1-1 - x];
  

   SoP1 = SoP1 + (bufferline_1[x]+buffer_1 )*incoef1_var[x];
  }
  
  stage_1 = sc_fixed<19, 2, SC_RND, SC_SAT>(SoP1);

  
  if(stage_counter_1 == 2) {
    stage_counter_1 = 0;
    stage_control_1 = true;
  }
  else {
    stage_counter_1++;
  }

  //================================
  // Stage 2
  //================================
  if (stage_control_1)
    in_stage_2 = stage_1;

  for(x =  BUFFER_SIZE_STAGE2-1; x > 0; x--) {
    bufferline_2[x] = bufferline_2[x-1];
  }
  bufferline_2[0] = in_stage_2;

  SoP2 = 0;
  stage_control_2 = false;


  if(stage_control_1){

    for (x = 0; x < TAPS_STAGE2; x++) {
      if( x == TAPS_STAGE2-1 )
	buffer_2 = 0;
      else
	buffer_2 = bufferline_2[ BUFFER_SIZE_STAGE2-1 - x];
      
      SoP2 = SoP2 + (bufferline_2[x]+buffer_2 )*incoef2_var[x];
    }


  stage_2 = sc_fixed<19, 2, SC_RND, SC_SAT>(SoP2);

  if(stage_counter_2 == 2){
    stage_counter_2 = 0;
    stage_control_2 = true;
  }
  else {
   stage_counter_2++;
  }
}
  
  //================================
  // Stage 3
  //================================
  if (stage_control_2)
      in_stage_3 = stage_2;
	  
  for(x = BUFFER_SIZE_STAGE3-1; x > 0; x--) {
    bufferline_3[x] = bufferline_3[x-1];
  }
  bufferline_3[0] = in_stage_3;

  SoP3 = 0;
  stage_control_3 = false;

  if(stage_control_2){

    for (x = 0; x < TAPS_STAGE3; x++) {
      if( x == TAPS_STAGE3-1 )
	buffer_3 = 0;
      else 
	buffer_3 = bufferline_3[BUFFER_SIZE_STAGE3-1 - x];
      SoP3 = SoP3 + (bufferline_3[x]+buffer_3)*incoef3_var[x];
    }
  


  stage_3 = sc_fixed<19, 2, SC_RND, SC_SAT>(SoP3);

  if(stage_counter_3 == 1) { 
   	stage_counter_3 = 0;
   	stage_control_3 = true;
  } else {
   	stage_counter_3++;
  }
}

  //================================
  // Stage 4
  //================================
  if (stage_control_3)
   in_stage_4 = stage_3;
 
  for(x = BUFFER_SIZE_STAGE4-1; x > 0; x--) {
   bufferline_4[x] = bufferline_4[x-1];
  }
  bufferline_4[0] = in_stage_4;

  SoP4 = 0;
  stage_control_4= false;

if(stage_control_3){

  for (x = 0; x < TAPS_STAGE4; x++) {
   if( x == TAPS_STAGE4-1 )
     buffer_4 = 0;
   else
     buffer_4 = bufferline_4[BUFFER_SIZE_STAGE4-1 - x];

   SoP4 = SoP4 + (bufferline_4[x]+buffer_4)*incoef4_var[x];
  }
  
  stage_4 = sc_fixed<19, 2, SC_RND, SC_SAT>(SoP4);
 
  if(stage_counter_4 == 1) {
    stage_counter_4 = 0;
    stage_control_4 = true;
  }
  else {
   stage_counter_4++;
  }
}
 
  //================================
  // Stage 5
  //================================
  
  if (stage_control_4)
   in_stage_5 = stage_4;
 
  for(x = BUFFER_SIZE_STAGE5-1; x > 0; x--) {
   bufferline_5[x] = bufferline_5[x-1];
  }
  bufferline_5[0] = in_stage_5;

  SoP5 = 0;
  stage_control_5 = false;


if(stage_control_4){
  for (x = 0; x < TAPS_STAGE5; x++) {
    SoP5 = SoP5 + (bufferline_5[x]+bufferline_5[ BUFFER_SIZE_STAGE5-1 - x])*incoef5_var[x];
  }


  stage_5 = sc_fixed<19, 2, SC_RND, SC_SAT>(SoP5);

  if(stage_counter_5 == 1) {
    stage_counter_5 = 0;
    stage_control_5 = true;
  }
  else {
    stage_counter_5++;
  }
}


// Write decimation filter output
 if(stage_control_5 == true){
   odata.write(stage_5);
   stage_control_5 = false;
 }	

 wait(); 
 }

}

 SC_CTOR (decfilt) {
   SC_CTHREAD (run, clk.pos()); 
   reset_signal_is(rst, false);
 };
 

   ~decfilt() {};


};

#endif   // END DECFILT_
