//========================================================================================
// 
//
// File Name    : define.cpp
// Description  : definition file
// Release Date : 14/02/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version    Author       Description
//----------------------------------------------------------------------------------------
//14/02/2013       1.0     PolyU     definition file
//=======================================================================================

#ifndef DEFINE_H
#define DEFINE_H

#include "systemc.h"

//#include <iostream>
#include "stdio.h"



#define FILTER_TAPS 10

#define INFILTERFILENAME         "fir_in_data.txt"   // random data
#define INCOEFFFILENAME          "fir_coeff.txt"

#define OUTFILENAME_GOLDEN       "fir_output_golden.txt"
#define OUTFILENAME              "fir_output.txt"
#define DIFFFILENAME             "firdiff.txt"

//#define WAVE_DUMP          // set do dump waveform or set as compile option -DWAVE_DUMP

#endif

