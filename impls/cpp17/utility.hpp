#ifndef MAL_UTILITY_HPP_INCLUDED
#define MAL_UTILITY_HPP_INCLUDED

#include <cstdint>
#include <utility>
#include <type_traits>


template <class Lambda, class... Ts>
constexpr auto test_sfinae(Lambda lambda, Ts&&...) 
    -> decltype(lambda(std::declval<Ts>()...), bool{}) { return true; }
constexpr bool test_sfinae(...)  { return false; }
// template <class T>
// constexpr bool bar(T var) {
//     if constexpr(test_sfinae([](auto v) -> decltype(v.foo()){}, var))
//        return true;
//     return false;
// }


namespace detail {


template <typename...>
struct is_preceding_active;

template <std::size_t n, bool ... v, std::size_t ...i>
struct is_preceding_active<
    std::integral_constant<std::size_t, n>, // 
    std::integer_sequence<bool, v...>,
    std::integer_sequence<std::size_t, i...>
> : std::integral_constant<bool,(false||...||((i < n) && v))>
{};

template <typename...>
struct preceding_active_sequence ;

template <bool ... v, std::size_t ...i>
struct preceding_active_sequence<
    std::integer_sequence<bool, v...>,
    std::integer_sequence<std::size_t, i...>
> {
    using type = std::integer_sequence<bool,
        is_preceding_active<
            std::integral_constant<std::size_t, i>, 
            std::integer_sequence<bool, v...>,
            std::integer_sequence<std::size_t, i...>
        >::value...>;
};

template <typename...>
struct fallback_active_sequence ;

template <bool ... v, bool ...p>
struct fallback_active_sequence<
    std::integer_sequence<bool, v...>, // invokable_seq
    std::integer_sequence<bool, p...>  // is_preceding_active_seq
> {
    using type = std::integer_sequence<bool, (v && !p)...>;
};

template <typename ...>
struct fallback_select_type ;

template <bool ...f,typename ...Ts>
struct fallback_select_type <
    std::integer_sequence<bool, f...>, // fallback_active_seq
    Ts...
> : std::enable_if<f, Ts>... {};

template <typename ...>
struct fallback_select_index;

template <bool ...f, std::size_t...i>
struct fallback_select_index <
    std::integer_sequence<bool, f...>, // fallback_active_seq
    std::integer_sequence<std::size_t, i...> // indices
> : std::integral_constant<std::size_t, (...+(f?i:0))> {};

template <typename ...Ts> struct types {};

template <
    typename OverloadedTypeSeq,
    typename ArgumentTypeSeq,
    bool add_lref = false,
    bool add_rref = false,
    bool add_const = false,
    bool add_volatile = false
> struct get_active_type ;

template <
    typename ...Ts, 
    typename ...Args, 
    bool add_lref,
    bool add_rref,
    bool add_const,
    bool add_volatile
>
struct get_active_type<
    types<Ts...>, // overloaded functors
    types<Args...>,// argument types
    add_lref,
    add_rref,
    add_const,
    add_volatile
>
{
    using invocable_seq = std::integer_sequence<bool, std::is_invocable_v<Ts, Args...>...>;
    using indices = std::make_index_sequence<sizeof...(Ts)>;
    using preceding_active_seq = typename detail::preceding_active_sequence<invocable_seq, indices>::type;
    using fallback_active_seq = typename detail::fallback_active_sequence<invocable_seq, preceding_active_seq>::type;
    using active_type =  typename fallback_select_type<fallback_active_seq, Ts...>::type;
    using active_index =  typename fallback_select_index<fallback_active_seq, indices>::type;
    using add_l_type = std::conditional_t<add_lref, std::add_rvalue_reference_t<active_type>, active_type>;
    using add_lr_type = std::conditional_t<add_rref, std::add_rvalue_reference_t<add_l_type>, add_l_type>;
    using add_lrc_type = std::conditional_t<add_const, std::add_const_t<add_lr_type>, add_lr_type>;
    using add_lrcv_type = std::conditional_t<add_volatile, std::add_volatile_t<add_lrc_type>, add_lrc_type>;
    using type = add_lrcv_type;
    using index = active_index;
};

template <
    typename OverloadedTypeSeq,
    typename ArgumentTypeSeq,
    bool add_lref=false,
    bool add_rref=false,
    bool add_const=false,
    bool add_volatile=false
>
using get_active_type_t = typename get_active_type<OverloadedTypeSeq,ArgumentTypeSeq,add_lref,add_rref,add_const,add_volatile>::type;

}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; }; // (1)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;  // (2)

// // simple implementation, this will ffail if Ts is not unique
// template <typename ...Ts>
// struct fallback_overloaded: Ts...
// {
//     template <typename ...Args> 
//     decltype(auto) operator() (Args&&...args) {
//         using type = detail::get_active_type_t<
//             detail::types<Ts...>,
//             detail::types<Args...>,
//             false,false,false,false>;
//         // return static_cast<type&>(o)(std::forward<Args>(args)...);
//         return type::operator()(std::forward<Args>(args)...);
//     }
// }; 

template <typename ...Ts>
struct fallback_overloaded: std::tuple<Ts...>
{
    using std::tuple<Ts...>::tuple;
    template <typename ...Args> 
    decltype(auto) operator() (Args&&...args)  {
        using active = detail::get_active_type<
            detail::types<Ts...>,
            detail::types<Args...>,
            false,false,false,false>;
        using index_type = typename active::index;
        using type = typename active::type;
        return static_cast<type&>(std::get<index_type::value>(*this))(std::forward<Args>(args)...);
        // return index_type::value; for debug
    }
}; 
template<class... Ts> fallback_overloaded(Ts...) -> fallback_overloaded<Ts...>;  // (2)


#endif//MAL_UTILITY_HPP_INCLUDED