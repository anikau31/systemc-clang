#ifndef _TEMPLATE_PARAMETERS_MATCHERS_H_
#define _TEMPLATE_PARAMETERS_MATCHERS_H_

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"

using namespace clang::ast_matchers;
using namespace scpar;
using namespace sc_ast_matchers;

class TemplateParametersMatcher : public MatchFinder::MatchCallback {
 private:
  // std::vector<std::tuple<FieldDecl*,

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
       
    auto match_template_decl =
        classTemplateDecl(
            has(cxxRecordDecl(forEachDescendant(
                fieldDecl(
                  anyOf(
                    hasType(hasUnqualifiedDesugaredType(recordType().bind("record_type")))
                    , hasType(hasUnqualifiedDesugaredType(templateSpecializationType().bind("specialization_type")))
                    , hasType(hasUnqualifiedDesugaredType(templateTypeParmType().bind("parm_type")))
                    )
                  ).bind("fd"))
              )
            )
          ).bind("template_decl");

    /* clang-format on */

    finder.addMatcher(match_template_decl, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    // auto decl{result.Nodes.getNodeAs<Decl>("decl")};
    auto template_decl{
        result.Nodes.getNodeAs<ClassTemplateDecl>("template_decl")};

    auto fd{result.Nodes.getNodeAs<FieldDecl>("fd")};

    auto record_type{result.Nodes.getNodeAs<RecordType>("record_type")};
    auto parm_type{result.Nodes.getNodeAs<TemplateTypeParmType>("parm_type")};
    auto template_special{result.Nodes.getNodeAs<TemplateSpecializationType>(
        "specialization_type")};
    llvm::outs() << "=============== TEST Template Parm Matcher ====== \n";

    if (fd) {
      llvm::outs() << "Found a FieldDecl\n";
      fd->dump();
    }

    if (template_special && fd) {
      llvm::outs() << "#### TemplateSpecializationType\n";
      template_special->dump();

      llvm::outs() << "##### Try to find the template types\n";
      FindTemplateTypes ftt{};
      ftt.Enumerate(template_special);
      ftt.printTemplateArguments(llvm::outs());
      llvm::outs() << "##### END\n";

      const TemplateArgument &targ{template_special->getArg(0)};

      switch (targ.getKind()) {
        case TemplateArgument::ArgKind::Integral: {
          auto q{targ.getAsIntegral()};
          llvm::outs() << "@@ Integral: " << q << "\n";
        }; break;
        case TemplateArgument::ArgKind::Type: {
          auto q{targ.getAsType()};
          auto name{q.getAsString()};
          llvm::outs() << "@@ arg: " << name << "\n";
        }; break;
        case TemplateArgument::ArgKind::Expression: {
          Expr *expr{targ.getAsExpr()};
          DeclRefExpr *dexpr{dyn_cast<DeclRefExpr>(expr)};
          if (dexpr) {
            llvm::outs() << "Template parameter: "
                         << dexpr->getNameInfo().getAsString() << "\n";
          }
        }
        default: {
        }
      };
    }

    // Since this is a RecordType, we can reuse our template type parsing.
    if (record_type && fd) {
      llvm::outs() << "#### RecordType\n";
      record_type->dump();
      FindTemplateTypes ftt{};
      ftt.Enumerate(record_type);
      ftt.printTemplateArguments(llvm::outs());
    }
    llvm::outs() << "\n";
  }

  void dump() {}
};

#endif
