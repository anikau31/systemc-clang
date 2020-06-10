#include <systemc.h>
#include <iostream>

SC_MODULE(monitor)	{
	sc_in<sc_bv<8> > filter_input, filter_output;

	void prc_monitor()	{
		cout << "At simulation time " << sc_simulation_time() << endl;
		cout << "\tinput: " << filter_input << "\r\n" << "\toutput: " << filter_output << endl;
	}

	SC_CTOR(monitor)	{
		SC_METHOD(prc_monitor);
			sensitive << filter_input << filter_output;
	}
};
