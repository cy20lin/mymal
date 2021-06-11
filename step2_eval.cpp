#include "reader.hpp"
#include "types.hpp"
#include "printer.hpp"
#include "utility.hpp"
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


template <std::size_t N>
auto to_array_args(MalType args) -> std::array<MalType, N> {
    std::array<MalType, N> result;
    if (auto *p = std::get_if<MalList>(&args.variant())) {
        std::size_t i = 0;
        std::copy_if(p->begin(), p->end(), result.begin(), [&i](auto&&) { return i++ < N; });
        if (i  != N) {
            throw std::logic_error("eval error: invalid arguments size " + std::to_string(i) + " expected " + std::to_string(N));
        }
        return result;
    }
    if (auto *p = std::get_if<MalVector>(&args.variant())) {
        std::size_t i = 0;
        std::copy_if(p->begin(), p->end(), result.begin(), [&i](auto&&) { return i++ < N; });
        if (i != N) {
            throw std::logic_error("eval error: invalid arguments size " + std::to_string(i) + " expected " + std::to_string(N));
        }
        return result;
    }
    throw std::logic_error("eval error: invalid arguments type");
};

template<std::size_t N, typename F>
MalFunction wrap_fn(F fn) {
    return [fn](MalType args) -> MalType { 
        std::array<MalType, N> array_args = to_array_args<N>(args);
        return std::apply([&fn](auto ... args) {
            return std::visit(fn, args.variant()...);
        }, array_args);
    };
}

using Environment = MalMap; 
Environment& get_repl_env() {
    static Environment e;
    // auto err = [](auto&&...) -> MalUndefined { throw std::runtime_error("eval error: no such operator"); };
    // auto add = overloaded {std::plus<MalInt>{}, std::plus<MalFloat>{}, std::plus<MalString>{}, err};
    // auto sub = overloaded {std::minus<MalInt>{}, std::minus<MalFloat>{}, std::minus<MalString>{}, err};
    // auto div = overloaded {std::divides<MalInt>{}, std::divides<MalFloat>{}, err};
    // auto mul = overloaded {std::multiplies<MalInt>{}, std::multiplies<MalFloat>{}, err};
    // auto mod = overloaded {std::modulus<MalInt>{}, std::modulus<MalFloat>{}, err};
    // auto add = [](MalType args) -> MalType {
    //     MalList list = std::get<MalList>(args.variant());
    //     return MalInt(1);
    // };
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
            return MalInt(a[0].get<MalFloat>() + a[1].get<MalFloat>());
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
    e[MalSymbol("+")] = MalFunction(add);
    e[MalSymbol("-")] = MalFunction(sub);
    e[MalSymbol("*")] = MalFunction(mul);
    e[MalSymbol("/")] = MalFunction(div);
    return e;
}

MalType READ(std::string str) {
    return read_str(str);
}

MalType EVAL(MalType ast, Environment & env);
MalType eval_ast(MalType ast, Environment & env) {
    if (MalSymbol *p = std::get_if<MalSymbol>(&ast.variant())) {
        return env.at(ast);
    } 
    else if (MalList *p = std::get_if<MalList>(&ast.variant())) {
        MalList list;
        auto i = list.before_begin();
        std::for_each(p->begin(), p->end(), [&](MalType& v){
            i = list.insert_after(i, EVAL(v, env));
        });
        return list;
    } 
    else {
        return ast;
    } 
}

MalType EVAL(MalType ast, Environment & env) {
    MalList *listp= std::get_if<MalList>(&ast.variant());
    if (!listp) {
        return eval_ast(ast, env);
    } else if (listp->empty()) {
        return ast;
    } else {
        MalList fn_args = std::get<MalList>(eval_ast(ast, env).variant());
        MalFunction* fnp = std::get_if<MalFunction>(&fn_args.front().variant());
        if (!fnp) throw std::runtime_error("eval error: not a function object for call");
        MalFunction fn = std::move(*fnp);
        fn_args.pop_front();
        MalList& args = fn_args;
        return fn(args);
    }
}

std::string PRINT(MalType value) {
    return pr_str(value);
}


std::string rep(std::string str, Environment & env) {
    MalType expr = READ(str);
    MalType value = EVAL(expr,env);
    std::string ostr = PRINT(value);
    return ostr;
}


int main() {
    const char* prompt = "user> ";
    Environment & repl_env = get_repl_env();
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