#ifndef _HDLHnode_H_
#define _HDLHnode_H_

// clang-format off

#include "SystemCClang.h"
#include "hNode.h"

using namespace clang;
using namespace systemc_clang;

using namespace hnode;

namespace systemc_hdl {
  
  static const string fielddelim{"##"};
  static const string tokendelim{"_"};
  static const string qualnamedelim(":");
  static const string pbstring{"()"};
  static const string arrsub{"ARRAYSUBSCRIPT"};
  static const string noname{"NONAME"};
  static const string sensop{"<<"};
  static const string localstr{"_local_"};
  static const string strsccore("sc_core__sc");

  static inline bool isInitPB(hNodep hp) {
    return (hp->h_op == hNode::hdlopsEnum::hBinop) &&
      (hp->h_name == pbstring);
  }
  static inline bool isInitSensitem(hNodep hp) {
    return (hp->h_op == hNode::hdlopsEnum::hBinop) &&
      (hp->h_name == sensop);
  }

  static inline bool isMethodCall(hNodep hp) {
    return ((hp->h_op == hNode::hdlopsEnum::hVarAssign) &&
	    (hp->child_list.size() == 2) &&
	    (hp->child_list[1]->h_op == hNode::hdlopsEnum::hMethodCall));
  }

  static inline bool isEdge(string &s) {
    return (s=="pos" ||
	    s=="posedge" ||
	    s=="neg" ||
	    s=="negedge");
  }

  static inline bool isSimEvent(string &s) {
    return (s=="value_changed_event");
  }
  class HDLConstructorHcode {
  public:
    HDLConstructorHcode() {};
    typedef struct {
      string name;
      int lo, hi, step;
      int curix;
    } for_info_t;

    hNodep ProcessCXXConstructorHcode(hNodep xconstructor);
    void HDLLoop(hNodep &hp, std::vector<for_info_t> &for_info);

    void PushRange(hNodep &hp, std::vector<for_info_t> &for_info);
    void PopRange(std::vector<for_info_t> &for_info);
    void UnrollBinding(hNodep &hp_orig, std::vector<for_info_t> &for_info);
    bool SetupSenslist(hNodep hp);
    void UnrollSensitem(hNodep &hp_orig, std::vector<for_info_t> &for_info);
    void SubstituteIndex(hNodep &hp, std::vector<for_info_t> &for_info);
    hNodep HnodeDeepCopy(hNodep hp);
    void RemoveSCMethod(hNodep &hp);
    void CleanupInitHcode(hNodep &hp);
  private:
    hNodep hnewpb;  // port binding list
    std::vector<hNodep> hnewsens; // sensitivity lists

  };
}
#endif
