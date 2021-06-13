#ifndef MAL_CORE_HEADER_HPP_INCLUDED
#define MAL_CORE_HEADER_HPP_INCLUDED
#include "types.hpp"
#include <memory>

class Env;
using EnvPtr = std::shared_ptr<Env>;
class Env {  
public:
    Env() = default;
    Env(const Env &) = default;
    Env(Env &&) = default;
    Env & operator=(const Env &) = default;
    Env & operator=(Env &&) = default;
    Env(std::shared_ptr<Env> outer) : outer_(outer), data_() {}
    Env(std::shared_ptr<Env> outer, MalList binds, MalList exprs) : outer_(outer), data_() {
        MalList::iterator ivalue = exprs.begin();
        for (auto& bind : binds) {
            if (!bind.get_if<MalSymbol>()) 
                throw std::runtime_error("eval error: env: bind is not a symbol");
            if (ivalue == exprs.end()) 
                throw std::runtime_error("eval error: env: bind doesn't correspond to an expr");
            data_.insert_or_assign(bind, *ivalue);
            ++ivalue;
        }
        if (ivalue != exprs.end()) 
            throw std::runtime_error("eval error: env: binds and exprs with different size");
    }
    void set(MalSymbol key, MalType value) {
        data_.insert_or_assign(key, std::move(value));
    }
    MalType find(MalSymbol key) {
        auto i = data_.find(key);
        if (i != data_.end()) {
            return i->second;
        } else if (outer_) {
            return outer_->find(key);
        } else {
            return MalUndefined();
        }
    }
    MalType get(MalSymbol key) {
        MalType value = find(key);
        if (value.get_if<MalUndefined>()) {
            throw std::runtime_error("env error: symbol '"+key+" not found");
        } else {
            return value;
        }
    }
private:
    std::shared_ptr<Env> outer_;
    MalMap data_;
};
#endif//MAL_CORE_HEADER_HPP_INCLUDED