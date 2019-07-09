//========================================================================================
// 
//
// File Name    : define.h
// Description  : Definition file for ADPCM encoder
// Release Date : 31/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date          Version  Author      Description
//----------------------------------------------------------------------------------------
// 31/07/2013     1.0     PolyU       definition file
//=======================================================================================

#ifndef DEFINE_ADPCM_H
#define DEFINE_ADPCM_H

#include "systemc.h"

#include <iostream>
#include "stdio.h"



#define INFILENAME             "adpcm_input.txt"    // random
#define OUTFILENAME_GOLDEN     "adpcm_output_golden.txt"
#define OUTFILENAME            "adpcm_output.txt"


#define DIFFFILENAME           "adpcm_diff.txt"

//#define WAVE_DUMP          // set do dump waveform or set as compile option -DWAVE_DUMP

#endif  // end DEFINE_ADPCM_H

