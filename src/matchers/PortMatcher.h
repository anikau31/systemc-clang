//===- PortMatcher.h - Matching Ports -------------------------*- C++ -*-=====//
//
// Part of the systemc-clang project.
// See License.rst
//
//===----------------------------------------------------------------------===//
//
/// \file
/// Parses ports of a SystemC module
//
/// \author Hiren Patel
//===----------------------------------------------------------------------===//

#ifndef _PORT_MATCHER_H_
#define _PORT_MATCHER_H_

#include <vector>
#include "ModuleDecl.h"
#include "PortDecl.h"

#include "clang/ASTMatchers/ASTMatchers.h"

/// Different matchers may use different DEBUG_TYPE
#undef DEBUG_TYPE
#define DEBUG_TYPE "PortMatcher"

using namespace clang::ast_matchers;

namespace sc_ast_matchers {

///////////////////////////////////////////////////////////////////////////////
//
/// Class PortMatcher
//
/// This class identifies a SystemC module's port declaration, and nested
/// submodules as well.
///
///////////////////////////////////////////////////////////////////////////////

class PortMatcher : public MatchFinder::MatchCallback {
 public:
  // typedef std::pair<std::string, ModuleDecl*> ModulePairType;
  // typedef std::vector<ModulePairType> ModuleMapType;

  /// A vector of tuples that holds the name of the port, and a pointer to
  /// PortDecl.
  typedef std::vector<std::tuple<std::string, PortDecl *> > MemberDeclType;

 private:
  //std::string top_module_decl_;

 public:
  /// Separate out the member declarations found within a SystemC module.
  MemberDeclType clock_ports_;
  MemberDeclType in_ports_;
  MemberDeclType out_ports_;
  MemberDeclType inout_ports_;
  MemberDeclType other_fields_;
  MemberDeclType signal_fields_;
  MemberDeclType instream_ports_;
  MemberDeclType outstream_ports_;
  MemberDeclType sc_ports_;

  /// Store the declaration of submodules.
  MemberDeclType submodules_;

 public:
  const MemberDeclType &getClockPorts() const { return clock_ports_; }
  const MemberDeclType &getInputPorts() const { return in_ports_; }
  const MemberDeclType &getOutputPorts() const { return out_ports_; }
  const MemberDeclType &getInOutPorts() const { return inout_ports_; }
  const MemberDeclType &getOtherVars() const { return other_fields_; }
  const MemberDeclType &getSignals() const { return signal_fields_; }
  const MemberDeclType &getSubmodules() const { return submodules_; }
  const MemberDeclType &getInputStreamPorts() const { return instream_ports_; }
  const MemberDeclType &getOutputStreamPorts() const {
    return outstream_ports_;
  }
  const MemberDeclType &getPorts() const { return sc_ports_; }

  PortMatcher() {}
      //: top_module_decl_{top_module} {}

  /// AST matcher to detect field declarations.
  auto makeFieldMatcher(const std::string &name) {
    /* clang-format off */

    /// The generic field matcher has the following conditions.
    /// * It is in the main provided file (not in the library files).
    /// * It is derived from sc_module
    /// * For each child that is a FieldDecl, 
    ///   - The type of it is a C++ class whose class name is "name"
  return  cxxRecordDecl(
      isExpansionInMainFile(),
      isDerivedFrom(hasName("::sc_core::sc_module")),
      forEach(
        fieldDecl(hasType(
            cxxRecordDecl(hasName(name))
            ) // hasType
          ).bind(name) // fieldDecl
        )
      );
  }

  /// This is a matcher to identify sc_signal.
  /// The reason for this matcher is for it to match arrays of sc_signals as well.
  /// The conditions are as follows:
  /// * It must be a FieldDecl
  ///   - It must have a type that is either an array whose type is a c++ class derived 
  ///     from a class name called "name"
  ///   - Or, it is has a type that is a c++ class that is derived from class name "name".
  auto makeSignalArrayType(const std::string &name) {
    return //hasType(
         arrayType(
           hasElementType(hasDeclaration(
               cxxRecordDecl(isDerivedFrom(hasName(name))).bind("desugar_"+name)
               )
             )
           ).bind("array_type");
         //);
  }

