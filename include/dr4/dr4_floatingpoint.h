// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)

#pragma once
#include <string>
#include <limits>
#include <cmath>
#include <cstdint>


namespace dr4 {

	class Float64 {
	public:
		typedef int64_t ulps_t;

		static constexpr double Max = std::numeric_limits<double>::max();
		//static constexpr double GeometryEpsilon = 1.0e-10;
		//static constexpr double NegativeGeometryEpsilon = -1.0e-10;
		static constexpr double GeometryEpsilon = 1.0e-8;
		static constexpr double NegativeGeometryEpsilon = -1.0e-8;
		static constexpr double VertexPositionTolerance = 1.0e-5;
		static constexpr ulps_t ulpsEpsilon = 10;

		static constexpr double PI = 3.14159265358979323846;
		/** Full circle in radians*/
		static constexpr double FullCircle = 2 * PI;

		static bool GeometryIsNonZero(double a) {
			return !GeometryIsCloseToZero(a);
		}

		static bool GeometryIsCloseToZero(double a) {
			//return fabs(a) < GeometryEpsilon; // alternative !(a > ge) && !( a < -ge)
			return (a < GeometryEpsilon) && (a > NegativeGeometryEpsilon);
		}

		static bool GeometryIsGreaterThanZero(double a) {
			return a > GeometryEpsilon;
		}

		static bool GeometryIsLessThanZero(double a) {
			return a < NegativeGeometryEpsilon;
		}

		static bool GeometryAreEqual(double a, double b) {
			return GeometryIsCloseToZero(b - a);
		}
		// a > b
		static bool GeometryIsGreaterThan(double a, double b) {
			return b < (a - GeometryEpsilon);
		}

		static ulps_t IntegerOfBytes(double d) {
			return *((ulps_t*)& d);
		}

		static bool UlpsAreNearlyEqual(double a, double b, ulps_t eps = ulpsEpsilon) {
			ulps_t au = IntegerOfBytes(a);
			ulps_t bu = IntegerOfBytes(b);
			return abs(au - bu) < eps;
		}

		/**/
		static int64_t UlpsDistance(const double a, const double b)
		{
			// We can skip all the following work if they're equal.
			if (a == b) return 0;

			constexpr auto max = std::numeric_limits<int64_t>::max();

			// We first check if the values are NaN.
			// If this is the case, they're inherently unequal;
			// return the maximum distance between the two.
			if (isnan(a) || isnan(b)) return max;

			// If one's infinite, and they're not equal,
			// return the max distance between the two.
			if (isinf(a) || isinf(b)) return max;

			// At this point we know that the floating-point values aren't equal and
			// aren't special values (infinity/NaN).
			// Because of how IEEE754 floats are laid out
			// (sign bit, then exponent, then mantissa), we can examine the bits
			// as if they were integers to get the distance between them in units
			// of least precision (ULPs).
			static_assert(sizeof(double) == sizeof(int64_t), "What size is float?");

			// memcpy to get around the strict aliasing rule.
			// The compiler knows what we're doing and will just transfer the float
			// values into integer registers.
			int64_t ia, ib;
			memcpy(&ia, &a, sizeof(double));
			memcpy(&ib, &b, sizeof(double));

			// If the signs of the two values aren't the same,
			// return the maximum distance between the two.
			// This is done to avoid integer overflow, and because the bit layout of
			// floats is closer to sign-magnitude than it is to two's complement.
			// This *also* means that if you're checking if a value is close to zero,
			// you should probably just use a fixed epsilon instead of this function.
			if ((ia < 0) != (ib < 0)) return max;

			// If we've satisfied all our caveats above, just subtract the values.
			// The result is the distance between the values in ULPs.
			int64_t distance = ia - ib;
			if (distance < 0) distance = -distance;
			return distance;
		}

	};

	class Float32 {
	public:
		static constexpr float Max = std::numeric_limits<float>::max();
		//static constexpr float GeometryEpsilon = 1.0e-10f;
		static constexpr float GeometryEpsilon = std::numeric_limits<float>::epsilon() * 10.f;
		static constexpr double NegativeGeometryEpsilon = -GeometryEpsilon;
		static constexpr float VertexPositionTolerance = 1.0e-5f;
		static constexpr int32_t ulpsEpsilon = 10;

		static constexpr float PI = 3.14159265f;
		static constexpr float DEG2RAD = PI / 180.f;
		static constexpr float RAD2DEG = 180.f / PI;

		static float degreesToRadians(float deg) {
			return DEG2RAD * deg;
		}
		
		static float radiansToDegrees(float rad) {
			return RAD2DEG * rad;
		}

		static bool GeometryIsNonZero(float a) {
			return !GeometryIsCloseToZero(a);
		}

		static bool GeometryIsCloseToZero(float a) {
			//return fabs(a) < GeometryEpsilon; // alternative !(a > ge) && !( a < -ge)
			return (a < GeometryEpsilon) && (a > NegativeGeometryEpsilon);
		}

		static bool GeometryIsGreaterThanZero(float a) {
			return a > GeometryEpsilon;
		}

		static bool GeometryIsLessThanZero(float a) {
			return a < NegativeGeometryEpsilon;
		}

		static bool GeometryAreEqual(float a, float b) {
			return GeometryIsCloseToZero(b - a);
		}
		// a > b
		static bool GeometryIsGreaterThan(float a, float b) {
			return b < (a - GeometryEpsilon);
		}


		// http://bitbashing.io/comparing-floats.html
		int32_t ulpsDistance(const float a, const float b)
		{
			// We can skip all the following work if they're equal.
			if (a == b) return 0;

			constexpr auto max = std::numeric_limits<int32_t>::max();

			// We first check if the values are NaN.
			// If this is the case, they're inherently unequal;
			// return the maximum distance between the two.
			if (isnan(a) || isnan(b)) return max;

			// If one's infinite, and they're not equal,
			// return the max distance between the two.
			if (isinf(a) || isinf(b)) return max;

			// At this point we know that the floating-point values aren't equal and
			// aren't special values (infinity/NaN).
			// Because of how IEEE754 floats are laid out
			// (sign bit, then exponent, then mantissa), we can examine the bits
			// as if they were integers to get the distance between them in units
			// of least precision (ULPs).
			static_assert(sizeof(float) == sizeof(int32_t), "What size is float?");

			// memcpy to get around the strict aliasing rule.
			// The compiler knows what we're doing and will just transfer the float
			// values into integer registers.
			int32_t ia, ib;
			memcpy(&ia, &a, sizeof(float));
			memcpy(&ib, &b, sizeof(float));

			// If the signs of the two values aren't the same,
			// return the maximum distance between the two.
			// This is done to avoid integer overflow, and because the bit layout of
			// floats is closer to sign-magnitude than it is to two's complement.
			// This *also* means that if you're checking if a value is close to zero,
			// you should probably just use a fixed epsilon instead of this function.
			if ((ia < 0) != (ib < 0)) return max;

			// If we've satisfied all our caveats above, just subtract the values.
			// The result is the distance between the values in ULPs.
			int32_t distance = ia - ib;
			if (distance < 0) distance = -distance;
			return distance;
		}
	};


	enum class MatrixError { Success, DeterminantIsNearZero };

}

