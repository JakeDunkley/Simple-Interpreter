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

using namespace std;

// Used for labeling tokens that are later passed to the parser.
typedef enum {
    badSymbol          = -1,
    identifier         =  0,
    integerLiteral     =  1,
    operatorAssignment =  2,
    operatorLessEq     =  3,
    operatorLess       =  4,
    operatorGreatEq    =  5,
    operatorGreat      =  6,
    operatorEqual      =  7,
    operatorNotEqual   =  8,
    operatorAdd        =  9,
    operatorSub        = 10,
    operatorMult       = 11,
    operatorDiv        = 12,
    operatorParenL     = 13,
    operatorParenR     = 14,
    keywordFunction    = 15,
    keywordEnd         = 16,
    keywordIf          = 17,
    keywordThen        = 18,
    keywordElse        = 19,
    keywordWhile       = 20,
    keywordDo          = 21,
    keywordRepeat      = 22,
    keywordUntil       = 23,
    keywordPrint       = 24
} TokenValue;

// Defines link between operator lexemes and their token values.
const map<string, TokenValue> operators {
        {"=",  operatorAssignment},
        {"<=",  operatorLessEq},
        {"<",  operatorLess},
        {">=",  operatorGreatEq},
        {">",  operatorGreat},
        {"==", operatorEqual},
        {"~=", operatorNotEqual},
        {"+",  operatorAdd},
        {"-",  operatorSub},
        {"*",  operatorMult},
        {"/",  operatorDiv},
        {"(",  operatorParenL},
        {")",  operatorParenR}
};

// Defines link between keyword lexemes and their token values.
const map<string, TokenValue> keywords {
        {"function", keywordFunction},
        {"end",      keywordEnd},
        {"if",       keywordIf},
        {"then",     keywordThen},
        {"else",     keywordElse},
        {"while",    keywordWhile},
        {"do",       keywordDo},
        {"repeat",   keywordRepeat},
        {"until",    keywordUntil},
        {"print",    keywordPrint}
};

// Returns TokenValue enum identifier. Just for debugging.
string getTokenValueIdentifier(const TokenValue& v) {
    switch(v) {
        case badSymbol:
            return "badSymbol";
        case identifier:
            return "identifier";
        case integerLiteral:
            return "integerLiteral";
        case operatorAssignment:
            return "operatorAssignment";
        case operatorLessEq:
            return "operatorLessEq";
        case operatorLess:
            return "operatorLess";
        case operatorGreatEq:
            return "operatorGreatEq";
        case operatorGreat:
            return "operatorGreat";
        case operatorEqual:
            return "operatorEqual";
        case operatorNotEqual:
            return "operatorNotEqual";
        case operatorAdd:
            return "operatorAdd";
        case operatorSub:
            return "operatorSub";
        case operatorMult:
            return "operatorMult";
        case operatorDiv:
            return "operatorDiv";
        case operatorParenL:
            return "operatorParenL";
        case operatorParenR:
            return "operatorParenR";
        case keywordFunction:
            return "keywordFunction";
        case keywordEnd:
            return "keywordEnd";
        case keywordIf:
            return "keywordIf";
        case keywordThen:
            return "keywordThen";
        case keywordElse:
            return "keywordElse";
        case keywordWhile:
            return "keywordWhile";
        case keywordDo:
            return "keywordDo";
        case keywordRepeat:
            return "keywordRepeat";
        case keywordUntil:
            return "keywordUntil";
        case keywordPrint:
            return "keywordPrint";
        default:
            return "TOKEN_VALUE_NOT_FOUND";
    }
}

// Structure to hold token values.
struct Token {
    TokenValue value;
    string lexeme;

    Token(TokenValue value_, const string& lexeme_) {
        value = value_;
        lexeme = lexeme_;
    }

    void show() {
        cout << "[ VALUE: " << setw(20) << left << getTokenValueIdentifier(value) + ", ";
        cout << "LEXEME: " << setw(11) << left << "\"" + lexeme + "\"" << "]" << endl;
    }
};

// (Semi) wrapper method for stoi that handles exception catching.
int* stringToInteger(const string& str) {
    int* toReturn = nullptr;

    try {
        toReturn = new int(stoi(str));
        return toReturn;
    }

    catch (exception& e) {
        return toReturn;
    }
}

// Easy way to find if a token lexeme is valid.
TokenValue lookupToken(const string& lexeme) {
    if (operators.find(lexeme) != operators.end()) {
        return operators.find(lexeme) -> second;
    }

    else if (keywords.find(lexeme) != keywords.end()) {
        return keywords.find(lexeme) -> second;
    }

    else if (stringToInteger(lexeme) != nullptr) {
        return integerLiteral;
    }

    else return badSymbol;
}

// Creates a new token from a string lexeme.
Token* createNewToken(string& lexeme) {
    return new Token{lookupToken(lexeme), lexeme};
}

// Creates a new token from a char lexeme.
Token* createNewToken(char& lexeme) {
    string lexToString{string() + lexeme};
    return new Token{lookupToken(lexToString), lexToString};
}
