//========================================================================================
// 
//
// File Name    : tb_qsort.cpp
// Description  : Testbench for bubble sort
// Release Date : 01/08/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra
// 
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version   Author           Description
//----------------------------------------------------------------------------------------
// 01/08/2013    1.0      PolyU DARC Lab  quick sort testbench
//
// g++ -o qsort.exe main.cpp tb_qsort.cpp qsort.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -lm
//=======================================================================================
#include "tb_qsort.h"


//--------------------------
// Send data thread
//-------------------------
void test_qsort::send(){

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
void test_qsort::recv(){

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
void test_qsort::compare_results(){

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

