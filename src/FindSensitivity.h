#ifndef _FIND_SENSITIVITY_H_
#define _FIND_SENSITIVITY_H_

#include <map>
#include <string>
#include <tuple>
#include "clang/AST/DeclCXX.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/raw_ostream.h"
// ANI : A module may have multiple threads and their corresponding
// sensitivity lists. So, this function should of finding sensitivity list
// should take into account that aspect.
namespace scpar {
using namespace clang;
using namespace std;

class FindSensitivity : public RecursiveASTVisitor<FindSensitivity> {
 public:
  typedef map<string, tuple<string, MemberExpr *> > senseMapType;
  typedef pair<string, tuple<string, MemberExpr *> > kvType;

  FindSensitivity(Stmt *d, llvm::raw_ostream &os);
  virtual ~FindSensitivity();

  virtual bool VisitMemberExpr(MemberExpr *);

  void dump();
  senseMapType getSenseMap();

 private:
  llvm::raw_ostream &os_;
  bool found_sensitive_node_;
  std::string clk_edge_;
  senseMapType sensitive_ports_;
};

}  // namespace scpar
#endif
