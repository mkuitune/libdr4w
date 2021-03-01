#pragma once

#include <dr4/dr4_spanf.h>
#include <dr4/dr4_tuples.h>
#include <dr4/dr4_geometryresult.h>
#include <dr4/dr4_dimension.h>
#include <dr4/dr4_geometry_transform.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <stdexcept>

namespace dr4 {
	struct Span3f {
		Spanf x;
		Spanf y;
		Spanf z;

		Span3f cover(const glm::vec3& v) const {
			return { x.cover(v.x),
				y.cover(v.y),
				z.cover(v.z) };
		}
		
		Span3f cover(const Tripletf& v) const {
			return { x.cover(v.x),
				y.cover(v.y),
				z.cover(v.z) };
		}
		
		Span3f cover(float xpos, float ypos, float zpos) const {
			return { x.cover(xpos),
				y.cover(ypos),
				z.cover(zpos) };
		}
		
		Span3f cover(const Span3f& sIn) const {
			return { x.cover(sIn.x),
				y.cover(sIn.y),
				z.cover(sIn.z) };
		}

		bool contains(const glm::vec3& v) const {
			return x.contains(v.x) && y.contains(v.y) && z.contains(v.z);
		}

		Span3f expandXsymmetric(float d) const { return { x.expandSymmetric(d), y, z }; }
		Span3f expandYsymmetric(float d) const { return { x, y.expandSymmetric(d), z }; }
		Span3f expandZsymmetric(float d) const { return { x, y, z.expandSymmetric(d) }; }
		Span3f expandDimSymmetric(Dimension dim, float d) const {
			switch (dim) {
			case Dimension::X: return expandXsymmetric(d);
			case Dimension::Y: return expandYsymmetric(d);
			case Dimension::Z: return expandZsymmetric(d);
			default: return *this;
			}
		}

		glm::vec3 minimumPoint() const noexcept { return { x.min, y.min, z.min }; }
		glm::vec3 center() const noexcept { return (maximumPoint() + minimumPoint()) * 0.5f; }
		glm::vec3 maximumPoint() const noexcept{ return { x.max, y.max, z.max }; }

		// Return corners. 
		void getCorners(glm::vec3 out[8]) {
			auto pn = minimumPoint();
			auto px = maximumPoint();
			auto dp = px - pn;
			auto dx = glm::vec3(dp.x, 0.f, 0.f);
			auto dy = glm::vec3(0.f, dp.y, 0.f);
			auto dz = glm::vec3(0.f, 0.f, dp.z);

			out[0] = pn;
			out[1] = pn + dx;
			out[2] = pn + dx + dy;
			out[3] = pn + dy;
			
			out[4] = out[0] + dz;
			out[5] = out[1] + dz;
			out[6] = out[2] + dz;
			out[7] = out[3] + dz;
		}

		bool overlapsWithTolerance(const Span3f& s) const {
			return x.overlapsWithTolerance(s.x) &&
				y.overlapsWithTolerance(s.y) && z.overlapsWithTolerance(s.z);
		}

		/** Return span that intersects this and input. Return false if the resulting span is empty.*/
		std::pair<Span3f, bool> intersect(const Span3f& in) const {
			auto xi = x.intersect(in.x);
			auto yi = y.intersect(in.y);
			auto zi = z.intersect(in.z);
			return { {xi.first, yi.first, zi.first}, xi.second && yi.second && zi.second };
		}

		/** Expand the range symmetrically in each direction.*/
		Span3f expandSymmetric(float d) const { return { x.expandSymmetric(d), y.expandSymmetric(d), z.expandSymmetric(d) }; }

		glm::vec3 diagonal() const {
			return maximumPoint() - minimumPoint();
		}

		Span3f transformBy(const glm::mat4& mat) {
			glm::vec3 corners[8];
			GeometryTransform gt = { mat };
			getCorners(corners);
			for (int i = 0; i < 8; i++)
				corners[i] = gt.apply(corners[i]);

			Span3f res;
			Span3f::TryCreate(corners, corners + 8, res);
			return res;
		}

