#ifndef _FIND_PAYLOAD_CHARACTERISTICS_H_
#define _FIND_PAYLOAD_CHARACTERISTICS_H_

#include <map>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "FindTemplateTypes.h"
//#include "ReflectionContainerClass.h"

namespace scpar {
  using namespace clang;
  using namespace std;

	class Payload {
		public:
			Payload();
			Payload(string, string, string, string, string, string, string, string);
			~Payload();	

			void dump(raw_ostream&);
		private:
			string _command;
			string _address;
			string _dataPtr;
			string _dataLength;
			string _streamingWidth;
			string _byteEnablePtr;
			string _DMIAllowed;
			string _responseStatus;

	};
  class FindPayloadCharacteristics: public RecursiveASTVisitor<FindPayloadCharacteristics> {
  public:
        
    FindPayloadCharacteristics(CXXMethodDecl*, llvm::raw_ostream&);  
		~FindPayloadCharacteristics();

	 	virtual bool VisitCXXMemberCallExpr(CXXMemberCallExpr *ce);
	
		typedef pair <string, Payload*> payloadCharacteristicsPairType;
		typedef map <string, Payload*> payloadCharacteristicsMapType;
	
		payloadCharacteristicsMapType returnPayloadCharacteristicsMap();
		void dump(raw_ostream&);
  private:
    llvm::raw_ostream &_os;
   	CXXMethodDecl* _d;
		payloadCharacteristicsMapType _payloadCharacteristicsMap;
  	string _payloadName;
		string _command;
		string _address;
		string _dataPtr;
		string _dataLength;
		string _streamingWidth;
		string _byteEnablePtr;
		string _DMIAllowed;
		string _responseStatus;
	};
}
#endif
