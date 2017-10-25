/**
 * File: symtable.h
 * ----------- 
 *  Header file for Symbol table implementation.
 */
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <iostream>
#include "ast_decl.h"
#include "ast_stmt.h"

enum ScopeCreator { Loop, Func, IfElse };

struct Scope {
    map<string, Decl*> scope_map;
    bool has_return;

    ScopeCreator creator;
    string creator_name;
};

class SymbolTable {

    protected:
        vector<Scope> symtab_vec;
        Scope *current_scope;

    public:
        SymbolTable();     
        void PushScope(ScopeCreator creator);
        void PopScope();
        void AttachSymbolToCurrentScope(string name);
        void AddSymbol(string name, Decl* decl_obj);
        void ReturnStmtDoesExist();

        bool CurrentScopeHasReturn();
        bool ContainsLoopScope();
        bool IsInCurrentScope(string name);
        bool IsInAllScopes(string name);
        Decl* FindSymbolInCurrentScope(string name);
        Decl* FindSymbolInAllScopes(string name);

};
