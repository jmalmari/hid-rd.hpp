#ifndef BYTES_HPP
#define BYTES_HPP

#include <array>

namespace hidrd { namespace bytes {

template <uint8_t data> struct Byte
{
    constexpr static const uint8_t Data = data;
};

template <typename...> struct Bytes;

// tag type that wraps already verified flat bytes
template <typename...> struct _flat;


// Entry point specializations, not requiring _flat<> in the beginning

template <typename... Inner, typename... Rhs>
struct Bytes<Bytes<Inner...>, Rhs...>
{
    typedef typename Bytes<Inner..., Rhs...>::Flatten Flatten;
};

template <uint8_t lhs, typename... Rhs>
struct Bytes<Byte<lhs>, Rhs...>
{
    typedef typename Bytes<_flat<Byte<lhs> >,
                           Rhs... >::Flatten Flatten;
};

// Inner Bytes specializations, populating _flat<> from right hand args

// append another Bytes<> list
template <uint8_t... arg1, typename... BytesValue, typename... Rhs>
struct Bytes<_flat<Byte<arg1>...>, Bytes<BytesValue...>, Rhs...>
{
    typedef typename Bytes<
        _flat<Byte<arg1>...>,
        BytesValue...,
        Rhs... >::Flatten Flatten;
};

// append a single Byte<>
template <uint8_t... arg1, uint8_t arg2, typename... Rhs>
struct Bytes<_flat<Byte<arg1>...>, Byte<arg2>, Rhs...>
{
    typedef typename Bytes<
        _flat<Byte<arg1>..., Byte<arg2> >,
        Rhs... >::Flatten Flatten;
};

// end of Bytes recursion: single _flat<>
template <uint8_t... args> struct Bytes<_flat<Byte<args>...>>
{
    typedef Bytes<_flat<Byte<args>...>> Flatten;
    constexpr static const std::array<uint8_t, sizeof...(args)> data = { args... };
};

template <uint8_t... args> constexpr const std::array<uint8_t, sizeof...(args)>
Bytes<_flat<Byte<args>...>>::data;

}}

#endif
