#include "SplitCFGBlock.h"

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

bool SplitCFGBlock::isFunctionCall(const clang::CFGElement& element) const {
  if (auto cfg_stmt = element.getAs<clang::CFGStmt>()) {
    auto stmt{cfg_stmt->getStmt()};
    stmt->dump();

    auto* expr{llvm::dyn_cast<clang::Expr>(stmt)};
    if (auto cxx_me = llvm::dyn_cast<clang::CXXMemberCallExpr>(expr)) {
      if (auto direct_callee = cxx_me->getDirectCallee()) {
        auto name{direct_callee->getNameInfo().getAsString()};
        llvm::dbgs() << "Function call: " << name << "\n";
        if (name != std::string("wait")) {
          llvm::dbgs() << "NOT A WAIT CALL\n";
          /// Generate the CFG
          auto method{ cxx_me->getMethodDecl()};
          auto fcfg {clang::CFG::buildCFG(method, method->getBody(), &method->getASTContext(),
                              clang::CFG::BuildOptions())};

          clang::LangOptions lang_opts;
          fcfg->dump(lang_opts, true);


        }
      }
    }
  }

  return true;
}

bool SplitCFGBlock::isWait(const clang::CFGElement& element) const {
  if (auto cfg_stmt = element.getAs<clang::CFGStmt>()) {
    auto stmt{cfg_stmt->getStmt()};

    stmt->dump();
    auto* expr{llvm::dyn_cast<clang::Expr>(stmt)};
    if (auto cxx_me = llvm::dyn_cast<clang::CXXMemberCallExpr>(expr)) {
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

    //////////////////////////////////////////
    /// Test code
    //////////////////////////////////////////
    if (isFunctionCall(*element)) {
      llvm::dbgs() << "YEAOW generate CFG for function call\n";

    }

    /// If the element is a wait() then split it.
    if (isWait(*element)) {
      if (num_elements == 1) {
        llvm::errs() << "Error: Only one statement and it is a wait().\n";
      } else {
        split_elements_.push_back(
            std::pair<unsigned int, unsigned int>(start, end - 1));
        split_elements_.push_back(
            std::pair<unsigned int, unsigned int>(end, end));
        start = end + 1;
      }
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
    for (auto const& element : split_elements_) {
      llvm::dbgs() << "[ " << element.first << ", " << element.second << " ]\n";
    }
  }
}


