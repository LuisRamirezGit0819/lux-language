#include "evaluator.h"

std::shared_ptr<Object> Evaluator::eval(Node* node, std::shared_ptr<Environment> env){

    //-----------Nodo raiz-------------------
    if (auto* n = dynamic_cast<Program*>(node)) {
        return evalProgram(n, env);
    }

    //-------------Sentencias-----------------
    //ExpressionStatement
    if (auto* n = dynamic_cast<ExpressionStatement*>(node)) {
        return eval(n->expression.get(), env);
    }

    if (auto* n = dynamic_cast<LetStatement*>(node)) {
        return evalLetStatement(n, env);
    }

    if (auto* n = dynamic_cast<Identifier*>(node)) {
        return evalIdentifier(n, env);
    }

    //------------Expresiones----------------
    if (auto* n = dynamic_cast<NumberLiteral*>(node)) {
        return evalNumberLiteral(n);
    }

    if (auto* n = dynamic_cast<BooleanLiteral*>(node)) {
        return evalBooleanLiteral(n);
    }

    if (auto* n = dynamic_cast<PrefixExpression*>(node)) {
        return evalPrefixExpression(n, env);
    }

    if (auto* n = dynamic_cast<BinaryExpression*>(node)) {
        return evalBinaryExpression(n, env);
    }

    return LUX_NULL;
}

std::shared_ptr<Object> Evaluator::evalProgram(Program* node, std::shared_ptr<Environment> env) {

    std::shared_ptr<Object> result = LUX_NULL;

    for (const auto& stmt : node->statements) {
        result = eval(stmt.get(), env);
        if (isError(result)) return result;
    }

    return result;
}

std::shared_ptr<Object> Evaluator::evalLetStatement(LetStatement* node, std::shared_ptr<Environment> env) {

    auto val = eval(node->value.get(), env);

    if (isError(val)) return val;

    env->set(node->name, val);

    return LUX_TRUE;
}

std::shared_ptr<Object> Evaluator::evalIdentifier(Identifier* node, std::shared_ptr<Environment> env) {

    auto val = env->get(node->name);

    if (val != nullptr) return val;

    return std::make_shared<Error>("variable no definida: " + node->name);
}

std::shared_ptr<Object> Evaluator::evalNumberLiteral(NumberLiteral* node) {
    return std::make_shared<Integer>(node->value);
}

std::shared_ptr<Object> Evaluator::evalBooleanLiteral(BooleanLiteral* node) {
    return nativeBoolToObject(node->value);
}

std::shared_ptr<Object> Evaluator::evalPrefixExpression(PrefixExpression* node, std::shared_ptr<Environment> env) {

    auto right = eval(node->right.get(), env);

    if (isError(right)) return right;

    if (node->op == "-") {
        auto* intObj = dynamic_cast<Integer*>(right.get());
        if (intObj == nullptr) {
            return std::make_shared<Error>("operrador desconocido: -" + right->inspect());
        }
        return std::make_shared<Integer>(-intObj->value);
    }

    if (node->op == "!") {
        auto* boolObj = dynamic_cast<Boolean*>(right.get());
        if (boolObj == nullptr) {
            return std::make_shared<Error>("operrador desconocido: !" + right->inspect());
        }
        return std::make_shared<Boolean>(!boolObj->value);
    }

    return std::make_shared<Error>("operrador desconocido: " + node->op);
}

std::shared_ptr<Object> Evaluator::evalBinaryExpression(BinaryExpression* node, std::shared_ptr<Environment> env) {

    auto left = eval(node->left.get(), env);
    if(isError(left)) return left;
    auto right = eval(node->right.get(), env);
    if (isError(right)) return right;

    if (left->type() == ObjectType::INTEGER && right->type() == ObjectType::INTEGER) {
        return evalIntegerBinaryExpression(node->op, left, right);
    }
    
    if (left->type()  == ObjectType::BOOLEAN && right->type() == ObjectType::BOOLEAN) {
        return evalBooleanBinaryExpression(node->op, left, right);
    }

    return std::make_shared<Error>("tipo incompatible: " + left->inspect() + " " + node->op + " " + right->inspect());
}

std::shared_ptr<Object> Evaluator::evalIntegerBinaryExpression(const std::string& op, std::shared_ptr<Object> left, std::shared_ptr<Object> right) {

    auto* l = dynamic_cast<Integer*>(left.get());
    auto* r = dynamic_cast<Integer*>(right.get());

    int lv = l->value;
    int rv = r->value;

    if (op == "+") return std::make_shared<Integer>(lv + rv);
    if (op == "-") return std::make_shared<Integer>(lv - rv);
    if (op == "*") return std::make_shared<Integer>(lv * rv);
    if (op == "/") {
        if (rv == 0) return std::make_shared<Error>("division por cero");
        return std::make_shared<Integer>(lv/rv);
    }
    if (op == "==") return nativeBoolToObject(lv == rv);
    if (op == "!=") return nativeBoolToObject(lv != rv);
    if (op == "<")  return nativeBoolToObject(lv <  rv);
    if (op == ">")  return nativeBoolToObject(lv >  rv);
    if (op == "<=") return nativeBoolToObject(lv <= rv);
    if (op == ">=") return nativeBoolToObject(lv >= rv);

    return std::make_shared<Error>("operador desconocido: " + op);
}

std::shared_ptr<Object> Evaluator::evalBooleanBinaryExpression (const std::string& op, std::shared_ptr<Object> left, std::shared_ptr<Object> right) {

    if (op == "==") return nativeBoolToObject(left == right);
    if (op == "!=") return nativeBoolToObject(left != right);

    return std::make_shared<Error>("operador desconocido entre booleanos: " + op);
}