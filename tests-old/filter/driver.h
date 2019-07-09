#include <systemc.h>

SC_MODULE(driver)	{
	sc_out<sc_bv<8> > filter_input;
	sc_out<bool> filter_clk;

	void filter_driver();
	void filter_clk_driver();

	SC_CTOR(driver)	{
		SC_THREAD(filter_driver);
		SC_METHOD(filter_clk_driver);
	}
};
