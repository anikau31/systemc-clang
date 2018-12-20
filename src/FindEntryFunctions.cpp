#include "FindEntryFunctions.h"

using namespace scpar;

FindEntryFunctions::FindEntryFunctions( CXXRecordDecl * d, llvm::raw_ostream & os ) :
  os_{os},
  _d{d},
  is_entry_function_{false},
  proc_type_{NONE},
  entry_cxx_record_decl_{nullptr},
  entry_method_decl_{nullptr},
  found_entry_decl_{false},
  constructor_stmt_{nullptr},
  pass_{1} {

    /// Pass_ 1:
    /// Find the constructor definition, and the Stmt* that has the code for it.
    /// Set the constructor_stmt_ pointer.
    //  os_ << "\n>>>> PASS 1\n";

    TraverseDecl( _d );
    //  os_ << "\n EntryFunctions found: " << entry_function_list_.size() << "\n";
    pass_ = 2;

    //    os_ << "\n>>>> PASS 2\n";
    /// Pass 2:
    /// Get the entry function name from constructor
    TraverseStmt( constructor_stmt_ );
    pass_ = 3;

    /// Pass 3:
    /// Find the CXXMethodDecl* to the entry function
    TraverseDecl( _d );
    pass_ = 4;

  }

FindEntryFunctions::~FindEntryFunctions() {
  entry_cxx_record_decl_ = nullptr;
  entry_method_decl_ = nullptr;
  constructor_stmt_ = nullptr;
}

bool FindEntryFunctions::VisitMemberExpr (MemberExpr * e) {
	//os_ << "\tVisitMemberExpr";
	switch( pass_ ) {
  case 2:	{
    string memberName = e->getMemberDecl ()->getNameAsString ();

    // os_ <<"\n member name : " <<memberName;

    //os_ << "####: MemberExpr -- " << memberName << "\n";
    if ( memberName == "create_method_process" ) {
      proc_type_ = METHOD;
    }
    else if ( memberName == "create_thread_process" )	{
      proc_type_ = THREAD;
    }
    else if ( memberName == "create_cthread_process" ) {
      proc_type_ = CTHREAD;
    }
    break;
  }
  default:
    break;
  }
	return true;
}

bool FindEntryFunctions::VisitStringLiteral( StringLiteral * s ) {
	//os_ << "\t\tVisitStringLiteral\n";
	switch( pass_ )	{
  case 2: {
    //os_ << "\nVisitStringLiteral\n";
    entry_name_ = s->getString();

    /// FIXME: Where do we erase these?
    /// Create the object to handle multiple entry functions.

    //  EntryFunctionType* ef = new EntryFunctionType();

    ef = new EntryFunctionContainer();
    //ef->setConstructorStmt (constructor_stmt_);
    ef->setName( entry_name_ );
    ef->setProcessType( proc_type_ );

    if ( proc_type_ != 0 ) {
      entry_function_list_.push_back( ef );
    }
    /*    ef->constructor_stmt_ = constructor_stmt_;
          ef->entry_name_ = entry_name_;
          ef->proc_type_ = proc_type_;
    */
    break;
  }
  case 3:	{
    break;
  }
  default:
    break;
  }
	return true;
}

bool FindEntryFunctions::VisitCXXMethodDecl( CXXMethodDecl * md ) {
	// os_ << "\nVisitCXXMethodDecl: " << pass_ << ", " << md->getNameAsString() << "\n";
	other_function_list_.push_back( md );
	switch( pass_ )	{
  case 1:	{
    //    os_ << "\n\nPass_ 1 of VisitCXXMethodDecl\n\n";
    /// Check if method is a constructor
    if (CXXConstructorDecl *cd {dyn_cast< CXXConstructorDecl >(md)} )	{
      const FunctionDecl * fd{nullptr};
      cd->getBody(fd);

      if ( cd->hasBody() )	{
        constructor_stmt_ = cd->getBody();
      }
    }
    break;
  }
  case 2:	{
    //    os_ << "\n\nPass_ 2 of VisitCXXMethodDecl\n\n";
    break;
  }
  case 3:	{
    /// Check if name is the same as _entryFunction.
    //os_ <<"\n md name : " <<md->getNameAsString();
    for ( size_t i = 0; i < entry_function_list_.size(); ++i ) {
      if ( md->getNameAsString() == entry_function_list_.at(i)->getName() )	{
        EntryFunctionContainer *tmp{ entry_function_list_.at(i) };
        entry_method_decl_ = md;
        if ( entry_method_decl_ != nullptr )	{
          tmp->setEntryMethod( entry_method_decl_ );
          //ef->entry_method_decl_ = entry_method_decl_;
        }
      }
    }
    //    os_ << "\n case 3: " << md->getNameAsString() << ", " << entry_name_ << "\n";
    break;
  }
  }
	return true;
}
// 
CXXRecordDecl *FindEntryFunctions::getEntryCXXRecordDecl() {
	assert( entry_cxx_record_decl_ != nullptr );
	return entry_cxx_record_decl_;
}

CXXMethodDecl *FindEntryFunctions::getEntryMethodDecl() {
	assert ( entry_method_decl_ != nullptr );
	return entry_method_decl_;
}

void FindEntryFunctions::dump() {
	os_ << "\n ============== FindEntryFunction ===============\n";
	os_ << "\n:> Print Entry Function informtion for : " <<
		_d->getNameAsString() << "\n";
	for ( size_t i = 0; i < entry_function_list_.size(); ++i ) {
			EntryFunctionContainer * ef = entry_function_list_[i];

			os_ << "\n:> Entry function name: " << ef->getName() <<
				", process type: ";
			switch ( ef->getProcessType() ) {
				case THREAD:
					os_ << " SC_THREAD\n";
					break;
				case METHOD:
					os_ << " SC_METHOD\n";
					break;
				case CTHREAD:
					os_ << " SC_CTHREAD\n";
					break;
				default:
					os_ << " NONE\n";
					break;
				}
			os_ << ":> CXXMethodDecl addr: " << ef->getEntryMethod() << "\n";
			//ef->entry_method_decl_->dump();
		}
	os_ << "\n ============== END FindEntryFunction ===============\n";
	os_ << "\n";
}

FindEntryFunctions::entryFunctionVectorType * FindEntryFunctions::getEntryFunctions() {
	return new entryFunctionVectorType( entry_function_list_ );
}

vector < CXXMethodDecl * > FindEntryFunctions::getOtherFunctions() {
	return other_function_list_;
}

string FindEntryFunctions::getEntryName() {
	return entry_name_;
}
