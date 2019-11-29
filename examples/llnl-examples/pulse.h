#ifndef _PULSE_H
#define _PULSE_H

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

#endif // _PULSE_H
