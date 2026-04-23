#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "lexer.h"
#include "ast.h"
#include "token.h"
#include "parser.h"
#include "environment.h"
#include "evaluator.h"

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

// ═════════════════════════════════════════════
// Prueba del Environment — Módulo 6
// ═════════════════════════════════════════════

int obtenerValor(std::shared_ptr<Object> obj) {
    if (obj == nullptr) return -1;
    auto* i = dynamic_cast<Integer*>(obj.get());
    if (i == nullptr) return -1;
    return i->value;
}

void probarEnvironment() {
    std::cout << std::endl;
    std::cout << "--------------------------------------------------------------------------" << std::endl;
    std::cout << "  MÓDULO 6 — Prueba del Environment"       << std::endl;
    std::cout << "--------------------------------------------------------------------------" << std::endl;

    // ── Caso 1: get y set en scope global ─────
    {
        auto global = std::make_shared<Environment>();
        global->set("x", std::make_shared<Integer>(10));
        global->set("y", std::make_shared<Integer>(20));

        std::cout << std::endl;
        std::cout << "  Caso 1: get y set en scope global" << std::endl;
        std::cout << "  x = " << obtenerValor(global->get("x"))
                    << "  (esperado: 10)" << std::endl;
        std::cout << "  y = " << obtenerValor(global->get("y"))
                    << "  (esperado: 20)" << std::endl;
        std::cout << "  z = " << obtenerValor(global->get("z"))
                    << "  (esperado: -1, no existe)" << std::endl;
    }

    // ── Caso 2: búsqueda en cadena ────────────
    {
        auto global = std::make_shared<Environment>();
        global->set("x", std::make_shared<Integer>(100));

        auto local = Environment::createEnclosed(global);
        local->set("a", std::make_shared<Integer>(5));

        std::cout << std::endl;
        std::cout << "  Caso 2: búsqueda en cadena" << std::endl;
        std::cout << "  a = " << obtenerValor(local->get("a"))
                    << "  (esperado: 5)" << std::endl;
        std::cout << "  x = " << obtenerValor(local->get("x"))
                    << "  (esperado: 100)" << std::endl;
        std::cout << "  z = " << obtenerValor(local->get("z"))
                    << "  (esperado: -1, no existe)" << std::endl;
    }

    // ── Caso 3: shadowing ─────────────────────
    {
        auto global = std::make_shared<Environment>();
        global->set("x", std::make_shared<Integer>(100));

        auto local = Environment::createEnclosed(global);
        local->set("x", std::make_shared<Integer>(999));

        std::cout << std::endl;
        std::cout << "  Caso 3: shadowing" << std::endl;
        std::cout << "  x desde local  = "
                    << obtenerValor(local->get("x"))
                    << "  (esperado: 999)" << std::endl;
        std::cout << "  x desde global = "
                    << obtenerValor(global->get("x"))
                    << "  (esperado: 100)" << std::endl;
    }

    // ── Caso 4: tres niveles de profundidad ───
    {
        auto S0 = std::make_shared<Environment>();
        S0->set("x", std::make_shared<Integer>(10));
        S0->set("y", std::make_shared<Integer>(20));

        auto S1 = Environment::createEnclosed(S0);
        S1->set("a", std::make_shared<Integer>(5));

        auto S2 = Environment::createEnclosed(S1);
        S2->set("b", std::make_shared<Integer>(3));

        std::cout << std::endl;
        std::cout << "  Caso 4: tres niveles de profundidad" << std::endl;
        std::cout << "  b = " << obtenerValor(S2->get("b"))
                    << "  (esperado: 3)"  << std::endl;
        std::cout << "  a = " << obtenerValor(S2->get("a"))
                    << "  (esperado: 5)"  << std::endl;
        std::cout << "  x = " << obtenerValor(S2->get("x"))
                    << "  (esperado: 10)" << std::endl;
        std::cout << "  z = " << obtenerValor(S2->get("z"))
                    << "  (esperado: -1, no existe)" << std::endl;
    }

    // ── Caso 5: set() no modifica el scope padre ──
    {
        auto global = std::make_shared<Environment>();
        global->set("x", std::make_shared<Integer>(10));

        auto local = Environment::createEnclosed(global);
        local->set("x", std::make_shared<Integer>(999));

        std::cout << std::endl;
        std::cout << "  Caso 5: set() no modifica el scope padre" << std::endl;
        std::cout << "  x desde local  = "
                    << obtenerValor(local->get("x"))
                    << "  (esperado: 999)" << std::endl;
        std::cout << "  x desde global = "
                    << obtenerValor(global->get("x"))
                    << "  (esperado: 10, sin cambios)" << std::endl;
    }

    // ── Caso 6: set() reemplaza si ya existe ──────
    {
        auto global = std::make_shared<Environment>();
        global->set("x", std::make_shared<Integer>(10));
        global->set("x", std::make_shared<Integer>(99));

        std::cout << std::endl;
        std::cout << "  Caso 6: set() reemplaza si ya existe" << std::endl;
        std::cout << "  x = " << obtenerValor(global->get("x"))
                    << "  (esperado: 99)" << std::endl;
    }

    // ── Caso 7: scopes hermanos son independientes
    {
        auto global = std::make_shared<Environment>();
        global->set("x", std::make_shared<Integer>(10));

        auto S1 = Environment::createEnclosed(global);
        S1->set("a", std::make_shared<Integer>(1));

        auto S2 = Environment::createEnclosed(global);
        S2->set("b", std::make_shared<Integer>(2));

        std::cout << std::endl;
        std::cout << "  Caso 7: scopes hermanos son independientes" << std::endl;
        std::cout << "  S1->a = " << obtenerValor(S1->get("a"))
                    << "  (esperado: 1)"  << std::endl;
        std::cout << "  S1->x = " << obtenerValor(S1->get("x"))
                    << "  (esperado: 10)" << std::endl;
        std::cout << "  S1->b = " << obtenerValor(S1->get("b"))
                    << "  (esperado: -1, S1 no ve a S2)" << std::endl;
        std::cout << "  S2->b = " << obtenerValor(S2->get("b"))
                    << "  (esperado: 2)"  << std::endl;
        std::cout << "  S2->x = " << obtenerValor(S2->get("x"))
                    << "  (esperado: 10)" << std::endl;
        std::cout << "  S2->a = " << obtenerValor(S2->get("a"))
                    << "  (esperado: -1, S2 no ve a S1)" << std::endl;
    }

    // ── Caso 8: cadena de 4 niveles ───────────────
    {
        auto S0 = std::make_shared<Environment>();
        S0->set("raiz", std::make_shared<Integer>(0));

        auto S1 = Environment::createEnclosed(S0);
        S1->set("nivel1", std::make_shared<Integer>(1));

        auto S2 = Environment::createEnclosed(S1);
        S2->set("nivel2", std::make_shared<Integer>(2));

        auto S3 = Environment::createEnclosed(S2);
        S3->set("nivel3", std::make_shared<Integer>(3));

        std::cout << std::endl;
        std::cout << "  Caso 8: cadena de 4 niveles" << std::endl;
        std::cout << "  raiz   = " << obtenerValor(S3->get("raiz"))
                    << "  (esperado: 0, sube 3 niveles)" << std::endl;
        std::cout << "  nivel1 = " << obtenerValor(S3->get("nivel1"))
                    << "  (esperado: 1, sube 2 niveles)" << std::endl;
        std::cout << "  nivel2 = " << obtenerValor(S3->get("nivel2"))
                    << "  (esperado: 2, sube 1 nivel)"  << std::endl;
        std::cout << "  nivel3 = " << obtenerValor(S3->get("nivel3"))
                    << "  (esperado: 3, está aquí mismo)" << std::endl;
    }
}

