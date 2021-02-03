#pragma once

#include <dr4/dr4_shapes.h>
#include <glm/glm.hpp>
namespace dr4 {

	class Distance {
	public:

		// signed distance using CCW rule
		static float Signed(Pairf pnt, const Line2D& line){
			using namespace glm;
			vec2 p(pnt.x, pnt.y);
			vec2 a(line.fst.x, line.fst.y);
			vec2 b(line.snd.x, line.snd.y);
			vec2 pa = p - a, ba = b - a;
			vec2 normal(ba.y, ba.x);
			float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
			vec2 dist = pa - ba * h;
			float sign = dot(normal, dist) > 0.f ? 1.0f : 0.0f;
			return sign * length(dist);
		}

		// unsigned distance
		static float Unsigned(Pairf pnt, const Line2D& line) {
			using namespace glm;
			vec2 p(pnt.x, pnt.y);
			vec2 a(line.fst.x, line.fst.y);
			vec2 b(line.snd.x, line.snd.y);
			vec2 pa = p - a, ba = b - a;
			float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
			vec2 dist = pa - ba * h;
			return length(dist);
		}

	};
}
