#ifndef _HDLTYPE_H_
#define _HDLTYPE_H_

#include "SystemCClang.h"
#include "Tree.h"
#include <tuple>
#include <unordered_map>

#include "hNode.h"

using namespace clang;
using namespace systemc_clang;

using namespace hnode;

class HDLType {
public:
  HDLType() {}
  ~HDLType() {}
  typedef std::unordered_map<string, QualType> usertype_map_t;
  
  void SCtype2hcode(string prefix, Tree<TemplateType> *template_argtp,
                    std::vector<llvm::APInt> *arr_sizes, hNode::hdlopsEnum h_op,
                    hNodep &h_info);
  void generatetype(
      systemc_clang::TreeNode<systemc_clang::TemplateType> *const &node,
      systemc_clang::Tree<systemc_clang::TemplateType> *const &treehead,
      hNodep &h_info, bool generate_initial_htype = true);
  hNodep addtype(string typname, QualType qtyp, ASTContext &astcontext);
  void addfieldtype(const FieldDecl *fld, hNodep &h_typdef);
  usertype_map_t usertypes;

private:
  util tutil;
};
#endif
