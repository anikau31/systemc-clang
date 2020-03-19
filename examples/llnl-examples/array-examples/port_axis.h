
#ifndef _PORT_AXIS_H
#define _PORT_AXIS_H

#include <iomanip> // hex, setw
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

// TODO: move somewhere else?
#define RLEVEL false

// control stream
#define CR 8  // number of registers
#define CA 3  // RACC address width
#define CD 32 // data width
#define CU 8  // user width
#define CI 4  // id width

// data stream
#if 1
#define DD 64 // data width
#define DU 1  // user width
#define DI 4  // id width
#else
#define DD 32 // data width
#define DU 1  // user width
#define DI 4  // id width
#endif

// AXI-Stream Interface

#if defined(__SYNTHESIS__)
#define RACC_T   RACC::ui_t
#define AXI_TC_T AXI_TC::ui_t
#define AXI_TD_T AXI_TD::ui_t
#else
#define RACC_T   RACC
#define AXI_TC_T AXI_TC
#define AXI_TD_T AXI_TD
#endif

#define RACC_CONCAT \
	(go,wr,sel,len)

class RACC { // register access command
public:
	typedef sc_uint<(1+1+2*CA)> ui_t;

	sc_uint<CA> len;
	sc_uint<CA> sel;
	sc_uint<1>  wr ;
	sc_uint<1>  go ;

	RACC(ui_t ui = 0)
	{
		RACC_CONCAT = ui;
	}

	RACC& operator=(ui_t ui)
	{
		RACC_CONCAT = ui;
		return *this;
	}

	operator ui_t() const
	{
		return RACC_CONCAT;
	}

	bool operator==(const RACC& ob)
	{
		return
			this->len == ob.len &&
			this->sel == ob.sel &&
			this->wr  == ob.wr  &&
			this->go  == ob.go;
	}
};

inline std::ostream& operator<<(std::ostream &os, const RACC& ob)
{
	return os << hex << ob.go << ':' << ob.wr << ':' << ob.sel << ':' << ob.len;
}

inline void sc_trace(sc_trace_file* tf, const RACC& ob, const std::string& nm)
{
	sc_trace(tf, ob.len, nm+".len");
	sc_trace(tf, ob.sel, nm+".sel");
	sc_trace(tf, ob.wr,  nm+".wr");
	sc_trace(tf, ob.go,  nm+".go");
}

#define AXI_TC_CONCAT \
	(tlast,tuser.go,tuser.wr,tuser.sel,tuser.len,tdest,tid,tdata)

class AXI_TC {
public:
	// typedef sc_biguint<(1+CU+2*CI+CD)> ui_t;

//	sc_uint<CD>       tdata;
	sc_bv<CD>         tdata;
	sc_uint<CI>       tid  ;
	sc_uint<CI>       tdest;
//	sc_uint<(CD+7)/8> tkeep;
//	sc_uint<(CD+7)/8> tstrb;
	RACC              tuser;
//	sc_uint<1>        tlast;
	bool              tlast;

#if 0
	AXI_TC(ui_t ui = 0)
	{
		AXI_TC_CONCAT = ui;
	}

	AXI_TC& operator=(ui_t ui)
	{
		AXI_TC_CONCAT = ui;
		return *this;
	}

	operator ui_t() const
	{
		return AXI_TC_CONCAT;
	}
#endif

	bool operator==(const AXI_TC& ob)
	{
		return
			this->tdata == ob.tdata &&
			this->tid   == ob.tid   &&
			this->tdest == ob.tdest &&
		//	this->tkeep == ob.tkeep &&
		//	this->tstrb == ob.tstrb &&
			this->tuser == ob.tuser &&
			this->tlast == ob.tlast;
	}
};

inline std::ostream& operator<<(std::ostream& os, const AXI_TC& ob)
{
	return os << hex << ob.tlast << ':' << ob.tuser << ':' << ob.tdest << ':' << ob.tid << ':' << ob.tdata;
}

