// Name:        Jake Dunkley
// Class:       CS 4308 - Concepts of Programming Languages
// Assignment:  Simple julia interpreter

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

// Used for labeling tokens that are later passed to the parser.
typedef enum {
    badSymbol           = -1,
    identifier          = 0,
    literalInteger      = 1,
    operatorAssignment  = 2,
    operatorLessEq      = 3,
    operatorLess        = 4,
    operatorGreatEq     = 5,
    operatorGreat       = 6,
    operatorEqual       = 7,
    operatorNotEqual    = 8,
    operatorAdd         = 9,
    operatorSub         = 10,
    operatorMult        = 11,
    operatorDiv         = 12
} TokenValue;

// Defines link between operator symbols and their token values.
map<string, TokenValue> operators {
        {"=", operatorAssignment},
        {"<=", operatorLessEq},
        {"<", operatorLess},
        {">=", operatorGreatEq},
        {">", operatorGreat},
        {"==", operatorEqual},
        {"!=", operatorNotEqual},
        {"+", operatorAdd},
        {"-", operatorSub},
        {"*", operatorMult},
        {"/", operatorDiv}
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

// Main.
int main() {

    // All characters that should be ignored.
    const string charsIgnore = "\n\t ";

    // Characters that are valid for building an integer literal.
    const string charsLiteralInteger = "0123456789";

    // Characters that are valid for building an identifier.
    const string charsIdentifier = "abcdefghijklmnopqrstuvwxyz";

    // Characters that are valid for building an identifier.
    const string charsOperator = "=!<>*/-+";

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

        // Loop for extracting characters out of program file.
        while (testFile.get(charCurrent)) {

            // Check if the current character should be ignored.
            if (charsIgnore.find(charCurrent) != string::npos) {
                continue;
            }

            // Check if the current character is (part of) an operator.
            if (charsOperator.find(charCurrent) != string::npos) {
                registerOperator.push_back(charCurrent);
                continue;
            }

            // If it is not an operator, check if the operator register needs to be emptied.
            else if (!registerOperator.empty()) {
                tokens.push_back(new Token(operators.find(registerOperator) -> second, registerOperator));
                registerOperator.clear();
            }

            // Check if the current character is a number.
            if (charsLiteralInteger.find(charCurrent) != string::npos) {
                registerLiteralInteger.push_back(charCurrent);
                continue;
            }

            // If it is not a number, check if the integer register needs to be emptied.
            else if (!registerLiteralInteger.empty()) {
                tokens.push_back(new Token(literalInteger, registerLiteralInteger));
                registerLiteralInteger.clear();
            }

            // Check if the current character is a letter (i.e. an identifier).
            if (charsIdentifier.find(charCurrent) != string::npos) {
                tokens.push_back(new Token(identifier, string() + charCurrent));
                continue;
            }

            // If the current character is not recognized, add a bad symbol token.
            tokens.push_back(new Token(badSymbol, string() + charCurrent));
        }

        // Check if any registers are occupied at the end of the file.
        if (!registerOperator.empty()) tokens.push_back(new Token(operators.find(registerOperator) -> second, registerOperator));
        if (!registerLiteralInteger.empty()) tokens.push_back(new Token(literalInteger, registerLiteralInteger));
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
