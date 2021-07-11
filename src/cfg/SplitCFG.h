#ifndef _SPLIT_CFG_H_
#define _SPLIT_CFG_H_

#include "clang/Analysis/CFG.h"
#include <vector>

namespace systemc_clang {

  class SplitCFGBlock {
    private:
      clang::CFGBlock *block_;
      std::vector<std::size_t> wait_ids;
      std::vector<std::pair<unsigned int, unsigned int> > split_blocks_;

    public:
      SplitCFGBlock();
      SplitCFGBlock(const SplitCFGBlock& from);

      clang::CFGBlock *getCFGBlock() const;
      std::size_t getElementsSize() const;

      void split_block(clang::CFGBlock *block);
      bool isWait(const clang::CFGElement &element) const;

      void dump();
  };

};

#endif /* _SPLIT_CFG_H_ */

