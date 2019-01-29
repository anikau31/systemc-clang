#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include "clang/AST/DeclCXX.h"
#include <map>
#include <string>

#include "FindSignals.h"
#include "FindTemplateTypes.h"

namespace scpar {
using namespace clang;
using namespace std;

class Signal {
public:
  Signal();
  Signal(const string &, SignalContainer *);

  // Set parameters
  void setModuleName(const string &);

  /// Get parameters
  string getName();
  FindTemplateTypes *getTemplateTypes();
  FieldDecl *getASTNode();

  // Print
  void dump(raw_ostream &, int tabn);

private:
  void check();

private:
  string _name;
  SignalContainer *_sig;
};
} // namespace scpar
#endif
