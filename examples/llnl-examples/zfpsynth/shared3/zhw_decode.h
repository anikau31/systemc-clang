
#ifndef ZHW_DECODE_H
#define ZHW_DECODE_H

#include "systemc.h"
#include "sc_stream_imp.h"
#include "zhw.h"
#include "ssplit.h"

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

//1: Declare and define modules
//		-
//		-
//		-

//-----------------------------------------------------------------------------
// reverse lifting transform of 4-vector
//-----------------------------------------------------------------------------
template<typename FP>
SC_MODULE(inv_lift)
{
	static constexpr int stages = 6;

	typedef typename FP::si_t si_t;

	sc_in<bool> clk;
	sc_in<bool> reset;

	//-------- ports --------
	sc_in <si_t> s_port[4];
	sc_in <bool> s_valid;
	sc_out<bool> s_ready;

	sc_out<si_t> m_port[4];
	sc_out<bool> m_valid;
	sc_in <bool> m_ready;

	//-------- registers --------
	sc_signal<si_t> x[stages];
	sc_signal<si_t> y[stages];
	sc_signal<si_t> z[stages];
	sc_signal<si_t> w[stages];
	sc_signal<sc_bv<stages> > v; // valid bit for each stage

	void mc_proc()
	{
		s_ready.write(m_ready.read());
		m_port[0].write(x[stages-1]);
		m_port[1].write(y[stages-1]);
		m_port[2].write(z[stages-1]);
		m_port[3].write(w[stages-1]);
		m_valid.write(v.read()[stages-1].to_bool());
	}

	void ms_proc()
	{
		if (reset == RLEVEL) {
			for (int i = 0; i < stages; i++) {
				x[i].write(0); y[i].write(0); z[i].write(0); w[i].write(0);
			}
			v.write(0);
		} else if (m_ready.read()) {

			w[0].write( (s_port[3].read())	);
			x[0].write( (s_port[0].read())	);
			y[0].write( (s_port[1].read() + (s_port[3].read() >>1 ))	);
			z[0].write( (s_port[2].read())	);

			w[1].write( (w[0].read() - (y[0].read() >> 1))	);
			x[1].write( (x[0].read())	);
			y[1].write( (y[0].read())	);
			z[1].write( (z[0].read())	);

			w[2].write( (w[1].read())	);
			x[2].write( (x[1].read())	);
			y[2].write( (y[1].read() + w[1].read())	);
			z[2].write( (z[1].read() + x[1].read())	);

			w[3].write( ((w[2].read() << 1) - y[2].read())	);
			x[3].write( ((x[2].read() << 1) - z[2].read())	);
			y[3].write( (y[2].read())	);
			z[3].write( (z[2].read())	);

			w[4].write( (w[3].read() + x[3].read())	);
			x[4].write( (x[3].read())	);
			y[4].write( (y[3].read() + z[3].read())	);
			z[4].write( (z[3].read())	);

			w[5].write( (w[4].read())	);
			x[5].write( ((x[4].read() << 1) - w[4].read())	);
			y[5].write( (y[4].read())	);
			z[5].write( ((z[4].read() << 1) - y[4].read())	);

			v.write((v.read() << 1) | s_valid.read());
		}
	}

#if defined(VCD)
	void start_of_simulation()
	{
		for (int i = 0; i < 4; i++)
			sc_trace(tf, s_port[i],
				(std::string(name())+".s_port"+std::to_string(i)).c_str());
		sc_trace(tf, s_valid, (std::string(name())+".s_valid").c_str());
		sc_trace(tf, s_ready, (std::string(name())+".s_ready").c_str());
		for (int i = 0; i < 4; i++)
			sc_trace(tf, m_port[i],
				(std::string(name())+".m_port"+std::to_string(i)).c_str());
		sc_trace(tf, m_valid, (std::string(name())+".m_valid").c_str());
		sc_trace(tf, m_ready, (std::string(name())+".m_ready").c_str());
		sc_trace(tf, v, (std::string(name())+".v").c_str());
	}
#endif

	SC_CTOR(inv_lift)
	{
		SC_METHOD(mc_proc);
			sensitive << m_ready;
			sensitive << x[stages-1] << y[stages-1] << z[stages-1] << w[stages-1];
			sensitive << v;
		SC_METHOD(ms_proc)
			sensitive << clk.pos();
			dont_initialize();
	}

};

//-----------------------------------------------------------------------------
// decode block of integers
//-----------------------------------------------------------------------------
template<typename FP, int DIM>
struct decode_block;

//--------------------------- 1D CASE --------------------------------
template<typename FP>
struct decode_block<FP, 1> : sc_module
{
	typedef typename FP::si_t si_t;
	typedef typename FP::ui_t ui_t;

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_in <ui_t> s_block[fpblk_sz(1)];
	sc_in <bool> s_valid;
	sc_out<bool> s_ready;

	sc_out<si_t> m_block[fpblk_sz(1)];
	sc_out<bool> m_valid;
	sc_in <bool> m_ready;

	/*-------- registers --------*/
	sc_signal<bool> xors_valid;		//xor pipeline stage valid

	/*-------- channels --------*/
	sc_signal<si_t> c_xt_data[4];
	sc_signal<bool> c_xt_valid;
	sc_signal<bool> c_xt_ready;


	/*-------- modules --------*/
	inv_lift<FP> u_xt;

	void mc_proc()
	{
		s_ready.write(m_ready.read()); // bypass u_xt and u_yt s_ready
	}

	void ms_rev_order()
	{
		if (reset == RLEVEL)			//On reset, clear xor_control flow register
		{
			xors_valid.write(false);
			m_valid.write(c_xt_valid);	//forward xt transform valid out of decode_block.
		}
		else							//normal flow
		{
			if (m_ready.read()) {
				c_xt_data[0].write((s_block[ 0].read() ^ NBMASK) - NBMASK);
				c_xt_data[1].write((s_block[ 1].read() ^ NBMASK) - NBMASK);
				c_xt_data[2].write((s_block[ 2].read() ^ NBMASK) - NBMASK);
				c_xt_data[3].write((s_block[ 3].read() ^ NBMASK) - NBMASK);

				xors_valid.write(s_valid.read());	//Tell permutation stages this data is good.
			}
		}
	}

#if defined(VCD)
	void start_of_simulation()
	{
		for (int i = 0; i < fpblk_sz(1); i++)
			sc_trace(tf, s_block[i],
				(std::string(name())+".s_block"+std::to_string(i)).c_str());
		sc_trace(tf, s_valid, (std::string(name())+".s_valid").c_str());
		sc_trace(tf, s_ready, (std::string(name())+".s_ready").c_str());

		for (int i = 0; i < fpblk_sz(1); i++)
			sc_trace(tf, m_block[i],
				(std::string(name())+".m_block"+std::to_string(i)).c_str());
		sc_trace(tf, m_valid, (std::string(name())+".m_valid").c_str());
		sc_trace(tf, m_ready, (std::string(name())+".m_ready").c_str());

		sc_trace(tf, c_xt_valid, (std::string(name())+".c_xt_valid").c_str());
	}
#endif

	SC_CTOR(decode_block) :
		u_xt("u_xt")
	{

		u_xt.clk(clk);
		u_xt.reset(reset);
		u_xt.s_valid(xors_valid); //wait until xors are done before u_xt starts.
		u_xt.s_ready(c_xt_ready); // output not used
		u_xt.m_valid(c_xt_valid);
		u_xt.m_ready(m_ready);


		//connect: permutation  ->lift  ->output
		//where:   c_xt_data[]->u_xt->m_block
		for (int i = 0; i < 4; i++)
		{
			u_xt.s_port[i](s_block[i]);	// in x
			u_xt.m_port[i](c_xt_data[i]);
		}

		SC_METHOD(mc_proc);
			sensitive << m_ready;
			sensitive << c_xt_valid;
		SC_METHOD(ms_rev_order);
			sensitive << clk.pos();
			dont_initialize();
	}

};

//--------------------------- 2D CASE --------------------------------
template<typename FP>
struct decode_block<FP, 2> : sc_module
{
	typedef typename FP::si_t si_t;
	typedef typename FP::ui_t ui_t;

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_in <ui_t> s_block[fpblk_sz(2)];
	sc_in <bool> s_valid;
	sc_out<bool> s_ready;

	sc_out<si_t> m_block[fpblk_sz(2)];
	sc_out<bool> m_valid;
	sc_in <bool> m_ready;

	/*-------- registers --------*/
	sc_signal<bool> xors_valid;		//xor pipeline stage valid

	/*-------- channels --------*/
	sc_signal<si_t> c_xt_data[4][4];
	sc_signal<bool> c_xt_valid[4];
	sc_signal<bool> c_xt_ready[4];

	sc_signal<si_t> c_yt_data[4][4];
	sc_signal<bool> c_yt_valid[4];
	sc_signal<bool> c_yt_ready[4];

	/*-------- modules --------*/
	inv_lift<FP> u_xt[4];
	inv_lift<FP> u_yt[4];

	void mc_proc()
	{
		s_ready.write(m_ready.read()); // bypass u_xt and u_yt s_ready
		m_valid.write(c_xt_valid[0]);	//forward xt transform valid out of decode_block.
	}

