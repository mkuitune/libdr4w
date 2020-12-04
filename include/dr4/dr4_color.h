// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)

#pragma once

#include <stdint.h>
#include <limits>

namespace dr4 {

	struct SRGBA {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	struct RGB {
		uint8_t r;
		uint8_t g;
		uint8_t b;
	};

	struct RGBA {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	/** Linear colorspace */
	struct RGBAFloat32 {
		float r;
		float g;
		float b;
		float a;

		bool operator==(const RGBAFloat32& c) { return r == c.r && g == c.g && b == c.b && a == c.a; }
		uint64_t toHash() const {
			uint16_t mv = std::numeric_limits<uint16_t>::max();
			uint64_t rs = (uint64_t)(r * (uint64_t)mv);
			uint64_t gs = (uint64_t)(g * (uint64_t)mv);
			uint64_t bs = (uint64_t)(b * (uint64_t)mv);
			uint64_t as = (uint64_t)(a * (uint64_t)mv);
			uint64_t res = (rs << 48) | (gs << 32) | (bs << 16) | as;
			return res;
		}

		static RGBAFloat32 Create(float v) { return { v, v, v, 1.f }; }
		static RGBAFloat32 Create(float r, float g, float b, float a) { return { r, g, b, a }; }

		static RGBAFloat32 Red() { return{ 1.f, 0.f, 0.f, 1.f }; }
		static RGBAFloat32 Green() { return{ 0.f, 1.f, 0.f, 1.f }; }
		static RGBAFloat32 Blue() { return{ 0.f, 0.f, 1.f, 1.f }; }
		static RGBAFloat32 Cyan() { return{ 0.f, 1.f, 1.f, 1.f }; }

		static RGBAFloat32 Violet() { return{ 1.f, 0.f, 1.f, 1.f }; }
		static RGBAFloat32 Yellow() { return{ 1.f, 1.f, 0.f, 1.f }; }
		static RGBAFloat32 Black() { return{ 0.f, 0.f, 0.f, 1.f }; }
		static RGBAFloat32 White() { return{ 1.f, 1.f, 1.f, 1.f }; }

		static RGBAFloat32 Pink() { return{ 1.f, 105.f / 255.f, 180.f / 255.f, 1.f }; } // Actually HotPink...
	};

	RGBAFloat32 ToRGBAFloat(const SRGBA& bcolor);
	RGBAFloat32 BlendPreMultipliedAlpha(const RGBAFloat32 source, const RGBAFloat32 target);
	SRGBA ToSRGBA(const RGBAFloat32& fcolor);
	SRGBA ToSRGBA(RGB fcolor);
	SRGBA ToSRGBA(RGBA fcolor);
	RGBAFloat32 Lerp(const RGBAFloat32& src, const RGBAFloat32& dst, float u);
}
