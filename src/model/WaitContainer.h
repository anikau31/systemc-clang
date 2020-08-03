#ifndef _WAIT_CONTAINER_H_
#define _WAIT_CONTAINER_H_

#include "clang/AST/DeclCXX.h"
#include "clang/AST/PrettyPrinter.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <string>

namespace systemc_clang {
using namespace clang;
using namespace std;

class WaitContainer {
public:
  // typedefs
  typedef vector<string> waitArgVectorType;

  WaitContainer();
  WaitContainer(CXXMethodDecl *, CallExpr *);

  ~WaitContainer();

  // Copy constructor.
  WaitContainer(const WaitContainer &);

  unsigned int getNumArgs();
  CXXMethodDecl *getEntryMethod();
  CallExpr *getASTNode();
  waitArgVectorType getArgList();

  void dump(raw_ostream &, int tabn = 0);

private:
  void populateArgMap();
  string getArgString(Expr *e);

private:
  CXXMethodDecl *_entryMethodDecl;
  CallExpr *_astNode;
  unsigned int _numArgs;

  waitArgVectorType _args;
};
} // namespace systemc_clang
#endif
