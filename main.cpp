#include <iostream>
#include <fstream>
#include <vector>
#include <queue>

using namespace std;

typedef enum {
    errorBadSymbol      = -1,
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
    // All characters that should be ignored.
    const vector<char> ignoredChars = {'\n', ' ', '\t'};

    // Stream for incoming program file.
    fstream testFile("/Users/jake/CLionProjects/Dunkley_CS4308_LexicalAnalyzer/program.txt");

    // Holds raw character data from file.
    vector<char> collectedChars;

    // Holds finished token information for later use.
    vector<Token> tokens;


    if (testFile.is_open()) {
        char curChar;                       // Holds value of current character being analyzed.
        queue<char> literalIntegerRegister; // Holds values of integer being built.

        // Main loop
        while (testFile.get(curChar)) {
            bool isValidChar = true;

            for (char iC : ignoredChars) {
                if (iC == curChar) isValidChar = false;
            }

            if (isValidChar) collectedChars.push_back(curChar);
        }
    }

    else {
        cout << "There was a problem opening the source file." << endl;
    }

    for (int i = 0; i < collectedChars.size(); i++) {
        cout << i << ": \"" << collectedChars[i] << "\"" << endl;
    }

    //TODO tokenize.

    return 0;
}
