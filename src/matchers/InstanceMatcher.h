#ifndef _INSTANCE_MATCHER_H_
#define _INSTANCE_MATCHER_H_

//#include <type_traits>
#include <vector>

#include "ModuleInstanceType.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "llvm/Support/Debug.h"

/// Different matchers may use different DEBUG_TYPE
#undef DEBUG_TYPE
#define DEBUG_TYPE "InstanceMatcher"

using namespace clang::ast_matchers;

namespace sc_ast_matchers {
  using namespace clang;

class InstanceArgumentMatcher : public MatchFinder::MatchCallback {
 private:
  clang::StringLiteral* instance_literal_;

 public:
  clang::StringLiteral *getInstanceLiteral() const { return instance_literal_; }

  void registerMatchers(MatchFinder &finder) {
    instance_literal_ = nullptr;
    auto arg_matcher =
        cxxConstructExpr(hasDescendant(
              cxxConstructExpr(hasArgument(0, 
                  stringLiteral().bind("inst_arg"))
                )
              )
            ).bind("ctor_expr");

    finder.addMatcher(arg_matcher, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    auto ctor_expr = const_cast<clang::CXXConstructExpr *>(
        result.Nodes.getNodeAs<clang::CXXConstructExpr>("ctor_expr"));
    auto inst_arg = const_cast<clang::StringLiteral *>(
        result.Nodes.getNodeAs<clang::StringLiteral>("inst_arg"));

    LLVM_DEBUG(llvm::dbgs() << "## InstanceArgumentMatcher\n");
    if (ctor_expr && inst_arg) {
      LLVM_DEBUG(llvm::dbgs() << "@@ ctor expr\n");
      instance_literal_ = inst_arg;
    }
  }

  void dump() {
    if (instance_literal_) {
      LLVM_DEBUG(instance_literal_->dump(););
    }
  }
};

///////////////////////////////////////////////////////////////////////////////
//
/// Class InstanceMatcher
//
//
///////////////////////////////////////////////////////////////////////////////
class InstanceMatcher : public MatchFinder::MatchCallback {
 public:
  typedef std::tuple<std::string, clang::Decl *, ModuleInstanceType>
      InstanceDeclType;
  typedef std::vector<InstanceDeclType> InstanceDeclarationsType;

  /// Store all instances in a map.
  /// The map key should be the Decl*.  This will work for both FieldDecl
  /// (instances within sub-modules) and VarDecl separate modules in sc_main().
  //

  typedef std::pair<clang::Decl *, ModuleInstanceType> ModuleInstanceTuple;
  typedef std::multimap<clang::Decl *, ModuleInstanceType> InstanceDeclarations;

 private:
  /// Instances can come in two forms:
  /// FieldDecl: this is when they are members of a class.
  /// VarDecl  : this is when they are simply variables such as in functions or
  /// in the main().
  ///
  /// The way to identify them both together is to look at its base class Decl.
  /// Then use dyn_cast<> to detect whether it is one of the two above mentioned
  /// types.
  //
  /// deprecated
  InstanceDeclarationsType instances_;

  /// Map of Decl* => ModuleInstanceType
  InstanceDeclarations instance_map_;

 public:
  const InstanceDeclarations &getInstanceMap() { return instance_map_; }

