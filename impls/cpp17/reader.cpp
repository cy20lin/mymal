#include <iostream>
#include <string>
#include <regex>
#include <exception>
#include <vector>
#include <algorithm>
#include <cstdint>
#include "reader.hpp"
#include "types.hpp"
#include "cctype"

Reader::Reader(std::string* begin, std::string * end)
    : begin_(begin), end_(end) {}

Reader::Reader(std::string* begin, std::size_t size)
    : begin_(begin), end_(begin+size) {}

std::string * Reader::next() {
    return begin_ == end_ ? nullptr : begin_++;
}

std::string * Reader::peek() {
    return begin_ == end_ ? nullptr : begin_;
}

MalType read_map(Reader & reader) { 
    MalMap result;
    MalMap::value_type pair;
    auto * token = reader.peek();
    if (*token == "{"){
        reader.next();
    } else {
        throw std::runtime_error("read error: unbalanced map, missing '{'");
    }
    while(reader.peek() && *reader.peek() != "}") {
        auto k = read_form(reader);
        if (!reader.peek() || *reader.peek() == "}")
            throw std::runtime_error("read error: map should contains even number of items");
        auto v = read_form(reader);
        result.insert_or_assign(std::move(k), std::move(v));
    }
    token = reader.peek();
    if (*token == "}") {
        reader.next();
        return result;
    } else {
        throw std::runtime_error("read error: unbalanced map, missing '}'");
    }
}

MalType read_list(Reader & reader) { 
    MalList result;
    auto * token = reader.peek();
    if (*token == "("){
        reader.next();
    } else {
        throw std::runtime_error("read error: unbalanced list, missing '('");
    }
    auto i = result.before_begin();
    while(reader.peek() && *reader.peek() != ")") {
        i = result.insert_after(i, read_form(reader));
    }
    token = reader.peek();
    if (token && *token == ")") {
        reader.next();
        return result;
    } else {
        throw std::runtime_error("read error: unbalanced list, missing ')'");
    }
}

