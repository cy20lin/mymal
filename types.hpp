#ifndef MAL_TYPES_HPP_INCLUDED
#define MAL_TYPES_HPP_INCLUDED
#include <exception>
#include <variant>
#include <cstddef>
#include <cstdint>
#include <string>
#include <forward_list>
#include <vector>
#include <map>
#include <functional>




class MalType;

using MalUndefined = std::monostate;

class MalNil {
public:
    MalNil() = default;
    MalNil(const MalNil &) = default;
    MalNil & operator=(const MalNil &) = default;
    void* data_;
};
constexpr bool operator!=(MalNil,MalNil) noexcept { return false; }
constexpr bool operator<(MalNil,MalNil) noexcept { return false; }
constexpr bool operator>(MalNil,MalNil) noexcept { return false; }
constexpr bool operator==(MalNil,MalNil) noexcept { return true; }
constexpr bool operator<=(MalNil,MalNil) noexcept { return true; }
constexpr bool operator>=(MalNil,MalNil) noexcept { return true; }

//TODO: Do we need cons data type ?
using MalBool = bool;
using MalInt = std::intmax_t;
using MalFloat = double;
using MalString = std::string;
class MalSymbol;
using MalList = std::forward_list<MalType>;
using MalVector = std::vector<MalType>;
using MalMap = std::map<MalType,MalType>;
// using MalFunction = std::function<MalType(MalType)>;

class NotImplementedException : public std::logic_error
{
public:
    NotImplementedException () : std::logic_error{"function not implemented"} {}
};
using MalFunctionBase = std::function<MalType(MalType)>;
struct MalFunction : MalFunctionBase {
    using MalFunctionBase::MalFunctionBase;
    MalFunction(const MalFunction&)=default;
    MalFunction(MalFunction&&)=default;
    MalFunction& operator=(const MalFunction&)=default;
    MalFunction& operator=(MalFunction&&)=default;
    bool operator<(const MalFunction&) const { throw NotImplementedException{}; }
    bool operator>(const MalFunction&) const { throw NotImplementedException{}; }
    bool operator<=(const MalFunction&) const { throw NotImplementedException{}; }
    bool operator>=(const MalFunction&) const { throw NotImplementedException{}; }
};

class MalSymbol : public std::string {
public:
    template <typename ... Ts>
    MalSymbol(Ts...ts): std::string(std::forward<Ts>(ts)...) {}
    MalSymbol(const MalSymbol &) = default;
    MalSymbol(MalSymbol &&) = default;
    // using std::string::operator=;
    std::string str() const { return *static_cast<const std::string*>(this);}
    MalSymbol & operator=(const MalSymbol &) = default;
    MalSymbol & operator=(MalSymbol &&) = default;
};

using MalTypeVariant =
    std::variant<
        MalUndefined,
        MalNil,
        MalBool,
        MalInt,
        MalFloat,
        MalString,
        MalSymbol,
        MalList,
        MalVector,
        MalMap,
        MalFunction
    >;

class MalType : public MalTypeVariant
{
public:
    // using MalTypeVariant::MalTypeVariant;
    template <typename ... Ts>
    MalType(Ts...ts): MalTypeVariant(std::forward<Ts>(ts)...) {}
    MalType(MalType&&) = default;
    MalType(const MalType&) = default;
    MalType& operator=(const MalType &) = default;
    MalType& operator=(MalType &&) = default;
    MalTypeVariant & variant() & {return *this;}
    MalTypeVariant && variant() && { return static_cast<MalTypeVariant&&>(*this);}
    const MalTypeVariant & variant() const & { return *this;}
    const MalTypeVariant && variant() const && {return static_cast<const MalTypeVariant&&>(*this);}
    template <std::size_t N> decltype(auto) get() {return std::get<N>(variant()); }
    template <typename T> decltype(auto) get() {return std::get<T>(variant()); }
    template <std::size_t N> decltype(auto) get_if() {return std::get_if<N>(variant()); }
    template <typename T> decltype(auto) get_if() {return std::get_if<T>(variant()); }
};

#endif//MAL_TYPES_HPP_INCLUDED