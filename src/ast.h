#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include "token.h"

//-----------------------------------------------------
//Forward Declarations
//-----------------------------------------------------
class Node;
class BlockStatement;
class Identifier;

//-----------------------------------------------------
//Alias de Tipos
//-----------------------------------------------------
using NodePtr = std::unique_ptr<Node>;
using NodeList = std::vector<NodePtr>;

//-----------------------------------------------------
//NODE - clase base abstracta
//Todo nodo AST hereda de esta clase
//-----------------------------------------------------
class Node {
public:
    //Funcion usada para debuging
    virtual std:: string toString() const = 0;

    virtual ~Node() {}
};

//-----------------------------------------------------
//IDENTIFIER
//-----------------------------------------------------
class Identifier : public Node {
public:
    std:: string name;

    explicit Identifier(std::string name) : name(std::move(name)) {}

    std:: string toString() const override{
        return name;
    }

};

//-----------------------------------------------------
//NUMBERLITERAL
//-----------------------------------------------------
class NumberLiteral : public Node {
public:
    int value;

    explicit NumberLiteral(int value) : value(value)  {}

    std::string toString() const override{
        return std::to_string(value);
    }
};

//-----------------------------------------------------
//BOOLENLITERAL
//-----------------------------------------------------
class BooleanLiteral : public Node {
public:
    bool value;

    explicit BooleanLiteral(bool value) : value(value) {}

    std::string toString() const override {
        return value ? "true" : "false";
    }
};

//-----------------------------------------------------
//PREFIXEXPRESSION
//-----------------------------------------------------
class PrefixExpression : public Node {
public:
    std::string op;
    NodePtr     right;

    PrefixExpression(std::string op, NodePtr right)
        : op(std::move(op)), right(std::move(right)) {}

    std::string toString() const override {
        return "(" + op + right->toString() + ")";
    }
};

//-----------------------------------------------------
//BINARYEXPRESION
//-----------------------------------------------------
class BinaryExpression : public Node {
public:
    NodePtr     left;
    std::string op;
    NodePtr     right;

    BinaryExpression(NodePtr left, std::string op, NodePtr right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
    
    std::string toString() const override {
        return "(" + left->toString() + " " + op + " " + right->toString() + ")";
    }
};

//-----------------------------------------------------
//BLOCKSTATEMENT
//-----------------------------------------------------
class BlockStatement : public Node {
public:
    NodeList statements;

    BlockStatement() {}

    std::string toString() const override{
        std::string result = "{ ";
        for (const auto& stmt : statements){
            result += stmt->toString() + "; ";
        }
        result += "}";
        return result;
    }
} ;

//-----------------------------------------------------
//LETSTATEMENT
//-----------------------------------------------------
class LetStatement : public Node {
public:
    std::string name;
    NodePtr    value;

    LetStatement(std::string name, NodePtr value)
        : name(std::move(name)), value(std::move(value)) {}

    std::string toString() const override {
        return "let " + name + " = " + value->toString() + ";";
    }
};

//-----------------------------------------------------
//  RETURNSTATEMENT
//-----------------------------------------------------
class ReturnStatement : public Node {
public:
    NodePtr value;

    explicit ReturnStatement(NodePtr value) : value(std::move(value)) {}

    std::string toString() const override {
        return "return " + value->toString() + ";";
    }
};

//-----------------------------------------------------
//EXPRESSIONSTATEMENT
//-----------------------------------------------------
class ExpressionStatement : public Node {
public:
    NodePtr expression;

    explicit ExpressionStatement(NodePtr expression)
        : expression(std::move(expression)) {}

    std::string toString() const override {
        return expression->toString() + ";";
    }
};

//-----------------------------------------------------
// IFEXPRESSION 
//-----------------------------------------------------
class IfExpression : public Node {
public:
    NodePtr         condition;
    std::unique_ptr<BlockStatement> consequence;
    std::unique_ptr<BlockStatement> alternative;

    IfExpression(NodePtr condition,
                        std::unique_ptr<BlockStatement> consequence,
                        std::unique_ptr<BlockStatement> alternative)
        : condition(std::move(condition))
        , consequence(std::move(consequence))
        , alternative(std::move(alternative)) {}

    std::string toString() const override {
        std::string result = "if " + condition->toString()
                                    + " " + consequence->toString();
        if (alternative != nullptr) {
            result += " else " + alternative->toString();
        }
        return result;
    }
};

//-----------------------------------------------------
//FUNCTIONLITERAL
//-----------------------------------------------------
class FunctionLiteral : public Node {
public:
    std::vector<std::string>      parameters;
    std::unique_ptr<BlockStatement> body;

    FunctionLiteral(std::vector<std::string> parameters,
                    std::unique_ptr<BlockStatement> body)
        : parameters(std::move(parameters)), body(std::move(body)) {}

    std::string toString() const override {
        std::string result = "fn(";
        for (int i = 0; i < (int)parameters.size(); i++) {
            result += parameters[i];
            if (i < (int)parameters.size() - 1) result += ", ";
        }
        result += ") " + body->toString();
        return result;
    }
};

//-----------------------------------------------------
//CALLEXPRESSION
//-----------------------------------------------------
class CallExpression : public Node {
public:
    NodePtr  function;   
    NodeList arguments;  

    CallExpression(NodePtr function, NodeList arguments)
        : function(std::move(function)), arguments(std::move(arguments)) {}

    std::string toString() const override {
        std::string result = function->toString() + "(";
        for (int i = 0; i < (int)arguments.size(); i++) {
            result += arguments[i]->toString();
            if (i < (int)arguments.size() - 1) result += ", ";
        }
        result += ")";
        return result;
    }
};

//-----------------------------------------------------
//PROGRAM
//-----------------------------------------------------
class Program : public Node {
public:
    NodeList statements;

    Program() {}

    std::string toString() const override {
        std::string result;
        for (const auto& stmt : statements) {
            result += stmt->toString() + "\n";
        }
        return result;
    }
};

#endif