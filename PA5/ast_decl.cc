/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "symtable.h"

Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this);
}


VarDecl::VarDecl(Identifier *n, Type *t, Expr *e) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
    if (e) (assignTo=e)->SetParent(this);
}

void VarDecl::PrintChildren(int indentLevel) {
   if (type) type->Print(indentLevel+1);
   if (id) id->Print(indentLevel+1);
   if (assignTo) assignTo->Print(indentLevel+1, "(initializer) ");
}

string VarDecl::Emit() {
    string varName = GetIdentifier()->GetName();

    if (assignTo) {
        string rhsRegName = assignTo->Emit();
        TACObject tacObj(varName, rhsRegName, 4, stmt);
        TACContainer.push_back(tacObj);
    }     

    return "VarDecl::Emit()";
}

FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
}

void FnDecl::SetFunctionBody(Stmt *b) {
    (body=b)->SetParent(this);
}

string FnDecl::Emit() {
    TACObject obj(id->GetName() + string(":"), "", 0, label);
    TACContainer.push_back(obj);

    for(int i = 0; i < formals->NumElements(); i++ ) {
        TACObject o("LoadParam",
                formals->Nth(i)->GetIdentifier()->GetName(),
                0, instr
                );
        TACContainer.push_back(o);
    }

    TACObject o("BeginFunc", "?", 0, instr);
    TACContainer.push_back(o);

    body->Emit();

    TACObject o1("EndFunc", "", 0, instr);
    TACContainer.push_back(o1);

    return "FnDecl::Emit()";
}

void FnDecl::PrintChildren(int indentLevel) {
    if (returnType) returnType->Print(indentLevel+1, "(return type) ");
    if (id) id->Print(indentLevel+1);
    if (formals) formals->PrintAll(indentLevel+1, "(formals) ");
    if (body) body->Print(indentLevel+1, "(body) ");
}


