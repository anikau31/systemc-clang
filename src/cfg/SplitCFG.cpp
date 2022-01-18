// ===-- src/SystemCClang.h - systec-clang class definition -------*- C++
// -*-===//
//
// This file is distributed under the University of Illinois License.
// See LICENSE.mkd for details.
//
// ===----------------------------------------------------------------------===//
// /
// / \file
// / A CFG representation with splitting basic blocks that have wait()
// statements
// /
// ===----------------------------------------------------------------------===//

#include "SplitCFG.h"

#include "llvm/Support/Debug.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/ADT/PostOrderIterator.h"
#include <regex>

using namespace systemc_clang;

////////////////////////////////////////////////////////
/// REWORK the Cthread Path generation
////////////////////////////////////////////////////////
///
void SplitCFG::dfs_visit_wait(const clang::CFGBlock* BB) {
  if (BB->succ_empty()) {
    /// Empty CFG block
    return;
  }

  llvm::SmallPtrSet<const clang::CFGBlock*, 8> visited_blocks;
  llvm::SmallVector<
      std::pair<const clang::CFGBlock*, clang::CFGBlock::const_succ_iterator>,
      8>
      to_visit_stack;

  visited_blocks.insert(BB);
  to_visit_stack.push_back(std::make_pair(BB, BB->succ_begin()));

  do {
    std::pair<const clang::CFGBlock*, clang::CFGBlock::const_succ_iterator>&
        Top = to_visit_stack.back();
    const clang::CFGBlock* ParentBB = Top.first;
    clang::CFGBlock::const_succ_iterator& I = Top.second;

    llvm::dbgs() << "BB# " << ParentBB->getBlockID() << "\n";

    // If there is a successor that has not been visited, then remember that
    // block.
    BB = getUnvisitedSuccessor(ParentBB, I, visited_blocks);
    bool FoundNew{(BB != nullptr)};

    if (FoundNew) {
      // Go down one level if there is a unvisited successor.
      to_visit_stack.push_back(std::make_pair(BB, BB->succ_begin()));
    } else {
      // Go up one level.
      llvm::dbgs() << "pop: " << to_visit_stack.size() << "\n";
      to_visit_stack.pop_back();
    }
  } while (!to_visit_stack.empty());
}

clang::CFGBlock* SplitCFG::getUnvisitedSuccessor(
    const clang::CFGBlock* curr_block, clang::CFGBlock::const_succ_iterator& I,
    llvm::SmallPtrSetImpl<const clang::CFGBlock*>& visited_blocks) {
  clang::CFGBlock* block{nullptr};
  while (I != curr_block->succ_end()) {
    block = *I++;
    if ((block != nullptr) && (visited_blocks.insert(block).second)) {
      break;
    }
  }
  return block;
}

void SplitCFG::dfs_rework() {
  /// G = cfg_
  const clang::CFGBlock* BB{&cfg_->getEntry()};
  // dfs_pop_on_wait(BB);
  //
  for (auto begin_it = cfg_->nodes_begin(); begin_it != cfg_->nodes_end();
       ++begin_it) {
    auto block = *begin_it;
    llvm::dbgs() << "Block " << block->getBlockID() << " isLoop "
                 << isLoop(block) << " starting DFS\n";
    block->dump();

    dfs_visit_wait(block);
  }
}

bool SplitCFG::isConditional(clang::CFGBlock* block) {
  /// Loop block has a terminator.
  /// The terminator is a clang::Stmt
  //
  if (block == nullptr) {
    return false;
  }

  auto stmt{block->getTerminatorStmt()};
  return stmt && (llvm::isa<clang::IfStmt>(stmt));
}

bool SplitCFG::isLoop(clang::CFGBlock* block) {
  /// Loop block has a terminator.
  /// The terminator is a clang::Stmt
  //
  if (block == nullptr) {
    return false;
  }

  auto stmt{block->getTerminatorStmt()};
  return stmt &&
         (llvm::isa<clang::WhileStmt>(stmt) ||
          llvm::isa<clang::ForStmt>(stmt) || llvm::isa<clang::DoStmt>(stmt));
}
////////////////////////////////////////////////////////

