// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)

#pragma once

#include <math.h>
#include <dr4/dr4_floatingpoint.h>

namespace dr4
{
	struct Pairf {
		float x, y; 

		float norm2() const {
			return x * x + y * y;
		}
		float norm() const {
			return sqrtf(norm2());
		}
		float dot(const Pairf& rhs) const {
			return x * rhs.x + y * rhs.y;
		}
		float kross(const Pairf& rhs) const {
			return x* rhs.y - y * rhs.x; // the z component of cross product
		}
		Pairf normalized() const {
			float len = norm();
			return {x / len, y / len};
		}

		Pairf operator-(const Pairf & p) const{
			return {x - p.x, y -p.y};
		}
		
		Pairf operator+(const Pairf & p) const{
			return {x + p.x, y + p.y};
		}

		Pairf operator*(float p) const {
			return { x * p, y * p};
		}
		
		Pairf operator/(float p) const {
			return { x / p, y / p};
		}
		
		void operator*=(float p){
			x *= p; y *= p;
		}
		
		void operator+=(Pairf rhs){
			x += rhs.x; y += rhs.y;
		}

		float operator[](size_t idx) const {
			return idx == 0 ? x : y;
		}

		Pairf rot90() const { return { -y, x }; }

		bool isEqualTo(const Pairf& rhs)const {
			return Float32::GeometryAreEqual(x, rhs.x) && Float32::GeometryAreEqual(y, rhs.y);
		}

	};

	struct NormalizedPairf {
		Pairf pair;
		bool valid;
		NormalizedPairf(const Pairf& input) {
			float inputLen = input.norm();
			valid = !Float32::GeometryIsCloseToZero(inputLen);
			pair = valid ? (input / inputLen) : Pairf{0.f, 0.f};
		}
		const Pairf& value() const { return pair; }
		bool isValid() const { return valid; }
		operator bool() const { return valid; }
	};
	
	struct RayEnumeratorPairf {
		Pairf value;
		Pairf delta;
		void operator++() { value += delta; }

		Pairf operator()() {
			Pairf res = value;
			value += delta;
			return res;
		}
	};

	struct Paird {
		double x, y;

		double norm2() const {
			return x * x + y * y;
		}
		double norm() const {
			return sqrt(norm2());
		}
		double dot(const Paird& rhs) const {
			return x * rhs.x + y * rhs.y;
		}
		Paird normalized() const {
			double len = norm();
			return { x / len, y / len };
		}

		Paird operator-(const Paird& p) const {
			return { x - p.x, y - p.y };
		}

		Paird operator*(double p) const {
			return { x * p, y * p };
		}

		void operator*=(double p) {
			x *= p; y *= p;
		}
	};

	struct TripletBool {
		bool fst, snd, thrd;

		TripletBool invert() const {
			return { !fst, !snd, !thrd };
		}

		bool all() const {
			return fst && snd && thrd;
		}

		bool allNot() const {
			return (!fst) && (!snd) && (!thrd);
		}
	};

	struct PairIdx {
		size_t x, y;
#if 0
		bool operator>=(const PairIdx& rhs) const {
			return x >= rhs.x && y >= rhs.y;
		}

		bool operator <(const PairIdx& rhs) const {
			return x < rhs.x&& y < rhs.y;
		}
#endif
	};

	struct Tripletf {
		float x, y, z;

		float norm2() const {
			return x * x + y * y + z * z;
		}
		float norm() const {
			return sqrtf(norm2());
		}
		float dot(const Tripletf& rhs) const {
			return x * rhs.x + y * rhs.y + z * rhs.z;
		}

		Tripletf normalized() const {
			float len = norm();
			return { x / len, y / len, z / len };
		}

		Tripletf operator-(const Tripletf& p) const {
			return { x - p.x, y - p.y, z - p.z };
		}

		Tripletf operator+(const Tripletf& p) const {
			return { x + p.x, y + p.y, z + p.z };
		}

		Tripletf operator*(float p) const {
			return { x * p, y * p, z * p };
		}

		void operator*=(float p) {
			x *= p; y *= p; z *= p;
		}

		float operator[](size_t idx) const {
			switch (idx) {
			case 0:return x;
			case 1:return y;
			default:return z;
			}
		}
	};

	struct TripletIdx { size_t x, y, z; };

	struct Tripletu {
		unsigned first, second, third;
	};

	struct Quadrupletf {
		
		float x, y, z, w; 
	
		float norm2() const {
			return x * x + y * y + z * z + w * w;
		}
		float norm() const {
			return sqrtf(norm2());
		}
		float dot(const Quadrupletf& rhs) const {
			return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
		}

		Quadrupletf normalized() const {
			float len = norm();
			return { x / len, y / len, z / len, w/len };
		}

		Quadrupletf operator-(const Quadrupletf& p) const {
			return { x - p.x, y - p.y, z - p.z, w - p.w };
		}

		Quadrupletf operator+(const Quadrupletf& p) const {
			return { x + p.x, y + p.y, z + p.z, w + p.w };
		}

		Quadrupletf operator*(float p) const {
			return { x * p, y * p, z * p , w * p};
		}

		void operator*=(float p) {
			x *= p; y *= p; z *= p; w *= p;
		}

		float operator[](size_t idx) const {
			switch (idx) {
			case 0:return x;
			case 1:return y;
			case 2:return z;
			default:return w;
			}
		}
	};

}
