/* File: parser.y
 * --------------
 * Bison input file to generate the parser for the compiler.
 *
 * pp2: your job is to write a parser that will construct the parse tree
 *      and if no parse errors were found, print it.  The parser should
 *      accept the language as described in specification, and as augmented
 *      in the pp2 handout.
 */

%{

/* Just like lex, the text within this first region delimited by %{ and %}
 * is assumed to be C/C++ code and will be copied verbatim to the y.tab.c
 * file ahead of the definitions of the yyparse() function. Add other header
 * file inclusions or C++ variable declarations/prototypes that are needed
 * by your code here.
 */
#include "lexer.h" // for yylex
#include "parser.h"
#include "errors.h"

void yyerror(const char *msg); // standard error-handling routine

%}

/* The section before the first %% is the Definitions section of the yacc
 * input file. Here is where you declare tokens and types, add precedence
 * and associativity options, and so on.
 */

/* yylval
 * ------
 * Here we define the type of the yylval global variable that is used by
 * the lexer to store attibute information about the token just scanned
 * and thus communicate that information to the parser.
 *
 * pp2: You will need to add new fields to this union as you add different
 *      attributes to your non-terminal symbols.
 */
%union {
    int integerConstant;
    bool boolConstant;
    char identifier[MaxIdentLen+1]; // +1 for terminating null

    Type *varType;
    Identifier *id;
    VarDecl *varDecl;
    List<VarDecl*> *varDeclList;
    FnDecl *fnDecl;
    Decl *decl;
    List<Decl*> *declList;

    Operator *oper;
    Expr *expr;
    List<Expr*> *argList;

    List<Stmt*> *stmtList;
    Stmt *stmt;
    StmtBlock *stmtBlock;

    ReturnStmt *returnStmt;
    BreakStmt *breakStmt;
    DeclStmt *declStmt;

    LoopStmt *loopStmt;
    ForStmt *forStmt;
    WhileStmt *whileStmt;
    IfStmt *ifStmt;

    AssignExpr *assignExpr;
    ArithmeticExpr *arithExpr;
    RelationalExpr *relExpr;
    LogicalExpr *logicalExpr;
    EqualityExpr *equalityExpr;

    Call *call;
}

/* Tokens
 * ------
 * Here we tell yacc about all the token types that we are using.
 * Bison will assign unique numbers to these and export the #define
 * in the generated y.tab.h header file.
 */
%token   T_Void T_Bool T_Int
%token   T_LessEqual T_GreaterEqual T_EQ T_NE T_LeftAngle T_RightAngle
%token   T_And T_Or
%token   T_Equal T_MulAssign T_DivAssign T_AddAssign T_SubAssign
%token   T_While T_For T_If T_Else T_Return T_Break T_Continue
%token   T_Inc T_Dec
%token   T_Semicolon T_Comma
%token   T_Dash T_Plus T_Star T_Slash
%token   T_LeftParen T_RightParen T_LeftBracket T_RightBracket T_LeftBrace T_RightBrace
%token   T_Public T_Private T_Static T_Class

%token   <identifier>T_Identifier
%token   <integerConstant>T_IntConstant
%token   <boolConstant>T_BoolConstant

%nonassoc LOWEST
%nonassoc LOWER_THAN_ELSE
%nonassoc T_Else
%right T_Equal T_MulAssign T_DivAssign T_AddAssign T_SubAssign
%left T_Or
%left T_And
%left T_EQ T_NE
%left T_LeftAngle T_RightAngle T_LessEqual T_GreaterEqual
%left T_Plus T_Dash
%left T_Star T_Slash
%left T_Inc T_Dec
%left T_LeftParen T_RightParen

/* Non-terminal types
 * ------------------
 * In order for yacc to assign/access the correct field of $$, $1, we
 * must to declare which field is appropriate for the non-terminal.
 * As an example, this first type declaration establishes that the DeclList
 * non-terminal uses the field named "declList" in the yylval union. This
 * means that when we are setting $$ for a reduction for DeclList ore reading
 * $n which corresponds to a DeclList nonterminal we are accessing the field
 * of the union named "declList" which is of type List<Decl*>.
 * pp2: You'll need to add many of these of your own.
 */
