//========================================================================================
// 
//
// File Name    : define.h
// Description  : Definition file for IDCT
// Release Date : 31/07/2013
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Author     Version     Description
//----------------------------------------------------------------------------------------
// 31/07/2013  PolyU       1.0       definition file
//=======================================================================================

#ifndef DEFINE_IDCT_H
#define DEFINE_IDCT_H

#include "systemc.h"

#include <iostream>
#include "stdio.h"

#define DCTSIZE 8
#define DCTSIZE2 64


#define INFILENAME             "idct_input.txt"   // random
#define INFILENAME_COEF        "idct_coef.txt"    // random
#define OUTFILENAME_GOLDEN     "idct_output_golden.txt"
#define OUTFILENAME            "idct_output.txt"


#define DIFFFILENAME           "idct_diff.txt"

//#define WAVE_DUMP          // set do dump waveform or set as compile option -DWAVE_DUMP
typedef short INT16;
typedef long  INT32;

#define ONE ((INT32) 1)
#define RIGHT_SHIFT(x,shft) ((x) >> (shft))
#define DESCALE(x,n) RIGHT_SHIFT((x) + (ONE << ((n)-1)), n)

#define MAXJSAMPLE 255
#define RANGE_MASK (MAXJSAMPLE * 4 + 3)

#define CONST_BITS  13
#define PASS1_BITS  2

#define FIX_0_298631336  ((INT32)  2446)	/* FIX(0.298631336) */
#define FIX_0_390180644  ((INT32)  3196)	/* FIX(0.390180644) */
#define FIX_0_541196100  ((INT32)  4433)	/* FIX(0.541196100) */
#define FIX_0_765366865  ((INT32)  6270)	/* FIX(0.765366865) */
#define FIX_0_899976223  ((INT32)  7373)	/* FIX(0.899976223) */
#define FIX_1_175875602  ((INT32)  9633)	/* FIX(1.175875602) */
#define FIX_1_501321110  ((INT32)  12299)	/* FIX(1.501321110) */
#define FIX_1_847759065  ((INT32)  15137)	/* FIX(1.847759065) */
#define FIX_1_961570560  ((INT32)  16069)	/* FIX(1.961570560) */
#define FIX_2_053119869  ((INT32)  16819)	/* FIX(2.053119869) */
#define FIX_2_562915447  ((INT32)  20995)	/* FIX(2.562915447) */
#define FIX_3_072711026  ((INT32)  25172)	/* FIX(3.072711026) */


#define MULTIPLY16C16(var,const)  (((INT16) (var)) * ((INT16) (const)))

#define MULTIPLY(var,const)  MULTIPLY16C16(var,const)

#define DEQUANTIZE(coef,quantval)  (((sc_int<32>) (coef)) * (quantval))


#endif  // end DEFINE_IDCT_H

