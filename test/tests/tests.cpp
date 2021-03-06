#include <iostream>
#include <filesystem>
#include <algorithm>
#include <optional>
#include <fstream>

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
#include <dr4/dr4_span2f.h>
#include <dr4/dr4_analysis.h>
#include <dr4/dr4_timer.h>
#include <dr4/dr4_handlemanager.h>

using namespace std;

class testharness_t {
public:
	class test_t {
	public:
        bool run = false;
		virtual ~test_t() {}
        virtual std::string name() const = 0;
		virtual void runtest() = 0;
        std::string prefix(const std::string& post) { return name() + "-" + post; }
        std::string errorString(const std::string& post) { return std::string("ERROR:") + name() + ":" + post; }
    };

    typedef std::vector<std::shared_ptr<test_t>> testset_t;
    static std::map<std::string, testset_t> sets;

    static void run() {
        using namespace std;
        cout << "Run tests"  << endl;
        for (auto& s : sets) {
            for (auto& t : s.second) {
                t->runtest();
                cout << s.first << ":" << t->name() << " DONE" << endl;
            }
        }
    }

    static void run(const std::vector<std::string> filter) {
        using namespace std;
        cout << "Run tests"  << endl;
        for (auto& s : sets) {
            for (auto& t : s.second) {
                auto name = t->name();
                if(std::find(filter.begin(), filter.end(), name) == filter.end())
                    continue;
                t->runtest();
                cout << s.first << ":" << t->name() << " DONE" << endl;
            }
        }
    }

    template<class T>
    static bool push(const std::string& setname) { 
        std::shared_ptr<test_t> ptr(new T());
        (sets[setname]).emplace_back((ptr)); 
        return true;
    }
};
std::map<std::string, testharness_t::testset_t> testharness_t::sets;

