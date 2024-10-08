/*-----------------------------------------------------------------------------
-- File Name: fifo_cc.h
-- $Revision: 1.00.a $

-- Description: FIFO module with common clock for read and write port.

-- History: at the end of the file
-----------------------------------------------------------------------------*/

#ifndef FIFO_CC_H
#define FIFO_CC_H

#include "systemc.h"

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
// FWFT: first word fall through enable
// RLEV: reset active level
// FLEV: flag (full, empty) active level
template <typename T, int IW, bool FWFT, bool RLEV, bool FLEV>
SC_MODULE(fifo_cc)
{
	static constexpr int MAX_DEPTH = (1 << IW);
	const unsigned depth;

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_in <T>    din;
	sc_in <bool> wr_en;
	sc_out<bool> full;

	sc_out<T>    dout;
	sc_in <bool> rd_en;
	sc_out<bool> empty;

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
		if (FWFT) {
			dout = data[rd_idx.read()];
		}

		wr_en_i = wr_en && !full_i;
		rd_en_i = rd_en && !empty_i;

		full = full_i == FLEV;
		empty = empty_i == FLEV;
	}

	void ms_proc()
	{
		sc_uint<IW> wr_inc;
		sc_uint<IW> rd_inc;

		wr_inc = (wr_idx.read() + 1) % MAX_DEPTH;
		rd_inc = (rd_idx.read() + 1) % MAX_DEPTH;
		if (reset == RLEV) {
			if (!FWFT) dout = T();
			for (unsigned i = 0; i < MAX_DEPTH; i++) data[i] = T();
			rd_idx = 0;
			wr_idx = 0;
			full_i = false;
			empty_i = true;
		} else {
			if (!FWFT) {
				if (rd_en_i) dout = data[rd_idx.read()];
			}
			if (wr_en_i) {
				data[wr_idx.read()] = din;
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

	SC_HAS_PROCESS(fifo_cc);

	fifo_cc(const sc_module_name& mn_, int size_ = MAX_DEPTH) :
		sc_module(mn_), depth(size_)
	{
		// if (size_ <= 0 || MAX_DEPTH < size_) {
		// 	SC_REPORT_ERROR("FIFO size out of bounds", name());
		// 	return;
		// }
		SC_METHOD(mc_proc);
			sensitive << wr_en << rd_en << full_i << empty_i << rd_idx;
			for (int i = 0; i < MAX_DEPTH; i++) sensitive << data[i];
		SC_METHOD(ms_proc);
			sensitive << clk.pos();
			dont_initialize();
	}

};

#endif // FIFO_CC_H

// $Log: fifo_cc.h $
//
