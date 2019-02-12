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

// This class is going to find the arguments from templates
class FindTemplateTypes : public RecursiveASTVisitor<FindTemplateTypes> {
public:
  /// Typedefs
  typedef vector<pair<string, const Type *>> type_vector_t;
  typedef vector<pair<string, const Type *>> argVectorType;

  // Constructor
  FindTemplateTypes();

  /// Copy constructor
  FindTemplateTypes(const FindTemplateTypes &rhs);
  FindTemplateTypes(const FindTemplateTypes *rhs);
  string getTemplateType();
  type_vector_t Enumerate(const Type *type);
  bool VisitType(Type *type);
  bool VisitIntegerLiteral(IntegerLiteral *l);
  type_vector_t getTemplateArgumentsType();
  void printTemplateArguments(llvm::raw_ostream &os);
  vector<string> getTemplateArguments();
  size_t size();

private:
  // (string, Type*)
  type_vector_t template_types_;
};
} // namespace scpar
#endif
