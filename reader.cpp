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
MalType read_list(Reader & reader);
MalType read_atom(Reader & reader);
MalType read_form(Reader & reader);
MalType read_list(Reader & reader) { 
    MalList result;
    auto * token = reader.peek();
    if (*token == "("){
        reader.next();
    } else {
        throw std::runtime_error("read error: invalid list begin, missing '('");
    }
    while(reader.peek() && *reader.peek() != ")") {
        result.push_back(read_form(reader));
    }
    token = reader.peek();
    if (*token == ")") {
        reader.next();
        return result;
    } else {
        throw std::runtime_error("read error: invalid list end, missing ')'");
    }
}

MalType read_atom(Reader & reader) {
    static const auto is_nil = [](std::string s) {
        return s == "nil";
    };
    static const auto is_bool = [](std::string s) {
        return s == "true" || s == "false";
    };
    static const auto is_integer = [](std::string s) {
        return !s.empty() && s.find_first_not_of("0123456789") == std::string::npos;
    };
    static const auto is_float = [](std::string s) {
        return !s.empty() && s.find_first_not_of("0123456789.") == std::string::npos;
    };
    static const auto is_string = [](std::string s) {
        return s.front() == '"' && s.back() == '"';
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
    if (is_float(*token)) {
        return MalFloat(std::stod(*token));
    } 
    if (is_integer(*token)) {
        return MalInt(std::stoll(*token));
    } 
    if (is_string(*token)) {
        // TODO: parse string literal
        return MalString(token->substr(1,token->size()-1));
    } 
    return MalSymbol(*token);
}

MalType read_form(Reader & reader) {
    MalType result;
    std::string* token = reader.peek();
    if (!token) {
        throw std::runtime_error("read error: empty input");
    } else if (*token == "("){
        result = read_list(reader);
    } else {
        result = read_atom(reader);
    }
    bool still_has_token = reader.peek();
    if (still_has_token) {
        throw std::runtime_error("read error: tokens not totally consumed");
    }
    return result;
}

MalType read_str(std::string str) {
    auto tokens = tokenize(str);
    auto reader = Reader(tokens.data(), tokens.size());
    return read_form(reader);
}


std::vector<std::string> tokenize(std::string str) {
    std::vector<std::string> result;
    static const std::regex re(R"raw(~@|[\[\]{}()'`~^@]|"(?:\\.|[^\\"])*"|;.*|[^\s\[\]{}('"`,;)]+)raw"); 
    std::sregex_token_iterator tokens_begin(str.begin(), str.end(), re, 0), tokens_end;
    std::for_each(tokens_begin, tokens_end,[&](auto token){
        result.push_back(token.str());
    });
    // TODO: find a better way to check the space specification,
    //       and for better error report.
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