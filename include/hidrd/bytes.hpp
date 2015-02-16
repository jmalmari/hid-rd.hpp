#ifndef BYTES_HPP
#define BYTES_HPP

#include <cstddef>

#ifndef BYTES_USE_CSTYLE_ARRAY
#include <array>
#endif

namespace hidrd { namespace bytes {

typedef unsigned char ByteType;

template <ByteType data> struct Byte
{
    constexpr static const ByteType Data = data;
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

template <ByteType lhs, typename... Rhs>
struct Bytes<Byte<lhs>, Rhs...>
{
    typedef typename Bytes<_flat<Byte<lhs> >,
                           Rhs... >::Flatten Flatten;
};

// Inner Bytes specializations, populating _flat<> from right hand args

// append another Bytes<> list
template <ByteType... arg1, typename... BytesValue, typename... Rhs>
struct Bytes<_flat<Byte<arg1>...>, Bytes<BytesValue...>, Rhs...>
{
    typedef typename Bytes<
        _flat<Byte<arg1>...>,
        BytesValue...,
        Rhs... >::Flatten Flatten;
};

// append a single Byte<>
template <ByteType... arg1, ByteType arg2, typename... Rhs>
struct Bytes<_flat<Byte<arg1>...>, Byte<arg2>, Rhs...>
{
    typedef typename Bytes<
        _flat<Byte<arg1>..., Byte<arg2> >,
        Rhs... >::Flatten Flatten;
};

// end of Bytes recursion: single _flat<>
template <ByteType... args> struct Bytes<_flat<Byte<args>...>>
{
    typedef Bytes<_flat<Byte<args>...>> Flatten;

    constexpr static const std::size_t Size = sizeof...(args);

#ifdef BYTES_USE_CSTYLE_ARRAY
    constexpr static const ByteType data[] = { args... };
#else
    constexpr static const std::array<ByteType, sizeof...(args)> data = { args... };
#endif
};

#ifdef BYTES_USE_CSTYLE_ARRAY
template <ByteType... args> constexpr const ByteType Bytes<_flat<Byte<args>...>>::data[];
#else
template <ByteType... args> constexpr const std::array<ByteType, sizeof...(args)>
    Bytes<_flat<Byte<args>...>>::data;
#endif

}}

#endif
