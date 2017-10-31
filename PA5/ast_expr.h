/* File: ast_expr.h
 * ----------------
 * The Expr class and its subclasses are used to represent
 * expressions in the parse tree.  For each expression in the
 * language (add, call, New, etc.) there is a corresponding
 * node class for that construct.
 */


#ifndef _H_ast_expr
#define _H_ast_expr

#include "ast.h"
#include "ast_stmt.h"
#include "list.h"
#include <string>
using namespace std;

class NamedType; // for new
class Type; // for NewArray

void yyerror(const char *msg);

class Expr : public Stmt
{
  public:
    Expr(yyltype loc) : Stmt(loc) {}
    Expr() : Stmt() {}
};

class ExprError : public Expr
{
  public:
    ExprError() : Expr() { yyerror(this->GetPrintNameForNode()); }
    const char *GetPrintNameForNode() { return "ExprError"; }
};

/* This node type is used for those places where an expression is optional.
 * We could use a NULL pointer, but then it adds a lot of checking for
 * NULL. By using a valid, but no-op, node, we save that trouble */
class EmptyExpr : public Expr
{
  public:
    const char *GetPrintNameForNode() { return "Empty"; }
    virtual string Emit();
};

class IntConstant : public Expr
{
  protected:
    int value;

  public:
    IntConstant(yyltype loc, int val);
    const char *GetPrintNameForNode() { return "IntConstant"; }
    void PrintChildren(int indentLevel);
    int GetValue() { return value; }
    virtual string Emit();
};

class BoolConstant : public Expr
{
  protected:
    bool value;

  public:
    BoolConstant(yyltype loc, bool val);
    const char *GetPrintNameForNode() { return "BoolConstant"; }
    void PrintChildren(int indentLevel);
    bool GetValue() { return value; }
    virtual string Emit();
};

class Operator : public Node
{
  protected:
    char tokenString[4];

  public:
    Operator(yyltype loc, const char *tok);
    const char *GetPrintNameForNode() { return "Operator"; }
    void PrintChildren(int indentLevel);
    virtual string Emit();
 };

class CompoundExpr : public Expr
{
  protected:
    Operator *op;
    Expr *left, *right; // left will be NULL if unary

  public:
    CompoundExpr(Expr *lhs, Operator *op, Expr *rhs); // for binary
    CompoundExpr(Operator *op, Expr *rhs);             // for unary
    CompoundExpr(Expr *lhs, Operator *op);             // for unary
    void PrintChildren(int indentLevel);
};

class ArithmeticExpr : public CompoundExpr
{
  public:
    ArithmeticExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
    ArithmeticExpr(Operator *op, Expr *rhs) : CompoundExpr(op,rhs) {}
    const char *GetPrintNameForNode() { return "ArithmeticExpr"; }
    virtual string Emit();
};

class RelationalExpr : public CompoundExpr
{
  public:
    RelationalExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
    const char *GetPrintNameForNode() { return "RelationalExpr"; }
    virtual string Emit();
};

class EqualityExpr : public CompoundExpr
{
  public:
    EqualityExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
    const char *GetPrintNameForNode() { return "EqualityExpr"; }
    virtual string Emit();
};

class LogicalExpr : public CompoundExpr
{
  public:
    LogicalExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
    LogicalExpr(Operator *op, Expr *rhs) : CompoundExpr(op,rhs) {}
    const char *GetPrintNameForNode() { return "LogicalExpr"; }
    virtual string Emit();
};

class SelectionExpr : public Expr
{
  protected:
    Expr *cond, *trueExpr, *falseExpr;
  public:
    SelectionExpr(Expr *c, Expr *t, Expr *f);
    void PrintChildren(int indentLevel);
    const char *GetPrintNameForNode() { return "SelectionExpr"; }
};

class AssignExpr : public CompoundExpr
{
  public:
    AssignExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
    const char *GetPrintNameForNode() { return "AssignExpr"; }
    virtual string Emit();
};

class PostfixExpr : public CompoundExpr
{
  public:
    PostfixExpr(Expr *lhs, Operator *op) : CompoundExpr(lhs,op) {}
    const char *GetPrintNameForNode() { return "PostfixExpr"; }
    virtual string Emit();
};

/* Like field access, call is used both for qualified base.field()
 * and unqualified field().  We won't figure out until later
 * whether we need implicit "this." so we use one node type for either
 * and sort it out later. */
class Call : public Expr
{
  protected:
    Expr *base;	// will be NULL if no explicit base
    Identifier *field;
    List<Expr*> *actuals;

  public:
    Call() : Expr(), base(NULL), field(NULL), actuals(NULL) {}
    Call(yyltype loc, Expr *base, Identifier *field, List<Expr*> *args);
    const char *GetPrintNameForNode() { return "Call"; }
    void PrintChildren(int indentLevel);
    virtual string Emit();
};

class VarExpr : public Expr
{
  protected:
    Identifier *id;
  public:
    VarExpr(yyltype loc, Identifier *ident);
    const char *GetPrintNameForNode() { return "VarExpr"; }
    void PrintChildren(int identLevel);
    string GetName() {return id->GetName();}
    virtual string Emit();
};

#endif
