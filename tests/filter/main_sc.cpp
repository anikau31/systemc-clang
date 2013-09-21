#include "driver.cpp"
#include "monitor.h"
#include "avg.cpp"

#include <iostream>

int sc_main(int argc, char* argv[])	{
	sc_signal<sc_bv<8> > input, output;
	sc_signal<bool> clk;

	avg newAvg("avger");
	driver newDriver("driver");
	monitor newMonitor("monitor");

	newAvg.clk(clk); newAvg.i_data(input); newAvg.o_data(output);
	newDriver.filter_input(input); newDriver.filter_clk(clk);
	newMonitor.filter_input(input); newMonitor.filter_output(output);

	sc_start(50, SC_NS);
	cout << "done!" << endl;
	return 0;
}
