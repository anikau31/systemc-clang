//========================================================================================
// 
//
// File Name    : define.h
// Description  : Main definition header for decimation filter
// Release Date : 23/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version Author      Description
//----------------------------------------------------------------------------------------
//29/07/2013      1.0    PolyU        definition file
//=======================================================================================

#ifndef DEFINE_H
#define DEFINE_H

#include "systemc.h"

//#include <iostream>
#include "stdio.h"

#define TAPS_STAGE1 4
#define TAPS_STAGE2 4
#define TAPS_STAGE3 4
#define TAPS_STAGE4 6
#define TAPS_STAGE5 12
#define STAGES 5

#define BUFFER_SIZE_STAGE1 7
#define BUFFER_SIZE_STAGE2 7
#define BUFFER_SIZE_STAGE3 7
#define BUFFER_SIZE_STAGE4 11
#define BUFFER_SIZE_STAGE5 24



#define IN_DATA_FILENAME          "decim_data.txt"
#define IN_COEFF_FILENAME         "decim_coeff.txt"

#define OUTFILENAME_GOLDEN   "decim_output_golden.txt"

#define OUTFILENAME         "decim_output.txt"
#define DIFFFILENAME        "decim_diff.txt"


//#define WAVE_DUMP          // set do dump waveform or set as compile option -DWAVE_DUMP

#endif  // DEFINE_H

