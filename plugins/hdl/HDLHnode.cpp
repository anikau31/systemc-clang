// clang-format off

#include <assert.h>
#include "HDLHnode.h"

#include "clang/Basic/Diagnostic.h"

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
 
    hp->child_list.erase( std::remove_if( hp->child_list.begin(), hp->child_list.end(), [] (hNodep x)
			     { return (((x->h_op==hNode::hdlopsEnum::hVarAssign) &&
					(x->child_list.size()==2) &&
					(x->child_list.back()->h_op != hNode::hdlopsEnum::hLiteral))
				        || (x->h_op == hNode::hdlopsEnum::hVardecl) // index variables
				       || (x->h_op == hNode::hdlopsEnum::hMethodCall) // sc_method
				       || (x->h_op == hNode::hdlopsEnum::hUnimpl));}), hp->child_list.end() );

    for_each(hp->child_list.begin(), hp->child_list.end(), RemoveSCMethod);      
  }

  //!
  //! for loop range for port bindings is expecting 3 simple range arguments:
  //! eg.
  //!
       //! hForStmt  NONAME [
       //!    hVarAssign  NONAME [
       //!      hVarref _local_5 NOLIST
       //!      hLiteral 0 NOLIST
       //!    ]
       //!    hBinop < [
       //!      hVarref _local_5 NOLIST
       //!      hLiteral 16 NOLIST
       //!    ]
       //!    hUnop ++ [
       //!      hVarref _local_5 NOLIST
       //!    ]
       //!    hBinop () [
       //!      hBinop ARRAYSUBSCRIPT [
       //!        hVarref u_encode_ints##s_block NOLIST
       //!        hVarref _local_5 NOLIST
       //!      ]
       //!      hBinop ARRAYSUBSCRIPT [
       //!        hVarref c_bb_block NOLIST
       //!        hVarref _local_5 NOLIST
       //!      ]
       //!    ]
       //!  ]
  //!
  
  void HDLConstructorHcode::PushRange(hNodep &hp, std::vector<for_info_t> &for_info) {

    for_info_t tmp{"FORNAME", 0, 1, 1};
    
    hNodep hlo = hp->child_list[0];
    hNodep hi = hp->child_list[1];
    hNodep hstep = hp->child_list[2];
    

    if ((hlo->h_op == hNode::hdlopsEnum::hVarAssign) && 
	(hlo->child_list.size() == 2) &&
	(hlo->child_list[0]-> h_op == hNode::hdlopsEnum::hVarref) &&
	(hlo->child_list[1]->h_op ==  hNode::hdlopsEnum::hLiteral)) {
      tmp.name = hlo->child_list[0]->h_name;
      tmp.lo = stoi(hlo->child_list[1]->h_name);
    }
    //else 
    
    if ((hi->h_op == hNode::hdlopsEnum::hBinop) && 
	(hlo->child_list.size() == 2) &&
	(hlo->child_list[0]-> h_op == hNode::hdlopsEnum::hVarref) &&
	(hlo->child_list[1]->h_op ==  hNode::hdlopsEnum::hLiteral)) {
      // check that names are same ... tmp.name = hlo->child_list[0]->h_name;
      tmp.hi = stoi(hi->child_list[1]->h_name);
    }
    for_info.push_back(tmp);

  }

  void HDLConstructorHcode::PopRange(std::vector<for_info_t> &for_info) {
      for_info.pop_back();
  }
  
  void HDLConstructorHcode::UnrollBinding(hNodep &hp, int unrollfactor, std::vector<for_info_t> &for_info) {

    // need to dismantle modname##field:
    // modname is the h_name for the portbinding
    // and field is the varref, e.g.
    // hBinop () [
          // hVarref u_fwd_cast##clk NOLIST
          // hVarref clk NOLIST
    // but if the submod port is ARRAYSUBSCRIPT
    // tree will look like
    // hBinop () [
    //           hBinop ARRAYSUBSCRIPT [
    //             hVarref u_xt NOLIST
    //             hVarref _local_0 NOLIST
    //             hVarref reset NOLIST
    //           ]
    //           hVarref reset NOLIST
    
    string submodport = hp->child_list[0]->h_name;
    static const string fielddelim{"##"};
    string thismodsig = hp->child_list[1]->h_name;
    if (for_info.size()>0) { // in a for loop
      for (int i = 0; i < unrollfactor; i++) {
	hNodep hpb = new hNode( hNode::hdlopsEnum::hPortbinding);
	hpb->child_list.push_back(new hNode(submodport+to_string(i), hNode::hdlopsEnum::hVarref));
	hpb->child_list.push_back(new hNode(thismodsig+to_string(i), hNode::hdlopsEnum::hVarref));
	hnewpb->child_list.push_back(hpb);

      }
    }
    else {
      hNodep hpb = new hNode( hNode::hdlopsEnum::hPortbinding);
      hpb->child_list.push_back(new hNode(submodport, hNode::hdlopsEnum::hVarref));
      hpb->child_list.push_back(new hNode(thismodsig, hNode::hdlopsEnum::hVarref));
      hnewpb->child_list.push_back(hpb);
    }
  }
  
  void HDLConstructorHcode::HDLLoop(hNodep &hp, std::vector<for_info_t> &for_info ) {
    if ((hp->h_op == hNode::hdlopsEnum::hForStmt) && (hp->child_list.size() > 3)) {
      PushRange(hp, for_info); // fill in name, lo, hi, step
      for (int i=3; i<hp->child_list.size(); i++) {
	if (isInitPB(hp->child_list[i])) {// hcode indicating port binding
	  UnrollBinding(hp->child_list[i], for_info.back().hi, for_info); // unroll all bindings up to this range
	}
	else if ((hp->child_list[i]->h_op == hNode::hdlopsEnum::hForStmt) ||
		 (hp->child_list[i]->h_op == hNode::hdlopsEnum::hCStmt)	)
	  HDLLoop(hp->child_list[i], for_info);
      }
    }
    else if (isInitPB(hp)) {
      UnrollBinding(hp, 1, for_info);
    }
    else if (hp->h_op == hNode::hdlopsEnum::hCStmt) {
      for (hNodep hpc:hp->child_list) {
	HDLLoop(hpc, for_info);
      }
    }
  }
  
  
  hNodep HDLConstructorHcode::ProcessCXXConstructorHcode(hNodep xconstructor) {
    std::vector<for_info_t> for_info; 

    if (xconstructor==nullptr) return xconstructor;
    RemoveSCMethod(xconstructor);
    hnewpb = new hNode(xconstructor->h_name, hNode::hdlopsEnum::hPortbindings);
    for (hNodep hp : xconstructor->child_list)
      HDLLoop(hp, for_info);
    xconstructor->child_list.push_back(hnewpb);
    return xconstructor;
  }

}
