/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "symtable.h"     
#include "errors.h"   
         
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

FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
}

void FnDecl::SetFunctionBody(Stmt *b) { 
    (body=b)->SetParent(this);
}

void FnDecl::PrintChildren(int indentLevel) {
    if (returnType) returnType->Print(indentLevel+1, "(return type) ");
    if (id) id->Print(indentLevel+1);
    if (formals) formals->PrintAll(indentLevel+1, "(formals) ");
    if (body) body->Print(indentLevel+1, "(body) ");
}

void VarDecl::Check() {
    //Check current scope
    if( symtab->IsInCurrentScope( id->GetName() ) ){
        Decl* previous_decl = symtab->FindSymbolInCurrentScope( id->GetName() );
        ReportError::DeclConflict(this, previous_decl);
    }
    
    // If this VarDecl has assinTo, which means it is doing an initialization,
    // then you need to check whether the type of the initialization is the same
    // as the type of this VarDecl
    if(assignTo) {
        // TODO: check type
    }

    symtab->AddSymbol(id->GetName(),this);      
}

void FnDecl::Check() {
    if( symtab->IsInCurrentScope( id->GetName() ) ){
        Decl* previous_decl = symtab->FindSymbolInCurrentScope( id->GetName() );
        ReportError::DeclConflict(this, previous_decl);
        return;
    }    

    symtab->AddSymbol(id->GetName(),this);

    // Push a new scope for this function declarartion
    symtab->PushScope();

    // Check formals(Lisf of VarDecl)
    for(int i = 0; i < formals->NumElements(); i++){
        formals->Nth(i)->Check();
    }

    // Check body(List of Stmt)
    body->Check();

    // TODO: Check for missing ReturnStmt


    // Finish semantic check for this function declaration,
    // thus pop its scope
    symtab->PopScope();
}
