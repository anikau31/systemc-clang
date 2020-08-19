
#include <cstdlib> // exit
#include <iostream> // cerr, endl
#include "zbatch.h"
#include "zfp.h"

// ZFP includes are contained in a separate compilation unit
// because of name conflicts with SystemC, specifically int64.


//-----------------------------------------------------------------------------
// Support
//-----------------------------------------------------------------------------

// floating-point block length given dimension
constexpr int block_len(int dim) {return 1 << 2*dim;} // 4^dim;
// #define block_len(dim) (1 << 2*dim)

inline zfp_stream *zfp_alloc(const zpar &par, char *buf, unsigned bytes)
{
	bitstream *stream = stream_open(buf, bytes); // alloc stream
	if (stream == nullptr) {
		std::cerr << " -- stream alloc failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	zfp_stream *zfp = zfp_stream_open(stream); // alloc zfp
	if (zfp == nullptr) {
		std::cerr << " -- zfp alloc failed" << std::endl;
		stream_close(stream);
		exit(EXIT_FAILURE);
	}
	zfp_stream_set_params(zfp, par.minbits, par.maxbits, par.maxprec, par.minexp);
	return zfp;
}

inline void zfp_free(zfp_stream *zfp)
{
	stream_flush(zfp->stream); // flush stream
	stream_close(zfp->stream); // free stream, does not flush
	zfp_stream_close(zfp); // free zfp
}

//-----------------------------------------------------------------------------
// 1-D
//-----------------------------------------------------------------------------

template<> // template function specialization
void zfp_encode<float,1>(const zpar &par, char *buf, unsigned bytes, float *fp, unsigned blocks)
{
	zfp_stream *zfp = zfp_alloc(par, buf, bytes);

	for (unsigned i = 0; i < blocks; i++) {
		zfp_encode_block_float_1(zfp, fp+block_len(1)*i);
	}

	zfp_free(zfp);
}

template // template function explicit instantiation
void zfp_encode<float,1>(const zpar &par, char *buf, unsigned bytes, float *fp, unsigned blocks);

template<> // template function specialization
void zfp_encode<double,1>(const zpar &par, char *buf, unsigned bytes, double *fp, unsigned blocks)
{
	zfp_stream *zfp = zfp_alloc(par, buf, bytes);

	for (unsigned i = 0; i < blocks; i++) {
		zfp_encode_block_double_1(zfp, fp+block_len(1)*i);
	}

	zfp_free(zfp);
}

template // template function explicit instantiation
void zfp_encode<double,1>(const zpar &par, char *buf, unsigned bytes, double *fp, unsigned blocks);

//-----------------------------------------------------------------------------
// 2-D
//-----------------------------------------------------------------------------

template<> // template function specialization
void zfp_encode<float,2>(const zpar &par, char *buf, unsigned bytes, float *fp, unsigned blocks)
{
	zfp_stream *zfp = zfp_alloc(par, buf, bytes);

	for (unsigned i = 0; i < blocks; i++) {
		zfp_encode_block_float_2(zfp, fp+block_len(2)*i);
	}

	zfp_free(zfp);
}

template // template function explicit instantiation
void zfp_encode<float,2>(const zpar &par, char *buf, unsigned bytes, float *fp, unsigned blocks);

template<> // template function specialization
void zfp_encode<double,2>(const zpar &par, char *buf, unsigned bytes, double *fp, unsigned blocks)
{
	zfp_stream *zfp = zfp_alloc(par, buf, bytes);

	for (unsigned i = 0; i < blocks; i++) {
		zfp_encode_block_double_2(zfp, fp+block_len(2)*i);
	}

	zfp_free(zfp);
}

template // template function explicit instantiation
void zfp_encode<double,2>(const zpar &par, char *buf, unsigned bytes, double *fp, unsigned blocks);

//-----------------------------------------------------------------------------
// 3-D
//-----------------------------------------------------------------------------

template<> // template function specialization
void zfp_encode<float,3>(const zpar &par, char *buf, unsigned bytes, float *fp, unsigned blocks)
{
	zfp_stream *zfp = zfp_alloc(par, buf, bytes);

	for (unsigned i = 0; i < blocks; i++) {
		zfp_encode_block_float_3(zfp, fp+block_len(3)*i);
	}

	zfp_free(zfp);
}

template // template function explicit instantiation
void zfp_encode<float,3>(const zpar &par, char *buf, unsigned bytes, float *fp, unsigned blocks);

template<> // template function specialization
void zfp_encode<double,3>(const zpar &par, char *buf, unsigned bytes, double *fp, unsigned blocks)
{
	zfp_stream *zfp = zfp_alloc(par, buf, bytes);

	for (unsigned i = 0; i < blocks; i++) {
		zfp_encode_block_double_3(zfp, fp+block_len(3)*i);
	}

	zfp_free(zfp);
}

template // template function explicit instantiation
void zfp_encode<double,3>(const zpar &par, char *buf, unsigned bytes, double *fp, unsigned blocks);
