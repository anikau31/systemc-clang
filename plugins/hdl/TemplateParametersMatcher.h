#ifndef _TEMPLATE_PARAMETERS_MATCHERS_H_
#define _TEMPLATE_PARAMETERS_MATCHERS_H_

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"
#include "llvm/Support/Debug.h"

/// Different matchers may use different DEBUG_TYPE
#undef DEBUG_TYPE
#define DEBUG_TYPE "HDL"

using namespace clang::ast_matchers;
using namespace systemc_clang;

namespace sc_ast_matchers {
AST_MATCHER(Type, isDependentNameType) { return (Node.isDependentType()); }
};  // namespace sc_ast_matchers

using namespace sc_ast_matchers;

class TemplateParametersMatcher : public MatchFinder::MatchCallback {
 private:
  std::vector<const FieldDecl *> found_fields_args;
  std::vector<const FieldDecl *> found_fields_parms;

 public:
  void registerMatchers(MatchFinder &finder) {
    // Overview of the matcher
    // Terminology: template parameter refers to the template typename name, and
    // template arguments refers to the substituted type. For example, template
    // <typename T> X{}; X<int> x; has the parameter as T and the argument as
    // int.
    //
    // 1. Start matching within a ClassTemplateDecl. Note that the match() call
    // has to be updated to ensure that the appropriate ClassTemplateDecl is
    // provided.
    // 2. The isExpansionInMainFile() makes the matcher focus on the provided
    // file (input), and not the underlying C++ files.
    //
    // 3. We check if the ClassTemplateDecl has a CXXRecordDecl within it (note
    // that it will have a CXXRecordDecl and a ClassTemplateSpecializationDecl,
    // but we want the former to extract the template parameters).
    // 4. For every fieldDecl that is a descendant, it is going to check if the
    // type for the FieldDecl's desugared type is a TemplateSpecialization or a
    // TemplateTypeParm.  This should cover typedefs as well. An alternative way
    // would be to just match with Type, and then in the callback one could cast
    // it to respective types that one cares about.
    //
    /* clang-format off */
       
    auto match_template_special_decl = 
      classTemplateSpecializationDecl(
          cxxRecordDecl(forEachDescendant(fieldDecl(
               ).bind("fd_arg")))
          ).bind("special_template_decl");
//
    auto match_template_decl =
        classTemplateDecl(
            has(cxxRecordDecl(forEachDescendant(
                fieldDecl(
                  // anyOf(
                    // hasType(hasUnqualifiedDesugaredType(recordType().bind("record_type")))
                    // ,  hasType(hasUnqualifiedDesugaredType(isDependentNameType()))
                    // , hasType(hasUnqualifiedDesugaredType(typedefType().bind("typedef_type")))
                    // , hasType(hasUnqualifiedDesugaredType(templateSpecializationType().bind("specialization_type")))
                    // , hasType(hasUnqualifiedDesugaredType(templateTypeParmType().bind("parm_type")))
                    // , hasType(hasUnqualifiedDesugaredType(builtinType().bind( "builtin_type")))
                    // ) // anyOf
                  ).bind("fd_parm"))
              )
            )
          ).bind("template_decl");
    /* clang-format on */

    finder.addMatcher(match_template_decl, this);
    finder.addMatcher(match_template_special_decl, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    // auto decl{result.Nodes.getNodeAs<Decl>("decl")};
    // auto template_decl{
    // result.Nodes.getNodeAs<ClassTemplateSpecializationDecl>("template_decl")};

    auto fd_arg{result.Nodes.getNodeAs<FieldDecl>("fd_arg")};
    auto fd_parm{result.Nodes.getNodeAs<FieldDecl>("fd_parm")};

    auto record_type{result.Nodes.getNodeAs<RecordType>("record_type")};
    auto parm_type{result.Nodes.getNodeAs<TemplateTypeParmType>("parm_type")};
    auto template_special{result.Nodes.getNodeAs<TemplateSpecializationType>(
        "specialization_type")};
    LLVM_DEBUG(llvm::dbgs()
               << "=============== TEST Template Parm Matcher ====== \n");

    if (fd_arg) {
      LLVM_DEBUG(llvm::dbgs() << "Found a Arg FieldDecl\n");
      fd_arg->dump(llvm::errs());
      fd_arg->getType().getTypePtr()->dump();
      found_fields_args.push_back(fd_arg);
    }

    if (fd_parm) {
      LLVM_DEBUG(llvm::dbgs() << "Found a Parm FieldDecl\n");
      fd_parm->dump(llvm::errs());
      fd_parm->getType().getTypePtr()->dump();
      found_fields_parms.push_back(fd_parm);
    }

    /*
    if (fd) {
      if (auto cxx = fd->getType().getTypePtr()->getAsCXXRecordDecl()) {
        if (auto template_decl =
                dyn_cast<ClassTemplateSpecializationDecl>(cxx)) {
          (llvm::dbgs() << "#### TemplateSpecializationType\n");
          template_decl->dump();

          (llvm::dbgs() << "##### Try to find the template types\n");
          FindTemplateTypes ftt{};
          ftt.Enumerate(template_decl);
          ftt.printTemplateArguments(llvm::outs());
          (llvm::dbgs() << "##### END\n");

          const TemplateArgumentList &arg_list{
              template_decl->getTemplateArgs()};

          const TemplateArgument &targ{arg_list.get(0)};
          targ.getAsType().dump();

          switch (targ.getKind()) {
            case TemplateArgument::ArgKind::Integral: {
              auto q{targ.getAsIntegral()};
              (llvm::dbgs() << "@@ Integral: " << q << "\n");
            }; break;
            case TemplateArgument::ArgKind::Type: {
              auto q{targ.getAsType()};
              auto name{q.getAsString()};
              (llvm::dbgs() << "@@ arg: " << name << "\n");
            }; break;
            case TemplateArgument::ArgKind::Expression: {
              Expr *expr{targ.getAsExpr()};
              DeclRefExpr *dexpr{dyn_cast<DeclRefExpr>(expr)};
              if (dexpr) {
                (llvm::dbgs() << "Template parameter: "
                              << dexpr->getNameInfo().getAsString() << "\n");
              }
            }
            default: {
            }
          };
        }

        // Since this is a RecordType, we can reuse our template type parsing.
        // if (record_type && fd) {
        // (llvm::dbgs() << "#### RecordType\n");
        // record_type->dump();
        // FindTemplateTypes ftt{};
        // ftt.Enumerate(record_type);
        // ftt.printTemplateArguments(llvm::errs());
        // }
        (llvm::dbgs() << "\n");
      }
    }
  */
  }

  void dump() {}
  void getFields(std::vector<const FieldDecl *> &flds) { flds = found_fields_parms; }
  void getArgFields(std::vector<const FieldDecl *> &flds) {
    flds = found_fields_args;
  }
  void getParmFields(std::vector<const FieldDecl *> &flds) {
    flds = found_fields_parms;
  }
};

#endif
