#ifndef ZBATCH_H
#define ZBATCH_H

struct zpar {       /* ZFP parameters */
  unsigned minbits; /* minimum number of bits per 4^d block */
  unsigned maxbits; /* maximum number of bits per 4^d block */
  unsigned maxprec; /* maximum precision (# bit planes coded) */
  int minexp;       /* minimum base-2 exponent; error <= 2^minexp */
};

template<typename R, int DIM>
void zfp_encode(const zpar &par, char *buf, unsigned bytes, R *fp, unsigned blocks);

#endif // ZBATCH_H
