#ifndef _XLATTYPE_H_
#define _XLATTYPE_H_

#include <tuple>
#include "SystemCClang.h"
#include "Tree.h"

#include "hNode.h"

using namespace clang;
using namespace scpar;

using namespace hnode;

class XlatType {
 public:
 XlatType() { }
  ~XlatType() {}

  void xlattype(string prefix,  Tree<TemplateType> *template_argtp,
		hNode::hdlopsEnum h_op, hNodep &h_info);
  void generatetype(scpar::TreeNode<scpar::TemplateType > * const &node,
		    scpar::Tree<scpar::TemplateType > * const &treehead, hNodep &h_info);
  hNodep addtype(string typname, QualType qtyp, ASTContext &astcontext);
  void addfieldtype(const FieldDecl * fld, hNodep &h_typdef);
  std::unordered_map<string, QualType > usertypes;
 private:
  util tutil;
};
#endif
