#pragma once

#include <dr4/dr4_tuples.h>
#include <dr4/dr4_span2f.h>
#include <optional>

namespace dr4 {
    struct RasterDomain {
        PairIdx origin;
        size_t width;
        size_t height;

        PairIdx extrema() const {
            return { origin.x + width, origin.y + height };
        }

        std::optional<RasterDomain> cropTo(const RasterDomain& ref) const {
            PairIdx myex = extrema();
            PairIdx refex = ref.extrema();

            size_t lowerx = std::max(origin.x, ref.origin.x);
            size_t upperx = std::min(myex.x, refex.x);
            if (lowerx >= upperx)
                return std::nullopt;

            size_t lowery = std::max(origin.y, ref.origin.y);
            size_t uppery = std::min(myex.y, refex.y);
            if (lowery >= uppery)
                return std::nullopt;

            RasterDomain res;
            res.origin = { lowerx, lowery };
            res.width = upperx - lowerx;
            res.height = uppery - lowery;
            return res;
        }

        std::pair<Pairf, Pairf> clipLine(Pairf lineStart, Pairf lineEnd) const{
            
        }

        float aspectRatio() const {
            float w = (float)width;
            float h = (float)height;
            return w / h;
        }

        static RasterDomain Create(size_t width, size_t height) {
            RasterDomain res;
            res.origin = { 0,0 };
            res.width = width;
            res.height = height;
            return res;
        }
        
        static RasterDomain Create(PairIdx origin, size_t width, size_t height) {
            RasterDomain res;
            res.origin = origin;
            res.width = width;
            res.height = height;
            return res;
        }
    };

    struct SceneDomain {
        Span2f span;

        std::optional<SceneDomain > cropTo(const SceneDomain& other) const {
            auto res = span.intersect(other.span);
            if (!res.second)
                return std::nullopt;

            SceneDomain  boundOut = { res.first };
            return boundOut;
        }

        SceneDomain Create(const Pairf& origin, float width, float height) {
            Pairf maxPoint = origin;
            maxPoint.x += width;
            maxPoint.y += height;

            return { Span2f::Create(origin, maxPoint) };
        }
    };
}