/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "errors.h"
#include "symtable.h"
#include "string.h"

Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

void Program::PrintChildren(int indentLevel) {
    decls->PrintAll(indentLevel+1);
    printf("\n");
}

StmtBlock::StmtBlock(List<Stmt*> *s) {
    Assert(s != NULL);
    (stmts=s)->SetParentAll(this);
}

void StmtBlock::PrintChildren(int indentLevel) {
    stmts->PrintAll(indentLevel+1);
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this); 
    (body=b)->SetParent(this);
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) { 
    Assert(i != NULL && t != NULL && b != NULL);
    (init=i)->SetParent(this);
    step = s;
    if ( s )
        (step=s)->SetParent(this);
}

void ForStmt::PrintChildren(int indentLevel) {
    init->Print(indentLevel+1, "(init) ");
    test->Print(indentLevel+1, "(test) ");
    if ( step )
        step->Print(indentLevel+1, "(step) ");
    body->Print(indentLevel+1, "(body) ");
}

void WhileStmt::PrintChildren(int indentLevel) {
    test->Print(indentLevel+1, "(test) ");
    body->Print(indentLevel+1, "(body) ");
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}

void IfStmt::PrintChildren(int indentLevel) {
    if (test) test->Print(indentLevel+1, "(test) ");
    if (body) body->Print(indentLevel+1, "(then) ");
    if (elseBody) elseBody->Print(indentLevel+1, "(else) ");
}

ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    expr = e;
    if (e != NULL) expr->SetParent(this);
}

void ReturnStmt::PrintChildren(int indentLevel) {
    if ( expr ) 
        expr->Print(indentLevel+1);
}

DeclStmt::DeclStmt(yyltype loc, Decl *decl) : Stmt(loc) {
    Assert(decl != NULL);
    (varDecl=decl)->SetParent(this);
}

void DeclStmt::PrintChildren(int indentLevel) {
    varDecl->Print(indentLevel+1);
}

void Program::Check() {
    /* pp3: here is where the semantic analyzer is kicked off.
     *      The general idea is perform a tree traversal of the
     *      entire program, examining all constructs for compliance
     *      with the semantic rules.  Each node can have its own way of
     *      checking itself, which makes for a great use of inheritance
     *      and polymorphism in the node classes.
     */

    if ( decls->NumElements() > 0 ) {
        for ( int i = 0; i < decls->NumElements(); ++i ) {
            Decl *d = decls->Nth(i);
            d->Check();
        }
    }
}

void StmtBlock::Check() {
    for (int i = 0; i < stmts->NumElements(); ++i) {
        Stmt *s = stmts->Nth(i);
        s->Check();
    }
}

void IfStmt::Check() {
    // Check that the test expression is a boolean type
    if (test->CheckExpr() != Type::boolType) {
        ReportError::TestNotBoolean(test);
    }

    // Push a new scope for this if statement
    symtab->PushScope(IfElse);

    // Check the if statement body
    body->Check();

    // Pop scope because the if statement has ended
    symtab->PopScope();

    // Check elseBody exists
    if (elseBody) {

        // Push a new scope for this else statement
        symtab->PushScope(IfElse);

        // Check the else statement body
        elseBody->Check();

        // Pop scope because the else statement has ended
        symtab->PopScope();
    } 
}

void WhileStmt::Check() {
    // Check that the test expression is a boolean type
    if (test->CheckExpr() != Type::boolType) {
        ReportError::TestNotBoolean(test);
    }

    // Push a new scope for this while statement
    symtab->PushScope(Loop);

    // Check the while body
    body->Check();

    // Pop scope because the while statement has ended
    symtab->PopScope();
}

void ForStmt::Check() {

    symtab->PushScope(Loop);

    init->CheckExpr();

    if (test->CheckExpr() != Type::boolType) {
        ReportError::TestNotBoolean(test);
    }

    step->CheckExpr();

    // Check the for loop body
    body->Check();

    // Pop scope because the for loop statement has ended
    symtab->PopScope();
}

void ReturnStmt::Check() {
    // Set the has_return flag of the function's direct scope to true
    symtab->ReturnStmtDoesExist();
    FnDecl* func = symtab->GetLatestFnDecl();

    Type *givenReturn = expr->CheckExpr();

    if (func->GetType() == Type::voidType && givenReturn != Type::bvec4Type) {
        ReportError::ReturnMismatch(this, givenReturn, func->GetType());
    }

    if (func->GetType() != Type::voidType && givenReturn != func->GetType()) {
        ReportError::ReturnMismatch(this, givenReturn, func->GetType());
    }
}

void BreakStmt::Check() {
    if (!symtab->ContainsLoopScope()) {
        ReportError::BreakOutsideLoop(this);
    }
}

void DeclStmt::Check() {
    // Check the variable declaration statement
    varDecl->Check();
}
