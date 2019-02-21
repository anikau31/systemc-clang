#include "ProcessDecl.h"

using namespace scpar;
//using namespace nlohmann::json;

ProcessDecl::ProcessDecl(string t, string n, CXXMethodDecl *entryMethodDecl,
                         EntryFunctionContainer *e) :
  process_type_(t), entry_name_(n), entry_method_decl_(entryMethodDecl), entry_function_ptr_(e) {}

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

string ProcessDecl::getType() const { return process_type_; }

string ProcessDecl::getName() const { return entry_name_; }

CXXMethodDecl *ProcessDecl::getEntryMethodDecl() const { return entry_method_decl_; }

void ProcessDecl::dump(raw_ostream &os) {

  os << "ProcessDecl " << this << " '" << entry_name_ << "' " << entry_method_decl_
     << " " << process_type_;
  // TODO: Remove this tabn

  os << "\nEntry function:\n";
    entry_function_ptr_->dump(os, 1);

    //dump_json();
}

json ProcessDecl::dump_json(raw_ostream & os ) const {

  // These are the three fields that we need to extract from entry_function_ptr.
  json process_j;
  process_j["entry_name"] = getName();
  process_j["procesS_type"] = getType();
  process_j["entry_method_declaration"] = to_string( getEntryMethodDecl() );

  //  os << process_j.dump(4);
  return process_j;
}
