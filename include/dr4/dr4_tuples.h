// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)

#pragma once

namespace dr4
{
	struct Pairf { float x, y; };

	struct PairIdx { size_t x, y; };

	struct Tripletf {
		float x, y, z;
	};

	struct TripletIdx { size_t x, y, z; };

	struct Tripletu {
		unsigned first, second, third;
	};

	struct Quadrupletf { float x, y, z, w; };

}
