#include <systemc.h>
#include <stdio.h>
#include <sys/time.h>
#define SIZE 0
#define ITERATIONS 1000
sc_event p, c, d, e;


void compute(int *a, int *b) {
	int i;
	for (i = 0; i<SIZE; i++) {
		b[i] = i*2;
	}
	for(i = 0; i<SIZE; i++) {
		a[i]  = b[i] *2;
	}
}

SC_MODULE(producer){

	sc_out <int> out;
	
	SC_CTOR(producer) {
		SC_THREAD(produce);
	}
	
	void produce() {

	wait(SC_ZERO_TIME);

		int i;
		for(i=0; i<ITERATIONS; i++) {
			p.notify();
			wait(e);
		}

		sc_stop();
	}
};

	SC_MODULE(consumer1) {

		sc_in<int> in;
		sc_out<int> out;

		SC_CTOR(consumer1) {
			SC_THREAD(consume1);	
		}

		void consume1() {
			int i;
		
			int *a;
			int *b;
			struct timeval t1_start, t1_end;
			double t1;
			a = (int*) malloc(SIZE*sizeof(int));
			b = (int*) malloc(SIZE*sizeof(int));

			while(true) {
				
				wait(p);				
				compute(a, b);	
				wait(1, SC_NS);
				gettimeofday(&t1_end,0);
				t1 = (t1_end.tv_sec-t1_start.tv_sec)*1000000 + t1_end.tv_usec - t1_start.tv_usec;
				cout<<"\n Consumer 1: " <<t1;
				out.write(a[1]);
				wait(1, SC_NS);
				d.notify();
			}
		}
	};

	SC_MODULE(consumer2) {

	SC_CTOR(consumer2) {
			SC_THREAD(consume2);
			
		}
		void consume2() {
			int i;
			int *a;
			int *b;
			
			a = (int*) malloc(SIZE*sizeof(int));
			b = (int*) malloc(SIZE*sizeof(int));
			struct timeval t2_start,t2_end;
			double t2;	
			while(true) {
			
				for(int i = 0; i<SIZE; i++) {
					b[i] = i * 2;
					wait(1, SC_NS);

				}
				compute(a, b);	
				wait(1, SC_NS);
				// I have simplified the example in terms of cuda code gen. the above wait was not present. must investigate how this is transformed to cuda code	
				for (int i = 0; i<SIZE; i++) {
					for (int j = 0; j<SIZE; j++) {
						a[i] = a[i] + b[j] *2;
						
					}
					wait(1, SC_NS);
				}	

			}
		}
	};

	SC_MODULE(consumer3) {
		
		SC_CTOR(consumer3) {
			SC_THREAD(consume3);
			
		}
		void consume3() {
			int i;
			int *a;
			int *b;
			
			a = (int*) malloc(SIZE*sizeof(int));
			b = (int*) malloc(SIZE*sizeof(int));
			struct timeval t3_start, t3_end;
			double t3;
			while(true) {

				wait(d);

				for(int i = 0; i<SIZE; i++) {
					b[i] = i * 2;

				}
				compute(a, b);
				for (int i = 0; i<SIZE; i++) {
					for (int j = 0; j<SIZE; j++) {
						a[i] = a[i] + b[j] *2;
						wait(1, SC_NS);
					}
					
				}	
				e.notify();	
			}
		}
	};

int sc_main(int argc, char *argv[]) {

	sc_signal <int> input;
	sc_signal <int> output;
	sc_signal <int> output2;

	producer p1 ("p1");
	p1.out(input);

	consumer1 c1 ("c1");
	c1.in(input);
//	c1.out(output);

	consumer2 c2("c2");

	consumer3 c3 ("c3");

	sc_start ();	
	return 0;
}
