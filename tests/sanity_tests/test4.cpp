#include "systemc.h"
sc_event e1;
SC_MODULE(producer) {
 sc_out<int*> outPort;

 void f1() {
  int *a = new int [5];
  int x;

  while(1) {
   wait(0, SC_NS);
   cout<<"\n Time stamp : " <<sc_time_stamp();
   for (int i = 0; i<5; i++) {
    a[i] = i*2;    
   }
   outPort.write(a);
   wait(10, SC_NS);
   e1.notify(5, SC_NS);   
  }
 }
 
 SC_CTOR(producer) {
  SC_THREAD(f1);
 }
};

SC_MODULE(consumer) {
 sc_in<int*> inPort;
 int *b;
 void f2() {
  int *a = new int [5];
  int x;

  while(1) {
   wait(e1);
   b = inPort.read();
   if (b[0] == 0) {
    wait(10, SC_NS);
   } 
  }
 }
 
 SC_CTOR(consumer) {
  SC_THREAD(f2);
 }
};


int sc_main(int argc, char *argv[]) {

  sc_signal<int*> s;

  producer p("p");
  consumer c("c");

  p.outPort(s);
  c.inPort(s);

  sc_start();
  return 0;
}
