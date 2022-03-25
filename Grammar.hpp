/*
 * Class:       CS 4308 Section 03
 * Term:        Spring 2022
 * Name:        Jake Dunkley
 * Instructor:  Sharon Perry
 * Project:     Deliverable P2/P3 Interpreter
 */

#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <vector>
#include "Token.hpp"

using namespace std;

// Defines and holds the definitions of the language.
const multimap<string, vector<string>> words {
        {"program",               {"function", "id", "paren_l", "paren_r", "block", "end"}},
        {"block",                 {"statement"}},
        {"block",                 {"statement", "block"}},
        {"statement",             {"statement_if"}},
        {"statement",             {"statement_assignment"}},
        {"statement",             {"statement_while"}},
        {"statement",             {"statement_print"}},
        {"statement",             {"statement_repeat"}},
        {"statement_if",          {"if", "expression_boolean", "then", "block", "else", "block", "end"}},
        {"statement_while",       {"while", "expression_boolean", "do", "block", "end"}},
        {"statement_assignment",  {"id", "operator_assignment", "expression_arithmetic"}},
        {"statement_repeat",      {"repeat", "block", "until", "expression_boolean"}},
        {"statement_print",       {"print", "paren_l", "expression_arithmetic", "paren_r"}},
        {"expression_boolean",    {"operator_relative", "expression_arithmetic", "expression_arithmetic"}},
        {"operator_relative",     {"operator_lessEq"}},
        {"operator_relative",     {"operator_less"}},
        {"operator_relative",     {"operator_greatEq"}},
        {"operator_relative",     {"operator_great"}},
        {"operator_relative",     {"operator_Equal"}},
        {"operator_relative",     {"operator_notEqual"}},
        {"expression_arithmetic", {"integer"}},
        {"expression_arithmetic", {"integer", "expression_arithmetic"}},
        {"expression_arithmetic", {"operator_arithmetic", "expression_arithmetic"}},
        {"expression_arithmetic", {"paren_l", "expression_arithmetic", "paren_r"}},
        {"expression_arithmetic", {"operator_arithmetic", "integer"}},
        {"integer",               {"id"}},
        {"integer",               {"integer_literal"}},
        {"operator_arithmetic",   {"operator_add"}},
        {"operator_arithmetic",   {"operator_sub"}},
        {"operator_arithmetic",   {"operator_mult"}},
        {"operator_arithmetic",   {"operator_div"}}
};

// Links grammar definitions to token values if they're terminal symbols.
const map<string, TokenValue> terminalSymbols {
        {"id", identifier},
        {"integer_literal",     integerLiteral},
        {"operator_assignment", operatorAssignment},
        {"operator_lessEq",     operatorLessEq},
        {"operator_less",       operatorLess},
        {"operator_greatEq",    operatorGreatEq},
        {"operator_great",      operatorGreat},
        {"operator_equal",      operatorEqual},
        {"operator_notEqual",   operatorNotEqual},
        {"operator_add",        operatorAdd},
        {"operator_sub",        operatorSub},
        {"operator_mult",       operatorMult},
        {"operator_div",        operatorDiv},
        {"paren_l",             operatorParenL},
        {"paren_r",             operatorParenR},
        {"function",            keywordFunction},
        {"end",                 keywordEnd},
        {"if",                  keywordIf},
        {"then",                keywordThen},
        {"else",                keywordElse},
        {"while",               keywordWhile},
        {"do",                  keywordDo},
        {"repeat",              keywordRepeat},
        {"until",               keywordUntil},
        {"print",               keywordPrint}
};

// Returns the valid definition of the requested term/word.
vector<string> getDefinition(const string& term, vector<Token*>* toks) {
    vector<string> possibleDef = words.find(term) -> second;
}