#include "HDLType.h"
#include "SensitivityMatcher.h"
#include "SystemCClang.h"
#include "TemplateParametersMatcher.h"
#include "Tree.h"
#include "clang/AST/DeclCXX.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include <ctype.h>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

#include <iostream>
#include <assert.h>

#undef DEBUG_TYPE
#define DEBUG_TYPE "HDL"

//!
//! Resolve types of systemc variables, signals, and ports
//! including templated types and generate primitive type declarations
//! Relies on TemplateTree matcher provided by front end and can
//! handle structured types.
//!

void HDLType::SCtype2hcode(string prefix, Tree<TemplateType> *template_argtp,
                           std::vector<llvm::APInt> *arr_sizes,
                           hNode::hdlopsEnum h_op, hNodep &h_info) {

  LLVM_DEBUG(llvm::dbgs() << "prefix "<< prefix << " HDLtype dump of templatetree args follows\n");
  LLVM_DEBUG(template_argtp->dump(llvm::dbgs()) ; );
  LLVM_DEBUG(llvm::dbgs() << "as string: " << template_argtp->dft() << "\n");
  
  if (!(template_argtp && (template_argtp->getRoot()))) {
    LLVM_DEBUG(llvm::dbgs() << "HDLtype no root prefix is " << prefix << " "
                            << template_argtp << "\n");

    return;
  }
  hNodep hmainp = new hNode(prefix, h_op); // opPort|Sig|Var prefix
  h_info->child_list.push_back(hmainp);
  string tmps;
  if (auto etype = dyn_cast<EnumType>(((template_argtp->getRoot())->getDataPtr())->getTypePtr()) )  {
    tmps = "int";
  }
  else tmps = ((template_argtp->getRoot())->getDataPtr())->getTypeName();
  if ((((template_argtp->getRoot())->getDataPtr())->getTypePtr())->isBuiltinType())
    tutil.make_ident(tmps); 
  hNodep h_typeinfo = new hNode(hNode::hdlopsEnum::hTypeinfo);
  hmainp->child_list.push_back(h_typeinfo);
  
  hNodep h_typ = new hNode(tmps, hNode::hdlopsEnum::hType);
  if ((arr_sizes) && (arr_sizes->size() > 0)) {
    string arr_string = "array";
    int arr_size = 0;
    for (int i = 0; i < arr_sizes->size(); i++) {
      arr_size = arr_sizes->at(i).getLimitedValue();
      arr_string += "##" + to_string(arr_size);
    }
    hNodep h_arr = new hNode(arr_string, hNode::hdlopsEnum::hType);
    h_arr->child_list.push_back(h_typ);
    h_typeinfo->child_list.push_back(h_arr);
  } else {
    h_typeinfo->child_list.push_back(h_typ);
  }

  if ((((template_argtp->getRoot())->getDataPtr())->getTypePtr())->isBuiltinType()) return;
  auto const vectreeptr{template_argtp->getChildren(template_argtp->getRoot())};
  if ((vectreeptr.size() == 0) && ((template_argtp->getRoot())->getDataPtr())->getTypePtr()->isStructureType())
    {
      LLVM_DEBUG(llvm::dbgs() << "root node has no children, is structure type, type dump follows\n");
      LLVM_DEBUG(((template_argtp->getRoot())->getDataPtr())->getTypePtr()->dump() ; );
      generatetype(template_argtp->getRoot(), template_argtp, h_typ, false);  // skip initial hType, already generated
      return;
    }

   if (checkusertype(template_argtp->getRoot(), template_argtp, tmps)) {
    h_typ->set(tmps);
    return; // we are not pushing into the template tree parameters for user defined types since template specialization record is saved
  }

  // template arguments seem to be stored in reverse order
  for (int i = vectreeptr.size() - 1; i >= 0; i--) {
    // for (auto const &node : vectreeptr) {
    // generatetype(node, template_argtp, h_typ);
    generatetype(vectreeptr[i], template_argtp, h_typ);
  }
  return;
}