	void ms_rev_order()
	{
		if (reset == RLEVEL)			//On reset, clear xor_control flow register
		{
			xors_valid.write(false);
		}
		else							//normal flow
		{
			if (m_ready.read()) {
				c_yt_data[0][0].write((s_block[ 0].read() ^ NBMASK) - NBMASK);
				c_yt_data[0][1].write((s_block[ 1].read() ^ NBMASK) - NBMASK);
				c_yt_data[1][0].write((s_block[ 2].read() ^ NBMASK) - NBMASK);
				c_yt_data[1][1].write((s_block[ 3].read() ^ NBMASK) - NBMASK);
				c_yt_data[0][2].write((s_block[ 4].read() ^ NBMASK) - NBMASK);
				c_yt_data[2][0].write((s_block[ 5].read() ^ NBMASK) - NBMASK);
				c_yt_data[1][2].write((s_block[ 6].read() ^ NBMASK) - NBMASK);
				c_yt_data[2][1].write((s_block[ 7].read() ^ NBMASK) - NBMASK);
				c_yt_data[0][3].write((s_block[ 8].read() ^ NBMASK) - NBMASK);
				c_yt_data[3][0].write((s_block[ 9].read() ^ NBMASK) - NBMASK);
				c_yt_data[2][2].write((s_block[10].read() ^ NBMASK) - NBMASK);
				c_yt_data[1][3].write((s_block[11].read() ^ NBMASK) - NBMASK);
				c_yt_data[3][1].write((s_block[12].read() ^ NBMASK) - NBMASK);
				c_yt_data[2][3].write((s_block[13].read() ^ NBMASK) - NBMASK);
				c_yt_data[3][2].write((s_block[14].read() ^ NBMASK) - NBMASK);
				c_yt_data[3][3].write((s_block[15].read() ^ NBMASK) - NBMASK);

				xors_valid.write(s_valid.read());	//Tell permutation stages this data is good.
			}
		}
	}

#if defined(VCD)
	void start_of_simulation()
	{
		for (int i = 0; i < fpblk_sz(2); i++)
			sc_trace(tf, s_block[i],
				(std::string(name())+".s_block"+std::to_string(i)).c_str());
		sc_trace(tf, s_valid, (std::string(name())+".s_valid").c_str());
		sc_trace(tf, s_ready, (std::string(name())+".s_ready").c_str());

		for (int i = 0; i < fpblk_sz(2); i++)
			sc_trace(tf, m_block[i],
				(std::string(name())+".m_block"+std::to_string(i)).c_str());
		sc_trace(tf, m_valid, (std::string(name())+".m_valid").c_str());
		sc_trace(tf, m_ready, (std::string(name())+".m_ready").c_str());

		sc_trace(tf, c_xt_valid[0], (std::string(name())+".c_xt_valid0").c_str());
		sc_trace(tf, c_yt_valid[0], (std::string(name())+".c_yt_valid0").c_str());
	}
#endif

	SC_CTOR(decode_block) :
		  u_xt{{"u_xt_0"}, {"u_xt_1"}, {"u_xt_2"}, {"u_xt_3"}},
		  u_yt{{"u_yt_0"}, {"u_yt_1"}, {"u_yt_2"}, {"u_yt_3"}}
	{
		for (int j = 0; j < 4; j++) {
			u_xt[j].clk(clk);
			u_xt[j].reset(reset);
			u_xt[j].s_valid(c_yt_valid[0]);
			u_xt[j].s_ready(c_xt_ready[j]); // output not used
			u_xt[j].m_valid(c_xt_valid[j]); // only index [0] used
			u_xt[j].m_ready(m_ready);

			u_yt[j].clk(clk);
			u_yt[j].reset(reset);
			u_yt[j].s_valid(xors_valid);	//This permutation step starts after xor arithmetic
			u_yt[j].s_ready(c_yt_ready[j]); // output not used
			u_yt[j].m_valid(c_yt_valid[j]); // only index [0] used
			u_yt[j].m_ready(m_ready);


			//connect: permutation  ->lift  ->permutation  ->lift  ->output
			//where:   c_yt_data[][]->u_yt[]->c_xt_data[][]->u_xt[]->m_block
			for (int i = 0; i < 4; i++)
			{
				u_xt[j].s_port[i](c_xt_data[j][i]); // in x
				u_xt[j].m_port[i](m_block[(j*4)+i]);

				u_yt[j].s_port[i](c_yt_data[i][j]); // in y
				u_yt[j].m_port[i](c_xt_data[i][j]);
			}

		}

		SC_METHOD(mc_proc);
			sensitive << m_ready;
			sensitive << c_xt_valid[0];
		SC_METHOD(ms_rev_order);
			sensitive << clk.pos();
			dont_initialize();
	}

};

//------------------------- 3D CASE -----------------------------
template<typename FP>
struct decode_block<FP, 3> : sc_module
{
	typedef typename FP::si_t si_t;
	typedef typename FP::ui_t ui_t;

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_in <ui_t> s_block[fpblk_sz(3)];
	sc_in <bool> s_valid;
	sc_out<bool> s_ready;

	sc_out<si_t> m_block[fpblk_sz(3)];
	sc_out<bool> m_valid;
	sc_in <bool> m_ready;

	/*-------- registers --------*/
	sc_signal<bool> xors_valid;		//xor pipeline stage valid

	/*-------- channels --------*/
	sc_signal<si_t> c_xt_data[4][4][4];
	sc_signal<bool> c_xt_valid[4][4];
	sc_signal<bool> c_xt_ready[4][4];

	sc_signal<si_t> c_yt_data[4][4][4];
	sc_signal<bool> c_yt_valid[4][4];
	sc_signal<bool> c_yt_ready[4][4];

	sc_signal<si_t> c_zt_data[4][4][4];
	sc_signal<bool> c_zt_valid[4][4];
	sc_signal<bool> c_zt_ready[4][4];

	/*-------- modules --------*/
	// sc_vector4<sc_vector4<fwd_lift<FP> > > u_xt;
	// sc_vector4<sc_vector4<fwd_lift<FP> > > u_yt;
	// sc_vector4<sc_vector4<fwd_lift<FP> > > u_zt;
	inv_lift<FP>  u_xt[4][4];
	inv_lift<FP>  u_yt[4][4];
	inv_lift<FP>  u_zt[4][4];

	void mc_proc()
	{
		s_ready.write(m_ready.read()); // bypass u_xt, u_yt, & u_zt s_ready
		m_valid.write(c_xt_valid[0]);	//forward xt transform valid out of decode_block.
	}

