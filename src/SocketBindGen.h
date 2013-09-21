#ifndef _SOCKET_BIND_GEN_H_
#define _SOCKET_BIND_GEN_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <map>
namespace scpar {

  using namespace clang; 
  using namespace std;
  class SocketBindGen: public RecursiveASTVisitor<SocketBindGen> {

  public:
    SocketBindGen(FunctionDecl*, llvm::raw_ostream&);
    ~SocketBindGen();

		typedef pair<string, string> initiatorTargetInstancePairType;
		typedef map <string, string> initiatorTargetInstanceMapType;

		typedef pair<string, string> initiatorTargetSocketPairType;
		typedef multimap <string, string> initiatorTargetSocketMapType;

		typedef pair<string, string> initiatorTargetPairType;
		typedef map<string, string> initiatorTargetMapType;

		typedef pair<initiatorTargetSocketPairType, initiatorTargetPairType> 
		bindPairType;
		typedef map <initiatorTargetSocketPairType, initiatorTargetPairType> 
		bindMapType;
		
		virtual bool VisitMemberExpr(MemberExpr *me);

    void dump();    

	private:

    llvm::raw_ostream& _os;
		string _initiatorSocket;
    string _targetSocket;
    string _initiatorModule;
    string _targetModule;
		string _initiatorInstance;
		string _targetInstance;
		initiatorTargetSocketMapType _initiatorTargetSocketMap;
		initiatorTargetInstanceMapType _initiatorTargetInstanceMap;
		initiatorTargetMapType _initiatorTargetMap;
		bindMapType _bindMap;
		int pass;
  };
}

#endif
