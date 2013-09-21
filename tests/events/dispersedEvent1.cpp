/*************************************************************************************

This example is a simple producer-consumer-other interation with waits on events.

*************************************************************************************/

#include <systemc.h>
#include <stdio.h>
sc_event e1, e2, e3;
SC_MODULE(producer){
	SC_CTOR(producer) {
		SC_THREAD(f1);
		x = new int[10];
	}
	void f1() {
		while(true) { 
			wait(SC_ZERO_TIME);
			cout<<"TimeStamp: " <<sc_time_stamp();
			for (int i = 0 ; i<10; i++) {
				x[i] = i;
			}
			wait(1, SC_NS);
			cout<<"\nTimeStamp wait(1, SC_NS): " <<sc_time_stamp();
			e1.notify();
			cout<<"\nTimeStamp e1.notify(): " <<sc_time_stamp();
			wait(e3);
			cout<<"\nTimeStamp wait(e3): " <<sc_time_stamp();
			sc_stop();
		}
	}
	~producer(){
		delete [] x;
	}
	private:
		int *x;
};

SC_MODULE(consumer) {

	SC_CTOR(consumer) {
		SC_THREAD(f2);
	}
	void f2() {
		while(true) {
			wait(e1);
			cout<<"\nTimeStamp wait(e1): " <<sc_time_stamp();
			wait(20, SC_NS);
			cout<<"\nTimeStamp wait(20, SC_NS): " <<sc_time_stamp();
			e2.notify();	
			cout<<"\nTimeStamp e2.notfy(): " <<sc_time_stamp();
		}
	}
};

SC_MODULE(other) {
	SC_CTOR(other) {
		SC_THREAD(f3);
	}
	void f3() {
		while(true){
			for(int i = 0 ;i <10; i++) {
				
			}
			wait(e2);
			cout<<"\nTimeStamp wait(e2): " <<sc_time_stamp();
			wait(2, SC_NS);
			cout<<"\nTimeStamp wait(2, SC_NS): " <<sc_time_stamp();
			e3.notify();
			cout<<"\nTimeStamp wait e3.notify(): "<<sc_time_stamp();
		}
	}
};

int sc_main(int argc, char* argv[]) {
	
	producer p1("p1");
	consumer c1("c1");
	other o1("o1");

	sc_start();

}
