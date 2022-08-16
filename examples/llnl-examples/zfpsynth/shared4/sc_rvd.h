#ifndef SC_RVD_H
#define SC_RVD_H

#include "systemc.h"

#define data_r() data.read()
#define data_w(d) data.write(d)
#define data_event() data.value_changed_event()
#define data_chg() data //.value_changed()

#define valid_r() valid.read()
#define valid_w(d) valid.write(d)
#define valid_event() valid.value_changed_event()
#define valid_chg() valid //.value_changed()

#define ready_r() ready.read()
#define ready_w(d) ready.write(d)
#define ready_event() ready.value_changed_event()
#define ready_chg() ready //.value_changed()


#ifdef CHAN_NS
namespace CHAN_NS {
#endif

template<typename T>
class sc_rvd_export_in
{
public:

	/* * * * * constructors * * * * */

	sc_rvd_export_in(const char* name=sc_gen_unique_name("sc_rvd_export_in")) :
		data ((std::string(name)+"_data").c_str() ),
		valid((std::string(name)+"_valid").c_str()),
		ready((std::string(name)+"_ready").c_str())
	{}

public:
	sc_export<sc_signal_in_if<T> >     data;
	sc_export<sc_signal_in_if<bool> >  valid;
	sc_export<sc_signal_out_if<bool> > ready;
};

template<typename T>
class sc_rvd_export_out
{
public:

	/* * * * * constructors * * * * */

	sc_rvd_export_out(const char* name=sc_gen_unique_name("sc_rvd_export_out")) :
		data ((std::string(name)+"_data").c_str() ),
		valid((std::string(name)+"_valid").c_str()),
		ready((std::string(name)+"_ready").c_str())
	{}

public:
	sc_export<sc_signal_out_if<T> >    data;
	sc_export<sc_signal_out_if<bool> > valid;
	sc_export<sc_signal_in_if<bool> >  ready;
};

// ----------------------------------------------------------------------------

template<typename T>
class sc_rvd_in
{
public:

	/* * * * * constructors * * * * */

	sc_rvd_in(const char* name=sc_gen_unique_name("sc_rvd_in")) :
		data ((std::string(name)+"_data").c_str() ),
		valid((std::string(name)+"_valid").c_str()),
		ready((std::string(name)+"_ready").c_str())
	{}

	template<typename C>
	void bind(C& chan)
	{
		data(chan.data);
		valid(chan.valid);
		ready(chan.ready);
	}

	template<typename C>
	void operator () (C& chan)
	{ bind(chan); }

	/* * * * * interface access methods * * * * */

	// read data, blocking
	const T& read()
	{
		ready = true;
		do { wait(); } while (!valid);
		ready = false;
		return data;
	}

	void read(T& d)
	{ d = read(); }

	/* * * * * other methods * * * * */

	void reset()
	{ ready = false; }

	inline operator const T& ()
	{ return read(); }

	const char* kind() const
	{ return "sc_rvd_in"; }

public:
	sc_in <T>    data;
	sc_in <bool> valid;
	sc_out<bool> ready;

private:
	// disabled
	sc_rvd_in(const sc_rvd_in&);
	sc_rvd_in& operator = (const sc_rvd_in&);
};

template<typename T>
class sc_rvd_out
{
public:

	/* * * * * constructors * * * * */

	sc_rvd_out(const char* name=sc_gen_unique_name("sc_rvd_out")) :
		data ((std::string(name)+"_data").c_str() ),
		valid((std::string(name)+"_valid").c_str()),
		ready((std::string(name)+"_ready").c_str())
	{}

	template<typename C>
	void bind(C& chan)
	{
		data(chan.data);
		valid(chan.valid);
		ready(chan.ready);
	}

	template<typename C>
	void operator () (C& chan)
	{ bind(chan); }

	/* * * * * interface access methods * * * * */

	// write data, blocking
	void write(const T& d)
	{
		valid = true;
		data = d;
		do { wait(); } while (!ready);
		valid = false;
	}

	/* * * * * other methods * * * * */

	void reset()
	{ valid = false; }

	sc_rvd_out& operator = (const T& d)
	{ write(d); return *this; }

	const char* kind() const
	{ return "sc_rvd_out"; }

public:
	sc_out<T>    data;
	sc_out<bool> valid;
	sc_in <bool> ready;

private:
	// disabled
	sc_rvd_out(const sc_rvd_out&);
	sc_rvd_out& operator = (const sc_rvd_out&);
};

// ----------------------------------------------------------------------------

template<typename T>
class sc_rvd
{
public:

	/* * * * * constructors * * * * */

	sc_rvd(const char* name=sc_gen_unique_name("sc_rvd")) :
		data ((std::string(name)+"_data").c_str() ),
		valid((std::string(name)+"_valid").c_str()),
		ready((std::string(name)+"_ready").c_str())
	{}

	/* * * * * interface access methods * * * * */

	// read data, blocking
	const T& read()
	{
		ready = true;
		do { wait(); } while (!valid);
		ready = false;
		return data;
	}

	void read(T& d)
	{ d = read(); }

	// write data, blocking
	void write(const T& d)
	{
		valid = true;
		data = d;
		do { wait(); } while (!ready);
		valid = false;
	}

	/* * * * * other methods * * * * */

	inline operator const T& ()
	{ return read(); }

	sc_rvd& operator = (const T& d)
	{ write(d); return *this; }

	void print(std::ostream& os=std::cout) const
	{
		data.print(os);
		valid.print(os);
		ready.print(os);
	}

	void dump(std::ostream& os=std::cout) const
	{
		data.dump(os);
		valid.dump(os);
		ready.dump(os);
	}

	virtual const char* kind() const
	{ return "sc_rvd"; }

	template <typename U>
	friend void
	sc_trace(sc_trace_file* tf, const sc_rvd<U>& ob, const std::string& nm);

public:
	sc_signal<T>    data;
	sc_signal<bool> valid;
	sc_signal<bool> ready;
};

// ----------------------------------------------------------------------------

template <typename T>
inline std::ostream&
operator<<(std::ostream& os, const sc_rvd<T>& ob)
{
	ob.print(os);
	return os;
}

template <typename T>
inline void
sc_trace(sc_trace_file* tf, const sc_rvd_in<T>& ob, const std::string& nm)
{
	sc_trace(tf, ob.data , nm+".data" );
	sc_trace(tf, ob.valid, nm+".valid");
	sc_trace(tf, ob.ready, nm+".ready");
}

template <typename T>
inline void
sc_trace(sc_trace_file* tf, const sc_rvd_out<T>& ob, const std::string& nm)
{
	sc_trace(tf, ob.data , nm+".data" );
	sc_trace(tf, ob.valid, nm+".valid");
	sc_trace(tf, ob.ready, nm+".ready");
}

template <typename T>
inline void
sc_trace(sc_trace_file* tf, const sc_rvd<T>& ob, const std::string& nm)
{
	sc_trace(tf, ob.data , nm+".data" );
	sc_trace(tf, ob.valid, nm+".valid");
	sc_trace(tf, ob.ready, nm+".ready");
}

#ifdef CHAN_NS
} // namespace CHAN_NS
#endif

#endif // SC_RVD_H
