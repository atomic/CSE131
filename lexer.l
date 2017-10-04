/* File:  lexer.l
 * ----------------
 * Lex input file to generate the lexer for the compiler.
 */
 
%{
#include <iostream>
#include "lexer.h"
#include "location.h"
#include "errors.h"
#include <vector>
using namespace std;

#define TAB_SIZE 8

/* Global variable: yylval
 * -----------------------
 * This global variable is how we get attribute information about the token
 * just scanned to the client. The scanner sets the global variable
 * appropriately and since it's global the client can just read it.  In the
 * future, this variable will be declared for us in the y.tab.c file
 * produced by Yacc, but for now, we declare it manually.
 */
YYSTYPE yylval;  // manually declared for pp1, later Yacc provides

/* Global variable: yylloc
 * -----------------------
 * This global variable is how we get position information about the token
 * just scanned to the client. (Operates similarly to yylval above)
 */
struct yyltype yylloc; // manually dclared for pp1, later Yacc provides


/* Macro: YY_USER_ACTION 
 * ---------------------
 * This flex built-in macro can be defined to provide an action which is
 * always executed prior to any matched rule's action. Basically, it is
 * a way of having a piece of code common to all actions factored out to
 * this routine.  We already defined it for you and left the empty
 * function DoBeforeEachAction ready for your use as needed. It will
 * be called once for each pattern scanned from the file, before
 * executing its action.
 */
static void DoBeforeEachAction(); 
#define YY_USER_ACTION DoBeforeEachAction();

int currentLineNum, currentColNum;
%}

/* States
 * ------
 * A little wrinkle on states is the COPY exclusive state which
 * I added to first match each line and copy it ot the list of lines
 * read before re-processing it. This allows us to print the entire
 * line later to provide context on errors.
 */

/* Definitions
 * -----------
 * To make our rules more readable, we establish some definitions here.
 */
letters             ([A-Za-z])
digits              ([0-9])
%% 

"void"              { return T_Void;        }

"true"|"false"      { yylval.boolConstant = (yytext[0] == 't');
                         return T_BoolConstant; }

[+\-]?[0-9]+     { 
            yylval.integerConstant = atoi(yytext);
            return T_IntConstant;
}

"bool"     { return T_Bool;         }
"int"      { return T_Int;          }
"while"    { return T_While;        }
"break"    { return T_Break;        }
"continue" { return T_Continue;     }
"else"     { return T_Else;         }
"for"      { return T_For;          }
"if"       { return T_If;           }
"return"   { return T_Return;       }
"++"       { return T_Inc;          }
"--"       { return T_Dec;          }
"<="       { return T_LessEqual;    }
">="       { return T_GreaterEqual; }
"="        { return T_EQ;           }
"!="       { return T_NE;           }
"&&"       { return T_And";         }
"||"       { return T_Or;           }
"*="       { return T_MulAssign;    }
"/="       { return T_DivAssign;    }
"+="       { return T_AddAssign;    }
"-="       { return T_SubAssign;    }
"("        { return T_LeftParen;    }
")"        { return T_RightParen;   }
"["        { return T_LeftBracket;  }
"]"        { return T_RightBracket; }
"{"        { return T_LeftBrace;    }
"}"        { return T_RightBrace;   }
"="        { return T_Equal;        }
";"        { return T_Semicolon;    }
"-"        { return T_Dash;         }
"+"        { return T_Plus;         }
"*"        { return T_Star;         }
"/"        { return T_Slash;        }
","        { return T_Comma;        }
"<"        { return T_LeftAngle;    }
">"        { return T_RightAngle;   }
"public"   { return T_Public;       }
"private"  { return T_Private;      }
"static"   { return T_Static;       }
"class"    { return T_Clas;         }
{letters}({digits}|{letters})+  { return T_Identifier; }

%%

/* Function: InitLexer
 * ---------------------
 * This function will be called before any calls to yylex().  It is designed
 * to give you an opportunity to do anything that must be done to initialize
 * the scanner (set global variables, configure starting state, etc.). One
 * thing it already does for you is assign the value of the global variable
 * yy_flex_debug that controls whether flex prints debugging information
 * about each token and what rule was matched. If set to false, no information
 * is printed. Setting it to true will give you a running trail that might
 * be helpful when debugging your scanner. Please be sure the variable is
 * set to false when submitting your final version.
 */
void InitLexer()
{
    yy_flex_debug = true;
    currentLineNum = 1;
    currentColNum = 1;
}

/* Function: DoBeforeEachAction()
 * ------------------------------
 * This function is installed as the YY_USER_ACTION. This is a place
 * to group code common to all actions.
 */
static void DoBeforeEachAction()
{
    yylloc.first_line = currentLineNum;
    yylloc.last_line = currentLineNum;

    yylloc.first_column = currentColNum;
    yylloc.last_column = currentColNum + yyleng - 1;
    currentColNum += yyleng;
}

static void PrintOneToken(TokenType token, const char *text, YYSTYPE value,
                          yyltype loc)
{
  char buffer[] = {'\'', (char)token, '\'', '\0'};
  const char *name = token >= T_Void ? gTokenNames[token - T_Void] : buffer;

  printf("%-12s line %d cols %d-%d is %s ", text,
       loc.first_line, loc.first_column, loc.last_column, name);

  switch(token) {
    case T_IntConstant:     
      printf("(value = %d)\n", value.integerConstant); break;
    case T_BoolConstant:    
      printf("(value = %s)\n", value.boolConstant ? "true" : "false"); break;
    case T_Identifier:
  if (strcmp(text, value.identifier)) {
    printf("(truncated to %s)\n", value.identifier);
    break;
  }
    default:
      printf("\n"); break;
  }
}

int main() {
    InitLexer();
    TokenType token;
    while ((token = (TokenType)yylex()) != 0) {
        PrintOneToken(token, yytext, yylval, yylloc);
    }
    return 0;
}