	void ms_rev_order()
	{
		if (reset == RLEVEL)			//On reset, clear xor_control flow register
		{
			xors_valid.write(false);
		}
		else							//normal flow
		{
			if (m_ready.read())
			{
				c_zt_data[0][0][0].write((s_block[ 0].read() ^ NBMASK) - NBMASK);
				c_zt_data[0][0][1].write((s_block[ 1].read() ^ NBMASK) - NBMASK);
				c_zt_data[0][1][0].write((s_block[ 2].read() ^ NBMASK) - NBMASK);
				c_zt_data[1][0][0].write((s_block[ 3].read() ^ NBMASK) - NBMASK);
				c_zt_data[1][1][0].write((s_block[ 4].read() ^ NBMASK) - NBMASK);
				c_zt_data[1][0][1].write((s_block[ 5].read() ^ NBMASK) - NBMASK);
				c_zt_data[0][1][1].write((s_block[ 6].read() ^ NBMASK) - NBMASK);
				c_zt_data[0][0][2].write((s_block[ 7].read() ^ NBMASK) - NBMASK);
				c_zt_data[0][2][0].write((s_block[ 8].read() ^ NBMASK) - NBMASK);
				c_zt_data[2][0][0].write((s_block[ 9].read() ^ NBMASK) - NBMASK);
				c_zt_data[1][1][1].write((s_block[10].read() ^ NBMASK) - NBMASK);
				c_zt_data[0][1][2].write((s_block[11].read() ^ NBMASK) - NBMASK);
				c_zt_data[1][0][2].write((s_block[12].read() ^ NBMASK) - NBMASK);
				c_zt_data[1][2][0].write((s_block[13].read() ^ NBMASK) - NBMASK);
				c_zt_data[0][2][1].write((s_block[14].read() ^ NBMASK) - NBMASK);
				c_zt_data[2][0][1].write((s_block[15].read() ^ NBMASK) - NBMASK);
				c_zt_data[2][1][0].write((s_block[16].read() ^ NBMASK) - NBMASK);
				c_zt_data[0][0][3].write((s_block[17].read() ^ NBMASK) - NBMASK);
				c_zt_data[0][3][0].write((s_block[18].read() ^ NBMASK) - NBMASK);
				c_zt_data[3][0][0].write((s_block[19].read() ^ NBMASK) - NBMASK);
				c_zt_data[1][1][2].write((s_block[20].read() ^ NBMASK) - NBMASK);
				c_zt_data[1][2][1].write((s_block[21].read() ^ NBMASK) - NBMASK);
				c_zt_data[2][1][1].write((s_block[22].read() ^ NBMASK) - NBMASK);
				c_zt_data[2][2][0].write((s_block[23].read() ^ NBMASK) - NBMASK);
				c_zt_data[2][0][2].write((s_block[24].read() ^ NBMASK) - NBMASK);
				c_zt_data[0][2][2].write((s_block[25].read() ^ NBMASK) - NBMASK);
				c_zt_data[0][1][3].write((s_block[26].read() ^ NBMASK) - NBMASK);
				c_zt_data[1][0][3].write((s_block[27].read() ^ NBMASK) - NBMASK);
				c_zt_data[1][3][0].write((s_block[28].read() ^ NBMASK) - NBMASK);
				c_zt_data[0][3][1].write((s_block[29].read() ^ NBMASK) - NBMASK);
				c_zt_data[3][0][1].write((s_block[30].read() ^ NBMASK) - NBMASK);
				c_zt_data[3][1][0].write((s_block[31].read() ^ NBMASK) - NBMASK);
				c_zt_data[2][2][1].write((s_block[32].read() ^ NBMASK) - NBMASK);
				c_zt_data[2][1][2].write((s_block[33].read() ^ NBMASK) - NBMASK);
				c_zt_data[1][2][2].write((s_block[34].read() ^ NBMASK) - NBMASK);
				c_zt_data[1][1][3].write((s_block[35].read() ^ NBMASK) - NBMASK);
				c_zt_data[1][3][1].write((s_block[36].read() ^ NBMASK) - NBMASK);
				c_zt_data[3][1][1].write((s_block[37].read() ^ NBMASK) - NBMASK);
				c_zt_data[0][2][3].write((s_block[38].read() ^ NBMASK) - NBMASK);
				c_zt_data[2][0][3].write((s_block[39].read() ^ NBMASK) - NBMASK);
				c_zt_data[2][3][0].write((s_block[40].read() ^ NBMASK) - NBMASK);
				c_zt_data[0][3][2].write((s_block[41].read() ^ NBMASK) - NBMASK);
				c_zt_data[3][0][2].write((s_block[42].read() ^ NBMASK) - NBMASK);
				c_zt_data[3][2][0].write((s_block[43].read() ^ NBMASK) - NBMASK);
				c_zt_data[2][2][2].write((s_block[44].read() ^ NBMASK) - NBMASK);
				c_zt_data[1][2][3].write((s_block[45].read() ^ NBMASK) - NBMASK);
				c_zt_data[2][1][3].write((s_block[46].read() ^ NBMASK) - NBMASK);
				c_zt_data[2][3][1].write((s_block[47].read() ^ NBMASK) - NBMASK);
				c_zt_data[1][3][2].write((s_block[48].read() ^ NBMASK) - NBMASK);
				c_zt_data[3][1][2].write((s_block[49].read() ^ NBMASK) - NBMASK);
				c_zt_data[3][2][1].write((s_block[50].read() ^ NBMASK) - NBMASK);
				c_zt_data[3][3][0].write((s_block[51].read() ^ NBMASK) - NBMASK);
				c_zt_data[3][0][3].write((s_block[52].read() ^ NBMASK) - NBMASK);
				c_zt_data[0][3][3].write((s_block[53].read() ^ NBMASK) - NBMASK);
				c_zt_data[2][2][3].write((s_block[54].read() ^ NBMASK) - NBMASK);
				c_zt_data[2][3][2].write((s_block[55].read() ^ NBMASK) - NBMASK);
				c_zt_data[3][2][2].write((s_block[56].read() ^ NBMASK) - NBMASK);
				c_zt_data[3][3][1].write((s_block[57].read() ^ NBMASK) - NBMASK);
				c_zt_data[3][1][3].write((s_block[58].read() ^ NBMASK) - NBMASK);
				c_zt_data[1][3][3].write((s_block[58].read() ^ NBMASK) - NBMASK);
				c_zt_data[3][3][2].write((s_block[60].read() ^ NBMASK) - NBMASK);
				c_zt_data[3][2][3].write((s_block[61].read() ^ NBMASK) - NBMASK);
				c_zt_data[2][3][3].write((s_block[62].read() ^ NBMASK) - NBMASK);
				c_zt_data[3][3][3].write((s_block[63].read() ^ NBMASK) - NBMASK);

				xors_valid.write(s_valid.read());	//Tell permutation stages this data is good.
			}
		}
	}

#if defined(VCD)
	void start_of_simulation()
	{
		for (int i = 0; i < fpblk_sz(3); i++)
			sc_trace(tf, s_block[i],
				(std::string(name())+".s_block"+std::to_string(i)).c_str());
		sc_trace(tf, s_valid, (std::string(name())+".s_valid").c_str());
		sc_trace(tf, s_ready, (std::string(name())+".s_ready").c_str());

		for (int i = 0; i < fpblk_sz(3); i++)
			sc_trace(tf, m_block[i],
				(std::string(name())+".m_block"+std::to_string(i)).c_str());
		sc_trace(tf, m_valid, (std::string(name())+".m_valid").c_str());
		sc_trace(tf, m_ready, (std::string(name())+".m_ready").c_str());

#if 0
		for (int k = 0; k < 4; k++) {
			for (int j = 0; j < 4; j++) {
				for (int i = 0; i < 4; i++) {
					sc_trace(tf, c_xt_data[k][j][i],
						(std::string(name())+".c_xt_data"+
						std::to_string(k)+
						std::to_string(j)+
						std::to_string(i)
						).c_str());
					sc_trace(tf, c_yt_data[k][j][i],
						(std::string(name())+".c_yt_data"+
						std::to_string(k)+
						std::to_string(j)+
						std::to_string(i)
						).c_str());
					sc_trace(tf, c_zt_data[k][j][i],
						(std::string(name())+".c_zt_data"+
						std::to_string(k)+
						std::to_string(j)+
						std::to_string(i)
						).c_str());
				}
			}
		}
#endif

		sc_trace(tf, c_xt_valid[0][0], (std::string(name())+".c_xt_valid00").c_str());
		sc_trace(tf, c_yt_valid[0][0], (std::string(name())+".c_yt_valid00").c_str());
		sc_trace(tf, c_zt_valid[0][0], (std::string(name())+".c_zt_valid00").c_str());
	}
#endif

	SC_CTOR(decode_block) :
	  // u_xt("u_xt"),
	  // u_yt("u_yt"),
	  // u_zt("u_zt")
	  u_xt{
	  	{{"u_xt_0_0"}, {"u_xt_0_1"}, {"u_xt_0_2"}, {"u_xt_0_3"}},
	  		{{"u_xt_1_0"}, {"u_xt_1_1"}, {"u_xt_1_2"}, {"u_xt_1_3"}},
	  		{{"u_xt_2_0"}, {"u_xt_2_1"}, {"u_xt_2_2"}, {"u_xt_2_3"}},
	  		{{"u_xt_3_0"}, {"u_xt_3_1"}, {"u_xt_3_2"}, {"u_xt_3_3"}}
	  },
	  u_yt{
	  	{{"u_yt_0_0"}, {"u_yt_0_1"}, {"u_yt_0_2"}, {"u_yt_0_3"}},
	  	{{"u_yt_1_0"}, {"u_yt_1_1"}, {"u_yt_1_2"}, {"u_yt_1_3"}},
	  	{{"u_yt_2_0"}, {"u_yt_2_1"}, {"u_yt_2_2"}, {"u_yt_2_3"}},
	  	{{"u_yt_3_0"}, {"u_yt_3_1"}, {"u_yt_3_2"}, {"u_yt_3_3"}}
	  },
	  u_zt{
	  	{{"u_zt_0_0"}, {"u_zt_0_1"}, {"u_zt_0_2"}, {"u_zt_0_3"}},
	  	{{"u_zt_1_0"}, {"u_zt_1_1"}, {"u_zt_1_2"}, {"u_zt_1_3"}},
	  	{{"u_zt_2_0"}, {"u_zt_2_1"}, {"u_zt_2_2"}, {"u_zt_2_3"}},
	  	{{"u_zt_3_0"}, {"u_zt_3_1"}, {"u_zt_3_2"}, {"u_zt_3_3"}}
	  }
	{
		for (int k = 0; k < 4; k++)
		{
			for (int j = 0; j < 4; j++)
			{
				u_xt[k][j].clk(clk);
				u_xt[k][j].reset(reset);
				u_xt[k][j].s_valid(c_zt_valid[0]);
				u_xt[k][j].s_ready(c_xt_ready[k][j]); // output not used
				u_xt[k][j].m_valid(c_xt_valid[k][j]); // only index [0][0] used
				u_xt[k][j].m_ready(m_ready);

				u_yt[k][j].clk(clk);
				u_yt[k][j].reset(reset);
				u_yt[k][j].s_valid(xors_valid);			// This permutation step starts after xor arithmetic
				u_yt[k][j].s_ready(c_yt_ready[k][j]); 	// output not used
				u_yt[k][j].m_valid(c_yt_valid[k][j]); 	// only index [0][0] used
				u_yt[k][j].m_ready(m_ready);

				u_zt[k][j].clk(clk);
				u_zt[k][j].reset(reset);
				u_zt[k][j].s_valid(c_yt_valid[0][0]);
				u_zt[k][j].s_ready(c_zt_ready[k][j]); // output not used
				u_zt[k][j].m_valid(c_zt_valid[k][j]); // only index [0][0] used
				u_zt[k][j].m_ready(m_ready);

				//connect: permutation  ->lift  ->permutation  ->lift  ->permutation  ->lift  ->output
				//where:   c_zt_data[][]->u_zt[]->c_yt_data[][]->u_yt[]->c_xt_data[][]->u_xt[]->m_block
				//Flip the port direction
				for (int i = 0; i < 4; i++) {
					u_xt[k][j].s_port[i](c_xt_data[k][j][i]);	//		 /*s_block[(k*4+j)*4+i]); // in x*/
					u_xt[k][j].m_port[i](m_block[(k*4+j)*4+i]); // OUT x /*c_xt_data[k][j][i]);*/

					u_yt[k][j].s_port[i](c_yt_data[k][i][j]);	// in y  /*c_xt_data[k][i][j]); // in y*/
					u_yt[k][j].m_port[i](c_xt_data[k][i][j]);			 /*c_yt_data[k][i][j]);*/

					u_zt[k][j].s_port[i](c_zt_data[i][k][j]);	// in z	 /*c_yt_data[i][k][j]); // in z*/
					u_zt[k][j].m_port[i](c_yt_data[i][k][j]);			 /*c_zt_data[i][k][j]);*/
				}

			}
		}

		SC_METHOD(mc_proc);
			sensitive << m_ready;
			sensitive << c_xt_valid[0];
		SC_METHOD(ms_rev_order);
			sensitive << clk.pos();
			dont_initialize();
	}

};

