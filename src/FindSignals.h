#ifndef _FIND_SIGNALS_H_
#define _FIND_SIGNALS_H_

#include "FindTemplateTypes.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include <map>

namespace scpar {
using namespace clang;
using namespace std;

struct SignalContainer {
  SignalContainer(string n, FindTemplateTypes *tt, FieldDecl *fd)
      : _name{n}, _template{tt}, _astNode{fd} {}

  ~SignalContainer() {
    // Only thing I create is FindTemplateType. Rest should be deleted by clang.
    //      llvm::errs () << "[[ Destructor SignalContainer ]]\n";
    delete _template;
  }

  SignalContainer(const SignalContainer &from) {
    _name = from._name;
    _template = new FindTemplateTypes(*(from._template));
    _astNode = from._astNode;
  }

  void dump(llvm::raw_ostream &os) {
    os << "[SignalContainer " << _name << " FindTemplateType " << _template
       << " FieldDecl " << _astNode << "\n";
    _template->printTemplateArguments(os);
    os << "]\n";
  }

  FindTemplateTypes *getTemplateTypes() {
    assert(!(_template == nullptr));
    return _template;
  }

  FieldDecl *getASTNode() {
    assert(!(_astNode == nullptr));
    return _astNode;
  }

  string getName() { return _name; }

  // FIXME: Make these protected.
  string _name;
  FindTemplateTypes *_template;
  FieldDecl *_astNode;
};

class FindSignals : public RecursiveASTVisitor<FindSignals> {
public:
  /// typedefs
  typedef pair<string, SignalContainer *> signalPairType;
  typedef map<string, SignalContainer *> signalMapType;

  FindSignals(CXXRecordDecl *, llvm::raw_ostream &);
  virtual ~FindSignals();

  virtual bool VisitFieldDecl(FieldDecl *);

  signalMapType *getSignals();
  void dump();

private:
  llvm::raw_ostream &_os;
  int state;
  signalMapType *_signals;
};

} // namespace scpar
#endif
