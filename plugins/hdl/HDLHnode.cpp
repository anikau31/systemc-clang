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
//!
namespace systemc_hdl {

  bool is_numeric(string &s) {
    for (char c : s) {
      if (!isdigit(c)) return false;
    }
    return true;
  }
  void HDLConstructorHcode::RemoveSCMethod(hNodep &hp) {
 
    hp->child_list.erase( std::remove_if( hp->child_list.begin(), hp->child_list.end(), [] (hNodep x) {
	  return (
		  //((x->h_op==hNode::hdlopsEnum::hVarAssign) &&
		  //(x->child_list.size()==2) &&
		  //(x->child_list.back()->h_op != hNode::hdlopsEnum::hLiteral)) ||
		  (x->h_op == hNode::hdlopsEnum::hVardecl) || // index variables
		  //(x->h_op == hNode::hdlopsEnum::hMethodCall) || // sc_method
		  (x->h_op == hNode::hdlopsEnum::hReturnStmt) || // remove return stmt in init block
		  (x->h_op == hNode::hdlopsEnum::hUnimpl));}), hp->child_list.end() );

    for (hNodep hpi :hp->child_list)
      RemoveSCMethod(hpi);      
  }

  void HDLConstructorHcode::CleanupInitHcode(hNodep &hp) {
    hp->child_list.erase( std::remove_if( hp->child_list.begin(), hp->child_list.end(), [] (hNodep x) {
	  return (((x->h_op==hNode::hdlopsEnum::hBinop) &&
		   (x->h_name==pbstring) || (x->h_name==sensop)) ||
		  ((x->h_op == hNode::hdlopsEnum::hSensvar) && // gratuitous sim method sens vars
		   (x->child_list[0]->h_name.find(localstr) != std::string::npos)) ||
		  (x->h_op==hNode::hdlopsEnum::hForStmt) ||
		  (x->h_op == hNode::hdlopsEnum::hVardeclrn)  || // renamed index variables
		  ((x->h_op==hNode::hdlopsEnum::hCStmt) &&
		   (x->child_list.empty())) ||
		  (x->h_op==hNode::hdlopsEnum::hVarAssign) ||
		  ((x->h_op==hNode::hdlopsEnum::hMethodCall) && (x->h_name.find(strsccore) !=std::string::npos)) ||
		  ((x->h_op == hNode::hdlopsEnum::hNoop) &&
		   (x->h_name==arrsub)));}), hp->child_list.end());
    for (hNodep hpi :hp->child_list)
      CleanupInitHcode(hpi);   
  }
    
