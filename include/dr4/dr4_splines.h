#pragma once

#include <dr4/dr4_math.h>
#include <dr4/dr4_tuples.h>
#include <dr4/dr4_result_types.h>
#include <dr4/dr4_metadata.h>

#include <optional>
#include <vector>
#include <map>

namespace dr4 {

	// centripetal catmull-rom - points between p1 -> p2
	// the points are not distributed evenly on the curve
	//https://qroph.github.io/2018/07/30/smooth-paths-using-catmull-rom-splines.html
	// https://en.wikipedia.org/wiki/Centripetal_Catmull%E2%80%93Rom_spline
	// can extend this to 3 and 4 dims if needed
	// https://stackoverflow.com/questions/9489736/catmull-rom-curve-with-no-cusps-and-no-self-intersections/23980479#23980479
	// note see watt&watt if distance space interpolation is needed
#if 0
	struct ccatmullrom2_t {
		const float t0;
		const float t1;
		const float t2;
		const float t3;
		const float t10;
		const float t21;
		const float t32;
		const float t20;
		const float t31;
		const Pairf p0;
		const Pairf p1;
		const Pairf p2;
		const Pairf p3;

		Pairf eval(float u) const {
			float t = lerp(t1, t2, u);
			Pairf A1 = p0 * ((t1 - t) / t10) + p1 * ((t - t0) / t10);
			Pairf A2 = p1 * ((t2 - t) / t21) + p2 * ((t - t1) / t21);
			Pairf A3 = p2 * ((t3 - t) / t32) + p3 * ((t - t2) / t32);

			Pairf B1 = A1 * ((t2 - t) / t20) + A2 * ((t - t0) / t20);
			Pairf B2 = A2 * ((t3 - t) / t31) + A3 * ((t - t1) / t31);

			return B1 * ((t2 - t) / (t2 - t1)) + B2 * ((t - t1) / (t2 - t1));
		}

		static ccatmullrom2_t Create(const Pairf& p0, const Pairf& p1, const Pairf& p2, const Pairf& p3) {
			const static float a2 = 0.25f; // sqrt(...) ^ 0.5 (for centripetal catmull-rom)
			float p01 = powf((p1 - p0).norm2(), 0.25f);
			float p12 = powf((p2 - p1).norm2(), 0.25f);
			float p23 = powf((p3 - p2).norm2(), 0.25f);
			float t0 = 0.f;
			float t1 = p01 + t0;
			float t2 = p12 + t1;
			float t3 = p23 + t2;
			float t10 = t1 - t0;
			float t21 = t2 - t1;
			float t32 = t3 - t2;
			float t20 = t2 - t0;
			float t31 = t3 - t1;

			return { t0, t1, t2, t3, t10, t21, t32, t20, t31, p0, p1, p2, p3 };
		}
	};
#endif

	template<class REAL>
	struct LoopRange{
		size_t endIdx;
		REAL delta;
		REAL valueStart;
		REAL valueEnd;

		LoopRange(size_t samples, REAL first, REAL last) {
			endIdx = samples;
			delta = (last - first) / (samples - 1);
			valueStart = first;
			valueEnd = last;
		}

		size_t size() const {
			return endIdx;
		}

		struct iterator {
			size_t i;
			REAL val;
			REAL delta;

			iterator operator++() {
				i++;
				val += delta;
				return *this;
			}

			bool operator!=(const iterator& rhs) const {
				return i != rhs.i;
			}

			REAL& operator*() {
				return val;
			}
		};

		iterator begin() const { return { 0, valueStart, delta }; }
		iterator end() const { return{ endIdx, valueEnd, delta }; }
	};

	template<class VEC_T>
	class LookUpTable {
	public:
		typedef float Real_t;

	private:
		std::vector<VEC_T> m_samples;
		Real_t m_rangeStart;
		Real_t m_rangeEnd;
		Real_t m_length;
		Real_t m_count;
	public:


		const std::vector<VEC_T>& getData() const { return m_samples; }

		VEC_T getNearest(Real_t x) const {
			if (x < m_rangeStart)
				return m_samples.front();
			else if (x >= m_rangeEnd)
				return m_samples.back();

			size_t idx = (size_t) floorf(m_count * ((x - m_rangeStart) / m_length) );

			return m_samples[idx];
		}

		std::vector<std::pair<Real_t, VEC_T>> getSamples() const { 
			std::vector<std::pair<Real_t, VEC_T>> res;
			Real_t dx = (m_rangeEnd - m_rangeStart) / (m_samples.size() - 1);
			Real_t x = m_rangeStart;
			for (auto s : m_samples) {
				res.push_back({x, s});
				x += dx;
			}
			return res;
		}

		// Source domain start
		Real_t sourceStart() const { return m_rangeStart; }
		// Source domain end
		Real_t sourceEnd() const { return m_rangeEnd; }

