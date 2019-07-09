//========================================================================================
// 
//
// File Name    : define.h
// Description  : Main definition header file for Kasumi
// Release Date : 23/07/2013
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version  Author    Description
//----------------------------------------------------------------------------------------
//23/07/2013     1.0     PolYU   Definition file for Kasumi core
//=======================================================================================

#ifndef _DEFINE_H
#define _DEFINE_H

#include "systemc.h"

//#include <iostream>
#include "stdio.h"

//typedef unsigned char u8;
//typedef unsigned short u16;
//typedef unsigned long u32;

typedef sc_uint<8> u8;
typedef sc_uint<16> u16;
typedef sc_uint<32> u32;


#define ROL16(a,b) (u16)((a<<b)|(a>>(16-b)))

typedef struct dword{
	u8 b8[4];
}DWORD; 

typedef struct word {
	u8 b8[2];
} WORD;



#define DEPTH 2
#define WIDTH 4

#define INFILENAME                "kasumi_indata.txt"  // random data
#define INFILEKEY                 "kasumi_key.txt"    // random data

#define OUTFILENAME_GOLDEN       "kasumi_output_golden.txt"
#define OUTFILENAME              "kasumi_output.txt"
#define DIFFFILENAME             "kasumi_diff.txt"

//#define WAVE_DUMP          // set do dump waveform or set as compile option -DWAVE_DUMP

#endif  // DEFINE_H

