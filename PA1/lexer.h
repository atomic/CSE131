#ifndef _H_scanner
#define _H_scanner

#include <stdio.h>

#define MaxIdentLen 31    // Maximum length for identifiers

/* Typedef: TokenType enum
 * -----------------------
 * This enumeration defines the constants for the different token types.
 * The scanner should return these values for the associated key words
 * and token types.  The numbers start at 256 because 0-255 are reserved
 * for single character token values. After pp1, we will rely on
 * y.tab.h generated by yacc for these constants.
 */
typedef enum { 
    T_Void = 256,
    T_Bool, 
    T_Int, 
    T_While, 
    T_Break,
    T_Continue,
    T_Else, 
    T_For, 
    T_If, 
    T_Return, 
    T_Identifier, 
    T_IntConstant, 
    T_BoolConstant, 
    T_Inc,
    T_Dec,
    T_LessEqual,
    T_GreaterEqual,
    T_EQ,
    T_NE,
    T_And, 
    T_Or,
    T_MulAssign,
    T_DivAssign,
    T_AddAssign,
    T_SubAssign,
    T_LeftParen,
    T_RightParen,
    T_LeftBracket,
    T_RightBracket,
    T_LeftBrace,
    T_RightBrace,
    T_Equal,
    T_Semicolon,
    T_Dash,
    T_Plus,
    T_Star,
    T_Slash,
    T_Comma,
    T_LeftAngle,
    T_RightAngle,
    T_Public,
    T_Private,
    T_Static,
    T_Class,
    T_NumTokenTypes
} TokenType;

/* These are a list of printable names for each token value defined
 * above.  The strings should match in position to the types. They
 * are used in our main program to verify output from your scanner.
 */
static const char *gTokenNames[T_NumTokenTypes] = {
  "T_Void", 
  "T_Bool", 
  "T_Int", 
  "T_While", 
  "T_Break", 
  "T_Continue",
  "T_Else", 
  "T_For", 
  "T_If", 
  "T_Return",
  "T_Identifier",
  "T_IntConstant",  
  "T_BoolConstant", 
  "T_Inc",
  "T_Dec",
  "T_LessEqual",
  "T_GreaterEqual",
  "T_EQ",
  "T_NE",
  "T_And", 
  "T_Or", 
  "T_MulAssign",
  "T_DivAssign",
  "T_AddAssign",
  "T_SubAssign",
  "T_LeftParen",
  "T_RightParen",
  "T_LeftBracket",
  "T_RightBracket",
  "T_LeftBrace",
  "T_RightBrace",
  "T_Equal",
  "T_Semicolon",
  "T_Dash",
  "T_Plus",
  "T_Star",
  "T_Slash",
  "T_Comma",
  "T_LeftAngle",
  "T_RightAngle",
  "T_Public",
  "T_Private",
  "T_Static",
  "T_Class"
};

/* Typedef: YYSTYPE
 * ----------------
 * Defines the union type that is used by the scanner to store
 * attibute information about tokens as they are scanned. This
 * type definition will be generated by Yacc in the later assignments.
 */
typedef union {
    int integerConstant;
    bool boolConstant;
    char identifier[MaxIdentLen+1]; // +1 for terminating null
} YYSTYPE;

 
/* Global variable: yylval
 * -----------------------
 * Strange name, but by convention, this is the name of union that
 * is written to by scanner and read by parser containing information
 * about the lexeme just scanned.
 */
extern YYSTYPE yylval;

extern char *yytext;      // Text of lexeme just scanned


int yylex();              // Defined in the generated lex.yy.c file

#endif