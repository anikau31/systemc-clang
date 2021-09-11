//===- SplitCFGBlock.h - Split the CFGBlock separated by waits -*- C++
//-*-=====//
//
// Part of the systemc-clang project.
// See License.rst
//
//===----------------------------------------------------------------------===//
//
/// \file
/// Split the CFG block in chunks separted by wait() statements.
//
/// \author Hiren Patel
//===----------------------------------------------------------------------===//

#ifndef _SPLIT_CFG_BLOCK_H_
#define _SPLIT_CFG_BLOCK_H_

#include "clang/Analysis/CFG.h"
#include <vector>

namespace systemc_clang {

///
/// This class  represents information that is stored to split a single CFGBlock
/// into elements that are wait() calls versus others.
///
///
class SplitCFGBlock {
 public:
  using VectorCFGElementPtr = llvm::SmallVector<const clang::CFGElement *>;
  using VectorCFGElementPtrImpl =
      llvm::SmallVectorImpl<const clang::CFGElement *>;
  using VectorSplitCFGBlockPtr = llvm::SmallVector<const SplitCFGBlock *>;
  using VectorSplitCFGBlockPtrImpl = llvm::SmallVector<const SplitCFGBlock *>;

 private:
  friend class SplitCFG;

  /// A pointer to the original CFGBlock.
  const clang::CFGBlock *block_;
  /// Whether this SplitCFGBlock is a wait block or not.  Only one element if it is a wait block. 
  bool has_wait_;
  /// The block id.
  unsigned int id_;
  /// The next state that the wait would transform to.
  unsigned int next_state_;
  /// The wait argument. 
  llvm::APInt wait_arg_;

  /// This holds the ids in split_elements_ that correspond to the wait
  /// statements.  This will be a single vector with just the wait() element.
  llvm::SmallVector<unsigned int> wait_element_ids_;

  /// CFG Elements
  VectorCFGElementPtr elements_;
  /// Predecessors and successors.
  llvm::SmallVector<const SplitCFGBlock *> predecessors_;
  llvm::SmallVector<const SplitCFGBlock *> successors_;

 public:
  struct SuccessorIterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = SplitCFGBlock;
    using pointer = SplitCFGBlock *;    // or also value_type*
    using reference = SplitCFGBlock &;  // or also value_type&
    using VectorSuccessors = llvm::SmallVector<const SplitCFGBlock *>;
    using const_iterator = VectorSuccessors::const_iterator;

   public:
    VectorSuccessors::const_iterator begin() { return succs_.begin(); }
    VectorSuccessors::const_iterator end() { return succs_.end(); }

    SuccessorIterator(const llvm::SmallVector<const SplitCFGBlock *> &succ)
        : succs_{succ} {}

   private:
    const VectorSuccessors &succs_;
  };

 public:
  using succ_iterator = SuccessorIterator;
  using const_succ_iterator = SuccessorIterator::const_iterator;
  using succ_iterator_range = llvm::iterator_range<succ_iterator>;
  using const_succ_iterator_range = llvm::iterator_range<const_succ_iterator>;

  bool succ_empty() const { return (successors_.size() == 0); }
  SuccessorIterator::const_iterator succ_begin() const {
    return SuccessorIterator{successors_}.begin();
  }
  SuccessorIterator::const_iterator succ_end() const {
    return SuccessorIterator{successors_}.end();
  }
  // succ_iterator_range succs() { return succ_iterator_range{succ_begin(),
  // succ_end()}; }
  const_succ_iterator_range const_succs() {
    return const_succ_iterator_range{succ_begin(), succ_end()};
  }

 private:
  // bool isFunctionCall(const clang::CFGElement &element) const;
  void setNextState(unsigned int state);

 public:
  /// \brief Constructor.
  SplitCFGBlock();

  /// \brief Copy constructor.
  SplitCFGBlock(const SplitCFGBlock &from);

  /// \brief Returns the pointer to the original CFGBlock from which the SplitCFGBlock was created. 
  const clang::CFGBlock *getCFGBlock() const;

  /// \brief Returns the number of CFGElements in this block. 
  std::size_t getNumOfElements() const;

  /// \brief Returns the elements in this block. 
  const VectorCFGElementPtrImpl &getElements() const;

  /// \brief Returns the successors for the block.
  const VectorSplitCFGBlockPtrImpl &getSuccessors() const;

  /// \brief Returns the predecessors for the block.
  const VectorSplitCFGBlockPtrImpl &getPredecessors() const;

  /// \brief Returns whether the SplitCFGBlock is a wait block or not.
  bool hasWait() const;

  /// \brief Returns the block ID for the SplitCFGBlock.
  unsigned int getBlockID() const;

  /// \brief Returns the next state. Only pertinent for blocks that have waits in them. 
  unsigned int getNextState() const;

  /// \brief Returns the integer value of the argument supplied to the wait().
  llvm::APInt getWaitArg() const;

  /// \brief The elements are added to this SplitCFGBlock. 
  void insertElements(VectorCFGElementPtr &elements);

  void dump() const;
};

};  // namespace systemc_clang

#endif
