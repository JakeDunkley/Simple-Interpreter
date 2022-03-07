/*
 * Class:       CS 4308 Section 03
 * Term:        Spring 2022
 * Name:        Jake Dunkley
 * Instructor:  Sharon Perry
 * Project:     Deliverable P1 Scanner
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

const bool debug = true;

// Used for labeling tokens that are later passed to the parser.
typedef enum {
    badSymbol           = -1,
    identifier          =  0,
    literalInteger      =  1,
    operatorAssignment  =  2,
    operatorLessEq      =  3,
    operatorLess        =  4,
    operatorGreatEq     =  5,
    operatorGreat       =  6,
    operatorEqual       =  7,
    operatorNotEqual    =  8,
    operatorAdd         =  9,
    operatorSub         = 10,
    operatorMult        = 11,
    operatorDiv         = 12,
    operatorParenL      = 13,
    operatorParenR      = 14,
    keywordFunction     = 15,
    keywordEnd          = 16,
    keywordIf           = 17,
    keywordThen         = 18,
    keywordElse         = 19,
    keywordWhile        = 20,
    keywordDo           = 21,
    keywordPrint        = 22
} TokenValue;

// Defines link between operator lexemes and their token values.
const map<string, TokenValue> operators {
        {"=", operatorAssignment},
        {"<=", operatorLessEq},
        {"<", operatorLess},
        {">=", operatorGreatEq},
        {">", operatorGreat},
        {"==", operatorEqual},
        {"~=", operatorNotEqual},
        {"+", operatorAdd},
        {"-", operatorSub},
        {"*", operatorMult},
        {"/", operatorDiv},
        {"(", operatorParenL},
        {")", operatorParenR}
};

// Defines link between keyword lexemes and their token values.
const map<string, TokenValue> keywords {
        {"function", keywordFunction},
        {"end", keywordEnd},
        {"if", keywordIf},
        {"then", keywordThen},
        {"else", keywordElse},
        {"while", keywordWhile},
        {"do", keywordDo},
        {"print", keywordPrint}
};

// Easy way to find if a token lexeme is valid.
TokenValue lookupToken(string& lexeme) {
    if (operators.find(lexeme) != operators.end()) {
        return operators.find(lexeme) -> second;
    }

    else if (keywords.find(lexeme) != keywords.end()) {
        return keywords.find(lexeme) -> second;
    }

    else return badSymbol;
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
        cout << "Value: " << value << "\t Lexeme: " << lexeme << endl;
    }
};

// Main.
int main() {

    // All characters that should be ignored.
    const string charsIgnore = "\n\t\r ";

    // Characters that are valid for building an integer literal.
    const string charsLiteralInteger = "0123456789";

    // Characters that are valid for building an identifier.
    const string charsLetters = "abcdefghijklmnopqrstuvwxyz";

    // Characters that are valid for building an identifier.
    const string charsOperator = "=~<>*/-+()";

    // Stream for incoming program file.
    // NOTE: macOS requires an absolute file path for some godforsaken reason.
    fstream testFile("/Users/jake/CLionProjects/Dunkley_CS4308_LexicalAnalyzer/program.txt");

    // Holds finished token information for later use.
    vector<Token*> tokens;

    // This area collects all non-ignored characters into a vector for easier parsing later.
    if (testFile.is_open()) {

        char charCurrent;               // Holds value of current character being analyzed.
        string registerLiteralInteger;  // Holds chars of integer being built.
        string registerOperator;        // Holds chars of operator being built.
        string registerLetters;         // Holds char(s) of keyword or identifier being built.

        // Loop for extracting characters out of program file.
        while (testFile.get(charCurrent)) {

            if (debug) cout << "Current char:\t" << charCurrent << endl;

            // Check if the current character means there's a comment/EOL and ignore the comment/rest of the line.
            if (registerOperator == "/" && charCurrent == '/') {

                if (debug) cout << charCurrent << ": Comment started" << endl;

                while (charCurrent != '\n') {
                    testFile.get(charCurrent);
                }

                registerOperator.clear();
                continue;
            }

            // Check if the current character is part of an integer literal.
            if (charsLiteralInteger.find(charCurrent) != string::npos) {
                if (debug) cout << charCurrent << ": is part of a number" << endl;
                registerLiteralInteger.push_back(charCurrent);
                continue;
            }

            // If it isn't, see if an integer literal token can be created.
            else if (!registerLiteralInteger.empty()) {
                if (debug) cout << charCurrent << ": Creating integer" << endl;
                tokens.push_back(new Token(literalInteger, registerLiteralInteger));
                registerLiteralInteger.clear();
            }

            // Check if the current character is part of an operator.
            if (charsOperator.find(charCurrent) != string::npos) {

                if (debug) cout << charCurrent << ": is an operator" << endl;

                // Check if the operator char is a paren.
                // NOTE: Yes this is a very meh workaround, but it's the best I could
                // come up with. There is probably a super elegant way to do this though...
                if (charCurrent == '(' || charCurrent == ')') {
                    if (debug) cout << charCurrent << ": is a paren" << endl;
                    string paren{string() + charCurrent};
                    tokens.push_back(new Token(operators.find(paren) -> second, paren));
                    registerOperator.clear();
                }

                // If not a paren, add to register.
                else {
                    if (debug) cout << charCurrent << ": is NOT a paren" << endl;
                    registerOperator.push_back(charCurrent);
                }

                continue;
            }

            // If it isn't, see if an operator token can be created.
            else if (!registerOperator.empty()) {
                if (debug) cout << charCurrent << ": Creating operator" << endl;
                tokens.push_back(new Token(lookupToken(registerOperator), registerOperator));
                registerOperator.clear();
            }

            // Check if the current character is part of a keyword or is an identifier.
            if (charsLetters.find(charCurrent) != string::npos) {
                if (debug) cout << charCurrent << ": is a letter" << endl;
                registerLetters.push_back(charCurrent);
                continue;
            }

            // If it isn't, see if a keyword token can be created.
            else if (!registerLetters.empty()) {

                if (debug) cout << charCurrent << ": Creating something" << endl;

                // If it's one letter (i.e. an identifier), create a new identifier token.
                if (registerLetters.size() == 1) {
                    if (debug) cout << charCurrent << ": Creating identifier" << endl;
                    tokens.push_back(new Token(identifier, string() + charCurrent));
                }

                // Otherwise, create a new keyword token.
                else {
                    if (debug) cout << charCurrent << ": Creating keyword" << endl;
                    tokens.push_back(new Token(lookupToken(registerLetters), registerLetters));
                }

                registerLetters.clear();
                continue;
            }

            // Check if the current character can be ignored.
            if (charsIgnore.find(charCurrent) != string::npos) {
                if (debug) cout << charCurrent << ": ignored" << endl;
                continue;
            }

            // Otherwise, create a bad symbol token.
            if (debug) cout << charCurrent << ": is a bad symbol" << endl;
            tokens.push_back(new Token(badSymbol, string() + charCurrent));

        }

        // After reading the whole file, see if any registers have data left in them.
        if (!registerLiteralInteger.empty()) {
            tokens.push_back(new Token(literalInteger, registerLiteralInteger));
            registerLiteralInteger.clear();
        }

        if (!registerOperator.empty()) {
            tokens.push_back(new Token(lookupToken(registerOperator), registerOperator));
            registerOperator.clear();
        }

        if (!registerLetters.empty()) {
            tokens.push_back(new Token(lookupToken(registerLetters), registerLetters));
            registerLetters.clear();
        }
    }

    else {
        cout << "There was a problem opening the source file." << endl;
    }

    cout << "Generated " << tokens.size() << " tokens: " << endl;

    for (Token* t : tokens) {
        t -> show();
    }

    return 0;
}
