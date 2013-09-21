#ifndef _PROCESSDECL_H_
#define _PROCESSDECL_H_

#include <string>
#include "clang/AST/DeclCXX.h"

namespace scpar {

  using namespace std;
  using namespace clang;
  
  class SensitivityDecl {
    
  public:
    SensitivityDecl(string name);
    SensitivityDecl(const SensitivityDecl& from);
    
    /// Get methods.
    string getName();
    
  protected:
    string _name;
  };
  
};

#endif
