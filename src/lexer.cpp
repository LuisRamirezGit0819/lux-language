#include "lexer.h"
#include <cctype>

//------------------------------------------
// Tabla de keywords
//------------------------------------------
std::map<std::string, TokenType> Lexer::keywords_ = {
    {  "let",         TokenType::LET         },
    {  "fn",          TokenType::FN          },
    {  "if",           TokenType::IF            },
    {  "else",       TokenType::ELSE       },
    {  "return",   TokenType::RETURN },
    {  "true",       TokenType::TRUE     },
    {  "false",      TokenType::FALSE    }   
};

//------------------------------------------
//Constructor
//------------------------------------------
Lexer::Lexer(std::string source) : source_(source), pos_(0) {}

//------------------------------------------
//Metodos de Navegacion
//------------------------------------------

//Devuelve el caracter en la pocision actual
//SI pos_ esta fuera del string, devielve '\0' (señal de fin)
char Lexer::currentChar(){
    if (pos_ >= (int)source_.size()){
        return '\0';
    }
    return source_[pos_];
}

// Mira el SIGUIENTE carácter sin avanzar (lookahead de 1)
char Lexer::peek(){
    int nextPos = pos_ + 1;
    if (nextPos >= (int)source_.size()){
        return '\0';
    }
    return source_[nextPos];
}

// Guarda el carácter actual, avanza pos_, y devuelve el carácter guardado
char Lexer::advance(){
    char c = currentChar();
    pos_++;
    return c;
}

//------------------------------------------
//Metodos de Salto
//------------------------------------------

// Avanza pos_ mientras el carácter actual sea whitespace
// Los espacios, tabs y saltos de línea no producen tokens en Lux
void Lexer::skipWhitespace(){
    while(currentChar() != '\0' && isspace(currentChar())){
        advance();
    }
}

//------------------------------------------
//Metodos de Lectura
//------------------------------------------

// Lee un identificador o keyword completo
// Precondición: currentChar() es una letra o '_'
Token Lexer::readIdentifierOrKeyword(){

    int start = pos_;

    // Avanzamos mientras el carácter sea letra, dígito o guión bajo
    while (currentChar() != '\0' && (isalpha(currentChar()) || isdigit(currentChar()) || currentChar() == '_')){
        advance();
    }

    // Extraemos el texto acumulado desde start hasta pos_ actual
    std::string word = source_.substr(start, pos_ - start);

    //Buscamos en la tabla de keywords
    auto it = keywords_.find(word);

    if (it != keywords_.end()){
        return Token{ it->second, word};
    }

    return Token{ TokenType:: IDENTIFIER, word};

}

// Lee un número entero completo
Token Lexer::readNumber(){
    int start = pos_;

    while (currentChar() != '\0' && isdigit(currentChar())){
        advance();
    }

    std::string number = source_.substr(start, pos_ - start);
    return Token{ TokenType::NUMBER, number};
}

// ─────────────────────────────────────────────
// tokenize() — el método principal
// Recorre todo el código y produce la lista completa de tokens
// ─────────────────────────────────────────────
std::vector<Token> Lexer::tokenize(){

    std::vector<Token> tokens;

    while (currentChar() != '\0'){
    
        skipWhitespace();

        if (currentChar() == '\0'){
            break;
        }

        char c = currentChar();

        if (isalpha(c)){
            tokens.push_back(readIdentifierOrKeyword());
        }

        else if (isdigit(c)){
            tokens.push_back(readNumber());
        }

        else if (c == '='){
            advance();
            if (currentChar() == '='){
                advance();
                tokens.push_back(Token{ TokenType::EQUAL_EQUAL, "=="});
            } else {
                tokens.push_back(Token{ TokenType::EQUAL, "="});
            }
        }

        else if (c == '!'){
            advance();
            if (currentChar() == '='){
                tokens.push_back(Token{ TokenType::NOT_EQUAL, "!="});
            } else {
                tokens.push_back(Token{ TokenType::BANG, "!"});
            }
        }

        else if (c == '+') { advance(); tokens.push_back(Token{ TokenType::PLUS,      "+" }); }

        else if (c == '-') { advance(); tokens.push_back(Token{ TokenType::MINUS,     "-" }); }

        else if (c == '*') { advance(); tokens.push_back(Token{ TokenType::ASTERISK,  "*" }); }

        else if (c == '/') { advance(); tokens.push_back(Token{ TokenType::SLASH,     "/" }); }

        else if (c == '<') { advance(); tokens.push_back(Token{ TokenType::LESS,      "<" }); }

        else if (c == '>') { advance(); tokens.push_back(Token{ TokenType::GREATER,   ">" }); }

        else if (c == '(') { advance(); tokens.push_back(Token{ TokenType::LPAREN,    "(" }); }

        else if (c == ')') { advance(); tokens.push_back(Token{ TokenType::RPAREN,    ")" }); }

        else if (c == '{') { advance(); tokens.push_back(Token{ TokenType::LBRACE,    "{" }); }

        else if (c == '}') { advance(); tokens.push_back(Token{ TokenType::RBRACE,    "}" }); }

        else if (c == ',') { advance(); tokens.push_back(Token{ TokenType::COMMA,     "," }); }

        else if (c == ';') { advance(); tokens.push_back(Token{ TokenType::SEMICOLON, ";" }); }

        //Caracter no reconocido
        else {
            std::string illegal(1, c);
            tokens.push_back(Token{ TokenType::ILLEGAL, illegal});
            advance();
        }

    }

    tokens.push_back(Token{ TokenType::END_OF_FILE, ""});

    return tokens;

}