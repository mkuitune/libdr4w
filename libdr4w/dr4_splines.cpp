
#if 0
#include <dr4/dr4_splines.h>

namespace dr4 {

	static void testInterpolation(
		std::vector<Pairf> points, size_t samplesPerSpan, size_t sampleCount) {
		PiecewiseSpline2 spline = Interpolate2(points, samplesPerSpan);
		LookUpTable<float> lut = spline.createByXLUT(sampleCount);

		LoopRange<float> rangeOver(sampleCount, lut.sourceStart(), lut.sourceEnd());
		for (auto& x : rangeOver) {

		}
	}
}
#endif