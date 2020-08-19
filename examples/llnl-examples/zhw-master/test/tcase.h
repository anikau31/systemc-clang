#ifndef TCASE_H
#define TCASE_H

#include <algorithm> // min, max
#include "zhw.h"
#include "zbatch.h"

#define CEIL(n,s) ((((n)+((s)-1)) / (s)) * (s))


//-----------------------------------------------------------------------------
// CPU cycle counter
//-----------------------------------------------------------------------------
#if defined(_X86_) || defined(__x86_64__)
typedef union {
	unsigned long long ui64;
	struct {unsigned int lo, hi;} ui32;
} tick_t;
#define tget(t) __asm__ __volatile__ ("lfence\n\trdtsc" : "=a" ((t).ui32.lo), "=d"((t).ui32.hi))
#define tval(t) ((t).ui64)

#else
#define tget(t)
#define tval(t) 0
#endif

//-----------------------------------------------------------------------------
// Test case base
//-----------------------------------------------------------------------------
#include <cstdint> // uint64_t, UINT64_MAX
#include <cstring> // memset
#include <ios> // hex, uppercase
#include <iomanip> // setfill, setw

template<typename R, typename U, int DIM>
class tcase {
public:
	static constexpr int block_len = zhw::fpblk_sz(DIM);

	// par: ZFP parameters
	// blocks: number of ZFP blocks of type R to process
	tcase(const zpar &_par, unsigned _blocks)
	{
		R1 = new R[block_len*_blocks];
		R2 = new R[block_len*_blocks];
		// assume bits already multiple of sizeof(U)
		EN = new U[_par.maxbits/(sizeof(U)*8)*_blocks];
		par = _par;
		blocks = _blocks;
	}

	virtual ~tcase()
	{
		delete[] R1;
		delete[] R2;
		delete[] EN;
	}

	// p is pointer to data, sz is size in bytes, ww is word width in bytes
	void dump(const void *p, unsigned sz, unsigned ww)
	{
		const unsigned char *cp = reinterpret_cast<const unsigned char *>(p);
		// const R *fp = reinterpret_cast<const R *>(p);
		auto f = cout.flags(); // save stream format state
		cout << " -- index:data" << endl;
		cout << std::hex << std::uppercase;
		for (unsigned i = 0; i < sz; i += ww) {
			cout << " " << std::setfill('0') << std::setw((log2rz(sz-1)+1+3)/4) << i << ":" ;
			for (unsigned n = std::min(sz-i, ww); n > 0; n--) {
				cout << std::setfill('0') << std::setw(2) << int{cp[i+n-1]};
			}
			// cout << ':' << fp[i/ww];
			cout << endl;
		}
		cout.flags(f); // restore stream format state
	}

	// p1 is reference, ok if s2 < s1, error if s2 > s1 and data not zero
	bool compare(const void *p1, unsigned s1, const void *p2, unsigned s2)
	{
		const unsigned char *cp1 = reinterpret_cast<const unsigned char *>(p1);
		const unsigned char *cp2 = reinterpret_cast<const unsigned char *>(p2);
		for (unsigned i = 0; i < s1 && i < s2; i++) {
			if (cp1[i] != cp2[i]) {
				unsigned b = std::max(0, (int)i-4); // show 8 bytes around difference
				unsigned e = std::min(i+4, std::min(s1, s2));
				auto f = cout.flags(); // save stream format state
				cout << " -- difference found (index:expected:actual)" << endl;
				cout << std::hex << std::uppercase;
				for (unsigned j = b; j < e; j++) {
					// hex & uppercase are sticky, setfill & setw are not
					cout << " " << std::setfill('0') << std::setw((log2rz(e)+1+3)/4) << j;
					cout << ":" << std::setfill('0') << std::setw(2) << int{cp1[j]};
					cout << ":" << std::setfill('0') << std::setw(2) << int{cp2[j]};
					if (j == i) cout << " *";
					cout << endl;
				}
				cout.flags(f); // restore stream format state
				return true;
			}
		}
		if (s2 > s1) {
			for (unsigned i = s1; i < s2; i++)
				if (cp2[i] != 0) return true;
		}
		return false;
	}

