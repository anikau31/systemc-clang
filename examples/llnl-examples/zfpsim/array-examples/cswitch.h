
#ifndef _CSWITCH_H
#define _CSWITCH_H

#include "systemc.h"

#include "port_axis.h"

// Xilinx specific:
// When depth is (1 >= D >= 11) internal FIFOs
// are implemented using shift registers.

#define QDEPTH 11

template <int N> // constraint: power of 2
SC_MODULE(csw_demux)
{
	sc_in<bool> clk;
	sc_in<bool> reset;

	axi_tc_in  s_port;
	axi_tc_out m_port[N];

	void ct_demux()
	{
		AXI_TC flit;
		wait();
		while (true) {
			flit = s_port.read();
// printf("csw_demux<%d> in tdest:%02X tid:%02X tdata:%08X tlast:%1X\n", N, flit.tdest.to_uint(), flit.tid.to_uint(), flit.tdata.to_uint(), flit.tlast.to_uint());
			// TODO: FIXME: use another template arg to specify a routing algorithm,
			// or specify a routing table.
			if (N == 2) {
				// printf("csw_demux<%d> out port:%d\n", N, flit.tdest.to_uint() & (N-1));
				m_port[flit.tdest.to_uint() & (N-1)].write(flit);
			} else {
				// printf("csw_demux<%d> out port:%d\n", N, (flit.tdest.to_uint() >> 1) & (N-1));
				m_port[(flit.tdest.to_uint() >> 1) & (N-1)].write(flit);
			}
		}
	}

	SC_CTOR(csw_demux)
	{
		SC_CTHREAD(ct_demux, clk.pos());
			reset_signal_is(reset, RLEVEL);
	}

};

// TODO: add arbitration, keep same input port until last flag
template <int N>
SC_MODULE(csw_mux)
{
	sc_in<bool> clk;
	sc_in<bool> reset;

	axi_tc_in  s_port[N];
	axi_tc_out m_port;

	axi_tc      c_queue;
	axi_tc_fifo u_queue;

	void ct_mux()
	{
		bool active = false;
		wait();
		while (true) {
			if (!active)
				wait();
			// printf("csw_mux<%d> wake\n", N);
			active = false;
			for (int i = 0; i < N; i++) // priority encoder & mux
				if (s_port[i].valid_r()) {
					// printf("csw_mux<%d> in port:%d\n", N, i);
					c_queue.write(s_port[i].read());
					active = true;
					break;
				}
		}
	}

	SC_CTOR(csw_mux) :
		c_queue("c_queue"),
		u_queue("u_queue", QDEPTH)
	{
		u_queue.clk(clk);
		u_queue.reset(reset);
		u_queue.s_port(c_queue);
		u_queue.m_port(m_port);

		SC_CTHREAD(ct_mux, clk.pos());
			reset_signal_is(reset, RLEVEL);
	}

};

template <int N>
SC_MODULE(cswitch)
{
	sc_in<bool> clk;
	sc_in<bool> reset;

	axi_tc_in  s_port[N];
	axi_tc_out m_port[N];

	axi_tc xbar[N][N];

	sc_vector<csw_demux<N> > u_csw_demux;
	sc_vector<csw_mux<N> > u_csw_mux;

#if defined(VCD)
	void start_of_simulation()
	{
		extern sc_trace_file *tf;
		for (int i = 0; i < N; i++)
			sc_trace(tf, s_port[i],
				(std::string(name())+".s_port"+std::to_string(i)).c_str());
		for (int i = 0; i < N; i++)
			sc_trace(tf, m_port[i],
				(std::string(name())+".m_port"+std::to_string(i)).c_str());
	}
#endif

	SC_CTOR(cswitch) :
		u_csw_demux("u_csw_demux", N),
		u_csw_mux("u_csw_mux", N)
	{
		for (int i = 0; i < N; i++) {
			u_csw_demux[i].clk(clk);
			u_csw_demux[i].reset(reset);
			u_csw_demux[i].s_port(s_port[i]);
			for (int j = 0; j < N; j++) {
				u_csw_demux[i].m_port[j](xbar[i][j]);
			}

			u_csw_mux[i].clk(clk);
			u_csw_mux[i].reset(reset);
			for (int j = 0; j < N; j++) {
				u_csw_mux[i].s_port[j](xbar[j][i]);
			}
			u_csw_mux[i].m_port(m_port[i]);
		}
	}
};

template <int N>
SC_MODULE(ctree)
{
	sc_in<bool> clk;
	sc_in<bool> reset;

	axi_tc_in  s_root;
	axi_tc_out m_root;

	axi_tc_in  s_port[N];
	axi_tc_out m_port[N];

	csw_demux<N> u_csw_demux;
	csw_mux<N> u_csw_mux;

	SC_CTOR(ctree) :
		u_csw_demux("u_csw_demux"),
		u_csw_mux("u_csw_mux")
	{
		u_csw_demux.clk(clk);
		u_csw_demux.reset(reset);
		u_csw_demux.s_port(s_root);
		for (int i = 0; i < N; i++) {
			u_csw_demux.m_port[i](m_port[i]);
		}

		u_csw_mux.clk(clk);
		u_csw_mux.reset(reset);
		for (int i = 0; i < N; i++) {
			u_csw_mux.s_port[i](s_port[i]);
		}
		u_csw_mux.m_port(m_root);
	}

};

#endif // _CSWITCH_H
