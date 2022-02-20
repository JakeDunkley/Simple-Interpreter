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

    // Holds raw character data from file.
    vector<char> charsCollected;

    // Holds finished token information for later use.
    vector<Token*> tokens;

    // This area collects all non-ignored characters into a vector for easier parsing later.
    if (testFile.is_open()) {
        char charCurrent;   // Holds value of current character being analyzed.

        // Loop for extracting characters out of program file.
        while (testFile.get(charCurrent)) {
            bool isValidChar = true;

            // Check if current character should be ignored.
            for (char cI : charsIgnore) {
                if (cI == charCurrent) isValidChar = false;
            }

            // If good character, add to collection.
            if (isValidChar) charsCollected.push_back(charCurrent);
        }

        string registerLiteralInteger;  // Holds chars of integer being built.
        char registerOperator;          // Holds chars of operators being built.

        //TODO Loop for building tokens from characters.

        // Is the current char a number?
            // YES: Add it to the int register.
            // NO: Is the int register occupied?
                // YES: Build a new int literal token and add it to the token list.
                // NO: Continue down

        // Is the current char an operator?
            // YES: Is the operator register occupied?
                // YES: Build a new operator token based on combined characters.
                // NO: Set the operator register to the current character.
            // NO: Is the operator registered occupied?
                // YES: Build a new operator token based on register contents.
                // NO: Continue down.

        // Is the current char a letter?
            // YES: Build a new identifier token and add it to the token list.
            // NO: Continue down.

        for (char charActive : charsCollected) {
            if (charsLiteralInteger.find(charActive) != string::npos) {
                registerLiteralInteger.push_back(charActive);
            }

            else if (!registerLiteralInteger.empty()) {
                tokens.push_back(new Token(literalInteger, registerLiteralInteger));
                registerLiteralInteger.clear();
            }

            if (charsOperator.find(charActive) != string::npos) {
                if (registerOperator != '\0') {
                    string compoundOperator = string() + charCurrent + registerOperator;
                    tokens.push_back(new Token(operators.find(compoundOperator) -> second, compoundOperator));
                    registerOperator = '\0';
                }

                else {
                    registerOperator = charActive;
                }
            }

            else {
                string singleOperator = string() + registerOperator;
                tokens.push_back(new Token(operators.find(singleOperator) -> second, singleOperator));
                registerOperator = '\0';
            }

            if (charsIdentifier.find(charActive) != string::npos) {
                tokens.push_back(new Token(identifier, string(1, charActive)));
            }

            else {
                tokens.push_back(new Token(badSymbol, string(1, charActive)));
            }
        }
    }

    else {
        cout << "There was a problem opening the source file." << endl;
    }

    cout << "Collected characters: " << charsCollected.size() << endl;

    for (int i = 0; i < charsCollected.size(); i++) {
        cout << i << ": \"" << charsCollected[i] << "\"" << endl;
    }

    cout << endl;
    cout << "Minted tokens: " << tokens.size() << endl;

    for (Token* t : tokens) {
        t -> show();
    }

    return 0;
}
