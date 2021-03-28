#pragma once

#include <dr4/dr4_splines.h>
#include <string>
#include <sstream>

namespace dr4 {

	class Analysis {
	public:

		static std::string PointsToMathematica(const std::vector<Pairf>& values) {
			using namespace std;
			stringstream res;
			res << "points = {";
			bool fst = true;
			for (auto p : values){
				if (fst) fst = false;
				else res << ",";

				res << "{" << p.x << "," << p.y << "}";
			}
			res << "};" << endl;
			res << "ListLinePlot[points, PlotMarkers -> {Automatic, 20}, Frame -> True]" << endl;
			return res.str();
		}
		
		static std::string PointsAndInterpolatedToMathematica(
			const std::vector<Pairf>& sourcePoints, const std::vector<Pairf>& values) {
			using namespace std;
			stringstream res;
			{
				res << "ip = {";
				bool fst = true;
				for (auto p : values) {
					if (fst) fst = false;
					else res << ",";

					res << "{" << p.x << "," << p.y << "}";
				}
				res << "};" << endl;
			}
			{
				res << "P = {";
				bool fst = true;
				for (auto p : sourcePoints) {
					if (fst) fst = false;
					else res << ",";

					res << "{" << p.x << "," << p.y << "}";
				}
				res << "};" << endl;
			}
			res << "ListPlot[{P, ip}, Joined -> {True, True}, PlotStyle -> {Dashed, Automatic}, PlotMarkers->Automatic,Frame->True]" << endl;
			return res.str();
		}
		
		static std::vector<Pairf> InterpolateData(
			std::vector<Pairf> points, size_t samplesPerSpan, size_t sampleCount) {
			PiecewiseSpline2 spline = Interpolate2(points, samplesPerSpan);
			LookUpTable<float> lut = spline.createByXLUT(sampleCount);
			std::vector<Pairf> valuesOut;
#if 0
			// get actual look up table samples...
			auto pts = lut.getSamples();
			for (auto p : pts) {
				valuesOut.push_back({p.first, p.second});
			}
#endif
#if 1
			// or lookup within given range - this simulates actual use better
			LoopRange<float> rangeOver(sampleCount, lut.sourceStart(), lut.sourceEnd());
			for (auto x : rangeOver) {
				Pairf sample = { x, lut.getNearest(x) };
				valuesOut.push_back(sample);
			}
#endif
			return valuesOut;
		}
		
		static std::vector<Pairf> InterpolateDataSpline(
			std::vector<Pairf> points, size_t samplesPerSpan){
			PiecewiseSpline2 spline = Interpolate2(points, samplesPerSpan);
			return spline.toPoints();
		}

		static std::string PointsToMathematicaWithInterpolation(
			std::vector<Pairf> points, size_t samplesPerSpan, size_t sampleCount){
			auto interpolated = InterpolateData(points, samplesPerSpan, sampleCount);
			return PointsToMathematica(interpolated);
		}
	};
}