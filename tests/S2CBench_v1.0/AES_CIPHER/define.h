//========================================================================================
// 
//
// File Name    : define.h
// Description  : Main definition header file for AES CIPHER
// Release Date : 14/02/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version  Author       Description
//----------------------------------------------------------------------------------------
//14/02/2013     1.0     PolYU        definition file for AES CIPHER
//=======================================================================================

#ifndef DEFINE_H
#define DEFINE_H

#include "systemc.h"

//#include <iostream>
#include "stdio.h"


#define SIZE 16

#define NB 4
#define NBb 16
#define  nk 4                            
#define  nr 10   


#define INFILENAME                "aes_cipher_input.txt"
#define INFILENAME_KEY            "aes_cipher_key.txt"

#define OUTFILENAME_GOLDEN       "aes_cipher_output_golden.txt"
#define OUTFILENAME              "aes_cipher_output.txt"
#define DIFFFILENAME             "diff.txt"

//#define WAVE_DUMP          // set do dump waveform or set as compile option -DWAVE_DUMP

#endif  // DEFINE_H

