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


		static RGBA ToPremultiplied(uint8_t r, uint8_t g, uint8_t b, float a) {
			uint8_t aOut = (uint8_t) (255.f * a);
			uint8_t rout = (uint8_t) ((float)(r) * a);
			uint8_t gout = (uint8_t) ((float)(g) * a);
			uint8_t bout = (uint8_t) ((float)(b) * a);

			return { rout,gout,bout, aOut };
		}
		
		static RGBA ToPremultiplied(RGB rgb, float a) {
			uint8_t aOut = (uint8_t) (255.f * a);
			uint8_t rout = (uint8_t) ((float)(rgb.r) * a);
			uint8_t gout = (uint8_t) ((float)(rgb.g) * a);
			uint8_t bout = (uint8_t) ((float)(rgb.b) * a);
			return { rout, gout, bout, aOut };
		}
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

		static RGBAFloat32 CreatePremultiplied(float r, float g, float b, float a) {
			return { r * a, g * a, b * a, a };
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
		static RGBAFloat32 Orange() { return{ 1.f, 0.65f, 0.f, 1.f }; }
		static RGBAFloat32 Navy() { return{ 0.f, 0.0f, 0.502f, 1.f }; }

		static RGBAFloat32 Pink() { return{ 1.f, 105.f / 255.f, 180.f / 255.f, 1.f }; } // Actually HotPink...
	};

	RGBAFloat32 ToRGBAFloat(const SRGBA& bcolor);
	RGBA ToRGBA(const SRGBA& bcolor);
	RGBAFloat32 BlendPreMultipliedAlpha(const RGBAFloat32 source, const RGBAFloat32 target);
	RGBAFloat32 BlendAlpha(const RGBAFloat32 source, const RGBAFloat32 target);
	SRGBA ToSRGBA(const RGBAFloat32& fcolor);
	SRGBA ToSRGBA(RGB fcolor);
	SRGBA ToSRGBA(RGBA fcolor);
	RGBA BlendPreMultiplied(RGBA src, RGBA trgt);
	RGBAFloat32 Lerp(const RGBAFloat32& src, const RGBAFloat32& dst, float u);
}
