//========================================================================================
// 
//
// File Name    : tb_qsort.h
// Description  : Testbench for Quick sort
// Release Date : 01/08/2013
// Author       : PolyU DARC Group
//                Benjamin Carrion Schafer, Anushree Mahapatra
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Author     Version     Description
//---------------------------------------------------------------------------------------
// 01/08/2013  PolyU       1.0       Quick Sort original testbench
//=======================================================================================

#include "define.h"

SC_MODULE (test_qsort){

  // Inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in<sc_uint<8> > odata; 


  //  Outputs
  sc_out<sc_uint<8> >  indata;


  //For data feeding
  FILE * in_file, *out_golden_file, *out_file_read;
  FILE  *out_file, *diff_file;


void send(){

  // Variables declaration
  int i=0;
  unsigned  in_read;

  //Reset routine
  in_file = fopen(INFILENAME, "rt");


  if(!in_file){
    cout << "Could not open " << INFILENAME << "\n";
    sc_stop();
    exit(EXIT_FAILURE);
  }


  indata.write(0);

  
  wait();

  while(true){

    while(fscanf(in_file,"%d", &in_read) != EOF){
	indata.write(in_read);
	wait();
    }
  
   
    fclose(in_file);
    cout << endl << "Starting comparing results " << endl;
 
    compare_results();
    sc_stop();

    wait();

  }//while_loop
}



//--------------------------
// Receive data thread
//--------------------------
void recv(){

  // Variables declaration
  unsigned int out_write=0;

  out_file = fopen (OUTFILENAME, "wt");

  if(!out_file){
    cout << "Could not open " << OUTFILENAME << "\n";
    sc_stop();
  }


  wait();

  while(true){

    out_write = odata.read();
    fprintf(out_file,"%d\n",out_write);

    //	cout << "\nReading "  << out_write;

    wait();
  }
}


//--------------------------
// Compare results function
//--------------------------
void compare_results(){

  int outsort, out_golden, line=1, errors=0;

  // Close file where outputs are stored
  fclose(out_file);

  out_file = fopen (OUTFILENAME, "rt");

  if(!out_file){
    cout << "Could not open " << OUTFILENAME << "\n";
    sc_stop();
    exit(EXIT_FAILURE);
  }

    //
    //Load the golden pattern
    //
      out_golden_file = fopen (OUTFILENAME_GOLDEN, "rt");
     if(!out_golden_file){
      cout << "Could not open " << OUTFILENAME_GOLDEN << "\n";
      sc_stop();
      exit(EXIT_FAILURE);
     }

    //
    //Dump the comparison result
    //
    diff_file = fopen (DIFFFILENAME, "w");
    if(!diff_file){
	 cout << "Could not open " << DIFFFILENAME<< "\n";
	 sc_stop();
       }

    while(fscanf(out_golden_file, "%d", &out_golden) != EOF){
      fscanf(out_file,"%d", &outsort);
     

      cout << endl <<"Cycle["<< line << "]: " << out_golden << "-- "<< outsort;

      if(outsort != out_golden){
	cout << "\nOutput missmatch [line:" << line << "] Golden:" << out_golden << " -- Output:" << outsort;

	fprintf(diff_file,"\nOutput missmatch[line:%d] Golden: %d -- Output: %d",line, out_golden, outsort);
	
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
    fclose(out_golden_file);



}



  SC_CTOR ( test_qsort ) {
 
    SC_CTHREAD(send,clk.pos());
    reset_signal_is(rst,false);

    SC_CTHREAD(recv,clk.pos());
    reset_signal_is(rst,false);
  }

  ~test_qsort(){}

};
