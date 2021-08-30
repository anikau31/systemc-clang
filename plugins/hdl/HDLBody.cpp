// clang-format off
#include "SystemCClang.h"
#include "HDLBody.h"
#include "HDLType.h"
#include "clang/Basic/OperatorKinds.h"
#include "clang/Basic/Diagnostic.h"
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

  HDLBody::HDLBody(clang::DiagnosticsEngine &diag_engine, const ASTContext &ast_context, hdecl_name_map_t &mod_vname_map) :
    diag_e{diag_engine}, ast_context_{ast_context}, mod_vname_map_{mod_vname_map} {
      LLVM_DEBUG(llvm::dbgs() << "Entering HDLBody constructor\n");
    }

  void HDLBody::Run(Stmt *stmt, hNodep &h_top, HDLBodyMode runmode) 
 
  {
    LLVM_DEBUG(llvm::dbgs() << "Entering HDLBody Run Method\n");
    h_ret = NULL;
    add_info = (runmode == rmodinit);
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
    // if (!mod_vname_map_.empty())
    // 	vname_map.insertall(mod_vname_map_);
     methodecls.set_prefix("_func_");
     bool ret1 = TraverseStmt(stmt);
     AddVnames(h_top);
     h_top->child_list.push_back(h_ret);
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
				     new hNode(val.toString(10), hNode::hdlopsEnum::hLiteral));
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
      h_ret = new hNode(hNode::hdlopsEnum::hBreak);
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
    } else {
      if (isa<CXXConstructExpr>(stmt)) {
	CXXConstructExpr *exp = (CXXConstructExpr *)stmt;
	if ((exp->getNumArgs() == 1) && (isa<IntegerLiteral>(exp->getArg(0)))) {
	  LLVM_DEBUG(llvm::dbgs()
		     << "CXXConstructExpr followed by integer literal found\n");
	  LLVM_DEBUG(exp->dump(llvm::dbgs(), ast_context_));
	  IntegerLiteral *lit = (IntegerLiteral *)exp->getArg(0);
	  string s = lit->getValue().toString(10, true);
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
	  hNodep htmp = NormalizeHcode(h_ret); // break up assignment chain
	  //h_ret = NormalizeHcode(h_ret); // break up assignment chain
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
	ProcessVarDecl(
		       vardecl);  // adds it to the list of renamed local variables
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
    hNodep h_vardecl = h_varlist->child_list.back();

    h_ret = NULL;

    if (Expr *declinit = vardecl->getInit()) {
      TraverseStmt(declinit);
    }

    vname_map.add_entry(vardecl, vardecl->getName().str(), h_vardecl);
    //string newn = lname.newname();
    // prepend original name to new name so it retains association
    //newn = vardecl->getName().str() + newn;
    //h_vardecl->set(newn);  // replace original name with new name
    //names_t names = {vardecl->getName().str(), newn, h_vardecl};
    //vname_map[vardecl] = names;

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
    if ((opcodestr == ",") && (lutil.isSCType(exprtypstr) || lutil.isSCBuiltinType(exprtypstr))){
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
    string s = lit->getValue().toString(10, true);
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
        new hNode(cd->getInitVal().toString(10), hNode::hdlopsEnum::hLiteral);
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
	h_ret = new hNode(result.Val.getInt().toString(10),
			  hNode::hdlopsEnum::hLiteral);
	return true;
      }
    }
    if (isa<FunctionDecl>(value)) {
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
	if (!add_info) methodecls.add_entry((FunctionDecl *)value, qualfuncname,  hfuncall);
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
	h_ret = new hNode(name, hNode::hdlopsEnum::hNoop);
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
    LLVM_DEBUG(llvm::dbgs() << "In TraverseArraySubscriptExpr, tree follows\n");
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
    LLVM_DEBUG(llvm::dbgs()
	       << "In TraverseCXXMemberCallExpr, printing implicit object arg\n");
    // Retrieves the implicit object argument for the member call.
    // For example, in "x.f(5)", this returns the sub-expression "x".
    Expr *arg = (callexpr->getImplicitObjectArgument())->IgnoreImplicit();

    LLVM_DEBUG(arg->dump(llvm::dbgs(), ast_context_));
    QualType argtyp = arg->getType();
    LLVM_DEBUG(llvm::dbgs() << "type of x in x.f(5) is " << argtyp.getAsString()
	       << "\n");

    string methodname = "NoMethod", qualmethodname = "NoQualMethod";
    CXXMethodDecl *methdcl = callexpr->getMethodDecl();

    // LLVM_DEBUG(llvm::dbgs() << "methoddecl follows\n");
    // LLVM_DEBUG(methdcl->dump(llvm::dbgs());
    if (isa<NamedDecl>(methdcl) && methdcl->getDeclName()) {
      methodname = methdcl->getNameAsString();
      qualmethodname = methdcl->getQualifiedNameAsString();
      // make_ident(qualmethodname);
      //      methodecls[qualmethodname] = methdcl;  // put it in the set of
      //      method decls

      LLVM_DEBUG(llvm::dbgs() << "here is method printname " << methodname
		 << " and qual name " << qualmethodname << " \n");
      if (methodname.compare(0, 8, "operator") ==
	  0) {  // 0 means compare =, 8 is len("operator")
	// the conversion we know about, can be skipped
	LLVM_DEBUG(llvm::dbgs() << "Found operator conversion node\n");
	TraverseStmt(arg);
	return true;
      }
    }

    hNode::hdlopsEnum opc;
    hNode * h_callp = NULL;
    LLVM_DEBUG(llvm::dbgs() << "found " << methodname << "\n");

    // if type of x in x.f(5) is primitive sc type (sc_in, sc_out, sc_inout,
    // sc_signal and method name is either read or write, generate a SigAssignL|R
    // -- FIXME need to make sure it is templated to a primitive type

    if ((methodname == "read") && (lutil.isSCType(qualmethodname)))
      opc = hNode::hdlopsEnum::hSigAssignR;
    else if ((methodname == "write") && (lutil.isSCType(qualmethodname)))
      opc = hNode::hdlopsEnum::hSigAssignL;
    else if (methodname == "wait")
      opc = hNode::hdlopsEnum::hWait;
    else if (lutil.isSCType(qualmethodname)) {  // operator from simulation library
      opc = hNode::hdlopsEnum::hNoop;
    } else {
      opc = hNode::hdlopsEnum::hMethodCall;
      lutil.make_ident(qualmethodname);
      if (add_info) qualmethodname+= ":"+ methodname;  // include unqualified name for future hcode processing !!!
      //methodecls[qualmethodname] = methdcl;  // put it in the set of method decls
      h_callp = new hNode(qualmethodname, opc);
      // don't add this method to methodecls if processing modinit
      if (!add_info) methodecls.add_entry(methdcl,qualmethodname, h_callp);
      methodname = qualmethodname;
    }

    if (h_callp == NULL) h_callp = new hNode(methodname, opc);  // list to hold call expr node

    hNodep save_hret = h_ret;
    TraverseStmt(arg);  // traverse the x in x.f(5)

    if (h_ret && (h_ret != save_hret)) h_callp->child_list.push_back(h_ret);

    for (auto arg : callexpr->arguments()) {
      hNodep save_h_ret = h_ret;
      TraverseStmt(arg);
      if (h_ret != save_h_ret) h_callp->child_list.push_back(h_ret);
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
    LLVM_DEBUG(llvm::dbgs() << "Type with name " << operatortype << " follows\n");
    LLVM_DEBUG(opcall->getType()->dump(llvm::dbgs(), ast_context_));

    if (lutil.isSCBuiltinType(operatortype) || lutil.isSCType(operatortype) ||
	(opcall->getType())->isBuiltinType() || (operatorname == "=") ||
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
	  h_operop = new hNode(operatorname, hNode::hdlopsEnum::hBinop);
	  if ((operatorname == ",") && (lutil.isSCBuiltinType(operatortype) || lutil.isSCType(operatortype)))
	    h_operop->set("concat"); // overloaded comma is concat for sc types
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
    LLVM_DEBUG(llvm::dbgs() << "In TraverseMemberExpr\n");
    string nameinfo = (memberexpr->getMemberNameInfo()).getName().getAsString();
    LLVM_DEBUG(llvm::dbgs() << "name is " << nameinfo
	       << ", base and memberdecl trees follow\n");
    LLVM_DEBUG(llvm::dbgs() << "base is \n");
    LLVM_DEBUG(memberexpr->getBase()->dump(llvm::dbgs(), ast_context_); );
    LLVM_DEBUG(llvm::dbgs() << "memberdecl is \n");
    LLVM_DEBUG(memberexpr->getMemberDecl()->dump(llvm::dbgs()));

    // traverse the memberexpr base in case it is a nested structure
    hNodep old_h_ret = h_ret;
    TraverseStmt(memberexpr->getBase());  // get hcode for the base
    if (h_ret != old_h_ret) {
      if (h_ret->h_op == hNode::hdlopsEnum::hVarref){
	// concatenate base name in front of field name
	hNodep memexprnode = new hNode(h_ret->h_name + "##" + nameinfo,
				       hNode::hdlopsEnum::hVarref);
	delete h_ret;
	h_ret = memexprnode;  // replace returned h_ret with single node, field
	// names concatenated
	return true;
      }
      else {
	LLVM_DEBUG(llvm::dbgs() << "Value returned from member expr base was not Varref\n");
	h_ret->print(llvm::dbgs());
	string newname = FindVname(memberexpr->getMemberDecl());
	LLVM_DEBUG(llvm::dbgs() << "member with base expr new name is " << newname << "\n");
	hNodep memexprnode = new hNode(newname.empty()? nameinfo : newname, hNode::hdlopsEnum::hVarref);
	memexprnode->child_list.push_back(h_ret);
	h_ret = memexprnode;
	return true;
      }

    }

    string newname = FindVname(memberexpr->getMemberDecl());
    LLVM_DEBUG(llvm::dbgs() << "member expr new name is " << newname << "\n");

    h_ret = new hNode(newname.empty()? nameinfo : newname, hNode::hdlopsEnum::hVarref);

    return true;
  }

  bool HDLBody::TraverseCallExpr(CallExpr *callexpr) {
    hNodep hcall = new hNode(hNode::hdlopsEnum::hMethodCall);
    hNodep save_hret = h_ret;

    if (isa<FunctionDecl>(callexpr->getCalleeDecl()) &&
	((FunctionDecl *)callexpr->getCalleeDecl())->isConstexpr()) {
      Expr::EvalResult res;
      if (callexpr->EvaluateAsRValue(
				     res, callexpr->getCalleeDecl()->getASTContext())) {
	h_ret =
          new hNode(res.Val.getInt().toString(10), hNode::hdlopsEnum::hLiteral);
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
				     new hNode(val.toString(10), hNode::hdlopsEnum::hLiteral));
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
    if (h_ret != old_ret) h_switchstmt->child_list.push_back(h_ret);

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

  string HDLBody::FindVname(NamedDecl *vard) {
    string newname = vname_map.find_entry_newn(vard);
    if (newname == "")
      newname = mod_vname_map_.find_entry_newn(vard);
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

  hNodep HDLBody::NormalizeHcode(hNodep hinp) {
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
					  
  // CXXMethodDecl *HDLBody::getEMD() {
  //   return _emd;
  // }
}
