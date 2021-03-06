/*****************************************************************************

  sc_sfifo_cc.h -- The sc_sfifo_cc<T> hierarchical channel class.

  Original Author: 

  CHANGE LOG IS AT THE END OF THE FILE
 *****************************************************************************/

#ifndef SC_SFIFO_CC_H
#define SC_SFIFO_CC_H

// Port & Channel Prefix
// m: port master
// s: port slave
// cr: channel read
// cw: channel write
// u: module instance (unit)

#include <typeinfo>

#include "sysc/kernel/sc_event.h"
#include "sysc/tracing/sc_trace.h"
#include "sc_stream_ifs.h"
#include "fifo_cc.h"


namespace sc_core {

// TODO: add depth parameter? test using constexpr log2rp()
template <typename T>
class sc_sfifo_cc :
	public sc_stream_in_if<T>,
	public sc_stream_out_if<T>,
	public sc_channel
{
	typedef fifo_cc<T,16,true,false,false> fifo_t;

public:
	sc_in<bool> aclk;
	sc_in<bool> aresetn;

	/* * * * * constructors * * * * */

	sc_sfifo_cc(const sc_module_name& mn_, int size_ = 2) :
		sc_channel(mn_),
		cw_data(NULL),
		cw_ready(NULL),
		cw_valid(NULL),
		cr_data("cr_data"),
		cr_ready("cr_ready"),
		cr_valid("cr_valid"),
		u_fifo(NULL)
	{
		reader = NULL;
		writer = NULL;

		if (size_ > 0) {
			cw_data  = new sc_signal<T>("cw_data");
			cw_ready = new sc_signal<bool>("cw_ready");
			cw_valid = new sc_signal<bool>("cw_valid");
			u_fifo = new fifo_t("u_fifo", size_);
			u_fifo->clk(aclk);
			u_fifo->srst(aresetn);
			u_fifo->din(*cw_data);
			u_fifo->wr_en(*cw_valid);
			u_fifo->full(*cw_ready);
			u_fifo->dout(cr_data);
			u_fifo->rd_en(cr_ready);
			u_fifo->empty(cr_valid);
		} else {
			cw_data  = &cr_data;
			cw_ready = &cr_ready;
			cw_valid = &cr_valid;
		}
	}

	/* * * * * destructor * * * * */

	virtual ~sc_sfifo_cc()
	{
		if (u_fifo != NULL) {
			delete u_fifo;
			delete cw_valid;
			delete cw_ready;
			delete cw_data;
		}
	}

	/* * * * * interface access methods * * * * */

	virtual void register_port(sc_port_base&, const char*);

	// read data, blocking
	virtual const T& read();

	virtual void read(T& d)
	{ d = read(); }

	// read data, non-blocking
	virtual const T& data_r() const
	{ return cr_data; }

	// read valid state, true if data is valid
	virtual const bool& valid_r() const
	{ return cr_valid; }

	// write ready state, true if ready to accept data
	virtual void ready_w(const bool& r)
	{ cr_ready = r; }

	// get the data changed event
	virtual const sc_event& data_event() const
	{ return cr_data.value_changed_event(); }

	// get the valid changed event
	virtual const sc_event& valid_event() const
	{ return cr_valid.value_changed_event(); }


	// write data, blocking
	virtual void write(const T& d);

	// write data, non-blocking
	virtual void data_w(const T& d)
	{ *cw_data = d; }

	// read ready state, true if ready to accept data
	virtual const bool& ready_r() const
	{ return *cw_ready; }

	// write valid state, true if data is valid
	virtual void valid_w(const bool& v)
	{ *cw_valid = v; }

	// get the ready changed event
	virtual const sc_event& ready_event() const
	{ return (*cw_ready).value_changed_event(); }

	/* * * * * other methods * * * * */

	inline operator const T& ()
	{ return read(); }

	sc_sfifo_cc& operator = (const T& d)
	{ write(d); return *this; }

	virtual void print(std::ostream& = std::cout) const;
	virtual void dump(std::ostream& = std::cout) const;

	virtual const char* kind() const
	{ return "sc_sfifo_cc"; }

	template <typename U>
	friend void
	sc_trace(sc_trace_file* tf, const sc_sfifo_cc<U>& ob, const std::string& nm);

protected:

	sc_port_base* reader; // used for static design rule checking
	sc_port_base* writer; // used for static design rule checking

	sc_signal<T>    *cw_data;
	sc_signal<bool> *cw_ready;
	sc_signal<bool> *cw_valid;

	sc_signal<T>    cr_data;
	sc_signal<bool> cr_ready;
	sc_signal<bool> cr_valid;

	fifo_t *u_fifo;

private:

	// disabled
	sc_sfifo_cc(const sc_sfifo_cc&);
	sc_sfifo_cc& operator = (const sc_sfifo_cc&);
};

/* * * * * interface access methods * * * * */

template <typename T>
inline
void
sc_sfifo_cc<T>::register_port(sc_port_base& port_, const char* if_typename_)
{
	std::string nm(if_typename_);
	if (nm == typeid(sc_stream_in_if<T>).name() ||
		nm == typeid(sc_stream_blocking_in_if<T>).name())
	{
		// only one reader can be connected
		if (reader != NULL) {
			SC_REPORT_ERROR(SC_ID_MORE_THAN_ONE_FIFO_READER_, 0);
			// may continue, if suppressed
		}
		reader = &port_;
	} else if (nm == typeid(sc_stream_out_if<T>).name() ||
			   nm == typeid(sc_stream_blocking_out_if<T>).name())
	{
		// only one writer can be connected
		if (writer != NULL) {
			SC_REPORT_ERROR(SC_ID_MORE_THAN_ONE_FIFO_WRITER_, 0);
			// may continue, if suppressed
		}
		writer = &port_;
	} else
	{
		SC_REPORT_ERROR(SC_ID_BIND_IF_TO_PORT_,
						 "sc_sfifo_cc<T> port not recognized");
		// may continue, if suppressed
	}
}

// read data, blocking
template <typename T>
inline const T&
sc_sfifo_cc<T>::read()
{
	cr_ready = true;
	// calling process must have static sensitivity to clock edge
	do {
		wait();
	} while (!cr_valid);
	cr_ready = false;
	return cr_data;
}

// write data, blocking
template <typename T>
inline void
sc_sfifo_cc<T>::write(const T& d)
{
	*cw_valid = true;
	*cw_data = d;
	// calling process must have static sensitivity to clock edge
	do {
		wait();
	} while (!*cw_ready);
	*cw_valid = false;
}

/* * * * * other methods * * * * */

template <typename T>
inline void
sc_sfifo_cc<T>::print(std::ostream& os) const
{
	if (u_fifo != NULL && !u_fifo->empty_i) {
		unsigned i = u_fifo->rd_idx.read();
		do {
			os << u_fifo->data[i] << ::std::endl;
			i = (i + 1) % u_fifo->depth;
		} while(i != u_fifo->wr_idx.read());
	}
}

template <typename T>
inline void
sc_sfifo_cc<T>::dump(std::ostream& os) const
{
	os << "name = " << name() << ::std::endl;
	if (u_fifo != NULL && !u_fifo->empty_i) {
		unsigned i = u_fifo->rd_idx.read();
		do {
			os << "value[" << i << "] = " << u_fifo->data[i] << ::std::endl;
			i = (i + 1) % u_fifo->depth;
		} while(i != u_fifo->wr_idx.read());
	}
}

// ----------------------------------------------------------------------------

template <typename T>
inline std::ostream&
operator<<(std::ostream& os, const sc_sfifo_cc<T>& ob)
{
	ob.print(os);
	return os;
}

template <typename T>
inline void
sc_trace(sc_trace_file* tf, const sc_sfifo_cc<T>& ob, const std::string& nm)
{
	sc_trace(tf, *ob.cw_data , nm+".in.data" );
	sc_trace(tf, *ob.cw_valid, nm+".in.valid");
	sc_trace(tf, *ob.cw_ready, nm+".in.ready");
	sc_trace(tf,  ob.cr_data , nm+".out.data" );
	sc_trace(tf,  ob.cr_valid, nm+".out.valid");
	sc_trace(tf,  ob.cr_ready, nm+".out.ready");
}

} // namespace sc_core

//$Log: sc_sfifo_cc.h,v $
//

#endif
