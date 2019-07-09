//========================================================================================
// 
// File Name    : tb_fir.h
// Description  : Testbench
// Release Date : 14/02/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
// 
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version    Author      Description
//---------------------------------------------------------------------------------------
//14/02/2013      1.0       PolyU     FIR filter original testbench
//=======================================================================================


#include "define.h"


#ifndef TB_FIR_H_
#define TB_FIR_H_

SC_MODULE (test_FIR){

  // Inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in<sc_uint<8> > filter_output; 

  // Outputs
  sc_out<sc_uint<8> >  in_data[FILTER_TAPS];
  sc_out<sc_uint<16> > coeff[FILTER_TAPS];


  //File pointers
  FILE * in_filter_file, *in_coeff_file, *out_filter_golden_file, *out_filter_file_read;
  FILE  *out_filter_file, *diff_file;

  void send(){

  // Variables declaration
  int i=0;
  unsigned int coeff_read, in_filter_read;

  //Reset routine
  in_filter_file = fopen(INFILTERFILENAME, "rt");
  in_coeff_file = fopen(INCOEFFFILENAME, "rt");


  if(!in_filter_file){
    cout << "Could not open " << INFILTERFILENAME << "\n";
    sc_stop();
    exit (-1);
  }


  if(!in_coeff_file){
    cout << "Could not open " << INCOEFFFILENAME << "\n";
    sc_stop();
    exit (-1);
  }


  for(i=0; i < FILTER_TAPS; i ++){
    in_data[i].write(0);
    if(fscanf(in_coeff_file, "%u", &coeff_read) == EOF)
      break;
    coeff[i].write(coeff_read);
  }
  


  wait();
  i= 0;
  while(true){

    while(fscanf(in_filter_file,"%u", &in_filter_read) != EOF){
	in_data[i++].write(in_filter_read);
   
	if(i == FILTER_TAPS){
	  wait();
	  i= 0;
	}
    }
  
   
    fclose(in_coeff_file);
    fclose(in_filter_file);

    cout << endl << "Starting comparing results " << endl;
 
    compare_results();
    sc_stop();

    wait();

  }//while_loop
}



//-----------------------------
// Receive data thread
//----------------------------
void recv(){

  // Variables declaration
  unsigned int filter_out_write=0;

  // out_filter_file.open (OUTFILENAME);
  out_filter_file = fopen (OUTFILENAME, "wt");

  if(!out_filter_file){
    cout << "Could not open " << OUTFILENAME << "\n";
    sc_stop();
    exit (-1);
  }


  wait();

  while(true){
    filter_out_write = filter_output.read();

    fprintf(out_filter_file,"%d\n",filter_out_write);

    wait();
  }
}


//---------------------------------
// Compare results function
//--------------------------------
void compare_results(){

  int outfilter, outfilter_golden, line=1, errors=0;

  // Close file where outputs are stored
  fclose(out_filter_file);

  // Open results file
  out_filter_file = fopen (OUTFILENAME, "rt");

  if(!out_filter_file){
    cout << "Could not open " << OUTFILENAME << endl;
    sc_stop();
    exit (-1);
  }

    //
    //Load the golden output from file
    //
    //out_filter_golden_file.open(OUTFILENAME_GOLDEN);
    out_filter_golden_file = fopen (OUTFILENAME_GOLDEN, "rt");
    if(!out_filter_golden_file){
      cout << "Could not open " << OUTFILENAME_GOLDEN << endl;
      sc_stop();
      exit (-1);
     }

    //
    // comparison result with golden output
    //

    diff_file = fopen (DIFFFILENAME, "w");
    if(!diff_file){
	 cout << "Could not open " << DIFFFILENAME<< "\n";
	 sc_stop();
	 exit (-1);
       }

    //      while(out_filter_golden_file.eof()){
    while(fscanf(out_filter_golden_file, "%d", &outfilter_golden) != EOF){
      fscanf(out_filter_file,"%d", &outfilter);
     

      cout << endl <<"Cycle["<< line << "]: " << outfilter_golden << "-- "<< outfilter;

      // out_filter_file_read >> outfilter;
      // out_filter_golden_file >> outfilter_golden;

	   if(outfilter != outfilter_golden){
	     cout << "\nOutput missmatch [line:" << line << "] Golden:" << outfilter_golden << " -- Output:" << outfilter;

	     fprintf(diff_file,"\nOutput missmatch[line:%d] Golden: %d -- Output: %d",line, outfilter_golden, outfilter);
	     
	     errors++;
	   }

          line ++;

    }

    if(errors == 0)
      cout << endl << "Finished simulation SUCCESSFULLY" << endl;
    else
      cout << endl << "Finished simulation " << errors << " MISSMATCHES between Golden and Simulation" << endl;


    fclose(out_filter_file);
    fclose(diff_file);
    fclose(out_filter_golden_file);



}

  SC_CTOR ( test_FIR ) {
 
    SC_CTHREAD(send,clk.pos());
    reset_signal_is(rst,false);

    SC_CTHREAD(recv,clk.pos());
    reset_signal_is(rst,false);
  }

  ~test_FIR(){}

};


#endif  // TB_FIR_H
