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
  using VectorCFGElementPtr = std::vector<const clang::CFGElement *>;

  clang::CFGBlock *block_;
  bool has_wait_;

  std::vector<VectorCFGElementPtr> split_elements_;

  std::vector<unsigned int> wait_element_ids_;

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
