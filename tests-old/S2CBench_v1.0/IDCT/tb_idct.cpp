//========================================================================================
// 
//
// File Name    : tb_idct.cpp
// Description  : Testbench for IDCT
// Release Date : 31/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Author     Version    Description
//----------------------------------------------------------------------------------------
//31/07/2013    PolyU         1.0     IDCT original testbench (send,receive data and compare results)
//
//=======================================================================================
#include "tb_idct.h"


//-------------------
// Send data thread
//------------------
void test_idct::send(){

  // Variables declaration
  int i=0;
  unsigned int  indata, incoef;

  // open input data files
  in_file = fopen(INFILENAME, "rt");

  if(!in_file){
    cout << "Could not open " << INFILENAME << "\n";
    sc_stop();
    exit(EXIT_FAILURE);
  }


  in_file_coef = fopen(INFILENAME_COEF, "rt");

  if(!in_file){
    cout << "Could not open " << INFILENAME_COEF << "\n";
    sc_stop();
    exit(EXIT_FAILURE);
  }

 
  wait();

  while(true){

   
    while(fscanf(in_file,"%u", &indata) != EOF && fscanf(in_file_coef,"%u", &incoef) != EOF){

      cout << endl << "inputs :" << indata << " " << incoef ;
      input_quant.write(indata);
      input_coef.write(incoef);

      wait();
    }
  


    fclose(in_file);
    fclose(in_file_coef);

    cout << endl << "Starting comparing results " << endl;
 
    compare_results();
    sc_stop();

    wait();

  }//while_loop
}



//---------------------------
// Receive data thread
//---------------------------
void test_idct::recv(){

  // Variables declaration
  unsigned int out_sample, out_row, out_col;
  int j=0;
  out_file = fopen (OUTFILENAME, "wt");

  if(!out_file){
    cout << "Could not open " << OUTFILENAME << "\n";
    sc_stop();
    exit (-1);
  }

  wait();

  while(true){

    while(output_start.read() == 0)
      wait();

  
    for ( j = 0; j < 8; j++ ) {
      out_sample = output_sample.read( );
      out_row    = output_row.read();
      out_col    = output_col.read(); 

      fprintf(out_file,"%u %u %u\n",  out_row, out_col, out_sample);   
      wait();
    }
  }
}




//---------------------------
// Compare results function
//---------------------------
void test_idct::compare_results(){

  unsigned int out_idct, out_golden;
  int  line=1, errors=0;

  // Close file where outputs are stored
  fclose(out_file);


  // Open results file
  out_file = fopen (OUTFILENAME, "rt");
  if(!out_file){
    cout << "Could not open " << OUTFILENAME << "\n";
    sc_stop();
    exit(EXIT_FAILURE);
  }



  //
  // Load the golden pattern
  //
  out_file_golden = fopen (OUTFILENAME_GOLDEN, "rt");
  if(!out_file_golden){
    cout << "Could not open " << OUTFILENAME_GOLDEN << "\n";
    sc_stop();
    exit(EXIT_FAILURE);
  }


    //
    // Dump the comparison result
    //
    diff_file = fopen (DIFFFILENAME, "w");
    if(!diff_file){
	 cout << "Could not open " << DIFFFILENAME<< "\n";
	 sc_stop();
       }

    while(fscanf(out_file_golden, "%u", &out_golden) != EOF){

      if(fscanf(out_file,"%u", &out_idct) == EOF)
	break;

      cout << endl <<"Cycle[" << line << "]: " << out_golden << " -- " << out_idct;

      if(out_idct != out_golden ){
	cout << "\nOutput missmatch [line:" << line << "] Golden:" << out_golden << " -- Output:" << out_idct;
	fprintf(diff_file,"\nOutput missmatch[line:%d] Golden: %u -- Output: %d+i%d",line, out_golden, out_idct);   
	errors++;
      }
      
      line ++;

    }

    if(errors == 0)
      cout << endl << "Finished simulation SUCCESSFULLY" << endl;
    else
      cout << endl << "Finished simulation " << errors << " MISSMATCHES between Golden and Simulation" << endl;


    fclose(out_file);
    fclose(diff_file);
    fclose(out_file_golden);



}

