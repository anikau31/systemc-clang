#ifndef _FIND_WAIT_H_
#define _FIND_WAIT_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <map>
namespace scpar {

  using namespace clang;
  using namespace std;
  
  class FindWait: public RecursiveASTVisitor<FindWait> {
  public:

    typedef vector<CallExpr* > waitListType;
    typedef vector<ForStmt*> forStmtArray;

    typedef pair<CallExpr*, int > waitTimesPairType;
    typedef map <CallExpr*,int > waitTimesMapType; 

		typedef pair <CallExpr*, int> waitNestLoopPairType;
		typedef map <CallExpr*, int> waitNestLoopMapType;

		typedef pair <ForStmt*, CallExpr*> forWaitCallPairType;
		typedef map <ForStmt*, CallExpr*> forWaitCallMapType;

		typedef pair<string, int> dataNestLoopPairType;
		typedef map <string, int> dataNestLoopMapType;

		typedef pair<string, int> dataTimesPairType;
		typedef map<string, int> dataTimesMapType;

		typedef pair <CXXMethodDecl*, vector<string> > processWaitEventPairType;
		typedef map <CXXMethodDecl*, vector<string> > processWaitEventMapType;

    FindWait(CXXMethodDecl*, llvm::raw_ostream&);
		~FindWait();
    
		virtual bool VisitCallExpr(CallExpr* expr);     
    virtual bool VisitForStmt(ForStmt *fs);
		virtual bool VisitExpr(Expr *e);

    CXXMethodDecl* getEntryMethod();
    waitListType getWaitCalls();
		waitTimesMapType getWaitTimesMap();    
    dataTimesMapType getDataTimesMap();
		string getArgumentName(Expr* arg);
		processWaitEventMapType getProcessWaitEventMap();
		waitNestLoopMapType getWaitNestLoopMap();
		dataNestLoopMapType getDataNestLoopMap();
		
    void dump();
		
  private:
		int iter;
		int loopLevel;
    CXXMethodDecl* _entryMethodDecl;
		forStmtArray _forStmtArray;
    llvm::raw_ostream& _os;
    CallExpr* _waitCall;
    Expr* _firstArg;
		bool _foundWait;
		waitListType _waitCalls;
    waitTimesMapType _waitTimesMap;
		dataTimesMapType _dataTimesMap;
		processWaitEventMapType _processWaitEventMap;
  	waitNestLoopMapType _waitNestLoopMap;
		dataNestLoopMapType _dataNestLoopMap;
		forWaitCallMapType _forWaitCallMap;
	};
}

#endif
