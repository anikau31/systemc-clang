// clang-format off
#include "SystemCClang.h"
#include "HDLBody.h"
//#include "HDLType.h"
#include "clang/Basic/OperatorKinds.h"
#include "clang/Basic/Diagnostic.h"
#include "APIntUtils.h"
#include "CallExprUtils.h"
#include "CXXRecordDeclUtils.h"

#include <iostream>
#include <unordered_map>
// clang-format on

/// Different matchers may use different DEBUG_TYPE
#undef DEBUG_TYPE
#define DEBUG_TYPE "HDL"

// We have to use the Traverse pattern rather than Visitor
// because we need control to come back to the point of call
// so that the generated tree can be returned back up the
// call chain.

// used https://github.com/super-ast/cpptranslate as example

using namespace std;
using namespace hnode;

//!
//! Entry point for SC_METHOD body generation
//!
//! Starting with outer compount statement,
//! generate hcode for each statement,
//! recursively traversing expressions
//! 

namespace systemc_hdl {
using namespace sc_ast_matchers::utils;

/*
  bool HDLBody::isSCConstruct(const Type* type, const CXXMemberCallExpr* ce) const {
    if (!ce) { return false;}

    std::vector<llvm::StringRef> sc_dt_ns{"sc_dt"};
    bool is_sc_builtin_t{ isInNamespace(type, sc_dt_ns)};

    std::vector<llvm::StringRef> ports_signals_wait{"sc_port_base", "sc_signal_in_if", "sc_signal_out_if", "sc_signal_inout_if", "sc_prim_channel", "sc_thread_process"};
    bool is_ports_signals_wait{isCXXMemberCallExprSystemCCall(ce, ports_signals_wait)};

    std::vector<llvm::StringRef> rvd{"sc_rvd"};
    bool is_rvd{isCXXMemberCallExprSystemCCall(ce, rvd)};


  return is_sc_builtin_t || is_ports_signals_wait || is_rvd;
  }
  */

  HDLBody::HDLBody(clang::DiagnosticsEngine &diag_engine,
		   const ASTContext &ast_context,
		   hdecl_name_map_t &mod_vname_map,
		   hfunc_name_map_t &allmethodecls,
		   overridden_method_map_t &overridden_method_map) :
    diag_e{diag_engine}, ast_context_{ast_context},
    mod_vname_map_{mod_vname_map}, allmethodecls_{allmethodecls}, overridden_method_map_{overridden_method_map} {
      LLVM_DEBUG(llvm::dbgs() << "Entering HDLBody constructor\n");
    }

  void HDLBody::Run(Stmt *stmt, hNodep &h_top, HDLBodyMode runmode, HDLType *HDLt_userclassesp) 
 
  {
    LLVM_DEBUG(llvm::dbgs() << "Entering HDLBody Run Method\n");
    h_ret = NULL;
    add_info = (runmode == rmodinit);
    HDLt_userclassesp_ = HDLt_userclassesp;
    thismode = runmode;
    methodecls.clear(); // clear out old state
    methodecls.set_prefix("_func_");

    vname_map.clear();
    if (thismode == rthread) {
      vname_map.set_prefix("_"+h_top->getname()+tvar_prefix);
    }
    else {
      vname_map.set_prefix("_"+h_top->getname()+vname_map.get_prefix());
    }

    LLVM_DEBUG(llvm::dbgs() << "allmethodecls_ size is " << allmethodecls_.size() << "\n");
    
    // if (!mod_vname_map_.empty())
    // 	vname_map.insertall(mod_vname_map_);
     bool ret1 = TraverseStmt(stmt);
     AddVnames(h_top);
     if (h_ret != NULL) h_top->child_list.push_back(h_ret);
     LLVM_DEBUG(llvm::dbgs() << "Exiting HDLBody Run Method\n");

  }
  HDLBody::~HDLBody() {
    LLVM_DEBUG(llvm::dbgs() << "[[ Destructor HDLBody ]]\n");
  }

  // order of checking is important as some exprs
  // inherit from binaryoperator

