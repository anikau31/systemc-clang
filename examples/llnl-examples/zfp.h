
#ifndef _ZFP_H
#define _ZFP_H

#include "systemc.h"

#if defined(RVD)
#include "sc_rvd.h"
#include "rvfifo_cc.h"
template <typename T> using sc_stream = sc_rvd<T>;
template <typename T> using sc_stream_in = sc_rvd_in<T>;
template <typename T> using sc_stream_out = sc_rvd_out<T>;
template <typename T> using sfifo_cc = rvfifo_cc<T>;

#else
#include "sc_stream_ports.h"
#include "sc_stream.h"
#include "sfifo_cc.h"
#endif

#include "sreg.h"

// Floating-point block size given dimension
constexpr int fpblk_sz(int dim) {return 1 << 2*dim;} // 4^dim;
// #define fpblk_sz(dim) (1 << 2*dim)

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

#define SREAD(data,in,out,outv) \
	in.ready_w(true); \
	wait(); \
	while (!in.valid_r()) {if (out.ready_r()) out.valid_w(outv = false); wait();} \
	data = in.data_r();

#define SWRITE(data,in,out,outv) \
	while (outv && !out.ready_r()) {in.ready_w(false); wait();} \
	out.valid_w(outv = true); \
	out.data_w(data);

// TODO:
// * look into using clocked thread just for states and gating data.

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
	sfifo_cc<FP> u_que_fp;
	sreg<expo_t,FWD_REV> u_reg_ex;

	void mc_proc()
	{
		s_fp.ready_w(c_sync);

		c_fp.data_w(s_fp.data_r());
		c_fp.valid_w(c_sync);

		c_ex.data_w(emax);
		c_ex.valid_w(emax_v);

		c_sync = s_fp.valid_r() && c_fp.ready_r() && (!emax_v || c_ex.ready_r());
	}

	void ms_proc()
	{
		if (reset == false) {
			count = fpblk_sz(DIM)-1;
			emax = 0;
			emax_v = false;
		} else {
			bool last = (count.read() == 0);
			FP fp = s_fp.data_r();
			if (c_sync) {
				if (last) {count = fpblk_sz(DIM)-1;}
				else {count = count.read() - 1;}
			}
			if (emax_v && c_ex.ready_r()) {
				if (s_fp.valid_r()) emax = fp.expo; else emax = 0;
			} else if (s_fp.valid_r() && fp.expo > emax) {
				emax = fp.expo;
			}
			if (emax_v && c_ex.ready_r()) emax_v = false;
			else if (c_sync && last) emax_v = true;
		}
	}

#if 1
	void start_of_simulation()
	{
		extern sc_trace_file *tf;
		sc_trace(tf, s_fp,   (std::string(name())+".s_fp").c_str());
		sc_trace(tf, m_fp,   (std::string(name())+".m_fp").c_str());
		sc_trace(tf, m_ex,   (std::string(name())+".m_ex").c_str());
		sc_trace(tf, count,  (std::string(name())+".count").c_str());
		sc_trace(tf, emax,   (std::string(name())+".emax").c_str());
		sc_trace(tf, emax_v, (std::string(name())+".emax_v").c_str());
	}
