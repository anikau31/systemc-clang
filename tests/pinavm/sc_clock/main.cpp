#include <systemc.h>

SC_MODULE(compteur)
{
	sc_in<bool> clock;
	sc_in<bool> reset;
	int value;

	void compte()
	{
		if (reset.event() == true) {
			value = 0;
			std::cout << "resetted...\n";	
		} else {
			value++;
			std::cout << value << "\n";
		}
	}

	SC_CTOR(compteur)
	{
		SC_METHOD(compte);
		sensitive << reset;
		sensitive << clock;
	}
};

SC_MODULE(resetter)
{
	sc_out<bool> reset;

	SC_CTOR(resetter)
	{
		SC_THREAD(process);
	}
	
	void process()
	{
		std::cout << "start resetter\n";

		while (true) {
			std::cout << "reset !\n";
			reset.write(true);
			wait(30, SC_NS);

			std::cout << "reset !\n";
			reset.write(false);
			wait(60, SC_NS);
			
		}
	}
};

int
sc_main (int argc , char *argv[]) 
{
	resetter res("resetter");
	compteur cpt("compteur");
	sc_clock clk("clk");
	sc_signal<bool> reset;

	cpt.clock(clk);
	cpt.reset(reset);
	res.reset(reset);

	sc_start(300, SC_NS);
}
