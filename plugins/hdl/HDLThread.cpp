// clang-format off
#include "HDLThread.h"
#include "SplitCFGBlock.h"
//clang-format on

/// Different matchers may use different DEBUG_TYPE
#undef DEBUG_TYPE
#define DEBUG_TYPE "HDL"

//!
//! Entry point for SC_THREAD  generation
//!
//! Starting with outer compound statement,
//! generate CFG and then 
//! generate hcode for basic block,
//! using HDLBody class
//! 


using namespace systemc_clang;

namespace systemc_hdl {

  HDLThread::HDLThread(EntryFunctionContainer *efc, hNodep &h_top, hNodep &h_portsigvarlist,
		       clang::DiagnosticsEngine &diag_engine,
		       const ASTContext &ast_context, hdecl_name_map_t &mod_vname_map,
		       hfunc_name_map_t &allmethodecls, overridden_method_map_t &overridden_method_map, hNodep h_resetvarinfo )
    : efc_(efc), h_top_{h_top}, diag_e{diag_engine}, ast_context_{ast_context},
      mod_vname_map_{mod_vname_map}, allmethodecls_{allmethodecls}, overridden_method_map_{overridden_method_map}, 
      h_resetvarinfo_{h_resetvarinfo}, scfg{const_cast<ASTContext &>(ast_context), efc->getEntryMethod()} {

      LLVM_DEBUG(llvm::dbgs() << "Entering HDLThread constructor (thread body)\n");
      
      CXXMethodDecl *emd = efc->getEntryMethod();
      
      h_ret = NULL;
      needwaitswitchcase = false;

      string threadname = h_top->getname();
      state_string ="state_"+ threadname+statestringsymbol;
      nextstate_string =NameNext(state_string);
      waitctr_string = "wait_counter_"+ threadname+statestringsymbol;
      nextwaitctr_string = NameNext(waitctr_string);
      waitnextstate_string = "wait_next_state_"+ threadname+statestringsymbol;
      savewaitnextstate_string = NameNext(waitnextstate_string);
      
      thread_vname_map.insertall(mod_vname_map_);
      thread_vname_map.reset_referenced();
      
      //xtbodyp = new HDLBody(diag_e, ast_context_, mod_vname_map_);
      xtbodyp = new HDLBody(diag_e, ast_context_, thread_vname_map, allmethodecls_, overridden_method_map);
      hNodep hthreadmainmethod = new hNode(h_top->getname(), hNode::hdlopsEnum::hMethod);
      hthreadblocksp = new hNode(hNode::hdlopsEnum::hSwitchStmt); // body is switch, each path is case alternative
      hthreadblocksp->append(new hNode(state_string, hNode::hdlopsEnum::hVarref));
      hlocalvarsp = new hNode(hNode::hdlopsEnum::hPortsigvarlist); // placeholder to collect local vars
      
      hNodep hthreadblockcstmt = new hNode(hNode::hdlopsEnum::hCStmt);
      hthreadblocksp->append(hthreadblockcstmt);
      // build SC CFG

      //SplitCFG scfg{const_cast<ASTContext &>(ast_context_), emd};
      scfg.generate_paths();
      LLVM_DEBUG(llvm::dbgs() << "Dumping scfg paths.\n");
      LLVM_DEBUG(scfg.dump());
      LLVM_DEBUG(scfg.dumpToDot());
      
      const llvm::SmallVectorImpl<llvm::SmallVector<SplitCFG::SplitCFGPathPair>> &paths_found{ scfg.getPathsFound()};

      numstates = paths_found.size();
      int state_num;

      for (state_num = 0; state_num < paths_found.size(); state_num++) {
	SGVisited.clear();
	// pathnodevisited keeps track of nodes already traversed in true and false paths.
	// those were already done by ProcessSplitGraphGroup and should be skipped
	pathnodevisited.clear();
	hNodep h_switchcase = new hNode( hNode::hdlopsEnum::hSwitchCase);
	h_switchcase->append(new hNode(std::to_string(state_num), hNode::hdlopsEnum::hLiteral));
	ProcessSplitGraphGroup(paths_found[state_num], 0,
			       paths_found[state_num].size(), 
			       state_num, h_switchcase);
	hthreadblockcstmt->append(h_switchcase);

      }

      if (needwaitswitchcase) {
	hNodep h_switchcase = new hNode( hNode::hdlopsEnum::hSwitchCase);
	h_switchcase->append(new hNode(std::to_string(numstates), hNode::hdlopsEnum::hLiteral));
	GenerateWaitCntUpdate(h_switchcase);
	hthreadblockcstmt->append(h_switchcase);
      }

      // for all variables (and signals) referenced in thread,
      // we need two copies to hold the variables' value across clock cycles.
      // these variables may be local or global
      hNodep h_shadowvarsp = new hNode(hNode::hdlopsEnum::hPortsigvarlist); // collect referenced variables 

      for (auto const &var: thread_vname_map) {
	if (var.second.referenced) {
	  // create a copy, same child_list
	  hNodep hs = new hNode(var.second.h_vardeclp->getname(), var.second.h_vardeclp->getopc());
	  hs->child_list = var.second.h_vardeclp->child_list;
	  h_shadowvarsp->append(hs);
	  if (var.second.newn.find(hnode::gvar_prefix) != string::npos) // global object
	    string gname = mod_vname_map.find_entry_newn(var.first, true); // set referenced bit in global name map
	}
      }
      
      //std::unique_ptr< CFG > threadcfg = clang::CFG::buildCFG(emd, emd->getBody(), &(emd->getASTContext()), clang::CFG::BuildOptions());
      //clang::LangOptions LO = ast_context.getLangOpts();
      //threadcfg->dump(LO, false);
      // HDLBody instance init
      // for (auto const& pt: paths_found) {
      // 	for (auto const& block : pt) {
      // 	  ProcessBB(*(block->getCFGBlock()));
      // 	}
      // }

      // top of main method
      hthreadmainmethod->append(GenerateBinop("=", nextstate_string, state_string, false));
      hthreadmainmethod->append(GenerateBinop("=", nextwaitctr_string, waitctr_string, false));
      hthreadmainmethod->append(GenerateBinop("=", savewaitnextstate_string, waitnextstate_string, false));
      for (hNodep onelocalvar : h_shadowvarsp->child_list) {
	 hthreadmainmethod->append(GenerateBinop("=", onelocalvar->getname(), shadowstring+onelocalvar->getname())); 
      }
      hthreadmainmethod->append(new hNode(threadname+"_func", hNode::hdlopsEnum::hMethodCall));
      //hthreadmainmethod->append(hthreadblocksp);

      // generate hnode tree for a function:
      // <function> <ret type none> <cstmt containing switch stmt>
      
      hNodep hfunctop = new hNode(threadname+"_func", hNode::hdlopsEnum::hFunction);
      hfunctop->append(new hNode(hNode::hdlopsEnum::hFunctionRetType)); // placeholder: no return value
      hNodep hcstmttmp = new hNode(hNode::hdlopsEnum::hCStmt);
      hcstmttmp->append(hthreadblocksp);
      hfunctop->append(hcstmttmp);
      //hfunctop->append(hthreadblocksp);
      //h_top->append(hthreadblocksp);
      h_top->append(hfunctop);
      
      // generate the local variables;
      GenerateStateVar(state_string);
      GenerateStateVar(NameNext(state_string));
      GenerateStateVar(waitctr_string);
      GenerateStateVar(NameNext(waitctr_string));
      GenerateStateVar(waitnextstate_string);
      GenerateStateVar(NameNext(waitnextstate_string));
      // generate the state update method
      hNodep hstatemethod = new hNode(threadname+"_state_update", hNode::hdlopsEnum::hMethod);
      GenerateStateUpdate(hstatemethod, h_shadowvarsp);
      h_top->append(hstatemethod);
      
      h_top->append(hthreadmainmethod);

      // remove globals from shadowvars
      h_shadowvarsp->child_list.erase(
	 std::remove_if( h_shadowvarsp->child_list.begin(),
			 h_shadowvarsp->child_list.end(), [] (hNodep x) {
			   return (x->getname().find(hnode::gvar_prefix) != string::npos);}),
	 h_shadowvarsp->child_list.end() );
								 
      // fix up names of local var shadows promoted
      for (hNodep hchild: h_shadowvarsp->child_list) {
	hchild->set(shadowstring+hchild->getname());
      }
      
      if (h_shadowvarsp->size()>0) {
	hlocalvarsp->child_list.insert(std::end(hlocalvarsp->child_list),
				       std::begin(h_shadowvarsp->child_list),
				       std::end(h_shadowvarsp->child_list));
      }
      // add thread local variables to module level list

      if (hlocalvarsp->size()!=0) h_portsigvarlist->child_list.insert(std::end(h_portsigvarlist->child_list),
								      std::begin(hlocalvarsp->child_list),
								      std::end(hlocalvarsp->child_list));
      //h_top->append(hthreadblocksp);
      
    }

