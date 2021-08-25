#ifndef _SPLIT_CFG_H_
#define _SPLIT_CFG_H_

#include <unordered_map>

#include "clang/Analysis/CFG.h"
#include "SplitCFGBlock.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/SmallPtrSet.h"

namespace systemc_clang {
/// ===========================================
/// SplitCFG
/// ===========================================
class SplitCFG {
 private:
  using VectorSplitCFGBlock = llvm::SmallVector<const SplitCFGBlock *>;
  using VectorCFGBlock = llvm::SmallVector<const clang::CFGBlock *>;
  using VectorCFGElementPtr = llvm::SmallVector<const clang::CFGElement *>;

 private:
  /// \brief The context necessary to access translation unit.
  clang::ASTContext &context_;

  /// \brief The saved CFG for a given method.
  std::unique_ptr<clang::CFG> cfg_;

  /// \brief The split blocks in the CFG.
  std::unordered_map<const clang::CFGBlock *, SplitCFGBlock> split_blocks_;

  /// \brief Paths of BBs generated.
  llvm::SmallVector<VectorCFGBlock> paths_found_;
  llvm::SmallVector<llvm::SmallVector<const SplitCFGBlock*>> sb_paths_found_;

  std::unordered_map<unsigned int, SplitCFGBlock*> sccfg_;
  llvm::SmallVector<std::pair<VectorCFGElementPtr, bool> > split_elements;

 private:
  /// \brief Checks if a CFGBlock has a wait() call in it.
  bool isWait(const clang::CFGBlock &block) const;
  bool isElementWait(const clang::CFGElement& element) const;
  void splitBlock(clang::CFGBlock* block);
  void addSuccessors(SplitCFGBlock* to, const clang::CFGBlock* from);
  void addPredecessors(SplitCFGBlock* to, const clang::CFGBlock* from);
  //void updateSuccessors();
  void createUnsplitBlocks();
  void dumpSplitElements(const llvm::SmallVector<std::pair<VectorCFGElementPtr, bool> > &split_elements );
void dumpSCCFG();

 public:
  SplitCFG(clang::ASTContext &context);
  SplitCFG(clang::ASTContext &context, const clang::CXXMethodDecl *cxx_decl);
  ~SplitCFG();

  const llvm::SmallVector<VectorSplitCFGBlock>& getPathsFound(); 
  void construct_sccfg(const clang::CXXMethodDecl* method);
  void split_wait_blocks(const clang::CXXMethodDecl *cxx_decl);
  void sb_dfs_pop_on_wait(
      const SplitCFGBlock *BB,
      llvm::SmallVectorImpl<const SplitCFGBlock *> &waits_in_stack,
      llvm::SmallPtrSetImpl<const SplitCFGBlock *> &visited_waits);


  void dfs_pop_on_wait(
      const clang::CFGBlock *BB,
      llvm::SmallVectorImpl<const clang::CFGBlock *> &waits_in_stack,
      llvm::SmallPtrSetImpl<const clang::CFGBlock *> &visited_waits);

  void sb_generate_paths();
  void generate_paths();
  void dump() const;
};

};  // namespace systemc_clang

#endif /* _SPLIT_CFG_H_ */
