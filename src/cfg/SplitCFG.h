#ifndef _SPLIT_CFG_H_
#define _SPLIT_CFG_H_

#include "clang/Analysis/CFG.h"
#include "SplitCFGBlock.h"
#include <vector>
#include <unordered_map>

namespace systemc_clang {
/// ===========================================
/// SplitCFG
/// ===========================================
class SplitCFG {
 private:
  std::unordered_map<const clang::CFGBlock *, SplitCFGBlock> split_blocks_;
  clang::ASTContext &context_;
  std::unique_ptr<clang::CFG> cfg_;
  bool isWait(const clang::CFGBlock& block) const;
 public:
  SplitCFG(clang::ASTContext &context);
  void split_wait_blocks(const clang::CXXMethodDecl *cxx_decl);
  void dfs_pop_on_wait(const clang::CFGBlock* BB);
  void dfs();
  void dfs( const clang::CFGBlock *BB);
  void dump() const;
};

};  // namespace systemc_clang

#endif /* _SPLIT_CFG_H_ */