	// get pointer to the real data buffer1
	R* get_buf_R1(void)
	{
		return R1;
	}

	// get pointer to the real data buffer2
	R* get_buf_R2(void)
	{
		return R2;
	}

	// get pointer to the encoded data buffer
	U* get_buf_EN(void)
	{
		return EN;
	}

	// check the real data buffer2 for match with buffer1
	virtual bool check_R2(void)
	{
		return false; // return true on error
	}

	// check the data in the encoded buffer, length in elements U
	virtual bool check_EN(unsigned len, uint64_t &eticks)
	{
		bool retval = false;
#if defined(ZCHK)
		// bsize must be multiple of ZFP stream word size (8)
		unsigned bsize = CEIL(par.maxbits/8*blocks, 8);
		char *bptr = new char[bsize]; // alloc buffer
		std::memset(bptr, 0, bsize);

		eticks = UINT64_MAX;
		for (int i = 0; i < 6; i++) {
			tick_t t0, t1;
			tget(t0);
			zfp_encode<R,DIM>(par, bptr, bsize, R1, blocks);
			tget(t1);
			uint64_t etmp = tval(t1) - tval(t0);
			if (etmp < eticks) eticks = etmp;
		}

		if (compare(bptr, bsize, EN, len*sizeof(U))) retval = true;
		// cout << " Compressed data (reference)" << endl;
		// dump(bptr, bsize, sizeof(U));
		delete[] bptr; // free buffer
#else
		cout << " -- warning: encoded data not checked" << endl;
#endif
		return retval; // return true on error
	}

protected: // three buffers for sequence of R1 -encode-> EN -decode-> R2
	R *R1; // Uncompressed real data (input to encode)
	U *EN; // Compressed data (output from encode, input to decode)
	R *R2; // Uncompressed real data (output from decode)
	zpar par;        // ZFP parameters
	unsigned blocks; // number of ZFP blocks of type R to process
};

//-----------------------------------------------------------------------------
// Test case 1; custom
//-----------------------------------------------------------------------------
#include <cstring> // memcpy

typedef unsigned long long ull;

template<int DIM>
struct tcase1_base {
	static const ull eblock[]; // encoded block
};

template<typename R, typename U, int DIM>
class tcase1 : public tcase<R,U,DIM>, public tcase1_base<DIM> {
	using tcase<R,U,DIM>::block_len;
	using tcase<R,U,DIM>::R1;
	using tcase<R,U,DIM>::EN;
	using tcase<R,U,DIM>::blocks;
	using tcase<R,U,DIM>::compare;
	using tcase1_base<DIM>::eblock;

public:

	tcase1(const zpar &par, unsigned blocks) :
		tcase<R,U,DIM>::tcase(par, blocks),
		tcase1_base<DIM>::tcase1_base()
	{
		// assert sizeof(R) is equal to sizeof(ull)
		// static_assert(sizeof(R)==sizeof(ull), "This case only supports 64-bit FP width");
		if (sizeof(R) != sizeof(ull)) {cout << " -- test 1 not supported" << endl; return;}
		for (unsigned j = 0; j < blocks; j++) {
			std::memcpy(R1+j*block_len, rblock, block_len*sizeof(R));
		}
	}

#if !defined(ZCHK)
	bool check_EN(unsigned len, double &esec)
	{
		unsigned cblen =  len / blocks;
		esec = 0.0;
		for (unsigned j = 0; j < blocks; j++) {
			if (compare(eblock, sizeof(eblock), EN+j*cblen, cblen*sizeof(U))) {
				cout << " in block: " << j << endl;
				return true;
			}
		}
		return false;
	}
#endif

	static const ull rblock[]; // real block
};

