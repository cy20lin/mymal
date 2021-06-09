#ifndef MAL_READER_HPP_INCLUDED
#define MAL_READER_HPP_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <vector>
#include "types.hpp"

class Reader {
public:
    Reader() = default;
    Reader(const Reader &) = default;
    Reader(std::string* begin, std::string * end);
    Reader(std::string* begin, std::size_t size);
    std::string * next();
    std::string * peek();
private:
    std::string * begin_;
    std::string * end_;
};

MalType read_str(std::string);
std::vector<std::string> tokenize(std::string str);

#endif//MAL_READER_HPP_INCLUDED