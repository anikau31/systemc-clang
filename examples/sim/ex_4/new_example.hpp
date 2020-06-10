#include "systemc.h"

SC_MODULE(fa) {
  sc_in<bool> a;
  sc_in<bool> b;
  sc_in<bool> cin;

  sc_out<bool> s;
  sc_out<bool> cout;

  SC_CTOR(fa) {
    SC_THREAD(adder); 
    sensitive << a;
    sensitive << b;
    sensitive << cin;
  }

  void adder(){

    while (true) {

      wait(5, SC_NS); // delay to produce outputs.
      cout = (a & b) | (a & cin) | (b & cin);
      s = a ^ b ^ cin;
    }
  }
};



int sc_main(int ac, char *av[])
{
  sc_signal<bool> cin;
  sc_signal<bool> a0;
  sc_signal<bool> a1;
  sc_signal<bool> a2;
  sc_signal<bool> a3;
  sc_signal<bool> b0;
  sc_signal<bool> b1;
  sc_signal<bool> b2;
  sc_signal<bool> b3;
  sc_signal<bool> s0;
  sc_signal<bool> s1;
  sc_signal<bool> s2;
  sc_signal<bool> s3;
  sc_signal<bool> c0;
  sc_signal<bool> c1;
  sc_signal<bool> c2;
  sc_signal<bool> cout;


  fa ad0("adder0");
  ad0.a(a0);
  ad0.b(b0);
  ad0.cin(cin);
  ad0.s(s0);
  ad0.cout(c0);

  fa ad1("adder1");
  ad1.a(a1);
  ad1.b(b1);
  ad1.cin(c0);
  ad1.s(s1);
  ad1.cout(c1);

  fa ad2("adder2");
  ad2.a(a2);
  ad2.b(b2);
  ad2.cin(c1);
  ad2.s(s2);
  ad2.cout(c2);

  fa ad3("adder3");
  ad3.a(a3);
  ad3.b(b3);
  ad3.cin(c2);
  ad3.s(s3);
  ad3.cout(cout);

  sc_start(200);

  return 0;
}

