#ifndef MAL_PRINTER_HPP_INCLUDED
#define MAL_PRINTER_HPP_INCLUDED
#include "types.hpp"
#include <string>

std::string pr_str(const MalType & value, bool print_readably = true);
#endif//MAL_PRINTER_HPP_INCLUDED