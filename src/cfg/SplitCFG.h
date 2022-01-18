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
 public:
  using VectorSplitCFGBlock = llvm::SmallVector<const SplitCFGBlock *>;
  using VectorSplitCFGBlockImpl = llvm::SmallVector<const SplitCFGBlock *>;
  using VectorCFGElementPtrImpl =
      llvm::SmallVectorImpl<const clang::CFGElement *>;
  using VectorCFGElementPtr = llvm::SmallVector<const clang::CFGElement *>;

 private:
  /// \brief The context necessary to access translation unit.
  clang::ASTContext &context_;

  /// \brief The saved CFG for a given method.
  std::unique_ptr<clang::CFG> cfg_;

  /// \brief The split blocks in the CFG.
  std::unordered_map<const clang::CFGBlock *, SplitCFGBlock> split_blocks_;

  /// \brief Paths of BBs generated.
  llvm::SmallVector<llvm::SmallVector<const SplitCFGBlock *>> paths_found_;

  /// \brief The block id to block for SCCFG.
  std::unordered_map<unsigned int, SplitCFGBlock *> sccfg_;

  llvm::SmallVector<std::pair<VectorCFGElementPtrImpl, bool>> split_elements;

  /// Predecessor SplitCFGBlock* => (Wait SplitCFGBlock*)
  std::unordered_map<const SplitCFGBlock *,
                     std::pair<const SplitCFGBlock *, unsigned int>>
      wait_next_state_;

  unsigned int next_state_count_;

 private:
  /// \brief Checks if a CFGBlock has a wait() call in it.
  bool isElementWait(const clang::CFGElement &element) const;

  /// \brief Split a CFGBlock into respective SplitCFGBlock if the CFGBlock has
  /// wait statements in it.
  void splitBlock(clang::CFGBlock *block);

  /// \brief Add successors to the SplitCFGBlock.
  void addSuccessors(SplitCFGBlock *to, const clang::CFGBlock *from);

  /// \brief Add predecessors to the SplitCFGBlock.
  void addPredecessors(SplitCFGBlock *to, const clang::CFGBlock *from);

  void addNextStatesToBlocks();

  /// \brief Creates SplitCFGBlocks for all CFGBlocks that do not have a wiat.
  /// splitBlock() creates the SplitCFGBlocks used for splitting CFGBLocks that
  /// have wait statements.
  void createUnsplitBlocks();

  /// \brief Creates the SplitCFGBlocks for CFGBlock with a wait.
  void createWaitSplitCFGBlocks(
      clang::CFGBlock *block,
      const llvm::SmallVectorImpl<std::pair<VectorCFGElementPtr, bool>>
          &split_elements);

  /// \brief Dump all the CFGElements that were split.
  void dumpSplitElements(
      const llvm::SmallVector<std::pair<VectorCFGElementPtr, bool>>
          &split_elements) const;

  void dumpSCCFG() const;

 public:
  /// \brief Constructor.
  SplitCFG(clang::ASTContext &context);
  /// \brief  Overloaded constructor.
  SplitCFG(clang::ASTContext &context, const clang::CXXMethodDecl *cxx_decl);

  /// \brief Disallow a copy constructor for SCCFG.
  SplitCFG(const SplitCFG &from) = delete;

  /// \brief Disallow assignment operator.
  SplitCFG &operator=(const SplitCFG &) = delete;

  /// \brief  Destructor that erases all SplitCFGBlocks created.
  virtual ~SplitCFG();

  /// \brief Returns the paths that were found in the SCCFG.
  const llvm::SmallVectorImpl<VectorSplitCFGBlock> &getPathsFound();

  /// \brief Construct the SCCFG.
  void construct_sccfg(const clang::CXXMethodDecl *method);

  /// \brief Modified DFS to create all paths within wait statements and from
  /// the root node.
  /// \param basic_block The current basic block to process.
  /// \param waits_in_stack The SplitCFGBlock that come after the wait
  /// statements. These need to be processed.
  /// \param visited_waits These are the SplitCFGBlocks that have waits and
  /// those that have been visited.
  void dfs_pop_on_wait(
      const SplitCFGBlock *basic_block,
      llvm::SmallVectorImpl<const SplitCFGBlock *> &waits_in_stack,
      llvm::SmallPtrSetImpl<const SplitCFGBlock *> &visited_waits);

  /// \brief Generates the paths between wait statements.
  void generate_paths();

  /// \brief Returns the argument to a wait statement.
  /// Note that the only one supported are no arguments or integer arguments.
  llvm::APInt getWaitArgument(const clang::CFGElement &element) const;

  /// Dump member functions.
  void dump() const;
  void dumpToDot() const;
  void dumpWaitNextStates() const;
  void dumpPaths() const;

  /// Rework
  //
  //

  void dfs_visit_wait(const clang::CFGBlock *BB);
  void dfs_rework();
  bool isLoop(clang::CFGBlock *block);
  bool isConditional(clang::CFGBlock *block);
  clang::CFGBlock *getUnvisitedSuccessor(
      const clang::CFGBlock *curr_block, clang::CFGBlock::const_succ_iterator &I,
      llvm::SmallPtrSetImpl<const clang::CFGBlock *> &visited);
};

};  // namespace systemc_clang

#endif /* _SPLIT_CFG_H_ */
