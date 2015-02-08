#ifndef HIDRD_HPP
#define HIDRD_HPP

#include <cstdint>
#include "bytes.hpp"
#include "typechain.hpp"

namespace hidrd {


namespace itemdata {

// This namespace contains strongly typed values retrieved from HID
// specifications.

enum DataSize
{
    NoData = 0x00,
    OneByte = 0x01,
    TwoBytes = 0x02,
    FourBytes = 0x03,
};

enum ShortItemType
{
    MainItem = 0,               // Input, Output, Feature, Collection
    GlobalItem = 1,             // Usage Page, Report Count, ...
    LocalItem = 2,              // Usage, Usage Minimum, ...
                                // Local items are used until next main item
};

namespace tags {

enum Main
{
    Input = 0x08,
    Output = 0x09,
    Feature = 0x0B,
    Collection = 0x0A,
    EndCollection = 0x0C,
};

enum Global
{
    UsagePage = 0x00,
    LogicalMinimum = 0x01,
    LogicalMaximum = 0x02,
    PhysicalMinimum = 0x03,
    PhysicalMaximum = 0x04,
    ReportSize = 0x07,
    ReportId = 0x08,
    ReportCount = 0x09,
};

enum Local
{
    Usage = 0x00,
    UsageMinimum = 0x01,
    UsageMaximum = 0x02,
};


} namespace args {


enum UsagePage
{
    GenericDesktop = 0x01,
    Button = 0x09,
};

enum GenericDesktopUsage
{
    Pointer = 0x01,             // Physical
    Mouse = 0x02,               // Application
    X = 0x30,                   // Dynamic Value
    Y = 0x31,                   // Dynamic Value
    Z = 0x32,                   // Dynamic Value
    Rx = 0x33,                  // Dynamic Value
    Ry = 0x34,                  // Dynamic Value
    Rz = 0x35,                  // Dynamic Value
};

enum Collection
{
    Physical = 0x00,
    Application = 0x01,
    Logical = 0x02,
    Report = 0x03,
    NamedArray = 0x04,
    UsageSwitch = 0x05,
    UsageModifier = 0x06,
};

// Mostly valid for Input, Output, and Feature item data
enum ReportBit
{
    Constant      = (1 << 0),
    Variable      = (1 << 1),
    Relative      = (1 << 2),
    Wrap          = (1 << 3),
    NonLinear     = (1 << 4),
    NoPreferred   = (1 << 5),
    NullState     = (1 << 6),
    Volatile      = (1 << 7), // not used for input reports
    BufferedBytes = (1 << 8),
};

} // end args

} // end namespace itemdata


template <typename Size, typename Type, typename Tag>
constexpr uint8_t make_hid_item_hdr(Size size, Type type, Tag tag)
{
    return ((tag << 4) | (type << 2) | size);
}

// These are provided as a convenience for creating item header bytes directly.
constexpr uint8_t item_hdr_main(itemdata::tags::Main tag, itemdata::DataSize size)
{
    return make_hid_item_hdr(size, itemdata::MainItem, tag);
}
constexpr uint8_t item_hdr_global(itemdata::tags::Global tag, itemdata::DataSize size)
{
    return make_hid_item_hdr(size, itemdata::GlobalItem, tag);
}
constexpr uint8_t item_hdr_local(itemdata::tags::Local tag, itemdata::DataSize size)
{
    return make_hid_item_hdr(size, itemdata::LocalItem, tag);
}


// ValueSerializer makes a byte list from numeric data. Numeric data
// is represented by a generic type that has a Value data member.
template <itemdata::DataSize size, typename T> struct ValueSerializer;
template <typename T> struct ValueSerializer<itemdata::NoData, T>
{
    typedef Bytes<> Result;
};
template <typename T> struct ValueSerializer<itemdata::OneByte, T>
{
    typedef Byte<T::Value> Result;
};
template <typename T> struct ValueSerializer<itemdata::TwoBytes, T>
{
    typedef Bytes<Byte<(T::Value & 0xff)>,
                  Byte<(T::Value >> 8)>> Result;
};
template <typename T> struct ValueSerializer<itemdata::FourBytes, T>
{
    typedef Bytes<Byte<(0xff & (T::Value))>,
                  Byte<(0xff & (T::Value >> 8))>,
                  Byte<(0xff & (T::Value >> 16))>,
                  Byte<(0xff & (T::Value >> 24))>
        > Result;
};



namespace tc = typechain;

struct EmptyDataNode;

template <typename Leaf, int depth = tc::NodeCount<Leaf>::Result> struct Item;
template <typename Leaf> struct Item<Leaf, 3>
{
    using DataNode = Leaf;
    using TagNode = typename DataNode::ParentNode;
    using TypeNode = typename TagNode::ParentNode;
};
template <typename Leaf> struct Item<Leaf, 2>
{
    using DataNode = EmptyDataNode;
    using TagNode = Leaf;
    using TypeNode = typename TagNode::ParentNode;
};

template <typename T> struct DataNodeSize
{
    // going with the assumption we're always using unsigned numbers
    constexpr static const itemdata::DataSize Result =
        T::Value < 0x100 ? itemdata::OneByte : (
            T::Value < 0x10000 ? itemdata::TwoBytes : itemdata::FourBytes);
};
template <> struct DataNodeSize<EmptyDataNode>
{
    constexpr static const itemdata::DataSize Result = itemdata::NoData;
};

template <typename T> struct ItemSerializer
{
    constexpr static const itemdata::DataSize DataSize =
        DataNodeSize<typename T::DataNode>::Result;

    typedef Bytes<

        Byte<make_hid_item_hdr(DataSize,
                               T::TypeNode::Value,
                               T::TagNode::Value)>,

        typename ValueSerializer<DataSize, typename T::DataNode>::Result

        > Result;
};


template <typename... Items> struct Descriptor :
    public Bytes<
        typename ItemSerializer< Item<Items> >::Result...
    >::Flatten
{
};


using Root = tc::Root<itemdata::ShortItemType>;


using MainItem = tc::Node<Root, itemdata::MainItem, itemdata::tags::Main>;
using GlobalItem = tc::Node<Root, itemdata::GlobalItem, itemdata::tags::Global>;
using LocalItem = tc::Node<Root, itemdata::LocalItem, itemdata::tags::Local>;


using CollectionTag = tc::Node<MainItem,
                               itemdata::tags::Collection,
                               itemdata::args::Collection>;
using EndCollectionTag = tc::Node<MainItem, itemdata::tags::EndCollection>;
using InputTag = tc::Node<MainItem, itemdata::tags::Input, int>;

using UsagePageTag = tc::Node<GlobalItem,
                              itemdata::tags::UsagePage,
                              itemdata::args::UsagePage>;
using LogicalMinimumTag = tc::Node<GlobalItem, itemdata::tags::LogicalMinimum, int>;
using LogicalMaximumTag = tc::Node<GlobalItem, itemdata::tags::LogicalMaximum, int>;
using ReportIdTag = tc::Node<GlobalItem, itemdata::tags::ReportId, uint8_t>;
using ReportCountTag = tc::Node<GlobalItem, itemdata::tags::ReportCount, std::size_t>;
using ReportSizeTag = tc::Node<GlobalItem, itemdata::tags::ReportSize, std::size_t>;

using GenericDesktopTag = tc::Node<LocalItem,
                                   itemdata::tags::Usage,
                                   itemdata::args::GenericDesktopUsage>;
using UsageMinimumTag = tc::Node<LocalItem, itemdata::tags::UsageMinimum, int>;
using UsageMaximumTag = tc::Node<LocalItem, itemdata::tags::UsageMaximum, int>;



// For report main items (input, feature, output)
template <typename... Values> struct BitwiseOr;
template <typename Car, typename... Cdr> struct BitwiseOr<Car, Cdr...>
{
    constexpr static const unsigned int result = Car::value | BitwiseOr<Cdr...>::result;
};
template <> struct BitwiseOr<>
{
    constexpr static const unsigned int result = 0;
};
template <int d> struct ValueHolder
{
    constexpr static const int value = d;
};


// Main items
template <CollectionTag::ChildValue value> using Collection =
    tc::Node<CollectionTag, value>;
using EndCollection = EndCollectionTag;
template <itemdata::args::ReportBit... d>
using Input = tc::Node<InputTag, BitwiseOr<ValueHolder<d>...>::result>;

// Global items
template <UsagePageTag::ChildValue value> using UsagePage = tc::Node<UsagePageTag, value>;
template <int value> using LogicalMinimum = tc::Node<LogicalMinimumTag, value>;
template <int value> using LogicalMaximum = tc::Node<LogicalMaximumTag, value>;
template <ReportIdTag::ChildValue value> using ReportId = tc::Node<ReportIdTag, value>;
template <ReportCountTag::ChildValue value> using ReportCount =
    tc::Node<ReportCountTag, value>;
template <ReportSizeTag::ChildValue value> using ReportSize =
    tc::Node<ReportSizeTag, value>;

// Local items
template <GenericDesktopTag::ChildValue value> using GenericDesktop = tc::Node<GenericDesktopTag, value>;
template <int value> using UsageMinimum = tc::Node<UsageMinimumTag, value>;
template <int value> using UsageMaximum = tc::Node<UsageMaximumTag, value>;


} // end namespace hiddesc

#endif
