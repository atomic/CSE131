/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"


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
    Assert(i != NULL && t != NULL && s != NULL && b != NULL);
    (init=i)->SetParent(this);
    (step=s)->SetParent(this);
}

void ForStmt::PrintChildren(int indentLevel) {
    init->Print(indentLevel+1, "(init) ");
    test->Print(indentLevel+1, "(test) ");
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
    Assert(e != NULL);
    (expr=e)->SetParent(this);
}

void ReturnStmt::PrintChildren(int indentLevel) {
    expr->Print(indentLevel+1);
}

DeclStmt::DeclStmt(yyltype loc, Decl *decl) : Stmt(loc) {
    Assert(decl != NULL);
    (varDecl=decl)->SetParent(this);
}

void DeclStmt::PrintChildren(int indentLevel) {
    varDecl->Print(indentLevel+1);
}

string Program::Emit() {
    if ( decls->NumElements() > 0 ) {
      for ( int i = 0; i < decls->NumElements(); ++i ) {
        Decl *d = decls->Nth(i);
        d->Emit();
      }
    }

    for (int i = 0; i < TACContainer.size(); i++) {
        switch(TACContainer[i].type) {
            case label: cout << TACContainer[i].lhs + ":" << endl;     break;

            case stmt:  cout << "    " + TACContainer[i].lhs
                             << " := " << TACContainer[i].rhs << endl; break;

            case instr: cout << "    " + TACContainer[i].lhs
                             << " "    + TACContainer[i].rhs << endl; break;

            case call:  cout << "    "   + TACContainer[i].lhs
                             << " call " + TACContainer[i].rhs << endl; break;

            case branch: cout << "    if " + TACContainer[i].lhs + " goto "
                             << TACContainer[i].rhs << endl; break;

            case jump:   cout << "    goto " + TACContainer[i].lhs << endl; break;

            default: cout << " ERRRORRR !!!! " << endl;
        }
    }

    return "Program::Emit()";
}

string StmtBlock::Emit() {
    for(int i = 0; i < stmts->NumElements(); i++){
        Stmt* ith_statement = stmts->Nth(i);
        ith_statement->Emit();
    }
    
    return "StmtBlock::Emit()";
}

string ForStmt::Emit() {
    string inc_var = init->Emit();
    string label0 = "L0";
    string label1 = "L1";
    string label2 = "L2";
    TACContainer.emplace_back(label0, "", 0, label);
    TACContainer.emplace_back(test->Emit(), label1, 0, branch);
    TACContainer.emplace_back(label2, "", 0, jump);
    TACContainer.emplace_back(label1, "", 0, label);
    body->Emit();
    step->Emit();
    TACContainer.emplace_back(label0, "", 0, jump);
    TACContainer.emplace_back(label2, "", 0, label);
    return "ForStmt::Emit()";
}

string WhileStmt::Emit() {
    string label0 = "L0";
    string label1 = "L1";
    string label2 = "L2";

    TACContainer.emplace_back(label0, "", 0, label);
    TACContainer.emplace_back(test->Emit(), label1, 0, branch);
    TACContainer.emplace_back(label2, "", 0, jump);
    TACContainer.emplace_back(label1, "", 0, label);
    body->Emit();
    TACContainer.emplace_back(label0, "", 0, jump);
    TACContainer.emplace_back(label2, "", 0, label);

    return "WhileStmt::Emit()";
}

string IfStmt::Emit() {
    string exitLabel = (elseBody) ? "L2" : "L1";
    string label0 = "L0";
    string label1 = "L1";

    TACContainer.emplace_back(test->Emit(), label0, 0, branch);

    TACContainer.emplace_back(label1, "", 0, jump);

    TACContainer.emplace_back(label0, "", 0, label);

    body->Emit();

    TACContainer.emplace_back(exitLabel, "", 0, jump);

    if (elseBody) {
        TACContainer.emplace_back(label1, "", 0, label);
        elseBody->Emit();
        TACContainer.emplace_back(exitLabel, "", 0, jump);
    }

    TACContainer.emplace_back(exitLabel, "", 0, label);

    return "ifStmt";
}

string ReturnStmt::Emit() {
    string rhs = expr->Emit();
    TACObject o("Return", rhs, 0, instr);
    TACContainer.push_back(o);

    return "ReturnStmt Emit()";
}

string DeclStmt::Emit() {
    varDecl->Emit();
    return "DeclStmt Emit()";
}

