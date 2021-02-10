#pragma once

#include <stdint.h>
#include <vector>
#include <functional>
#include <algorithm>
#include <stack>

#include <dr4/dr4_math.h>
#include <dr4\dr4_tuples.h>

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

	struct FieldQuadtreeNode;

#if 1
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
		
		float maxSampleDifference(float measured[5]) const {
#if 0
			int i = 0;
			float d = fabsf(values[i] - measured[i]);
			i++;
			for (; i < 5; i++){
				d = std::max(d,fabsf(values[i] - measured[i]));
			}
#endif

#if 1
			float d = 0.f;
			for (int i = 0; i < 5; i++){
				d += fabsf(values[i] - measured[i]);
			}
#endif
			return d;
		}
		
		float maxSampleDifferenceRelative(float measured[5]) const {
			int i = 0;
			float d = fabsf((values[i] - measured[i])/measured[i]);
			i++;
			for (; i < 5; i++){
				d = std::max(d,fabsf((values[i] - measured[i])/measured[i]));
			}

			return d;
		}

		void sampleField(float realValues[5],std::function<float(float, float)> field) const {
			for (int i = 0; i < 5; i++) {
				auto pnt = coords[i];
				realValues[i] = field(pnt.x, pnt.y);
			}
		}
#if 0	
		void sampleFieldGradient(Pairf normals[5],float d,std::function<float(float, float)> field) const {
			float h = d / 10;
			float i2h = 1.f / 2.f * h;
			for (int i = 0; i < 5; i++) {
				auto pnt = coords[i];
				// grad = (-df/dx, -df/dy,1)
				// df/dx = (f(x + h, y) - f(x - h,y)) / 2h
				normals[i].x = (field(pnt.x - h, pnt.y) - field(pnt.x + h, pnt.y)) * i2h;
				normals[i].y = (field(pnt.x, pnt.y - h) - field(pnt.x, pnt.y + h)) * i2h;
			}
		}
#endif
	};
#endif
#if 0
	inline float gradientDifference(const Pairf normals[5]) {

		Pairf ref = normals[1];
		for (int i = 2; i < 4; i++)
		{
			ref.x += normals[i].x;
			ref.y += normals[i].y;
		}
		return (normals[1].x - 0.25 * ref.x) + (normals[1].y - 0.25f * ref.y);
	}
#endif
#if 0
	struct fq_interpoloation_samples_t {
		fq_coordinate_t coords[9];
		float values[9];

		float sampleDifference(float measured[9]) const {
			float d = 0.f;
			for (int i = 0; i < 9; i++){
				d += fabsf(values[i] - measured[i]);
			}
			return d;
		}
		
		float maxSampleDifference(float measured[9]) const {
			int i = 0;
			float d = fabsf(values[i] - measured[i]);
			i++;
			for (; i < 9; i++){
				d = std::max(d,fabsf(values[i] - measured[i]));
			}
			return d;
		}

		float maxFieldDifference(std::function<float(float, float)> field) const {
			float realValues[9];
			for (int i = 0; i < 9; i++) {
				auto pnt = coords[i];
				realValues[i] = field(pnt.x, pnt.y);
			}
			//float diff = sampleDifference(realValues);
			float diff = maxSampleDifference(realValues);
			return diff;
		}
	};
