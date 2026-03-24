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

#endif