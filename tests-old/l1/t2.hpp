#include <systemc.h>

SC_MODULE(top) {
  sc_in_clk clk;
	SC_CTOR(top) {
	  SC_THREAD(topEntry);
	}


void topEntry() {
while(true) {
	wait(1, SC_NS);
	var++;
	wait(2, SC_MS);	
	--var;
	y = y + 1;
	wait(3, SC_NS);	
	var = var * 2;
}

}

public:
 int var;
 int y;
};

