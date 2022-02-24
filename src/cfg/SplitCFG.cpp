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
#include <iostream>

using namespace systemc_clang;

////////////////////////////////////////////////////////
/// REWORK the Cthread Path generation
////////////////////////////////////////////////////////
///
void SplitCFG::dfs_visit_wait(
    const SplitCFGBlock* BB,
    llvm::SmallPtrSet<const SplitCFGBlock*, 32>& visited_blocks,
    llvm::SmallVectorImpl<const SplitCFGBlock*>& waits_to_visit,
    llvm::SmallPtrSetImpl<const SplitCFGBlock*>& visited_waits,
    llvm::SmallVector<std::pair<const SplitCFGBlock*, SplitCFGPathInfo>>&
        curr_path) {
  /// Empty CFG block
  if (BB->succ_empty()) {
    return;
  }

  popping_ = false;
  // successors to visit
  llvm::SmallVector<
      std::pair<const SplitCFGBlock*, SplitCFGBlock::const_succ_iterator>, 8>
      to_visit;

  visited_blocks.insert(BB);
  to_visit.push_back(std::make_pair(BB, BB->succ_begin()));

  do {
    std::pair<const SplitCFGBlock*, SplitCFGBlock::const_succ_iterator>& Top =
        to_visit.back();
    const SplitCFGBlock* ParentBB{Top.first};
    SplitCFGBlock::const_succ_iterator& I{Top.second};

    /// If the block contains a wait.  It is a wait block.
    bool bb_has_wait{(ParentBB->hasWait())};
    bool cond_block{isConditional(ParentBB)};

    /// If we are traversing down then we are not popping back up.
    if (!popping_) {
      llvm::dbgs() << "Visit Parent BB# " << ParentBB->getBlockID() << " isCondition: " << cond_block << " ";
      curr_path.push_back(std::make_pair(ParentBB, SplitCFGPathInfo{ParentBB}));
    } else {
    }

    /// Handle the case when the block has a wait in it.  There should only be a
    /// single statement, which is the wait.
    if (bb_has_wait) {
      llvm::dbgs() << " has WAIT ";
      bool found_suc{getUnvisitedSuccessor(ParentBB, I, visited_blocks, BB)};
      // A wait to be visited is added.
      if (visited_waits.insert(ParentBB).second) {
        waits_to_visit.insert(waits_to_visit.begin(), BB);
        ++next_state_count_;
        auto wit{wait_next_state_.find(ParentBB)};
        if (wit == wait_next_state_.end()) {
          wait_next_state_.insert(
              std::make_pair(ParentBB, std::make_pair(BB, next_state_count_)));
        }
      }
    }

    // Current block is a loop and has two successors.  So, we should start with
    // a new call to dfs, and provide a new visited_blocks.
    //
    // If there is a successor that has not been visited, then remember that
    // block.
    dumpVisitedBlocks(visited_blocks);
    bool found_succ{getUnvisitedSuccessor(ParentBB, I, visited_blocks, BB)};
    llvm::dbgs() << "Found successor BB " << BB->getBlockID()
                << " for parentBB " << ParentBB->getBlockID() << "\n";

    // FIXME: Should this have found_succ &&?
    if (isLoopWithTwoSuccessors(ParentBB) && found_succ) {
      do {
        // llvm::dbgs() << "\n==============================================";
        llvm::dbgs() << "\n#### BB " << ParentBB->getBlockID()
                     << " is a loop with 2 succ\n";
        llvm::SmallPtrSet<const SplitCFGBlock*, 32> loop_visited_blocks{visited_blocks};
        // ParentBB has been visited so don't revisit it
        loop_visited_blocks.insert(ParentBB);
        dumpVisitedBlocks(loop_visited_blocks);
        visited_blocks.insert(BB);
        llvm::dbgs() << "\n==============================================";
        llvm::dbgs() << "\nRecurse DFS starting at BB " << BB->getBlockID()
                     << " visited_block size " << visited_blocks.size() <<
                     "\n";
        dfs_visit_wait(BB, loop_visited_blocks, waits_to_visit, visited_waits,
                       curr_path);
        llvm::dbgs() << "\n";

        /// This only updates the visited blocks for the subgraph within the
        /// loop. We do not want to update the global visited_blocks yet.
        //updateVisitedBlocks(loop_visited_blocks, loop_visited_blocks);
        // dumpVisitedBlocks(visited_blocks);
        llvm::dbgs() << "\nEND Recurse DFS"
                     << " visited_block size " << visited_blocks.size() <<
                     "\n";
        llvm::dbgs() << "\n==============================================";

        /// There are two parts to updating the visited blocks.
        /// 1. You do not update the visited blocks when iterating over a
        /// subgraph.  This is because for each successor, we want to traverse
        /// the nodes without having visited them when traversing another
        /// successor.
        /// 2. However, when the subgraph has been fully visited, we need to
        /// update the visited_blocks with all the blocks visited so that we let
        /// the DFS know to not revist them. The below is doing (2).
        //

        found_succ = getUnvisitedSuccessor(ParentBB, I, visited_blocks, BB);
        // llvm::dbgs() << "unvisited BB " << BB->getBlockID() << " is succ "
        // << found_succ << "\n";
        //
        /// Update the visited blocks when there is no more successor to visit
        /// in the subgraph.  This means, the loop will be exiting.
        if (!found_succ) {
          updateVisitedBlocks(visited_blocks, loop_visited_blocks);
        }
      } while (found_succ);

    } else {
      // Recursive call in the if-then will traverse the subgraph.
      // Only insert successor if recursive call does not visit subgraph.
    }
    addSuccessorToVisitOrPop(bb_has_wait, BB, to_visit, found_succ);
    //std::cin.get();
    // llvm::dbgs() << "to_visit ";
    // dumpSmallVector(to_visit);
    //    llvm::dbgs() << " End loop \n";
  } while (!to_visit.empty());
}

