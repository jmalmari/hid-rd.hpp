#include <iostream>
#include "hidrd/hidrd.hpp"

using namespace hidrd::typechain;
using hidrd::DataNodeSize;
using hidrd::itemdata::DataSize;

template <int data> using SignedSize =
    DataNodeSize<Node<Root<int>, data>>;

template <unsigned int data> using UnsignedSize =
    DataNodeSize<Node<Root<unsigned int>, data>>;

std::string dataSizeString(DataSize size)
{
    switch (size)
    {
    case DataSize::NoData:
        return "NoData";
    case DataSize::OneByte:
        return "OneByte";
    case DataSize::TwoBytes:
        return "TwoBytes";
    case DataSize::FourBytes:
        return "FourBytes";
    default:
        return "invalid size";
    }
}

void COMPARE(DataSize actual, DataSize expected, char const* msg)
{
    if (actual != expected)
    {
        std::cerr << "Check failed: " << msg
                  << ", expected " << dataSizeString(expected)
                  << ", got " << dataSizeString(actual) << "\n";
        throw 1;
    }
}

int main()
{
    try
    {
        COMPARE(UnsignedSize<0>::Result, DataSize::OneByte, "unsigned 0");
        COMPARE(UnsignedSize<1>::Result, DataSize::OneByte, "unsigned 1");

        // One to two bytes boundary
        COMPARE(UnsignedSize<255>::Result, DataSize::OneByte, "unsigned 255");
        COMPARE(UnsignedSize<256>::Result, DataSize::TwoBytes, "unsigned 256");

        // Two to four bytes boundary
        //
        // Note: The HID spec doesn't really say whether unsigned
        // 32-bit values are possible.
        COMPARE(UnsignedSize<65535>::Result, DataSize::TwoBytes, "unsigned 65535");
        COMPARE(UnsignedSize<65536>::Result, DataSize::FourBytes, "unsigned 65536");


        COMPARE(SignedSize<0>::Result, DataSize::OneByte, "signed 0");
        COMPARE(SignedSize<1>::Result, DataSize::OneByte, "signed 1");

        // One to two bytes boundary
        COMPARE(SignedSize<127>::Result, DataSize::OneByte, "signed 127");
        COMPARE(SignedSize<128>::Result, DataSize::TwoBytes, "signed 128");

        // Two to four bytes boundary
        COMPARE(SignedSize<32767>::Result, DataSize::TwoBytes, "signed 32767");
        COMPARE(SignedSize<32768>::Result, DataSize::FourBytes, "signed 32768");

        // And similar for negative values
        COMPARE(SignedSize<-1>::Result, DataSize::OneByte, "signed -1");
        COMPARE(SignedSize<-127>::Result, DataSize::OneByte, "signed -127");
        COMPARE(SignedSize<-128>::Result, DataSize::OneByte, "signed -128");
        COMPARE(SignedSize<-129>::Result, DataSize::TwoBytes, "signed -129");
        COMPARE(SignedSize<-32767>::Result, DataSize::TwoBytes, "signed -32767");
        COMPARE(SignedSize<-32768>::Result, DataSize::TwoBytes, "signed -32768");
        COMPARE(SignedSize<-32769>::Result, DataSize::FourBytes, "signed -32769");
    }
    catch (int error)
    {
        return error;
    }

    return 0;
}
