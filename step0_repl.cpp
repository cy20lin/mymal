#include <iostream>
#include <string>


std::string readline() {
    std::string line;
    std::getline(std::cin, line, '\n');
    return line;
}

std::string READ() {
    return readline();
}

std::string EVAL(std::string expr) {
    std::string value = expr;
    return value;
}

void printline(std::string value) {
    std::cout << value << std::endl;
}

void PRINT(std::string value) {
    static const std::string prefix = "user>";
    printline(prefix + value);
}

void rep() {
    while(true) {
        std::string expr = READ();
        if (!std::cin) break;
        std::string value = EVAL(expr);
        PRINT(value);
    }
}

int main() {
    rep();
    return 0;
}