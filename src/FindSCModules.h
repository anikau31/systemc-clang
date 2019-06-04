#ifndef _FIND_SC_MODULES_H_
#define _FIND_SC_MODULES_H_
#include "clang/AST/DeclCXX.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <set>
#include <string>

namespace scpar {
using namespace clang;
using namespace std;

class FindSCModules : public RecursiveASTVisitor<FindSCModules> {
public:
  typedef map<string, CXXRecordDecl *> moduleMapType;
  typedef pair<string, CXXRecordDecl *> modulePairType;

  FindSCModules(TranslationUnitDecl *, llvm::raw_ostream &);
  virtual ~FindSCModules();
  virtual bool VisitCXXRecordDecl(CXXRecordDecl *);
  moduleMapType getSystemCModulesMap();
  void dump();

private:
  llvm::raw_ostream &_os;
  moduleMapType _moduleMap;
    vector<string> template_parameters_;
};
} // namespace scpar
#endif
