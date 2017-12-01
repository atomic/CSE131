/**
 * File: ast.h
 * ----------- 
 * This file defines the abstract base class Node and the concrete 
 * Identifier and Error node subclasses that are used through the tree as 
 * leaf nodes. A parse tree is a hierarchical collection of ast nodes (or, 
 * more correctly, of instances of concrete subclassses such as VarDecl,
 * ForStmt, and AssignExpr).
 * 
 * Location: Each node maintains its lexical location (line and columns in 
 * file), that location can be NULL for those nodes that don't care/use 
 * locations. The location is typcially set by the node constructor.  The 
 * location is used to provide the context when reporting semantic errors.
 *
 * Parent: Each node has a pointer to its parent. For a Program node, the 
 * parent is NULL, for all other nodes it is the pointer to the node one level
 * up in the parse tree.  The parent is not set in the constructor (during a 
 * bottom-up parse we don't know the parent at the time of construction) but 
 * instead we wait until assigning the children into the parent node and then 
 * set up links in both directions. The parent link is typically not used 
 * during parsing, but is more important in later phases.
 *
 * Printing: The only interesting behavior of the node classes for pp2 is the 
 * bility to print the tree using an in-order walk.  Each node class is 
 * responsible for printing itself/children by overriding the virtual 
 * PrintChildren() and GetPrintNameForNode() methods. All the classes we 
 * provide already implement these methods, so your job is to construct the
 * nodes and wire them up during parsing. Once that's done, printing is a snap!

 */

#ifndef _H_ast
#define _H_ast

#include <stdlib.h>   // for NULL
#include <iostream>
#include <string>
#include "location.h"
#include <vector>
#include <set>

using namespace std;
class SymbolTable;

enum tactype { label, instr, stmt, call, print, branch, jump };
enum sccode { sc_None,
    sc_PrintInt, sc_ReadInt,
    sc_MemAlloc  // for BeginFunc ? not sure
};

struct TACObject{
    string lhs;    // lhs of the TAC
    string rhs;    // rhs of the TAC
    int bytes;     // # of bytes for this TAC
    tactype type;
    sccode sc_code;

    TACObject(string lhs, string rhs, int b, tactype type, sccode sc_code = sc_None ) :
    lhs(lhs),
    rhs(rhs),
    bytes(b),
    type(type),
    sc_code(sc_code) {
    }
};

class Node  {
  protected:
    yyltype *location;
    Node *parent;
    static SymbolTable *symtab;

    // Declare any global variables you need here
    // And initialize them in ast.cc
    static int stackRegister;
    static int tempRegister;
    static int labelCounter;
    static vector<TACObject> TACContainer;

  public:
    Node(yyltype loc);
    Node();
    virtual ~Node() {}
    
    yyltype *GetLocation()   { return location; }
    void SetParent(Node *p)  { parent = p; }
    Node *GetParent()        { return parent; }

    virtual const char *GetPrintNameForNode() = 0;
    
    // Print() is deliberately _not_ virtual
    // subclasses should override PrintChildren() instead
    void Print(int indentLevel, const char *label = NULL); 
    virtual void PrintChildren(int indentLevel)  {}
    virtual string Emit();
};
   

class Identifier : public Node 
{
  protected:
    char *name;
    
  public:
    Identifier(yyltype loc, const char *name);
    const char *GetPrintNameForNode()   { return "Identifier"; }
    void PrintChildren(int indentLevel);
    char *GetName() const { return name; }

    // virtual string Emit();
};


// This node class is designed to represent a portion of the tree that 
// encountered syntax errors during parsing. The partial completed tree
// is discarded along with the states being popped, and an instance of
// the Error class can stand in as the placeholder in the parse tree
// when your parser can continue after an error.
class Error : public Node
{
  public:
    Error() : Node() {}
    const char *GetPrintNameForNode()   { return "Error"; }
};



#endif
