#pragma once

#include <dr4/dr4_image.h>

namespace dr4 {

    struct Painter {
        dr4::ImageRGBA8SRGB& m_img;
        size_t height;
        Painter(dr4::ImageRGBA8SRGB& img) :m_img(img), height(img.dim2()) {}

        inline void SetPixel(float x, float y, const dr4::SRGBA& color)
        {
            if (x < 0.0f || y < 0.0f)
                return;

            m_img.set((unsigned int)x, height - (unsigned int)y - 1, color);
        }
        
        inline void SetPixeli(unsigned x, unsigned y, const dr4::SRGBA& color)
        {
            m_img.set(x, (height - y - 1), color);
        }

        inline void SetPixelNatural(float x, float y, const dr4::SRGBA& color)
        {
            if (x < 0.0f || y < 0.0f)
                return;

            m_img.set((unsigned int)x, (unsigned int)y, color);
        }
    };

    // These work from the presumption that 0,0 is at left lower corner - so the mapping to the natural pixel
    // coordinates where y=0 is the top row is done internally
    class Razz {
    public:
        static void DrawLine(Painter& p, SRGBA color, float x1, float y1, float x2, float y2);

        static void DrawTriangle(Painter& p, const SRGBA& color1, float x1, float y1,
            float x2, float y2, float x3, float y3);
       
        // halfspace
        static void DrawTriangle2(Painter& p, const SRGBA& color1, float x1, float y1,
            float x2, float y2, float x3, float y3);
       
        // halfspace with incremental row differentials
        static void DrawTriangle3(Painter& p, const SRGBA& color1, float x1, float y1,
            float x2, float y2, float x3, float y3);
    };
}
