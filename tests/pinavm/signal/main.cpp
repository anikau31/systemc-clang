#include <systemc.h>

SC_MODULE(nand)
{
public:
	sc_out<bool> F;         // output signal ports
	sc_in<bool> B;
	sc_in<bool> A;       // input signal ports

//	SC_HAS_PROCESS(nand);

	void do_nand() {         // a C++ function
		F.write( !(A.read() && B.read()) );
	}

	// constructor for nand
	SC_CTOR(nand) {
		SC_THREAD(do_nand);  // register do_nand with kernel
		sensitive << A << B;  // sensitivity list
	}
};

SC_MODULE(exor)
{
	sc_signal<bool> ASig, BSig, FSig;

	sc_in<bool> A, B;
	sc_out<bool> F;

	nand n1, n2, n3, n4;

	sc_signal<bool> S1;
	sc_signal<bool> S2, S3;

	SC_CTOR(exor) : n1("N1"), n2("N2"), n3("N3"), n4("N4")
	{
		printf("------------------------------- kikoo\n");
		A(ASig);
		B(BSig);
		F(FSig);

		n1.A(ASig);
		n1.B(BSig);
		n1.F(S1);

		n2.A(ASig);
		n2.B(S1);
		n2.F(S2);
		
		n3.A(S1);
		n3.B(BSig);
		n3.F(S3);

		n4.A(S2);
		n4.B(S3);
		n4.F(FSig);
		printf("------------------------------- kikoo\n");

	}
};

SC_MODULE(stim)
{
public:
	sc_out<bool> A, B;
	sc_in<bool> Clk;
//	SC_HAS_PROCESS(stim);

	void StimGen()
	{
		A.write(false);
		B.write(false);
		wait();
		A.write(false);
		B.write(true);
		wait();
		A.write(true);
		B.write(false);
		wait();
		A.write(true);
		B.write(true);
		wait();
//		sc_stop();
	}

	SC_CTOR(stim)
	{
		SC_THREAD(StimGen);
//		sensitive << Clk.pos();
		sensitive << Clk;
	}
};

SC_MODULE(mon)
{
public:
	sc_in<bool> A, B;
	sc_in<bool> F;
	sc_in<bool> Clk;
//	sc_in_clk Clk;

//	SC_HAS_PROCESS(mon);

	void StimGen()
	{
		A.read();
		B.read();
		wait();
		A.read();
		B.read();
		wait();
		A.read();
		B.read();
		wait();
		A.read();
		B.read();
		wait();
//		sc_stop();
	}

	SC_CTOR(mon)
	{
		SC_THREAD(StimGen);
//		sensitive << Clk.pos() << A << B;
		sensitive << A << B;
	}
};

SC_MODULE(gen_clock)
{
	sc_out<bool> out;
	void generate(void) {
		bool val;
		val = out.read();
		out.write(!val);
		//next_trigger(10, SC_NS);
	}

	SC_CTOR(gen_clock) {
		SC_METHOD(generate);
	}
};

int sc_main(int argc, char* argv[])
{
//	sc_clock TestClk("TestClock");
	sc_signal<bool> TestClk;

	gen_clock genclk("GenClk");
	genclk.out.bind(TestClk);
//	sc_clock TestClk("TestClock", 10, SC_NS,0.5);

	exor DUT("exor");

	stim Stim1("Stimulus");
	Stim1.A(DUT.ASig);
	Stim1.B(DUT.BSig);
	Stim1.Clk(TestClk);

	mon Monitor1("Monitor");
	Monitor1.A(DUT.ASig);
	Monitor1.B(DUT.BSig);
	Monitor1.F(DUT.FSig);
	Monitor1.Clk(TestClk);

	sc_start();  // run forever

	return 0;
}
