
#ifndef ZFP_H
#define ZFP_H

#include "systemc.h"

#if defined(RVD)
#include "sc_rvd.h"
#include "rvfifo_cc.h"
template <typename T> using sc_stream = sc_rvd<T>;
template <typename T> using sc_stream_in = sc_rvd_in<T>;
template <typename T> using sc_stream_out = sc_rvd_out<T>;
template <typename T, int IW,  bool RLEV> using sfifo_cc = rvfifo_cc<T, IW, RLEV>;

#else
#include "sc_stream_ports.h"
#include "sc_stream.h"
#include "sfifo_cc.h"
#endif

#include "sreg.h"

// floating-point block size given dimension
constexpr int fpblk_sz(int dim) {return 1 << 2*dim;} // 4^dim;
// #define fpblk_sz(dim) (1 << 2*dim)

extern sc_trace_file *tf;

//-----------------------------------------------------------------------------
// floating-point type
//-----------------------------------------------------------------------------
template<int E, int F>
struct fp_t {
	static constexpr int ebits = E;
	static constexpr int fbits = F;
	static constexpr int bits = 1+E+F;

	// exponent bias
	// When E (bits) =  8, ebias =  127
	// When E (bits) = 11, ebias = 1023
	static constexpr int ebias = (1 << (E-1))-1;

	typedef sc_int <bits> si_t;
	typedef sc_uint<bits> ui_t;

	typedef sc_uint<F> frac_t;
	typedef sc_uint<E> expo_t;
	typedef sc_uint<1> sign_t;

	frac_t frac;
	expo_t expo; // biased by ebias
	sign_t sign;

	fp_t(ui_t ui = 0)
	{
		(sign,expo,frac) = ui;
	}

	fp_t& operator=(ui_t ui)
	{
		(sign,expo,frac) = ui;
		return *this;
	}

	operator ui_t() const
	{
		return (sign,expo,frac);
	}

	bool operator==(const fp_t& fp)
	{
		return
			this->frac == fp.frac &&
			this->expo == fp.expo &&
			this->sign == fp.sign;
	}
};

template<int E, int F>
inline std::ostream& operator<<(std::ostream& os, const fp_t<E,F>& fp)
{
	return os << hex << fp.sign << ':'  << fp.expo << ':' << fp.frac;
}

template<int E, int F>
void sc_trace(sc_trace_file* tf, const fp_t<E,F>& ob, const std::string& nm)
{
	sc_trace(tf, ob.frac, nm+".frac");
	sc_trace(tf, ob.expo, nm+".expo");
	sc_trace(tf, ob.sign, nm+".sign");
}

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

	sc_stream_in <FP>     s_fp;
	sc_stream_out<FP>     m_fp;
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
#endif // VCD

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
			reset_signal_is(reset, RLEVEL);
			dont_initialize();
	}

};

//-----------------------------------------------------------------------------
// Encode IEEE floating-point values
//-----------------------------------------------------------------------------
template<typename FP, int DIM, typename B>
SC_MODULE(zfp_encode)
{
	typedef typename FP::expo_t expo_t;
	typedef typename FP::si_t si_t;

	/*-------- ports --------*/
	sc_in<bool> clk;
	sc_in<bool> reset;

	sc_stream_in <FP> s_fp;
	sc_stream_out<B>  m_enc;

	/*-------- channels --------*/
	sc_stream<FP>     c_fe_fp;
	sc_stream<expo_t> c_fe_ex;

	sc_signal<si_t> c_fc_block[fpblk_sz(DIM)];
	sc_signal<bool> c_fc_valid;
	sc_signal<bool> c_fc_ready;

	/*-------- modules --------*/
	find_emax<FP, DIM> u_find_emax;
	fwd_cast<FP, DIM> u_fwd_cast;

	// Temporary for debug
	void mc_io()
	{
		B block;
		for (int i = 0; i < fpblk_sz(DIM); i++)
			block(FP::bits*(i+1)-1, FP::bits*i) = c_fc_block[i].read();
		m_enc.data_w(block);
		m_enc.valid_w(c_fc_valid.read());
		c_fc_ready.write(m_enc.ready_r());
	}

	SC_CTOR(zfp_encode) :
		u_find_emax("u_find_emax"),
		u_fwd_cast("u_fwd_cast")
	{
		// connect u_find_emax
		u_find_emax.clk(clk);
		u_find_emax.reset(reset);
		u_find_emax.s_fp(s_fp);
		u_find_emax.m_fp(c_fe_fp);
		u_find_emax.m_ex(c_fe_ex);

		// connect u_fwd_cast
		u_fwd_cast.clk(clk);
		u_fwd_cast.reset(reset);
		u_fwd_cast.s_fp(c_fe_fp);
		u_fwd_cast.s_ex(c_fe_ex);
		for (int i = 0; i < fpblk_sz(DIM); i++)
			u_fwd_cast.m_block[i](c_fc_block[i]);
		u_fwd_cast.m_valid(c_fc_valid);
		u_fwd_cast.m_ready(c_fc_ready);

	// Temporary for debug
		SC_METHOD(mc_io);
			for (int i = 0; i < fpblk_sz(DIM); i++)
				sensitive << c_fc_block[i];
			sensitive << c_fc_valid;
			sensitive << m_enc.ready_chg();
	}

};

#endif // ZFP_H