/* example 3D block of (reinterpreted) doubles */
template<typename R, typename U, int DIM>
const ull tcase1<R,U,DIM>::rblock[] = {
	0xbf7c3a7bb8495ca9ULL,
	0xbf79f9d9058ffdafULL,
	0xbf77c7abd0b61999ULL,
	0xbf75a42c806bd1daULL,
	0xbf738f8f740b8ea8ULL,
	0xbf718a050399fef8ULL,
	0xbf6f2772ff8c30feULL,
	0xbf6b59aa63d22f68ULL,
	0xbf67aaf8b80cff9eULL,
	0xbf641b9e71983592ULL,
	0xbf60abd3f723f2b7ULL,
	0xbf5ab7934169cc04ULL,
	0xbf54574f6f4897d3ULL,
	0xbf4c6e39da7fb99bULL,
	0xbf40ae5826a893d1ULL,
	0xbf25bce8e19d48e1ULL,
	0x3f253bfed65904d7ULL,
	0x3f3f18ab46a04cf3ULL,
	0x3f4948e7cb74278bULL,
	0x3f51427b51aeec2eULL,
	0x3f55a0716d8b4b6bULL,
	0x3f59be96aeaac56fULL,
	0x3f5d9d3ba7bfd327ULL,
	0x3f609e608469e93eULL,
	0x3f624ecbcfa3832cULL,
	0x3f63e0202ae84b4dULL,
	0x3f6552a61a3f4812ULL,
	0x3f66a6ae305af268ULL,
	0x3f67dc910e9935bcULL,
	0x3f68f4af65036ff7ULL,
	0x3f69ef71f24e7182ULL,
	0x3f6acd4983da7d43ULL,
	0x3f6b8eaef5b348a0ULL,
	0x3f6c3423328ffb7aULL,
	0x3f6cbe2f33d33034ULL,
	0x3f6d2d64018af3acULL,
	0x3f6d825ab270c540ULL,
	0x3f6dbdb46be996ccULL,
	0x3f6de01a6205cca9ULL,
	0x3f6dea3dd7813dafULL,
	0x3f6ddcd81dc33335ULL,
	0x3f6db8aa94de690fULL,
	0x3f6d7e7eab910d8fULL,
	0x3f6d2f25df44c187ULL,
	0x3f6ccb79bc0e9844ULL,
	0x3f6c545bdcaf1795ULL,
	0x3f6bcab5ea9237c4ULL,
	0x3f6b2f799dcf639bULL,
	0x3f6a83a0bd297862ULL,
	0x3f69c82d1e0ec5deULL,
	0x3f68fe28a4990e53ULL,
	0x3f6826a5438d8685ULL,
	0x3f6742bcfc5cd5b2ULL,
	0x3f665391df231599ULL,
	0x3f655a4e0aa7d278ULL,
	0x3f645823ac5e0b09ULL,
	0x3f634e4d00643085ULL,
	0x3f623e0c518426a3ULL,
	0x3f6128abf933439aULL,
	0x3f600f7e5f92501cULL,
	0x3f5de7bbf6db0eb7ULL,
	0x3f5bae5aa4792e11ULL,
	0x3f5975adf0453ea2ULL,
	0x3f57409b1fdc65c4ULL
};

// full specialization of member variable
template<>
const ull tcase1_base<1>::eblock[] = {
	0x44226b6814c8b7f1ULL,
	0x7a0bce452dd3f926ULL,
	0x307f612513b680c3ULL,
	0x000002859db30dfeULL,
	0x0000000000000000ULL
};

template<>
const ull tcase1_base<2>::eblock[] = {
	0x003884d03245c7f1ULL,
	0x091d010ad047252cULL,
	0x5285d1806620da84ULL,
	0x40d7881600be02d1ULL,
	0xa072df0e1e27bc09ULL,
	0x90f423f80e05bfdeULL,
	0xa28c611b70c42f1aULL,
	0x25ba68972981ebd6ULL,
	0x05da951068d286aeULL,
	0x7b0228252a17f908ULL,
	0x020f8973bda689e0ULL,
	0x13b505feba999afdULL,
	0x00000000014fe72fULL,
	0x0000000000000000ULL
};

