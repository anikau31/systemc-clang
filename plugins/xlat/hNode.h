#include "llvm/Support/raw_ostream.h"

#include <stack>

using namespace scpar;

namespace hnode {
  class hNode;

  typedef hNode * hNodep;

  class hNode {

#define HNODEen \
  etype(hNoop), \
  etype(hModule), \
  etype(hProcesses), \
  etype(hProcess), \
  etype(hCStmt), \
    etype(hPortsigvarlist), \
  etype(hPortin), \
  etype(hPortout), \
  etype(hPortio), \
  etype(hSenslist), \
  etype(hSensvar), \
  etype(hSensedge), \
  etype(hTypeinfo), \
  etype(hType), \
  etype(hInt), \
  etype(hSigdecl), \
  etype(hVardecl), \
  etype(hVarref), \
  etype(hSigAssignL), \
  etype(hSigAssignR), \
  etype(hVarAssign), \
  etype(hBinop), \
  etype(hUnop), \
  etype(hIfStmt), \
  etype(hForStmt), \
  etype(hForInit), \
  etype(hForCond), \
  etype(hForInc), \
  etype(hForBody), \
  etype(hWhileStmt),				\
  etype(hLiteral), \
  etype(hUnimpl), \
  etype(hLast)


  public:

#define etype(x) x
 
    typedef enum { HNODEen } hdlopsEnum;

    //bool is_leaf;
    
    string h_name;
    hdlopsEnum h_op;
    list<hNodep> child_list;
 
#undef etype
#define etype(x) #x

    const string hdlop_pn [hLast+1]  =  { HNODEen };

    //hNode() { is_leaf = true;}
    hNode(bool lf) {
      //is_leaf = lf;
      h_op = hdlopsEnum::hNoop;
      h_name = "";
    }

    hNode(hdlopsEnum h) {
      h_op = h;
      h_name = "";
    }
       
  
    hNode(string s, hdlopsEnum h) {
      //is_leaf = true;
      h_op = h;
      h_name = s;
    }

    ~hNode() {
      //return;
      if (!child_list.empty()) {
	list<hNodep>::iterator it;
	for (it = child_list.begin(); it != child_list.end(); it++) {
	  if (*it)
	    cout << "child list element " << *it << "\n";
	  if (*it) delete *it;
	}
      }
      else cout << printname(h_op) << " '" << h_name << "' NOLIST\n";
      //cout << "visited hNode destructor\n";
	    
    }

    void set( hdlopsEnum h, string s = "") {
      h_op = h;
      h_name = s;
    }
    
    string printname(hdlopsEnum opc) {
      return hdlop_pn[static_cast<int>(opc)];
    }

    // for completeness
    hdlopsEnum str2hdlopenum(string st) {
      const int n = sizeof (hdlop_pn)/sizeof (hdlop_pn[0]);
      for (int i = 0; i < n; i++) {
	if (hdlop_pn[i] == st)
	  return (hdlopsEnum) i;
      }
      return hLast;
    }
    void print(llvm::raw_fd_ostream & modelout, unsigned int indnt=2) {
      modelout.indent(indnt);
      modelout << printname(h_op) << " ";
      if (h_name == "")
	modelout << " NONAME";
      else modelout << h_name;
      if (child_list.empty())
	modelout << " NOLIST\n";
      else {
	modelout << " [\n";
	for (auto child : child_list)
	  if (child)
	    child->print(modelout, indnt+2);
	  else {
	    modelout.indent(indnt+2);
	    modelout << "<null child>\n";
	  }
	modelout.indent(indnt);
	modelout << "]\n";
      }
    }

  };
}
