#include "SplitCFG.h"

#include "llvm/Support/Debug.h"
#include "llvm/ADT/PostOrderIterator.h"

using namespace systemc_clang;

/// ===========================================
/// SplitCFG
/// ===========================================
void SplitCFG::dfs_pop_on_wait(
    const clang::CFGBlock* basic_block,
    llvm::SmallVectorImpl<const clang::CFGBlock*>& waits_in_stack,
    llvm::SmallPtrSetImpl<const clang::CFGBlock*>& visited_waits) {
  /// Empty CFG block
  if (basic_block->succ_empty()) {
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

  VectorCFGBlock curr_path;
  do {
    std::pair<const clang::CFGBlock*, clang::CFGBlock::const_succ_iterator>&
        Top = visit_stack.back();
    const clang::CFGBlock* parent_bb = Top.first;
    clang::CFGBlock::const_succ_iterator& I = Top.second;

    /// If BB has a wait() then just return.
    bool bb_has_wait{isWait(*parent_bb)};

    if (bb_has_wait) {
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
            waits_in_stack.push_back(basic_block);
            break;
          }
        }
      }
    }

    /// A new BB has been found.  Add its successors to be visited in the
    /// future.
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
      // Go down one level if there is a unvisited successor.
      llvm::dbgs() << "Visited BB#" << parent_bb->getBlockID() << "\n";
      visit_stack.push_back(
          std::make_pair(basic_block, basic_block->succ_begin()));
      curr_path.push_back(parent_bb);
    } else {
      // Go up one level.
      // llvm::dbgs() << "pop: " << visit_stack.size() << "\n";
      visit_stack.pop_back();
    }
  } while (!visit_stack.empty());

  /// Insert the path constructed.
  paths_found_.push_back(curr_path);
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

  //dumpSplitElements(split_elements);
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
    if (id == 0 ) {
      auto scit{sccfg_.find(block->getBlockID())};
      // Must be true.
      bool already_exists{scit != sccfg_.end()};
      new_split = scit->second;
      llvm::dbgs() << " already exists ";
      new_split->id_ = block->getBlockID();
    } else {
      new_split = new SplitCFGBlock{};
      new_split->id_ = block->getBlockID() * 10 + id;
      sccfg_.insert(std::make_pair(block->getBlockID() * 10 + id, new_split));
    }

    new_split->block_ = block;
    new_split->has_wait_ = elements.second;
    new_split->elements_ = elements.first;



    //new_split->id_ = block->getBlockID();

    /*
    /// If there is a wait then update successor.
    if (new_split->has_wait_) {
      llvm::dbgs() << "has a WAIT\n";
      /// If it is the first block then it is already_exists should be true
      /// (invariant).
      if (id != 0) {
        prev_block->successors_.push_back(new_split);
        new_split->predecessors_.push_back(prev_block);
        // new_split->id_ = block->getBlockID() * 10 + id;
        // sccfg_.insert(std::make_pair(block->getBlockID() * 10 + id, new_split));
      }
    } else {
      /// No wait so it should be sequenial statements.
      /// Only add if it does not exist already.
      if (!already_exists) {
        sccfg_.insert(std::make_pair(block->getBlockID(), new_split));
        llvm::dbgs() << "No wait but insert " << block->getBlockID() << "\n";
      }
    }
    */
    prev_block = new_split;
    ++id;
  }
}

void SplitCFG::dumpSCCFG() {
  llvm::dbgs() << "sccfg( " << sccfg_.size() << ") ids: ";
  for (auto const& entry : sccfg_) {
    llvm::dbgs() << entry.first << "  ";
  }
  llvm::dbgs() << "\n";


}

void SplitCFG::dumpSplitElements(
    const llvm::SmallVector<std::pair<VectorCFGElementPtr, bool> >&
        split_elements) {
  unsigned int id{0};
  for (auto const& elements : split_elements) {
    llvm::dbgs() << "Element number " << id << " has " << elements.first.size()
                 << " items and has ";
    if (elements.second) {
      llvm::dbgs() << " a WAIT\n";
    } else {
      llvm::dbgs() << " NO WAIT\n";
    }
    for (auto const &element: elements.first) {
      element->dump();
    }

    ++id;
  }
}
void SplitCFG::updateSuccessors() {
  /// Go through all blocks and set their successors.
  for (auto const& entry : sccfg_) {
    unsigned int id{entry.first};
    SplitCFGBlock* block{entry.second};
    clang::CFGBlock* cfg_block{block->getCFGBlock()};

    /// Set the successors
    for (auto const& cfg_succ : cfg_block->succs()) {
      if ((cfg_succ) && (!block->has_wait_)) {
        llvm::dbgs() << "Setting successor of " << block->getID() << " to "
                     << cfg_succ->getBlockID() << "\n";
        // block->successors_.push_back(sccfg_[cfg_succ->getBlockID()]);
      }
    }

    /*
    /// Set the predcessors
    for (auto const& cfg_pred: cfg_block->preds()) {
      if ((cfg_pred) && (!block->has_wait_)) {
      llvm::dbgs() << "Setting predecessor to" << block->getID() << " of "
                   << cfg_pred->getBlockID() << "\n";
      block->predecessors_.push_back(sccfg_[cfg_pred->getBlockID()]);
      }
    }
    */
  }
}

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
  // updateSuccessors();

  //
  // llvm::dbgs() << "Postorder iterator\n";
  // for (auto begin_it = llvm::po_begin(&cfg_->getEntry()),
  // end_it = llvm::po_end(&cfg_->getEntry());
  // begin_it != end_it; begin_it++) {
  // llvm::dbgs() << "Derefernce iterator\n";
  // auto block{*begin_it};
  // if (block) {
  // llvm::dbgs() << "block id " << block->getBlockID() << "\n";
  // block->dump();
  // }
  // llvm::dbgs() << "Next block\n";
  // }
  // llvm::dbgs() << "Done postorder\n";
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
    llvm::dbgs() << "Node id " << block.first << "\n";
    block.second->dump();
  }
  /// Dump all the paths found.
  /*
  llvm::dbgs() << "Dump all paths to wait() found in the CFG.\n";
  unsigned int i{0};
  for (auto const& block_vector : paths_found_) {
    llvm::dbgs() << "Path S" << i++ << ": ";
    for (auto const& block : block_vector) {
      llvm::dbgs() << block->getBlockID() << " ";
    }
    if (i == 1) {
      llvm::dbgs() << " (reset path)";
    }
    llvm::dbgs() << "\n";
  }
  */

  /*
  llvm::dbgs() << "Dump all blocks that were split\n";
  for (auto const& sblock : split_blocks_) {
    llvm::dbgs() << "Block ID: " << sblock.first->getBlockID() << "\n";
    sblock.first->dump();
    const SplitCFGBlock* block_with_wait{&sblock.second};
    llvm::dbgs() << "Print all info for split block\n";
    block_with_wait->dump();
  }
  */
}

SplitCFG::SplitCFG(clang::ASTContext& context) : context_{context} {}

SplitCFG::SplitCFG(clang::ASTContext& context,
                   const clang::CXXMethodDecl* method)
    : context_{context} {
  cfg_ = clang::CFG::buildCFG(method, method->getBody(), &context_,
                              clang::CFG::BuildOptions());
}
