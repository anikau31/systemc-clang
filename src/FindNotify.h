#ifndef _FIND_NOTIFY_H_
#define _FIND_NOTIFY_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <map>
namespace scpar {

  using namespace clang;
  using namespace std;
  
  class FindNotify: public RecursiveASTVisitor<FindNotify> {
  public:
    typedef vector<CallExpr* > notifyCallListType;		
		typedef pair<CXXMethodDecl*, vector<string> > processNotifyEventPairType;
		typedef map <CXXMethodDecl*, vector<string> > processNotifyEventMapType;
    
    FindNotify(CXXMethodDecl*, raw_ostream&);
		~FindNotify();  
    virtual bool VisitCallExpr(CallExpr* expr);     
    string getArgumentName(Expr* arg);
		notifyCallListType getNotifyCallList();
		processNotifyEventMapType getProcessNotifyEventMap();
    void dump();

  private:
    CXXMethodDecl* _entryMethodDecl;
    raw_ostream& _os;
    CallExpr* _notifyCall;
		processNotifyEventMapType _processNotifyEventMap;
    notifyCallListType _notifyCalls;

  };
}

#endif