%type <declList>     DeclList
%type <decl>         Decl
%type <varDecl>      SingleDecl 
%type <varType>      TypeSpecifier
%type <fnDecl>       FuncDefinition 
%type <oper>         AssignmentOper 
%type <expr>         Expr 
%type <fnDecl>       FuncPrototype 
%type <stmtBlock>    CompoundStmtWithScope 
%type <fnDecl>       FuncPrototypeHeader
%type <varDeclList>  ParameterDeclList
%type <varDecl>      ParameterDecl
%type <stmtList>     statement_list
%type <stmt>         statement
%type <stmt>         simple_statement
%type <expr>         ExprStmt
%type <ifStmt>       SelectionStmt
%type <loopStmt>     IterationStmt
%type <returnStmt>   ReturnStmt
%type <declStmt>     decl_statement
%type <breakStmt>    BreakStmt
%type <whileStmt>    WhileStmt
%type <forStmt>      for_statement
%type <expr>         condition
%type <assignExpr>   AssignmentExpr
%type <arithExpr>    ArithmeticExpr
%type <relExpr>      RelationalExpr
%type <equalityExpr> EqualityExpr
%type <logicalExpr>  LogicalExpr
%type <expr>         UnaryExpr
%type <expr>         PostfixExpr
%type <expr>         PrimaryExpr
%type <call>         func_call_expression
%type <call>         func_call_header_with_parameters
%type <call>         func_call_header_with_no_parameters
%type <id>           FuncIdentifier
%type <argList>      ArgList
%type <expr>         constant


%%
/* Rules
 * -----
 * All productions and actions should be placed between the start and stop
 * %% markers which delimit the Rules section.

 */
Program   :    DeclList   {
                            @1;
                            /* pp2: The @1 is needed to convince
                            * yacc to set up yylloc. You can remove
                            * it once you have other uses of @n*/
                            Program *program = new Program($1);
                            if (ReportError::NumErrors() == 0) {
                                // program->Print(0);
                                // program->Check();
                                program->Emit();
                            }
                          }
          ;

DeclList  :    DeclList Decl       { ($$=$1)->Append($2); } 
          |    Decl                { ($$ = new List<Decl*>)->Append($1); } 
          ;

Decl      :    SingleDecl          { $$ = $1; } 
          |    FuncDefinition      { $$ = $1; }
          ;

SingleDecl : TypeSpecifier T_Identifier T_Semicolon   
             { $$ = new VarDecl(new Identifier(@2, $2), $1); }
           | TypeSpecifier T_Identifier AssignmentOper Expr T_Semicolon
             { $$ = new VarDecl(new Identifier(@2, $2), $1, $4); }
           ;

TypeSpecifier         : T_Void     { $$ = Type::voidType; } 
                      | T_Int      { $$ = Type::intType; }
                      | T_Bool     { $$ = Type::boolType; } 
                      ;

FuncDefinition        : FuncPrototype CompoundStmtWithScope
                        { ($$=$1)->SetFunctionBody($2); }
                      | FuncPrototype T_Semicolon
                        { $$ = $1; }
                      ;

FuncPrototype         : FuncPrototypeHeader T_RightParen
                        { $$ = $1; }
                      ;

FuncPrototypeHeader   : TypeSpecifier T_Identifier T_LeftParen
                        { $$ = new FnDecl(new Identifier(@2, $2), $1, new List<VarDecl*>); }
                      | TypeSpecifier T_Identifier T_LeftParen ParameterDeclList
                        { $$ = new FnDecl(new Identifier(@2, $2), $1, $4); }
                      ;

ParameterDeclList     : ParameterDeclList T_Comma ParameterDecl
                        { ($$=$1)->Append($3); }
                      | ParameterDecl
                        { ($$ = new List<VarDecl*>)->Append($1); }
                      ;

ParameterDecl         : TypeSpecifier T_Identifier
                        { $$ = new VarDecl(new Identifier(@2, $2), $1); }
                      ;

