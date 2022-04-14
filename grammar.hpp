/*
 * Class:       CS 4308 Section 03
 * Term:        Spring 2022
 * Name:        Jake Dunkley
 * Instructor:  Sharon Perry
 * Project:     Complete Interpreter
 */

#pragma once

#include <vector>
#include <queue>
#include "tokens.hpp"

namespace grammar {

    // Used for labeling grammar nodes to build a complete parse tree of the input program.
    typedef enum {
        program =                    -3,
        block =                      -2,
        statement =                  -1,
        functionHeader =              0,
        end =                         1,
        assignment =                  2,
        expressionArithmetic =        3,
        expressionBoolean =           4,
        statementPrint =              5,
        statementIf =                 6,
        branchElse =                  7,
        statementWhile =              8,
        statementRepeat =             9,
        branchUntil =                10,
        terminalIdentifier =         11,
        terminalIntegerLiteral =     12,
        terminalOperatorArithmetic = 13,
        terminalOperatorRelative =   14
    } GrammarValue;
}

std::string getGrammarNodeIdentifier(const grammar::GrammarValue& value) {
    switch(value) {
        case grammar::program:
            return "program";
        case grammar::block:
            return "block";
        case grammar::statement:
            return "stat";
        case grammar::functionHeader:
            return "funcHead";
        case grammar::end:
            return "end";
        case grammar::assignment:
            return "assignment";
        case grammar::expressionArithmetic:
            return "exprAr";
        case grammar::expressionBoolean:
            return "exprBool";
        case grammar::statementPrint:
            return "statPrint";
        case grammar::statementIf:
            return "statIf";
        case grammar::branchElse:
            return "branchElse";
        case grammar::statementWhile:
            return "statWhile";
        case grammar::statementRepeat:
            return "statRepeat";
        case grammar::branchUntil:
            return "branchUntil";
        case grammar::terminalIdentifier:
            return "termIdent";
        case grammar::terminalIntegerLiteral:
            return "termIntLit";
        case grammar::terminalOperatorArithmetic:
            return "termOpAr";
        case grammar::terminalOperatorRelative:
            return "termOpRel";
    }
}

struct GrammarNode {
    grammar::GrammarValue value;        // The value of the node.
    GrammarNode* parent;                // Points to a parent node if the node has one.
    std::vector<GrammarNode*> children; // Points to node's children if the node has any.
    bool isTerminal = false;            // Flags if the grammar node holds a terminal token.
    Token* linkedToken;                 // If a terminal node, create a link with the terminal token.

    // Constructor for non-terminal node
    explicit GrammarNode(const grammar::GrammarValue& value_) {
        value = value_;
        parent = nullptr;
        linkedToken = nullptr;
    }

    // Constructor for terminal node.
    GrammarNode(const grammar::GrammarValue& value_, Token* linkedToken_) {
        value = value_;
        parent = nullptr;
        isTerminal = true;
        linkedToken = linkedToken_;
    }

    // Adds child node.
    void addChild(const grammar::GrammarValue& value_) {
        GrammarNode* newChild = new GrammarNode(value_);
        newChild -> parent = this;
        children.push_back(newChild);
    }

    // Adds terminal child node.
    void addChildTerminal(const grammar::GrammarValue& value_, Token* linkedToken_) {
        GrammarNode* newChildTerminal = new GrammarNode(value_);
        newChildTerminal -> parent = this;
        newChildTerminal -> isTerminal = true;
        newChildTerminal -> linkedToken = linkedToken_;
        children.push_back(newChildTerminal);
    }

    // Add already created node as child.
    void addChildDirect(GrammarNode* child) {
        child -> parent = this;
        children.push_back(child);
    }

    // Prints the structure of the node tree and its members.
    void show(int depth = 0) {
        static const int spacing = 6;

        for (int i = 0; i < depth; i++) {
            std::cout << std::setw(spacing) << std::right << "|";
        }

        std::cout << "_";
        if (isTerminal) std::cout << linkedToken -> toString();
        else std::cout << getGrammarNodeIdentifier(value) << std::endl;

        if (!children.empty()) {
            for (int i = 0; i < children.size(); i++) {
                children[i] -> show(depth + 1);
                if (i < children.size() - 1) std::cout << std::endl;
            }
        }
    }

    // Delete node.
    ~GrammarNode() {
        if (!children.empty()) {
            for (GrammarNode* c : children) {
                delete c;
            }
        }

        delete linkedToken;
    }
};

// Functions to create nodes based on grammar definitions.
GrammarNode* createNodeFunctionHeader(std::queue<Token*>& tokens) {
    GrammarNode* nodeFunctionHeader = new GrammarNode(grammar::functionHeader);

    // If tokens has more than 4 tokens, throw an error.
    if (tokens.size() > 4) {
        throw std::runtime_error("[Error] Function header: Too many symbols.");
    }

    // Otherwise, build the node group.
    else {
        // The first token HAS to be function, as that's how we got here, so we don't need to check that.
        tokens.pop();

        // The next token needs to be an identifier, and it's the only one we hold on to.
        if (tokens.front() -> value != tokens::identifier) {
            throw std::runtime_error("[Error] Function header: No identifier.");
        }

        else {
            nodeFunctionHeader -> addChildTerminal(grammar::terminalIdentifier, tokens.front());
            tokens.pop();
        }

        // The next token needs to be a left parenthesis.
        if (tokens.front() -> value != tokens::operatorParenL) {
            throw std::runtime_error("[Error] Function header: No left parenthesis.");
        }

        tokens.pop();

        // The last token needs to be a right parenthesis.
        if (tokens.front() -> value != tokens::operatorParenR) {
            throw std::runtime_error("[Error] Function header: No right parenthesis.");
        }

        tokens.pop();
    }

    return nodeFunctionHeader;
}