#endif

	SC_CTOR(find_emax) :
		u_que_fp("u_que_fp", fpblk_sz(DIM)),
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
			reset_signal_is(reset, false);
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

	sc_in<bool> clk;
	sc_in<bool> reset;

	/*-------- ports --------*/
	sc_stream_in<expo_t> s_ex;
	sc_stream_in <FP> s_fp;
	sc_stream_out<si_t> m_int;

	/*-------- registers --------*/
	sc_signal<sc_uint<2*DIM> > count;

	/*-------- channels --------*/
	sc_signal<bool> c_sync;
	sc_stream<si_t> c_int;

	/*-------- modules --------*/
	sreg<si_t,FWD_REV> u_reg_int;

	void mc_proc()
	{
		c_sync = s_ex.valid_r() && s_fp.valid_r() && c_int.ready_r();

		s_ex.ready_w(c_sync && (count.read() == 0));
		s_fp.ready_w(c_sync);

		expo_t emax = s_ex.data_r();
		FP fp = s_fp.data_r();
		si_t si;
		if (fp.sign) si = -si_t((sc_uint<3>(1),fp.frac,sc_uint<FP::ebits-2>(0)) >> (emax-fp.expo));
		else si = ((sc_uint<3>(1),fp.frac,sc_uint<FP::ebits-2>(0)) >> (emax-fp.expo));
		c_int.data_w(si);
		c_int.valid_w(c_sync);
	}

	void ms_proc()
	{
		if (reset == false) {
			count = fpblk_sz(DIM)-1;
		} else {
			if (c_sync) {
				if (count.read() == 0) {count = fpblk_sz(DIM)-1;}
				else {count = count.read() - 1;}
			}
		}
	}

	// void ct_io()
	// {
		// FP fp;
		// expo_t emax;
		// si_t si;
		// s_ex.reset();
		// s_fp.reset();
		// m_int.reset();
		// wait();
		// while (true) {
			// emax = s_ex.read();
			// for (int i = 0; i < fpblk_sz(DIM); i++) {
				// fp = s_fp.read();
				// if (fp.sign) si = -si_t((sc_uint<3>(1),fp.frac,sc_uint<FP::ebits-2>(0)) >> (emax-fp.expo));
				// else si = ((sc_uint<3>(1),fp.frac,sc_uint<FP::ebits-2>(0)) >> (emax-fp.expo));
				// m_int.write(si);
			// }
		// }
	// }

	// SC_CTOR(fwd_cast)
	// {
		// SC_CTHREAD(ct_io, clk.pos());
			// reset_signal_is(reset, false);
	// }

#if 1
	void start_of_simulation()
	{
		extern sc_trace_file *tf;
		sc_trace(tf, s_ex,   (std::string(name())+".s_ex").c_str());
		sc_trace(tf, s_fp,   (std::string(name())+".s_fp").c_str());
		sc_trace(tf, m_int,   (std::string(name())+".m_int").c_str());
		sc_trace(tf, count,  (std::string(name())+".count").c_str());
	}
#endif

	SC_CTOR(fwd_cast) :
		u_reg_int("u_reg_int")
	{
		// connect u_reg_int
		u_reg_int.clk(clk);
		u_reg_int.reset(reset);
		u_reg_int.s_port(c_int);
		u_reg_int.m_port(m_int);

		SC_METHOD(mc_proc);
			sensitive << s_ex.valid_chg() << s_ex.data_chg();
			sensitive << s_fp.valid_chg() << s_fp.data_chg();
			sensitive << c_int.ready_event();
			sensitive << count << c_sync;
		SC_METHOD(ms_proc);
			sensitive << clk.pos();
			reset_signal_is(reset, false);
			dont_initialize();
	}

};

//-----------------------------------------------------------------------------
// Encode IEEE floating-point values
//-----------------------------------------------------------------------------
template<typename FP, int DIM>
SC_MODULE(zfp_encode)
{
	typedef typename FP::expo_t expo_t;
	typedef typename FP::si_t si_t;

	sc_in<bool> clk;
	sc_in<bool> reset;

	sc_stream_in <FP>   s_fp;
	sc_stream_out<si_t> m_int;

	sc_stream<FP>     c_fp;
	sc_stream<expo_t> c_ex;
	sc_stream<si_t>   c_int;

	find_emax<FP, DIM>* u_find_emax;
	fwd_cast<FP, DIM> u_fwd_cast;

	// Temporary for debug
	void mc_io()
	{
		c_int.ready_w(m_int.ready_r());
		m_int.data_w(c_int.data_r());
		m_int.valid_w(c_int.valid_r());
	}

	SC_CTOR(zfp_encode) :
		//u_find_emax("u_find_emax"),
		u_fwd_cast("u_fwd_cast")
	{
		// connect u_find_emax
		(*u_find_emax).clk(clk);
		(*u_find_emax).reset(reset);
		(*u_find_emax).s_fp(s_fp);
		(*u_find_emax).m_fp(c_fp);
		(*u_find_emax).m_ex(c_ex);

		// connect u_fwd_cast
		u_fwd_cast.clk(clk);
		u_fwd_cast.reset(reset);
		u_fwd_cast.s_fp(c_fp);
		u_fwd_cast.s_ex(c_ex);
		u_fwd_cast.m_int(c_int);

	// Temporary for debug
		SC_METHOD(mc_io);
			sensitive << c_int.valid_event() << c_int.data_event();
			sensitive << m_int.ready_chg();
	}

};

#endif // _ZFP_H
