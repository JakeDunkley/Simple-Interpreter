/*
 * Class:       CS 4308 Section 03
 * Term:        Spring 2022
 * Name:        Jake Dunkley
 * Instructor:  Sharon Perry
 * Project:     Complete Interpreter
 */

#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <map>

namespace tokens {
    const std::string charsIgnore = "\n\t\r ";                       // Characters that should be ignored.
    const std::string charsLetters = "abcdefghijklmnopqrstuvwxyz";   // Characters that are valid for building an identifier.
    const std::string charsOperator = "=~<>*/-+()";                  // Characters that are valid for building an identifier.
    const std::string charsLiteralInteger = "0123456789";            // Characters that are valid for building an integer literal.

    // Used for labeling tokens that are later passed to the parser.
    typedef enum {
        endOfLine          = -2,
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
    const std::map<std::string, TokenValue> operators {
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
    const std::map<std::string, TokenValue> keywords {
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
}

// Used by parser to check symbol type.
bool isOperatorArithmetic(tokens::TokenValue value) {
    return (value >= 9 && value <= 12);
}

// Used by parser to check symbol type.
bool isOperatorRelative(tokens::TokenValue value) {
    return (value >= 3 && value <= 8);
}

// Returns TokenValue enum identifier. Just for debugging.
std::string getTokenValueIdentifier(const tokens::TokenValue& v) {
    switch(v) {
        case tokens::endOfLine:
            return "endOfLine";
        case tokens::badSymbol:
            return "badSymbol";
        case tokens::identifier:
            return "identifier";
        case tokens::integerLiteral:
            return "integerLiteral";
        case tokens::operatorAssignment:
            return "operatorAssignment";
        case tokens::operatorLessEq:
            return "operatorLessEq";
        case tokens::operatorLess:
            return "operatorLess";
        case tokens::operatorGreatEq:
            return "operatorGreatEq";
        case tokens::operatorGreat:
            return "operatorGreat";
        case tokens::operatorEqual:
            return "operatorEqual";
        case tokens::operatorNotEqual:
            return "operatorNotEqual";
        case tokens::operatorAdd:
            return "operatorAdd";
        case tokens::operatorSub:
            return "operatorSub";
        case tokens::operatorMult:
            return "operatorMult";
        case tokens::operatorDiv:
            return "operatorDiv";
        case tokens::operatorParenL:
            return "operatorParenL";
        case tokens::operatorParenR:
            return "operatorParenR";
        case tokens::keywordFunction:
            return "keywordFunction";
        case tokens::keywordEnd:
            return "keywordEnd";
        case tokens::keywordIf:
            return "keywordIf";
        case tokens::keywordThen:
            return "keywordThen";
        case tokens::keywordElse:
            return "keywordElse";
        case tokens::keywordWhile:
            return "keywordWhile";
        case tokens::keywordDo:
            return "keywordDo";
        case tokens::keywordRepeat:
            return "keywordRepeat";
        case tokens::keywordUntil:
            return "keywordUntil";
        case tokens::keywordPrint:
            return "keywordPrint";
        default:
            return "TOKEN_VALUE_NOT_FOUND";
    }
}

// Structure to hold token values.
struct Token {
    tokens::TokenValue value;   // Holds the type of token.
    std::string lexeme;              // Holds the actual contents of the token.

    Token(tokens::TokenValue value_, const std::string& lexeme_) {
        value = value_;
        lexeme = lexeme_;
    }

    void show() {
        std::cout << "[ VALUE: " << std::setw(20) << std::left << getTokenValueIdentifier(value) + ", ";
        std::cout << "LEXEME: " << std::setw(11) << std::left << "\"" + lexeme + "\"" << "]" << std::endl;
    }

    std::string toString() {
        return (getTokenValueIdentifier(value) + " \"" + lexeme + "\"");
    }
};

// (Semi) wrapper method for stoi that handles exception catching.
int* stringToInteger(const std::string& str) {
    int* toReturn = nullptr;

    try {
        toReturn = new int(stoi(str));
        return toReturn;
    }

    catch (std::exception& e) {
        return toReturn;
    }
}

// Easy way to find if a token lexeme is valid.
tokens::TokenValue lookupToken(const std::string& lexeme) {
    if (tokens::operators.find(lexeme) != tokens::operators.end()) {
        return tokens::operators.find(lexeme) -> second;
    }

    else if (tokens::keywords.find(lexeme) != tokens::keywords.end()) {
        return tokens::keywords.find(lexeme) -> second;
    }

    else if (stringToInteger(lexeme) != nullptr) {
        return tokens::integerLiteral;
    }

    else return tokens::badSymbol;
}

// Creates a new token from a string lexeme.
Token* createNewToken(std::string& lexeme) {
    return new Token{lookupToken(lexeme), lexeme};
}

// Creates a new token from a char lexeme.
Token* createNewToken(char& lexeme) {
    std::string lexToString{std::string() + lexeme};
    return new Token{lookupToken(lexToString), lexToString};
}
