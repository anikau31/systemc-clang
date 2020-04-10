#include "XlatEntryMethod.h"

// We have to use the Traverse pattern rather than Visitor 
// because we need control to come back to the point of call
// so that the generated tree can be returned back up the 
// call chain.

// used https://github.com/super-ast/cpptranslate as example

using namespace std;
using namespace hnode;

// MACRO FOR TRAVERSING
#define TRY_TO(CALL_EXPR)                                                      \
do {                                                                           \
  if (!getDerived().CALL_EXPR)                                                 \
    return false;                                                              \
} while (0)

XlatMethod::XlatMethod(CXXMethodDecl * emd, hNodep & h_top, llvm::raw_ostream & os):
  os_(os){ 
  os_ << "Entering XlatMethod constructor, has body is " << emd->hasBody()<< "\n";
  
  h_ret = NULL;
  cnt = 0;
  bool ret1 = TraverseStmt(emd->getBody());
  VnameDump();
  h_top = h_ret;
  os_ << "Exiting XlatMethod constructor for method body\n";
}

// leaving this in for the future in case 
// we need to traverse starting at a lower point in the tree.

XlatMethod::XlatMethod(Stmt * stmt, hNodep & h_top, llvm::raw_ostream & os):
  os_(os){
  h_ret = NULL;
  bool ret1 = TraverseStmt(stmt);
  h_top = h_ret;
  os_ << "Exiting XlatMethod constructor for stmt\n";
}

XlatMethod::~XlatMethod() {
  os_ << "[[ Destructor XlatMethod ]]\n";
}
  
// order of checking is important as some exprs
// inherit from binaryoperator

bool XlatMethod::TraverseStmt(Stmt *stmt) {
  os_ << "In TraverseStmt\n";

  if (isa<CompoundStmt>(stmt)) {
    os_ << "calling traverse compoundstmt from traversestmt\n";
    TraverseCompoundStmt((CompoundStmt *)stmt);
  }
  else if (isa<DeclStmt>(stmt)) {
    TRY_TO(TraverseDeclStmt((DeclStmt *) stmt));
  }
  else if (isa<CallExpr>(stmt)) {
    if (CXXOperatorCallExpr *opercall = dyn_cast<CXXOperatorCallExpr>(stmt)) {
      os_ << "found cxxoperatorcallexpr\n";
      TraverseCXXOperatorCallExpr(opercall);
    }
    else if (isa<CXXMemberCallExpr>(stmt)) {
      TraverseCXXMemberCallExpr((CXXMemberCallExpr *) stmt);
    }
    else {
      h_ret = new hNode(stmt->getStmtClassName(), hNode::hdlopsEnum::hUnimpl);
      os_ << "found a call expr" << " AST follows\n ";
      stmt->dump();
      return RecursiveASTVisitor::TraverseStmt(stmt);
    }
  }
  else if (isa<BinaryOperator>(stmt)) {
    TRY_TO(TraverseBinaryOperator((BinaryOperator *) stmt));
  }
  else if (isa<UnaryOperator>(stmt)) {
    TRY_TO(TraverseUnaryOperator((UnaryOperator *) stmt));
  }
  else if (isa<MaterializeTemporaryExpr>(stmt)) {
    TRY_TO(TraverseStmt(((MaterializeTemporaryExpr *) stmt)->GetTemporaryExpr()));
  }

  else if (isa<IntegerLiteral>(stmt)) {
    TRY_TO(TraverseIntegerLiteral((IntegerLiteral *)stmt));
  }  
  else if (isa<CXXBoolLiteralExpr>(stmt)) {
    TRY_TO(TraverseCXXBoolLiteralExpr((CXXBoolLiteralExpr *)stmt));
  }
  else if (isa<IfStmt>(stmt)){
    os_ << "Found if stmt\n";
    TRY_TO(TraverseIfStmt((IfStmt *)stmt));
  }
  else if (isa<ForStmt>(stmt)) {
    os_ << "Found if stmt\n";
    TRY_TO(TraverseForStmt((ForStmt *)stmt));
  }
  else if (isa<SwitchStmt>(stmt)){
    os_ << "Found switch stmt, not yet implemented\n";
    return true;
  }
  else {  
    os_ << "stmt type " << stmt->getStmtClassName() << " not recognized, calling default recursive ast visitor\n";
    hNodep oldh_ret = h_ret;
    RecursiveASTVisitor::TraverseStmt(stmt);      
    if (h_ret != oldh_ret) {
      os_ << "default recursive ast visitor called - returning translation\n";
      return true;
    }
    for( auto arg : stmt->children()) {
      os_ << "child stmt type " << ((Stmt *) arg)->getStmtClassName() << "\n";
      TRY_TO(TraverseStmt(arg));
      if (h_ret == oldh_ret) {
	os_ << "child stmt not handled\n";
	// no output generated
	h_ret = new hNode(arg->getStmtClassName(), hNode::hdlopsEnum::hUnimpl);
      }
    }
  }

  return true;  
}

