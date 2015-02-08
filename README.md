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

At the time of writing, the only thing needed from actual C++11
**library** is the new `std::array` type which is quite trivial to
change to basic C-style array. All the rest is just new syntax. This
means that you may be able to use the header (with the aforementioned
minor modification) even with older platforms, given that you have a
new enough compiler.

## Usage (no pun intended)

### Use in your project

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

### Run tests

1. cmake
2. make check

Because of the compile time nature of the code-under-test, a major
role in the tests is that they actually compile.
