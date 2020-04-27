#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include <string>

#include "json.hpp"

#include "FindSignals.h"

namespace scpar {
using namespace clang;
using json = nlohmann::json;

// Forward declarations.
class FindTemplateTypes;
class SignalContainer;

class Signal {
 public:
  Signal();
  ~Signal();
  Signal(const std::string &, SignalContainer *);

  // Set parameters
  void setModuleName(const std::string &);

  /// Get parameters
  std::string getName();
  FindTemplateTypes *getTemplateTypes();
  FieldDecl *getASTNode();

  // Print
  void dump(llvm::raw_ostream &, int tabn);

  json dump_json(llvm::raw_ostream &);

 private:
  void check();

 private:
  std::string signal_name_;
  SignalContainer *signal_container_;
};
}  // namespace scpar
#endif