  auto signalMatcher(const std::string &name) {
  return anyOf(
          hasType(makeSignalArrayType(name))
          ,
          // 2-d
          hasType(arrayType(hasElementType(makeSignalArrayType(name) ))) 
          ,
          // 3-d
          hasType(arrayType(hasElementType(arrayType(hasElementType(makeSignalArrayType(name)) ))))
          ,
          // Regular field declaration
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

  auto makeArrayTypeMatcher(const std::string &name) {
    return   recordType(hasDeclaration(cxxRecordDecl(hasName(name)).bind("desugar_"+name)));
   
  }
  /// This is a matcher for sc_port.
  /// It has the following conditions:
  /// * It must be a FieldDecl
  ///   - It has a type that is an array whose type has a name "name"
  ///   - Or, it has a type that is a C++ class whose class name is "name".
  ///
  /// I'm not sure how to do 1d,2d,3d array matching other than the way it is 
  /// done. The key idea is to see that there is an arrayType() within an arrayType(), 
  /// and so on.
  auto portNameMatcher(const std::string &name) {
      return 
          anyOf(
            hasType(
              arrayType(

                anyOf(

                  // 1-d
                hasElementType(hasUnqualifiedDesugaredType(
                    recordType(
                      hasDeclaration(
                        cxxRecordDecl(hasName(name)).bind("desugar_"+name)
                      )
                    )
                  )
                )//hasElementType

                ,

                // 2-d
                hasElementType(hasUnqualifiedDesugaredType(
                    arrayType(hasElementType(hasUnqualifiedDesugaredType(
                          makeArrayTypeMatcher(name)
                          ))
                      )//arrayType
                  )
                )//hasElementType

                  ,
                //3-d

                hasElementType(hasUnqualifiedDesugaredType(
                    arrayType(hasElementType(hasUnqualifiedDesugaredType(
                          arrayType(hasElementType(hasUnqualifiedDesugaredType(
                            makeArrayTypeMatcher(name)
                          ))
                            )//arrayType
                          ))
                      )//arrayType
                  )
                )//hasElementType


                )//anyOf
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

  /// This is a matcher for sc_in_clk since it uses a NamedDecl.
  /// It has the following conditions:
  /// * It must be a FieldDecl,
  ///  - It has a type that is an array whose type has a name "name".
  ///  - Or, it has a type that is a NamedDecl whose name is "name".
  ///
  auto makePortHasNamedDeclNameMatcher(const std::string &name) {
    return 
      fieldDecl(
          anyOf(
            hasType(arrayType(hasElementType(asString(name)))),
            hasType(namedDecl(hasName(name)))
          )
      );
  }

  auto makeMemberIsSubModule() {
    return
      fieldDecl(
          hasType(hasUnqualifiedDesugaredType(
              recordType(
                hasDeclaration(
                  cxxRecordDecl(
                    isDerivedFrom(hasName("::sc_core::sc_module")),
                    unless(isDerivedFrom(matchesName("sc_event_queue")))
                    ).bind("submodule")
                  ) //hasDeclaration
                ) //recordType
              ) //hasUnqualifiedDesugaredType
            ) //hasType
          ).bind("submodule_fd"); // fieldDecl;

  }
  /* clang-format on */

  template <typename NodeType>
  auto checkMatch(const std::string &name,
                  const MatchFinder::MatchResult &result) {
    return result.Nodes.getNodeAs<NodeType>(name);
  }

  void printTemplateArguments(MemberDeclType &found_ports) {
    // Input ports
    for (const auto &i : found_ports) {
      LLVM_DEBUG(llvm::dbgs() << "name: " << get<0>(i) << ", FieldDecl*: "
                              << get<1>(i)->getAsFieldDecl());
      LLVM_DEBUG(
          get<1>(i)->getTemplateType()->printTemplateArguments(llvm::outs()));
      LLVM_DEBUG(llvm::dbgs() << "\n");
    }
  }

  template <typename T>
  auto parseTemplateType(const T *fd) {
    clang::QualType qual_type{fd->getType()};
    const clang::Type *type_ptr{qual_type.getTypePtr()};
    auto template_ptr{new FindTemplateTypes()};
    template_ptr->Enumerate(type_ptr);
    LLVM_DEBUG(template_ptr->printTemplateArguments(llvm::outs()));
    return template_ptr;
  }

  template <typename T>
  void insert_port(MemberDeclType &port, T *decl, bool isFieldDecl = true) {
    // port is a map entry [CXXRecordDecl* => vector<PortDecl*>]

    std::string name{};
    if (auto *fd = dyn_cast<clang::FieldDecl>(decl)) {
      name = fd->getIdentifier()->getNameStart();

      PortDecl *new_pd{new PortDecl(name, decl, parseTemplateType(fd))};

      clang::QualType field_type{fd->getType()};

      // Need to extract all the array index arguments.
      /// Cast it to see if it's array type.
      auto array_type{dyn_cast<ConstantArrayType>(field_type)};

      if (array_type) {
        new_pd->setArrayType();
        while (array_type != nullptr) {
          llvm::APInt array_size{};
          array_size = array_type->getSize();
          LLVM_DEBUG(llvm::dbgs() << "Size of array: " << array_size << "\n";);
          array_type =
              dyn_cast<ConstantArrayType>(array_type->getElementType());

          new_pd->addArraySize(array_size);
        }
      }

      auto port_entry{std::make_tuple(name, new_pd)};
      port.push_back(port_entry);

    } else {

      if (auto *vd = dyn_cast<clang::VarDecl>(decl)) {
        name = vd->getIdentifier()->getNameStart();
        PortDecl *new_pd{new PortDecl(name, decl, parseTemplateType(vd))};

        auto field_type{vd->getType()};
        /// Cast it to see if it's array type.
        auto array_type{dyn_cast<ConstantArrayType>(field_type)};
        if (array_type) {
          new_pd->setArrayType();
          while (array_type != nullptr) {
            llvm::APInt array_size{};
            array_size = array_type->getSize();
            LLVM_DEBUG(llvm::dbgs()
                           << "Size of array: " << array_size << "\n";);
            array_type =
                dyn_cast<ConstantArrayType>(array_type->getElementType());

            new_pd->addArraySize(array_size);
          }
        }

        auto port_entry{std::make_tuple(name, new_pd)};
        port.push_back(port_entry);
      }
    }
  }

  void registerMatchers(MatchFinder &finder) {
    /* clang-format off */

    //
    /// Matchers for compositions.
    /// These matchers are used to form other matchers. 
    auto match_module_decls = 
      cxxRecordDecl(
          //matchesName(top_module_decl_), // Specifies the top-level module name.
          hasDefinition(),               // There must be a definition.
          unless( isImplicit() ),        // Templates generate implicit structs - so ignore.
          isDerivedFrom(
            hasName("::sc_core::sc_module") 
            ),
          unless(isDerivedFrom(matchesName("sc_event_queue")))
      );      


     /// Matches all the SystemC Ports.
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
              unless(portNameMatcher("sc_rvd_out")),
              unless(makeMemberIsSubModule())
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
              unless(portNameMatcher("sc_rvd_out")),
              unless(makeMemberIsSubModule())
     
            ).bind("other_fvdecl")));

    auto match_submodules = cxxRecordDecl(
        forEach(
          makeMemberIsSubModule()
          )
        );
    /* clang-format on */

    // Add matchers to finder.
    finder.addMatcher(match_all_ports, this);
    finder.addMatcher(match_non_sc_types_fdecl, this);
    finder.addMatcher(match_non_sc_types_vdecl, this);
    finder.addMatcher(match_submodules, this);
    // finder.addMatcher(match_sc_ports, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    auto sc_in_field{checkMatch<clang::CXXRecordDecl>("desugar_sc_in", result)};
    auto sc_out_field{
        checkMatch<clang::CXXRecordDecl>("desugar_sc_out", result)};
    auto sc_inout_field{
        checkMatch<clang::CXXRecordDecl>("desugar_sc_inout", result)};
    auto sc_signal_field{
        checkMatch<clang::CXXRecordDecl>("desugar_sc_signal_inout_if", result)};
    auto sc_stream_in_field{
        checkMatch<clang::CXXRecordDecl>("desugar_sc_stream_in", result)};
    auto sc_stream_out_field{
        checkMatch<clang::CXXRecordDecl>("desugar_sc_stream_out", result)};
    auto sc_rvd_in_field{
        checkMatch<clang::CXXRecordDecl>("desugar_sc_rvd_in", result)};
    auto sc_rvd_out_field{
        checkMatch<clang::CXXRecordDecl>("desugar_sc_rvd_out", result)};
    auto sc_port_field{
        checkMatch<clang::CXXRecordDecl>("desugar_sc_port", result)};
    auto other_fields{checkMatch<clang::Decl>("other_fields", result)};
    auto other_fvdecl{checkMatch<clang::Decl>("other_fvdecl", result)};

    /// Array types
    // auto array_type{checkMatch<clang::ArrayType>("array_type", result)};

    /// Submodules
    auto submodule_fd{checkMatch<clang::FieldDecl>("submodule_fd", result)};

    if (submodule_fd) {
      auto name{submodule_fd->getNameAsString()};
      LLVM_DEBUG(llvm::dbgs() << "Found submodule: " << name << "\n");
      insert_port(submodules_, submodule_fd);
    }

    if (sc_in_field && other_fields) {
      if (auto *p_field{dyn_cast<clang::FieldDecl>(other_fields)}) {
        auto fd = p_field;
        auto port_name{fd->getIdentifier()->getNameStart()};
        LLVM_DEBUG(llvm::dbgs() << " Found sc_in: " << port_name << "\n");
        insert_port(in_ports_, fd);
      }
    }

    if (sc_out_field && other_fields) {
      if (auto *p_field{dyn_cast<clang::FieldDecl>(other_fields)}) {
        auto fd = p_field;
        auto port_name{fd->getIdentifier()->getNameStart()};
        LLVM_DEBUG(llvm::dbgs() << " Found sc_out: " << port_name << "\n");

        insert_port(out_ports_, fd);
      }
    }

    if (sc_inout_field && other_fields) {
      if (auto *p_field{dyn_cast<clang::FieldDecl>(other_fields)}) {
        auto fd = p_field;
        auto port_name{fd->getIdentifier()->getNameStart()};
        LLVM_DEBUG(llvm::dbgs() << " Found sc_inout: " << port_name << "\n");
        insert_port(inout_ports_, fd);
      }
    }

    if (sc_signal_field && other_fields) {
      if (auto *p_field{dyn_cast<clang::FieldDecl>(other_fields)}) {
        auto fd = p_field;
        auto signal_name{fd->getIdentifier()->getNameStart()};
        llvm::dbgs() << " Found sc_signal: " << signal_name << "\n";
        insert_port(signal_fields_, fd);
      }
    }

    if (sc_stream_in_field && other_fields) {
      if (auto *p_field{dyn_cast<clang::FieldDecl>(other_fields)}) {
        auto fd = p_field;
        auto field_name{fd->getIdentifier()->getNameStart()};
        LLVM_DEBUG(llvm::dbgs()
                   << " Found sc_stream_in: " << field_name << "\n");
        insert_port(instream_ports_, fd);
      }
    }

    if (sc_stream_out_field && other_fields) {
      if (auto *p_field{dyn_cast<clang::FieldDecl>(other_fields)}) {
        auto fd = p_field;
        auto field_name{fd->getIdentifier()->getNameStart()};
        LLVM_DEBUG(llvm::dbgs()
                   << " Found sc_stream_out: " << field_name << "\n");
        insert_port(outstream_ports_, fd);
      }
    }

    if (sc_rvd_in_field && other_fields) {
      if (auto *p_field{dyn_cast<clang::FieldDecl>(other_fields)}) {
        auto fd = p_field;
        auto field_name{fd->getIdentifier()->getNameStart()};
        LLVM_DEBUG(llvm::dbgs() << " Found sc_rvd_in: " << field_name << "\n");
        insert_port(instream_ports_, fd);
      }
    }

    if (sc_rvd_out_field && other_fields) {
      if (auto *p_field{dyn_cast<clang::FieldDecl>(other_fields)}) {
        auto fd = p_field;
        auto field_name{fd->getIdentifier()->getNameStart()};
        LLVM_DEBUG(llvm::dbgs() << " Found sc_rvd_out: " << field_name << "\n");
        insert_port(outstream_ports_, fd);
      }
    }

    if (sc_port_field && other_fields) {
      if (auto *p_field{dyn_cast<clang::FieldDecl>(other_fields)}) {
        auto fd = p_field;
        auto field_name{fd->getIdentifier()->getNameStart()};
        LLVM_DEBUG(llvm::dbgs() << " Found sc_port : " << field_name << "\n");
        insert_port(sc_ports_, fd);
      }
    }

    auto is_ports{(sc_in_field) || sc_out_field || sc_inout_field ||
                  sc_signal_field || sc_stream_in_field || sc_stream_out_field};

    /// Not a submodule and not a port
    if ((!is_ports)) {
      // These will be either FieldDecl or VarDecl.
      auto fd{other_fvdecl};
      if (fd) {
        LLVM_DEBUG(llvm::dbgs() << "Print out the other fd\n");

        if (auto *p_field{dyn_cast<clang::FieldDecl>(fd)}) {
          auto field_name{p_field->getIdentifier()->getNameStart()};
          LLVM_DEBUG(llvm::dbgs()
                     << " Found field other_fields: " << field_name << "\n");

          insert_port(other_fields_, p_field);

        } else {
          auto *p_var{dyn_cast<clang::VarDecl>(fd)};
          auto field_name{p_var->getIdentifier()->getNameStart()};
          LLVM_DEBUG(llvm::dbgs()
                     << " Found var other_fields: " << field_name << "\n");
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
    LLVM_DEBUG(llvm::dbgs() << "Identified submodules\n");
    printTemplateArguments(submodules_);
  }
};
};  // namespace sc_ast_matchers

#endif
