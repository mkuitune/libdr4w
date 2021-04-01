// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)

#define GLM_ENABLE_EXPERIMENTAL

#include <dr4/dr4_color.h>

#include <dr4/dr4_math.h>
#include <dr4/dr4_image.h>
#include <dr4/dr4_floatingpoint.h>
#include <dr4/dr4_analysis.h>

namespace dr4 {

	// from http://paulbourke.net/miscellaneous/colourspace/
	// https://en.wikipedia.org/wiki/SRGB

	constexpr float D65x = 0.9504f;
	constexpr float D65y = 1.0f;
	constexpr float D65z = 1.0888f;

	inline static Quadrupletf rgbToXyz(const RGBAFloat32& c) {

		float x = c.r * 0.4124f + c.g * 0.3576f + c.b * 0.1805f;
		float y = c.r * 0.2126f + c.g * 0.7152f + c.b * 0.0722f;
		float z = c.r * 0.0193f + c.g * 0.1192f + c.b * 0.9505f;

		return { x, y, z , c.a};
	}

	inline static RGBAFloat32 xyzToRgb(const Quadrupletf& xyz) {
		RGBAFloat32 rgb;

		rgb.r = xyz.x * 3.2406f + xyz.y * (-1.5372f) + xyz.z * (-0.4986f);
		rgb.g = xyz.x * (-0.9689f) + xyz.y * 1.8758f + xyz.z * 0.0415f;
		rgb.b = xyz.x * 0.0557f + xyz.y * (-0.2040f) + xyz.z * 1.0570f;

		rgb.r = clampf(rgb.r, 0.f, 1.f);
		rgb.g = clampf(rgb.g, 0.f, 1.f);
		rgb.b = clampf(rgb.b, 0.f, 1.f);
		rgb.a = xyz.w;

		return rgb;
	}

	static LABFloat32 xyzToLab(const Quadrupletf xyz) {
		float X = xyz.x / D65x;
		float Y = xyz.y / D65y;
		float Z = xyz.z / D65z;

		if (X > 0.008856f)
			X = powf(X, 1.f / 3.0f);
		else
			X = (903.3f * X + 16.0f) / 116.0f;
		if (Y > 0.008856f)
			Y = powf(Y, 1 / 3.0f);
		else
			Y = (903.3f * Y + 16.0f) / 116.0f;
		if (Z > 0.008856f)
			Z = powf(Z, 1 / 3.0f);
		else
			Z = (903.3f * Z + 16.0f) / 116.0f;

		float l = (116 * Y) - 16;
		float a = 500 * (X - Y);
		float b = 200 * (Y - Z);
		float alpha = xyz.w;
		return {l,a,b,alpha};
	}

	// Conversion from xyz to lab uses D65 reference illuminate whitepoint
	static Quadrupletf labToXyz(LABFloat32 lab) {
		float Y = (lab.l + 16.0f) / 116.0f;
		float X = lab.a / 500.0f + Y;
		float Z = Y - lab.b / 200.0f;

		//Y = Adjust(Y);
		//X = Adjust(X);
		//Z = Adjust(Z);
		//Adjust:=
		//if (Math.pow(a, 3.0) > 0.008856)
		//		return(Math.pow(a, 3.0));
		//	else
		//		return((a - 16.0 / 116.0) / 7.787);
		
		float y3 = Y * Y * Y;
		float x3 = X * X * X;
		float z3 = Z * Z * Z;

		Y = (y3 > 0.008856f) ? y3 : ((Y - 16.0f / 116.0f) / 7.787f);
		X = (x3 > 0.008856f) ? x3 : ((X - 16.0f / 116.0f) / 7.787f);
		Z = (z3 > 0.008856f) ? z3 : ((Z - 16.0f / 116.0f) / 7.787f);

		Quadrupletf  res = { X * D65x, Y * D65y, Z * D65z, lab.alpha };
		return res;
	}

	RGBAFloat32 ToRGBAFloat(const LABFloat32& lab)
	{
		Quadrupletf xyz = labToXyz(lab);
		return xyzToRgb(xyz);
	}

