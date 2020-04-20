#ifndef _INSTANCE_MATCHER_H_
#define _INSTANCE_MATCHER_H_

#include <vector>

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"

using namespace clang::ast_matchers;

namespace sc_ast_matchers {

struct ModuleInstanceType {
  std::string var_name;
  std::string var_type_name;
  std::string instance_name;
  bool is_field_decl;
  clang::Decl *decl;

  ModuleInstanceType()
      : var_name{},
        var_type_name{},
        instance_name{},
        is_field_decl{false},
        decl{nullptr} {}

  ModuleInstanceType(const ModuleInstanceType &rhs) {
    var_name = rhs.var_name;
    var_type_name = rhs.var_type_name;
    instance_name = rhs.instance_name;
    is_field_decl = rhs.is_field_decl;
    decl = rhs.decl;
  }

  bool operator==(const ModuleInstanceType &rhs) {
    return std::tie(var_name, var_type_name, instance_name, is_field_decl,
                    decl) == std::tie(rhs.var_name, rhs.var_type_name,
                                      rhs.instance_name, rhs.is_field_decl,
                                      rhs.decl);
  }

  void dump() {
    llvm::outs() << decl << " " << var_type_name << " " << var_name << " "
                 << instance_name << " " << is_field_decl << "\n";
  }
};

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

  // Store all instances in a map.
  // The map key should be the Decl*.  This will work for both FieldDecl
  // (instances within sub-modules) and VarDecl separate modules in sc_main().
  //

  typedef std::pair<Decl *, ModuleInstanceType> ModuleInstanceTuple;
  typedef std::map<Decl *, ModuleInstanceType> InstanceDeclarations;

 private:
  // Instances can come in two forms:
  // FieldDecl: this is when they are members of a class.
  // VarDecl  : this is when they are simply variables such as in functions or
  // in the main().
  //
  // The way to identify them both together is to look at its base class Decl.
  // Then use dyn_cast<> to detect whether it is one of the two above mentioned
  // types.
  //
  // deprecated
  InstanceDeclarationsType instances_;

  InstanceDeclarations instance_map_;

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
      auto p_field_var_decl{std::get<1>(element)};

      llvm::outs() << "=> inst name: " << std::get<0>(element) << "\n";
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

  auto match_ctor_arg(const std::string &bind_arg_name,
                      const std::string &bind_ctor_expr) {
    return cxxConstructExpr(hasArgument(0, stringLiteral().bind(bind_arg_name)))
        .bind(bind_ctor_expr);
  }

