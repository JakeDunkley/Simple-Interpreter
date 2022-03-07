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

const string charsIgnore = "\n\t\r ";                       // All characters that should be ignored.
const string charsLetters = "abcdefghijklmnopqrstuvwxyz";   // Characters that are valid for building an identifier.
const string charsOperator = "=~<>*/-+()";                  // Characters that are valid for building an identifier.
const string charsLiteralInteger = "0123456789";            // Characters that are valid for building an integer literal.

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

Token* createNewToken(string& lexeme) {
    return new Token{lookupToken(lexeme), lexeme};
}

Token* createNewToken(char& lexeme) {
    string lexToString{string() + lexeme};
    return new Token{lookupToken(lexToString), lexToString};
}

bool checkRegister(string& reg, vector<Token*>& toks) {
    if (!reg.empty()) {
        toks.push_back(createNewToken(reg));
        cout << reg << endl;
        reg.clear();
        return true;
    }

    return false;
}

bool checkRegisterLetters(string& reg, vector<Token*>& toks) {
    if (!reg.empty()) {
        if (reg.size() == 1) {
            toks.push_back(new Token(identifier, reg));
        }

        else {
            toks.push_back(createNewToken(reg));
        }

        cout << reg << endl;
        reg.clear();
        return true;
    }

    return false;
}

// All data and variables needed for parsing.

// Stream for incoming program file.
// NOTE: macOS requires an absolute file path for some godforsaken reason.
fstream testFile("/Users/jake/CLionProjects/Dunkley_CS4308_LexicalAnalyzer/program.txt");

vector<Token*> tokens;          // Holds finished token information for later use.
char charCurrent;               // Holds value of current character being analyzed.
string registerLetters;         // Holds char(s) of keyword or identifier being built.
string registerOperator;        // Holds chars of operator being built.
string registerLiteralInteger;  // Holds chars of integer being built.

// Main.
// Abandon hope, all ye who enter...
int main() {

    // This area collects all non-ignored characters into a vector for easier parsing later.
    if (testFile.is_open()) {

        // Loop for extracting characters out of program file.
        while (testFile.get(charCurrent)) {

            // Check if the current character means there's a comment/EOL and ignore the comment/rest of the line.
            if (registerOperator == "/" && charCurrent == '/') {

                while (charCurrent != '\n') {
                    testFile.get(charCurrent);
                }

                registerOperator.clear();
                continue;
            }

            if (charsIgnore.find(charCurrent) != string::npos) {
                checkRegisterLetters(registerLetters, tokens);
                checkRegister(registerOperator, tokens);
                checkRegister(registerLiteralInteger, tokens);
            }

            // Check if char is a letter.
            else if (charsLetters.find(charCurrent) != string::npos) {
                checkRegister(registerOperator, tokens);
                checkRegister(registerLiteralInteger, tokens);

                registerLetters.push_back(charCurrent);
            }

            // Check if char is an operator.
            else if (charsOperator.find(charCurrent) != string::npos) {
                checkRegisterLetters(registerLetters, tokens);
                checkRegister(registerLiteralInteger, tokens);

                if (charCurrent == '(' || charCurrent == ')') {
                    tokens.push_back(createNewToken(charCurrent));
                }

                else {
                    registerOperator.push_back(charCurrent);
                }
            }

            // Check if char is an integer.
            else if (charsLiteralInteger.find(charCurrent) != string::npos) {
                checkRegisterLetters(registerLetters, tokens);
                checkRegister(registerOperator, tokens);

                registerLiteralInteger.push_back(charCurrent);
            }

            // Otherwise, create a bad symbol token.
            else {
                string curToString{string() + charCurrent};
                tokens.push_back(new Token(badSymbol, curToString));
            }

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
