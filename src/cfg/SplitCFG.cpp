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

#include <regex>
#include <iostream>
#include <queue>
#include <set>

using namespace systemc_clang;
using namespace clang;

////////////////////////////////////////////////////////
/// class SupplementaryInfo
////////////////////////////////////////////////////////
SupplementaryInfo::SupplementaryInfo(const SplitCFGBlock* block)
    : split_block_{block}, false_idx_{0}, path_idx_{0} {}

SupplementaryInfo::SupplementaryInfo(const SupplementaryInfo& from) {
  *this = from;
}

SupplementaryInfo& SupplementaryInfo::operator=(const SupplementaryInfo& from) {
  false_idx_ = from.false_idx_;
  path_idx_ = from.path_idx_;
  split_block_ = from.split_block_;
  return *this;
}

int SupplementaryInfo::getPathId() const { return path_idx_; }

int SupplementaryInfo::getFalseId() const { return false_idx_; }

const SplitCFGBlock* SupplementaryInfo::getSplitCFGBlock() const {
  return split_block_;
}

SupplementaryInfo::~SupplementaryInfo() {}

////////////////////////////////////////////////////////
/// class SplitCFGPathInfo
////////////////////////////////////////////////////////
SplitCFGPathInfo::SplitCFGPathInfo(const SplitCFGBlock* block)
    : split_block_{block},
      cfg_block_{block->getCFGBlock()} {
          // false_startix = -1;
      };

SplitCFGPathInfo::SplitCFGPathInfo(const SplitCFGPathInfo& from) {
  *this = from;
}

SplitCFGPathInfo& SplitCFGPathInfo::operator=(const SplitCFGPathInfo& from) {
  split_block_ = from.split_block_;
  cfg_block_ = from.cfg_block_;
  // false_startix = from.false_startix;
  true_path_ = from.true_path_;
  false_path_ = from.false_path_;
  path_idx_ = from.path_idx_;
  return *this;
}

std::string SplitCFGPathInfo::toStringFalsePath() const {
  std::string str{};
  for (const auto& block : false_path_) {
    str += std::to_string(block->getBlockID());
    str += " ";
  }
  if (str.size() > 0) {
    str.pop_back();
  }

  return str;
}

std::string SplitCFGPathInfo::toStringTruePath() const {
  std::string str{};
  for (const auto& block : true_path_) {
    str += std::to_string(block->getBlockID());
    str += " ";
  }
  if (str.size() > 0) {
    str.pop_back();
  }

  return str;
}

void SplitCFGPathInfo::dump() const {
  llvm::dbgs() << "* BB# " << split_block_->getBlockID() << ": ";
  //           << " F:" << false_startix << "\n";
  llvm::dbgs() << "T ";
  for (const auto block : true_path_) {
    llvm::dbgs() << block->getBlockID() << " ";
  }
  llvm::dbgs() << "; F ";
  for (const auto block : false_path_) {
    llvm::dbgs() << block->getBlockID() << " ";
  }
  llvm::dbgs() << "*";
}

SplitCFGPathInfo::~SplitCFGPathInfo() {}

