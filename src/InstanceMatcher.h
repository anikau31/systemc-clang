#ifndef _INSTANCE_MATCHER_H_
#define _INSTANCE_MATCHER_H_

#include <vector>

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"

using namespace clang::ast_matchers;
using namespace scpar;



namespace sc_ast_matchers {


///////////////////////////////////////////////////////////////////////////////
//
// Class InstanceMatcher
//
//
///////////////////////////////////////////////////////////////////////////////
class InstanceMatcher : public MatchFinder::MatchCallback {
 public:
  typedef std::tuple<std::string, Decl *> InstanceDeclType;
  typedef std::vector<InstanceDeclType> InstanceDeclarationsType;

 private:
  // Instances can come in two forms:
  // FieldDecl: this is when they are members of a class.
  // VarDecl  : this is when they are simply variables such as in functions or
  // in the main().
  //
  // The way to identify them both together is to look at its base class Decl.
  // Then use dyn_cast<> to detect whether it is one of the two above mentioned
  // types.
  InstanceDeclarationsType instances_;

 public:
  // Finds the instance with the same type as the argument.
  // Pass by reference to the instance.
  bool findInstance(CXXRecordDecl *decl,
                    std::vector<InstanceDeclType> &found_instances) {
    // First check in the instance_fields.
    // Check to see if the pointer to the type is the same as the sc_module
    // type.

    llvm::outs() << "[findInstance] instance size: " << instances_.size()
                 << "\n";

    llvm::outs() << "[findInstance] must find decl name: " << decl->getName()
                 << "\n";
    // Walk through all the instances.
    for (auto const &element : instances_) {
      auto p_field_var_decl{get<1>(element)};

      llvm::outs() << "=> inst name: " << get<0>(element) << "\n";
      // Check if it is a FieldDecl

      // TODO factor out this code to be handled for both.
      if (auto *p_field{dyn_cast<FieldDecl>(p_field_var_decl)}) {
        auto qtype{p_field->getType().getTypePtr()};
        // qtype->dump();
        if (qtype->isRecordType()) {
          auto rt{qtype->getAsCXXRecordDecl()};
          std::string dbg{"[InstanceMatcher] FieldDecl"};
          llvm::outs() << dbg << " decl: " << decl->getName()
                       << ", inst name: " << rt->getName() << "\n";
          if (rt == decl) {
            llvm::outs() << "==> Insert fieldDecl into found instance\n";
            found_instances.push_back(
                InstanceDeclType(get<0>(element), get<1>(element)));
          }
        }
      } else {
        // This is a VarDecl instance.
        auto p_var{dyn_cast<VarDecl>(p_field_var_decl)};
        auto qtype{p_var->getType().getTypePtr()};

        std::string dbg{"[InstanceMatcher] VarDecl"};
        // p_var->dump();
        if (qtype->isRecordType()) {
          auto rt{qtype->getAsCXXRecordDecl()};
          llvm::outs() << dbg << " decl: " << decl->getName()
                       << ", inst name: " << rt->getName() << "\n";
          if (rt == decl) {
            llvm::outs() << "==> Insert vardecl into found instance\n";
            found_instances.push_back(InstanceDeclType(get<0>(element), rt));
          }
        }
      }
    }
    llvm::outs() << "=> found_instances: " << found_instances.size() << "\n";

    return (found_instances.size() != 0);
  }

