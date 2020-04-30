#ifndef _PORT_MATCHER_H_
#define _PORT_MATCHER_H_

#include <vector>
#include "ModuleDecl.h"
#include "PortDecl.h"

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"

using namespace clang::ast_matchers;
using namespace scpar;



namespace sc_ast_matchers {

///////////////////////////////////////////////////////////////////////////////
// Class PortMatcher
//
//
///////////////////////////////////////////////////////////////////////////////

class PortMatcher : public MatchFinder::MatchCallback {
 public:
  typedef std::pair<string, ModuleDecl *> ModulePairType;
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
      isDerivedFrom(hasName("::sc_core::sc_module")),
      forEach(
        fieldDecl(hasType(
            cxxRecordDecl(hasName(name)))).bind(name)
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
    return hasType(
         arrayType(
           hasElementType(hasDeclaration(
               cxxRecordDecl(isDerivedFrom(hasName(name))).bind("desugar_"+name)
               )
             )
           )
         );
  }

  auto signalMatcher(const std::string &name) {
  return  anyOf(
          makeArrayType(name),
          hasType(
            cxxRecordDecl(isDerivedFrom(hasName(name))).bind("desugar_"+name)
            )
          );
  }

  auto makeSignalMatcher(const std::string &name) {
    return fieldDecl(
        signalMatcher(name)
          ).bind("other_fields");
  }

  // This is a matcher for sc_port.
  // It has the following conditions:
  // * It must be a FieldDecl
  //   - It has a type that is an array whose type has a name "name"
  //   - Or, it has a type that is a C++ class whose class name is "name".
  //
  auto portNameMatcher(const std::string &name) {
      return 
          anyOf(
            hasType(
              arrayType(
                hasElementType(hasUnqualifiedDesugaredType(
                    recordType(
                      hasDeclaration(
                        cxxRecordDecl(hasName(name)).bind("desugar_"+name)
                      )
                    )
                  )
                )
              )
            ),
            hasType(hasUnqualifiedDesugaredType(
                recordType(
                  hasDeclaration(
                    cxxRecordDecl(hasName(name)).bind("desugar_"+name)
                  )
                )
              )
            )
          );
  }

  auto makePortHasNameMatcher(const std::string &name) {
    return fieldDecl(
        portNameMatcher(name)
        ).bind("other_fields");
  }

  // This is a matcher for sc_in_clk since it uses a NamedDecl.
  // It has the following conditions:
  // * It must be a FieldDecl,
  //  - It has a type that is an array whose type has a name "name".
  //  - Or, it has a type that is a NamedDecl whose name is "name".
  //
  auto makePortHasNamedDeclNameMatcher(const std::string &name) {
    return 
      fieldDecl(
          anyOf(
            hasType(arrayType(hasElementType(asString(name)))),
            hasType(namedDecl(hasName(name)))
          )
      );
  }

  /* clang-format on */

  template <typename NodeType>
auto checkMatch(const std::string &name,
                const MatchFinder::MatchResult &result) {
  return result.Nodes.getNodeAs<NodeType>(name);
}


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

     auto match_all_ports = cxxRecordDecl(
         match_module_decls,
         eachOf(
           forEach(
             makePortHasNamedDeclNameMatcher("sc_in_clk")),
           forEach(
               makePortHasNameMatcher("sc_in")),
           forEach( 
               makePortHasNameMatcher("sc_out")),
           forEach(
               makePortHasNameMatcher("sc_inout")),
           forEach(
             makeSignalMatcher("sc_signal_inout_if")),
           forEach(
               makePortHasNameMatcher("sc_stream_in")),
           forEach(
               makePortHasNameMatcher("sc_rvd_in")),
           forEach(
               makePortHasNameMatcher("sc_rvd_out")),
           forEach(
               makePortHasNameMatcher("sc_stream_out"))
           )
               );
     // Notice that the ports can also be arrays.
     
     auto match_non_sc_types_fdecl = 
      cxxRecordDecl(
      match_module_decls,
      //unless(match_all_ports),
          forEach(fieldDecl(
              unless(portNameMatcher("sc_in_clk")),
              unless(portNameMatcher("sc_in")),
              unless(portNameMatcher("sc_out")),
              unless(portNameMatcher("sc_inout")),
              unless(portNameMatcher("sc_stream_in")),
              unless(portNameMatcher("sc_stream_out")),
              unless(portNameMatcher("sc_rvd_in")),
              unless(portNameMatcher("sc_rvd_out")),
              unless(signalMatcher("sc_signal_inout_if")),
              unless(portNameMatcher("sc_rvd_in")),
              unless(portNameMatcher("sc_rvd_out"))
              ).bind("other_fvdecl")));

    auto match_non_sc_types_vdecl = cxxRecordDecl(
        forEach(
          varDecl(
          unless(portNameMatcher("sc_in_clk")),
              unless(portNameMatcher("sc_in")),
              unless(portNameMatcher("sc_out")),
              unless(portNameMatcher("sc_inout")),
              unless(portNameMatcher("sc_stream_in")),
              unless(portNameMatcher("sc_stream_out")),
              unless(portNameMatcher("sc_rvd_in")),
              unless(portNameMatcher("sc_rvd_out")),
              unless(signalMatcher("sc_signal_inout_if")),
              unless(portNameMatcher("sc_rvd_in")),
              unless(portNameMatcher("sc_rvd_out"))
     
            ).bind("other_fvdecl")));

    /* clang-format on */