  HDLThread::~HDLThread() {
    LLVM_DEBUG(llvm::dbgs() << "[[ Destructor HDLThread ]]\n");
  }

  bool HDLThread::IsWaitStmt(hNodep hp) {
    return ((hp->child_list.size() >=1) and ((hp->child_list.back())->getopc() == hNode::hdlopsEnum::hWait));
  }

  void HDLThread::CheckVardecls(hNodep &hp, string &blockid) { 
    int varcnt = 0;
    for (auto oneop : hp->child_list) {
      if ((oneop != NULL) && ((oneop->getopc() == hNode::hdlopsEnum::hVardecl) || (oneop->getopc() == hNode::hdlopsEnum::hSigdecl))) {
	LLVM_DEBUG(llvm::dbgs() << "Detected vardecl for SG Block ID " << blockid << "\n");
	if (SGVisited[blockid] == 1) { 	  hlocalvarsp->append(oneop);
	}
	else {
	  LLVM_DEBUG(llvm::dbgs() << "SGVisited for blockid " << SGVisited[blockid] 
		     << " " << blockid << "\n");
	}
	varcnt += 1;
      }
      else break; // all vardecls are first in the list of ops
    }
    if (varcnt >=1) {
      hp->child_list.erase(hp->child_list.begin(), hp->child_list.begin()+varcnt);
      if (SGVisited[blockid] == 1) { 
	thread_vname_map.insertall(xtbodyp->vname_map);
      }
    }
  }

