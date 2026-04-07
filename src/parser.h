# ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <map>
#include <string>
#include <memory>
#include "token.h"
#include "ast.h"

//------------------------------------------------
//Nivelesz de precedencia del Pratt Parser
//------------------------------------------------
enum class Precedence {
    LOWEST      = 0,
    EQUALS      = 1,   // == !=
    LESSGREATER = 2,   // < > <= >=
    SUM         = 3,   // + -
    PRODUCT     = 4,   // * /
    PREFIX      = 5,   // -x  !x
    CALL        = 6    // f(x)
};

//------------------------------------------------
//Parser: convierte lista de tokens en AST
//------------------------------------------------
class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    std::unique_ptr<Program> parse();

    std::vector<std::string> errors;

private:
    std::vector<Token> tokens_;
    int                             pos_;

    //-----Navegacion-----------------------
    Token& currentToken();
    Token& peekToken();
    void        advance();
    bool        expect(TokenType t);

    //-----Precedencias-----------------------
    static std::map<TokenType, Precedence> precedences_;
    Precedence currentPrecedence();
    Precedence peekPrecendence();

    //-----Parseo de sentencias-----------------------
    NodePtr parseStatement();
    NodePtr parseLetStatement();
    NodePtr parseReturnStatement();
    NodePtr parseExpressStatement();

    std::unique_ptr<BlockStatement> parseBlockStatement();

    //-----Parse de expresiones (Pratt)-----------------------
    NodePtr parseExpression(Precedence minima);
    NodePtr parseGroupedExpression();

    //-----nud: tokens que inician una expresion-----------------------
    NodePtr parseIdentifier();
    NodePtr parseNumberLiteral();
    NodePtr parseBooleanLiteral();
    NodePtr parsePrefixExpression();
    NodePtr parseIfExpression();
    NodePtr parseFuctionLiteral();
    // Helper: lee la lista de nombres de parámetros entre paréntesi
    std::vector<std::string> parseFuctionParameters();

    //-----led: tokens que continuan una expresion-----------------------
    NodePtr parseInfixExpression(NodePtr left);
    NodePtr parseCallExpression(NodePtr function);
};

#endif