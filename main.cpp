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
#include <queue>
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
    map<string, int> symbolTable; // Holds variable identifiers and their values.
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
                tokens.push_back(new Token(endOfLine, ""));
                continue;
            }

            // Check if current character should be ignored.
            if (charsIgnore.find(charCurrent) != string::npos) {
                checkRegisterLetters(registerLetters, tokens);
                checkRegister(registerOperator, tokens);
                checkRegister(registerLiteralInteger, tokens);
                if (charCurrent == '\n') tokens.push_back(new Token(endOfLine, ""));
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

            // Just to remove unnecessary tokens.
            // NOTE: If there were a situation where the program file had lots and lots of end of line
            // characters at the beginning, this while cause problems as the tokens are not actually
            // being deleted O_O
            while (!tokens.empty() && tokens[0] -> value == endOfLine) tokens.pop_back();

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
        if (t -> value == badSymbol) {
            cout << "Error: Bad symbol!" << endl;
            return 0;
        }
    }

    if (tokens[0] -> value != keywordFunction || tokens[1] -> value != identifier ||
        tokens[2] -> value != operatorParenL || tokens[3] -> value != operatorParenR) {
        cout << "Error: Bad function definition!" << endl;
        return 0;
    }

    cout << "Lexical analysis done!\n" << endl;

    /* ---------------- Parsing Section ---------------- */

    cout << "Start of parsing..." << endl;

    // Find how many statements were generated and add statement nodes to the stack and
    // collect all identifiers into the symbol table.
    for (int i = 0; i < tokens.size(); i++) {
        switch(tokens[i] -> value) {
            case (operatorAssignment):
            case (keywordIf):
            case (keywordWhile):
            case (keywordRepeat):
            case (keywordPrint):
                cout << "Found statement token (" << tokens[i] -> toString() << ")." << endl;
                break;
            case (identifier):
                if (i > 0 && tokens[i - 1] -> value != keywordFunction) {
                    symbolTable.insert(pair<string, int>(tokens[i] -> lexeme, 0));
                }
                break;
            default:
                break;
        }
    }

    // Show final symbol table.
    cout << "Added " << symbolTable.size() << " symbol" << (symbolTable.size() > 1 ? "s " : " ") << "to the symbol table." << endl;

    for (pair<string, int> s : symbolTable) {
        cout << s.first << endl;
    }

    // Step 0: Parse function definition out of rest of linesTokens.
    // Step 1: Parse all linesTokens.
        // a) Get each line (seperated by EOLs) and divide them into queues.
        // b) LR parse each queue and build a GrammarNode tree for each queue.
    // Step 2: Combine the GrammarNode trees of related linesTokens (i.e. if, else, end).
        // a) Find start of multi-line structures.
        // b) Identify linesTokens that should be below it.
        // c) Link the root of lower grammar trees to the root of the top tree.
    // Step 3: ??
    // Step 4: Profit

    /* ---------------- ---------------- ---------------- */

    // All the data and variables needed for generating a parse tree.
    GrammarNode parseTree("program");
    queue<queue<Token*>> linesTokens;
    queue<GrammarNode*> linesNodes;

    // Step 0: Parse function definition out of rest of linesTokens.
    // Step 1: Parse all linesTokens.
    // 1a) Get each line (seperated by EOLs) and divide them into queues.
    queue<Token*>* curLine = new queue<Token*>;

    for (Token* t : tokens) {
        if (t -> value == endOfLine) {
            linesTokens.push(*curLine);
            delete curLine;
            curLine = new queue<Token*>;
        }

        else {
            curLine -> push(t);
        }
    }

    linesTokens.push(*curLine);
    delete curLine;

    // 1b) LR parse each queue and build a GrammarNode tree for each queue.
     while (!linesTokens.empty()) {
         queue<Token*> curQueue = linesTokens.front();
         GrammarNode *curNode = nullptr;

         while (!curQueue.empty()) {
             switch (curQueue.front()->value) {
                 case identifier:
                     curNode = new GrammarNode("statementAssignment");
                     curNode -> addChild(new GrammarNode(curQueue.front()));
                     curQueue.pop();

                     if (curQueue.front() -> value == operatorAssignment) {
                         curQueue.pop();
                         curNode -> addChild(getNodeFromArithmetic(curQueue));
                     }

                     else {
                         cout << "Error: Bad assignment operation!" << endl;
                     }
                     break;

                 case keywordEnd:
                     if (curNode != nullptr) curNode -> addChild(new GrammarNode("end"));
                     break;

                 case keywordIf:
                     curNode = new GrammarNode("statementIf");
                     curNode -> addChild(new GrammarNode(curQueue.front()));
                     curQueue.pop();
                     curNode -> addChild(getNodeFromArithmetic(curQueue));
                     break;

                 case keywordElse:
                     curNode = new GrammarNode("statementElse");
                     curNode -> addChild(new GrammarNode(curQueue.front()));
                     curQueue.pop();
                     break;

                 case keywordWhile:
                     curNode = new GrammarNode("statementWhile");
                     curNode -> addChild(new GrammarNode(curQueue.front()));
                     curQueue.pop();
                     curNode -> addChild(getNodeFromArithmetic(curQueue));
                     break;

                 case keywordRepeat:
                     curNode = new GrammarNode("statementRepeat");
                     curNode -> addChild(new GrammarNode(curQueue.front()));
                     curQueue.pop();
                     break;

                 case keywordPrint:
                     curNode = new GrammarNode("statementPrint");
                     curNode -> addChild(new GrammarNode(curQueue.front()));
                     curQueue.pop();
                     curNode -> addChild(getNodeFromArithmetic(curQueue));
                     break;

                 default:
                     break;
             }
         }
    }
    // Step 2: Combine the GrammarNode trees of related linesTokens (i.e. if, else, end).
        // a) Find start of multi-line structures.
        // b) Identify linesTokens that should be below it.
        // c) Link the root of lower grammar trees to the root of the top tree.
    // Step 3: ??
    // Step 4: Profit

    return 0;
}