#define TESTFUN(family_, testname_) class  testname_ : public testharness_t::test_t { public:\
    static bool latched;\
    virtual ~testname_() {} \
    virtual std::string name() const override {return #testname_;}\
    virtual void runtest() override; \
}; \
bool testname_::latched = testharness_t::push<testname_>(#family_); \
void testname_::runtest()


#if 0
class foottst : public testharness_t::test_t {
public:
    static bool latched;
    virtual ~foottst() {}
    void impl();
    virtual void operator()() override { impl(); }
};
bool foottst::latched = testharness_t::push<foottst>("testfamily");
void foottst::impl()
{
}
#endif


//------------------------------------------------------
// Test utilites. Figure out what to do with these
//------------------------------------------------------

TESTFUN(common, testrand){
//void testrand() {
    using namespace dr4;
    RandFloat r(-1.0, 1.0);
    size_t n = 30;
    DoTimes(n, [&]() {cout << r.rand() << endl; });
    RandInt i(0,100);
    DoTimes(n, [&]() {cout << i.rand() << endl; });
}

TESTFUN(common, imageIO){
//void testImageIO() {
    using namespace dr4;
    auto numberpathres = Resources::GetResourcePath(PathString("Images/uvtest-numbers.png"));
    if (!numberpathres)
        return;
    auto numberpath = numberpathres.value();
    
    auto numberreadres = readImage(numberpath.string());
    if (!numberreadres.second.empty())
        return;
    auto& numberimg = numberreadres.first;

    writeImageAsPng(*numberimg, prefix("numbers.png"));
}

struct RendererTestSetup1 {

    unsigned width = 640;
    unsigned height = 480;
    dr4::RasterConfig2D camera;
    std::shared_ptr<dr4::IRasterizer> rasterizer;
    //dr4::ParallelExecutor executor;
    dr4::SequentialExecutor executor;
    dr4::RasterDomain rasterDomain;

    RendererTestSetup1() { 
        rasterDomain.origin = { 0,0 };
        rasterDomain.width = width;
        rasterDomain.height = height;
        rasterizer = dr4::CreateRasterizer(width, height);
        //camera = dr4::RasterConfig2D::Default();
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

TESTFUN(scene, scenetest01){
//void sceneTest01() {
    using namespace dr4;
    RendererTestSetup1 state;

    Scene2D scene = GetTestScene01();
    FrameTasks tasks;

    state.rasterizer->draw2D(state.camera, scene, tasks);
    state.executor.runBlock(tasks.tasks);
    state.rasterizer->applyResult(tasks);
    auto image = state.rasterizer->getColorAsSRGB();
    writeImageAsPng(image, prefix("out.png"));
}

TESTFUN(rasterize, drawRandomLines){
//void testDrawRandLines() {
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

    ptr.writeOut(prefix("out.png"));
}

TESTFUN(rasterize, drawRandomDots){
//void testDrawRandDots() {
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
    writeImageAsPng(imageOut, prefix("out.png"));
}

TESTFUN(rasterize, triangles1){
//void testTriangles1() {
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
    ptr.writeOut(prefix("01.png"));

    image.setAll(background);
    Razz::DrawTriangle2(ptr, black, 10, 10, 30,10, 20, 30);
    Razz::DrawLine(ptr, red, 10, 10, 30, 30);
    ptr.writeOut(prefix("02.png"));

    image.setAll(background);
    Razz::DrawTriangle3(ptr, black, 10, 10, 30,10, 20, 30);
    Razz::DrawLine(ptr, red, 10, 10, 30, 30);
    ptr.writeOut(prefix("03.png"));
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

TESTFUN(sdf, sdf1){
//void test2DSDF1() {
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
    ptr.writeOut(prefix("1.png"));

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

    const size_t startEdge = 0;
    const size_t endEdge = 3;

    for (size_t i = startEdge; i < endEdge; i++) 
    //size_t i = 1;
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

    outputTreeDbg(prefix("treedbg1.png"), quadtree);

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

    ptr.writeOut(prefix("2.png"));

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
    ptr.writeOut(prefix("2_sdf_field.png"));
#endif

#if 1
// colorize per real distance
    Array2D<float> realDists(image.dim1(), image.dim2(), 1.0e9f);
    for (size_t i = startEdge; i < endEdge; i++) 
    //size_t i = 1;
    {
        auto line = polygon2.lineAt(i);
        LineDistance2D dist(line);
        auto fun = dist.bindUnsigned();
        for (int i = 0; i < realDists.dim2(); i++) {
            for (int j = 0; j < realDists.dim1(); j++)
            {
                float x = (float)j, y = (float)i;
                float d = dist.unsignedDistance({ x, y });
                realDists.setIfSmaller(j, i, d);
                float samp = quadtree.getDeepSample(x, y);
            }
        }
    }
    
    Array2D<float> distDiff(image.dim1(), image.dim2(), 1.0e9f);
    {
        for (int i = 0; i < realDists.dim2(); i++) {
            for (int j = 0; j < realDists.dim1(); j++)
            {
                float x = (float)j, y = (float)i;
                float d = realDists.at(j, i);
                float samp = quadtree.getDeepSample(x, y);
                float diff = fabsf(samp - d);
                distDiff.set(j, i, diff);
            }
        }
    }

    // print out real values
    image.setAll(white);
    //RGBAFloat32 col1 = RGBAFloat32::Orange();
    //RGBAFloat32 col2 = RGBAFloat32::Navy();

    for (unsigned y = 0; y < image.dim2(); y++){
        for (unsigned x = 0; x < image.dim1(); x++){
            float dist = realDists.at(x, y);
            RGBAFloat32 col = Lerp(col1, col2, fabsf(sin(dist/2)));
            ptr.SetPixeli(x, y,  col);
        }
    }
    ptr.writeOut(prefix("2_sdf_field_real.png"));
    
    // print out value difference
    float mx = -1e9;
    PairIdx maxIdx;
    float mn = 1e9;
    PairIdx minIdx = {0,0};
    for (unsigned x = 0; x < image.dim1(); x++){
        for (unsigned y = 0; y < image.dim2(); y++){
            float df = distDiff.at(x, y);
            if (df > mx) {
                mx = df;
                maxIdx = { x,y };
            }
            if (df < mn) {
                mn = df;
                minIdx = { x,y };
            }
        }
    }
    float dm = mx - mn;
    size_t maxCell = quadtree.getIdxAt((float)maxIdx.x, (float)maxIdx.y);
    auto node = quadtree.nodes[maxCell];
    float realCorners[] = {
        realDists.at((size_t)node.x0, (size_t)node.y0),
        realDists.at((size_t)(node.x0 + node.d),(size_t) node.y0),
        realDists.at((size_t)(node.x0 + node.d),(size_t) (node.y0 + node.d)),
        realDists.at((size_t)node.x0, (size_t)(node.y0 + node.d)),
    };
    float maxTreeVal = quadtree.getDeepSample((float)maxIdx.x, (float)maxIdx.y);
    float maxRealVal = realDists.at(maxIdx);

    fq_interpoloation_samples_t mxSamples = node.samplepoints();
    float realValsAtSamples[5];
    for (int i = 0; i < 5; i++)
    {
        PairIdx p = {(size_t) mxSamples.coords[i].x, (size_t) mxSamples.coords[i].y };
        realValsAtSamples[i] = realDists.at(p);
    }

    std::cout << "max diff:" << mx << " min diff:" << mn << std::endl;
    // output dif values
    image.setAll(white);
    for (unsigned y = 0; y < image.dim2(); y++){
        for (unsigned x = 0; x < image.dim1(); x++){
            float dist = distDiff.at(x, y);
            float val = clampf((dist - mn) / dm, 0.f, 1.f);
            RGBAFloat32 col = Lerp(col1, col2, val);
            ptr.SetPixeli(x, y,  col);
        }
    }
    ptr.writeOut(prefix("2_sdf_field_diff.png"));

#endif
}

TESTFUN(sdf, SDFPolygon){
//void test2DSDFPolygon() {
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

    ptr.writeOut(prefix("1.png"));

    //
    // ASDF test - use pixel coordinates for tree to simplify initial testing
    //
    float din = (float) max(w, h);
    float xin = 0.0f;
    float yin = 0.0f;
    FieldQuadtreeBuilder builder(xin, yin, din);

    // TODO
    // The polygon can be tessellated EITHER as a set of lines, or as a single polygon
#if 0
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
#endif
    PolygonDistance2D distp2(polygon2);
    auto fun2 = distp2.bindSigned();
    builder.add(fun2);
    auto quadtree = builder.build();

    outputTreeDbg(prefix("treedbg.png"), quadtree);

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

    ptr.writeOut(prefix("sdf.png"));
}

void proto2DRendering() {
    // p : pixel buffer
    // for each 2d shape
    // 
    using namespace dr4;
    int pixW = 364;
    int pixH = 512;
    ImageRGBA32Linear image(pixW, pixH);
    RGBAFloat32 background = RGBAFloat32::White();
    RGBAFloat32 brush = RGBAFloat32::Black();;
    Painter ptr(image);

    RasterDomain targetBound = ptr.getFullRasterDomain();

    Pairf sceneOrigin = {0.f, 0.f};
    float sceneToPixelScale = 0.1f;
    // align scene and 
    // scene

    // bounds = get scene bounding box
    // mapping = map full scene to pixelviewbound
    // sceneview = view from mapping,scene
    // for each e in scene
    //     ebound = get bound of e
    //     renderbound = get pixelbound from mapping, ebound
    //     renderbound = renderbound.cropto targetbound
    //     quadtree (origin = ebound.origin, d = maxdim e)
    //     sizeOfPixelInScene = 1.f / sceneToPixelScale;
    //     maxError = sizeOfPixelInScene
    //     add to tree (distanceof(e), err = maxError)
    //     for xy in rendebound
    //        scenecoord = mapping.pixeltoscene(xy)
    //        dist = tree.sample(scenecoord)
    //        outpixel = colorize dist or skip

    return;
}


void outputGradient(dr4::GradientFloat32 grad, const std::string& name) {
    using namespace std;
    using namespace dr4;

    const int w = 512;
    const int h = 256;
    ImageRGBA32Linear image(w, h);
    RGBAFloat32 background = RGBAFloat32::Navy();
    auto lut = GradientToLUT(grad);
    Painter ptr(image);
    image.setAll(background);

    ptr.applyGradient({0,0 }, {512,0},lut);
    ptr.writeOut(name);

}

TESTFUN(common, gradient01){
//void testGradient01() {
    using namespace dr4;
    using namespace std;

    RGBAFloat32 background = RGBAFloat32::Navy();
    RGBAFloat32 black = RGBAFloat32::Black();
    RGBAFloat32 white = RGBAFloat32::White();
    RGBAFloat32 yellow = RGBAFloat32::Yellow();
    RGBAFloat32 green = RGBAFloat32::Green();
    RGBAFloat32 red = RGBAFloat32::Red();
    RGBAFloat32 blue = RGBAFloat32::Blue();

    GradientFloat32 grad1 = { { {0.0f, white}, {1.0f, black} } };
    GradientFloat32 grad2 = { { {0.0f, red}, {0.6f, yellow}, {1.f, blue }} };
    GradientFloat32 grad3 = { { {0.0f, red}, {0.8f, yellow}, {1.f, blue }} };
    GradientFloat32 grad4 = { { {0.0f, yellow},{0.5f, green} ,{1.f, blue }} };
    //GradientFloat32 grad4 = { { {0.0f, red},{0.5f, yellow}} };

    outputGradient(grad1, prefix("grad01.png"));
    outputGradient(grad2, prefix("grad02.png"));
    outputGradient(grad3, prefix("grad03.png"));
    outputGradient(grad4, prefix("grad04.png"));
}

//
// Interpolation tests
//

float compareLutPerf(const dr4::PiecewiseSpline2& spline, size_t sampleCount, size_t lutSize) {
    using namespace std;
    using namespace dr4;
    auto lut = spline.createByXLUT(lutSize);

    float xstart = spline.m_spans[0].start.x;
    float xlast = spline.m_spans.back().end.x;
    float dx = (xlast - xstart) / (sampleCount - 1);
    float junk = 0.f;

    // sample spline - lets just warm up first time
    float x;
    size_t i;

    // warmup
	for (i = 0, x = xstart; i < sampleCount; i++, x+= dx) {
        junk += spline.evalAt(x);
	}

    Timer timer;
    // sample spline 
	for (i = 0, x = xstart; i < sampleCount; i++, x+= dx) {
        junk += spline.evalAt(x);
	}
    double splineMs = timer.milliseconds();

    timer.reset();
	for (i = 0, x = xstart; i < sampleCount; i++, x+= dx) {
        junk += lut.getNearest(x);
	}
    double lutms = timer.milliseconds();

    cout << "sampleCount:" << sampleCount << " lutSize:" << lutSize << endl;
    cout << "Spline ms:" << splineMs << endl;
    cout << "LUT    ms:" << lutms << endl;

    return junk;
}

void interpolateAndWrite(const std::vector<dr4::Pairf>& points, const std::string filename) {

    using namespace dr4;
    using namespace std;
    size_t interval = 10;
    size_t samplecount = 30;
    auto interpolated = Analysis::InterpolateData(points, interval, samplecount);
    //auto interpolated = Analysis::InterpolateDataSpline(points, 10);
    auto mathscript = Analysis::PointsAndInterpolatedToMathematica(points, interpolated);
    Analysis::TextDump(mathscript, filename);
}

void interpolateAndWriteBez(const std::vector<dr4::Pairf>& points, const std::string filename) {

    using namespace dr4;
    using namespace std;
    size_t interval = 10;
    size_t samplecount = 30;
    auto interpolated = Analysis::InterpolateDataBezier(points, interval, samplecount);
    auto mathscript = Analysis::PointsAndInterpolatedToMathematica(points, interpolated);
    Analysis::TextDump(mathscript, filename);
}

void interpolateAndWriteBezMonotone(const std::vector<dr4::Pairf>& points, const std::string filename) {

    using namespace dr4;
    using namespace std;
    size_t interval = 10;
    size_t samplecount = 30;
    auto interpolatedRes = Analysis::InterpolateDataBezierMonotone(points, interval, samplecount);
    if (!interpolatedRes) {
        cout << string("  Error: Can't create monotone:") + interpolatedRes.metaToString() << endl;
        return;
    }
    auto interpolated = interpolatedRes.value();
    auto mathscript = Analysis::PointsAndInterpolatedToMathematica(points, interpolated);
    Analysis::TextDump(mathscript, filename);
}

TESTFUN(common, interpolate01){
//void interpolateTest01() {
    using namespace dr4;
    using namespace std;
    interpolateAndWrite({ {0.f, 0.f},{1.f, 1.f},{2.f, 0.f} }, prefix("plot01.nb"));
    interpolateAndWrite({ {0.f, 0.f},{1.f, 1.f},{2.f, 0.f}, {4.f, 5.f} }, prefix("plot02.nb"));
    
    interpolateAndWriteBez({ {0.f, 0.f},{1.f, 1.f},{2.f, 0.f} }, prefix("plot01_bez.nb"));
    interpolateAndWriteBez({ {0.f, 0.f},{1.f, 1.f},{2.f, 0.f}, {4.f, 5.f} }, prefix("plot02_bez.nb"));
    
    interpolateAndWriteBezMonotone({ {0.f, 0.f},{1.f, 1.f},{2.f, 0.f} }, prefix("plot01_bez_mon.nb"));
    interpolateAndWriteBezMonotone({ {0.f, 0.f},{1.f, 1.f},{2.f, 0.f}, {4.f, 5.f} }, prefix("plot02_bez_mon.nb"));
    interpolateAndWriteBezMonotone({ {0.f, 0.f},{1.f, 0.f},{1.f, 1.f},{2.f, 1.f}, {2.f, 0.f}, {4.f, 5.f}, {5.f, 3.f}}, prefix("plot03_bez_mon.nb"));
    cout << "  Error statement on next line is expected:" << endl;
    interpolateAndWriteBezMonotone({ {0.f, 0.f},{1.f, 1.f},{0.99f, 0.f},{2.f, 0.f} }, prefix("plot04_bez_mon.nb")); // intentionally wrong
    //interpolateAndWriteBez({ {0.f, 0.f},{1.f, 1.f},{2.f, 0.f}, {3.f, 1.f} }, "plot02_bez.nb");

#if 0
    // Impromptu spline vs lut perf testing
    // Debug lut is 4x faster than spline
    // Release lut is 7x faster than spline
    // perftest - spline vs lut
    size_t nSpans = 100;
    auto samples = Analysis::SampleLinearSpan({ 0.0f, 0.0f }, {1.0f, 1.0f}, nSpans);
    auto spline = PiecewiseSpline2::Create(samples);
    //size_t sampleCount = 10000000000;
    size_t sampleCount = 10000000;
    //size_t sampleCount = 100000;
    float junk = compareLutPerf(spline, sampleCount,100);
    cout << junk << endl;
    junk = compareLutPerf(spline, sampleCount, 1000);
    cout << junk << endl;
    junk = compareLutPerf(spline, sampleCount, 10000);
    cout << junk << endl;
#endif
}

namespace dr4 {
#define DECL_HANDLE_STORAGE(type_param_, local_name_)\
    HandleBuffer<type_param_> local_name_;\
    type_param_* local_name_##Get(HandleBuffer<type_param_>::Handle h){ return local_name_.get(h);} \
    bool local_name_##Delete(HandleBuffer<type_param_>::Handle h){ return local_name_.release(h);} \
    HandleBuffer<type_param_>::Handle local_name_##Create(){return local_name_.create();} \
	template<typename ... Args> \
	HandleBuffer<type_param_>::Handle local_name_##Create(Args&&...args)  { return local_name_.create(std::forward<Args>(args)...);}
#define DECL_HANDLE_STORAGE_CONSTRUC(local_name_, buf_size_) local_name_(buf_size_)


    struct testhandlemanager_t {
        DECL_HANDLE_STORAGE(std::string, genericNames)
        DECL_HANDLE_STORAGE(dr4::Scene2D, scenes2D)

        testhandlemanager_t():
            DECL_HANDLE_STORAGE_CONSTRUC(genericNames, 256) ,
            DECL_HANDLE_STORAGE_CONSTRUC(scenes2D, 32) 
        {}
    };
}

TESTFUN(common, handlebuffertest) {
    using namespace dr4;
    using namespace std;

    testhandlemanager_t man;

    auto hscen = man.scenes2DCreate();
    auto hscen2 = man.scenes2DCreate();
    auto hscen3 = man.scenes2DCreate();
    man.scenes2DDelete(hscen2);
    auto scen = man.scenes2DGet(hscen3);
    auto hstr = man.genericNamesCreate("foo foo foo");
    auto str = man.genericNamesGet(hstr);

    return;
}



#define RN(fname_)(#fname_)

int main()
{
    using namespace std;
    vector<string> filter = {
        //RN(gradient01),
        //RN(interpolate01)
        RN(scenetest01),
        RN(handlebuffertest)
    };

    //testharness_t::run();
    testharness_t::run(filter);
}