  bool HDLBody::TraverseStmt(Stmt *stmt) {
    LLVM_DEBUG(llvm::dbgs() << "In TraverseStmt\n");
    if (stmt == nullptr) return true;  // null statement, keep going
  
    if (isa<CompoundStmt>(stmt)) {
      LLVM_DEBUG(llvm::dbgs()
		 << "calling traverse compoundstmt from traversestmt\n");
      TraverseCompoundStmt((CompoundStmt *)stmt);
    } else if (isa<DeclStmt>(stmt)) {
      TraverseDeclStmt((DeclStmt *)stmt);
    } else if (isa<CallExpr>(stmt)) {
      if (CXXOperatorCallExpr *opercall = dyn_cast<CXXOperatorCallExpr>(stmt)) {
	LLVM_DEBUG(llvm::dbgs() << "found cxxoperatorcallexpr\n");
	TraverseCXXOperatorCallExpr(opercall);
      } else if (isa<CXXMemberCallExpr>(stmt)) {
	TraverseCXXMemberCallExpr((CXXMemberCallExpr *)stmt);
      } else {
	TraverseCallExpr((CallExpr *)stmt);
      }
    } else if (isa<BinaryOperator>(stmt)) {
      TraverseBinaryOperator((BinaryOperator *)stmt);
    } else if (isa<UnaryOperator>(stmt)) {
      TraverseUnaryOperator((UnaryOperator *)stmt);
    } else if (isa<ConditionalOperator>(stmt)) {
      TraverseConditionalOperator((ConditionalOperator *)stmt);
    } else if (isa<MaterializeTemporaryExpr>(stmt)) {
      TraverseStmt(((MaterializeTemporaryExpr *)stmt)->getSubExpr());
      // TraverseStmt(((MaterializeTemporaryExpr *) stmt)->getTemporary());
    } else if (isa<DeclRefExpr>(stmt)) {
      TraverseDeclRefExpr((DeclRefExpr *)stmt);
    } else if (isa<MemberExpr>(stmt)) {
      TraverseMemberExpr((MemberExpr *)stmt);
    } else if (isa<IntegerLiteral>(stmt)) {
      TraverseIntegerLiteral((IntegerLiteral *)stmt);
    } else if (isa<CXXBoolLiteralExpr>(stmt)) {
      TraverseCXXBoolLiteralExpr((CXXBoolLiteralExpr *)stmt);
    } else if (isa<IfStmt>(stmt)) {
      LLVM_DEBUG(llvm::dbgs() << "Found if stmt\n");
      TraverseIfStmt((IfStmt *)stmt);
    } else if (isa<ForStmt>(stmt)) {
      LLVM_DEBUG(llvm::dbgs() << "Found for stmt\n");
      TraverseForStmt((ForStmt *)stmt);
    } else if (isa<WhileStmt>(stmt)) {
      LLVM_DEBUG(llvm::dbgs() << "Found while stmt\n");
      TraverseWhileStmt((WhileStmt *)stmt);
    } else if (isa<DoStmt>(stmt)) {
      LLVM_DEBUG(llvm::dbgs() << "Found do-while stmt\n");
      TraverseDoStmt((DoStmt *)stmt);
    } else if (isa<SwitchStmt>(stmt)) {
      LLVM_DEBUG(llvm::dbgs() << "Found switch stmt\n");
      TraverseSwitchStmt((SwitchStmt *)stmt);
    } else if (isa<CaseStmt>(stmt)) {
      LLVM_DEBUG(llvm::dbgs() << "Found case stmt\n");
      hNodep old_hret = h_ret;
      hNodep hcasep = new hNode(hNode::hdlopsEnum::hSwitchCase);
      if (ConstantExpr *expr =
	  dyn_cast<ConstantExpr>(((CaseStmt *)stmt)->getLHS())) {
	llvm::APSInt val = expr->getResultAsAPSInt();
	hcasep->child_list.push_back(
	  new hNode(systemc_clang::utils::apint::toString(val), hNode::hdlopsEnum::hLiteral));
      }

      TraverseStmt(((CaseStmt *)stmt)->getSubStmt());
      if (h_ret != old_hret)
	hcasep->child_list.push_back(h_ret);
      else
	hcasep->child_list.push_back(new hNode(hNode::hdlopsEnum::hUnimpl));

      h_ret = hcasep;
    } else if (isa<DefaultStmt>(stmt)) {
      LLVM_DEBUG(llvm::dbgs() << "Found default stmt\n");
      hNodep old_hret = h_ret;
      hNodep hcasep = new hNode(hNode::hdlopsEnum::hSwitchDefault);
      TraverseStmt(((DefaultStmt *)stmt)->getSubStmt());
      if (h_ret != old_hret)
	hcasep->child_list.push_back(h_ret);
      else
	hcasep->child_list.push_back(new hNode(hNode::hdlopsEnum::hUnimpl));
      h_ret = hcasep;
    } else if (isa<BreakStmt>(stmt)) {
      //const unsigned cxx_record_id =
        //diag_e.getCustomDiagID(clang::DiagnosticsEngine::Remark,
	//          "Break stmt not supported, substituting noop");
	//clang::DiagnosticBuilder diag_builder{
	//diag_e.Report(stmt->getBeginLoc(), cxx_record_id)};
      LLVM_DEBUG(llvm::dbgs() << "Found break stmt\n");
      h_ret = new hNode(thismode == rthread ? hNode::hdlopsEnum::hReturnStmt : hNode::hdlopsEnum::hBreak);
    } else if (isa<ContinueStmt>(stmt)) {
      LLVM_DEBUG(llvm::dbgs() << "Found continue stmt\n");
      h_ret = new hNode(hNode::hdlopsEnum::hContinue);
    } else if (isa<CXXDefaultArgExpr>(stmt)) {
      TraverseStmt(((CXXDefaultArgExpr *)stmt)->getExpr());
    } else if (isa<ReturnStmt>(stmt)) {
      hNodep hretstmt = new hNode(hNode::hdlopsEnum::hReturnStmt);
      if (((ReturnStmt *)stmt)->getRetValue() !=nullptr) {
	TraverseStmt(((ReturnStmt *)stmt)->getRetValue());
	hretstmt->child_list.push_back(h_ret);
      }
      h_ret = hretstmt;
    } else if (isa<CXXTemporaryObjectExpr>(stmt)) {
      int nargs = ((CXXTemporaryObjectExpr *)stmt)->getNumArgs();
      if (nargs == 0) {  // end of the road
	h_ret = new hNode(
			  "0",
			  (hNode::hdlopsEnum::hLiteral));  // assume this is an initializer of 0
      } else {
	Expr **objargs = ((CXXTemporaryObjectExpr *)stmt)->getArgs();
	for (int i = 0; i < nargs; i++) {
	  TraverseStmt(objargs[i]);
	}
      }
    }
    else if (isa<InitListExpr>(stmt)) {
      hNodep h_initlist = new hNode(hNode::hdlopsEnum::hVarInitList);
      for (auto tmpexpr: ((InitListExpr *) stmt)->inits()) {
	TraverseStmt(tmpexpr);
	h_initlist->append(h_ret);
      }
      h_ret = h_initlist;
    }
    else {
      if (isa<CXXConstructExpr>(stmt)) {
	CXXConstructExpr *exp = (CXXConstructExpr *)stmt;
	// CXXConstructExpr argument not yet handled
	// LLVM_DEBUG(llvm::dbgs()
	// 	   << "CXXConstructExpr found, expr below\n");
	// LLVM_DEBUG(exp->dump(llvm::dbgs(), ast_context_));
	 
	if ((exp->getNumArgs() == 1) && (isa<IntegerLiteral>(exp->getArg(0)))) {
	  LLVM_DEBUG(llvm::dbgs()
		     << "CXXConstructExpr followed by integer literal found\n");
	  LLVM_DEBUG(exp->dump(llvm::dbgs(), ast_context_));
	  IntegerLiteral *lit = (IntegerLiteral *)exp->getArg(0);
	  string s = systemc_clang::utils::apint::toString(lit->getValue());
	  // need to add type to back of h_ret
	  FindTemplateTypes *te = new FindTemplateTypes();
	  te->Enumerate((exp->getType()).getTypePtr());
	  HDLType HDLt;
	  hNodep h_tmp = new hNode(hNode::hdlopsEnum::hNoop);
	  HDLt.SCtype2hcode(s, te->getTemplateArgTreePtr(), 0,
			    hNode::hdlopsEnum::hLiteral, h_tmp);
	  h_ret = h_tmp->child_list.back();
	  return true;
	}
      }
      LLVM_DEBUG(llvm::dbgs()
		 << "stmt type " << stmt->getStmtClassName()
		 << " not recognized, calling default recursive ast visitor\n");
      hNodep oldh_ret = h_ret;
      RecursiveASTVisitor::TraverseStmt(stmt);
      if (h_ret != oldh_ret) {
	LLVM_DEBUG(
		   llvm::dbgs()
		   << "default recursive ast visitor called - returning translation\n");
	return true;
      }
      for (auto arg : stmt->children()) {
	LLVM_DEBUG(llvm::dbgs() << "child stmt type "
		   << ((Stmt *)arg)->getStmtClassName() << "\n");
	if( isa<CXXThisExpr>(arg)) continue;
	TraverseStmt(arg);
	if (h_ret == oldh_ret) {
	  LLVM_DEBUG(llvm::dbgs() << "child stmt not handled\n");
	  // no output generated
	  h_ret = new hNode(arg->getStmtClassName(), hNode::hdlopsEnum::hUnimpl);
	}
      }
    }

    return true;
  }

  bool HDLBody::TraverseCompoundStmt(CompoundStmt *cstmt) {
    // Traverse each statement and append it to the array
    hNodep h_cstmt = new hNode(hNode::hdlopsEnum::hCStmt);

    for (clang::Stmt *stmt : cstmt->body()) {
      TraverseStmt(stmt);
      if (h_ret) {
	if ((isAssignOp(h_ret) ) && (h_ret->child_list.size() == 2) &&
	    (isAssignOp(h_ret->child_list[1]))){
	  hNodep htmp = NormalizeAssignmentChain(h_ret); // break up assignment chain
	  //h_ret = NormalizeAssignmentChain(h_ret); // break up assignment chain
	  h_cstmt->child_list.insert(h_cstmt->child_list.end(),
				     htmp->child_list.begin(), htmp->child_list.end());
	}
	else h_cstmt->child_list.push_back(h_ret);
      } else
	LLVM_DEBUG(llvm::dbgs() << "stmt result was empty\n");
      h_ret = NULL;
    }

    h_ret = h_cstmt;
    return true;
  }

