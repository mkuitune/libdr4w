
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

		//Pairf virtualStart = points[0] - Pairf{ (points[1] - points[0]).x, points[0].y };
		//Pairf virtualEnd = points[lastData] + ((points[lastData] - points[lastData - 1]));

		//Pairf virtualStart = points[0] - (points[1] - points[0]);
		//Pairf virtualEnd = points[lastData] + (points[lastData] - points[lastData - 1]);

		//Pairf virtualStart = points[0] - ((points[1] - points[0]).normalized());
		//Pairf virtualEnd = points[lastData] + ((points[lastData] - points[lastData - 1]).normalized());
	

		//Pairf v0 = ((points[1] - points[0]));
		//Pairf virtualStart = points[0] - Pairf{v0.x/10.f,0.f};
		//Pairf v1 = ((points[lastData] - points[lastData - 1]));
		//Pairf virtualEnd = points[lastData] + Pairf{v1.x/10.f, 0.f};
		
		Pairf v0 = ((points[1] - points[0]));
		Pairf virtualStart = points[0] - (v0*0.1f);
		Pairf v1 = ((points[lastData] - points[lastData - 1]));
		Pairf virtualEnd = points[lastData] + (v1 * 0.1f);

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

	PiecewiseSpline2 Interpolate2Smooth(std::vector<Pairf> points, size_t samplesPerSpan) {
		// Just use smoothstep to interpolate between points
		std::vector<Pairf> samplesOut;
		size_t nSplines = points.size() - 1;
		size_t lastPointIdx = nSplines - 1;

		const float du = 1.0f / (samplesPerSpan - 1); // don't eval the last point except in the last spline

		for (size_t i = 0; i < nSplines; i++) {
			Pairf p1 = points[i];
			Pairf p2 = points[i + 1];

			// don't eval the last point except in the last spline
			size_t lastSampleIdx = (i == lastPointIdx ? samplesPerSpan : samplesPerSpan - 1);
			float u = 0.f;
			for (size_t j = 0; j < lastSampleIdx; j++) {
				float t = smoothstepf(u);
				float x = lerp(p1.x, p2.x, u);
				float y = lerp(p1.y, p2.y, t);
				Pairf p = { x,y };
				samplesOut.push_back(p);
				u += du;
			}
		}
		return PiecewiseSpline2::Create(samplesOut);
	}

	PiecewiseSpline2 Interpolate2Bezier(std::vector<Pairf> points, size_t samplesPerSpan){
		// 1. Points to bezier spline
		// 2. eval bezier splines

		auto bezierResult = SplineBezierCubic::InterpolatePoints(points);
		if (!bezierResult)
			return {};

		auto beziers = bezierResult.value();

		std::vector<Pairf> samplesOut;
		size_t nSplines = beziers.size();
		size_t lastPointIdx = nSplines - 1;
		size_t lastData = points.size() - 1;

		const float du = 1.0f / (samplesPerSpan - 1); // don't eval the last point except in the last spline

		for (size_t i = 0; i < nSplines; i++) {
			// don't eval the last point except in the last spline
			size_t lastSampleIdx = (i == lastPointIdx ? samplesPerSpan : samplesPerSpan - 1);
			float u = 0.f;
			const auto& spline = beziers[i];
			for (size_t j = 0; j < lastSampleIdx; j++) {
				samplesOut.push_back(spline.eval(u));
				u += du;
			}
		}
		return PiecewiseSpline2::Create(samplesOut);
	}

	SplineBezierCubic::GeometryResultBezier SplineBezierCubic::InterpolatePoints(
		const std::vector<Pairf>& points, std::optional<Pairf> firstDirection, std::optional<Pairf> lastDirection){
		using namespace std;
		size_t pCount = points.size();
		if (pCount < 3)
			return SplineBezierCubic::GeometryResultBezier::Error(GeometryResult::InputElementCountTooSmall);
			
		size_t splineCount = pCount - 1;
		size_t last = pCount - 2;
		GeometryResultBezier::result_type_t res;
		
		optional<NormalizedPairf> fstDir = nullopt;
		if (firstDirection)
			fstDir = NormalizedPairf(firstDirection.value());
		optional<NormalizedPairf> lastDir = nullopt;
		if (lastDirection)
			lastDir = NormalizedPairf(lastDirection.value());


		for (size_t i = 0; i < splineCount; i++){
			if (i == 0) {
				auto a = points[0];
				auto b = points[1];
				auto c = points[2];
				auto handles = constructInterpolantsFirst(fstDir, a, b, c);
				if (!handles)
					return SplineBezierCubic::GeometryResultBezier::Error(handles.meta());
				auto h = handles.value();
				res.push_back(SplineBezierCubic::Create(a, h.first, h.second, b));
			}
			else if (i == last){
				auto a = points[i -1];
				auto b = points[i];
				auto c = points[i + 1];
				auto handles = constructInterpolantsLast(lastDir,a, b, c);
				if (!handles)
					return SplineBezierCubic::GeometryResultBezier::Error(handles.meta());
				auto h = handles.value();
				res.push_back(SplineBezierCubic::Create(b, h.first, h.second, c));
			}
			else {
				auto a = points[i - 1];
				auto b = points[i];
				auto c = points[i + 1];
				auto d = points[i + 2];
				auto handles = constructInterpolants(a, b, c, d);
				if (!handles)
					return SplineBezierCubic::GeometryResultBezier::Error(handles.meta());
				auto h = handles.value();
				res.push_back(SplineBezierCubic::Create(b, h.first, h.second, c));
			}
		}

		return GeometryResultBezier::Success(res);
	}
}