  // Finds the instance with the same type as the argument.
  // Pass by reference to the instance.
  bool findInstanceByVariableType(
      clang::CXXRecordDecl *decl,
      std::vector<InstanceDeclType> &found_instances) {
    // First check in the instance_fields.
    // Check to see if the pointer to the type is the same as the sc_module
    // type.

    LLVM_DEBUG(llvm::dbgs() << "\n[findInstance] instance size: "
                            << instance_map_.size() << "\n");

    LLVM_DEBUG(llvm::dbgs()
               << "find decl name: " << decl->getName() << " " << decl << "\n");

    // Walk through all the instances.
    for (auto const &element : instance_map_) {
      auto p_field_var_decl{element.first};
      auto instance{element.second};

      // TODO factor out this code to be handled for both.
      if (auto *p_field{clang::dyn_cast<clang::FieldDecl>(p_field_var_decl)}) {
        auto qtype{p_field->getType().getTypePtr()};
        if (qtype->isRecordType()) {
          auto rt{qtype->getAsCXXRecordDecl()};
          LLVM_DEBUG(llvm::dbgs() << "- fd " << rt << " "
                                  << p_field->getCanonicalDecl() << " ");
          LLVM_DEBUG(instance.dump());

          if (rt == decl) {
            LLVM_DEBUG(llvm::dbgs()
                       << "- Insert fieldDecl into found instance\n");
            found_instances.push_back(
                InstanceDeclType(instance.instance_name, rt, instance));
          }
        }
      } else {
        // This is a VarDecl instance.
        auto p_var{clang::dyn_cast<clang::VarDecl>(p_field_var_decl)};
        auto qtype{p_var->getType().getTypePtr()};

        std::string dbg{"[InstanceMatcher] VarDecl"};
        if (qtype->isRecordType()) {
          auto rt{qtype->getAsCXXRecordDecl()};
          LLVM_DEBUG(llvm::dbgs() << "- vd " << rt << " "
                                  << " ");
          LLVM_DEBUG(instance.dump());
          if (rt == decl) {
            LLVM_DEBUG(llvm::dbgs()
                       << "- Insert vardecl into found instance\n");
            found_instances.push_back(
                InstanceDeclType(instance.instance_name, rt, instance));
          }
        }
      }
    }
    LLVM_DEBUG(llvm::outs()
               << "=> found_instances: " << found_instances.size() << "\n");

    return (found_instances.size() != 0);
  }

  auto match_ctor_arg(const std::string &bind_arg_name,
                      const std::string &bind_ctor_expr) {
    return cxxConstructExpr(hasArgument(0, stringLiteral().bind(bind_arg_name)))
        .bind(bind_ctor_expr);
  }

  auto makeArraySubModule(llvm::StringRef name) {
    return arrayType(hasElementType(hasUnqualifiedDesugaredType(
        recordType(hasDeclaration(cxxRecordDecl(isDerivedFrom(hasName(name)))
                                      .bind("submodule"))  // hasDeclaration
                   )                                       // recordType
        )));
  }

