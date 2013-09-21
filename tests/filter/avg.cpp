#include <systemc.h>
#include <iostream>

SC_MODULE(avg)	{
	// port declarations
	sc_in<bool> clk;	// clock signal
	sc_in<sc_bv<8> > i_data;
	sc_out<sc_bv<8> > o_data;

	sc_signal<sc_bv<8> > tap0, tap1, tap2, tap3;
	int coef1, coef2, coef3;
	int prod1, prod2, prod3;
	sc_signal<sc_bv<8> > sum2, sum3; // need the space after the "<8> >"
	
	SC_CTOR(avg) {
		SC_METHOD(first_function);
			sensitive << i_data;
		SC_METHOD(second_function);
			sensitive << clk.pos();
		SC_METHOD(third_function);
			sensitive << tap1 << tap2 << tap3;
		
	}
	
	void first_function(void)	{
		tap0.write(i_data.read());
	}

	void second_function(void)	{
		tap1.write(tap0.read());
		tap2.write(tap1.read());
		tap3.write(tap2.read());
	}

	void third_function(void)	{
		coef1 = 2;
		coef2 = 2;
		coef3 = 2;

		sc_bv<8> tmpTap1 = tap1.read();
		sc_bv<8> tmpTap2 = tap2.read();
		sc_bv<8> tmpTap3 = tap3.read();
//		cout << "\t" << tmpTap1.to_int() << "\t" << tmpTap2.to_int() << "\t" << tmpTap3.to_int() << endl;
			
		prod1 = tmpTap1.to_int() * coef1;
		prod2 = tmpTap2.to_int() * coef2;
		prod3 = tmpTap3.to_int() * coef3;
		cout << "\t" << prod1 << "\t" << prod2 << "\t" << prod3 << endl;

		int result2 = prod1 + prod2;
		int result3 = prod1 + prod2 + prod3;
		sum2.write(result2);
		sum3.write(result3);
//		cout << "\t" << sum2.read().to_int() << "(" << result2 << ")" << "\t" << sum3.read().to_int() << "(" << result3 << ")" << endl;
			  
		o_data = sum3.read();
		}
};