void HDLType::generatetype(
    systemc_clang::TreeNode<systemc_clang::TemplateType> *const &node,
    systemc_clang::Tree<systemc_clang::TemplateType> *const &treehead,
    hNodep &h_info, bool generate_initial_htype) {

  
  string tmps = (node->getDataPtr())->getTypeName();
  if (((node->getDataPtr())->getTypePtr())->isBuiltinType())
    tutil.make_ident(tmps);
  // LLVM_DEBUG(llvm::dbgs() << "generatetype node name is " << tmps << " type
  // follows\n"); (node->getDataPtr())->getTypePtr()->dump(llvm::dbgs());

  LLVM_DEBUG(llvm::dbgs() << "generatetype node name is " << tmps << " as string is " << node->toString() << "\n");
  hNodep nodetyp;
  if (generate_initial_htype) {
    nodetyp =
      new hNode(tmps, tutil.isposint(tmps) ? hNode::hdlopsEnum::hLiteral
		: hNode::hdlopsEnum::hType);
    h_info->child_list.push_back(nodetyp);
  }
  else nodetyp = h_info; // the type node was already generated
  if (((node->getDataPtr())->getTypePtr())->isBuiltinType())
    return;
 
  if (checkusertype(node, treehead, tmps)) {
    nodetyp->set(tmps);
    return; // we are not pushing into the template tree parameters for user defined types since template specialization record is saved
  }
  auto const vectreeptr{treehead->getChildren(node)};
  // template arguments seem to be stored in reverse order
  for (int i = vectreeptr.size() - 1; i >= 0; i--) {
    // for (auto const &chnode : vectreeptr) {
    // generatetype(chnode, treehead, nodetyp);
    generatetype(vectreeptr[i], treehead, nodetyp);
  }
}

bool HDLType::checkusertype(systemc_clang::TreeNode<systemc_clang::TemplateType> *const &node,
			      systemc_clang::Tree<systemc_clang::TemplateType> *const &treehead, string &tmps)
{
  bool retval = false;
  const Type *typ = node->getDataPtr()->getTypePtr();
  // ========================== CHECK HDLType =====================
  // FIXME: Cleanup
  bool t1 = tutil.isSCType(tmps) || tutil.isSCBuiltinType(tmps);
  bool t2 = tutil.isSCByType(typ);

  if (t1 != t2) {
    llvm::dbgs() << "### CHECK1: old " << t1 << " != new " << t2 << "\n";
    // comment out to see hcode output. assert(0); //llvm::dbgs() << t1/0;
    //std::cin.get();
  }
  // ========================== END CHECK =====================
  //
  //if (!(tutil.isSCByType(typ) ||
  if (!(tutil.isSCType(tmps) || tutil.isSCBuiltinType(tmps) ||
        tutil.isposint(tmps) || tutil.isTypename(tmps))) {
    string tmps_full = treehead->dft();
    size_t type_ix = tmps_full.find(tmps);
    if (type_ix != string::npos) { // found the name in the template args
      tmps = tmps_full.substr(type_ix);
      tutil.make_ident(tmps);
      retval = true;
    }
    LLVM_DEBUG(llvm::dbgs() << "user defined type full " << tmps+ " " + tmps_full << "\n"); // here build specialized type
    const RecordType *tstp =
        dyn_cast<RecordType>((node->getDataPtr())->getTypePtr());
    if (tstp) {
      LLVM_DEBUG(llvm::dbgs() << "generatetype found record type and type pointer from RecordType is " << tstp << "\n");
      // RecordDecl *   tstdp = (tstp->getDecl())->getDefinition();

      usertype_info.usertypes[tmps] =
          ((tstp->getDecl())->getTypeForDecl())->getCanonicalTypeInternal();
      usertype_info.userrectypes[tstp] = tmps;  // reverse map from RecType * to generated name
    }
    else usertype_info.usertypes[tmps] =
        ((node->getDataPtr())->getTypePtr())->getCanonicalTypeInternal();
   }
   return retval;

}

