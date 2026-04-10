#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <map>
#include <string>
#include <memory>
#include "object.h"

class Environment {
public:
    Environment(): outer_(nullptr) {}

    explicit Environment(std::shared_ptr<Environment> outer): outer_(std::move(outer)) {}

    std::shared_ptr<Object> get(const std::string& name) {
        auto it = store_.find(name);
        if (it != store_.end()) {
            return it->second;
        }
        if (outer_ != nullptr) {
            return outer_->get(name);
        }
        return nullptr;
    }

    void set(const std::string& name, std::shared_ptr<Object> value) {
        store_[name] = std::move(value);
    }

    static std::shared_ptr<Environment> createEnclosed(std::shared_ptr<Environment> outer) {
        return std::make_shared<Environment>(std::move(outer));
    }

private:
    std::map<std::string, std::shared_ptr<Object>> store_;
    std::shared_ptr<Environment> outer_;
};

#endif