#ifndef REAL_H
#define REAL_H

/* IEEE 754 half-precision: expo: 5 bits, frac: 10 bits */
/* IEEE 754 single-precision: expo: 8 bits, frac: 23 bits */
/* IEEE 754 double-precision: expo: 11 bits, frac: 52 bits */
/* Extended precision: expo: 15 bits, frac: 64 bits */
/* IEEE 754 quadruple-precision: expo: 15 bits, frac: 112 bits */

typedef long double ldouble;

#if defined(USE_QUAD)
// quad may give error (g++ 6.4.0) without -std=c++11, use quadp, f128?
typedef __float128 quad;
#include <quadmath.h>
#undef isnan
#undef isinf
#undef isfinite
namespace std
{
	inline quad fabs(quad arg) { return fabsq(arg); }
	inline quad ceil(quad arg) { return ceilq(arg); }
	inline quad sqrt(quad arg) { return sqrtq(arg); }
	inline quad cbrt(quad arg) { return cbrtq(arg); }
	inline quad pow(quad base, quad exp) { return powq(base, exp); }
	inline quad fmax(quad x, quad y) { return fmaxq(x, y); }
	inline quad fmin(quad x, quad y) { return fminq(x, y); }
	inline bool isnan(quad arg) { return isnanq(arg); }
	inline bool isinf(quad arg) { return isinfq(arg); }
	// inline quad logb(quad arg) { return logbq(arg); }
	inline quad logb(quad arg) { return logb((long double)arg); }
	inline quad ldexp(quad arg, int exp) { return ldexpq(arg, exp); }
	inline bool isfinite(quad arg) { return finiteq(arg); }
	inline quad frexp(quad arg, int *exp) { return frexpq(arg, exp); }
}

//TODO: numeric_limits<quad>?
// namespace std {
	// template <> class numeric_limits<quad>
	// {
	// public:
	// };
// }

#else
#include <cmath>

#endif // USE_*


#if defined(USE_QUAD)
typedef quad real_t;  // floating point representation
#define RL(arg) (arg##Q)

#elif defined(USE_LDOUBLE)
typedef long double real_t;  // floating point representation
#define RL(arg) (arg##L)

#elif defined(USE_FLOAT)
typedef float real_t;  // floating point representation
#define RL(arg) (arg##F)

#else // USE_DOUBLE
typedef double real_t;  // floating point representation
#define RL(arg) (arg)

#endif // USE_*

#endif // REAL_H
