#include <iostream>
#include <string>
#include "reader.hpp"
#include "types.hpp"
#include "printer.hpp"

std::string readline() {
    std::string line;
    std::getline(std::cin, line, '\n');
    return line;
}

MalType READ() {
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
    static const std::string prefix = "user>";
    printline(prefix + pr_str(value));
}

void rep() {
    while(true) {
        auto expr = READ();
        if (!std::cin) break;
        auto value = EVAL(expr);
        PRINT(value);
    }
}
int main() {
    rep();
}