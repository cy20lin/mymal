#include <utility>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <functional>

#include "reader.hpp"
#include "types.hpp"
#include "printer.hpp"
#include "utility.hpp"

std::string readline() {
    std::string line;
    std::getline(std::cin, line, '\n');
    return line;
}

MalType READ() {
    const char* prompt = "user> ";
    std::cout << prompt << std::flush;
    return read_str(readline());
}

MalType EVAL(MalType expr) {
    MalType value = expr;
    return value;
}

void printline(std::string value) {
    std::cout << value << std::endl;
}

void PRINT(MalType value) {
    printline(pr_str(value));
}

void rep() {
    while(true) {
        auto expr = READ();
        if (!std::cin) break;
        auto value = EVAL(expr);
        PRINT(value);
    }
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
        // return MalInt();
        return std::apply([&fn](auto ... args) {
            return std::visit(fn, args.variant()...);
        }, array_args);
    };
    // return [fn](MalType args) -> MalType {
    //     std::array<MalType, N> array_args = to_array_args<N>(args);
    //     std::apply([&fn](auto && ... args) -> decltype(auto) {
    //         return std::visit(fn, args.variant()...);
    //     }, array_args);
    // };
}


using Environment = MalMap; 
Environment& get_repl_env() {
    static Environment e;
    auto noop = [](auto&&...) -> MalUndefined { throw std::runtime_error("eval error: no such operator"); };
    auto plus = fallback_overloaded {std::plus<>{}, noop};
    // auto minus= overloaded { noop, std::minus<MalInt>{}, std::minus<MalFloat>{}, std::minus<MalString>{} };
    // plus("a",MalUndefined{});
    // plus(1,2);
    // plus(1);

    // e[MalSymbol("+")] = wrap_fn<2>(noop);
    // MalType args = MalList{MalInt(1),MalInt(2)};
    // MalType res = wrap_fn<2>(plus)(args);

    return e;
}
struct FF {
    template <typename A, typename B>
    auto operator()(A a, B b) -> decltype(a+b) {return a+b;}

};
struct empty{};
int main() {
    get_repl_env();
    std::variant<bool, int, std::string> a = false;
    std::variant<bool, int, std::string> b = true;
    auto f = [](auto a, auto b)-> decltype(a+b){ return a;};
    auto h = [](auto a, auto b)-> decltype(auto){ return a+b;};
    auto g =  [](auto&&...){ std::cout << "a" << std::endl;};
    auto fn = fallback_overloaded{
        f,g
    };

    std::string s;
    // std::cout << std::is_invocable_v<FF,bool,empty> << std::endl;
    // std::cout << std::is_invocable_v<decltype(f),bool,empty> << std::endl;
    // std::cout << std::is_invocable_v<decltype(h),bool,empty> << std::endl;
    // std::cout << test_sfinae([]()->decltype(f(0,empty{}){}) << std::endl;
    std::cout << fn(false,false)<< std::endl;
    std::cout << fn(false,0)<< std::endl;
    std::cout << fn(false,s)<< std::endl;
    std::cout << fn(0,false)<< std::endl;
    std::cout << fn(0,0)<< std::endl;
    std::cout << fn(0,s)<< std::endl;
    std::cout << fn(s,false)<< std::endl;
    std::cout << fn(s,0) << std::endl;
    std::cout << fn("","") << std::endl;
    // std::visit(fn,a,b);


    // rep();
}