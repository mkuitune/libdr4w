#pragma once

#include <dr4/dr4_shapes.h>
#include <functional>
namespace dr4 {

	float Distance2DSigned(Pairf pnt, const Line2D& line);
	float Distance2DUnsigned(Pairf pnt, const Line2D & line);

	class CircleDistance2D {
		Pairf m_origin;
		float m_radius;

		CircleDistance2D(Pairf origin, float radius):
			m_origin(origin), m_radius(radius) {}

		float signedDistance(Pairf pnt) const {
			auto dist = pnt - m_origin;
			return dist.norm() - m_radius;
		}
		float signedDistance(float x, float y) const {
			return signedDistance({ x,y });
		};

		float unsignedDistance(Pairf pnt) const {
			auto dist = pnt - m_origin;
			return fabsf(dist.norm() - m_radius);
		}
		float unsignedDistance(float x, float y) const {
			return unsignedDistance({ x,y });
		}

		std::function<float(float, float)> bindSigned() const{
			return[this](float a, float b) {
				return this->signedDistance(a, b);
			};
		}
		
		std::function<float(float, float)> bindUnsigned() const{
			return[this](float a, float b) {
				return this->unsignedDistance(a, b);
			};
		}
	};

	class PointDistance2D {
		Pairf m_origin;

		PointDistance2D(Pairf origin):m_origin(origin) {}

		float signedDistance(Pairf pnt) const {
			auto dist = pnt - m_origin;
			return dist.norm();
		}
		float signedDistance(float x, float y) const {
			return signedDistance({ x,y });
		};

		float unsignedDistance(Pairf pnt) const {
			auto dist = pnt - m_origin;
			return dist.norm();
		}
		float unsignedDistance(float x, float y) const {
			return unsignedDistance({ x,y });
		}

		std::function<float(float, float)> bindSigned() const{
			return[this](float a, float b) {
				return this->signedDistance(a, b);
			};
		}
		
		std::function<float(float, float)> bindUnsigned() const{
			return[this](float a, float b) {
				return this->unsignedDistance(a, b);
			};
		}
	};

	class LineDistance2D {
		Line2D m_line;
		Pairf a;
		Pairf b;
		Pairf m_vec;
		Pairf m_normal;
		float m_vecdot;

	public:

		LineDistance2D(const Line2D& line) {
			a = line.fst;
			b = line.snd;

			m_line = line;
			m_vec = b - a;
			m_vecdot = m_vec.norm2();

			m_normal.x = m_vec.y;
			m_normal.y = -m_vec.x;
			m_normal = m_normal.normalized();
		}

		float signedDistance(Pairf pnt) const;
		float signedDistance(float x, float y) const {
			return signedDistance({ x,y });
		};
		float unsignedDistance(Pairf pnt) const;
		float unsignedDistance(float x, float y) const {
			return unsignedDistance({ x,y });
		}

		std::function<float(float, float)> bindSigned() const{
			return[this](float a, float b) {
				return this->signedDistance(a, b);
			};
		}
		
		std::function<float(float, float)> bindUnsigned() const{
			return[this](float a, float b) {
				return this->unsignedDistance(a, b);
			};
		}
	};

}
