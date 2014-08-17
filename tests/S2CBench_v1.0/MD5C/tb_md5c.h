//========================================================================================
// 
//
// File Name    : tb_md5c.h
// Description  : Testbench
// Release Date : 29/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version  Author          Description
//---------------------------------------------------------------------------------------
//29/07/2013     1.0     PolyU DARC Lab  MD5C original testbench
//=======================================================================================


#include "define.h"

SC_MODULE (test_md5c){

  // Inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in< bool >         o_busy;
  sc_in< bool >         o_start;
  sc_in< sc_uint<8> >   o_digest;


  //  Outputs
  sc_out< bool >          i_req;
  sc_out< bool >          i_final;
  sc_out< sc_uint<32> >   i_inputLen;




  //For data feeding
  FILE * in_file, *out_file_golden, *out_file, *diff_file;

void send(){

  // Variables declaration
  int i=0;
  unsigned int inputlen;

  //Reset routine
  in_file = fopen(INFILENAME, "rt");


  if(!in_file){
    cout << "Could not open " << INFILENAME << "\n";
    sc_stop();
    exit(EXIT_FAILURE);
  }

 
  wait();

  while(true){
 
 
    i_final.write(false);
    while(fscanf(in_file,"%u", &inputlen) != EOF){

      i_inputLen.write(inputlen);
      i_req.write(true);  // start sending data to MD5C module
      wait();
      i_final.write(true);

      // Wait until the output has been completely writting out before 
      // feeding new data
      while(o_busy.read() == true)
	wait();


    }
  
    i_req.write(false);  

    fclose(in_file);


    cout << endl << "Starting comparing results " << endl;
 
    compare_results();
    sc_stop();

    wait();

  }//while_loop
}



//------------------------
// Receive data thread
//------------------------
void recv(){

  // Variables declaration
  unsigned int digest, flag_out=0;

  out_file = fopen (OUTFILENAME, "wt");

  if(!out_file){
    cout << "Could not open " << OUTFILENAME << "\n";
    sc_stop();
    exit (-1);
  }

  wait();

  while(true){

    while(o_busy.read() == false)
      wait();
    flag_out = 0;
    while(o_start.read() == true){
     digest = o_digest.read();
     fprintf(out_file,"%u ",digest);
     flag_out = 1;
     wait();
    }    

    if(flag_out == 1)
      fprintf(out_file,"\n");     


    wait();

  }
}


//------------------------
// Compare results function
//------------------------
void compare_results(){

  unsigned int outdigest, outdigest_golden;
  int  line=1, errors=0;

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
  out_file_golden = fopen (OUTFILENAME_GOLDEN, "rt");
  if(!out_file_golden){
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

    while(fscanf(out_file_golden, "%u", &outdigest_golden) != EOF){


      if(fscanf(out_file,"%u", &outdigest) == EOF)
	break;


      cout << endl <<"Cycle["<< line << "]: " << outdigest_golden  << " -- "<< outdigest;

      if(outdigest != outdigest_golden){
	cout << "\nOutput missmatch [line:" << line << "] Golden:" << outdigest_golden << " -- Output:" << outdigest;
	fprintf(diff_file,"\nOutput missmatch[line:%d] Golden: %u -- Output: %u",line, outdigest_golden, outdigest);
	
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

  SC_CTOR ( test_md5c ) {
 
    SC_CTHREAD(send,clk.pos());
    reset_signal_is(rst,false);

    SC_CTHREAD(recv,clk.pos());
    reset_signal_is(rst,false);
  };

  ~test_md5c(){};

};
