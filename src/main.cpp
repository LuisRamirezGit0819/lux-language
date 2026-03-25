#include <iostream>
#include <string>
#include "lexer.h"

// Función auxiliar: tokeniza un string e imprime todos los tokens
void probarLexer(const std::string& descripcion, const std::string& codigo) {
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "Prueba: " << descripcion           << std::endl;
    std::cout << "Codigo: " << codigo                << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    Lexer lexer(codigo);
    std::vector<Token> tokens = lexer.tokenize();

    for (int i = 0; i < (int)tokens.size(); i++) {
        std::string tipo   = tokenTypeToString(tokens[i].type);
        std::string lexema = tokens[i].lexeme;

        while (tipo.size() < 15) tipo += " ";

        std::cout << "  [" << i << "] "
                    << tipo
                    << " | \""  << lexema << "\""
                    << std::endl;
    }
    std::cout << std::endl;
}

int main(){
    // ── Prueba 1: asignación simple ───────────
    probarLexer(
        "asignacion simple",
        "let x = 42;"
    );

    // ── Prueba 2: operadores aritméticos ──────
    probarLexer(
        "operadores aritmeticos",
        "2 + 3 * 4 - 1 / 2"
    );

    // ── Prueba 3: comparaciones y lógica ─────
    probarLexer(
        "comparaciones",
        "x == 5; y != 10; !true"
    );

    // ── Prueba 4: función completa ────────────
    probarLexer(
        "funcion completa",
        "let suma = fn(a, b) { return a + b; };"
    );

    // ── Prueba 5: if/else ─────────────────────
    probarLexer(
        "if else",
        "if (x < 10) { return true; } else { return false; }"
    );

    // ── Prueba 6: carácter ilegal ─────────────
    probarLexer(
        "caracter ilegal",
        "let x = @;"
    );

    // ── Prueba 7: operadores de comparación extendidos ──
    probarLexer(
        "comparaciones con <= y >=",
        "x <= 10; y >= 3; a < b; c > d"
    );

    return 0;
}