//-----------------------------------------------------------------------------
// decompress sequence of unsigned integers
//-----------------------------------------------------------------------------
template<typename FP, int DIM>
SC_MODULE(decode_ints)
{

	//Unary decoder #defines
	#define FOUND_1 1
	#define FIRST_0 2
	#define FOUND_0S 4

	//these are actually not true states and are combinatorial control path routes, since the unary decoder ought to be a parallel unit.
	#define S0 0
	#define S1 (S0+FOUND_1+FIRST_0)
	#define S2 (S0+FOUND_1)
	#define S3 (S0+FOUND_0S)
	#define SBITS 4

	//typedefs
	typedef typename FP::ui_t ui_t;

	static constexpr int planes = FP::bits; // number of bit planes
	static constexpr int intprec = FP::bits;// bits in an integer type. Also same as #bit planes.

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	//IN
	sc_stream_in<sc_bv<bw_w(DIM)> > s_bp;	//<-stream_reader (input); encoded bit plane
	sc_in <uconfig_t> s_maxbits;			//max #bits per block (including FP::ebits + 1 header bits)
	sc_in <uconfig_t> s_maxprec;
	sc_in <uconfig_t> s_minbits;			//min #bits per block (including FP::ebits + 1 header bits)

	//OUT (control path)
	sc_stream_out<sc_uint<bc_w(DIM)> > m_bc;	//->stream_reader (output); bit offset to next "plane" with respect to the current "plane"
	sc_stream_out<bool> m_blk_start;			//->stream_reader (output); indicate starting to decode a block (used to extract comm exponent / test for nought block).

	//OUT (data path)
	sc_out <ui_t> m_block[fpblk_sz(DIM)];
	sc_out <bool> m_valid;	//assert to you when I have some data
	sc_in <bool> m_ready;	//assert to me when you can take more data

	/*-------- registers --------*/				//let DIM=2 and planes = 64
	sc_signal<sc_uint<log2rz(planes-1)+1> > k;	//log2rz(... = 7
	sc_signal<uconfig_t> kmin;


	/*-------- channels --------*/
	sc_signal<sc_bv<fpblk_sz(DIM)> > c_bplane[planes];	//sc_signal<int16> c_bplane[64], 64 sets of 16 bit "vector" types

	/*-------- non-register HW constructs --------*/
	sc_uint<2*DIM+1>  n;					//5
	sc_uint<SBITS> state;					//hold decoder state.
	sc_bv<planes> x;						//the plane being decoded (not a register, not clocked.)
	sc_bv<bw_w(DIM)>stream_window;			//maximum size stream window per plane
	sc_uint<sizeof(unsigned int)*8> bits;	//maximum number of bits in the stream (will stop decoding if limit reached)
	sc_uint<sizeof(unsigned int)*8> bitoff;	//maximum number of bits in the stream (will stop decoding if limit reached)


	//on c_bplane (combinatorial cross bar mapping to m_block).
	void mc_proc()
	{
		//rewire bitplanes into a block
		sc_bv<planes> tmp;
		for (int j = 0; j < fpblk_sz(DIM); j++) {
			for (int i = 0; i < planes; i++) {
				tmp[i] = c_bplane[i].read()[j].to_bool();
			}
			m_block[j].write(tmp); // keep plane order
		}

		//maintain k_min based on maxprec (difficult to synchronize maxprec read on bootstrap)
		// intprec > s_maxprec.read() ? kmin.write(intprec - s_maxprec.read()) : kmin.write(0);//lowest bitplane to decode (computed per block)
    //lowest bitplane to decode (computed per block)
    if(intprec > s_maxprec.read()) {
      kmin.write(intprec - s_maxprec.read());
    } else {
      kmin.write(0);
    }
	}

	//On clock.
	void ms_proc()
	{
		if (reset == RLEVEL)
		{
			state = S0;		//Start state machine in initial state.
			k = planes-1;	//start decoding most significant plane
			n = 0;
			bits = s_maxbits.read()
						- (FP::ebits +1);	//subtract header bits (exponent bits + zero bit), which would have been read and parsed upstream
			bitoff = 0;
			m_blk_start.data_w(true);m_blk_start.valid_w(true);
		}
		else
		{

			//--- Control flow ---
			bool m_bc_valid = (	m_ready.read()
								&& (!m_bc.ready_r()  || s_bp.valid_r())
																			);
			bool s_bp_ready = (	m_ready.read()
								&& (!s_bp.valid_r() || m_bc.ready_r())
																			);
			s_bp.ready_w(s_bp_ready);
			m_bc.valid_w(m_bc_valid);

			//RESET READY VALID FOR START BLK. (set lower to override reset logic)
			if(m_blk_start.ready_r() == true)
				{m_blk_start.data_w(false);m_blk_start.valid_w(false);}

			//--- Data flow ---
			if (	(m_ready.read() || (k.read()!=kmin.read()) ) 	//progress with decode unless it is time to output the block and m_block listner is not ready
					&& m_bc.ready_r() 											//stall decode if stream_reader is not ready for feedback
					&& s_bp.valid_r()) 											//stall decode if stream_reader is outputting junk
			{
				unsigned m;

				stream_window = s_bp.data_r();	//get flit from stream reader.
				// decode first n bits of bit plane #k
				// n < bits ? m=n : m=bits;//		m = MIN(n, bits);
        if(n < bits) {
          m = n;
        } else {
          m = bits;
        }
				bits -= m;

				//copy first m bits into results plane (unless m is 0, then do nothing)
				if(m > 0)
					x = stream_window & (~ui_t(0)>>(planes-m));
				else
					x = 0;

				//make this work on sub strings. but how?
				bitoff = m;

				//START unary decoder
				while(n < fpblk_sz(DIM))
				{
					//looking for 0's in a unary substring  INNER LOOP IN ZFP
					if(state&FOUND_1)
					{
						if(state&FIRST_0)			//account for the first 1 seen in the unary substring using the "bits" state variable.
							{state=S2;if(n < fpblk_sz(DIM))bits--;}

						if(n < fpblk_sz(DIM) - 1 && !(state&FOUND_0S))	//Find length of '0's substring. terminate (go->S3) if a '1' is found.
						{
							if((!bits) || (!bits--))			//if bits cannot still be decoded..
								{x[n]=true; state=S3;}			//tally the last bit positively identified and terminate (go->S3).
							else								//however if bits can still be decoded...
							{
								if(stream_window[bitoff++])		//check if '0' is in the current string position
									{x[n]=true; state=S3;} 	//a 1 was found, signifying end of substring (go->S3)
							}
							n++;	//consider next bit in the plane to be decoded
						}

						//accounts for the case when no more 0's could possibly fit in the main string, but substring not explicitly terminated.
						if((n==fpblk_sz(DIM)-1) || (state&FOUND_0S))
						{
							if((n >= fpblk_sz(DIM)-1) && !(state&FOUND_0S))
								{x[n]=true; n++;}			  //force substring end
							state=S3;						  //force substring termination (go->S3)
						}
					}

					//looking for 1's in a unary substring. OUTER LOOP IN ZFP
					if(!(state&FOUND_1))
					{
						if(!bits || (n>=fpblk_sz(DIM)))	//nothing left to search through...
							break;				//exit the unary decoder state machine.
						//account for n = size case? what should happen is that n gets incremented to size and you break here before doing the stuff below.
						if(stream_window[bitoff++])			//A unary substring was found, next find all '0''s after this '1' in the string.
							state=S1;
						else break;	//stop looking for valid unary strings. This is the same as going to S0.
					}

				}
				//implicit bit fix, patch end of outer loop logic to emulate side effects in original code.
				if((n < fpblk_sz(DIM)) && bits)
					bits--;

				state = S0;	//reset state machine for next block.
				//END unary decoder.

				if(k.read()==planes-1)										//must re-initialize planes every block
					for(size_t i=0; i<planes-1; i++)c_bplane[i].write(0);
				c_bplane[k.read()].write(x);								//write new plane to registers
				m_bc.data_w(bitoff);										//number of bits to offset in next plane

				//--- Control Flow ---
				//increment k after all logic to look at next plane.
				bool last = (bits==0) || (k.read()<=(kmin.read()));
				if (last)
				{
					k = planes-1;
					n=0;		// reset plane (look at most significant plane)
					bits = s_maxbits.read()
									- (FP::ebits +1);	//subtract header bits (exponent bits + zero bit), which would have been read and parsed upstream
				}
				else
				{
					k = k.read() - 1;				// advance plane
				}

				//write output port valid status
				m_valid.write(last);
				m_blk_start.valid_w(last);
				m_blk_start.data_w(last);

			}

			//leave m_valid latched until read.
			if(m_valid.read() && m_ready.read())
				m_valid.write(false);
		}
	}

#if defined(VCD)
	void start_of_simulation()
	{
		//input (data)
		sc_trace(tf, s_bp,    (std::string(name())+".s_bp").c_str());

		//output (control)
		sc_trace(tf, m_bc,    (std::string(name())+".m_bc").c_str());
		sc_trace(tf, m_blk_start,    (std::string(name())+".m_blk_start").c_str());

		//output (data)
		for (int i = 0; i < fpblk_sz(DIM); i++)
			sc_trace(tf, m_block[i],
				(std::string(name())+".m_block."+std::to_string(i)).c_str());
		sc_trace(tf, m_valid, (std::string(name())+".m_block.valid").c_str());
		sc_trace(tf, m_ready, (std::string(name())+".m_block.ready").c_str());

		//check what k is
		sc_trace(tf, k,   (std::string(name())+".k").c_str());
		//check what kmin is
		sc_trace(tf, kmin,   (std::string(name())+".kmin").c_str());
		//check what n is
		sc_trace(tf, n,   (std::string(name())+".n").c_str());

		sc_trace(tf, stream_window,    (std::string(name())+".stream_window").c_str());
		sc_trace(tf, bits, (std::string(name())+".bits").c_str());			//maximum number of bits in the stream (will stop decoding if limit reached)
		sc_trace(tf, bitoff,  (std::string(name())+".bitoff").c_str());
		sc_trace(tf, x, (std::string(name())+".x").c_str());
		for (int i = 0; i < planes; i++)
			sc_trace(tf, c_bplane[i],
				(std::string(name())+".c_bplane."+std::to_string(i)).c_str());
	}
#endif

	SC_CTOR(decode_ints)
	{
		SC_METHOD(mc_proc);								//combinational module
			for (int i = 0; i < planes; i++)
				sensitive << c_bplane[i];				//The transpose is supposed to be wires
			sensitive << s_maxprec;						//must keep an up to date version.
		SC_METHOD(ms_proc);								//clocked module (registers)
			sensitive << clk.pos();
			dont_initialize();
	}

};