  void registerMatchers(MatchFinder &finder) {
    // We will have two matchers.
    //
    // Match when the following conditions are satisifed:
    // * It is a FieldDecl
    // * It has a type that is a C++ class that is derived from sc_module
    //
    // These are field members within a class declaration.  Hence, we only need
    // to collect their FieldDecl pointers and their variable names.  Their
    // instance names would appear when the constructor of the class that has
    // these field members within it.  This constructor would use the
    // initialization list to provide an argument, which would be the instance
    // name.
    //
    /* clang-format off */
    auto match_instances = 
      fieldDecl(
        hasType(
          cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module"))
            ).bind("cxx_record_decl")
          )
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
                cxxRecordDecl(
                  isDerivedFrom(hasName("::sc_core::sc_module")),
                  hasDescendant(cxxConstructorDecl(
                      forEachDescendant(match_ctor_arg("ctor_arg", "ctor_expr"))
                      ).bind("ctor_decl")
                    )
                  ).bind("cxx_record_decl")
              ),
            hasDescendant(match_ctor_arg("ctor_arg", "constructor_expr")
              )
            ).bind("instances_in_vardecl");

    /* clang-format on */

    // Add the two matchers.
    finder.addMatcher(match_instances, this);
    finder.addMatcher(match_instances_vars, this);
  }

  // This is the callback function whenever there is a match.
  virtual void run(const MatchFinder::MatchResult &result) {
    auto instance_fd = const_cast<FieldDecl *>(
        result.Nodes.getNodeAs<FieldDecl>("instances_in_fielddecl"));
    // auto instance_name_fd = const_cast<CXXConstructExpr *>(
    // result.Nodes.getNodeAs<CXXConstructExpr>("constructor_expr"));

    // VD
    //
    auto instance_vd = const_cast<VarDecl *>(
        result.Nodes.getNodeAs<VarDecl>("instances_in_vardecl"));
    auto instance_name_vd = const_cast<CXXConstructExpr *>(
        result.Nodes.getNodeAs<CXXConstructExpr>("constructor_expr"));
    auto ctor_decl = const_cast<CXXConstructorDecl *>(
        result.Nodes.getNodeAs<CXXConstructorDecl>("ctor_decl"));
    auto ctor_init = const_cast<CXXCtorInitializer *>(
        result.Nodes.getNodeAs<CXXCtorInitializer>("ctor_init"));
    auto ctor_expr = const_cast<CXXConstructExpr *>(
        result.Nodes.getNodeAs<CXXConstructExpr>("ctor_expr"));
    auto ctor_arg =
        const_cast<Stmt *>(result.Nodes.getNodeAs<Stmt>("ctor_arg"));

    // FD
    if (instance_fd) {
      llvm::outs() << "InstanceMatcher\n";
      instance_fd->dump();

      llvm::outs() << "## FieldDecl\n";
      //
      // NAND n1 such that n1("N1") somewhere
      // variable name: n1, var_type_name: NAND, instance_name: N1
      //
      // Get the variable name
      auto var_name{instance_fd->getNameAsString()};
      // We do not get the instance name from within the field declaration.
      // Get the type of the class of the field.
      auto var_type_name{instance_fd->getType().getAsString()};

      llvm::outs() << "=> var_type_name " << var_type_name << " var_name "
                   << var_name << "\n";
      instances_.push_back(std::make_tuple(var_name, instance_fd));

      ModuleInstanceType parsed_instance{};
      parsed_instance.var_name = var_name;
      parsed_instance.var_type_name = var_type_name;
      parsed_instance.decl = instance_fd;
      parsed_instance.is_field_decl = true;

      instance_map_.insert(
          std::pair<Decl *, ModuleInstanceType>(instance_fd, parsed_instance));
    }

    if (instance_vd && instance_name_vd) {
      std::string name{instance_vd->getIdentifier()->getNameStart()};
      llvm::outs() << "@@ Found a member variable instance: " << name << "\n";
      if (ctor_decl && ctor_expr && ctor_arg) {
        ctor_arg->dump();
        llvm::outs() << "=> instance name: "
                     << cast<StringLiteral>(ctor_arg)->getString() << "\n";
      }

      // This is the main object's constructor name
      llvm::outs() << "Found constructor expression argument: "
                   << instance_name_vd->getNumArgs() << "\n";

      auto var_name{instance_vd->getNameAsString()};
      // We do not get the instance name from within the field declaration.
      // Get the type of the class of the field.
      auto var_type_name{instance_vd->getType().getAsString()};
      auto instance_name{cast<StringLiteral>(ctor_arg)->getString().str()};

      // This is the instance name.
      instances_.push_back(std::make_tuple(instance_name, instance_vd));

      ModuleInstanceType parsed_instance{};
      parsed_instance.var_name = var_name;
      parsed_instance.var_type_name = var_type_name;
      parsed_instance.instance_name = instance_name;
      parsed_instance.decl = instance_vd;
      parsed_instance.is_field_decl = false;

      // Don't add repeated matches
      auto found_it{instance_map_.find(instance_vd)};
      if (found_it == instance_map_.end()) {
        instance_map_.insert(std::pair<Decl *, ModuleInstanceType>(
            instance_vd, parsed_instance));
      }
    }
  }

  void dump() {
    // Instances holds both FieldDecl and VarDecl as its base class Decl.
    for (const auto &i : instance_map_) {
      llvm::outs() << "decl* " << i.first;
      auto instance{i.second};

      auto instance_field{instance.decl};
      if (isa<FieldDecl>(instance_field)) {
      //if (instance.is_field_decl){     
        llvm::outs() << " FieldDecl ";
      } else {
        llvm::outs() << " VarDecl ";
      }
      instance.dump();
    }
  }
};
};  // namespace sc_ast_matchers
#endif
