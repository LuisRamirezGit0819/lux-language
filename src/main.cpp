#include <iostream>
#include <string>
#include "lexer.h"
#include "ast.h"

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
    //!── Prueba del LEXER ───────
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


    //! ── Prueba del AST: construcción manual ───────
std::cout << "------------------------------------------------------------" << std::endl;
std::cout << "Prueba: AST manual"                 << std::endl;
std::cout << "------------------------------------------------------------" << std::endl;

// Representamos: let resultado = 2 + 3;

// Paso 1: crear los nodos hoja
auto izq = std::make_unique<NumberLiteral>(2);
auto der = std::make_unique<NumberLiteral>(3);

// Paso 2: crear la expresión binaria con los hijos
auto suma = std::make_unique<BinaryExpression>(
    std::move(izq),   // transfiere propiedad de izq
    "+",
    std::move(der)    // transfiere propiedad de der
);

// Paso 3: crear el LetStatement con la expresión
auto letStmt = std::make_unique<LetStatement>(
    "resultado",
    std::move(suma)   // transfiere propiedad de suma
);

// Paso 4: crear el Program raíz y agregar la sentencia
auto program = std::make_unique<Program>();
program->statements.push_back(std::move(letStmt));

// Imprimir el AST completo
std::cout << program->toString() << std::endl;

    return 0;
}