  void HDLThread::ProcessDeclStmt(const DeclStmt *declstmt, hNodep htmp) {
    //!
    //! called when a declstmt is instantiated more than once
    //! can skip the decl, but need to process initializer
    //!
    
    // adapted from ProcessVarDecl in HDLBody.cpp
    for (auto *DI : declstmt->decls()) {
      if (DI) {
	auto *vardecl = dyn_cast<VarDecl>(DI);
	if (!vardecl) continue;
	if ( Expr *declinit = vardecl->getInit()) {
	  Stmt * cdeclinit = declinit;
	  //  need to generated initializer code
	  //xtbodyp->Run(const_cast<Stmt *>((const Stmt *)declinit), hinitcode, rthread);
	  hNodep varinitp = new hNode(hNode::hdlopsEnum::hVarAssign);
	  varinitp->append(new hNode(thread_vname_map.find_entry_newn(vardecl, true),
				     hNode::hdlopsEnum::hVarref)); // set referenced bit
	  xtbodyp->Run(cdeclinit, varinitp, rthread);
	  htmp->append(varinitp);
	  
	}
      }
    }
  }
	  
  void HDLThread::MarkStatements(const Stmt *S, llvm::SmallDenseMap<const Stmt*, bool> &Map) {
    if (S != NULL) {
      Map[S] = true;
      for (const Stmt *K : S->children())
	MarkStatements(K, Map);
    }
  }

