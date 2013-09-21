#include "FindMemberFunctions.h"
#include "llvm/Support/raw_ostream.h"

using namespace scpar;
using namespace std;

MemberFunctionType::MemberFunctionType(): _name(""), _entry(NULL)  {
}

MemberFunctionType::MemberFunctionType(const MemberFunctionType& newEntry)  {
  _name = newEntry._name;
  _entry = newEntry._entry;
}

string MemberFunctionType::getName() {
  return _name;
}

CXXMemberCallExpr* MemberFunctionType::getEntryMethod() {
  return _entry;
}

FindMemberFunctions::FindMemberFunctions(CXXMethodDecl *decl, llvm::raw_ostream &os):
  _memberFunctCall(decl), _os(os) {
      //_functionMap = std::map<std::string , MemberFunctionType >();
      TraverseDecl(decl);
  }

FindMemberFunctions::~FindMemberFunctions() {
	_functionMap.clear();
}

bool FindMemberFunctions::VisitCXXMemberCallExpr(CXXMemberCallExpr* expr)  {
 MemberFunctionType * _newEntry = new MemberFunctionType();
 _newEntry->_entry = expr;
 _newEntry->_name = expr->getMethodDecl()->getNameAsString();


if((_newEntry->_name != "wait") && (_newEntry->_name != "notify") &&(_newEntry->_name != "write") &&(_newEntry->_name != "read")){
	_functionMap.insert(functionPairType(_newEntry->_name, *_newEntry));
}
 return true;
}

void FindMemberFunctions::dump() {
  _os << "\n ============== FindMemberFunctions  ===============\n";
  for(functionMapType::iterator it= _functionMap.begin();
      it != _functionMap.end();  it++) {
    MemberFunctionType mf = it->second;
    _os << "- name: " << mf.getName() << ", CXXMemberCallExpr*: " << mf.getEntryMethod() << "\n";
  }
  _os << "\n ============== END FindMemberFunctions  ===============";
}

FindMemberFunctions::functionMapType FindMemberFunctions::returnFunctionMap() {
	return _functionMap;
}