		static Span3f Create(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
			Spanf x = Spanf::Init(a.x).cover(b.x).cover(c.x);
			Spanf y = Spanf::Init(a.y).cover(b.y).cover(c.y);
			Spanf z = Spanf::Init(a.z).cover(b.z).cover(c.z);
			return { x,y,z };
		}

		static Span3f Create(const glm::vec3& a, const glm::vec3& b) {
			Spanf x = Spanf::Init(a.x).cover(b.x);
			Spanf y = Spanf::Init(a.y).cover(b.y);
			Spanf z = Spanf::Init(a.z).cover(b.z);
			return { x,y,z };
		}

		static Span3f Init(const glm::vec3& v) {
			Span3f s;
			s.x = Spanf::Init(v.x);
			s.y = Spanf::Init(v.y);
			s.z = Spanf::Init(v.z);
			return s;
		}

		static Span3f Init(const Tripletf& v) {
			Span3f s;
			s.x = Spanf::Init(v.x);
			s.y = Spanf::Init(v.y);
			s.z = Spanf::Init(v.z);
			return s;
		}
		
		static Span3f Init(float xpos, float ypos, float zpos) {
			Span3f s;
			s.x = Spanf::Init(xpos);
			s.y = Spanf::Init(ypos);
			s.z = Spanf::Init(zpos);
			return s;
		}

		static  GeometryResult TryCreate(const glm::vec3* begin, const glm::vec3* end, Span3f& result) {
			if (begin == end) {
				return GeometryResult::InputRangeEmpty;
			}

			Spanf x = Spanf::Init(begin[0].x);
			Spanf y = Spanf::Init(begin[0].y);
			Spanf z = Spanf::Init(begin[0].z);

			while (begin != end) {
				x = x.cover(begin->x);
				y = y.cover(begin->y);
				z = z.cover(begin->z);
				begin++;
			}
			result = { x,y,z };
			return GeometryResult::Success;
		}

		static glm::vec3 AbsoluteOf_(const glm::vec3& v) {
			return {fabsf(v.x), fabsf(v.y), fabsf(v.z)};
		}
		static int LargestElement_(const glm::vec3& vIn) {
			glm::vec3 v = AbsoluteOf_(vIn);
			float l = v.x;
			int i = 0;
			if (v.y > l) {
				l = v.y;
				i = 1;
			}
			if (v.z > l) {
				l = v.z;
				i = 2;
			}
			return i;
		}

		static int SmallestElement_(const glm::vec3& vIn) {
			glm::vec3 v = AbsoluteOf_(vIn);
			float l = v.x;
			int i = 0;
			if (v.y < l) {
				l = v.y;
				i = 1;
			}
			if (v.z < l) {
				l = v.z;
				i = 2;
			}
			return i;
		}

		static GeometryResult TryExpandToMinimumRelativeThickness(Span3f bounds, const float minthick, Span3f& spanOut) {
			// Make sure bounds are not thin - sounds like a numerical problem waiting to happen

			glm::vec3 boundsDiagonal = bounds.diagonal();

			const int maxdim = LargestElement_(boundsDiagonal);
			const int mindim = SmallestElement_(boundsDiagonal);

			const float maxv = fabs(boundsDiagonal[maxdim]);
			const float minv = fabs(boundsDiagonal[mindim]);
			float fac = minv / maxv;

			if (fac < minthick) {
				float expand = minthick * maxv;

				Dimension dimOut;
				GeometryResult geomRes = IntToDimension(mindim, dimOut);
				if (geomRes != GeometryResult::Success) {
					return geomRes;
				}
				bounds = bounds.expandDimSymmetric(dimOut, expand);
			}
			else {
				return GeometryResult::VolumeCloseToZero;
			}

			spanOut = bounds;
			return GeometryResult::Success;
		}
	};
}