//============================================================================
//EXTRACT METADATA FROM BITSTREAM BLOCKS AND FORWARD PLANES TO DECODER MODULES
//============================================================================

//---------    custom header data type    ------
//Declare struct
template<typename FP> struct block_header
{
	typedef typename FP::expo_t expo_t;

	bool zb;					//full flag.
	expo_t exp;	//word
	block_header(){zb=false;exp=0;}
	block_header(expo_t _exp){zb=false;exp=_exp;}
	block_header(bool _zb){zb=_zb;exp=0;}
	block_header(bool _zb, expo_t _exp){zb=_zb;exp=_exp;}
	block_header& operator=(const block_header<FP>& rhs){zb = rhs.zb; exp = rhs.exp; return *this;}
	bool operator==(const block_header<FP>& rhs){return zb == rhs.zb && exp == rhs.exp;}

	//setter pattern
public:
	// block_header& set_exp(expo_t _exp) { exp = _exp; return *this; }//"fluent" API to set exponent.
	// block_header& set_zb(bool _zb) { zb = _zb; return *this; } 		//"fluent" API to set zero block.
	void set_exp(expo_t _exp) { exp = _exp;  }//"fluent" API to set exponent.
	void set_zb(bool _zb) { zb = _zb;  } 		//"fluent" API to set zero block.

	bool is_zero(){return zb;}
};

// functions on block_header to comply with current systemc-clang
template<typename FP>
void set_exp(block_header<FP>& blk_hdr, typename FP::expo_t _exp) { blk_hdr.exp = _exp;  }

template<typename FP>
void set_zb(block_header<FP>& blk_hdr, bool _zb) { blk_hdr.zb = _zb;  }

template<typename FP>
bool is_zero(block_header<FP>& blk_hdr){return blk_hdr.zb;}

//overload operators.
template<typename FP>
std::ostream& operator<<(std::ostream& os, const block_header<FP>& val){os << "zb = " << val.zb << "; exp = " << val.exp << std::endl; return os;}
template<typename FP>
inline void sc_trace(sc_trace_file*& f, const block_header<FP>& val, std::string name){sc_trace(f,val.zb, name + ".zb");sc_trace(f, val.exp, name + ".exp");}


//--------    custom buffer type    --------

//Declare struct
template<int DIM> struct plane_reg
{
	bool f;					//full flag.
	sc_uint<bw_w(DIM)> w;	//word
	plane_reg(){f=false;w=0;}
	plane_reg(sc_uint<bw_w(DIM)> wi){f=true;w=wi;}
	plane_reg& operator=(const plane_reg<DIM>& rhs){f = rhs.f; w = rhs.w; return *this;}
	bool operator==(const plane_reg<DIM>& rhs){return f == rhs.f && w == rhs.w;}

	//setter pattern
public:
	plane_reg& set_word(sc_uint<bw_w(DIM)> nw) { f=true; w = nw; return *this; } //"fluent" API to set word.

	bool is_empty(){return f;}
};

//overload operators.
template<int DIM>
std::ostream& operator<<(std::ostream& os, const plane_reg<DIM>& val){os << "f = " << val.f << "; w = " << val.w << std::endl; return os;}
template<int DIM>
inline void sc_trace(sc_trace_file*& f, const plane_reg<DIM>& val, std::string name){sc_trace(f,val.f, name + ".f");sc_trace(f, val.w, name + ".w");}

//It is necessary to implement a different decode_stream depending on DIM and FP settings
template<typename FP, typename B, int DIM> struct decode_stream;

