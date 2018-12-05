#include "FindGPUMacro.h"
#include "clang/AST/ASTContext.h"
using namespace scpar;

GPUMacro::GPUMacro(int blockIdx, int blockIdy, int blockIdz, int threadIdx, int threadIdy, int threadIdz, int gpuTime, int cpuTime) :
  _blockIdx(blockIdx),
  _blockIdy(blockIdy),
  _blockIdz(blockIdz),
  _threadIdx(threadIdx),
  _threadIdy(threadIdy),
  _threadIdz(threadIdz),
  _gpuTime(gpuTime),
  _cpuTime(cpuTime),
	_gpuFit(false){
  }

GPUMacro::GPUMacro():
  _blockIdx(1),
  _blockIdy(1),
  _blockIdz(1),
  _threadIdx(1),
  _threadIdy(1),
  _threadIdz(1),
  _gpuTime(0),
  _cpuTime(0),
	_gpuFit(false){
					
  
  }

GPUMacro::~GPUMacro() {
}

void GPUMacro::addGPUFit() {
	_gpuFit = true;
}

void GPUMacro::denyGPUFit() {
	_gpuFit = false;
}

bool GPUMacro::isGPUFit() {
	return _gpuFit;
}

int GPUMacro::getBlockIdx(){
 return _blockIdx;
}

int GPUMacro::getBlockIdy(){
 return _blockIdy;
}


int GPUMacro::getBlockIdz(){
 return _blockIdz;
}


int GPUMacro::getThreadIdx(){
 return _threadIdx;
}


int GPUMacro::getThreadIdy(){
 return _threadIdx;
}


int GPUMacro::getThreadIdz(){
 return _threadIdz;
}

int GPUMacro::getCPUTime() {
 return _cpuTime;
}

int GPUMacro::getGPUTime() {
 return _gpuTime;
}

void GPUMacro::dump(raw_ostream& os) {
 os <<"\n Block Ids : " <<_blockIdx<<" " <<_blockIdy<<" " <<_blockIdz;

 os <<"\n Thread Ids : " <<_threadIdx<<" " <<_threadIdy<<" " <<_threadIdz;
 
 os <<"\n GPU time : " <<_gpuTime<<" CPU time : " <<_cpuTime;
}

///////////////////////////////////////////////////////
FindGPUMacro::FindGPUMacro(CXXMethodDecl* entryFunction, int instanceNum, llvm::raw_ostream& os):
  _entryFunction(entryFunction),
  _instanceNum(instanceNum),
	_os(os)
{
 TraverseDecl(_entryFunction);
}

FindGPUMacro::~FindGPUMacro()
{
}

