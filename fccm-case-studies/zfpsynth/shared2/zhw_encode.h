
#ifndef ZHW_ENCODE_H
#define ZHW_ENCODE_H

#include "systemc.h"
#include "sc_stream_imp.h"
#include "ssplit.h"
#include "sreg.h"
#include "fifo_cc.h"
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
// compute maximum floating-point exponent in block of n values
//-----------------------------------------------------------------------------
template<typename FP, int DIM>
SC_MODULE(find_emax)
{
	typedef typename FP::expo_t expo_t;

	/*-------- ports --------*/
	sc_in<bool> clk;
	sc_in<bool> reset;

	sc_stream_in <FP> s_fp;
	sc_stream_out<FP> m_fp;
	sc_stream_out<expo_t> m_ex;

	/*-------- registers --------*/
	sc_signal<sc_uint<2*DIM> > count;
	sc_signal<expo_t> emax;
	sc_signal<bool> emax_v;

	/*-------- channels --------*/
	sc_signal<bool> c_sync;
	sc_stream<FP> c_fp;
	sc_stream<expo_t> c_ex;

	/*-------- modules --------*/
	sfifo_cc<FP,2*DIM+1,RLEVEL> u_que_fp;
	sreg<expo_t,FWD_REV,RLEVEL> u_reg_ex;

	void mc_proc()
	{
		s_fp.ready_w(c_sync);

		c_fp.data_w(s_fp.data_r());
		c_fp.valid_w(c_sync);

		c_ex.data_w(emax.read());
		c_ex.valid_w(emax_v);

		c_sync = s_fp.valid_r() && c_fp.ready_r() && (!emax_v || c_ex.ready_r());
	}

	void ms_proc()
	{
		if (reset == RLEVEL) {
			count = fpblk_sz(DIM)-1;
			emax = 0;
			emax_v = false;
		} else {
			bool last = (count.read() == 0);
			FP fp = s_fp.data_r();
			expo_t expo;
			// Does not handle infinity (fp.expo == expo_t(~0) && fp.frac == 0).
			// Zero is encoded as a special case and one is added to all other.
			// TODO: add extra channels to max exponent stream (zero, inf, nan)
			if (fp.expo == 0 && fp.frac == 0) {
				expo = fp.expo;
			} else {
				expo = fp.expo + expo_t(1);
			}
			if (c_sync) {
				if (last) {count = fpblk_sz(DIM)-1;}
				else {count = count.read() - 1;}
			}
			if (emax_v && c_ex.ready_r()) {
				if (s_fp.valid_r()) emax = expo; else emax = 0;
			} else if (s_fp.valid_r() && expo > emax) {
				emax = expo;
			}
			if (emax_v && c_ex.ready_r()) emax_v = false;
			else if (c_sync && last) emax_v = true;
		}
	}

#if defined(VCD)
	void start_of_simulation()
	{
		sc_trace(tf, s_fp,   (std::string(name())+".s_fp").c_str());
		sc_trace(tf, m_fp,   (std::string(name())+".m_fp").c_str());
		sc_trace(tf, m_ex,   (std::string(name())+".m_ex").c_str());
		sc_trace(tf, count,  (std::string(name())+".count").c_str());
		sc_trace(tf, emax,   (std::string(name())+".emax").c_str());
		sc_trace(tf, emax_v, (std::string(name())+".emax_v").c_str());
	}
#endif

	SC_CTOR(find_emax) :
		u_que_fp("u_que_fp"),
		u_reg_ex("u_reg_ex")
	{
		// connect u_que_fp
		u_que_fp.clk(clk);
		u_que_fp.reset(reset);
		u_que_fp.s_port(c_fp);
		u_que_fp.m_port(m_fp);

		// connect u_reg_ex
		u_reg_ex.clk(clk);
		u_reg_ex.reset(reset);
		u_reg_ex.s_port(c_ex);
		u_reg_ex.m_port(m_ex);

		SC_METHOD(mc_proc);
			sensitive << s_fp.valid_chg() << s_fp.data_chg();
			sensitive << c_fp.ready_event() << c_ex.ready_event();
			sensitive << emax << emax_v << c_sync;
		SC_METHOD(ms_proc);
			sensitive << clk.pos();
			dont_initialize();
	}

};

//-----------------------------------------------------------------------------
// forward block-floating-point transform to signed integers
//-----------------------------------------------------------------------------
template<typename FP, int DIM>
SC_MODULE(fwd_cast)
{
	typedef typename FP::expo_t expo_t;
	typedef typename FP::si_t si_t;
	typedef typename FP::ui_t ui_t;

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_stream_in<expo_t> s_ex;
	sc_stream_in<FP>     s_fp;

	sc_out<si_t> m_block[fpblk_sz(DIM)];
	sc_out<bool> m_valid;
	sc_in <bool> m_ready;

	/*-------- registers --------*/
	sc_signal<sc_uint<2*DIM> > count;

	/*-------- channels --------*/
	sc_signal<bool> c_sync;
	sc_signal<si_t> c_int;

	/*-------- modules --------*/

	void mc_proc()
	{
		c_sync = s_ex.valid_r() && s_fp.valid_r() && m_ready.read();

		s_ex.ready_w(c_sync && (count.read() == fpblk_sz(DIM)-1));
		s_fp.ready_w(c_sync);

		expo_t emax = s_ex.data_r();
		if (emax != 0) emax -= expo_t(1); // decode exponent
		FP fp = s_fp.data_r();
		sc_uint<3> hid = fp.expo != 0; // hidden bit, handle subnormals
		ui_t ui = (hid,fp.frac,sc_uint<FP::ebits-2>(0)) >> (emax-fp.expo);
		si_t si;
		if (fp.sign) si = -si_t(ui);
		else         si =  si_t(ui);
		// if (s_ex.valid_r() && s_fp.valid_r() && m_ready.read()) {
			// cout << hex;
			// cout << "emax':" << emax << " ";
			// cout << "shift:" << (emax-fp.expo) << " ";
			// cout << "fp:" << fp << " ";
			// cout << "si:" << si << endl;
		// }
		c_int.write(si);
	}

	void ms_proc()
	{
		if (reset == RLEVEL) {
			count = 0;
			m_valid.write(false);
		} else {
			if (c_sync) {
				if (count.read() == fpblk_sz(DIM)-1) {count = 0; m_valid.write(true);}
				else {count = count.read() + 1; m_valid.write(false);}
				m_block[count.read()].write(c_int.read());
			} else if (m_ready.read()) m_valid.write(false);
		}
	}

#if defined(VCD)
	void start_of_simulation()
	{
		sc_trace(tf, s_ex, (std::string(name())+".s_ex").c_str());
		sc_trace(tf, s_fp, (std::string(name())+".s_fp").c_str());

		for (int i = 0; i < fpblk_sz(DIM); i++)
			sc_trace(tf, m_block[i],
				(std::string(name())+".m_block"+std::to_string(i)).c_str());
		sc_trace(tf, m_valid, (std::string(name())+".m_valid").c_str());
		sc_trace(tf, m_ready, (std::string(name())+".m_ready").c_str());

		sc_trace(tf, count, (std::string(name())+".count").c_str());
	}
#endif

	SC_CTOR(fwd_cast)
	{
		SC_METHOD(mc_proc);
			sensitive << s_ex.valid_chg() << s_ex.data_chg();
			sensitive << s_fp.valid_chg() << s_fp.data_chg();
			sensitive << m_ready;
			sensitive << count << c_sync << c_int;
		SC_METHOD(ms_proc);
			sensitive << clk.pos();
			dont_initialize();
	}

};

