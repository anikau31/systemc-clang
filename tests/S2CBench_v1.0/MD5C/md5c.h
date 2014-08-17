//=============================================================================
//
// File Name    : md5c.h
// Description  : MD5 message-digest algorithm
// Release Date : 16/07/13
// Author       : RSA Data Security, Inc. 
// Modified     : PolyU
//
// Revision History
//---------------------------------------------------------------------------
// Date       Version      Author       Description
//---------------------------------------------------------------------------
// 1991       1.0          RSA Data Security, Inc
//16/07/13    1.1           PolyU        Converted into Synthesizable SystemC
//==========================================================================


#ifndef MD5C_H_
#define MD5C_H_

#include "define.h"


SC_MODULE( md5c ) {

  // Inputs
  sc_in_clk              clk;
  sc_in< bool >          rst;

  sc_in< bool >          i_req;
  sc_in< bool >          i_final;
  sc_in< sc_uint<32> >   i_inputLen;

  // Outputs
  sc_out< bool >         o_busy;
  sc_out< bool >         o_start;
  sc_out< sc_uint<8> >   o_digest;


  // Member variables declaration
  sc_uint<32>		m_inputLen;
  sc_uint<8>		m_input[ MD5C_INPUT_BUFSIZE ];

  sc_uint<32>	m_state[4];	/* state (ABCD) */
  sc_uint<32>	m_count[2];	/* number of bits, modulo 2^64 (lsb first) */
  sc_uint<8>	m_buffer[64];	/* input buffer */

void
MD5Init()
{
  //cerr << "MD5Init()" << endl;
  o_busy.write( 1 );
  o_start.write( 0 );
  o_digest.write( 0 );

  m_count[0] = m_count[1] = 0;

  /* Load magic initialization constants.  */
  m_state[0] = 0x67452301;
  m_state[1] = 0xefcdab89;
  m_state[2] = 0x98badcfe;
  m_state[3] = 0x10325476;
}

/* 
 * MD5 block update operation. Continues an MD5 message-digest
 * operation, processing another message block, and updating the
 * context.
 */

void
MD5Update()
{

  sc_uint<32> inputLen = m_inputLen;

  /* Compute number of bytes mod 64 */
  sc_uint<6> idx = ((m_count[0] >> 3) & 0x3F);

  /* Update number of bits */
  if ((m_count[0] += ((u_int32_t)inputLen << 3))
      < ((u_int32_t)inputLen << 3)) {
    m_count[1]++;
  }
  m_count[1] += ((u_int32_t)inputLen >> 29);

  sc_uint<7> partLen = 64 - idx;

  /* Transform as many times as possible. */
  unsigned int i;
  if ( inputLen >= partLen ) {


    for ( unsigned j = 0; j < partLen; j++ ) m_buffer[idx+j] = m_input[j];
    MD5Transform();


    for (i = partLen; i + 63 < inputLen; i += 64) {

      for ( int j = 0; j < 64; j++ ) m_buffer[j] = m_input[i+j];
      MD5Transform();
    }
    idx = 0;

  } else {
    i = 0;
  }

  /* Buffer remaining input */

  for ( unsigned j = 0; j < inputLen-i; j ++ ) m_buffer[idx+j] = m_input[i+j];
}

/*
 * MD5 padding. Adds padding followed by original length.
 */

void
MD5Pad()
{


  /* Save number of bits */
  sc_uint<8> bits[8];
  ( bits[3], bits[2], bits[1], bits[0] ) = m_count[0];
  ( bits[7], bits[6], bits[5], bits[4] ) = m_count[1];

  /* Pad out to 56 mod 64. */
  sc_uint<6> idx = (m_count[0] >> 3) & 0x3f;
  sc_uint<7> padLen = (idx < 56) ? (56 - idx) : (120 - idx);

  for ( unsigned j = 0; j < padLen; j++ ) m_input[j] = PADDING[j];
  m_inputLen = padLen;
  MD5Update();

  /* Append length (before padding) */
  for ( int j = 0; j < 8; j++ ) m_input[j] = bits[j];
  m_inputLen = 8;
  MD5Update();
}

/*
 * MD5 finalization. Ends an MD5 message-digest operation, writing the
 * the message digest and zeroizing the context.
 */

void
MD5Final()
{


  /* Do padding. */
  MD5Pad();

  /* Store state in digest */
  for ( int j = 0; j < 4; j ++ ) {
    o_start.write( (j==0) ? 1 : 0 );
    o_digest.write( m_state[j].range(  7,  0 ) ); wait();
    o_digest.write( m_state[j].range( 15,  8 ) ); wait();
    o_digest.write( m_state[j].range( 23, 16 ) ); wait();
    o_digest.write( m_state[j].range( 31, 24 ) ); wait();
  }
}

/* MD5 basic transformation. Transforms state based on block. */

void
MD5Transform ()
{
  u_int32_t a = m_state[0], b = m_state[1], c = m_state[2], d = m_state[3];
  u_int32_t x[16];
  for ( int j = 0; j < 16; j ++ ) {
    sc_uint<8> b0 = m_buffer[4*j+0];
    sc_uint<8> b1 = m_buffer[4*j+1];
    sc_uint<8> b2 = m_buffer[4*j+2];
    sc_uint<8> b3 = m_buffer[4*j+3];
    x[ j ] = ( b3, b2, b1, b0 );
  }

  /* Round 1 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
  FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
  FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
  FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
  FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
  FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
  FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
  FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
  FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
  FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
  FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
  FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
  FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
  FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
  FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
  FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
  FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

  /* Round 2 */
#define S21 5
#define S22 9
#define S23 14
#define S24 20
  GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
  GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
  GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
  GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
  GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
  GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
  GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
  GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
  GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
  GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
  GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
  GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
  GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
  GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
  GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
  GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

  /* Round 3 */
#define S31 4
#define S32 11
#define S33 16
#define S34 23
  HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
  HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
  HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
  HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
  HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
  HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
  HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
  HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
  HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
  HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
  HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
  HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
  HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
  HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
  HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
  HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

  /* Round 4 */
#define S41 6
#define S42 10
#define S43 15
#define S44 21
  II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
  II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
  II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
  II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
  II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
  II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
  II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
  II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
  II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
  II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
  II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
  II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
  II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
  II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
  II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
  II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

  m_state[0] += a;
  m_state[1] += b;
  m_state[2] += c;
  m_state[3] += d;
}

void
entry()
{

  bool final;
  MD5Init();  // Reset state

  wait();


  // Main computational loop
  while (1) {
    o_busy.write( 0 );
    wait();
    while( i_req.read() == false );

    m_inputLen = i_inputLen.read();

    assert( m_inputLen <= MD5C_INPUT_BUFSIZE );
    final = i_final.read();

    o_busy.write(1);
    wait();

    if( final == true ) {
      MD5Final();
    }
    else {
      MD5Update();
    }
  }//while
}



  SC_CTOR( md5c ){
      SC_CTHREAD( entry, clk.pos() );
      reset_signal_is(rst,false);
    }

  ~md5c(){};


};

#endif // MD5C_H_

