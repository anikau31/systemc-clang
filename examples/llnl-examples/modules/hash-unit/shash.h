
#ifndef SHASH_H
#define SHASH_H

#include "systemc.h"

#define KEYLW 8
#define KEYW 128
#define TAPW 256
#define NUMW (TAPW/4)
#define STAGES 12

typedef sc_biguint<NUMW> num_t;


SC_MODULE(short_hash) // Hash Function
{
	/*-------- ports --------*/
	sc_in<bool> clk;
	sc_in<bool> reset;

	sc_stream_in <sc_biguint<KEYLW+KEYW> > s_dat;
	sc_stream_in <sc_biguint<TAPW> > s_tap;
	sc_stream_out<sc_biguint<TAPW> > m_tap;

	/*-------- registers --------*/
	sc_signal<num_t> a[STAGES];
	sc_signal<num_t> b[STAGES];
	sc_signal<num_t> c[STAGES];
	sc_signal<num_t> d[STAGES];
	sc_signal<sc_bv<STAGES> > v; // valid bit for each stage

	inline void emix(
		sc_signal<num_t> (&y)[STAGES],
		sc_signal<num_t> (&x)[STAGES],
		sc_signal<num_t> (&u)[STAGES],
		sc_signal<num_t> (&t)[STAGES],
		const unsigned ro, // rotate
		const unsigned ns) // next stage
	{
		num_t xt = x[ns-1].read();
		num_t xr = sc_bv<NUMW>(xt).lrotate(ro);
		t[ns].write(t[ns-1].read());
		u[ns].write(u[ns-1].read());
		x[ns].write(xr);
		y[ns].write((y[ns-1].read() ^ xt) + xr);
	}

	void mc_mix()
	{
		s_dat.ready_w(m_tap.ready_r() && s_tap.valid_r());
		s_tap.ready_w(m_tap.ready_r() && s_dat.valid_r());
		m_tap.data_w((d[11],c[11],b[11],a[11]));
		m_tap.valid_w(v.read()[11].to_bool());
	}

	void ms_mix()
	{
		num_t len;
		num_t data[2];
		num_t tapi[4];

		if (reset.read() == RLEVEL) {
			for (int i = 0; i < STAGES; i++) {
				a[i].write(0); b[i].write(0); c[i].write(0); d[i].write(0);
			}
			v.write(0);
		} else if (m_tap.ready_r()) {
			(len(KEYLW-1,0),data[1],data[0]) = s_dat.data_r();
			(tapi[3],tapi[2],tapi[1],tapi[0]) = s_tap.data_r();

			a[0].write(tapi[0] ^  len);
			b[0].write(tapi[1] ^ ~len);
			c[0].write(tapi[2] + data[0]);
			d[0].write(tapi[3] + data[1]);

			emix(d,c,b,a,15, 1);
			emix(a,d,c,b,52, 2);
			emix(b,a,d,c,26, 3);

			emix(c,b,a,d,51, 4);
			emix(d,c,b,a,28, 5);
			emix(a,d,c,b, 9, 6);
			emix(b,a,d,c,47, 7);

			emix(c,b,a,d,54, 8);
			emix(d,c,b,a,32, 9);
			emix(a,d,c,b,25,10);
			emix(b,a,d,c,63,11);

			v.write((v.read() << 1) | (s_dat.valid_r() && s_tap.valid_r()));
		}
	}

#if defined(VCD)
	void start_of_simulation()
	{
		extern sc_trace_file *tf;
		sc_trace(tf, a[0], (std::string(name())+".a0").c_str());
		sc_trace(tf, b[0], (std::string(name())+".b0").c_str());
		sc_trace(tf, c[0], (std::string(name())+".c0").c_str());
		sc_trace(tf, d[0], (std::string(name())+".d0").c_str());
		sc_trace(tf, a[11], (std::string(name())+".a11").c_str());
		sc_trace(tf, b[11], (std::string(name())+".b11").c_str());
		sc_trace(tf, c[11], (std::string(name())+".c11").c_str());
		sc_trace(tf, d[11], (std::string(name())+".d11").c_str());
		sc_trace(tf, v, (std::string(name())+".v").c_str());
	}
#endif // VCD

	SC_CTOR(short_hash)
	{
		SC_METHOD(mc_mix);
			sensitive << s_dat.valid_chg();
			sensitive << s_tap.valid_chg();
			sensitive << m_tap.ready_chg();
			sensitive << a[11] << b[11] << c[11] << d[11] << v;
		SC_METHOD(ms_mix)
			sensitive << clk.pos();
			dont_initialize();
	}

};

#endif // SHASH_H