		bool empty()const { return m_samples.empty(); }

		static LookUpTable CreateEmpty() {
			std::vector<VEC_T> samples;
			return { samples, 0, 0, 0, 0};
		}

		static LookUpTable Create(std::vector<VEC_T> samples, Real_t start, Real_t end) {
			Real_t length = end - start;
			Real_t count = (Real_t)samples.size();
			LookUpTable table;
			table.m_samples = samples;
			table.m_rangeStart = start;
			table.m_rangeEnd = end;
			table.m_length = length;
			table.m_count = count;
			return table;
		}
	};

	// Piecewise 2d lines
	struct linearspan2_t {
		Pairf start;
		Pairf end;
		float k;
		float c;
		float evalAt(float x) const {
			return k * x + c;
		}
		float guardedEvalAt(float x) const {
			if (x < start.x) return start.y;
			else if (x >= end.x) return end.y;
			return evalAt(x);
		}
		static linearspan2_t Create(Pairf valueStart, Pairf valueEnd) {
			auto diff = valueEnd - valueStart;
			float k = diff.y / diff.x;
			float c = valueStart.y - k * valueStart.x;
			return { valueStart, valueEnd, k, c };
		}
		static std::vector<linearspan2_t> CreateSpline(const std::vector<Pairf>& points) {
			size_t last = points.size() - 1;
			std::vector<linearspan2_t> spans;
			for (size_t i = 0; i < last; i++) {
				spans.push_back(linearspan2_t::Create(points[i], points[i + 1]));
			}
			return spans;
		}
	};

	struct PiecewiseSpline2 {
		std::vector<linearspan2_t> m_spans;

		float evalAt(float x) const {
			if (x < m_spans[0].start.x)
				return m_spans[0].start.y;
			for (auto& s : m_spans) {
				if (x < s.end.x)
					return s.evalAt(x);
			}
			return m_spans.back().end.y;
		}

		float sourceStart() const { return m_spans.front().start.x; }
		float sourceEnd() const { return m_spans.back().end.x; }

		std::vector<Pairf> toPoints() const {
			std::vector<Pairf> res;
			for (const auto& span : m_spans) res.push_back(span.start);
			res.push_back(m_spans.back().end);
			return res;
		}

		LookUpTable<float> createByXLUT(size_t sampleCount) const{ 
			const float dist = (m_spans.back().end.x - m_spans.front().start.x);
			float delta = dist / (sampleCount - 1);
			const float start = 0;
			std::vector<float> samples;
			for (size_t i = 0; i < sampleCount; i++) {
				float  x = start + ((float)i) * delta;
				samples.push_back(evalAt(x));
			}
			return LookUpTable<float>::Create(samples, start, dist);
		}

		static PiecewiseSpline2 Create(const std::vector<Pairf>& points) {
			return {linearspan2_t::CreateSpline(points)};
		}
	};

	// Piecewise 3d lines with distance metric attached
	struct linearspan3_t {
		float start; // distance along
		float end;
		float length;
		Tripletf valueStart;
		Tripletf valueEnd;
		Tripletf diff; // valueEnd - valueStart

		Tripletf evalAt(float dist) const {
			float u = (dist - start) / length;
			return valueStart + (diff * u);
		}
		
		Tripletf guardedEvalAt(float dist) const {
			if (dist < start) return valueStart;
			else if (dist >= end) return valueEnd;
			return evalAt(dist);
		}

		static linearspan3_t Create(float start, Tripletf valueStart, Tripletf valueEnd) {
			auto diff = valueEnd - valueStart;
			float length = diff.norm();
			float end = start + length;
			return { start, end, length, valueStart, valueEnd, diff };
		}

		static std::vector<linearspan3_t> CreateSpline(const std::vector<Tripletf>& points) {
			std::vector<linearspan3_t> res;
			float dist = 0;
			size_t last = points.size() - 1;
			for (size_t i = 0; i < last; i++) {
				auto span = linearspan3_t::Create(dist, points[i], points[i + 1]);
				dist = span.end;
				res.push_back(span);
			}
			return res;
		}
	};

	class PiecewiseSpline3 {
	public:
		std::vector<linearspan3_t> m_spans;

		Tripletf evalAt(float x) const {
			if (x < m_spans[0].start)
				return m_spans[0].valueStart;
			else if (x >= m_spans.back().end)
				return m_spans.back().valueEnd;
			for (const auto& span : m_spans) {
				if (x <= span.end)
					return span.evalAt(x);
			}
			return m_spans.back().valueEnd; // Never should reach this line
		}

		LookUpTable<Tripletf> CreateByDistanceLUT(size_t sampleCount) const{ 
			const float dist = m_spans.back().end;
			float delta = dist / (sampleCount - 1);
			const float start = 0;
			std::vector<Tripletf> samples;
			for (size_t i = 0; i < sampleCount; i++) {
				float  x = start + ((float)i) * delta;
				samples.push_back(evalAt(x));
			}
			return LookUpTable<Tripletf>::Create(samples, start, dist);
		}

