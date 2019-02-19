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

 struct SignalContainer  {
  SignalContainer(string n, FindTemplateTypes *tt, FieldDecl *fd)
      : signal_name_{n}, template_types_{tt}, ast_node_{fd} {}

  ~SignalContainer() {
    // Only thing I create is FindTemplateType. Rest should be deleted by clang.
    //      llvm::errs () << "[[ Destructor SignalContainer ]]\n";
    delete template_types_;
  }

  SignalContainer(const SignalContainer &from) {
    signal_name_ = from.signal_name_;
    template_types_ = new FindTemplateTypes(*(from.template_types_));
    ast_node_ = from.ast_node_;
  }

  void dump(llvm::raw_ostream &os) {
    os << "[SignalContainer " << signal_name_ << " FindTemplateType " << template_types_
       << " FieldDecl " << ast_node_ << "\n";
    template_types_->printTemplateArguments(os);
    os << "]\n";
  }

  FindTemplateTypes *getTemplateTypes() {
    assert(!(template_types_ == nullptr));
    return template_types_;
  }

  FieldDecl *getASTNode() {
    assert(!(ast_node_ == nullptr));
    return ast_node_;
  }

  string getName() { return signal_name_; }

  
 private: 
  string signal_name_;
  FindTemplateTypes *template_types_;
  FieldDecl *ast_node_;
};

class FindSignals : public RecursiveASTVisitor<FindSignals> {
public:
  /// typedefs
  typedef pair<string, SignalContainer *> signalPairType;
  typedef map<string, SignalContainer *> signalMapType;

  FindSignals(CXXRecordDecl *, llvm::raw_ostream &);
  virtual ~FindSignals();

  virtual bool VisitFieldDecl(FieldDecl *);

  signalMapType getSignals() const ;
  void dump();

private:
  llvm::raw_ostream &_os;
  //int state_;
  signalMapType signalcontainer_map_;
};

} // namespace scpar
#endif
