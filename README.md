# hid-rd.hpp

## Intro

This project contains C++ class templates for building compile-time
HID report descriptors.

The main purpose is to provide type safe building of the binary
descriptor that would otherwise be quite complex and error-prone to
create or change, or would require external application to generate.

It was created as an exercise for myself in template programming,
motivated by the ease of introducing errors in binary descriptors when
tweaking them by hand.

Related specifications:

- USB HID definition v1.11 - the structure of the binary report
- USB HID Usage Tables v1.12 - detailed field descriptions

## Features

- compile time byte array for the descriptor data
- supports only Short Items (1-5 bytes)
- type safe data for items, e.g. UsagePage with argument Pointer leads
  to compiler error (Pointer is *usage*, not *usage page*)
- automatically packs data arguments using the least amount of bytes

It doesn't check everything for you. You still have to pay attention
to the basic descriptor building rules. Some gotchas to keep an eye
on:

- valid item scopes
- ending collections
- check logical min/max value against ReportSize value
- only use usages that belong to the current usage page

It may be possible for some or all of these to be enforced by
adding more hierarchically structured types that span multiple
items.

Also, the coverage for Short Items is far from complete but the
framework is there. I've only tested it with a rather simple
descriptor made for a mouse (see `./tests/descriptor.cpp`.)

If something is missing, request it, stay tuned, or help out ;)

## Requirements

Compiler with C++11 enabled. For tests, you need CMake.

The C++11 requirement doesn't necessarily mean that you couldn't
include the report descriptor in your build process for target
platforms that don't have C++11. More on this below.

## Usage (no pun intended)

All code resides in header files so no need for a separate build
step. Just add `./include` to your project's include path and then
include the `hidrd.hpp` header as

    #include <hidrd/hidrd.hpp>

There are some other header files as well in that directory. Those are
included by `hidrd.hpp` itself.

The central type is the type Descriptor that takes items as its
template parameters. Its primary template looks like:

    template <typename... Items> struct Descriptor;

Individual items are defined separately and most of them take further
data arguments.

Brief (and incomplete) example usage:

    #include <hidrd/hidrd.hpp>

    using namespace hidrd;
    using namespace hidrd::itemdata;

    typedef Descriptor<
        UsagePage<args::GenericDesktop>,
        GenericDesktop<args::Mouse>,
        ReportId<1>,
        Collection<args::Application>,
        GenericDesktop<args::Pointer>,
        ...
        EndCollection> MouseDescriptor;

    // Do something with its binary data
    publishDescriptor(MouseDescriptor::data);

The data member is an `std::array<uint8_t, N>` array.

### No C++(11) for your target platform?

I suspect many potential users writing report descriptors are actually
targeting platforms that don't have C++ support, let alone
C++11. However, a specialized build step might still work as long as
you have a C++11 compiler on your build system.

#### Export to a binary file

It's trivial to generate a binary file out of the descriptor. For an
example, check `examples/dump.cpp`.

If a C header file is wanted, it's possible to convert the binary file
with other tools that exists out there. But then you might want to
rethink whether you want to use those other tools from the get-go.

#### Use C++11 for only one translation unit

*"Doesn't this defeat the purpose of avoiding C++11?"*

Partly, maybe. But it's possible to drop the dependency on the actual
C++11 *library* and only use C++11 *language* features which can make
all the difference. You only have to ensure that the C++ compiler
generates compatible code.

1. Drop standard library dependencies

    Define `BYTES_USE_CSTYLE_ARRAY` which makes the code use regular
    C-style array instead of an `std::array`.

2. Expose symbols for the descriptor data

    The data array and size can be defined as globals for other
    translation units to use. Make sure all C++(11) symbols have
    internal linkage.

3. Compile into your program

    Most build tools allow you to define a different compiler for
    specific code file. Compile your descriptor .cpp file with C++11
    compiler and link as normal.

    If you're cross compiling, I recommend using Clang's C++ compiler
    since it has built-in support for many targets, depending on its
    configuration.

Here's an example for a C application:

    #define BYTES_USE_CSTYLE_ARRAY
    #include <hidrd/hidrd.hpp>

    namespace {
        typedef Descriptor<... your items here ...> MyDescriptor;
    }

    extern "C"
    unsigned int FetchDescriptor(unsigned char const **data)
    {
        *data = &MyDescriptor::data[0];
        return MyDescriptor::Size;
    }

and refer to them like so:

    extern unsigned int FetchDescriptor(unsigned char const **data);

For a C++ application, you could define:

    extern std::size_t const DescriptorDataSize = MyDescriptor::Size;
    unsigned char const (&DescriptorData)[MyDescriptor::Size] = MyDescriptor::data;

and refer to them like so:

    extern std::size_t const DescriptorDataSize;
    extern unsigned char const (&DescriptorData)[];


## Run tests

1. cmake
2. make check

Because of the compile time nature of the code, a major role in the
tests is that they actually compile.
