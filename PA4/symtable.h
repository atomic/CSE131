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

enum ScopeType { Loop, Other };

struct Scope {
    map<string, Decl*> scope_map;
    bool has_return;
    bool is_breakable;
};

class SymbolTable {
    protected:
        vector<Scope> symtab_vec;
        Scope *current_scope;

    public:
        SymbolTable();     
        void PushScope();
        void PushLoopScope();
        void PopScope();
        void AddSymbol(string name, Decl* decl_obj);

        bool HasLoopScope();
        bool IsCurrentScopeBreakable();
        bool IsInCurrentScope(string name);
        bool IsInAllScopes(string name);
        Decl* FindSymbolInCurrentScope(string name);
        Decl* FindSymbolInAllScopes(string name);

    private:
        Scope CreateScope(ScopeType type);
};