/// ===========================================
/// SplitCFG
/// ===========================================
void SplitCFG::dfs_pop_on_wait(
    const SplitCFGBlock* basic_block,
    llvm::SmallVectorImpl<const SplitCFGBlock*>& waits_in_stack,
    llvm::SmallPtrSetImpl<const SplitCFGBlock*>& visited_waits) {
  /// Empty CFG block
  if (basic_block->succ_empty()) {
    return;
  }

  bool walking_up_from_wait{false};
  llvm::SmallPtrSet<const SplitCFGBlock*, 8> visited;
  llvm::SmallVector<
      std::pair<const SplitCFGBlock*, SplitCFGBlock::const_succ_iterator>, 8>
      visit_stack;
  llvm::SmallVector<const SplitCFGBlock*, 8> in_stack;

  visited.insert(basic_block);
  visit_stack.push_back(std::make_pair(basic_block, basic_block->succ_begin()));

  // llvm::SmallVector<const SplitCFGBlock*> curr_path;
  VectorSplitCFGBlock curr_path;
  do {
    std::pair<const SplitCFGBlock*, SplitCFGBlock::const_succ_iterator>& Top =
        visit_stack.back();
    const SplitCFGBlock* parent_bb = Top.first;
    SplitCFGBlock::const_succ_iterator& I = Top.second;

    /// If BB has a wait() then just return.
    bool bb_has_wait{(parent_bb->hasWait())};

    if (bb_has_wait) {
      walking_up_from_wait = bb_has_wait;
      llvm::dbgs() << "BB# " << parent_bb->getBlockID()
                   << " has a wait in it\n";
      llvm::dbgs() << "Visited BB#" << parent_bb->getBlockID() << "\n";
      curr_path.push_back(parent_bb);

      /// The wait has not been processed yet so add it to the visited_wait
      /// sets. Then add it in the stack to process the waits.
      if (visited_waits.insert(parent_bb).second == true) {
        // Insert the successor of the block.

        while (I != parent_bb->succ_end()) {
          basic_block = *I++;
          if ((basic_block != nullptr)) {
            llvm::dbgs() << "Insert successor of BB#" << parent_bb->getBlockID()
                         << ": BB#" << basic_block->getBlockID() << "\n";
            // Mimic FIFO
            waits_in_stack.insert(waits_in_stack.begin(), basic_block);
            auto wit = wait_next_state_.find(parent_bb);
            if (wit == wait_next_state_.end()) {
              wait_next_state_.insert(std::make_pair(
                  parent_bb, std::make_pair(basic_block, next_state_count_)));
              ++next_state_count_;
            }
            break;
          }
        }
      }
    }

    // A new block is found only if the block is not visited, and it is not a
    // wait(). If yes, then mark it as visited.
    bool FoundNew = false;
    while (I != parent_bb->succ_end()) {
      basic_block = *I++;
      if ((basic_block != nullptr) && (!bb_has_wait) &&
          (visited.insert(basic_block).second)) {
        FoundNew = true;
        break;
      }
    }

    if (FoundNew) {
      // If a new successor has been found, then go down one level if there is a
      // unvisited successor.
      llvm::dbgs() << "Visited BB#" << parent_bb->getBlockID() << "\n";
      visit_stack.push_back(
          std::make_pair(basic_block, basic_block->succ_begin()));
      curr_path.push_back(parent_bb);
      if (walking_up_from_wait) {
        visited.clear();
        // Reinsert the successor block
        visited.insert(basic_block);
        walking_up_from_wait = false;
      }
    } else {
      // Go up one level.
      // llvm::dbgs() << "pop: " << visit_stack.size() << "\n";
      visit_stack.pop_back();
    }
  } while (!visit_stack.empty());

  /// Insert the path constructed.
  paths_found_.push_back(curr_path);
}

llvm::APInt SplitCFG::getWaitArgument(const clang::CFGElement& element) const {
  if (auto cfg_stmt = element.getAs<clang::CFGStmt>()) {
    auto stmt{cfg_stmt->getStmt()};

    if (auto* expr = llvm::dyn_cast<clang::Expr>(stmt)) {
      if (auto cxx_me = llvm::dyn_cast<clang::CXXMemberCallExpr>(expr)) {
        auto args{cxx_me->getNumArgs()};
        if (args == 1) {
          if (auto first_arg{cxx_me->getArg(0)}) {
            llvm::dbgs() << "*************** FIRST ARG ****************\n";
            first_arg->dump();
            const clang::IntegerLiteral* IntLiteral =
                clang::dyn_cast<clang::IntegerLiteral>(first_arg);
            llvm::dbgs() << " ARG VAL: " << IntLiteral->getValue() << "\n";
            return IntLiteral->getValue();
          }
        }
      }
    }
  }

  return llvm::APInt{32, 0, false};
};