  void registerMatchers(MatchFinder &finder) {
    /// We will have two matchers.
    ///
    /// Match when the following conditions are satisifed:
    /// * It is a FieldDecl
    /// * It has a type that is a C++ class that is derived from sc_module
    ///
    /// These are field members within a class declaration.  Hence, we only need
    /// to collect their FieldDecl pointers and their variable names.  Their
    /// instance names would appear when the constructor of the class that has
    /// these field members within it.  This constructor would use the
    /// initialization list to provide an argument, which would be the instance
    /// name.
    ///
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

                        anyOf(

                          hasType(
                            hasUnqualifiedDesugaredType(
                              recordType(
                                hasDeclaration(
                                  cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module")))
                            )))
                          )//hasType

                          ,
                          //1d
                            hasType(
                               hasUnqualifiedDesugaredType(
                                 arrayType(
                                   hasElementType(hasUnqualifiedDesugaredType(
                                           recordType(
                                             hasDeclaration(
                                              cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module"))).bind("submodule")
                                              ) //hasDeclaration
                                           )// recordType
                                         )
                                         )
                                   )//arrayType
                                 )
                               )//hasType

                          ) //anyOf
                        ,
                      fieldDecl().bind("ctor_fd") 
                      )
                    )
                  ).bind("ctor_init")
                )
              )
            ) //hasDescendant
          ); //cxxRecordDecl


    auto match_with_parent = 
      valueDecl(hasType(
            hasUnqualifiedDesugaredType(
              recordType(
                hasDeclaration(match_cxx_ctor_init))))).bind("parent_fd");


    auto match_instances_decl = 
      valueDecl( 
        hasDescendant(
          match_ctor_arg("ctor_arg", "constructor_expr"))
        , 
        anyOf(
            // 1d
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
            ) // hasType
          // 2d
          ,
           hasType(
             hasUnqualifiedDesugaredType(
               arrayType(
                 hasElementType(hasUnqualifiedDesugaredType(
                         recordType(
                           hasDeclaration(
                            cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module"))).bind("submodule")
                            ) //hasDeclaration
                         )// recordType
                       )
                       )
                 )//arrayType
               )
             )//hasType
        )// anyOf
      ).bind("instance_vd");

    
    auto test = 
      fieldDecl(
        hasType(
          hasUnqualifiedDesugaredType(
            recordType(
              hasDeclaration(
                cxxRecordDecl(isDerivedFrom("::sc_core::sc_module"))
                ) //hasDeclaration
              )  //recordType
          ) //hasUnqualifiedDesugaredType
        ) //hasType
      ).bind("test_fd");
    /* clang-format on */

    /// Add the two matchers.
    //
    finder.addMatcher(match_instances_decl, this);
    finder.addMatcher(match_with_parent, this);
  }

  void parseVarDecl(clang::VarDecl *instance_decl, std::string &instance_name) {
    std::string name{instance_decl->getIdentifier()->getNameStart()};

    /// This is the main object's constructor name
    auto var_name{instance_decl->getNameAsString()};
    /// We do not get the instance name from within the field declaration.
    /// Get the type of the class of the field.
    auto var_type_name{instance_decl->getType().getAsString()};
    /// auto instance_name{cast<StringLiteral>(ctor_arg)->getString().str()};

    std::string parent_name{};
    clang::ValueDecl *parent_rdecl{nullptr};

    LLVM_DEBUG(llvm::dbgs()
               << "=> VD: var_name " << var_name << " var_type_name "
               << var_type_name << " parent_name " << parent_name
               << "\n");  // instance_name "; // << instance_name << "\n";

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

    LLVM_DEBUG(parsed_instance.dump(););
    // Don't add repeated matches
    //    auto found_it{instance_map_.find(instance_decl)};
    //   if (found_it == instance_map_.end()) {
    LLVM_DEBUG(llvm::dbgs() << "Inserting VD instance"
                            << "\n");
    instance_map_.insert(
        std::pair<Decl *, ModuleInstanceType>(instance_decl, parsed_instance));
    //   }
  }

  void parseFieldDecl(clang::FieldDecl *instance_decl,
                      clang::ValueDecl *parent_decl,
                      std::string instance_name) {
    std::string name{instance_decl->getIdentifier()->getNameStart()};

    // This is the main object's constructor name
    auto var_name{instance_decl->getNameAsString()};
    // We do not get the instance name from within the field declaration.
    // Get the type of the class of the field.
    auto var_type_name{instance_decl->getType().getAsString()};

    std::string parent_name{};
    if (parent_decl) {
      parent_name = parent_decl->getName();
    }

    LLVM_DEBUG(llvm::dbgs()
               << "=> FD: var_name " << var_name << " var_type_name "
               << var_type_name << " parent_name " << parent_name << "\n");

    ModuleInstanceType parsed_instance{};
    parsed_instance.var_name = var_name;
    parsed_instance.var_type_name = var_type_name;
    instance_decl->getType()->dump();

    auto array_type{
        instance_decl->getType().getTypePtr()->getAsArrayTypeUnsafe()};
    // Array type.
    if (array_type) {
      auto element_type{array_type->getElementType().getTypePtr()};
      parsed_instance.decl = element_type->getAsCXXRecordDecl();
    } else {
      // Not an array type.
      parsed_instance.decl =
          instance_decl->getType().getTypePtr()->getAsCXXRecordDecl();
    }

    instance_decl->dump();
    parsed_instance.instance_decl = instance_decl;
    parsed_instance.is_field_decl = true;
    parsed_instance.parent_name = parent_name;
    parsed_instance.parent_decl = parent_decl;
    parsed_instance.instance_name = instance_name;

    LLVM_DEBUG(parsed_instance.dump(););
    // Don't add repeated matches
    LLVM_DEBUG(llvm::dbgs() << "Inserting FD instance\n");
    instance_map_.insert(
        std::pair<Decl *, ModuleInstanceType>(instance_decl, parsed_instance));
    llvm::outs() << "INSERTED\n";
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    LLVM_DEBUG(llvm::dbgs()
               << " ================== INSTANCE MATCHER ================= \n");
    // General decl
    auto instance_vd = const_cast<clang::VarDecl *>(
        result.Nodes.getNodeAs<clang::VarDecl>("instance_vd"));
    auto ctor_init = const_cast<clang::CXXCtorInitializer *>(
        result.Nodes.getNodeAs<clang::CXXCtorInitializer>("ctor_init"));
    auto ctor_fd = const_cast<clang::FieldDecl *>(
        result.Nodes.getNodeAs<FieldDecl>("ctor_fd"));
    auto parent_fd = const_cast<clang::ValueDecl *>(
        result.Nodes.getNodeAs<clang::ValueDecl>("parent_fd"));

    auto ctor_arg = const_cast<clang::Stmt *>(
        result.Nodes.getNodeAs<clang::Stmt>("ctor_arg"));

    auto test_fd = const_cast<clang::FieldDecl *>(
        result.Nodes.getNodeAs<clang::FieldDecl>("test_fd"));
    //
    if (ctor_fd && ctor_init && parent_fd) {
      LLVM_DEBUG(llvm::dbgs()
                 << "#### CTOR_FD: parent_fd " << parent_fd->getNameAsString()
                 << " ctor_fd " << ctor_fd->getNameAsString() << "\n");
      LLVM_DEBUG(ctor_fd->dump());

      // llvm::outs() << "### DEBUG\n";
      // ctor_init->getInit()->dump();
      clang::Expr *expr = ctor_init->getInit()->IgnoreImplicit();
      clang::CXXConstructExpr *cexpr{clang::dyn_cast<clang::CXXConstructExpr>(expr)};
      clang::InitListExpr *iexpr{clang::dyn_cast<clang::InitListExpr>(expr)};

      // For arrays, an InitListExpr is generated.
      // For non-arrays, CXXConstructExpr is directly castable.
      //
      // If it is an array, then get to its InitListExpr, and then get the first
      // element's constructor.
      if ((iexpr != nullptr) && (cexpr == nullptr)) {
        llvm::outs() << "### IEXPR is not NULL\n";

        for (auto init : iexpr->inits()) {
          cexpr = clang::dyn_cast<clang::CXXConstructExpr>(init);
          // TODO: move into a function

          MatchFinder iarg_registry{};
          InstanceArgumentMatcher iarg_matcher{};
          iarg_matcher.registerMatchers(iarg_registry);
          iarg_registry.match(*cexpr, *result.Context);

          LLVM_DEBUG(iarg_matcher.dump(););

          // This retrieves the submodule instance name.
          if (auto inst_literal = iarg_matcher.getInstanceLiteral()) {
            auto submodule_instance_name = inst_literal->getString().str();

            parseFieldDecl(ctor_fd, parent_fd, submodule_instance_name);
            LLVM_DEBUG(llvm::dbgs() << "=> submodule_instance_name "
                                    << submodule_instance_name << "\n");
          }
        }
        // cexpr = cast<clang::CXXConstructExpr>(iexpr->inits()[0]);
      } else {
        MatchFinder iarg_registry{};
        InstanceArgumentMatcher iarg_matcher{};
        iarg_matcher.registerMatchers(iarg_registry);
        iarg_registry.match(*cexpr, *result.Context);

        LLVM_DEBUG(iarg_matcher.dump(););

        // This retrieves the submodule instance name.
        if (auto inst_literal = iarg_matcher.getInstanceLiteral()) {
          auto submodule_instance_name = inst_literal->getString().str();
          parseFieldDecl(ctor_fd, parent_fd, submodule_instance_name);
          LLVM_DEBUG(llvm::dbgs() << "=> submodule_instance_name "
                                  << submodule_instance_name << "\n");
        }
      }
    }

    // Is it a FieldDecl or VarDecl

    if (instance_vd) {
      auto instance_name{cast<StringLiteral>(ctor_arg)->getString().str()};
      LLVM_DEBUG(llvm::dbgs()
                 << "## VD: " << instance_vd->getNameAsString() << " "
                 << instance_vd << " instance_name " << instance_name << "\n");

      parseVarDecl(instance_vd, instance_name);
    }
  }

  // This is the callback function whenever there is a match.

  void dump() {
    // Instances holds both FieldDecl and VarDecl as its base class Decl.
    llvm::outs() << "################## INSTANCE MATCHER DUMP \n";
    for (const auto &i : instance_map_) {
      auto instance{i.second};
      llvm::outs() << "decl* " << i.first << "  " << instance.instance_name;

      auto instance_field{instance.decl};
      llvm::outs() << " instance_field*: " << instance_field << "\n";

      if (clang::dyn_cast<clang::FieldDecl>(instance_field)) {
        if (instance.is_field_decl) {
          llvm::outs() << " FieldDecl ";
        } else {
          llvm::outs() << " VarDecl ";
        }
      }

      instance.dump();
    }
  }
};
};  // namespace sc_ast_matchers
#endif
