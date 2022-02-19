// Name:        Jake Dunkley
// Class:       CS 4308 - Concepts of Programming Languages
// Assignment:  Simple julia interpreter

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>

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

// Structure to hold token values.
struct Token {
    TokenValue value;
    int literalIntegerValue;
};

// Main.
int main() {
    const string charsIgnore = "\n\t ";                             // All characters that should be ignored.
    const string charsLiteralInteger = "0123456789";                // Characters that are valid for building an integer
                                                                    // literal.
    const string charsIdentifier = "abcdefghijklmnopqrstuvwxyz";    // Characters that are valid for building an
                                                                    // identifier.

    // Stream for incoming program file.
    //NOTE: macOS requires an absolute file path for some godforsaken reason.
    fstream testFile("/Users/jake/CLionProjects/Dunkley_CS4308_LexicalAnalyzer/program.txt");

    // Holds raw character data from file.
    vector<char> collectedChars;

    // Holds finished token information for later use.
    vector<Token> tokens;


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
            if (isValidChar) collectedChars.push_back(charCurrent);
        }

        queue<char> registerLiteralInteger;     // Holds chars of integer being built.
        queue<char> registerIdentifier;         // Holds chars of identifier being built.

        //TODO Loop for building tokens from characters.
    }

    else {
        cout << "There was a problem opening the source file." << endl;
    }

    for (int i = 0; i < collectedChars.size(); i++) {
        cout << i << ": \"" << collectedChars[i] << "\"" << endl;
    }

    return 0;
}