bool XlatMethod::TraverseCompoundStmt(CompoundStmt* cstmt) {
    // Traverse each statement and append it to the array
  hNodep h_cstmt = new hNode(hNode::hdlopsEnum::hCStmt);
  
  for (clang::Stmt* stmt : cstmt->body()) {
    TRY_TO(TraverseStmt(stmt));
    if (h_ret) {
      h_cstmt->child_list.push_back(h_ret);
    }
    else os_ << "stmt result was empty\n";
    h_ret = NULL;
  }

  h_ret = h_cstmt;
  return true;
}

bool XlatMethod::TraverseDeclStmt(DeclStmt * declstmt) {
  hNodep h_varlist = NULL;
  if (!declstmt->isSingleDecl()) {
      h_varlist = new hNode(hNode::hdlopsEnum::hPortsigvarlist);
    }
  // from https://clang.llvm.org/doxygen/DeadStoresChecker_8cpp_source.html
  for (auto *DI : declstmt->decls())
    if (DI) {
	auto *vardecl = dyn_cast<VarDecl>(DI);
	if (!vardecl)
	  continue;
	hNodep h_vardecl = new hNode(vardecl->getName(), hNode::hdlopsEnum::hVardecl);
	ProcessVarDecl(vardecl, h_vardecl);
	if (h_varlist) h_varlist->child_list.push_back(h_vardecl);
	else h_varlist = h_vardecl; // single declaration
      }
  h_ret = h_varlist;
  return true;
}

bool XlatMethod::ProcessVarDecl( VarDecl * vardecl, hNodep &h_vardecl) {
  os_ << "ProcessVarDecl var name is " << vardecl->getName() << "\n";
  names_t names = {vardecl->getName(), newname()};
  h_vardecl->set(names.newn); // replace original name with new name
  vname_map[vardecl] = names;
  hNodep h_typeinfo = new hNode( hNode::hdlopsEnum::hTypeinfo);
  QualType q = vardecl->getType();
  const Type *tp = q.getTypePtr();
  os_ << "ProcessVarDecl type name is " << q.getAsString() << "\n";
  FindTemplateTypes *te = new FindTemplateTypes();

  te->Enumerate(tp);
 
  scpar::FindTemplateTypes::type_vector_t ttargs = te->getTemplateArgumentsType();
  for (auto const &targ : ttargs) {

    //h_typeinfo->child_list.push_back(new hNode("\"" + targ.getTypeName() + "\"", hNode::hdlopsEnum::hType));
    string tmps = targ.getTypeName();
    lutil.make_ident(tmps);
    h_typeinfo->child_list.push_back(new hNode(tmps, hNode::hdlopsEnum::hType));

  }
  
  if (h_typeinfo->child_list.empty()) { // front end didn't parse type info
    //h_typeinfo->child_list.push_back(new hNode("\"" + q.getAsString() + "\"", hNode::hdlopsEnum::hType));
    string tmps = q.getAsString();
    lutil.make_ident(tmps);
    h_typeinfo->child_list.push_back(new hNode(tmps, hNode::hdlopsEnum::hType));
  }
				     
  h_vardecl->child_list.push_back(h_typeinfo);
  if (Expr * declinit = vardecl->getInit()) {

    TraverseStmt(declinit);
    if (h_ret) {
      hNodep varinitp = new hNode(hNode::hdlopsEnum::hVarInit);
      varinitp->child_list.push_back(h_ret);
      h_vardecl->child_list.push_back(varinitp);
    }
  }
  return true;
}

