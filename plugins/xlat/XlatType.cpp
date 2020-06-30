#include <vector>
#include <string>
#include <unordered_set>
#include <ctype.h>
#include <tuple>
#include "clang/AST/DeclCXX.h"
#include "SystemCClang.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/StringRef.h"
#include "Tree.h"
#include "TemplateParametersMatcher.h"
#include "SensitivityMatcher.h"
#include "XlatType.h"

void XlatType::xlattype(string prefix,  Tree<TemplateType> *template_argtp,
		     hNode::hdlopsEnum h_op, hNodep &h_info) {

  //llvm::outs()  << "xlattype dump of templatetree args follows\n";
  //template_argtp->dump();

    if (!(template_argtp &&  (template_argtp->getRoot()))) {
      llvm::errs() << "xlattype no root prefix is " << prefix << " " << template_argtp << "\n";;

    return;
    }										 
  hNodep hmainp = new hNode(prefix, h_op); // opPort|Sig|Var prefix
  h_info->child_list.push_back(hmainp);
  string tmps = ((template_argtp->getRoot())->getDataPtr())->getTypeName();
  hNodep h_typeinfo = new hNode(hNode::hdlopsEnum::hTypeinfo);
  hmainp->child_list.push_back(h_typeinfo);
  hNodep h_typ = new hNode(tmps, hNode::hdlopsEnum::hType);
  h_typeinfo->child_list.push_back(h_typ);
  
  auto const vectreeptr{ template_argtp->getChildren(template_argtp->getRoot())};
  // template arguments seem to be stored in reverse order
  for (int i=vectreeptr.size()-1; i>=0; i--) {
    //for (auto const &node : vectreeptr) {
    //generatetype(node, template_argtp, h_typ);
    generatetype(vectreeptr[i], template_argtp, h_typ);
  }
  return;
}

void XlatType::generatetype(scpar::TreeNode<scpar::TemplateType > * const &node,
			scpar::Tree<scpar::TemplateType > * const &treehead, hNodep &h_info) {

  string tmps = (node->getDataPtr())->getTypeName();
  llvm::errs() << "generatetype node name is " << tmps << "type follows\n";
  (node->getDataPtr())->getTypePtr()->dump(llvm::errs());				 
  hNodep nodetyp = new hNode (tmps, hNode::hdlopsEnum::hType);
  h_info->child_list.push_back(nodetyp);
  if (((node->getDataPtr())->getTypePtr())->isBuiltinType())
     return;
  if (!(tutil.isSCType(tmps) || tutil.isSCBuiltinType(tmps) || tutil.isposint(tmps)))
    {
      llvm::errs() << "adding user defined type " << tmps << "\n";
      const RecordType * tstp = dyn_cast<RecordType>((node->getDataPtr())->getTypePtr());
      if (tstp) {
	//llvm::errs() << "generatetype found record type, dump of definition follows\n";
	llvm::errs() << "generatetype found record type\n";
	//RecordDecl *   tstdp = (tstp->getDecl())->getDefinition();
	//tstdp->dump(llvm::errs());
	usertypes[tmps] = ((tstp->getDecl())->getTypeForDecl())->getCanonicalTypeInternal();
      }
      usertypes[tmps] = ((node->getDataPtr())->getTypePtr())->getCanonicalTypeInternal();
    }
  auto const vectreeptr{ treehead->getChildren(node)};
  // template arguments seem to be stored in reverse order
  for (int i=vectreeptr.size()-1; i>=0; i--) {
  //for (auto const &chnode : vectreeptr) {
    //generatetype(chnode, treehead, nodetyp);
    generatetype(vectreeptr[i], treehead, nodetyp);
  }					       

}

hNodep XlatType::addtype(string typname, QualType qtyp, ASTContext &astcontext) {
  hNodep h_typdef = new hNode(typname, hNode::hdlopsEnum::hTypedef);
  llvm::errs() << "addtype entered with type name " << typname << "\n";
  const Type * typ = qtyp.getTypePtr();
  if (typ->isBuiltinType())
    {
      hNodep hprim = new hNode(qtyp.getAsString(), hNode::hdlopsEnum::hType);
      llvm::errs() << "addtype found prim type " << qtyp.getAsString() << "\n";
      h_typdef->child_list.push_back(hprim);
      return h_typdef;
    }

  if (const RecordType * rectype = dyn_cast<RecordType>(typ)) {
    llvm::errs() << "addtype record type found, name is " << typname << "\n";
    if ( isa<ClassTemplateSpecializationDecl>(rectype->getDecl())) {
      llvm::errs() << "addtype isa template specialzation decl found, name is " << typname << "\n";
      ClassTemplateSpecializationDecl * ctsd = dyn_cast<ClassTemplateSpecializationDecl>(rectype->getDecl());
      ClassTemplateDecl * ctd = ctsd->getSpecializedTemplate();
      ctd->dump(llvm::errs());
      llvm::errs() << "####### ============================== MATCHER ========================= ##### \n";
      TemplateParametersMatcher template_matcher{};
      MatchFinder matchRegistry{};
      template_matcher.registerMatchers(matchRegistry);
      matchRegistry.match(*ctd, astcontext); //getContext());
      llvm::errs() << "####### ============================== END MATCHER ========================= ##### \n";
      

      TemplateParameterList * tpl = ctd->getTemplateParameters();
      llvm::errs() << "addtype her are template parameters\n";
      for (auto param : *tpl) {
	llvm::errs() << "addtype template param name is " << param->getName() << "\n";
	//param->dump(llvm::errs());
	h_typdef->child_list.push_back(new hNode(param->getName(), hNode::hdlopsEnum::hTypeTemplateParam));
      }
      std::vector<const FieldDecl *> fields;
      template_matcher.getFields(fields);
      if (fields.size()>0) {
	for (const FieldDecl *fld : fields) {
	  addfieldtype(fld, h_typdef);
	}
      }
    }
    else if (!rectype->getDecl()->field_empty()) {
	  for (auto const &fld: rectype->getDecl()->fields()) {
	    addfieldtype(fld, h_typdef);
	   }
	}
	else { // record type but no fields
	  // get the type name
	  llvm::errs() << "Found record with no fields, name is " << (rectype->getDecl())->getName() << "\n"; 
	  h_typdef->child_list.push_back(new hNode(rectype->getDecl()->getName(), hNode::hdlopsEnum::hType));
	      
	}
  }

  return h_typdef; 
}

void XlatType::addfieldtype(const FieldDecl * fld, hNodep &h_typdef) {
  llvm::errs() << "field of record type \n";
  fld ->dump(llvm::errs());
  llvm::errs() << "field: found name " << fld->getName() << "\n";
  // Try to get the template type of these fields.
  const Type *field_type{fld->getType().getTypePtr()};
  FindTemplateTypes find_tt{};
  find_tt.Enumerate(field_type);

  // Get the tree.
  auto template_args{find_tt.getTemplateArgTreePtr()};
  hNodep hfld = new hNode(fld->getNameAsString(), hNode::hdlopsEnum::hTypeField);
  h_typdef->child_list.push_back(hfld);
  llvm::errs() << "calling generatetype with template args of field\n";
  if (template_args->getRoot()) 
    generatetype(template_args->getRoot(), template_args, hfld);
  else llvm::errs() << "FindTemplateTypes returned null root\n";

}
