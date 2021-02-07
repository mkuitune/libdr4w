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
#include <dr4/dr4_distance.h>

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
    ImageRGBA32Linear image(w, h);
    RGBAFloat32 background = RGBAFloat32::White();
    RGBAFloat32 brush = RGBAFloat32::Black();;
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

    ptr.writeOut("testDrawRandLines.png");
}

void testDrawRandDots() {
    using namespace dr4;
    using namespace std;
    const int w = 256;
    const int h = 256;
    ImageRGBA32Linear image(w, h);
    RGBAFloat32 background = RGBAFloat32::White();
    RGBAFloat32 brush= RGBAFloat32::Black();
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
    auto imageOut = convertRBGA32LinearToSrgb(image);
    writeImageAsPng(imageOut, "testDrawRandDots.png");
}

void testTriangles1() {
    using namespace dr4;
    using namespace std;
    const int w = 256;
    const int h = 256;
    ImageRGBA32Linear image(w, h);
    RGBAFloat32 background = RGBAFloat32::White();
    RGBAFloat32 black= RGBAFloat32::Black();
    RGBAFloat32 red = RGBAFloat32::Red();

    Painter ptr(image);

    image.setAll(background);
    Razz::DrawTriangle(ptr, black, 10, 10, 30,10, 20, 30);
    Razz::DrawLine(ptr, red, 10, 10, 30, 30);
    ptr.writeOut("testTriangles1.png");

    image.setAll(background);
    Razz::DrawTriangle2(ptr, black, 10, 10, 30,10, 20, 30);
    Razz::DrawLine(ptr, red, 10, 10, 30, 30);
    ptr.writeOut("testTriangles2.png");

    image.setAll(background);
    Razz::DrawTriangle3(ptr, black, 10, 10, 30,10, 20, 30);
    Razz::DrawLine(ptr, red, 10, 10, 30, 30);
    ptr.writeOut("testTriangles3.png");
}

namespace dr4 {
    void outputTreeDbg(const std::string& name, const FieldQuadtree& tree) {
        size_t depth = tree.maxDepth();
        size_t w = depth;
        size_t h = depth;
        w = std::max((size_t)512,w);
        h = std::max((size_t)512,h);
        ImageRGBA32Linear image(w, h);
        RGBAFloat32 navy = {0.f, 0.f, 0.52f, 1.f};
        RGBAFloat32 white = RGBAFloat32::White();
        image.setAll(navy);
        float scale = w / tree.nodes[0].d;
        Pairf origin = {tree.nodes[0].x0, tree.nodes[0].y0};
        Painter painter(image);
        for (const auto& n : tree.nodes) {
            float ox = n.x0 - origin.x;
            float oy = n.y0 - origin.y;
            float cx = ox + n.d;
            float cy = oy + n.d;
            ox *= scale;
            oy *= scale;
            cx *= scale;
            cy *= scale;
            Razz::DrawLine(painter, white, ox, oy, cx, oy);
            Razz::DrawLine(painter, white, ox, cy, cx, cy);
            Razz::DrawLine(painter, white, ox, oy, ox, cy);
            Razz::DrawLine(painter, white, cx, oy, cx, cy);
        }
        painter.writeOut(name);
    }

    void verifyTreeIsCoherent() 
    {
    }
}

