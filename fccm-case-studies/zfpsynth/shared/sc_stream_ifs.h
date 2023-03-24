/*****************************************************************************

  sc_stream_ifs.h -- The sc_stream<T> interface classes.

  Original Author: 

  CHANGE LOG IS AT THE END OF THE FILE
 *****************************************************************************/

#ifndef SC_STREAM_IFS_H
#define SC_STREAM_IFS_H

#include "sysc/communication/sc_interface.h"

namespace sc_core {

// ----------------------------------------------------------------------------
//  The sc_stream<T> input non-blocking interface class.
// ----------------------------------------------------------------------------

template <class T>
class sc_stream_nonblocking_in_if :
	virtual public sc_interface
{
public:

	// read data, non-blocking
	virtual const T& data_r() const = 0;

	// read valid, true if data is valid
	virtual const bool& valid_r() const = 0;

	// write ready, true if ready to accept data
	virtual void ready_w(const bool& r) = 0;

	// get a reference to ready value (for tracing)
	virtual const bool& ready_ref() const = 0;

	// get the data changed event
	virtual const sc_event& data_event() const = 0;

	// get the valid changed event
	virtual const sc_event& valid_event() const = 0;
};

// ----------------------------------------------------------------------------
//  The sc_stream<T> input blocking interface class.
// ----------------------------------------------------------------------------

template <class T>
class sc_stream_blocking_in_if :
	virtual public sc_interface
{
public:

	// read data, blocking
	virtual const T& read() = 0;
	virtual void read(T& d) = 0;
};

// ----------------------------------------------------------------------------
//  The sc_stream<T> input interface class.
// ----------------------------------------------------------------------------

template <class T>
class sc_stream_in_if :
	public sc_stream_nonblocking_in_if<T>,
	public sc_stream_blocking_in_if<T>
{
protected:

	// constructor
	sc_stream_in_if()
	{}

private:

	// disabled
	sc_stream_in_if(const sc_stream_in_if&);
	sc_stream_in_if& operator = (const sc_stream_in_if&);
};


// ----------------------------------------------------------------------------
//  The sc_stream<T> non-blocking output interface class.
// ----------------------------------------------------------------------------

template <class T>
class sc_stream_nonblocking_out_if :
	virtual public sc_interface
{
public:

	// write data, non-blocking
	virtual void data_w(const T& d) = 0;

	// write valid, true if data is valid
	virtual void valid_w(const bool& v) = 0;

	// read ready, true if ready to accept data
	virtual const bool& ready_r() const = 0;

	// get a reference to data value (for tracing)
	virtual const T& data_ref() const = 0;

	// get a reference to valid value (for tracing)
	virtual const bool& valid_ref() const = 0;

	// get the ready changed event
	virtual const sc_event& ready_event() const = 0;
};

// ----------------------------------------------------------------------------
//  The sc_stream<T> blocking output interface class.
// ----------------------------------------------------------------------------

template <class T>
class sc_stream_blocking_out_if :
	virtual public sc_interface
{
public:

	// write data, blocking
	virtual void write(const T& d) = 0;
};

// ----------------------------------------------------------------------------
//  The sc_stream<T> output interface class.
// ----------------------------------------------------------------------------

template <class T>
class sc_stream_out_if :
	public sc_stream_nonblocking_out_if<T>,
	public sc_stream_blocking_out_if<T>
{
protected:

	// constructor
	sc_stream_out_if()
	{}

private:

	// disabled
	sc_stream_out_if(const sc_stream_out_if&);
	sc_stream_out_if& operator = (const sc_stream_out_if&);
};

//$Log: sc_stream_ifs.h,v $
//

} // namespace sc_core

#endif
