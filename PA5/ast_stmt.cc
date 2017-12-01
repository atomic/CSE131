/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include <cctype>
#include <unordered_map>
#include <sstream>
#include <iterator>

Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

void Program::PrintChildren(int indentLevel) {
    decls->PrintAll(indentLevel+1);
    printf("\n");
}

StmtBlock::StmtBlock(List<Stmt*> *s) {
    Assert(s != NULL);
    (stmts=s)->SetParentAll(this);
}

void StmtBlock::PrintChildren(int indentLevel) {
    stmts->PrintAll(indentLevel+1);
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) {
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this);
    (body=b)->SetParent(this);
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) {
    Assert(i != NULL && t != NULL && s != NULL && b != NULL);
    (init=i)->SetParent(this);
    (step=s)->SetParent(this);
}

void ForStmt::PrintChildren(int indentLevel) {
    init->Print(indentLevel+1, "(init) ");
    test->Print(indentLevel+1, "(test) ");
    step->Print(indentLevel+1, "(step) ");
    body->Print(indentLevel+1, "(body) ");
}

void WhileStmt::PrintChildren(int indentLevel) {
    test->Print(indentLevel+1, "(test) ");
    body->Print(indentLevel+1, "(body) ");
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) {
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}

void IfStmt::PrintChildren(int indentLevel) {
    if (test) test->Print(indentLevel+1, "(test) ");
    if (body) body->Print(indentLevel+1, "(then) ");
    if (elseBody) elseBody->Print(indentLevel+1, "(else) ");
}

ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) {
    Assert(e != NULL);
    (expr=e)->SetParent(this);
}

void ReturnStmt::PrintChildren(int indentLevel) {
    expr->Print(indentLevel+1);
}

DeclStmt::DeclStmt(yyltype loc, Decl *decl) : Stmt(loc) {
    Assert(decl != NULL);
    (varDecl=decl)->SetParent(this);
}

void DeclStmt::PrintChildren(int indentLevel) {
    varDecl->Print(indentLevel+1);
}

void split(const string& s, const string& delim, vector<string>& v, bool clear = true) {
    int start = 0;
    int end = s.find(delim, start);

    if (clear)
        v.clear();

    while (end != string::npos) {
        v.push_back(s.substr(start, end - start));
        start = end + delim.length();
        end = s.find(delim, start);
    }

   v.push_back(s.substr(start, s.length()));
}

bool isNumeric(const string& s) {
    return s.find_first_not_of("0123456789") == string::npos;
}

bool isOperator(const string& s) {
    return s.find_first_not_of("+-/*%^=") == string::npos;
}

/**
 * evaluate arithmetic expressions in code
 * @param TACContainer
 * @return
 */
void constantFolding(vector<TACObject>& TACContainer) {
    vector<string> split_result;

    for (int i = 0; i < TACContainer.size(); ++i) {
        if (TACContainer[i].type != stmt)
            continue;

        split(TACContainer[i].rhs, " ", split_result);
        
        if (split_result.size() != 3)
            continue;

        bool qualified = isNumeric(split_result[0]) && isNumeric(split_result[2]);

        if (qualified) {
            int a = stoi(split_result[0]);
            int b = stoi(split_result[2]);

            string op = split_result[1];
            if (op.compare("+") == 0)
                TACContainer[i].rhs = to_string(a + b);
            else if (op.compare("-") == 0)
                TACContainer[i].rhs = to_string(a - b);
            else if (op.compare("/") == 0)
                TACContainer[i].rhs = to_string(a / b);
            else if (op.compare("*") == 0) {
                TACContainer[i].rhs = to_string(a * b);
            }
        }
    }
}

/**
 * remove unused initalized variables from code
 * @param TACContainer
 * @return
 */
void deadCodeElimination(vector<TACObject>& TACContainer) {
    vector<pair<int,string>> variables;
    vector<string> used_tokens;

    // collect all tokens both rhs and lhs
    for (int j = 0; j < TACContainer.size(); ++j) {
        TACObject &taco = TACContainer[j];
        if (taco.type == stmt) {
            split(taco.rhs, " ", used_tokens, false); // false here means update the used_tokens, instead of reseting it
            variables.emplace_back(j, taco.lhs);
        } else if (taco.type == branch)
            split(taco.lhs, " ", used_tokens, false); // for branch, lhs may contain variables used in the code
        else if (taco.type == instr && taco.lhs == "Return")
            split(taco.rhs, " ", used_tokens, false); // for return, rhs may contains variable
    }

    // Cleans up token that is not a variable (integer and operators)
    for (int i = 0; i < used_tokens.size(); ++i)
        if (isNumeric(used_tokens[i]) || isOperator(used_tokens[i]))
            used_tokens.erase(used_tokens.begin() + i-- );

    set<string> rhs_unique(used_tokens.begin(), used_tokens.end());


    // remove varible that does not appear in rhs_token (add index to remove first)
    vector<int> index_to_delete;
    for (auto var : variables)
        if (rhs_unique.find(var.second) == rhs_unique.end())
            index_to_delete.insert(index_to_delete.begin(), var.first);
    for (auto item : index_to_delete)
        TACContainer.erase(TACContainer.begin() + item);
}


