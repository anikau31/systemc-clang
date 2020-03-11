#ifndef _MATCHERS_HPP_
#define _MATCHERS_HPP_
#include <map>
#include <tuple>
#include <vector>
#include "FindTemplateTypes.h"
#include "ModuleDecl.h"
#include "PortDecl.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace scpar;

namespace sc_ast_matchers {

template <typename NodeType>
auto checkMatch(const std::string &name,
                const MatchFinder::MatchResult &result) {
  return result.Nodes.getNodeAs<NodeType>(name);
}

/*
 AST_MATCHER_P(CXXCtorInitializer, forField,
               internal::Matcher<FieldDecl>, InnerMatcher) {
   const FieldDecl *NodeAsDecl = Node.getAnyMember();
   return (NodeAsDecl != nullptr &&
       InnerMatcher.matches(*NodeAsDecl, Finder, Builder));
 }
*/

AST_MATCHER(FieldDecl, matchesTypeName) {
  auto type_ptr{Node.getType().getTypePtr()};
  llvm::outs() << "[[OWN MATCHER]]\n";
  Node.dump();
  type_ptr->dump();

  if ((type_ptr == nullptr) || (type_ptr->isBuiltinType())) {
    type_ptr->dump();
    return true;
  } else {
    FindTemplateTypes te;
    te.Enumerate(type_ptr);
    te.printTemplateArguments(llvm::outs());

    auto args{te.getTemplateArgumentsType()};
    FindTemplateTypes::argVectorType::iterator ait{args.begin()};

    if (args.size() == 0) {
      return true;
    }

    string field_type{ait->getTypeName()};
    llvm::outs() << "\n@@@@@ Field type: " << field_type << "\n";

    if ((field_type != "sc_in") && (field_type != "sc_out") &&
        (field_type != "sc_signal") && (field_type != "sc_stream_in") &&
        (field_type != "sc_stream_out")) {
      return true;
    } else {
      return false;
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
  bool findInstance(CXXRecordDecl *decl, InstanceDeclType &instance) {
    // First check in the instance_fields.
    // Check to see if the pointer to the type is the same as the sc_module
    // type.

    InstanceDeclarationsType::iterator found_it = std::find_if(
        instances_.begin(), instances_.end(),
        [&decl](const InstanceDeclType &element) {
          // Get the CXXRecordDecl for the instance.
          // The instance is the second element in the tuple.
          auto p_field_var_decl{get<1>(element)};

          if (auto *p_field{dyn_cast<FieldDecl>(p_field_var_decl)}) {
            auto qtype{p_field->getType().getTypePtr()};
            if (qtype->isRecordType()) {
              if (auto dp = qtype->getAs<TemplateSpecializationType>()) {
                auto rt{dp->getAsCXXRecordDecl()};
                return (rt == decl);
              }
            }
          } else {
            // This is a VarDecl instance.
            auto p_var{dyn_cast<VarDecl>(p_field_var_decl)};
            auto qtype{p_var->getType().getTypePtr()};
            if (qtype->isRecordType()) {
              auto rt{qtype->getAsCXXRecordDecl()};
              return (rt == decl);
            }
          }
          return false;
        });

    if (found_it != instances_.end()) {
      get<0>(instance) = get<0>(*found_it);
      get<1>(instance) = get<1>(*found_it);

      return true;
    }
    return false;
  }

  void registerMatchers(MatchFinder &finder) {
    /* clang-format off */

    // We will have two matchers.
    //
    // Match when the following conditions are satisifed:
    // * It is a FieldDecl
    // * It has a type that is a C++ class that is derived from sc_module
    //
    auto match_instances = fieldDecl(
        hasType(
          cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module"))))
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
      instances_.push_back(std::make_tuple(name, instance));
    }

    if (auto instance = const_cast<VarDecl *>(
            result.Nodes.getNodeAs<VarDecl>("instances_in_vardecl"))) {
      std::string name{instance->getIdentifier()->getNameStart()};
      llvm::outs() << "@@ Found a member variable instance: " << name << "\n";

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
      llvm::outs() << "[DBG] module declarations name: " << get<0>(i) << ", "
                   << get<1>(i) << "\n";

      auto p_field_var_decl{get<1>(i)};
      if (isa<FieldDecl>(p_field_var_decl)) {
        llvm::outs() << "[DBG] FieldDecl\n";
      } else {
        llvm::outs() << "[DBG] VarDecl\n";
      }
    }
  }
};

///////////////////////////////////////////////////////////////////////////////
// Class PortMatcher
//
//
///////////////////////////////////////////////////////////////////////////////

class PortMatcher : public MatchFinder::MatchCallback {
 public:
  typedef pair<string, ModuleDecl *> ModulePairType;
  typedef std::vector<ModulePairType> ModuleMapType;

  typedef std::vector<std::tuple<std::string, PortDecl *> > PortType;

 private:
  std::string top_module_decl_;

 public:
  PortType clock_ports_;
  PortType in_ports_;
  PortType out_ports_;
  PortType inout_ports_;
  PortType other_fields_;
  PortType signal_fields_;
  PortType instream_ports_;
  PortType outstream_ports_;
  PortType sc_ports_;

 public:
  const PortType &getClockPorts() const { return clock_ports_; }
  const PortType &getInputPorts() const { return in_ports_; }
  const PortType &getOutputPorts() const { return out_ports_; }
  const PortType &getInOutPorts() const { return inout_ports_; }
  const PortType &getOtherVars() const { return other_fields_; }
  const PortType &getSignals() const { return signal_fields_; }
  const PortType &getInputStreamPorts() const { return instream_ports_; }
  const PortType &getOutputStreamPorts() const { return outstream_ports_; }
  const PortType &getPorts() const { return sc_ports_; }

  PortMatcher(const std::string &top_module = ".*")
      : top_module_decl_{top_module} {}

  // AST matcher to detect field declarations
  auto makeFieldMatcher(const std::string &name) {
    /* clang-format off */

    // The generic field matcher has the following conditions.
    // * It is in the main provided file (not in the library files).
    // * It is derived from sc_module
    // * For each child that is a FieldDecl, 
    //   - The type of it is a C++ class whose class name is "name"
  return  cxxRecordDecl(
      isExpansionInMainFile(),
      isDerivedFrom(hasName("::sc_core::sc_module")
        ),
      forEach(
        fieldDecl(hasType(cxxRecordDecl(hasName(name)))).bind(name)
        )
      );
  }

  // This is a matcher to identify sc_signal.
  // The reason for this matcher is for it to match arrays of sc_signals as well.
  // The conditions are as follows:
  // * It must be a FieldDecl
  //   - It must have a type that is either an array whose type is a c++ class derived 
  //     from a class name called "name"
  //   - Or, it is has a type that is a c++ class that is derived from class name "name".
  auto makeArrayType(const std::string &name) {
    return hasType(arrayType(hasElementType(hasDeclaration(cxxRecordDecl(isDerivedFrom(hasName(name)))))));
  }

  auto makeSignalMatcher(const std::string &name) {
    return fieldDecl(anyOf(
          makeArrayType(name),
        hasType(cxxRecordDecl(isDerivedFrom(hasName(name))))));
  }

  // This is a matcher for sc_port.
  // It has the following conditions:
  // * It must be a FieldDecl
  //   - It has a type that is an array whose type has a name "name"
  //   - Or, it has a type that is a C++ class whose class name is "name".
  //
  auto makePortHasNameMatcher(const std::string &name) {
    return fieldDecl(anyOf(hasType(arrayType(hasElementType(asString(name)))),
                           hasType(cxxRecordDecl((hasName(name))))));
  }

  // This is a matcher for sc_in_clk since it uses a NamedDecl.
  // It has the following conditions:
  // * It must be a FieldDecl,
  //  - It has a type that is an array whose type has a name "name".
  //  - Or, it has a type that is a NamedDecl whose name is "name".
  //
  auto makePortHasNamedDeclNameMatcher(const std::string &name) {
    return fieldDecl(anyOf(hasType(arrayType(hasElementType(asString(name)))),
                           hasType(namedDecl(hasName(name)))));
  }

  /* clang-format on */
  void printTemplateArguments(PortType &found_ports) {
    // Input ports
    for (const auto &i : found_ports) {
      llvm::outs() << "name: " << get<0>(i)
                   << ", FieldDecl*: " << get<1>(i)->getFieldDecl();
      get<1>(i)->getTemplateType()->printTemplateArguments(llvm::outs());
      llvm::outs() << "\n";
    }
  }

  template <typename T>
  auto parseTemplateType(const T *fd) {
    QualType qual_type{fd->getType()};
    const Type *type_ptr{qual_type.getTypePtr()};
    auto template_ptr{new FindTemplateTypes()};
    template_ptr->Enumerate(type_ptr);
    template_ptr->printTemplateArguments(llvm::outs());
    return template_ptr;
  }

  template <typename T>
  void insert_port(PortType &port, T *decl, bool isFieldDecl = true) {
    // port is a map entry [CXXRecordDecl* => vector<PortDecl*>]

    std::string name{};
    if (auto *fd = dyn_cast<FieldDecl>(decl)) {
      name = fd->getIdentifier()->getNameStart();
      port.push_back(std::make_tuple(
          name, new PortDecl(name, decl, parseTemplateType(fd))));
    } else {
      auto *vd = dyn_cast<VarDecl>(decl);
      name = vd->getIdentifier()->getNameStart();
      port.push_back(std::make_tuple(
          name, new PortDecl(name, decl, parseTemplateType(vd))));
    }
  }

  void registerMatchers(MatchFinder &finder) {
    /* clang-format off */

    //
    // Matchers for compositions.
    // These matchers are used to form other matchers. 
    auto match_module_decls = 
      cxxRecordDecl(
          matchesName(top_module_decl_), // Specifies the top-level module name.
          hasDefinition(),               // There must be a definition.
          unless( isImplicit() ),        // Templates generate implicit structs - so ignore.
          isDerivedFrom(
            hasName("::sc_core::sc_module") 
            ),
          unless(isDerivedFrom(matchesName("sc_event_queue")))
      );      


     // Matches all the SystemC Ports.
    auto match_sc_ports = cxxRecordDecl(
        match_module_decls,
        forEach(
          fieldDecl(
            hasType(
              cxxRecordDecl(isDerivedFrom(hasName("sc_port")))
            )
          ).bind("sc_port")
        )
      );

     // Notice that the ports can also be arrays.
     auto match_all_ports = cxxRecordDecl(
         match_module_decls,
         eachOf(
           forEach(
             makePortHasNamedDeclNameMatcher("sc_in_clk").bind("sc_in_clk")),
           forEach(
               makePortHasNameMatcher("sc_in").bind("sc_in")),
           forEach( 
               makePortHasNameMatcher("sc_out").bind("sc_out")),
           forEach(
               makePortHasNameMatcher("sc_inout").bind("sc_inout")),
           forEach(
             makeSignalMatcher("sc_signal_inout_if").bind("sc_signal")),
           forEach(
               makePortHasNameMatcher("sc_stream_in").bind("sc_stream_in")),
           forEach(
               makePortHasNameMatcher("sc_stream_out").bind("sc_stream_out")))
               );

    // TODO: I'm not convinced that this would work. 
    //
    // I wonder if the way to fix this is to to unless(match_all_ports))
    auto match_non_sc_types = cxxRecordDecl(
        match_module_decls,
        forEach(
          fieldDecl(
            anyOf(hasType(builtinType()),
                  // hasType(arrayType(hasElementType(hasDeclaration(
                          // unless(cxxRecordDecl(isDerivedFrom(hasName("sc_signal_inout_if")))))
                        // )
                      // )
                    // ),
                   hasType(cxxRecordDecl(allOf(
                      unless(hasName("sc_in")), 
                      unless(hasName("sc_inout")),
                      unless(hasName("sc_out")), 
                      unless(hasName("sc_signal_inout_if")),
                      unless(hasName("sc_signal")),
                      unless(hasName("sc_stream_in")),
                      unless(hasName("sc_stream_out")))))))
            .bind("other_fields")));


    // auto match_non_sc_types = cxxRecordDecl(
        // match_module_decls,
        // forEach(fieldDecl(
            // anyOf(hasType(builtinType()),
                  // hasType(arrayType()),
                  // hasType(cxxRecordDecl(allOf(
                        // unless(match_all_ports),
                      // unless(hasName("sc_in")),
                      // unless(hasName("sc_inout")),
                      // unless(hasName("sc_out")),
                      // unless(hasName("sc_signal")),
                      // unless(hasName("sc_stream_in")),
                      // unless(hasName("sc_stream_out")))))))
            // .bind("other_fields")));
//
    auto match_non_sc_types_vdecl = cxxRecordDecl(forEach(
        varDecl(
            anyOf(hasType(builtinType()),
                  hasType(cxxRecordDecl(allOf(
                      unless(hasName("sc_in")), 
                      unless(hasName("sc_inout")),
                      unless(hasName("sc_out")), 
                      unless(hasName("sc_signal")),
                      unless(hasName("sc_signal_inout_if")),
                      unless(hasName("sc_stream_in")),
                      unless(hasName("sc_stream_out")))))))
            .bind("other_fields")));

    /* clang-format on */

    // Add matchers to finder.
    finder.addMatcher(match_all_ports, this);
    finder.addMatcher(match_non_sc_types, this);
    finder.addMatcher(match_non_sc_types_vdecl, this);
    finder.addMatcher(match_sc_ports, this);

    // This is only for testing.
    //
    // It is a way to show that we can write our own complex predicates for AST
    // matchers :)
    auto test_matcher =
        cxxRecordDecl(forEachDescendant(fieldDecl(matchesTypeName())));
    // finder.addMatcher(test_matcher, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    if (auto fd = checkMatch<FieldDecl>("sc_in_clk", result)) {
      std::string port_name{fd->getIdentifier()->getNameStart()};
      llvm::outs() << "Found sc_in_clk: " << port_name << "\n";
      insert_port(clock_ports_, fd);
    }

    if (auto fd = checkMatch<FieldDecl>("sc_in", result)) {
      auto port_name{fd->getIdentifier()->getNameStart()};
      llvm::outs() << " Found sc_in: " << port_name << "\n";
      insert_port(in_ports_, fd);
    }

    if (auto fd = checkMatch<FieldDecl>("sc_out", result)) {
      auto port_name{fd->getIdentifier()->getNameStart()};
      llvm::outs() << " Found sc_out: " << port_name << "\n";
      insert_port(out_ports_, fd);
    }

    if (auto fd = checkMatch<FieldDecl>("sc_inout", result)) {
      auto port_name{fd->getIdentifier()->getNameStart()};
      llvm::outs() << " Found sc_inout: " << port_name << "\n";
      insert_port(inout_ports_, fd);
    }

    if (auto fd = checkMatch<FieldDecl>("sc_signal", result)) {
      auto signal_name{fd->getIdentifier()->getNameStart()};
      llvm::outs() << " Found sc_signal: " << signal_name << "\n";
      insert_port(signal_fields_, fd);
    }

    // if (auto fd = checkMatch<FieldDecl>("other_fields", result)) {
    // auto field_name{fd->getIdentifier()->getNameStart()};
    // llvm::outs() << " Found other_fields: " << field_name << "\n";
    //     insert_port(other_fields_, fd);
    // }
    //
    if (auto fd = checkMatch<Decl>("other_fields", result)) {
      // These will be either FieldDecl or VarDecl.

      if (auto *p_field{dyn_cast<FieldDecl>(fd)}) {
        auto field_name{p_field->getIdentifier()->getNameStart()};
        llvm::outs() << " Found field other_fields: " << field_name << "\n";
        insert_port(other_fields_, p_field);

      } else {
        auto *p_var{dyn_cast<VarDecl>(fd)};
        auto field_name{p_var->getIdentifier()->getNameStart()};
        llvm::outs() << " Found var other_fields: " << field_name << "\n";
        insert_port(other_fields_, p_var);
      }
      // llvm::outs() << " Found field/vardecl other_fields: " << field_name <<
      // "\n"; insert_port(other_fields_, fd);
    }

    if (auto fd = checkMatch<FieldDecl>("sc_stream_in", result)) {
      auto field_name{fd->getIdentifier()->getNameStart()};
      llvm::outs() << " Found sc_stream_in: " << field_name << "\n";
      insert_port(instream_ports_, fd);
    }

    if (auto fd = checkMatch<FieldDecl>("sc_stream_out", result)) {
      auto field_name{fd->getIdentifier()->getNameStart()};
      llvm::outs() << " Found sc_stream_out: " << field_name << "\n";
      insert_port(outstream_ports_, fd);
    }

    if (auto fd = checkMatch<FieldDecl>("sc_port", result)) {
      auto field_name{fd->getIdentifier()->getNameStart()};
      llvm::outs() << " Found sc_port : " << field_name << "\n";
      insert_port(sc_ports_, fd);
    }
  }

  void dump() {
    printTemplateArguments(clock_ports_);
    printTemplateArguments(in_ports_);
    printTemplateArguments(out_ports_);
    printTemplateArguments(inout_ports_);
    printTemplateArguments(instream_ports_);
    printTemplateArguments(outstream_ports_);
    printTemplateArguments(signal_fields_);
    printTemplateArguments(other_fields_);
    printTemplateArguments(sc_ports_);
  }
};

///////////////////////////////////////////////////////////////////////////////
// Class ModuleDeclarationMatcher
//
//
///////////////////////////////////////////////////////////////////////////////
class ModuleDeclarationMatcher : public MatchFinder::MatchCallback {
  //
 public:
  typedef std::vector<std::tuple<std::string, CXXRecordDecl *> >
      ModuleDeclarationType;
  // Map to hold CXXREcordDecl to module declaration type name.
  typedef std::pair<CXXRecordDecl *, std::string> ModuleDeclarationPairType;
  typedef std::map<CXXRecordDecl *, std::string> ModuleDeclarationMapType;

  // typedef std::tuple<std::string, Decl *> InstanceDeclType;
  typedef std::vector<InstanceMatcher::InstanceDeclType> InstanceListType;
  typedef std::pair<CXXRecordDecl *, InstanceListType>
      DeclarationInstancePairType;
  typedef std::map<CXXRecordDecl *, InstanceListType>
      DeclarationsToInstancesMapType;

  // This will store all the modules as ModuleDecl
  // typedef ModuleDecl* ModulePairType;
  typedef std::pair<CXXRecordDecl *, ModuleDecl *> ModulePairType;
  typedef std::map<CXXRecordDecl *, ModuleDecl *> ModuleMapType;

 private:
  std::string top_module_decl_;
  ModuleDeclarationType found_declarations_;
  ModuleDeclarationType found_template_declarations_;
  // One of those needs to be removed.
  ModuleDeclarationType pruned_declarations_;
  ModuleDeclarationMapType pruned_declarations_map_;

  DeclarationsToInstancesMapType declaration_instance_map_;

  // This will store the pruned modules as pair of string, ModuleDecl*
  // The string will be the class name?
  ModuleMapType modules_;

  // Match nested instances
  InstanceMatcher instance_matcher;

  // Match ports
  // PortMatcher port_matcher_;

 public:
  // const PortMatcher &getPortMatcher() const { return port_matcher_; }

  const DeclarationsToInstancesMapType &getInstances() {
    return declaration_instance_map_;
  };

  void set_top_module_decl(const std::string &top) {
    // If there is no top specified, then match all, otherwise only top module.
    // The regular expression for matchesName() matcher for ".*" matches
    // anything. That regular expression is enabled only when top module is not
    // specified.
    if ((top == "!none") || (top == "")) {
      top_module_decl_ = ".*";
    } else {
      top_module_decl_ = top;
    }
  }

  // Register the matchers
  void registerMatchers(MatchFinder &finder) {
    /* clang-format off */

    // This is in case the set method is not called explicitly.
    // Simply pass in what is the default.
    set_top_module_decl( top_module_decl_ );

    auto match_module_decls = 
      cxxRecordDecl(
          matchesName(top_module_decl_),  // Specifies the top-level module name.
          hasDefinition(),            // There must be a definition.
          unless( isImplicit() ),     // Templates generate implicit structs - so ignore.
          isDerivedFrom(hasName("::sc_core::sc_module")),
          unless(isDerivedFrom(matchesName("sc_event_queue")))
          ).bind("sc_module");
    /* clang-format on */

    // add all the matchers.
    finder.addMatcher(match_module_decls, this);

    // add instance matcher
    instance_matcher.registerMatchers(finder);

    // add port (field) matcher
    // port_matcher_.registerMatchers(finder);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    if (auto decl = const_cast<CXXRecordDecl *>(
            result.Nodes.getNodeAs<CXXRecordDecl>("sc_module"))) {
      llvm::outs() << " Found sc_module: "
                   << decl->getIdentifier()->getNameStart()
                   << " CXXRecordDecl*: " << decl << "\n";
      std::string name{decl->getIdentifier()->getNameStart()};
      // decl->dump();
      //
      // TODO: Should we need this separation now?
      // It seems that we can simply store them whether they are template
      // specializations or not.
      // This is necessary because of the way clang represents them in their
      // AST.
      //
      if (isa<ClassTemplateSpecializationDecl>(decl)) {
        // llvm::outs() << "TEMPLATE SPECIAL\n";
        found_template_declarations_.push_back(std::make_tuple(name, decl));
      } else {
        // llvm::outs() << "NOT TEMPLATE SPECIAL\n";
        found_declarations_.push_back(std::make_tuple(name, decl));
      }

      // This is the new data structure that uses ModuleDecl internally.
      // Unpruned
      auto add_module{new ModuleDecl(name, decl)};
      modules_.insert(
          std::pair<CXXRecordDecl *, ModuleDecl *>(decl, add_module));

      // Instances should not be in subtree matching.
      //

      // Subtree matcher
      MatchFinder port_registry{};
      PortMatcher port_matcher{top_module_decl_};
      port_matcher.registerMatchers(port_registry);
      port_registry.match(*decl, *result.Context);
      port_matcher.dump();

      // All the ports for the CXXRecordDecl should be matched.
      // We can populate the ModuleDecl with that information.
      add_module->addPorts(port_matcher.getInputPorts(), "sc_in");
      // Clock ports are also sc_in
      add_module->addPorts(port_matcher.getClockPorts(), "sc_in");
      add_module->addPorts(port_matcher.getOutputPorts(), "sc_out");
      add_module->addPorts(port_matcher.getInOutPorts(), "sc_inout");
      add_module->addPorts(port_matcher.getOtherVars(), "others");
      add_module->addPorts(port_matcher.getSignals(), "sc_signal");
      add_module->addPorts(port_matcher.getInputStreamPorts(), "sc_stream_in");
      add_module->addPorts(port_matcher.getOutputStreamPorts(),
                           "sc_stream_out");

      // TODO: Add all ports (getPorts) that are derived from sc_port.
      // This was requested by Scott.  The matcher is already in place.
      //
    }
  }

  const ModuleMapType &getFoundModuleDeclarations() const { return modules_; }

  void removeModule(CXXRecordDecl *decl) {
    // Remove declarations from modules_
    // This is the pruning.

    auto remove_it{modules_.find(decl)};
    if (remove_it != modules_.end()) {
      modules_.erase(remove_it);
    }
  }

  void pruneMatches() {
    // Must have found instances.
    // 1. For every module found, check if there is an instance.
    // 2. If there is an instance, then add it into the list.

    for (auto const &element : found_declarations_) {
      auto decl{get<1>(element)};
      // std::llvm::outs() << "## fd  name: " << get<0>(element) << "\n ";
      InstanceListType instance_list;
      InstanceMatcher::InstanceDeclType instance;
      if (instance_matcher.findInstance(decl, instance)) {
        // pruned_declarations_.push_back(element);
        pruned_declarations_map_.insert(
            ModuleDeclarationPairType(decl, get<0>(element)));
        instance_list.push_back(instance);
        declaration_instance_map_.insert(
            DeclarationInstancePairType(decl, instance_list));
      } else {
        // Remove it from modules_
        removeModule(decl);
      }
    }

    for (auto const &element : found_template_declarations_) {
      auto decl{get<1>(element)};
      // std::llvm::outs() << "## ftd name: " << get<0>(element) << "\n ";
      InstanceListType instance_list;
      InstanceMatcher::InstanceDeclType instance;
      if (instance_matcher.findInstance(decl, instance)) {
        // pruned_declarations_.push_back(element);
        pruned_declarations_map_.insert(
            ModuleDeclarationPairType(decl, get<0>(element)));
        instance_list.push_back(instance);
        declaration_instance_map_.insert(
            DeclarationInstancePairType(decl, instance_list));
      } else {
        removeModule(decl);
      }
    }
  }

  void dump() {
    llvm::outs() << "[DBG] Top-level module: " << top_module_decl_ << "\n";
    llvm::outs() << "[DBG] Non-template module declarations: "
                 << found_declarations_.size() << "\n";
    for (const auto &i : found_declarations_) {
      llvm::outs() << "module name         : " << get<0>(i) << ", " << get<1>(i)
                   << "\n";
    }

    llvm::outs() << "[DBG] Template module declarations: "
                 << found_template_declarations_.size() << "\n";
    for (const auto &i : found_template_declarations_) {
      llvm::outs() << "template module name: " << get<0>(i) << ", " << get<1>(i)
                   << "\n";
    }

    llvm::outs() << "[DBG] Pruned declaration Map: "
                 << pruned_declarations_map_.size() << "\n";
    for (const auto &i : pruned_declarations_map_) {
      auto decl{i.first};
      auto decl_name{i.second};
      llvm::outs() << "CXXRecordDecl* " << i.first
                   << ", module name: " << decl_name << "\n";
    }

    llvm::outs() << "[DBG] Module declarations: " << modules_.size() << "\n";
    for (const auto &i : modules_) {
      auto cxx_decl{i.first};
      // TODO: really awkward
      auto module_decl{i.second};
      auto decl_name{module_decl->getName()};

      llvm::outs() << "CXXRecordDecl* " << cxx_decl
                   << ", module name: " << decl_name << "\n";
      module_decl->dump(llvm::outs());
    }

    // Print the instances.
    instance_matcher.dump();

    llvm::outs() << "\n[DBG] Dump map of decl->instances: "
                 << declaration_instance_map_.size() << "\n";

    for (const auto &i : declaration_instance_map_) {
      auto decl{i.first};
      auto instance_list{i.second};

      llvm::outs() << "decl: " << decl->getIdentifier()->getNameStart();
      for (const auto &instance : instance_list) {
        llvm::outs() << ", instance type: " << get<0>(instance) << ",   "
                     << get<1>(instance) << "\n";
      }
    }
  }
};

};  // namespace sc_ast_matchers

#endif
