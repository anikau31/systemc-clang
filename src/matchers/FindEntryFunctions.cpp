#include "FindEntryFunctions.h"

#include "matchers/ResetMatcher.h"

using namespace systemc_clang;
using namespace sc_ast_matchers;

FindEntryFunctions::FindEntryFunctions(const clang::CXXRecordDecl *d,
                                       llvm::raw_ostream &os,
                                       clang::ASTContext &ctx)
    : context_{ctx},
      os_{os},
      _d{d},
      is_entry_function_{false},
      proc_type_{PROCESS_TYPE::NONE},
      entry_cxx_record_decl_{nullptr},
      entry_method_decl_{nullptr},
      found_entry_decl_{false},
      constructor_stmt_{nullptr},
      pass_{1},
      ctor_decl_{nullptr},
      process_me_{nullptr} {
  /// Pass_ 1:
  /// Find the constructor definition, and the Stmt* that has the code for it.
  /// Set the constructor_stmt_ pointer.
  //  os_ << "\n>>>> PASS 1\n";

  TraverseDecl(const_cast<clang::CXXRecordDecl *>(_d));
  //  os_ << "\n EntryFunctions found: " << entry_function_list_.size() << "\n";
  pass_ = 2;

  /// Reset logic.

  //    os_ << "\n>>>> PASS 2\n";
  /// Pass 2:
  /// Get the entry function name from constructor
  TraverseStmt(constructor_stmt_);
  pass_ = 3;

  /// Pass 3:
  /// Find the CXXMethodDecl* to the entry function
  TraverseDecl(const_cast<clang::CXXRecordDecl *>(_d));
  pass_ = 4;
}

FindEntryFunctions::~FindEntryFunctions() {
  entry_cxx_record_decl_ = nullptr;
  entry_method_decl_ = nullptr;
  constructor_stmt_ = nullptr;
}

bool FindEntryFunctions::shouldVisitTemplateInstantiations() const {
  return true;
}

bool FindEntryFunctions::VisitMemberExpr(MemberExpr *e) {
  // os_ << "\tVisitMemberExpr";
  switch (pass_) {
    case 2: {
      std::string memberName = e->getMemberDecl()->getNameAsString();

      // os_ <<"\n member name : " <<memberName;

      // os_ << "####: MemberExpr -- " << memberName << "\n";
      if (memberName == "create_method_process") {
        proc_type_ = PROCESS_TYPE::METHOD;
        process_me_ = e;
      } else if (memberName == "create_thread_process") {
        proc_type_ = PROCESS_TYPE::THREAD;
        process_me_ = e;
      } else if (memberName == "create_cthread_process") {
        proc_type_ = PROCESS_TYPE::CTHREAD;
        process_me_ = e;
      }
      break;
    }
    default:
      break;
  }
  return true;
}

bool FindEntryFunctions::VisitStringLiteral(StringLiteral *s) {
  // os_ << "\t\tVisitStringLiteral\n";
  switch (pass_) {
    case 2: {
      // os_ << "\nVisitStringLiteral\n";
      entry_name_ = s->getString().str();

      /// FIXME: Where do we erase these?
      /// Create the object to handle multiple entry functions.

      LLVM_DEBUG(llvm::dbgs() << "********** 5. RESET Matcher ***********\n";);
      assert(constructor_stmt_ != nullptr);

      if (ctor_decl_ && constructor_stmt_ && process_me_) {
        ctor_decl_->dump();
        constructor_stmt_->dump();
        ResetMatcher reset_matcher{};
        MatchFinder resetMatchRegistry{};
        reset_matcher.registerMatchers(resetMatchRegistry, process_me_);
        resetMatchRegistry.match(*constructor_stmt_, context_);
        reset_matcher.dump();

        ef = new EntryFunctionContainer();
        // ef->setConstructorStmt (constructor_stmt_);
        ef->setName(entry_name_);
        ef->setProcessType(proc_type_);
        ef->addResetSignal(reset_matcher.getResetSignal());
        ef->addResetEdge(reset_matcher.getResetEdge());
        ef->addResetType(reset_matcher.getResetType());
      }

      if (proc_type_ != PROCESS_TYPE::NONE) {
        entry_function_list_.push_back(ef);
      }
      /*    ef->constructor_stmt_ = constructor_stmt_;
            ef->entry_name_ = entry_name_;
            ef->proc_type_ = proc_type_;
      */
      break;
    }
    case 3: {
      break;
    }
    default:
      break;
  }
  return true;
}