bool XlatMethod::TraverseBinaryOperator(BinaryOperator* expr) 
{ 
  // ... handle expr. Can use
  //bool isIntegerConstantExpr(llvm::APSInt &Result, const ASTContext &Ctx,
  //                          SourceLocation *Loc = nullptr,
  //                         bool isEvaluated = true) const;

  hNodep  h_binop = new hNode(expr->getOpcodeStr(), hNode::hdlopsEnum::hBinop); // node to hold binop expr
  os_ << "in TraverseBinaryOperator, opcode is " << expr->getOpcodeStr() << "\n";

  TRY_TO(TraverseStmt(expr->getLHS()));
  h_binop->child_list.push_back(h_ret);

  hNodep save_h_ret = h_ret;
  TRY_TO(TraverseStmt(expr->getRHS()));
  if (h_ret == save_h_ret)
    h_binop->child_list.push_back(new hNode(hNode::hdlopsEnum::hUnimpl));
  else
      h_binop->child_list.push_back(h_ret);

  h_ret = h_binop;

  return true;

} 

bool XlatMethod::TraverseUnaryOperator(UnaryOperator* expr) 
{ 
  os_ << "in TraverseUnaryOperatory expr node is \n";
  expr->dump(os_);
  
  auto opcstr = expr->getOpcode();
  
  hNodep  h_unop = new hNode(expr->getOpcodeStr(opcstr), hNode::hdlopsEnum::hUnop); // node to hold unop expr

  TRY_TO(TraverseStmt(expr->getSubExpr()));
  h_unop->child_list.push_back(h_ret);

  h_ret = h_unop;

  return true;

} 

bool XlatMethod::TraverseIntegerLiteral(IntegerLiteral * lit)
{
  os_ << "In integerliteral\n";
  string s = lit->getValue().toString(10, true);
  h_ret = new hNode(s, hNode::hdlopsEnum::hLiteral);
  
  return true;
}

bool XlatMethod::TraverseCXXBoolLiteralExpr(CXXBoolLiteralExpr * b) {
  os_ << "In boollitexpr\n";
  bool v = b->getValue();
  h_ret = new hNode(v?"1":"0", hNode::hdlopsEnum::hLiteral);

  return true;
}

bool XlatMethod::TraverseDeclRefExpr(DeclRefExpr* expr) 
{ 
  // ... handle expr
  // get a var name
  os_ << "In TraverseDeclRefExpr\n";
  string name = (expr->getNameInfo()).getName().getAsString();
  os_ << "name is " << name << "\n";
  string newname = "";
  auto vname_it{vname_map.find(expr->getDecl())};
      if (vname_it != vname_map.end()) {
	newname = vname_map[expr->getDecl()].newn;
      }
  os_ << "new name is "<< newname << "\n";
  h_ret = new hNode(newname.empty() ? name : newname, hNode::hdlopsEnum::hVarref);
  return true; 
}

bool XlatMethod::TraverseArraySubscriptExpr(ArraySubscriptExpr* expr) {
  os_ << "In TraverseArraySubscriptExpr, tree follows\n";
  expr->dump(os_);
  hNodep h_arrexpr = new hNode("ARRAYSUBSCRIPT", hNode::hdlopsEnum::hBinop);
  TRY_TO(TraverseStmt(expr->getLHS()));
  h_arrexpr->child_list.push_back(h_ret);
  TRY_TO(TraverseStmt(expr->getRHS()));
  h_arrexpr->child_list.push_back(h_ret);
  h_ret = h_arrexpr;
  return true;
}

