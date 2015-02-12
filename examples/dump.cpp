#include <unistd.h>
#include <algorithm>
#include <iterator>
#include <iostream>

#include <hidrd/hidrd.hpp>

using namespace hidrd;
using namespace hidrd::itemdata;

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
    > MouseDescriptor;

int main(int, char *argv[])
{
    if (isatty(STDOUT_FILENO))
    {
        std::cerr << "Dump the binary report descriptor\n\n"
                  << "Usage:\n"
                  << "\t" << argv[0] << " > file\n"
                  << "\t" << argv[0] << " | foo\n";
        return 1;
    }
    else
    {
        std::copy(MouseDescriptor::data.begin(),
                  MouseDescriptor::data.end(),
                  std::ostream_iterator<uint8_t>(std::cout));
        return 0;
    }
}
