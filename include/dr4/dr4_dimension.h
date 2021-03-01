#pragma once

#include <dr4/dr4_geometryresult.h>

namespace dr4 {
	enum class Dimension : unsigned char { X = 0, Y = 1, Z = 2 };
	enum class Dimension2D : unsigned char { X = 0, Y = 1};

	inline int DimensionToInt(Dimension d) {
		switch (d) {
		case Dimension::X:return 0;
		case Dimension::Y:return 1;
		case Dimension::Z:return 2;
		}
	}

	inline GeometryResult IntToDimension(int i, Dimension& dim)
	{
		GeometryResult result = GeometryResult::Success;
		switch (i) {
		case 0: dim = Dimension::X; break;
		case 1: dim = Dimension::Y; break;
		case 2: dim = Dimension::Z; break;
		default:
			result = GeometryResult::IncompatibleDimension;
		}
		return result;
	}
	
	inline GeometryResult IntToDimension2D(int i, Dimension2D& dim)
	{
		GeometryResult result = GeometryResult::Success;
		switch (i) {
		case 0: dim = Dimension2D::X; break;
		case 1: dim = Dimension2D::Y; break;
		default:
			result = GeometryResult::IncompatibleDimension;
		}
		return result;
	}
}

