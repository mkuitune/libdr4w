// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)

#pragma once
#include <string>

namespace dr4 {
	enum class GeometryResult {
		Success,
		AreaCloseToZero,
		PointsColinear,
		LengthCloseToZero,
		PlanesParallel,
		LinesParallel,
		CantSolveConstraints,
		NoIntersection,
		InputRangeEmpty,
		NotEnoughVertices,
		BasisVectorsNotOrthogonal,
		IncompatibleDimension,
		VolumeCloseToZero
	};

	std::string GeometryErrorToString(GeometryResult e);
}