	LABFloat32 ToLAB(const RGBAFloat32& col)
	{
		Quadrupletf xyz = rgbToXyz(col);
		return xyzToLab(xyz);
	}

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

	// The Porter-Duff 'Over' operator https://en.wikipedia.org/wiki/Alpha_compositing
	RGBAFloat32 BlendAlpha(const RGBAFloat32 source, const RGBAFloat32 target)
	{
		float fb = 1.f - source.a;
		float a0 = source.a + target.a * fb;

		return{ (source.a * source.r + target.a * target.r * fb) / a0,
				(source.a * source.g + target.a * target.g * fb) / a0,
				(source.a * source.b + target.a * target.b * fb) / a0,
				a0};
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

		const float fb = ((float)(255 - src.a)) / 255.f;
		uint8_t rout = (uint8_t)(src.r + trgt.r * fb);
		uint8_t gout = (uint8_t)(src.g + trgt.g * fb);
		uint8_t bout = (uint8_t)(src.b + trgt.b * fb);
		uint8_t aout = (uint8_t)(src.a + trgt.a * fb);
		return{ rout, gout,bout,aout };
	}

	RGBAFloat32 Lerp(const RGBAFloat32& src, const RGBAFloat32& dst, float u) {
		return{ dr4::lerp(src.r, dst.r, u),
			dr4::lerp(src.g, dst.g, u),
			dr4::lerp(src.b, dst.b, u),
			dr4::lerp(src.a, dst.a, u) };
	}

	LookUpTable<RGBAFloat32> GradientToLUT(const GradientFloat32& gradient) {
		using namespace std;
		//const size_t nSamples = 10;
		const size_t nSamples = 1024;
		//const size_t nSamples = 7;
		const size_t samplesPerSpan = 10;
		vector<Pairf> lKeys, aKeys, bKeys, alphaKeys;

		for (auto p : gradient) {
			LABFloat32 lab = ToLAB(p.second);
			lKeys.push_back({p.first, lab.l});
			aKeys.push_back({p.first, lab.a});
			bKeys.push_back({p.first, lab.b});
			alphaKeys.push_back({p.first, lab.alpha});
		}

		auto sl = Interpolate2(lKeys, samplesPerSpan);
		auto sa = Interpolate2(aKeys, samplesPerSpan);
		auto sb = Interpolate2(bKeys, samplesPerSpan);
		auto salpha = Interpolate2(alphaKeys, samplesPerSpan);

		Analysis::TextDump(Analysis::ToCSV(sl), "sl.csv");
		Analysis::TextDump(Analysis::ToCSV(sa), "sa.csv");
		Analysis::TextDump(Analysis::ToCSV(sb), "sb.csv");

		auto lutl = sl.createByXLUT(nSamples);
		auto luta = sa.createByXLUT(nSamples);
		auto lutb = sb.createByXLUT(nSamples);
		auto lutalpha = salpha.createByXLUT(nSamples);


		bool rangesCorrect = 
			Float32::GeometryAreEqual(lutl.sourceStart(), luta.sourceStart()) && 
			Float32::GeometryAreEqual(luta.sourceStart(), lutb.sourceStart()) &&
			Float32::GeometryAreEqual(lutb.sourceStart(), lutalpha.sourceStart()) &&
			Float32::GeometryAreEqual(lutl.sourceEnd(), luta.sourceEnd()) && 
			Float32::GeometryAreEqual(luta.sourceEnd(), lutb.sourceEnd()) &&
			Float32::GeometryAreEqual(lutb.sourceEnd(), lutalpha.sourceEnd());
		assert(rangesCorrect);

		float rangeMin = lutl.sourceStart();
		float rangeMax = lutl.sourceEnd();;

		vector<RGBAFloat32> samples(nSamples);

		for (size_t i = 0; i < nSamples; i++) {
			LABFloat32 lab = {lutl.getData()[i], luta.getData()[i], lutb.getData()[i], lutalpha.getData()[i]};
			RGBAFloat32 rgb = ToRGBAFloat(lab);
			samples[i] = rgb;
		}

		return LookUpTable<RGBAFloat32>::Create(samples, rangeMin, rangeMax);
	}
}