inline void sc_trace(sc_trace_file* tf, const AXI_TC& ob, const std::string& nm)
{
	sc_trace(tf, ob.tdata, nm+".tdata");
	sc_trace(tf, ob.tid,   nm+".tid");
	sc_trace(tf, ob.tdest, nm+".tdest");
//	sc_trace(tf, ob.tkeep, nm+".tkeep");
//	sc_trace(tf, ob.tstrb, nm+".tstrb");
	sc_trace(tf, ob.tuser, nm+".tuser");
	sc_trace(tf, ob.tlast, nm+".tlast");
}

#define AXI_TD_CONCAT \
	(tlast,tuser,tkeep,tdest,tid,tdata)

class AXI_TD {
public:
//	typedef sc_biguint<(1+DU+((DD+7)/8)+2*DI+DD)> ui_t;

//	sc_biguint<DD>    tdata;
	sc_bv<DD>         tdata;
	sc_uint<DI>       tid  ;
	sc_uint<DI>       tdest;
	sc_uint<(DD+7)/8> tkeep;
//	sc_uint<(DD+7)/8> tstrb;
	sc_uint<DU>       tuser;
//	sc_uint<1>        tlast;
	bool              tlast;

#if 0
	AXI_TD(ui_t ui = 0)
	{
		AXI_TD_CONCAT = ui;
	}

	AXI_TD& operator=(ui_t ui)
	{
		AXI_TD_CONCAT = ui;
		return *this;
	}

	operator ui_t() const
	{
		return AXI_TD_CONCAT;
	}
#endif

	bool operator==(const AXI_TD& ob)
	{
		return
			this->tdata == ob.tdata &&
			this->tid   == ob.tid   &&
			this->tdest == ob.tdest &&
			this->tkeep == ob.tkeep &&
		//	this->tstrb == ob.tstrb &&
			this->tuser == ob.tuser &&
			this->tlast == ob.tlast;
	}
};

inline std::ostream& operator<<(std::ostream& os, const AXI_TD& ob)
{
	return os << hex << ob.tlast << ':' << ob.tuser << ':' << ob.tkeep << ':'  << ob.tdest << ':' << ob.tid << ':' << ob.tdata;
}

inline void sc_trace(sc_trace_file* tf, const AXI_TD& ob, const std::string& nm)
{
	sc_trace(tf, ob.tdata, nm+".tdata");
	sc_trace(tf, ob.tid,   nm+".tid");
	sc_trace(tf, ob.tdest, nm+".tdest");
	sc_trace(tf, ob.tkeep, nm+".tkeep");
//	sc_trace(tf, ob.tstrb, nm+".tstrb");
	sc_trace(tf, ob.tuser, nm+".tuser");
	sc_trace(tf, ob.tlast, nm+".tlast");
}

// Control Stream
typedef sc_stream    <AXI_TC_T> axi_tc;
typedef sc_stream_in <AXI_TC_T> axi_tc_in;
typedef sc_stream_out<AXI_TC_T> axi_tc_out;
typedef sfifo_cc <AXI_TC_T> axi_tc_fifo;

// Data Stream
typedef sc_stream    <AXI_TD_T> axi_td;
typedef sc_stream_in <AXI_TD_T> axi_td_in;
typedef sc_stream_out<AXI_TD_T> axi_td_out;
typedef sfifo_cc <AXI_TD_T> axi_td_fifo;

/* TODO:
 * Is ui_t and concat needed if tdata and tuser types are templated?
 * Work out how a structures bit width can be known by a translation tool.
   Can a tool determine length without a length method? The C++ sizeof()
   operator on a struct is not correct if sc_* types are included.
 * Add a bit length method to the structures/classes. May need to run-time
   compute calling length method of nested structures if no extra length
   template parameter is passed to the class. Use C++11 constexpr?
*/
/* NOTES:
 * sc_(u)int provides implicit conversion to C++ integer type
 * sc_big(u)int does not provide implicit conversion to C++ integer type
 * Integer concat provides implicit conversion to sc_unsigned
 * sc_(b|l)v supports explicit conversion to C++ integer types,
   to_int(), to_uint()...
 * Concat operations only supported within sc_(*)int types
   or sc_(b|l)v types, not between the two.
*/

#endif // _PORT_AXIS_H
