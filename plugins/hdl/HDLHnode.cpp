// clang-format off
#include "hNode.h"
#include <assert.h>
#include "HDLHnode.h"

// clang-format on

//!
//! Re-write hcode generated from CXXConstructorDecl for an SC_MODULE:
//! Pass variable initialization statements through
//! Unroll for loops enclosing port bindings and generate unrolled port bindings
//! Discard instantiations of SC_METHOD and similar decls
//! Discard Unimplemented op codes, typically sensitivity lists
//!
namespace systemc_hdl {

  void RemoveSCMethod(hNodep &hp) {
 
    hp->child_list.remove_if([] (hNodep x)
			     { return (((x->h_op==hNode::hdlopsEnum::hVarAssign) &&
					(x->child_list.size()==2) &&
					(x->child_list.back()->h_op != hNode::hdlopsEnum::hLiteral))
				       || (x->h_op == hNode::hdlopsEnum::hMethodCall)
				       || (x->h_op == hNode::hdlopsEnum::hUnimpl));} );

    for_each(hp->child_list.begin(), hp->child_list.end(), RemoveSCMethod);      
  }

  //std::vector<string, tuple<int low, int hi, int step>> for_index_map;

  void ProcessForLoop(hNodep &hp) {
    
  }
  
  hNodep ProcessCXXConstructorHcode(hNodep xconstructor) {
    if (xconstructor==nullptr) return xconstructor;
    RemoveSCMethod(xconstructor);
    ProcessForLoop(xconstructor);
    return xconstructor;
  }

}
