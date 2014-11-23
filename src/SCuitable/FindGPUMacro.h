#ifndef __FIND_GPU_MACRO_H
#define __FIND_GPU_MACRO_H

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <map>

namespace scpar {
 
  using namespace std;
  using namespace clang;

  class GPUMacro {
   
    public:
      GPUMacro(int, int, int, int, int, int, int, int);
      GPUMacro();
      ~GPUMacro();

      int getBlockIdx();
      int getBlockIdy();
      int getBlockIdz();
      int getThreadIdx();
      int getThreadIdy();
      int getThreadIdz();
      int getGPUTime();
      int getCPUTime(); 
      void dump(raw_ostream&);      

    private:
      int _blockIdx;
      int _blockIdy;
      int _blockIdz;
      int _threadIdx;
      int _threadIdy;
      int _threadIdz;
      int _gpuTime;
      int _cpuTime; 
  };
  

  class FindGPUMacro:public RecursiveASTVisitor <FindGPUMacro> {
    public:
      
      typedef pair <ForStmt*, GPUMacro*> forStmtGPUMacroPairType;
      typedef map<ForStmt*, GPUMacro*> forStmtGPUMacroMapType; 
      

      FindGPUMacro(CXXMethodDecl*, raw_ostream&);
      ~FindGPUMacro();
      // ANI : Need to add other loops as well.....       
      virtual bool VisitForStmt(ForStmt *);

      forStmtGPUMacroMapType getForStmtGPUMacroMap();

      void dump();


    private:
     forStmtGPUMacroMapType _forStmtGPUMacroMap; 
     CXXMethodDecl* _entryFunction;
     raw_ostream& _os; 
  };
}
#endif
