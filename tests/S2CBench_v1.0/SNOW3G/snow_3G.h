//---------------------------------------------------------------------------
//
//
// File Name    : snow.h
// Description  : stream cipher that forms theheart of the 3GPP confidentiality
//                algorithm UEA2 and the 3GPP integrity algorithm UIA2
//
// Release Date : 26/07/13
// Author       : ETSI/SAGE (www.gsm.com)
// Modified     : PolyU DARC Lab
//
// Revision History
//---------------------------------------------------------------------------
// Date      Version   Author          Description
//---------------------------------------------------------------------------
// 2006        1.0    ETSI/SAGE        Original ANSI C description
//26/07/13     1.1    PolyU DARC Lab   Converted into Synthesizable SystemC
//---------------------------------------------------------------------------


#ifndef SNOW_3G_H
#define SNOW_3G_H

#include "define.h"


typedef sc_uint<8> u8;
typedef sc_uint<32> u32;
typedef sc_biguint<64> u64;


SC_MODULE (snow_3G) {
 
 // inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in<sc_uint<32> >  ks_in[SIZE];

  // outputs
  sc_out<sc_uint<32> >  ks_out[SIZE];


  /* Variables declaration*/
  sc_uint<32> ks[SIZE];

  /* LFSR */
  u32 LFSR_S0 ;
  u32 LFSR_S1 ;
  u32 LFSR_S2 ;
  u32 LFSR_S3 ;
  u32 LFSR_S4 ;
  u32 LFSR_S5 ;
  u32 LFSR_S6 ;
  u32 LFSR_S7 ;
  u32 LFSR_S8 ;
  u32 LFSR_S9 ;
  u32 LFSR_S10 ;
  u32 LFSR_S11 ;
  u32 LFSR_S12 ;
  u32 LFSR_S13 ;
  u32 LFSR_S14 ;
  u32 LFSR_S15 ;
  /* FSM */
  u32 FSM_R1 ;
  u32 FSM_R2 ;
  u32 FSM_R3 ; 


  /* C */
void  GenerateKeystream(){

  u32 t = 0;
  u32 F= 0x0;

  /* LFSR */
  LFSR_S0 = 0x00;
  LFSR_S1 = 0x00;
  LFSR_S2 = 0x00;
  LFSR_S3 = 0x00;
  LFSR_S4 = 0x00;
  LFSR_S5 = 0x00;
  LFSR_S6 = 0x00;
  LFSR_S7 = 0x00;
  LFSR_S8 = 0x00;
  LFSR_S9 = 0x00;
  LFSR_S10 = 0x00;
  LFSR_S11 = 0x00;
  LFSR_S12 = 0x00;
  LFSR_S13 = 0x00;
  LFSR_S14 = 0x00;
  LFSR_S15 = 0x00;
  /* FSM */
  FSM_R1 = 0x00;
  FSM_R2 = 0x00;
  FSM_R3 = 0x00; 



  wait();

  while(1){

    for (t=0 ; t < SIZE; t++)
      ks[t] = ks_in[t];

  
  ClockFSM(); /* Clock FSM once. Discard the output. */

  ClockLFSRKeyStreamMode(); /* Clock LFSR in keystream mode once. */

  for ( t=0; t < SIZE; t++){
    F = ClockFSM(); /* STEP 1 */
   
    ks[t] =  F ^ LFSR_S0; /* STEP 2 */
    
    /* Note that ks[t] corresponds to z_{t+1} in section 4.2*/
    ClockLFSRKeyStreamMode(); /* STEP 3 */
  
  }



  /* Write outputs */
  for(t=0; t<SIZE;t++)
    ks_out[t] = ks[t];


  wait();

 }

 }



//
// MULx.
// Input V: an 8-bit input.
// Input c: an 8-bit input.
// Output : an 8-bit output.
// See section 3.1.1 for details.
// 

u8 MULx(u8 V, u8 c)
{
  if ( V & 0x80 )
    return ( (V << 1) ^ c);
  else
    return ( V << 1);
}

//
// MULxPOW.
// Input V: an 8-bit input.
// Input i: a positive integer.
// Input c: an 8-bit input.
// Output : an 8-bit output.
// See section 3.1.2 for details.
//
// PolyU DARC :  Modified to avoid recurssion not supported in HLS 

template <class T>
u8  MULxPOW(u8 V, T i,  u8 c) 
{

  //if ( i == 0)  /* original recursive description */
  //  return V;
  //else
  // return MULx( MULxPOW( V, i-1, c ), c);

 while(i > 0){

   V = MULx( V, c);
   i=i-1;

 }

 return V;


}


//
// The function MUL alpha.
// Input c: 8-bit input.
// Output : 32-bit output.
// See section 3.4.2 for details.
//
u32 MULalpha(u8 c)
{
  return ( ( ((u32)MULxPOW(c, 23, 0xa9)) << 24 )|
	   ( ((u32)MULxPOW(c, 245, 0xa9)) << 16 ) |
	   ( ((u32)MULxPOW(c, 48, 0xa9)) << 8 ) |
	   ( ((u32)MULxPOW(c, 239, 0xa9)) ) ) ;
}

//
// The function DIV alpha.
// Input c: 8-bit input.
// Output : 32-bit output.
// See section 3.4.3 for details.
//
u32  DIValpha(u8 c)
{
return ( ( ((u32)MULxPOW(c, 16, 0xa9)) << 24 )|
	 ( ((u32)MULxPOW(c, 39, 0xa9)) << 16 ) |
	 ( ((u32)MULxPOW(c, 6, 0xa9)) << 8 ) |
	 ( ((u32)MULxPOW(c, 64, 0xa9)) ) ) ; 
 
}

// The 32x32-bit S-Box S1
// Input: a 32-bit input.
// Output: a 32-bit output of S1 box.
// See section 3.3.1.
//
u32  S1(u32 w)
{
u8 r0=0, r1=0, r2=0, r3=0;
u8 srw0 = SR[ (u8)((w >> 24) & 0xff) ];
u8 srw1 = SR[ (u8)((w >> 16) & 0xff) ];
u8 srw2 = SR[ (u8)((w >> 8) & 0xff) ];
u8 srw3 = SR[ (u8)((w) & 0xff) ];

r0 = ( ( MULx( srw0 , 0x1b) ) ^( srw1 ) ^( srw2 ) ^( (MULx( srw3, 0x1b)) ^ srw3 ));
r1 = ( ( ( MULx( srw0 , 0x1b) ) ^ srw0 ) ^( MULx(srw1, 0x1b) ) ^( srw2 ) ^( srw3 ));
r2 = ( ( srw0 ) ^( ( MULx( srw1 , 0x1b) ) ^ srw1 ) ^( MULx(srw2, 0x1b) ) ^( srw3 ));
r3 = ( ( srw0 ) ^ ( srw1 ) ^( ( MULx( srw2 , 0x1b) ) ^ srw2 ) ^( MULx( srw3, 0x1b) ));
return ( ( ((u32)r0) << 24 ) | ( ((u32)r1) << 16) | ( ((u32)r2) << 8 ) |( ((u32)r3) ) );
}

//
// The 32x32-bit S-Box S2
// Input: a 32-bit input.
// Output: a 32-bit output of S2 box.
// See section 3.3.2.
//
u32  S2(u32 w)
{
u8 r0=0, r1=0, r2=0, r3=0;

u8 sqw0 = SQ[ (u8)((w >> 24) & 0xff) ];
u8 sqw1 = SQ[ (u8)((w >> 16) & 0xff) ];
u8 sqw2 = SQ[ (u8)((w >> 8) & 0xff) ];
u8 sqw3 = SQ[ (u8)((w) & 0xff) ];
r0 = ( ( MULx( sqw0 , 0x69) ) ^( sqw1 ) ^( sqw2 ) ^( (MULx( sqw3, 0x69)) ^ sqw3 ));
r1 = ( ( ( MULx( sqw0 , 0x69) ) ^ sqw0 ) ^( MULx(sqw1, 0x69) ) ^( sqw2 ) ^( sqw3 ));
r2 = ( ( sqw0 ) ^( ( MULx( sqw1 , 0x69) ) ^ sqw1 ) ^( MULx(sqw2, 0x69) ) ^( sqw3 ));
r3 = ( ( sqw0 ) ^( sqw1 ) ^( ( MULx( sqw2 , 0x69) ) ^ sqw2 ) ^( MULx( sqw3, 0x69) ));

return ( ( ((u32)r0) << 24 ) | ( ((u32)r1) << 16) | ( ((u32)r2) << 8 ) |( ((u32)r3) ) );
} 


//
// Clocking LFSR in initialization mode.
// LFSR Registers S0 to S15 are updated as the LFSR receives a single clock.
// Input F: a 32-bit word comes from output of FSM.
// See section 3.4.4.
//
void  ClockLFSRInitializationMode(u32 F)
{

u32 v = ( ( (LFSR_S0 << 8) & 0xffffff00 )^( MULalpha( (u8)((LFSR_S0>>24) & 0xff)) ) ^( LFSR_S2 ) ^( (LFSR_S11 >> 8) & 0x00ffffff )^( DIValpha( (u8)( ( LFSR_S11) & 0xff )) ) ^( F ));
 LFSR_S0 = LFSR_S1;
 LFSR_S1 = LFSR_S2;
 LFSR_S2 = LFSR_S3; 
 LFSR_S3 = LFSR_S4;
 LFSR_S4 = LFSR_S5;
 LFSR_S5 = LFSR_S6;
 LFSR_S6 = LFSR_S7;
 LFSR_S7 = LFSR_S8;
 LFSR_S8 = LFSR_S9;
 LFSR_S9 = LFSR_S10;
 LFSR_S10 = LFSR_S11;
 LFSR_S11 = LFSR_S12;
 LFSR_S12 = LFSR_S13;
 LFSR_S13 = LFSR_S14;
 LFSR_S14 = LFSR_S15;
 LFSR_S15 = v;
}


//
// Clocking LFSR in keystream mode.
// LFSR Registers S0 to S15 are updated as the LFSRreceives a single clock.
// See section 3.4.5.
//
void  ClockLFSRKeyStreamMode()
{
u32 v = ( ( (LFSR_S0 << 8) & 0xffffff00 ) ^( MULalpha( (u8)((LFSR_S0>>24) & 0xff)) ) ^( LFSR_S2 ) ^( (LFSR_S11 >> 8) & 0x00ffffff )^( DIValpha( (u8)( ( LFSR_S11) & 0xff )) ));

 LFSR_S0 = LFSR_S1;
 LFSR_S1 = LFSR_S2;
 LFSR_S2 = LFSR_S3;
 LFSR_S3 = LFSR_S4;
 LFSR_S4 = LFSR_S5;
 LFSR_S5 = LFSR_S6;
 LFSR_S6 = LFSR_S7;
 LFSR_S7 = LFSR_S8;
 LFSR_S8 = LFSR_S9;
 LFSR_S9 = LFSR_S10;
 LFSR_S10 = LFSR_S11;
 LFSR_S11 = LFSR_S12;
 LFSR_S12 = LFSR_S13;
 LFSR_S13 = LFSR_S14;
 LFSR_S14 = LFSR_S15;
 LFSR_S15 = v;
} 

//
// Clocking FSM.
// Produces a 32-bit word F.
// Updates FSM registers R1, R2, R3.
// See Section 3.4.6.
//
u32  ClockFSM()
{
  u32 F = ( ( LFSR_S15 + FSM_R1 ) & 0xffffffff ) ^FSM_R2 ;
  u32 r = ( FSM_R2 + ( FSM_R3 ^ LFSR_S5 ) ) & 0xffffffff ;
  FSM_R3 = S2(FSM_R2);
  FSM_R2 = S1(FSM_R1);
  FSM_R1 = r;
  return F;
}

//
// Initialization.
// Input k[4]: Four 32-bit words making up 128-bit key.
// Input IV[4]: Four 32-bit words making 128-bit initialization variable.
// Output: All the LFSRs and FSM are initialized for key generation. 
// See Section 4.1.
//
void  Initialize(u32 k[4], u32 IV[4])
{
 u8 i=0;
 u32 F = 0x0;
 LFSR_S15 = k[3] ^ IV[0];
 LFSR_S14 = k[2];
 LFSR_S13 = k[1];
 LFSR_S12 = k[0] ^ IV[1];
 LFSR_S11 = k[3] ^ 0xffffffff;
 LFSR_S10 = k[2] ^ 0xffffffff ^ IV[2];
 LFSR_S9 = k[1] ^ 0xffffffff ^ IV[3];
 LFSR_S8 = k[0] ^ 0xffffffff;
 LFSR_S7 = k[3];
 LFSR_S6 = k[2];
 LFSR_S5 = k[1];
 LFSR_S4 = k[0];
 LFSR_S3 = k[3] ^ 0xffffffff;
 LFSR_S2 = k[2] ^ 0xffffffff;
 LFSR_S1 = k[1] ^ 0xffffffff;
 LFSR_S0 = k[0] ^ 0xffffffff;
 FSM_R1 = 0x0;
 FSM_R2 = 0x0;
 FSM_R3 = 0x0;


 for(i=0;i<32;i++)
   {
  F = ClockFSM();
  ClockLFSRInitializationMode(F);
 }

} 



  SC_CTOR (snow_3G){
     SC_CTHREAD(GenerateKeystream,clk.pos());
     reset_signal_is(rst,false);
  };


  ~snow_3G(){};


};

#endif  // SNOW_3G_H
