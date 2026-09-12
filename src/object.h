#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <memory>

enum class ObjectType {
    INTEGER,
    BOOLEAN,
    NULL_OBJ,
    RETURN_VALUE,
    FUNCTION,
    ERROR
};

inline std::string objectTypeToString(ObjectType t) {
    switch (t)
    {
    case ObjectType::INTEGER:        return "INTEGER";
    case ObjectType::BOOLEAN:       return "BOOLEAN";
    case ObjectType::NULL_OBJ:         return "NULL";
    case ObjectType::RETURN_VALUE:  return "RETURN_VALUE";
    case ObjectType::FUNCTION:          return "FUNCTION";
    case ObjectType::ERROR:                 return "ERROR";
    default:                                        return "UNKNOWN";
    }
}

class Object {
public:
    virtual ~Object() {}

    virtual ObjectType type() const = 0;

    virtual std::string inspect() const = 0;
}; 

class Integer : public Object {
public:
    int value;

    explicit Integer(int v) : value(v) {}

    ObjectType type() const override {return ObjectType::INTEGER;}

    std::string inspect() const override {return std::to_string(value);}
};

class Boolean : public Object {
public:
    bool value;

    explicit Boolean(bool v) : value(v) {}

    ObjectType type() const override {return ObjectType::BOOLEAN;}

    std::string inspect() const override {return value ? "true" : "false";}
};

class Null : public Object {
public:
    Null() {}

    ObjectType type() const override {return ObjectType::NULL_OBJ;}

    std::string inspect() const override {return "null";}
};

class Error : public Object {
public:
    std::string message;

    explicit Error(std::string msg) : message(std::move(msg)) {}

    ObjectType      type()  const override { return ObjectType::ERROR; }
    std::string     inspect() const override { return "ERROR: " + message; }
};

class ReturnValue : public Object {
public:
    std::shared_ptr<Object> value;

    explicit ReturnValue(std::shared_ptr<Object> v) : value(std::move(v)) {}

    ObjectType   type()   const override { return ObjectType::RETURN_VALUE; }
    std::string inspect() const override { return value->inspect(); }
};

inline std::shared_ptr<Boolean> LUX_TRUE = std::make_shared<Boolean>(true);
inline std::shared_ptr<Boolean> LUX_FALSE = std::make_shared<Boolean>(false);
inline std::shared_ptr<Null> LUX_NULL = std::make_shared<Null>();

inline std::shared_ptr<Boolean> nativeBoolToObject(bool input){
    return input ? LUX_TRUE : LUX_FALSE;
}

inline bool isError(const std::shared_ptr<Object>& obj) {
    return obj != nullptr && obj->type() == ObjectType::ERROR;
}

inline bool isTruthy (const std::shared_ptr<Object>& obj) {
    if (obj == LUX_NULL) return false;
    if (obj == LUX_TRUE) return true;
    if (obj == LUX_FALSE) return false;
    return true;
}

#endif