#include "hidrd/typechain.hpp"

using namespace typechain;

enum Level0_Data
{
    Level1,
    Level2,
};


enum Level1_Data
{
    Level1_1,
    Level1_2,
};
enum Level2_Data
{
    Level2_1,
    Level2_2,
};


enum Level2_1_Data
{
    Level2_1_1 = 50,
    Level2_1_2 = 51,
};


using L0 = Root<Level0_Data>;

using L1 = Node<L0, Level1, Level1_Data>;
using L2 = Node<L0, Level2, Level2_Data>;
using L2 = Node<L0, Level2, Level2_Data>;

using L11 = Node<L1, Level1_1>;
using L12 = Node<L1, Level1_2>;
using L21 = Node<L2, Level2_1, Level2_1_Data>;
using L22 = Node<L2, Level2_2>;

using L211 = Node<L21, Level2_1_1>;
using L212 = Node<L21, Level2_1_2>;

using Data1 = L211;
using Data2 = L212;

// If it compiles, assume it works.
int main() { return 0; }
