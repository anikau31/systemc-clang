//========================================================================================
// 
// File Name    : fir.h
// Description  : FIR filter module declaration
// Release Date : 14/02/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
// 
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version    Author      Description
//---------------------------------------------------------------------------------------
//14/02/2013      1.0       PolyU     FIR filter module declaration
//=======================================================================================

#ifndef FIR_H_
#define FIR_H_


#include "define.h"


SC_MODULE (fir) {


public:

   // Inputs
   sc_in_clk clk;
   sc_in<bool> rst;
   sc_in<sc_uint<8> > in_data[FILTER_TAPS] ;
   sc_in<sc_uint<16> > coeff[FILTER_TAPS] ;

   // Output
   sc_out< sc_uint<8> > filter_output ;


   /* F */

void fir_main ( void ) {

   // Variables declaration
    sc_uint<8> filter_output_function ; 
    sc_uint<8> in_data_read[9];
    sc_uint<16> coeff_read[9] ;
    int i;

    // Reset state - should be executable in 1 clock cycle
     
     wait();  
 

   // Main thread	
   while (1) {

   	 for(i=0;i<FILTER_TAPS-1;i++){
	   in_data_read[i] = in_data[i].read();
      	   coeff_read[i] = coeff[i].read();
   	}

	 // Filter function
   	 filter_output_function = filter(in_data_read, coeff_read);


	 filter_output.write(filter_output_function) ;
	 wait();
}


}

// Filter function
sc_uint<8> filter(
    sc_uint<8>  *ary, 
    sc_uint<16>  *coeff)
{
    int sop=0;
    sc_uint <8> filter_result ;
    int i ;


    // Sum of product (SOP) generation 
    for(i=0;i<FILTER_TAPS-1;i++){
        sop += ary[i] * coeff[i] ;
    } 

    // Sign adjustment and rounding to sc_unit <8>)
    if ( sop < 0 ){
        sop = 0 ;
    }

    filter_result = sc_uint<8>(sop);

    return filter_result;
}



   // Constructor
 SC_CTOR (fir) {

       SC_CTHREAD (fir_main, clk.pos() );
       reset_signal_is(rst, false) ;
       sensitive << clk.pos();

   }

   // Destructor
   ~fir() {}


};


#endif   //  FIR_H_

