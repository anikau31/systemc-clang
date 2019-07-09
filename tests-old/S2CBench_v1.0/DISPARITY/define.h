//============================================================================
// 
// File Name    : define.h
// Description  : Define the parameters
// Release Date : 16/07/2013
// Author       :
//
// Revision History
//---------------------------------------------------------------------------
// Date     Author      Version     Description
//---------------------------------------------------------------------------
// 16/07/13 PolyU           1.0     library, macros and parameters declaration      
//===========================================================================

#ifndef DEFINE_H
#define DEFINE_H
#include <systemc.h>

#define Width_max    1920 //maximum image width

#define HEIGHT          64 //macroblock horizontal size
#define WIDTH           64 //macroblock vertical size
#define FULL            64 // Search range

#define ADJ          128/FULL
#define DIST_ADJUST_LOW     6
#define DIST_ADJUST_HIGH   36

#define MAX_DIFF   128

#define HOFFSET     20
#define WOFFSET     200 


#define INFILENAME           "3D_crysis.bmp"
#define OUTFILENAME          "3D_crysis_out.bmp"
#define OUTFILENAME_GOLDEN   "3D_crysis_out_golden.bmp"
#define DIFFFILENAME         "disparity_diff.txt"

//#define WAVE_DUMP


#endif  // end DEFINE_H
