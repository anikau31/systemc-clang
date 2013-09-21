#ifndef _FIND_CLASS_DATA_MEMBERS_H_
#define _FIND_CLASS_DATA_MEMBERS_H_

#include <map>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/ADT/APInt.h"
//#include "ReflectionContainerClass.h"

namespace scpar {
  using namespace clang;
  using namespace std;
	
	class SocketCharacteristics {
		public:
			SocketCharacteristics(string, vector<string>);
			~SocketCharacteristics();
			
			typedef vector<string> registerCallbackMethods;

			string getSocketType();
			registerCallbackMethods getRegisterCallbackMethods();

		private:
			string _socketType;
			registerCallbackMethods _callbackMethods;			
	};

 	////////////////////////////////////////////////////////////////////////////
  class FindClassDataMembers: public RecursiveASTVisitor<FindClassDataMembers> {
  public:
    /// typedefs
    typedef pair<string, vector<unsigned int> > classMemberSizePairType;
    typedef map<string, vector<unsigned int> > classMemberSizeMapType;
    typedef map<string, FieldDecl*> classMemberMapType;
    typedef pair<string, FieldDecl*> kvType;
		
		typedef pair<string, SocketCharacteristics*> socketCharacteristicsPairType;
		typedef map<string, SocketCharacteristics*> socketCharacteristicsMapType;
		
    FindClassDataMembers(CXXRecordDecl*, llvm::raw_ostream&);
		~FindClassDataMembers();
		
    /// RecursiveASTVisitor methods
    virtual bool VisitFieldDecl(FieldDecl*);
    virtual bool VisitCXXMethodDecl(CXXMethodDecl *);
    virtual bool VisitIntegerLiteral(IntegerLiteral *);
		virtual bool VisitBinaryOperator (BinaryOperator *);
		
		string returnSocketType(string);

		/// Access methods
    classMemberMapType getInClassMembers();
    classMemberSizeMapType getClassMemberSize();
    string getDataMembertype(FieldDecl* fd);
		socketCharacteristicsMapType getSocketCharacteristicsMap();

    /// Print methods
    void dump();
   
  private:
    CXXRecordDecl *_d;
    llvm::raw_ostream &_os;
    classMemberMapType _inClassMembers;
    int pass;
    Stmt *_constructorStmt;
		socketCharacteristicsMapType _socketCharacteristicsMap;
    classMemberSizeMapType _classMemberSize;
    string _memberName;
    vector<unsigned int> _sizeArray;
  };
}
#endif
