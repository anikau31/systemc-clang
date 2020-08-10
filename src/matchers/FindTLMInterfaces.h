#ifndef _FIND_TLM_INTERFACES_H_
#define _FIND_TLM_INTERFACES_H_

#include "FindTemplateTypes.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <map>

namespace systemc_clang {
using namespace clang;

class FindTLMInterfaces : public RecursiveASTVisitor<FindTLMInterfaces> {
public:
  // / typedefs
  typedef std::map<string, FindTemplateTypes *> interfaceType;
  typedef std::pair<string, FindTemplateTypes *> kvType;

  FindTLMInterfaces(CXXRecordDecl *, llvm::raw_ostream &);
  virtual ~FindTLMInterfaces();

  virtual bool VisitFieldDecl(FieldDecl *);

  // ANI : Identifying only FIFO interfaces.
  // Need to add the other interfaces
  interfaceType getInputInterfaces();
  interfaceType getOutputInterfaces();
  interfaceType getInputOutputInterfaces();

  void dump();

private:
  llvm::raw_ostream &_os;
  interfaceType _inInterfaces;
  interfaceType _outInterfaces;
  interfaceType _inoutInterfaces;
};
} // namespace systemc_clang
#endif
