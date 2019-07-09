//---------------------------------------------------------------------------
//
//
// File Name    : qsort.h
// Description  : Quick sort module declaration
// Release Date : 01/08/13
// Author       : PolyU DARC Group
//                Benjamin Carrion Schafer, Anushree Mahapatra
//
// Revision History
//---------------------------------------------------------------------------
// Date       Version      Author       Description
//---------------------------------------------------------------------------
//            1.0       Darel Rex Finley   Initial version. This public-domain C implementation by
// 01/08/13   1.1       PolyU DARC Group   Conversion into Synthesizable SystemC
//---------------------------------------------------------------------------

#ifndef QSORT_H
#define QSORT_H

#include "define.h"


SC_MODULE( quicksort )
{
    sc_in<bool> clk;
    sc_in<bool> rst;

   sc_in<sc_uint<8> > indata;
   sc_out<sc_uint<8> > odata;

	void run(){
	
  sc_uint<8>  data[SIZE], data_out[SIZE];
  int x;
  
  wait();

  while(1){

     for(x = 0 ; x < SIZE ; x++){
       data[x] = indata.read() ;
       odata.write(data_out[x]);
       wait();
     }

 
     // Main quick sort routine 
     sort(data);

		
      for(x = 0 ;x < SIZE ;x++){
	data_out[x] = data[x];
      }


      //  wait() ;
  }

}

//--------------------------
// Main sorting function
//-------------------------
void sort(sc_uint<8> *arr){


  // Variables declaration
  int  piv, beg[SIZE], end[SIZE], i=0, L, R;

  beg[0]=0;
  end[0]=SIZE;

  while (i>=0) {
    
    L=beg[i]; R=end[i]-1;
    if (L<R) {
      piv=arr[L];
    
      while (L<R) {
        while (arr[R]>=piv && L<R)
	  R--;
	if (L<R)
	  arr[L++]=arr[R];
       
	while (arr[L]<=piv && L<R)
	  L++;
	if(L<R) 
	  arr[R--]=arr[L];
      }
      
      arr[L]=piv;
      beg[i+1]=L+1;
      end[i+1]=end[i];
      end[i++]=L;
      
      if (end[i]-beg[i]>end[i-1]-beg[i-1]){
	swap(&end[i], &beg[i]);

      }
    } 

    else{
      i--;
    }
  } // end while

}


void swap(int *end, int *beg){

  int swap;
  
  swap=*beg;
  *beg=*(beg-1);
  *(beg-1)=swap;

  swap=*end;
  *end=*(end-1); 
  *(end-1)=swap; 



}
   SC_CTOR( quicksort)
     {
       SC_CTHREAD(run, clk.pos()) ;
       reset_signal_is(rst, false) ;
     }

   ~quicksort(){}

};

#endif // QOSRT_H