template<typename FP, typename B> struct decode_stream<FP, B, 2>: sc_module
{
	typedef typename FP::expo_t expo_t;
	typedef typename B::uic_t uic_t;


	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_in <uconfig_t> s_minbits;
	sc_in <uconfig_t> s_maxbits;
	sc_in <uconfig_t> s_maxprec;
	sc_in <sconfig_t> s_minexp;


	//IN PORTS
	sc_stream_in <bool> s_blk_start;		//Indicates start of header at current bit offset.
	sc_stream_in<sc_uint<bc_w(2)> > s_bc;	//number of bits read from previous flit.
	sc_stream_in<B> s_bits; 				// inherit from encoder design assume B has tdata and tlast fields

	// OUT PORTS
	sc_stream_out<block_header<FP> > m_bhdr;	//exponent, biased + nought block flag.
	sc_stream_out<sc_bv<bw_w(2)> > m_bp;		//a "flit" that encodes a bit plane.
	sc_out <uconfig_t> m_block_maxprec;			//per-block maxprec (computed using minexp and block exponent)

	// SUBMODULE PORTS
	sc_stream<B> c_s_bfifo, c_m_bfifo;


	/*-------- registers --------*/
	sc_signal< plane_reg<2> > b_c[4];							//current plane flit width "bit stream window" shift registers.
	sc_signal<sc_uint<log2rz(fpblk_sz(2))+2> >c_wordoff;		//amount to offset b1 and b2 on each cycle
																//note: +2 to support offset of max window (log2(16)+1 = 32) + extra exponent bits & zero bit
	sc_signal<sconfig_t> c_rembits;					//used to safely drain register file without incorrect stalling on final few planes

	sc_signal<uconfig_t> bits;						//Track bits actually read per block
	sc_signal<uconfig_t> minbits;					//Determine if padding bits are to be read between blocks.

	sc_signal<bool> csync;							//Stall clocked thread on register file, header ready signal...
	sc_signal<bool> skpbts;							//skipping bits
	/*-------- local --------*/


	/*-------- modules --------*/
	sfifo_cc<B,2,RLEVEL> u_bfifo;					//use to keep register file full. SWITCH TO SC_RVD (sc_rvd is a stream is synthesizable) fifo_cc (not a stream)


	//Register file maintainence logic. Use pass by reference, which is supported: https://verificationguide.com/systemc/systemc-functions-argument-passing/
	inline bool refresh_next_bs_regs(plane_reg<2> (&pb_c)[4])				//write update to smart register file (b_n) using: A; processed current registers (pb_c).
																			//												   B; incoming bitstream data (s_bits.data_r())
	{																		//return value is used for bitstream ready_w() (do not listen to bitstream if buffer is full)

		constexpr int reg_thresh = 3;
		B word = c_m_bfifo.data_r();


		//each read from bitstream gives 2 "words" for the decode ints module.
		plane_reg<2>w[B::dbits/bw_w(2)];
		plane_reg<2>empty;
		plane_reg<2>tmp[4];

		bool read_data = false;	//write to the "ready" port of fifo so that a new word falls through next cycle.

		if(c_m_bfifo.valid_r())
		{
			//1) down shift register file so that full registers start at index 0
			//2) read in new bitstream data from bitstream FIFO and put it at the end of register file

			if(c_m_bfifo.ready_r())
			{
				//chop the latest bitstream word into "sliding window (bw_w)" sized pieces and store in an array, "w"
				for(size_t i=0; i < B::dbits/bw_w(2); i++)  {
          // inlining operator=
					// w[i] = plane_reg<2>((sc_uint<bw_w(2)>)(word.tdata>>(bw_w(2)*i)));
					w[i].f = 1;
					w[i].w = (sc_uint<bw_w(2)>)(word.tdata>>(bw_w(2)*i));
        }
			}
			//else... have to just drain the register file by writing 0 in place of emptied registers.

			//1) Down shift register file...
			//1.a, get lowest empty register
			sc_uint<log2rz(4)+1>srcreg;
			for(srcreg =0; srcreg< 4; srcreg++)
				if(pb_c[srcreg].f)break;

			//1. b, Down shift register file st. full registers start at index 0
			sc_uint<log2rz(4)+1>tgtreg;
			for(tgtreg=0; srcreg<4; )
			{
				if(!pb_c[srcreg].f)	break;
				else				tmp[tgtreg]=pb_c[srcreg];
        srcreg++;
        tgtreg++;
			}

			//2) copy in new data from bitstream fifo.
			if(tgtreg<reg_thresh)
			{
				for(size_t i=0; i < B::dbits/bw_w(2); i++)
					tmp[tgtreg++]=w[i];
			}

		}
		else
			{tmp[0]=pb_c[0]; tmp[1]=pb_c[1]; tmp[2]=pb_c[2]; tmp[3]=pb_c[3];}	//If fifo data is not valid, do not update the register file.

		//assign next state registers
		b_c[0].write(tmp[0]); b_c[1].write(tmp[1]); b_c[2].write(tmp[2]); b_c[3].write(tmp[3]);

		if(!tmp[2].f)			//the register file is empty enough to take in more data.
			read_data = true;
		else
			read_data = false;

		//register file maintainence return value is used to set bitstream ready flag.
		return read_data;

	}

	//bitplane window offset logic. construct bitplane window
	inline sc_uint<bw_w(2) > get_window(plane_reg<2> (&wb_c)[4],					//current cycle bitstream register file
										sc_uint<log2rz(fpblk_sz(2))+2> bitoff)		//current cycle working bit offset within register file.
	{
		sc_uint<2> wordoff = bitoff/bw_w(2);
		for(int i =0; i<4; i++)
		{
			if(!wb_c[i].f)wordoff++;						//corner case where something else cleared a full flag.
			if(wb_c[i].f)break;
		}
		sc_uint<bw_w(2)> b1(wb_c[wordoff].w), b2(wb_c[wordoff+1].w),window;
		for(sc_uint<2>i=0; i<wordoff; i++)wb_c[i].f=false;	//clear any full flags necessary

		sc_uint<log2rz(fpblk_sz(2))+2>b1rshift = bitoff%bw_w(2);
		sc_uint<log2rz(fpblk_sz(2))+2>b2lshift = bw_w(2) - b1rshift;

		window = ((b1 >> b1rshift) | (b2 << b2lshift));

		return window;
	}

	//blockwise maximum precision computation
	inline uconfig_t get_block_maxprec(expo_t maxexp) //see (zfp) src/template/codecf.c:6 "precision"
	{
#if (ZFP_ROUNDING_MODE != ZFP_ROUND_NEVER) && defined(ZFP_WITH_TIGHT_ERROR)
		sconfig_t _MAX = ((sconfig_t)maxexp - s_minexp.read() + 2 * 2 + 1);
#else
		sconfig_t _MAX = (sconfig_t)((sconfig_t)maxexp - s_minexp.read() + 2 * 2 + 2);
#endif
		if(_MAX < 0 )_MAX = 0;
		if((uconfig_t)_MAX < s_maxprec.read())
			return _MAX;
		else
			return s_maxprec.read();
	}

	void mc_proc()
	{
		bool _s_blk_cycle = ((s_blk_start.data_r()==true) && s_blk_start.valid_r());
		bool stall_for_fifo = !(b_c[1].read().f);
		bool stall_for_exp = _s_blk_cycle & ! m_bhdr.ready_r();

		csync.write(stall_for_fifo || stall_for_exp);

		//FIFO sub module plumbing
		c_s_bfifo.data_w(s_bits.data_r());
		s_bits.ready_w(c_s_bfifo.ready_r());						//request more data if the FIFO is not full.
		c_s_bfifo.valid_w(s_bits.valid_r());

	}//end mc_proc()

	void ms_proc()
	{
		//progress state machine on clock.
		if (reset == RLEVEL) {
			//all sc_signals use default constructor, so b_c{:}=0 and x_wordoff=0.
			minbits = s_maxbits.read() - s_minbits.read();
			c_m_bfifo.ready_w(false);
		} else {
			plane_reg<2>b_wrk[4] ={b_c[0].read(),b_c[1].read(),b_c[2].read(),b_c[3].read()}; //get most up to date register file.
			sc_uint<log2rz(fpblk_sz(2))+2>w_wordoff = c_wordoff.read();
			bool _s_blk_cycle = ((s_blk_start.data_r()==true) && s_blk_start.valid_r());

			block_header<FP> bhdr(true);													//concatenate zero bit and exponent into one message

			sconfig_t w_rembits;						 									//remaining bits in current block. Used to decide if pad bits should be read in zero block / variable mode decoding,

			//skip bits if necessary.
			if(		(!csync.read() && (_s_blk_cycle || skpbts.read()))	//not stalled and a start block , or already skipping bits
																		)
			{
				 w_rembits = c_rembits.read();
				//skip block padding bits if necessary
				if(w_rembits>0)
				{
					sconfig_t dreg_bits;	//bits to skip.

					//flush padding words in bitstream register file (maximum of one register flushed per clock cycle)
					if(w_rembits/bw_w(2))
					{																		//flush all whole padding bitstream windows in bitstream register file
						dreg_bits = bw_w(2);
						get_window(b_wrk,dreg_bits);
					}
					else
					{																		//flush extra bitstream window register if "remainder bits" end at a bitstream window register boundary.
						dreg_bits = (w_rembits%bw_w(2));
						get_window(b_wrk,dreg_bits+1);										//+1 will force get_window to flush boundary register
					}

					c_rembits.write((w_rembits-dreg_bits));									//update minimum remaining bits (to skip) for this block

					if(w_rembits > bw_w(2))													//check if finished skipping bits
						skpbts.write(true);													//not finished skipping
					else																	//finished skipping, fix up bitstream offset and enable header search.
					{
						c_wordoff.write((dreg_bits+1)%bw_w(2));								//modify the bitstream window register offset used by decoder to account for dreg bits
						skpbts.write(false);												//enable header search.
					}

				}
				else
					skpbts.write(false);
			}


			//Get block header if necessary.
			if(		(!csync.read() && _s_blk_cycle)			//start of block &&
					&& !skpbts.read()						//not skipping bits &&
					&& (m_bp.ready_r() && s_bc.valid_r())	//feedback loop is not stalled
															)
			{
				w_rembits = s_maxbits.read();//Assume maxbits are available in the bitstream (well formed block).
				w_wordoff+=s_bc.data_r();

				// bhdr.set_zb(!(get_window(b_wrk,w_wordoff)&1));
				set_zb(bhdr, !(get_window(b_wrk,w_wordoff)&1));
				w_wordoff+=1;

				w_wordoff = w_wordoff%bw_w(2);	//working register file offset should drop to a bit offset within the first valid register in b_c[:] register file.
				w_rembits -= 1;					//keep track of all read bits
				if(!bhdr.zb)					//if no zero block, read an exponent.
				{
					expo_t blockexpt = get_window(b_wrk,w_wordoff);	//Extract the exponent from valid bitstream window
					w_wordoff+=FP::ebits;

					w_wordoff = w_wordoff%bw_w(2);	//working register file offset should drop to a bit offset within the first valid register in b_c[:] register file.
					w_rembits -= FP::ebits;			//keep track of all read bits
					blockexpt -= FP::ebias;			//Assume encoded with bias, and remove this bias from exponent

					m_block_maxprec.write(get_block_maxprec(blockexpt));	//Compute and output per-block maxprec.
					// bhdr.set_exp(blockexpt);
					set_exp(bhdr, blockexpt);
					s_blk_start.ready_w(true);
				}
				else						//zero block. move to skip bits.
				{
					m_bp.valid_w(false);	//turn off feedback loop
					s_bc.ready_w(false);

					skpbts.write(true);		//prepare to skip bits
				}

				c_wordoff.write(w_wordoff);
				c_rembits.write(w_rembits);

				m_bhdr.data_w(bhdr);
				m_bhdr.valid_w(true);



			}
			else
			{
				if(m_bhdr.ready_r() )
					m_bhdr.valid_w(false);

				s_blk_start.ready_w(false);
			}

			//Get block if necessary
			if(		!csync.read()
					&& (!_s_blk_cycle || !bhdr.zb)			//not detecting header (or a normal block was already detected)
					&& !skpbts.read()						//not skipping bits &&
					&& (m_bp.ready_r() && s_bc.valid_r())	//feedback loop is not stalled

					)
			{
				w_rembits = c_rembits.read() - s_bc.data_r();
				//read in next bit count
				if(!_s_blk_cycle)
					w_wordoff+=s_bc.data_r();							//use last cycles feedback to offset to next bit plane window
				sc_uint<bw_w(2) > planewdw= get_window(b_wrk,w_wordoff);//get window in bitstream. could span b1, b2 and b3 after get_expt.
				w_wordoff = w_wordoff%bw_w(2);							//Register file offset should drop to a bit offset for the 0th register

				//Bitplane data is to be output.
				m_bp.data_w(planewdw);
				m_bp.valid_w(true);
				s_bc.ready_w(true);
				c_wordoff.write(w_wordoff);						//update register file offset register for next cycle
				c_rembits.write(w_rembits);						//update "remaining bits" counter register for next cycle
			}
			else
			{
				if(m_bp.ready_r() || s_bc.valid_r())				//technically should be && but,
				{
					m_bp.valid_w(false); s_bc.ready_w(false);
				}
			}

			//Register file maintainence logic
			c_m_bfifo.ready_w(								//upstream input port is ready if register file is not full.
							refresh_next_bs_regs(b_wrk));	//update register file (b_n[:]) and set ready flag

		}
	}

#if defined(VCD)
	void start_of_simulation()
	{
		//Configuration
		sc_trace(tf, s_minbits, (std::string(name())+".s_minbits").c_str());
		sc_trace(tf, s_maxbits, (std::string(name())+".s_maxbits").c_str());
		sc_trace(tf, s_maxprec, (std::string(name())+".s_maxprec").c_str());
		sc_trace(tf, s_minexp,  (std::string(name())+".s_minexp").c_str());

		//Output
		sc_trace(tf, m_bp,    (std::string(name())+".m_bp").c_str());
		sc_trace(tf, m_bhdr,    (std::string(name())+".m_bhdr").c_str());

		//Input
		sc_trace(tf, s_bc,			(std::string(name())+".s_bc").c_str());
		sc_trace(tf, s_bits,    	(std::string(name())+".s_bits").c_str());
		sc_trace(tf, s_blk_start,	(std::string(name())+".s_blk_start").c_str());

		//Working registers
		for(size_t i = 0; i < 4; i++)
			sc_trace(tf, b_c[i],    	(std::string(name())+".b_c"+std::to_string(i)).c_str());

		sc_trace(tf, c_wordoff, (std::string(name())+".c_wordoff").c_str());
		sc_trace(tf, c_rembits, (std::string(name())+".c_rembits").c_str());

		//Bitstream FIFO
		sc_trace(tf, u_bfifo.s_port, (std::string(name())+".u_bfifo.s_port").c_str());
		sc_trace(tf, u_bfifo.m_port, (std::string(name())+".u_bfifo.m_port").c_str());

		//clocked thread stall signals (register file, block header etc...)
		sc_trace(tf, csync, (std::string(name())+".csync").c_str());
		sc_trace(tf, skpbts, (std::string(name())+".skpbts").c_str());
	}
#endif // VCD

	SC_CTOR(decode_stream) : u_bfifo("u_bfifo")
	{
		//connect register file FIFO.
		u_bfifo.clk(clk);
		u_bfifo.reset(reset);
		u_bfifo.s_port(c_s_bfifo); //FIFO input is bitstream.
		u_bfifo.m_port(c_m_bfifo);	//FIFO output pipes to a stream read by module

		SC_METHOD(mc_proc);
			sensitive << s_bits.valid_chg() << s_bits.data_chg();//pipe bitstream status signals to FIFO
			sensitive << s_bc.valid_chg();
			sensitive << c_s_bfifo.ready_event();				//pipe FIFO full signal to bitstream
			sensitive << m_bhdr.ready_chg();
			sensitive << m_bp.ready_chg();
			for(size_t i=0; i<4; i++){sensitive << b_c[i];}		//sensitize to changes in register file
			sensitive << s_minbits << s_maxbits << s_maxprec << s_minexp;
			sensitive << s_blk_start.valid_chg() << s_blk_start.data_chg();
			sensitive << c_rembits;
		SC_METHOD(ms_proc);
			sensitive << clk.neg();//necesary for same cycle feedback
			dont_initialize();
	}

};


