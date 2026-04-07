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

     // ── Módulo 5 — paso 5.6: IfExpression ───
    std::cout << std::endl;
    std::cout << "--------------MODULO 5 - Paso 5.6: IfExpression---------------------" << std::endl;

    // Caso 1: if sin else
    probarParser("if sin else",
                    "if (x > 0) { return x; }");

    // Caso 2: if con else
    probarParser("if con else",
                    "if (x > 0) { return x; } else { return 0; }");

    // Caso 3: if como expresión asignada a let
    // Demuestra que if produce un valor
    probarParser("if como valor en let",
                    "let resultado = if (x > 0) { x } else { 0 };");

    // Caso 4: condición booleana literal
    probarParser("if con condicion booleana",
                    "if (true) { return 1; } else { return 0; }");

    // Caso 5: if anidado — if dentro del bloque de otro if
    probarParser("if anidado",
                    "if (x > 0) { if (x < 10) { return x; } }");

    // ── Módulo 5 — paso 5.8: FunctionLiteral ──
    std::cout << std::endl;
    std::cout << "MODULO 5 ---- Paso 5.8: FunctionLiteral" << std::endl;

    // Caso 1: función sin parámetros
    probarParser("fn sin parametros",
                    "fn() { return 42; }");

    // Caso 2: función con un parámetro
    probarParser("fn con un parametro",
                 "fn(x) { return x * 2; }");

    // Caso 3: función con dos parámetros
    probarParser("fn con dos parametros",
                    "fn(a, b) { return a + b; }");

    // Caso 4: función asignada a variable con let
    probarParser("fn asignada a let",
                    "let sumar = fn(a, b) { return a + b; };");

    // Caso 5: función con cuerpo de varias sentencias
    probarParser("fn con cuerpo compuesto",
                 "fn(x) { let doble = x * 2; return doble + 1; }");

    // Caso 6: función que recibe otra función como parámetro
    probarParser("fn que recibe fn como parametro",
                    "let aplicar = fn(f, x) { return f(x); };");
    
    // Caso 7: FunctionLiteral llamado directamente (IIFE)
    probarParser("fn literal llamado directamente",
                 "fn(x) { return x * 2; }(5);");

    // Caso 8: llamada encadenada — el resultado de una llamada se llama
    probarParser("llamadas encadenadas",
                    "obtenerFn()(10);");

    // ── Módulo 5 — paso 5.10: Prueba integradora
    std::cout << std::endl;
    std::cout << "MODULO 5 --- Paso 5.10: Prueba integradora del Parser"
                << std::endl;

    // ── Programa 1: valor absoluto ─────────────
    // Función con if/else — verifica interacción
    // entre FunctionLiteral, IfExpression y ReturnStatement
    probarParser(
        "Programa 1: valor absoluto",
        "let absoluto = fn(x) {"
        "    if (x < 0) { return -x; } else { return x; }"
        "};"
    );

    // ── Programa 2: máximo de dos números ──────
    // Función con if/else + llamada con argumentos literales
    // Verifica que CallExpression funciona con el resultado
    // de otra expresión como argumento
    probarParser(
        "Programa 2: maximo y llamada",
        "let maximo = fn(a, b) {"
        "    if (a > b) { return a; } else { return b; }"
        "};"
        "let resultado = maximo(10, 3);"
    );

    // ── Programa 3: fibonacci recursivo ────────
    // Caso más complejo: función que se llama a sí misma,
    // argumentos como expresiones, resultado de llamada
    // usado como operando de BinaryExpression
    probarParser(
        "Programa 3: fibonacci recursivo",
        "let fibonacci = fn(n) {"
        "    if (n < 2) { return n; }"
        "    return fibonacci(n - 1) + fibonacci(n - 2);"
        "};"
    );
    return 0;
}