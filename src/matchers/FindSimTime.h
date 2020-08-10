#ifndef _FIND_SIM_TIME_H_
#define _FIND_SIM_TIME_H_

#include "Utility.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
namespace systemc_clang {

using namespace clang;
using namespace std;

class FindSimTime : public RecursiveASTVisitor<FindSimTime>, public Utility {
public:
  typedef pair<string, string> simulationTimePairType;
  typedef map<string, string> simulationTimeMapType;

  FindSimTime(FunctionDecl *, llvm::raw_ostream &);
  virtual ~FindSimTime();
  virtual bool VisitCallExpr(CallExpr *c);

  simulationTimeMapType returnSimTime();

private:
  llvm::raw_ostream &os_;
  //    FunctionDecl *_sigInst;
  simulationTimeMapType simulation_time_;
  //    CallExpr *call_expr_;
};
} // namespace systemc_clang
#endif
