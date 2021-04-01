
#include <dr4/dr4_splines.h>

namespace dr4 {

#if 0
	static void testInterpolation(
		std::vector<Pairf> points, size_t samplesPerSpan, size_t sampleCount) {
		PiecewiseSpline2 spline = Interpolate2(points, samplesPerSpan);
		LookUpTable<float> lut = spline.createByXLUT(sampleCount);

		LoopRange<float> rangeOver(sampleCount, lut.sourceStart(), lut.sourceEnd());
		for (auto& x : rangeOver) {

		}
	}

#endif

	PiecewiseSpline2 Interpolate2(std::vector<Pairf> points, size_t samplesPerSpan) {
		// catmull-rom needs additional point at start and end to evaluate the tangents there
		std::vector<Pairf> samplesOut;
		size_t nSplines = points.size() - 1;
		size_t lastPointIdx = nSplines - 1;
		size_t lastData = points.size() - 1;

		Pairf virtualStart = points[0] - Pairf{ (points[1] - points[0]).x, points[0].y };
		Pairf virtualEnd = points[lastData] + ((points[lastData] - points[lastData - 1]));

		//Pairf virtualStart = points[0] - ((points[1] - points[0]).normalized());
		//Pairf virtualEnd = points[lastData] + ((points[lastData] - points[lastData - 1]).normalized());

		//Pairf virtualStart = points[0] - Pairf{(points[1] - points[0]).x, points[0].y};
		//Pairf virtualEnd = points[lastData] + ((points[lastData] - points[lastData - 1]).normalized());

		const float du = 1.0f / (samplesPerSpan - 1); // don't eval the last point except in the last spline

		for (size_t i = 0; i < nSplines; i++) {
			Pairf p0 = (i == 0) ? virtualStart : points[i - 1];
			Pairf p1 = points[i];
			Pairf p2 = points[i + 1];
			Pairf p3 = (i == lastPointIdx) ? virtualEnd : points[i + 2];
			SplineCatmullRom2 spline = SplineCatmullRom2::Create(p0, p1, p2, p3);

			// don't eval the last point except in the last spline
			size_t lastSampleIdx = (i == lastPointIdx ? samplesPerSpan : samplesPerSpan - 1);
			float u = 0.f;
			for (size_t j = 0; j < lastSampleIdx; j++) {
				samplesOut.push_back(spline.eval(u));
				u += du;
			}
		}
		return PiecewiseSpline2::Create(samplesOut);
	}
}

