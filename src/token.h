#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {

    // Literales — valores escritos directamente en el código
    NUMBER,         // 42, 7, 100
    TRUE,           // true
    FALSE,          // false

    // Identificadores — nombres inventados por el programador
    IDENTIFIER,     // x, miVariable, suma

    // Keywords — palabras reservadas del lenguaje
    LET,            // let
    FN,             // fn
    IF,             // if
    ELSE,           // else
    RETURN,         // return

    // Operadores aritméticos
    PLUS,           // +
    MINUS,          // -
    ASTERISK,       // *
    SLASH,          // /

    // Operadores de comparación
    EQUAL_EQUAL,    // ==
    NOT_EQUAL,      // !=
    LESS,           // <
    GREATER,        // >

    // Operador de asignación (distinto de comparación)
    EQUAL,          // =

    // Operador lógico
    BANG,           // !

    // Delimitadores
    LPAREN,         // (
    RPAREN,         // )
    LBRACE,         // {
    RBRACE,         // }
    COMMA,          // ,
    SEMICOLON,      // ;

    // Especiales
    ILLEGAL,        // carácter no reconocido
    END_OF_FILE     // fin del código fuente

};

struct Token {
    TokenType   type;    
    std::string lexeme;
};

// ─────────────────────────────────────────────
// Convierte un TokenType a su nombre como string
// Útil para imprimir tokens durante el desarrollo
// ─────────────────────────────────────────────
inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::NUMBER:       return "NUMBER";
        case TokenType::TRUE:         return "TRUE";
        case TokenType::FALSE:        return "FALSE";
        case TokenType::IDENTIFIER:   return "IDENTIFIER";
        case TokenType::LET:          return "LET";
        case TokenType::FN:           return "FN";
        case TokenType::IF:           return "IF";
        case TokenType::ELSE:         return "ELSE";
        case TokenType::RETURN:       return "RETURN";
        case TokenType::PLUS:         return "PLUS";
        case TokenType::MINUS:        return "MINUS";
        case TokenType::ASTERISK:     return "ASTERISK";
        case TokenType::SLASH:        return "SLASH";
        case TokenType::EQUAL_EQUAL:  return "EQUAL_EQUAL";
        case TokenType::NOT_EQUAL:    return "NOT_EQUAL";
        case TokenType::LESS:         return "LESS";
        case TokenType::GREATER:      return "GREATER";
        case TokenType::EQUAL:        return "EQUAL";
        case TokenType::BANG:         return "BANG";
        case TokenType::LPAREN:       return "LPAREN";
        case TokenType::RPAREN:       return "RPAREN";
        case TokenType::LBRACE:       return "LBRACE";
        case TokenType::RBRACE:       return "RBRACE";
        case TokenType::COMMA:        return "COMMA";
        case TokenType::SEMICOLON:    return "SEMICOLON";
        case TokenType::ILLEGAL:      return "ILLEGAL";
        case TokenType::END_OF_FILE:  return "EOF";
        default:                      return "UNKNOWN";
    }
}

#endif