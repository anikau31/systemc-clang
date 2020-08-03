#ifndef _SIGNAL_DECL_H_
#define _SIGNAL_DECL_H_

#include <string>

#include "json.hpp"

#include "PortDecl.h"

namespace systemc_clang {

using json = nlohmann::json;

/// Forward declarations
class FindTemplateTypes;

class SignalDecl : public PortDecl {
 public:
  SignalDecl();
  virtual ~SignalDecl();
  SignalDecl(const std::string &name, clang::FieldDecl *fd,
             FindTemplateTypes *tt);

  /// Get parameters
  std::string getName();
  FindTemplateTypes *getTemplateTypes();
  clang::FieldDecl *getASTNode();

  json dump_json();
};
}  // namespace systemc_clang
#endif
