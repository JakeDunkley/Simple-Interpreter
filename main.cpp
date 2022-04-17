/*
 * Class:       CS 4308 Section 03
 * Term:        Spring 2022
 * Name:        Jake Dunkley
 * Instructor:  Sharon Perry
 * Project:     Complete Interpreter
 */

// 0x2d466f7220446164

#include <iostream>
#include <fstream>
#include <vector>
#include "tokens.hpp"
#include "grammar.hpp"
#include "interpret.hpp"

using namespace std;

// Checks if a register is occupied, and creates a new token if so.
bool checkRegister(string& reg, vector<Token*>& tokens) {
    if (!reg.empty()) {
        tokens.push_back(createNewToken(reg));
        reg.clear();
        return true;
    }

    return false;
}

// Checks if the letter register is occupied, and creates a new token if so.
// Has special rules for creating an identifier or a keyword.
// NOTE: this is a bool for debugging purposes. Could be changed but ehh.
bool checkRegisterLetters(string& reg, vector<Token*>& tokens) {
    if (!reg.empty()) {

        if (reg.size() == 1) {
            tokens.push_back(new Token(tokens::identifier, reg));
        }

        else {
            tokens.push_back(createNewToken(reg));
        }

        reg.clear();
        return true;
    }

    return false;
}

