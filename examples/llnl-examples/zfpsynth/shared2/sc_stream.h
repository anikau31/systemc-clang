/*****************************************************************************

  sc_stream.h -- The sc_stream<T> hierarchical channel class.

  Original Author: 

  CHANGE LOG IS AT THE END OF THE FILE
 *****************************************************************************/

#ifndef SC_STREAM_H
#define SC_STREAM_H

#include <typeinfo>

#include "sysc/kernel/sc_event.h"
#include "sysc/tracing/sc_trace.h"
#include "sc_stream_ifs.h"


#ifdef CHAN_NS
namespace CHAN_NS {
#endif

template <typename T>
class sc_stream :
	public sc_stream_in_if<T>,
	public sc_stream_out_if<T>,
	public sc_channel
{
public:

	/* * * * * constructors * * * * */

	sc_stream(const sc_module_name& mn_=sc_gen_unique_name("sc_stream")) :
		sc_channel(mn_),
		data("data"),
		valid("valid"),
		ready("ready")
	{
		reader = NULL;
		writer = NULL;
	}

	/* * * * * destructor * * * * */

	virtual ~sc_stream()
	{ }

	/* * * * * interface access methods * * * * */

	virtual void register_port(sc_port_base&, const char*);

	// read data, blocking
	virtual const T& read();

	virtual void read(T& d)
	{ d = read(); }

	// read data, non-blocking
	virtual const T& data_r() const
	{ return data; }

	// read valid, true if data is valid
	virtual const bool& valid_r() const
	{ return valid; }

	// write ready, true if ready to accept data
	virtual void ready_w(const bool& r)
	{ ready = r; }

	// get a reference to ready value (for tracing)
	virtual const bool& ready_ref() const
	{ return ready; }

	// get the data changed event
	virtual const sc_event& data_event() const
	{ return data.value_changed_event(); }

	// get the valid changed event
	virtual const sc_event& valid_event() const
	{ return valid.value_changed_event(); }


	// write data, blocking
	virtual void write(const T& d);

	// write data, non-blocking
	virtual void data_w(const T& d)
	{ data = d; }

	// write valid, true if data is valid
	virtual void valid_w(const bool& v)
	{ valid = v; }

	// read ready, true if ready to accept data
	virtual const bool& ready_r() const
	{ return ready; }

	// get a reference to data value (for tracing)
	virtual const T& data_ref() const
	{ return data; }

	// get a reference to valid value (for tracing)
	virtual const bool& valid_ref() const
	{ return valid; }

	// get the ready changed event
	virtual const sc_event& ready_event() const
	{ return ready.value_changed_event(); }

	/* * * * * other methods * * * * */

	inline operator const T& ()
	{ return read(); }

	sc_stream& operator = (const T& d)
	{ write(d); return *this; }

	virtual void print(std::ostream& os = std::cout) const;
	virtual void dump(std::ostream& os = std::cout) const;

	virtual const char* kind() const
	{ return "sc_stream"; }

	template <typename U>
	friend void
	sc_trace(sc_trace_file* tf, const sc_stream<U>& ob, const std::string& nm);

protected:

	sc_port_base* reader; // used for static design rule checking
	sc_port_base* writer; // used for static design rule checking

	sc_signal<T>    data;
	sc_signal<bool> valid;
	sc_signal<bool> ready;

private:

	// disabled
	sc_stream(const sc_stream&);
	sc_stream& operator = (const sc_stream&);
};

/* * * * * interface access methods * * * * */

template <typename T>
inline
void
sc_stream<T>::register_port(sc_port_base& port_, const char* if_typename_)
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
						 "sc_stream<T> port not recognized");
		// may continue, if suppressed
	}
}

// read data, blocking
template <typename T>
inline const T&
sc_stream<T>::read()
{
	ready = true;
	// calling process must have static sensitivity to clock edge
	do { wait(); } while (!valid);
	ready = false;
	return data;
}

// write data, blocking
template <typename T>
inline void
sc_stream<T>::write(const T& d)
{
	valid = true;
	data = d;
	// calling process must have static sensitivity to clock edge
	do { wait(); } while (!ready);
	valid = false;
}

/* * * * * other methods * * * * */

template <typename T>
inline void
sc_stream<T>::print(std::ostream& os) const
{
	data.print(os);
	valid.print(os);
	ready.print(os);
}

template <typename T>
inline void
sc_stream<T>::dump(std::ostream& os) const
{
	os << "name = " << name() << std::endl;
	data.dump(os);
	valid.dump(os);
	ready.dump(os);
}

// ----------------------------------------------------------------------------

template <typename T>
inline std::ostream&
operator<<(std::ostream& os, const sc_stream<T>& ob)
{
	ob.print(os);
	return os;
}

template <typename T>
inline void
sc_trace(sc_trace_file* tf, const sc_stream<T>& ob, const std::string& nm)
{
	sc_trace(tf, ob.data , nm+".data" );
	sc_trace(tf, ob.valid, nm+".valid");
	sc_trace(tf, ob.ready, nm+".ready");
}

#ifdef CHAN_NS
} // namespace CHAN_NS
#endif

#endif // SC_STREAM_H

//$Log: sc_stream.h,v $
//
