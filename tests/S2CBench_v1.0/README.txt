===================================================================================
				S2CBENCH
===================================================================================

S2CBENCH stands for Synthesizable SystemC Benchmark suite. It is a open source
SystemC benchmarks created to help designers evaluate the QoR of state of the art
HLS Tools. All of the main HLS tools support the synthesizable subset of SystemC
and hence should be able to synthesize all of these designs without any modifications
EXCEPT for the FFT. The FFT is non-synthesizable as it is because it inludes floating
point and trigonometric functions which are not supported as per the latest SystemC
Synthesizable subset draft 1.3. The FFT was included in order to help you evaluate
how the different vendors deal with floating point synthesis and if they support trigonometric
functions.

Most of the work has been done by the DARC Lab at the Hong Kong Polytechnic Universities
Department of Electronic and Information Engineering (EIE)
DARC =Design Automation and Reconfigurable Computing Lab - Benjamin Carrion Schafer, Anushree Mahapatra


S2CBench is distributed in the hope that it will be useful.S2CBench is free software; you can 
redistribute it and/or modify it, but please remember but WITHOUT ANY WARRANTY; without even the 
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

S2CBENCH includes the following Testcases:

---------------------------------------------------------------------------------
|   NAME      |      		Description             |     Author   
|-------------+-----------------------------------------+------------------------
| ADPCM       | Adaptive Differential Pulse-Code        | FFmpeg
|	      |	Modulation (encoder part only)         	| PolyU DARC Lab 
|-------------+-----------------------------------------+-----------------------
| AES CIPHER  | Advanced Encryption standared (AES)     | pjc.co.jp 
| 	      | 128-bits (cipher only)		        | PolyU DARC Lab 
|-------------+-----------------------------------------+-----------------------
| Disparity   | Stereoscopic image disparity estimator  | Miscellanous
| Estimator   |					        | PolyU DARC Lab 
|-------------+-----------------------------------------+-----------------------
| FFT         | Fast Fourier Transform                  | R.Goswami,Synopsys, Inc.
|             |						| PolyU DARC Lab 
|-------------+-----------------------------------------+------------------------
| FIR         | 10-Tap FIR filter                       | PolyU DARC Lab 
|-------------+-----------------------------------------+-----------------------
| DECIMATION  | 5 Stages decimation filter              | PolyU DARC Lab
|-------------+-----------------------------------------+-----------------------
|Interpolation| 4 Stages interpolation filter           | PolyU DARC Lab
|-------------+-----------------------------------------+-----------------------
| IDCT        | Inverse Discrete Cosine Transform       | Thomas G. Lange
|	      |						| PolyU DARC Lab
|-------------+-----------------------------------------+-----------------------
| Kasumi      | Kasumi encryption algorithm             |  ETSI/SAGE
|             |						|  PolyU DARC Lab
|-------------+-----------------------------------------+-----------------------
| MD5C        | Message Digest Algorithm                | RSA Data Security, Inc
|	      |						| PolyU DARC Lab
|-------------+-----------------------------------------+-----------------------
| QSORT       | Quick sort                              | Darel Rex Finley 
|	      | 					| PolyU DARC Lab
|-------------+-----------------------------------------+-----------------------
| SNOW 3G     | snow 3G encryption algorithm            | ETSI/SAGE
|	      |						| PolyU DARC Lab
|-------------+-----------------------------------------+-----------------------
| Sobel       | Sobel filter                            | PolyU DARC Lab
|------------------------------------------------------------------------------


Each benchmark contains the following files:

Makefile -- Need to modify the path to the systemC folder
--------
make : Generates the executable binary
make wave :Generates the same binary, but the simulate creates a VCD file to view the simulation results
make debug : to create a debug version	   -- VCD File can be visualied with GTKwave (free VCD file viewer)
make clean : cleans the exe and .o files

SystemC files
------------
main.cpp  : Instantiates the modules which sends and receives data and the unit under test
benchmark.cpp /.h : Main description of the benchmark
tb_benchmark.cpp /.h : Testbech for the given benchmark
define.h  : Includes define statments and stimuli filenames

Stimuli files (.txt)
-------------------
<name>.txt       :  File with iput stimuli (could be more than one)
<name>_golden.txt :  File with golden output with which the simulation results will be compared



Extraction instruction (Linux):
%tar -zxvf S2Cbench_<ver>.tar.gz



							[END]





