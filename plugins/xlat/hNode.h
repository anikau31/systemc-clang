#ifndef _HNODE_H_
#define _HNODE_H_


#include "llvm/Support/raw_ostream.h"

#include <stack>
#include <algorithm>

using namespace scpar;

namespace hnode {
  class hNode;

  typedef hNode * hNodep;

  class hNode {

#define HNODEen \
  etype(hNoop), \
  etype(hModule), \
  etype(hPortbindings), \
  etype(hPortbinding), \
  etype(hProcesses), \
  etype(hProcess), \
  etype(hMethod), \
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
  etype(hVarInit), \
  etype(hSigAssignL), \
  etype(hSigAssignR), \
  etype(hVarAssign), \
  etype(hBinop), \
  etype(hUnop), \
  etype(hMethodCall), \
  etype(hIfStmt), \
  etype(hBreakStmt), \
  etype(hForStmt), \
  etype(hSwitchStmt), \
  etype(hSwitchCase), \
  etype(hSwitchDefault), \
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
	  /* if (*it) */
	  /*   cout << "child list element " << *it << "\n"; */
	  if (*it) delete *it;
	}
      }
      //else cout << printname(h_op) << " '" << h_name << "' NOLIST\n";
      //cout << "visited hNode destructor\n";
	    
    }

    void set( hdlopsEnum h, string s = "") {
      h_op = h;
      h_name = s;
    }

    void set(string s = "") {
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
  
  class util { 
  public:
    const  int numstr = 6;
     const string scbuiltintype [6] = {
      "sc_uint",
      "sc_int",
      "sc_bigint",
      "sc_biguint",
      "sc_bv",
      "sc_logic"
    };
    int scbtlen [ 6 ];
    
    util() {
      for (int i=0; i < numstr; i++)
	scbtlen[i] = scbuiltintype[i].length();
    }
    ~util() {}
    
    static inline void make_ident(string &nm) {
      // https://stackoverflow.com/questions/14475462/remove-set-of-characters-from-the-string
      //str.erase(
      // std::remove_if(str.begin(), str.end(), [](char chr){ return chr == '&' || chr == ' ';}),
      //str.end());
      std::replace(nm.begin(), nm.end(), ' ', '_');
      nm.erase(std::remove_if(nm.begin(), nm.end(),
			      [](char c){ return c!='_' && !isalnum(c) ;}), nm.end());

    }
     inline bool isSCBuiltinType(string tstring){
      // linear search sorry, but at least the length
      // isn't hard coded in ...
      for (int i=0; i < numstr; i++) {
	if (tstring.substr(0, scbtlen[i]) == scbuiltintype[i])
	  return true;
      }
      return false;
    }
    
    static inline bool isSCType(string tstring) {
      // linear search and the length is hard coded in ...
      // used in the method name logic
      if (tstring.substr(0, 6) == "class ") // this is so stupid
	tstring = tstring.substr(6, tstring.length() - 6);
      return ((tstring.substr(0, 12) == "sc_core::sc_") ||
	      (tstring.substr(0, 5) == "sc_in") ||
	      (tstring.substr(0, 6) == "sc_out") ||
	      (tstring.substr(0, 8) == "sc_inout") ||
	      (tstring.substr(0, 9) == "sc_signal") ||
	      (tstring.substr(0,5) == "sc_dt"));
    }

  };
} // end namespace hnode

#endif
