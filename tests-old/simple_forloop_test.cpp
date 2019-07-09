#include "systemc.h"
#include "gpumacro.h"
sc_event e1;

SC_MODULE(consumer) {
	sc_in<int> in;

    int field1[25];
    int field2[30];

	void f2() {
		int x;
		while(true) {

            int a[5] = { 0 };
            int b[10] = { 1 };
            int c[15] = { 2 };
            int d[20] = { 3 };

            for (int i = 0; i < 5; ++i) {

                a[i] = b[i * 2];

                a[i] = b[i * 2] + a[i] * 5;

                a[i] = c[i * 3] % (b[i * 2] + i);

                d[i * 4] = a[i] % (c[i * 3] + i);

                field1[i * 5] = a[i];

                field2[i * 6] = a[i];
            }

		}
	}
	SC_CTOR(consumer) {
		SC_THREAD(f2);
	}
};

int sc_main(int argc, char *argv[]) {
	sc_signal<int> s1;
	consumer c1("consumer1");
	c1.in(s1);

    return 0;
}
