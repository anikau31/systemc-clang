#include "systemc.h"
//sc_event e1;

template< unsigned D1, unsigned D2>
SC_MODULE(producer){
    int global;
    sc_out <int> out;
    void f1() {
        int i, j;
        while(true) {
        wait(2, SC_NS);
        wait(D1, SC_NS);
        }
    }
    SC_CTOR(producer) {
        SC_THREAD(f1);
    }
};

SC_MODULE(consumer) {
    sc_in<int> in;
    void f2() {
        int x;
        while(true) {

        }
    }
    SC_CTOR(consumer) {
        SC_THREAD(f2);
    }
};

int sc_main(int argc, char *argv[]) {

    sc_signal<int> s1;

    producer<222,333> p1("producer");
//        producer p1("producer");
//	consumer c1("consumer");

    p1.out(s1);
//	c1.in(s1);

    return 0;
}
