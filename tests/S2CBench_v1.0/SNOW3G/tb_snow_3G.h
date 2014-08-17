//========================================================================================
// 
//
// File Name    : tb_snow_3G.h
// Description  : Testbench
// Release Date : 26/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version  Author             Description
//---------------------------------------------------------------------------------------
//26/07/2013   1.0      PolyU DARC Lab     snow 3G encryption testbench module declaration
//=======================================================================================


#ifndef TB_SNOW_3G_H
#define TB_SNOW_3G_H

#include "define.h"

SC_MODULE (test_snow_3G){

  // inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in<sc_uint<32> >  ks_out[SIZE];

  // outputs
  sc_out<sc_uint<32> >  ks_in[SIZE];




  //For data feeding
  FILE * in_file,  *out_golden_file, *out_file;
  FILE  *out_snow_file, *diff_file;

void send(){

  // Variables declaration
  int i=0, ret=0;
  unsigned int  in_read;

  //Reset routine

  in_file = fopen(INFILENAME, "rt");



  if(!in_file){
    cout << "Could not open " << INFILENAME << "\n";
    sc_stop();
  }


  wait();

  while(true){
   
    do{
      for(i=0; i < SIZE; i ++){
	ret = fscanf(in_file,"%d", &in_read);
	if(ret == EOF){
	  i = SIZE;
	  break;
	}
	
	ks_in[i].write(in_read);
      }

      if(ret == EOF)
	break;

      wait();
    }while(1);
  
      
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
void recv(){

  // Variables declaration
  unsigned int out_write=0;
  int i=0;

  out_file = fopen (OUTFILENAME, "wt");

  if(!out_file){
    cout << "Could not open " << OUTFILENAME << "\n";
    sc_stop();
    exit(-1);
  }


  wait();

  while(true){

    for(i=0; i < SIZE; i ++){
      out_write = ks_out[i].read();
      fprintf(out_file,"%d\n",out_write);
      wait();
    }
  }
}


//----------------------------
// Compare results function
//----------------------------
void compare_results(){

  int outsnow, out_golden, line=1, errors=0;

  // Close file where outputs are stored
  fclose(out_file);

  //  out_filter_file_read.open(OUTFILENAME);
  out_file = fopen (OUTFILENAME, "rt");

  if(!out_file){
    cout << "Could not open " << OUTFILENAME << "\n";
    sc_stop();
  }

    //
    // Load the golden pattern
    //
    out_golden_file = fopen (OUTFILENAME_GOLDEN, "rt");
    if(!out_golden_file){
      cout << "Could not open " << OUTFILENAME_GOLDEN << "\n";
      sc_stop();
     }

    //
    // Compare result with golden output
    //
    diff_file = fopen (DIFFFILENAME, "w");
    if(!diff_file){
	 cout << "Could not open " << DIFFFILENAME<< "\n";
	 sc_stop();
       }

    while(fscanf(out_golden_file, "%d", &out_golden) != EOF){
      fscanf(out_file,"%d", &outsnow);
     

      cout << endl <<"Cycle["<< line << "]: " << out_golden << "-- "<< outsnow;

	   if(outsnow != out_golden){
	     cout << "\nOutput missmatch [line:" << line << "] Golden:" << out_golden << " -- Output:" << outsnow;

	     fprintf(diff_file,"\nOutput missmatch[line:%d] Golden: %d -- Output: %d",line, out_golden, outsnow);
	     
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

  //clk("clk"),
     //rst("rst"),
     // in_data("in_data"),
     // coeff("coeff"),
     //    filter_output("filter_output")

  SC_CTOR ( test_snow_3G ) {
 
    SC_CTHREAD(send,clk.pos());
    sensitive<<rst.neg();
    reset_signal_is(rst,false);

    SC_CTHREAD(recv,clk.pos());
    sensitive<<rst.neg();
    reset_signal_is(rst,false);
  };

  ~test_snow_3G(){};

};


#endif // TB_SNOW_3G_H
