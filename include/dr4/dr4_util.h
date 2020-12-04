// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)

#pragma once

namespace dr4 {

	template<class T>
	void DoTimes(size_t count, const T & fun) { for (size_t i = 0; i < count; i++) fun(); }
}
