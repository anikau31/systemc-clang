
#ifndef SFIFO_CC_H
#define SFIFO_CC_H

#include "systemc.h"
#include "sc_stream_ports.h"
#ifdef CHAN_NS
using namespace CHAN_NS;
#endif

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
// T: FIFO data type
// IW: FIFO index and counter bit width
// RLEV: reset active level
template <typename T, int IW = 1,  bool RLEV = false>
SC_MODULE(sfifo_cc) // Stream FIFO with Common Clock
{
	static constexpr int MAX_DEPTH = (1 << IW);
	const unsigned depth;

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_stream_in <T> s_port;
	sc_stream_out<T> m_port;

	/*-------- registers --------*/
	sc_signal<T> data[MAX_DEPTH];

	sc_signal<sc_uint<IW> > rd_idx;
	sc_signal<sc_uint<IW> > wr_idx;

	sc_signal<bool> full_i;
	sc_signal<bool> empty_i;

	/*-------- channels --------*/
	sc_signal<bool> wr_en_i;
	sc_signal<bool> rd_en_i;

	/*-------- modules --------*/

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

	SC_HAS_PROCESS(sfifo_cc);

	sfifo_cc(const sc_module_name& mn_, int size_ = MAX_DEPTH) :
		sc_module(mn_), depth(size_)
	{
		if (size_ <= 0 || MAX_DEPTH < size_) {
			SC_REPORT_ERROR("FIFO size out of bounds", name());
			return;
		}
		SC_METHOD(mc_proc);
			sensitive << s_port.valid_chg() << m_port.ready_chg() << full_i << empty_i << rd_idx;
			for (int i = 0; i < size_; i++) sensitive << data[i];
		SC_METHOD(ms_proc);
			sensitive << clk.pos();
			dont_initialize();
	}

};

#endif // SFIFO_CC_H
