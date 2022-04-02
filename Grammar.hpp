/*
 * Class:       CS 4308 Section 03
 * Term:        Spring 2022
 * Name:        Jake Dunkley
 * Instructor:  Sharon Perry
 * Project:     Deliverable P2 Parser
 */

#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <vector>
#include "Token.hpp"

using namespace std;

// This is the definitions of all words in the grammar.
const map<vector<string>, string> definitions {
        {{"identifier"}, "number"},
        {{"integerLiteral"}, "number"},
        {{"number"}, "expressionArithmetic"},
        {{"(", "expressionArithmetic", ")"}, "expressionArithmetic"},
        {{"operatorAdd"}, "operatorArithmetic"},
        {{"operatorSub"}, "operatorArithmetic"},
        {{"operatorMult"}, "operatorArithmetic"},
        {{"operatorDiv"}, "operatorArithmetic"},
        {{"operatorArithmetic", "expressionArithmetic", "expressionArithmetic"}, "expressionArithmetic"},
        {{"operatorLessEq"}, "operatorRelative"},
        {{"operatorLess"}, "operatorRelative"},
        {{"operatorGreatEq"}, "operatorRelative"},
        {{"operatorGreat"}, "operatorRelative"},
        {{"operatorEqual"}, "operatorRelative"},
        {{"operatorNotEqual"}, "operatorRelative"},
        {{"operatorRelative", "expressionArithmetic", "expressionArithmetic"}, "expressionBoolean"},
        {{"identifier", "operatorAssignment", "expressionArithmetic"}, "statementAssignment"},
        {{"keywordPrint", "(", "expressionArithmetic", ")"}, "statementPrint"},
        {{"keywordIf", "expressionBoolean", "keywordThen"}, "statementIf"},
};

// Structure used in construction of parse tree.
struct GrammarNode {
    string word;
    GrammarNode* parent;
    vector<GrammarNode*> children;
    bool isTerminal;
    Token* tokenLink;

    explicit GrammarNode(const string& word_) {
        word = word_;
        parent = nullptr;
        isTerminal = false;
        tokenLink = nullptr;
    }

    explicit GrammarNode(Token* token) {
        word = getTokenValueIdentifier(token -> value);
        parent = nullptr;
        isTerminal = true;
        tokenLink = token;
    }

    void linkParent(GrammarNode* parent_) {
        parent = parent_;
    }

    void addChild(GrammarNode* child) {
        children.push_back(child);
        child -> parent = this;
    }
};

// Creates a grammar node tree from a queue of arithmetic-related tokens.
GrammarNode* getNodeFromArithmetic(queue<Token*>& arithTokens, string statType = "statementArithmetic") {
    GrammarNode* toReturn = new GrammarNode(statType);

    while (!arithTokens.empty()) {

        switch(arithTokens.front() -> value) {
            case operatorAdd:
            case operatorSub:
            case operatorMult:
            case operatorDiv: {
                GrammarNode* opArithNode = new GrammarNode("operatorArithmetic");
                opArithNode -> addChild(new GrammarNode(arithTokens.front()));
                toReturn -> addChild(opArithNode);
                break;
            }

            case identifier:
            case integerLiteral: {
                GrammarNode* numNode = new GrammarNode("number");
                numNode -> addChild(new GrammarNode(arithTokens.front()));
                toReturn -> addChild(numNode);
                break;
            }

            case operatorParenL: {
                arithTokens.pop();

                switch(arithTokens.front() -> value) {
                    case operatorLessEq:
                    case operatorLess:
                    case operatorGreatEq:
                    case operatorGreat:
                    case operatorEqual:
                    case operatorNotEqual: {
                        toReturn -> addChild(getNodeFromArithmetic(arithTokens, "statementBoolean"));
                    }

                    case operatorAdd:
                    case operatorSub:
                    case operatorMult:
                    case operatorDiv: {
                        toReturn -> addChild(getNodeFromArithmetic(arithTokens));
                    }

                    default:
                        break;
                }

                toReturn -> addChild(getNodeFromArithmetic(arithTokens));
                break;
            }

            case operatorParenR: {
                return toReturn;
            }

            case keywordThen:
                toReturn -> addChild(new GrammarNode("keywordThen"));
                break;

            case keywordDo:
                toReturn -> addChild(new GrammarNode("keywordDo"));
                break;

            case keywordUntil:
                toReturn -> addChild(new GrammarNode("keywordUntil"));
                break;

            default:
                break;

        }

        arithTokens.pop();
    }

    return toReturn;
}