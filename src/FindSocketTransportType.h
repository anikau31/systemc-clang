#ifndef _FIND_SOCKET_TRANSPORT_TYPE_H_
#define _FIND_SOCKET_TRANSPORT_TYPE_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <map>
namespace scpar {
	using namespace clang;
	using namespace std;

	///////////////////////////////////////////////////////////////////////////////////
	class Properties {
		public:
			Properties(string, string, string, string);
			Properties(string, string, string);
			~Properties();

			void dump(raw_ostream&);

		private:
			string _transportType;
			string _delay;
			string _phase;
			string _payloadName;
	};

	///////////////////////////////////////////////////////////////////////////////////
	class FindSocketTransportType: public RecursiveASTVisitor<FindSocketTransportType> {
		
		public:		
			FindSocketTransportType(CXXMethodDecl*, llvm::raw_ostream&);
			~FindSocketTransportType();

			typedef pair<string, Properties*> socketTransportPairType;
			typedef multimap<string, Properties*> socketTransportMapType;
						
			virtual bool VisitCXXMemberCallExpr (CXXMemberCallExpr*);
			socketTransportMapType returnSocketTransportMap();
			
			void dump();

		private:	
			CXXMethodDecl *_d;
			llvm::raw_ostream& _os;
			socketTransportMapType _socketTransportMap;
	};
}
#endif
