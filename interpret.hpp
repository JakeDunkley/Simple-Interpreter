/*
 * Term:        Spring 2022
 * Name:        Jake Dunkley
 * Project:     Complete Interpreter
 */

#pragma once

#include <vector>
#include "grammar.hpp"

namespace interpret {
    std::string functionSymbol;
    std::map<std::string, int> symbolTable;

    // Prints out the symbol table. Used for debugging segfaults -_-
    void showSymbolTable() {
        std::cout << "ID | Value" << std::endl;
        std::cout << "----------" << std::endl;
        std::cout << std::setw(2) << std::left << functionSymbol << " | fxnID" << std::endl;
        for (std::pair<std::string, int> i : symbolTable) {
            std::cout << std::setw(2) << std::left << i.first << " | ";
            std::cout << std::setw(5) << std::right << i.second << std::endl;
        }
        std::cout << std::endl;
    }
}

// Sees if a symbol has a logged value.
// Data races? Never heard of 'em...
int* lookupSymbol(const std::string& symbol) {

    // If the symbol is the function ID, throw an error.
    if (symbol == interpret::functionSymbol) {
        std::cout << "[Error] Identifier: Cannot use function identifier as a variable." << std::endl;
        exit(-1);
    }

    // If the symbol is valid and has an entry, return the entry.
    else if (interpret::symbolTable.find(symbol) != interpret::symbolTable.end()) {
        return &interpret::symbolTable.find(symbol) -> second;
    }

    // Otherwise, return a nullptr.
    else {
        return nullptr;
    }
}

// Evaluates an arithmetic expression node and returns the result as an integer.
int evalExpressionArithmetic(GrammarNode* node) {

    // Find the value of all child arguments.
    std::vector<int> vals;

    for (int childIndex = 1; childIndex < node -> children.size(); childIndex++) {
        switch (node -> children[childIndex] -> value) {

            case grammar::terminalIntegerLiteral: {
                vals.push_back(std::stoi(node -> children[childIndex] -> linkedToken -> lexeme));
                break;
            }

            case grammar::terminalIdentifier: {
                int* val = lookupSymbol(node -> children[childIndex] -> linkedToken -> lexeme);

                if (val == nullptr) {
                    std::cout << "[Error] Arithmetic expression: Using variable that is not defined." << std::endl;
                    exit(-1);
                }

                else {
                    vals.push_back(*val);
                }

                break;
            }

            case grammar::expressionArithmetic: {
                vals.push_back(evalExpressionArithmetic(node -> children[childIndex]));
                break;
            }

            default: {
                break;
            }
        }
    }

    // Do the actual math here.
    int result = vals[0];

    switch (node -> children[0] -> linkedToken ->  value) {
        case tokens::operatorAdd: {
            for (int i = 1; i < vals.size(); i++) {
                result += vals[i];
            }

            return result;
        }

        case tokens::operatorSub: {
            for (int i = 1; i < vals.size(); i++) {
                result -= vals[i];
            }

            return result;
        }

        case tokens::operatorMult: {
            for (int i = 1; i < vals.size(); i++) {
                result *= vals[i];
            }

            return result;
        }

        case tokens::operatorDiv: {
            for (int i = 1; i < vals.size(); i++) {
                result /= vals[i];
            }

            return result;
        }

        default: {
            break;
        }
    }

    return 0; // This is here to appease the compiler, this line should never be reached in theory.
}

// Evaluated a boolean expression node and returns the result.
bool evalExpressionBoolean(GrammarNode* node) {

    // Get value of things we're comparing.
    int vals[2];

    for (int i = 0; i < 2; i++) {

        // Case of literal value.
        if (node -> children[i + 1] -> value == grammar::terminalIntegerLiteral) {
            vals[i] = std::stoi(node -> children[i + 1] -> linkedToken -> lexeme);
        }

        // If it's not a literal value, find the variable's value.
        else {
            int* litVal = lookupSymbol(node -> children[i + 1] -> linkedToken -> lexeme);

            if (litVal == nullptr) {
                std::cout << "[Error] Boolean expression: Compared value has not been initialized.";
                exit(-1);
            }

            else {
                vals[i] = *litVal;
            }
        }
    }

    // Do actual comparison here.
    switch (node -> children[0] -> linkedToken -> value) {

        case tokens::operatorLessEq: {
            return vals[0] <= vals[1];
        }

        case tokens::operatorLess: {
            return vals[0] < vals[1];
        }

        case tokens::operatorGreatEq: {
            return vals[0] >= vals[1];
        }

        case tokens::operatorGreat: {
            return vals[0] > vals[1];
        }

        case tokens::operatorEqual: {
            return vals[0] == vals[1];
        }

        case tokens::operatorNotEqual: {
            return vals[0] != vals[1];
        }

        default: {
            return false; // This is here to appease the compiler.
        }
    }
}

