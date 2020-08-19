#ifndef PULSE_H
#define PULSE_H

#include "systemc.h"

// Process Prefix
// --------------
// mc: method combinatorial logic
// ms: method sequential logic
// ct: clocked thread
// th: thread

// Parameters
// DELAY : delay before pulse in cycles
// CYCLES: pulse width in cycles
// PLEV  : pulse active level
template <unsigned DELAY, unsigned CYCLES, bool PLEV>
SC_MODULE(pulse) // Produce a Pulse
{
	/*-------- ports --------*/
	sc_in<bool> clk;
	sc_out<bool> sig;

	/*-------- registers --------*/
	enum state_e {INIT_S, PRE_S, PULSE_S, POST_S};
	typedef sc_uint<2> state_t;
	state_t st; // state
	sc_signal<sc_uint<8> > count;

	void ms_pulse()
	{
		bool dec = false;
		switch (st) {
		case INIT_S:
			if (DELAY && CYCLES) {
				st = PRE_S;
				count = DELAY-1;
				sig = !PLEV;
			} else if (CYCLES) {
				st = PULSE_S;
				count = CYCLES-1;
				sig = PLEV;
			} else {
				st = POST_S;
				sig = !PLEV;
			}
			break;
		case PRE_S:
			if (count.read() == 0) {
				st = PULSE_S;
				count = CYCLES-1;
				sig = PLEV;
			} else dec = true;
			break;
		case PULSE_S:
			if (count.read() == 0) {
				st = POST_S;
				sig = !PLEV;
			} else dec = true;
			break;
		case POST_S:
			break;
		}
		if (dec) count = count.read() - 1;
	}

#if defined(VCD)
	void start_of_simulation()
	{
		extern sc_trace_file *tf;
		sc_trace(tf, sig,   (std::string(name())+".sig").c_str());
		sc_trace(tf, st,    (std::string(name())+".st").c_str());
		sc_trace(tf, count, (std::string(name())+".count").c_str());
	}
#endif // VCD

	SC_CTOR(pulse) :
		st(INIT_S)
	{
		SC_METHOD(ms_pulse);
			sensitive << clk.pos();
			dont_initialize();
	}
};

#if 0
template <unsigned DELAY, unsigned CYCLES, bool PLEV>
SC_MODULE(pulse)
{
	sc_in<bool> clk;
	sc_out<bool> sig;

	void ct_pulse()
	{
		if (DELAY > 0) {
			sig = !PLEV;
			wait(DELAY);
		}
		sig = PLEV;
		wait(CYCLES);
		sig = !PLEV;
	}

	SC_CTOR(pulse)
	{
		SC_CTHREAD(ct_pulse, clk.pos());
	}
};
#endif

#endif // PULSE_H
