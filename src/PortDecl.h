#ifndef _PORT_DECL_H_
#define _PORT_DECL_H_

#include "json.hpp"
#include "systemc-clang.h"

#include <map>
#include <string>

#include "FindTemplateTypes.h"
#include "clang/AST/DeclCXX.h"

namespace scpar {
using namespace clang;
using namespace std;
using json = nlohmann::json;

class PortDecl {
 public:
  PortDecl();
  PortDecl(const std::string &, FindTemplateTypes *);
  PortDecl(const std::string &, const Decl *, FindTemplateTypes *);

  PortDecl(const PortDecl &);

  ~PortDecl();
  // Set parameters
  void setModuleName(const std::string &);

  /// Get parameters
  std::string getName() const;
  FieldDecl *getFieldDecl() const;
  VarDecl *getAsVarDecl() const;
  FindTemplateTypes *getTemplateType();

  // Print
  void dump(raw_ostream &, int tabn = 0);

  json dump_json(raw_ostream &);

 private:
  // Name of the port
  string port_name_;
  // This holds the types for the port declaration
  FindTemplateTypes *template_type_;
  Decl *field_decl_;

  // This is the information for binding this port.
  // The variable it is bound to.
  VarDecl *bound_vardecl_;
  std::string bound_var_name_;

};
}  // namespace scpar
#endif
