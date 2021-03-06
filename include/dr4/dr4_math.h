// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)
#pragma once

#include <dr4/dr4_unitvector3f.h>
#include <dr4/dr4_tuples.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


namespace dr4 {

	struct MaximumElementf {
		enum Dimension {DIM_0, DIM_1, DIM_2, DIM_3};
		float value;
		Dimension dimension;
	};

	template<class XYZ>
	MaximumElementf MaximumElement3D(const XYZ& v ){
		MaximumElementf m = { v.x, MaximumElementf::DIM_0 };
		if (m.value > v.y)
			m = { v.y,  MaximumElementf::DIM_1};
		if(m.value > v.z)
			m = { v.z,  MaximumElementf::DIM_2};
		return m;
	}
	
	template<class XY>
	MaximumElementf MaximumElement2D(const XY& v ){
		MaximumElementf m = { v.x, MaximumElementf::DIM_0 };
		if (m.value > v.y)
			m = { v.y, MaximumElementf::DIM_1};
		return m;
	}

	//
	// Normalized number type
	//
	template<class T>
	class Normalized {
		T m_value;
	public:

		constexpr static T Zero() { return (T)0; }
		constexpr static T One() { return (T)1; }

		Normalized() {
			m_value = Zero();
		}
		Normalized(const T& v) {
			m_value = (v < Zero()) ? Zero() : (v > One() ? One() : v);
		}
		Normalized& operator=(const T& v) {
			m_value = (v < Zero()) ? Zero() : (v > One() ? One() : v);
			return *this;
		}
		T value() const noexcept { 
			return m_value;
		};
	};

	//
	// Utility functions
	//

	inline float lerp(float src, float dst, float u){
		return (1.0f - u) * src + u * dst;
	}

	inline float clampf(float val, float minval, float maxval){
		return val < minval ? minval : (val > maxval ? maxval : val); 
	}
	inline double clampd(double val, double minval, double maxval){
		return val < minval ? minval : (val > maxval ? maxval : val); 
	}
	// smoothstep between 0 and 1.
	inline float smoothstepf(float x) {
		return x * x * (3.f - 2.f * x);
	}
	// smoothstep between 0 and 1. Clip values at ends.
	inline float smoothstepClampf(float x) {
		float u =  x > 0.f ? (x < 1.f? x : 1.f) : 0.f; 
		return u * u * (3.f - 2.f * u);
	}
	
	// smoothstep between start and end. Clip values at ends.
	inline float smoothstepClampRangef(float x, float rangeStart, float rangeEnd) {
		float u = clampf(x, rangeStart, rangeEnd);
		u = u / (rangeEnd- rangeStart);
		return u * u * (3.f - 2.f * u);
	}


#if 0
    //https://martin.ankerl.com/2012/01/25/optimized-approximative-pow-in-c-and-cpp/
	inline double fastPow(double a, double b) {
		union { double d; int x[2]; } u = { a };
		u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
		u.x[0] = 0;
		return u.d;
	}
#endif

	inline float KahanSumf(float* inputStart, float* inputEnd) {

		float sum = 0.0f;                    // Prepare the accumulator.
		float c = 0.0f;                      // A running compensation for lost low-order bits.
		for (auto& i = inputStart; i != inputEnd; i++) {    
			float y = *i - c;    // c is zero the first time around.
			float t = sum + y;   // Alas, sum is big, y small, so low-order digits of y are lost.
			c = (t - sum) - y;   // (t - sum) cancels the high-order part of y; subtracting y recovers negative (low part of y)
			sum = t;             // Algebraically, c should always be zero. Beware overly-aggressive optimizing compilers!
		}
		return sum;
	}

	inline glm::mat4 LocalToWorldOfBasis(const glm::vec3& x, const glm::vec3& y) {
		glm::vec3 z = glm::cross(x, y);
		return glm::mat4(
			x.x, y.x, z.x, 0.f ,
			x.y, y.y, z.y, 0.f ,
			x.z, y.z, z.z, 0.f ,
			0.f, 0.f, 0.f, 1.f);
	}
	
	inline glm::mat4 LocalToWorldOfBasis(const UnitVector3f& x, const UnitVector3f& y) {
		return LocalToWorldOfBasis(x.v, y.v);
	}
	
	inline glm::mat4 WorldToLocalOfBasis(const glm::vec3& x, const glm::vec3& y) {
		glm::vec3 z = glm::cross(x, y);
		return glm::mat4(
			x.x, x.y, x.z, 0.f ,
			y.x, y.y, y.z, 0.f ,
			z.x, z.y, z.z, 0.f ,
			0.f, 0.f, 0.f, 1.f);
	}

	inline glm::mat4 WorldToLocalOfBasis(const UnitVector3f& x, const UnitVector3f& y) {
		return WorldToLocalOfBasis(x.v, y.v);
	}
	
	
	inline glm::vec3 RotateFromWorldToLocal(
		const glm::vec3& wc, // world coordinates
		const glm::vec3& x,
		const glm::vec3& y,
		const glm::vec3& z) noexcept {
		return glm::vec3(glm::dot(x, wc), glm::dot(y,wc), glm::dot(z,wc));
	}
	
	inline glm::vec3 RotateFromWorldToLocal(
		const glm::vec3& wc, // world coordinates
		const glm::vec3& x,
		const glm::vec3& y) noexcept {
		return RotateFromWorldToLocal(wc, x, y, glm::cross(x, y));
	}
	
	inline glm::vec3 RotateFromLocalToWorld(
		const glm::vec3& lc, // local coordinates
		const glm::vec3& x,
		const glm::vec3& y,
		const glm::vec3& z) noexcept {
		return glm::vec3(
			x.x * lc.x + y.x * lc.y + z.x * lc.z, 
			x.y * lc.x + y.y * lc.y + z.y * lc.z, 
			x.z * lc.x + y.z * lc.y + z.z * lc.z
		);
	}
	
	inline glm::vec3 RotateFromLocalToWorld(
		const glm::vec3& lc, // local coordinates
		const glm::vec3& x,
		const glm::vec3& y) noexcept {
		glm::vec3 z = glm::cross(x,y);
		return RotateFromLocalToWorld(lc, x, y, z);
	}
	
	inline glm::quat LocalToWorldQuaternionOfBasis(const UnitVector3f& x, const UnitVector3f& y) noexcept {
		const float unitElem = 0.5773502692f;//(1/sqrt(3))
		glm::vec3 local(unitElem, unitElem, unitElem);
		glm::vec3 world = RotateFromLocalToWorld(local, x.v, y.v);
		glm::vec3 axisOfRotation = glm::cross(local, world);
		float angle = acos(glm::dot(local, world));
		return glm::angleAxis(angle, axisOfRotation);
	}


	struct GeometryTransform {
		glm::mat4 transform;

		glm::vec3 apply(const glm::vec3& vIn) const noexcept {
			glm::vec4  v4(vIn.x, vIn.y, vIn.z, 1.f);
			auto res = transform * v4;
			return { res.x, res.y, res.z };
		}
	};


	GeometryTransform AngleAxisRotation(const UnitVector3f& axis, float degreeRad);

	inline glm::vec3 CenterOf(
		const glm::vec3& fst,
		const glm::vec3& snd,
		const glm::vec3& thrd) {
		return (fst + snd + thrd) / 3.f;
	}
}