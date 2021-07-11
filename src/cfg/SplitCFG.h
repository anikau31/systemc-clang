#ifndef _SPLIT_CFG_H_
#define _SPLIT_CFG_H_

#include "clang/Analysis/CFG.h"
#include <vector>
#include <unordered_map>

namespace systemc_clang {
class SplitCFGBlock {
 private:
  clang::CFGBlock *block_;
  bool has_wait_;
  std::vector<std::pair<unsigned int, unsigned int> > split_elements_;

 private:
  bool isWait(const clang::CFGElement &element) const;

 public:
  SplitCFGBlock();
  SplitCFGBlock(const SplitCFGBlock &from);

  clang::CFGBlock *getCFGBlock() const;
  std::size_t getSplitBlockSize() const;

  void split_block(clang::CFGBlock *block);

  void dump() const;
};

/// ===========================================
/// SplitCFG
/// ===========================================
class SplitCFG {
 private:
   /// Block ID, SplitBlcokCFG
  std::unordered_map<unsigned int, SplitCFGBlock> split_blocks_;
  clang::ASTContext &context_;
  std::unique_ptr<clang::CFG> cfg_;

 public:
  SplitCFG(clang::ASTContext &context);
  void split_wait_blocks(const clang::CXXMethodDecl *cxx_decl);
  void dump() const;
};

};  // namespace systemc_clang

#endif /* _SPLIT_CFG_H_ */