  //!
  //! Process local variable and signal declarations,
  //! promoting to module level with unique names
  //!

  bool HDLBody::TraverseDeclStmt(DeclStmt *declstmt) {
    // hNodep h_varlist = NULL;
    // if (!declstmt->isSingleDecl()) {
    //     h_varlist = new hNode(hNode::hdlopsEnum::hPortsigvarlist);
    //   }
    // from https://clang.llvm.org/doxygen/DeadStoresChecker_8cpp_source.html
    for (auto *DI : declstmt->decls())
      if (DI) {
	auto *vardecl = dyn_cast<VarDecl>(DI);
	if (!vardecl) continue;
	ProcessVarDecl(vardecl);  // adds it to the list of renamed local variables
      }
    // h_ret = NULL;
    return true;
  }

  bool HDLBody::ProcessVarDecl(VarDecl *vardecl) {
    LLVM_DEBUG(llvm::dbgs() << "ProcessVarDecl var name is " << vardecl->getName()
	       << "\n");
    // create head node for the vardecl
    hNodep h_varlist = new hNode(hNode::hdlopsEnum::hPortsigvarlist);

    QualType q = vardecl->getType();
    const Type *tp = q.getTypePtr();
    LLVM_DEBUG(llvm::dbgs() << "ProcessVarDecl type name is " << q.getAsString()
	       << "\n");
    FindTemplateTypes *te = new FindTemplateTypes();

    te->Enumerate(tp);
    HDLType HDLt;
    std::vector<llvm::APInt> array_sizes = sc_ast_matchers::utils::array_type::getConstantArraySizes(vardecl);
    HDLt.SCtype2hcode(generate_vname(vardecl->getName().str()), te->getTemplateArgTreePtr(), &array_sizes,
		      hNode::hdlopsEnum::hVardecl, h_varlist);

    h_ret = NULL;

    if (h_varlist->child_list.size() == 0) return true;
    
    hNodep h_vardecl = h_varlist->child_list.back();
    vname_map.add_entry(vardecl, vardecl->getName().str(), h_vardecl);

    bool isuserdefinedclass = false;
    if (HDLt.usertype_info.userrectypes.size()>0) {
      const Type * tstp = (((te->getTemplateArgTreePtr())->getRoot())->getDataPtr())->getTypePtr();
      LLVM_DEBUG(llvm::dbgs() << "ProcessVarDecl init of user class, tstp in processvardecl is " << tstp << " isscbytype says " << lutil.isSCByType(tstp) << "\n");
      LLVM_DEBUG(HDLt.print(llvm::dbgs()));
      auto recmapiter = HDLt.usertype_info.userrectypes.find(tstp);
      if (recmapiter != HDLt.usertype_info.userrectypes.end()) {// && (recmapiter->second.find("sc_process_handle")== string::npos)) {
	isuserdefinedclass = true;
	//varinitp->set( hNode::hdlopsEnum::hMethodCall, recmapiter->second);
      }
    }
    
    if (Expr *declinit = vardecl->getInit()) {
      LLVM_DEBUG(llvm::dbgs() << "ProcessVarDecl has an init: \n");
      LLVM_DEBUG(declinit->dump(llvm::dbgs(), ast_context_));
      CXXConstructExpr *tmpdeclinit = dyn_cast<CXXConstructExpr>(declinit);
      if (isuserdefinedclass && (tmpdeclinit!= NULL)) {
	CXXConstructorDecl *cnstrdcl = tmpdeclinit->getConstructor();
	string methodname = cnstrdcl->getNameAsString();
	string qualmethodname = cnstrdcl->getQualifiedNameAsString();
	LLVM_DEBUG(llvm::dbgs() << "ConstructorDecl " << methodname << ", " << qualmethodname << " for var follows\n");
	LLVM_DEBUG(cnstrdcl->dump());
	
      }
      TraverseStmt(declinit);
    }

    if (h_ret) {
      hNodep varinitp = new hNode(hNode::hdlopsEnum::hVarAssign);
      varinitp->child_list.push_back(new hNode(FindVname(vardecl), hNode::hdlopsEnum::hVarref));
      varinitp->child_list.push_back(h_ret);
      h_ret = varinitp;
    }
    return true;
  }

  bool HDLBody::TraverseBinaryOperator(BinaryOperator *expr) {
    // ... handle expr. Can use
    // bool isIntegerConstantExpr(llvm::APSInt &Result, const ASTContext &Ctx,
    //                          SourceLocation *Loc = nullptr,
    //                         bool isEvaluated = true) const;

    hNodep h_binop =
      new hNode(expr->getOpcodeStr().str(),
                hNode::hdlopsEnum::hBinop);  // node to hold binop expr

    string opcodestr = expr->getOpcodeStr().str();
    string exprtypstr = expr->getType().getAsString();
    LLVM_DEBUG(llvm::dbgs() << "in TraverseBinaryOperator, opcode is "
	       << opcodestr << "\n");
//lutil.checktypematch(exprtypstr, expr->getType().getTypePtr(), lutil.bothofthem); // 

    // ========================== CHECK 1 =====================
    // FIXME: Cleanup
    bool t11 = ((opcodestr == ",") && (lutil.isSCType(exprtypstr, expr->getType().getTypePtr() ) ||
                              lutil.isSCBuiltinType(exprtypstr, expr->getType().getTypePtr())));
    bool t21 = ((opcodestr == ",") && (lutil.isSCByType(expr->getType().getTypePtr() )));
 
    if (t11 != t21) {
      llvm::dbgs() << "### CHECK1: t11 != t21\n";
      llvm::dbgs() << t11/0;
      //        std::cin.get();
    }
    // ========================== END CHECK =====================
    //
    if ((opcodestr == ",") && (lutil.isSCType(exprtypstr, expr->getType().getTypePtr() ) ||
                              lutil.isSCBuiltinType(exprtypstr, expr->getType().getTypePtr()))){
      //if ((opcodestr == ",") && (lutil.isSCType(expr->getType()){

      //if ((opcodestr == ",") && (lutil.isSCByType(expr->getType().getTypePtr()))){
      LLVM_DEBUG(llvm::dbgs() << "found comma, with sc type, expr follows\n");
      LLVM_DEBUG(expr->dump(llvm::dbgs(), ast_context_); );
      h_binop->set("concat");
    }
    TraverseStmt(expr->getLHS());
    h_binop->child_list.push_back(h_ret);

    hNodep save_h_ret = h_ret;
    TraverseStmt(expr->getRHS());
    if (h_ret == save_h_ret)
      h_binop->child_list.push_back(new hNode(hNode::hdlopsEnum::hUnimpl));
    else
      h_binop->child_list.push_back(h_ret);

    h_ret = h_binop;

    return true;
  }

  bool HDLBody::TraverseUnaryOperator(UnaryOperator *expr) {
    LLVM_DEBUG(llvm::dbgs() << "in TraverseUnaryOperator expr node is \n");
    LLVM_DEBUG(expr->dump(llvm::dbgs(), ast_context_); );

    auto opcstr = expr->getOpcode();

    hNodep h_unop;
     
    if ((expr->getOpcodeStr(opcstr).str() == "++") ||
	(expr->getOpcodeStr(opcstr).str() == "--")) {
      if (expr->isPostfix())
	h_unop = new hNode(expr->getOpcodeStr(opcstr).str(),
		       hNode::hdlopsEnum::hPostfix);
      else 
	h_unop = new hNode(expr->getOpcodeStr(opcstr).str(),
			   hNode::hdlopsEnum::hPrefix);
    }
    else h_unop = new hNode(expr->getOpcodeStr(opcstr).str(),
                hNode::hdlopsEnum::hUnop);  // node to hold unop expr
    
    TraverseStmt(expr->getSubExpr());
    h_unop->child_list.push_back(h_ret);

    h_ret = h_unop;

    return true;
  }

