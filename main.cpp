#include <iostream>
#include <fstream>

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

    // Open up streams for incoming program and output token file.
    ifstream fileIn;
    ofstream fileOut;

    fileIn.open("/Users/jake/CLionProjects/Dunkley_CS4308_LexicalAnalyzer/program.txt");
    fileOut.open("/Users/jake/CLionProjects/Dunkley_CS4308_LexicalAnalyzer/tokens.txt");

    //TODO tokenize.

    return 0;
}
