// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)

#define GLM_ENABLE_EXPERIMENTAL

#include <dr4/dr4_color.h>

#include <dr4/dr4_math.h>
#include <dr4/dr4_image.h>


namespace dr4 {

		RGBAFloat32 ToRGBAFloat(const SRGBA& bcolor)
		{
			float af = bcolor.a;
			const static float iaf = 1.f / 255.f;
			return{ SRGBUint8ToLinearFloat(bcolor.r),
				SRGBUint8ToLinearFloat(bcolor.g),
				SRGBUint8ToLinearFloat(bcolor.b),
				af * iaf };
		}

		RGBAFloat32 BlendPreMultipliedAlpha(const RGBAFloat32 source, const RGBAFloat32 target)
		{
			const float fb = 1.f - source.a;
			return{ source.r + target.r * fb,
					source.g + target.g * fb,
					source.b + target.b * fb,
					source.a + target.a * fb };
		}
		
		RGBAFloat32 BlendAlpha(const RGBAFloat32 source, const RGBAFloat32 target)
		{
			const float fb = 1.f - source.a;
			return{ source.a * source.r + target.a * target.r * fb,
					source.a * source.g + target.a * target.g * fb,
					source.a * source.b + target.a * target.b * fb,
					source.a + target.a * fb };
		}

		SRGBA ToSRGBA(const RGBAFloat32& fcolor) {
			uint8_t a = (uint8_t)(255.f * clampf(fcolor.a, 0.f, 1.f));// TODO FIXME
			return{ LinearFloatToSRGBUint8(fcolor.r), LinearFloatToSRGBUint8(fcolor.g),
				LinearFloatToSRGBUint8(fcolor.b), a };
		}

		SRGBA ToSRGBA(RGB color) {
			uint8_t a = 255;
			float r = ((float)color.r) / 255.f;
			float g = ((float)color.g) / 255.f;
			float b = ((float)color.b) / 255.f;
			return{ dr4::LinearFloatToSRGBUint8(r), dr4::LinearFloatToSRGBUint8(g),
				dr4::LinearFloatToSRGBUint8(b), a };
		}

		SRGBA ToSRGBA(RGBA color) {
			SRGBA out = ToSRGBA(RGB{ color.r, color.g, color.b });
			out.a = color.a;
			return out;
		}

		RGBA BlendPreMultiplied(RGBA src, RGBA trgt) {

			const float fb = ((float)(255 - src.a))/255.f;
			uint8_t rout = (uint8_t)(src.r + trgt.r * fb);
			uint8_t gout = (uint8_t)(src.g + trgt.g * fb);
			uint8_t bout = (uint8_t)(src.b + trgt.b * fb);
			uint8_t aout = (uint8_t)(src.a + trgt.a * fb);
			return{rout, gout,bout,aout };
		}

		RGBAFloat32 Lerp(const RGBAFloat32& src, const RGBAFloat32& dst, float u) {
			return{ dr4::lerp(src.r, dst.r, u),
				dr4::lerp(src.g, dst.g, u),
				dr4::lerp(src.b, dst.b, u),
				dr4::lerp(src.a, dst.a, u) };
		}
}

