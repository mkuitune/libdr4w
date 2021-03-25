// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)

#pragma once

#include <math.h>

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
		
		void operator*=(float p){
			x *= p; y *= p;
		}

		float operator[](size_t idx) const {
			return idx == 0 ? x : y;
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
			return {x / len, y / len};
		}

		Paird operator-(const Paird & p) const{
			return {x - p.x, y -p.y};
		}

		Paird operator*(double p) const {
			return { x * p, y * p};
		}
		
		void operator*=(double p){
			x *= p; y *= p;
		}
	};

	struct TripletBool {
		bool fst, snd, thrd;

		TripletBool invert() const {
			return { !fst, !snd, !thrd};
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
			return {x / len, y / len, z/len};
		}

		Tripletf operator-(const Tripletf & p) const{
			return {x - p.x, y -p.y, z - p.z};
		}
		
		Tripletf operator+(const Tripletf & p) const{
			return {x + p.x, y + p.y, z + p.z};
		}

		Tripletf operator*(float p) const {
			return { x * p, y * p, z * p};
		}
		
		void operator*=(float p){
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

	struct Quadrupletf { float x, y, z, w; };

}
