#pragma once

#include <dr4/dr4_spanf.h>
#include <dr4/dr4_tuples.h>
#include <dr4/dr4_geometryresult.h>
#include <dr4/dr4_dimension.h>
#include <dr4/dr4_math.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <stdexcept>

namespace dr4 {
	struct Span2f {
		Spanf x;
		Spanf y;

		Span2f cover(const glm::vec2& v) const {
			return { x.cover(v.x),
				y.cover(v.y) };
		}
#if 0		
		Span2f cover(const Tripletf& v) const {
				return { x.cover(v.x),
					y.cover(v.y) };
		}
#endif
		
		Span2f cover(float xpos, float ypos) const {
			return { x.cover(xpos),
				y.cover(ypos)};
		}
		
		Span2f cover(const Span2f& sIn) const {
			return { x.cover(sIn.x),
				y.cover(sIn.y)};
		}

		bool contains(const glm::vec2& v) const {
			return x.contains(v.x) && y.contains(v.y);
		}

		Span2f expandXsymmetric(float d) const { return { x.expandSymmetric(d), y}; }
		Span2f expandYsymmetric(float d) const { return { x, y.expandSymmetric(d)}; }

		Span2f expandDimSymmetric(Dimension2D dim, float d) const {
			switch (dim) {
			case Dimension2D::X: return expandXsymmetric(d);
			case Dimension2D::Y: return expandYsymmetric(d);
			default: return *this;
			}
		}

		Pairf minimumPoint() const noexcept { return { x.min, y.min}; }
		Pairf center() const noexcept { return (maximumPoint() + minimumPoint()) * 0.5f; }
		Pairf maximumPoint() const noexcept{ return { x.max, y.max}; }

		// Return corners. 
		void getCorners(Pairf out[4]) {
			auto pn = minimumPoint();
			auto px = maximumPoint();
			auto dp = px - pn;
			Pairf dx = { dp.x, 0.f };
			Pairf dy = { 0.f, dp.y };

			out[0] = pn;
			out[1] = pn + dx;
			out[2] = pn + dx + dy;
			out[3] = pn + dy;
		}

		bool overlapsWithTolerance(const Span2f& s) const {
			return x.overlapsWithTolerance(s.x) && y.overlapsWithTolerance(s.y);
		}

		/** Return span that intersects this and input. Return false if the resulting span is empty.*/
		std::pair<Span2f, bool> intersect(const Span2f& in) const {
			auto xi = x.intersect(in.x);
			auto yi = y.intersect(in.y);
			return { {xi.first, yi.first}, xi.second && yi.second};
		}

		/** Expand the range symmetrically in each direction.*/
		Span2f expandSymmetric(float d) const { return { x.expandSymmetric(d), y.expandSymmetric(d)}; }

		Pairf diagonal() const {
			return maximumPoint() - minimumPoint();
		}

#if 0
		Span2f transformBy(const glm::mat4& mat) {
			glm::vec3 corners[8];
			GeometryTransform gt = { mat };
			getCorners(corners);
			for (int i = 0; i < 8; i++)
				corners[i] = gt.apply(corners[i]);

			Span2f res;
			Span2f::TryCreate(corners, corners + 8, res);
			return res;
		}
#endif

		static Span2f Create(const Pairf& a, const Pairf& b) {
			Spanf x = Spanf::Init(a.x).cover(b.x);
			Spanf y = Spanf::Init(a.y).cover(b.y);
			return { x,y};
		}

		static Span2f Init(const Pairf& v) {
			Span2f s;
			s.x = Spanf::Init(v.x);
			s.y = Spanf::Init(v.y);
			return s;
		}
		
		static Span2f Init(float xpos, float ypos) {
			Span2f s;
			s.x = Spanf::Init(xpos);
			s.y = Spanf::Init(ypos);
			return s;
		}

		static  GeometryResult TryCreate(const Pairf* begin, const Pairf* end, Span2f& result) {
			if (begin == end) {
				return GeometryResult::InputRangeEmpty;
			}

			Spanf x = Spanf::Init(begin[0].x);
			Spanf y = Spanf::Init(begin[0].y);

			while (begin != end) {
				x = x.cover(begin->x);
				y = y.cover(begin->y);
				begin++;
			}
			result = { x,y};
			return GeometryResult::Success;
		}

		static Pairf AbsoluteOf_(const Pairf& v) {
			return {fabsf(v.x), fabsf(v.y)};
		}

		static int LargestElement_(const Pairf& vIn) {
			Pairf v = AbsoluteOf_(vIn);
			float l = v.x;
			int i = 0;
			if (v.y > l) {
				l = v.y;
				i = 1;
			}

			return i;
		}

		static int SmallestElement_(const Pairf& vIn) {
			Pairf v = AbsoluteOf_(vIn);
			float l = v.x;
			int i = 0;
			if (v.y < l) {
				l = v.y;
				i = 1;
			}
			return i;
		}

		static GeometryResult TryExpandToMinimumRelativeThickness(Span2f bounds, const float minthick, Span2f& spanOut) {
			// Make sure bounds are not thin - sounds like a numerical problem waiting to happen

			Pairf boundsDiagonal = bounds.diagonal();

			const int maxdim = LargestElement_(boundsDiagonal);
			const int mindim = SmallestElement_(boundsDiagonal);

			const float maxv = fabs(boundsDiagonal[maxdim]);
			const float minv = fabs(boundsDiagonal[mindim]);
			float fac = minv / maxv;

			if (fac < minthick) {
				float expand = minthick * maxv;

				Dimension2D dimOut;
				GeometryResult geomRes = IntToDimension2D(mindim, dimOut);
				if (geomRes != GeometryResult::Success) {
					return geomRes;
				}
				bounds = bounds.expandDimSymmetric(dimOut, expand);
			}
			else {
				return GeometryResult::AreaCloseToZero;
			}

			spanOut = bounds;
			return GeometryResult::Success;
		}
	};
}

