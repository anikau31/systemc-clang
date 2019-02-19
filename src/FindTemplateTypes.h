#ifndef _FIND_TEMPLATE_TYPES_H_
#define _FIND_TEMPLATE_TYPES_H_

#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Type.h"
#include "llvm/Support/raw_ostream.h"
#include <string>
#include <vector>

#include <iostream>

namespace scpar {
  using namespace clang;
  using namespace std;

  // This class holds the name of the type, and a pointer to the
  // type object.
  class TemplateType {
  public:
    TemplateType( string, const Type* );
    ~TemplateType( );
    TemplateType( const TemplateType& );

    string getTypeName() const;
    const Type* getTypePtr();

  private:
    string type_name_;
    const Type * type_ptr_;
  };

  // This class is going to find the arguments from templates
  class FindTemplateTypes : public RecursiveASTVisitor<FindTemplateTypes> {
  public:
    /// Typedefs
    typedef TemplateType TemplateTypePtr;
    //typedef vector< TemplateTypePtr > type_vector_t_new;
    //typedef vector<pair<string, const Type *>> type_vector_t;
    typedef vector< TemplateTypePtr > type_vector_t;
    typedef vector< TemplateTypePtr > argVectorType;
    //typedef vector<pair<string, const Type *>> argVectorType;

    // Constructor
    FindTemplateTypes();

    /// Copy constructor
    FindTemplateTypes(const FindTemplateTypes &rhs);
    FindTemplateTypes(const FindTemplateTypes *rhs);

    ~FindTemplateTypes();
    string getTemplateType();
    bool VisitType(Type *type);
    bool VisitIntegerLiteral(IntegerLiteral *l);
    type_vector_t Enumerate(const Type *type);
    type_vector_t getTemplateArgumentsType();
    void printTemplateArguments(llvm::raw_ostream &os);
    vector<string> getTemplateArguments();
    size_t size();

  private:
    // (string, Type*)
    // Classes such as sc_port and sc_in can have nested types within it.
    // For example: sc_in< sc_int<16> >
    // The general way to handle this would be to have a vector starting from the
    // outside type to the inside type. 

    type_vector_t template_types_;
    //type_vector_t_new template_types_new_;
  };
} // namespace scpar
#endif
