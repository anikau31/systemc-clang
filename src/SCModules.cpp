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

void SCModules::analyze_array_base(Expr *base) {
    if (CastExpr *cast = dyn_cast<CastExpr>(base)) {
        Expr *subExpr = cast->getSubExpr();

        _os << "base subexpr name: " << subExpr->getStmtClassName() << "\n";

        if (DeclRefExpr *declRef = dyn_cast<DeclRefExpr>(subExpr)) {
            analyze_decl_ref_expr(declRef);
        }
    }
}

void SCModules::analyze_lhs(Expr *expr) {
    if (ArraySubscriptExpr *array = dyn_cast<ArraySubscriptExpr>(expr)) {
        Expr *base = array->getBase();
        Expr *idx = array->getIdx();

        _os << "base name: " << base->getStmtClassName() << "\n";
        analyze_array_base(base);

        _os << "idx name: " << idx->getStmtClassName() << "\n";
    }
}

void SCModules::analyze_rhs(Expr *expr) {
    if (ArraySubscriptExpr *array = dyn_cast<ArraySubscriptExpr>(expr)) {
        Expr *base = array->getBase();
        Expr *idx = array->getIdx();

        _os << "base name: " << base->getStmtClassName() << "\n";
        analyze_array_base(base);

        _os << "idx name: " << idx->getStmtClassName() << "\n";
    }
    else if (CastExpr *cast = dyn_cast<CastExpr>(expr)) {
        analyze_rhs(cast->getSubExpr());
    }
}

void SCModules::analyze_expr(Expr *expr) {
    expr->dump();

    if (BinaryOperator *binOp = dyn_cast<BinaryOperator>(expr)) {
        if (binOp->isAssignmentOp()) {
            analyze_lhs(binOp->getLHS());
            analyze_rhs(binOp->getRHS());
        }
        else if (binOp->isCompoundAssignmentOp()) {
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

        _os << "expr name: " << expr->getStmtClassName() << "\n";
        analyze_expr(expr);
    }

    _os << "NEW FORLOOP END\n";
}

bool SCModules::VisitForStmt(ForStmt *fstmt) {
    analyze_data_struct(fstmt->getBody());
    return true;
}
