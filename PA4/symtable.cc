/*
 * Symbol table implementation
 *
 */

 #include "symtable.h"

SymbolTable::SymbolTable() {
    //Add the initial scope
    Scope initial_scope;
    symtab_vec.push_back(initial_scope);
    current_scope = &(symtab_vec.back());
 }

void SymbolTable::PushScope() {
    Scope new_scope;
    new_scope.has_return = false;

    symtab_vec.push_back(new_scope);
    current_scope = &(symtab_vec.back());
}

void SymbolTable::PopScope() {
    symtab_vec.pop_back();
    current_scope = &(symtab_vec.back());
}

void SymbolTable::AddSymbol(string name, Decl* decl_obj) {

    // Result of insert, used to check success of insertion
    pair<map<string, Decl*>::iterator, bool> result;
    result = (current_scope->scope_map).insert(pair<string, Decl*>(name, decl_obj));

    // if(result.second) cout << "Successfully add symbol: " << name << endl;
    // else cout << "Fail to add symbol: " << name << endl;
}

bool SymbolTable::IsInCurrentScope(string name) {
    return ( (current_scope->scope_map).count(name) != 0 );
}

bool SymbolTable::IsInAllScopes(string name) {
    bool result = false;

    for (int i = symtab_vec.size() - 1; i >= 0 ; i--)
    {
        if (symtab_vec[i].scope_map.count(name) != 0) result = true;
    }    

    return result;
}

Decl* SymbolTable::FindSymbolInCurrentScope(string name) {
    typedef map<string, Decl*>::const_iterator MapIterator;    
    for (MapIterator iter = (current_scope->scope_map).begin(); iter != (current_scope->scope_map).end(); iter++)
    {            
        if(iter->first == name) return iter->second;    
    }
    
    // cout << "Cannot find symbol: " << name << " in SymbolTable::FindSymbolInCurrentScope()!!!" << endl;
    return NULL;
}

Decl* SymbolTable::FindSymbolInAllScopes(string name) {
    for (int i = symtab_vec.size() - 1; i >= 0 ; i--)
    {
        typedef map<string, Decl*>::const_iterator MapIterator;    
        for (MapIterator iter = symtab_vec[i].scope_map.begin(); iter != symtab_vec[i].scope_map.end(); iter++)
        {            
            if(iter->first == name) return iter->second;                            
        }
    }  

    // cout << "Cannot find symbol: " << name << " in SymbolTable::FindSymbolInAllScopes()!!!" << endl;
    return NULL;    
}