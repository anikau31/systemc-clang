#ifndef _SPLIT_CFG_BLOCK_H_
#define _SPLIT_CFG_BLOCK_H_

#include "clang/Analysis/CFG.h"
#include <vector>

namespace systemc_clang {

///
/// This class  represents information that is stored to split a single CFGBlock
/// into elements that are wait() calls versus others.
///

class SplitCFGBlock {
 private:
  clang::CFGBlock *block_;
  bool has_wait_;

  /// Store the element indicies that split the block.
  std::vector<std::pair<unsigned int, unsigned int> > split_elements_;

 private:
  bool isWait(const clang::CFGElement &element) const;
  bool isFunctionCall(const clang::CFGElement &element) const;

 public:
  SplitCFGBlock();
  SplitCFGBlock(const SplitCFGBlock &from);

  clang::CFGBlock *getCFGBlock() const;
  std::size_t getSplitBlockSize() const;

  void split_block(clang::CFGBlock *block);

  void dump() const;
};

};  // namespace systemc_clang

#endif