		static PiecewiseSpline3 Create(const std::vector<Tripletf>& points) {
			return { linearspan3_t::CreateSpline(points) };
		}
	};

	static float VecNorm2(const Pairf& p) { return p.norm2(); }

	struct SplineCatmullRom2 {
		const Pairf p0;
		const Pairf p1;
		const Pairf p2;
		const Pairf p3;

		float GetT(float t, float alpha, const Pairf & p0, const Pairf & p1) const
		{
			auto d = p1 - p0;
			float a = d.dot(d);
			float b = pow(a, alpha * .5f);
			return (b + t);
		}

		Pairf CatMullRom(float t /* between 0 and 1 */, float alpha = .5f /* between 0 and 1 */)const
		{
			float t0 = 0.0f;
			float t1 = GetT(t0, alpha, p0, p1);
			float t2 = GetT(t1, alpha, p1, p2);
			float t3 = GetT(t2, alpha, p2, p3);

			t = lerp(t1, t2, t);

			Pairf A1 = p0 * ((t1 - t) / (t1 - t0)) + p1 * ((t - t0) / (t1 - t0));
			Pairf A2 = p1 * ((t2 - t) / (t2 - t1)) + p2 * ((t - t1) / (t2 - t1));
			Pairf A3 = p2 * ((t3 - t) / (t3 - t2)) + p3 * ((t - t2) / (t3 - t2));
			Pairf B1 = A1 * ((t2 - t) / (t2 - t0)) + A2 * ((t - t0) / (t2 - t0));
			Pairf B2 = A2 * ((t3 - t) / (t3 - t1)) + A3 * ((t - t1) / (t3 - t1));
			Pairf C =  B1 * ((t2 - t) / (t2 - t1)) + B2 * ((t - t1) / (t2 - t1));
			return C;
		}
		Pairf eval(float u) const {
			return CatMullRom(u, 0.5f);
		}

		static SplineCatmullRom2 Create(const Pairf& p0, const Pairf& p1, const Pairf& p2, const Pairf& p3) {
			return { p0, p1, p2, p3 };
		}
	};

	struct SplineBezierCubic {
		
		const Pairf p0;
		const Pairf p1;
		const Pairf p2;
		const Pairf p3;

		Pairf BezierCubic(float t) const {
			float it = 1.f - t;
			float itt = it * it;
			float ittt = itt * it;
			float tt = t * t;
			float ttt = t * t * t;
			return p0 * ittt
				+ p1 * (3 * t * itt)
				+ p2 * (3.f * tt * it)
				+ p3 * ttt;
		}

		Pairf eval(float u) const {
			return BezierCubic(u);
		}


		//
		// Builder helpers
		//

	private:
		// get normal for b for points a .. b .. c . Here the input vectors ab = b-a and cb = b-c are already computed
		static GeometryResultPairf getNormal(Pairf ab, Pairf b, Pairf cb) {
			auto ablen = ab.norm();
			auto cblen = cb.norm();

			auto ac = ab - cb;
			auto aclen = ac.norm();

			// This is the only condition where we really can't proceed - 2 or 3 points close to same
			if (Float32::GeometryIsCloseToZero(ablen) || Float32::GeometryIsCloseToZero(cblen)|| Float32::GeometryIsCloseToZero(aclen))
				return GeometryResultPairf::Error(GeometryResult::AreaCloseToZero);

			ab = ab / ablen;
			cb = cb / cblen;

			auto n = ab + cb;
			auto nlen = n.norm();

			if (Float32::GeometryIsCloseToZero(nlen) ) {
				// lines colinear, exract normal from the tangent
				Pairf normal = { -ab.y, ab.x };
				return GeometryResultPairf::Success(normal);
			}
			else {
				n = n / nlen;
				auto sign = ac.kross(ab) > 0.f ? 1.f : -1.f; // we want the normal always point "left" along the path so that 
															 // forward and backward tangent always can be computed the same way
				auto nout = n * sign;
				return GeometryResultPairf::Success(nout);
			}
		}

		static Pairf backTangential(const Pairf& normal, float scale) {
			Pairf b = {-normal.y, normal.x};
			return b * scale;
		}
		static Pairf foreTangential(const Pairf& normal, float scale) {
			Pairf f = {normal.y, -normal.x};
			return f * scale;
		}

