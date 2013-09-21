#ifndef _FIND_LOOP_TIME_H_
#define _FIND_LOOP_TIME_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ExprCXX.h"
#include <map>
namespace scpar {

  using namespace clang;
  using namespace std;
  
  class FindLoopTime: public RecursiveASTVisitor<FindLoopTime> {
  public:

		FindLoopTime(CXXMethodDecl*, llvm::raw_ostream&);
		~FindLoopTime();

		typedef pair<double, double> loopTimePairType;

		typedef pair<ForStmt*, loopTimePairType> forStmtLoopTimePairType;
		typedef map<ForStmt*, loopTimePairType> forStmtLoopTimeMapType;

    virtual bool VisitCXXConstructExpr(CXXConstructExpr* expr);    
		virtual bool VisitForStmt(ForStmt *fs);
		forStmtLoopTimeMapType returnLoopTimeMap();
    void dump();

  private:

		CXXMethodDecl *_entryMethodDecl;
		ForStmt *_fs;
		double _cpuTime;
		double _gpuTime;
		forStmtLoopTimeMapType _forStmtLoopTimeMap;
		llvm::raw_ostream &_os;
	};
}

#endif
