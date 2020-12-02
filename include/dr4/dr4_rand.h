#pragma once

#include <stdint.h>

namespace dr4 {

	class RandXoshiro256ppUniform {
		uint64_t s[4];
		void jump();
		void long_jump();
		uint64_t next();
	public:
		uint64_t rand();

	};

	class RandFloat {
		RandXoshiro256ppUniform m_generator;
		double m_min = 0.0;
		double m_max = 1.0;
	public:
		RandFloat();
		RandFloat(double min, double max);
		double rand();
	};
	
	class RandInt {
		RandXoshiro256ppUniform m_generator;
		int64_t m_min = 0;
		int64_t m_max = 10;
	public:
		RandInt();
		RandInt(int64_t min, int64_t max);
		int64_t rand();
	};

}