  bool HDLBody::TraverseConditionalOperator(ConditionalOperator *expr) {
    LLVM_DEBUG(llvm::dbgs() << "in TraverseConditionalOperator expr node is \n");
    LLVM_DEBUG(expr->dump(llvm::dbgs(), ast_context_); );

    hNodep h_condop = new hNode(hNode::hdlopsEnum::hCondop);
    TraverseStmt(expr->getCond());
    h_condop->child_list.push_back(
				   h_ret);  // need to check if it's null or didn't get changed
    TraverseStmt(expr->getTrueExpr());
    h_condop->child_list.push_back(
				   h_ret);  // need to check if it's null or didn't get changed
    TraverseStmt(expr->getFalseExpr());
    h_condop->child_list.push_back(
				   h_ret);  // need to check if it's null or didn't get changed
    h_ret = h_condop;
    return true;
  }
  bool HDLBody::TraverseIntegerLiteral(IntegerLiteral *lit) {
    LLVM_DEBUG(llvm::dbgs() << "In integerliteral\n");
    string s = systemc_clang::utils::apint::toString(lit->getValue());
    h_ret = new hNode(s, hNode::hdlopsEnum::hLiteral);

    return true;
  }

  bool HDLBody::TraverseCXXBoolLiteralExpr(CXXBoolLiteralExpr *b) {
    LLVM_DEBUG(llvm::dbgs() << "In boollitexpr\n");
    bool v = b->getValue();
    h_ret = new hNode(v ? "1" : "0", hNode::hdlopsEnum::hLiteral);

    return true;
  }

  bool HDLBody::TraverseDeclRefExpr(DeclRefExpr *expr) {
    // ... handle expr
    LLVM_DEBUG(llvm::dbgs() << "In TraverseDeclRefExpr\n");

    ValueDecl *value = expr->getDecl();
    if (isa<EnumConstantDecl>(value)) {
      EnumConstantDecl *cd = (EnumConstantDecl *)value;
      LLVM_DEBUG(llvm::dbgs()
		 << "got enum constant value " << cd->getInitVal() << "\n");
      h_ret =
        new hNode(systemc_clang::utils::apint::toString(cd->getInitVal()), hNode::hdlopsEnum::hLiteral);
      return true;
    }

    // get a name

    string name = (expr->getNameInfo()).getName().getAsString();
    LLVM_DEBUG(llvm::dbgs() << "name is " << name << "\n");

    // if this is variable reference has a constant initializer, return that value
    if (isa<VarDecl>(value) && ((VarDecl *)value)->isConstexpr()) {
      VarDecl *vard = (VarDecl *)value;
      Expr *einit = vard->getInit();
      clang::Expr::EvalResult result;
      if (einit->EvaluateAsInt(result, vard->getASTContext())) {
        h_ret = new hNode(systemc_clang::utils::apint::toString(result.Val.getInt()),
			  hNode::hdlopsEnum::hLiteral);
	return true;
      }
    }
    if (isa<FunctionDecl>(value)) {

      // ============= CHECK ================
       bool t1 =  !lutil.isSCFunc(name);
       bool t2 =  !lutil.isSCByCallExpr(expr); 

       if (t1 != t2) {
         llvm::dbgs() << "@@@@ CHECK isSCFunc failed " << t1 << " t2 " << t2 << " name " << name << "\n";
       }
      // ============= END CHECK ================
      if (!lutil.isSCFunc(name)) {  // similar to method call, skip builtin
	FunctionDecl *funval = (FunctionDecl *)value;
	
	string qualfuncname{value->getQualifiedNameAsString()};
	lutil.make_ident(qualfuncname);
	if (add_info) qualfuncname += ":"+ name; // !!! add unqualified name for future hcode processing
	//methodecls[qualfuncname] =
	//  (FunctionDecl *)value;  // add to list of "methods" to be generated
	//methodecls.insert(make_pair(qualfuncname, (FunctionDecl *)value));
	
	// create the call expression
	hNodep hfuncall = new hNode(qualfuncname, hNode::hdlopsEnum::hMethodCall);
	// don't add this method to methodecls if processing modinit
	if (!add_info) {
	  string tmpname = FindFname((FunctionDecl *)value);
	  if (tmpname == "") { // isn't in local or global symbol table
	    LLVM_DEBUG(llvm::dbgs() << "adding method " << qualfuncname << " with pointer " << value << " \n");
	    LLVM_DEBUG(methodecls.print(llvm::dbgs()));
	    methodecls.add_entry((CXXMethodDecl *)funval, qualfuncname,  hfuncall);
	  }
	  else hfuncall->set(tmpname);
	}
	h_ret = hfuncall;
	return true;
      }
      else { // here it is an SCFunc
	string typname = (expr->getType()).getAsString();
	if (typname.find("sc_dt::sc_concat") !=std::string::npos) {
	  // found concat function call
	  hNodep hconcat = new hNode(name, hNode::hdlopsEnum::hBinop);
	  h_ret = hconcat;
	  return true;
	}
	h_ret = new hNode(name, hNode::hdlopsEnum::hBuiltinFunction);
	return true;
	// may have other special functions to recognize later
      }
    }
    
    string newname = FindVname(expr->getDecl());
    LLVM_DEBUG(llvm::dbgs() << "new name is " << newname << "\n");
    LLVM_DEBUG(expr->getDecl()->dump(llvm::dbgs()));

    h_ret =
      new hNode(newname.empty() ? name : newname, hNode::hdlopsEnum::hVarref);
    return true;
  }

  bool HDLBody::TraverseArraySubscriptExpr(ArraySubscriptExpr *expr) {
    LLVM_DEBUG(llvm::dbgs() << "In TraverseArraySubscriptExpr, base, idx, tree follow\n");
    LLVM_DEBUG(llvm::dbgs() << "base:\n");
    LLVM_DEBUG(expr->getBase()->dump(llvm::dbgs(), ast_context_));
    LLVM_DEBUG(llvm::dbgs() << "idx:\n");
    LLVM_DEBUG(expr->getIdx()->dump(llvm::dbgs(), ast_context_));
    LLVM_DEBUG(llvm::dbgs() << "tree:\n");
    LLVM_DEBUG(expr->dump(llvm::dbgs(), ast_context_));
    hNodep h_arrexpr = new hNode("ARRAYSUBSCRIPT", hNode::hdlopsEnum::hBinop);
    TraverseStmt(expr->getLHS());
    h_arrexpr->child_list.push_back(h_ret);
    TraverseStmt(expr->getRHS());
    h_arrexpr->child_list.push_back(h_ret);
    h_ret = h_arrexpr;
    return true;
  }

