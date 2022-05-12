
#ifndef SC_STREAM_IMP_H
#define SC_STREAM_IMP_H

#ifndef CHAN_NS
#define CHAN_NS conduit
#endif

#if defined(__SYNTHESIS__) || defined(RVD)
#include "sc_rvd.h"
#include "rvfifo_cc.h"
template <typename T> using sc_stream = sc_rvd<T>;
template <typename T> using sc_stream_in = sc_rvd_in<T>;
template <typename T> using sc_stream_out = sc_rvd_out<T>;
template <typename T, int IW = 1, bool RLEV = false> using sfifo_cc = rvfifo_cc<T, IW, RLEV>;

#else
#include "sc_stream_ports.h"
#include "sc_stream.h"
#include "sfifo_cc.h"
#endif

#ifdef CHAN_NS
using namespace CHAN_NS;
#endif

#endif // SC_STREAM_IMP_H