int main() {

    // Stream for incoming program file.
    fstream testFile("../program.jl");

    /* ---------------- ---------------- Lexical Analysis Section ---------------- ---------------- */

    // All data and variables needed for building tokens from file.
    vector<Token*> tokens;          // Holds finished token information for later use.
    char charCurrent;               // Holds value of current character being analyzed.
    string registerLetters;         // Holds char(s) of keyword or identifier being built.
    string registerOperator;        // Holds chars of operator being built.
    string registerLiteralInteger;  // Holds chars of integer being built.

    // Variables needed for throwing error(s).
    string registerLine;            // Holds chars in current line.
    int lineIndex;                  // Keeps track of current index on current line.

    // This area collects all non-ignored characters into a vector for easier parsing later.
    if (testFile.is_open()) {
        cout << "Start of lexical analysis..." << endl;

        // Loop for extracting characters out of program file.
        while (testFile.get(charCurrent)) {

            // Check if the current character means there's a comment/EOL and ignore the comment/rest of the line.
            if (registerOperator == "/" && charCurrent == '/') {

                while (charCurrent != '\n') {
                    testFile.get(charCurrent);
                }

                registerOperator.clear();
                tokens.push_back(new Token(tokens::endOfLine, ""));
                continue;
            }

            // Check if current character should be ignored and
            // if end of line token should be added.
            if (tokens::charsIgnore.find(charCurrent) != string::npos) {
                checkRegisterLetters(registerLetters, tokens);
                checkRegister(registerOperator, tokens);
                checkRegister(registerLiteralInteger, tokens);

                // Add new end of line character only if there isn't a duplicate. Only one
                // is necessary to define where lines end, more just gunks up the parser.
                if (charCurrent == '\n' && tokens.back() -> value != tokens::endOfLine) {
                    tokens.push_back(new Token(tokens::endOfLine, ""));
                }
            }

            // Check if current character is a letter.
            else if (tokens::charsLetters.find(charCurrent) != string::npos) {
                checkRegister(registerOperator, tokens);
                checkRegister(registerLiteralInteger, tokens);

                registerLetters.push_back(charCurrent);
            }

            // Check if current character is an operator.
            else if (tokens::charsOperator.find(charCurrent) != string::npos) {
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
            else if (tokens::charsLiteralInteger.find(charCurrent) != string::npos) {
                checkRegisterLetters(registerLetters, tokens);
                checkRegister(registerOperator, tokens);

                registerLiteralInteger.push_back(charCurrent);
            }

            // Otherwise, create a bad symbol token.
            else {
                string curToString{string() + charCurrent};
                tokens.push_back(new Token(tokens::badSymbol, curToString));
            }

            // Just to remove unnecessary tokens that may add up on the top of the file.
            while (!tokens.empty() && tokens[0] -> value == tokens::endOfLine) {
                delete tokens.back();
                tokens.pop_back();
            }

        }

        // After reading the whole file, see if any registers have data left in them.
        checkRegisterLetters(registerLetters, tokens);
        checkRegister(registerOperator, tokens);
        checkRegister(registerLiteralInteger, tokens);
    }

    // If there's a problem opening the file, display a message.
    else {
        cout << "[Error] There was a problem opening the source file." << endl;
        return -1;
    }

    // Show final tokens.
    cout << "Generated " << tokens.size() << " tokens." << endl;

    for (Token* t : tokens) {
        t -> show();

        if (t -> value == tokens::badSymbol) {
            cout << "[Error] Bad symbol." << endl;
            return -1;
        }
    }

    cout << "Lexical analysis done!" << endl;

    /* ---------------- ---------------- Parser Section ---------------- ---------------- */

    // All data and variables needed for building nodes groups from tokens.
    queue<queue<Token*>> tokenLines; // Queues of tokens arranged by line.
    queue<GrammarNode*> nodes;       // Generated node groups are stored here.
    GrammarNode* parseTree = new GrammarNode(grammar::program);

    cout << endl;
    cout << "Start of parsing..." << endl;

    // Split up list of tokens by line and put them in separate queues for node building.
    tokenLines.push(queue<Token*>{});
    for (Token* t : tokens) {
        if (t -> value == tokens::endOfLine) {
            tokenLines.push(queue<Token*>{});
        }

        else {
            tokenLines.back().push(t);
        }
    }

    cout << "Creating node groups for each line..." << endl;

    // This loop generates node groups for each line.
    // These will be combined into whole statements next, and then
    // those will be combined into the complete program parse tree.
    while (!tokenLines.empty()) {
        switch(tokenLines.front().front() -> value) {

            case tokens::identifier: {
                nodes.push(createNodeStatementAssignment(tokenLines.front()));
                break;
            }

            case tokens::keywordFunction: {
                nodes.push(createNodeFunctionHeader(tokenLines.front()));
                break;
            }

            case tokens::keywordEnd: {
                nodes.push(createNodeEnd());
                break;
            }

            case tokens::keywordIf: {
                nodes.push(createNodeStatementIf(tokenLines.front()));
                break;
            }

            case tokens::keywordElse: {
                nodes.push(createNodeBranchElse());
                break;
            }

            case tokens::keywordWhile: {
                nodes.push(createNodeStatementWhile(tokenLines.front()));
                break;
            }

            case tokens::keywordRepeat: {
                nodes.push(createNodeStatementRepeat());
                break;
            }

            case tokens::keywordUntil: {
                nodes.push(createNodeStatementUntil(tokenLines.front()));
                break;
            }
            case tokens::keywordPrint: {
                nodes.push(createNodeStatementPrint(tokenLines.front()));
                break;
            }

            default:
                break;
        }

        tokenLines.pop();
        cout << "Parsed line " << nodes.size() << ": " << endl;

        // Show node groups as they are completed.
        nodes.back() -> show();
    }

    cout << "Done parsing lines!" << endl;
    cout << "Starting combination process..." << endl;

    // Next, build the full parse tree from the generated node groups.
    while (!nodes.empty()) {
        parseTree -> addChildDirect(createSuperNode(nodes));
    }

    cout << "Combination process done!" << endl;

    // Show completed parse tree.
    parseTree -> show();

    cout << "Parsing done!" << endl;

    /* ---------------- ---------------- Interpreter Section ---------------- ---------------- */

    cout << endl;
    cout << "Program Output:" << endl;

    interpret::functionSymbol = parseTree -> children[0] -> children[0] -> linkedToken -> lexeme;

    for (int i = 1; i < parseTree -> children.size() - 1; i++) {
        evalNode(parseTree -> children[i]);
    }

    return 0;
}