  bool HDLBody::TraverseCXXMemberCallExpr(CXXMemberCallExpr *callexpr) {
    bool is_explicitly_overridden = false;
        // this doesn't seem to help
    LangOptions LangOpts;

    LangOpts.CPlusPlus = true;
    const PrintingPolicy Policy(LangOpts);
    
    LLVM_DEBUG(llvm::dbgs()
	       << "In TraverseCXXMemberCallExpr, printing implicit object arg\n");
    // Retrieves the implicit object argument for the member call.
    // For example, in "x.f(5)", this returns the sub-expression "x".

    Expr *rawarg = (callexpr->getImplicitObjectArgument());
    LLVM_DEBUG(llvm::dbgs() << "raw implicitobjectargument follows\n");
    LLVM_DEBUG(rawarg->dump(llvm::dbgs(), ast_context_));
    
    Expr *objarg = (callexpr->getImplicitObjectArgument())->IgnoreImplicit();
    LLVM_DEBUG(llvm::dbgs() << "implicitobjectargument, ignore implicit follows\n");
    LLVM_DEBUG(objarg->dump(llvm::dbgs(), ast_context_));
    CXXRecordDecl* cdecl = callexpr->getRecordDecl();
    const Type * typeformethodclass = cdecl->getTypeForDecl();
    LLVM_DEBUG(llvm::dbgs() << "Type pointer from RecordDecl is " << typeformethodclass << "\n");
    
    QualType argtyp;
    if (dyn_cast<ImplicitCastExpr>(rawarg)) { // cast to a specfic type
      argtyp = rawarg->getType();
      is_explicitly_overridden = true;
    }
    else {
      argtyp = objarg->getType();
    }
    LLVM_DEBUG(llvm::dbgs() << "type of x in x.f(5) is " << argtyp.getAsString(Policy)
	       << "\n");
    QualType objtyp = callexpr->getObjectType();
    LLVM_DEBUG(llvm::dbgs() << "... and object type is " << objtyp.getAsString(Policy)
	       << "\n");
    string methodname = "NoMethod", qualmethodname = "NoQualMethod";

    CXXMethodDecl *methdcl = callexpr->getMethodDecl();
    if ((!is_explicitly_overridden) && (overridden_method_map_.size() > 0) && (overridden_method_map_.find(methdcl) != overridden_method_map_.end())) {
      methdcl = const_cast<CXXMethodDecl *>(overridden_method_map_[methdcl]);
    }
     LLVM_DEBUG(llvm::dbgs() << "methoddecl follows\n");
     LLVM_DEBUG(methdcl->dump(llvm::dbgs()));
    if (isa<NamedDecl>(methdcl) && methdcl->getDeclName()) {
      methodname = methdcl->getNameAsString();
      qualmethodname = methdcl->getQualifiedNameAsString();
      // make_ident(qualmethodname);
      //      methodecls[qualmethodname] = methdcl;  // put it in the set of
      //      method decls

      LLVM_DEBUG(llvm::dbgs() << "here is method printname " << methodname
		 << " and qual name " << qualmethodname << " and declp " << methdcl << " \n");
      if (methodname.compare(0, 8, "operator") ==
	  0) {  // 0 means compare =, 8 is len("operator")
	// the conversion we know about, can be skipped
	LLVM_DEBUG(llvm::dbgs() << "Found operator conversion node\n");
	TraverseStmt(objarg);
	return true;
      }
    }

    hNode::hdlopsEnum opc;
    hNode * h_callp = NULL;
    LLVM_DEBUG(llvm::dbgs() << "found " << methodname << "\n");

    // if type of x in x.f(5) is primitive sc type (sc_in, sc_out, sc_inout,
    // sc_signal and method name is either read or write, generate a SigAssignL|R
    // -- FIXME need to make sure it is templated to a primitive type

    //lutil.isSCType(qualmethodname, typeformethodclass);
    //lutil.checktypematch(qualmethodname, typeformethodclass, lutil.issctype);

    //bool inns_result = sc_ast_matchers::utils::isInNamespace(callexpr, "sc_core") || sc_ast_matchers::utils::isInNamespace(callexpr, "sc_dt");
    bool foundsctype = lutil.isSCType(qualmethodname, typeformethodclass);
    bool newfoundsctype = lutil.isSCByCallExpr(callexpr);// || lutil.isSCType(typeformethodclass);
    if (foundsctype != newfoundsctype ) {
      LLVM_DEBUG(llvm::dbgs() << "callexpr isSCType nonmatch -- old one returned " << foundsctype << " for " << qualmethodname << "\n");
      callexpr->dump();
      //std::cin.get();
      //foundsctype = newfoundsctype; // ADD THIS TO TEST SEGV
    }

    //    bool foundsctype = lutil.isSCByCallExpr(callexpr);

    if ((methodname == "read") && foundsctype)
      opc = hNode::hdlopsEnum::hSigAssignR;
    else if ((methodname == "write") && foundsctype)
      opc = hNode::hdlopsEnum::hSigAssignL;
    else if (methodname == "wait")
      opc = hNode::hdlopsEnum::hWait;
    else if (foundsctype) {  // operator from simulation library
      opc = hNode::hdlopsEnum::hBuiltinFunction;
    } else {
      opc = hNode::hdlopsEnum::hMethodCall;
      lutil.make_ident(qualmethodname);
      if (add_info) qualmethodname+= ":"+ methodname;  // include unqualified name for future hcode processing !!!
      //methodecls[qualmethodname] = methdcl;  // put it in the set of method decls
      h_callp = new hNode(qualmethodname, opc);
      // don't add this method to methodecls if processing modinit
      if (!add_info) {
	string tmpname = FindFname((FunctionDecl *)methdcl);
	if (tmpname == "") { // isn't in local or global symbol table
	  LLVM_DEBUG(llvm::dbgs() << "adding method " << qualmethodname << " with pointer " << methdcl << " \n");
	  methodecls.print(llvm::dbgs());
	  methodecls.add_entry(methdcl, qualmethodname,  h_callp);
	  //string objstr = objtyp.getAsString(Policy);
	  //lutil.make_ident(objstr);
	  // bool a = !isCXXMemberCallExprSystemCCall(callexpr), b =sc_ast_matchers::utils::isInNamespace(callexpr, "sc_core") ;
	  
	  // LLVM_DEBUG(llvm::dbgs() << "is sysc call " << qualmethodname << " old, new "<< a
	  // 	     << " " << b << " end\n");
	  if (!isCXXMemberCallExprSystemCCall(callexpr)) methodecls.methodobjtypemap[methdcl] = typeformethodclass;
	}
	else h_callp->set(tmpname);
      }
      methodname = qualmethodname;
    }

    if (h_callp == NULL) h_callp = new hNode(methodname, opc);  // list to hold call expr node

    hNodep save_hret = h_ret;
    // insert "this" argument if mod init block or recognized as special method (read|write|wait of sc type),
    // or it is a method but not derived for scmodule hierarchy
    if ((add_info) || ((opc != hNode::hdlopsEnum::hMethodCall) ||
		       ( opc == hNode::hdlopsEnum::hMethodCall) && (!isCXXMemberCallExprSystemCCall(callexpr)))) {
     TraverseStmt(objarg);  // traverse the x in x.f(5)
     if (h_ret && (h_ret != save_hret)) h_callp->child_list.push_back(h_ret);
    }

    for (auto arg : callexpr->arguments()) {
      save_hret = h_ret;
      TraverseStmt(arg);
      if (h_ret != save_hret) h_callp->child_list.push_back(h_ret);
    }
    h_ret = h_callp;
    return true;
  }