//-----------------------------------------------------------------------------
// inverse block-floating-point transform from signed integers
//-----------------------------------------------------------------------------
template<typename FP, int DIM>
SC_MODULE(inv_cast)
{
	typedef typename FP::expo_t expo_t;		//Exponent part of floating point type
	typedef typename FP::si_t si_t;			//Integer representation of floating point type?
	typedef typename FP::ui_t ui_t;

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	//Inputs, exponent and block
	sc_stream_in<block_header<FP> > s_bhdr;

	sc_in<si_t> s_block[fpblk_sz(DIM)];	//block replacement of stream in: sc_stream_in<FP>     s_fp;
	sc_in <bool> s_valid;					//Explicit valid and ready signals for the block input (these are integrated ito s_ex and m_stream)
	sc_out <bool> s_ready;

	//Output, stream
	sc_stream_out<FP>	m_stream;			//Stream out not replacement of block in: sc_out<si_t> m_block[fpblk_sz(DIM)];

	//-------- registers --------
	sc_signal<si_t> r_blk[fpblk_sz(DIM)];	//Registers for the block to be cast into float. (note, signed representation is used, important later on!
	sc_signal<expo_t> r_ex;					//Register for the exponent, used when casting floats.
	sc_signal<sc_uint<(2*DIM)> > count;		//Used as a program counter. Input to combinatorial control path also. count up to the number of elements in a block to process.
	sc_signal<bool> zb;						//Register for "zero" block indication during stream out.

	sc_signal<bool> got_bhdr;				//harden against stall conditions when reading in block for inverse cast
	sc_signal<bool> got_blk;				//as above

	//-------- channels -------- (what is a channel for?)
	sc_signal<bool> c_sync;					//Used as a control path signal. Raise to run state machine. Lower to halt state machine.

	/*-------- modules --------*/

	//Control path block
	void mc_proc()
	{
		FP fp;								//IEEE float to stream out of m_stream at a rate of 1 per clock cycle
		sc_uint<1> s = 0; 					//fp's sign bit (to be determined during a block element re-cast)
		expo_t _r_ex = 0;					//common exponent for block
		// --- Control flow ---
		bool stall = (m_stream.ready_r() == false ||
						((count.read() == 0) && !got_bhdr.read())	||				  //dont have any header
						((count.read() == 0) && (got_bhdr.read() && !got_blk.read())) //have header but unknown block
		);
		c_sync.write(!stall);
		m_stream.valid_w(!stall);

		// --- Data flow ---

		// determine if number is negative
		si_t si = r_blk[count.read()].read();	//convert the current block entry to be streamed out.
		ui_t neg_mask = (1LL <<(FP::bits-1));	//a mask to see if an unsigned type is negative in twos compliment
		if(si & neg_mask)						//check if the integer is "negative" in 2's compliment
			{si = -si; s = 1;}					//separate sign (s) from scalar value

		// handle y = 0 and the zero block logic as special cases where output is 0.
		bool zero_output = ((si == 0)								//the floating point encoding is 0.
							|| (zb.read())		//zero block detected & part way through outputting zero block already
																);

		if(zero_output)
			fp =  (typename FP::ui_t)0;
		else
		{
			// Compute x = 2^emax * (y / 2^(p - 2))
			ui_t rn = ui_t(si);

			// determine position, e, of leading one-bit: 2^e <= y < 2^(e+1)
			ui_t e = 0;
			while (rn >> (e + 1))
			  e++;

			// align significand such that leading one-bit is in position FP::fbitsm
			int shift = FP::fbits - e;
			if (shift > 0)
				rn <<= +shift;
			else
				rn >>= -shift;

			// add in bias, block exponent, and coefficient normalization
			e += FP::ebias + r_ex.read() - (FP::bits - 2);

			// handle special case of subnormals
			if (e <= 0)
			{
				// subnormal number: shift significand
				rn >>= 1 - e;
				e = 0;
			}
			else
			{
				// normalized number: zero hidden one-bit
				rn &= (1ul << FP::fbits) - 1;
			}

			// construct floating-point value from sign, exponent, and significand
			rn += ((s << FP::ebits) + e) << FP::fbits;

			//rn is now in IEEE floating point format. initialize SystemC stream type with rn's bits.
			fp = FP(rn);
		}
		m_stream.data_w(fp);			//always write out converted data

	}

	//data path block (registers).
	void ms_proc()
	{
		//write registers aparently
		if (reset == RLEVEL)			//On reset, clear everything. All registers.
		{
			count = 0;					//start stream counter at 0
		}
		else							//On not reset, chug through all data and clock it out.
		{
			//control flow
			bool _zb = false;
			bool _got_bhdr = false;
			//get block header, independent of block registers.
			//header is on a fifo. Fifo will blindly hold valid high waiting for a transaction
			if((count.read() == 0) && s_bhdr.valid_r() && (got_bhdr.read() == false))	//ensure a single data transaction occurs at count 0.
			{
				_got_bhdr = true; got_bhdr.write(_got_bhdr);					//record block header detected
				_zb = s_bhdr.data_r().zb; zb.write(_zb);						//record zero_block
				r_ex.write(s_bhdr.data_r().exp);								//record exponent
				if(_zb)
				{
					for(unsigned i = 0; i < fpblk_sz(DIM); i++)r_blk[i]=0;		//set block registers if zero block
					got_blk.write(true);										//indicate got a block
				}
				s_bhdr.ready_w(true);											//indicate header was read.
			}
			if((got_bhdr.read() == true))			//de-latch header ready signal
				s_bhdr.ready_w(false);

			//get block, dependent on block header not containing a zero bit.
			//block is on a fifo. Fifo will blindly hold valid high waiting for a transaction.
			if((count.read() == 0) && ( (_got_bhdr && !_zb) || (got_bhdr.read() && !zb.read()) ) )
			{
				if(s_valid.read())						//we should read a block. ready is high, valid is high. clear both
				{
					got_blk.write(true);
					for(unsigned i = 0; i < fpblk_sz(DIM); i++)
						r_blk[i]= s_block[i].read();	//put read data into working registers
					s_ready.write(true);
				}
			}
			if(got_blk.read())
				s_ready.write(false);

			//clear header read related state registers if need be.
			if(count.read() != 0)
				{got_bhdr.write(false);got_blk.write(false);}

			//output state
			if(c_sync)										//if fwd_cast state machine should run..
			{
				if (count.read() == fpblk_sz(DIM)-1)		//Tell combinatorial thread, perform fwd_cast for the 0th state
					{count.write(0);}						//Start streaming again from 0
				else
					{count.write(count.read() + 1);}		//Tell combinatorial thread, perform fwd_cast on next state
			}

		}
	}

	SC_CTOR(inv_cast)
	{

		SC_METHOD(mc_proc);			//Combinational / control path is sensitive to all control signals
			//Input related sensitivity
			sensitive << s_valid;								//Sensitive to block input ctl signal
			sensitive << s_bhdr.valid_chg();
			sensitive << got_blk;
			sensitive << got_bhdr;
			sensitive << m_stream.ready_chg();					//Sensitive to stream output ctl signal
			sensitive << count;									//Sensitive to count (acts like PC counter for combinatorial logic units)
			sensitive << c_sync;								//Sensitive to combinatorial output (useful for defined reset behavior)

		SC_METHOD(ms_proc);			//Register transfer / current state counter. This path is sensitive to clock only
			sensitive << clk.pos();
			dont_initialize();
	}
#if defined(VCD)

	void start_of_simulation()
	{
		//trace input ports
		sc_trace(tf, s_bhdr, (std::string(name())+".s_bhdr").c_str());			//control signals integrated to input
		sc_trace(tf, got_bhdr, (std::string(name())+".got_bhdr").c_str());		//control signals integrated to input
		sc_trace(tf, got_blk, (std::string(name())+".got_blk").c_str());		//control signals integrated to input
		sc_trace(tf, r_ex, (std::string(name())+".r_ex").c_str());				//control signals integrated to input
		sc_trace(tf, zb, (std::string(name())+".zb").c_str());					//inv_cast mode register
		for (int i = 0; i < fpblk_sz(DIM); i++)
					sc_trace(tf, s_block[i],
						(std::string(name())+".s_block"+std::to_string(i)).c_str());

		sc_trace(tf, s_valid, (std::string(name())+".s_valid").c_str());	//control signals exterior to input
		sc_trace(tf, s_ready, (std::string(name())+".s_ready").c_str());

		//trace output ports
		sc_trace(tf, m_stream, (std::string(name())+".m_stream").c_str());

		//trace interior control signals
		sc_trace(tf, c_sync, (std::string(name())+".c_sync").c_str());

		//trace interior registers
		sc_trace(tf, count, (std::string(name())+".count").c_str());
	}
#endif
};
//end inv_cast

