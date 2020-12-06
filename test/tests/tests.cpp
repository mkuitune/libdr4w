#include <iostream>
using namespace std;

#include <filesystem>

#include <dr4/dr4_rand.h>
#include <dr4/dr4_util.h>
#include <dr4/dr4_exe_util.h>
#include <dr4/dr4_image.h>

//------------------------------------------------------
// Test utilites. Figure out what to do with these
//------------------------------------------------------

void testrand() {
    using namespace dr4;
    RandFloat r(-1.0, 1.0);
    size_t n = 30;
    DoTimes(n, [&]() {cout << r.rand() << endl; });
    RandInt i(0,100);
    DoTimes(n, [&]() {cout << i.rand() << endl; });
}

void testImageIO() {
    using namespace dr4;
    auto numberpathres = Resources::GetResourcePath(PathString("Images/uvtest-numbers.png"));
    if (!numberpathres)
        return;
    auto numberpath = numberpathres.value();
    
    auto numberreadres = readImage(numberpath.string());
    if (!numberreadres.second.empty())
        return;
    auto& numberimg = numberreadres.first;

    writeImageAsPng(*numberimg, "numbers.png");
}

int main()
{
    testImageIO();
    testrand();
    return 0;
}