  bool HDLBody::isLogicalOp(clang::OverloadedOperatorKind opc) {
    switch (opc) {
    case OO_Less:
    case OO_LessEqual:
    case OO_Greater:
    case OO_GreaterEqual:
    case OO_ExclaimEqual:
    case OO_EqualEqual:
      return true;

    default:
      return false;
    }
  }

  bool HDLBody::TraverseCXXOperatorCallExpr(CXXOperatorCallExpr *opcall) {
    string operatorname = getOperatorSpelling(opcall->getOperator());
    string operatortype = (opcall->getType()).getAsString();
    hNodep h_operop;

    LLVM_DEBUG(llvm::dbgs() << "In TraverseCXXOperatorCallExpr, Operator name is "
	       << operatorname << "\n");
    LLVM_DEBUG(llvm::dbgs() << "Type name " << operatortype << "\n");
    LLVM_DEBUG(opcall->getType()->dump(llvm::dbgs(), ast_context_));

    // ========================== CHECK  2=====================
    const Type *optypepointer = opcall->getType().getTypePtr();

    bool t12 =  ((operatorname == "=") || lutil.isSCBuiltinType(operatortype) ||
       lutil.isSCType(operatortype) ||
       (opcall->getType())->isBuiltinType() || 
       ((operatorname=="<<") && (operatortype.find("sensitive") != std::string::npos)));
  bool t22 =  ((operatorname == "=") ||  lutil.isSCByType(optypepointer) || (opcall->getType())->isBuiltinType() || 
       ((operatorname=="<<") && (operatortype.find("sensitive") != std::string::npos)));

    if (t12 != t22) {
      llvm::dbgs() << "CHECK### 2: t12 != t22\n";
      //std::cin.get();
    }
    // ========================== END CHECK =====================
    //


     
    if ((operatorname == "=") || lutil.isSCBuiltinType(operatortype,optypepointer ) ||
       lutil.isSCType(operatortype, optypepointer) ||

	 //if ((operatorname == "=") || lutil.isSCByType(optypepointer ) ||
	(opcall->getType())->isBuiltinType() || 
	((operatorname=="<<") && (operatortype.find("sensitive") != std::string::npos))) {
      LLVM_DEBUG(llvm::dbgs() << "Processing operator call type\n");
      // operator for an SC type
      if ((operatorname.compare("()") == 0) &&
	  (operatortype.find("subref") != string::npos) &&
	  (opcall->getNumArgs() == 3)) {
	// bit slice
	h_operop = new hNode("SLICE", hNode::hdlopsEnum::hBinop);
      } else {
	if (operatorname == "[]")  // subscript in operator call expre
	  h_operop = new hNode("ARRAYSUBSCRIPT", hNode::hdlopsEnum::hBinop);
	else if ((operatorname == "++")||(operatorname == "--")) {
	  if (opcall->getNumArgs()==2) h_operop =  new hNode(operatorname, hNode::hdlopsEnum::hPostfix);
	  else  h_operop =  new hNode(operatorname, hNode::hdlopsEnum::hPrefix);
	}
	else {
	  if (opcall->getNumArgs() == 1)
	    h_operop = new hNode(operatorname, hNode::hdlopsEnum::hUnop);
	  else
	    h_operop = new hNode(operatorname, hNode::hdlopsEnum::hBinop);

    if ((operatorname == ",") && (lutil.isSCByCallExpr(opcall)))
	    h_operop->set("concat"); // overloaded comma is concat for sc types
                               //
                               //
	}
      }
      int nargs = (h_operop->getopc() == hNode::hdlopsEnum::hPostfix ||
		   h_operop->getopc() == hNode::hdlopsEnum::hPrefix) ? 1:
	opcall->getNumArgs();
      for (int i = 0; i < nargs; i++) {
	hNodep save_h_ret = h_ret;
	TraverseStmt(opcall->getArg(i));
	if (h_ret == save_h_ret)
	  h_operop->child_list.push_back(new hNode(hNode::hdlopsEnum::hUnimpl));
	else
	  h_operop->child_list.push_back(h_ret);
	LLVM_DEBUG(llvm::dbgs()
		   << "operator call argument " << i << " follows\n");
	LLVM_DEBUG(opcall->getArg(i)->dump(llvm::dbgs(), ast_context_));
      }
      h_ret = h_operop;
      return true;
    }

    LLVM_DEBUG(llvm::dbgs() << "not yet implemented operator call expr, opc is "
	       << clang::getOperatorSpelling(opcall->getOperator())
	       << " num arguments " << opcall->getNumArgs()
	       << " skipping\n");
    LLVM_DEBUG(opcall->dump(llvm::dbgs(), ast_context_));
    h_ret = new hNode(hNode::hdlopsEnum::hUnimpl);
    return true;
  }

  bool HDLBody::TraverseMemberExpr(MemberExpr *memberexpr) {

    bool founduserclass = false;
    LLVM_DEBUG(llvm::dbgs() << "In TraverseMemberExpr\n");
    string nameinfo = (memberexpr->getMemberNameInfo()).getName().getAsString();
    LLVM_DEBUG(llvm::dbgs() << "name is " << nameinfo
	       << ", base and memberdecl trees follow\n");
    LLVM_DEBUG(llvm::dbgs() << "base is \n");
    LLVM_DEBUG(memberexpr->getBase()->dump(llvm::dbgs(), ast_context_); );
    LLVM_DEBUG(llvm::dbgs() << "memberdecl is " << memberexpr->getMemberDecl() << " \n");
    // if field decl, check if parent is a userdefined type XXXXXX
    LLVM_DEBUG(memberexpr->getMemberDecl()->dump(llvm::dbgs()));
    if ( FieldDecl * fld = dyn_cast<FieldDecl>( memberexpr->getMemberDecl())) {
      LLVM_DEBUG(llvm::dbgs() << "and field decl parent record pointer is " << fld->getParent() << "\n");
      const Type * classrectype = fld->getParent()->getTypeForDecl();
      LLVM_DEBUG(llvm::dbgs() << "and field decl parent record type is " << classrectype << "\n");
      if (isUserClass(classrectype)) {
	LLVM_DEBUG(llvm::dbgs() << "member expr, found user defined class in usertypes " << classrectype << "\n");
	founduserclass = true;
      }
    }

    string thisref = founduserclass? "hthis##":"";
    // traverse the memberexpr base in case it is a nested structure
    hNodep old_h_ret = h_ret;
    TraverseStmt(memberexpr->getBase());  // get hcode for the base
    if (h_ret != old_h_ret) {
      if (h_ret->h_op == hNode::hdlopsEnum::hVarref){
	// concatenate base name in front of field name
	hNodep memexprnode = new hNode(thisref + h_ret->h_name + "##" + nameinfo,
				       hNode::hdlopsEnum::hVarref);
	delete h_ret;
	h_ret = memexprnode;  // replace returned h_ret with single node, field
	// names concatenated
	return true;
      }
      else {
	LLVM_DEBUG(llvm::dbgs() << "Value returned from member expr base was not Varref\n");
	LLVM_DEBUG(h_ret->print(llvm::dbgs()));
	string newname = FindVname(memberexpr->getMemberDecl());
	LLVM_DEBUG(llvm::dbgs() << "member with base expr new name is " << newname << "\n");
	if ((newname == "") && (thismode != rmodinit)) {
	  LLVM_DEBUG(llvm::dbgs() << "vname lookup of memberdecl is null, assuming field reference\n");
	  hNodep hfieldref = new hNode(hNode::hdlopsEnum::hFieldaccess);
	  hfieldref->append(h_ret);
	  hfieldref->append(new hNode(thisref+nameinfo, hNode::hdlopsEnum::hField));
	  h_ret = hfieldref;
	  return true;
	}
	else {
	  hNodep memexprnode = new hNode(newname==""? thisref+nameinfo: thisref+newname, hNode::hdlopsEnum::hVarref);
	  memexprnode->child_list.push_back(h_ret);
	  h_ret = memexprnode;
	  return true;
	}
      }

    }

    string newname = FindVname(memberexpr->getMemberDecl());
    LLVM_DEBUG(llvm::dbgs() << "member expr new name is " << newname << "\n");

    h_ret = new hNode(newname.empty()? thisref+nameinfo : thisref+newname, hNode::hdlopsEnum::hVarref);

    return true;
  }