// --------------------------------------------------------------------------
// Prueba del sistema de tipos — Módulo 7
// --------------------------------------------------------------------------
void probarSistemaDetipos() {
    std::cout << std::endl;
    std::cout << "--------------------------------------------------------------------------" << std::endl;
    std::cout << "  MODULO 7-- Sistema de tipos"             << std::endl;
    std::cout << "--------------------------------------------------------------------------" << std::endl;

    // ── Integer ───────────────────────────────
    auto i = std::make_shared<Integer>(42);
    std::cout << std::endl;
    std::cout << "  Integer(42):" << std::endl;
    std::cout << "    type()    = " << objectTypeToString(i->type())
                << "  (esperado: INTEGER)" << std::endl;
    std::cout << "    inspect() = " << i->inspect()
                << "  (esperado: 42)" << std::endl;

    // ── Boolean true ─────────────────────────
    std::cout << std::endl;
    std::cout << "  LUX_TRUE:" << std::endl;
    std::cout << "    type()    = " << objectTypeToString(LUX_TRUE->type())
                << "  (esperado: BOOLEAN)" << std::endl;
    std::cout << "    inspect() = " << LUX_TRUE->inspect()
                << "  (esperado: true)" << std::endl;

    // ── Boolean false ────────────────────────
    std::cout << std::endl;
    std::cout << "  LUX_FALSE:" << std::endl;
    std::cout << "    type()    = " << objectTypeToString(LUX_FALSE->type())
                << "  (esperado: BOOLEAN)" << std::endl;
    std::cout << "    inspect() = " << LUX_FALSE->inspect()
                << "  (esperado: false)" << std::endl;

    // ── Null ──────────────────────────────────
    std::cout << std::endl;
    std::cout << "  LUX_NULL:" << std::endl;
    std::cout << "    type()    = " << objectTypeToString(LUX_NULL->type())
                << "  (esperado: NULL)" << std::endl;
    std::cout << "    inspect() = " << LUX_NULL->inspect()
                << "  (esperado: null)" << std::endl;

    // ── nativeBoolToObject ────────────────────
    std::cout << std::endl;
    std::cout << "  nativeBoolToObject(5 > 3)  = "
                << nativeBoolToObject(5 > 3)->inspect()
                << "  (esperado: true)" << std::endl;
    std::cout << "  nativeBoolToObject(1 == 2) = "
                << nativeBoolToObject(1 == 2)->inspect()
                << "  (esperado: false)" << std::endl;

    // ── Instancias compartidas ────────────────
    std::cout << std::endl;
    std::cout << "  Instancias compartidas (mismo puntero):" << std::endl;
    std::cout << "    nativeBoolToObject(true) == LUX_TRUE:  "
                << (nativeBoolToObject(true) == LUX_TRUE ? "SI" : "NO")
                << "  (esperado: SI)" << std::endl;
    std::cout << "    nativeBoolToObject(false) == LUX_FALSE: "
                << (nativeBoolToObject(false) == LUX_FALSE ? "SI" : "NO")
                << "  (esperado: SI)" << std::endl;

    // ── Polimorfismo: llamar inspect() sin saber el tipo ──
    std::cout << std::endl;
    std::cout << "  Polimorfismo -- inspect() via Object*:" << std::endl;
    std::vector<std::shared_ptr<Object>> objetos = {
        std::make_shared<Integer>(7),
        LUX_TRUE,
        LUX_FALSE,
        LUX_NULL,
        std::make_shared<Integer>(-3)
    };
    for (const auto& obj : objetos) {
        // obj es shared_ptr<Object> pero apunta a Integer, Boolean o Null.
        // obj->inspect() llama la implementación correcta de cada subclase.
        std::cout << "    [" << objectTypeToString(obj->type()) << "] "
                    << obj->inspect() << std::endl;
    }
}