//-----------------------------------------------------------------------------
// forward lifting transform of 4-vector
//-----------------------------------------------------------------------------
template<typename FP>
SC_MODULE(fwd_lift)
{
	static constexpr int stages = 6;

	typedef typename FP::si_t si_t;

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_in <si_t> s_port[4];
	sc_in <bool> s_valid;
	sc_out<bool> s_ready;

	sc_out<si_t> m_port[4];
	sc_out<bool> m_valid;
	sc_in <bool> m_ready;

	/*-------- registers --------*/
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
			x[0].write((s_port[0].read() + s_port[3].read()) >> 1);
			y[0].write(s_port[1].read());
			z[0].write((s_port[2].read() + s_port[1].read()) >> 1);
			w[0].write(s_port[3].read());

			x[1].write((x[0].read() + z[0].read()) >> 1);
			y[1].write(y[0].read() - z[0].read());
			z[1].write(z[0].read());
			w[1].write(w[0].read() - x[0].read());

			x[2].write(x[1].read());
			y[2].write(y[1].read());
			z[2].write(z[1].read() - x[1].read());
			w[2].write((w[1].read() + y[1].read()) >> 1);

			x[3].write(x[2].read());
			y[3].write(y[2].read() - w[2].read());
			z[3].write(z[2].read());
			w[3].write(w[2].read());

			x[4].write(x[3].read());
			y[4].write(y[3].read());
			z[4].write(z[3].read());
			w[4].write(w[3].read() + (y[3].read() >> 1));

			x[5].write(x[4].read());
			y[5].write(y[4].read() - (w[4].read() >> 1));
			z[5].write(z[4].read());
			w[5].write(w[4].read());

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

	SC_CTOR(fwd_lift)
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
// encode block of integers
//-----------------------------------------------------------------------------
template<typename FP, int DIM>
struct encode_block;

// vector with a size of 4.
/* template <class T> */
/* class sc_vector4 : public sc_vector<T> { */
/* 	public: */
/* 		sc_vector4() : sc_vector<T>(4) {} */
/* 		explicit sc_vector4(const char* name_) : sc_vector<T>(name_, 4) {} */
/* }; */

template<typename T>
struct vector4_t {
public:
  T vector4[4];
  
  vector4_t(){};
};

template<typename FP>
struct encode_block<FP, 1> : sc_module
{
	typedef typename FP::si_t si_t;
	typedef typename FP::ui_t ui_t;

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_in <si_t> s_block[fpblk_sz(1)];
	sc_in <bool> s_valid;
	sc_out<bool> s_ready;

	sc_out<ui_t> m_block[fpblk_sz(1)];
	sc_out<bool> m_valid;
	sc_in <bool> m_ready;

	/*-------- registers --------*/
	/*-------- channels --------*/
	sc_signal<si_t> c_xt_data[4];
	sc_signal<bool> c_xt_valid;
	sc_signal<bool> c_xt_ready;

	/*-------- modules --------*/
	fwd_lift<FP> u_xt;

	void mc_proc()
	{
		s_ready.write(m_ready.read()); // bypass u_xt s_ready
	}

	void ms_fwd_order()
	{
		if (m_ready.read()) {
			m_block[0].write((c_xt_data[0].read() + NBMASK) ^ NBMASK);
			m_block[1].write((c_xt_data[1].read() + NBMASK) ^ NBMASK);
			m_block[2].write((c_xt_data[2].read() + NBMASK) ^ NBMASK);
			m_block[3].write((c_xt_data[3].read() + NBMASK) ^ NBMASK);
			m_valid.write(c_xt_valid.read());
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

	SC_CTOR(encode_block) :
		u_xt("u_xt")
	{
		u_xt.clk(clk);
		u_xt.reset(reset);
		u_xt.s_valid(s_valid);
		u_xt.s_ready(c_xt_ready); // output not used
		u_xt.m_valid(c_xt_valid);
		u_xt.m_ready(m_ready);
		for (int i = 0; i < 4; i++) {
			u_xt.s_port[i](s_block[i]); // in x
			u_xt.m_port[i](c_xt_data[i]);
		}

		SC_METHOD(mc_proc);
			sensitive << m_ready;
		SC_METHOD(ms_fwd_order);
			sensitive << clk.pos();
			dont_initialize();
	}

};

template<typename FP>
struct encode_block<FP, 2> : sc_module
{
	typedef typename FP::si_t si_t;
	typedef typename FP::ui_t ui_t;

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_in <si_t> s_block[fpblk_sz(2)];
	sc_in <bool> s_valid;
	sc_out<bool> s_ready;

	sc_out<ui_t> m_block[fpblk_sz(2)];
	sc_out<bool> m_valid;
	sc_in <bool> m_ready;

	/*-------- registers --------*/

	/*-------- channels --------*/
	sc_signal<si_t> c_xt_data[4][4];
	sc_signal<bool> c_xt_valid[4];
	sc_signal<bool> c_xt_ready[4];

	sc_signal<si_t> c_yt_data[4][4];
	sc_signal<bool> c_yt_valid[4];
	sc_signal<bool> c_yt_ready[4];

	/*-------- modules --------*/
	// sc_vector4<fwd_lift<FP> > u_xt;
	// sc_vector4<fwd_lift<FP> > u_yt;
	fwd_lift<FP> u_xt[4];
	fwd_lift<FP> u_yt[4];

	void mc_proc()
	{
		s_ready.write(m_ready.read()); // bypass u_xt and u_yt s_ready
	}

	void ms_fwd_order()
	{
		if (m_ready.read()) {
			m_block[ 0].write((c_yt_data[0][0].read() + NBMASK) ^ NBMASK);
			m_block[ 1].write((c_yt_data[0][1].read() + NBMASK) ^ NBMASK);
			m_block[ 2].write((c_yt_data[1][0].read() + NBMASK) ^ NBMASK);
			m_block[ 3].write((c_yt_data[1][1].read() + NBMASK) ^ NBMASK);
			m_block[ 4].write((c_yt_data[0][2].read() + NBMASK) ^ NBMASK);
			m_block[ 5].write((c_yt_data[2][0].read() + NBMASK) ^ NBMASK);
			m_block[ 6].write((c_yt_data[1][2].read() + NBMASK) ^ NBMASK);
			m_block[ 7].write((c_yt_data[2][1].read() + NBMASK) ^ NBMASK);
			m_block[ 8].write((c_yt_data[0][3].read() + NBMASK) ^ NBMASK);
			m_block[ 9].write((c_yt_data[3][0].read() + NBMASK) ^ NBMASK);
			m_block[10].write((c_yt_data[2][2].read() + NBMASK) ^ NBMASK);
			m_block[11].write((c_yt_data[1][3].read() + NBMASK) ^ NBMASK);
			m_block[12].write((c_yt_data[3][1].read() + NBMASK) ^ NBMASK);
			m_block[13].write((c_yt_data[2][3].read() + NBMASK) ^ NBMASK);
			m_block[14].write((c_yt_data[3][2].read() + NBMASK) ^ NBMASK);
			m_block[15].write((c_yt_data[3][3].read() + NBMASK) ^ NBMASK);
			m_valid.write(c_yt_valid[0].read());
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

	SC_CTOR(encode_block) :
	  // u_xt("u_xt"),
	  // y_xt("u_yt")
	  u_xt{{"u_xt_0"}, {"u_xt_1"}, {"u_xt_2"}, {"u_xt_3"}},
	  u_yt{{"u_yt_0"}, {"u_yt_1"}, {"u_yt_2"}, {"u_yt_3"}}
	{
		for (int j = 0; j < 4; j++) {
			u_xt[j].clk(clk);
			u_xt[j].reset(reset);
			u_xt[j].s_valid(s_valid);
			u_xt[j].s_ready(c_xt_ready[j]); // output not used
			u_xt[j].m_valid(c_xt_valid[j]); // only index [0] used
			u_xt[j].m_ready(m_ready);

			u_yt[j].clk(clk);
			u_yt[j].reset(reset);
			u_yt[j].s_valid(c_xt_valid[0]);
			u_yt[j].s_ready(c_yt_ready[j]); // output not used
			u_yt[j].m_valid(c_yt_valid[j]); // only index [0] used
			u_yt[j].m_ready(m_ready);

			for (int i = 0; i < 4; i++) {
				u_xt[j].s_port[i](s_block[(j*4)+i]); // in x
				u_xt[j].m_port[i](c_xt_data[j][i]);

				u_yt[j].s_port[i](c_xt_data[i][j]); // in y
				u_yt[j].m_port[i](c_yt_data[i][j]);
			}
		}

		SC_METHOD(mc_proc);
			sensitive << m_ready;
		SC_METHOD(ms_fwd_order);
			sensitive << clk.pos();
			dont_initialize();
	}

};

template<typename FP>
struct encode_block<FP, 3> : sc_module
{
	typedef typename FP::si_t si_t;
	typedef typename FP::ui_t ui_t;

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_in <si_t> s_block[fpblk_sz(3)];
	sc_in <bool> s_valid;
	sc_out<bool> s_ready;

	sc_out<ui_t> m_block[fpblk_sz(3)];
	sc_out<bool> m_valid;
	sc_in <bool> m_ready;

	/*-------- registers --------*/

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
	fwd_lift<FP>  u_xt[4][4];
	fwd_lift<FP>  u_yt[4][4];
	fwd_lift<FP>  u_zt[4][4];

	void mc_proc()
	{
		s_ready.write(m_ready.read()); // bypass u_xt, u_yt, & u_zt s_ready
	}

	void ms_fwd_order()
	{
		if (m_ready.read()) {
			m_block[ 0].write((c_zt_data[0][0][0].read() + NBMASK) ^ NBMASK);
			m_block[ 1].write((c_zt_data[0][0][1].read() + NBMASK) ^ NBMASK);
			m_block[ 2].write((c_zt_data[0][1][0].read() + NBMASK) ^ NBMASK);
			m_block[ 3].write((c_zt_data[1][0][0].read() + NBMASK) ^ NBMASK);
			m_block[ 4].write((c_zt_data[1][1][0].read() + NBMASK) ^ NBMASK);
			m_block[ 5].write((c_zt_data[1][0][1].read() + NBMASK) ^ NBMASK);
			m_block[ 6].write((c_zt_data[0][1][1].read() + NBMASK) ^ NBMASK);
			m_block[ 7].write((c_zt_data[0][0][2].read() + NBMASK) ^ NBMASK);
			m_block[ 8].write((c_zt_data[0][2][0].read() + NBMASK) ^ NBMASK);
			m_block[ 9].write((c_zt_data[2][0][0].read() + NBMASK) ^ NBMASK);
			m_block[10].write((c_zt_data[1][1][1].read() + NBMASK) ^ NBMASK);
			m_block[11].write((c_zt_data[0][1][2].read() + NBMASK) ^ NBMASK);
			m_block[12].write((c_zt_data[1][0][2].read() + NBMASK) ^ NBMASK);
			m_block[13].write((c_zt_data[1][2][0].read() + NBMASK) ^ NBMASK);
			m_block[14].write((c_zt_data[0][2][1].read() + NBMASK) ^ NBMASK);
			m_block[15].write((c_zt_data[2][0][1].read() + NBMASK) ^ NBMASK);
			m_block[16].write((c_zt_data[2][1][0].read() + NBMASK) ^ NBMASK);
			m_block[17].write((c_zt_data[0][0][3].read() + NBMASK) ^ NBMASK);
			m_block[18].write((c_zt_data[0][3][0].read() + NBMASK) ^ NBMASK);
			m_block[19].write((c_zt_data[3][0][0].read() + NBMASK) ^ NBMASK);
			m_block[20].write((c_zt_data[1][1][2].read() + NBMASK) ^ NBMASK);
			m_block[21].write((c_zt_data[1][2][1].read() + NBMASK) ^ NBMASK);
			m_block[22].write((c_zt_data[2][1][1].read() + NBMASK) ^ NBMASK);
			m_block[23].write((c_zt_data[2][2][0].read() + NBMASK) ^ NBMASK);
			m_block[24].write((c_zt_data[2][0][2].read() + NBMASK) ^ NBMASK);
			m_block[25].write((c_zt_data[0][2][2].read() + NBMASK) ^ NBMASK);
			m_block[26].write((c_zt_data[0][1][3].read() + NBMASK) ^ NBMASK);
			m_block[27].write((c_zt_data[1][0][3].read() + NBMASK) ^ NBMASK);
			m_block[28].write((c_zt_data[1][3][0].read() + NBMASK) ^ NBMASK);
			m_block[29].write((c_zt_data[0][3][1].read() + NBMASK) ^ NBMASK);
			m_block[30].write((c_zt_data[3][0][1].read() + NBMASK) ^ NBMASK);
			m_block[31].write((c_zt_data[3][1][0].read() + NBMASK) ^ NBMASK);
			m_block[32].write((c_zt_data[2][2][1].read() + NBMASK) ^ NBMASK);
			m_block[33].write((c_zt_data[2][1][2].read() + NBMASK) ^ NBMASK);
			m_block[34].write((c_zt_data[1][2][2].read() + NBMASK) ^ NBMASK);
			m_block[35].write((c_zt_data[1][1][3].read() + NBMASK) ^ NBMASK);
			m_block[36].write((c_zt_data[1][3][1].read() + NBMASK) ^ NBMASK);
			m_block[37].write((c_zt_data[3][1][1].read() + NBMASK) ^ NBMASK);
			m_block[38].write((c_zt_data[0][2][3].read() + NBMASK) ^ NBMASK);
			m_block[39].write((c_zt_data[2][0][3].read() + NBMASK) ^ NBMASK);
			m_block[40].write((c_zt_data[2][3][0].read() + NBMASK) ^ NBMASK);
			m_block[41].write((c_zt_data[0][3][2].read() + NBMASK) ^ NBMASK);
			m_block[42].write((c_zt_data[3][0][2].read() + NBMASK) ^ NBMASK);
			m_block[43].write((c_zt_data[3][2][0].read() + NBMASK) ^ NBMASK);
			m_block[44].write((c_zt_data[2][2][2].read() + NBMASK) ^ NBMASK);
			m_block[45].write((c_zt_data[1][2][3].read() + NBMASK) ^ NBMASK);
			m_block[46].write((c_zt_data[2][1][3].read() + NBMASK) ^ NBMASK);
			m_block[47].write((c_zt_data[2][3][1].read() + NBMASK) ^ NBMASK);
			m_block[48].write((c_zt_data[1][3][2].read() + NBMASK) ^ NBMASK);
			m_block[49].write((c_zt_data[3][1][2].read() + NBMASK) ^ NBMASK);
			m_block[50].write((c_zt_data[3][2][1].read() + NBMASK) ^ NBMASK);
			m_block[51].write((c_zt_data[3][3][0].read() + NBMASK) ^ NBMASK);
			m_block[52].write((c_zt_data[3][0][3].read() + NBMASK) ^ NBMASK);
			m_block[53].write((c_zt_data[0][3][3].read() + NBMASK) ^ NBMASK);
			m_block[54].write((c_zt_data[2][2][3].read() + NBMASK) ^ NBMASK);
			m_block[55].write((c_zt_data[2][3][2].read() + NBMASK) ^ NBMASK);
			m_block[56].write((c_zt_data[3][2][2].read() + NBMASK) ^ NBMASK);
			m_block[57].write((c_zt_data[3][3][1].read() + NBMASK) ^ NBMASK);
			m_block[58].write((c_zt_data[3][1][3].read() + NBMASK) ^ NBMASK);
			m_block[59].write((c_zt_data[1][3][3].read() + NBMASK) ^ NBMASK);
			m_block[60].write((c_zt_data[3][3][2].read() + NBMASK) ^ NBMASK);
			m_block[61].write((c_zt_data[3][2][3].read() + NBMASK) ^ NBMASK);
			m_block[62].write((c_zt_data[2][3][3].read() + NBMASK) ^ NBMASK);
			m_block[63].write((c_zt_data[3][3][3].read() + NBMASK) ^ NBMASK);
			m_valid.write(c_zt_valid[0][0].read());
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

	SC_CTOR(encode_block) :
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
		for (int k = 0; k < 4; k++) {
			for (int j = 0; j < 4; j++) {
				u_xt[k][j].clk(clk);
				u_xt[k][j].reset(reset);
				u_xt[k][j].s_valid(s_valid);
				u_xt[k][j].s_ready(c_xt_ready[k][j]); // output not used
				u_xt[k][j].m_valid(c_xt_valid[k][j]); // only index [0][0] used
				u_xt[k][j].m_ready(m_ready);

				u_yt[k][j].clk(clk);
				u_yt[k][j].reset(reset);
				u_yt[k][j].s_valid(c_xt_valid[0][0]);
				u_yt[k][j].s_ready(c_yt_ready[k][j]); // output not used
				u_yt[k][j].m_valid(c_yt_valid[k][j]); // only index [0][0] used
				u_yt[k][j].m_ready(m_ready);

				u_zt[k][j].clk(clk);
				u_zt[k][j].reset(reset);
				u_zt[k][j].s_valid(c_yt_valid[0][0]);
				u_zt[k][j].s_ready(c_zt_ready[k][j]); // output not used
				u_zt[k][j].m_valid(c_zt_valid[k][j]); // only index [0][0] used
				u_zt[k][j].m_ready(m_ready);

				for (int i = 0; i < 4; i++) {
					u_xt[k][j].s_port[i](s_block[(k*4+j)*4+i]); // in x
					u_xt[k][j].m_port[i](c_xt_data[k][j][i]);

					u_yt[k][j].s_port[i](c_xt_data[k][i][j]); // in y
					u_yt[k][j].m_port[i](c_yt_data[k][i][j]);

					u_zt[k][j].s_port[i](c_yt_data[i][k][j]); // in z
					u_zt[k][j].m_port[i](c_zt_data[i][k][j]);
				}
			}
		}

		SC_METHOD(mc_proc);
			sensitive << m_ready;
		SC_METHOD(ms_fwd_order);
			sensitive << clk.pos();
			dont_initialize();
	}

};

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

//-----------------------------------------------------------------------------
// compress sequence of unsigned integers
//-----------------------------------------------------------------------------
template<typename FP, int DIM>
SC_MODULE(encode_ints)
{
	static constexpr int planes = FP::bits; // number of bit planes

	typedef typename FP::ui_t ui_t;

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_in <ui_t> s_block[fpblk_sz(DIM)];
	sc_in <bool> s_valid;
	sc_out<bool> s_ready;

	sc_in <bool> s_flush;

	sc_out<sc_uint<bc_w(DIM)> > m_bc;
	sc_out<sc_bv<bp_w(DIM)> > m_bp;
	sc_out<bool> m_last;
	sc_out<bool> m_valid;
	sc_in <bool> m_ready;

	/*-------- registers --------*/
	sc_signal<sc_uint<log2rz(planes-1)+1> > k0;
	sc_signal<sc_uint<2*DIM+1> > n0;
	sc_signal<sc_uint<2*DIM+1> > n1;
	sc_signal<sc_uint<2*DIM+1> > bc1;
	sc_signal<sc_bv<fpblk_sz(DIM)> > bp1;
	sc_signal<bool> last1;
	sc_signal<bool> valid1;
	sc_signal<bool> flush1;

	/*-------- channels --------*/
	sc_signal<sc_bv<fpblk_sz(DIM)> > c_bplane[planes];

	/*-------- modules --------*/

	void mc_proc()
	{
		bool last =
			k0.read() == planes-1 || // done processing planes
			(s_flush.read() && s_flush.read() != flush1.read()); // flush

		// rewire block as bit planes
		sc_bv<fpblk_sz(DIM)> tmp;
		for (int j = 0; j < planes; j++) {
			for (int i = 0; i < fpblk_sz(DIM); i++) {
				tmp[i] = s_block[i].read()[j].to_bool();
			}
			c_bplane[(planes-1)-j].write(tmp); // reverse plane order
		}

		s_ready.write(
			(last && m_ready.read()) || // last and ready
			!s_valid.read() // prime
		);
	}

	void ms_proc()
	{
		if (reset == RLEVEL) {
			k0 = 0;
			n0 = 0; n1 = 0;
			bc1 = 0; m_bc = 0;
			bp1 = 0; m_bp = 0;
			last1 = false; m_last = false;
			valid1 = false; m_valid = false;
		} else {
			if (m_ready.read()) {
				bool last =
					k0.read() == planes-1 || // done processing planes
					(s_flush.read() && s_flush.read() != flush1.read()); // flush

				// Stage 1 - find most significant bit (bc1-1)
				unsigned b = 0;
				bool frst = 1;
				for (unsigned i = fpblk_sz(DIM); i > 0; i--) {
					if (c_bplane[k0.read()].read()[i-1]) {
					  if (frst) {
					    frst=0;
					    b = i;
					  }
					}
				}
				n1.write(n0.read());
				bc1.write(b);
				bp1.write(c_bplane[k0.read()].read());
				last1.write(last);
				valid1.write(s_valid.read());
				flush1.write(s_flush.read());
				if (s_valid.read()) { // advance
					if (b > n0.read()) n0.write(b);
					if (last) {k0 = 0; n0 = 0;}
					else k0 = k0.read() + 1;
				}

				// Stage 2a - copy bits upto n1
				sc_bv<bp_w(DIM)> tmp = 0;
				/** The original code */
				// unsigned j = n1.read();
				// for (unsigned i = 0; i < n1.read(); i++) {
				//	tmp[i] = bp1.read()[i].to_bool();
				// }

				// // Stage 2b - encode remaining bits >= n1
				// unsigned n;
				// for (n = n1.read(); n < bc1.read(); n++) {
				//	tmp[j++] = true; // group bit
				//	while (!bp1.read()[n].to_bool()) {n++; tmp[j++] = false;}
				//	// conditional for implicit bit
				//	if (n != fpblk_sz(DIM)-1) tmp[j++] = true;
				// }
				// // conditional for implicit zero
				// if (n != fpblk_sz(DIM)) tmp[j++] = false;
				/** The transformed code */
				/**
				 * 1. generate the encoded 2-bit for each bp1 bit
				 * 2. in the meantime of 1e also record the correct values of start and end position of j (n1_shiftamt and first_after_bc)
				 * 3. slice encoded bp1 and concatenate it with bp1 prefix, this is done with bitwise operations
				 * 4. The final j is generated by arithmetic operations
				 */
				unsigned n;
				unsigned shiftamt = 0; 
				unsigned n1_shiftamt = 0;
				unsigned first_after_bc = n1.read(); 
				unsigned last_n = n1.read();

				bool found = false;

				/* step 1, 2 */
				shiftamt = 0;
				for(n = 0; n < fpblk_sz(DIM); n++) { 
					if(n == n1.read()) {
						n1_shiftamt = shiftamt;
					}
					if(n == 0) {
						shiftamt += 2;
						if(bp1.read()[0]) {
							tmp[0] = 1;
							tmp[1] = 1;
						} else {
							tmp[0] = 1;
							tmp[1] = 0;
						}
					} else {
						if(bp1.read()[n].to_bool() && bp1.read()[n - 1].to_bool()) { // a 1'b1 maps to 2'b11
							tmp[shiftamt] = 1;
							tmp[shiftamt + 1] = 1;
							shiftamt = shiftamt + 2;
						} else if(bp1.read()[n].to_bool() && !bp1.read()[n - 1].to_bool()) {
							// Note: in this "1" followed by "0" pattern, 1 will be skipped in the main loop
							if(n != fpblk_sz(DIM) - 1) {
								tmp[shiftamt] = 1;
								shiftamt = shiftamt + 1;
							}
						}else if(!bp1.read()[n].to_bool() && !bp1.read()[n-1].to_bool()) { // A 0 following a 0 maps to 1'b0
							tmp[shiftamt] = 0;
							shiftamt = shiftamt + 1;
						} else if(!bp1.read()[n].to_bool() && bp1.read()[n-1].to_bool()) { // A 0 following a 1 maps to 2'b01
							tmp[shiftamt] = 1;
							tmp[shiftamt + 1] = 0;
							shiftamt = shiftamt + 2;
						}
					}
					// trying to locate last j modified
					if(!found && bp1.read()[n].to_bool() && n >= bc1.read() - 1 && n1 < bc1.read()) {
						found = true;
						if(bp1.read()[n].to_bool() && !bp1.read()[n - 1].to_bool()) {
							if(n != fpblk_sz(DIM) - 1)
								first_after_bc = shiftamt;
							else {
								if(bc1.read() == fpblk_sz(DIM)) {
									first_after_bc = shiftamt;
								} else {
									first_after_bc = shiftamt - 1;
								}
							}
						} else { // 10
							if(n != fpblk_sz(DIM) - 1)
								first_after_bc = shiftamt;
							else
								first_after_bc = shiftamt - 1;
						}
						last_n = n + 1; // the last n before the final n++
					}
				}
				/* step 3 */
				sc_bv<bp_w(DIM)> shiftmask = -1;
				if(first_after_bc < bp_w(DIM)) {
					tmp = tmp & ~(shiftmask << (first_after_bc));
				}
				unsigned delta = 0;
				if(last_n != fpblk_sz(DIM)) tmp[first_after_bc] = 0;
				if(n1.read() >= bc1.read()) {
					tmp = 0;
				} else {
					if(n1.read() == 0) {
						// nothing needs to change, the first bit is correct and we only take from tmp
					} else {
						if( (!bp1.read()[n1.read()] && !bp1.read()[n1.read() - 1]) || (bp1.read()[n1.read()] && !bp1.read()[n1.read() - 1])) {
							tmp = (((tmp >> n1_shiftamt) << 1) | 1) << n1.read();
							delta = 1;
						} else {
							tmp = tmp >> n1_shiftamt << n1.read();
						}
					}
				}
				sc_bv<bp_w(DIM)> shiftmask_2 = -1;
				tmp = (tmp & (shiftmask_2 << n1.read())) | (bp1.read() & ~(shiftmask_2 << n1.read()));
				/* step 4 */
				unsigned j = 0;
				if(n1.read() >= bc1.read()) {
					// loop is not executed
					j = n1.read();
					if(n1.read() != fpblk_sz(DIM)) {
						j = j + 1;
					}
				} else {
					j = first_after_bc + delta - n1_shiftamt + n1.read();
					if(last_n != fpblk_sz(DIM)) {
						j = j + 1;
					}
				}
				/** new code ends */


				m_bc.write(j); // derived from n1, bc1 & bp1
				m_bp.write(tmp); //
				m_last.write(last1.read());
				m_valid.write(valid1.read());
			} else {
				if (!s_flush.read()) flush1.write(false);
			}
		}
	}

#if defined(VCD)
	void start_of_simulation()
	{
		for (int i = 0; i < fpblk_sz(DIM); i++)
			sc_trace(tf, s_block[i],
				(std::string(name())+".s_block"+std::to_string(i)).c_str());
		sc_trace(tf, s_valid, (std::string(name())+".s_valid").c_str());
		sc_trace(tf, s_ready, (std::string(name())+".s_ready").c_str());

		sc_trace(tf, s_flush, (std::string(name())+".s_flush").c_str());

		sc_trace(tf, m_bc,    (std::string(name())+".m_bc").c_str());
		sc_trace(tf, m_bp,    (std::string(name())+".m_bp").c_str());
		sc_trace(tf, m_last,  (std::string(name())+".m_last").c_str());
		sc_trace(tf, m_valid, (std::string(name())+".m_valid").c_str());
		sc_trace(tf, m_ready, (std::string(name())+".m_ready").c_str());

		sc_trace(tf, k0,   (std::string(name())+".k0").c_str());
		sc_trace(tf, n0,  (std::string(name())+".n0").c_str());
		sc_trace(tf, n1,  (std::string(name())+".n1").c_str());
		sc_trace(tf, bc1, (std::string(name())+".bc1").c_str());
		sc_trace(tf, bp1, (std::string(name())+".bp1").c_str());

		sc_trace(tf, last1,  (std::string(name())+".last1").c_str());
		sc_trace(tf, valid1, (std::string(name())+".valid1").c_str());
	}
#endif

	SC_CTOR(encode_ints)
	{
		SC_METHOD(mc_proc);
			for (int i = 0; i < fpblk_sz(DIM); i++)
				sensitive << s_block[i];
			sensitive << k0 << flush1 << m_ready << s_valid << s_flush;
		SC_METHOD(ms_proc);
			sensitive << clk.pos();
			dont_initialize();
	}

};

//-----------------------------------------------------------------------------
// pack bitplanes into a bitstream
//-----------------------------------------------------------------------------
template<typename FP, int DIM, typename B>
SC_MODULE(encode_stream)
{
	typedef typename FP::expo_t expo_t;

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_in <uconfig_t> minbits;
	sc_in <uconfig_t> maxbits;
	sc_in <uconfig_t> maxprec;
	sc_in <sconfig_t> minexp; // assume biased

	sc_stream_in<expo_t> s_ex; // exponent, biased

	sc_in <sc_uint<bc_w(DIM)> > s_bc; // bit count
	sc_in <sc_bv<bp_w(DIM)> > s_bp; // bit plane
	sc_in <bool> s_last;
	sc_in <bool> s_valid;
	sc_out<bool> s_ready;

	sc_out<bool> m_flush;

	sc_stream_out<B> m_bits; // assume B has tdata and tlast fields

	/*-------- local --------*/
	// precision width
	static constexpr int pre_w = log2rz(FP::bits)+1;
	// total bit count width
	static constexpr int tbc_w = log2rz(bp_w(DIM)*FP::bits+FP::ebits+1)+1;
	// buffer width
	static constexpr int buf_w = max(bp_w(DIM),B::dbits)+B::dbits;
	// output buffer position width
	static constexpr int pos_w = log2rz(buf_w)+1;

	enum state_e {START, ZERO, EXPO, PLANES, PAD};

	struct state_t {
		sc_uint<3>     s;      // state

		sc_uint<pre_w> prec;   // precision (planes to encode)
		sc_uint<pre_w> planes; // planes encoded

		sc_bv<buf_w>   buf;    // output buffer
		sc_uint<tbc_w> bits;   // bits encoded (through output buffer)
		sc_uint<pos_w> posr;   // read bit position in output buffer
		sc_uint<pos_w> posw;   // write bit position in output buffer

		// use default constructor and assignment

		bool operator==(const state_t& ob)
		{
			return
				this->s      == ob.s &&
				this->prec   == ob.prec &&
				this->planes == ob.planes &&
				this->buf    == ob.buf &&
				this->bits   == ob.bits &&
				this->posr   == ob.posr &&
				this->posw   == ob.posw;
		}

		friend std::ostream& operator<<(std::ostream& os, const state_t& ob)
		{
			return os << hex
				<< ob.s      << ':'
				<< ob.prec   << ':'
				<< ob.planes << ':'
				<< ob.buf    << ':'
				<< ob.bits   << ':'
				<< ob.posr   << ':'
				<< ob.posw;
		}

		friend void sc_trace(sc_trace_file* tf, const state_t& ob, const std::string& nm)
		{
			sc_trace(tf, ob.s,      nm+".s");
			sc_trace(tf, ob.prec,   nm+".prec");
			sc_trace(tf, ob.planes, nm+".planes");
			sc_trace(tf, ob.buf,    nm+".buf");
			sc_trace(tf, ob.bits,   nm+".bits");
			sc_trace(tf, ob.posr,   nm+".posr");
			sc_trace(tf, ob.posw,   nm+".posw");
		}
	};

	sc_signal<state_t> cs, ns;  // current and next state

	/*-------- modules --------*/

	// checks for reaching maximum bits.
	// add bits to output buffer, if space.
	bool pack_bits(state_t &ts, sc_uint<tbc_w> bc, sc_bv<buf_w> bp)
	{
		sc_uint<tbc_w> n;

		n = sc_min(uconfig_t(bc), uconfig_t(maxbits.read()-ts.bits));
		// if space, add bits to buffer
		if (buf_w-ts.posw >= n) {
			ts.buf(ts.posw+n-1,ts.posw) = bp(n-1,0);
			ts.bits = ts.bits+n;
			ts.posw = ts.posw+n;
			return true;
		}
		return false;
	}

	// assume maxbits already checked when bits were inserted in output buffer.
	// done indicates that final bits have been added to the output buffer.
	// the return value indicates flush done.
	bool out_bits(state_t &ts, bool done)
	{
		sc_uint<pos_w> bbits; // number of bits in output buffer

		bbits = ts.posw - ts.posr;
		// if enough for flit and output port ready, output bits from buffer
		if (bbits >= B::dbits && m_bits.ready_r()) {
			B flit;
			flit.tdata = ts.buf(ts.posr+B::dbits-1, ts.posr);
			flit.tlast = done && bbits == B::dbits; // flush done
			m_bits.data_w(flit);
			m_bits.valid_w(true);
			// realign output in buffer
			ts.buf(ts.posw-B::dbits, 0) = ts.buf(ts.posw-1, ts.posr+B::dbits);
			ts.posr = 0;
			ts.posw = bbits - B::dbits;
			return flit.tlast;
		}
		return done && bbits == 0;
	}

	void mc_proc()
	{
		state_t ts; // temporary state
		bool dis_done; // discard done
		bool add_done; // add done
		bool pad_done; // pad done
		bool fls_done; // flush done

		s_ex.ready_w(false);
		s_ready.write(false);
		m_flush.write(false);
		m_bits.valid_w(false);
		ts = cs; // get current state
		switch (ts.s) {
		case START: {
			// read next max exponent, calculate precision
			// (see zfp/src/template/codecf.c:precision)
			ts.prec = sc_min(maxprec.read(),
			        uconfig_t(sc_max(sconfig_t(0), sconfig_t(s_ex.data_r() - minexp.read() + 2 * (DIM + 1)))));
			// reset counters and registers
			ts.bits   = 0;
			ts.planes = 0;
			ts.buf    = 0;
			ts.posr   = 0;
			ts.posw   = 0;
			if (s_ex.valid_r()) {
				if (s_ex.data_r() != 0 && ts.prec != 0) {
					ts.s = EXPO; // next: if biased max exponent and prec is nonzero
				} else {
					ts.s = ZERO; // next: otherwise
				}
			}
			break;
		}
		case ZERO: {
			// encode '0' - does not check maxbits
			ts.buf[0] = false;
			ts.bits = 1;
			ts.posw = 1;
			s_ex.ready_w(true);
			ts.s = PAD; // next:
			break;
		}
		case EXPO: {
			// encode '1' and exponent - does not check maxbits
			// constraint - buffer width >= ebits+1
			static_assert(buf_w >= FP::ebits+1, "Buffer width must be >= exponent width + 1");
			ts.buf[0] = true;
			ts.buf(FP::ebits, 1) = s_ex.data_r();
			ts.bits = FP::ebits+1;
			ts.posw = FP::ebits+1;
			s_ex.ready_w(true);
			if (ts.planes < ts.prec) ts.s = PLANES; // next:
			else ts.s = PAD; // next:
			break;
		}
		case PLANES: {
			// pack bits from bit plane into output buffer
			if (s_valid.read()) {
				if (pack_bits(ts, s_bc.read(), s_bp.read())) {
					s_ready.write(true);
					if (s_last.read()) ts.planes = FP::bits; // signal end of planes
					else ts.planes = ts.planes+1U;
				}
			} else s_ready.write(true);
			dis_done = ts.planes == FP::bits; // discard done
			add_done = ts.planes >= ts.prec || ts.bits >= maxbits.read(); // add done
			pad_done = ts.bits >= minbits.read(); // pad done
			fls_done = out_bits(ts, add_done && pad_done); // flush done
			// encode while planes < prec and bits < maxbits
			if (add_done) {
				if (dis_done && pad_done && fls_done) ts.s = START; // next:
				else ts.s = PAD; // next:
			}
			break;
		}
		case PAD: {
			// discard remaining planes, look for last, do in parallel with pad
			if (s_valid.read()) {
				if (ts.planes < FP::bits) {
					if (ts.planes < FP::bits-2) m_flush.write(true); // if not too close to end
					s_ready.write(true);
					if (s_last.read()) ts.planes = FP::bits; // signal end of planes
					else ts.planes = ts.planes+1U;
				}
			} else s_ready.write(true);
			// pad with '0' while bits < minbits
			if (ts.bits < minbits.read()) {
				sc_uint<tbc_w> pbits = minbits.read() - ts.bits; // pad bits remaining
				sc_uint<tbc_w> abits = B::dbits-(ts.bits & (B::dbits-1)); // align to a flit
				sc_uint<tbc_w> n = sc_min(pbits,abits); // don't add more than a flit at a time
				pack_bits(ts, n, sc_bv<B::dbits>(0));
			}
			dis_done = ts.planes == FP::bits; // discard done
			pad_done = ts.bits >= minbits.read(); // pad done
			fls_done = out_bits(ts, pad_done); // flush done
			// if discard, pad and flush done
			if (dis_done && pad_done && fls_done) ts.s = START; // next:
			break;
		}
		}
		ns = ts;
	}

	void ms_proc()
	{
		if (reset == RLEVEL) {
			cs = state_t();
		} else {
			cs = ns;
		}
	}

#if defined(VCD)
	void start_of_simulation()
	{
		sc_trace(tf, minbits, (std::string(name())+".minbits").c_str());
		sc_trace(tf, maxbits, (std::string(name())+".maxbits").c_str());
		sc_trace(tf, maxprec, (std::string(name())+".maxprec").c_str());
		sc_trace(tf, minexp,  (std::string(name())+".minexp").c_str());

		sc_trace(tf, s_ex,    (std::string(name())+".s_ex").c_str());

		sc_trace(tf, s_bc,    (std::string(name())+".s_bc").c_str());
		sc_trace(tf, s_bp,    (std::string(name())+".s_bp").c_str());
		sc_trace(tf, s_last,  (std::string(name())+".s_last").c_str());
		sc_trace(tf, s_valid, (std::string(name())+".s_valid").c_str());
		sc_trace(tf, s_ready, (std::string(name())+".s_ready").c_str());

		sc_trace(tf, m_flush, (std::string(name())+".m_flush").c_str());

		sc_trace(tf, m_bits,  (std::string(name())+".m_bits").c_str());

		sc_trace(tf, cs,      (std::string(name())+".cs").c_str());
	}
#endif // VCD

	SC_CTOR(encode_stream)
	{
		SC_METHOD(mc_proc);
			sensitive << s_ex.valid_chg() << s_ex.data_chg();
			sensitive << m_bits.ready_chg();
			sensitive << minbits << maxbits << maxprec << minexp;
			sensitive << s_bc << s_bp << s_last << s_valid;
			sensitive << cs;
		SC_METHOD(ms_proc);
			sensitive << clk.pos();
			dont_initialize();
	}

};

//-----------------------------------------------------------------------------
// Encode IEEE floating-point values to stream
//-----------------------------------------------------------------------------
template<typename FP, int DIM, typename B>
SC_MODULE(encode)
{
	typedef typename FP::expo_t expo_t;
	typedef typename FP::si_t si_t;
	typedef typename FP::ui_t ui_t;

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_in <uconfig_t> minbits;
	sc_in <uconfig_t> maxbits;
	sc_in <uconfig_t> maxprec;
	sc_in <sconfig_t> minexp; // assume biased

	sc_stream_in <FP> s_fp;
	sc_stream_out<B>  m_bits;

	/*-------- registers --------*/

	/*-------- channels --------*/
	sc_stream<FP>     c_fe_fp;
	sc_stream<expo_t> c_fe_ex;

	sc_stream<expo_t> c_sp_ex1;
	sc_stream<expo_t> c_sp_ex2;

	sc_signal<si_t> c_fc_block[fpblk_sz(DIM)];
	sc_signal<bool> c_fc_valid;
	sc_signal<bool> c_fc_ready;

	sc_signal<ui_t> c_eb_block[fpblk_sz(DIM)];
	sc_signal<bool> c_eb_valid;
	sc_signal<bool> c_eb_ready;

	sc_signal<ui_t> c_bb_block[fpblk_sz(DIM)];
	sc_signal<bool> c_bb_valid;
	sc_signal<bool> c_bb_ready;

	sc_signal<sc_uint<bc_w(DIM)> > c_ei_bc;
	sc_signal<sc_bv<bp_w(DIM)> > c_ei_bp;
	sc_signal<bool> c_ei_last;
	sc_signal<bool> c_ei_valid;
	sc_signal<bool> c_ei_ready;

	sc_signal<bool> c_es_flush;

	/*-------- modules --------*/
	find_emax<FP, DIM> u_find_emax;
	// the ssplit IW2 parameter should be sized according
	// to the maximum number of blocks in the pipeline
	ssplit<expo_t,1,4,RLEVEL> u_ssplit_ex;
	fwd_cast<FP, DIM> u_fwd_cast;
	encode_block<FP, DIM> u_encode_block;
	block_buffer<FP, DIM> u_block_buffer;
	encode_ints<FP, DIM> u_encode_ints;
	encode_stream<FP, DIM, B> u_encode_stream;

	SC_CTOR(encode) :
		u_find_emax("u_find_emax"),
		u_ssplit_ex("u_ssplit_ex"),
		u_fwd_cast("u_fwd_cast"),
		u_encode_block("u_encode_block"),
		u_block_buffer("u_block_buffer"),
		u_encode_ints("u_encode_ints"),
		u_encode_stream("u_encode_stream")
	{
		// connect u_find_emax
		u_find_emax.clk(clk);
		u_find_emax.reset(reset);
		u_find_emax.s_fp(s_fp);
		u_find_emax.m_fp(c_fe_fp);
		u_find_emax.m_ex(c_fe_ex);

		// connect u_ssplit_ex
		u_ssplit_ex.clk(clk);
		u_ssplit_ex.reset(reset);
		u_ssplit_ex.s_port(c_fe_ex);
		u_ssplit_ex.m_port1(c_sp_ex1);
		u_ssplit_ex.m_port2(c_sp_ex2);

		// connect u_fwd_cast
		u_fwd_cast.clk(clk);
		u_fwd_cast.reset(reset);
		u_fwd_cast.s_fp(c_fe_fp);
		u_fwd_cast.s_ex(c_sp_ex1);
		for (int i = 0; i < fpblk_sz(DIM); i++)
			u_fwd_cast.m_block[i](c_fc_block[i]);
		u_fwd_cast.m_valid(c_fc_valid);
		u_fwd_cast.m_ready(c_fc_ready);

		// connect u_encode_block
		u_encode_block.clk(clk);
		u_encode_block.reset(reset);
		for (int i = 0; i < fpblk_sz(DIM); i++)
			u_encode_block.s_block[i](c_fc_block[i]);
		u_encode_block.s_valid(c_fc_valid);
		u_encode_block.s_ready(c_fc_ready);
		for (int i = 0; i < fpblk_sz(DIM); i++)
			u_encode_block.m_block[i](c_eb_block[i]);
		u_encode_block.m_valid(c_eb_valid);
		u_encode_block.m_ready(c_eb_ready);

		// connect u_block_buffer
		u_block_buffer.clk(clk);
		u_block_buffer.reset(reset);
		for (int i = 0; i < fpblk_sz(DIM); i++)
			u_block_buffer.s_block[i](c_eb_block[i]);
		u_block_buffer.s_valid(c_eb_valid);
		u_block_buffer.s_ready(c_eb_ready);
		for (int i = 0; i < fpblk_sz(DIM); i++)
			u_block_buffer.m_block[i](c_bb_block[i]);
		u_block_buffer.m_valid(c_bb_valid);
		u_block_buffer.m_ready(c_bb_ready);

		// connect u_encode_ints
		u_encode_ints.clk(clk);
		u_encode_ints.reset(reset);
		for (int i = 0; i < fpblk_sz(DIM); i++)
			u_encode_ints.s_block[i](c_bb_block[i]);
		u_encode_ints.s_valid(c_bb_valid);
		u_encode_ints.s_ready(c_bb_ready);
		u_encode_ints.s_flush(c_es_flush);
		u_encode_ints.m_bc(c_ei_bc);
		u_encode_ints.m_bp(c_ei_bp);
		u_encode_ints.m_last(c_ei_last);
		u_encode_ints.m_valid(c_ei_valid);
		u_encode_ints.m_ready(c_ei_ready);

		// connect u_encode_stream
		u_encode_stream.clk(clk);
		u_encode_stream.reset(reset);
		u_encode_stream.minbits(minbits);
		u_encode_stream.maxbits(maxbits);
		u_encode_stream.maxprec(maxprec);
		u_encode_stream.minexp(minexp);
		u_encode_stream.s_ex(c_sp_ex2);
		u_encode_stream.s_bc(c_ei_bc);
		u_encode_stream.s_bp(c_ei_bp);
		u_encode_stream.s_last(c_ei_last);
		u_encode_stream.s_valid(c_ei_valid);
		u_encode_stream.s_ready(c_ei_ready);
		u_encode_stream.m_flush(c_es_flush);
		u_encode_stream.m_bits(m_bits);
	}

};

} // namespace zhw

/* TODO:
 * Why does hex uppercase not work when printing SystemC types?
 */
#endif // ZHW_ENCODE_H
