#include "reader.hpp"
#include "types.hpp"
#include "printer.hpp"
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

MalType READ(std::string str) {
    return read_str(str);
}

MalType EVAL(MalType expr) {
    MalType value = expr;
    return value;
}

std::string PRINT(MalType value) {
    return pr_str(value);
}

std::string rep(std::string str) {
    MalType expr = READ(str);
    MalType value = EVAL(expr);
    std::string ostr = PRINT(value);
    return ostr;
}

int main() {
    const char* prompt = "user> ";
    bool ok = true;
    while (true) {
        std::cout << prompt << std::flush;
        std::string input = readline(ok);
        if (!ok)
            break;
        try {
            std::string output = rep(input);
            printline(output);
        } catch (std::exception & e) {
            printline(e.what(), &std::cerr);
        }
    }
    return 0;
}