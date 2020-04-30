#ifndef _SIGNAL_DECL_H_
#define _SIGNAL_DECL_H_

#include <string>

#include "json.hpp"

#include "PortDecl.h"
#include "FindTemplateTypes.h"

#include "llvm/Support/raw_ostream.h"

namespace scpar {
using namespace clang;
using json = nlohmann::json;

// Forward declarations.

class SignalDecl : public PortDecl {
 public:
  SignalDecl();
  virtual ~SignalDecl();
  SignalDecl(const std::string &name, clang::FieldDecl *fd, FindTemplateTypes *tt);

  // Set parameters
  // void setModuleName(const std::string &);

  /// Get parameters
  std::string getName();
  FindTemplateTypes *getTemplateTypes();
  clang::FieldDecl *getASTNode();

  json dump_json(llvm::raw_ostream &);
};
}  // namespace scpar
#endif
