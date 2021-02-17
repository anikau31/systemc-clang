#include "ProcessDecl.h"
#include "EntryFunctionContainer.h"
#include "clang/AST/DeclCXX.h"

using namespace systemc_clang;

ProcessDecl::ProcessDecl(std::string process_type, std::string entry_name,
                         clang::CXXMethodDecl *entry_method_decl,
                         EntryFunctionContainer *entry_fn)
    : process_type_(process_type),
      entry_name_(entry_name),
      entry_method_decl_(entry_method_decl),
      entry_function_ptr_(entry_fn) {}

ProcessDecl::~ProcessDecl() {
  // The following points do NOT need to be deleted:entry_method_decl_,
  // _constructorStmt. This is because they are pointers to the clang AST, which
  // are going to be freed by clang itself.

  entry_method_decl_ = nullptr;
  entry_function_ptr_ = nullptr;
}

ProcessDecl::ProcessDecl(const ProcessDecl &from) {
  process_type_ = from.process_type_;
  entry_name_ = from.entry_name_;
  entry_method_decl_ = from.entry_method_decl_;
  entry_function_ptr_ = from.entry_function_ptr_;
}

std::string ProcessDecl::getType() const { return process_type_; }

std::string ProcessDecl::getName() const { return entry_name_; }

const clang::CXXMethodDecl *ProcessDecl::getEntryMethodDecl() const {
  return entry_method_decl_;
}

EntryFunctionContainer *ProcessDecl::getEntryFunction() {
  return entry_function_ptr_;
}

void ProcessDecl::dump() {
  LLVM_DEBUG(llvm::dbgs() << "ProcessDecl " << this << " '" << entry_name_
                          << "' " << entry_method_decl_ << " "
                          << process_type_;);

  LLVM_DEBUG(llvm::dbgs() << "\nEntry function:\n";);
  LLVM_DEBUG(entry_function_ptr_->dump(llvm::outs(), 1));
}

std::string ProcessDecl::asString() const {
  std::string str{};

  str += "entry_name: " + getName() + "\n";
  str += "process_type: " + getType() + "\n";
  str += "entry_method_declaration: " + to_string(getEntryMethodDecl());

  if (entry_function_ptr_ != nullptr) {
    auto sense_map{entry_function_ptr_->getSenseMap()};
    str += "number_of_sensitivity_signals: " + std::to_string(sense_map.size()) + "\n";

    for (auto const &sense : sense_map) {
      str += "sensitivity_list[" + sense.first + "]" + std::to_string(sense.second.size()) + "\n";
    }
  }
  return str;
}