// Recursive function used to construct arithmetic expression node trees.
GrammarNode* createNodeExpressionArithmetic(std::queue<Token*>& tokens) {

    GrammarNode* nodeExprAr = new GrammarNode(grammar::expressionArithmetic);

    // Loop until list of tokens associated with exprAr are used up.
    while (!tokens.empty()) {

        // Check if current token is an arithmetic operator, aka the valid start of an arith expr.
        if (isOperatorArithmetic(tokens.front() -> value)) {

            // If the exprArith node already has an operator attached, then another operator is a problem.
            if (!nodeExprAr -> children.empty()) {
                throw std::runtime_error("Error: Arithmetic expression: Too many operators.");
            }

            // Otherwise, add it to the node, and continue to the next token.
            else {
                nodeExprAr -> addChildTerminal(grammar::terminalOperatorArithmetic, tokens.front());
            }
        }

        // Check if current token is an identifier
        else if (tokens.front() -> value == tokens::identifier) {

            // If the current exprAr node does not have an operator child yet, this is a problem.
            if (nodeExprAr -> children.empty()) {
                throw std::runtime_error("Error: Arithmetic expression: Adding identifier with no operator.");
            }

            // Otherwise, add it do the node, and continue to the next token.
            else {
                nodeExprAr -> addChildTerminal(grammar::terminalIdentifier, tokens.front());
            }
        }

        // Check if current token is an integer literal.
        else if (tokens.front() -> value == tokens::integerLiteral) {

            // If the current exprAr node does not have an operator child yet, this is a problem.
            if (nodeExprAr -> children.empty()) {
                throw std::runtime_error("Error: Arithmetic expression: Adding literal with no operator.");
            }

            // Otherwise, add it do the node, and continue to the next token.
            else {
                nodeExprAr -> addChildTerminal(grammar::terminalIntegerLiteral, tokens.front());
            }
        }

        // Check if current token is a left parenthesis.
        // If this is the case, we make a recursive call and add the returned child to the root node.
        else if (tokens.front() -> value == tokens::operatorParenL) {
            tokens.pop();

            GrammarNode* nestedExprAr = createNodeExpressionArithmetic(tokens);
            nodeExprAr -> addChildDirect(nestedExprAr);
        }

        // Check if current token is a right parenthesis.
        // If this is the case, we end building the current node and check its validity.
        else if (tokens.front() -> value == tokens::operatorParenR) {

            // If the expression does not have the min # of args, this is a problem.
            if (nodeExprAr -> children.size() < 3) {
                throw std::runtime_error("Error: Arithmetic expression: Parenthesis mismatch.");
            }

            else {
                return nodeExprAr;
            }
        }

        // If any other type of token is found, this is a problem.
        else {
            throw std::runtime_error("Error: Arithmetic expression: Invalid token.");
        }

        tokens.pop();
    }

    // Once the queue has been emptied, check that the final node has the min # of args required.
    if (nodeExprAr -> children.size() < 3) {
        throw std::runtime_error("Error: Arithmetic expression: Not enough arguments.");
    }

    else return nodeExprAr;
}

// Function to construct boolean expression node tree.
// NOTE: This only accepts 3 arguments in this order: [opRel, id/literal, id/literal]
GrammarNode* createNodeExpressionBoolean(std::queue<Token*>& tokens) {
    GrammarNode* nodeExpressionBoolean = new GrammarNode(grammar::expressionBoolean);

    // If tokens has more than 3 tokens, throw an error.
    if (tokens.size() > 3) {
        throw std::runtime_error("[Error] Boolean expression: Too many arguments.");
    }

    // Otherwise, create the node group.
    else {
        // Try to add first token, which has to be a relative operator.
        if (!isOperatorRelative(tokens.front() -> value)) {
            throw std::runtime_error("[Error] Boolean expression: No relative operator.");
        }

        else {
            nodeExpressionBoolean -> addChildTerminal(grammar::terminalOperatorRelative, tokens.front());
            tokens.pop();
        }

        // Try to add second and third tokens, which have to be identifiers or literals.
        while (!tokens.empty()) {
            if (tokens.front() -> value == tokens::identifier) {
                nodeExpressionBoolean -> addChildTerminal(grammar::terminalIdentifier, tokens.front());
            }

            else if (tokens.front() -> value == tokens::integerLiteral) {
                nodeExpressionBoolean -> addChildTerminal(grammar::terminalIntegerLiteral, tokens.front());
            }

            else {
                throw std::runtime_error("[Error] Boolean expression: Incorrect arguments.");
            }

            tokens.pop();
        }
    }

    return nodeExpressionBoolean;
}

// Function to create a print statement node group.
GrammarNode* createNodeStatementPrint(std::queue<Token*>& tokens) {

    // if the exprAr is not contained in parens or started with print, throw an error as the grammar does not permit this.
    if (tokens.front() -> value != tokens::keywordPrint || tokens.back() -> value != tokens::operatorParenR) {
        throw std::runtime_error("Error: Print statement: Poorly formed print statement.");
    }

    // If we're good, remove the print token, the front and back parens & generate the exprAr.
    else {
        tokens.pop();
        if (tokens.front() -> value != tokens::operatorParenL) throw std::runtime_error("Error: Print statement: Poorly formed print statement.");
        else tokens.pop();

        std::queue<Token*> tokensSansParens;
        while (tokens.size() > 1) {
            tokensSansParens.push(tokens.front());
            tokens.pop();
        }

        tokens = tokensSansParens;
    }

    GrammarNode* nodePrintStatement = new GrammarNode(grammar::statementPrint);
    nodePrintStatement -> addChildDirect(createNodeExpressionArithmetic(tokens));

    return nodePrintStatement;
}