// ═════════════════════════════════════════════
// Prueba del pipeline completo — Módulo 7
// Lexer → Parser → Evaluator → resultado
// ═════════════════════════════════════════════
void probarEvaluador() {
    std::cout << std::endl;
    std::cout << "---------------------------------------------------------------------------" << std::endl;
    std::cout << "  MODULO 7 --- Pipeline completo"            << std::endl;
    std::cout << "---------------------------------------------------------------------------" << std::endl;

    Evaluator ev;

    // Helper lambda: recibe código Lux, lo evalúa y muestra el resultado.
    // auto: C++ deduce el tipo (en este caso una lambda — función anónima).
    // [&ev]: captura la variable ev por referencia para usarla dentro.
    // const std::string&: el código como referencia constante, sin copiar.
    auto evaluar = [&ev](const std::string& descripcion,
                            const std::string& codigo) {
        // Crear entorno global fresco para cada prueba
        auto env = std::make_shared<Environment>();

        // Pipeline: Lexer → tokens
        Lexer lexer(codigo);
        auto tokens = lexer.tokenize();

        // Pipeline: tokens → AST
        Parser parser(std::move(tokens));
        auto program = parser.parse();

        // Si el parser encontró errores, los mostramos y no evaluamos
        if (!parser.errors.empty()) {
            std::cout << std::endl;
            std::cout << "  [ERROR PARSER] " << descripcion << std::endl;
            for (const auto& e : parser.errors) {
                std::cout << "    " << e << std::endl;
            }
            return;
        }

        // Pipeline: AST → valor
        auto resultado = ev.eval(program.get(), env);

        // Mostrar resultado
        std::cout << std::endl;
        std::cout << "  " << descripcion << std::endl;
        std::cout << "  Codigo   : " << codigo << std::endl;
        std::cout << "  Tipo     : " << objectTypeToString(resultado->type()) << std::endl;
        std::cout << "  Resultado: " << resultado->inspect() << std::endl;
    };

    // ── Literales simples ─────────────────────
    evaluar("Literal entero",   "42;");
    evaluar("Literal negativo", "-7;");
    evaluar("Literal true",     "true;");
    evaluar("Literal false",    "false;");

    // ── Aritmética ────────────────────────────
    evaluar("Suma",              "1 + 2;");
    evaluar("Resta",             "10 - 3;");
    evaluar("Multiplicacion",    "3 * 4;");
    evaluar("Division",          "10 / 2;");
    evaluar("Precedencia",       "1 + 2 * 3;");
    evaluar("Parentesis",        "(1 + 2) * 3;");
    evaluar("Expresion larga",   "2 + 3 * 4 - 1;");

    // ── Prefijos ─────────────────────────────
    evaluar("Negacion numerica", "-5;");
    evaluar("Negacion logica",   "!true;");
    evaluar("Doble negacion",    "!!false;");
    evaluar("Negacion compleja", "-(3 + 4);");

    // ── Comparaciones enteras → Boolean ───────
    evaluar("Menor que (true)",   "3 < 5;");
    evaluar("Menor que (false)",  "5 < 3;");
    evaluar("Mayor que",          "10 > 2;");
    evaluar("Igual (true)",       "5 == 5;");
    evaluar("Igual (false)",      "5 == 6;");
    evaluar("Distinto",           "4 != 5;");
    evaluar("Menor o igual",      "3 <= 3;");
    evaluar("Mayor o igual",      "7 >= 10;");

    // ── Comparaciones booleanas ───────────────
    evaluar("true == true",   "true == true;");
    evaluar("true == false",  "true == false;");
    evaluar("false != true",  "false != true;");

    // ── Expresiones anidadas ──────────────────
    evaluar("Comparacion con aritmetica", "1 + 2 == 3;");
    evaluar("Expresion compleja",         "2 * 3 + 4 * 5;");

    std::cout << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl;
    std::cout << "MODULO 8.2 --- Identifier, LetStatement y Error" << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl << std::endl;

    evaluar("let simple",
                    "let x = 5; x;");

    evaluar("let con expresion",
                    "let resultado = 3 + 4 * 2; resultado;");

    evaluar("dos variables",
                    "let a = 10; let b = 3; a + b;");

    evaluar("variable no definida -> Error",
                    "z + 1;");

    evaluar("division por cero -> Error",
                    "10 / 0;");

    evaluar("tipo incorrecto en prefijo -> Error",
                    "-true;");

    evaluar("tipos incompatibles en binaria -> Error",
                    "5 + true;");

    evaluar("error se propaga (no evalua resto)",
                    "z; 1 + 2;");

    std::cout << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl;
    std::cout << "MODULO 8.3/8.4 --- BlockStatement e If" << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl << std::endl;

    // Bloque sin else — condición verdadera
    evaluar("if true devuelve valor del bloque",
            "if (true) { let x = 5; x; }");

    // Bloque sin else — condición falsa, sin alternative → null
    evaluar("if false sin else devuelve null",
            "if (false) { 999; }");

    // if/else con condición verdadera
    evaluar("if true con else",
            "if (true) { 10; } else { 20; }");

    // if/else con condición falsa
    evaluar("if false con else",
            "if (false) { 10; } else { 20; }");

    // Condición es una comparación
    evaluar("condicion aritmetica verdadera",
            "if (3 > 1) { 42; } else { 0; }");

    evaluar("condicion aritmetica falsa",
            "if (3 < 1) { 42; } else { 0; }");

    // if como valor asignado a variable
    evaluar("if como valor en let",
            "let x = 5; let resultado = if (x > 3) { 100; } else { 0; }; resultado;");

    // Bloque con varias sentencias — devuelve la última
    evaluar("bloque devuelve ultima sentencia",
            "if (true) { let a = 3; let b = 4; a + b; }");

    // Error en la condición se propaga
    evaluar("error en condicion se propaga",
            "if (z > 0) { 1; } else { 2; }");

    // Error dentro del bloque se propaga
    evaluar("error dentro del bloque",
            "if (true) { z; 99; }");

    // if anidado
    evaluar("if anidado",
            "let x = 5; if (x > 0) { if (x > 3) { 1; } else { 2; } } else { 0; }");
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
    //!Prueba--Environment
    probarEnvironment();

    //!Prueba-Sistema-de-Tipos
    probarSistemaDetipos();

    //!Prueba-Evaluador
    probarEvaluador();

    return 0;
}