bool XlatMethod::TraverseCXXMemberCallExpr(CXXMemberCallExpr *callexpr) {
  os_ << "In TraverseCXXMemberCallExpr, printing implicit object arg\n";
      // Retrieves the implicit object argument for the member call.
    //For example, in "x.f(5)", this returns the sub-expression "x".
    Expr *arg = (callexpr->getImplicitObjectArgument())->IgnoreImplicit();
 
    arg->dump(os_);
    QualType argtyp = arg->getType();
    os_ << "type of x in x.f(5) is " << argtyp.getAsString() << "\n";
    
    string methodname = "NoMethod", qualmethodname = "NoQualMethod";
    CXXMethodDecl * methdcl = callexpr->getMethodDecl();

    
    // os_ << "methoddecl follows\n";
    // methdcl->dump();
    if (isa<NamedDecl>(methdcl) && methdcl->getDeclName()) {
    
      methodname = methdcl->getNameAsString();
      qualmethodname = methdcl->getQualifiedNameAsString();
      os_ << "here is qualmethod printname " <<qualmethodname << "\n";
      //make_ident(qualmethodname);
      //      methodecls[qualmethodname] = methdcl;  // put it in the set of method decls
      
      os_ << "here is method printname " << methodname << " and qual name " << qualmethodname << " \n";
      if (methodname.compare(0, 8, "operator")==0) { // 0 means compare =, 8 is len("operator")
	// the conversion we know about, can be skipped
	os_ << "Found operator conversion node\n";
	TRY_TO(TraverseStmt(arg)); 
	return true;
      }

    }

    hNode::hdlopsEnum opc; 
    
    os_ << "found " << methodname << "\n";

    // if type of x in x.f(5) is primitive sc type (sc_in, sc_out, sc_inout, sc_signal
    // and method name is either read or write,
    // generate a SigAssignL|R -- FIXME need to make sure it is templated to a primitive type

    if ((methodname == "read") && (lutil.isSCType(qualmethodname))) opc = hNode::hdlopsEnum::hSigAssignR;
    else if ((methodname == "write") && (lutil.isSCType(qualmethodname))) opc = hNode::hdlopsEnum::hSigAssignL;
    else {
      opc = hNode::hdlopsEnum::hMethodCall;
      lutil.make_ident(qualmethodname);
      methodecls[qualmethodname] = methdcl;  // put it in the set of method decls
      methodname = qualmethodname;
    }


    hNode * h_callp = new hNode(methodname, opc); // list to hold call expr node

    TRY_TO(TraverseStmt(arg)); // traverse the x in x.f(5)

    if (h_ret) h_callp -> child_list.push_back(h_ret);

    for (auto arg : callexpr->arguments()) {
      hNodep save_h_ret = h_ret;
      TRY_TO(TraverseStmt(arg));
      if (h_ret != save_h_ret) h_callp->child_list.push_back(h_ret);
    }
    h_ret = h_callp;	  
    return true;
}

