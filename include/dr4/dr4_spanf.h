#pragma once

#include <dr4/dr4_floatingpoint.h>

#include <algorithm>

namespace dr4 {

	struct Spanf {
		float min, max;

		float length() const { return max - min; }

		bool contains(const Spanf& innerCandidate) const {
			return innerCandidate.min >= min && innerCandidate.min <= max && innerCandidate.max >= min && innerCandidate.max <= max;
		}

		bool contains(float d) const {
			return d >= min && d < max;
		}

		/** Expand range in both directions with the given amount. */
		Spanf expandSymmetric(float x) const { return { min - x, max + x }; }

		std::pair<Spanf, bool> intersect(const Spanf& s) const {
			float lower = std::max(min, s.min);
			float upper = std::min(max, s.max);

			Spanf res = { lower, upper };
			bool success = lower <= upper;
			return std::make_pair(res, success);
		}

		bool overlapsWithTolerance(const Spanf& s) const {
			return !(min > (s.max + Float64::GeometryEpsilon)
				|| max < (s.min - Float64::GeometryEpsilon));
		}

		/** Return span covering both.*/
		Spanf cover(const Spanf& s) const {
			float lower = std::min(min, s.min);
			float upper = std::max(max, s.max);
			return { lower, upper };
		}

		Spanf cover(float d) const {
			float nmin = std::min(min, d);
			float nmax = std::max(max, d);
			return{ nmin, nmax };
		}

		static Spanf Init(float p) {
			return{ p, p };
		}
	};

}
