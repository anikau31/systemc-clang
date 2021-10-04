/**
 * Child module signal assignments
 * B_top -> C -> A
 * in1 assigned in C
 * in2 assigned in B_top
 * out connected in B_top
 */

#include "systemc.h"

using namespace sc_core;

// Child module port access -- that is error
class A : public sc_module
{
public:
    sc_signal<bool>     in1{"in1"};
    sc_in<bool>     in2{"in2"};
    sc_out<bool>    out{"out"};


    SC_CTOR(A) {
        SC_METHOD(proc);
        sensitive << in1 << in2;
    }

    void proc() {
      this->out = in1 & in2;
    }

};


class C : public sc_module
{
public:
    A a{"a"};
    sc_signal<bool> s{"s"};
    
    

    SC_CTOR(C)
    {
        SC_METHOD(proc);
        sensitive << s;
    }
    
    void proc() {
        a.in1 = s;
    }
};


class B_top : public sc_module 
{
public:
    C c_mod{"c_mod"};
    sc_signal<bool> s1{"s1"};
    sc_out<bool> opt{"out"};

    SC_CTOR(B_top) {
        c_mod.a.in2(s1);
        this->opt(c_mod.a.out);
    }
};

int sc_main(int argc, char *argv[]) {
    B_top b_mod{"b_mod"};
    sc_start();
    return 0;
}
