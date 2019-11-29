
#ifndef SREG_H
#define SREG_H

#include "systemc.h"
#include "sc_stream_ports.h"

// Port & Channel Prefix
// m: port master
// s: port slave
// c: channel
// u: module instance (unit)

enum sreg_t {BYPASS, FWD_REV, FWD, REV};

// Parameters
// ----------
// T: register slice data type
// C: register configuration
// TODO: RLEV: reset active level
template <typename T, sreg_t C /*, bool RLEV = false*/>
class sreg;

//-----------------------------------------------------------------------------
// BYPASS
//-----------------------------------------------------------------------------
template <typename T>
class sreg<T, BYPASS> :
	public sc_module
{
public:

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_stream_in <T> s_port;
	sc_stream_out<T> m_port;

	void mc_io()
	{
		s_port.ready_w(m_port.ready_r());
		m_port.data_w(s_port.data_r());
		m_port.valid_w(s_port.valid_r());
	}

	SC_CTOR(sreg)
	{
		SC_METHOD(mc_io);
			sensitive << s_port.valid_chg() << s_port.data_chg();
			sensitive << m_port.ready_chg();
	}
};

//-----------------------------------------------------------------------------
// FWD
//-----------------------------------------------------------------------------
template <typename T>
class sreg<T, FWD> :
	public sc_module
{
	static constexpr bool RLEV = false;

public:

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_stream_in <T> s_port;
	sc_stream_out<T> m_port;

	/*-------- local --------*/
	sc_signal<bool> c_valid;

	void mc_proc()
	{
		s_port.ready_w(m_port.ready_r() || !c_valid);
		m_port.valid_w(c_valid);
	}

	void ms_proc()
	{
		if (reset == RLEV) {
			m_port.data_w(T());
			c_valid = false;
		} else {
			if (m_port.ready_r() || !c_valid) {
				m_port.data_w(s_port.data_r());
				c_valid = s_port.valid_r();
			}
		}
	}

	SC_CTOR(sreg)
	{
		SC_METHOD(mc_proc);
			sensitive << m_port.ready_chg();
			sensitive << c_valid;
		SC_METHOD(ms_proc);
			sensitive << clk.pos();
			reset_signal_is(reset, RLEV);
			dont_initialize();
	}
};

//-----------------------------------------------------------------------------
// FWD_REV
//-----------------------------------------------------------------------------
template <typename T>
class sreg<T, FWD_REV> :
	public sc_module
{
	static constexpr int  IW = 1;
	static constexpr bool RLEV = false;
	static constexpr unsigned depth = 2;

public:

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_stream_in <T> s_port;
	sc_stream_out<T> m_port;

	/*-------- local --------*/
	sc_signal<T> data[depth];

	sc_signal<sc_uint<IW> > rd_idx;
	sc_signal<sc_uint<IW> > wr_idx;

	sc_signal<bool> wr_en_i;
	sc_signal<bool> rd_en_i;
	sc_signal<bool> full_i;
	sc_signal<bool> empty_i;

	void mc_proc()
	{
		m_port.data_w(data[rd_idx.read()]);

		wr_en_i = s_port.valid_r() && !full_i;
		rd_en_i = m_port.ready_r() && !empty_i;

		s_port.ready_w(!full_i);
		m_port.valid_w(!empty_i);
	}

	void ms_proc()
	{
		sc_uint<IW> wr_inc;
		sc_uint<IW> rd_inc;

		wr_inc = (wr_idx.read() + 1) % depth;
		rd_inc = (rd_idx.read() + 1) % depth;
		if (reset == RLEV) {
			for (unsigned i = 0; i < depth; i++) data[i] = T();
			rd_idx = 0;
			wr_idx = 0;
			full_i = false;
			empty_i = true;
		} else {
			if (wr_en_i) {
				data[wr_idx.read()] = s_port.data_r();
				wr_idx = wr_inc;
				if (!rd_en_i) {
					if (wr_inc == rd_idx) full_i = true;
					empty_i = false;
				}
			}
			if (rd_en_i) {
				rd_idx = rd_inc;
				if (!wr_en_i) {
					full_i = false;
					if (rd_inc == wr_idx) empty_i = true;
				}
			}
		}
	}

	SC_CTOR(sreg)
	{
		SC_METHOD(mc_proc);
			sensitive << s_port.valid_chg() << m_port.ready_chg() << full_i << empty_i << rd_idx;
			for (unsigned i = 0; i < depth; i++) sensitive << data[i];
		SC_METHOD(ms_proc);
			sensitive << clk.pos();
			reset_signal_is(reset, RLEV);
			dont_initialize();
	}

};

#endif // SREG_H
