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
    MalSymbol(Ts&&...ts): std::string(std::forward<Ts>(ts)...) {}
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
    template <std::size_t N> auto get() {return std::get<N>(variant()); }
    template <typename T> auto get() {return std::get<T>(variant()); }
    template <std::size_t N> auto* get_if() {return std::get_if<N>(&variant()); }
    template <typename T> auto* get_if() {return std::get_if<T>(&variant()); }
};

inline std::vector<MalType> to_vector_args(MalType args) {
    std::vector<MalType> result;
    if (auto *p = std::get_if<MalList>(&args.variant())) {
        std::copy(p->begin(), p->end(), std::back_inserter(result));
    } 
    else if (auto *p = std::get_if<MalVector>(&args.variant())) {
        result = *p;
    }
    return result;
    throw std::logic_error("eval error: invalid arguments type");
};

template <std::size_t N>
auto to_array_args(MalType args) -> std::array<MalType, N> {
    std::array<MalType, N> result;
    if (auto *p = std::get_if<MalList>(&args.variant())) {
        std::size_t i = 0;
        std::copy_if(p->begin(), p->end(), result.begin(), [&i](auto&&) { return i++ < N; });
        if (i  != N) {
            throw std::logic_error("eval error: invalid arguments size " + std::to_string(i) + " expected " + std::to_string(N));
        }
    }
    else if (auto *p = std::get_if<MalVector>(&args.variant())) {
        std::size_t i = 0;
        std::copy_if(p->begin(), p->end(), result.begin(), [&i](auto&&) { return i++ < N; });
        if (i != N) {
            throw std::logic_error("eval error: invalid arguments size " + std::to_string(i) + " expected " + std::to_string(N));
        }
    }
    return result;
    throw std::logic_error("eval error: invalid arguments type");
};

template<std::size_t N, typename F>
MalFunction wrap_fn(F fn) {
    return [fn](MalType args) -> MalType { 
        std::array<MalType, N> array_args = to_array_args<N>(args);
        return std::apply([&fn](auto ... args) {
            return std::visit(fn, args.variant()...);
        }, array_args);
    };
}

#endif//MAL_TYPES_HPP_INCLUDED