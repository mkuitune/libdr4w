// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)

#pragma once

#include <dr4/dr4_geometryresult.h>
#include <dr4/dr4_floatingpoint.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <string>

namespace dr4 {
	struct UnitVector2f {
		glm::vec2 v;

		UnitVector2f() :v(0.f, 1.f) {}

		UnitVector2f(const glm::vec2& vIn) {
			v = vIn;
			v = glm::normalize(v);
		}

		UnitVector2f(const UnitVector2f& vIn) :v(vIn.v) {}

		UnitVector2f reverse() const { return { {-v.x, -v.y} }; }

		UnitVector2f rotate90() const { return { {v.y, -v.x} }; }

		float x() const { return v.x; }
		float y() const { return v.y; }

		glm::vec2 vec() const { return v; }
		glm::vec2 operator*(float d) const { return v * d; }

		float dot(const glm::vec2& vec) const { return dot(vec); }
		float dot(const UnitVector2f& vec) const { return dot(vec.v); }

		float kross(const UnitVector2f& vec) const { return v.x * vec.v.y - v.y * vec.v.x; }
		float kross(const glm::vec2& vec) const { return  v.x * vec.y - v.y * vec.x; }

		static GeometryResult TryCreate(const glm::vec2& a, UnitVector2f& v) {
			float len = (float) glm::length(a);
			if (Float32::GeometryIsCloseToZero(len)) {
				return GeometryResult::LengthCloseToZero;
			}
			UnitVector2f res = { a / len };
			return GeometryResult::Success;
		}
	};
}

