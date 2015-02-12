#include <iostream>
#include <vector>
#include <cstring>
#include "hidrd/bytes.hpp"

using hidrd::bytes::Bytes;
using hidrd::bytes::Byte;

typedef Bytes< Byte<'a'>,
               Byte<'b'>,
               Byte<'c'>,
               Byte<'d'> > Bytes1;

typedef Bytes< Bytes<Byte<'a'>>,
               Bytes<Byte<'b'>,
                     Byte<'c'>,
                     Byte<'d'>> > Bytes2;

typedef Bytes<
    Bytes<
        Bytes<
            Bytes<
                Byte<'a'>
                >
            >
        >
    >
Bytes3;

typedef Bytes<
    Bytes< Byte<'1'>, Byte<'2'>, Byte<'3'> >,
    Bytes< Byte<'4'>, Byte<'5'>, Byte<'6'> >
    >
Bytes4;

typedef Bytes<
    Byte<'1'>,
    Bytes< Bytes<Byte<'2'>> >
    >
Bytes5;

typedef Bytes< Bytes1, Bytes4, Bytes1 > Bytes6;

typedef Bytes1::Flatten Flatten1;
typedef Bytes2::Flatten Flatten2;
typedef Bytes3::Flatten Flatten3;
typedef Bytes4::Flatten Flatten4;
typedef Bytes5::Flatten Flatten5;
typedef Bytes6::Flatten Flatten6;

struct TestItem
{
    std::string name;
    std::string expected;
    uint8_t const * const data;
    std::size_t dataSize;
};

#define APPEND_DATA(Type) &Type::data[0], Type::data.size()

TestItem items[] = {
    { "simple list", "abcd", APPEND_DATA(Flatten1) },
    { "nested list", "abcd", APPEND_DATA(Flatten2) },
    { "deep", "a", APPEND_DATA(Flatten3) },
    { "two lists", "123456", APPEND_DATA(Flatten4) },
    { "byte followed by list", "12", APPEND_DATA(Flatten5) },
    { "complex", "abcd123456abcd", APPEND_DATA(Flatten6) }
};

int main(int, char*[])
{
    for (auto const& item : items)
    {
        if (item.expected.size() != item.dataSize)
        {
            std::cout << "Test \"" << item.name << "\" failed.\n";
            std::cout << "Data length don't match." << std::endl;
            return 1;
        }
        else if (memcmp(item.expected.c_str(), item.data, item.dataSize))
        {
            std::cout << "Test " << item.name << " failed.\n";
            std::cout << "Data don't match. Expected " << item.expected << ", got:\n";
            for (std::size_t i = 0; i < item.dataSize; ++i)
            {
                uint8_t byte = item.data[i];
                std::cout << std::hex << "0x" << (0xff & byte) << "(" << byte << ") ";
            }
            std::cout << std::endl;
            return 2;
        }
    }
    return 0;
}
