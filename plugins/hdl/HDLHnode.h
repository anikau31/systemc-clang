#ifndef _HDLHnode_H_
#define _HDLHnode_H_

// clang-format off

#include "SystemCClang.h"
#include "hNode.h"

using namespace clang;
using namespace systemc_clang;

using namespace hnode;

namespace systemc_hdl {
  void RemoveSCMethod(hNodep &hp);
  
  static inline bool isInitPB(hNodep hp) {
    return (hp->h_op == hNode::hdlopsEnum::hBinop) &&
      (hp->h_name == "()");
  }
  
  class HDLConstructorHcode {
  public:
    HDLConstructorHcode() {};
    typedef struct {
      string name;
      int lo, hi, step;
    } for_info_t;
      
    hNodep ProcessCXXConstructorHcode(hNodep xconstructor);
    void HDLLoop(hNodep &hp, std::vector<for_info_t> &for_info);

    void PushRange(hNodep &hp, std::vector<for_info_t> &for_info);
    void PopRange(std::vector<for_info_t> &for_info);
    void UnrollBinding(hNodep &hp, int unrollfactor, std::vector<for_info_t> &for_info);
  private:
    hNodep hnewpb;

  };
}
#endif
