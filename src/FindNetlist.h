#ifndef _NETLIST_GEN_H_
#define _NETLIST_GEN_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <map>
namespace scpar {

	using namespace clang;
	using namespace std;
  
	class FindNetlist : public RecursiveASTVisitor < FindNetlist > {
	public:
	  // Removed Model::moduleMapType from constructor
   // Have to think of a way to include building 
   // netlist from module constructors. Right now I am not 
   // doing it.	
   FindNetlist(FunctionDecl *);
	 FindNetlist(const FindNetlist &);
   virtual ~FindNetlist();

  public: 
  typedef pair <string, string> instanceModulePairType;
  typedef map <string, string> instanceModuleMapType;

  typedef pair <string, string> portSignalPairType;
  typedef map <string, string> portSignalMapType;

  typedef pair<string, portSignalMapType> instancePortSignalPairType;
  typedef map<string, portSignalMapType> instancePortSignalMapType;

  typedef pair<string, vector<string> > instanceListModulePairType;
  typedef map <string, vector<string> > instanceListModuleMapType;

  virtual bool VisitCXXOperatorCallExpr (CXXOperatorCallExpr *ce); 
  
  void updateInstanceListModuleMap(string, string);

  instanceModuleMapType getInstanceModuleMap();
  instancePortSignalMapType getInstancePortSignalMap();
  instanceListModuleMapType getInstanceListModuleMap(); 
	portSignalMapType getPortSignalMap();
  int getNumInstances(string);
  void dump ();

	private:  
    int _pass;     
    instanceModuleMapType _instanceModuleMap;
    portSignalMapType _portSignalMap;
    instancePortSignalMapType _instancePortSignalMap;
    instanceListModuleMapType _instanceListModuleMap;
 };
}
#endif
