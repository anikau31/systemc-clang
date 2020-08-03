#ifndef _XLATTYPE_H_
#define _XLATTYPE_H_

#include <tuple>
#include "SystemCClang.h"
#include "Tree.h"

#include "hNode.h"

using namespace clang;
using namespace systemc_clang;

using namespace hnode;

class XlatType {
 public:
 XlatType() { }
  ~XlatType() {}

  void xlattype(string prefix,  Tree<TemplateType> *template_argtp,
		hNode::hdlopsEnum h_op, hNodep &h_info);
  void generatetype(systemc_clang::TreeNode<systemc_clang::TemplateType > * const &node,
		    systemc_clang::Tree<systemc_clang::TemplateType > * const &treehead, hNodep &h_info);
  hNodep addtype(string typname, QualType qtyp, ASTContext &astcontext);
  void addfieldtype(const FieldDecl * fld, hNodep &h_typdef);
  std::unordered_map<string, QualType > usertypes;
 private:
  util tutil;
};
#endif