  void registerMatchers(MatchFinder &finder) {
    /* clang-format off */

    // We will have two matchers.
    //
    // Match when the following conditions are satisifed:
    // * It is a FieldDecl
    // * It has a type that is a C++ class that is derived from sc_module
    //
    auto match_instances = 
      fieldDecl(
        hasType(
          cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module"))))

        /*
            hasDescendant(
              cxxConstructExpr(hasArgument(0,
              stringLiteral().bind("constructor_arg"))).bind("constructor_expr")
              )
              */
        ).bind("instances_in_fielddecl");

    // Match when the following conditions are satisifed:
    // * It is a VarDecl
    // * It has a type that is a C++ class that is derived from sc_module
    //   - It has a descendant that has a constructor that has as its first argument
    //     a name provided for it.
    //     (Every sc_module instantiated must have a string literal.
    auto match_instances_vars =
        varDecl(
            hasType(
                cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module")))),
            hasDescendant(
              cxxConstructExpr(hasArgument(0,
              stringLiteral().bind("constructor_arg"))).bind("constructor_expr")
              )
            )
            .bind("instances_in_vardecl");

    /* clang-format on */

    // Add the two matchers.
    finder.addMatcher(match_instances, this);
    finder.addMatcher(match_instances_vars, this);
  }

  // This is the callback function whenever there is a match.
  virtual void run(const MatchFinder::MatchResult &result) {
    if (auto instance = const_cast<FieldDecl *>(
            result.Nodes.getNodeAs<FieldDecl>("instances_in_fielddecl"))) {
      std::string name{instance->getIdentifier()->getNameStart()};
      llvm::outs() << "InstanceMatcher\n";
      instance->dump();
      //      instances_.push_back(std::make_tuple(name, instance));
      //
      if (auto instance_name = const_cast<CXXConstructExpr *>(
              result.Nodes.getNodeAs<CXXConstructExpr>("constructor_expr"))) {
        llvm::outs() << "Found constructor expression argument: "
                     << instance_name->getNumArgs() << "\n";
        auto first_arg{instance_name->getArg(0)};

        // Code to get the instance name
        clang::LangOptions LangOpts;
        LangOpts.CPlusPlus = true;
        clang::PrintingPolicy Policy(LangOpts);

        std::string name_string;
        llvm::raw_string_ostream sstream(name_string);
        first_arg->printPretty(sstream, 0, Policy);
        // The instance name comes with " and we should remove them.
        std::string strip_quote_name{sstream.str()};
        strip_quote_name.erase(
            std::remove(strip_quote_name.begin(), strip_quote_name.end(), '\"'),
            strip_quote_name.end());

        // This is the instance name.
        instances_.push_back(std::make_tuple(strip_quote_name, instance));
      }
    }

    if (auto instance = const_cast<VarDecl *>(
            result.Nodes.getNodeAs<VarDecl>("instances_in_vardecl"))) {
      std::string name{instance->getIdentifier()->getNameStart()};
      llvm::outs() << "@@ Found a member variable instance: " << name << "\n";

      // TODO: Is this how we want the instance name?
      // This is actually a good way to identify instances.
      // This is because these names will truly be unique. Variable names could
      // be arrays, which may not be the best way to identify unique sc_module
      // instances.

      if (auto instance_name = const_cast<CXXConstructExpr *>(
              result.Nodes.getNodeAs<CXXConstructExpr>("constructor_expr"))) {
        llvm::outs() << "Found constructor expression argument: "
                     << instance_name->getNumArgs() << "\n";
        auto first_arg{instance_name->getArg(0)};

        // Code to get the instance name
        clang::LangOptions LangOpts;
        LangOpts.CPlusPlus = true;
        clang::PrintingPolicy Policy(LangOpts);

        std::string name_string;
        llvm::raw_string_ostream sstream(name_string);
        first_arg->printPretty(sstream, 0, Policy);
        // The instance name comes with " and we should remove them.
        std::string strip_quote_name{sstream.str()};
        strip_quote_name.erase(
            std::remove(strip_quote_name.begin(), strip_quote_name.end(), '\"'),
            strip_quote_name.end());

        // This is the instance name.
        instances_.push_back(std::make_tuple(strip_quote_name, instance));
      }
    }
  }

  void dump() {
    // Instances holds both FieldDecl and VarDecl as its base class Decl.
    for (const auto &i : instances_) {
      llvm::outs() << "[InstanceMatcher] module instance declaration name: "
                   << get<0>(i) << ", " << get<1>(i) << "\n";

      auto p_field_var_decl{get<1>(i)};
      if (isa<FieldDecl>(p_field_var_decl)) {
        llvm::outs() << "[DBG] FieldDecl\n";
        p_field_var_decl->dump();
      } else {
        llvm::outs() << "[DBG] VarDecl\n";
      }
    }
  }
};
};
#endif 

