#ifndef _FIND_ENTRY_FUNCTIONS_H_
#define _FIND_ENTRY_FUNCTIONS_H_

#include "EntryFunctionContainer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <map>
#include <string>
#include <vector>

namespace systemc_clang {

using namespace clang;

class FindEntryFunctions : public RecursiveASTVisitor<FindEntryFunctions> {
public:
  /// Typedefs
  typedef std::vector<EntryFunctionContainer *> entryFunctionVectorType;

  typedef std::pair<string, vector<string>> entryFunctionLHSPairType;
  typedef std::map<string, vector<string>> entryFunctionLHSMapType;

  FindEntryFunctions(const clang::CXXRecordDecl *d, llvm::raw_ostream &os, clang::ASTContext &ctx);
  virtual ~FindEntryFunctions();

  /// Virtual methods from RecursiveASTVisitor
  virtual bool VisitStringLiteral(StringLiteral *l);
  virtual bool VisitCXXMethodDecl(CXXMethodDecl *d);
  virtual bool VisitMemberExpr(MemberExpr *e);
  bool shouldVisitTemplateInstantiations() const;

  // Access Methods
  CXXRecordDecl *getEntryCXXRecordDecl();
  CXXMethodDecl *getEntryMethodDecl();
  string getEntryName();
  entryFunctionVectorType *getEntryFunctions();
  vector<CXXMethodDecl *> getOtherFunctions();

  void dump();

private:
  clang::ASTContext &context_;
  llvm::raw_ostream &os_;
  const CXXRecordDecl *_d;
  bool is_entry_function_;
  PROCESS_TYPE proc_type_;
  string entry_name_;
  CXXRecordDecl *entry_cxx_record_decl_;
  CXXMethodDecl *entry_method_decl_;
  bool found_entry_decl_;
  Stmt *constructor_stmt_;
  EntryFunctionContainer *ef;
  int pass_;
  clang::CXXConstructorDecl* ctor_decl_;
  entryFunctionVectorType entry_function_list_;
  entryFunctionLHSMapType entry_function_map_;
  vector<CXXMethodDecl *> other_function_list_;
  clang::MemberExpr* process_me_;

  // Disallow constructor with no argument
  FindEntryFunctions(llvm::raw_ostream &os);
};
} // namespace systemc_clang
#endif