bool FindEntryFunctions::VisitCXXMethodDecl(CXXMethodDecl *md) {
  // os_ << "\nVisitCXXMethodDecl: " << pass_ << ", " << md->getNameAsString()
  // << "\n";
  other_function_list_.push_back(md);
  switch (pass_) {
    case 1: {
      //    os_ << "\n\nPass_ 1 of VisitCXXMethodDecl\n\n";
      /// Check if method is a constructor
      if (CXXConstructorDecl * cd{dyn_cast<CXXConstructorDecl>(md)}) {
        ctor_decl_ = cd;
        const FunctionDecl *fd{nullptr};
        cd->getBody(fd);

        if (cd->hasBody()) {
          constructor_stmt_ = cd->getBody();
        }
      }
      break;
    }
    case 2: {
      //    os_ << "\n\nPass_ 2 of VisitCXXMethodDecl\n\n";
      break;
    }
    case 3: {
      /// Check if name is the same as _entryFunction.
      // os_ <<"\n md name : " <<md->getNameAsString();
      for (size_t i = 0; i < entry_function_list_.size(); ++i) {
        if (md->getNameAsString() == entry_function_list_.at(i)->getName()) {
          EntryFunctionContainer *tmp{entry_function_list_.at(i)};
          entry_method_decl_ = md;
          if (entry_method_decl_ != nullptr) {
            tmp->setEntryMethod(entry_method_decl_);
            // ef->entry_method_decl_ = entry_method_decl_;
          }
        }
      }
      //    os_ << "\n case 3: " << md->getNameAsString() << ", " << entry_name_
      //    << "\n";
      break;
    }
  }
  return true;
}
//
CXXRecordDecl *FindEntryFunctions::getEntryCXXRecordDecl() {
  assert(entry_cxx_record_decl_ != nullptr);
  return entry_cxx_record_decl_;
}

CXXMethodDecl *FindEntryFunctions::getEntryMethodDecl() {
  assert(entry_method_decl_ != nullptr);
  return entry_method_decl_;
}

void FindEntryFunctions::dump() {
  os_ << "\n ============== FindEntryFunction ===============\n";
  os_ << "\n:> Print Entry Function informtion for : " << _d->getNameAsString()
      << "\n";
  for (size_t i = 0; i < entry_function_list_.size(); ++i) {
    EntryFunctionContainer *ef = entry_function_list_[i];

    os_ << "\n:> Entry function name: " << ef->getName() << ", process type: ";
    switch (ef->getProcessType()) {
      case PROCESS_TYPE::THREAD:
        os_ << " SC_THREAD\n";
        break;
      case PROCESS_TYPE::METHOD:
        os_ << " SC_METHOD\n";
        break;
      case PROCESS_TYPE::CTHREAD:
        os_ << " SC_CTHREAD\n";
        break;
      default:
        os_ << " NONE\n";
        break;
    }
    os_ << ":> CXXMethodDecl addr: " << ef->getEntryMethod() << "\n";
    // ef->entry_method_decl_->dump();
  }
  os_ << "\n ============== END FindEntryFunction ===============\n";
  os_ << "\n";
}

FindEntryFunctions::entryFunctionVectorType *
FindEntryFunctions::getEntryFunctions() {
  return new entryFunctionVectorType(entry_function_list_);
}

vector<CXXMethodDecl *> FindEntryFunctions::getOtherFunctions() {
  return other_function_list_;
}

string FindEntryFunctions::getEntryName() { return entry_name_; }
