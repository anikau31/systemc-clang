#ifndef _NETLIST_GEN_H_
#define _NETLIST_GEN_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <map>
namespace scpar {

  using namespace clang; 
  using namespace std;
  class NetlistGen: public RecursiveASTVisitor<NetlistGen> {

  public:
    NetlistGen(FunctionDecl*, llvm::raw_ostream&);
    ~NetlistGen();

		typedef pair<string, string> instanceModulePairType;
		typedef map <string, string> instanceModuleMapType;

		typedef pair <string, string> signalPortPairType;
		typedef pair <signalPortPairType, string> signalPortTypePairType;
		typedef map <signalPortPairType, string> signalPortTypeMapType;
		
		typedef pair<instanceModulePairType, signalPortPairType> netlistPairType;
		typedef multimap<instanceModulePairType, signalPortPairType> netlistMapType;

		virtual bool VisitMemberExpr(MemberExpr *me);
    virtual bool VisitImplicitCastExpr(ImplicitCastExpr *ice);

    void dump();    

		instanceModuleMapType getInstanceModuleMap();
		signalPortTypeMapType getSignalPortTypeMap();
		netlistMapType getNetlistMap();

	private:

    llvm::raw_ostream& _os;
		instanceModuleMapType _instanceModuleMap;
		signalPortTypeMapType _signalPortTypeMap;
		netlistMapType _netlistMap;
		string _portName;
    string _moduleInstance;
    string _moduleName;
    string _signalName;
    string _portType;

  };
}

#endif