void SplitCFG::dumpVisitedBlocks(
    llvm::SmallPtrSetImpl<const SplitCFGBlock*>& visited) {
  llvm::dbgs() << "\nVisited blocks ";
  for (const auto sblock : visited) {
    llvm::dbgs() << sblock->getBlockID() << "  ";
  }
  llvm::dbgs() << "\n";
}

void SplitCFG::updateVisitedBlocks(
    llvm::SmallPtrSetImpl<const SplitCFGBlock*>& to,
    const llvm::SmallPtrSetImpl<const SplitCFGBlock*>& from) {
  for (const auto sblock : from) {
    to.insert(sblock);
  }
}

bool SplitCFG::isLoopWithTwoSuccessors(const SplitCFGBlock* block) const {
  // The number of successor has to be exactly 2 in order to create a new
  // visited_blocks.
  //
  // Note that CFGBlock often times has a NULL successor.  We have to ignore
  // that.
  auto cfg_block{block->getCFGBlock()};
  bool last_succ_is_null{false};
  if (cfg_block->succ_size() == 2) {
    // Check that the last one is not NULL

    if (*cfg_block->succ_rbegin() == nullptr) {
      last_succ_is_null = true;
    }
  }

  return (block && isLoop(block) && (last_succ_is_null == false));
}

void SplitCFG::addSuccessorToVisitOrPop(
    bool parent_has_wait, const SplitCFGBlock* BB,
    llvm::SmallVector<
        std::pair<const SplitCFGBlock*, SplitCFGBlock::const_succ_iterator>, 8>&
        to_visit,
    bool found) {
  if ((found) && (parent_has_wait == false)) {
    // Go down one level if there is a unvisited successor.
    to_visit.push_back(std::make_pair(BB, BB->succ_begin()));
    popping_ = false;
  } else {
    // Go up one level.
    // llvm::dbgs() << "\npop: " << to_visit.size() << "\n";
    to_visit.pop_back();
    popping_ = true;
  }
}

bool SplitCFG::getUnvisitedSuccessor(
    const SplitCFGBlock* curr_block, SplitCFGBlock::const_succ_iterator& I,
    llvm::SmallPtrSetImpl<const SplitCFGBlock*>& visited_blocks,
    const SplitCFGBlock*& block) {
  bool found{false};
  while (I != curr_block->succ_end()) {
    block = *I++;
    if ((block != nullptr) && (visited_blocks.insert(block).second)) {
      found = true;
      break;
    }
  }
  return found;

  /*
  bool FoundNew = false;
  while (I != ParentBB->succ_end()) {
    BB = *I++;
    if ((BB != nullptr) && (visited_blocks.insert(BB).second)) {
      FoundNew = true;
      break;
    }
  }
  */
}

bool SplitCFG::isConditional(const SplitCFGBlock* block) const {
  /// Loop block has a terminator.
  /// The terminator is a clang::Stmt
  //
  if (block == nullptr) {
    return false;
  }

  //auto stmt{block->getCFGBlock()->getTerminatorStmt()};
  //return stmt && (llvm::isa<clang::IfStmt>(stmt));
  return block->isConditional();
}

bool SplitCFG::isLoop(const SplitCFGBlock* block) const {
  /// Loop block has a terminator.
  /// The terminator is a clang::Stmt
  //
  if (block == nullptr) {
    return false;
  }

  auto stmt{block->getCFGBlock()->getTerminatorStmt()};
  return stmt &&
         (llvm::isa<clang::WhileStmt>(stmt) ||
          llvm::isa<clang::ForStmt>(stmt) || llvm::isa<clang::DoStmt>(stmt));
}

