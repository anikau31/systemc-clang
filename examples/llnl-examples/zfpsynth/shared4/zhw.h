
#ifndef ZHW_H
#define ZHW_H

#include "systemc.h"
#include "ieee.h"
#include "bits.h"

// RLEVEL may be defined previously
#ifndef RLEVEL
#define RLEVEL false
#endif

/* negabinary mask */
#define NBMASK (typename FP::ui_t(0xAAAAAAAAAAAAAAAAULL))

// log2rp(0) = 0, log2rz(0) = 0 // actually -Inf
// log2rp(1) = 0, log2rz(1) = 0
// log2rp(2) = 1, log2rz(2) = 1
// log2rp(3) = 2, log2rz(3) = 1
// log2rp(4) = 2, log2rz(4) = 2
// log2rp(5) = 3, log2rz(5) = 2
// log2rp(6) = 3, log2rz(6) = 2
// log2rp(7) = 3, log2rz(7) = 2
// log2rp(8) = 3, log2rz(8) = 3

// return the log base 2 of the argument x rounded to zero
constexpr unsigned int log2rz(unsigned int x)
{
	return (x > 1) ? log2rz(x>>1)+1 : 0;
}

namespace zhw {

typedef signed   short sconfig_t;
typedef unsigned short uconfig_t;

// floating-point block size given dimension
constexpr int fpblk_sz(int dim) {return 1 << 2*dim;} // 4^dim;
// #define fpblk_sz(dim) (1 << 2*dim)

constexpr int bc_w(int dim) {return 2*dim+2;} // bit count width
constexpr int bp_w(int dim) {return 2*fpblk_sz(dim)+1;} // bit plane width MB: bug bug, this is incorrect and should be "-1" not "+1"
constexpr int bw_w(int dim) {return 2*fpblk_sz(dim);}// bit window (max encoded bit plane width) + 1

//decoder support 1-3D
//constexpr int pr_w(int dim) {return dim < 2 ? 16 : bw_w(dim);};	//bit plane register datatype width
//constexpr int sr_sz(int dim){return dim < 2 ? 8 : 4;};			//number of smart registers. Minimum is 3 for 3d but that is complicated. 1D must have 8 to make things work.
//constexpr int r_th(int dim){return dim < 2 ? 5 : 3;}		//do not drop register file content below this level.

//Assume machine shift register width is max 32.
constexpr int pr_w(int dim) {return dim < 2 ? 16 : bw_w(2);};			//bit plane register datatype width
constexpr int sr_sz(int dim){return dim == 1 ? 8 : dim == 2 ? 4 : 16;};	//number of smart registers.
constexpr int r_th(int dim){return dim == 1 ? 5 : dim == 2 ? 3 : 13;}	//do not drop register file content below this level.

template<class T>
constexpr const T& max(const T& a, const T& b)
{return a > b ? a : b;}

extern sc_trace_file *tf;

} // namespace zhw

#endif // ZHW_H

