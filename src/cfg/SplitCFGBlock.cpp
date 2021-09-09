#include "SplitCFGBlock.h"

#include "llvm/Support/Debug.h"

using namespace systemc_clang;

SplitCFGBlock::SplitCFGBlock()
    : block_{nullptr},
      has_wait_{false},
      id_{0},
      next_state_{0},
      wait_arg_{32, 0, false} {}

SplitCFGBlock::SplitCFGBlock(const SplitCFGBlock& from) {
  block_ = from.block_;
  has_wait_ = from.has_wait_;
  wait_element_ids_ = from.wait_element_ids_;
  successors_ = from.successors_;
  predecessors_ = from.predecessors_;
  next_state_ = from.next_state_;
  id_ = from.id_;
  next_state_ = from.next_state_;
  wait_arg_ = from.wait_arg_;
}

clang::CFGBlock* SplitCFGBlock::getCFGBlock() const { return block_; }

const SplitCFGBlock::VectorCFGElementPtrImpl& SplitCFGBlock::getElements()
    const {
  return elements_;
}

const SplitCFGBlock::VectorSplitCFGBlockPtrImpl& SplitCFGBlock::getSuccessors()
    const {
  return successors_;
}
const SplitCFGBlock::VectorSplitCFGBlockPtrImpl&
SplitCFGBlock::getPredecessors() const {
  return predecessors_;
}

bool SplitCFGBlock::hasWait() const { return has_wait_; }

std::size_t SplitCFGBlock::getNumOfElements() const { return elements_.size(); }

/*
bool SplitCFGBlock::isFunctionCall(const clang::CFGElement& element) const {
  if (auto cfg_stmt = element.getAs<clang::CFGStmt>()) {
    auto stmt{cfg_stmt->getStmt()};
    // stmt->dump();

    auto* expr{llvm::dyn_cast<clang::Expr>(stmt)};
    if (auto cxx_me = llvm::dyn_cast<clang::CXXMemberCallExpr>(expr)) {
      if (auto direct_callee = cxx_me->getDirectCallee()) {
        auto name{direct_callee->getNameInfo().getAsString()};
        llvm::dbgs() << "Function call: " << name << "\n";
        if (name != std::string("wait")) {
          llvm::dbgs() << "NOT A WAIT CALL\n";
          /// Generate the CFG
          auto method{cxx_me->getMethodDecl()};
          auto fcfg{clang::CFG::buildCFG(method, method->getBody(),
                                         &method->getASTContext(),
                                         clang::CFG::BuildOptions())};

  //        clang::LangOptions lang_opts;
          // fcfg->dump(lang_opts, true);
        }
      }
    }
  }

  return true;
}
*/

void SplitCFGBlock::insertElements(VectorCFGElementPtr& elements) {
  elements_ = elements;
}

unsigned int SplitCFGBlock::getBlockID() const { return id_; }

unsigned int SplitCFGBlock::getNextState() const { return next_state_; }

llvm::APInt SplitCFGBlock::getWaitArg() const { return wait_arg_; }

void SplitCFGBlock::dump() const {
  if (block_) {
    llvm::dbgs() << "\nSB" << getBlockID() << " (B" << block_->getBlockID()
                 << ") ";
    if (hasWait()) {
      llvm::dbgs() << llvm::buffer_ostream::Colors::RED << " (WAIT)";
      llvm::dbgs() << llvm::buffer_ostream::Colors::BLUE
                   << " (Arg: " << wait_arg_ << ")\n";
    }
    llvm::dbgs() << "\n" << llvm::buffer_ostream::Colors::RESET;

    unsigned int i{0};
    for (auto const& element : elements_) {
      llvm::dbgs() << "  " << i << ": ";
      element->dump();
      ++i;
    }

    llvm::dbgs() << "\n";

    llvm::dbgs() << llvm::buffer_ostream::Colors::GREEN << "  Preds ("
                 << llvm::buffer_ostream::Colors::RESET << predecessors_.size()
                 << llvm::buffer_ostream::Colors::GREEN << "): ";
    for (auto const& pre : predecessors_) {
      llvm::dbgs() << "SB" << pre->getBlockID() << " ";
    }

    llvm::dbgs() << llvm::buffer_ostream::Colors::MAGENTA << "\n  Succs ("
                 << llvm::buffer_ostream::Colors::RESET << successors_.size()
                 << llvm::buffer_ostream::Colors::MAGENTA << "): ";
    for (auto const& succ : successors_) {
      llvm::dbgs() << "SB" << succ->getBlockID() << " ";
    }
    llvm::dbgs() << llvm::buffer_ostream::Colors::RESET;
    llvm::dbgs() << "\n";
  }
}
