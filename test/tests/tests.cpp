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
#include <dr4/dr4_task.h>

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

void drawLine(dr4::ImageRGBA8SRGB& image, dr4::SRGBA color, float x1, float y1, float x2, float y2) {
    float xdiff = (x2 - x1);
    float ydiff = (y2 - y1);

    if (xdiff == 0.0f && ydiff == 0.0f) {
        image.set((size_t)x1, (size_t)y1, color);
        return;
    }

    if (fabs(xdiff) > fabs(ydiff)) {
        float xmin, xmax;

        // set xmin to the lower x value given
        // and xmax to the higher value
        if (x1 < x2) {
            xmin = x1;
            xmax = x2;
        }
        else {
            xmin = x2;
            xmax = x1;
        }

        // draw line in terms of y slope
        float slope = ydiff / xdiff;
        for (float x = xmin; x <= xmax; x += 1.0f) {
            float y = y1 + ((x - x1) * slope);
            //Color color = color1 + ((color2 - color1) * ((x - x1) / xdiff));
            image.set((size_t)x,(size_t) y, color);
        }
    }
    else {
        float ymin, ymax;

        // set ymin to the lower y value given
        // and ymax to the higher value
        if (y1 < y2) {
            ymin = y1;
            ymax = y2;
        }
        else {
            ymin = y2;
            ymax = y1;
        }

        // draw line in terms of x slope
        float slope = xdiff / ydiff;
        for (float y = ymin; y <= ymax; y += 1.0f) {
            float x = x1 + ((y - y1) * slope);
            //Color color = color1 + ((color2 - color1) * ((y - y1) / ydiff));
            image.set((size_t)x,(size_t) y, color);
        }
    }

}

void testDrawRandLines() {
    using namespace dr4;
    using namespace std;
    const int w = 512;
    const int h = 256;
    ImageRGBA8SRGB image(w, h);
    SRGBA background = { 255u,255u,255u,255u};
    SRGBA brush= { 0u, 0u, 0u, 255u };
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
        drawLine(image, brush, (float)xprev, (float)yprev, (float)x, (float)y);
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

int main()
{
    testImageIO();
    testrand();
    testDrawRandDots();
    testDrawRandLines();
    return 0;
}

