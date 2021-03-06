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
#include <utility>
#include <sstream>
#include <iterator>
#include <iomanip>

typedef pair<string, string> Trump;

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

            int a = split_result[0] == "" ? 0 : stoi(split_result[0]);
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

/**
 * Function assigns a temporary register to each variable found within the TAC
 * container.
 *
 * @param
 * @return 
 */
void linearScan(map<string, Trump>& regMap, vector<TACObject>& container) {
    vector<string> rhs_tokens;
    string curr_context;
    unordered_map<string, int> registerCount;

    for (auto &taco : container) {
        if (taco.type == label && taco.lhs[0] != 'L') {
            curr_context = taco.lhs;
            continue;
        }

        if (taco.type != stmt)
            continue; 

        split(taco.rhs, " ", rhs_tokens);

        if (rhs_tokens.size() != 1) {
            if (regMap.find(taco.lhs) != regMap.end())
                continue;

            if (taco.lhs[0] == 't') {
                regMap[taco.lhs] = make_pair(taco.lhs, curr_context);
                continue;
            }

            auto reg = "t" + to_string(Node::tempRegister[curr_context]);
            Node::tempRegister[curr_context]++; Node::stackRegister++;
            regMap[taco.lhs] = make_pair(reg, curr_context);
        } else {
            if (taco.rhs[0] == 't') {
                regMap[taco.lhs] = make_pair(taco.rhs, curr_context);
            } else {
                auto reg = "t" + to_string(Node::tempRegister[curr_context]);
                Node::tempRegister[curr_context]++; Node::stackRegister++;
                regMap[taco.lhs] = make_pair(reg, curr_context);
            }
        }
    }
}


