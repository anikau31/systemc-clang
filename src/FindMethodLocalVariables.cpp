#include "FindMethodLocalVariables.h"
#include "FindTemplateTypes.h"
#include "clang/AST/PrettyPrinter.h"
using namespace scpar;
using namespace std;


VariableTypeInfo::~VariableTypeInfo() {
  
}

VariableTypeInfo::VariableTypeInfo(llvm::raw_ostream& os): _os(os) {
  _expr = NULL; _arraySize = 0; _isArray = false; 
}

VariableTypeInfo::VariableTypeInfo(const VariableTypeInfo& rhs):_os(rhs._os) {
  _expr = rhs._expr;
  _tempArgs = rhs._tempArgs;
  _arraySize = rhs._arraySize;
  _type = rhs._type;
  _isArray = rhs._isArray;
  _name = rhs._name;
  _tempArgs = rhs._tempArgs;
}

void VariableTypeInfo::print() {
  _os << "\n- name: " << _name << ", type: " << _type << ", isArray: " << _isArray << ", _arraySize: " << _arraySize << ", expr*: " << _expr << "\n";
  /// FIXME: Add code for handling template arguments.
}

bool VariableTypeInfo::isArrayType()  {
  return _isArray;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// FindMethodLocalVariables
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

map<string,string> FindMethodLocalVariables::getMethodVariables() {
  map<string,string> vars;
  for(classMemberMapType::iterator vit = _inMethodVariables.begin(); 
      vit!= _inMethodVariables.end(); vit++) {
    vars.insert(pair<string,string>(vit->first, getDataMembertype(vit->second._expr)));
  }
  return vars;
}

map<string,string> FindMethodLocalVariables::getLHSVariables() {
  map<string,string> lhs;
  for(classMemberMapType::iterator vit = _lhsVariables.begin(); 
      vit!= _lhsVariables.end(); vit++) {
    lhs.insert(pair<string,string>(vit->first, getDataMembertype(vit->second._expr)));
    //    _os << "\n- name: " << vit->first << ", type: " << getDataMembertype(vit->second) << ", Expr*: " << vit->second ;
  }
  return lhs;
}

FindMethodLocalVariables::FindMethodLocalVariables(CXXMethodDecl* d, 
                                                   llvm::raw_ostream &os
                                                  ):_os(os), _d(d) {
  _state = LHS;
  TraverseDecl(_d);
  
  //  _state = RHS;
  //  TraverseDecl(d);  
//  printInClassMembers();
}

bool FindMethodLocalVariables::VisitBinaryOperator(BinaryOperator *b) {
  switch(_state) {
    case LHS: {
      if (!b->isAssignmentOp()) {
        break;
      }
      //      _os << "\n ### LHS \n ";    
      Expr* e = b->getLHS();
      
      if (e == NULL) {
        return false;
      }    
      
      VariableTypeInfo v(_os);
      v._name = getVariableName(e);
      v._expr = e;
      
      _lhsVariables.insert(kvType(v._name, v));
      break;
    }
    case RHS: {
      Expr* rhs = b->getRHS();
//      _os << "- name: " << getVariableName(rhs) << "\n";
      break;
    }
  }  
  return true;
}

bool FindMethodLocalVariables::VisitMemberExpr(MemberExpr* e) {
  //  _os << "\n=> MemberExpr\n";
  switch(_state) {
    case LHS:
      break;
    case RHS:
      // Only if we are parsing RHS.
      QualType q = e->getType();
//      _os << "\n- @@@ type: " << q.getAsString() << ", name: " << getVariableName(e); 
      break;
  };
  
  return true;
}

bool FindMethodLocalVariables::VisitDeclRefExpr(DeclRefExpr* e) {
  switch(_state) {
    case LHS: {
//      _os << "#######: DeclRefExpr: " << getVariableName(e) << ", lvalue: " << e->getDecl()->getDeclKindName() << "\n";
      
      //    e->dumpAll();
      ValueDecl* vd = e->getDecl();
      if (vd == NULL)
        break;
      
      QualType q = vd->getType();
      const Type* t = q.getTypePtr();
      
      Decl* d = e->getDecl();
      if (d == NULL)
        break;
      
      string declKind(d->getDeclKindName());
      if ((getDataMembertype(e) == "enum sc_core::sc_time_unit") ||
          (declKind == "CXXMethod"))      {
        break;
      }
      string name = getVariableName(e);
      if ((_inMethodVariables.find(name)) != _inMethodVariables.end()) {
        // Found entry.
        break;
      }
      //      _os << "=> @@@@@@@@@@@@@@@@@@@@@@@@@@@ Insert\n";
      /// Prepare variable information.
      VariableTypeInfo v(_os);
      v._name = getVariableName(e);
      v._expr = e;
      v._isArray = t->isConstantArrayType();
      v._type = getDataMembertype(e);
      if (v._isArray) {
        if (const ConstantArrayType* ca = dyn_cast<ConstantArrayType>(t)) {
          v._arraySize = ca->getSize();
        }
      }
      
      /// Freed in the destructor of VariableTypeInfo.
      FindTemplateTypes te;
      te.Enumerate(t);
      v._tempArgs = te;
//      v.print();
      
      _inMethodVariables.insert(kvType(v._name, v));
      break;
    }
    case RHS: {
      //_os << "\n=> DeclRefExpr\n";
      break;
    }
  };
  
  return true;
}

string FindMethodLocalVariables::getVariableName(Expr* e) {
  
  clang::LangOptions LangOpts;
  LangOpts.CPlusPlus = true;
  clang::PrintingPolicy Policy(LangOpts);
  
  string TypeS;
  llvm::raw_string_ostream str(TypeS);
  
  e->printPretty(str, 0, Policy);
  //_os << ", argument: " << str.str();
  return str.str();
  
}

FindMethodLocalVariables::classMemberMapType FindMethodLocalVariables::getInClassMembers() {
  return _inMethodVariables;
}

string FindMethodLocalVariables::getDataMembertype(Expr* fd) {
  QualType q = fd->getType();
  return q.getAsString();
}



void FindMethodLocalVariables::dump() {
  _os << "\n ============== FindMethodLocalVariables  ===============";
  _os <<"\n For CXXMethodDecl : " <<_d->getNameAsString();
	_os << "\n:> Print entry method local variables";
  for(classMemberMapType::iterator vit = _inMethodVariables.begin(); vit!= _inMethodVariables.end(); vit++) {
    _os << "\n- name: " << vit->first << ", type: " << getDataMembertype(vit->second._expr);
    vit->second.print();
    vit->second._tempArgs.printTemplateArguments(_os);
  }
  
  _os << "\n\n:> LHS variables";
  for(classMemberMapType::iterator vit = _lhsVariables.begin(); 
      vit!= _lhsVariables.end(); vit++) {
    _os << "\n- name: " << vit->first << ", type: " << getDataMembertype(vit->second._expr) ;
    vit->second.print();
    vit->second._tempArgs.printTemplateArguments(_os);
  }
  
  _os << "\n ============== END FindMethodLocalVariables ===============";  
}