void test2DSDF1() {
    using namespace dr4;
    using namespace std;
    const int w = 256;
    const int h = 256;
    ImageRGBA32Linear image(w, h);
    RGBAFloat32 white = RGBAFloat32::White();
    RGBAFloat32 black = RGBAFloat32::Black();
    RGBAFloat32 red = RGBAFloat32::Red();
    RGBAFloat32 blue = RGBAFloat32::Blue();

    Painter ptr(image);
    image.setAll(white);

    Polygon2D polygon = { {{{10.f, 10.f}, {50.f, 10.f}, {30.f, 30.f}}} };
    Polygon2D polygon2 = { {{{200.f, 200.f}, {50.f, 200.f}, {125.f, 50.f}}} };

    for (auto line : polygon) {
        Razz::DrawLine(ptr, red, line.fst,line.snd);
    }
    for (auto line : polygon2) {
        Razz::DrawLine(ptr, blue, line.fst, line.snd);
    }
    ptr.writeOut("test2DSDF1_1.png");

    //
    // ASDF test - use pixel coordinates for tree to simplify initial testing
    //
    float din = (float) max(w, h);
    float xin = 0.0f;
    float yin = 0.0f;
    FieldQuadtreeBuilder builder(xin, yin, din);

    // TODO
    // The polygon can be tessellated EITHER as a set of lines, or as a single polygon
#if 1

    //for (size_t i = 1; i < 3; i++) 
    size_t i = 1;
    {
        auto line = polygon2.lineAt(i);
        LineDistance2D dist(line);
        auto fun = dist.bindUnsigned();
        builder.add(fun);
    }
#endif
#if 0
    {
        auto line = polygon2.lineAt(0);
        LineDistance2D dist(line);
        auto fun = dist.bindUnsigned();
        builder.add(fun);
    }
#endif
    auto quadtree = builder.build();

    outputTreeDbg("treedbg1.png", quadtree);

    image.setAll(white);
    float thickness = 5.f / 2;
#if 1 // colorize as line
    // Map quadtree to output texture
    float k = 4.f;
    for (unsigned y = 0; y < image.dim2(); y++){
        for (unsigned x = 0; x < image.dim1(); x++){
            float dist = quadtree.getDeepSample((float)x, (float)y);
            float fd = fabsf(dist);
            if (fabsf(dist) <= thickness) {
                red.a = clampf(k * (1.0f - fd / thickness) , 0.f, 1.f);
                ptr.BlendPixeli(x, y, red);
            }
        }
    }
#endif

    ptr.writeOut("test2DSDF1_2_sdf.png");
#if 1 // colorize as striped gradient
    // Map quadtree to output texture

    image.setAll(white);
    RGBAFloat32 col1 = RGBAFloat32::Orange();
    RGBAFloat32 col2 = RGBAFloat32::Navy();

    for (unsigned y = 0; y < image.dim2(); y++){
        for (unsigned x = 0; x < image.dim1(); x++){
            float dist = quadtree.getDeepSample((float)x, (float)y);
            RGBAFloat32 col = Lerp(col1, col2, fabsf(sin(dist/2)));
            ptr.SetPixeli(x, y,  col);
        }
    }
#endif
    ptr.writeOut("test2DSDF1_2_sdf_field.png");

}

void test2DSDF2() {
    using namespace dr4;
    using namespace std;
    const int w = 256;
    const int h = 256;
    ImageRGBA32Linear image(w, h);
    RGBAFloat32 background = RGBAFloat32::White();
    RGBAFloat32 black = RGBAFloat32::Black();
    RGBAFloat32 red = RGBAFloat32::Red();
    RGBAFloat32 blue = RGBAFloat32::Blue();

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

    ptr.writeOut("test2DSDF1_1.png");

    //
    // ASDF test - use pixel coordinates for tree to simplify initial testing
    //
    float din = (float) max(w, h);
    float xin = 0.0f;
    float yin = 0.0f;
    FieldQuadtreeBuilder builder(xin, yin, din);

    // TODO
    // The polygon can be tessellated EITHER as a set of lines, or as a single polygon
    auto poly2linecount = polygon2.size();
#if 1
    //for (size_t i = 0; i < poly2linecount; i++) {
    for (size_t i = 0; i < 3; i++) {
        auto line = polygon2.lineAt(i);
        LineDistance2D dist(line);
        auto fun = dist.bindUnsigned();
        builder.add(fun);
    }
#endif
    auto quadtree = builder.build();

    outputTreeDbg("treedbg2.png", quadtree);

    // TODO: Visualize quadtree!

    image.setAll(background);

#if 1 // colorize as gradient
    // Map quadtree to output texture

    RGBAFloat32 col1 = RGBAFloat32::White();
    RGBAFloat32 col2 = RGBAFloat32::Red();
    for (unsigned y = 0; y < image.dim2(); y++){
        for (unsigned x = 0; x < image.dim1(); x++){
            float dist = quadtree.getDeepSample((float)x, (float)y);
            float u = clampf(dist / 100.f, 0.f, 1.f);
            RGBAFloat32 col = Lerp(col1, col2, u);
            ptr.SetPixeli(x, y,  col);
        }
    }
#endif

    ptr.writeOut("test2DSDF2_2_sdf.png");
}

int main()
{
    testImageIO();
    testrand();
    testDrawRandDots();
    testDrawRandLines();
    testTriangles1();
    test2DSDF1();
    test2DSDF2();
    return 0;
}

