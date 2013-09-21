#ifndef _CLASS_MEMBER_FUNCTIONS_H_
#define _CLASS_MEMBER_FUNCTIONS_H_

#include "clang/AST/DeclCXX.h"
#include <string>
#include <map>
#include "Utility.h"

namespace scpar {
  using namespace clang;
  using namespace std;
  
  class ClassMemberFunctions {
  public:
    // Typedefs.
    typedef pair<string, CXXMethodDecl*> memberFunctionPairType;
    typedef map<string, CXXMethodDecl*> memberFunctionMapType;
    
    // Constructors.
    ClassMemberFunctions();
    ClassMemberFunctions(const string&, CXXMethodDecl* );

    // Copy constructor.
    ClassMemberFunctions(const ClassMemberFunctions& );

    // Destructor.
    ~ClassMemberFunctions();
    
    /// Get parameters
    string getName();
    CXXMethodDecl* getASTNode();
    // Print
    void dump(raw_ostream&, int tabn=0 );
    void ptrCheck();
  private:
    string _name;
    CXXMethodDecl* _astNode;
  };
}
#endif