hNodep HDLType::addtype(string typname, QualType qtyp, ASTContext &astcontext) {
  hNodep h_typdef = new hNode(typname, hNode::hdlopsEnum::hTypedef);
  LLVM_DEBUG(llvm::dbgs() << "addtype entered with type name " << typname
                          << "\n");
  const Type *typ = qtyp.getTypePtr();
  LLVM_DEBUG( typ->dump(llvm::dbgs(), astcontext ); );
  if (typ->isBuiltinType()) {
    string tmps = qtyp.getAsString();
    tutil.make_ident(tmps);
    hNodep hprim = new hNode(tmps, hNode::hdlopsEnum::hType);
    LLVM_DEBUG(llvm::dbgs() << "addtype found prim type " << tmps << "\n");
    h_typdef->child_list.push_back(hprim);
    return h_typdef;
  }

  if (const RecordType *rectype = dyn_cast<RecordType>(typ)) {
    LLVM_DEBUG(llvm::dbgs()
               << "addtype record type found, name is " << typname << " rectype ptr is " << rectype << "\n");
    if (isa<ClassTemplateSpecializationDecl>(rectype->getDecl())) {
      LLVM_DEBUG(llvm::dbgs()
                 << "addtype isa template specialzation decl found, name is "
                 << typname << "\n");
      ClassTemplateSpecializationDecl *ctsd =
          dyn_cast<ClassTemplateSpecializationDecl>(rectype->getDecl());
      ClassTemplateDecl *ctd = ctsd->getSpecializedTemplate();
      LLVM_DEBUG(ctd->dump(llvm::dbgs()));
      LLVM_DEBUG(llvm::dbgs() << "####### ============================== "
                                 "MATCHER ========================= ##### \n");
      TemplateParametersMatcher template_matcher{};
      MatchFinder matchRegistry{};
      template_matcher.registerMatchers(matchRegistry);
      matchRegistry.match(*ctsd, astcontext); // ctd for user defined type, ctsd for actual
      LLVM_DEBUG(llvm::dbgs() << "####### ============================== END "
                                 "MATCHER ========================= ##### \n");

      TemplateParameterList *tpl = ctd->getTemplateParameters();
      LLVM_DEBUG(llvm::dbgs() << "addtype here are template parameters\n");
      for (auto param : *tpl) {
        LLVM_DEBUG(llvm::dbgs() << "addtype template param name is "
                                << param->getName() << "\n");
         param->dump(llvm::dbgs());
	 LLVM_DEBUG(llvm::dbgs() << "end dump of param\n");
        //h_typdef->child_list.push_back(
	//  new hNode(param->getName().str(), hNode::hdlopsEnum::hTypeTemplateParam));
      }
      
      std::vector<const FieldDecl *> fields;
      template_matcher.getArgFields(fields); //  Parm for formal, use getArgFields for actual
      if (fields.size() > 0) {
        for (const FieldDecl *fld : fields) {
          addfieldtype(fld, h_typdef);
        }
      }
    } else if (!rectype->getDecl()->field_empty()) {
      for (auto const &fld : rectype->getDecl()->fields()) {
        addfieldtype(fld, h_typdef);
      }
    } else { // record type but no fields
      // get the type name
      LLVM_DEBUG(llvm::dbgs() << "Found record with no fields, name is "
                              << (rectype->getDecl())->getName() << "\n");
      h_typdef->child_list.push_back(
          new hNode(rectype->getDecl()->getName().str(), hNode::hdlopsEnum::hType));
    }
  }

  return h_typdef;
}

void HDLType::addfieldtype(const FieldDecl *fld, hNodep &h_typdef) {
  LLVM_DEBUG(llvm::dbgs() << "field of record type \n");
  LLVM_DEBUG(fld->dump(llvm::dbgs()));
  LLVM_DEBUG(llvm::dbgs() << "field: found name " << fld->getName() << "\n");
  // Try to get the template type of these fields.
  const Type *field_type{fld->getType().getTypePtr()};
  FindTemplateTypes find_tt{};
  find_tt.Enumerate(field_type);

  // Get the tree.
  auto template_args{find_tt.getTemplateArgTreePtr()};
  hNodep hfld =
      new hNode(fld->getNameAsString(), hNode::hdlopsEnum::hTypeField);
  h_typdef->child_list.push_back(hfld);
  LLVM_DEBUG(llvm::dbgs() << "calling generatetype with template args of field\n");
  if (template_args->getRoot())
    generatetype(template_args->getRoot(), template_args, hfld);
  else
    LLVM_DEBUG(llvm::dbgs() << "FindTemplateTypes returned null root\n");
}