  // this version is no longer being used
  void HDLThread::FindStatements(const CFGBlock &B, std::vector<const Stmt *> &SS) {
    llvm::SmallDenseMap<const Stmt*, bool> Map;

    // Mark subexpressions of each element in the block.
    for (auto I = B.begin(); I != B.end(); ++I) {
      CFGElement E = *I;
      if (auto SE = E.getAs<CFGStmt>()) {
	const Stmt *S = SE->getStmt();
	for (const Stmt *K : S->children()) 
	  MarkStatements(K, Map);
      }
    }
    // mark subexpressions coming from terminator statement
    if (B.getTerminator().isValid()) {
      const Stmt *S = B.getTerminatorStmt();
      for (const Stmt *K : S->children())
	MarkStatements(K, Map);
    }
    // Any expressions not in Map are top level statements.
    for (auto I = B.begin(); I != B.end(); ++I) {
      CFGElement E = *I;
      if (auto SE = E.getAs<CFGStmt>()) {
	const Stmt *S = SE->getStmt();
	if (Map.find(S) == Map.end()) {
	  SS.push_back(S);
	}
      }
    }
  }

  void HDLThread::FindStatements(const SplitCFGBlock *B, std::vector<const Stmt *> &SS) {
    llvm::SmallDenseMap<const Stmt*, bool> Map;

    // Mark subexpressions of each element in the block.
    for (auto I : B->getElements()) {
      CFGElement E = *I;
      if (auto SE = E.getAs<CFGStmt>()) {
	const Stmt *S = SE->getStmt();
	for (const Stmt *K : S->children()) 
	  MarkStatements(K, Map);
      }
    }
    // // mark subexpressions coming from terminator statement
    if ((B->getCFGBlock())->getTerminator().isValid()) {
      const Stmt *S = (B->getCFGBlock())->getTerminatorStmt();
      for (const Stmt *K : S->children()) {
     	MarkStatements(K, Map);
      }
      //if (auto S1 = dyn_cast<WhileStmt>(S) 
      // if (Map.find(S) == Map.end()) {
      // 	  SS.push_back(S);
      // }
      LLVM_DEBUG(llvm::dbgs() << "Stmt contains terminator\n");
      //LLVM_DEBUG(S->dump(llvm::dbgs(), ast_context_));
    }
    
    // Any expressions not in Map are top level statements.
    for (auto I : B->getElements()) {
      CFGElement E = *I;
      if (auto SE = E.getAs<CFGStmt>()) {
	const Stmt *S = SE->getStmt();
	if (Map.find(S) == Map.end()) {
	  SS.push_back(S);
	}
      }
    }
  }

  int HDLThread::GetFalseLength(const SplitCFG::SplitCFGPath &pt, int cond_node_ix) {
    auto path_info_{scfg.getPathInfo()};
    auto sblock{pt[cond_node_ix].first};
    auto supp_info{pt[cond_node_ix].second};
    auto found_it{path_info_.find(supp_info.split_block_)};
    int flen = 0;
    LLVM_DEBUG(llvm::dbgs() << "Getting false path length of ");
    LLVM_DEBUG(llvm::dbgs() << "(" << supp_info.path_idx_ << "," << sblock->getBlockID()
	       << "," << supp_info.false_idx_);
    if (found_it != path_info_.end()) {
      flen = found_it->second.getFalsePath().size();
    }
    LLVM_DEBUG(llvm::dbgs() << " |" << flen << "|");
    LLVM_DEBUG(llvm::dbgs() << ")\n");
    return flen;
  }
    

  void HDLThread::ProcessSplitGraphGroup(const SplitCFG::SplitCFGPath pt,
    //  SplitCFGPath is llvm::SmallVector<std::pair<const SplitCFGBlock*, SplitCFGPathInfo>>
					 int startix, int num_ele,
					 int state_num, hNodep h_switchcase)
  {

    LLVM_DEBUG(llvm::dbgs() << "Split Graph Group startix, num_ele, state_num are " << startix << " " << num_ele << " " <<  state_num << "\n");

    int pvix = startix;
    while ( pvix<startix+num_ele) {
      if (pathnodevisited.find(pvix) == pathnodevisited.end()) { //haven't visited
	pathnodevisited.insert(pvix);
	ProcessSplitGraphBlock(pt, pvix, state_num, h_switchcase); 
	pvix++;
      }
      else pvix++;
    }
  }

