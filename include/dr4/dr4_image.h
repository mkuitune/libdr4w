// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)
#pragma once

#include <dr4/dr4_color.h>
#include <dr4/dr4_array2d.h>

#include <filesystem>
#include <string>

namespace dr4 {

	typedef Array2D<SRGBA> ImageRGBA8SRGB;
	typedef Array2D<RGBAFloat32> ImageRGBA32Linear;

	uint8_t LinearFloatToSRGBUint8(const float f);
	float SRGBUint8ToLinearFloat(const uint8_t u);

	ImageRGBA8SRGB convertToSrgb(const ImageRGBA32Linear& linear);
	ImageRGBA32Linear convertToLinear(const ImageRGBA8SRGB& srgb);

	std::string writeImageAsPng(const ImageRGBA8SRGB& image, const char* pathStr);

	// TODO use std::path instead
	std::pair<std::unique_ptr<ImageRGBA8SRGB>, std::string> readImage(const std::string& str);

	std::pair<std::unique_ptr<ImageRGBA8SRGB>, std::string> readImageFromBytes(const uint8_t* buffer, int bufSize);
}

