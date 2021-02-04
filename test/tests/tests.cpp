#include <iostream>
using namespace std;

#include <filesystem>
#include <algorithm>

#include <dr4/dr4_rand.h>
#include <dr4/dr4_camera.h>
#include <dr4/dr4_util.h>
#include <dr4/dr4_exe_util.h>
#include <dr4/dr4_image.h>
#include <dr4/dr4_scene2d.h>
#include <dr4/dr4_rasterizer.h>
#include <dr4/dr4_rasterizer_algorithms.h>
#include <dr4/dr4_task.h>
#include <dr4/dr4_quadtree.h>

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

struct RendererTestSetup1 {

    unsigned width = 640;
    unsigned height = 480;
    dr4::Camera2D camera;
    std::shared_ptr<dr4::IRasterizer> rasterizer;
    //dr4::ParallelExecutor executor;
    dr4::SequentialExecutor executor;
    RendererTestSetup1() { 
        rasterizer = dr4::CreateRasterizer(width, height);
        camera = dr4::Camera2D::Default();
    }
};

dr4::Scene2D GetTestScene01(){
    using namespace dr4;
    Scene2DBuilder builder;

    size_t layerIdx = builder.addLayer();
    Material2D mat;
    mat.colorFill = RGBAFloat32::Red();
    mat.colorLine = RGBAFloat32::Black();
    mat.linewidth = 1.0f;
    size_t materialIdx = builder.addMaterial(mat);

    ColorFill fill = { RGBAFloat32::White() };

    Line2DCollection lines;
    lines.material = materialIdx;
    Pairf a = { -0.3f, -0.3f };
    Pairf b = { 0.0f, 0.3f };
    Pairf c = { 0.3f, -0.3f };
    lines.append({a, b});
    lines.append({b, c});
    lines.append({c, a});

    builder.add(layerIdx, fill);
    builder.add(layerIdx, lines);
    return builder.build();
}

void sceneTest01() {
    using namespace dr4;
    RendererTestSetup1 state;

    Scene2D scene = GetTestScene01();
    FrameTasks tasks;
    state.rasterizer->draw2D(state.camera, scene, tasks);
    state.executor.runBlock(tasks.tasks);
    state.rasterizer->applyResult(tasks);
    auto image = state.rasterizer->getColorAsSRGB();
    writeImageAsPng(image, "sceneTest01.png");
}

void testDrawRandLines() {
    using namespace dr4;
    using namespace std;
    const int w = 512;
    const int h = 256;
    ImageRGBA8SRGB image(w, h);
    SRGBA background = { 255u,255u,255u,255u};
    SRGBA brush= { 0u, 0u, 0u, 255u };
    Painter ptr(image);

    image.setAll(background);
    int n = 100;
    RandIntGenerator random;
    IntegerRange xrange(0, w);
    IntegerRange yrange(0, h);

    int xprev = (int)xrange.rand(random);
    int yprev = (int)yrange.rand(random);
    for (int i = 0; i < n; i++)
    {
        int x = (int)xrange.rand(random);
        int y = (int)yrange.rand(random);
        Razz::DrawLine(ptr, brush, (float)xprev, (float)yprev, (float)x, (float)y);
        xprev = x;
        yprev = y;
    }
    writeImageAsPng(image, "testDrawRandLines.png");
}

void testDrawRandDots() {
    using namespace dr4;
    using namespace std;
    const int w = 256;
    const int h = 256;
    ImageRGBA8SRGB image(w, h);
    SRGBA background = { 255u,255u,255u,255u};
    SRGBA brush= { 0u, 0u, 0u, 255u };
    image.setAll(background);
    int n = 100;
    RandIntGenerator random;
    IntegerRange xrange(0, w);
    IntegerRange yrange(0, h);

    for (int i = 0; i < n; i++)
    {
        int x = (int)xrange.rand(random);
        int y = (int)yrange.rand(random);
        image.set(x, y, brush);
    }
    writeImageAsPng(image, "testDrawRandDots.png");
}

void testTriangles1() {
    using namespace dr4;
    using namespace std;
    const int w = 256;
    const int h = 256;
    ImageRGBA8SRGB image(w, h);
    SRGBA background = { 255u,255u,255u,255u};
    SRGBA black= { 0u, 0u, 0u, 255u };
    SRGBA red = { 255u, 0u, 0u, 255u };

    Painter ptr(image);

    image.setAll(background);
    Razz::DrawTriangle(ptr, black, 10, 10, 30,10, 20, 30);
    Razz::DrawLine(ptr, red, 10, 10, 30, 30);
    writeImageAsPng(image, "testTriangles1.png");

    image.setAll(background);
    Razz::DrawTriangle2(ptr, black, 10, 10, 30,10, 20, 30);
    Razz::DrawLine(ptr, red, 10, 10, 30, 30);
    writeImageAsPng(image, "testTriangles2.png");

    image.setAll(background);
    Razz::DrawTriangle3(ptr, black, 10, 10, 30,10, 20, 30);
    Razz::DrawLine(ptr, red, 10, 10, 30, 30);
    writeImageAsPng(image, "testTriangles3.png");
}

void test2DSDF1() {
    using namespace dr4;
    using namespace std;
    const int w = 256;
    const int h = 256;
    ImageRGBA8SRGB image(w, h);
    SRGBA background = { 255u,255u,255u,255u};
    SRGBA black= { 0u, 0u, 0u, 255u };
    SRGBA red = { 255u, 0u, 0u, 255u };
    SRGBA blue = { 0u, 0u, 255u, 255u };

    Painter ptr(image);
    image.setAll(background);

    Polygon2D polygon = { {{{10.f, 10.f}, {50.f, 10.f}, {30.f, 30.f}}} };
    Polygon2D polygon2 = { {{{200.f, 200.f}, {50.f, 200.f}, {125.f, 50.f}}} };

    for (auto line : polygon) {
        Razz::DrawLine(ptr, red, line.fst,line.snd);
    }
    for (auto line : polygon2) {
        Razz::DrawLine(ptr, blue, line.fst, line.snd);
    }

    writeImageAsPng(image, "test2DSDF1_1.png");

    //
    // ASDF test - use pixel coordinates for tree to simplify initial testing
    //
    float din = (float) max(w, h);
    float xin = 0.0f;
    float yin = 0.0f;
    FieldQuadtreeBuilder builder(xin, yin, din);

    // TODO
    // The polygon can be tessellated EITHER as a set of lines, or as a single polygon
    for (auto line : polygon) {
    }

    image.setAll(background);
}

int main()
{
    testImageIO();
    testrand();
    testDrawRandDots();
    testDrawRandLines();
    testTriangles1();
    test2DSDF1();
    return 0;
}

