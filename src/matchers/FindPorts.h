#ifndef _FIND_PORTS_H_
#define _FIND_PORTS_H_

#include "FindTemplateTypes.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include <map>

namespace scpar {
using namespace clang;
using namespace std;

class FindPorts : public RecursiveASTVisitor<FindPorts> {
public:
  /// typedefs
  typedef map<string, FindTemplateTypes *> PortType;
  typedef pair<string, FindTemplateTypes *> kvType;

  FindPorts(CXXRecordDecl *, llvm::raw_ostream &);
  virtual ~FindPorts();
  virtual bool VisitFieldDecl(FieldDecl *);
  // This is necessary for it to traverse template implicit/explicit specializations
  bool shouldVisitTemplateInstantiations();

  PortType getInputPorts() const;
  PortType getOutputPorts() const;
  PortType getInputOutputPorts() const;
  PortType getInStreamPorts() const;
  PortType getOutStreamPorts() const;
  PortType getOtherVars() const;
  void dump();

private:
  llvm::raw_ostream &os_;
  PortType inPorts_;
  PortType outPorts_;
  PortType inStreamPorts_;
  PortType outStreamPorts_;
  PortType inoutPorts_;
  PortType otherVars_;
};

} // namespace scpar
#endif