  //  SplitCFGPath = llvm::SmallVector<SplitCFGPathPair>;
  //  SplitCFGPathPair = std::pair<const SplitCFGBlock *, SupplementaryInfo>;
  void HDLThread::ProcessSplitGraphBlock(const SplitCFG::SplitCFGPath &pt,
					 int thisix,
					 int state_num, hNodep h_switchcase)
  {
    const SplitCFGBlock *sgb{pt[thisix].first};
    bool iswait = false;
    if (sgb != NULL) {
      string blkid = "S" + std::to_string(state_num) + "_" + std::to_string(sgb->getBlockID());

      if (SGVisited.find(blkid) == SGVisited.end()) {
      	SGVisited[blkid] = 1;
      }
      else {
	SGVisited[blkid] += 1;
      }
      //else return; // already visited this block
      
      LLVM_DEBUG(llvm::dbgs() << "Split Graph num ele, blockid are " << sgb->getNumOfElements() << " " << blkid << "\n");

      if (((sgb->getCFGBlock())->getTerminator().isValid()) && !isBreak(sgb->getCFGBlock()->getTerminatorStmt())){
	hNodep hcondstmt = new hNode(hNode::hdlopsEnum::hIfStmt);
	const Stmt * S = sgb->getCFGBlock()->getTerminatorStmt();

	if (const WhileStmt *S1 = dyn_cast<WhileStmt> (S)) {
	  LLVM_DEBUG(llvm::dbgs() << "Terminator for block " <<blkid << " is a while stmt\n");
	  xtbodyp->Run((Stmt *)S1->getCond(), hcondstmt, rthread);
	}
	else if (const ForStmt *S1 = dyn_cast<ForStmt> (S)) {
	  LLVM_DEBUG(llvm::dbgs() << "Terminator for block " << blkid << " is a for stmt\n");
	  // if (!sgb->hasTerminatorWait()) {
	  //   LLVM_DEBUG(llvm::dbgs() << "Terminator for block " << blkid << " doesn't have a wait()\n");
	  //   LLVM_DEBUG(S1->dump(llvm::dbgs(), ast_context_));
	  //   xtbodyp->Run((Stmt *)S1, h_switchcase, rmethod);
	  //   for (int i = thisix+1; i < pt[thisix].second.getFalseId(); i++) {
	  //     // need to mark all the true branch nodes in path vector as visited.
	  //     updatepnvisited(i);
	  //   }
	  //   return;
	  // }
	  xtbodyp->Run((Stmt *)S1->getCond(), hcondstmt, rthread);
	}
	else if (const IfStmt *S1 = dyn_cast<IfStmt> (S)) {
	  LLVM_DEBUG(llvm::dbgs() << "Terminator for block " << blkid << " is an if stmt\n");
	  xtbodyp->Run((Stmt *)S1->getCond(), hcondstmt, rthread);
	}
	else if (const ConditionalOperator *S1 = dyn_cast<ConditionalOperator>(S)) {
	  LLVM_DEBUG(llvm::dbgs() << "Terminator for block " << blkid << " is conditional operator, skipping\n");
	  // below code doesn't work due to skipping too many nodes in true and false paths
	  // int flsix = pt[thisix].second.getFalseId();
	  // for (int i = thisix+1; i <flsix; i++) {
	  //     // need to mark all the true branch nodes in path vector as visited.
	  //   updatepnvisited(i);
	  // }
	  // for (int i =flsix; i < flsix+GetFalseLength(pt, thisix); i++) {
	  //   // need to mark all the false branch nodes in path vector as visited.
	  //   updatepnvisited(i);
	  // }
	  return;
	}
	else {
	  LLVM_DEBUG(llvm::dbgs() << "Terminator for block " << blkid << " is not handled\n");
	}

	// process true branch
	hNodep if1 = new hNode(hNode::hdlopsEnum::hCStmt);
	int flseix = pt[thisix].second.getFalseId();
	ProcessSplitGraphGroup(pt, thisix+1, flseix - (thisix+1), state_num, if1);
	//ProcessSplitGraphGroup(pt, thisix+1, flseix_vec[thisix].first - (thisix+1),
	//state_num,if1);
	hcondstmt->append(if1);
		  
	if (flseix != 0) {// has false branch
	  if1 = new hNode(hNode::hdlopsEnum::hCStmt);
	  
	  ProcessSplitGraphGroup(pt, flseix, GetFalseLength(pt, thisix),
				 state_num, if1);
	  hcondstmt->append(if1);
	}
	h_switchcase->append(hcondstmt);
	return;
      } // end if this was a terminator block
      

      if (sgb->getNumOfElements() > 0) {
	
	// from http://clang-developers.42468.n3.nabble.com/Visiting-statements-of-a-CFG-one-time-td4069440.html#a4069447
	// had to add recursive traversal of AST node children
	std::vector<const Stmt *> SS;
	FindStatements(sgb, SS);
	hNodep htmp = new hNode(h_top_->getname(), hNode::hdlopsEnum::hNoop); 
	//for (auto E : sgb->getElements()) {
	//if (auto SE = E->getAs<CFGStmt>()) {
	for (auto S : SS) {
	  //const Stmt *S = SE->getStmt();
	  if (sgb->hasWait()) iswait = true;
	  LLVM_DEBUG(llvm::dbgs() << "Split Graph Stmt follows\n");
	  LLVM_DEBUG(S->dump(llvm::dbgs(), ast_context_));
	  
	  htmp->child_list.clear();

	  // Check if this CFG block has already been generated
	  // if so, skip the var decls. they were done on the
	  // first instantiation
	  // However, still need to generate code for their initializers
	  const DeclStmt *declstmt = dyn_cast<DeclStmt>(S);
	  if ((declstmt!=NULL) && (SGVisited[blkid]>1))
	    ProcessDeclStmt(declstmt, htmp);
	  else xtbodyp->Run(const_cast<Stmt *>(S), htmp, rthread); // not declstmt

	  LLVM_DEBUG(llvm::dbgs() << "after Run, htmp follows\n");
	  //htmp->dumphcode();
	  LLVM_DEBUG(htmp->print(llvm::dbgs()));
	  CheckVardecls(htmp,blkid);
	  if (IsWaitStmt(htmp)) {
	    ProcessHWait(htmp, sgb->getNextState());
	    //htmp->child_list.back()->set(std::to_string(sgb->getNextState()));
	  }
	  if (htmp->child_list.size() >0) 
	    h_switchcase->child_list.insert(h_switchcase->child_list.end(), htmp->child_list.begin(), htmp->child_list.end());
	    
	  //htmp->child_list.clear();
	    
	  methodecls.insertall(xtbodyp->methodecls);
	}
	//hthreadblocksp->append(h_switchcase);
      }
	
	
      // if (!iswait)  {
      // 	for (auto succ : sgb->getSuccessors()) {
      // 	  LLVM_DEBUG(llvm::dbgs() << "element successor path, block id is " << succ->getBlockID() << "\n");
      // 	  ProcessSplitGraphBlock(succ, state_num, h_switchcase, scfg);
      // 	}
      // }
    }
  }

