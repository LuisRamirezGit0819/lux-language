#include "parser.h"
#include <stdexcept>

//---------------------------------------------------
//Tabla de precedencias
//---------------------------------------------------
std::map<TokenType, Precedence> Parser::precedences_ = {
    { TokenType::EQUAL_EQUAL,       Precedence::EQUALS      },
    { TokenType::NOT_EQUAL,           Precedence::EQUALS      },
    { TokenType::LESS,                       Precedence::LESSGREATER },
    { TokenType::GREATER,                Precedence::LESSGREATER },
    { TokenType::LESS_EQUAL,           Precedence::LESSGREATER },
    { TokenType::GREATER_EQUAL,    Precedence::LESSGREATER },
    { TokenType::PLUS,                       Precedence::SUM         },
    { TokenType::MINUS,                    Precedence::SUM         },
    { TokenType::ASTERISK,                Precedence::PRODUCT     },
    { TokenType::SLASH,                     Precedence::PRODUCT     },
    { TokenType::LPAREN,                   Precedence::CALL        },
};

//---------------------------------------------------
//Constructor
//---------------------------------------------------
Parser::Parser(std::vector<Token> tokens)
    : tokens_(std::move(tokens)), pos_(0) {}

//---------------------------------------------------
//Navegacion
//---------------------------------------------------
Token& Parser::currentToken() {
    if (pos_ >= (int)tokens_.size()) return tokens_.back();
    return tokens_[pos_];
}

Token& Parser::peekToken() {
    int next = pos_ + 1;
    if (next >= (int)tokens_.size()) return tokens_.back();
    return tokens_[next];
}

void Parser::advance() {
    if (pos_ < (int)tokens_.size() - 1) pos_++;
}

bool Parser::expect(TokenType t) {
    if (peekToken().type == t) {
        advance();
        return true;
    }
    std::string msg = "esperaba '" + tokenTypeToString(t)
                            + "' pero encontre '"
                            + tokenTypeToString(peekToken().type)
                            + "' (\"" + peekToken().lexeme + "\")";
    errors.push_back(msg);
    return false;
}

//---------------------------------------------------
//Precedencias
//---------------------------------------------------
Precedence Parser::currentPrecedence() {
    auto it = precedences_.find(currentToken().type);
    if (it != precedences_.end()) return it->second;
    return Precedence::LOWEST;
}

Precedence Parser::peekPrecendence() {
    auto it = precedences_.find(peekToken().type);
    if (it != precedences_.end()) return it->second;
    return Precedence::LOWEST;
}

//---------------------------------------------------
//parse() - punto de entrada
//---------------------------------------------------
std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();

    while (currentToken().type != TokenType::END_OF_FILE) {
        NodePtr stmt = parseStatement();
        if (stmt != nullptr) {
            program->statements.push_back(std::move(stmt));
        }
        advance();
    }
    return program;
}

//---------------------------------------------------
//parseStatement() - decide que tipo de sentencia parsear
//---------------------------------------------------
NodePtr Parser::parseStatement() {
    switch (currentToken().type) {
        case TokenType::LET:    return parseLetStatement();
        case TokenType::RETURN:      return parseReturnStatement();
        default:                    return parseExpressStatement();
    }
}

//---------------------------------------------------
//parseLetStatement()
//---------------------------------------------------
NodePtr Parser::parseLetStatement() {
    if (!expect(TokenType::IDENTIFIER)) return nullptr;

    std::string name = currentToken().lexeme;

    if (!expect(TokenType::EQUAL)) return nullptr;

    advance();

    NodePtr value = parseExpression(Precedence::LOWEST);

    if (peekToken().type == TokenType::SEMICOLON) advance();

    return std::make_unique<LetStatement>(std::move(name), std::move(value));
}

//---------------------------------------------------
//ParseReturnStatement()
//---------------------------------------------------
NodePtr Parser::parseReturnStatement() {
    advance();

    NodePtr value= parseExpression(Precedence::LOWEST);

    if (peekToken().type == TokenType::SEMICOLON) advance();
    
    return std::make_unique<ReturnStatement>(std::move(value));
}

//---------------------------------------------------
//parseExpressionStatement()
//---------------------------------------------------
NodePtr Parser::parseExpressStatement() {
    NodePtr expr = parseExpression(Precedence::LOWEST);

    if (peekToken().type == TokenType::SEMICOLON) advance();

    return std::make_unique<ExpressionStatement>(std::move(expr));
}