/**
 * turn X = 5; X = X + 3  -->  X = 5 + 3
 * @param TACContainer
 * @return
 */
void constantPropagation(vector<TACObject>& TACContainer) {

    unordered_map<string, string> value_map;
    // collect all tokens both rhs and lhs
    vector<string> rhs_tokens;
    for (auto &taco : TACContainer) {
        if (taco.type == stmt) {
            if (isNumeric(taco.rhs)) {
                if (value_map.find(taco.lhs) != value_map.end())    // re-assignment, stop
                    break;
                value_map[taco.lhs] = taco.rhs;                     // save initialized values
            }
            else { // do replacement
                split(taco.rhs, " ", rhs_tokens);
                bool changed = false;
                for (auto &token : rhs_tokens) {
                    if (value_map.find(token) != value_map.end() ) {
                        token = value_map[token];
                        changed = true;
                    }
                }
                if (changed) {
                    stringstream imploded;
                    copy(rhs_tokens.begin(), rhs_tokens.end(), ostream_iterator<string>(imploded, " "));
                    taco.rhs = imploded.str();
                    taco.rhs.pop_back();  // remove last space from the join operation
                }
            }
        }
    }

//    for (auto pair : value_map)
//        cout << pair.first << "," << pair.second << endl;

}

string assignRegister(unordered_map<string, string> map, string key) {
    if (map.find(key) != map.end()) {
        return map[key];
    }

    auto registerStr = "t" + Node::tempRegister;
    Node::tempRegister++; Node::stackRegister;
    return registerStr;
}

void convertToMIPS(string rd, string rs, string rt, string op) {
    if (op.compare("<") == 0) {
        cout << "  slt $" + rd + ", $" + rs + ", $" + rt << endl;
    } else if (op.compare(">") == 0) {
        cout << "  slt $" + rd + ", $" + rt + ", $" + rs << endl;
    } else if (op.compare("<=") == 0) {
        cout << "  slt $" + rd + ", $" + rt + ", $" + rs << endl;
        cout << "  not $" + rd + ", $" + rd << endl;
        cout << "  andi $" + rd + ", $" + rd + ", 1" << endl;
    } else if (op.compare("=>") == 0) {
        cout << "  slt $" + rd + ", $" + rs + ", $" + rt << endl;
        cout << "  not $" + rd + ", $" + rd << endl;
        cout << "  andi $" + rd + ", $" + rd + ", 1" << endl;
    } else if (op.compare("+") == 0) {
        cout << "  add $" + rd + ", $" + rs + ", $" + rt << endl;
    } else if (op.compare("-") == 0) {
        cout << "  add $" + rd + ", $" + rs + ", $" + rt << endl;
    } else {
        cout << "" << endl;
    }
}

void generateIR(const vector<TACObject>& TACContainer) {
    for (int i = 0; i < TACContainer.size(); ++i) {
        switch(TACContainer[i].type) {
            case label:  cout << TACContainer[i].lhs + ":" << endl;
                break;

            case stmt:   cout << "    " + TACContainer[i].lhs << " := " << TACContainer[i].rhs << endl;
                break;

            case instr:  cout << "    " + TACContainer[i].lhs << " " + TACContainer[i].rhs << endl;
                break;

            case print:
            case call:   cout << "    " + TACContainer[i].lhs << " call " + TACContainer[i].rhs << endl;
                break;

            case branch: cout << "    if " + TACContainer[i].lhs << " goto " + TACContainer[i].rhs << endl;
                break;

            case jump:   cout << "    goto " + TACContainer[i].lhs << endl;
                break;

            default:     cout << " ERRRORRR !!!! " << endl;
        }
    }
}

