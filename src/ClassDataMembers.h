//===-- src/ClassDataMembers.h - systec-clang class definition -------*- C++ -*-===//
//
//                     systemc-clang: SystemC Parser
//
// This file is distributed under the University of Illinois License.
// See LICENSE.mkd for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Stores the data members found in a class.
///
//===----------------------------------------------------------------------===//
#ifndef _CLASS_DATA_MEMBERS_H_
#define _CLASS_DATA_MEMBERS_H_

#include "clang/AST/DeclCXX.h"
#include <string>
#include <map>
#include "Utility.h"

namespace scpar {
  using namespace clang;
  using namespace std;
  
  class ClassDataMembers {
  public:

    // Typedefs.
    typedef pair<string, FieldDecl*> dataMemberPairType;
    typedef map<string, FieldDecl*> dataMemberMapType;
    
    // Constructors.
    ClassDataMembers();
    ClassDataMembers(const string&, double size, FieldDecl* );

    // Copy constructor.
    ClassDataMembers(const ClassDataMembers& );

    // Destructor.
    ~ClassDataMembers();
    
    /// Get parameters
		string _name;
    double getSize();
    FieldDecl* getASTNode();

    // Print
    void dump(raw_ostream&, int tabn=0 );
    void ptrCheck();
  private:
    FieldDecl* _astNode;
    double _size;
  };
}
#endif