		static constexpr float k() { return 0.5f; }
		// the curve passes through a, [b, c],  d - get tangent points fst, snd
		// a ...b->fst ... snd <- c .... d
		static GeometryResultHandles2D constructInterpolants(Pairf a, Pairf b, Pairf c, Pairf d) {

			auto ab = b - a;
			auto cb = b - c;
			auto bc = c - b;
			auto dc = c - d;

			auto bclen = bc.norm();
			// If b == c, just use b as point. Remember this is just for interval (b,c)
			if (Float32::GeometryIsCloseToZero(bclen)) {
				// TODO: Is a singularity acceptable result? It's mathematically correct at least.
				return GeometryResultHandles2D::Success({ b, c }); // If b == c, all the points are in single spot
			}

			Pairf fst;
			Pairf snd;

			// First point a..b..[fst]..snd..c..d
			auto nB = getNormal(ab, b, cb);
			if (nB) {
				fst = b + foreTangential(nB.value(), bclen * k());
			}
			else {
				// can't compute tangent. Pick a point in the line from b to c (since b != c)
				fst = b + (bc * k()); // just pick a point in line
			}

			// Second point a..b..fst..[snd]..c...d
			auto nC = getNormal(bc, c, dc);
			if (nC) {
				snd = c + backTangential(nC.value(), bclen * k());
			}
			else {
				snd = c + (cb * k()); // just pick a point in line
			}

			return GeometryResultHandles2D::Success({fst, snd}); // If b == c, all the points are in single spot

		}

		// the curve passes through a, b, c - get tangent points fst, snd
		// a->fst ... snd <- b ... c
		// The user must give the initial normalized direction for the point fst from a (the length is computed automatically
		// dirstart can be {0.f, 0.f}

		static Pairf offsetOrOrigin(Pairf p, std::optional<NormalizedPairf> dir, float len) {
			if (!dir || !(dir.value().isValid())) {
				return p;
			}
			auto d = dir.value().value();
			return p + d * len;

		}

		static GeometryResultHandles2D  constructInterpolantsFirst(std::optional<NormalizedPairf> dirStart, Pairf a, Pairf b, Pairf c) {
			auto ab = b - a;
			auto cb = b - c;

			auto ablen = ab.norm();
			// If b == c, just use b as point. Remember this is just for interval (b,c)
			if (Float32::GeometryIsCloseToZero(ablen)) {
				// TODO: Is a singularity acceptable result? It's mathematically correct at least.
				return GeometryResultHandles2D::Success({ a, b }); // If b == c, all the points are in single spot
			}
			auto len = ablen * k();

			Pairf fst;
			Pairf snd;
			
			// First point a .. [fst] .. snd .. b .. c
			fst = offsetOrOrigin(a, dirStart, len);

			// Second point a .. fst .. [snd] .. b .. c
			auto nB = getNormal(ab, b, cb);
			if (nB) {
				snd = b + backTangential(nB.value(), len);
			}
			else {
				snd = b - (ab * k()); // just pick a point in line
			}

			return GeometryResultHandles2D::Success({fst, snd}); // If b == c, all the points are in single spot
		}
	
		// a ... b .. fst .. snd .. c
		static GeometryResultHandles2D  constructInterpolantsLast(std::optional<NormalizedPairf> dirLast, Pairf a, Pairf b, Pairf c) {
			auto ab = b - a;
			auto cb = b - c;
			auto bc = c - b;

			auto ablen = ab.norm();
			// If b == c, just use b as point. Remember this is just for interval (b,c)
			if (Float32::GeometryIsCloseToZero(ablen)) {
				// TODO: Is a singularity acceptable result? It's mathematically correct at least.
				return GeometryResultHandles2D::Success({ a, b }); // If b == c, all the points are in single spot
			}
			auto len = ablen * k();

			Pairf fst;
			Pairf snd;
		
			snd = offsetOrOrigin(c, dirLast, len);

			// First point a .. b .. [fst] .. snd .. c
			auto nB = getNormal(ab, b, cb);
			if (nB) {
				fst = b + foreTangential(nB.value(), len);
			}
			else {
				fst = b + (bc * k()); // just pick a point in line
			}

			return GeometryResultHandles2D::Success({fst, snd}); // If b == c, all the points are in single spot
		}

	public:
		static SplineBezierCubic Create(const Pairf& p0, const Pairf& p1, const Pairf& p2, const Pairf& p3) {
			return { p0, p1, p2, p3 };
		}

		typedef TypedResult<GeometryResult, std::vector<SplineBezierCubic>> GeometryResultBezier;

		static GeometryResultBezier InterpolatePoints(const std::vector<Pairf>& points, std::optional<Pairf> firstDirection = std::nullopt, std::optional<Pairf> lastDirection = std::nullopt);

	};

	PiecewiseSpline2 Interpolate2(std::vector<Pairf> points, size_t samplesPerSpan);

	PiecewiseSpline2 Interpolate2Smooth(std::vector<Pairf> points, size_t samplesPerSpan);
	
	PiecewiseSpline2 Interpolate2Bezier(std::vector<Pairf> points, size_t samplesPerSpan);


}
