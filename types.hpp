#ifndef MAL_TYPES_HPP_INCLUDED
#define MAL_TYPES_HPP_INCLUDED
#include <variant>
#include <vector>
#include <cstddef>
#include <cstdint>
#include <string>

class MalType;
using MalNil = std::monostate;
using MalBool = bool;
using MalInt = std::intmax_t;
using MalFloat = double;
using MalString = std::string;
class MalSymbol;
using MalList = std::vector<MalType>;
// using MalMap = std::map<MalType,MalType>;


class MalSymbol : public std::string {
public:
    template <typename ... Ts>
    MalSymbol(Ts...ts): std::string(std::forward<Ts>(ts)...) {}
    MalSymbol(const MalSymbol &) = default;
    MalSymbol(MalSymbol &&) = default;
    // using std::string::operator=;
    std::string str() const { return *static_cast<const std::string*>(this);}
};

using MalTypeVariant =
    std::variant<
        MalNil,
        MalBool,
        MalInt,
        MalFloat,
        MalString,
        MalSymbol,
        MalList
    >;

class MalType : public MalTypeVariant
{
public:
    // using MalTypeVariant::MalTypeVariant;
    template <typename ... Ts>
    MalType(Ts...ts): MalTypeVariant(std::forward<Ts>(ts)...) {}
    MalType(MalType&&) = default;
    MalType(const MalType&) = default;
};

#endif//MAL_TYPES_HPP_INCLUDED