bool SplitCFG::isElementWait(const clang::CFGElement& element) const {
  if (auto cfg_stmt = element.getAs<clang::CFGStmt>()) {
    auto stmt{cfg_stmt->getStmt()};

    // stmt->dump();
    if (auto* expr = llvm::dyn_cast<clang::Expr>(stmt)) {
      if (auto cxx_me = llvm::dyn_cast<clang::CXXMemberCallExpr>(expr)) {
        if (auto direct_callee = cxx_me->getDirectCallee()) {
          auto name{direct_callee->getNameInfo().getAsString()};
          if (name == std::string("wait")) {
            // llvm::dbgs() << "@@@@ FOUND WAIT @@@@\n";

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
  }

  return false;
};

void SplitCFG::splitBlock(clang::CFGBlock* block) {
  assert(block != nullptr);

  unsigned int num_elements{block->size()};

  llvm::dbgs() << "\n\nChecking if block " << block->getBlockID()
               << " has a wait()\n";
  llvm::dbgs() << "Number of elements " << num_elements << "\n";
  // We are going to have two vectors.
  // 1. A vector of vector pointers to CFGElements.
  // 2. A vector of pointers to CFGElements that are waits.
  //

  VectorCFGElementPtr vec_elements{};
  llvm::SmallVector<std::pair<VectorCFGElementPtr, bool> > split_elements;

  /// 0 elements so simply just add them and return.
  if (num_elements == 0) {
    split_elements.push_back(std::make_pair(vec_elements, false));
  } else {
    bool has_wait{false};
    for (auto const& element : block->refs()) {
      has_wait = false;
      llvm::dbgs() << "Is this a wait?    ";
      element->dump();

      /// refs() returns an iterator, which actually stores an ElementRefImpl<>
      /// interface. In order to get the correct pointer to CFGElement, we need
      /// to explicitly call operator->(). Odd!
      const clang::CFGElement* element_ptr{element.operator->()};
      /// If the element is a wait() then split it.
      if (isElementWait(*element)) {
        llvm::dbgs() << " IT IS A WAIT\n";
        /// There is only one statement and it's a wait().
        if (num_elements == 1) {
          llvm::dbgs() << "DBG: Only one statement and it is a wait().\n";
        }

        // Add all elements before the wait into the split elements.
        if (vec_elements.size() != 0) {
          split_elements.push_back(std::make_pair(vec_elements, false));
          vec_elements.clear();
        }

        /// Add the wait as a separate entry in the list.
        vec_elements.push_back(element_ptr);
        split_elements.push_back(std::make_pair(vec_elements, true));
        vec_elements.clear();

        has_wait = true;
      } else {
        vec_elements.push_back(element_ptr);
      }
    }

    if (vec_elements.size() != 0) {
      split_elements.push_back(std::make_pair(vec_elements, has_wait));
    }
  }

  createWaitSplitCFGBlocks(block, split_elements);
}

void SplitCFG::createWaitSplitCFGBlocks(
    clang::CFGBlock* block,
    const llvm::SmallVectorImpl<
        std::pair<SplitCFG::VectorCFGElementPtr, bool> >& split_elements) {
  // dumpSplitElements(split_elements);
  /// Go through all the split_elements and create blocks.
  llvm::dbgs() << "Number of entries in split_elements "
               << split_elements.size() << "\n";
  unsigned int id{0};
  SplitCFGBlock* prev_block{nullptr};

  for (auto const& elements : split_elements) {
    llvm::dbgs() << "Element number " << id << " ";
    /// check if the block has already been created.
    SplitCFGBlock* new_split{nullptr};

    bool already_exists{false};
    // id = 0 is when it is the first sequence of elements.
    if (id == 0) {
      auto scit{sccfg_.find(block->getBlockID())};
      // Must be true.
      // bool already_exists{scit != sccfg_.end()};
      new_split = scit->second;
      new_split->id_ = block->getBlockID();

      // Successor: succ block from CFGBlock (overridden)
      /// First one
      if (split_elements.size() == 1) {
        addSuccessors(new_split, block);
      }
      // Prececessor: prev block from CFGBlock
      addPredecessors(new_split, block);
    } else {
      new_split = new SplitCFGBlock{};
      new_split->id_ = block->getBlockID() * 10 + id;

      /// Succesors
      //
      /// Last one
      llvm::dbgs() << "SB# " << new_split->id_ << " size "
                   << split_elements.size() << "\n";
      if (id == split_elements.size() - 1) {
        llvm::dbgs() << " add successors of BB# " << block->getBlockID()
                     << "\n";
        addSuccessors(new_split, block);
      }
      prev_block->successors_.push_back(new_split);
      /// Predecessors
      new_split->predecessors_.push_back(prev_block);
      sccfg_.insert(std::make_pair(block->getBlockID() * 10 + id, new_split));
    }

    new_split->block_ = block;
    new_split->has_wait_ = elements.second;
    new_split->elements_ = elements.first;

    // If it's a wait() then get the argument.
    if (new_split->has_wait_) {
      /// Get the first element of the SmallVector, which will be a wait()
      const clang::CFGElement* wait_el = (*new_split->elements_.begin());
      new_split->wait_arg_ = getWaitArgument(*wait_el);
    }

    // Set the successor of new_split.
    prev_block = new_split;
    ++id;
  }
}

void SplitCFG::addSuccessors(SplitCFGBlock* to, const clang::CFGBlock* from) {
  for (auto const& succ : from->succs()) {
    if (succ) {
      auto fit{sccfg_.find(succ->getBlockID())};
      SplitCFGBlock* next_succ{fit->second};
      to->successors_.push_back(next_succ);
    }
  }
}

void SplitCFG::addPredecessors(SplitCFGBlock* to, const clang::CFGBlock* from) {
  for (auto const& pre : from->preds()) {
    if (pre) {
      auto fit{sccfg_.find(pre->getBlockID())};
      SplitCFGBlock* next_pre{fit->second};
      to->predecessors_.push_back(next_pre);
    }
  }
}

void SplitCFG::dumpSCCFG() const {
  llvm::dbgs() << "sccfg( " << sccfg_.size() << ") ids: ";
  for (auto const& entry : sccfg_) {
    llvm::dbgs() << entry.first << "  ";
  }
  llvm::dbgs() << "\n";
}

void SplitCFG::dumpSplitElements(
    const llvm::SmallVector<std::pair<VectorCFGElementPtr, bool> >&
        split_elements) const {
  unsigned int id{0};
  for (auto const& elements : split_elements) {
    llvm::dbgs() << "Element number " << id << " has " << elements.first.size()
                 << " items and has ";
    if (elements.second) {
      llvm::dbgs() << " a WAIT\n";
    } else {
      llvm::dbgs() << " NO WAIT\n";
    }
    for (auto const& element : elements.first) {
      element->dump();
    }

    ++id;
  }
}

void SplitCFG::generate_paths() {
  /// Set of visited wait blocks.
  llvm::SmallPtrSet<const SplitCFGBlock*, 8> visited_waits;
  VectorSplitCFGBlock waits_in_stack;

  /// G = cfg_
  const clang::CFGBlock* block{&cfg_->getEntry()};
  const SplitCFGBlock* entry{sccfg_[block->getBlockID()]};
  // Mimic FIFO
  waits_in_stack.insert(waits_in_stack.begin(), entry);
  // Entry=>Entry is the reset.
  wait_next_state_.insert(
      std::make_pair(entry, std::make_pair(entry, next_state_count_)));
  ++next_state_count_;  // Reset has been assigned to 0
  do {
    entry = waits_in_stack.pop_back_val();
    llvm::dbgs() << "Processing SB " << entry->getBlockID() << "\n";
    dfs_pop_on_wait(entry, waits_in_stack, visited_waits);
    llvm::dbgs() << "\n";
  } while (!waits_in_stack.empty());

  addNextStatesToBlocks();
  dumpWaitNextStates();
  dumpPaths();
}

void SplitCFG::addNextStatesToBlocks() {
  for (auto const& wait : wait_next_state_) {
    SplitCFGBlock* wait_block{const_cast<SplitCFGBlock*>(wait.first)};
    auto next_block{wait.second.first};
    auto next_state_id{wait.second.second};

    wait_block->setNextState(next_state_id);
  }
}

void SplitCFG::dumpWaitNextStates() const {
  llvm::dbgs() << "Dump all wait next states\n";
  for (auto const& wait : wait_next_state_) {
    auto wait_block{wait.first};
    auto next_block{wait.second.first};
    auto next_state_id{wait.second.second};

    llvm::dbgs() << "SB" << wait_block->getBlockID() << " (SB"
                 << next_block->getBlockID() << ")"
                 << " [" << next_state_id << "]\n";
  }
}

void SplitCFG::dumpPaths() const {
  llvm::dbgs() << "Dump all SB paths to wait() found in the CFG.\n";
  unsigned int i{0};
  for (auto const& block_vector : paths_found_) {
    llvm::dbgs() << "Path S" << i++ << ": ";
    for (auto const& block : block_vector) {
      llvm::dbgs() << block->getBlockID() << " ";
      auto wit = wait_next_state_.find(block);
      if (wit != wait_next_state_.end()) {
        auto next_state{wit->second.second};
        llvm::dbgs() << "[" << next_state << "] ";
      }
    }
    if (i == 1) {
      llvm::dbgs() << " (reset path)";
    }

    llvm::dbgs() << "\n";
  }
}

void SplitCFG::createUnsplitBlocks() {
  for (auto begin_it = cfg_->nodes_begin(); begin_it != cfg_->nodes_end();
       ++begin_it) {
    auto block{*begin_it};
    SplitCFGBlock* new_block{new SplitCFGBlock{}};
    new_block->id_ = block->getBlockID();
    sccfg_.insert(std::make_pair(new_block->id_, new_block));
  }

  llvm::dbgs() << "sccfg( " << sccfg_.size() << ") ids: ";
  for (auto const& entry : sccfg_) {
    llvm::dbgs() << entry.first << "  ";
  }
  llvm::dbgs() << "\n";
}

void SplitCFG::construct_sccfg(const clang::CXXMethodDecl* method) {
  cfg_ = clang::CFG::buildCFG(method, method->getBody(), &context_,
                              clang::CFG::BuildOptions());

  clang::LangOptions lang_opts;
  cfg_->dump(lang_opts, true);

  createUnsplitBlocks();

  for (auto begin_it = cfg_->nodes_begin(); begin_it != cfg_->nodes_end();
       ++begin_it) {
    auto block{*begin_it};

    splitBlock(block);
  }
  llvm::dbgs() << "\nPrepare to update successors\n";
  dumpSCCFG();
}

SplitCFG::~SplitCFG() {
  for (auto block : sccfg_) {
    if (block.second) {
      delete block.second;
    }
  }
}

void SplitCFG::dump() const {
  llvm::dbgs() << "Dump all nodes in SCCFG\n";
  for (auto const& block : sccfg_) {
    SplitCFGBlock* sblock{block.second};
    sblock->dump();
  }
  dumpPaths();
}

void SplitCFG::dumpToDot() const {
  std::error_code OutErrorInfo;
  std::error_code ok;
  llvm::raw_fd_ostream dotos(llvm::StringRef("sccfg.dot"), OutErrorInfo,
                             llvm::sys::fs::OF_None);

  dotos << "digraph SCCFG {\n";
  dotos << " rankdir=TD\n";
  dotos << " node [shape=record]\n";
  /// Create names for each node and its pattern.
  for (auto const& block : sccfg_) {
    SplitCFGBlock* sblock{block.second};
    /// Generate the string with CFGElements

    std::string element_str{};
    llvm::raw_string_ostream element_os(element_str);
    auto num_elements{sblock->getElements().size()};
    auto i{0};

    element_os << "|{";
    for (auto const& element : sblock->getElements()) {
      element_os << "| " << i << ":";
      element->dumpToStream(element_os);
      ++i;
      // if (i < num_elements) {
      // element_os << "|";
      // }
    }
    element_os << "}";

    std::regex re("\\-");
    element_str = std::regex_replace(element_str, re, "\\-");
    std::regex replus("\\+");
    element_str = std::regex_replace(element_str, replus, "\\+");
    std::regex relt("\\<");
    element_str = std::regex_replace(element_str, relt, "\\<");
    std::regex regt("\\>");
    element_str = std::regex_replace(element_str, regt, "\\>");
    std::regex reamp("\\&");
    element_str = std::regex_replace(element_str, reamp, "\\&");

    if (sblock->hasWait()) {
      dotos << "SB" << sblock->getBlockID() << " [ \n color=red, label=\"SB"
            << sblock->getBlockID() << "\n"
            << sblock->getNextState() << "|" << element_str << "\"\n]"
            << "\n";
    } else {
      dotos << "SB" << sblock->getBlockID() << " [ \n label=\"SB"
            << sblock->getBlockID() << "\n"
            << element_str << "\"\n]"
            << "\n";
    }
  }
  /// Generate the connections
  for (auto const& block : sccfg_) {
    SplitCFGBlock* sblock{block.second};
    for (auto const& succ : sblock->successors_) {
      dotos << "SB" << sblock->getBlockID();
      dotos << " -> SB" << succ->getBlockID() << "\n";
    }
  }
  dotos << "}\n";
}

const llvm::SmallVectorImpl<SplitCFG::VectorSplitCFGBlock>&
SplitCFG::getPathsFound() {
  return paths_found_;
}

SplitCFG::SplitCFG(clang::ASTContext& context)
    : context_{context}, next_state_count_{0} {}

SplitCFG::SplitCFG(clang::ASTContext& context,
                   const clang::CXXMethodDecl* method)
    : context_{context}, next_state_count_{0} {
  construct_sccfg(method);
}
