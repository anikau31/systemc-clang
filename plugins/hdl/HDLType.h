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
  typedef std::unordered_map<const clang::Type *, string> userrectype_map_t;

  typedef struct {
    // map from generated name to qual type
    usertype_map_t usertypes;
    // map from type * to generated name; used for user-defined method calls
    // as first parameter to the method; only populated for record types
    userrectype_map_t userrectypes;
  } usertype_info_t;
  
  void SCtype2hcode(string prefix, Tree<TemplateType> *template_argtp,
                    std::vector<llvm::APInt> *arr_sizes, hNode::hdlopsEnum h_op,
                    hNodep &h_info);
  void generatetype(
      systemc_clang::TreeNode<systemc_clang::TemplateType> *const &node,
      systemc_clang::Tree<systemc_clang::TemplateType> *const &treehead,
      hNodep &h_info, bool generate_initial_htype = true);
  bool checkusertype(systemc_clang::TreeNode<systemc_clang::TemplateType> *const &node,
		     systemc_clang::Tree<systemc_clang::TemplateType> *const &treehead, string &tmps);
  hNodep addtype(string typname, QualType qtyp, ASTContext &astcontext);
  void addfieldtype(const FieldDecl *fld, hNodep &h_typdef);

  //usertype_map_t usertypes;
  usertype_info_t usertype_info;
  void print(llvm::raw_ostream & modelout=llvm::outs(), unsigned int indnt=2) {
    modelout << "Userrectypes follows\n";
    for( auto entry:usertype_info.userrectypes) {
      modelout << entry.first << " " <<entry.second << "\n";
    }
    modelout << "Userrectypes end\n";
  }

  // used in debugging due to lldb limitations
  void print(){
    llvm::dbgs() << "Userrectypes follows\n";
    for( auto entry:usertype_info.userrectypes) {
      llvm::dbgs()  << entry.first << " " <<entry.second << "\n";
    }
    llvm::dbgs()  << "Userrectypes end\n";
  }
private:
  util tutil;
};
#endif
