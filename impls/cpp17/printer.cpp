#include "printer.hpp"
#include <iostream>
#include <sstream>
#include <type_traits>
#include <cmath>

std::string to_escaped_string(std::string s) {
    char oct[] = "01234567";
    std::string out;
    out.push_back('\"');
    for (auto c : s) {
        switch (c) {
        // case '\'': out += "\\\'"; continue;
        case '\'': out += "\'"; continue;
        case '\"': out += "\\\""; continue;
        case '\\': out += "\\\\"; continue;
        case '\a': out += "\\a"; continue;
        case '\b': out += "\\b"; continue;
        case '\f': out += "\\f"; continue;
        case '\n': out += "\\n"; continue;
        case '\r': out += "\\r"; continue;
        case '\t': out += "\\t"; continue;
        case '\v': out += "\\v"; continue;
        }
        if (std::isprint(c)) {
            out.push_back(c);
        } else {
            unsigned char a = c;
            char data[]= {
                '\\',
                oct[a / 8u / 8u],
                oct[a / 8u % 8u],
                oct[a % 8u],
                '\0'};
            out += data;
        }
        
    }
    out.push_back('\"');
    return out;
}

std::string pr_str(const MalType & value, bool print_readably) {
    const auto visitor = [](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        std::stringstream out;
        if constexpr (std::is_same_v<T, MalNil>) {
            out << "nil" << std::flush;
        }
        else if constexpr (std::is_same_v<T, MalBool>) {
            out << std::boolalpha << arg << std::flush;
        }
        else if constexpr (std::is_same_v<T, MalInt>){
            out << arg << std::flush;
        }
        else if constexpr (std::is_same_v<T, MalFloat>){
            out << arg;
            bool is_integer = (std::floor(arg) == arg);
            if (is_integer) {
                out << "."; // with extra "." in the end to disinguish between int(2)
            }
            out << std::flush;
        }
        else if constexpr (std::is_same_v<T, MalSymbol>) {
            out <<  arg.str() << std::flush;
        }
        else if constexpr (std::is_same_v<T, MalString>){
            // FIXME: escape some special character, eg. '\n' '\\'
            // out << "\"" << arg << "\"" << std::flush;
            out << to_escaped_string(arg) << std::flush;
        }
        else if constexpr (std::is_same_v<T, MalList>) {
            if (arg.empty()) {
                out << "()" << std::flush;
            } else {
                out << "(";
                auto i = arg.begin();
                while (i != arg.end()) {
                    out << pr_str(*i);
                    ++i;
                    out << (i != arg.end() ? " " : ")");
                }
            }
        } 
        else if constexpr (std::is_same_v<T, MalVector>) {
            if (arg.empty()) {
                out << "[]" << std::flush;
            } else {
                out << "[";
                for (auto & item : arg) {
                    out << pr_str(item);
                    out << (&item != &arg.back() ? " " : "]");
                    out << std::flush;
                }
            }
        } 
        else if constexpr (std::is_same_v<T, MalMap>) {
            if (arg.empty()) {
                out << "{}" << std::flush;
            } else {
                std::size_t n = arg.size();
                out << "{";
                std::size_t i = 0;
                for (auto & item : arg) {
                    out << pr_str(item.first);
                    out << " ";
                    out << pr_str(item.second);
                    out << (++i < n ? " " : "}");
                    out << std::flush;
                }
            }
        } 
        else if constexpr (std::is_same_v<T, MalFunction>) {
            out << "#<function:" << std::hex << arg.target_type().hash_code() << ">" << std::flush;
        }
        else {
            static_assert("no such type");
        }
        return out.str();
    };
    // https://stackoverflow.com/questions/51309467/using-stdvisit-on-a-class-inheriting-from-stdvariant-libstdc-vs-libc
    return std::visit(visitor, static_cast<const MalTypeVariant&>(value));

}


