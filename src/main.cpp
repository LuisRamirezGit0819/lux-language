#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "lexer.h"
#include "ast.h"
#include "token.h"
#include "parser.h"

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

// Función auxiliar: parsea un string e imprime el AST
void probarParser(const std::string& descripcion, const std::string& codigo) {
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "Prueba: " << descripcion           << std::endl;
    std::cout << "Codigo: " << codigo                << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    Lexer lexer(codigo);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(std::move(tokens));
    std::unique_ptr<Program> program = parser.parse();

    // Mostrar errores si los hay
    if (!parser.errors.empty()) {
        std::cout << "  ERRORES:" << std::endl;
        for (const auto& err : parser.errors) {
            std::cout << "    - " << err << std::endl;
        }
    } else {
        std::cout << program->toString() << std::endl;
    }
}

int main(){
    //!── Prueba del LEXER ───────
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "Prueba: LEXER"                 << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

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

    //! ── Prueba del PARSER ───────
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "Prueba: PARSER"                 << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    // BLOQUE 1: NumberLiteral
    std::cout << "[ NumberLiteral ]" << std::endl;

    probarParser("entero simple",          "5;");
    probarParser("entero cero",            "0;");
    probarParser("entero grande",          "12345;");

    // BLOQUE 2: Identifier
    std::cout << "[ Identifier ]" << std::endl;

    probarParser("variable simple",        "x;");
    probarParser("variable con guion bajo","mi_variable;");
    probarParser("variable larga",         "resultado;");

    // BLOQUE 3: BooleanLiteral
    std::cout << "[ BooleanLiteral ]" << std::endl;

    probarParser("true",                   "true;");
    probarParser("false",                  "false;");

    // BLOQUE 4: LetStatement con literales
    std::cout << "[ LetStatement ]" << std::endl;

    probarParser("let con numero",         "let x = 42;");
    probarParser("let con booleano",       "let activo = true;");
    probarParser("let con variable",       "let y = x;");

    // BLOQUE 5: ReturnStatement con literales
    std::cout << "[ ReturnStatement ]" << std::endl;

    probarParser("return numero",          "return 7;");
    probarParser("return variable",        "return resultado;");
    probarParser("return booleano",        "return false;");

    // BLOQUE 6: Precedencia (verificación visual)
    std::cout << "[ Precedencia ]" << std::endl;

    probarParser("suma y producto",        "2 + 3 * 4;");
    probarParser("parentesis",             "(2 + 3) * 4;");
    probarParser("prefijo negativo",       "-x;");
    probarParser("prefijo bang",           "!activo;");

    return 0;
}