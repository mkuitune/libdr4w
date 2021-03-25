#pragma once

#include <dr4/dr4_image.h>
#include <dr4/dr4_rasterizer_area.h>

#include <map>

namespace dr4 {

    struct Gradient {
        std::map<float, RGBAFloat32> points;
    };

    struct Painter {
        ImageRGBA32Linear& m_img;
        size_t height;
        Painter(dr4::ImageRGBA32Linear& img) :m_img(img), height(img.dim2()) {}

        RasterDomain getFullRasterDomain() {
            auto size = m_img.size();
            return RasterDomain::Create(size.first, size.second);
        }

        void distanceToGradient(RasterDomain renderArea) {
        }

        void fillByDistance(size_t ) {

        }

        void strokeByDistance() {
        }

        void fill(const dr4::RGBAFloat32& color) {
            m_img.fill(color);
        }

        inline void SetPixel(float x, float y, const dr4::RGBAFloat32& color)
        {
            if (x < 0.0f || y < 0.0f)
                return;
            unsigned ux = (unsigned)x;
            unsigned uy = (unsigned)y;
            if (ux >= m_img.dim1() || uy >= m_img.dim2())
                return;
            m_img.set(ux, height - uy - 1, color);
        }
        
        inline void SetPixelUnsafe(float x, float y, const dr4::RGBAFloat32& color)
        {
            unsigned ux = (unsigned)x;
            unsigned uy = (unsigned)y;
            m_img.set(ux, height - uy - 1, color);
        }
        
        inline void SetPixeli(unsigned x, unsigned y, const dr4::RGBAFloat32& color)
        {
            m_img.set(x, (height - y - 1), color);
        }
        
        inline void BlendPixeli(unsigned x, unsigned y, const dr4::RGBAFloat32& color)
        {
            if (x < 0.0f || y < 0.0f)
                return;
            unsigned ux = (unsigned)x;
            unsigned uy = (unsigned)y;
            if (ux >= m_img.dim1() || uy >= m_img.dim2())
                return;

            auto dst = m_img.at(x, (height - y - 1));
            auto out = BlendPreMultipliedAlpha(color, dst);
            m_img.set(x, (height - y - 1), out);
        }

        inline void SetPixelNatural(float x, float y, const dr4::RGBAFloat32& color)
        {
            if (x < 0.0f || y < 0.0f)
                return;

            m_img.set((unsigned int)x, (unsigned int)y, color);
        }

        void writeOut(const std::string& filename) {
            auto srgb = convertRBGA32LinearToSrgb(m_img);
            writeImageAsPng(srgb, filename);
        }
    };

    // These work from the presumption that 0,0 is at left lower corner - so the mapping to the natural pixel
    // coordinates where y=0 is the top row is done internally
    class Razz {
    public:
        static void DrawLine(Painter& p, RGBAFloat32 color, float x1, float y1, float x2, float y2);
        static void DrawLine(Painter& p, RGBAFloat32 color, Pairf fst, Pairf snd){
            DrawLine(p, color, fst.x, fst.y, snd.x, snd.y);
        }

        static void DrawTriangle(Painter& p, const RGBAFloat32& color1, float x1, float y1,
            float x2, float y2, float x3, float y3);
       
        // halfspace
        static void DrawTriangle2(Painter& p, const RGBAFloat32& color1, float x1, float y1,
            float x2, float y2, float x3, float y3);
       
        // halfspace with incremental row differentials
        static void DrawTriangle3(Painter& p, const RGBAFloat32& color1, float x1, float y1,
            float x2, float y2, float x3, float y3);
    };
}
