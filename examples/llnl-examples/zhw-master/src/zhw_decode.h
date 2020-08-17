
#ifndef ZHW_DECODE_H
#define ZHW_DECODE_H

#include "systemc.h"
#include "sc_stream_imp.h"
#include "zhw.h"

// Process Prefix
// --------------
// mc: method combinatorial logic
// ms: method sequential logic
// ct: clocked thread
// th: thread

// Port & Channel Prefix
// ---------------------
// m: port master
// s: port slave
// c: channel
// u: module instance (unit)

namespace zhw {

//-----------------------------------------------------------------------------
// Decode stream to IEEE floating-point values
//-----------------------------------------------------------------------------
template<typename FP, int DIM, typename B>
SC_MODULE(decode)
{
	typedef typename FP::expo_t expo_t;
	typedef typename FP::si_t si_t;
	typedef typename FP::ui_t ui_t;

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_in <config_t> maxbits;
	sc_in <config_t> minbits;
	sc_in <config_t> maxprec;
	sc_in <config_t> minexp;

	sc_stream_in <B>  s_bits;
	sc_stream_out<FP> m_fp;

	/*-------- registers --------*/
	/*-------- channels --------*/
	/*-------- modules --------*/

	SC_CTOR(decode) :
	{
	}

};

} // namespace zhw

#endif // ZHW_DECODE_H
