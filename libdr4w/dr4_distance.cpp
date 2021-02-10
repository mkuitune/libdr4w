#include <dr4/dr4_distance.h>
#include <dr4/dr4_math.h>
#include <glm/glm.hpp>

// signed distance using CCW rule
float dr4::Distance2DSigned(Pairf pnt, const Line2D& line) {
	using namespace glm;
	vec2 p(pnt.x, pnt.y);
	vec2 a(line.fst.x, line.fst.y);
	vec2 b(line.snd.x, line.snd.y);
	vec2 pa = p - a, ba = b - a;
	vec2 normal(ba.y, -ba.x);
	float h = clampf(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
	vec2 dist = pa - ba * h;
	float sign = dot(normal, dist) > 0.f ? 1.0f : -1.0f;
	return sign * length(dist);
}

// unsigned distance
float dr4:: Distance2DUnsigned(Pairf pnt, const Line2D& line) {
	using namespace glm;
	vec2 p(pnt.x, pnt.y);
	vec2 a(line.fst.x, line.fst.y);
	vec2 b(line.snd.x, line.snd.y);
	vec2 pa = p - a, ba = b - a;
	float h = clampf(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
	vec2 dist = pa - ba * h;
	return length(dist);
}

#if 1
float dr4::LineDistance2D::signedDistance(Pairf p) const {
	auto pa = p - a;
	float h = clampf(pa.dot(m_vec) / m_vecdot, 0.0f, 1.0f);
	auto dist = pa - (m_vec * h);
	float sign = m_normal.dot(dist) > 0.f ? 1.0f : -1.0f;
	return sign * dist.norm();
}

float dr4::LineDistance2D::unsignedDistance(Pairf p) const {
	auto pa = p - a;
	float h = clampf(pa.dot(m_vec) / m_vecdot, 0.0f, 1.0f);
	auto dist = pa - (m_vec * h);
	return dist.norm();
}
#endif

#if 0
float dr4::LineDistance2D::signedDistance(Pairf p) const {
	//auto pa = p - a;
	Paird da = {a.x, a.y};
	Paird dp = { p.x, p.y };
	Paird pa = dp - da;
	double h = clampd(pa.dot(m_vecd) / m_vecdot, 0.0, 1.0);
	auto dist = pa - (m_vecd * h);
	double sign = m_normald.dot(dist) > 0.0 ? 1.0 : -1.0;
	return sign * dist.norm();
}

float dr4::LineDistance2D::unsignedDistance(Pairf p) const {
	//auto pa = p - a;
	Paird da = {a.x, a.y};
	Paird dp = { p.x, p.y };
	Paird pa = dp - da;
	double h = clampd(pa.dot(m_vecd) / m_vecdot, 0.0, 1.0);
	auto dist = pa - (m_vecd * h);
	return dist.norm();
}
#endif
