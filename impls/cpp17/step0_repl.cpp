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

std::string READ(std::string str) {
    return str;
}

std::string EVAL(std::string expr) {
    std::string value = expr;
    return value;
}

std::string PRINT(std::string value) {
    return value;
}

std::string rep(std::string str) {
    std::string expr = READ(str);
    std::string value = EVAL(expr);
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