CompoundStmtWithScope : T_LeftBrace statement_list T_RightBrace
                        { $$ = new StmtBlock($2); }
                      | T_LeftBrace T_RightBrace
                        { $$ = new StmtBlock(new List<Stmt*>); }
                      ;

statement_list        : statement_list statement    
                        { ($$=$1)->Append($2); }
                      | statement        
                        { ($$ = new List<Stmt*>)->Append($1); }
                      ;

statement             : CompoundStmtWithScope
                        { $$ = $1; }
                      | simple_statement
                        { $$ = $1; }
                      ;

simple_statement      : ExprStmt       { $$ = $1; }
                      | SelectionStmt  { $$ = $1; } 
                      | IterationStmt  { $$ = $1; }
                      | ReturnStmt     { $$ = $1; }
                      | decl_statement { $$ = $1; }
                      | BreakStmt      { $$ = $1; }
                      ;

ExprStmt       : T_Semicolon
                 { $$ = new EmptyExpr(); }
               | Expr T_Semicolon
                 { $$ = $1; }
               ;

SelectionStmt  : T_If T_LeftParen Expr T_RightParen CompoundStmtWithScope
                 { $$ = new IfStmt($3, $5, NULL); }
               | T_If T_LeftParen Expr T_RightParen CompoundStmtWithScope T_Else CompoundStmtWithScope
                 { $$ = new IfStmt($3, $5, $7); }
               ;

IterationStmt  : WhileStmt      { $$ = $1; }
               | for_statement  { $$ = $1; }
               ;

WhileStmt      : T_While T_LeftParen condition T_RightParen statement 
                 { $$ = new WhileStmt($3, $5); }
               ;

for_statement  : T_For T_LeftParen ExprStmt ExprStmt Expr T_RightParen statement
                 { $$ = new ForStmt($3, $4, $5, $7); }
               ;

condition      : Expr { $$ = $1; }
               ;

ReturnStmt     : T_Return ExprStmt
                 { $$ = new ReturnStmt(@2, $2); }
               ;

decl_statement : SingleDecl
                 { $$ = new DeclStmt(@1, $1); }
               ;

BreakStmt      : T_Break T_Semicolon   
                 { $$ = new BreakStmt(@1); }
               ;

Expr           : AssignmentExpr  { $$ = $1; }
               | ArithmeticExpr  { $$ = $1; }
               | RelationalExpr  { $$ = $1; }
               | EqualityExpr    { $$ = $1; }
               | LogicalExpr     { $$ = $1; }
               | UnaryExpr       { $$ = $1; }
               ;

AssignmentExpr : UnaryExpr AssignmentOper Expr
                 { $$ = new AssignExpr($1, $2, $3); }
               ;

AssignmentOper : T_Equal       
                 { $$ = new Operator(@1, "="); }
               | T_MulAssign
                 { $$ = new Operator(@1, "*="); }
               | T_DivAssign
                 { $$ = new Operator(@1, "/="); }
               | T_AddAssign
                 { $$ = new Operator(@1, "+="); }
               | T_SubAssign
                 { $$ = new Operator(@1, "-="); }
               ;

ArithmeticExpr : Expr T_Plus Expr    
                 { $$ = new ArithmeticExpr($1, new Operator(@2, "+"), $3); }
               | Expr T_Dash Expr
                 { $$ = new ArithmeticExpr($1, new Operator(@2, "-"), $3); }
               | Expr T_Star Expr
                 { $$ = new ArithmeticExpr($1, new Operator(@2, "*"), $3); }
               | Expr T_Slash Expr
                 { $$ = new ArithmeticExpr($1, new Operator(@2, "/"), $3); }
               ;

RelationalExpr : Expr T_LeftAngle Expr
                 { $$ = new RelationalExpr($1, new Operator(@2, "<"), $3); }
               | Expr T_RightAngle Expr
                 { $$ = new RelationalExpr($1, new Operator(@2, ">"), $3); }
               | Expr T_LessEqual Expr
                 { $$ = new RelationalExpr($1, new Operator(@2, "<="), $3); }
               | Expr T_GreaterEqual Expr
                 { $$ = new RelationalExpr($1, new Operator(@2, ">="), $3); }
               ;

