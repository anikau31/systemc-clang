
#ifndef BITS_H
#define BITS_H

#include "systemc.h"

//-----------------------------------------------------------------------------
// bit stream type
//-----------------------------------------------------------------------------
template<int D>
struct bits_t {
	static constexpr int dbits = D;
	static constexpr int bits = 1+D;

	// TODO: handle 8, 16, 32, 64, 128?
	// TODO: better way of mapping D to standard types, or vise versa?
	typedef typename std::conditional<D==32, unsigned int, unsigned long long>::type uic_t;

#if 1
	sc_bv<D>   tdata;
//	sc_uint<U> tuser; // consider using as count of bits in tdata
	bool       tlast;

#else
	typedef sc_biguint<(bits)> ui_t;
	sc_uint<D> tdata;
//	sc_uint<U> tuser; // consider using as count of bits in tdata
	sc_uint<1> tlast;

	bits_t(ui_t ui = 0)
	{
		(tlast,tdata) = ui;
	}

	bits_t& operator=(ui_t ui)
	{
		(tlast,tdata) = ui;
		return *this;
	}

	operator ui_t() const
	{
		return (tlast,tdata);
	}
#endif

	bool operator==(const bits_t& ob)
	{
		return
			this->tdata == ob.tdata &&
			this->tlast == ob.tlast;
	}
};

template<int D>
inline std::ostream& operator<<(std::ostream& os, const bits_t<D>& ob)
{
	return os << hex << ob.tlast << ':' << ob.tdata;
}

template<int D>
inline void sc_trace(sc_trace_file* tf, const bits_t<D>& ob, const std::string& nm)
{
	sc_trace(tf, ob.tdata, nm+".tdata");
	sc_trace(tf, ob.tlast, nm+".tlast");
}

#endif // BITS_H