void SplitCFG::addPathToSpecialNode(const SplitCFG::SplitCFGPath& from) {
  for (auto const& path_pair : from) {
    sub_path_to_special_node_.push_back(path_pair);
  }
}
////////////////////////////////////////////////////////
/// REWORK the Cthread Path generation
////////////////////////////////////////////////////////<
///
// const llvm::SmallVector<std::pair<const SplitCFGBlock*, SplitCFGPathInfo>>
const SplitCFG::SplitCFGPath SplitCFG::dfs_visit_wait(
    const SplitCFGBlock* BB,
    llvm::SmallPtrSet<const SplitCFGBlock*, 32>& visited_blocks,
    llvm::SmallVectorImpl<const SplitCFGBlock*>& waits_to_visit,
    llvm::SmallPtrSetImpl<const SplitCFGBlock*>& visited_waits,
    llvm::SmallVector<SplitCFGPathPair>& curr_path) {
  bool true_path_{false};
  bool false_path_{false};
  bool stop_local_path{false};

  SplitCFGPath local_path;
  /// Empty CFG block
  if (BB->succ_empty()) {
    return local_path;
  }

  popping_ = false;
  // successors to visit
  llvm::SmallVector<
      std::pair<const SplitCFGBlock*, SplitCFGBlock::const_succ_iterator>, 8>
      to_visit;

  visited_blocks.insert(BB);
  to_visit.push_back(std::make_pair(BB, BB->succ_begin()));

  // Loop
  do {
    std::pair<const SplitCFGBlock*, SplitCFGBlock::const_succ_iterator>& Top =
        to_visit.back();
    const SplitCFGBlock* ParentBB{Top.first};
    SplitCFGBlock::const_succ_iterator& I{Top.second};

    /// If the block contains a wait.  It is a wait block.
    bool bb_has_wait{(ParentBB->hasWait())};
    bool cond_block{isConditional(ParentBB)};

    /// If we are traversing down then we are not popping back up.
    int id{-1};
    if (!popping_) {
      llvm::dbgs() << "Visit Parent BB# " << ParentBB->getBlockID() << "   ; ";
      SupplementaryInfo info{ParentBB};
      info.path_idx_ = curr_path.size();
      curr_path.push_back(std::make_pair(ParentBB, info));
      id = curr_path.size() - 1;

      if (!stop_local_path) {
        local_path.push_back(
            std::make_pair(ParentBB, SupplementaryInfo{ParentBB}));
      }
    } else {
      // llvm::dbgs() << "POPPING\n";
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

    // If there is a successor that has not been visited, then remember that
    // block.
    llvm::SmallPtrSet<const SplitCFGBlock*, 32> save_visited_blocks{
        visited_blocks};
    // dumpVisitedBlocks(visited_blocks);

    bool found_succ{getUnvisitedSuccessor(ParentBB, I, visited_blocks, BB)};
    if (found_succ) {
      llvm::dbgs() << "Found successor BB " << BB->getBlockID()
                   << " for parentBB " << ParentBB->getBlockID() << "\n";
    }

    // Use the recursive call for loops with 2 successors, and IF blocks.
    // START: LOOP/SPECIAL NODE
    if ((isLoopWithTwoSuccessors(ParentBB) || isConditional(ParentBB)) &&
        found_succ) {
      do {
        // llvm::dbgs() << "\n==============================================";
        llvm::dbgs() << "\n#### ParentBB " << ParentBB->getBlockID()
                     << " is a loop with 2 succ or conditional.  Succ BB# "
                     << BB->getBlockID() << "\n";

        /// Check if the found succesor is the TRUE or FALSE for the
        /// conditional. The first successor is the true, and the second is the
        /// false.
        auto block_path{path_info_.find(ParentBB)};
        if (block_path != path_info_.end()) {
          if (const auto true_block_succ = *ParentBB->succ_begin()) {
            if (true_block_succ == BB) {
              true_path_ = true;
              llvm::dbgs() << "Going down the TRUE path for BB"
                           << block_path->first->getBlockID() << " \n";

            } else {
              false_path_ = true;
              llvm::dbgs() << "Going down the FALSE path for BB"
                           << block_path->first->getBlockID() << " \n";
            }
          }
        }

        llvm::SmallPtrSet<const SplitCFGBlock*, 32> loop_visited_blocks{
            save_visited_blocks};
        // llvm::SmallPtrSet<const SplitCFGBlock*, 32> loop_visited_blocks{};

        // TODO: Do we need this?
        // visited_blocks.insert(BB);

        // ParentBB has been visited so don't revisit it
        loop_visited_blocks.insert(ParentBB);

        llvm::dbgs() << "\n==============================================";
        llvm::dbgs() << "\n[START] Recursive DFS starting at BB "
                     << BB->getBlockID() << " parent BB# "
                     << ParentBB->getBlockID() << " T " << true_path_ << " F "
                     << false_path_ << "\n";
        llvm::dbgs() << "loop visited ";
        dumpVisitedBlocks(loop_visited_blocks);
        llvm::dbgs() << "visited      ";
        dumpVisitedBlocks(visited_blocks);
        llvm::dbgs() << "local_path    : ";
        dumpSmallVector(local_path);
        llvm::dbgs() << "\n";

        SplitCFGPath sub_path_to_special_node;
        // =================== DFS Recurse ====================
        sub_path_to_special_node = dfs_visit_wait(
            BB, loop_visited_blocks, waits_to_visit, visited_waits, curr_path);
        llvm::dbgs() << "\n";
        // =================== END DFS Recurse ====================

        addPathToSpecialNode(sub_path_to_special_node);

        /// This only updates the visited blocks for the subgraph within the
        /// loop. We do not want to update the global visited_blocks yet.
        // updateVisitedBlocks(loop_visited_blocks, loop_visited_blocks);
        llvm::dbgs() << "[AFTER] DFS for BB# " << BB->getBlockID()
                     << " for parent BB# " << ParentBB->getBlockID() << " T "
                     << true_path_ << " F " << false_path_ << "\n";
        llvm::dbgs() << "loop visited ";
        dumpVisitedBlocks(loop_visited_blocks);
        llvm::dbgs() << "visited      ";
        dumpVisitedBlocks(visited_blocks);
        llvm::dbgs() << "sub_path      : ";
        dumpSmallVector(sub_path_to_special_node);

        llvm::dbgs() << "\n[END] Recurse DFS at BB " << BB->getBlockID()
                     << " parent BB# " << ParentBB->getBlockID() << " T "
                     << true_path_ << " F " << false_path_ << "\n";

        // std::cin.get();
        /// Haven't looked at new successor yet.
        /// Find the new visited blocks.
        // new_visited.insert(BB);

        found_succ = getUnvisitedSuccessor(ParentBB, I, visited_blocks, BB);

        if (true_path_) {
          llvm::dbgs() << "[TRUE PATH] Set path for BB "
                       << ParentBB->getBlockID() << " to sub_path ";
          dumpSmallVector(sub_path_to_special_node);
          llvm::dbgs() << "\n";

          true_path_ = false;
          auto& info{curr_path[id]};
          setTruePathInfo(ParentBB, sub_path_to_special_node, curr_path.size());
          llvm::dbgs() << "curr_path     :";
          dumpSmallVector(curr_path);
          info.second.false_idx_ = curr_path.size();

          // If there is no successor, then we should find the next block on the
          // false path and set that to the false_idx_ from curr_path.
          if (!found_succ) {
            // There is no successor for ParentBB that is not visited.  So, it
            // must have already been visited, and we must find it in the
            // curr_path.
            const clang::CFGBlock* cblock{ParentBB->getCFGBlock()};
            const clang::CFGBlock* fsucc{*cblock->succ_rbegin()};

            if (fsucc) {
              fsucc->dump();
              // Find the index from curr_path, and set it to the false_idx_, if
              // found.
              for (auto const visited_blk : curr_path) {
                if (visited_blk.first->getCFGBlock() == fsucc) {
                  info.second.false_idx_ = visited_blk.second.path_idx_;
                }
              }
            }
          }

          // llvm::dbgs() << "ID is " << id << " INFO id is "
          // << info.second.path_idx_ << " SB "
          // << info.first->getBlockID() << " size of path "
          // << curr_path.size() << "\n";
        } else if (false_path_) {
          // llvm::dbgs() << "FALSE PATH DIFF BB# " << ParentBB->getBlockID();
          llvm::dbgs() << "[FALSE PATH] Set path for BB "
                       << ParentBB->getBlockID() << " to sub_path ";
          dumpSmallVector(sub_path_to_special_node);
          llvm::dbgs() << "\n";
          false_path_ = false;
          setFalsePathInfo(ParentBB, sub_path_to_special_node);
          dumpPathInfo();
          dumpAllPathInfo();
        }

        // dumpVisitedBlocks(new_visited);

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

        if (!found_succ) {
          llvm::dbgs() << "\nNO SUCCESOR for BB " << ParentBB->getBlockID()
                       << "\n";
        }
        /// Update the visited blocks when there is no more successor to visit
        /// in the subgraph.  This means, the loop will be exiting.
        if (!found_succ) {
          updateVisitedBlocks(visited_blocks, loop_visited_blocks);
        }

      } while (found_succ);

      llvm::dbgs() << "@@@@@@ Exiting loop\n";
      dumpSmallVector(sub_path_to_special_node_);

    } else {
      // Recursive call in the if-then will traverse the subgraph.
      // Only insert successor if recursive call does not visit subgraph.
    }
    addSuccessorToVisitOrPop(bb_has_wait, BB, to_visit, found_succ);
    // std::cin.get();
    //  llvm::dbgs() << "to_visit ";
    //  dumpSmallVector(to_visit);
    //     llvm::dbgs() << " End loop \n";
  } while (!to_visit.empty());
  return local_path;
}

void SplitCFG::setTruePathInfo(const SplitCFGBlock* sblock,
                               const SplitCFGPath& newly_visited, int ix) {
  auto block_path{path_info_.find(sblock)};
  if (block_path != path_info_.end() && !block_path->second.isTruePathValid()) {
    // block_path->second.false_startix = ix;  // index is start of false path
    for (const auto block : newly_visited) {
      block_path->second.true_path_.push_back(block.first);
    }
  }
}

void SplitCFG::setFalsePathInfo(const SplitCFGBlock* sblock,
                                const SplitCFGPath& newly_visited) {
  auto block_path{path_info_.find(sblock)};
  if (block_path != path_info_.end() &&
      !block_path->second.isFalsePathValid()) {
    for (const auto block : newly_visited) {
      block_path->second.false_path_.push_back(block.first);
      llvm::dbgs() << "=> " << block.first->getBlockID() << " ";
    }
  }
}

void SplitCFG::setDifference(
    const llvm::SmallPtrSetImpl<const SplitCFGBlock*>& larger,
    const llvm::SmallPtrSetImpl<const SplitCFGBlock*>& smaller,
    llvm::SmallPtrSetImpl<const SplitCFGBlock*>& to) {
  for (const auto& element : larger) {
    if (!smaller.contains(element)) {
      to.insert(element);
    }
  }
}

bool SplitCFG::isTruePath(const SplitCFGBlock* parent_block,
                          const SplitCFGBlock* block) const {
  if (!parent_block || !block) {
    return false;
  }

  return (*parent_block->succ_begin() == block);
}

void SplitCFG::dumpVisitedBlocks(
    llvm::SmallPtrSetImpl<const SplitCFGBlock*>& visited) {
  llvm::dbgs() << " : ";
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
  return block->isLoopWithTwoSuccessors();
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
}

bool SplitCFG::isConditional(const SplitCFGBlock* block) const {
  /// Loop block has a terminator.
  /// The terminator is a clang::Stmt
  //
  if (block == nullptr) {
    return false;
  }

  return block->isConditional() && !isTernaryOperator(block);
}

bool SplitCFG::isTernaryOperator(const SplitCFGBlock* block) const {
  if (block == nullptr) {
    return false;
  }
  auto stmt{block->getCFGBlock()->getTerminatorStmt()};
  return stmt && clang::dyn_cast<clang::ConditionalOperator>(stmt);
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
  // llvm::SmallVector<std::pair<const SplitCFGBlock*, SplitCFGPathInfo>>
  SplitCFGPath curr_path;
  // Special: Insert root node to start visiting.
  llvm::dbgs()
      << "##############################################################\n";
  llvm::dbgs() << "@@@@@ DFS call for SB " << entry->getBlockID() << "\n";
  // std::cin.get();

  // Make copy of original paths_info_
  std::unordered_map<const SplitCFGBlock*, SplitCFGPathInfo>
      prepared_path_info_{path_info_};

  dfs_visit_wait(entry, visited_blocks, waits_to_visit, visited_waits,
                 curr_path);
  paths_.push_back(curr_path);
  llvm::dbgs() << "curr path 1 below\n";
  dumpCurrPath(curr_path);
  dumpPaths();

  all_path_info_.push_back(path_info_);
  path_info_ = prepared_path_info_;

  while (!waits_to_visit.empty()) {
    curr_path.clear();
    visited_blocks.clear();  // llvm::SmallPtrSet<const SplitCFGBlock*, 32>
                             // visited_blocks;
    entry = waits_to_visit.pop_back_val();
    llvm::dbgs()
        << "##############################################################\n";

    llvm::dbgs() << "\n@@@@@ DFS call for SB " << entry->getBlockID() << "\n";

    dfs_visit_wait(entry, visited_blocks, waits_to_visit, visited_waits,
                   curr_path);
    paths_.push_back(curr_path);
    llvm::dbgs() << "curr path 2 below\n";
    dumpCurrPath(curr_path);
    all_path_info_.push_back(path_info_);
    path_info_ = prepared_path_info_;
  }

  addNextStatesToBlocks();
  dumpWaitNextStates();
  dumpPaths();
  dumpPathInfo();
  dumpAllPathInfo();
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

            // Check if we can evaluate the expression with constant folding
            // We only accept integer ones.  So, for other expressions it might
            // not work.
            if (first_arg->isEvaluatable(context_)) {
              clang::Expr::EvalResult result{};
              first_arg->EvaluateAsInt(result, context_);
              llvm::dbgs() << " ARG VAL: ";
              result.Val.dump();
              return result.Val.getInt();
            }
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
      /// Use cast to CallExpr instead of CXXMemberCallExpr.
      if (auto cxx_me = llvm::dyn_cast<clang::CallExpr>(expr)) {
        if (auto direct_callee = cxx_me->getDirectCallee()) {
          auto name{direct_callee->getNameInfo().getAsString()};
          if (name == std::string("wait")) {
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
      auto scit{sccfg_.find(block->getBlockID())};
      new_split = new SplitCFGBlock{};  //*scit->second};
      new_split->id_ = block->getBlockID() * 10 + id;

      auto is_wait = elements.second;
      auto stmt{block->getTerminatorStmt()};
      // A wait block cannot be a conditional.
      new_split->is_conditional_ =
          !is_wait && (stmt && (llvm::isa<clang::IfStmt>(stmt) ||
                                llvm::isa<clang::ConditionalOperator>(stmt)));

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

    /// Propogate in the SplitCFGBlock whether the block has a terminator that
    /// has a break statement.
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

void SplitCFG::dumpCurrPath(
    // llvm::SmallVector<std::pair<const SplitCFGBlock*, SplitCFGPathInfo>>&
    SplitCFGPath& curr_path) const {
  llvm::dbgs() << "Dump curr SB path to wait() found in the CFG.\n";

  for (auto const& block : curr_path) {
    auto sblock{block.first};
    llvm::dbgs() << sblock->getBlockID() << " ";
    // Print the wait state
    auto wit = wait_next_state_.find(sblock);
    if (wit != wait_next_state_.end()) {
      auto next_state{wit->second.second};
      llvm::dbgs() << "[S" << next_state << "] ";
    }
  }
  llvm::dbgs() << "\n";
}

void SplitCFG::dumpPaths() const {
  llvm::dbgs() << "#### Dump all SB paths to wait() found in the CFG.\n";
  unsigned int i{0};

  for (auto const& block_vector : paths_) {
    llvm::dbgs() << "Path S" << i++ << ": ";
    for (auto const& block : block_vector) {
      auto sblock{block.first};
      auto supp_info{block.second};
      auto found_it{path_info_.find(supp_info.split_block_)};
      // The key thing to remember about the false idx (third element) is that
      // it is the index created in the path.  It points to the index of the
      // first block that is in the false path.
      //
      // (path idx, blockID, false idx).

      llvm::dbgs() << "(" << supp_info.path_idx_ << "," << sblock->getBlockID()
                   << "," << supp_info.false_idx_;
      if (found_it != path_info_.end()) {
        llvm::dbgs() << " |" << found_it->second.getFalsePath().size() << "|";
        if (found_it->second.getFalsePath().size() > 0) {
          found_it->second.dump();
        }
      }
      llvm::dbgs() << ") ";
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

void SplitCFG::dumpAllPathInfo() const {
  llvm::dbgs() << "Dump [ALL] path info\n";

  unsigned int i{0};
  for (const auto& path : all_path_info_) {
    llvm::dbgs() << "S" << i << " : \n";
    for (const auto& path_pair : path) {
      auto sblock{path_pair.first};
      auto sinfo{path_pair.second};
      // llvm::dbgs() << "BB #" << sblock->getBlockID() << "\n";
      sinfo.dump();
      llvm::dbgs() << "\n";
    }
    ++i;
  }
}

void SplitCFG::dumpPathInfo() const {
  llvm::dbgs() << "Dump all path info\n";

  for (const auto& path_pair : path_info_) {
    auto sblock{path_pair.first};
    auto sinfo{path_pair.second};
    // llvm::dbgs() << "BB #" << sblock->getBlockID() << "\n";
    sinfo.dump();
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
    new_block->is_conditional_ =
        stmt && (llvm::isa<clang::IfStmt>(stmt) ||
                 llvm::isa<clang::ConditionalOperator>(stmt));

    bool last_succ_is_null{false};
    if (block->succ_size() == 2) {
      // Check that the last one is not NULL
      if (*block->succ_rbegin() == nullptr) {
        last_succ_is_null = true;
      }
    }

    /// Set if the block is a loop with two successors
    bool is_loop{stmt && (llvm::isa<clang::WhileStmt>(stmt) ||
                          llvm::isa<clang::ForStmt>(stmt) ||
                          llvm::isa<clang::DoStmt>(stmt))};

    new_block->is_loop_with_two_succ_ =
        (stmt && is_loop && (last_succ_is_null == false));

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

    // Identify whether the CFG block is a ternary operator.
    auto stmt{block->getTerminatorStmt()};
    if (stmt && clang::dyn_cast<clang::ConditionalOperator>(stmt)) {
      has_ternary_op_ = true;
    }

    splitBlock(block);
  }
  preparePathInfo();

  // Only generate the confluence block map if there is a ternary operator in
  // the CFG.
  if (has_ternary_op_) {
    identifyConfluenceBlocks();
  }

  llvm::dbgs() << "\nPrepare to update successors\n";
  dumpSCCFG();
}

void SplitCFG::preparePathInfo() {
  for (auto& block : sccfg_) {
    if (isLoopWithTwoSuccessors(block.second) || isConditional(block.second)) {
      path_info_.insert(
          std::make_pair(block.second, SplitCFGPathInfo(block.second)));
    }
  }
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
  dumpPathInfo();
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
      if (isTernaryOperator(sblock)) {
        element_str += " | TERNOP";
      }

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

[[deprecated("Use SplitCFG::getAllPathInfo()")]] const std::unordered_map<
    const SplitCFGBlock*, SplitCFGPathInfo>&
SplitCFG::getPathInfo() const {
  return path_info_;
}

const llvm::SmallVector<
    std::unordered_map<const SplitCFGBlock*, SplitCFGPathInfo>>&
SplitCFG::getAllPathInfo() const {
  return all_path_info_;
}

const llvm::SmallVectorImpl<llvm::SmallVector<SplitCFG::SplitCFGPathPair>>&
SplitCFG::getPathsFound() {
  return paths_;
}

SplitCFG::SplitCFG(clang::ASTContext& context)
    : context_{context},
      next_state_count_{0},
      popping_{false},
      outter_top_(nullptr),
      has_ternary_op_{false} {}
// true_path_{false},
// false_path_{false} {}

SplitCFG::SplitCFG(clang::ASTContext& context,
                   const clang::CXXMethodDecl* method)
    : context_{context},
      next_state_count_{0},
      popping_{false},
      outter_top_(nullptr),
      has_ternary_op_{false} {
  //     true_path_{false},
  //      false_path_{false} {
  construct_sccfg(method);
}

std::map<SplitCFGBlock*, SplitCFGBlock*> SplitCFG::getConfluenceBlocks() const {
  return cop_;
}

std::set<SplitCFGBlock*> SplitCFG::identifySkipBlocks() {
  llvm::dbgs() << "########### BFS Identify confluence blocks ############ \n";
  std::queue<SplitCFGBlock*> Q{};
  std::set<SplitCFGBlock*> discovered{};

  if (!outter_top_) return discovered;

  SplitCFGBlock* v{outter_top_};
  SplitCFGBlock* target = cop_[outter_top_];
  llvm::dbgs() << "Outter ternop is BB" << outter_top_->getBlockID() << " and confluence block is BB" << target->getBlockID() << "\n";
  // if (!source) v = sccfg_[cfg_->getEntry().getBlockID()];
  // else v = source; 

  discovered.insert(v);

  Q.push(v);

  while (!Q.empty()) {
    v = Q.front();
    Q.pop();
    llvm::dbgs() << "visited " << v->getBlockID() << "\n";

    for (auto succ : v->getCFGBlock()->succs()) {
      if (succ && (v->getBlockID() != target->getBlockID())) {
        auto blk{sccfg_[succ->getBlockID()]};
        if (discovered.find(blk) == discovered.end()) {
          discovered.insert(blk);
          Q.push(blk);
        }
      }
    }
  }

  llvm::dbgs() << "Discovered\n";
  for (auto disc : discovered ) {
    llvm::dbgs() << disc->getBlockID() <<"  ";

  }
  return discovered;
}

void SplitCFG::identifyConfluenceBlocks() {
  llvm::dbgs() << "########### Identify confluence blocks ############ \n";

  // ConditionalOperator block => Confluence Block
  // std::map<SplitFGBlock*, SplitCFGBlock*> cop_;
  std::vector<SplitCFGBlock*> ternops;

  std::vector<SplitCFGBlock*> S{};
  std::set<SplitCFGBlock*> discovered{};
  // Do DFS whenever you reach a conditional operator block.
  SplitCFGBlock* v = sccfg_[cfg_->getEntry().getBlockID()];

  SplitCFGBlock* outter{nullptr};

  S.push_back(v);
  while (!S.empty()) {
    v = S.back();
    S.pop_back();
    if (discovered.find(v) == discovered.end()) {
      discovered.insert(v);
      llvm::dbgs() << "visited " << v->getBlockID() << "\n";

      // Found ConditionalOperator
      auto stmt{v->getCFGBlock()->getTerminatorStmt()};
      if (stmt && clang::dyn_cast<clang::ConditionalOperator>(stmt)) {
        llvm::dbgs() << "Found a TERNARY OP block\n";

        if (!outter) outter = v;

        cop_.insert(std::make_pair(v, nullptr));
        ternops.push_back(v);

      } else if (ternops.size() > 0) {
        auto top_cop{ternops.back()};
        // Successor is the confluence
        if (v->getCFGBlock()->succ_size() == 1) {
          auto conf_blk{*v->getCFGBlock()->succ_begin()};
          llvm::dbgs() << "Found confluence block of " << conf_blk->getBlockID()
                       << " from block " << v->getBlockID() << " of "
                       << top_cop->getBlockID() << "\n";
          cop_[top_cop] = sccfg_[conf_blk->getBlockID()];

          conf_blk->dump();
          ternops.pop_back();
        }
      }

      for (auto next_v : v->getCFGBlock()->succs()) {
        if (next_v) S.push_back(sccfg_[next_v->getBlockID()]);
      }
    }
  }
  // Print the cop map.
  llvm::dbgs() << "Block ids for COP ";
  for (auto& co : cop_) {
    llvm::dbgs() << co.first->getBlockID() << " :=> " << co.second->getBlockID()
                 << " ;  ";
  }
  llvm::dbgs() << "\n";

  outter_top_ = outter;
}
