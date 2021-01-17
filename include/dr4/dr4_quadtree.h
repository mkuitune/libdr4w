#pragma once

#include <stdint.h>
#include <vector>
#include <functional>
#include <algorithm>
#include <stack>

namespace dr4 {


	//  i:th level cell
	//  x--x
	//  |  |
	//  x--x
	//	
	//  vertex indices
	//  3--2
	//  |  |
	//  0--1
	//   
	//  i+1:th level cells
	//
	// x--x--x
	// |nw|ne|
	// x--x--x
	// |sw|se|
	// x--x--x
	//	
	// x--x--x
	// |3 |2 |
	// x--x--x
	// |0 |1 |
	// x--x--x
	//
	// Sample points in cell
	// x--3--x
	// |     |
    // 4  0  2  
	// |     |
	// x--1--x

	struct fq_coordinate_t {
		float x, y;
	};

	struct fq_corners_t {
		fq_coordinate_t corners[4];
	};
	
	struct fq_interpoloation_samples_t {
		fq_coordinate_t coords[5];
		float values[5];

		float sampleDifference(float measured[5]) const {
			float d = 0.f;
			for (int i = 0; i < 5; i++){
				d += fabsf(values[i] - measured[i]);
			}
			return d;
		}

		float fieldDifference(std::function<float(float, float)> field) const {
			float realValues[5];
			for (int i = 0; i < 5; i++) {
				auto pnt = coords[i];
				realValues[i] = field(pnt.x, pnt.y);
			}
			float diff = sampleDifference(realValues);
			return diff;
		}
	};

	struct cornerdata_t {
		float field;
	};

	inline float lerp(float a, float b, float u) {
		return (1.f - u) * a + u * b;
	}

	struct FieldQuadtreeNode{
		cornerdata_t cornerdata[4]; // store adf value here
		size_t childs; // every subdividion creates four children
		float x0, y0, d;

		fq_corners_t corners() const {
			return {
				{{x0,y0},{x0 + d, y0},{x0 + d, y0 + d},{x0, y0 + d}}
			};
		}

		float fieldDifferencesAtSamplepoints(std::function<float(float, float)> field) const {
			auto samples = samplepoints();
			return samples.fieldDifference(field);
		}

		void applyField(std::function<float(float, float)> field) {
			auto corpos = corners();
			for (int i = 0; i < 4; i++) {
				auto pos = corpos.corners[i];
				cornerdata[i].field = field(pos.x, pos.y);
			}
		}

		void applyFieldToExisting(std::function<float(float, float)> field) {
			auto corpos = corners();
			for (int i = 0; i < 4; i++) {
				auto pos = corpos.corners[i];
				float smp = field(pos.x, pos.y);
				if (fabsf(cornerdata[i].field) > fabsf(smp))
					cornerdata[i].field = smp;
			}
		}

		// NOTE: Fieldvalues must have some preset before applying this
		void applyPrevious(const FieldQuadtreeNode& prev) {
			auto corpos = corners();
			for (int i = 0; i < 4; i++) {
				auto pos = corpos.corners[i];
				float smp = prev.sampleCorners(pos.x, pos.y);
				if (fabsf(cornerdata[i].field) > fabsf(smp))
					cornerdata[i].field = smp;
			}
		}
		
		void applyPreviousOverride(const FieldQuadtreeNode& prev) {
			auto corpos = corners();
			for (int i = 0; i < 4; i++) {
				auto pos = corpos.corners[i];
				cornerdata[i].field = prev.sampleCorners(pos.x, pos.y);
			}
		}

		fq_interpoloation_samples_t samplepoints() const {
			float h = d / 2;
			fq_interpoloation_samples_t s = {
			   {{x0 + h,y0 + h},{x0 + h, y0},{x0 + d, y0 + h},{x0 + h, y0 + d}, {x0, y0 + h}},
			   {0.f, 0.f, 0.f, 0.f, 0.f}
			};
			for (int i = 0; i < 5; i++) {
				auto pnt = s.coords[i];
				s.values[i] = sampleCorners(pnt.x, pnt.y);
			}
			return s;
		}

		float interpolateX(float x) const {
			return (x - x0) / d;
		}
		float interpolateY(float y) const {
			return (y - y0) / d;
		}

		// bilinear interpolation of corner values
		float sampleCorners(float x, float y) const {
			float u = interpolateX(x);
			float v = interpolateY(y);

			float r1 = lerp(cornerdata[3].field, cornerdata[2].field, u);
			float r0 = lerp(cornerdata[0].field, cornerdata[1].field, u);
			return lerp(r0, r1, v);
		}

		constexpr static float FieldInitial() {
			return -1.0e9;
		}

		static FieldQuadtreeNode Init(float x, float y, float d) {
			FieldQuadtreeNode n;
			n.childs = 0;
			n.x0 = x;
			n.y0 = y;
			n.d = d;
			n.cornerdata[0].field = FieldInitial();
			n.cornerdata[1].field = FieldInitial();
			n.cornerdata[2].field = FieldInitial();
			n.cornerdata[3].field = FieldInitial();
			return n;
		}
	};

	struct FieldQuadtree {
		std::vector<FieldQuadtreeNode> nodes;
	};

	struct FieldQuadtreeBuilder {

		FieldQuadtree  tree;
		float x; float y; float d;
		float threshold = 0.1f;

		// Lower left corner of tree plus initial size
		//FieldQuadtreeBuilder(float x, float y, float d) {}
		FieldQuadtreeBuilder(float xin, float yin, float din) :x(xin), y(yin), d(din) {
		}


		FieldQuadtree  build() {
			return tree;
		}

		void add(std::function<float(float, float)> field) {
			if (tree.nodes.empty())
				addNew(field);
			else
				addExisting(field);
		}

		void addExisting(std::function<float(float, float)> field);
		void addNew(std::function<float(float, float)> field);
	};
}
