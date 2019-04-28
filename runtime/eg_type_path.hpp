
#ifndef EG_TYPE_PATH
#define EG_TYPE_PATH

#include "eg_common.hpp"

template< typename... Ts >
struct [[clang::eg_type( eg::id_TypePath )]] __eg_type_path{};

namespace eg
{
    //https://stackoverflow.com/questions/19838965/how-can-i-use-variadic-templates-to-flatten-a-tree-of-types

    // first parameter - accumulator
    // second parameter - input __eg_type_path
    template <class T, class U>
    struct CanonicaliseTypePath_impl;

    // first case - the head of the __eg_type_path is __eg_type_path too
    // expand this __eg_type_path and continue
    template <class... Ts, class... Heads, class... Tail>
    struct CanonicaliseTypePath_impl< __eg_type_path<Ts...>, __eg_type_path< __eg_type_path<Heads...>, Tail...> > 
    {
        using Type = typename CanonicaliseTypePath_impl< __eg_type_path<Ts...>, __eg_type_path<Heads..., Tail...> >::Type;
    };

    // second case - the head of the __eg_type_path is not a __eg_type_path
    // append it to our new, flattened __eg_type_path
    template <class... Ts, class Head, class... Tail>
    struct CanonicaliseTypePath_impl<__eg_type_path<Ts...>, __eg_type_path<Head, Tail...>> 
    {
        using Type = typename CanonicaliseTypePath_impl< __eg_type_path<Ts..., Head>, __eg_type_path<Tail...> >::Type;
    };

    // base case - input __eg_type_path is empty
    // return our flattened __eg_type_path
    template <class... Ts>
    struct CanonicaliseTypePath_impl< __eg_type_path<Ts...>, __eg_type_path<> > 
    {
        using Type = __eg_type_path<Ts...>;
    };

    template< typename T >
    struct CanonicaliseTypePath
    {
        using Type = __eg_type_path< T >;
    };

    template< typename... Ts >
    struct CanonicaliseTypePath< __eg_type_path< Ts... > >
    {
        using Type = typename CanonicaliseTypePath_impl< __eg_type_path<>, __eg_type_path< Ts... > >::Type;
    };

}

#endif //EG_TYPE_PATH