#ifndef ZHW_ENCODE_H
//-----------------------------------------------------------------------------
// block buffer
//-----------------------------------------------------------------------------
template<typename FP, int DIM>
SC_MODULE(block_buffer)
{
	typedef typename FP::ui_t ui_t;
	typedef sc_bv<FP::bits*fpblk_sz(DIM)> block_t;

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_in <ui_t> s_block[fpblk_sz(DIM)];
	sc_in <bool> s_valid;
	sc_out<bool> s_ready;

	sc_out<ui_t> m_block[fpblk_sz(DIM)];
	sc_out<bool> m_valid;
	sc_in <bool> m_ready;

	/*-------- registers --------*/

	/*-------- channels --------*/
	sc_signal<block_t> c_bi;
	sc_signal<block_t> c_bo;

	/*-------- modules --------*/
	fifo_cc<block_t,1,true,RLEVEL,false> u_bbuf;

	void mc_proc()
	{
		block_t bbufi;
		block_t bbufo;

		for (int i = 0; i < fpblk_sz(DIM); i++)
			bbufi((i+1)*FP::bits-1,i*FP::bits) = s_block[i];
		c_bi = bbufi;

		bbufo = c_bo;
		for (int i = 0; i < fpblk_sz(DIM); i++)
			m_block[i].write(sc_bv<FP::bits>(bbufo((i+1)*FP::bits-1,i*FP::bits)));
	}

#if defined(VCD)
	void start_of_simulation()
	{
		for (int i = 0; i < fpblk_sz(DIM); i++)
			sc_trace(tf, s_block[i],
				(std::string(name())+".s_block"+std::to_string(i)).c_str());
		sc_trace(tf, s_valid, (std::string(name())+".s_valid").c_str());
		sc_trace(tf, s_ready, (std::string(name())+".s_ready").c_str());

		for (int i = 0; i < fpblk_sz(DIM); i++)
			sc_trace(tf, m_block[i],
				(std::string(name())+".m_block"+std::to_string(i)).c_str());
		sc_trace(tf, m_valid, (std::string(name())+".m_valid").c_str());
		sc_trace(tf, m_ready, (std::string(name())+".m_ready").c_str());
	}
#endif

	SC_CTOR(block_buffer) :
		u_bbuf("u_bbuf")
	{
		// connect u_bbuf
		u_bbuf.clk(clk);
		u_bbuf.reset(reset);
		u_bbuf.din(c_bi);
		u_bbuf.wr_en(s_valid);
		u_bbuf.full(s_ready);
		u_bbuf.dout(c_bo);
		u_bbuf.rd_en(m_ready);
		u_bbuf.empty(m_valid);

		SC_METHOD(mc_proc);
			for (int i = 0; i < fpblk_sz(DIM); i++)
				sensitive << s_block[i];
			sensitive << c_bo;
	}

};
#endif //ZHW_ENCODE_H

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

	//-------- ports --------//
	//			:configuration
	sc_in <uconfig_t> maxbits;
	sc_in <uconfig_t> minbits;
	sc_in <uconfig_t> maxprec;
	sc_in <sconfig_t> minexp;
	//			:data in
	sc_stream_in <B>  s_bits;
	//			:data out
	sc_stream_out<FP> m_stream;

	//-------- channels --------//
	sc_stream<B> _c_s_bits;
	sc_stream<sc_uint<zhw::bc_w(DIM)> > c_bc;
	sc_stream<sc_bv<zhw::fpblk_sz(DIM)*2> > c_bp;
	sc_stream<block_header<FP> > c_shdr;
	sc_stream<block_header<FP> > c_fhdr;
	sc_stream<bool> c_blk_start;
	sc_signal<uconfig_t> c_block_maxprec;

	sc_signal <ui_t> c_i_m_block[fpblk_sz(DIM)];	//integer
	sc_signal <bool> c_i_m_valid;
	sc_signal <bool> c_i_m_ready;

	sc_signal <ui_t> c_b_m_block[fpblk_sz(DIM)];	//block xfm
	sc_signal <bool> c_b_m_valid;
	sc_signal <bool> c_b_m_ready;

	sc_signal <si_t> c_l_m_block[fpblk_sz(DIM)];	//lifted
	sc_signal <bool> c_l_m_valid;
	sc_signal <bool> c_l_m_ready;

	sc_signal <si_t> c_c_s_block[fpblk_sz(DIM)];	//cast
	sc_signal <bool> c_c_s_valid;
	sc_signal <bool> c_c_s_ready;

	//-------- modules --------//
	decode_stream<FP, B, DIM> 	u_decode_stream;	//bitstream->|-> bit plane "windows"
													//			 |-> zero block detect bit
													//			 |-> block common exponent
	sfifo_cc< block_header<FP> ,2, RLEVEL> u_hfifo;//block header FIFO, amalgomates previous u_efifo and u_zfifo.
	decode_ints<FP, DIM> 		u_decode_ints;		//bit plane "window"-> unary decoded negbinary (block)
	block_buffer<FP, DIM,ui_t> 		u_block_buffer;		//unary decoded negbinary buffer -> decode_block
	decode_block<FP, DIM>		u_decode_block;		//buffered negbinary block -> transform + integer conversion (block)
	block_buffer<FP, DIM,si_t> 		u_cast_buffer;	//needed to organize the mess between decode_block, decode_stream and inv_cast.
	inv_cast<FP, DIM>			u_inv_cast;			//integer block  --|--> floating point stream
													//common exponent--|


	SC_CTOR(decode) :
		u_decode_stream("u_decode_stream"),
		u_hfifo("u_hdrfifo"),
		u_decode_ints("u_decode_ints"),
		u_block_buffer("u_block_buffer"),
		u_decode_block("u_decode_block"),
		u_cast_buffer("u_cast_buffer"),
		u_inv_cast("u_inv_cast")
	{
		u_decode_stream.clk(clk);
		u_decode_stream.reset(reset);

		//plumb together decode_stream input
		u_decode_stream.s_maxbits(maxbits);
		u_decode_stream.s_maxprec(maxprec);
		u_decode_stream.s_minbits(minbits);
		u_decode_stream.s_minexp(minexp);
		u_decode_stream.s_blk_start(c_blk_start);
		u_decode_stream.s_bits(s_bits);
		u_decode_stream.s_bc(c_bc);

		//plumb together decode_stream output
		u_decode_stream.m_bp(c_bp);
		u_decode_stream.m_bhdr(c_shdr);
		u_decode_stream.m_block_maxprec(c_block_maxprec);

		//plumb together fifo 1 (header)
		u_hfifo.clk(clk);
		u_hfifo.reset(reset);
		u_hfifo.s_port(c_shdr);
		u_hfifo.m_port(c_fhdr);

		//plumb together decode ints
		u_decode_ints.clk(clk);
		u_decode_ints.reset(reset);
		u_decode_ints.s_bp(c_bp);
		u_decode_ints.s_maxbits(maxbits);
		u_decode_ints.s_minbits(minbits);
		u_decode_ints.s_maxprec(c_block_maxprec);
		u_decode_ints.m_bc(c_bc);
		u_decode_ints.m_blk_start(c_blk_start);

		for(size_t i=0; i<fpblk_sz(DIM); i++)
			u_decode_ints.m_block[i](c_i_m_block[i]);
		u_decode_ints.m_valid(c_i_m_valid);
		u_decode_ints.m_ready(c_i_m_ready);

		//plumb together block buffer
		u_block_buffer.clk(clk);
		u_block_buffer.reset(reset);
		for(size_t i=0; i<fpblk_sz(DIM); i++)
			u_block_buffer.s_block[i](c_i_m_block[i]);
		u_block_buffer.s_valid(c_i_m_valid);
		u_block_buffer.s_ready(c_i_m_ready);

		for(size_t i=0; i<fpblk_sz(DIM); i++)
			u_block_buffer.m_block[i](c_b_m_block[i]);
		u_block_buffer.m_valid(c_b_m_valid);
		u_block_buffer.m_ready(c_b_m_ready);

		//plumb together decode block
		u_decode_block.clk(clk);
		u_decode_block.reset(reset);
		for(size_t i=0; i<fpblk_sz(DIM); i++)
			u_decode_block.s_block[i](c_b_m_block[i]);
		u_decode_block.s_valid(c_b_m_valid);
		u_decode_block.s_ready(c_b_m_ready);

		for(size_t i=0; i<fpblk_sz(DIM); i++)
			u_decode_block.m_block[i](c_l_m_block[i]);
		u_decode_block.m_valid(c_l_m_valid);
		u_decode_block.m_ready(c_l_m_ready);

		//plumb together secondary block buffer
		u_cast_buffer.clk(clk);
		u_cast_buffer.reset(reset);
		for(size_t i=0; i<fpblk_sz(DIM); i++)
			u_cast_buffer.s_block[i](c_l_m_block[i]);
		u_cast_buffer.s_valid(c_l_m_valid);
		u_cast_buffer.s_ready(c_l_m_ready);

		for(size_t i=0; i<fpblk_sz(DIM); i++)
			u_cast_buffer.m_block[i](c_c_s_block[i]);
		u_cast_buffer.m_valid(c_c_s_valid);
		u_cast_buffer.m_ready(c_c_s_ready);
		//inv cast
		u_inv_cast.clk(clk);
		u_inv_cast.reset(reset);
		u_inv_cast.s_bhdr(c_fhdr);
		for(size_t i=0; i<fpblk_sz(DIM); i++)
			u_inv_cast.s_block[i](c_c_s_block[i]);
		u_inv_cast.s_valid(c_c_s_valid);
		u_inv_cast.s_ready(c_c_s_ready);
		u_inv_cast.m_stream(m_stream);
	}
};


} // namespace zhw

#endif // ZHW_DECODE_H
