/*
 * Class:       CS 4308 Section 03
 * Term:        Spring 2022
 * Name:        Jake Dunkley
 * Instructor:  Sharon Perry
 * Project:     Deliverable P2 Parser
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stack>
#include <chrono>
#include "Token.hpp"
#include "Grammar.hpp"

using namespace std;

// Debug stuff
const bool debugFlag = true;

// Stream for incoming program file.
fstream testFile("../program.txt");

const string charsIgnore = "\n\t\r ";                       // Characters that should be ignored.
const string charsLetters = "abcdefghijklmnopqrstuvwxyz";   // Characters that are valid for building an identifier.
const string charsOperator = "=~<>*/-+()";                  // Characters that are valid for building an identifier.
const string charsLiteralInteger = "0123456789";            // Characters that are valid for building an integer literal.

// Checks if a register is occupied, and creates a new token if so.
bool checkRegister(string& reg, vector<Token*>& toks) {
    if (!reg.empty()) {
        toks.push_back(createNewToken(reg));
        reg.clear();
        return true;
    }

    return false;
}

// Checks if the letter register is occupied, and creates a new token if so.
// Has special rules for creating an identifier or a keyword.
// NOTE: this is a bool for debugging purposes. Could be changed but ehh.
bool checkRegisterLetters(string& reg, vector<Token*>& toks) {
    if (!reg.empty()) {
        if (reg.size() == 1) {
            toks.push_back(new Token(identifier, reg));
        }

        else {
            toks.push_back(createNewToken(reg));
        }

        reg.clear();
        return true;
    }

    return false;
}

// Method to push all words in a definition to the grammar stack.
void pushDefToStack(stack<string>* grammars, multimap<string, vector<string>>* dict) {
}

// Main.
int main() {

    // Data needed for multiple sections.
    map<string, int> symbolTable;
    auto debugTimerStart = chrono::steady_clock::now();
    auto debugTimerEnd = chrono::steady_clock::now();

    /* ---------------- Lexical Analysis Section ---------------- */

    // All data and variables needed for building tokens from file.
    vector<Token*> tokens;          // Holds finished token information for later use.
    char charCurrent;               // Holds value of current character being analyzed.
    string registerLetters;         // Holds char(s) of keyword or identifier being built.
    string registerOperator;        // Holds chars of operator being built.
    string registerLiteralInteger;  // Holds chars of integer being built.

    // This area collects all non-ignored characters into a vector for easier parsing later.
    if (testFile.is_open()) {
        cout << "Start of lexical analysis..." << endl;
        debugTimerStart = chrono::steady_clock::now();

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

            // Check if current character should be ignored.
            if (charsIgnore.find(charCurrent) != string::npos) {
                checkRegisterLetters(registerLetters, tokens);
                checkRegister(registerOperator, tokens);
                checkRegister(registerLiteralInteger, tokens);
            }

            // Check if current character is a letter.
            else if (charsLetters.find(charCurrent) != string::npos) {
                checkRegister(registerOperator, tokens);
                checkRegister(registerLiteralInteger, tokens);

                registerLetters.push_back(charCurrent);
            }

            // Check if current character is an operator.
            else if (charsOperator.find(charCurrent) != string::npos) {
                checkRegisterLetters(registerLetters, tokens);
                checkRegister(registerLiteralInteger, tokens);

                // Parens have to be identified immediately, otherwise nested parens could be read as one operator.
                if (charCurrent == '(' || charCurrent == ')') {
                    tokens.push_back(createNewToken(charCurrent));
                }

                else {
                    registerOperator.push_back(charCurrent);
                }
            }

            // Check if current character is an integer.
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
        checkRegisterLetters(registerLetters, tokens);
        checkRegister(registerOperator, tokens);
        checkRegister(registerLiteralInteger, tokens);
    }

    // If there's a problem opening the file, display a message.
    else {
        cout << "There was a problem opening the source file." << endl;
    }

    // Show final tokens.
    debugTimerEnd = chrono::steady_clock::now();
    auto debugTimerElapsed = chrono::duration<float, milli>(debugTimerEnd - debugTimerStart).count();
    cout << "Generated " << tokens.size() << " tokens in " << debugTimerElapsed << "ms" << endl;

    for (Token* t : tokens) {
        t -> show();
    }

    cout << "Lexical analysis done!\n" << endl;

    /* ---------------- Parsing Section ---------------- */

    cout << "Start of parsing..." << endl;

    // All the data and variables needed for generating a parse tree.
    stack<string> parseStack;
    GrammarNode parseTree("program");

    parseStack.push("end");

    // Find how many statements were generated and add statement nodes to the stack and
    // collect all identifiers into the symbol table.
    for (Token* t : tokens) {
        switch(t -> value) {
            case (operatorAssignment):
            case (keywordIf):
            case (keywordWhile):
            case (keywordRepeat):
            case (keywordPrint):
                parseStack.push("statement");
                cout << "Found statement token (" << t -> toString() << ")." << endl;
                break;
            case (identifier):
                symbolTable.insert(pair<string, int>(t -> lexeme, 0));
                break;
            default:
                break;
        }
    }

    parseStack.push("functionHeader");

    // Show final symbol table.
    cout << "Added " << symbolTable.size() << " symbol" << (symbolTable.size() > 1 ? "s " : " ") << "to the symbol table." << endl;

    for (pair<string, int> s : symbolTable) {
        cout << s.first << endl;
    }

    // Main loop to construct parse tree.
    while (!parseStack.empty()) {
        // idk man this is so freaking difficult.
    }

    return 0;
}
