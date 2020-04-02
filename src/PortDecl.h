#ifndef _PORT_DECL_H_
#define _PORT_DECL_H_

#include "json.hpp"

#include <map>
#include <string>

#include "clang/AST/DeclCXX.h"

namespace scpar {
using namespace clang;
using json = nlohmann::json;

// Forward declarations
//
class FindTemplateTypes;

class PortDecl {
 public:
  PortDecl();
  PortDecl(const std::string &, FindTemplateTypes *);
  PortDecl(const std::string &, const Decl *, FindTemplateTypes *);

  PortDecl(const PortDecl &);

  ~PortDecl();
  // Set parameters
  void setModuleName(const std::string &);
  void setBinding(VarDecl *vd);

  /// Get parameters
  std::string getName() const;
  FieldDecl *getFieldDecl() const;
  VarDecl *getAsVarDecl() const;
  FindTemplateTypes *getTemplateType();

  // Print
  void dump(llvm::raw_ostream &, int tabn = 0);

  json dump_json(llvm::raw_ostream &);

 private:
  // Name of the port
  std::string port_name_;
  // This holds the types for the port declaration
  FindTemplateTypes *template_type_;
  Decl *field_decl_;

};
}  // namespace scpar
#endif