MalType read_vector(Reader & reader) { 
    MalVector result;
    auto * token = reader.peek();
    if (*token == "["){
        reader.next();
    } else {
        throw std::runtime_error("read error: unbalanced vector, missing '['");
    }
    while(reader.peek() && *reader.peek() != "]") {
        result.push_back(read_form(reader));
    }
    token = reader.peek();
    if (token && *token == "]") {
        reader.next();
        return result;
    } else {
        throw std::runtime_error("read error: unbalanced vector, missing ']'");
    }
}
std::string from_escaped_string(std::string s){
    std::string out;
    enum { BEGIN, NORMAL, ESCAPE, OCT, HEX, END };
    // for DEBUG use
    // const char *mode_str[] = { "BEGIN", "NORMAL", "ESCAPE", "OCT", "HEX", "END"};
    int mode = BEGIN;
    std::uintmax_t i = 0;
    int digit = 0;
    if (s.empty()) {
        throw std::runtime_error{"empty string literal"};
    }
    std::size_t pos = 0;
    while (pos < s.size()) {
        char c = s[pos];
        // for DEBUG use
        // std::cout << c << ": " << mode_str[mode] << std::endl;
        switch (mode) {
        case BEGIN:
            switch(c) {
            case '\"': mode = NORMAL; ++pos; continue;
            default: 
                throw std::runtime_error{"unbalanced string, missing \" in the begin"};
            }
        case END:
            throw std::runtime_error{"string literal with suffix is not supported"};
        case NORMAL:
            switch(c) {
            case '\"': mode = END; ++pos; continue;
            case '\\': mode = ESCAPE; ++pos; continue;
            default: out.push_back(c); mode = NORMAL; ++pos; continue;
            }
        case ESCAPE:
            switch (c) {
            case '\"': out.push_back('\"'); mode = NORMAL; ++pos; continue;
            case '\'': out.push_back('\''); mode = NORMAL; ++pos; continue;
            case '\\': out.push_back('\\'); mode = NORMAL; ++pos; continue;
            case 'a': out.push_back('\a');  mode = NORMAL; ++pos; continue;
            case 'b': out.push_back('\b');  mode = NORMAL; ++pos; continue;
            case 'f': out.push_back('\f');  mode = NORMAL; ++pos; continue;
            case 'n': out.push_back('\n');  mode = NORMAL; ++pos; continue;
            case 'r': out.push_back('\r');  mode = NORMAL; ++pos; continue;
            case 't': out.push_back('\t');  mode = NORMAL; ++pos; continue;
            case 'v': out.push_back('\v');  mode = NORMAL; ++pos; continue;
            case '?': out.push_back('?');  mode = NORMAL; ++pos; continue;
            case 'x': case 'X': i=0; mode = HEX; ++pos; continue;
            case '0': i = 0; mode = OCT; digit=1; ++pos; continue;
            case '1': i = 1; mode = OCT; digit=1; ++pos; continue;
            case '2': i = 2; mode = OCT; digit=1; ++pos; continue;
            case '3': i = 3; mode = OCT; digit=1; ++pos; continue;
            case '4': i = 4; mode = OCT; digit=1; ++pos; continue;
            case '5': i = 5; mode = OCT; digit=1; ++pos; continue;
            case '6': i = 6; mode = OCT; digit=1; ++pos; continue;
            case '7': i = 7; mode = OCT; digit=1; ++pos; continue;
            default: throw std::runtime_error{"unknown char in escaped sequence"};
            }
        case OCT:
            switch(c) {
            case '0': i = (i<<3) + 0; mode = OCT; ++digit; break;
            case '1': i = (i<<3) + 1; mode = OCT; ++digit; break;
            case '2': i = (i<<3) + 2; mode = OCT; ++digit; break;
            case '3': i = (i<<3) + 3; mode = OCT; ++digit; break;
            case '4': i = (i<<3) + 4; mode = OCT; ++digit; break;
            case '5': i = (i<<3) + 5; mode = OCT; ++digit; break;
            case '6': i = (i<<3) + 6; mode = OCT; ++digit; break;
            case '7': i = (i<<3) + 7; mode = OCT; ++digit; break;
            case '\"': 
                out.push_back(static_cast<char>(i)); 
                i=0;
                mode = END; 
                digit=0;
                ++pos; continue; 
            default: 
                out.push_back(static_cast<char>(i)); 
                i=0;
                mode = NORMAL; 
                digit=0;
                ++pos; continue;
            }
            if (i > 255u) {
                // push current number and re-process 
                // current character since it is out of range
                out.push_back(static_cast<char>(i));
                i=0;
                mode = NORMAL;
                digit=0;
                continue;
            } else if (digit == 3) {
                // if i <= 255 and has 3 digits already
                out.push_back(static_cast<char>(i));
                i=0;
                mode = NORMAL;
                digit=0;
                ++pos; continue;
            } else {
                ++pos; continue;
            }
        case HEX:
            switch(c) {
            case '0': i =(i<<4) + 0; mode = HEX; ++digit; break;
            case '1': i =(i<<4)+ 1; mode = HEX; ++digit; break;
            case '2': i =(i<<4)+ 2; mode = HEX; ++digit; break;
            case '3': i =(i<<4)+ 3; mode = HEX; ++digit; break;
            case '4': i =(i<<4)+ 4; mode = HEX; ++digit; break;
            case '5': i =(i<<4)+ 5; mode = HEX; ++digit; break;
            case '6': i =(i<<4)+ 6; mode = HEX; ++digit; break;
            case '7': i =(i<<4)+ 7; mode = HEX; ++digit; break;
            case '8': i =(i<<4)+ 8; mode = HEX; ++digit; break;
            case '9': i =(i<<4)+ 9; mode = HEX; ++digit; break;
            case 'a': case 'A': i =(i<<4)+ 10; mode = HEX; ++digit; break;
            case 'b': case 'B': i =(i<<4)+ 11; mode = HEX; ++digit; break;
            case 'c': case 'C': i =(i<<4)+ 12; mode = HEX; ++digit; break;
            case 'd': case 'D': i =(i<<4)+ 13; mode = HEX; ++digit; break;
            case 'e': case 'E': i =(i<<4)+ 14; mode = HEX; ++digit; break;
            case 'f': case 'F': i =(i<<4)+ 15; mode = HEX; ++digit; break;
            case '"': 
                out.push_back(static_cast<char>(i)); 
                i=0;
                mode = END; 
                digit = 0;
                ++pos; continue; 
            default: 
                out.push_back(static_cast<char>(i)); 
                i=0;
                mode = NORMAL; 
                digit = 0;
                ++pos; continue;
            }
            if (i > 255u) {
                out.push_back(static_cast<char>(i));
                i=0;
                digit = 0;
                mode = NORMAL;
                // if add one digit make the number over the bound
                // re-processing it
                continue;
            } else {
                ++pos; continue;
            }
        }
    }
    // for DEBUG use
    // std::cout << "(eof): " << mode_str[mode] << std::endl;
    if (mode != END) {
        throw std::runtime_error{"unbalanced string, missing \" in the end"};
    }
    return out;
}

