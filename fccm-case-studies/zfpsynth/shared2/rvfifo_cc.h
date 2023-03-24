
#ifndef RVFIFO_CC_H
#define RVFIFO_CC_H

#include "systemc.h"
#include "sc_rvd.h"
#include "fifo_cc.h"
#ifdef CHAN_NS
using namespace CHAN_NS;
#endif

// Port & Channel Prefix
// m: port master
// s: port slave
// c: channel
// u: module instance (unit)

// Parameters
// ----------
// T: FIFO data type
// IW: FIFO index and counter bit width
// RLEV: reset active level
template <typename T, int IW = 1,  bool RLEV = false>
class rvfifo_cc :
	public sc_module
{
	static constexpr int MAX_DEPTH = (1 << IW);
	typedef fifo_cc<T,/*IW*/IW,/*FWFT*/true,/*RLEV*/RLEV,/*FLEV*/false> fifo_t;

public:

	sc_in<bool> clk;
	sc_in<bool> reset;

	sc_rvd_in <T> s_port;
	sc_rvd_out<T> m_port;

	fifo_t u_fifo;

	/* * * * * constructors * * * * */

	rvfifo_cc(const sc_module_name& mn_) :
		sc_module(mn_),
		u_fifo("u_fifo")
	{
		u_fifo.clk(clk);
		u_fifo.reset(reset);
		u_fifo.din  (s_port.data);
		u_fifo.wr_en(s_port.valid);
		u_fifo.full (s_port.ready);
		u_fifo.dout (m_port.data);
		u_fifo.rd_en(m_port.ready);
		u_fifo.empty(m_port.valid);
	}

};

#endif // RVFIFO_CC_H
