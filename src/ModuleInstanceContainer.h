#ifndef _FIND_MODULE_INSTANCE_CONTAINER_H_
#define _FIND_MODULE_INSTANCE_CONTAINER_H_

#include <map>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"

namespace scpar {
  using namespace clang;
  using namespace std;
  
  class ModuleInstanceContainer {
    public:
      ModuleInstanceContainer();
      ModuleInstanceContainer(string, string, CXXConstructExpr* );

      ~ModuleInstanceContainer();

      // Copy constructor.
      ModuleInstanceContainer(const ModuleInstanceContainer& );
      string getName();
      string getModuleType();
      CXXConstructExpr* getASTNode();
    
    void dump(raw_ostream&, int);

    private:
      string _name;
      string _moduleType;
      CXXConstructExpr* _astNode;
  };
}
#endif
