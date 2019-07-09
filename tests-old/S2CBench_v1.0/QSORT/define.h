//========================================================================================
// 
//
// File Name    : define.h
// Description  : define parameter file for quick sort
// Release Date : 01/08/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra
//
// Revision History
//---------------------------------------------------------------------------
// Date       Version   Author               Description
//---------------------------------------------------------------------------
//            1.0       Darel Rex Finley   This public-domain C implementation by
// 01/08/13   1.1       PolyU DARC Lab      Conversion into Synthesizable SystemC
//=======================================================================================

#ifndef DEFINE_H
#define DEFINE_H

#include "systemc.h"

#include <iostream>
#include "stdio.h"


#define SIZE 10

#define INFILENAME               "qsort_in_data.txt"


#define OUTFILENAME_GOLDEN       "qsort_output_golden.txt"
#define OUTFILENAME              "qsort_output.txt"
#define DIFFFILENAME             "qsort_diff.txt"

//#define WAVE_DUMP          // set do dump waveform or set as compile option -DWAVE_DUMP

#endif

