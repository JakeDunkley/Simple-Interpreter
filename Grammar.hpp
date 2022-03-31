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

// This is used later to find all matches to a certain word.
typedef multimap<string, vector<string>>::const_iterator defIter;

// This is the definitions of all words in the grammer.
const multimap<string, vector<string>> definitions {
        {"functionHeader",       {"keywordFunction", "identifier", "operatorParenL", "operatorParenR"}},
        {"statement",            {"statementIf"}},
        {"statement",            {"statementWhile"}},
        {"statement",            {"statementAssignment"}},
        {"statement",            {"statementPrint"}},
        {"statement",            {"statementRepeat"}},
        {"statementIf",          {"keywordIf", "expressionBoolean", "keywordThen", "block", "keywordElse", "block", "keywordEnd"}},
        {"statementWhile",       {"keywordWhile", "expressionBoolean", "do", "block", "keywordEnd"}},
        {"statementAssignment",  {"identifier", "operatorAssignment", "expressionArithmetic"}},
        {"statementPrint",       {"keywordPrint", "operatorParenL", "expressionArithmetic", "operatorParenR"}},
        {"statementRepeat",      {"keywordRepeat", "block", "keywordUntil", "expressionBoolean"}},
        {"expressionBoolean",    {"operatorsRelative", "expressionArithmetic", "expressionArithmetic"}},
        {"expressionArithmetic", {"operatorParenL", "expressionArithmetic", "operatorParenR"}},
        {"expressionArithmetic", {"expressionArithmetic", "expressionArithmetic"}},
        {"expressionArithmetic", {"operatorsArithmetic", "integer", "integer"}},
        {"expressionArithmetic", {"integer"}},
        {"operatorsArithmetic",  {"operatorAdd"}},
        {"operatorsArithmetic",  {"operatorSub"}},
        {"operatorsArithmetic",  {"operatorMult"}},
        {"operatorsArithmetic",  {"operatorDiv"}},
        {"operatorsRelative",    {"operatorLessEq"}},
        {"operatorsRelative",    {"operatorLess"}},
        {"operatorsRelative",    {"operatorGreatEq"}},
        {"operatorsRelative",    {"operatorGreat"}},
        {"operatorsRelative",    {"operatorEqual"}},
        {"operatorsRelative",    {"operatorNotEqual"}},
        {"integer",              {"identifier"}},
        {"integer",              {"integerLiteral"}}
};

// Returns vector of all possible definitions for a word.
vector<vector<string>> findAllDefs(const string& word) {
    vector<vector<string>> defs;
    pair<defIter, defIter> matches = definitions.equal_range(word);

    for (auto curDef = matches.first; curDef != matches.second; curDef++) {
        defs.push_back(curDef -> second);
    }

    return defs;
}

// Structure used in construction of parse tree.
struct GrammarNode {
    string word;
    GrammarNode* parent;
    vector<GrammarNode*> children;

    explicit GrammarNode(const string& word_) {
        word = word_;
        parent = nullptr;
    }

    void linkParent(GrammarNode* parent_) {
        parent = parent_;
    }

    void addChild(GrammarNode* child) {
        children.push_back(child);
    }
};