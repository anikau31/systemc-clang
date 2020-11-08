#ifndef _GET_AST_INFO_
#define _GET_AST_INFO_

#include <vector>
#include "clang/AST/Type.h"
#include "clang/AST/DeclCXX.h"

#undef DEBUG_TYPE
#define DEBUG_TYPE "GetASTInfo"

namespace sc_ast_matchers {

typedef std::vector<llvm::APInt> ArraySizesType;

class GetASTInfo {
 private:
  GetASTInfo(){};

 public:
  static ArraySizesType getConstantArraySizes(const clang::FieldDecl *fd) {
    ArraySizesType sizes;

    clang::QualType field_type{fd->getType()};

    auto array_type{clang::dyn_cast<clang::ConstantArrayType>(field_type)};
    while (array_type != nullptr) {
      llvm::APInt array_size{};
      array_size = array_type->getSize();
      LLVM_DEBUG(llvm::dbgs() << "Size of array: " << array_size << "\n";);
      array_type = clang::dyn_cast<clang::ConstantArrayType>(
          array_type->getElementType());

      sizes.push_back(array_size);
    }

    return sizes;
  }
};

};  // namespace sc_ast_matchers

#endif