// Assigns a value to a variable ID.
void evalAssignment(GrammarNode* node) {

    // First, compute the value of the RHS.
    int value;

    switch (node -> children[1] -> value) {

        case grammar::terminalIntegerLiteral: {
            value = std::stoi(node -> children[1] -> linkedToken -> lexeme);
            break;
        }

        case grammar::terminalIdentifier: {
            int* val = lookupSymbol(node -> children[1] -> linkedToken -> lexeme);

            if (val == nullptr) {
                std::cout << "[Error] Assignment: Using variable that's not defined." << std::endl;
                exit(-1);
            }

            else {
                value = *val;
            }

            break;
        }

        case grammar::expressionArithmetic: {
            value = evalExpressionArithmetic(node -> children[1]);
            break;
        }

        default: {
            break;
        }
    }

    // Now assign it to a variable by either create a new symbol entry or changing a current one.
    std::string symbolID = node -> children[0] -> linkedToken -> lexeme;

    // If the symbol doesn't exist, create a new entry.
    if (lookupSymbol(symbolID) == nullptr) {
        interpret::symbolTable.insert(std::pair<std::string, int>(symbolID, value));
    }

    // Otherwise, change the current entry value.
    else {
        interpret::symbolTable.at(symbolID) = value;
    }
}

// Evaluated a print statement and prints out the internal statement.
void evalStatementPrint(GrammarNode* node) {
    std::string toPrint;

    switch (node -> children[0] -> value) {

        case grammar::terminalIntegerLiteral: {
            toPrint = node -> children[0] -> linkedToken -> lexeme;
            break;
        }

        case grammar::terminalIdentifier: {
            toPrint = std::to_string(*lookupSymbol(node -> children[0] -> linkedToken -> lexeme));
            break;
        }

        case grammar::expressionArithmetic: {
            toPrint = std::to_string(evalExpressionArithmetic(node -> children[0]));
            break;
        }

        default: {
            break;
        }
    }

    std::cout << toPrint << std::endl;
}

// Recursive function that evaluates nodes and their children.
void evalNode(GrammarNode* root) {
    //interpret::showSymbolTable();

    switch (root -> value) {

        case grammar::assignment: {
            evalAssignment(root);
            break;
        }

        case grammar::statementPrint: {
            evalStatementPrint(root);
            break;
        }

        case grammar::statementIf: {
            bool cond = evalExpressionBoolean(root -> children[0]);

            if (cond) {
                for (int i = 1; i < root -> children.size() - 2; i++) {
                    evalNode(root -> children[i]);
                }
            }

            else {
                evalNode(root -> children[root -> children.size() - 2]);
            }

            break;
        }

        case grammar::statementWhile: {
            bool cond = evalExpressionBoolean(root -> children[0]);

            while (cond) {
                for (int i = 1; i < root -> children.size() - 1; i++) {
                    evalNode(root -> children[i]);
                }

                cond = evalExpressionBoolean(root -> children[0]);
            }

            break;
        }

        case grammar::statementRepeat: {
            bool cond;

            do {
                for (int i = 0; i < root -> children.size() - 1; i++) {
                    evalNode(root -> children[i]);
                }

                cond = !evalExpressionBoolean(root -> children[root -> children.size() - 1] -> children[0]);
            } while (cond);

            break;
        }

        case grammar::branchElse: {
            for (int i = 0; i < root -> children.size(); i++) {
                evalNode(root -> children[i]);
            }

            break;
        }

        default: {
            break;
        }
    }
}