#ifndef _FIND_GLOBAL_EVENTS_H_
#define _FIND_GLOBAL_EVENTS_H_
//#include "systemc-clang.h"

#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/raw_ostream.h"

#include <map>
#include <string>
#include <vector>

/// Clang forward declarations
namespace clang {
class VarDecl;
class TranslationUnitDecl;
};

namespace systemc_clang {

class FindGlobalEvents : public clang::RecursiveASTVisitor<FindGlobalEvents> {
 public:
  typedef std::map<std::string, clang::VarDecl *> globalEventMapType;
  typedef std::pair<std::string, clang::VarDecl *> kvType;

  FindGlobalEvents(clang::TranslationUnitDecl *, llvm::raw_ostream &);
  virtual ~FindGlobalEvents();

  virtual bool VisitVarDecl(clang::VarDecl *);

  globalEventMapType getEventMap();
  std::vector<std::string> getEventNames();

  void dump();
  std::string asString() const;

 private:
  llvm::raw_ostream &_os;
  globalEventMapType _globalEvents;
};
}  // namespace systemc_clang
#endif
