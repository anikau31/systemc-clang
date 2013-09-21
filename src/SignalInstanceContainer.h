#ifndef _SIGNAL_INSTANCE_CONTAINER_H_
#define _SIGNAL_INSTANCE_CONTAINER_H_

#include <map>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "FindTemplateTypes.h"

namespace scpar {
  using namespace clang;
  using namespace std;
  
  class SignalInstanceContainer {
    public:
      SignalInstanceContainer();
      SignalInstanceContainer(string, string, FindTemplateTypes*, CXXConstructExpr* );

      ~SignalInstanceContainer();

      // Copy constructor.
      SignalInstanceContainer(const SignalInstanceContainer& );
      string getName();
      string getType();
    FindTemplateTypes* getTemplateTypes();
      CXXConstructExpr* getASTNode();
    
    void dump(raw_ostream&, int tabn = 0);

    private:
      string _name;
      string _signalType;
    FindTemplateTypes* _templateType;
      CXXConstructExpr* _astNode;
  };
}
#endif
