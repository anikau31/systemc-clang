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
    : diag_e{diag_engine}, ast_context_{ast_context} {
      LLVM_DEBUG(llvm::dbgs() << "Entering HDLThread constructor (thread body)\n");
      h_ret = NULL;

      // get CFG
      
      std::unique_ptr< CFG > threadcfg = clang::CFG::buildCFG(emd, emd->getBody(), &(emd->getASTContext()), clang::CFG::BuildOptions());
      clang::LangOptions LO = ast_context.getLangOpts();
      threadcfg->dump(LO, false);
      int blkix = 0;
      int stateix = 0;
      
      // for each CFG bblock in the thread
      for (CFG::const_iterator I = threadcfg->begin(), E = threadcfg->end(); I != E; ++I ) {
	string blkid =  std::to_string((*I)->getBlockID());
	LLVM_DEBUG(llvm::dbgs() << "Block ID " << blkid << " size " << (*I)->size() <<"\n");
	if ((*I)->getTerminator().isValid()) {
	  LLVM_DEBUG(llvm::dbgs() << "Terminator follows\n");
	  LLVM_DEBUG((*I)->getTerminatorStmt()->dump(llvm::dbgs(), ast_context_));
	}
	
	if ((*I)->size() > 0) {
	  hNodep h_body =  new hNode("B"+blkid, hNode::hdlopsEnum::hMethod);
	  // from http://clang-developers.42468.n3.nabble.com/Visiting-statements-of-a-CFG-one-time-td4069440.html#a4069447
	  // had to add recursive traversal of AST node children
	  std::vector<const Stmt *> SS;
	  findStatements(*I, SS);
	  SCBBlock *curscblock = new SCBBlock(*I, SS);
	  bool seenwait = false;
	  int stmtix = 0;
	  curscblock->startstmtix = stmtix;
	  curscblock->endstmtix = SS.size();
	  curscblock->blkix = blkix;
	  curscblock->dump();
	  for (auto stmt: SS) {
	    LLVM_DEBUG(llvm::dbgs() << "Stmt follows\n");
	    LLVM_DEBUG(stmt->dump(llvm::dbgs(), ast_context_));
	    HDLBody xmethod(const_cast<Stmt *>(stmt), h_body, diag_engine, ast_context_, mod_vname_map);
	    if (is_wait_stmt(h_body)) {
	      LLVM_DEBUG(llvm::dbgs() << "Detected wait stmt\n");
	      SCBBlock * wb = new SCBBlock(*I, SS, -1, blkix, blkix+1, stmtix, stmtix+1, stateix, h_body->child_list.back());
	      stateix++;
	      waitblocks.push_back(wb);
	      h_body->child_list.pop_back(); // remove wait hcode
	      curscblock->endstmtix-=1; // the wait statement is not part of the current sc block
	      if (stmtix < SS.size()-1) { // more statements follow wait
		LLVM_DEBUG(llvm::dbgs() << "stmt following wait stmt\n");
		h_top->child_list.push_back(h_body); // end this method
		curscblock->endstmtix = stmtix; // update scblock end stmt ix
		scbblocks.push_back(curscblock);
		h_body =  new hNode("B"+blkid+std::to_string(stmtix), hNode::hdlopsEnum::hMethod);
		curscblock = new SCBBlock(*I, SS);
		curscblock->startstmtix = stmtix+1;
		curscblock->endstmtix = SS.size();
		blkix++;
		curscblock->blkix = blkix;
	      }
	    }
	    stmtix++;
	    methodecls.insertall(xmethod.methodecls);
	  }
	  blkix+=1;
	  h_top->child_list.push_back(h_body);
	  scbblocks.push_back(curscblock);
	}
      }
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
  
  void HDLThread::markStatements(const Stmt *S, llvm::SmallDenseMap<const Stmt*, bool> &Map) {
    Map[S] = true;
    for (const Stmt *K : S->children())
      markStatements(K, Map);
  }
  
  void HDLThread::findStatements(CFGBlock *B, std::vector<const Stmt *> &SS) {
    llvm::SmallDenseMap<const Stmt*, bool> Map;

    // Mark subexpressions of each element in the block.
    for (auto I = B->begin(); I != B->end(); ++I) {
      CFGElement E = *I;
      if (auto SE = E.getAs<CFGStmt>()) {
	const Stmt *S = SE->getStmt();
	for (const Stmt *K : S->children())
	  markStatements(K, Map);
      }
    }
    
    // Any expressions not in Map are top level statements.
    for (auto I = B->begin(); I != B->end(); ++I) {
      CFGElement E = *I;
      if (auto SE = E.getAs<CFGStmt>()) {
	const Stmt *S = SE->getStmt();
	if (Map.find(S) == Map.end()) {
	  SS.push_back(S);
	}
      }
    }
  }
  
  void SCBBlock::dump() {
    LLVM_DEBUG(llvm::dbgs() << "SCBBlock " << blkix << " from CFG Block "
	       << cfgb->getBlockID() << " startstmtix " << startstmtix <<
	       " endstmtix " << endstmtix << " statenum " << statenum << "\n");
    if (hblockbody!=NULL) hblockbody->dumphcode();
    // for (auto stmt: stmts)
    //   LLVM_DEBUG(stmt->dump(llvm::dbgs(), ast_context_));
  }
}
