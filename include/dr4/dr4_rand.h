// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)

#pragma once

#include <stdint.h>

namespace dr4 {

	// from https://en.wikipedia.org/wiki/Xorshift
	struct xorshift64_state {
		uint64_t a;

		uint64_t next()
		{
			uint64_t x = a;
			x ^= x << 13;
			x ^= x >> 7;
			x ^= x << 17;
			return a = x;
		}
	};

	class RandFloat {
		xorshift64_state m_generator = {0xbadf00d};
		double m_bias = 0.0;
		double m_to_range = 1.0 / ((double)UINT64_MAX);
	public:
		RandFloat() {}

		RandFloat(double min, double max)
			:m_bias(min), m_to_range((max - min) / ((double)(UINT64_MAX))) {}

		double rand() {
			return m_bias +  m_to_range * m_generator.next();
		}
	};

	class RandInt {
		xorshift64_state m_generator= {0xbadf00d};
		int64_t m_bias = 0;
		uint64_t m_range = 10;
	public:
		RandInt() {}
		RandInt(int64_t min, int64_t max):m_bias(min), m_range((uint64_t) (max - min)) {
		}
		int64_t rand() {
			return m_bias + ((int64_t)(m_generator.next() % m_range));
		}
	};

}