MalType read_atom(Reader & reader) {
    static const auto is_nil = [](std::string s) {
        return s == "nil";
    };
    static const auto is_bool = [](std::string s) {
        return s == "true" || s == "false";
    };
    static const auto is_integer = [](std::string s) {
        auto f = s.front();
        if (!s.empty() && (f == '-' || f == '+'))
            s = s.substr(1);
        return !s.empty() && s.find_first_not_of("0123456789") == std::string::npos;
    };
    static const auto is_float = [](std::string s) {
        ///FIXME: implement the scientific notation
        auto f = s.front();
        if (!s.empty() && (f == '-' || f == '+'))
            s = s.substr(1);
        return !s.empty() && s.find_first_not_of("0123456789.") == std::string::npos;
    };
    static const auto is_string = [](std::string s) {
        return s.front() == '"';
    };
    auto token = reader.peek();
    if (!token) throw std::runtime_error("read error: end of token");
    reader.next();

    if (is_nil(*token)) {
        return MalNil();
    } 
    if (is_bool(*token)) {
        return MalBool(*token != "false");
    } 
    if (is_integer(*token)) {
        return MalInt(std::stoll(*token));
    }
    if (is_float(*token)) {
        return MalFloat(std::stod(*token));
    } 
    if (is_string(*token)) {
        // TODO: parse string literal
        return MalString(from_escaped_string(*token));
    } 
    return MalSymbol(*token);
}

MalType read_form(Reader & reader) {
    MalType result;
    std::string* token = reader.peek();
    if (!token) {
        ///NOTE: Ignore empty input by returning undefined value
        return MalUndefined{};
    } else if (*token == "("){
        result = read_list(reader);
    } else if (*token == "["){
        result = read_vector(reader);
    } else if (*token == "{"){
        result = read_map(reader);
    } else {
        result = read_atom(reader);
    }
    return result;
}

MalType read_str(std::string str) {
    auto tokens = tokenize(str);
    auto reader = Reader(tokens.data(), tokens.size());
    auto result = read_form(reader);
    bool still_has_token = reader.peek();
    if (still_has_token) {
        throw std::runtime_error("read error: tokens not totally consumed");
    }
    return result;
}


std::vector<std::string> tokenize(std::string str) {
    std::vector<std::string> result;
    static const std::regex re(R"raw(~@|[\[\]{}()'`~^@]|"(?:\\.|[^\\"])*"?|;.*|[^\s\[\]{}('"`,;)]+)raw"); 
    std::sregex_token_iterator tokens_begin(str.begin(), str.end(), re, 0), tokens_end;
    std::for_each(tokens_begin, tokens_end,[&](auto token){
        ///filter out the comment
        ///TODO: need a better way to process comment and spaces
        bool is_comment = token.str().front() == ';';
        if (!is_comment)
            result.push_back(token.str());
    });
    ///NOTE: Then check if the non token part is good, i.e. is all space characters
    ///TODO: find a better way to check the space specification,
    ///      and for better error report.
    std::sregex_token_iterator spaces_begin(str.begin(), str.end(), re, -1), spaces_end;
    std::for_each(spaces_begin, spaces_end,[&](auto space){
        std::string space_ = space.str();
        bool good = std::all_of(space_.begin(), space_.end(), [](char c) {
            return std::isspace(c) || c == ',';
        });
        if (!good) {
            throw std::runtime_error("tokenize error: space error");
        }
    });
    return result;
}