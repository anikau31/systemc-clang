#include "SplitCFG.h"

#include "llvm/Support/Debug.h"

using namespace systemc_clang;

SplitCFGBlock::SplitCFGBlock() : block_{nullptr}, has_wait_{false} {}

SplitCFGBlock::SplitCFGBlock(const SplitCFGBlock& from) {
  block_ = from.block_;
  has_wait_ = from.has_wait_;
  split_elements_ = from.split_elements_;
}

clang::CFGBlock* SplitCFGBlock::getCFGBlock() const { return block_; }

std::size_t SplitCFGBlock::getSplitBlockSize() const {
  return split_elements_.size();
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

          /// Check that there is only 1 or 0 arguments
          auto args{cxx_me->getNumArgs()};
          if (args >= 2) {
            llvm::errs() << "wait() must have either 0 or 1 argument.\n";
            return false;
          }
          return true;
        }
      }
    }
  }

  return false;
};

void SplitCFGBlock::split_block(clang::CFGBlock* block) {
  llvm::dbgs() << "============== SPLIT BLOCK ==============\n";
  assert(block != nullptr);
  block_ = block;

  unsigned int start{1};
  unsigned int end{1};
  unsigned int num_elements{block_->size()};

  for (auto const& element : block->refs()) {
    llvm::dbgs() << "element index: " << block->getBlockID() << ":" << start
                 << "\n";
    element->dump();

    /// If the element is a wait() then split it.
    if (isWait(*element)) {
      split_elements_.push_back(
          std::pair<unsigned int, unsigned int>(start, end - 1));
      split_elements_.push_back(
          std::pair<unsigned int, unsigned int>(end, end));
      start = end + 1;
      has_wait_ = true;
    }

    // Increment end location.
    ++end;
  }
  if (has_wait_) {
    split_elements_.push_back(
        std::pair<unsigned int, unsigned int>(start, num_elements));
  }
  llvm::dbgs() << "============== END SPLIT BLOCK ==============\n";
}

void SplitCFGBlock::dump() const {
  if (block_) {
    block_->dump();
    llvm::dbgs() << "Dump split blocks\n";
    for (auto const& element: split_elements_) {
      llvm::dbgs() << "[ " << element.first << ", " << element.second << " ]\n";
    }
  }
}

/// ===========================================
/// SplitCFG
/// ===========================================
SplitCFG::SplitCFG(clang::ASTContext& context) : context_{context} {}

void SplitCFG::split_wait_blocks(const clang::CXXMethodDecl* method) {
  cfg_ = clang::CFG::buildCFG(method, method->getBody(), &context_,
                                        clang::CFG::BuildOptions());

  clang::LangOptions lang_opts;
  cfg_->dump(lang_opts, true);

  // Go through all CFG blocks
  llvm::dbgs() << "Iterate through all CFGBlocks.\n";
  /// These iterators are not in clang 12.
  // for (auto const &block: CFG->const_nodes()) {
  for (auto begin_it = cfg_->nodes_begin(); begin_it != cfg_->nodes_end();
       ++begin_it) {
    auto block{*begin_it};
    block->dump();

    /// Try to split the block.
    SplitCFGBlock sp{};
    sp.split_block(block);
    sp.dump();

    if (sp.getSplitBlockSize() != 0) {
      /// The block has been split.
      split_blocks_.insert(std::pair<unsigned int, SplitCFGBlock>(block->getBlockID(),sp));
    }
  }
}

void SplitCFG::dump() const {
    llvm::dbgs() << "Dump all blocks that were split\n";
    for (auto const &sblock : split_blocks_) {
      llvm::dbgs() << "Block ID: " << sblock.first << "\n";
      const SplitCFGBlock *block_with_wait{ &sblock.second };
      llvm::dbgs() << "Print all info for split block\n";
      block_with_wait->dump();

    }
}