string binaryExprToMIPS(string c, string a, string b, string op) {
    /*
     * The parameters are ordered based on the binary expression.
     * c := a op b 
     *
     *
     * The local variables rd, rs, rt follows the MIPS green sheet format.
     * For R-type instructions:
     *    rd - the destination register
     *    rs - a source register
     *    rt - a source register
     * The format for R-type usually looks like the following:
     *   (R-type instruction) $rd, $rs, $rt
     *
     * For I-type instructions:
     *   rt - the destination register
     *   rs - a source register
     *   imm - some immediate value
     * The formart for I-type usually looks like the following:
     *   (I-type instruction) $rt, $rs, imm
     *
     * NOTE: for I-type instructions I simply re-use the 'rt' as my immediate 
     * value
     */

    if (op == ">")
        return binaryExprToMIPS(c, b, a, "<");
    if (op == ">=")
        return binaryExprToMIPS(c, b, a, "<=");


    string mipsCode = "";
    string instr = "";
    string rs = "$" + a;
    string rt = "$" + b; 
    string rd = "$" + c;

    const string &context = Node::current_context;

    // Assume parameter 'b' not a constant. Use R-Type instruction.
    bool iType = false;

    // How to translate 100 < $t0 to MIPS? First use li to store 100 into
    // register $rs, then use slt $rd, $rs, $rt
    if (isNumeric(a) && (op == "<" || op == "<=")) {
        rs  = "$t" + to_string(Node::tempRegister[context]);
        Node::tempRegister[context]++; Node::stackRegister;

        mipsCode += "  li " + rs + ", " + a + "\n";
    }

    // How to translate a := 4 + a to MIPS? Change positions to a := a + 4, and
    // use addi.
    if (isNumeric(a) && (op != "<" && op != "<=")) {
        iType = true;
        rt = a;
        rs = "$" + b;
    }

    // How to translate $t0 < 140 to MIPS? Use I-type instruction.
    if (isNumeric(b)) {
        iType = true;
        rt = b;
    }

    if (op == "<") {
        instr = "  slt";
        instr += (iType) ? "i" : "";

        mipsCode += instr + " " + rd + ", " + rs + ", " + rt;
        return mipsCode;
    } 
    if (op == "<=") {

        mipsCode += binaryExprToMIPS(c, a, b, ">") + "\n";
        mipsCode += "  not "  + rd + ", " + rd + "\n";
        mipsCode += "  andi " + rd + ", " + rd + ", 1";
        return mipsCode;
    }
    if (op == "+") {
        instr = "  add";
        instr += (iType) ? "i" : "";

        mipsCode += instr + " " + rd + ", " + rs + ", " + rt;
        return mipsCode;
    } 

    return "ERROR operator (" + op + ") not supported!";
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

/**
 * Debug function to print out the detail of TACObject
 *
 * @param taco : the TACObject of interest
 */
void printTAC(const TACObject& taco) {
    map<tactype ,string> tactype_map {
            {label, "Label"}, {instr, "instr"}, {stmt, "stmt"},
            {call, "call"}, {print, "print"}, {branch, "branch"}, {jump, "jump"}, };

    cout << setw(20) << "(" <<  tactype_map[taco.type] << ")"
        << "\tlhs :  " << setw(5) << taco.lhs << setw(8)
        << "\trhs :  " << setw(5) << taco.rhs << setw(8)
        << "\tbytes: " << setw(5) << taco.bytes << endl;
}

bool does_trump_exists(string key, map<string, Trump>& regMap) {
    for (const auto &v : regMap) {
        if (v.first == key)
            return true;
    }

    return false;
}

void generateMIPS(vector<TACObject>& TACContainer, const bool& debug = false) {
    map<string, pair<string, string>> regMap;

    vector<string> rhs_tokens;
    //bool is_main = false;

    string stack_size = "";
    int pushparam_taken = 0;
    int registerNum = 0;

    linearScan(regMap, TACContainer);

    /** DEBUG **/
    Color::Modifier c_red(Color::Code::FG_RED);
    Color::Modifier c_green(Color::Code::FG_GREEN);
    Color::Modifier c_blue(Color::Code::FG_BLUE);
    Color::Modifier c_def(Color::Code::FG_DEFAULT);
    if (debug) {
        cout << c_blue << "(regMap content): " << endl;
        for (const auto& trump : regMap)
            cout << "---(dbg) " << setw(7) << trump.first << ":" << setw(7) << trump.second.first << endl;
        cout << c_def << endl; }
    /** END DEBUG **/

    cout << "  jal main" << endl;
    for (auto &taco : TACContainer) {

        /** DEBUG **/ if (debug) {
        cout << c_blue ;
        printTAC(taco);
        cout << c_def ; }
        /** END DEBUG **/

        switch(taco.type) {
            case label:  cout << taco.lhs + ":" << endl;
                         if (taco.lhs[0] != 'L') 
                             Node::current_context = taco.lhs;
                         break;
            case stmt:
                split(taco.rhs, " ", rhs_tokens);

                // Case 1) Variable is assigned a register.
                // Examples:  a := t1, b := t4, c := t0
                if (rhs_tokens.size() == 1 && rhs_tokens[0][0] == 't') {
                    regMap[taco.lhs].first = taco.rhs;
                } 
                // Case 2) Variable is assigned a constant.
                // Examples:  a := 2, b := 4, c := 8
                else if (rhs_tokens.size() == 1) {
                   // varConstToMIPS(regMap[taco.lhs], taco.rhs);
                    cout << "  li $" + regMap[taco.lhs].first + ", " + taco.rhs 
                         << endl;
                } 
                // Case 3) Variable is assigned to a binary expression.
                // Examples:  a := t3 + t1, b := t6 + t0
                else {
                    string a = rhs_tokens[0];
                    string b = rhs_tokens[2];

                    if (does_trump_exists(rhs_tokens[0], regMap))
                        a = regMap[rhs_tokens[0]].first;
                    if (does_trump_exists(rhs_tokens[2], regMap))
                        b = regMap[rhs_tokens[2]].first;

                    auto code = binaryExprToMIPS(regMap[taco.lhs].first, a, b, rhs_tokens[1]);

                    cout << code << endl;
                }

                break;

//            case instr:  cout << "(DEBUG) sc_code : " << taco.sc_code << endl;
            case instr:   
                if (taco.lhs == "BeginFunc") {
                    cout << "  addi $sp, $sp, -" + taco.rhs << endl;
                    stack_size = taco.rhs;
                } else if (taco.lhs == "Return") {
                    cout << "  move $v0, $" + regMap[taco.rhs].first << endl;
                    registerNum = 0;
                } else if (taco.lhs == "LoadParam") {
                    cout << "  lw $t" + to_string(registerNum)
                         << ", " << to_string(registerNum * 4) << "($sp)" 
                         << endl;
                    regMap[taco.rhs] = make_pair("t" + to_string(registerNum++), "");
                } else if (taco.lhs == "PushParam") {
                    cout << "  addi $sp, $sp, -4" << endl;
                    cout << "  sw $" + regMap[taco.rhs].first + ", 0($sp)" << endl;
                    pushparam_taken++;
                } else if (taco.lhs == "EndFunc") {
                    cout << "  addi $sp, $sp, " + stack_size << endl;
                    if (Node::current_context != "main")
                        cout << "  jr $ra" << endl;
                } else if (taco.lhs == "SaveRegisters") {
                    int count = 0;
                    cout << "  # save registers..." << endl;
                    for (const auto &r : regMap) {
                        Trump trump = r.second;
                        if (trump.second == "main") {
                            cout << "  sw $" + trump.first + ", " + to_string(count * 4) + "($sp)" << endl;
                            count++;
                        }
                    }

                } else if (taco.lhs == "RestoreRegisters") {
                    cout << "  addi $sp, $sp, " + to_string(4 * pushparam_taken) << endl;
                    cout << "  # restore registers..." << endl;
                    int count = 0;
                     for (const auto &r : regMap) {
                        Trump trump = r.second;
                        if (trump.second == "main") {
                            cout << "  sw $" + trump.first + ", " + to_string(count * 4) + "($sp)" << endl;
                            count++;
                        }
                    }
                }
                break;
                break;

            case call:
                switch (taco.sc_code) {
                    case sc_ReadInt:
                        cout << "  li $v0, 5"       << endl
                             << "  syscall"         << endl
                             << "  move $" << taco.lhs << ", $v0" << endl;
                        break;
                    case sc_None:
                        split(taco.rhs, " ", rhs_tokens);
                        cout << "  jal " + rhs_tokens[0] << endl;
                        cout << "  move $" + taco.lhs + ", $v0" << endl;
                        break;
                    default:
                        cout << "ERROR" << endl;
                        break;
                }
                break;
            case print: cout << "  li $v0, 1\n"
                              << "  move $a0, $" + regMap[taco.rhs].first << "\n"
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
    cout << "  # End Program" << endl;
    cout << "  li $v0, 10" << endl;
    cout << "  syscall" << endl;
//    for (const auto &asd : regMap)
//        cout << asd.first << " is mapped to " << asd.second.first << " in " << asd.second.second << endl;
}

string Program::Emit() {
    if ( decls->NumElements() > 0 )
        for ( int i = 0; i < decls->NumElements(); ++i )
            decls->Nth(i)->Emit();

    constantFolding(TACContainer);
    //constantPropagation(TACContainer);
    //deadCodeElimination(TACContainer);

    //generateIR(TACContainer);
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
