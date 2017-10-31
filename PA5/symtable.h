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
    bool is_loop;
    bool is_switch;

    bool is_naked;

    bool has_return;

    FnDecl* c_func_decl;
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
        // void PrintTable();
        FnDecl* GetCurrentFnDecl();
        
        void SetCurrentScopeIsLoop(bool isLoop);
        bool CurrentScopeIsLoop();
        bool IsInLoop(Stmt* statement);
        
        void SetCurrentScopeIsSwitch(bool isSwitch);
        bool CurrentScopeIsSwitch();
        bool IsInSwitch(Stmt* statement);

        void SetCurrentScopeNaked(bool naked);
        bool IsCurrentScopeNaked();

        void SetCurrentFnDeclHasReturn();
        bool IsCurrentScopeHasReturn();

        void SetCurrentFnDecl(FnDecl* func_decl);

};
