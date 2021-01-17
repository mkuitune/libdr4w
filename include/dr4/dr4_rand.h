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

		constexpr static xorshift64_state Default() { return { 0xbadf00d }; }
	};
	
	class RandIntGenerator{
		xorshift64_state m_generator = xorshift64_state::Default();
	public:
		RandIntGenerator() {}
		int64_t next() {
			return m_generator.next();
		}
	};

	class RandFloat {
		//xorshift64_state m_generator = {0xbadf00d};
		RandIntGenerator m_generator;
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

	struct IntegerRange {
		int64_t m_bias = 0;
		uint64_t m_range = 10;
		IntegerRange() {}
		IntegerRange(int64_t min, int64_t max) :m_bias(min), m_range((uint64_t)(max - min)) {}
		int64_t cast(const int64_t& ival) {
			return m_bias + ((int64_t)(ival % m_range));
		}
		int64_t rand(RandIntGenerator& gen) {
			return cast(gen.next());
		}
	};

	class RandInt {
		RandIntGenerator m_generator;
		IntegerRange range;
	public:
		RandInt() {}
		RandInt(int64_t min, int64_t max):range(min, max) {}
		int64_t rand() {
			return range.rand(m_generator);
		}
	};

}