EqualityExpr   : Expr T_EQ Expr
                 { $$ = new EqualityExpr($1, new Operator(@2, "=="), $3); }
               | Expr T_NE Expr
                 { $$ = new EqualityExpr($1, new Operator(@2, "!="), $3); }
               ;

LogicalExpr    : Expr T_And Expr
                 { $$ = new LogicalExpr($1, new Operator(@2, "&&"), $3); }
               | Expr T_Or Expr
                 { $$ = new LogicalExpr($1, new Operator(@2, "||"), $3); }
               ;

PostfixExpr    : PrimaryExpr
                 { $$ = $1; }
               | PostfixExpr T_Inc
                 { $$ = new PostfixExpr($1, new Operator(@2, "++")); }
               | PostfixExpr T_Dec
                 { $$ = new PostfixExpr($1, new Operator(@2, "--")); }
               | func_call_expression
                 { $$ = $1; }
               ;

func_call_expression  : func_call_header_with_parameters T_RightParen
                        { $$ = $1; }
                      | func_call_header_with_no_parameters T_RightParen
                        { $$ = $1; }
                      ;

func_call_header_with_no_parameters : FuncIdentifier T_LeftParen T_Void
                                      { $$ = new Call(@1, NULL, $1, new List<Expr*>); }
                                    | FuncIdentifier T_LeftParen
                                      { $$ = new Call(@1, NULL, $1, new List<Expr*>); }
                                    ;

func_call_header_with_parameters    : FuncIdentifier T_LeftParen ArgList
                                      { $$ = new Call(@1, NULL, $1, $3); }
                                    ;

ArgList        : AssignmentExpr
                 { ($$ = new List<Expr*>)->Append($1); }
               | ArgList T_Comma AssignmentExpr
                 { ($$ = $1)->Append($3); }
               | PrimaryExpr
                 { ($$ = new List<Expr*>)->Append($1); }
               | ArgList T_Comma PrimaryExpr
                 { ($$ = $1)->Append($3); }
               ;

FuncIdentifier : T_Identifier
                 { $$ = new Identifier(@1, $1); }
               ;

PrimaryExpr    : T_Identifier
                 { $$ = new VarExpr(@1, new Identifier(@1, $1)); }
               | constant
                 { $$ = $1; }
               | T_LeftParen Expr T_RightParen
                 { $$ = $2; }
               ;

UnaryExpr      : PostfixExpr
                 { $$ = $1; }
               | T_Inc UnaryExpr
                 { $$ = new ArithmeticExpr(new Operator(@1, "++"), $2); }
               | T_Dec UnaryExpr
                 { $$ = new ArithmeticExpr(new Operator(@1, "--"), $2); }
               | T_Plus UnaryExpr
                 { $$ = new ArithmeticExpr(new Operator(@1, "+"), $2); }
               | T_Dash UnaryExpr
                 { $$ = new ArithmeticExpr(new Operator(@1, "-"), $2); }
               ;

constant       : T_IntConstant      
                 { $$ = new IntConstant(@1, $1); }
               | T_BoolConstant    
                 { $$ = new BoolConstant(@1, $1); }
               ;
%%

/* The closing %% above marks the end of the Rules section and the beginning
 * of the User Subroutines section. All text from here to the end of the
 * file is copied verbatim to the end of the generated y.tab.c file.
 * This section is where you put definitions of helper functions.
 */

/* Function: InitParser
 * --------------------
 * This function will be called before any calls to yyparse().  It is designed
 * to give you an opportunity to do anything that must be done to initialize
 * the parser (set global variables, configure starting state, etc.). One
 * thing it already does for you is assign the value of the global variable
 * yydebug that controls whether yacc prints debugging information about
 * parser actions (shift/reduce) and contents of state stack during parser.
 * If set to false, no information is printed. Setting it to true will give
 * you a running trail that might be helpful when debugging your parser.
 * Please be sure the variable is set to false when submitting your final
 * version.
 */
void InitParser()
{
   PrintDebug("parser", "Initializing parser");
   yydebug = false;
}
