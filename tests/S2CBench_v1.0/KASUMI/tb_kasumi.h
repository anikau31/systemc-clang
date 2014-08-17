//========================================================================================
// 
//
// File Name    : tb_kasumi.h
// Description  : Testbench
// Release Date : 23/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version  Author        Description
//---------------------------------------------------------------------------------------
//23/07/2013     1.0    PolyU        Kasumi  testbench
//=======================================================================================


#ifndef TB_KASUMI_H
#define TB_KASUMI_H

#include "define.h"

SC_MODULE (test_kasumi){

  // inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in<sc_uint<8> >  odata[DEPTH][WIDTH];

  // outputs
  sc_out<sc_uint<8> >  indata[DEPTH][WIDTH];
  sc_out<sc_uint<8> >  k[8][2];


  //For data feeding
  FILE * in_file, *in_kfile,  *out_golden_file, *out_kasumi_file;
  FILE  *out_file, *diff_file;


void send(){

  // Variables declaration
  int x, y, ret1=0, ret2=0;
  unsigned int  in_read, in_k;

  //Reset routine

  in_file = fopen(INFILENAME, "rt");
  in_kfile = fopen(INFILEKEY, "rt");


  if(!in_file){
    cout << "Could not open " << INFILENAME << endl;
    sc_stop();
  }

  if(!in_kfile){
    cout << "Could not open " << INFILEKEY << endl;
    sc_stop();
  }


  wait();

  while(true){

    do{
      for(x=0; x< DEPTH; x++){
	for(y=0;y< WIDTH;y++){

	  ret1 = fscanf(in_file,"%d", &in_read);
	  if(ret1 == EOF){
	    x= DEPTH; break;
	  }
	    
	  indata[x][y].write(in_read);
	}
      }

      for(x=0; x< 8; x++){
	for(y=0;y< 2;y++){

	  ret2 = fscanf(in_kfile,"%d", &in_k);
	  if(ret2 == EOF){
	    x = 8;
	    break;
	  }

	  k[x][y].write(in_k);
	}
      }
  
    
      if(ret1 == EOF || ret2 == EOF)
	break;

    wait();
    
    }while(1);
 

    fclose(in_file);
    fclose(in_kfile);

    cout << endl << "Starting comparing results " << endl;
 
    compare_results();
    sc_stop();

    wait();

  }//while_loop
}



//-----------------------------
// Receive data thread
//-----------------------------
void recv(){

  // Variables declaration
  unsigned int out_write=0;
  int x=0, y=0;

  out_file = fopen (OUTFILENAME, "wt");

  if(!out_file){
    cout << "Could not open " << OUTFILENAME << endl;
    sc_stop();
    exit(-1);
  }


  wait();

  while(true){

    for(x=0; x< DEPTH; x++){
	for(y=0;y< WIDTH;y++){ 
	  out_write = odata[x][y].read();
	  fprintf(out_file,"%d\n",out_write);
	  wait();
    }
  }
  }
}


//-------------------------------
// Compare results function
//------------------------------
void compare_results(){

  int outkasumi, out_golden, line=1, errors=0;

  // Close file where outputs are stored
  fclose(out_file);

  //  out_filter_file_read.open(OUTFILENAME);
  out_file = fopen (OUTFILENAME, "rt");

  if(!out_file){
    cout << "Could not open " << OUTFILENAME << "\n";
    sc_stop();
  }

    //
    // Read the golden output
    //
    out_golden_file = fopen (OUTFILENAME_GOLDEN, "rt");
    if(!out_golden_file){
      cout << "Could not open " << OUTFILENAME_GOLDEN << "\n";
      sc_stop();
     }

    //
    // Compare SystemC simulation results vs. golden output
    //
    diff_file = fopen (DIFFFILENAME, "w");
    if(!diff_file){
	 cout << "Could not open " << DIFFFILENAME<< "\n";
	 sc_stop();
       }

    while(fscanf(out_golden_file, "%d", &out_golden) != EOF){
      fscanf(out_file,"%d", &outkasumi);
     

      cout << endl <<"Cycle["<< line << "]: " << out_golden << " -- "<< outkasumi;

	   if(outkasumi != out_golden){
	     cout << "\nOutput missmatch [line:" << line << "] Golden:" << out_golden << " -- Output: " << outkasumi;

	     fprintf(diff_file,"\nOutput missmatch[line:%d] Golden: %d -- Output: %d",line, out_golden, outkasumi);
	     
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

  SC_CTOR ( test_kasumi ) {
 
    SC_CTHREAD(send,clk.pos());
    reset_signal_is(rst,false);

    SC_CTHREAD(recv,clk.pos());
    reset_signal_is(rst,false);
  };

  ~test_kasumi(){};

};


#endif // TB_KASUMI_H
