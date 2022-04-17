/*
 * Class:       CS 4308 Section 03
 * Term:        Spring 2022
 * Name:        Jake Dunkley
 * Instructor:  Sharon Perry
 * Project:     Complete Interpreter
 */

#pragma once

#include <vector>
#include "grammar.hpp"

namespace interpret {
    std::string functionSymbol;
    std::map<std::string, int> symbolTable;
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

    // Find the value of both child arguments.
    std::vector<int> vals;

    for (int childIndex = 0; childIndex < node -> children.size(); childIndex++) {
        switch (node -> children[1] -> value) {
            case grammar::terminalIntegerLiteral: {
                vals.push_back(std::stoi(node -> children[childIndex] -> linkedToken -> lexeme));
            }

            case grammar::terminalIdentifier: {
                int* val = lookupSymbol(node -> children[childIndex] -> linkedToken -> lexeme);

                if (val == nullptr) {
                    std::cout << "[Error] Arithmetic expression: Using variable that's not defined." << std::endl;
                    exit(-1);
                }

                else {
                    vals.push_back(*val);
                }
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