bool FindGPUMacro::VisitForStmt(ForStmt *fstmt) {
 
  Stmt *body = fstmt->getBody();

  analyze_data_struct(body);

  int tx = 1, ty = 1, tz = 1 , bx = 1, by = 1, bz = 1, gpu_time = 0, cpu_time = 0, instanceNum = 0;
  for (Stmt::child_iterator it = body->child_begin(), eit = body->child_end();
      it != eit;
      it++) {

    Stmt *s = *it; 

    if (DeclStmt *ds = dyn_cast<DeclStmt>(s)){
      if (VarDecl *vd = dyn_cast<VarDecl>(ds->getSingleDecl())){
        string className = vd->getTypeSourceInfo()->getType().getBaseTypeIdentifier()->getName();
				if (className == "profile_time") {
	 				if (CXXConstructExpr *ce = dyn_cast<CXXConstructExpr>(vd->getInit()->IgnoreImpCasts())) {					 
	 	 				if (MaterializeTemporaryExpr *me = dyn_cast<MaterializeTemporaryExpr>(ce->getArg(0)->IgnoreImpCasts())) {
							if (CXXTemporaryObjectExpr *co = dyn_cast<CXXTemporaryObjectExpr>(me->GetTemporaryExpr()->IgnoreImpCasts())) {

								IntegerLiteral *x = dyn_cast<IntegerLiteral>(co->getArg(0));
								IntegerLiteral *y = dyn_cast<IntegerLiteral>(co->getArg(1));
	  						IntegerLiteral *z = dyn_cast<IntegerLiteral>(co->getArg(2));

								instanceNum = x->getValue().getSExtValue();
	  						gpu_time = y->getValue().getSExtValue();
	  						cpu_time = z->getValue().getSExtValue();
	 						}	 
						}
				}
		}	
		
    if (className == "sc_gpu_thread_hierarchy") {   
    	if (CXXConstructExpr *ce = dyn_cast<CXXConstructExpr>(vd->getInit()->IgnoreImpCasts())) {
				if (MaterializeTemporaryExpr *me = dyn_cast<MaterializeTemporaryExpr>(ce->getArg(0)->IgnoreImpCasts())) {
					if (CXXTemporaryObjectExpr *co = dyn_cast<CXXTemporaryObjectExpr>(me->GetTemporaryExpr()->IgnoreImpCasts())) {
          	IntegerLiteral *x = dyn_cast<IntegerLiteral>(co->getArg(1)); 
          	IntegerLiteral *y = dyn_cast<IntegerLiteral>(co->getArg(2));
          	IntegerLiteral *z = dyn_cast<IntegerLiteral>(co->getArg(3)); 
						IntegerLiteral *w = dyn_cast<IntegerLiteral>(co->getArg(4));
          	instanceNum = x->getValue().getSExtValue();
						tx = x->getValue().getSExtValue();
          	ty = y->getValue().getSExtValue();
          	tz = z->getValue().getSExtValue();          
         	}        
        }
			}
		}
    if (className == "sc_gpu_block_hierarchy") {   
    	if (CXXConstructExpr *ce = dyn_cast<CXXConstructExpr>(vd->getInit()->IgnoreImpCasts())) {
				if (MaterializeTemporaryExpr *me = dyn_cast<MaterializeTemporaryExpr>(ce->getArg(0)->IgnoreImpCasts())) {
					if (CXXTemporaryObjectExpr *co = dyn_cast<CXXTemporaryObjectExpr>(me->GetTemporaryExpr()->IgnoreImpCasts())) {
          	IntegerLiteral *x = dyn_cast<IntegerLiteral>(co->getArg(1)); 
          	IntegerLiteral *y = dyn_cast<IntegerLiteral>(co->getArg(2));
          	IntegerLiteral *z = dyn_cast<IntegerLiteral>(co->getArg(3)); 
          	IntegerLiteral *w = dyn_cast<IntegerLiteral>(co->getArg(4));
				  
						instanceNum = x->getValue().getSExtValue();	
						bx = y->getValue().getSExtValue();
          	by = z->getValue().getSExtValue();
          	bz = w->getValue().getSExtValue();           
         			}        
        		}
					}
				}
  		}
		}
	
		//_os <<"\n gpu_time : " <<gpu_time<<" cpu_time : " <<cpu_time<<" instanceNum : " <<_instanceNum<<" " <<instanceNum;
  	if (tx && ty && tz && bx && by && bz && gpu_time && cpu_time && (_instanceNum == instanceNum)) {
			//_os <<"\n instance num : " <<_instanceNum<<" " <<instanceNum;
  	  GPUMacro *gm = new GPUMacro(bx, by, bz, tx, ty, tz, gpu_time, cpu_time);
			//_os <<"\n for stmt : " <<fstmt;
			forStmtInstanceIdPairType forStmtInstanceId = make_pair(_instanceNum, fstmt);
			_forStmtGPUMacroMap.insert(forStmtGPUMacroPairType(forStmtInstanceId, gm)); 			
			break;
		}
	}
  return true;
}

FindGPUMacro::forStmtGPUMacroMapType FindGPUMacro::getForStmtGPUMacroMap() {
 
  return _forStmtGPUMacroMap;
}

void FindGPUMacro::dump(){
 _os <<"\n For Stmt GPU Macro";
 for (forStmtGPUMacroMapType::iterator it = _forStmtGPUMacroMap.begin(), eit = _forStmtGPUMacroMap.end();
     it != eit;
     it++) {
  
   _os <<"\n For Stmt : " <<it->first.second;
   _os <<"\n GPU kernel parameters \n";
   it->second->dump(_os);
 }
}

void FindGPUMacro::analyze_value_decl(ValueDecl *val) {
    QualType type = val->getType();

    /*
    std::pair<uint64_t, unsigned> fieldInfo =
        val->getASTContext().getTypeInfo(val->getType());
    uint64_t typeSize = fieldInfo.first;
    unsigned fieldAlign = fieldInfo.second;

    _os << "base type: " << type.getCanonicalType().getAsString()
        << ", size (bits): " << typeSize
        << ", align (bits): " << fieldAlign
        << "\n";
        */
}

void FindGPUMacro::analyze_array_base(Expr *base, bool isLHS) {
    if (CastExpr *cast = dyn_cast<CastExpr>(base)) {
        Expr *subExpr = cast->getSubExpr();

        if (DeclRefExpr *declRef = dyn_cast<DeclRefExpr>(subExpr)) {
            if (isLHS) {
                lhs_decls.insert(declRef->getDecl());
                _os << "LHS: ";
            }
            else {
                rhs_decls.insert(declRef->getDecl());
                _os << "RHS: ";
            }
            analyze_value_decl(declRef->getDecl());
        }
        else if (MemberExpr *member = dyn_cast<MemberExpr>(subExpr)) {
            if (isLHS) {
                lhs_decls.insert(member->getMemberDecl());
                _os << "LHS: ";
            }
            else {
                rhs_decls.insert(member->getMemberDecl());
                _os << "RHS: ";
            }
            analyze_value_decl(member->getMemberDecl());
        }
        else {
            _os << "Type not a DeclRefExpr"
                << cast->getStmtClassName()
                << "\n";
        }
    }
    else {
        _os << "Type not a CastExpr"
            << cast->getStmtClassName()
            << "\n";
    }
}

