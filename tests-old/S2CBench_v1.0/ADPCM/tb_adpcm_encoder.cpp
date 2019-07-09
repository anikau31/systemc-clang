//========================================================================================
// 
// File Name    : tb_adpcm_encoder.cpp
// Description  : Testbench for ADPCM encoder
// Release Date : 31/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date          Version     Author      Description
//----------------------------------------------------------------------------------------
//31/07/2013         1.0      PolyU     ADPCM encoder send,receive data and compare results
//
//=======================================================================================
#include "tb_adpcm_encoder.h"


//-------------------
// Send data thread
//------------------
void test_adpcm::send()
{

  // Variables declaration
  int i=0;
  unsigned int  indata;

  //Reset routine
  in_file = fopen(INFILENAME, "rt");


  if(!in_file){
    cout << "Could not open " << INFILENAME << "\n";
    sc_stop();
    exit(EXIT_FAILURE);
  }

 
  wait();

  while(true){

   
    while(fscanf(in_file,"%f", &indata) != EOF){
      idata.write(indata);
      wait();
    }
  

    fclose(in_file);

    cout << endl << "Starting comparing results " << endl;
 
    compare_results();
    sc_stop();

    wait();

  }//while_loop
}



//---------------------------
// Receive data thread
//---------------------------
void test_adpcm::recv()
{

  // Variables declaration
  unsigned int outdata;

  out_file = fopen (OUTFILENAME, "wt");

  if(!out_file){
    cout << "Could not open " << OUTFILENAME << "\n";
    sc_stop();
    exit (-1);
  }

  wait();

  while(true){

    outdata = odata.read();
    fprintf(out_file,"%u ", outdata);

    wait();



  }
}


//---------------------------
// Compare results function
//---------------------------
void test_adpcm::compare_results()
{

  unsigned int out_adpcm, out_golden;
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

      if(fscanf(out_file,"%u", &out_adpcm) == EOF)
	break;

      cout << endl <<"Cycle[" << line << "]: " << out_golden << " -- " << out_adpcm;

      if(out_adpcm != out_golden ){
	cout << "\nOutput missmatch [line:" << line << "] Golden:" << out_golden << " -- Output:" << out_adpcm;
	fprintf(diff_file,"\nOutput missmatch[line:%d] Golden: %u -- Output: %d+i%d",line, out_golden, out_adpcm);   
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

