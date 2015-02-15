#include <iostream>
#include <array>
#include <cstring>
#include "hidrd/hidrd.hpp"

using namespace hidrd;
using namespace hidrd::itemdata;
using ByteType = hidrd::bytes::ByteType;

template <typename... Types> constexpr auto make_array(Types... args)
    -> std::array<ByteType, sizeof...(args)>
{
    return { static_cast<ByteType>(args)... };
}

auto const ReferenceDescriptor = make_array(
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0x85, 1,                       // REPORT_ID (1)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)

    0x09, 0x30,                    //     USAGE (X)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x26, 0x56, 3,                 //     LOGICAL_MAXIMUM (854)
    0x75, 0x10,                    //     REPORT_SIZE (16)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)

    0x09, 0x31,                    //     USAGE (Y)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x26, 0xE0, 1,                 //     LOGICAL_MAXIMUM (480)
    0x75, 0x10,                    //     REPORT_SIZE (16)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)

    0xc0,                          //   END_COLLECTION
    0xc0                           // END_COLLECTION
);


// Create the same array using data values from hidrd.hpp but
// construct the descriptor by hand, byte by byte;

auto const DescriptorTypes = make_array(
    item_hdr_global(tags::UsagePage, OneByte),              args::GenericDesktop,
    item_hdr_local( tags::Usage, OneByte),                  args::Mouse,
    item_hdr_global(tags::ReportId, OneByte), 1,
    item_hdr_main(  tags::Collection, OneByte),             args::Application,
    item_hdr_local(   tags::Usage, OneByte),                args::Pointer,
    item_hdr_main(    tags::Collection, OneByte),           args::Physical,
    item_hdr_global(    tags::UsagePage, OneByte),          args::Button,
    item_hdr_local(     tags::UsageMinimum, OneByte),       1,
    item_hdr_local(     tags::UsageMaximum, OneByte),       3,
    item_hdr_global(    tags::LogicalMinimum, OneByte),     0,
    item_hdr_global(    tags::LogicalMaximum, OneByte),     1,
    item_hdr_global(    tags::ReportCount, OneByte),        3,
    item_hdr_global(    tags::ReportSize, OneByte),         1,
    item_hdr_main(      tags::Input, OneByte),              args::Variable,
    item_hdr_global(    tags::ReportCount, OneByte),        1,
    item_hdr_global(    tags::ReportSize, OneByte),         5,
    item_hdr_main(      tags::Input, OneByte),     args::Variable | args::Constant,
    item_hdr_global(    tags::UsagePage, OneByte),          args::GenericDesktop,

    item_hdr_local(     tags::Usage, OneByte),              args::X,
    item_hdr_global(    tags::LogicalMinimum, OneByte),     0,
    item_hdr_global(    tags::LogicalMaximum, TwoBytes),    0x56, 0x03,
    item_hdr_global(    tags::ReportSize, OneByte),         16,
    item_hdr_global(    tags::ReportCount, OneByte),        1,
    item_hdr_main(      tags::Input, OneByte),              args::Variable,

    item_hdr_local(     tags::Usage, OneByte),              args::Y,
    item_hdr_global(    tags::LogicalMinimum, OneByte),     0,
    item_hdr_global(    tags::LogicalMaximum, TwoBytes),    0xE0, 0x01,
    item_hdr_global(    tags::ReportSize, OneByte),         16,
    item_hdr_global(    tags::ReportCount, OneByte),        1,
    item_hdr_main(      tags::Input, OneByte),              args::Variable,

    item_hdr_main(    tags::EndCollection, NoData),
    item_hdr_main(  tags::EndCollection, NoData)
);

// The actual descriptor to test
typedef Descriptor<
    UsagePage<args::GenericDesktop>,
    GenericDesktop<args::Mouse>,
    ReportId<1>,
    Collection<args::Application>,
    GenericDesktop<args::Pointer>,
    Collection<args::Physical>,

    UsagePage<args::Button>,
    UsageMinimum<1>,
    UsageMaximum<3>,
    LogicalMinimum<0>,
    LogicalMaximum<1>,
    ReportCount<3>,
    ReportSize<1>,
    Input<args::Variable>,
    ReportCount<1>,
    ReportSize<5>,
    Input<args::Constant, args::Variable>,

    UsagePage<args::GenericDesktop>,

    GenericDesktop<args::X>,
    LogicalMinimum<0>,
    LogicalMaximum<854>,
    ReportSize<16>,
    ReportCount<1>,
    Input<args::Variable>,

    GenericDesktop<args::Y>,
    LogicalMinimum<0>,
    LogicalMaximum<480>,
    ReportSize<16>,
    ReportCount<1>,
    Input<args::Variable>,

    EndCollection,
    EndCollection

    > DescriptorToTest;


template <std::size_t N1, std::size_t N2>
bool compareByteByByte(std::array<ByteType, N1> const& refdata,
                       std::array<ByteType, N2> const& data)
{
    std::size_t minsize = std::min(N1, N2);

    for (std::size_t i = 0; i < minsize; ++i)
    {
        int expected = refdata[i];
        int actual = data[i];

        std::cout << std::dec << i << std::hex << ": 0x" << actual;

        if (expected == actual)
        {
            std::cout << " ok!" << std::endl;
        }
        else
        {
            std::cout << " mismatch! " << std::hex << " Expected 0x"
                      << expected << std::endl;
            return false;
        }
    }
    return true;
}

template <std::size_t N1, std::size_t N2>
bool compare(std::array<ByteType, N1> const& refdata,
             std::array<ByteType, N2> const& data)
{
    if (!compareByteByByte(refdata, data))
    {
        return false;
    }

    if (N1 != N2)
    {
        std::cout << std::dec
                  << "Lengths differ. Expected " << N1 << ", got " << N2 << "\n";
        return false;
    }

    return true;
}

int main(int, char*[])
{
    if (!compare(ReferenceDescriptor, DescriptorTypes))
    {
        std::cout << "Reference descriptors don't match.\n";
        return 1;
    }

    if (!compare(ReferenceDescriptor, DescriptorToTest::data))
    {
        return 2;
    }

    return 0;
}
