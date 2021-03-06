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
    stackRegister++; // add total register during declaration? (what if variable use same name?)

    if (assignTo) {
        string rhsRegName = assignTo->Emit();
        TACContainer.emplace_back (varName, rhsRegName, 4, stmt);
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
    stackRegister = 0;  // beginning of function stack
    current_context = id->GetName();
    tempRegister[current_context] = 1;

    TACContainer.emplace_back(id->GetName(), "", 0, label);
    for(int i = 0; i < formals->NumElements(); ++i)
        TACContainer.emplace_back("LoadParam", formals->Nth(i)->GetIdentifier()->GetName(), 0, instr);
    //stackRegister += formals->NumElements();

    TACContainer.emplace_back("BeginFunc", "?", 0, instr, sc_MemAlloc);
    size_t begin_pos = TACContainer.size() - 1;
    body->Emit();
    TACContainer.emplace_back("EndFunc", "", 0, instr);
    TACContainer[begin_pos].bytes = stackRegister*4;
    TACContainer[begin_pos].rhs = to_string(stackRegister*4);
    return "FnDecl::Emit()";
}

void FnDecl::PrintChildren(int indentLevel) {
    if (returnType) returnType->Print(indentLevel+1, "(return type) ");
    if (id) id->Print(indentLevel+1);
    if (formals) formals->PrintAll(indentLevel+1, "(formals) ");
    if (body) body->Print(indentLevel+1, "(body) ");
}


