#include "SplitCFGBlock.h"

#include "llvm/Support/Debug.h"

using namespace systemc_clang;

SplitCFGBlock::SplitCFGBlock()
    : block_{nullptr},
      has_wait_{false},
      is_conditional_{false},
      is_loop_with_two_succ_{false},
      terminator_has_break_{false},
      terminator_has_wait_{false},
      id_{0},
      next_state_{0},
      wait_arg_{32, 0, false} {}

SplitCFGBlock::SplitCFGBlock(const SplitCFGBlock& from) {
  block_ = from.block_;
  has_wait_ = from.has_wait_;
  is_conditional_ = from.is_conditional_;
  is_loop_with_two_succ_ = from.is_loop_with_two_succ_;
  wait_element_ids_ = from.wait_element_ids_;
  successors_ = from.successors_;
  predecessors_ = from.predecessors_;
  next_state_ = from.next_state_;
  id_ = from.id_;
  wait_arg_ = from.wait_arg_;
  terminator_has_break_ = from.terminator_has_break_;
  terminator_has_wait_ = from.terminator_has_wait_;
}

bool SplitCFGBlock::isLoopWithTwoSuccessors() const {
  return is_loop_with_two_succ_;
}

bool SplitCFGBlock::isConditional() const { return is_conditional_; }

bool SplitCFGBlock::hasTerminatorBreak() const { return terminator_has_break_; }

bool SplitCFGBlock::hasTerminatorWait() const { return terminator_has_wait_; }

void SplitCFGBlock::setNextState(unsigned int state) { next_state_ = state; }

const clang::CFGBlock* SplitCFGBlock::getCFGBlock() const { return block_; }

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

void SplitCFGBlock::insertElements(VectorCFGElementPtr& elements) {
  elements_ = elements;
}

unsigned int SplitCFGBlock::getBlockID() const { return id_; }

unsigned int SplitCFGBlock::getNextState() const { return next_state_; }

llvm::APInt SplitCFGBlock::getWaitArg() const { return wait_arg_; }

void SplitCFGBlock::identifyBreaks(clang::ASTContext& context) {
  if (block_ && block_->getTerminator().isValid()) {
    /// See if we can iterate through all the terminator code.
    auto stmt{block_->getTerminator().getStmt()};
    MatchFinder bm_reg{};
    BreakMatcher bm{};
    bm.registerMatchers(bm_reg);
    bm_reg.match(*stmt, context);
    terminator_has_wait_ = bm.hasWait();
    terminator_has_break_ = bm.hasBreak();
  }
}

void SplitCFGBlock::dump() const {
  if (block_) {
    llvm::dbgs() << "\nSB" << getBlockID() << " (B" << block_->getBlockID()
                 << ") ";
    if (hasWait()) {
      llvm::dbgs() << " (WAIT)";
      llvm::dbgs() << " (Arg: " << wait_arg_ << ")"
                   << " (NextState: " << getNextState() << ")\n";
    }
    llvm::dbgs() << "\n";

    unsigned int i{0};
    for (auto const& element : elements_) {
      llvm::dbgs() << "  " << i << ": ";
      element->dump();
      ++i;
    }
    /// Print the terminator.
    clang::LangOptions lang_opts;
    if (!hasWait() && block_->getTerminator().isValid()) {
      llvm::dbgs() << "  T: ";
      block_->printTerminator(llvm::dbgs(), lang_opts);
    }

    llvm::dbgs() << "\n\n";

    llvm::dbgs() << "  Preds (" << predecessors_.size() << "): ";
    for (auto const& pre : predecessors_) {
      llvm::dbgs() << "SB" << pre->getBlockID() << " ";
    }

    llvm::dbgs() << "\n  Succs (" << successors_.size() << "): ";
    for (auto const& succ : successors_) {
      llvm::dbgs() << "SB" << succ->getBlockID() << " ";
    }
    llvm::dbgs() << "\n";
  }
}

void SplitCFGBlock::dumpColored() const {
  if (block_) {
    llvm::dbgs() << "\nSB" << getBlockID() << " (B" << block_->getBlockID()
                 << ") ";
    if (hasWait()) {
      llvm::dbgs() << llvm::buffer_ostream::Colors::RED << " (WAIT)";
      llvm::dbgs() << llvm::buffer_ostream::Colors::BLUE
                   << " (Arg: " << wait_arg_ << ")"
                   << llvm::buffer_ostream::Colors::GREEN
                   << " (NextState: " << getNextState() << ")\n";
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
