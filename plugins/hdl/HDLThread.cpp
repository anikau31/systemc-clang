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

namespace systemc_hdl {

  HDLThread::HDLThread(CXXMethodDecl *emd, hNodep &h_top,
		       clang::DiagnosticsEngine &diag_engine, const ASTContext &ast_context, hdecl_name_map_t &mod_vname_map )
    : h_top_{h_top}, diag_e{diag_engine}, ast_context_{ast_context}, mod_vname_map_{mod_vname_map} {
      LLVM_DEBUG(llvm::dbgs() << "Entering HDLThread constructor (thread body)\n");
      h_ret = NULL;

      blkix = 0;
      stateix = 0;

      xtbodyp = new HDLBody(diag_e, ast_context_, mod_vname_map_);
      hthreadblocksp = new hNode(hNode::hdlopsEnum::hProcesses); // placeholder to collect methods
      hlocalvarsp = new hNode(hNode::hdlopsEnum::hPortsigvarlist); // placeholder to collect local vars
      
      // get CFG
      
      std::unique_ptr< CFG > threadcfg = clang::CFG::buildCFG(emd, emd->getBody(), &(emd->getASTContext()), clang::CFG::BuildOptions());
      clang::LangOptions LO = ast_context.getLangOpts();
      threadcfg->dump(LO, false);

      // HDLBody instance init
      
      const CFGBlock & B = threadcfg->getEntry();
      AddThreadMethod(B);

      h_top->child_list.push_back(hlocalvarsp);
      h_top->child_list.push_back(hthreadblocksp);
      
      LLVM_DEBUG(llvm::dbgs() << "SCBBlocks:\n"); 
      for (auto scbl: scbblocks)
	scbl->dump();
      LLVM_DEBUG(llvm::dbgs() << "SCWait Blocks:\n"); 
      for (auto bl: waitblocks)
	bl->dump();
    }

  HDLThread::~HDLThread() {
    LLVM_DEBUG(llvm::dbgs() << "[[ Destructor HDLThread ]]\n");
  }

  bool HDLThread::is_wait_stmt(hNodep hp) {
    return ((hp->child_list.size() >=1) and ((hp->child_list.back())->getopc() == hNode::hdlopsEnum::hWait));
  }

  void HDLThread::CheckVardecls(hNodep &hp) {
    int varcnt = 0;
    for (auto oneop : hp->child_list) {
      if ((oneop->getopc() == hNode::hdlopsEnum::hVardecl) || (oneop->getopc() == hNode::hdlopsEnum::hSigdecl)) {
	LLVM_DEBUG(llvm::dbgs() << "Detected vardecl\n");
	hlocalvarsp->child_list.push_back(oneop);
	varcnt += 1;
      }
      else break; // all vardecls are first in the list of ops
    }
    if (varcnt >=1) {
      hp->child_list.erase(hp->child_list.begin(), hp->child_list.begin()+varcnt);
      mod_vname_map_.insertall(xtbodyp->vname_map);
    }
  }

  void HDLThread::MarkStatements(const Stmt *S, llvm::SmallDenseMap<const Stmt*, bool> &Map) {
    Map[S] = true;
    for (const Stmt *K : S->children())
      MarkStatements(K, Map);
  }
  
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
    if (const Stmt *S = B.getTerminatorStmt()) {
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

  // BFS traversal so all local decls are seen before being referenced
  
  void HDLThread::AddThreadMethod(const CFGBlock &BI) {
    std::vector<const CFGBlock *> succlist, nextsucclist;
    ProcessBB(BI);
    Visited[BI.getBlockID()] = true;
    for (const auto &succ : BI.succs() ) { // gather successors
      const CFGBlock *SuccBlk = succ.getReachableBlock();
      if (SuccBlk!=NULL) succlist.push_back(SuccBlk);
    }
    bool changed;
    do {
      changed = false;
      for (const CFGBlock *si: succlist) { // process BB of successors at this level
	if (Visited.find(si->getBlockID()) == Visited.end()) {
	  Visited[si->getBlockID()] = true;
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
      SCBBlock *curscblock = new SCBBlock(BI, SS);
      bool seenwait = false;
      int stmtix = 0;
      curscblock->startstmtix = stmtix;
      curscblock->endstmtix = SS.size();
      curscblock->blkix = blkix;
      curscblock->dump();
      hNodep htmp = new hNode(h_top_->getname(), hNode::hdlopsEnum::hNoop); // put the statement here temporarily
      for (auto stmt: SS) {
	LLVM_DEBUG(llvm::dbgs() << "Stmt follows\n");
	LLVM_DEBUG(stmt->dump(llvm::dbgs(), ast_context_));
	//generate hcode for this statement, 
	//HDLBody xmethod(const_cast<Stmt *>(stmt), h_body, diag_e, ast_context_, mod_vname_map_, false);
	xtbodyp->Run(const_cast<Stmt *>(stmt), htmp, rthread);
	CheckVardecls(htmp);
	if (htmp->child_list.size() >0)
	  h_body->child_list.insert(h_body->child_list.end(), htmp->child_list.begin(), htmp->child_list.end());
	
	htmp->child_list.clear();
	
	stmtix++;
	methodecls.insertall(xtbodyp->methodecls);
      }
      blkix+=1;
      hthreadblocksp->child_list.push_back(h_body);
      scbblocks.push_back(curscblock);
    }
  }
  
  
  void SCBBlock::SCBBlock::dump() {
    LLVM_DEBUG(llvm::dbgs() << "SCBBlock " << blkix << " from CFG Block "
	       << cfgb.getBlockID() << " startstmtix " << startstmtix <<
	       " endstmtix " << endstmtix << " statenum " << statenum << "\n");
    if (hblockbody!=NULL) hblockbody->dumphcode();
    // for (auto stmt: stmts)
    //   LLVM_DEBUG(stmt->dump(llvm::dbgs(), ast_context_));
  }
}
