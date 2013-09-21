#include "DumpCFG.h"
#include "clang/Analysis/CFG.h"
#include "clang/Analysis/Visitors/CFGStmtVisitor.h"

using namespace scpar;

CFGFindWait::CFGFindWait(CFG* c, llvm::raw_ostream &os)
: _cfg(c), _os(os){
}

void CFGFindWait::BlockStmt_VisitExpr(Expr* e) {
//  llvm::errs() << "BLOCK STMT VISIT EXPR\n";
  LangOptions LO;
  LO.CPlusPlus = true;
  PrintingPolicy Policy(LO);

  if (e->getStmtClass() == 42){
     
        CallExpr *s = cast<CallExpr>(e);
        if (s->getDirectCallee()->getNameInfo().getAsString() == "wait") {
        	_os << "Found wait";
        }
  } 


  //_os << "printing pretty something here";
 // s->printPretty(llvm::errs(), 0,Policy,0);  

}

CFG& CFGFindWait::getCFG() {
  return *_cfg;
}


////////////////////////////////////////////////////////////////////////////////
//
// DumpCFG
// 
////////////////////////////////////////////////////////////////////////////////
DumpCFG::DumpCFG(CXXMethodDecl* d, ASTContext* a, llvm::raw_ostream& os )
: _d(d)
, _os(os)
, _cfg(NULL)
, _stmtMap(NULL) {
  
  
  _os << "*********** CFG ************\n";
  //d->dump();
  const CFG::BuildOptions& b= CFG::BuildOptions();
  // This needs to be cleaned up!
  _cfg = CFG::buildCFG(cast<Decl>(d), d->getBody(), a, b);
  dump();
  /// TODO: I have to delete this myself.
  /// FIXME: Providing NULL to ParentMap is wrong.  It should be the root of the AST fragment
  /// that one wants to build the Map for.  It seems that it should be for the entry function.
  
  Stmt* efBody = d->getBody();
  _stmtMap = CFGStmtMap::Build(_cfg, new ParentMap(efBody));
}

void DumpCFG::splitBlock() {
    LangOptions LO;
    LO.CPlusPlus = true;

    const CFGBlock* orig = NULL;
    int cnt = 0;
  for (CFG::iterator I = _cfg->begin(), E = _cfg->end(); 
       I != E; 
       ++I) {
    const CFGBlock *block = *I;
    LangOptions LO;
    LO.CPlusPlus = true;
    if (cnt == 2) {
      _os << "==========================================================\n";
      _os << "Dump CFG Block\n";
      block->dump(_cfg, LO, true);
      _os << "==========================================================\n";
      orig = block;
    }
    ++cnt;
  }

   _os <<" Anirudh's work space \n";

   for (CFG::iterator I = _cfg->begin(), E = _cfg->end(); I != E; ++I) {
    
     CFGBlock *tmp = *I;
     tmp->dump(_cfg, LO, true);
     for (CFGBlock::iterator I_2 = tmp->begin(), E_2 = tmp->end(); I_2 != E_2; I_2++) {
    
    
       CFGElement In = *I_2;
       const CFGStmt* s = In.getAs<CFGStmt>();
   //    _os << s->getStmt()->getStmtClassName();
      Stmt* st = const_cast<Stmt*>(s->getStmt());
      CFGFindWait f(_cfg, _os);
      f.BlockStmt_Visit(st); 
    
     }
   
   }

   _os <<"Anirudh's work space \n ";
  orig->dump(_cfg, LO, true);

  CFGBlock* copy = const_cast<CFGBlock*>(orig); //new CFGBlock(*orig);
  _os << "Copy is the same?\n";
  copy->dump(_cfg,LO,true);

  // Create a new block
  CFGBlock* bn = _cfg->createBlock();
  
  CFGElement first(orig->front());
  _os << "Dump first element\n";
  if (const CFGStmt* s = first.getAs<CFGStmt>()) {
    Stmt* st = const_cast<Stmt*>(s->getStmt());
    copy->appendStmt(st, _cfg->getBumpVectorContext());
    bn->appendStmt(st,  _cfg->getBumpVectorContext());
  }

  _os << "Altered is the same?\n";
  copy->dump(_cfg,LO,true);

  // Setup bn
  bn->addSuccessor(copy, _cfg->getBumpVectorContext());
  bn->addSuccessor(&_cfg->getExit(), _cfg->getBumpVectorContext());  
  _os << "New block?\n";
  bn->dump(_cfg,LO,true);
  
  dump();

  
  //  delete copy;
  
}

void DumpCFG::test() {
  for (CFG::iterator I = _cfg->begin(), E = _cfg->end(); 
       I != E; 
       ++I) {
    const CFGBlock *block = *I;
    LangOptions LO;
    LO.CPlusPlus = true;
    _os << "==========================================================\n";
    _os << "Dump CFG Block\n";
    block->dump(_cfg, LO, true);
    _os << "==========================================================\n";
    //block->print(_os, _cfg, LO, true);


    
        
  }
}
    


void DumpCFG::dumpSource() {
  for (CFG::iterator I = _cfg->begin(), E = _cfg->end(); 
       I != E; 
       ++I) {

    LangOptions LO;
    LO.CPlusPlus = true;
    const CFGBlock *block = *I;
    CFGBlock block_tmp = _cfg->getEntry();
    _os <<"something new dump here \n ";
    block_tmp.dump(_cfg, LO, true);
    _os <<"End Something new dump here \n ";
    	
    _os << "==========================================================\n";
    _os << "Dump CFG Block\n";
    block->dump(_cfg, LO, true);
//    _os << "==========================================================\n";

    for (CFGBlock::const_iterator bit = block->begin(), bite = block->end();
         bit != bite;
         ++bit) {
      // bit is CFGElement type.
      if (const CFGStmt * s = bit->getAs<CFGStmt>()) {
        _os << "==>\n";
        _os << " Dump source code for stmt:\n";
//        Stmt* st = const_cast<Stmt*>(s->getStmt());
 //       st->dump();
        // _os << "Use ParentMap to get CFGBlock\n";
        // (_stmtMap->getBlock(st))->dump(_cfg, LO, true);

        
        //_os << "Dump the source code\n";
//        CFGFindWait f(_cfg);
  //      f.BlockStmt_Visit(st);
        _os << "\n===>";
      }
    }
  }
}


void DumpCFG::dump() {
  if (_cfg == NULL) {
    return;
  }
  LangOptions LO;
  LO.CPlusPlus = true;
  
  _cfg->dump(LO, true);
  
}
