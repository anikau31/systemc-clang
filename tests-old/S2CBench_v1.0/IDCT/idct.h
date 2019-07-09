//========================================================================================
// 
//
// File Name    : idct.h
// Description  : IDCT
// Release Date : 31/07/2013
//
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Author      Version      Description
//----------------------------------------------------------------------------------------
//  1991-1998  Thomas G. Lange 1.0      Origiinal IDCT implementation    
//  31/07/2013  PolyU          1.1      Converted into Synthesizable SystemC                 
//
//=======================================================================================


#ifndef IDCT_H_
#define IDCT_H_

#include "define.h"

SC_MODULE( idct ) {

  typedef int        workspace_t;

  // Inputs
  sc_in_clk              clk;
  sc_in< bool >          rst;

  sc_in<  bool >         input_ready;
  sc_in< sc_int<16>  >   input_coef;
  sc_in< sc_uint<6>  >   input_quant;

  // Outputs
  sc_out< bool >         output_start;
  sc_out< sc_uint<8>  >  output_sample;
  sc_out< sc_uint<3> >   output_row;
  sc_out< sc_uint<3> >   output_col;
  
 
void jpeg_idct_islow()
{
  INT32 tmp0, tmp1, tmp2, tmp3;
  INT32 tmp10, tmp11, tmp12, tmp13;
  INT32 z1, z2, z3, z4, z5;
  int ctr;

  sc_int<16>      inptr[8];
  sc_uint<6>   quantptr[8];


  workspace_t workspace[ DCTSIZE2 ];

  static const sc_uint<8> range_limit[ 1024 ] = {
#   include "range_limit.dat"
  };

  /* Pass 1: process columns from input, store into work array. */
  /* Note results are scaled up by sqrt(8) compared to a true IDCT; */
  /* furthermore, we scale the results by 2**PASS1_BITS. */

  workspace_t* wsptr = workspace;
 
  for (ctr = DCTSIZE; ctr > 0; ctr--) {


    // Read inputs
    for ( int j = 0; j < 8; j++ ) {
      inptr[j]    = input_coef.read();
      quantptr[j] = input_quant.read();
      wait();
    }

    if (inptr[1] == 0 && inptr[2] == 0 &&
	inptr[3] == 0 && inptr[4] == 0 &&
	inptr[5] == 0 && inptr[6] == 0 &&
	inptr[7] == 0) {
      /* AC terms all zero */
      workspace_t dcval = DEQUANTIZE(inptr[0], quantptr[0]) << PASS1_BITS;
      
      wsptr[DCTSIZE*0] = dcval;
      wsptr[DCTSIZE*1] = dcval;
      wsptr[DCTSIZE*2] = dcval;
      wsptr[DCTSIZE*3] = dcval;
      wsptr[DCTSIZE*4] = dcval;
      wsptr[DCTSIZE*5] = dcval;
      wsptr[DCTSIZE*6] = dcval;
      wsptr[DCTSIZE*7] = dcval;
      
      wsptr++;			/* advance pointers to next column */
      continue;
    }
    
    /* Even part: reverse the even part of the forward DCT. */
    /* The rotator is sqrt(2)*c(-6). */
    
    z2 = DEQUANTIZE(inptr[2], quantptr[2]);
    z3 = DEQUANTIZE(inptr[6], quantptr[6]);
    
    z1 = MULTIPLY(z2 + z3, FIX_0_541196100);
    tmp2 = z1 + MULTIPLY(z3, - FIX_1_847759065);
    tmp3 = z1 + MULTIPLY(z2, FIX_0_765366865);
    
    z2 = DEQUANTIZE(inptr[0], quantptr[0]);
    z3 = DEQUANTIZE(inptr[4], quantptr[4]);

    tmp0 = (z2 + z3) << CONST_BITS;
    tmp1 = (z2 - z3) << CONST_BITS;
    
    tmp10 = tmp0 + tmp3;
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;
    
    /* Odd part per figure 8; the matrix is unitary and hence its
     * transpose is its inverse.  i0..i3 are y7,y5,y3,y1 respectively.
     */
    
    tmp0 = DEQUANTIZE(inptr[7], quantptr[7]);
    tmp1 = DEQUANTIZE(inptr[5], quantptr[5]);
    tmp2 = DEQUANTIZE(inptr[3], quantptr[3]);
    tmp3 = DEQUANTIZE(inptr[1], quantptr[1]);
    
    z1 = tmp0 + tmp3;
    z2 = tmp1 + tmp2;
    z3 = tmp0 + tmp2;
    z4 = tmp1 + tmp3;
    z5 = MULTIPLY(z3 + z4, FIX_1_175875602); /* sqrt(2) * c3 */
    
    tmp0 = MULTIPLY(tmp0, FIX_0_298631336); /* sqrt(2) * (-c1+c3+c5-c7) */
    tmp1 = MULTIPLY(tmp1, FIX_2_053119869); /* sqrt(2) * ( c1+c3-c5+c7) */
    tmp2 = MULTIPLY(tmp2, FIX_3_072711026); /* sqrt(2) * ( c1+c3+c5-c7) */
    tmp3 = MULTIPLY(tmp3, FIX_1_501321110); /* sqrt(2) * ( c1+c3-c5-c7) */
    z1 = MULTIPLY(z1, - FIX_0_899976223); /* sqrt(2) * (c7-c3) */
    z2 = MULTIPLY(z2, - FIX_2_562915447); /* sqrt(2) * (-c1-c3) */
    z3 = MULTIPLY(z3, - FIX_1_961570560); /* sqrt(2) * (-c3-c5) */
    z4 = MULTIPLY(z4, - FIX_0_390180644); /* sqrt(2) * (c5-c3) */
    
    z3 += z5;
    z4 += z5;
    
    tmp0 += z1 + z3;
    tmp1 += z2 + z4;
    tmp2 += z2 + z3;
    tmp3 += z1 + z4;
    
    /* Final output stage: inputs are tmp10..tmp13, tmp0..tmp3 */
    
    wsptr[DCTSIZE*0] = (workspace_t) DESCALE(tmp10 + tmp3, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*7] = (workspace_t) DESCALE(tmp10 - tmp3, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*1] = (workspace_t) DESCALE(tmp11 + tmp2, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*6] = (workspace_t) DESCALE(tmp11 - tmp2, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*2] = (workspace_t) DESCALE(tmp12 + tmp1, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*5] = (workspace_t) DESCALE(tmp12 - tmp1, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*3] = (workspace_t) DESCALE(tmp13 + tmp0, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*4] = (workspace_t) DESCALE(tmp13 - tmp0, CONST_BITS-PASS1_BITS);

    wsptr++;			/* advance pointers to next column */
  }
  
  /* Pass 2: process rows from work array, store into output array. */
  /* Note that we must descale the results by a factor of 8 == 2**3, */
  /* and also undo the PASS1_BITS scaling. */

  wsptr = workspace;
 
  for (ctr = 0; ctr < DCTSIZE; ctr++) {
    sc_uint<8> outptr[8];
    
#ifndef NO_ZERO_ROW_TEST
    if (wsptr[1] == 0 && wsptr[2] == 0 && wsptr[3] == 0 && wsptr[4] == 0 &&
	wsptr[5] == 0 && wsptr[6] == 0 && wsptr[7] == 0) {
      /* AC terms all zero */
       sc_uint<8> dcval = range_limit[(int) DESCALE((INT32) wsptr[0], PASS1_BITS+3)
				  & RANGE_MASK];
      
      outptr[0] = dcval;
      outptr[1] = dcval;
      outptr[2] = dcval;
      outptr[3] = dcval;
      outptr[4] = dcval;
      outptr[5] = dcval;
      outptr[6] = dcval;
      outptr[7] = dcval;

      wsptr += DCTSIZE;		/* advance pointer to next row */
      goto OUTPUT;
    }
#endif
    
    /* Even part: reverse the even part of the forward DCT. */
    /* The rotator is sqrt(2)*c(-6). */
    
    z2 = (INT32) wsptr[2];
    z3 = (INT32) wsptr[6];
    
    z1 = MULTIPLY(z2 + z3, FIX_0_541196100);
    tmp2 = z1 + MULTIPLY(z3, - FIX_1_847759065);
    tmp3 = z1 + MULTIPLY(z2, FIX_0_765366865);
    
    tmp0 = ((INT32) wsptr[0] + (INT32) wsptr[4]) << CONST_BITS;
    tmp1 = ((INT32) wsptr[0] - (INT32) wsptr[4]) << CONST_BITS;
    
    tmp10 = tmp0 + tmp3;
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;
    
    /* Odd part per figure 8; the matrix is unitary and hence its
     * transpose is its inverse.  i0..i3 are y7,y5,y3,y1 respectively.
     */
    
    tmp0 = (INT32) wsptr[7];
    tmp1 = (INT32) wsptr[5];
    tmp2 = (INT32) wsptr[3];
    tmp3 = (INT32) wsptr[1];
    
    z1 = tmp0 + tmp3;
    z2 = tmp1 + tmp2;
    z3 = tmp0 + tmp2;
    z4 = tmp1 + tmp3;
    z5 = MULTIPLY(z3 + z4, FIX_1_175875602); /* sqrt(2) * c3 */
    
    tmp0 = MULTIPLY(tmp0, FIX_0_298631336);  /* sqrt(2) * (-c1+c3+c5-c7) */
    tmp1 = MULTIPLY(tmp1, FIX_2_053119869);  /* sqrt(2) * ( c1+c3-c5+c7) */
    tmp2 = MULTIPLY(tmp2, FIX_3_072711026);  /* sqrt(2) * ( c1+c3+c5-c7) */
    tmp3 = MULTIPLY(tmp3, FIX_1_501321110);  /* sqrt(2) * ( c1+c3-c5-c7) */
    z1 = MULTIPLY(z1, - FIX_0_899976223);    /* sqrt(2) * (c7-c3) */
    z2 = MULTIPLY(z2, - FIX_2_562915447);    /* sqrt(2) * (-c1-c3) */
    z3 = MULTIPLY(z3, - FIX_1_961570560);    /* sqrt(2) * (-c3-c5) */
    z4 = MULTIPLY(z4, - FIX_0_390180644);    /* sqrt(2) * (c5-c3) */
    
    z3 += z5;
    z4 += z5;
    
    tmp0 += z1 + z3;
    tmp1 += z2 + z4;
    tmp2 += z2 + z3;
    tmp3 += z1 + z4;
    
    /* Final output stage: inputs are tmp10..tmp13, tmp0..tmp3 */
    
    outptr[0] = range_limit[(int) DESCALE(tmp10 + tmp3,
					  CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[7] = range_limit[(int) DESCALE(tmp10 - tmp3,
					  CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[1] = range_limit[(int) DESCALE(tmp11 + tmp2,
					  CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[6] = range_limit[(int) DESCALE(tmp11 - tmp2,
					  CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[2] = range_limit[(int) DESCALE(tmp12 + tmp1,
					  CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[5] = range_limit[(int) DESCALE(tmp12 - tmp1,
					  CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[3] = range_limit[(int) DESCALE(tmp13 + tmp0,
					  CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[4] = range_limit[(int) DESCALE(tmp13 - tmp0,
					  CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    
    wsptr += DCTSIZE;		/* advance pointer to next row */

    // Write outputs
  OUTPUT:
    output_start.write( 1 );
    for ( int j = 0; j < 8; j++ ) {
      output_sample.write( outptr[j] );
      output_row.write( ctr );
      output_col.write( j );

      wait();
    }
    output_start.write( 0 );



  }
}


void run()
{

  //---  Reset- should be executable in 1 clock cycle
  output_start.write( 0 );
  output_sample.write( 12345 );
  output_row.write( 0 );
  output_col.write( 0 );
 

  wait();

  //---- Main IDCT computation
  while( true ) {
    
    jpeg_idct_islow();


  }
}

  SC_CTOR( idct ) {
      SC_CTHREAD(run, clk.pos());
      reset_signal_is(rst, false);
  };



    ~idct(){};

};

#endif // IDCT_H_
