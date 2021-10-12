// clang-format off
#include "HDLThread.h"
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
		       clang::DiagnosticsEngine &diag_engine, const ASTContext &ast_context, hdecl_name_map_t &mod_vname_map, hNodep h_resetvarinfo )
    : efc_(efc), h_top_{h_top}, diag_e{diag_engine}, ast_context_{ast_context}, mod_vname_map_{mod_vname_map}, h_resetvarinfo_{h_resetvarinfo} {

      LLVM_DEBUG(llvm::dbgs() << "Entering HDLThread constructor (thread body)\n");
      
      CXXMethodDecl *emd = efc->getEntryMethod();
      
      h_ret = NULL;
      needwaitswitchcase = false;

      string threadname = h_top->getname();
      state_string ="_scclang_state_"+ threadname;
      nextstate_string = "_scclang_next_state_"+ threadname;
      waitctr_string = "_scclang_wait_counter_"+ threadname;
      nextwaitctr_string = "_scclang_nextwait_counter_"+ threadname;
      waitnextstate_string = "_scclang_wait_next_state_"+ threadname;
      savewaitnextstate_string = "_scclang_save_wait_next_state_"+ threadname;
      
      thread_vname_map.insertall(mod_vname_map_);
      //xtbodyp = new HDLBody(diag_e, ast_context_, mod_vname_map_);
      xtbodyp = new HDLBody(diag_e, ast_context_, thread_vname_map);
      hNodep hthreadall = new hNode(h_top->getname(), hNode::hdlopsEnum::hMethod);
      hthreadblocksp = new hNode(hNode::hdlopsEnum::hSwitchStmt); // body is switch, each path is case alternative
      hthreadblocksp->append(new hNode(state_string, hNode::hdlopsEnum::hVarref));
      hlocalvarsp = new hNode(hNode::hdlopsEnum::hPortsigvarlist); // placeholder to collect local vars
      hNodep hthreadblockcstmt = new hNode(hNode::hdlopsEnum::hCStmt);
      hthreadblocksp->append(hthreadblockcstmt);
      // build SC CFG

      SplitCFG scfg{const_cast<ASTContext &>(ast_context_), emd};
      scfg.generate_paths();
      LLVM_DEBUG(scfg.dump());
      LLVM_DEBUG(scfg.dumpToDot());

      hNodep hstatemethod = new hNode(threadname+"_state_update", hNode::hdlopsEnum::hMethod);

      GenerateStateUpdate(hstatemethod);
      GenerateStateVar(state_string);
      GenerateStateVar(nextstate_string);
      GenerateStateVar(waitctr_string);
      GenerateStateVar(nextwaitctr_string);
      GenerateStateVar(waitnextstate_string);
      GenerateStateVar(savewaitnextstate_string);
      
      const llvm::SmallVectorImpl<SplitCFG::VectorSplitCFGBlock> &paths_found{ scfg.getPathsFound()};
      numstates = paths_found.size();
      int state_num = 0;
      for (auto const& pt: paths_found) {

	hNodep h_switchcase = new hNode( hNode::hdlopsEnum::hSwitchCase);
	h_switchcase->append(new hNode(std::to_string(state_num), hNode::hdlopsEnum::hLiteral));
	ProcessSplitGraphBlock(pt[0], state_num, h_switchcase);
	hthreadblockcstmt->append(h_switchcase);
	state_num++;
      }

      if (needwaitswitchcase) {
	hNodep h_switchcase = new hNode( hNode::hdlopsEnum::hSwitchCase);
	h_switchcase->append(new hNode(std::to_string(numstates), hNode::hdlopsEnum::hLiteral));
	GenerateWaitCntUpdate(h_switchcase);
	hthreadblockcstmt->append(h_switchcase);
      }

      //std::unique_ptr< CFG > threadcfg = clang::CFG::buildCFG(emd, emd->getBody(), &(emd->getASTContext()), clang::CFG::BuildOptions());
      clang::LangOptions LO = ast_context.getLangOpts();
      //threadcfg->dump(LO, false);
      // HDLBody instance init
      // for (auto const& pt: paths_found) {
      // 	for (auto const& block : pt) {
      // 	  ProcessBB(*(block->getCFGBlock()));
      // 	}
      // }

      // add thread local variables to module level list
      if (hlocalvarsp->size()!=0) h_portsigvarlist->child_list.insert(std::end(h_portsigvarlist->child_list),
								      std::begin(hlocalvarsp->child_list),
								      std::end(hlocalvarsp->child_list));
      hthreadall->append(hthreadblocksp);
      h_top->append(hstatemethod);
      h_top->append(hthreadall);
      //h_top->append(hthreadblocksp);
      
    }

  HDLThread::~HDLThread() {
    LLVM_DEBUG(llvm::dbgs() << "[[ Destructor HDLThread ]]\n");
  }

  bool HDLThread::IsWaitStmt(hNodep hp) {
    return ((hp->child_list.size() >=1) and ((hp->child_list.back())->getopc() == hNode::hdlopsEnum::hWait));
  }

  void HDLThread::CheckVardecls(hNodep &hp, unsigned int cfgblockid) {
    int varcnt = 0;
    for (auto oneop : hp->child_list) {
      if ((oneop->getopc() == hNode::hdlopsEnum::hVardecl) || (oneop->getopc() == hNode::hdlopsEnum::hSigdecl)) {
	LLVM_DEBUG(llvm::dbgs() << "Detected vardecl for CFG Block ID " << cfgblockid << "\n");
	if (CFGVisited[cfgblockid]==1) hlocalvarsp->append(oneop);
	varcnt += 1;
      }
      else break; // all vardecls are first in the list of ops
    }
    if (varcnt >=1) {
      hp->child_list.erase(hp->child_list.begin(), hp->child_list.begin()+varcnt);
      if (CFGVisited[cfgblockid]==1) thread_vname_map.insertall(xtbodyp->vname_map);
    }
  }

  void HDLThread::ProcessDeclStmt(const DeclStmt *declstmt, hNodep htmp) {
    //!
    //! called when a CFG declstmt is instantiated more than once
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
	  varinitp->append(new hNode(thread_vname_map.find_entry_newn(vardecl), hNode::hdlopsEnum::hVarref));
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
  
  void HDLThread::ProcessSplitGraphBlock(const SplitCFGBlock *sgb, int state_num, hNodep h_switchcase) {
    bool iswait = false;

    if (sgb != NULL) {
      string blkid = "S" + std::to_string(state_num) + "_" + std::to_string(sgb->getBlockID());

      if (SGVisited.find(blkid) == SGVisited.end()) {
	SGVisited[blkid] = true;
	CFGVisited[(sgb->getCFGBlock())->getBlockID()]+= 1;
      }
      else return; // already visited this block
      
      LLVM_DEBUG(llvm::dbgs() << "Split Graph num ele, blockid are " << sgb->getNumOfElements() << " " << blkid << "\n");

      if ((sgb->getCFGBlock())->getTerminator().isValid()) {
	hNodep hcondstmt = new hNode(hNode::hdlopsEnum::hIfStmt);
	auto spgsucc = sgb->getSuccessors();
	const Stmt * S = sgb->getCFGBlock()->getTerminatorStmt();

	if (const WhileStmt *S1 = dyn_cast<WhileStmt> (S)) {
	  LLVM_DEBUG(llvm::dbgs() << "Terminator for block " <<blkid << " is a while stmt\n");
	  xtbodyp->Run((Stmt *)S1->getCond(), hcondstmt, rthread);
	}
	else if (const ForStmt *S1 = dyn_cast<ForStmt> (S)) {
	  LLVM_DEBUG(llvm::dbgs() << "Terminator for block " << blkid << " is a for stmt\n");
	  xtbodyp->Run((Stmt *)S1->getCond(), hcondstmt, rthread);
	}
	else if (const IfStmt *S1 = dyn_cast<IfStmt> (S)) {
	  LLVM_DEBUG(llvm::dbgs() << "Terminator for block " << blkid << " is an if stmt\n");
	  xtbodyp->Run((Stmt *)S1->getCond(), hcondstmt, rthread);
	}
	else {
	  LLVM_DEBUG(llvm::dbgs() << "Terminator for block " << blkid << " is not handled\n");
	}

	// for each successor need to make a compound statement

	for (auto succ: spgsucc) {
	  hNodep hcstmt = new hNode(hNode::hdlopsEnum::hCStmt);
	  ProcessSplitGraphBlock(succ, state_num, hcstmt);
	  hcondstmt->append(hcstmt);
	  
	}
	h_switchcase->append(hcondstmt);
	return;
      }

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
	  if ((declstmt!=NULL) &&  (CFGVisited[(sgb->getCFGBlock()->getBlockID()) > 1]))
	    ProcessDeclStmt(declstmt, htmp);
	  else xtbodyp->Run(const_cast<Stmt *>(S), htmp, rthread); // no initializer, so normal

	  LLVM_DEBUG(llvm::dbgs() << "after Run, htmp follows\n");
	  htmp->dumphcode();
	  CheckVardecls(htmp, (sgb->getCFGBlock())->getBlockID());
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
	
	
      if (!iswait)  {
	for (auto spgsucc : sgb->getSuccessors()) {
	  ProcessSplitGraphBlock(spgsucc, state_num, h_switchcase);
	}
      }
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
      htmp->append(GenerateBinop("=", savewaitnextstate_string, std::to_string(nxtstate), false));

      // nextstate = waitstate
      htmp->append(GenerateBinop("=", nextstate_string, std::to_string(numstates)));
    }
  }

  void HDLThread::GenerateWaitCntUpdate(hNodep h_switchcase) {
    // wait ctr -=1
    hNodep hw = new hNode("--", hNode::hdlopsEnum::hPostfix);
    hw->append(new hNode(waitctr_string, hNode::hdlopsEnum::hVarref));
    h_switchcase->append(hw);
    // if (wait ctr == 0) nextstate = nextwaitstate
    hw =  new hNode(hNode::hdlopsEnum::hIfStmt);
    hw->append(GenerateBinop("==", waitctr_string, "0"));
    // then clause
    hw->append(GenerateBinop("=", nextstate_string, waitnextstate_string, false));
    h_switchcase->append(hw);
    
  }
  
  void HDLThread::GenerateStateUpdate(hNodep hstatemethod){
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
    hcstmt->append(GenerateBinop("=", state_string, "0"));
    hcstmt->append(GenerateBinop("=", waitnextstate_string, "0"));
    hcstmt->append(GenerateBinop("=", waitctr_string, "0"));
    hifblock->append(hcstmt);

    // else part: set state transition variables
    hcstmt = new hNode(hNode::hdlopsEnum::hCStmt);
    hcstmt->append(GenerateBinop("=", state_string, nextstate_string, false));
    //hcstmt->append(GenerateBinop("=", waitnextstate_string, nextwaitnextstate_string, false));
    hcstmt->append(GenerateBinop("=", waitctr_string, nextwaitctr_string, false));
    hcstmt->append(GenerateBinop("=", waitnextstate_string, savewaitnextstate_string, false));

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

}
