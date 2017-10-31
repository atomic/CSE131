/*
 * Symbol table implementation
 *
 */

 #include "symtable.h"

 SymbolTable::SymbolTable() {
    //Add the initial scope
    Scope initial_scope;

    initial_scope.is_loop = false;
    initial_scope.is_switch = false;
    initial_scope.has_return = false;

    symtab_vec.push_back(initial_scope);
    current_scope = &(symtab_vec.back());
 }

 void SymbolTable::PushScope() {
    // printf("PushScope\n");
    Scope new_scope;

    new_scope.is_loop = current_scope->is_loop;
    new_scope.is_switch = current_scope->is_switch;
    new_scope.has_return = false;

    symtab_vec.push_back(new_scope);
    current_scope = &(symtab_vec.back());
 }

 void SymbolTable::PopScope() {
    // printf("PopScope\n");  
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

 /*
  * Return true if symbol with the same key(same identifier)
  * already exists in the current scope  
  */
 bool SymbolTable::IsInCurrentScope(string name) {
    return ( (current_scope->scope_map).count(name) != 0 );
 }

 /*
  * Return true if symbol with the same key(same identifier)
  * exist in parent scopes(other scopes except for current scope) 
  */
 bool SymbolTable::IsInAllScopes(string name) {
    bool result = false;

    for (int i = symtab_vec.size() - 1; i >= 0 ; i--)
    {
        if (symtab_vec[i].scope_map.count(name) != 0) result = true;
    }    

    return result;
 }

 /*
  * Return the Decl* with the specified name in current scope
  */
  Decl* SymbolTable::FindSymbolInCurrentScope(string name){

    typedef map<string, Decl*>::const_iterator MapIterator;    
    for (MapIterator iter = (current_scope->scope_map).begin(); iter != (current_scope->scope_map).end(); iter++)
    {            
        if(iter->first == name) return iter->second;    
    }
    
    // cout << "Cannot find symbol: " << name << " in SymbolTable::FindSymbolInCurrentScope()!!!" << endl;
    return NULL;
  }

 /*
  * Return the Decl* with the specified name in parent scopes
  */
  Decl* SymbolTable::FindSymbolInAllScopes(string name){

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

 /*
  * Return the FnDecl object of the current scope(the big function surrounds the current scope)
  */
  FnDecl* SymbolTable::GetCurrentFnDecl(){
    
    // if(symtab_vec.size() == 1) {
    //     printf("Cannot return a FnDecl objec in SymbolTable::GetCurrentFnDecl(), only one scope in symtab\n");
    // }
    // else {        
    //     for(int i = symtab_vec.size() - 1 ;i >= 0; i--){
          
    //       typedef map<string, Decl*>::const_iterator MapIterator; 
    //       for (MapIterator iter = (symtab_vec[i].scope_map).begin(); iter != (symtab_vec[i].scope_map).end(); iter++)
    //       {            
    //           Decl* the_decl = iter->second;        
    //           FnDecl *result_decl = dynamic_cast<FnDecl *>(the_decl);
    //           if(result_decl != NULL) {
    //               return result_decl;
    //           }
    //       }

    //     }
    // }

    // printf("Cannot return a FnDecl objec in SymbolTable::GetCurrentFnDecl(), something is wrong!!!\n");
    // return NULL;

    return current_scope->c_func_decl;
  }
  
  /*
   * Set a current scope's is_loop flag to indicate whether the scope is of a loop
   */
  void SymbolTable::SetCurrentScopeIsLoop(bool isLoop){
    current_scope->is_loop = isLoop;
  }
  
  /*
   * Check whether a certain Stmt is in a loop or not
   */
  bool SymbolTable::IsInLoop(Stmt* statement){
    for(int i = symtab_vec.size() - 1; i >= 0; i--){
        if(symtab_vec[i].is_loop){
            return true;
        }
    }

    return false;
  }  

  /*
   * Check whether current scope is loop or not
   */
   bool SymbolTable::CurrentScopeIsLoop(){
        return current_scope->is_loop;
   }

  /*
   * Set a current scope's is_switch flag to indicate whether the scope is of a loop
   */
  void SymbolTable::SetCurrentScopeIsSwitch(bool isSwitch){
    current_scope->is_switch = isSwitch;
  }
  
  /*
   * Check whether a certain Stmt is in a switch or not
   */
  bool SymbolTable::IsInSwitch(Stmt* statement){
    for(int i = symtab_vec.size() - 1; i >= 0; i--){
        if(symtab_vec[i].is_switch){
            return true;
        }
    }

    return false;
  }  

  /*
   * Check whether current scope is switch or not
   */
   bool SymbolTable::CurrentScopeIsSwitch(){
        return current_scope->is_switch;
   }

   
   void SymbolTable::SetCurrentScopeNaked(bool naked){
      current_scope->is_naked = naked;
   }
   
   bool SymbolTable::IsCurrentScopeNaked(){
      return current_scope->is_naked;
   }
   
  /*
  * Set the has_return flag of the Scope of current FnDecl
  */
  void SymbolTable::SetCurrentFnDeclHasReturn(){
    
    // bool success_set = false;

    // if(symtab_vec.size() == 1) {
    //     printf("Cannot return a FnDecl objec in SymbolTable::GetCurrentFnDecl(), only one scope in symtab\n");
    // }
    // else {        
    //     for(int i = symtab_vec.size() - 1 ;i >= 0; i--){
          
    //       typedef map<string, Decl*>::const_iterator MapIterator; 
    //       for (MapIterator iter = (symtab_vec[i].scope_map).begin(); iter != (symtab_vec[i].scope_map).end(); iter++)
    //       {            
    //           Decl* the_decl = iter->second;        
    //           FnDecl *result_decl = dynamic_cast<FnDecl *>(the_decl);
    //           if(result_decl != NULL) {
    //               symtab_vec[i].has_return = true;
    //               success_set = true;
    //           }
    //       }
    //     }

    //     if(!success_set){
    //       printf("Failed to set CurrentFnDeclHasReturn!!!\n");
    //     }        
    // }

    current_scope->has_return = true;

  }

   bool SymbolTable::IsCurrentScopeHasReturn(){
      return current_scope->has_return;
   }

   void SymbolTable::SetCurrentFnDecl(FnDecl* func_decl){
      current_scope->c_func_decl = func_decl;
   }

 // void SymbolTable::PrintTable() {

 //    cout << "*********************************************" << endl;
 //    cout << "SymbolTable Content Printout:" << endl;
 //    /************* Print out current scope *************/
 //    cout << "Current Scope:" << endl;
 //    typedef map<string, Decl*>::const_iterator MapIterator;    
 //    for (MapIterator iter = current_scope->begin(); iter != current_scope->end(); iter++)
 //    {        
 //        cout << "Key: " << iter->first << endl;
 //    }
 //    cout << "Size of current scope is: " << current_scope->size() << endl << endl;

 //    /************* Print out other scopes *************/
 //    for (int i = 0; i < symtab_vec.size() - 1 ; ++i)
 //    {
 //        cout << i << "th scope:" << endl;
 //        map<string, Decl*> ith_scope = symtab_vec[i];

 //        typedef map<string, Decl*>::const_iterator MapIterator;    
 //        for (MapIterator iter = ith_scope.begin(); iter != ith_scope.end(); iter++)
 //        {            
 //            cout << "Key: " << iter->first << endl;
 //        }
 //        cout << "Size of this scope is: " << ith_scope.size() << endl << endl;
 //    }

 //    cout << "Size of SymbolTable is: " << symtab_vec.size() << endl;
 //    cout << "**********************************************" << endl;

 // }