  void HDLThread::ProcessHWait(hNodep htmp, int nxtstate) {
    hNodep hw = htmp->child_list.back(); // the wait instruction
    if (hw->child_list.size() == 0) {
      hw->set(hNode::hdlopsEnum::hBinop, "=");
      hw->append(new hNode(nextstate_string, hNode::hdlopsEnum::hVarref));
      hw->append(new hNode(std::to_string(nxtstate), hNode::hdlopsEnum::hLiteral));
    }
    else {
      needwaitswitchcase = true;
      // generate waitcounter = wait arg; nextwaitstate = nxtstate; nextstate = waitstate (which is numstates)
      hNodep hnewinstr = hw->child_list.back();
      string waitarg = hnewinstr->getname();
      // wait counter = wait arg
      hw->set( hNode::hdlopsEnum::hBinop, "=");
      hnewinstr->set(hNode::hdlopsEnum::hVarref, nextwaitctr_string);
      hw->append(new hNode(waitarg, hNode::hdlopsEnum::hLiteral));

      // nextwaitstate = nextstate
      htmp->append(GenerateBinop("=", savewaitnextstate_string, std::to_string(nxtstate), true));

      // nextstate = waitstate
      htmp->append(GenerateBinop("=", nextstate_string, std::to_string(numstates)));
    }
    htmp->append(new hNode(hNode::hdlopsEnum::hReturnStmt));
  }

