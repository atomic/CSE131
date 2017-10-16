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
#include "utility.h"
#include "parser.h"
using namespace std;

#define TAB_SIZE 8
vector<const char*> savedLines;

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
CHARS               ([A-Za-z_])
DIGITS              ([0-9])
INLINE_COMMENT      ("//")
COMMENT_START       ("/*")
COMMENT_END         ("*/")
WHITESPACE          (" ")
ILLEGAL_CHARS       ([\^~#@$'\\"!\.%|&])
%x comment
%x COPY
%option stack
%% 

<COPY>.*              { savedLines.push_back(strdup(yytext)); 
                        currentColNum = 1;
                        yy_pop_state(); 
                        yyless(0);      }
<COPY><<EOF>>         { yy_pop_state(); }

<*>\n                 { currentLineNum++; 
                        currentColNum = 1;
                        if (YYSTATE == COPY) savedLines.push_back("");
                        else yy_push_state(COPY); }

"/*"                    BEGIN(comment);
<comment>"*/"           BEGIN(INITIAL);
<comment>[^*\n]+
<comment>"*"+[^*/\n]*

"void"                { return T_Void;         }
"true"|"false"        { yylval.boolConstant = (yytext[0] == 't');
                        return T_BoolConstant; }
{DIGITS}+             { yylval.integerConstant = atoi(yytext);
                        return T_IntConstant;  }

"boolean"  { return T_Bool;         }       /* Keywords */
"int"      { return T_Int;          }
"while"    { return T_While;        }
"break"    { return T_Break;        }
"continue" { return T_Continue;     }
"else"     { return T_Else;         }
"for"      { return T_For;          }
"if"       { return T_If;           }
"return"   { return T_Return;       }
"public"   { return T_Public;       }
"private"  { return T_Private;      }
"static"   { return T_Static;       }
"class"    { return T_Class;        }

"++"       { return T_Inc;          }       /* Operators */
"--"       { return T_Dec;          }
"<="       { return T_LessEqual;    }
">="       { return T_GreaterEqual; }
"=="       { return T_EQ;           }
"!="       { return T_NE;           }
"&&"       { return T_And;          }
"||"       { return T_Or;           }
"*="       { return T_MulAssign;    }
"/="       { return T_DivAssign;    }
"+="       { return T_AddAssign;    }
"-="       { return T_SubAssign;    }
"-"        { return T_Dash;         }       /* Single Operators */
"+"        { return T_Plus;         }
"*"        { return T_Star;         }
"/"        { return T_Slash;        }

"("        { return T_LeftParen;    }       /* Braces */
")"        { return T_RightParen;   }
"["        { return T_LeftBracket;  }
"]"        { return T_RightBracket; }
"{"        { return T_LeftBrace;    }
"}"        { return T_RightBrace;   }
"="        { return T_Equal;        }
";"        { return T_Semicolon;    }

","        { return T_Comma;        }       /* Others */
"<"        { return T_LeftAngle;    }
">"        { return T_RightAngle;   }

{ILLEGAL_CHARS}     { ReportError::UnrecogChar(&yylloc, yytext[0]); }
<comment><<EOF>>    { ReportError::UntermComment(); yyterminate();  }

{CHARS}({DIGITS}|{CHARS})*  { 
    strcpy(yylval.identifier, yytext);
    return T_Identifier;
}

{INLINE_COMMENT}.*
{WHITESPACE}
\t            { currentColNum += (TAB_SIZE + 1) - (currentColNum % TAB_SIZE); }

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
    yy_flex_debug = false;
    currentLineNum = 1;
    currentColNum = 1;
    yy_push_state(COPY);
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

/* Function: GetLineNumbered()
 * ---------------------------
 * Returns string with contents of line numbered n or NULL if the
 * contents of that line are not available. Our scanner copies each line scanned
 * and appends each to a list so we can later retrieve them to report the
 * context for errors.
 */
 const char *GetLineNumbered(int num) {
     if (num <= 0 || num > savedLines.size()) return NULL;
     return savedLines[num - 1];
 }
