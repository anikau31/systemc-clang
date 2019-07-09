//========================================================================================
// 
// File Name    : define.h
// Description  : Main definition header
// Release Date : 23/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra  
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version Author      Description
//----------------------------------------------------------------------------------------
//14/02/2013      1.0    PolyU      definition file
//=======================================================================================

#ifndef DEFINE_H
#define DEFINE_H

#include "systemc.h"

//#include <iostream>
#include "stdio.h"

#define TAPS 8


#define IN_DATA   "in_data.txt"
#define IN_FACTOR_DATA  "in_factor_data.txt"

#define GOLDEN_OUTPUT "odata_golden.txt"
#define GOLDEN_OUTPUT_ENABLE "odata_enable_golden.txt"

#define OUT_FILE   "odata.txt"
#define OUT_FILE_ENABLE "odata_enable.txt"


//#define WAVE_DUMP          // set do dump waveform or set as compile option -DWAVE_DUMP

#endif  // DEFINE_H

