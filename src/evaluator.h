#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <memory>
#include "ast.h"
#include "object.h"
#include "environment.h"

class Evaluator {
public:
    std::shared_ptr<Object> eval(Node* node, std::shared_ptr<Environment> env);

private:

    // ── Evaluadores especializados ────────────

    // Evalúa el nodo raíz: ejecuta todas las sentencias en orden.
    // Devuelve el valor de la última sentencia evaluada.
    std::shared_ptr<Object> evalProgram(Program* node, std::shared_ptr<Environment> env);

    std::shared_ptr<Object> evalLetStatement(LetStatement* node, std::shared_ptr<Environment> env);

    // Evalúa un NumberLiteral: convierte el int del nodo a Integer.
    // No necesita entorno — es un valor literal, no depende de variables.
    std::shared_ptr<Object> evalNumberLiteral(NumberLiteral* node);

    // Evalúa un BooleanLiteral: devuelve LUX_TRUE o LUX_FALSE.
    // No necesita entorno por la misma razón.
    std::shared_ptr<Object> evalBooleanLiteral(BooleanLiteral* node);

    std::shared_ptr<Object> evalIdentifier(Identifier* node, std::shared_ptr<Environment> env);

    // Evalúa una PrefixExpression: primero evalúa el operando,
    // luego aplica el operador (- o !).
    std::shared_ptr<Object> evalPrefixExpression(PrefixExpression* node, std::shared_ptr<Environment> env);

    // Evalúa una BinaryExpression: evalúa izquierda y derecha,
    // luego aplica el operador.
    std::shared_ptr<Object> evalBinaryExpression(BinaryExpression* node, std::shared_ptr<Environment> env);

    // ── Helpers de operadores ─────────────────

    // Aplica un operador binario a dos enteros.
    // op: "+", "-", "*", "/", "==", "!=", "<", ">", "<=", ">="
    std::shared_ptr<Object> evalIntegerBinaryExpression(
        const std::string& op,
        std::shared_ptr<Object> left,
        std::shared_ptr<Object> right
    );

    // Aplica un operador de igualdad/desigualdad a dos booleanos.
    std::shared_ptr<Object> evalBooleanBinaryExpression(
        const std::string& op,
        std::shared_ptr<Object> left,
        std::shared_ptr<Object> right
    );
};

#endif