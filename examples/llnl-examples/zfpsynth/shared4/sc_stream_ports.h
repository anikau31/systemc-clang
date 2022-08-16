/*****************************************************************************

  sc_stream_ports.h -- The sc_stream<T> port classes.

  Original Author: 

  CHANGE LOG IS AT THE END OF THE FILE
 *****************************************************************************/

#ifndef SC_STREAM_PORTS_H
#define SC_STREAM_PORTS_H

#include "sysc/communication/sc_port.h"
// #include "sysc/communication/sc_stream_ifs.h"
#include "sc_stream_ifs.h"
#include "sysc/tracing/sc_trace.h"


#ifdef CHAN_NS
namespace CHAN_NS {
#endif

// ----------------------------------------------------------------------------
//  The sc_stream<T> input port class.
// ----------------------------------------------------------------------------

template <class T>
class sc_stream_in :
	public sc_port<sc_stream_in_if<T>,0,SC_ONE_OR_MORE_BOUND>
{
public:

	/* * * * * typedefs * * * * */

	typedef T                                       data_type;

	typedef sc_stream_in_if<data_type>              if_type;
	typedef sc_port<if_type,0,SC_ONE_OR_MORE_BOUND> base_type;
	typedef sc_stream_in<data_type>                 this_type;

	typedef if_type                                 in_if_type;
	typedef sc_port_b<in_if_type>                   in_port_type;

public:

	/* * * * * constructors * * * * */

	sc_stream_in()
	: base_type()
	{}

	explicit sc_stream_in(const char* name_)
	: base_type(name_)
	{}

	explicit sc_stream_in(in_if_type& interface_)
	: base_type(interface_)
	{}

	sc_stream_in(const char* name_, in_if_type& interface_)
	: base_type(name_, interface_)
	{}

	explicit sc_stream_in(in_port_type& parent_)
	: base_type(parent_)
	{}

	sc_stream_in(const char* name_, in_port_type& parent_)
	: base_type(name_, parent_)
	{}

	sc_stream_in(this_type& parent_)
	: base_type(parent_)
	{}

	sc_stream_in(const char* name_, this_type& parent_)
	: base_type(name_, parent_)
	{}

	/* * * * * destructor * * * * */

	virtual ~sc_stream_in()
	{}

	/* * * * * interface access methods * * * * */

	// read data, blocking
	const data_type& read()
	{ return (*this)->read(); }

	void read(data_type& value_)
	{ (*this)->read(value_); }

	// read data, non-blocking
	const data_type& data_r()
	{ return (*this)->data_r(); }

	// read valid, true if data is valid
	const bool& valid_r() const
	{ return (*this)->valid_r(); }

	// write ready, true if ready to accept data
	void ready_w(const bool& value_)
	{ (*this)->ready_w(value_); }

	// get a reference to ready value (for tracing)
	const bool& ready_ref() const
	{ return (*this)->ready_ref(); }

	// get the data changed event
	const sc_event& data_event() const
	{ return (*this)->data_event(); }

	// get the valid changed event
	const sc_event& valid_event() const
	{ return (*this)->valid_event(); }

	/* * * * * other methods * * * * */

	// reset the interface value
	void reset()
	{ (*this)->ready_w(false); }

	// use for static sensitivity to data changed event
	sc_event_finder& data_chg() const
	{
		return *new sc_event_finder_t<in_if_type>(
			*this, &in_if_type::data_event);
	}

	// use for static sensitivity to valid changed event
	sc_event_finder& valid_chg() const
	{
		return *new sc_event_finder_t<in_if_type>(
			*this, &in_if_type::valid_event);
	}

	virtual const char* kind() const
	{ return "sc_stream_in"; }

private:

	// disabled
	sc_stream_in(const this_type&);
	this_type& operator = (const this_type&);
};

// ----------------------------------------------------------------------------
//  The sc_stream<T> output port class.
// ----------------------------------------------------------------------------

template <class T>
class sc_stream_out :
	public sc_port<sc_stream_out_if<T>,0,SC_ONE_OR_MORE_BOUND>
{
public:

	/* * * * * typedefs * * * * */

	typedef T                                        data_type;

	typedef sc_stream_out_if<data_type>              if_type;
	typedef sc_port<if_type,0,SC_ONE_OR_MORE_BOUND>  base_type;
	typedef sc_stream_out<data_type>                 this_type;

	typedef if_type                                  out_if_type;
	typedef sc_port_b<out_if_type>                   out_port_type;

public:

	/* * * * * constructors * * * * */

	sc_stream_out()
	: base_type()
	{}

	explicit sc_stream_out(const char* name_)
	: base_type(name_)
	{}

	explicit sc_stream_out(out_if_type& interface_)
	: base_type(interface_)
	{}

	sc_stream_out(const char* name_, out_if_type& interface_)
	: base_type(name_, interface_)
	{}

	explicit sc_stream_out(out_port_type& parent_)
	: base_type(parent_)
	{}

	sc_stream_out(const char* name_, out_port_type& parent_)
	: base_type(name_, parent_)
	{}

	sc_stream_out(this_type& parent_)
	: base_type(parent_)
	{}

	sc_stream_out(const char* name_, this_type& parent_)
	: base_type(name_, parent_)
	{}

	/* * * * * destructor * * * * */

	virtual ~sc_stream_out()
	{}

	/* * * * * interface access methods * * * * */

	// write data, blocking
	void write(const data_type& value_)
	{ (*this)->write(value_); }

	// write data, non-blocking
	void data_w(const data_type& value_)
	{ (*this)->data_w(value_); }

	// write valid, true if data is valid
	void valid_w(const bool& value_)
	{ (*this)->valid_w(value_); }

	// read ready, true if ready to accept data
	const bool& ready_r() const
	{ return (*this)->ready_r(); }

	// get a reference to data value (for tracing)
	const data_type& data_ref() const
	{ return (*this)->data_ref(); }

	// get a reference to valid value (for tracing)
	const bool& valid_ref() const
	{ return (*this)->valid_ref(); }

	// get the ready changed event
	const sc_event& ready_event() const
	{ return (*this)->ready_event(); }

	/* * * * * other methods * * * * */

	// reset the interface value
	void reset()
	{ (*this)->data_w(data_type()); (*this)->valid_w(false); }

	// use for static sensitivity to ready changed event
	sc_event_finder& ready_chg() const
	{
		return *new sc_event_finder_t<out_if_type>(
			*this, &out_if_type::ready_event);
	}

	virtual const char* kind() const
	{ return "sc_stream_out"; }

private:

	// disabled
	sc_stream_out(const this_type&);
	this_type& operator = (const this_type&);
};

// ----------------------------------------------------------------------------

template <typename T>
inline void
sc_trace(sc_trace_file* tf, const sc_stream_in_if<T>& ob, const std::string& nm)
{
	sc_trace(tf, ob.data_r(),    nm+".data" );
	sc_trace(tf, ob.valid_r(),   nm+".valid");
	sc_trace(tf, ob.ready_ref(), nm+".ready");
}

template <typename T>
inline void
sc_trace(sc_trace_file* tf, const sc_stream_out_if<T>& ob, const std::string& nm)
{
	sc_trace(tf, ob.data_ref(),  nm+".data" );
	sc_trace(tf, ob.valid_ref(), nm+".valid");
	sc_trace(tf, ob.ready_r(),   nm+".ready");
}

template <class T>
inline
void
sc_trace(sc_trace_file* tf, const sc_stream_in<T>& ob, const std::string& nm)
{
	const sc_stream_in_if<T>* iface = 0;
	if (sc_get_curr_simcontext()->elaboration_done()) {
		iface = dynamic_cast<const sc_stream_in_if<T>*>(ob.get_interface());
	}

	if (iface) sc_trace(tf, *iface, nm);
	// else ob.add_trace_internal(tf, nm); // TODO: see sc_signal_ports.(h,cpp)
}

template <class T>
inline
void
sc_trace(sc_trace_file* tf, const sc_stream_out<T>& ob, const std::string& nm)
{
	const sc_stream_out_if<T>* iface = 0;
	if (sc_get_curr_simcontext()->elaboration_done()) {
		iface = dynamic_cast<const sc_stream_out_if<T>*>(ob.get_interface());
	}

	if (iface) sc_trace(tf, *iface, nm);
	// else ob.add_trace_internal(tf, nm); // TODO: see sc_signal_ports.(h,cpp)
}

#ifdef CHAN_NS
} // namespace CHAN_NS
#endif

#endif // SC_STREAM_PORTS_H

//$Log: sc_stream_ports.h,v $
//
