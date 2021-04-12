// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)

#include <dr4/dr4_geometryresult.h>

std::string dr4::GeometryErrorToString(dr4::GeometryResult e)
{
	switch (e) {
	case GeometryResult::Success: return "GeometryError::Success";
	case GeometryResult::AreaCloseToZero:return "GeometryError::AreaCloseToZero";
	case GeometryResult::LengthCloseToZero: return "GeometryError::LengthCloseToZero";
	case GeometryResult::PlanesParallel: return "GeometryError::PlanesParallel";
	case GeometryResult::LinesParallel: return "GeometryError::LinesParallel";
	case GeometryResult::CantSolveConstraints: return "GeometryError::CantSolveConstraints";
	case GeometryResult::NoIntersection: return "GeometryError::NoIntersection";
	case GeometryResult::InputRangeEmpty: return "GeometryError::InputRangeEmpty";
	case GeometryResult::InputElementCountTooSmall: return "GeometryError::InputElementCountTooSmall";
	case GeometryResult::NotEnoughVertices: return "GeometryError::NotEnoughVertices";
	case GeometryResult::BasisVectorsNotOrthogonal: return "GeometryError::BasisVectorNotOrghogonal";
	case GeometryResult::IncompatibleDimension: return "GeometryError::IncompatibleDimension";
	case GeometryResult::PointsColinear: return "GeometryError::PointsColinear";
	case GeometryResult::VolumeCloseToZero: return "GeometryError::VolumeCloseToZero";
	default: return "GeometryError::<Unknown value>";
	}

}