template<>
const ull tcase1_base<3>::eblock[] = {
	0x0cd0aa15419447f1ULL,
	0x710010017d4c0e54ULL,
	0x620600333202028dULL,
	0x02040073e404000aULL,
	0x008c800c044b5433ULL,
	0x4001496a0010a813ULL,
	0x524807588a001585ULL,
	0x95be1fe880651a42ULL,
	0x6cd5b1b17003fa12ULL,
	0xdbe4ded0e0040760ULL,
	0xffc2134fc814c1e3ULL,
	0x9f1e471f9062b6cfULL,
	0xafc3aecc29c74eeeULL,
	0xf250ef5002361ddfULL,
	0x5879fbb553895d2dULL,
	0xa7a9eb9ab6b77a10ULL,
	0xffb97428d21bbccaULL,
	0xc4796652882b7906ULL,
	0xc07cf56bd0667207ULL,
	0xea619acd01489503ULL,
	0x1a90a8e2c5d12080ULL,
	0x9ca654114706fadeULL,
	0x13757b758b935484ULL,
	0x129ad98283279ba9ULL,
	0x76e8b21276cbad2bULL,
	0x6ff340306eae1b32ULL,
	0xd1f71976575ec676ULL,
	0x03003e66d927a06eULL,
	0xc5c2f4f098a38800ULL,
	0xe77f4d73e0a1c499ULL,
	0x11543a41f4245173ULL,
	0x49cb8b8e9a9783b9ULL,
	0x6a490f2b19988b7eULL,
	0x0000000000000325ULL,
	0x0000000000000000ULL
};

#if 0
// partial specialization of template class leaves out other methods
template<typename R, typename U>
class tcase1<R,U,1> {
	const ull eblock[] = {
	0x44226b6814c8b7f1ULL,
	0x7a0bce452dd3f926ULL,
	0x307f612513b680c3ULL,
	0x000002859db30dfeULL,
	0x0000000000000000ULL};
};
#endif

//-----------------------------------------------------------------------------
// Test case 2: all zero
//-----------------------------------------------------------------------------

template<typename R, typename U, int DIM>
class tcase2 : public tcase<R,U,DIM> {
	using tcase<R,U,DIM>::block_len;
	using tcase<R,U,DIM>::R1;

public:

	tcase2(const zpar &par, unsigned blocks) :
		tcase<R,U,DIM>::tcase(par, blocks)
	{
		for (unsigned i = 0; i < blocks*block_len; i++) {
			R1[i] = 0;
		}
	}

};

//-----------------------------------------------------------------------------
// Test case 3: range from zero upward
//-----------------------------------------------------------------------------
#include <limits> // numeric_limits

template<typename R, typename U, int DIM>
class tcase3 : public tcase<R,U,DIM> {
	using tcase<R,U,DIM>::block_len;
	using tcase<R,U,DIM>::R1;

public:


	tcase3(const zpar &par, unsigned blocks) :
		tcase<R,U,DIM>::tcase(par, blocks)
	{
		R tmp = 0;
		R inc = std::numeric_limits<R>::has_denorm == std::denorm_present ?
			std::numeric_limits<R>::denorm_min() :
			std::numeric_limits<R>::min();
		for (unsigned i = 0; i < blocks*block_len; i++) {
			R1[i] = tmp;
			tmp += inc;
		}
	}

};

//-----------------------------------------------------------------------------
// Test case 4: range from norm down to minimum (or subnorm)
//-----------------------------------------------------------------------------
#include <limits> // numeric_limits
#include <cmath> // ldexp

template<typename R, typename U, int DIM>
class tcase4 : public tcase<R,U,DIM> {
	using tcase<R,U,DIM>::block_len;
	using tcase<R,U,DIM>::R1;

public:

	tcase4(const zpar &par, unsigned blocks) :
		tcase<R,U,DIM>::tcase(par, blocks)
	{
		// limit scale to exponent range
		unsigned erng = std::numeric_limits<R>::max_exponent -
			std::numeric_limits<R>::min_exponent;
		int scale = std::min(blocks*block_len-1, erng);
		if (std::numeric_limits<R>::has_denorm == std::denorm_present)
			scale -= 4;
		R tmp = std::ldexp(std::numeric_limits<R>::min(), scale);
		for (unsigned i = 0; i < blocks*block_len; i++) {
			R1[i] = tmp;
			tmp /= 2;
		}
	}

};

