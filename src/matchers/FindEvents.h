#ifndef _FIND_EVENTS_H_
#define _FIND_EVENTS_H_

#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <string>
#include <vector>

namespace clang {
  class FieldDecl;
  class CXXRecordDecl;
};

namespace systemc_clang {

class FindEvents : public clang::RecursiveASTVisitor<FindEvents> {
public:
  /// typedefs
  typedef std::map<std::string, clang::FieldDecl *> classEventMapType;
  typedef std::pair<std::string, clang::FieldDecl *> kvType;

  FindEvents(clang::CXXRecordDecl *, llvm::raw_ostream &);
  FindEvents(const FindEvents &);
  virtual ~FindEvents();
  /// RecursiveASTVisitor methods
  virtual bool VisitFieldDecl(clang::FieldDecl *);

  /// Access methods
  classEventMapType getInClassEvents();
  std::vector<std::string> getEventNames();

  /// Print methods
  void dump();

private:
  llvm::raw_ostream &os_;
  classEventMapType _inClassEvents;
  //    reflectionDataStructure * _reflectionMap;
};

} // namespace systemc_clang
#endif
