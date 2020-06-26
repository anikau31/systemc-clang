#ifndef _INSTANCE_MATCHER_H_
#define _INSTANCE_MATCHER_H_

#include <type_traits>
#include <vector>

#include "clang/ASTMatchers/ASTMatchers.h"

#include "ModuleInstanceType.h"

using namespace clang;
using namespace clang::ast_matchers;

namespace sc_ast_matchers {

AST_MATCHER(FieldDecl, hasTemplateDeclParent) {
  auto parent{Node.getParent()};

  if (isa<ClassTemplateDecl>(parent)) {
    // llvm::outs() << " @@@@@@@@ CTD \n";
    return false;
  }

  if (isa<ClassTemplateSpecializationDecl>(parent)) {
    // llvm::outs() << " @@@@@@@@ CTSD \n";
  }

  return true;
};

class InstanceArgumentMatcher : public MatchFinder::MatchCallback {
 private:
  clang::StringLiteral *instance_literal_;

 public:
  clang::StringLiteral *getInstanceLiteral() const { return instance_literal_; }

  void registerMatchers(MatchFinder &finder) {
    instance_literal_ = nullptr;
    auto arg_matcher =
        cxxConstructExpr(hasDescendant(cxxConstructExpr(
                             hasArgument(0, stringLiteral().bind("inst_arg")))))
            .bind("ctor_expr");

    // cxxConstructExpr(hasArgument(0,
    // stringLiteral().bind("inst_arg"))).bind("ctor_expr");

    finder.addMatcher(arg_matcher, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    auto ctor_expr = const_cast<CXXConstructExpr *>(
        result.Nodes.getNodeAs<CXXConstructExpr>("ctor_expr"));
    auto inst_arg = const_cast<StringLiteral *>(
        result.Nodes.getNodeAs<StringLiteral>("inst_arg"));

    llvm::outs() << "## InstanceArgumentMatcher\n";
    if (ctor_expr && inst_arg) {
      llvm::outs() << "@@ ctor expr\n";
      instance_literal_ = inst_arg;
    }
  }

  void dump() {
    if (instance_literal_) {
      instance_literal_->dump();
    }
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
  typedef std::tuple<std::string, Decl *, ModuleInstanceType> InstanceDeclType;
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

  // Map of Decl* => ModuleInstanceType
  InstanceDeclarations instance_map_;

 public:
  const InstanceDeclarations &getInstanceMap() { return instance_map_; }

  // Finds the instance with the same type as the argument.
  // Pass by reference to the instance.
  bool findInstanceByVariableType(
      CXXRecordDecl *decl, std::vector<InstanceDeclType> &found_instances) {
    // First check in the instance_fields.
    // Check to see if the pointer to the type is the same as the sc_module
    // type.

    llvm::outs() << "[findInstance] instance size: " << instance_map_.size()
                 << "\n";

    llvm::outs() << "find decl name: " << decl->getName() << " " << decl
                 << "\n";

    // Walk through all the instances.
    for (auto const &element : instance_map_) {
      auto p_field_var_decl{element.first};
      auto instance{element.second};

      // TODO factor out this code to be handled for both.
      if (auto *p_field{dyn_cast<FieldDecl>(p_field_var_decl)}) {
        auto qtype{p_field->getType().getTypePtr()};
        // qtype->dump();
        if (qtype->isRecordType()) {
          auto rt{qtype->getAsCXXRecordDecl()};
          llvm::outs() << "- fd " << rt << " " << p_field->getCanonicalDecl()
                       << " ";
          instance.dump();

          if (rt == decl) {
            llvm::outs() << "- Insert fieldDecl into found instance\n";
            found_instances.push_back(
                InstanceDeclType(instance.instance_name, rt, instance));
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
          llvm::outs() << "- vd " << rt << " "
                       << " ";
          instance.dump();
          if (rt == decl) {
            llvm::outs() << "- Insert vardecl into found instance\n";
            found_instances.push_back(
                InstanceDeclType(instance.instance_name, rt, instance));
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
       auto match_cxx_ctor_init =
      cxxRecordDecl(
          hasDescendant(
            cxxConstructorDecl(
              forEachConstructorInitializer(
                  cxxCtorInitializer(
                    isMemberInitializer(),
                    forField(
                      allOf(
                        hasType(hasUnqualifiedDesugaredType(recordType(hasDeclaration(
                          cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module")))
                          )))
                      ),
                      fieldDecl().bind("ctor_fd") 
                      )
                    )
                  ).bind("ctor_init")
                )
              )
            )
          );

 
    auto match_with_parent = 
      valueDecl(hasType(
            hasUnqualifiedDesugaredType(
              recordType(
                hasDeclaration(match_cxx_ctor_init))))).bind("parent_fd");


    auto match_instances_decl = 
      varDecl( 
        hasDescendant(match_ctor_arg("ctor_arg", "constructor_expr"))
        , 
        hasType(
          hasUnqualifiedDesugaredType(
            recordType(
              hasDeclaration(
                cxxRecordDecl(
                  isDerivedFrom("::sc_core::sc_module")
                  ).bind("var_cxx_decl")
                )
              ).bind("record_type")
            )
          )
        ).bind("instance_vd");

    
    auto test = fieldDecl(hasType(
          hasUnqualifiedDesugaredType(
            recordType(
              hasDeclaration(
          cxxRecordDecl(isDerivedFrom("::sc_core::sc_module"))))
            ))).bind("test_fd");
    /* clang-format on */

    // Add the two matchers.
    //
    finder.addMatcher(match_instances_decl, this);
    finder.addMatcher(match_with_parent, this);
    //finder.addMatcher(match_cxx_ctor_init, this);
    //finder.addMatcher( test, this );
  }

  void parseVarDecl(VarDecl *instance_decl, std::string &instance_name) {
    std::string name{instance_decl->getIdentifier()->getNameStart()};

    // This is the main object's constructor name
    auto var_name{instance_decl->getNameAsString()};
    // We do not get the instance name from within the field declaration.
    // Get the type of the class of the field.
    auto var_type_name{instance_decl->getType().getAsString()};
    // auto instance_name{cast<StringLiteral>(ctor_arg)->getString().str()};

    std::string parent_name{};
    ValueDecl *parent_rdecl{nullptr};

    llvm::outs() << "=> var_name " << var_name << " var_type_name "
                 << var_type_name << " parent_name " << parent_name
                 << "\n";  // instance_name "; // << instance_name << "\n";

    ModuleInstanceType parsed_instance{};
    parsed_instance.var_name = var_name;
    parsed_instance.var_type_name = var_type_name;
    parsed_instance.instance_name = instance_name;
    // This is the Type of the FieldDecl.
    parsed_instance.decl =
        instance_decl->getType().getTypePtr()->getAsCXXRecordDecl();
    // This is the FieldDecl.
    parsed_instance.instance_decl = instance_decl;
    parsed_instance.is_field_decl = false;
    parsed_instance.parent_name = parent_name;
    parsed_instance.parent_decl = parent_rdecl;

    parsed_instance.dump();
    // Don't add repeated matches
    auto found_it{instance_map_.find(instance_decl)};
    if (found_it == instance_map_.end()) {
      llvm::outs() << "Inserting VD instance\n";
      instance_map_.insert(std::pair<Decl *, ModuleInstanceType>(
          instance_decl, parsed_instance));
    }
  }

  void parseFieldDecl(FieldDecl* instance_decl, ValueDecl* parent_decl) {
    std::string name{instance_decl->getIdentifier()->getNameStart()};

    // This is the main object's constructor name
    auto var_name{instance_decl->getNameAsString()};
    // We do not get the instance name from within the field declaration.
    // Get the type of the class of the field.
    auto var_type_name{instance_decl->getType().getAsString()};
    // auto instance_name{cast<StringLiteral>(ctor_arg)->getString().str()};

    std::string parent_name{};
    //RecordDecl *parent_rdecl{parent_decl};
    //parent_rdecl = instance_decl->getParent();
    if (parent_decl) {
      parent_name = parent_decl->getName();
    }

    llvm::outs() << "=> var_name " << var_name << " var_type_name "
                 << var_type_name << " parent_name " << parent_name
                 << "\n";  // instance_name "; // << instance_name << "\n";

    ModuleInstanceType parsed_instance{};
    parsed_instance.var_name = var_name;
    parsed_instance.var_type_name = var_type_name;
    //    parsed_instance.instance_name = instance_name;
    parsed_instance.decl =
        instance_decl->getType().getTypePtr()->getAsCXXRecordDecl();
    parsed_instance.instance_decl = instance_decl;
    parsed_instance.is_field_decl = true;
    parsed_instance.parent_name = parent_name;
    parsed_instance.parent_decl = parent_decl;

    parsed_instance.dump();
    // Don't add repeated matches
    auto found_it{instance_map_.find(instance_decl)};
    if (found_it == instance_map_.end()) {
      llvm::outs() << "Inserting FD instance\n";
      instance_map_.insert(std::pair<Decl *, ModuleInstanceType>(
          instance_decl, parsed_instance));
    }
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    llvm::outs() << " ================== INSTANCE MATCHER ================= \n";
    // General decl
    auto instance_vd =
        const_cast<VarDecl *>(result.Nodes.getNodeAs<VarDecl>("instance_vd"));
    auto ctor_init = const_cast<CXXCtorInitializer *>(
        result.Nodes.getNodeAs<CXXCtorInitializer>("ctor_init"));
    auto ctor_fd =
        const_cast<FieldDecl *>(result.Nodes.getNodeAs<FieldDecl>("ctor_fd"));
    auto parent_fd =
        const_cast<ValueDecl *>(result.Nodes.getNodeAs<ValueDecl>("parent_fd"));

    auto ctor_arg = const_cast<Stmt *>(result.Nodes.getNodeAs<Stmt>("ctor_arg"));

    auto test_fd = const_cast<FieldDecl*>(result.Nodes.getNodeAs<FieldDecl>("test_fd"));
    if (test_fd) {
       llvm::outs() << "###### FOUND TESTFD\n";
        test_fd->dump();
     }
//
    if (ctor_fd && ctor_init && parent_fd ) {
      llvm::outs() << "#### CTOR_FD: parent_fd " << parent_fd->getNameAsString() << " ctor_fd " << ctor_fd->getNameAsString() << "\n";
      ctor_fd->dump();
      parseFieldDecl(ctor_fd, parent_fd);

      Expr *expr = ctor_init->getInit()->IgnoreImplicit();
      CXXConstructExpr *cexpr = cast<CXXConstructExpr>(expr);

      MatchFinder iarg_registry{};
      InstanceArgumentMatcher iarg_matcher{};
      iarg_matcher.registerMatchers(iarg_registry);
      iarg_registry.match(*cexpr, *result.Context);

      iarg_matcher.dump();

      // This retrieves the submodule instance name.
      if (auto inst_literal = iarg_matcher.getInstanceLiteral()) {
        auto submodule_instance_name = inst_literal->getString().str();

        // Find the instance if it has been already recorded.
        auto found_it{instance_map_.find(ctor_fd)};
        if (found_it != instance_map_.end()) {
          // has to be a reference
          auto &inst{found_it->second};
          inst.instance_name = submodule_instance_name;
        }
        llvm::outs() << "=> submodule_instance_name " << submodule_instance_name
                     << "\n";
      }
    }

    // Is it a FieldDecl or VarDecl

    if (instance_vd) {
      //if (auto vd_instance = dyn_cast<VarDecl>(decl_instance)) {
        auto instance_name{cast<StringLiteral>(ctor_arg)->getString().str()};
        llvm::outs() << "## FD: " << instance_vd->getNameAsString() << " "
                     << instance_vd << " instance_name " << instance_name << "\n";

        parseVarDecl(instance_vd, instance_name);
      //}
    }
  }

  // This is the callback function whenever there is a match.

  void dump() {
    // Instances holds both FieldDecl and VarDecl as its base class Decl.
    for (const auto &i : instance_map_) {
      llvm::outs() << "decl* " << i.first;
      auto instance{i.second};

      auto instance_field{instance.decl};
      if (isa<FieldDecl>(instance_field)) {
        // if (instance.is_field_decl){
        llvm::outs() << " FieldDecl ";
      } else {
        llvm::outs() << " VarDecl ";
      }
      instance.dump();
    }
  }
};  // namespace sc_ast_matchers
};  // namespace sc_ast_matchers
#endif
