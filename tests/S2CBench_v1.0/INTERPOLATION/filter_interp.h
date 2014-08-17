//========================================================================================
// 
// File Name    : filter_interp.h
// Description  : Main interpolation filter component declaration
// Release Date : 23/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra
//
// Revision History
//---------------------------------------------------------------------------------------
// Date       Version   Author      Description
//----------------------------------------------------------------------------------------
//23/07/2013   1.0      PolyU       Interpolation filter declaration file
//=======================================================================================

#ifndef INTERP_H_
#define INTERP_H_

#define SC_INCLUDE_FX

#include "define.h"


SC_MODULE (interp) {

public:
   
  // Inputs
   sc_in_clk clk;
   sc_in<bool> rst;
   
   // Inputs
   sc_in< sc_fixed<16,1,SC_TRN,SC_WRAP> > indata;
   sc_in< sc_ufixed<9,0,SC_TRN,SC_WRAP> > infactor;
  

   // Output   
   sc_out< bool > odata_en;
   sc_out< sc_fixed<16,1,SC_RND,SC_SAT> > odata;

   // Variables
   sc_fixed<16,1,SC_TRN,SC_WRAP> buffer[TAPS] ;

  sc_fixed<16,1,SC_TRN,SC_WRAP> indata_read;
  sc_ufixed<9,0,SC_TRN,SC_WRAP> infactor_read;
  sc_fixed<16,1,SC_RND,SC_SAT> odata_write;


   sc_fixed<36,6,SC_TRN,SC_WRAP> SoP1;
   sc_fixed<36,6,SC_TRN,SC_WRAP> SoP2;
   sc_fixed<36,6,SC_TRN,SC_WRAP> SoP3;
   sc_fixed<36,6,SC_TRN,SC_WRAP> SoP4;

  // Functions declarations


   /* R */

void run ( void ) {


  unsigned int n;

   const sc_fixed<17,2,SC_TRN,SC_WRAP> coeff1[TAPS] = {0.002561105374,
						       -0.0374560342,
						       0.261339038123,
						       0.236616189234,
						       -0.0175371872,
						       0.003662109373,
						       0.040705008125,
						       -0.0063781875};

   const sc_fixed<17,2,SC_TRN,SC_WRAP> coeff2[TAPS] = {-0.0012220704128,
						       -0.00992526286873,
						        0.28326416015625,
						       -0.38756410156234,
						        0.01092529296875,
						        0.0111770703125,
						       -0.01092529296875,
						       0.28326416015625};

   const sc_fixed<17,2,SC_TRN,SC_WRAP> coeff3[TAPS] = {-0.0254359140667,
						       0.12369433597894,
						       -0.08766552334675,
						       -0.08944552734377,
						       0.4443443359323,
						       -0.0172119140625,
						       0.1116943359375,
						       0.2222743352321};

   const sc_fixed<17,2,SC_TRN,SC_WRAP> coeff4[TAPS] = {-0.022432453112228,
						        0.056155029296845,
						       -0.562341259765625,
						        0.34291256765612,
						       -0.078155029296811,
						        0.003434129453125,
						        0.045560371453428,
						        0.00014556453135};
   odata_en = 0;
   odata = 0;

   wait();

   while (1) {

      // Read inputs
      indata_read = indata.read();
      infactor_read = infactor.read();
      

      odata_en.write(0);

    // Read inputs by shifting previous data
    for ( n = 5; n>0; n--)
         buffer[n] = buffer[n-1];
      buffer[0] = indata_read;
  
   
      // FIR 1 : Sum of Products of 1st filter
      SoP1 = 0;
      for (n = 0; n < TAPS; n++ ) 
	SoP1 = SoP1 + buffer[n] * coeff1[n];
      
      // FIR 2 : Sum of Products of 2nd filter
      SoP2 = 0;
      for (n = 0; n < TAPS; n++ ) 
	SoP2 =SoP2 + buffer[n] * coeff2[n];

      // FIR 3 : Sum of Products of 3rd filter
      SoP3 = 0;
      for ( n = 0; n < TAPS; n++ ) 
	SoP3 = SoP3 + buffer[n] * coeff3[n];

      // FIR 4 : Sum of Products of 4th filter
      SoP4 = 0;
      for (n = 0; n < TAPS; n++ ) 
	SoP4 = SoP4 + buffer[n] * coeff4[n];


   // Output computation 
   odata_write = SoP1 +
               SoP2 * infactor_read +
               SoP3 * infactor_read*infactor_read + 
               SoP4 * infactor_read*infactor*infactor_read;

   // Manual polynomial decomposition (reduces the number of arithmetic operations -> leads to smaller design)
//	tmp1 = SoP4*infactor_read + SoP3;
//	tmp2 = tmp1 * infactor_read + SoP2;
//    	odata_write = tmp2 * infactor_read + SoP1;


     // Write results back
      odata_en.write( 1 );
      odata.write( odata_write );
      
      wait();
   }
}

  
   SC_CTOR (interp) {
      SC_CTHREAD (run, clk.pos());
      reset_signal_is (rst, false );
   }

   ~interp() {}


}; 

#endif //  INTERP_H_
