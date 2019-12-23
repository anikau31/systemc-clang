#include <iostream>
#include "Matchers.h"
#include "clang/AST/DeclCXX.h"

using namespace std;
using namespace sc_ast_matchers;

//
// Helper functions that can be made private to this class.
//
// AST matcher to detect instances of sc_modules.
auto makeInstanceInModuleMatcher(const std::string &name) {
  auto match_module_instance = fieldDecl(
      hasType(cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module")))));
  return match_module_instance;
}
// End of helper functions

// Register the matchers
void ModuleDeclarationMatcher::registerMatchers(MatchFinder &finder) {
  /* clang-format off */
//
auto match_module_decls = 
  cxxRecordDecl(
      hasDefinition(), // There must be a definition.
      unless( isImplicit() ), // Templates generate implicit structs - so ignore.
      isDerivedFrom(
        hasName("::sc_core::sc_module") 
        ),
      unless(isDerivedFrom(matchesName("sc_event_queue")))
      );      
  /* clang-format on */

  // add all the matchers.
  finder.addMatcher(match_module_decls.bind("sc_module"), this);

  // add instance matcher
  instance_matcher.registerMatchers(finder);

  // add port (field) matcher
  port_matcher.registerMatchers(finder);

}

void ModuleDeclarationMatcher::run(const MatchFinder::MatchResult &result) {
  if (auto decl = const_cast<CXXRecordDecl *>(
          result.Nodes.getNodeAs<CXXRecordDecl>("sc_module"))) {
    cout << " Found sc_module: " << decl->getIdentifier()->getNameStart()
         << " CXXRecordDecl*: " << decl << endl;
    std::string name{decl->getIdentifier()->getNameStart()};
    // decl->dump();
    //
    if (isa<ClassTemplateSpecializationDecl>(decl)) {
      cout << "TEMPLATE SPECIAL\n";
      found_template_declarations_.push_back(std::make_tuple(name, decl));
    } else {
      found_declarations_.push_back(std::make_tuple(name, decl));
    }

    /* This is going to match only in subtree.
    MatchFinder instance_registry{};
    instance_matcher.registerMatchers(instance_registry);
    instance_registry.match(*decl, *result.Context);
    */
  }
}

void ModuleDeclarationMatcher::pruneMatches() {
  // Must have found instances.
  // 1. For every module found, check if there is an instance.
  // 2. If there is an instance, then add it into the list.

  for (auto const &element : found_declarations_) {
    auto decl{get<1>(element)};
    // std::cout << "## fd  name: " << get<0>(element) << "\n ";
    InstanceListType instance_list;
    InstanceMatcher::InstanceDeclType instance;
    if (instance_matcher.findInstance(decl, instance)) {
      // pruned_declarations_.push_back(element);
      pruned_declarations_map_.insert(
          ModuleDeclarationPairType(decl, get<0>(element)));
      instance_list.push_back(instance);
    declaration_instance_map_.insert( DeclarationInstancePairType(decl, instance_list));
    }

  }

  for (auto const &element : found_template_declarations_) {
    auto decl{get<1>(element)};
    // std::cout << "## ftd name: " << get<0>(element) << "\n ";
    InstanceListType instance_list;
    InstanceMatcher::InstanceDeclType instance;
    if (instance_matcher.findInstance(decl, instance)) {
      // pruned_declarations_.push_back(element);
      pruned_declarations_map_.insert(
          ModuleDeclarationPairType(decl, get<0>(element)));
      instance_list.push_back(instance);
    declaration_instance_map_.insert(
        DeclarationInstancePairType(decl, instance_list));
    }
  }
}

void ModuleDeclarationMatcher::dump() {

  cout << "## Non-template module declarations: " << found_declarations_.size() << "\n";
  for (const auto &i : found_declarations_) {
    cout << "module name         : " << get<0>(i) << ", " << get<1>(i)
         << std::endl;
  }

  cout << "## Template module declarations: " << found_template_declarations_.size() << "\n";
  for (const auto &i : found_template_declarations_) {
    cout << "template module name: " << get<0>(i) << ", " << get<1>(i)
         << std::endl;
  }

  // for (const auto &i : pruned_declarations_) {
  // cout << "pruned module name: " << get<0>(i) << ", " << get<1>(i)
  // << std::endl;
  // }
  
  cout << "## Pruned declaration Map: " << pruned_declarations_map_.size() << "\n";
  for (const auto &i : pruned_declarations_map_) {
    auto decl{i.first};
    auto decl_name{i.second};
    cout << "CXXRecordDecl* " << i.first << ", module name: " << decl_name
         << "\n";
  }

  // Print the instances.
  instance_matcher.dump();

  cout << "\n## Dump map of decl->instances: " << declaration_instance_map_.size() << "\n";

  for (const auto &i : declaration_instance_map_) {
    auto decl{i.first};
    auto instance_list{i.second};

    cout << "decl: " << decl->getIdentifier()->getNameStart();
    for (const auto &instance : instance_list) {
      cout << ", instance type: " << get<0>(instance) << ",   "
           << get<1>(instance) << "\n";
    }
  }

  cout << "\n";
  cout << "## Printing ports" << endl;
  port_matcher.dump();
}