#endif

	struct cornerdata_t {
		float field;
	};

	struct FieldQuadtreeNode{
		cornerdata_t cornerdata[4]; // store adf value here
		size_t childs; // every subdividion creates four children
		float x0, y0, d;
		uint8_t depth = 0;

		fq_corners_t corners() const {
			return {
				{{x0,y0},{x0 + d, y0},{x0 + d, y0 + d},{x0, y0 + d}}
			};
		}

		float divergence(float measured[5]) const;

		float rot() const {
			float s = 0.f;
			for (int i = 1; i < 4; i++)
				s += (cornerdata[i % 4].field - cornerdata[(i - 1) % 4].field);
			return s;
		}

		bool isInside(float x, float y) const {
			return (x >= x0 && x <= (x0 + d)) &&
				   (y >= y0 && y <= (y0 + d));
		}

		//float maxFieldDifferenceAtSamplepoints(std::function<float(float, float)> field) const {
		//	auto samples = samplepoints();
		//	return samples.maxFieldDifference(field);
		//}

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

		void initFromPrevious(const FieldQuadtreeNode& prev) {
			auto corpos = corners();
			for (int i = 0; i < 4; i++) {
				auto pos = corpos.corners[i];
				cornerdata[i].field = prev.sampleCorners(pos.x, pos.y);
			}
		}

#if 0
		fq_interpoloation_samples_t samplepoints() const {
			float h = d / 2;
			fq_interpoloation_samples_t s = {
			   {{x0 + h,y0 + h},{x0 + h, y0},{x0 + d, y0 + h},{x0 + h, y0 + d}, {x0, y0 + h},
				{x0,y0},{x0 + d,y0}, {x0 + d, y0 + d}, {x0, y0 + d}
				},
			   {0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}
			};
			for (int i = 0; i < 9; i++) {
				auto pnt = s.coords[i];
				s.values[i] = sampleCorners(pnt.x, pnt.y);
			}
			return s;
		}
#endif
		fq_interpoloation_samples_t samplepoints() const {
			float h = d / 2;
			fq_interpoloation_samples_t s = {
			   {{x0 + h,y0 + h},{x0 + h, y0},{x0 + d, y0 + h},{x0 + h, y0 + d}, {x0, y0 + h},
				},
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

#if 0 // uncomment if need to debug this for some reason
			if (u < 0.f || u > 1.f)
				throw std::exception("u wrong");
			if (v < 0.f || v > 1.f)
				throw std::exception("v wrong");
#endif

			float r1 = lerp(cornerdata[3].field, cornerdata[2].field, u);
			float r0 = lerp(cornerdata[0].field, cornerdata[1].field, u);
			return lerp(r0, r1, v);
		}

		constexpr static float FieldInitial() {
			return -1.0e9;
		}

		size_t getChildIdx(float x, float y) const {
			size_t childIdx = 0;
			float h = d / 2;
			if ((y - y0) > h) {
				childIdx = (x - x0) < h ? 3 : 2;
			}
			else {
				childIdx = (x - x0) < h ? 0 : 1;
			}
			return childs + childIdx;
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
		static const size_t NPOS = -1;

		size_t maxDepth() const {
			const float d0 = nodes[0].d;
			float cd = 1.0f;
			for (const auto& n : nodes) {
				float cdn = (d0) / (n.d);
				cd = std::max(cd, cdn);
			}
			return (size_t) (cd * 0.5f);
		}

		float getDeepSampleInt(size_t idx, float x, float y) const {
			while(nodes[idx].childs != 0){
				idx = nodes[idx].getChildIdx(x, y);
			}
			return nodes[idx].sampleCorners(x, y);
		}

		float getDeepSample(float x, float y) const {
			if (!nodes[0].isInside(x, y))
				return FieldQuadtreeNode::FieldInitial();

			return getDeepSampleInt(0, x, y);
		}
		
		float getIdxAtFromInt(size_t idx, float x, float y) const {
			while(nodes[idx].childs != 0){
				idx = nodes[idx].getChildIdx(x, y);
			}
			return idx;
		}

		size_t getIdxAt(float x, float y) const {
			if (!nodes[0].isInside(x, y))
				return NPOS;

			return getIdxAtFromInt(0, x, y);
		}
		

	};

	struct FieldQuadtreeBuilder {

		FieldQuadtree  tree;
		float x; float y; float d;
		//float threshold = 0.1f;
		//float threshold = 10.0f;
		float threshold = 1.f;
		float thresholdrelative = 0.01f;
		//float divergenceLimit = 0.5f;
		// Lower left corner of tree plus initial size
		//FieldQuadtreeBuilder(float x, float y, float d) {}

		uint8_t maxnodedepth = 8;

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
