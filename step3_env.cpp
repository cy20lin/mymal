#include "reader.hpp"
#include "types.hpp"
#include "printer.hpp"
#include "utility.hpp"
#include <memory>
#include <iostream>
#include <string>

std::string readline(bool & ok, std::istream * in = nullptr) {
    if (!in) in = &std::cin;
    std::string line;
    std::getline(*in, line, '\n');
    ok = in->operator bool();
    return line;
}

void printline(std::string value, std::ostream * out = nullptr) {
    if (!out) out = &std::cout;
    *out << value << std::endl;
}

using Environment = MalMap; 
class Env;
using EnvPtr = std::shared_ptr<Env>;
class Env {
public:
    Env() = default;
    Env(const Env &) = default;
    Env(Env &&) = default;
    Env(std::shared_ptr<Env> outer) : outer_(outer), data_() {}
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

EnvPtr get_repl_env() {
    static EnvPtr env = std::make_shared<Env>();
    auto add = [](MalType args) -> MalType {
        auto a = to_array_args<2>(args);
        if (a[0].get_if<MalInt>() && a[1].get_if<MalInt>()) {
            return MalInt(a[0].get<MalInt>() + a[1].get<MalInt>());
        }
        if (a[0].get_if<MalInt>() && a[1].get_if<MalFloat>()) {
            return MalFloat(a[0].get<MalInt>() + a[1].get<MalFloat>());
        }
        if (a[0].get_if<MalFloat>() && a[1].get_if<MalInt>()) {
            return MalFloat(a[0].get<MalFloat>() + a[1].get<MalInt>());
        }
        if (a[0].get_if<MalFloat>() && a[1].get_if<MalFloat>()) {
            return MalFloat(a[0].get<MalFloat>() + a[1].get<MalFloat>());
        }
        if (a[0].get_if<MalString>() && a[1].get_if<MalString>()) {
            return MalString(a[0].get<MalString>() + a[1].get<MalString>());
        }
        throw std::runtime_error("eval error: add: invalid arguments");
    };
    auto sub = [](MalType args) -> MalType {
        auto a = to_array_args<2>(args);
        if (a[0].get_if<MalInt>() && a[1].get_if<MalInt>()) {
            return MalInt(a[0].get<MalInt>() - a[1].get<MalInt>());
        }
        if (a[0].get_if<MalInt>() && a[1].get_if<MalFloat>()) {
            return MalFloat(a[0].get<MalInt>() - a[1].get<MalFloat>());
        }
        if (a[0].get_if<MalFloat>() && a[1].get_if<MalInt>()) {
            return MalFloat(a[0].get<MalFloat>() - a[1].get<MalInt>());
        }
        if (a[0].get_if<MalFloat>() && a[1].get_if<MalFloat>()) {
            return MalFloat(a[0].get<MalFloat>() - a[1].get<MalFloat>());
        }
        throw std::runtime_error("eval error: sub: invalid arguments");
    };
    auto mul = [](MalType args) -> MalType {
        auto a = to_array_args<2>(args);
        if (a[0].get_if<MalInt>() && a[1].get_if<MalInt>()) {
            return MalInt(a[0].get<MalInt>() * a[1].get<MalInt>());
        }
        if (a[0].get_if<MalInt>() && a[1].get_if<MalFloat>()) {
            return MalFloat(a[0].get<MalInt>() * a[1].get<MalFloat>());
        }
        if (a[0].get_if<MalFloat>() && a[1].get_if<MalInt>()) {
            return MalFloat(a[0].get<MalFloat>() * a[1].get<MalInt>());
        }
        if (a[0].get_if<MalFloat>() && a[1].get_if<MalFloat>()) {
            return MalFloat(a[0].get<MalFloat>() * a[1].get<MalFloat>());
        }
        throw std::runtime_error("eval error: mul: invalid arguments");
    };
    auto div = [](MalType args) -> MalType {
        auto a = to_array_args<2>(args);
        if (a[0].get_if<MalInt>() && a[1].get_if<MalInt>()) {
            return MalInt(a[0].get<MalInt>() / a[1].get<MalInt>());
        }
        if (a[0].get_if<MalInt>() && a[1].get_if<MalFloat>()) {
            return MalFloat(a[0].get<MalInt>() / a[1].get<MalFloat>());
        }
        if (a[0].get_if<MalFloat>() && a[1].get_if<MalInt>()) {
            return MalFloat(a[0].get<MalFloat>() / a[1].get<MalInt>());
        }
        if (a[0].get_if<MalFloat>() && a[1].get_if<MalFloat>()) {
            return MalFloat(a[0].get<MalFloat>() / a[1].get<MalFloat>());
        }
        throw std::runtime_error("eval error: div: invalid arguments");
    };
    env->set(MalSymbol("+"), MalFunction(add));
    env->set(MalSymbol("-"), MalFunction(sub));
    env->set(MalSymbol("*"), MalFunction(mul));
    env->set(MalSymbol("/"), MalFunction(div));
    return env;
}

MalType READ(std::string str) {
    return read_str(str);
}

MalType EVAL(MalType ast, EnvPtr env);
MalType eval_ast(MalType ast, EnvPtr env) {
    if (MalSymbol *p = std::get_if<MalSymbol>(&ast.variant())) {
        if (!env) throw std::runtime_error("eval error: symbol not found with null env");
        return env->get(*p);
    } 
    else if (MalList *p = std::get_if<MalList>(&ast.variant())) {
        MalList list;
        auto i = list.before_begin();
        std::for_each(p->begin(), p->end(), [&](MalType& v){
            i = list.insert_after(i, EVAL(v, env));
        });
        return list;
    } 
    else if (MalVector *p = std::get_if<MalVector>(&ast.variant())) {
        MalVector vector;
        std::for_each(p->begin(), p->end(), [&](MalType& v){
            vector.push_back(EVAL(v, env));
        });
        return vector;
    } 
    else if (MalMap *p = std::get_if<MalMap>(&ast.variant())) {
        MalMap map;
        std::for_each(p->begin(), p->end(), [&](MalMap::value_type& v){
            map.insert_or_assign(v.first, EVAL(v.second, env));
        });
        return map;
    } 
    else {
        return ast;
    } 
}

MalType EVAL(MalType ast, EnvPtr env) {
    MalList *listp= std::get_if<MalList>(&ast.variant());
    if (!listp) {
        return eval_ast(ast, env);
    } else if (listp->empty()) {
        return ast;
    } else {
        MalType first = listp->front();
        MalList& rest = (listp->pop_front(), *listp);
        if (MalSymbol* symp = first.get_if<MalSymbol>()) {
            if (*symp == "def!") {
                std::array<MalType,2> a = to_array_args<2>(rest);
                MalSymbol key = a[0].get<MalSymbol>();
                MalType value = EVAL(a[1], env);
                env->set(key, value);
                return value;
            } else if (*symp == "let*") {
                EnvPtr new_env = std::make_shared<Env>(env);
                auto a = to_array_args<2>(rest); // args
                auto b = to_vector_args(a[0]); // bindings
                if (b.size() %2 == 1) throw std::runtime_error("eval error: let*: invalid binding list, expect even items but got odd");
                for (std::size_t i = 0; i < b.size()/2; ++i) {
                    auto k = i*2;
                    auto v = i*2+1;
                    MalSymbol key = b[k].get<MalSymbol>();
                    MalType value = EVAL(b[v], new_env);
                    new_env->set(key, value);
                }
                return EVAL(a[1], new_env);
            } else {
                MalType eval_first =  eval_ast(first, env);
                MalType eval_rest = eval_ast(rest, env);
                MalFunction* fnp = eval_first.get_if<MalFunction>();
                if (!fnp) throw std::runtime_error("eval error: not a function object for call");
                MalFunction& fn = *fnp;
                MalType & args = eval_rest;
                return fn(args);
            }
        }
    }
}

std::string PRINT(MalType value) {
    return pr_str(value);
}


std::string rep(std::string str, EnvPtr env) {
    MalType expr = READ(str);
    MalType value = EVAL(expr,env);
    std::string ostr = PRINT(value);
    return ostr;
}


int main() {
    const char* prompt = "user> ";
    EnvPtr repl_env = get_repl_env();
    bool ok = true;
    while (true) {
        std::cout << prompt << std::flush;
        std::string input = readline(ok);
        if (!ok)
            break;
        try {
            std::string output = rep(input, repl_env);
            printline(output);
        } catch (std::exception & e) {
            printline(e.what(), &std::cerr);
        }
    }
    return 0;
}