//---------------------------------------------------
//parseExpression
//---------------------------------------------------
NodePtr Parser::parseExpression(Precedence minima) {
    NodePtr left = nullptr;

    switch (currentToken().type) {
        case TokenType::NUMBER:
            left = parseNumberLiteral();
            break;
        case TokenType::IDENTIFIER:
            left = parseIdentifier();
            break;
        case TokenType::TRUE:
        case TokenType::FALSE:
            left = parseBooleanLiteral();
            break;
        case TokenType::MINUS:
        case TokenType::BANG:
            left = parsePrefixExpression();
            break;
        case TokenType::LPAREN:
            left = parseGroupedExpression();
            break;
        case TokenType::IF:
            left = parseIfExpression();
            break;
        case TokenType::FN:
            left = parseFuctionLiteral();
            break;
        default:
            errors.push_back(
                "no se como parsear '" + currentToken().lexeme + "' como expresion"
            );
            return nullptr;
    }

    while (peekToken().type != TokenType::END_OF_FILE && peekPrecendence() > minima) {
        advance();
        left = parseInfixExpression(std::move(left));
    }

    return left;
}

//---------------------------------------------------
//nud: parsear el inicio de una expresion
//---------------------------------------------------
NodePtr Parser::parseIdentifier() {
    return std::make_unique<Identifier>(currentToken().lexeme);
}

NodePtr Parser::parseNumberLiteral() {
    int value = std::stoi(currentToken().lexeme);
    return std::make_unique<NumberLiteral>(value);
}

NodePtr Parser::parseBooleanLiteral() {
    bool value = (currentToken().type ==TokenType::TRUE);
    return std::make_unique<BooleanLiteral>(value);
}

NodePtr Parser::parsePrefixExpression() {
    std::string op = currentToken().lexeme;
    advance();
    NodePtr right = parseExpression(Precedence::PREFIX);
    return std::make_unique<PrefixExpression>(std::move(op), std::move(right));
}

NodePtr Parser::parseGroupedExpression() {
    advance();
    NodePtr expr = parseExpression(Precedence::LOWEST);
    if (!expect(TokenType::RPAREN)) return nullptr;
    return expr;
}

NodePtr Parser::parseIfExpression() {
    if (!expect(TokenType::LPAREN)) return nullptr;

    advance();

    NodePtr condition = parseExpression(Precedence::LOWEST);
    if (condition == nullptr) return nullptr;

    if (!expect(TokenType::RPAREN)) return nullptr;

    if (!expect(TokenType::LBRACE)) return nullptr;

    auto consequence = parseBlockStatement();

    std::unique_ptr<BlockStatement> alternative = nullptr;

    if (peekToken().type == TokenType::ELSE) {
        advance();

        advance();

        if (currentToken().type != TokenType::LBRACE) {
            errors.push_back("esperaba '{' despues de 'else'");
            return nullptr;
        }
        alternative = parseBlockStatement();
    }
    return std::make_unique<IfExpression>(
        std::move(condition),
        std::move(consequence),
        std::move(alternative)
    );
}

std::vector<std::string> Parser::parseFuctionParameters() {
    std::vector<std::string> params;

    if (peekToken().type == TokenType::RPAREN) {
        advance();
        return params;
    }

    advance();

    params.push_back(currentToken().lexeme);

    while (peekToken().type == TokenType::COMMA ) {
        advance();
        advance();
        params.push_back(currentToken().lexeme);
    }

    if (!expect(TokenType::RPAREN)) return {};

    return params;
}

NodePtr Parser::parseFuctionLiteral() {
    if (!expect(TokenType::LPAREN)) return nullptr;

    std::vector<std::string> params = parseFuctionParameters();

    if (!expect(TokenType::LBRACE)) return nullptr;

    auto body = parseBlockStatement();

    return std::make_unique<FunctionLiteral>(
        std::move(params), std::move(body)
    );
}

//---------------------------------------------------
//led: parsear un operador infix
//---------------------------------------------------
NodePtr Parser::parseInfixExpression(NodePtr left) {
    if (currentToken().type == TokenType::LPAREN) {
        return parseCallExpression(std::move(left));
    }

    std::string op          = currentToken().lexeme;
    Precedence precedence = currentPrecedence();

    advance();

    NodePtr right = parseExpression(precedence);

    return std::make_unique<BinaryExpression>(std::move(left), std::move(op), std::move(right));
}

NodePtr Parser::parseCallExpression(NodePtr function) {
    NodeList args;

    if (peekToken().type == TokenType::RPAREN) {
        advance();
        return std::make_unique<CallExpression>(
            std::move(function), std::move(args)
        );
    }

    advance();
    args.push_back(parseExpression(Precedence::LOWEST));

    while (peekToken().type == TokenType::COMMA) {
        advance();
        advance();
        args.push_back(parseExpression(Precedence::LOWEST));
    }

    if (!expect(TokenType::RPAREN)) return nullptr;

    return std::make_unique<CallExpression>(
        std::move(function), std::move(args)
    );
}

//---------------------------------------------------
//parseBlockStatement()
//---------------------------------------------------
std::unique_ptr<BlockStatement> Parser::parseBlockStatement() {
    auto block = std::make_unique<BlockStatement>();

    advance();

    while (currentToken().type != TokenType::RBRACE && currentToken().type != TokenType::END_OF_FILE) {
        NodePtr stmt = parseStatement();
        if (stmt != nullptr) {
            block -> statements.push_back(std::move(stmt));
        }
        advance();
    }
    return block;
}