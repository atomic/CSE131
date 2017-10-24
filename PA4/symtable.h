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

struct Scope{
    map<string, Decl*> scope_map;
    bool has_return;
};

class SymbolTable {
    protected:
        vector<Scope> symtab_vec;
        Scope *current_scope;

    public:
        SymbolTable();     
        void PushScope();
        void PopScope();
        void AddSymbol(string name, Decl* decl_obj);

        bool IsInCurrentScope(string name);
        bool IsInAllScopes(string name);
        Decl* FindSymbolInCurrentScope(string name);
        Decl* FindSymbolInAllScopes(string name);
};
