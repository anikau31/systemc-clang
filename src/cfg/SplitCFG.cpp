#include "SplitCFG.h"

#include "llvm/Support/Debug.h"
#include "llvm/ADT/PostOrderIterator.h"

using namespace systemc_clang;

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


    //////////////////////////////////////////
    //
    if (sp.getSplitBlockSize() != 0) {
      /// The block has been split.
      split_blocks_.insert(
          std::pair<unsigned int, SplitCFGBlock>(block->getBlockID(), sp));
    }
  }
}

void SplitCFG::build_sccfg(const clang::CXXMethodDecl* method) {
  cfg_ = clang::CFG::buildCFG(method, method->getBody(), &context_,
                              clang::CFG::BuildOptions());

  llvm::dbgs() << "Build SCCFG\n";

  clang::LangOptions lang_opts;
  cfg_->dump(lang_opts, true);

  for (auto begin_it = cfg_->nodes_begin(); begin_it != cfg_->nodes_end();
       ++begin_it) {
    auto block{*begin_it};
    llvm::dbgs() << "=> " << block->getBlockID() << "\n";
    block->dump();
  }

  /*
  llvm::outs() << "Basic blocks in post-order:\n";
  for (llvm::po_iterator<clang::CFGBlock*>
           I = llvm::po_begin(&cfg_->getEntry()),
           IE = llvm::po_end(&cfg_->getEntry());
       I != IE; ++I) {
    llvm::outs() << "  " << (*I)->getBlockID() << "\n";
  }
  */
}

void SplitCFG::dump() const {
  llvm::dbgs() << "Dump all blocks that were split\n";
  for (auto const& sblock : split_blocks_) {
    llvm::dbgs() << "Block ID: " << sblock.first << "\n";
    const SplitCFGBlock* block_with_wait{&sblock.second};
    llvm::dbgs() << "Print all info for split block\n";
    block_with_wait->dump();
  }
}
