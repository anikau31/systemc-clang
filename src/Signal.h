#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include "clang/AST/DeclCXX.h"
#include <map>
#include <string>

#include "systemc-clang.h"
#include "json.hpp"

#include "FindSignals.h"
#include "FindTemplateTypes.h"

namespace scpar {
using namespace clang;
using namespace std;
 using json = nlohmann::json;

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

  json dump_json(raw_ostream &);

private:
  void check();

private:
  string signal_name_;
  SignalContainer * signal_container_;
};
} // namespace scpar
#endif
