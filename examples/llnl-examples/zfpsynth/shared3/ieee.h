
#ifndef IEEE_H
#define IEEE_H

#include "systemc.h"

//-----------------------------------------------------------------------------
// IEEE floating-point type
//-----------------------------------------------------------------------------
template<int E, int F>
struct fp_t {
	static constexpr int ebits = E;    // exponent bits
	static constexpr int fbits = F;    // fraction bits
	static constexpr int bits = 1+E+F; // total bits

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

	fp_t(ui_t ui)
	{
		(sign,expo,frac) = ui;
	}

	fp_t(sc_dt::uint64 ui = 0)
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

	//---------- conditional int types ----------//

	typedef typename std::conditional<bits==32, int, long long>::type sic_t;
	typedef typename std::conditional<bits==32, unsigned int, unsigned long long>::type uic_t;

#if !defined(__SYNTHESIS__)

	//---------- real_t ----------//

	typedef typename std::conditional<bits==32, float, double>::type real_t;

	explicit fp_t(real_t r)
	{
		sc_dt::uint64 ui = 0;
		std::memcpy(&ui, &r, sizeof(r));
		(sign,expo,frac) = ui;
	}

	fp_t& operator=(real_t r)
	{
		sc_dt::uint64 ui = 0;
		std::memcpy(&ui, &r, sizeof(r));
		(sign,expo,frac) = ui;
		return *this;
	}

	real_t to_real() const
	{
		sc_dt::uint64 ui = (sign,expo,frac);
		real_t r;
		std::memcpy(&r, &ui, sizeof(r));
		return r;
	}
#endif // end !__SYNTHESIS__
};

template<int E, int F>
inline std::ostream& operator<<(std::ostream& os, const fp_t<E,F>& fp)
{
	os << hex << fp.sign << ':'  << fp.expo << ':' << fp.frac;
#if !defined(__SYNTHESIS__)
	os << ", " << fp.to_real();
#endif // end !__SYNTHESIS__
	return os;
}

template<int E, int F>
void sc_trace(sc_trace_file* tf, const fp_t<E,F>& ob, const std::string& nm)
{
	sc_trace(tf, ob.frac, nm+".frac");
	sc_trace(tf, ob.expo, nm+".expo");
	sc_trace(tf, ob.sign, nm+".sign");
}

#endif // IEEE_H
