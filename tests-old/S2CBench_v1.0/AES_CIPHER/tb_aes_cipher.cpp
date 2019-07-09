//========================================================================================
// 
//
// File Name    : tb_aes_cipher.cpp
// Description  : Testbench
// Release Date : 29/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version  Author     Description
//----------------------------------------------------------------------------------------
//14/02/2013      1.0    PolyU      aes cipher original testbench
//
//=======================================================================================
#include "tb_aes_cipher.h"


/*
** Send data thread
*/
void test_aes_cipher::send(){

  // Variables declaration
  int i=0, ret=0;
  unsigned int  in_read, in_read_key;

  //Reset routine
  in_file = fopen(INFILENAME, "rt");

  if(!in_file){
    cout << "Could not open " << INFILENAME << "\n";
    sc_stop();
    exit (-1);
  }

  in_file_key = fopen(INFILENAME_KEY, "rt");

  if(!in_file_key){
    cout << "Could not open " << INFILENAME_KEY << "\n";
    sc_stop();
    exit (-1);
  }

  for(i=0; i < SIZE; i ++){
    idata[i].write(0); 
    fscanf(in_file_key, "%u", &in_read_key);
    input_key[i] = in_read_key;
  }
  i = 0;

  wait();

  while(true){
   
      while(fscanf(in_file,"%u", &in_read) != EOF){
	idata[i].write(in_read);
	ikey[i].write(input_key[i]);
       	i++;
	
	if(i == SIZE){
	  i = 0;
	  wait();
	}
      }
  
  
  
    fclose(in_file);
    fclose(in_file_key);

    cout << endl << "Starting comparing results " << endl;
 
    compare_results();
    sc_stop();

    wait();

  }//while_loop
}



/*
** Receive data thread
*/
void test_aes_cipher::recv(){

  // Variables declaration
  sc_uint<8> out_write[SIZE];

  int i=0;

  out_file = fopen (OUTFILENAME, "wt");

  if(!out_file){
    cout << "Could not open " << OUTFILENAME << "\n";
    sc_stop();
    exit(-1);
  }


  wait();

  while(true){

    for(i=0; i< SIZE; i++){
      out_write[i] = odata[i].read();
      fprintf(out_file,"%u ",(unsigned int)out_write[i]);
    }
    fprintf(out_file,"\n");
    wait();
    }
  
}


/*
** Compare results function
*/
void test_aes_cipher::compare_results(){

  unsigned int outaes_cipher, out_golden, line=1, element=1, errors=0;

  // Close file where outputs are stored
  fclose(out_file);

  //  out_filter_file_read.open(OUTFILENAME);
  out_file = fopen (OUTFILENAME, "rt");

  if(!out_file){
    cout << "Could not open " << OUTFILENAME << "\n";
    sc_stop();
    exit(-1);
  }

    //
    //Load the golden pattern
    //
    out_golden_file = fopen (OUTFILENAME_GOLDEN, "rt");
    if(!out_golden_file){
      cout << "Could not open " << OUTFILENAME_GOLDEN << "\n";
      sc_stop();
    exit(-1);
     }

    //
    //Dump the comparison result
    //
    diff_file = fopen (DIFFFILENAME, "w");
    if(!diff_file){
	 cout << "Could not open " << DIFFFILENAME<< "\n";
	 sc_stop();
       }

    while(fscanf(out_golden_file, "%u", &out_golden) != EOF){
      fscanf(out_file,"%u", &outaes_cipher);
     
      cout << endl <<"Cycle["<< line << "][" << element << "]: " << out_golden << " -- "<< outaes_cipher;

      if(outaes_cipher != out_golden){
	cout << "\nOutput missmatch [line:" << line << "] Golden:" << out_golden << " -- Output:" << outaes_cipher;
	fprintf(diff_file,"\nOutput missmatch[line:%d][%d] Golden: %u -- Output: %u",line,element, out_golden, outaes_cipher);
	  errors++;
      }

      if(element == SIZE){
	element =0;
	line ++;
      }
      element ++;

   }

    if(errors == 0)
      cout << endl << "Finished simulation SUCCESSFULLY" << endl;
    else
      cout << endl << "Finished simulation " << errors << " MISSMATCHES between Golden and Simulation" << endl;


    fclose(out_file);
    fclose(diff_file);
    fclose(out_golden_file);


}

