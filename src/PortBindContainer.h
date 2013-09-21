#ifndef _PORT_BIND_CONTAINER_H_
#define _PORT_BIND_CONTAINER_H_

#include <map>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"

namespace scpar {
  using namespace clang;
  using namespace std;
  
  class PortBindContainer {
  public:
    PortBindContainer();
    PortBindContainer(string, string, string, CXXOperatorCallExpr* );
    
    ~PortBindContainer();
    
    // Copy constructor.
    PortBindContainer(const PortBindContainer& );
    
    string getPortName();
    string getModuleInstanceName();
    string getSignalName();
    
    CXXOperatorCallExpr* getASTNode();
    
    void dump(raw_ostream& os, int tabn =0);
    
  private:
    // Name of port on which the binding happens.
    string _portName;
    // Name of the module instance.
    string _moduleInstanceName;
    // Name of the signal that is bound to the port of that instance.
    string _signalName;
    
    CXXOperatorCallExpr* _astNode;
  };
}

#endif