void generateMIPS(vector<TACObject>& TACContainer) {
    unordered_map<string, string> registerMap;
    vector<string> rhs_tokens;

    for (auto &taco : TACContainer) {
        switch(taco.type) {
            case label:  cout << taco.lhs + ":" << endl;
                         break;

            case stmt:  
                        split(taco.rhs, " ", rhs_tokens);
                        if (rhs_tokens.size() == 1 && rhs_tokens[0][0] == 't') {
                            registerMap.insert( {taco.lhs, taco.rhs} );
                        } else {
                            auto rs = assignRegister(registerMap, rhs_tokens[0]);
                            auto rt = assignRegister(registerMap, rhs_tokens[2]);
                            auto op = rhs_tokens[1];
                            convertToMIPS(taco.lhs, rs, rt, op);
                        }
                        break;

//            case instr:  cout << "(DEBUG) sc_code : " << taco.sc_code << endl;
            case instr:  // cout << "    " + taco.lhs
                         // << " " + taco.rhs << endl;
                         break;

            case call:
                switch (taco.sc_code) {
                    case sc_ReadInt:
                        cout << "  li $v0, 5" << endl
                             << "  syscall"   << endl;
                        break;
                    default:
                        cout << "  (CALL ERROR) taco.sc_code: " << taco.sc_code << endl;
                        break;
                    case sc_None:break;
                }
            case print:  cout << "  li $v0, 1\n"
                              << "  move $a0, " + registerMap[taco.rhs] << "\n"
                              << "  syscall" 
                         << endl;
                         break;

            case branch: cout << "  bne $" + taco.lhs + ", $zero, " + taco.rhs 
                         << endl;
                         break;

            case jump:   cout << "  j " + taco.lhs << endl; 
                         break;

            default:     cout << "(TACO Type Error) type: " << taco.type << endl;
        }
    }

    // End of Program
    cout << "  li $v0, 10" << endl;
    cout << "  syscall" << endl;
}

string Program::Emit() {
    if ( decls->NumElements() > 0 ) {
        for ( int i = 0; i < decls->NumElements(); ++i ) {
            Decl *d = decls->Nth(i);
            d->Emit();
        }
    }

    //constantFolding(TACContainer);
    //constantPropagation(TACContainer);
    //deadCodeElimination(TACContainer);

//    generateIR(TACContainer);
    generateMIPS(TACContainer);
    return "Program::Emit()";
}

string StmtBlock::Emit() {
    for(int i = 0; i < stmts->NumElements(); i++){
        Stmt* ith_statement = stmts->Nth(i);
        ith_statement->Emit();
    }

    return "StmtBlock::Emit()";
}

string ForStmt::Emit() {
    string inc_var = init->Emit();

    string label0 = "L" + to_string(labelCounter++);
    string label1 = "L" + to_string(labelCounter++);
    string label2 = "L" + to_string(labelCounter++);

    TACContainer.emplace_back(label0, "", 0, label);
    TACContainer.emplace_back(test->Emit(), label1, 0, branch);
    TACContainer.emplace_back(label2, "", 0, jump);
    TACContainer.emplace_back(label1, "", 0, label);
    body->Emit();
    step->Emit();
    TACContainer.emplace_back(label0, "", 0, jump);
    TACContainer.emplace_back(label2, "", 0, label);
    return "ForStmt::Emit()";
}

string WhileStmt::Emit() {
    string label0 = "L" + to_string(labelCounter++);
    string label1 = "L" + to_string(labelCounter++);
    string label2 = "L" + to_string(labelCounter++);

    TACContainer.emplace_back(label0, "", 0, label);
    TACContainer.emplace_back(test->Emit(), label1, 0, branch);
    TACContainer.emplace_back(label2, "", 0, jump);
    TACContainer.emplace_back(label1, "", 0, label);
    body->Emit();
    TACContainer.emplace_back(label0, "", 0, jump);
    TACContainer.emplace_back(label2, "", 0, label);
    return "WhileStmt::Emit()";
}

string IfStmt::Emit() {
    string ifLabel = "L" + to_string(labelCounter++);
    string elseLabel = "L" + to_string(labelCounter++);

    TACContainer.emplace_back(test->Emit(), ifLabel, 0, branch);

    TACContainer.emplace_back(elseLabel, "", 0, jump);

    TACContainer.emplace_back(ifLabel, "", 0, label);

    body->Emit();

    string exitLabel = (elseBody) ? "L" + to_string(labelCounter++) : elseLabel;
    TACContainer.emplace_back(exitLabel, "", 0, jump);

    if (elseBody) {
        TACContainer.emplace_back(elseLabel, "", 0, label);
        elseBody->Emit();
        TACContainer.emplace_back(exitLabel, "", 0, jump);
    }

    TACContainer.emplace_back(exitLabel, "", 0, label);
    return "ifStmt";
}

string ReturnStmt::Emit() {
    string rhs = expr->Emit();
    TACContainer.emplace_back("Return", rhs, 0, instr);

    return "ReturnStmt Emit()";
}

string DeclStmt::Emit() {
    varDecl->Emit();
    return "DeclStmt Emit()";
}
