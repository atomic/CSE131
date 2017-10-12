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

%token   T_Identifier
%token   T_IntConstant
%token   T_BoolConstant

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
%type <integerConstant>  DeclList
%type <integerConstant>  Decl
%type <integerConstant>  single_declaration 
%type <integerConstant>  function_definition 
%type <integerConstant>  assignment_operator 
%type <integerConstant>  expression 
%type <integerConstant>  function_prototype 
%type <integerConstant>  compound_statement_with_scope 
%type <integerConstant>  function_prototype_header
%type <integerConstant>  parameter_declaration_list
%type <integerConstant>  parameter_declaration
%type <integerConstant>  statement_list
%type <integerConstant>  statement
%type <integerConstant>  simple_statement
%type <integerConstant>  expression_statement
%type <integerConstant>  selection_statement
%type <integerConstant>  iteration_statement
%type <integerConstant>  return_statement
%type <integerConstant>  decl_statement
%type <integerConstant>  break_statement
%type <integerConstant>  while_statement
%type <integerConstant>  for_statement
%type <integerConstant>  condition
%type <integerConstant>  assignment_expression
%type <integerConstant>  arithmetic_expression
%type <integerConstant>  relational_expression
%type <integerConstant>  equality_expression
%type <integerConstant>  logical_expression
%type <integerConstant>  unary_expression
%type <integerConstant>  postfix_expression
%type <integerConstant>  primary_expression
%type <integerConstant>  func_call_expression
%type <integerConstant>  func_call_header_with_parameters
%type <integerConstant>  func_call_header_with_no_parameters
%type <integerConstant>  function_identifier
%type <integerConstant>  arg_list
%type <integerConstant>  constant


%%
/* Rules
 * -----
 * All productions and actions should be placed between the start and stop
 * %% markers which delimit the Rules section.

 */
Program   :    DeclList            {
                                      @1;
                                      /* pp2: The @1 is needed to convince
                                       * yacc to set up yylloc. You can remove
                                       * it once you have other uses of @n*/
                                      if (ReportError::NumErrors() == 0)
                                          printf("Successfully finished parsing\n");
                                    }
          ;

DeclList  :    DeclList Decl        
          |    Decl                 
          ;

Decl      :    single_declaration   
          |    function_definition
          ;

single_declaration  : type_specifier T_Identifier T_Semicolon   
                    | type_specifier T_Identifier assignment_operator expression T_Semicolon
                    ;

type_specifier      : T_Void     
                    | T_Int      
                    | T_Bool     
                    ;

function_definition : function_prototype compound_statement_with_scope
                    | function_prototype T_Semicolon
                    ;

function_prototype  : function_prototype_header T_RightParen
                    ;

function_prototype_header  : type_specifier T_Identifier T_LeftParen
                           | type_specifier T_Identifier T_LeftParen parameter_declaration_list
                           ;

parameter_declaration_list : parameter_declaration_list T_Comma parameter_declaration
                           | parameter_declaration
                           ;

parameter_declaration      : type_specifier T_Identifier
                           ;

compound_statement_with_scope : T_LeftBrace statement_list T_RightBrace
                              | T_LeftBrace T_RightBrace
                              ;

statement_list        : statement_list statement
                      | statement
                      ;

statement             : compound_statement_with_scope
                      | simple_statement
                      ;

simple_statement      : expression_statement
                      | selection_statement
                      | iteration_statement
                      | return_statement
                      | decl_statement
                      | break_statement
                      ;

expression_statement  : T_Semicolon
                      | expression T_Semicolon
                      ;

selection_statement   : T_If T_LeftParen expression T_RightParen compound_statement_with_scope
                      | T_If T_LeftParen expression T_RightParen compound_statement_with_scope T_Else compound_statement_with_scope
                      ;

iteration_statement   : while_statement
                      | for_statement
                      ;

while_statement       : T_While T_LeftParen condition T_RightParen statement
                      ;

for_statement         : T_For T_LeftParen expression_statement expression_statement expression T_RightParen statement
                      ;

condition             : expression
                      ;

return_statement      : T_Return expression_statement
                      ;

decl_statement        : single_declaration
                      ;

break_statement       : T_Break T_Semicolon
                      ;

expression            : assignment_expression
                      | arithmetic_expression
                      | relational_expression
                      | equality_expression
                      | logical_expression
                      | unary_expression
                      ;

assignment_expression : unary_expression assignment_operator expression
                      ;

assignment_operator   : T_Equal
                      | T_MulAssign
                      | T_DivAssign
                      | T_AddAssign
                      | T_SubAssign
                      ;

arithmetic_expression : expression T_Plus expression
                      | expression T_Dash expression
                      | expression T_Star expression
                      | expression T_Slash expression
                      ;

relational_expression : expression T_LeftAngle expression
                      | expression T_RightAngle expression
                      | expression T_LessEqual expression
                      | expression T_GreaterEqual expression
                      ;

equality_expression   : expression T_EQ expression
                      | expression T_NE expression
                      ;

logical_expression    : expression T_And expression
                      | expression T_Or expression
                      ;

postfix_expression    : primary_expression
                      | postfix_expression T_Inc
                      | postfix_expression T_Dec
                      | func_call_expression
                      ;

func_call_expression  : func_call_header_with_parameters T_RightParen
                      | func_call_header_with_no_parameters T_RightParen
                      ;

func_call_header_with_no_parameters : function_identifier T_LeftParen T_Void
                                    | function_identifier T_LeftParen
                                    ;

func_call_header_with_parameters    : function_identifier T_LeftParen arg_list
                                    ;

arg_list              : assignment_expression
                      | arg_list T_Comma assignment_expression
                      | primary_expression
                      | arg_list T_Comma primary_expression
                      ;

function_identifier   : T_Identifier
                      ;

primary_expression    : T_Identifier
                      | constant
                      | T_LeftParen expression T_RightParen
                      ;

unary_expression      : postfix_expression
                      | T_Inc unary_expression
                      | T_Dec unary_expression
                      | T_Plus unary_expression
                      | T_Dash unary_expression
                      ;

constant              : T_IntConstant
                      | T_BoolConstant
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
