//========================================================================================
// 
// File Name    : tb_adpcm_encoder.h
// Description  : Testbench for adpcm encoder
// Release Date : 31/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra  
//
// Revision History
//---------------------------------------------------------------------------------------
// Date       Version   Author           Description
//---------------------------------------------------------------------------------------
//31/07/2013    1.0    PolyU DARC Lab   ADPCM encoder testbench module declaration
//=======================================================================================


#include "define.h"

SC_MODULE (test_adpcm){

  // Inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in<sc_uint<4> > odata;

  //  Outputs
  sc_out<sc_uint<16> > idata;


  //For data feeding
  FILE * in_file, *out_file_golden, *out_file, *diff_file;


  /* C */
  void compare_results()
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

  /* R */
  void send(){

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



  /* S */
  void recv(){

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


  SC_CTOR ( test_adpcm ) {
 
    SC_CTHREAD(send,clk.pos());
    reset_signal_is(rst,false);

    SC_CTHREAD(recv,clk.pos());
    reset_signal_is(rst,false);
  };

  ~test_adpcm(){};

};
