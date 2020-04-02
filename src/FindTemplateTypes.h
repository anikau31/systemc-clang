#ifndef _FIND_TEMPLATE_TYPES_H_
#define _FIND_TEMPLATE_TYPES_H_

#include <string>
#include <vector>
#include <stack>
#include "json.hpp"

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Type.h"
#include "llvm/Support/raw_ostream.h"


#include "Tree.h"

namespace scpar {
using namespace clang;
using json = nlohmann::json;

// Forward declarations
//

// This class holds the name of the type, and a pointer to the
// type object.
class TemplateType {
 public:
  // Default constructor.
  TemplateType();

  // Overloaded constructor
  TemplateType(std::string, const Type *);
  ~TemplateType();
  TemplateType(const TemplateType &);

  std::string getTypeName() const;
  std::string toString() const;
  const Type *getTypePtr() const;

 private:
  std::string type_name_;
  const Type *type_ptr_;
};

// This class is going to find the arguments from templates
class FindTemplateTypes : public RecursiveASTVisitor<FindTemplateTypes> {
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

  bool VisitIntegerLiteral(IntegerLiteral *l);
  bool VisitTemplateSpecializationType(
      TemplateSpecializationType *special_type);
  bool VisitTypedefType(TypedefType *typedef_type);
  bool VisitCXXRecordDecl(CXXRecordDecl *cxx_type);
  bool VisitRecordType(RecordType *rt);
  bool VisitBuiltinType(BuiltinType *bi_type);

  ~FindTemplateTypes();
  type_vector_t Enumerate(const Type *type);
  type_vector_t getTemplateArgumentsType();
  void printTemplateArguments(llvm::raw_ostream &os);
  json dump_json();
  std::vector<std::string> getTemplateArguments();
  size_t size();
  Tree<TemplateType> *getTemplateArgTreePtr();

 private:
  // (std::string, Type*)
  // Classes such as sc_port and sc_in can have nested types within it.
  // For example: sc_in< sc_int<16> >
  // The general way to handle this would be to have a vector starting from the
  // outside type to the inside type.

  type_vector_t template_types_;

  Tree<TemplateType> template_args_;
  Tree<TemplateType>::TreeNodePtr current_type_node_;
  std::stack<Tree<TemplateType>::TreeNodePtr> stack_current_node_;
};
}  // namespace scpar
#endif
