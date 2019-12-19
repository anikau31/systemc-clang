#include <iostream>
#include "Matchers.h"
#include "clang/AST/DeclCXX.h"

using namespace std;
using namespace sc_ast_matchers;

//
// Helper functions that can be made private to this class.
//

void printTemplateArguments(ModuleDeclarationMatcher::PortType &found_ports) {
  // Input ports
  for (const auto &i : found_ports) {
    llvm::outs() << "name: " << get<0>(i)
                 << ", FieldDecl*: " << get<1>(i)->getFieldDecl();
    get<1>(i)->getTemplateType()->printTemplateArguments(llvm::outs());
    llvm::outs() << "\n";
  }
}

auto parseTemplateType(const FieldDecl *fd) {
  //}, const ModuleDeclarationMatcher::PortType &found_ports ) {
  QualType qual_type{fd->getType()};
  const Type *type_ptr{qual_type.getTypePtr()};
  auto template_ptr{new FindTemplateTypes()};
  template_ptr->Enumerate(type_ptr);
  return template_ptr;
}

template <typename NodeType>
auto checkMatch(const std::string &name,
                const MatchFinder::MatchResult &result) {
  return result.Nodes.getNodeAs<NodeType>(name);
}

// AST matcher to detect instances of sc_modules.
auto makeInstanceInModuleMatcher(const std::string &name) {
  auto match_module_instance = fieldDecl(
      hasType(cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module")))));
  return match_module_instance;
}

// AST matcher to detect field declarations
auto makeFieldMatcher(const std::string &name) {
  /* clang-format off */
  return  cxxRecordDecl(
      isExpansionInMainFile(),
      isDerivedFrom(
        hasName("::sc_core::sc_module")
        ),
      forEach(
        fieldDecl(hasType(cxxRecordDecl(hasName(name)))).bind(name)
        )
      );
  /* clang-format on */
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

auto match_sc_in_clk = cxxRecordDecl( isDerivedFrom(hasName("sc_module")),
    forEach( 
      fieldDecl(
        hasType(
          namedDecl(
            hasName("sc_in_clk")
            )
          )
        ).bind("sc_in_clk")
      )
    );

auto match_non_sc_types = cxxRecordDecl(
    match_module_decls,
    forEach(
      fieldDecl(
        allOf(
        unless(hasType(cxxRecordDecl(matchesName("sc*")))),
          unless(hasType(namedDecl(hasName("sc_in_clk"))))
          )
        ).bind("other_fields")
      )
    );

  /* clang-format on */

  auto match_in_ports = makeFieldMatcher("sc_in");
  auto match_out_ports = makeFieldMatcher("sc_out");
  auto match_in_out_ports = makeFieldMatcher("sc_inout");
  auto match_internal_signal = makeFieldMatcher("sc_signal");

  // add all the matchers.
  finder.addMatcher(match_module_decls.bind("sc_module"), this);

  // add instance matcher
  instance_matcher.registerMatchers(finder);

  finder.addMatcher(match_sc_in_clk, this);
  finder.addMatcher(match_in_ports, this);
  finder.addMatcher(match_out_ports, this);
  finder.addMatcher(match_in_out_ports, this);
  finder.addMatcher(match_internal_signal, this);
  finder.addMatcher(match_non_sc_types, this);
}

template <typename T>
void ModuleDeclarationMatcher::insert_port(PortType &port, T *decl) {
  auto name{decl->getIdentifier()->getNameStart()};
  port.push_back(
      std::make_tuple(name, new PortDecl(name, decl, parseTemplateType(decl))));
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

  if (auto fd = checkMatch<FieldDecl>("sc_in_clk", result)) {
    std::string port_name{fd->getIdentifier()->getNameStart()};
    cout << " Found sc_in_clk: " << port_name << endl;

    insert_port(clock_ports_, fd);
  }

  if (auto fd = checkMatch<FieldDecl>("sc_in", result)) {
    auto port_name{fd->getIdentifier()->getNameStart()};
    cout << " Found sc_in: " << port_name << endl;
    insert_port(in_ports_, fd);
  }

  if (auto fd = checkMatch<FieldDecl>("sc_out", result)) {
    auto port_name{fd->getIdentifier()->getNameStart()};
    cout << " Found sc_out: " << port_name << endl;
    insert_port(out_ports_, fd);
  }

  if (auto fd = checkMatch<FieldDecl>("sc_inout", result)) {
    auto port_name{fd->getIdentifier()->getNameStart()};
    cout << " Found sc_inout: " << port_name << endl;
    insert_port(inout_ports_, fd);
  }

  if (auto fd = checkMatch<FieldDecl>("sc_signal", result)) {
    auto signal_name{fd->getIdentifier()->getNameStart()};
    cout << " Found sc_signal: " << signal_name << endl;
    insert_port(signal_fields_, fd);
  }

  if (auto fd = checkMatch<FieldDecl>("other_fields", result)) {
    auto field_name{fd->getIdentifier()->getNameStart()};
    cout << " Found others fields: " << field_name << endl;
    insert_port(other_fields_, fd);
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
  printTemplateArguments(clock_ports_);
  printTemplateArguments(in_ports_);
  printTemplateArguments(out_ports_);
  printTemplateArguments(inout_ports_);
  printTemplateArguments(signal_fields_);
  printTemplateArguments(other_fields_);
}