    // Add matchers to finder.
    finder.addMatcher(match_all_ports, this);
    finder.addMatcher(match_non_sc_types_fdecl, this);
    finder.addMatcher(match_non_sc_types_vdecl, this);
    // finder.addMatcher(match_sc_ports, this);

    // This is only for testing.
    //
    // It is a way to show that we can write our own complex predicates for AST
    // matchers :)
    //auto test_matcher =
        //cxxRecordDecl(forEachDescendant(fieldDecl(matchesTypeName())));
    // finder.addMatcher(test_matcher, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    auto sc_in_field{checkMatch<CXXRecordDecl>("desugar_sc_in", result)};
    auto sc_out_field{checkMatch<CXXRecordDecl>("desugar_sc_out", result)};
    auto sc_inout_field{checkMatch<CXXRecordDecl>("desugar_sc_inout", result)};
    auto sc_signal_field{
        checkMatch<CXXRecordDecl>("desugar_sc_signal_inout_if", result)};
    auto sc_stream_in_field{
        checkMatch<CXXRecordDecl>("desugar_sc_stream_in", result)};
    auto sc_stream_out_field{
        checkMatch<CXXRecordDecl>("desugar_sc_stream_out", result)};
    auto sc_rvd_in_field{
        checkMatch<CXXRecordDecl>("desugar_sc_rvd_in", result)};
    auto sc_rvd_out_field{
        checkMatch<CXXRecordDecl>("desugar_sc_rvd_out", result)};
    auto sc_port_field{checkMatch<CXXRecordDecl>("desugar_sc_port", result)};
    auto other_fields{checkMatch<Decl>("other_fields", result)};
    auto other_fvdecl{checkMatch<Decl>("other_fvdecl", result)};

    // llvm::outs() << "\n";
    // llvm::outs() << "in: " << sc_in_field << ", out: " << sc_out_field
    // << ", inout: " << sc_inout_field << ", other: " << other_fields
    // << "\n";
    //
    if (sc_in_field && other_fields) {
      if (auto *p_field{dyn_cast<FieldDecl>(other_fields)}) {
        auto fd = p_field;
        auto port_name{fd->getIdentifier()->getNameStart()};
        llvm::outs() << " Found sc_in: " << port_name << "\n";
        insert_port(in_ports_, fd);
      }
    }

    if (sc_out_field && other_fields) {
      if (auto *p_field{dyn_cast<FieldDecl>(other_fields)}) {
        auto fd = p_field;
        auto port_name{fd->getIdentifier()->getNameStart()};
        llvm::outs() << " Found sc_out: " << port_name << "\n";
        insert_port(out_ports_, fd);
      }
    }

    if (sc_inout_field && other_fields) {
      if (auto *p_field{dyn_cast<FieldDecl>(other_fields)}) {
        auto fd = p_field;
        auto port_name{fd->getIdentifier()->getNameStart()};
        llvm::outs() << " Found sc_inout: " << port_name << "\n";
        insert_port(inout_ports_, fd);
      }
    }

    if (sc_signal_field && other_fields) {
      if (auto *p_field{dyn_cast<FieldDecl>(other_fields)}) {
        auto fd = p_field;
        auto signal_name{fd->getIdentifier()->getNameStart()};
        llvm::outs() << " Found sc_signal: " << signal_name << "\n";
        insert_port(signal_fields_, fd);
      }
    }

    if (sc_stream_in_field && other_fields) {
      if (auto *p_field{dyn_cast<FieldDecl>(other_fields)}) {
        auto fd = p_field;
        auto field_name{fd->getIdentifier()->getNameStart()};
        llvm::outs() << " Found sc_stream_in: " << field_name << "\n";
        insert_port(instream_ports_, fd);
      }
    }

    if (sc_stream_out_field && other_fields) {
      if (auto *p_field{dyn_cast<FieldDecl>(other_fields)}) {
        auto fd = p_field;
        auto field_name{fd->getIdentifier()->getNameStart()};
        llvm::outs() << " Found sc_stream_out: " << field_name << "\n";
        insert_port(outstream_ports_, fd);
      }
    }

    if (sc_rvd_in_field && other_fields) {
      if (auto *p_field{dyn_cast<FieldDecl>(other_fields)}) {
        auto fd = p_field;
        auto field_name{fd->getIdentifier()->getNameStart()};
        llvm::outs() << " Found sc_rvd_in: " << field_name << "\n";
        insert_port(instream_ports_, fd);
      }
    }

    if (sc_rvd_out_field && other_fields) {
      if (auto *p_field{dyn_cast<FieldDecl>(other_fields)}) {
        auto fd = p_field;
        auto field_name{fd->getIdentifier()->getNameStart()};
        llvm::outs() << " Found sc_rvd_out: " << field_name << "\n";
        insert_port(outstream_ports_, fd);
      }
    }

    if (sc_port_field && other_fields) {
      if (auto *p_field{dyn_cast<FieldDecl>(other_fields)}) {
        auto fd = p_field;
        auto field_name{fd->getIdentifier()->getNameStart()};
        llvm::outs() << " Found sc_port : " << field_name << "\n";
        insert_port(sc_ports_, fd);
      }
    }

    auto is_ports{(sc_in_field) || sc_out_field || sc_inout_field ||
                  sc_signal_field || sc_stream_in_field || sc_stream_out_field};
    // llvm::outs() << "is_ports: " << is_ports << "\n";

    if ((!is_ports)) {
      // These will be either FieldDecl or VarDecl.
      auto fd{other_fvdecl};
      if (fd) {
        llvm::outs() << "Print out the other fd\n";

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
      }
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
}; // namespace

#endif 

