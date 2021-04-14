#pragma once

#include <dr4/dr4_result_types.h>

namespace dr4 {
	struct AnalyticLinef {
		typedef TypedResult<GeometryResult, AnalyticLinef> CreateResult;

		float k = 0.f, c = 0.f;

		static CreateResult Create(Pairf p0, Pairf p1) {
			auto d = p1 - p0;

			if (Float32::GeometryAreEqual(p0.x, p1.x)) {
				return CreateResult::Error(GeometryResult::DivisionByZero);
			}

			AnalyticLinef line;
			line.k = d.y / d.x;
			line.c = p0.y - line.k * p0.x;
			return line;
		}

		float at(float x) const { return k * x + c; }
	};
}
