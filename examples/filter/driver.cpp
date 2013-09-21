#include "driver.h"

void driver::filter_driver()	{
	filter_input = 0;
	wait(4, SC_NS);
	filter_input = 1;
	wait(4, SC_NS);
	filter_input = 2;
	wait(4, SC_NS);
	filter_input = 3;
	wait(4, SC_NS);
	filter_input = 4;
	wait(4, SC_NS);
	filter_input = 5;
	wait(4, SC_NS);
	filter_input = 6;
	wait(4, SC_NS);
}

void driver::filter_clk_driver()	{
	filter_clk = not(filter_clk);
	cout << sc_simulation_time() << " " << "clk = " << filter_clk << endl;
	next_trigger(4, SC_NS);
}
