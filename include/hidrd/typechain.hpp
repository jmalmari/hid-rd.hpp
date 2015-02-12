#ifndef TYPECHAIN_HPP
#define TYPECHAIN_HPP

#include <cstddef>

namespace hidrd { namespace typechain {

template <typename T> struct Root
{
    using ChildValue = T;
};

template <typename P, typename P::ChildValue value, typename C = void> struct Node
{
    using ParentNode = P;
    using ChildValue = C;
    constexpr static const auto Value = value;
};


// Get number of nodes
template <typename T> struct NodeCount
{
    constexpr static const std::size_t Result = 1 +
        NodeCount<typename T::ParentNode>::Result;
};
template <typename T> struct NodeCount<Root<T>>
{
    constexpr static const std::size_t Result = 0;
};

}}

#endif
