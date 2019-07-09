#include "systemc.h"

SC_MODULE(producer) {
 sc_fifo_in<int> p1;

 void f1() {
  int i = 0;
  while(1) {
   wait(5, SC_NS);   
   cout<<"\n before Producer time-stamp : " <<sc_time_stamp();
   p1.read(i);
   cout<<"\n after Producer time-stamp : " <<sc_time_stamp();
  }
 }
 SC_CTOR(producer) {
  SC_THREAD(f1);
  sc_fifo<int> p1 (1);
 }
};

SC_MODULE(consumer) {
 sc_fifo_out<int> p2;

 void f2() {
  int i = 0;

  while(1) {
   wait(0, SC_NS);
   cout<<"\n before Consumer time-stamp : " <<sc_time_stamp();
   p2.write(i++);
   cout<<"\n after Consumer time-stamp : " <<sc_time_stamp();
  }
 }
 SC_CTOR(consumer) {
  SC_THREAD(f2);
 
  sc_fifo<int> p2 (1);
 }
};


int sc_main(int argc, char *argv[]) {
  sc_fifo<int> s (1);

  producer p1("producer");
  consumer c1("consumer");

  p1.p1(s);
  c1.p2(s);
  
  sc_start(100, SC_NS);
  
  return 0;

}