  void HDLThread::GenerateWaitCntUpdate(hNodep h_switchcase) {
    // wait ctr -=1
    hNodep hw = new hNode("--", hNode::hdlopsEnum::hPostfix);
    hw->append(new hNode(nextwaitctr_string, hNode::hdlopsEnum::hVarref));
    h_switchcase->append(hw);
    // if (wait ctr == 0) nextstate = nextwaitstate
    hw =  new hNode(hNode::hdlopsEnum::hIfStmt);
    hw->append(GenerateBinop("==", waitctr_string, "0"));
    // then clause
    hw->append(GenerateBinop("=", nextstate_string, waitnextstate_string, false));
    h_switchcase->append(hw);
    
  }
  
  void HDLThread::GenerateStateUpdate(hNodep hstatemethod, hNodep hshadowvarsp){
    const string comb_assign = "@=";
    hNodep hifblock = new hNode(hNode::hdlopsEnum::hIfStmt);
    // expecting
    // hSensvar ASYNC [
    //   hVarref arst NOLIST
    //   hLiteral 0 NOLIST
    // ]
	    
    if ((h_resetvarinfo_ != NULL) && (h_resetvarinfo_->child_list.size() == 2)) {
      hifblock->append(GenerateBinop("==", h_resetvarinfo_->child_list[0]->getname(),
				     h_resetvarinfo_->child_list[1]->getname()));
    }
    else hifblock->append(GenerateBinop("==", efc_->getResetSignal().first, efc_->getResetEdge().first, false));

    // then part: reset state transition variables
    hNodep hcstmt = new hNode(hNode::hdlopsEnum::hCStmt);
    hcstmt->append(GenerateBinop(comb_assign, state_string, "0"));
    hcstmt->append(GenerateBinop(comb_assign, waitnextstate_string, "0"));
    hcstmt->append(GenerateBinop(comb_assign, waitctr_string, "0"));

    // now do the referenced variables
    for (hNodep onelocalvar : hshadowvarsp->child_list) {
      //hcstmt->append(GenerateBinop(comb_assign, onelocalvar->getname(), "0"));  // assume all coercible to int; need prior check
      hcstmt->append(GenerateBinop(comb_assign, shadowstring+onelocalvar->getname(), "0"));  // assume all coercible to int; need prior check
      
    }
    
    hifblock->append(hcstmt);

    // else part: set state transition variables
    hcstmt = new hNode(hNode::hdlopsEnum::hCStmt);
    hcstmt->append(GenerateBinop(comb_assign, state_string, nextstate_string, false));
    //hcstmt->append(GenerateBinop(comb_assign, waitnextstate_string, nextwaitnextstate_string, false));
    hcstmt->append(GenerateBinop(comb_assign, waitctr_string, nextwaitctr_string, false));
    hcstmt->append(GenerateBinop(comb_assign, waitnextstate_string, savewaitnextstate_string, false));

    // now do the referenced variables
    for (hNodep onelocalvar : hshadowvarsp->child_list) {
      string s =  onelocalvar->getname();
      hcstmt->append(GenerateBinop(comb_assign,shadowstring+s, s, false));  
    }
    hifblock->append(hcstmt);
    hstatemethod->append(hifblock);
  }