//-----------------------------------------------------------------------------
// Test case 5: range from norm up to max
//-----------------------------------------------------------------------------
#include <limits> // numeric_limits
#include <cmath> // ldexp, isfinite

template<typename R, typename U, int DIM>
class tcase5 : public tcase<R,U,DIM> {
	using tcase<R,U,DIM>::block_len;
	using tcase<R,U,DIM>::R1;

public:

	tcase5(const zpar &par, unsigned blocks) :
		tcase<R,U,DIM>::tcase(par, blocks)
	{
		// limit scale to exponent range
		unsigned erng = std::numeric_limits<R>::max_exponent -
			std::numeric_limits<R>::min_exponent;
		int scale = std::min(blocks*block_len-1, erng);
		R tmp = std::ldexp(std::numeric_limits<R>::max(), -scale);
		for (unsigned i = 0; i < blocks*block_len; i++) {
			R1[i] = std::isfinite(tmp) ? tmp : std::numeric_limits<R>::max();
			tmp *= 2;
		}
	}

};

//-----------------------------------------------------------------------------
// Test case 6: function
//-----------------------------------------------------------------------------
#include <limits> // numeric_limits
#include <cmath> // pow
#include <cstdlib> // rand, srand

template<typename R, typename U, int DIM>
class tcase6 : public tcase<R,U,DIM> {
	using tcase<R,U,DIM>::block_len;
	using tcase<R,U,DIM>::R1;

	// given flat index i, return blocked ZFP order index idx, n: axis length
	void order(unsigned *idx, unsigned n, unsigned i)
	{
		unsigned b = n/4; // axis length in blocks
		unsigned bidx = i / block_len;
		unsigned nidx = i % block_len;
		for (unsigned d = 0; d < DIM; d++) {
			idx[d] = (bidx % b)*4 + nidx % 4;
			bidx /= b; nidx /= 4;
		}
		idx[DIM] = bidx;
	}

#if 1
	// function(idx) = e*(x^3 + y^3 + ...)
	R equation(unsigned *idx, unsigned n)
	{
		R e(2.718281828459045235360287471352662497757247093699959574966967627724);
		R tmp = 0;
		for (unsigned d = 0; d < DIM; d++) {
			R val = R(idx[d]) - R(n/2); // -n/2 <= val < +n/2
			tmp += val*val*val;
		}
		tmp *= e;
		return tmp;
	}
#else
	// function(idx) = x/DIM + y/DIM + ...)
	R equation(unsigned *idx, unsigned n)
	{
		R tmp = 0;
		for (unsigned d = 0; d < DIM; d++) {
			R val = 2*R(idx[d])/n - 1; // -1 <= val < +1
			tmp += val/DIM;
		}
		return tmp;
	}
#endif

public:

	tcase6(const zpar &par, unsigned blocks) :
		tcase<R,U,DIM>::tcase(par, blocks)
	{
		unsigned entries = blocks*block_len;
		unsigned n = (unsigned)(pow(blocks,1.0/DIM)+0.5)*4; // axis length
		R vmin(+std::numeric_limits<R>::max());
		R vmax(-std::numeric_limits<R>::max());

		std::srand(47);
		unsigned idx[DIM+1] = {0}; // index for each dimension + 1 for carry
		for (unsigned i = 0; i < entries; i++) {
			order(idx, n, i);
			R tmp = equation(idx, n);
			tmp += tmp * (2*R(std::rand())/RAND_MAX-1)*1e-3; // add 0.1% jitter
			if (tmp < vmin) vmin = tmp;
			if (tmp > vmax) vmax = tmp;
			R1[i] = tmp;
			// cout << ' ' << i << ' ';
			// for (unsigned j = 0; j <= DIM; j++) cout << idx[DIM-j] << ':';
			// cout << ' ' << tmp << endl;
		}
		cout << "axis length: " <<    n << endl;
		cout << "array min  : " << vmin << endl;
		cout << "array max  : " << vmax << endl;
	}

};

#endif // TCASE_H

/* TODO:
 * Test case with infinity.
*/
