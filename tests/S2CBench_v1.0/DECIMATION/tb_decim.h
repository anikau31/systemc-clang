//========================================================================================
// 
//
// File Name    : tb_decim.h
// Description  : Testbench
// Release Date : 31/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version   Author       Description
//---------------------------------------------------------------------------------------
// 31/07/2013   1.0      PolyU       Decimation filter stimuli generation module
//=======================================================================================

#define SC_INCLUDE_FX 
#include "define.h"

SC_MODULE (test_decim){

  // Inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in < sc_fixed<19, 2, SC_RND, SC_SAT> > odata;

  // Outputs
   sc_out<bool> load_coeff;  
   sc_out < sc_fixed<19,2,SC_RND,SC_SAT> > indata;

   sc_out< sc_fixed<17,2,SC_RND,SC_SAT> > incoef1[TAPS_STAGE1] ;
   sc_out< sc_fixed<17,2,SC_RND,SC_SAT> > incoef2[TAPS_STAGE2] ;
   sc_out< sc_fixed<17,2,SC_RND,SC_SAT> > incoef3[TAPS_STAGE3] ;
   sc_out< sc_fixed<17,2,SC_RND,SC_SAT> > incoef4[TAPS_STAGE4] ;
   sc_out< sc_fixed<17,2,SC_RND,SC_SAT> > incoef5[TAPS_STAGE5] ;


  //For data feeding

  FILE * in_filter_file, *in_coeff_file, *out_filter_golden_file, *out_filter_file_read;
  FILE  *out_filter_file, *diff_file;

void send(){

  // Variables declaration
  int i=0;
  float coeff_read, in_filter_read;

  float incoef1_read[TAPS_STAGE1] ;
  float incoef2_read[TAPS_STAGE2] ;
  float incoef3_read[TAPS_STAGE3] ;
  float incoef4_read[TAPS_STAGE4] ;
  float incoef5_read[TAPS_STAGE5] ;



  //Reset routine
  in_filter_file = fopen(IN_DATA_FILENAME, "rt");
  in_coeff_file = fopen(IN_COEFF_FILENAME, "rt");


  if(!in_filter_file){
    cout << "Could not open " << IN_DATA_FILENAME << "\n";
    sc_stop();
    exit (-1);
  }


  if(!in_coeff_file){
    cout << "Could not open " << IN_COEFF_FILENAME << "\n";
    sc_stop();
    exit (-1);
  }


  for(i=0; i <TAPS_STAGE1; i ++){
    fscanf(in_coeff_file, "%f", &incoef1_read[i]);
  }
  
  for(i=0; i <TAPS_STAGE2; i ++){
    fscanf(in_coeff_file, "%f", &incoef2_read[i]);
  }

  for(i=0; i <TAPS_STAGE3; i ++){
    fscanf(in_coeff_file, "%f", &incoef3_read[i]);
  }  

  for(i=0; i <TAPS_STAGE4; i ++){
    fscanf(in_coeff_file, "%f", &incoef4_read[i]);
  } 
  for(i=0; i <TAPS_STAGE5; i ++){
    fscanf(in_coeff_file, "%f", &incoef5_read[i]);
  } 


  cout << endl << "Read coefficients from file " << endl;

  wait();

  while(true){

    load_coeff.write(true);
      for(i=0; i <TAPS_STAGE1; i ++)
        incoef1[i].write(incoef1_read[i]);

     for(i=0; i <TAPS_STAGE2; i ++)
      incoef2[i].write(incoef2_read[i]);

    for(i=0; i <TAPS_STAGE3; i ++)
      incoef3[i].write(incoef3_read[i]);

     for(i=0; i <TAPS_STAGE4; i ++)
       incoef4[i].write(incoef4_read[i]);

     for(i=0; i <TAPS_STAGE5; i ++)
       incoef5[i].write(incoef5_read[i]);


   wait();

   load_coeff.write(false);

    while(fscanf(in_filter_file,"%f", &in_filter_read) != EOF){
       	indata.write(in_filter_read);
   
	wait();
    }
  
   
    fclose(in_coeff_file);
    fclose(in_filter_file);

    cout << endl << "Starting comparing results " << endl;
 
    compare_results();
    sc_stop();

    wait();

  }//while_loop
}



//----------------------------
// Receive data thread
//---------------------------
void recv(){

  // Variables declaration
  float filter_out_write=0;

  out_filter_file = fopen (OUTFILENAME, "wt");

  if(!out_filter_file){
    cout << "Could not open " << OUTFILENAME << "\n";
    sc_stop();
    exit (-1);
  }


  wait();

  while(true){
    filter_out_write = odata.read();
    fprintf(out_filter_file,"%f\n",filter_out_write);
    wait();
  }
}


//-----------------------------
// Compare results function
//-----------------------------
void compare_results(){

  float outfilter, outfilter_golden;
  int line=1, errors=0;

  // Close file where outputs are stored
  fclose(out_filter_file);

  // Open results file
  out_filter_file = fopen (OUTFILENAME, "rt");

  if(!out_filter_file){
    cout << "Could not open " << OUTFILENAME << endl;
    sc_stop();
    exit (-1);
  }

    //-----------------------
    //Load the golden pattern
    //-----------------------
    out_filter_golden_file = fopen (OUTFILENAME_GOLDEN, "rt");
    if(!out_filter_golden_file){
      cout << "Could not open " << OUTFILENAME_GOLDEN << endl;
      sc_stop();
      exit (-1);
     }

    //----------------------------
    //Dump the comparison result
    //---------------------------

    diff_file = fopen (DIFFFILENAME, "w");
    if(!diff_file){
	 cout << "Could not open " << DIFFFILENAME<< "\n";
	 sc_stop();
	 exit (-1);
       }

    while(fscanf(out_filter_golden_file, "%f", &outfilter_golden) != EOF){
      fscanf(out_filter_file,"%f", &outfilter);
     

      cout << endl <<"Cycle["<< line << "]: " << outfilter_golden << " -- "<< outfilter;

      // out_filter_file_read >> outfilter;
      // out_filter_golden_file >> outfilter_golden;

	   if(outfilter != outfilter_golden){
	     cout << "\nOutput missmatch [line:" << line << "] Golden:" << outfilter_golden << " -- Output:" << outfilter;

	     fprintf(diff_file,"\nOutput missmatch[line:%d] Golden: %f -- Output: %f",line, outfilter_golden, outfilter);
	     
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




  SC_CTOR ( test_decim ) {
 
    SC_CTHREAD(send,clk.pos());
    sensitive<<rst.neg();
    reset_signal_is(rst,false);

    SC_CTHREAD(recv,clk.pos());
    sensitive<<rst.neg();
    reset_signal_is(rst,false);
  };

  ~test_decim(){};

};