void SplitCFG::dfs_rework() {
  /// G = cfg_
  VectorSplitCFGBlock waits_to_visit{};
  llvm::SmallPtrSet<const SplitCFGBlock*, 32> visited_waits;

  const clang::CFGBlock* block{&cfg_->getEntry()};
  const SplitCFGBlock* entry{sccfg_[block->getBlockID()]};
  llvm::SmallPtrSet<const SplitCFGBlock*, 32> visited_blocks;

  /// Record the current path.
  llvm::SmallVector<std::pair<const SplitCFGBlock*, SplitCFGPathInfo>>
      curr_path;
  // Special: Insert root node to start visiting.
  llvm::dbgs() << "@@@@@ DFS call for SB " << entry->getBlockID() << "\n";
  // std::cin.get();
  dfs_visit_wait(entry, visited_blocks, waits_to_visit, visited_waits,
                 curr_path);
  paths_.push_back(curr_path);

  // Add the next state.
  // wait_next_state_.insert(
  //    std::make_pair(entry, std::make_pair(entry, next_state_count_)));
  //++next_state_count_;  // Reset has been assigned to 0

  while (!waits_to_visit.empty()) {
    curr_path.clear();
    visited_blocks.clear();//llvm::SmallPtrSet<const SplitCFGBlock*, 32> visited_blocks;
    entry = waits_to_visit.pop_back_val();

    llvm::dbgs() << "\n@@@@@ DFS call for SB " << entry->getBlockID() << "\n";
    // std::cin.get();
    dfs_visit_wait(entry, visited_blocks, waits_to_visit, visited_waits,
                   curr_path);
    paths_.push_back(curr_path);
  }

  addNextStatesToBlocks();
  dumpWaitNextStates();
  dumpPaths();
}

////////////////////////////////////////////////////////

/// ===========================================
/// SplitCFG
/// ===========================================
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
  llvm::SmallVector<std::pair<VectorCFGElementPtr, bool>> split_elements;

  /// 0 elements so simply just add them and return.
  if (num_elements == 0) {
    split_elements.push_back(std::make_pair(vec_elements, false));
  } else {
    bool has_wait{false};
    for (auto const& element : block->refs()) {
      has_wait = false;
      llvm::dbgs() << "Is this a wait?    ";
      element->dump();

      /// refs() returns an iterator, which actually stores an
      /// ElementRefImpl<> interface. In order to get the correct pointer to
      /// CFGElement, we need to explicitly call operator->(). Odd!
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
    const llvm::SmallVectorImpl<std::pair<SplitCFG::VectorCFGElementPtr, bool>>&
        split_elements) {
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

    /// Propogate in the SplitCFGBlock whether the block has a terminator that has a break statement.
    //
    if (isLoop(new_split)) {
      new_split->identifyBreaks(context_);
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
    const llvm::SmallVector<std::pair<VectorCFGElementPtr, bool>>&
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

void SplitCFG::generate_paths() { dfs_rework(); }

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
                 << " [S" << next_state_id << "]\n";
  }
}

void SplitCFG::dumpPaths() const {
  llvm::dbgs() << "Dump all SB paths to wait() found in the CFG.\n";
  unsigned int i{0};

  for (auto const& block_vector : paths_) {
    llvm::dbgs() << "Path S" << i++ << ": ";
    for (auto const& block : block_vector) {
      auto sblock{block.first};
      llvm::dbgs() << sblock->getBlockID() << " ";
      // Print the wait state
      auto wit = wait_next_state_.find(sblock);
      if (wit != wait_next_state_.end()) {
        auto next_state{wit->second.second};
        llvm::dbgs() << "[S" << next_state << "] ";
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
  
    /// Set if the block is an IF conditional
    auto stmt{block->getTerminatorStmt()};
    new_block->is_conditional_ = stmt && llvm::isa<clang::IfStmt>(stmt);

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
  dumpWaitNextStates();
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
    const SplitCFGBlock* sblock{block.second};
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
            << " [" << sblock->getNextState() << "] | WAIT " << element_str
            << "\"\n]"
            << "\n";
    } else {
      if (isLoop(sblock)) {
        auto terminator{sblock->getCFGBlock()->getTerminatorStmt()};
        if (llvm::isa<clang::WhileStmt>(terminator)) {
          element_str += " | WHILE ";
        }
        if (llvm::isa<clang::ForStmt>(terminator)) {
          element_str += " | FOR ";
        }
        if (llvm::isa<clang::DoStmt>(terminator)) {
          element_str += " | DOWHILE ";
        }
        // sblock->getCFGBlock()->getTerminatorStmt()->dumpPretty(context);
        // auto stmt{sblock->getCFGBlock()->getTerminatorStmt()};
        // stmt->printPretty(dotos, nullptr,
        // clang::PrintingPolicy(context_.getLangOpts()));
      }

      if (isConditional(sblock)) {
        auto terminator{sblock->getCFGBlock()->getTerminatorStmt()};
        if (llvm::isa<clang::IfStmt>(terminator)) {
          element_str += " | IF ";
        }
      }

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

const llvm::SmallVectorImpl<
    llvm::SmallVector<std::pair<const SplitCFGBlock*, SplitCFGPathInfo>>>&
SplitCFG::getPathsFound() {
  return paths_;
}

SplitCFG::SplitCFG(clang::ASTContext& context)
    : context_{context}, next_state_count_{0} {}

SplitCFG::SplitCFG(clang::ASTContext& context,
                   const clang::CXXMethodDecl* method)
    : context_{context}, next_state_count_{0} {
  construct_sccfg(method);
}
