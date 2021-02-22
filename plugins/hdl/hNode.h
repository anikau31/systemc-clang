#ifndef _HNODE_H_
#define _HNODE_H_


#include "llvm/Support/raw_ostream.h"

#include <stack>
#include <algorithm>

using namespace systemc_clang;

//!
//! The hNode class defines the language-neutral HDL opcodes describing
//! systemc constructs and more generally statements and expression that
//! may occur in an SC_MODULE or SC_METHOD. Accessor and print methods
//! are provided.
//!
namespace hnode {

  class hNode;

  typedef hNode * hNodep;

  class hNode {

#define HNODEen \
  etype(hNoop), \
  etype(hModule), \
  etype(hModinitblock), \
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
  etype(hTypeField), \
  etype(hTypedef), \
  etype(hTypeTemplateParam), \
  etype(hInt), \
  etype(hSigdecl), \
  etype(hVardecl), \
  etype(hVardeclrn), \
  etype(hModdecl), \
  etype(hVarref), \
  etype(hVarInit), \
  etype(hSigAssignL), \
  etype(hSigAssignR), \
  etype(hVarAssign), \
  etype(hBinop), \
  etype(hUnop), \
  etype(hCondop), \
  etype(hMethodCall), \
  etype(hIfStmt), \
  etype(hForStmt), \
  etype(hSwitchStmt), \
  etype(hSwitchCase), \
  etype(hSwitchDefault), \
  etype(hWhileStmt),   	\
  etype(hReturnStmt),  	\
  etype(hLiteral), \
  etype(hFunction), \
  etype(hFunctionRetType), \
  etype(hFunctionParams), \
  etype(hFunctionParamI), \
  etype(hFunctionParamIO), \
  etype(hUnimpl), \
  etype(hLast)


  public:

#define etype(x) x
 
    typedef enum { HNODEen } hdlopsEnum;

    //bool is_leaf;
    
    string h_name;
    hdlopsEnum h_op;
    //list<hNodep> child_list;
    std::vector<hNodep> child_list;
 
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
	//list<hNodep>::iterator it;
	vector<hNodep>::iterator it;
	for (it = child_list.begin(); it != child_list.end(); it++) {
	  /* if (*it) */
	  /*   cout << "child list element " << *it << "\n"; */
	  if (*it) delete *it;
	}
      }
    }

    void set( hdlopsEnum h, string s = "") {
      h_op = h;
      h_name = s;
    }

    void set(string s = "") {
      h_name = s;
    }
    
    string printopc(hdlopsEnum opc) {
      return hdlop_pn[static_cast<int>(opc)];
    }

    string getname() {
      return h_name;
    }

    hdlopsEnum getopc() {
      return h_op;
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
    //void print(llvm::raw_fd_ostream & modelout, unsigned int indnt=2) {
    void print(llvm::raw_ostream & modelout=llvm::outs(), unsigned int indnt=2) {
      modelout.indent(indnt);
      modelout << printopc(h_op) << " ";
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

      // default arguments don't work in lldb
    void dumphcode() {
      print(llvm::outs(), 2);
    }
  
  };

  //!
  //! The util class provides small utility functions to generate and
  //! recognize C++ and SystemC conformant identifiers.
  //!
  
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
      string strings[] = {"sc_in", "sc_rvd", "sc_out", "sc_rvd", "sc_inout",
			  "sc_signal", "sc_subref", "sc_dt"};
      for (string onestring : strings) {
	if (tstring.find(onestring)!=string::npos)
	  return true;
      }
      return false;
      /* if (tstring.substr(0, 6) == "class ") // this is so stupid */
      /* 	tstring = tstring.substr(6, tstring.length() - 6); */
      /* //return (tstring.find("sc_in")!=string::npos)  */
      /* return ((tstring.substr(0, 12) == "sc_core::sc_") || */
      /* 	      (tstring.substr(0, 5) == "sc_in") || */
      /* 	      (tstring.substr(0, 9) == "sc_rvd_in") || */
      /* 	      (tstring.substr(0, 6) == "sc_out") || */
      /* 	      (tstring.substr(0, 10) == "sc_rvd_out") || */
      /* 	      (tstring.substr(0, 8) == "sc_inout") || */
      /* 	      (tstring.substr(0, 9) == "sc_signal") || */
      /* 	      (tstring.substr(0, 6) == "sc_rvd") || */
      /* 	      (tstring.substr(0, 9) == "sc_subref") || */
      /* 	      (tstring.substr(0,5) == "sc_dt")); */
    }

    static inline bool isposint(const std::string &str) {
      // https://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c
      // towards the middle
      return !str.empty() && str.find_first_not_of("0123456789") == string::npos;
    }

  };

  typedef struct {
    string oldn;
    string newn;
    hNodep h_vardeclp;
  } names_t;

 typedef std::map<Decl *, names_t> hdecl_name_map_t;
 typedef std::map<ModuleInstance *, names_t> hmodinst_name_map_t;
 
  class name_serve {
  private:
    int cnt;
    string prefix;
  public:
  name_serve(string prefx="_local_") : prefix(prefx), cnt(0){ }
    string newname() {
      return (prefix+to_string(cnt++));
    }
  };
    
} // end namespace hnode

#endif
