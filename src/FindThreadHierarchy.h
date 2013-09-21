#ifndef _FIND_THREAD_HIERARCHY_H_
#define _FIND_THREAD_HIERARCHY_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ExprCXX.h"
#include <map>
namespace scpar {

  using namespace clang;
  using namespace std;
  
  class FindThreadHierarchy: public RecursiveASTVisitor<FindThreadHierarchy> {
  public:

		FindThreadHierarchy(CXXMethodDecl*, llvm::raw_ostream&);
		~FindThreadHierarchy();

		typedef vector<int> blockDim;
		typedef vector<int> gridDim;
		typedef pair<blockDim, gridDim> threadHierarchy;

		typedef pair<ForStmt*, threadHierarchy> forStmtThreadHierarchyPairType;
		typedef map<ForStmt*, threadHierarchy> forStmtThreadHierarchyMapType;

    virtual bool VisitCXXConstructExpr(CXXConstructExpr* expr);    
		virtual bool VisitForStmt(ForStmt *fs);
		forStmtThreadHierarchyMapType returnThreadHierarchy();
    void dump();

  private:

		CXXMethodDecl *_entryMethodDecl;
		ForStmt *_fs;
		blockDim _blockDimensions;
		gridDim _gridDimensions;
		forStmtThreadHierarchyMapType _forStmtThreads;
		llvm::raw_ostream &_os;
	};
}

#endif
