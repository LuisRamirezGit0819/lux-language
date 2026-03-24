#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <map>
#include "token.h"

class Lexer{
public:

    Lexer(std::string source);

    std::vector<Token> tokenize();

private:
    std::string source_;
    int         pos_;

// ── Métodos de navegación ──────────────────
    char currentChar();   // carácter en pos_ (o '\0' si llegamos al final)
    char peek();          // siguiente carácter sin avanzar (lookahead)
    char advance();       // devuelve currentChar() y avanza pos_

    // ── Métodos de salto ──────────────────────
    void skipWhitespace();

    // ── Métodos de lectura ────────────────────
    Token readIdentifierOrKeyword();
    Token readNumber();

    // ── Tabla de keywords ─────────────────────
    // static: es la misma para todos los Lexers, no cambia
    static std::map<std::string, TokenType> keywords_;

};

#endif