// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)
#pragma once

#include <dr4/dr4_geometryresult.h>
#include <dr4/dr4_floatingpoint.h>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <string>

namespace dr4 {
	struct UnitVector3f {
		glm::vec3 v;

		UnitVector3f() :v(0.f, 0.f, 1.f) {}
		
		UnitVector3f(float x, float y, float z){
			v = {x, y, z};
			v = glm::normalize(v);
		}

		UnitVector3f(const glm::vec3& vIn) {
			v = vIn;
			v = glm::normalize(v);
		}

		UnitVector3f(const UnitVector3f& vIn) :v(vIn.v) {}

		UnitVector3f reverse() const { return { {-v.x, -v.y, -v.z} }; }

		float x() const { return v.x; }
		float y() const { return v.y; }
		float z() const { return v.z; }

		glm::vec3 vec() const { return v; }

		glm::vec3 operator*(float d) const { return v * d; }

		float dot(const glm::vec3& vec) const { return glm::dot(vec, v); }
		float dot(const UnitVector3f& vec) const { return dot(vec.v); }

		glm::vec3 cross(const UnitVector3f& vec) const { return glm::cross(v, vec.v); }
		glm::vec3 cross(const glm::vec3& vec) const { return glm::cross(v, vec); }


		static GeometryResult TryCreate(const glm::vec3& a, UnitVector3f& v) {
			float len = (float) glm::length(a);
			if (Float32::GeometryIsCloseToZero(len)) {
				return GeometryResult::LengthCloseToZero;
			}
			UnitVector3f res = { a / len };
			return GeometryResult::Success;
		}
	};
}
