//===- SplitCFGBlock.h - Split the CFGBlock separated by waits -*- C++ -*-=====//
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
 private:
  using VectorCFGElementPtr = llvm::SmallVector<const clang::CFGElement *>;

  clang::CFGBlock *block_;
  bool has_wait_;

  /// Split the elements into blocks separated by wait() statements
  llvm::SmallVector<VectorCFGElementPtr> split_elements_;

  /// This holds the ids in split_elements_ that correspond to the wait
  /// statements.  This will be a single vector with just the wait() element.
  llvm::SmallVector<unsigned int> wait_element_ids_;

 private:
  bool isWait(const clang::CFGElement &element) const;
  bool isFunctionCall(const clang::CFGElement &element) const;

 public:
  SplitCFGBlock();
  SplitCFGBlock(const SplitCFGBlock &from);

  clang::CFGBlock *getCFGBlock() const;
  std::size_t getSplitBlockSize() const;
  bool hasWait() const;

  void split_block(clang::CFGBlock *block);

  void dump() const;
};

};  // namespace systemc_clang

#endif
