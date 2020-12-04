#include <iostream>
using namespace std;

#include <dr4/dr4_rand.h>
#include <dr4/dr4_util.h>

void testrand() {
    using namespace dr4;
    RandFloat r(-1.0, 1.0);
    size_t n = 30;
    DoTimes(n, [&]() {cout << r.rand() << endl; });
    RandInt i(0,100);
    DoTimes(n, [&]() {cout << r.rand() << endl; });
}

int main()
{
    testrand();
    return 0;
}

