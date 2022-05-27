#ifndef _FIND_TEMPLATE_TYPES_H_
#define _FIND_TEMPLATE_TYPES_H_

#include <string>
#include <vector>
#include <stack>

#include "clang/AST/RecursiveASTVisitor.h"

#include "Tree.h"

#undef DEBUG_TYPE
#define DEBUG_TYPE "Templates"

namespace clang {
class Type;
}

namespace systemc_clang {
// using namespace clang;

// Forward declarations
//

// This class holds the name of the type, and a pointer to the
// type object.
class TemplateType {
 public:
  // Default constructor.
  TemplateType();

  // Overloaded constructor
  TemplateType(std::string, const clang::Type *);
  virtual ~TemplateType();
  TemplateType(const TemplateType &);

  std::string getTypeName() const;
  std::string toString() const;

  const clang::Type *getTypePtr() const;
  void dump();

 private:
  std::string type_name_;
  const clang::Type *type_ptr_;
};

// This class is going to find the arguments from templates
class FindTemplateTypes : public clang::RecursiveASTVisitor<FindTemplateTypes> {
 public:
  /// Typedefs
  typedef TemplateType TemplateTypePtr;
  typedef std::vector<TemplateTypePtr> type_vector_t;
  typedef std::vector<TemplateTypePtr> argVectorType;

  // typedef tree< TemplateType > template_arguments_type;
  // Constructor
  FindTemplateTypes();

  /// Copy constructor
  FindTemplateTypes(const FindTemplateTypes &rhs);
  FindTemplateTypes(const FindTemplateTypes *rhs);

  // This allows for template instantiations to be visited using RAV.
  bool shouldVisitTemplateInstantiations() const;

  bool VisitEnumType(clang::EnumType *e);
  bool VisitIntegerLiteral(clang::IntegerLiteral *l);
  bool VisitTemplateSpecializationType(
      clang::TemplateSpecializationType *special_type);
  bool VisitTypedefType(clang::TypedefType *typedef_type);
  bool VisitCXXRecordDecl(clang::CXXRecordDecl *cxx_type);
  bool VisitRecordType(clang::RecordType *rt);
  bool VisitBuiltinType(clang::BuiltinType *bi_type);
bool VisitDependentNameType(clang::DependentNameType *type);

  bool VisitDeclRefExpr(clang::DeclRefExpr *dre);

  virtual ~FindTemplateTypes();
  void Enumerate(const clang::Type *type);
  // type_vector_t getTemplateArgumentsType();
  // std::vector<std::string> getTemplateArguments();
  Tree<TemplateType> *getTemplateArgTreePtr();
  // size_t size();

  void printTemplateArguments(llvm::raw_ostream &os);

  /// Returns the TemplateType data as a std::string.
  std::string asString();

 private:
  // (std::string, Type*)
  // Classes such as sc_port and sc_in can have nested types within it.
  // For example: sc_in< sc_int<16> >
  // The general way to handle this would be to have a vector starting from the
  // outside type to the inside type.

  // type_vector_t template_types_;

  Tree<TemplateType> template_args_;
  Tree<TemplateType>::TreeNodePtr current_type_node_;
  std::stack<Tree<TemplateType>::TreeNodePtr> stack_current_node_;
};
}  // namespace systemc_clang
#endif
