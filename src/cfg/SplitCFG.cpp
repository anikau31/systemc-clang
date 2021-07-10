#include "SplitCFG.h"

#include "llvm/Support/Debug.h"

using namespace systemc_clang;

SplitCFGBlock::SplitCFGBlock() : block_{nullptr} {}

SplitCFGBlock::SplitCFGBlock(const SplitCFGBlock& from) {
  block_ = from.block_;
}

clang::CFGBlock* SplitCFGBlock::getCFGBlock() const { return block_; }

std::size_t SplitCFGBlock::getElementsSize() const {
  assert(block_ == nullptr);
  return block_->size();
}

bool SplitCFGBlock::isWait(const clang::CFGElement& element) const {
  if (auto cfg_stmt = element.getAs<clang::CFGStmt>()) {
    auto stmt{cfg_stmt->getStmt()};

    stmt->dump();
    auto* expr{llvm::dyn_cast<clang::Expr>(stmt)};
    if (auto cxx_me = llvm::dyn_cast<clang::CXXMemberCallExpr>(expr)) {
      llvm::dbgs() << "STMTEXPR\n";
      cxx_me->dump();
      if (auto direct_callee = cxx_me->getDirectCallee()) {
        auto name{direct_callee->getNameInfo().getAsString()};
        if (name == std::string("wait")) {
          llvm::dbgs() << "@@@@ FOUND WAIT @@@@\n";
        }
      }
    }
  }

  return false;
};

void SplitCFGBlock::split_block(clang::CFGBlock* block) {
  llvm::dbgs() << "============== SPLIT BLOCK ==============\n";
  std::size_t id{1};

  for (auto const& element : block->refs()) {
    llvm::dbgs() << "element index: " << block->getBlockID() << ":" << id
                 << "\n";
    element->dump();
    ++id;

    /// If the element is a wait() then split it.
    if (isWait(*element)) {
      block_ = block;
    }
  }
  llvm::dbgs() << "============== END SPLIT BLOCK ==============\n";
}

void SplitCFGBlock::dump() {}
