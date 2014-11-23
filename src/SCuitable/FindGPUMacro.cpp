#include "FindGPUMacro.h"
using namespace scpar;

GPUMacro::GPUMacro(int blockIdx, int blockIdy, int blockIdz, int threadIdx, int threadIdy, int threadIdz, int gpuTime, int cpuTime) :
  _blockIdx(blockIdx),
  _blockIdy(blockIdy),
  _blockIdz(blockIdz),
  _threadIdx(threadIdx),
  _threadIdy(threadIdy),
  _threadIdz(threadIdz),
  _gpuTime(gpuTime),
  _cpuTime(cpuTime){
  }

GPUMacro::GPUMacro():
  _blockIdx(1),
  _blockIdy(1),
  _blockIdz(1),
  _threadIdx(1),
  _threadIdy(1),
  _threadIdz(1),
  _gpuTime(0),
  _cpuTime(0){
  
  }

GPUMacro::~GPUMacro() {
}

int GPUMacro::getBlockIdx(){
 return _blockIdx;
}

int GPUMacro::getBlockIdy(){
 return _blockIdy;
}


int GPUMacro::getBlockIdz(){
 return _blockIdz;
}


int GPUMacro::getThreadIdx(){
 return _threadIdx;
}


int GPUMacro::getThreadIdy(){
 return _threadIdx;
}


int GPUMacro::getThreadIdz(){
 return _threadIdz;
}

int GPUMacro::getCPUTime() {
 return _cpuTime;
}

int GPUMacro::getGPUTime() {
 return _gpuTime;
}

void GPUMacro::dump(raw_ostream& os) {
 os <<"\n Block Ids : " <<_blockIdx<<" " <<_blockIdy<<" " <<_blockIdz;

 os <<"\n Thread Ids : " <<_threadIdx<<" " <<_threadIdy<<" " <<_threadIdz;
 
 os <<"\n GPU time : " <<_gpuTime<<" CPU time : " <<_cpuTime;
}

///////////////////////////////////////////////////////
FindGPUMacro::FindGPUMacro(CXXMethodDecl* entryFunction, llvm::raw_ostream& os):
  _entryFunction(entryFunction), 
  _os(os)
{
 TraverseDecl(_entryFunction);
}

FindGPUMacro::~FindGPUMacro()
{
}

bool FindGPUMacro::VisitForStmt(ForStmt *fstmt) {
 
  Stmt *body = fstmt->getBody();
  int tx = 1, ty = 1, tz = 1 , bx = 1, by = 1, bz = 1, gpu_time = 0, cpu_time = 0;
  for (Stmt::child_iterator it = body->child_begin(), eit = body->child_end();
      it != eit;
      it++) {

    Stmt *s = *it; 

    if (DeclStmt *ds = dyn_cast<DeclStmt>(s)){
      if (VarDecl *vd = dyn_cast<VarDecl>(ds->getSingleDecl())){
        string className = vd->getTypeSourceInfo()->getType().getBaseTypeIdentifier()->getName();

	if (className == "profile_time") {
	 if (CXXConstructExpr *ce = dyn_cast<CXXConstructExpr>(vd->getInit()->IgnoreImpCasts())) {
	  IntegerLiteral *x = dyn_cast<IntegerLiteral>(ce->getArg(0));
	  IntegerLiteral *y = dyn_cast<IntegerLiteral>(ce->getArg(1));

	  gpu_time = x->getValue().getSExtValue();
	  cpu_time = y->getValue().getSExtValue();
	 }	 
	}
        if (className == "sc_gpu_thread_hierarchy") {   
         if (CXXConstructExpr *ce = dyn_cast<CXXConstructExpr>(vd->getInit()->IgnoreImpCasts())) {

          IntegerLiteral *x = dyn_cast<IntegerLiteral>(ce->getArg(0)); 
          IntegerLiteral *y = dyn_cast<IntegerLiteral>(ce->getArg(1));
          IntegerLiteral *z = dyn_cast<IntegerLiteral>(ce->getArg(2)); 
          tx = x->getValue().getSExtValue();
          ty = y->getValue().getSExtValue();
          tz = z->getValue().getSExtValue();          
         }        
        }
        if (className == "sc_gpu_block_hierarchy") {   
         if (CXXConstructExpr *ce = dyn_cast<CXXConstructExpr>(vd->getInit()->IgnoreImpCasts())) {

          IntegerLiteral *x = dyn_cast<IntegerLiteral>(ce->getArg(0)); 
          IntegerLiteral *y = dyn_cast<IntegerLiteral>(ce->getArg(1));
          IntegerLiteral *z = dyn_cast<IntegerLiteral>(ce->getArg(2)); 
          bx = x->getValue().getSExtValue();
          by = y->getValue().getSExtValue();
          bz = z->getValue().getSExtValue();           
         }        
        }      
      }
    }
  }
  if (tx && ty && tz && bx && by && bz && gpu_time && cpu_time) {

    GPUMacro *gm = new GPUMacro(bx, by, bz, tx, ty, tz, gpu_time, cpu_time);
    _forStmtGPUMacroMap.insert(forStmtGPUMacroPairType(fstmt, gm));  
  }
  return true;
}

FindGPUMacro::forStmtGPUMacroMapType FindGPUMacro::getForStmtGPUMacroMap() {
 
  return _forStmtGPUMacroMap;
}

void FindGPUMacro::dump(){
 _os <<"\n For Stmt GPU Macro";
 for (forStmtGPUMacroMapType::iterator it = _forStmtGPUMacroMap.begin(), eit = _forStmtGPUMacroMap.end();
     it != eit;
     it++) {
  
   _os <<"\n For Stmt : " <<it->first;
   _os <<"\n GPU kernel parameters \n";
   it->second->dump(_os);
 }
}
