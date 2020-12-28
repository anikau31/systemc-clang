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

    for_info_t tmp{"FORNAME", 0, 1, 1, 0};
    
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

  void HDLConstructorHcode::SubstituteIndex(hNodep &hp, std::vector<for_info_t> &for_info) {
    for (int i=0; i < for_info.size(); i++) {
      if ((hp->h_op == hNode::hdlopsEnum::hVarref) && (hp->h_name == for_info[i].name)) {
	hp->h_name = to_string(for_info[i].curix);
	hp->h_op = hNode::hdlopsEnum::hLiteral;
	break;
      }  
    }
    for (hNodep hpi : hp->child_list)
      SubstituteIndex(hpi, for_info);
  }

  hNodep HDLConstructorHcode::HnodeDeepCopy(hNodep hp) {
    hNodep h_ret = new hNode(hp->h_name, hp->h_op);
    for (hNodep hchild : hp->child_list) {
      h_ret->child_list.push_back(HnodeDeepCopy(hchild));
    }
    return h_ret;
  }
  
  // Generate a port binding
    // need to dismantle modname##field:
    // modname is the h_name for the portbinding
    // and field is the varref, e.g.
    // Case 0: hBinop () [
          // hVarref u_fwd_cast##clk NOLIST
          // hVarref clk NOLIST
    // but if the submod instance/port is ARRAYSUBSCRIPT in loop,
    // can have (if submodule is not an array of submods)
    // Case 1: hBinop () [
    //         hBinop ARRAYSUBSCRIPT [
    //           hVarref u_fwd_cast##m_block NOLIST
    //           hVarref _local_0 NOLIST
    //         ]
    //         hBinop ARRAYSUBSCRIPT [
    //           hVarref c_fc_block NOLIST
    //           hVarref _local_0 NOLIST
    //         ]
    //       ]
    //     ]

    // or it will look like this if submod is array ( u_yt[_local_0].clk:clk )
    // Case 2: hBinop () [
    // hVarref clk [
    //     hBinop ARRAYSUBSCRIPT [
    //       hVarref u_yt NOLIST
    //       hVarref _local_0 NOLIST
    //     ]
    //   ]
    //   hVarref clk NOLIST
    // ]
    // or for module instance array ref and port array ref
    // 
    // Case 3: hBinop () [
    // hBinop ARRAYSUBSCRIPT [
    //     hVarref m_port [
    //       hBinop ARRAYSUBSCRIPT [
    //         hVarref u_xt NOLIST
    //         hVarref _local_0 NOLIST
    //       ]
    //     ]
    //     hVarref _local_1 NOLIST
    //   ]
    //   hBinop ARRAYSUBSCRIPT [
    //     hBinop ARRAYSUBSCRIPT [
    //       hVarref c_xt_data NOLIST
    //       hVarref _local_0 NOLIST
    //     ]
    //     hVarref _local_1 NOLIST
    //   ]
    // ]
  void HDLConstructorHcode::UnrollBinding(hNodep &hp_orig, std::vector<for_info_t> &for_info) {

    static const string fielddelim{"##"};
    static const string tokendelim{"_"};
    static const string pbstring{"()"};
    static const string arrsub{"ARRAYSUBSCRIPT"};
    
    assert ((hp_orig->h_op == hNode::hdlopsEnum::hBinop) && (hp_orig->h_name == pbstring));

    // Case 0
    if (for_info.size() == 0 ) { // simple case, not in a for loop
      string submodport = hp_orig->child_list[0]->h_name;
      string thismodsig = hp_orig->child_list[1]->h_name;
      // part before delimiter is submodule name, after delimiter is port name
      hNodep hpb = new hNode(submodport.substr(0, submodport.find(fielddelim)),
			     hNode::hdlopsEnum::hPortbinding);
      hpb->child_list.push_back(new hNode(submodport.substr(submodport.find(fielddelim)+fielddelim.size()),
					  hNode::hdlopsEnum::hVarref));
      hpb->child_list.push_back(new hNode(thismodsig, hNode::hdlopsEnum::hVarref));
      hnewpb->child_list.push_back(hpb);
      return;
    }

    hNodep hp = HnodeDeepCopy(hp_orig);  // will be modifying subtrees, so make a copy
    
    hNodep hsubmodport = hp->child_list[0];  // submoduleport being bound
    hNodep hthismodsig = hp->child_list[1];

    // need to duplicate parts of the binding tree that are arraysubscripts
    // those nodes will have the loop variable replaced by current index.
    
    string submodport{"XXX"}, thismodsig{"YYY"};
    string submod{"SUBMOD"};
    
    // Case 2
    if ((hsubmodport->h_op ==  hNode::hdlopsEnum::hVarref) && (hsubmodport->child_list.size() > 0)) {
      
    // in a for loop, unroll the port bindings
     // hVarref clk [
    //     hBinop ARRAYSUBSCRIPT [
    //       hVarref u_yt NOLIST
    //       hVarref _local_0 NOLIST // changed to hLiteral by Substitute index
    //     ]
    //   ]
    //   hVarref clk NOLIST
    // ]
      hNodep hportchild = hsubmodport->child_list[0];
      while ((hportchild != nullptr) && (hportchild->h_name == arrsub)) {
	if ((hportchild->child_list[0]->h_op == hNode::hdlopsEnum::hVarref) &&
	    (hportchild->child_list[0]->child_list.size() == 0)) { // simple varref
	  submod = hportchild->child_list[0]->h_name;  // will add instance suffix in loop
	  break;
	}
	hportchild = hportchild->child_list[0];
      }
    }
    else if (hsubmodport->h_name ==  arrsub) { // check Case 1, 3
      hNodep hportchild = hsubmodport->child_list[0];
      hNodep hparent = hportchild;
      
      while ((hportchild != nullptr) && (hportchild->h_name == arrsub)) {
	hparent = hportchild;
	hportchild = hportchild->child_list[0];
      }
      if ((hportchild != nullptr) && (hportchild->h_op == hNode::hdlopsEnum::hVarref) &&
	  (hportchild->child_list.size() == 0)) {
	submod = hportchild->h_name;  
	size_t found = submod.find(fielddelim);
	if ( found != std::string::npos) { // module name prefix, not a vector of modules
	  hportchild->h_name = submod.substr(found+fielddelim.size());
	  submod = submod.substr(0, found);
	}
	else { // need to handle Case 3 by removing the (arraysubscript submod ix) node
	  hNodep hsubmodixname = hparent->child_list[1];
	  string ixname = hsubmodixname->h_name;
	  for (int i = 0; i < for_info.size(); i++) {
	    if (for_info[i].name == ixname) {
	      submod+=ixname+to_string(for_info[i].curix);
	      break;
	    }
	  }
	} 
      }	
    }
    
    hNodep hpb = new hNode( submod+tokendelim, hNode::hdlopsEnum::hPortbinding);
    //hpb->child_list.push_back(new hNode(submodport+tokendelim+to_string(i), hNode::hdlopsEnum::hVarref));
    //hpb->child_list.push_back(new hNode(thismodsig+tokendelim+to_string(i), hNode::hdlopsEnum::hVarref));

    hpb->child_list.push_back(hsubmodport);
    hpb->child_list.push_back(hthismodsig);
    SubstituteIndex(hpb, for_info);
    hnewpb->child_list.push_back(hpb);
  }
  
  void HDLConstructorHcode::HDLLoop(hNodep &hp, std::vector<for_info_t> &for_info ) {
    if ((hp->h_op == hNode::hdlopsEnum::hForStmt) && (hp->child_list.size() > 3)) {
      PushRange(hp, for_info); // fill in name, lo, hi, step
      for (int forloopix = for_info.back().lo; forloopix < for_info.back().hi; forloopix+=for_info.back().step) {
	for_info.back().curix = forloopix;
	for (int i=3; i<hp->child_list.size(); i++) {
	  if (isInitPB(hp->child_list[i])) {// hcode indicating port binding
	    UnrollBinding(hp->child_list[i], for_info); // unroll all bindings up to this range
	  }
	  else if ((hp->child_list[i]->h_op == hNode::hdlopsEnum::hForStmt) ||
		   (hp->child_list[i]->h_op == hNode::hdlopsEnum::hCStmt)	)
	    HDLLoop(hp->child_list[i], for_info);
	}
      }
      for_info.pop_back();
    }
    else if (isInitPB(hp)) {
      UnrollBinding(hp, for_info);
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
