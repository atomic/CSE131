/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */

#include <string.h>
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "symtable.h"


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

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}

void Operator::PrintChildren(int indentLevel) {
    printf("%s",tokenString);
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

void VarExpr::PrintChildren(int identLevel) {
    id->Print(identLevel + 1);
}

VarExpr::VarExpr(yyltype loc, Identifier *ident) : Expr(loc) {
    id = ident;
}

string IntConstant::Emit() {
    return to_string(value);
}

string BoolConstant::Emit() {
    return value ? "true" : "false";
}

string Operator::Emit() {
    return string(tokenString);
}

string Call::Emit() {
    for (int i = 0; i < actuals->NumElements(); ++i) {
        string argName = actuals->Nth(i)->Emit();
        TACContainer.emplace_back("PushParam", argName, 0, instr);
    }

    string registerStr = "t" + to_string(tempRegister);
    tempRegister++; stackRegister++;
    string rhs = string(field->GetName()) + " " + to_string(actuals->NumElements());

    TACContainer.emplace_back (registerStr, rhs, 0, call);
    int deallocate = actuals->NumElements() * 4;
    TACContainer.emplace_back("PopParam", to_string(deallocate), 0, instr);

    return registerStr;
}

string VarExpr::Emit() {
    string localVarName = id->GetName();
    return string(localVarName);
}

string EmptyExpr::Emit() {
    return "EmptyExpr Emit";
}

string ArithmeticExpr::Emit() {
    string leftStr = left->Emit();
    string rightStr = right->Emit();
    string opString = op->Emit();

    string registerStr = "t" + to_string(tempRegister);
    tempRegister++; stackRegister++;
    string assignTo = leftStr + string(" ") + opString + string(" ") + rightStr;
    TACObject tacObj(registerStr, assignTo, 4, stmt);

    TACContainer.push_back(tacObj);

    return registerStr;
}

string RelationalExpr::Emit() {
    string leftStr = left->Emit();
    string rightStr = right->Emit();
    string opString = op->Emit();

    string registerStr = "t" + to_string(tempRegister);
    tempRegister++; stackRegister++;
    string assignTo = leftStr + string(" ") + opString + string(" ") + rightStr;

    TACContainer.emplace_back(registerStr, assignTo, 4, stmt);

    return registerStr;
}

string AssignExpr::Emit() {
    string lhs = left->Emit();
    string rhs = right->Emit();
    string opString = op->Emit();

    TACContainer.emplace_back(lhs, rhs, 0, stmt);

    return "AssignExpr::Emit()";
}

string LogicalExpr::Emit() {
    return "LogicalExpr::Emit()";
}

string EqualityExpr::Emit() {
    string leftStr = left->Emit();
    string rightStr = right->Emit();
    string opString = op->Emit();

    string registerStr = "t" + to_string(tempRegister);
    tempRegister++; stackRegister++;
    string assignTo = leftStr + string(" ") + opString + string(" ") + rightStr;

    TACContainer.emplace_back(registerStr, assignTo, 4, stmt);

    return registerStr;
}

string PostfixExpr::Emit() {
    string leftStr = left->Emit();
    string opString = op->Emit();
    string assignTo = leftStr + " " + opString[0] + " 1";
    TACContainer.emplace_back(leftStr, assignTo, 0, stmt);

    return "PostfixExpr::Emit()";
}