  //!
  //! for loop range for port bindings is expecting 3 simple range arguments. They must be
  //! numeric constants. If not, defaults are used.
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
      //FIXME -- put in error message if not a numeric constant
      if (is_numeric(hlo->child_list[1]->h_name))
	tmp.lo = stoi(hlo->child_list[1]->h_name);
    }
    if ((hi->h_op == hNode::hdlopsEnum::hBinop) && 
	(hlo->child_list.size() == 2) &&
	(hlo->child_list[0]-> h_op == hNode::hdlopsEnum::hVarref) &&
	(hlo->child_list[1]->h_op ==  hNode::hdlopsEnum::hLiteral)) {
      // check that names are same ... tmp.name = hlo->child_list[0]->h_name;
      //FIXME -- put in error message if not a numeric constant
      if (is_numeric(hi->child_list[1]->h_name))
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
    
    assert ((hp_orig->h_op == hNode::hdlopsEnum::hBinop) && (hp_orig->h_name == pbstring));

    // Case 0
    if (for_info.empty()) { // simple case, not in a for loop
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
      hNodep hparent = hsubmodport;
      while ((hportchild != nullptr) && (hportchild->h_name == arrsub)) {
	if ((hportchild->child_list[0]->h_op == hNode::hdlopsEnum::hVarref) &&
	    (hportchild->child_list[0]->child_list.empty())) { // simple varref
	  submod = hportchild->child_list[0]->h_name;  
	  break;
	}
	hparent = hportchild;
	hportchild = hportchild->child_list[0];
      }
      hNodep hsubmodixname = hportchild->child_list[1];
      string ixname = hsubmodixname->h_name;
      for (int i = 0; i < for_info.size(); i++) {
	if (for_info[i].name == ixname) {
	  submod+=tokendelim+to_string(for_info[i].curix);
	  break;
	}
      }
      hparent->child_list.pop_back();
      delete hportchild;
    }
    else if (hsubmodport->h_name ==  arrsub) { // check Case 1, 3
      hNodep hportchild = hsubmodport->child_list[0];
      hNodep hparent = hsubmodport;
      
      while ((hportchild != nullptr) &&
	     ((hportchild->h_name == arrsub) ||
	      ((hportchild->h_op == hNode::hdlopsEnum::hVarref) &&
	       (hportchild->child_list.size() > 0)))) {
	hparent = hportchild;
	hportchild = hportchild->child_list[0];
      }
      if ((hportchild != nullptr) && (hportchild->h_op == hNode::hdlopsEnum::hVarref)) {
	if (hportchild->child_list.empty()) { // Case 1
	  submod = hportchild->h_name;  
	  size_t found = submod.find(fielddelim);
	  if ( found != std::string::npos) { // module name prefix, not a vector of modules
	    hportchild->h_name = submod.substr(found+fielddelim.size());
	    submod = submod.substr(0, found);
	  }
	  else { // Varref has child; need to handle Case 3 by removing the (arraysubscript submod ix) node
	    hNodep hsubmodixname = hparent->child_list[1];
	    string ixname = hsubmodixname->h_name;
	    for (int i = 0; i < for_info.size(); i++) {
	      if (for_info[i].name == ixname) {
		submod+=tokendelim+to_string(for_info[i].curix);
		break;
	      }
	    }
	    hparent->h_op = hNode::hdlopsEnum::hNoop; // get rid of mod instance reference
	    hparent->child_list.clear();
	  }
	} 
      }
    }
    
    hNodep hpb = new hNode( submod, hNode::hdlopsEnum::hPortbinding);
    //hpb->child_list.push_back(new hNode(submodport+tokendelim+to_string(i), hNode::hdlopsEnum::hVarref));
    //hpb->child_list.push_back(new hNode(thismodsig+tokendelim+to_string(i), hNode::hdlopsEnum::hVarref));

    hpb->child_list.push_back(hsubmodport);
    hpb->child_list.push_back(hthismodsig);
    SubstituteIndex(hpb, for_info);
    hnewpb->child_list.push_back(hpb);
  }
  bool HDLConstructorHcode::SetupSenslist(hNodep hp) {

    // hMethodCall sc_coresc_simcontextcreate_method_process:create_method_process [
    //         hMethodCall sc_coresc_get_curr_simcontext:sc_get_curr_simcontext NOLIST
    //         hUnimpl StringLiteral NOLIST
    //         hLiteral 0 NOLIST
    //         hUnop & [
    //           hMethodCall zhwencode_blockfp_t11_52_2mc_proc:mc_proc NOLIST
    //         ]
    //         hUnimpl CXXThisExpr NOLIST
    //         hLiteral 0 NOLIST
    //       ]

    if (hp->h_op == hNode::hdlopsEnum::hMethodCall) {
      for (hNodep hpi: hp->child_list) {
	if (hpi->h_op == hNode::hdlopsEnum::hUnop) {
	  std::size_t found = (hpi->child_list[0]->h_name).find(qualnamedelim);
	  if ( found != std::string::npos) { // should be SC_METHOD name
	    hnewsens.push_back(new hNode(hpi->child_list[0]->h_name.substr(found+1),
				       hNode::hdlopsEnum::hSenslist));
	    return true; // got the name
	  }
	  else { // couldn't find the ":"
	    hnewsens.push_back(new hNode(hpi->child_list[0]->h_name, hNode::hdlopsEnum::hSenslist));
	    return true;
	  }
	}
      }
    }
    return false;
  }
    
  void HDLConstructorHcode::UnrollSensitem(hNodep &hp_orig, std::vector<for_info_t> &for_info) {
    // hBinop << [
    //      hVarref sensitive NOLIST
    //      hNoop pos [
    //        hVarref clk NOLIST
    //      ]
    //    ]

    // check for list of sens items
    if (isInitSensitem(hp_orig->child_list[0])) {
      UnrollSensitem(hp_orig->child_list[0], for_info);
      }

    // at a primitive sens item
    hNodep hp = HnodeDeepCopy(hp_orig); // need to keep the subtrees when the original tree gets released

    hp->h_op = hNode::hdlopsEnum::hSensvar;
    hp->h_name = noname;
    
    delete hp->child_list[0]; // release that hnode
    hp->child_list.erase(hp->child_list.begin()); // remove the first item
    if (!for_info.empty()) {
      SubstituteIndex(hp, for_info);
    }

    // check for edge sensitivity
    // eg
    //    hSensvar pos [  <-- hp
    //      hNoop pos [   <-- hedge
    //        hVarref clk NOLIST <-- hedge->child_list[0]
    //      ]
    //    ]

    if (isEdge(hp->child_list[0]->h_name)) {
      hNodep hedge = hp->child_list[0];
      hp->child_list[0] = hedge->child_list[0];
      hedge->child_list.pop_back();
      hp->child_list.push_back(hedge);
    }
    else {
      
      if (isSimEvent(hp->child_list[0]->h_name)) {

      // hSensvar NONAME [
      //     hNoop value_changed_event [
      //       hVarref c_fp##ready NOLIST
      //     ]
      //     hNoop always NOLIST
      //   ]
      
      hNodep htmp =  hp->child_list[0]->child_list[0];
      hp->child_list.erase(hp->child_list.begin());
      hp->child_list.push_back(htmp);
      }
   
      hp->child_list.push_back(new hNode("always", hNode::hdlopsEnum::hNoop));
    };
    
    hnewsens.back()->child_list.push_back(hp);
  }
  
  void HDLConstructorHcode::HDLLoop(hNodep &hp, std::vector<for_info_t> &for_info ) {
    // check in order of expected frequency
    if (isInitPB(hp)) {
      UnrollBinding(hp, for_info);
    }
    else if (isInitSensitem(hp)) {
      UnrollSensitem(hp, for_info);
    }
    else if ((hp->h_op == hNode::hdlopsEnum::hForStmt) && (hp->child_list.size() > 3)) {
      PushRange(hp, for_info); // fill in name, lo, hi, step
      for (int forloopix = for_info.back().lo; forloopix < for_info.back().hi; forloopix+=for_info.back().step) {
	for_info.back().curix = forloopix;
	for (int i=3; i<hp->child_list.size(); i++) {
	  if (isInitPB(hp->child_list[i])) {// hcode indicating port binding
	    UnrollBinding(hp->child_list[i], for_info); // unroll all bindings in this range
	  }
	  else if (isInitSensitem(hp->child_list[i])) { // hcode indicating sensitivity item
	    UnrollSensitem(hp->child_list[i], for_info); // unroll all sensitems in this range
	  }
	  else if ((hp->child_list[i]->h_op == hNode::hdlopsEnum::hForStmt) ||
		   (hp->child_list[i]->h_op == hNode::hdlopsEnum::hCStmt))
	    HDLLoop(hp->child_list[i], for_info);
	}
      }
      for_info.pop_back();
    }
    else if (hp->h_op == hNode::hdlopsEnum::hCStmt) {
      for (hNodep hpc:hp->child_list) {
	HDLLoop(hpc, for_info);
      }
    }
    else if (isMethodCall(hp)) { // hVarAssign child[1] is a method call;
      if (!SetupSenslist(hp->child_list[1])) { // points to first hMethodCall, push SC_METHOD name onto hnewsens
	// oops couldn't parse it
	hnewsens.push_back(new hNode( "METHOD ???", hNode::hdlopsEnum::hSenslist));
      }
    }

  }
  
  
  hNodep HDLConstructorHcode::ProcessCXXConstructorHcode(hNodep xconstructor) {
    std::vector<for_info_t> for_info; 

    if (xconstructor==nullptr) return xconstructor;

    // this is a workaround to make lldb find dumphcode
    // since lldb doesn't pick up default parameters in print
    // and doesn't recognize llvm::outs()
    { int junk =2; if (junk!=2) xconstructor->dumphcode();}
    
    RemoveSCMethod(xconstructor);
    hnewpb = new hNode(xconstructor->h_name, hNode::hdlopsEnum::hPortbindings);
    // FIXME name should be the SC_METHOD name
    //hnewsens = new hNode(xconstructor->h_name, hNode::hdlopsEnum::hSenslist); 
    for (hNodep hp : xconstructor->child_list)
      HDLLoop(hp, for_info);
    if (!hnewpb->child_list.empty()) {
      xconstructor->child_list.push_back(hnewpb);
    }
    if (!hnewsens.empty()) {
      for (hNodep onesens: hnewsens) {
	if (!onesens->child_list.empty()) {
	  xconstructor->child_list.push_back(onesens);
	}
      }
    }
    CleanupInitHcode(xconstructor);
    return xconstructor;
  }

}
