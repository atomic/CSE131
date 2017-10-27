/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */

#include <string.h>
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "symtable.h"
#include "errors.h"

IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
}
void IntConstant::PrintChildren(int indentLevel) { 
    printf("%d", value);
}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
}
void BoolConstant::PrintChildren(int indentLevel) { 
    printf("%s", value ? "true" : "false");
}

VarExpr::VarExpr(yyltype loc, Identifier *ident) : Expr(loc) {
    Assert(ident != NULL);
    this->id = ident;
}

void VarExpr::PrintChildren(int indentLevel) {
    id->Print(indentLevel+1);
}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}

void Operator::PrintChildren(int indentLevel) {
    printf("%s",tokenString);
}

bool Operator::IsOp(const char *op) const {
    return strcmp(tokenString, op) == 0;
}

CompoundExpr::CompoundExpr(Expr *l, Operator *o, Expr *r) 
  : Expr(Join(l->GetLocation(), r->GetLocation())) {
    Assert(l != NULL && o != NULL && r != NULL);
    (op=o)->SetParent(this);
    (left=l)->SetParent(this); 
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Operator *o, Expr *r) 
  : Expr(Join(o->GetLocation(), r->GetLocation())) {
    Assert(o != NULL && r != NULL);
    left = NULL; 
    (op=o)->SetParent(this);
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Expr *l, Operator *o) 
  : Expr(Join(l->GetLocation(), o->GetLocation())) {
    Assert(l != NULL && o != NULL);
    (left=l)->SetParent(this);
    (op=o)->SetParent(this);
}

void CompoundExpr::PrintChildren(int indentLevel) {
   if (left) left->Print(indentLevel+1);
   op->Print(indentLevel+1);
   if (right) right->Print(indentLevel+1);
}

SelectionExpr::SelectionExpr(Expr *c, Expr *t, Expr *f)
  : Expr(Join(c->GetLocation(), f->GetLocation())) {
    Assert(c != NULL && t != NULL && f != NULL);
    (cond=c)->SetParent(this);
    (trueExpr=t)->SetParent(this);
    (falseExpr=f)->SetParent(this);
}

void SelectionExpr::PrintChildren(int indentLevel) {
    cond->Print(indentLevel+1);
    trueExpr->Print(indentLevel+1, "(true) ");
    falseExpr->Print(indentLevel+1, "(false) ");
}
   
ConditionalExpr::ConditionalExpr(Expr *c, Expr *t, Expr *f)
  : Expr(Join(c->GetLocation(), f->GetLocation())) {
    Assert(c != NULL && t != NULL && f != NULL);
    (cond=c)->SetParent(this);
    (trueExpr=t)->SetParent(this);
    (falseExpr=f)->SetParent(this);
}

void ConditionalExpr::PrintChildren(int indentLevel) {
    cond->Print(indentLevel+1, "(cond) ");
    trueExpr->Print(indentLevel+1, "(true) ");
    falseExpr->Print(indentLevel+1, "(false) ");
}

Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
    Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
    base = b;
    if (base) base->SetParent(this);
    (field=f)->SetParent(this);
    (actuals=a)->SetParentAll(this);
}

void Call::PrintChildren(int indentLevel) {
   if (base) base->Print(indentLevel+1);
   if (field) field->Print(indentLevel+1);
   if (actuals) actuals->PrintAll(indentLevel+1, "(actuals) ");
}

Type* IntConstant::CheckExpr() {
    return Type::intType;
}

Type* BoolConstant::CheckExpr() {
    return Type::boolType;
}

Type* VarExpr::CheckExpr() {
    Decl* d = symtab->FindSymbolInAllScopes(id->GetName());
    
    if ( d == NULL ) {
        ReportError::IdentifierNotDeclared(id, LookingForVariable);
        return Type::errorType;
    }

    VarDecl* v = dynamic_cast<VarDecl*>(d);
    return v->GetType();
}

Type* Call::CheckExpr() {
    Decl* d = symtab->FindSymbolInAllScopes(field->GetName());
    FnDecl *f = dynamic_cast<FnDecl*>(d);

    if (f) {

        List<VarDecl*> *formals = f->GetFormals();
        int expCount = formals->NumElements();
        int actualCount = actuals->NumElements();

        if (expCount < actualCount) {
            ReportError::ExtraFormals(field, expCount, actualCount);
        }

        if (expCount > actualCount) {
            ReportError::LessFormals(field, expCount, actualCount);
        }

        for (int i = 0; i < actualCount - 1; ++i) {
            Type *givenType = actuals->Nth(i)->CheckExpr();
            Type *expType = formals->Nth(i)->GetType();

            if (givenType != expType) {
                ReportError::FormalsTypeMismatch(field, i, expType, givenType);
            }
        }

        return f->GetType();

    } else {
        ReportError::NotAFunction(field);
        return Type::errorType;
    }
}

Type* ArithmeticExpr::CheckExpr() {
    // Extract the type of the RHS
    Type *tR = right->CheckExpr();

    if (left) {

        // Extract the type of the LHS
        Type *tL = left->CheckExpr();

        // Check that both LHS and RHS are of int type
        if (tL != Type::intType || tR != Type::intType) {
            ReportError::IncompatibleOperands(op, tL, tR);
        }

    } else {
        // Check that the RHS is an int type
        if (tR != Type::intType) {
            ReportError::IncompatibleOperand(op, tR);
        }

    }

    return Type::intType;
}

Type* RelationalExpr::CheckExpr() {
    // Extract the types of the LHS and RHS
    Type *tL = left->CheckExpr();
    Type *tR = right->CheckExpr();

    // Check that both types are of int types
    if (tL != Type::intType || tR != Type::intType) {
        ReportError::IncompatibleOperands(op, tL, tR);
    }
    
    return Type::boolType;
}

Type* EqualityExpr::CheckExpr() {
    // Extract the types of the LHS and RHS
    Type *tL = left->CheckExpr();
    Type *tR = right->CheckExpr();

    // Check that the two types match
    if (tL != tR) {
        ReportError::IncompatibleOperands(op, tL, tR);
    }

    return Type::boolType;
}

Type* LogicalExpr::CheckExpr() {
    // Extract the types of the LHS and RHS
    Type *tL = left->CheckExpr();
    Type *tR = right->CheckExpr();

    if (tL != Type::boolType && tR != Type::boolType) {
        ReportError::IncompatibleOperands(op, tL, tR);
    }
    return Type::boolType;
}

Type* AssignExpr::CheckExpr() {
    // Extract the types of the LHS and RHS
    Type *tL = left->CheckExpr();
    Type *tR = right->CheckExpr();

    if (tR->IsError())
        return Type::errorType;

    if (tL->IsError())
        return Type::errorType;

    if (tL != tR) {
        ReportError::IncompatibleOperands(op, tL, tR);
        return Type::errorType;
    }

    return tL;
}

Type* PostfixExpr::CheckExpr() {
    // Extract the types of the LHS and RHS
    Type *tL = left->CheckExpr();

    if (tL != Type::intType) {

    }

    return Type::intType;
}
