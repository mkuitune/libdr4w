//
// Geometric shape definitions
//
#pragma once

#include <dr4/dr4_tuples.h>

#include <vector>

namespace dr4 {

	struct Line2D {
		Pairf fst;
		Pairf snd;
	};

	// Interpreted as a set of points
	struct PointList2D {
		std::vector<Pairf> points;
	};

	// A _closed_ polygon drawn by connecting first and last point
	struct Polygon2D {
		PointList2D points;
		
		size_t size() const {
			return points.points.size();
		}

		const Pairf& operator[](size_t idx) const {
			return points.points[idx];
		}

		Line2D lineAt(size_t idx) const {
			size_t s = points.points.size();
			return { points.points[idx % s], points.points[(idx + 1) % s] };
		}

		struct LineIterator {
			const Polygon2D& poly;
			size_t idx = 0;
			Line2D operator*() {
				return poly.lineAt(idx);
			}
			void operator++() { idx++; }
			bool operator!=(const LineIterator& i) { return idx != i.idx; }
		};

		LineIterator begin() const { return { *this, 0 }; }
		LineIterator end() const { return { *this, size() }; }

	};



}
