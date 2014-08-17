//========================================================================================
// 
//
// File Name    : define.h
// Description  : Definition file for FFT
// Release Date : 16/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version    Author    Description
//----------------------------------------------------------------------------------------
// 16/07/2013     1.0     PolyU     definition file forFFT
//=======================================================================================

#ifndef DEFINE_FFT_H
#define DEFINE_FFT_H

#include "systemc.h"

#include <iostream>
#include "stdio.h"
#include "math.h"


#define INFILENAME             "fft_input.txt"  // random
#define OUTFILENAME_GOLDEN     "fft_output_golden.txt"
#define OUTFILENAME            "fft_output.txt"


#define DIFFFILENAME           "fft_diff.txt"

//#define WAVE_DUMP          // set do dump waveform or set as compile option -DWAVE_DUMP

#endif  // end DEFINE_FFT_H

