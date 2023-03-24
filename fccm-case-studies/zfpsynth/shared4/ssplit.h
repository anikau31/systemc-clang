
#ifndef SSPLIT_H
#define SSPLIT_H

#include "systemc.h"
#include "sc_stream_imp.h"

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

// Parameters
// ----------
// T: data type
// IW[1|2]: FIFO index and counter bit width
// RLEV: reset active level
template <typename T, int IW1 = 1, int IW2 = 1, bool RLEV = false>
SC_MODULE(ssplit)
{
	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_stream_in <T> s_port;
	sc_stream_out<T> m_port1;
	sc_stream_out<T> m_port2;

	/*-------- registers --------*/

	/*-------- channels --------*/
	sc_stream<T> c_port1;
	sc_stream<T> c_port2;

	/*-------- modules --------*/
	sfifo_cc<T,IW1,RLEV> u_que1;
	sfifo_cc<T,IW2,RLEV> u_que2;

	void mc_proc()
	{
		c_port1.data_w(s_port.data_r());
		c_port2.data_w(s_port.data_r());
		c_port1.valid_w(s_port.valid_r() && c_port2.ready_r());
		c_port2.valid_w(s_port.valid_r() && c_port1.ready_r());
		s_port.ready_w(c_port1.ready_r() && c_port2.ready_r());
	}

	SC_CTOR(ssplit) :
		c_port1("c_port1"),
		c_port2("c_port2"),
		u_que1("u_que1"),
		u_que2("u_que2")
	{
		u_que1.clk(clk);
		u_que1.reset(reset);
		u_que1.s_port(c_port1);
		u_que1.m_port(m_port1);

		u_que2.clk(clk);
		u_que2.reset(reset);
		u_que2.s_port(c_port2);
		u_que2.m_port(m_port2);

		SC_METHOD(mc_proc);
			sensitive << s_port.data_chg() << s_port.valid_chg() << c_port1.ready_event() << c_port2.ready_event();
	}

};

#endif // SSPLIT_H
