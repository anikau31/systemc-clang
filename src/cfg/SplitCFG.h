#ifndef _SPLIT_CFG_H_
#define _SPLIT_CFG_H_

#include <unordered_map>

#include "clang/AST/ASTContext.h"
#include "clang/Analysis/CFG.h"
#include "SplitCFGBlock.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/Support/Debug.h"

namespace systemc_clang {

/// ===========================================
/// SplitCFGPathInfo
/// ===========================================
class SplitCFGPathInfo {
 public:
  using SplitCFGBlockPtrVector = llvm::SmallVector<const SplitCFGBlock *>;
  friend class SplitCFG;

 public:
  SplitCFGPathInfo(const SplitCFGBlock *block)
      : split_block_{block}, cfg_block_{block->getCFGBlock()} {};

  virtual ~SplitCFGPathInfo() {}

  bool isTruePathValid() const { return (true_path_.size() > 0); }

  bool isFalsePathValid() const { return ((false_path_.size() > 0)); }
  const SplitCFGBlockPtrVector &getTruePath() const { return true_path_; }
  const SplitCFGBlockPtrVector &getFalsePath() const { return true_path_; }

  std::string toStringFalsePath() const {
    std::string str{};
    for (const auto & block : false_path_) {
      str += std::to_string(block->getBlockID());
      str += " ";
    }
    str.pop_back();

    return str;
  }

 
  std::string toStringTruePath() const {
    std::string str{};
    for (const auto & block : true_path_) {
      str += std::to_string(block->getBlockID());
      str += " ";
    }
    str.pop_back();

    return str;
  }

  void dump() {
    llvm::dbgs() << " BB# " << split_block_->getBlockID() << "\n";
    llvm::dbgs() << "  TRUE  path: ";
    for (const auto block : true_path_) {
      llvm::dbgs() << block->getBlockID() << " ";
    }
    llvm::dbgs() << "\n";
    llvm::dbgs() << "  FALSE path: ";
    for (const auto block : false_path_) {
      llvm::dbgs() << block->getBlockID() << " ";
    }
  }

 private:
  const SplitCFGBlock *split_block_;
  const clang::CFGBlock *cfg_block_;
  SplitCFGBlockPtrVector true_path_;
  SplitCFGBlockPtrVector false_path_;
};

/// ===========================================
/// SplitCFG
/// ===========================================
class SplitCFG {
 public:
  using SplitCFGPath =
      llvm::SmallVector<std::pair<const SplitCFGBlock *, SplitCFGPathInfo>>;

  // TODO: deprecated
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
  llvm::SmallVector<SplitCFGPath> paths_;

  /// \brief The block id to block for SCCFG.
  std::unordered_map<unsigned int, SplitCFGBlock *> sccfg_;

  llvm::SmallVector<std::pair<VectorCFGElementPtrImpl, bool>> split_elements;

  /// \brief Predecessor SplitCFGBlock* => (Wait SplitCFGBlock*)
  std::unordered_map<const SplitCFGBlock *,
                     std::pair<const SplitCFGBlock *, unsigned int>>
      wait_next_state_;

  /// \brief Map a SplitCFGBlock* to its path information.
  std::unordered_map<const SplitCFGBlock *, SplitCFGPathInfo> path_info_;

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
  // const llvm::SmallVectorImpl<VectorSplitCFGBlock> &

  const llvm::SmallVectorImpl<
      llvm::SmallVector<std::pair<const SplitCFGBlock *, SplitCFGPathInfo>>>
      &getPathsFound();

  /// \brief Construct the SCCFG.
  void construct_sccfg(const clang::CXXMethodDecl *method);

  /// \brief Generates the paths between wait statements.
  void generate_paths();

  
  const std::unordered_map<const SplitCFGBlock *, SplitCFGPathInfo>& getPathInfo() const;
  void preparePathInfo();
  /// \brief Returns the argument to a wait statement.
  /// Note that the only one supported are no arguments or integer arguments.
  llvm::APInt getWaitArgument(const clang::CFGElement &element) const;

  /// Dump member functions.
  void dump() const;
  void dumpToDot() const;
  void dumpWaitNextStates() const;
  void dumpPaths() const;
  void dumpPathInfo() const;

  /// Rework
  //
  //

  template <typename T>
  void dumpSmallVector(llvm::SmallVectorImpl<T> &vlist) {
    for (const auto v : vlist) {
      llvm::dbgs() << v.first->getBlockID() << " ";
    }
  }

  /// \brief Modified DFS to create all paths within wait statements and from
  /// the root node.
  /// \param basic_block The current basic block to process.
  /// \param waits_in_stack The SplitCFGBlock that come after the wait
  /// statements. These need to be processed.
  /// \param visited_waits These are the SplitCFGBlocks that have waits and
  /// those that have been visited.

  const llvm::SmallVector<std::pair<const SplitCFGBlock *, SplitCFGPathInfo>>
  dfs_visit_wait(
      const SplitCFGBlock *BB,
      llvm::SmallPtrSet<const SplitCFGBlock *, 32> &visited_blocks,
      llvm::SmallVectorImpl<const SplitCFGBlock *> &waits_to_visit,
      llvm::SmallPtrSetImpl<const SplitCFGBlock *> &visited_waits,
      llvm::SmallVector<std::pair<const SplitCFGBlock *, SplitCFGPathInfo>>
          &curr_path);
  void dfs_rework();
  bool isLoop(const SplitCFGBlock *block) const;
  bool isConditional(const SplitCFGBlock *block) const;
  bool getUnvisitedSuccessor(
      const SplitCFGBlock *curr_block, SplitCFGBlock::const_succ_iterator &I,
      llvm::SmallPtrSetImpl<const SplitCFGBlock *> &visited,
      const SplitCFGBlock *&block);
  bool isLoopWithTwoSuccessors(const SplitCFGBlock *block) const;
  void addSuccessorToVisitOrPop(
      bool parent_has_wait, const SplitCFGBlock *BB,
      llvm::SmallVector<
          std::pair<const SplitCFGBlock *, SplitCFGBlock::const_succ_iterator>,
          8> &to_visit,
      bool found);

  bool isTruePath(const SplitCFGBlock *parent_block,
                  const SplitCFGBlock *block) const;

  /// \brief Compute the set difference between two SmallPtrSets.
  void setDifference(
      const llvm::SmallPtrSetImpl<const SplitCFGBlock *> &larger,
      const llvm::SmallPtrSetImpl<const SplitCFGBlock *> &smaller,
      llvm::SmallPtrSetImpl<const SplitCFGBlock *> &to);
  void setTruePathInfo(
      const SplitCFGBlock *sblock,
      const llvm::SmallVector<
          std::pair<const SplitCFGBlock *, SplitCFGPathInfo>> &newly_visited);

  void setFalsePathInfo(
      const SplitCFGBlock *sblock,
      const llvm::SmallVector<
          std::pair<const SplitCFGBlock *, SplitCFGPathInfo>> &newly_visited);

  void updateVisitedBlocks(
      llvm::SmallPtrSetImpl<const SplitCFGBlock *> &to,
      const llvm::SmallPtrSetImpl<const SplitCFGBlock *> &from);
  void dumpVisitedBlocks(llvm::SmallPtrSetImpl<const SplitCFGBlock *> &visited);

  bool popping_;
};

};  // namespace systemc_clang

#endif /* _SPLIT_CFG_H_ */
