#include <iostream>
#include <stdio.h>
using namespace std;

class sc_gpu_thread_hierarchy {
public:
  sc_gpu_thread_hierarchy(int xd, int yd, int zd)
  :_xd(xd), _yd(yd), _zd(zd) {
  } void dump() {
    printf("thread: xd %d, yd %d, zd %d\n", _xd, _yd, _zd);
  }

  int _xd;
  int _yd;
  int _zd;

};


class sc_gpu_block_hierarchy {
public:
  sc_gpu_block_hierarchy(int xd, int yd, int zd)
  :_xd(xd), _yd(yd), _zd(zd) {
  } void dump() {
    printf("block: xd %d, yd %d, zd %d\n", _xd, _yd, _zd);
  }

  int _xd;
  int _yd;
  int _zd;
};

#define SC_GPU_THREADS(xd, yd, zd) \
    sc_gpu_thread_hierarchy tht(xd,yd,zd); \
  tht.dump();


#define SC_GPU_BLOCKS(xd, yd, zd) \
    sc_gpu_block_hierarchy thb(xd,yd,zd); \
  thb.dump();
