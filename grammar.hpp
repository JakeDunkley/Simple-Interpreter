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

    void addChild(const grammar::GrammarValue& value_) {
        GrammarNode* newChild = new GrammarNode(value_);
        newChild -> parent = this;
        children.push_back(newChild);
    }

    void addChildTerminal(const grammar::GrammarValue& value_, Token* linkedToken_) {
        GrammarNode* newChildTerminal = new GrammarNode(value_);
        newChildTerminal -> parent = this;
        newChildTerminal -> isTerminal = true;
        newChildTerminal -> linkedToken = linkedToken_;
        children.push_back(newChildTerminal);
    }

    void addChildDirect(GrammarNode* child) {
        child -> parent = this;
        children.push_back(child);
    }

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
GrammarNode* createNodeFunctionHeader(Token* identToken) {
    GrammarNode* nodeFunctionHeader = new GrammarNode(grammar::functionHeader);
    nodeFunctionHeader -> addChildTerminal(grammar::terminalIdentifier, identToken);

    return nodeFunctionHeader;
}

// Recursive function used to construct arithmetic expression node trees.
GrammarNode* createNodeExpressionArithmetic(std::queue<Token*>& tokens) {

    GrammarNode* toReturn = new GrammarNode(grammar::expressionArithmetic);

    // Loop until list of tokens associated with exprAr are used up.
    while (!tokens.empty()) {

        // Check if current token is an arithmetic operator, aka the valid start of an arith expr.
        if (isOperatorArithmetic(tokens.front() -> value)) {

            // If the exprArith node already has an operator attached, then another operator is a problem.
            if (!toReturn -> children.empty()) {
                throw std::runtime_error("Error: Arithmetic expression: Too many operators.");
            }

            // Otherwise, add it to the node, and continue to the next token.
            else {
                toReturn -> addChildTerminal(grammar::terminalOperatorArithmetic, tokens.front());
            }
        }

        // Check if current token is an identifier
        else if (tokens.front() -> value == tokens::identifier) {

            // If the current exprAr node does not have an operator child yet, this is a problem.
            if (toReturn -> children.empty()) {
                throw std::runtime_error("Error: Arithmetic expression: Adding identifier with no operator.");
            }

            // Otherwise, add it do the node, and continue to the next token.
            else {
                toReturn -> addChildTerminal(grammar::terminalIdentifier, tokens.front());
            }
        }

        // Check if current token is an integer literal.
        else if (tokens.front() -> value == tokens::integerLiteral) {

            // If the current exprAr node does not have an operator child yet, this is a problem.
            if (toReturn -> children.empty()) {
                throw std::runtime_error("Error: Arithmetic expression: Adding literal with no operator.");
            }

            // Otherwise, add it do the node, and continue to the next token.
            else {
                toReturn -> addChildTerminal(grammar::terminalIntegerLiteral, tokens.front());
            }
        }

        // Check if current token is a left parenthesis.
        // If this is the case, we make a recursive call and add the returned child to the root node.
        else if (tokens.front() -> value == tokens::operatorParenL) {
            tokens.pop();

            GrammarNode* nestedExprAr = createNodeExpressionArithmetic(tokens);
            toReturn -> addChildDirect(nestedExprAr);
        }

        // Check if current token is a right parenthesis.
        // If this is the case, we end building the current node and check its validity.
        else if (tokens.front() -> value == tokens::operatorParenR) {

            // If the expression does not have the min # of args, this is a problem.
            if (toReturn -> children.size() < 3) {
                throw std::runtime_error("Error: Arithmetic expression: Parenthesis mismatch.");
            }

            else {
                return toReturn;
            }
        }

        // If any other type of token is found, this is a problem.
        else {
            throw std::runtime_error("Error: Arithmetic expression: Invalid token.");
        }

        tokens.pop();
    }

    // Once the queue has been emptied, check that the final node has the min # of args required.
    if (toReturn -> children.size() < 3) {
        throw std::runtime_error("Error: Arithmetic expression: Not enough arguments.");
    }

    else return toReturn;
}

// Function to create a print statement node group.
GrammarNode* createNodeStatementPrint(std::queue<Token*>& tokens) {
    // if the exprAr is not contained in parens, throw an error as the grammar does not permit this.
    if (tokens.front() -> value != tokens::operatorParenL || tokens.back() -> value != tokens::operatorParenR) {
        throw std::runtime_error("Error: Print statement: Poorly formed print statement.");
    }

    // If we're good, remove the front and back parens to generate the exprAr.
    else {
        tokens.pop();

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