  bool HDLBody::TraverseCallExpr(CallExpr *callexpr) {
    hNodep hcall;// = new hNode(hNode::hdlopsEnum::hMethodCall);
    hNodep save_hret = h_ret;

    if (isa<FunctionDecl>(callexpr->getCalleeDecl()) &&
	((FunctionDecl *)callexpr->getCalleeDecl())->isConstexpr()) {
      Expr::EvalResult res;
      if (callexpr->EvaluateAsRValue(
				     res, callexpr->getCalleeDecl()->getASTContext())) {
	h_ret =
          new hNode(systemc_clang::utils::apint::toString(res.Val.getInt()), hNode::hdlopsEnum::hLiteral);
	return true;
      }
    }

    TraverseStmt(callexpr->getCallee());
    // unlike methodcall, the function call name will hopefully resolve to a
    // declref. in traversedeclref, we create the hnode for the function call
    if ((h_ret != save_hret)// &&
	//(h_ret->getopc() == hNode::hdlopsEnum::hMethodCall)) {
	) {
      hcall = h_ret;
    } else {
      hcall = new hNode(
			hNode::hdlopsEnum::hMethodCall);  // function name was more complicated
      hcall->child_list.push_back(h_ret);
    }
    for (auto arg : callexpr->arguments()) {
      hNodep sret = h_ret;
      TraverseStmt(arg);
      if (h_ret != sret) {
	hcall->child_list.push_back(h_ret);
      }
    }
    h_ret = hcall;
    LLVM_DEBUG(llvm::dbgs() << "found a call expr"
	       << " AST follows\n ");
    LLVM_DEBUG(callexpr->dump(llvm::dbgs(), ast_context_););
    return true;
  }

  bool HDLBody::TraverseIfStmt(IfStmt *ifs) {
    hNodep h_ifstmt, h_ifc = NULL, h_ifthen = NULL, h_ifelse = NULL;
    h_ifstmt = new hNode(hNode::hdlopsEnum::hIfStmt);
    if (ifs->getConditionVariable()) {
      // Variable declarations are not allowed in if conditions
      LLVM_DEBUG(llvm::dbgs() << "Variable declarations are not allowed in if "
		 "conditions, skipping\n");
      return true;
    } else {
      TraverseStmt(ifs->getCond());
      h_ifc = h_ret;
    }
    TraverseStmt(ifs->getThen());
    if (h_ret != h_ifc)  // unchanged if couldn't translate the then clause
      h_ifthen = h_ret;

    if (ifs->getElse()) {
      TraverseStmt(ifs->getElse());
      if ((h_ret != h_ifc) && (h_ret != h_ifthen)) h_ifelse = h_ret;
    }
    h_ifstmt->child_list.push_back(h_ifc);
    h_ifstmt->child_list.push_back(h_ifthen);
    if (h_ifelse) h_ifstmt->child_list.push_back(h_ifelse);
    h_ret = h_ifstmt;
    return true;
  }

  bool HDLBody::TraverseForStmt(ForStmt *fors) {
    hNodep h_forstmt, h_forinit, h_forcond, h_forinc, h_forbody;
    LLVM_DEBUG(llvm::dbgs() << "For stmt\n");
    h_forstmt = new hNode(hNode::hdlopsEnum::hForStmt);
    if ((fors->getInit()!=NULL) && (isa<CompoundStmt>(fors->getInit())))
      LLVM_DEBUG(llvm::dbgs()
		 << "Compound stmt not handled in for init, skipping\n");
    else {
      // if (isa<DeclStmt>(stmt)) {
      TraverseStmt(fors->getInit());
    }
    h_forinit = (h_ret == NULL) ? new hNode(hNode::hdlopsEnum::hNoop)
      : h_ret;  // null if in place var decl
    TraverseStmt(fors->getCond());
    h_forcond = h_ret;
    TraverseStmt(fors->getInc());
    h_forinc = h_ret;
    LLVM_DEBUG(llvm::dbgs() << "For loop body\n");
    LLVM_DEBUG(fors->getBody()->dump(llvm::dbgs(), ast_context_ ););
    TraverseStmt(fors->getBody());
    h_forbody = h_ret;
    h_forstmt->child_list.push_back(h_forinit);
    h_forstmt->child_list.push_back(h_forcond);
    h_forstmt->child_list.push_back(h_forinc);
    h_forstmt->child_list.push_back(h_forbody);
    h_ret = h_forstmt;

    return true;
  }
  bool HDLBody::ProcessSwitchCase(SwitchCase *sc) {
    LLVM_DEBUG(llvm::dbgs() << "In ProcessSwitchCase\n");
    hNodep hcasep;
    hNodep old_hret = h_ret;
    if (isa<DefaultStmt>(sc)) {
      LLVM_DEBUG(llvm::dbgs() << "Found default stmt in switchcase\n");
      hcasep = new hNode(hNode::hdlopsEnum::hSwitchDefault);
      TraverseStmt((DefaultStmt *)sc->getSubStmt());
    } else {
      LLVM_DEBUG(llvm::dbgs() << "Found case stmt in switchcase\n");
      hcasep = new hNode(hNode::hdlopsEnum::hSwitchCase);
      if (ConstantExpr *expr =
	  dyn_cast<ConstantExpr>(((CaseStmt *)sc)->getLHS())) {
	llvm::APSInt val = expr->getResultAsAPSInt();
	hcasep->child_list.push_back(
				     new hNode(systemc_clang::utils::apint::toString(val), hNode::hdlopsEnum::hLiteral));
      }
      TraverseStmt((CaseStmt *)sc->getSubStmt());
    }
    if (h_ret != old_hret) {
      hcasep->child_list.push_back(h_ret);
    } else {
      hcasep->child_list.push_back(new hNode(hNode::hdlopsEnum::hUnimpl));
    }
    h_ret = hcasep;
    return true;
  }