bool XlatMethod::isLogicalOp(clang::OverloadedOperatorKind opc) {
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

bool XlatMethod::TraverseCXXOperatorCallExpr(CXXOperatorCallExpr * opcall) {

  os_ << "In TraverseCXXOperatorCallExpr\n";
  if ((opcall->isAssignmentOp())|| 
      (isLogicalOp(opcall->getOperator()))) {
    if (opcall->getNumArgs() == 2) {
      os_ << "assignment or logical operator, 2 args\n";
      hNodep h_assignop = new hNode ("=", hNode::hdlopsEnum::hBinop); // node to hold assignment expr
      TRY_TO(TraverseStmt(opcall->getArg(0)));
      h_assignop->child_list.push_back(h_ret);
      hNodep save_h_ret = h_ret;
      TRY_TO(TraverseStmt(opcall->getArg(1)));
      if (h_ret == save_h_ret) h_assignop->child_list.push_back(new hNode(hNode::hdlopsEnum::hUnimpl));
      else h_assignop->child_list.push_back(h_ret);
      h_ret = h_assignop;
      opcall->getArg(0)->dump(os_);
      opcall->getArg(1)->dump(os_);
      return true;
    }
  }
  os_ << "not yet implemented operator call expr, opc is " << clang::getOperatorSpelling(opcall->getOperator()) << " num arguments " << opcall->getNumArgs() << " skipping\n";
  h_ret = new hNode(hNode::hdlopsEnum::hUnimpl);
  return true;
}

bool XlatMethod::TraverseMemberExpr(MemberExpr *memberexpr){
  os_ << "In TraverseMemberExpr\n";
  string nameinfo = (memberexpr->getMemberNameInfo()).getName().getAsString();
  os_ << "name is " << nameinfo << ", base and memberexpr trees follow\n";
  os_ << "base is \n";
  memberexpr->getBase()->dump(os_);
  os_ << "memberdecl is \n";
  memberexpr->getMemberDecl()->dump(os_);

  // traverse the memberexpr in case it is a nested structure
  auto *baseexpr = dyn_cast<MemberExpr>(memberexpr->getBase()); // nested field decl
  if (baseexpr) {

    hNodep old_h_ret = h_ret;
    TRY_TO(TraverseStmt(baseexpr));
    if (h_ret != old_h_ret) {
      if (h_ret->h_op == hNode::hdlopsEnum::hLiteral) {
	//concatenate base name in front of field name
	hNodep memexprnode = new hNode(h_ret->h_name+"_"+nameinfo, hNode::hdlopsEnum::hLiteral);
	delete h_ret;
	h_ret = memexprnode;  // replace returned h_ret with single node, field names concatenated
	return h_ret;
      }
    }
    
    // FIXME Only handling one level right now
    //const Type *unqualtyp = baseexpr->getType()->getUnqualifiedDesugaredType();
    //QualType q = unqualtyp->getCanonicalTypeInternal();
    //QualType q = (baseexpr->getType())->getDesugaredType();
     //string basestr = tp->getAsString();
     //nameinfo.insert((size_t) 0, q.getAsString()); //baseexpr->getMemberNameInfo().getName().getAsString() + "_") ;
     //make_ident(nameinfo);
  }

    
  h_ret = new hNode(nameinfo, hNode::hdlopsEnum::hLiteral);

  return true;
}

bool XlatMethod::TraverseIfStmt(IfStmt *ifs) {
  hNodep h_ifstmt, h_ifc = NULL, h_ifthen = NULL, h_ifelse = NULL;
  h_ifstmt = new hNode(hNode::hdlopsEnum::hIfStmt);
  if (ifs->getConditionVariable()) {
      // Variable declarations are not allowed in if conditions
    os_ << "Variable declarations are not allowed in if conditions, skipping\n";
    return true;
  }
  else {
    TRY_TO(TraverseStmt(ifs->getCond()));
    h_ifc = h_ret;
  }
  TRY_TO(TraverseStmt(ifs->getThen()));
  if (h_ret != h_ifc) // unchanged if couldn't translate the then clause
    h_ifthen = h_ret;

  if (ifs->getElse()) {
    TRY_TO(TraverseStmt(ifs->getElse()));
    if ((h_ret != h_ifc) && (h_ret != h_ifthen))
      h_ifelse = h_ret;
  }
  h_ifstmt->child_list.push_back(h_ifc);
  h_ifstmt->child_list.push_back(h_ifthen);
  if(h_ifelse) h_ifstmt->child_list.push_back(h_ifelse);
  h_ret = h_ifstmt;
  return true;
}

bool XlatMethod::TraverseForStmt(ForStmt *fors) {
  hNodep h_forstmt, h_forinit, h_forcond, h_forinc, h_forbody;
  os_ << "For stmt\n";
  h_forstmt = new hNode(hNode::hdlopsEnum::hForStmt);
  if (isa<CompoundStmt>(fors->getInit()))
    os_ << "Compound stmt not handled in for init, skipping\n";
  else TRY_TO(TraverseStmt(fors->getInit()));
  h_forinit = h_ret;
  TRY_TO(TraverseStmt(fors->getCond()));
  h_forcond = h_ret;
  TRY_TO(TraverseStmt(fors->getInc()));
  h_forinc = h_ret;
  os_ << "For loop body\n";
  fors->getBody()->dump(os_);
  TRY_TO(TraverseStmt(fors->getBody()));
  h_forbody = h_ret;
  h_forstmt->child_list.push_back(h_forinit);
  h_forstmt->child_list.push_back(h_forcond);
  h_forstmt->child_list.push_back(h_forinc);
  h_forstmt->child_list.push_back(h_forbody);
  h_ret = h_forstmt;
  
  return true;
}

// wouldn't appear in a SC_METHOD, but put it in anyway
// won't put in do stmt for now
bool XlatMethod::TraverseWhileStmt(WhileStmt *whiles) {
  hNodep h_whilestmt,  h_whilecond, h_whilebody;
  os_ << "While stmt\n";
  h_whilestmt = new hNode(hNode::hdlopsEnum::hWhileStmt);
  if (whiles->getConditionVariable()) {
    os_ << "Variable declarations not handled in while condition, skipping\n";
  }
  else {
    // Get condition
    TRY_TO(TraverseStmt(whiles->getCond()));
    h_whilecond = h_ret;
  }

  // Get the body
  TRY_TO(TraverseStmt(whiles->getBody()));
  h_whilebody = h_ret;
  h_whilestmt->child_list.push_back(h_whilecond);
  h_whilestmt->child_list.push_back(h_whilebody);
  h_ret = h_whilestmt;
  
  return true;
}

void XlatMethod::VnameDump() {
  os_ << "Vname Dump\n";
  for (auto const &var : vname_map) {
    os_ << "(" << var.first << "," << var.second.oldn << ", " << var.second.newn << ")\n";
						  
  }
}


// CXXMethodDecl *XlatMethod::getEMD() {
//   return _emd;
// }