  void HDLThread::GenerateStateVar(string sname) {
    // add var decls for a state variable
    
    hNodep hsigp = new hNode(sname, hNode::hdlopsEnum::hVardecl); 
    hNodep htypeinfo = new hNode(hNode::hdlopsEnum::hTypeinfo);
    htypeinfo->append(new hNode("int", hNode::hdlopsEnum::hType));
    hsigp->append(htypeinfo);
    hlocalvarsp->append(hsigp);
  }

  hNodep HDLThread::GenerateBinop( string opname, string lhs, string rhs, bool rhs_is_literal) {
    hNodep newbinop = new hNode(opname, hNode::hdlopsEnum::hBinop);
    newbinop->append(new hNode(lhs, hNode::hdlopsEnum::hVarref));
    newbinop->append(new hNode(rhs, rhs_is_literal ? hNode::hdlopsEnum::hLiteral :hNode::hdlopsEnum::hVarref));
    return newbinop;
  }
  
  // Code below this line is obsolete
  // BFS traversal so all local decls are seen before being referenced
 #if 0 
  void HDLThread::AddThreadMethod(const CFGBlock &BI) {
    std::vector<const CFGBlock *> succlist, nextsucclist;
    ProcessBB(BI);
    //CFGVisited[BI.getBlockID()]+=1;
    for (const auto &succ : BI.succs() ) { // gather successors
      const CFGBlock *SuccBlk = succ.getReachableBlock();
      if (SuccBlk!=NULL) succlist.push_back(SuccBlk);
    }
    bool changed;
    do {
      changed = false;
      for (const CFGBlock *si: succlist) { //process BB of successors at this level
	if (CFGVisited.find(si->getBlockID()) == CFGVisited.end()) {
	  CFGVisited[si->getBlockID()]+= 1;
	  LLVM_DEBUG(llvm::dbgs() << "Visiting Block " << si->getBlockID() << "\n");
	  ProcessBB(*si);
	  changed = true;
	  for (auto sii: si->succs()) {
	    const CFGBlock *SuccBlk = sii.getReachableBlock();
	    if (SuccBlk!=NULL) nextsucclist.push_back(SuccBlk); // gather successors at next level
	  }
	}
      }
      succlist = nextsucclist;
    }
    while (changed);
  }

  void HDLThread::ProcessBB(const CFGBlock &BI) {
    string blkid =  std::to_string(BI.getBlockID());
    if (BI.size() > 0) {
      hNodep h_body =  new hNode("B"+blkid, hNode::hdlopsEnum::hMethod);
      // from http://clang-developers.42468.n3.nabble.com/Visiting-statements-of-a-CFG-one-time-td4069440.html#a4069447
      // had to add recursive traversal of AST node children
      std::vector<const Stmt *> SS;
      FindStatements(BI, SS);

      hNodep htmp = new hNode(h_top_->getname(), hNode::hdlopsEnum::hNoop); // put the statement here temporarily
      for (auto stmt: SS) {
	LLVM_DEBUG(llvm::dbgs() << "Stmt follows\n");
	LLVM_DEBUG(stmt->dump(llvm::dbgs(), ast_context_));
	//generate hcode for this statement, 
	//HDLBody xmethod(const_cast<Stmt *>(stmt), h_body, diag_e, ast_context_, mod_vname_map_, false);
	xtbodyp->Run(const_cast<Stmt *>(stmt), htmp, rthread);
	CheckVardecls(htmp, BI.getBlockID());
	if (htmp->child_list.size() >0)
	  h_body->child_list.insert(h_body->child_list.end(), htmp->child_list.begin(), htmp->child_list.end());
	
	htmp->child_list.clear();
	
	methodecls.insertall(xtbodyp->methodecls);
      }
      hthreadblocksp->append(h_body);
    }
  }
#endif
  
}
