#ifndef _PORT_DECL_H_
#define _PORT_DECL_H_

#include "systemc-clang.h"
#include "json.hpp"

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
  PortDecl(const string &, FindTemplateTypes *);

  PortDecl(const PortDecl &);

  ~PortDecl();
  // Set parameters
  void setModuleName(const string &);

  /// Get parameters
  string getName() const;
  FindTemplateTypes *getTemplateType();

  // Print
  void dump(raw_ostream &, int tabn = 0);

  json dump_json();
  
private:
  string port_name_;
  FindTemplateTypes *template_type_;
};
} // namespace scpar
#endif
