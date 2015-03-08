#include "SCModules.h"
#include "FindModule.h"
#include "clang/AST/ASTContext.h"
using namespace scpar;

SCModules::SCModules(TranslationUnitDecl * tuDecl, llvm::raw_ostream & os):
_os(os)
{
  assert(!(tuDecl == NULL));
  TraverseDecl(tuDecl);
}

bool SCModules::VisitCXXRecordDecl(CXXRecordDecl * cxxDecl)
{
  FindModule mod(cxxDecl, _os);

  if (!mod.isSystemCModule()) {
    return true;
  }
  string modName = mod.getModuleName();
  _moduleMap.insert(modulePairType(modName, cxxDecl));
  return true;
}

SCModules::moduleMapType SCModules::getSystemCModulesMap()
{
  return _moduleMap;
}

void SCModules::printSystemCModulesMap()
{
  _os << "\n================= SCModules ================";
  _os << "\n Print SystemC Module Map";
  for (moduleMapType::iterator mit = _moduleMap.begin();
       mit != _moduleMap.end(); mit++) {
    _os << "\n:> name: " << mit->first << ", CXXRecordDecl*: " << mit->second;
  }
  _os << "\n================= END SCModules ================";
  _os << "\n\n";
}

void SCModules::analyze_decl_ref_expr(DeclRefExpr *declRef) {
    ValueDecl *val = declRef->getDecl();

    QualType type = val->getType();

    std::pair<uint64_t, unsigned> fieldInfo =
        val->getASTContext().getTypeInfo(val->getType());
    uint64_t typeSize = fieldInfo.first;
    unsigned fieldAlign = fieldInfo.second;

    _os << "base type: " << type.getCanonicalType().getAsString()
        << " size (bits): " << typeSize
        << " align (bits): " << fieldAlign
        << "\n";
}

void SCModules::analyze_array_base(Expr *base, bool isLHS) {
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
            analyze_decl_ref_expr(declRef);
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

void SCModules::analyze_lhs(Expr *expr) {
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

void SCModules::analyze_rhs(Expr *expr) {
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

void SCModules::analyze_expr(Expr *expr) {
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

void SCModules::analyze_data_struct(Stmt *stmtList) {
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
        _os << "LHS ValueDecl AST Node: " << *itr << "\n";
    }

    for (std::set<ValueDecl*>::iterator itr = rhs_decls.begin();
         itr != rhs_decls.end();
         ++itr) {
        _os << "RHS ValueDecl AST Node: " << *itr << "\n";
    }

    _os << "NEW FORLOOP END\n";
}

bool SCModules::VisitForStmt(ForStmt *fstmt) {
    analyze_data_struct(fstmt->getBody());
    return true;
}