void FindGPUMacro::analyze_lhs(Expr *expr) {
    if (ArraySubscriptExpr *array = dyn_cast<ArraySubscriptExpr>(expr)) {
        Expr *base = array->getBase();
        Expr *idx = array->getIdx();

        analyze_array_base(base, true /* isLHS */);
    }
    else if (CastExpr *cast = dyn_cast<CastExpr>(expr)) {
        analyze_lhs(cast->getSubExpr());
    }
    else if (ParenExpr *paren = dyn_cast<ParenExpr>(expr)) {
        analyze_lhs(paren->getSubExpr());
    }
    else if (BinaryOperator *binOp = dyn_cast<BinaryOperator>(expr)) {
        if (binOp->isAssignmentOp()) {
            analyze_lhs(binOp->getLHS());
            analyze_rhs(binOp->getRHS());
        }
        else if (binOp->isCompoundAssignmentOp()) {
            analyze_lhs(binOp->getLHS());
            analyze_rhs(binOp->getRHS());
        }
        else {
            _os << "Operation without effects on LHS: "
                << binOp->getStmtClassName()
                << "\n";
        }
    }
}

void FindGPUMacro::analyze_rhs(Expr *expr) {
    if (ArraySubscriptExpr *array = dyn_cast<ArraySubscriptExpr>(expr)) {
        Expr *base = array->getBase();
        Expr *idx = array->getIdx();

        analyze_array_base(base, false /* isLHS */);
    }
    else if (CastExpr *cast = dyn_cast<CastExpr>(expr)) {
        analyze_rhs(cast->getSubExpr());
    }
    else if (ParenExpr *paren = dyn_cast<ParenExpr>(expr)) {
        analyze_rhs(paren->getSubExpr());
    }
    else if (BinaryOperator *binOp = dyn_cast<BinaryOperator>(expr)) {
        if (binOp->isAssignmentOp()) {
            analyze_lhs(binOp->getLHS());
            analyze_rhs(binOp->getRHS());
        }
        else if (binOp->isCompoundAssignmentOp()) {
            analyze_lhs(binOp->getLHS());
            analyze_rhs(binOp->getRHS());
        }
        else {
            analyze_rhs(binOp->getLHS());
            analyze_rhs(binOp->getRHS());
        }
    }
}

void FindGPUMacro::analyze_expr(Expr *expr) {
    if (BinaryOperator *binOp = dyn_cast<BinaryOperator>(expr)) {
        if (binOp->isAssignmentOp()) {
            analyze_lhs(binOp->getLHS());
            analyze_rhs(binOp->getRHS());
        }
        else if (binOp->isCompoundAssignmentOp()) {
            analyze_lhs(binOp->getLHS());
            analyze_rhs(binOp->getRHS());
        }
        else {
            _os << "Operation without effects: "
                << binOp->getStmtClassName()
                << "\n";
        }
    }
}

void FindGPUMacro::analyze_data_struct(Stmt *stmtList) {
    if (!stmtList) return;

    _os << "\nNEW FORLOOP BEGIN\n";

    for (Stmt::child_iterator itr = stmtList->child_begin();
         itr != stmtList->child_end();
         ++itr) {
        Stmt *stmt = *itr;
        if (!stmt) continue;

        Expr *expr = dyn_cast<Expr>(stmt);
        if (!expr) continue;

        // Dump out the AST tree
        expr->dump();

        analyze_expr(expr);
    }

    for (std::set<ValueDecl*>::iterator itr = lhs_decls.begin();
         itr != lhs_decls.end();
         ++itr) {
        _os << "LHS ValueDecl AST Node: " << *itr
            << ", Name: '" << (*itr)->getNameAsString() << "', ";
        analyze_value_decl(*itr);
    }

    for (std::set<ValueDecl*>::iterator itr = rhs_decls.begin();
         itr != rhs_decls.end();
         ++itr) {
        _os << "RHS ValueDecl AST Node: " << *itr
            << ", Name: '" << (*itr)->getNameAsString() << "', ";
        analyze_value_decl(*itr);
    }

    _os << "NEW FORLOOP END\n";
}
