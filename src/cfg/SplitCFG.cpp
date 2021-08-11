#include "SplitCFG.h"

#include "llvm/Support/Debug.h"

using namespace systemc_clang;

/// ===========================================
/// SplitCFG
/// ===========================================
SplitCFG::SplitCFG(clang::ASTContext& context) : context_{context} {}

void SplitCFG::dfs_pop_on_wait(
    const clang::CFGBlock* basic_block,
    llvm::SmallVectorImpl<const clang::CFGBlock*>& waits_in_stack,
    llvm::SmallPtrSetImpl<const clang::CFGBlock*>& visited_waits) {
  if (basic_block->succ_empty()) {
    /// Empty CFG block
    return;
  }

  llvm::SmallPtrSet<const clang::CFGBlock*, 8> visited;
  llvm::SmallVector<
      std::pair<const clang::CFGBlock*, clang::CFGBlock::const_succ_iterator>,
      8>
      visit_stack;
  llvm::SmallVector<const clang::CFGBlock*, 8> in_stack;

  visited.insert(basic_block);
  visit_stack.push_back(std::make_pair(basic_block, basic_block->succ_begin()));

  do {
    std::pair<const clang::CFGBlock*, clang::CFGBlock::const_succ_iterator>&
        Top = visit_stack.back();
    const clang::CFGBlock* parent_bb = Top.first;
    clang::CFGBlock::const_succ_iterator& I = Top.second;

    /// If BB has a wait() then just return.
    if (isWait(*parent_bb)) {
      llvm::dbgs() << "BB# " << parent_bb->getBlockID()
                   << " has a wait in it\n";

      /// The wait has not been processed yet so add it to the visited_wait
      /// sets. Then add it in the stack to process the waits.
      if (visited_waits.insert(parent_bb).second == true) {
        // Insert the successor of the block.
        llvm::dbgs() << "Visited BB#" << parent_bb->getBlockID() << "\n";
        while (I != parent_bb->succ_end()) {
          basic_block = *I++;
          if ((basic_block != nullptr)) {
            llvm::dbgs() << "Insert successor of BB#" << parent_bb->getBlockID()
                         << ": BB#" << basic_block->getBlockID() << "\n";
            waits_in_stack.push_back(basic_block);
          }
        }
      }
    }

    bool FoundNew = false;
    while (I != parent_bb->succ_end()) {
      basic_block = *I++;
      if ((basic_block != nullptr) && (!isWait(*parent_bb)) &&
          (visited.insert(basic_block).second)) {
        FoundNew = true;
        break;
      }
    }

    if (FoundNew) {
      // Go down one level if there is a unvisited successor.
      llvm::dbgs() << "Visited BB#" << parent_bb->getBlockID() << "\n";
      visit_stack.push_back(
          std::make_pair(basic_block, basic_block->succ_begin()));
    } else {
      // Go up one level.
      // llvm::dbgs() << "pop: " << visit_stack.size() << "\n";
      visit_stack.pop_back();
    }
  } while (!visit_stack.empty());
}

bool SplitCFG::isWait(const clang::CFGBlock& block) const {
  for (auto const& element : block.refs()) {
    if (auto cfg_stmt = element->getAs<clang::CFGStmt>()) {
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
  }

  return false;
};

void SplitCFG::generate_paths() {
  /// Set of visited wait blocks.
  llvm::SmallPtrSet<const clang::CFGBlock*, 8> visited_waits;
  llvm::SmallVector<const clang::CFGBlock*> waits_in_stack;

  /// G = cfg_
  const clang::CFGBlock* BB{&cfg_->getEntry()};
  waits_in_stack.push_back(BB);
  do {
    BB = waits_in_stack.pop_back_val();
    llvm::dbgs() << "Processing BB# " << BB->getBlockID() << "\n";
    dfs_pop_on_wait(BB, waits_in_stack, visited_waits);
    llvm::dbgs() << "\n";
  } while (!waits_in_stack.empty());
}

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

    /// Try to split the block.
    SplitCFGBlock sp{};
    sp.split_block(block);

    //////////////////////////////////////////
    //
    if (sp.hasWait()) {
      /// The block has been split.
      split_blocks_.insert(
          std::pair<const clang::CFGBlock*, SplitCFGBlock>(block, sp));
    }
  }
}

void SplitCFG::dump() const {
  llvm::dbgs() << "Dump all blocks that were split\n";
  for (auto const& sblock : split_blocks_) {
    llvm::dbgs() << "Block ID: " << sblock.first->getBlockID() << "\n";
    sblock.first->dump();
    const SplitCFGBlock* block_with_wait{&sblock.second};
    llvm::dbgs() << "Print all info for split block\n";
    block_with_wait->dump();
  }
}
