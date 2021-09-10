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

  HDLThread::HDLThread(CXXMethodDecl *emd, hNodep &h_top,
		       clang::DiagnosticsEngine &diag_engine, const ASTContext &ast_context, hdecl_name_map_t &mod_vname_map )
    : h_top_{h_top}, diag_e{diag_engine}, ast_context_{ast_context}, mod_vname_map_{mod_vname_map} {
      LLVM_DEBUG(llvm::dbgs() << "Entering HDLThread constructor (thread body)\n");
      h_ret = NULL;

      xtbodyp = new HDLBody(diag_e, ast_context_, mod_vname_map_);
      hthreadblocksp = new hNode(hNode::hdlopsEnum::hSwitchStmt); // body is switch, each path is case alternative
      hlocalvarsp = new hNode(hNode::hdlopsEnum::hPortsigvarlist); // placeholder to collect local vars
      
      // build SC CFG
      SplitCFG scfg{const_cast<ASTContext &>(ast_context_), emd};
      //scfg.split_wait_blocks(emd);
      // scfg.build_sccfg( method );
      scfg.generate_paths();
      scfg.dump();

      const llvm::SmallVectorImpl<SplitCFG::VectorSplitCFGBlock> &paths_found{ scfg.getPathsFound()};
      llvm::dbgs() << "Dump sc cfg from hdlthread\n";
      unsigned int id{0};
      for (auto const& pt: paths_found) {
	llvm::dbgs() << "S" << id << ": ";
	for (auto const& block : pt) {
	  llvm::dbgs() << block->getBlockID() << "  ";
	}
	llvm::dbgs() << "\n";
	++id;
      }
      int state_num = 0;
      for (auto const& pt: paths_found) {
	hNodep h_switchcase = new hNode(std::to_string(state_num), hNode::hdlopsEnum::hSwitchCase);
	ProcessSplitGraphBlock(pt[0], state_num, h_switchcase);
	hthreadblocksp->child_list.push_back(h_switchcase);
	state_num++;
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
      h_top->child_list.push_back(hlocalvarsp);
      h_top->child_list.push_back(hthreadblocksp);
      
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
    if (S != NULL) {
      Map[S] = true;
      for (const Stmt *K : S->children())
	MarkStatements(K, Map);
    }
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
    if (const Stmt *S = (B->getCFGBlock())->getTerminatorStmt()) {
       for (const Stmt *K : S->children())
     	MarkStatements(K, Map);
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
  
  // BFS traversal so all local decls are seen before being referenced
  
  void HDLThread::AddThreadMethod(const CFGBlock &BI) {
    std::vector<const CFGBlock *> succlist, nextsucclist;
    ProcessBB(BI);
    //Visited[BI.getBlockID()] = true;
    for (const auto &succ : BI.succs() ) { // gather successors
      const CFGBlock *SuccBlk = succ.getReachableBlock();
      if (SuccBlk!=NULL) succlist.push_back(SuccBlk);
    }
    bool changed;
    do {
      changed = false;
      for (const CFGBlock *si: succlist) { //process BB of successors at this level
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

  void HDLThread::ProcessSplitGraphBlock(const SplitCFGBlock *sgb, int state_num, hNodep h_switchcase) {
    bool iswait = false;
    if (sgb != NULL) {
      if (sgb->getNumOfElements() > 0) {
	string blkid = "S" + std::to_string(state_num) + "_" + std::to_string(sgb->getBlockID());
	LLVM_DEBUG(llvm::dbgs() << "Split Graph num ele, blockid are " << sgb->getNumOfElements() << " " << blkid << "\n");
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
	    //generate hcode for this statement, 
	    //HDLBody xmethod(const_cast<Stmt *>(stmt), h_body, diag_e, ast_context_, mod_vname_map_, false);
	    xtbodyp->Run(const_cast<Stmt *>(S), htmp, rthread);
	    CheckVardecls(htmp);
	    if (htmp->child_list.size() >0)
	      h_switchcase->child_list.insert(h_switchcase->child_list.end(), htmp->child_list.begin(), htmp->child_list.end());
	    
	    htmp->child_list.clear();
	    
	    methodecls.insertall(xtbodyp->methodecls);
	  }
	  //hthreadblocksp->child_list.push_back(h_switchcase);
	}
	
	//}
      if (! iswait) {
	for (auto spgsucc : sgb->getSuccessors()) {
	  ProcessSplitGraphBlock(spgsucc, state_num, h_switchcase);
	}
      }
    }
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
	CheckVardecls(htmp);
	if (htmp->child_list.size() >0)
	  h_body->child_list.insert(h_body->child_list.end(), htmp->child_list.begin(), htmp->child_list.end());
	
	htmp->child_list.clear();
	
	methodecls.insertall(xtbodyp->methodecls);
      }
      hthreadblocksp->child_list.push_back(h_body);
    }
  }

}