  bool HDLBody::TraverseSwitchStmt(SwitchStmt *switchs) {
    hNodep h_switchstmt;
    LLVM_DEBUG(llvm::dbgs() << "Switch stmt body -----\n");
    LLVM_DEBUG(switchs->getBody()->dump(llvm::dbgs(), ast_context_););
    LLVM_DEBUG(llvm::dbgs() << "End Switch stmt body -----\n");

    h_switchstmt = new hNode(hNode::hdlopsEnum::hSwitchStmt);
    // Stmt * swinit = dyn_cast<Stmt>(switchs->getInit());
    // if (swinit) {
    //  LLVM_DEBUG(llvm::dbgs() << "switch init not handled, skipping\n");
    //}
    hNodep old_ret = h_ret;
    TraverseStmt(switchs->getCond());
    if (h_ret != old_ret) {
      h_switchstmt->child_list.push_back(h_ret);
    } else
      h_switchstmt->child_list.push_back(new hNode(hNode::hdlopsEnum::hUnimpl));

    old_ret = h_ret;
    
 
    TraverseStmt(switchs->getBody());

    if (h_ret != old_ret) {
      NormalizeSwitchStmt(h_ret);
    // here need extra code to append non switchcase hcode into previous
    // switchcase group, which happens if the switchcase isn't wrapped in
    // a compound statement {}.
    
      h_switchstmt->child_list.push_back(h_ret);
    }

    // for (SwitchCase *sc = switchs->getSwitchCaseList(); sc != NULL;
    //      sc = sc->getNextSwitchCase()) {
    //   LLVM_DEBUG(llvm::dbgs() << "Switch case\n");
    //   LLVM_DEBUG(sc->dump(llvm::dbgs()));
    //   if (isa<DefaultStmt>(sc)) {
    //     LLVM_DEBUG(llvm::dbgs() << "Found default stmt in case\n");
    //   }
    //   else {
    //     ProcessSwitchCase(sc);
    //     h_switchstmt->child_list.push_back(h_ret);
    //   }
    // }
    // TraverseStmt(switchs->getBody());
    // h_switchbody = h_ret;
    // h_switchstmt->child_list.push_back(h_switchinit);

    // h_switchstmt->child_list.push_back(h_switchbody);
    h_ret = h_switchstmt;

    return true;
  }
  
  bool HDLBody::TraverseWhileStmt(WhileStmt *whiles) {
    hNodep h_whilestmt, h_whilecond, h_whilebody;
    LLVM_DEBUG(llvm::dbgs() << "While stmt\n");
    h_whilestmt = new hNode(hNode::hdlopsEnum::hWhileStmt);
    if (whiles->getConditionVariable()) {
      LLVM_DEBUG(
		 llvm::dbgs()
		 << "Variable declarations not handled in while condition, skipping\n");
    } else {
      // Get condition
      TraverseStmt(whiles->getCond());
      h_whilecond = h_ret;
    }

    // Get the body
    TraverseStmt(whiles->getBody());
    h_whilebody = h_ret;
    h_whilestmt->child_list.push_back(h_whilecond);
    h_whilestmt->child_list.push_back(h_whilebody);
    h_ret = h_whilestmt;

    return true;
  }

  // unfortunately clang ast doesn't do inheritance on the classes
  // so code is duplicated
  
    bool HDLBody::TraverseDoStmt(DoStmt *whiles) {
    hNodep h_whilestmt, h_whilecond, h_whilebody;
    LLVM_DEBUG(llvm::dbgs() << "Do stmt\n");
    h_whilestmt = new hNode(hNode::hdlopsEnum::hDoStmt);
    // Get condition
    TraverseStmt(whiles->getCond());
    h_whilecond = h_ret;
   
    // Get the body
    TraverseStmt(whiles->getBody());
    h_whilebody = h_ret;
    h_whilestmt->child_list.push_back(h_whilecond);
    h_whilestmt->child_list.push_back(h_whilebody);
    h_ret = h_whilestmt;

    return true;
  }

  
  // these two functions are so clumsy. The data structure should handle
  // multi-level symbol tables.
  
  string HDLBody::FindVname(NamedDecl *vard) {
    string newname = vname_map.find_entry_newn(vard, thismode==rthread); // set referenced bit if in thread
    if (newname == "")
      newname = mod_vname_map_.find_entry_newn(vard, thismode==rthread); // set referenced bit if in thread
    return newname;
  }

  string HDLBody::FindFname(FunctionDecl *funcd) {
    string newname = methodecls.find_entry_newn(funcd, thismode==rthread); // set referenced bit if in thread
    if (newname == "")
      newname = allmethodecls_.find_entry_newn(funcd, thismode==rthread); // set referenced bit if in thread
    return newname;
  }
  
  void HDLBody::AddVnames(hNodep &hvns) {
    LLVM_DEBUG(llvm::dbgs() << "Vname Dump\n");
    //for (auto const &var : vname_map.hdecl_name_map) {
    for (auto const &var : vname_map) {
      LLVM_DEBUG(llvm::dbgs() << "(" << var.first << "," << var.second.oldn
		 << ", " << var.second.newn << ")\n");
      if (add_info && (var.second.newn.find(gvar_prefix)==std::string::npos)) {
	// if this isn't a global variable 
	// mark this var decl as a renamed var decl and tack on the original name
	// used in later processing of hcode
	var.second.h_vardeclp->h_op = hNode::hdlopsEnum::hVardeclrn;
	var.second.h_vardeclp->child_list.push_back(new hNode(var.second.oldn, hNode::hdlopsEnum::hLiteral));
      }
      if (var.second.newn.find(gvar_prefix)==std::string::npos)
	// don't add global variable to local list
	hvns->child_list.push_back(var.second.h_vardeclp);
    }
  }

  hNodep HDLBody::NormalizeAssignmentChain(hNodep hinp) {
    // break up chain of assignments a = b = c = d = 0;
    // at entry there is a chain of at least two: a = b = 0;
    
    hNodep hassignchain = new hNode(hNode::hdlopsEnum::hCStmt);
    hNodep htmp = hinp; // (= a subtree)
    do {
      hNodep htmp2 = htmp->child_list[1]; // (= b subtree)
      htmp->child_list[1] = htmp2->child_list[0];  // (= a b)
      hassignchain->child_list.push_back(htmp);
      htmp = htmp2; // (= b subtree)
    }
    while (isAssignOp(htmp->child_list[1]));
    hassignchain->child_list.push_back(htmp);
    std::reverse(hassignchain->child_list.begin(), hassignchain->child_list.end());
    return hassignchain;
  }

  void HDLBody::NormalizeSwitchStmt(hNodep hswitchstmt) {
    if (hswitchstmt->child_list.size() == 0) return;
    hNodep hprev = hswitchstmt->child_list[0]; // should be a switchcase node
    for (int i= 1; i< hswitchstmt->child_list.size(); i++) {
      if ((hswitchstmt->child_list[i]->getopc() != hNode::hdlopsEnum::hSwitchCase) &&
	  (hswitchstmt->child_list[i]->getopc() != hNode::hdlopsEnum::hSwitchDefault)){
	hNodep htmp = new hNode((hswitchstmt->child_list[i])->getname(), (hswitchstmt->child_list[i])->getopc());
	htmp->child_list = (hswitchstmt->child_list[i])->child_list;
	hprev->append(htmp);
	hswitchstmt->child_list[i]->set(hNode::hdlopsEnum::hLast);	
      }
      else hprev = hswitchstmt->child_list[i];
    }
    hswitchstmt->child_list.erase(std::remove_if(hswitchstmt->child_list.begin(), hswitchstmt->child_list.end(),
						 [] (hNodep hp){return hp->getopc() == hNode::hdlopsEnum::hLast;}),
				  hswitchstmt->child_list.end());
  }

					  
  // CXXMethodDecl *HDLBody::getEMD() {
  //   return _emd;
  // }
}
