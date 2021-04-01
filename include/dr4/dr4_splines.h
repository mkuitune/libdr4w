#pragma once

#include <dr4/dr4_math.h>
#include <dr4/dr4_tuples.h>

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
#if 0
	struct CatmullRomComponents {
		const float t0;
		const float t1;
		const float t2;
		const float t3;
		const float t10;
		const float t21;
		const float t32;
		const float t20;
		const float t31;
	
		template<class VEC_T>
		VEC_T eval(float u, const VEC_T& p0, const VEC_T& p1, const VEC_T& p2, const VEC_T& p3) const {
			float t = lerp(t1, t2, u);
			VEC_T A1 = p0 * ((t1 - t) / t10) + p1 * ((t - t0) / t10);
			VEC_T A2 = p1 * ((t2 - t) / t21) + p2 * ((t - t1) / t21);
			VEC_T A3 = p2 * ((t3 - t) / t32) + p3 * ((t - t2) / t32);

			VEC_T B1 = A1 * ((t2 - t) / t20) + A2 * ((t - t0) / t20);
			VEC_T B2 = A2 * ((t3 - t) / t31) + A3 * ((t - t1) / t31);

			return B1 * ((t2 - t) / (t2 - t1)) + B2 * ((t - t1) / (t2 - t1));
		}

		// normaij = VecNorm2(v_i - v_j)
		static CatmullRomComponents Create(float norm01, float norm12, float norm23) {
			const static float a2 = 0.25f; // sqrt(...) ^ 0.5 (for centripetal catmull-rom)
			float p01 = powf(norm01, 0.25f);
			float p12 = powf(norm12, 0.25f);
			float p23 = powf(norm23, 0.25f);
			float t0 = 0.f;
			float t1 = p01 + t0;
			float t2 = p12 + t1;
			float t3 = p23 + t2;
			float t10 = t1 - t0;
			float t21 = t2 - t1;
			float t32 = t3 - t2;
			float t20 = t2 - t0;
			float t31 = t3 - t1;
			return { t0, t1, t2, t3, t10, t21, t32, t20, t31};
		}
	};

	struct SplineCatmullRom2 {
		CatmullRomComponents comp;
		const Pairf p0;
		const Pairf p1;
		const Pairf p2;
		const Pairf p3;

		Pairf eval(float u) const {
			return comp.eval(u, p0, p1, p2, p3);
		}

		static SplineCatmullRom2 Create(const Pairf& p0, const Pairf& p1, const Pairf& p2, const Pairf& p3) {
			auto comp = CatmullRomComponents::Create((p1 - p0).norm2(), (p2 - p1).norm2(), (p3 - p1).norm2());
			return { comp, p0, p1, p2, p3 };
		}
	};
#endif

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
			float it2 = it * it;
			float it3 = it2 * it;
			float t2 = t * t;
			float t3 = t * t * t;
			return p0 * it3 
				 + p1 * (3 * t * it2)
				 + p2 * (3.f * t2 * it)
				 + p3 * t3;
		}

		Pairf eval(float u) const {
			return BezierCubic(u);
		}


		// the curve passes through a, b, c - get tangent points fst, snd
		// a->fst ... snd <- b .... c
		//void constructInterpolants(Pairf a, Pairf b, Pairf c, Pairf& fst, Pairf& snd) {
		//	const float thr = 1.f / 3.f;
		//	auto ac = c - a;
		//	auto nilb = c - a;
		//	fst = b + (ac * thr);
		//	snd = b - (bd * thr);
		//}
		
		// the curve passes through a, b, c d - get tangent points fst, snd
		// a ...b->fst ... snd <- c .... d
		void constructInterpolants(Pairf a, Pairf b, Pairf c, Pairf d, Pairf& fst, Pairf& snd) {
			const float thr = 1.f / 3.f;
			auto ac = c - a;
			auto bd = d - b;
			fst = b + (ac * thr);
			snd = b - (bd * thr);
		}

		static SplineBezierCubic Create(const Pairf& p0, const Pairf& p1, const Pairf& p2, const Pairf& p3) {
			return { p0, p1, p2, p3 };
		}

	};

#if 0
	struct SplineCatmullRom3 {
		CatmullRomComponents comp;
		const Tripletf p0;
		const Tripletf p1;
		const Tripletf p2;
		const Tripletf p3;



		Tripletf eval(float u) const {
			return comp.eval(u, p0, p1, p2, p3);
		}

		static SplineCatmullRom3 Create(const Tripletf& p0, const Tripletf& p1, const Tripletf& p2, const Tripletf& p3) {
			auto comp = CatmullRomComponents::Create((p1 - p0).norm2(), (p2 - p1).norm2(), (p3 - p1).norm2());
			return { comp, p0, p1, p2, p3 };
		}
	};
#endif


#if 0
	// centripetal catmull-rom - points between p1 -> p2
	Pairf splinecatmullrom2(const Pairf& p0, const Pairf& p1, const Pairf& p2, const Pairf& p3, float u) {
		SplineCatmullRom2 spline = SplineCatmullRom2::Create(p0, p1, p2, p3);
		return spline.eval(u);
	}
#endif

	PiecewiseSpline2 Interpolate2(std::vector<Pairf> points, size_t samplesPerSpan);

	PiecewiseSpline2 Interpolate2Smooth(std::vector<Pairf> points, size_t samplesPerSpan);
#if 0
	inline PiecewiseSpline3 Interpolate3(std::vector<Tripletf> points, size_t samplesPerSpan) {

		std::vector<Tripletf> samples;
		// catmull-rom needs additional point at start and end to evaluate the tangents there
		size_t nSplines = samples.size() - 2;
		size_t last = nSplines - 1;
		Tripletf virtualStart = samples[0] - (samples[1] - samples[0]);
		Tripletf virtualEnd = samples[last] + (samples[last] - samples[last - 1]);

		const float du = 1.0f / (samplesPerSpan - 1); // don't eval the last point except in the last spline

		for (size_t i = 0; i < nSplines; i++) {
			Tripletf p0 = (i == 0) ? virtualStart : points[i - 1];
			Tripletf p1 = points[i];
			Tripletf p2 = points[i + 1];
			Tripletf p3 = (i == last) ? virtualEnd : points[i + 2];
			SplineCatmullRom3 spline = SplineCatmullRom3::Create(p0, p1, p2, p3);
			
			// don't eval the last point except in the last spline
			size_t lastSampleIdx = i == last ? samplesPerSpan : samplesPerSpan - 1;
			float u = 0.f;
			for (size_t j = 0; j < lastSampleIdx; j++) { 
				u += du;
				samples.push_back(spline.eval(u));
			}
		}
		return PiecewiseSpline3::Create(samples);
	}
#endif

#if 0
	template<class VEC_T>
	PiecewiseSpline<VEC_T> InterpolateToPiecewise(
		const std::vector<VEC_T>& samples, size_t sampleCount, int lookupDim
	) {
		// If there are less than 3 points just linear interpolate them;
		PiecewiseSplineBuilder<VEC_T> builder;
		if (samples.count() < 2) {
			return LookUpTable<VEC_T>::CreateEmpty();
		}
		if (samples.count() < 3)
		{
			builder.add(samples[0], samples[0])
		}

		// catmull-rom needs additional point at start and end to evaluate the tangents there
		size_t last = samples.size() - 1;
		VEC_T virtualStart = samples[0] - (samples[1] - samples[0]);
		VEC_T virtualEnd = samples[last] + (samples[last] - samples[last - 1])

	}
#endif

}
