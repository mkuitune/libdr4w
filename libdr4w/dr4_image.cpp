// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)

#include <dr4/dr4_image.h>
#include <dr4/dr4_io.h>

#include <filesystem>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>
#undef STB_IMAGE_RESIZE_IMPLEMENTATION

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#undef STB_IMAGE_WRITE_IMPLEMENTATION

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

//
// Seq - TODO - replace
//
template<class T>
class Sequence1D {
	T m_begin;
	T m_end;

public:
	struct const_iterator {
		T val;
		void operator++() { val += 1; }
		bool operator!=(const const_iterator& rhs) { return val != rhs.val; }
		const T& operator*() { return val; }
	};

	Sequence1D(T fst, T snd)
		: m_begin(fst), m_end(snd) {}
	Sequence1D(T snd)
		: m_begin(0), m_end(snd) {}
	const_iterator begin() const { return { m_begin }; }
	const_iterator end() const { return { m_end }; }
};

template<class T>
inline Sequence1D<T> make_seq(T endSize) { return Sequence1D<T>(endSize); }

typedef Sequence1D<size_t> Sequence1Ds;
typedef Sequence1D<int> Sequence1Di;
//
// Seq end
//

uint8_t dr4::LinearFloatToSRGBUint8(const float f) {
	return stbir__linear_to_srgb_uchar(f);
}

float dr4::SRGBUint8ToLinearFloat(const uint8_t u) {
	return stbir__srgb_uchar_to_linear_float[u];
}

dr4::ImageRGBA8SRGB dr4::convertToSrgb(const ImageRGBA32Linear& linear)
{
	ImageRGBA8SRGB res(linear.size());
	auto seq = make_seq(linear.elementCount());
	for (auto i : seq) {
		res.at(i) = ToSRGBA(linear.at(i));
	}
	return res;
}

dr4::ImageRGBA32Linear dr4::convertToLinear(const ImageRGBA8SRGB& srgb)
{
	ImageRGBA32Linear res(srgb.size());
	auto seq = make_seq(srgb.elementCount());
	for (auto i : seq) {
		res.at(i) = ToRGBAFloat(srgb.at(i));
	}
	return res;
}

static void write_bytes(void* context, void* data, int size) {
	std::vector<uint8_t>* bytes = (std::vector<uint8_t>*)(context);
	uint8_t* byteData = (uint8_t*)data;
	*bytes = std::vector<uint8_t>(byteData, byteData + size);
}

std::string dr4::writeImageAsPng(const ImageRGBA8SRGB& image, const char* path) {
	std::vector<uint8_t> byteArray;
	int width = (int)image.dim1();
	int height = (int)image.dim2();
	int rowlength = 4 * width;
	stbi_write_png_to_func(write_bytes, (void*)&byteArray, width, height, 4, image.data(), rowlength);
	return writeBytesToPath(byteArray, path);
}

std::pair<std::unique_ptr<dr4::ImageRGBA8SRGB>, std::string> dr4::readImage(const std::string& strPath)
{
	using namespace std;

	const char* path = strPath.c_str();
	array_result_t bytesResult = readBytesFromPath(path);
	if (!bytesResult.second.empty()) {
		return std::make_pair(nullptr, bytesResult.second);
	}
	std::vector<uint8_t>& bytes(*bytesResult.first);

	int len = (int)bytes.size();
	int x, y, channelsInFile;

	if (!stbi_info_from_memory(bytes.data(), len, &x, &y, &channelsInFile)) {
		return std::make_pair(nullptr, std::string("Could not read data in file as image:") + path);
	}

	SRGBA full = { 0xff, 0xaa, 0xaa, 0xff };

	auto img = make_unique<ImageRGBA8SRGB>((size_t)x, (size_t)y, full);

	stbi_uc* stbBuf = stbi_load_from_memory(bytes.data(), len, &x, &y, &channelsInFile, 4);

	if (!stbBuf) {
		return std::make_pair(nullptr, std::string("Could not read data in file as image:") + path);
	}

	uint8_t* dataOut = (uint8_t*)img->asVector().data();
	size_t elemCount = img->elementCount() * 4;
	for (size_t i = 0; i < elemCount; i++) {
		dataOut[i] = (uint8_t)stbBuf[i];
	}

	stbi_image_free(stbBuf);

	return std::make_pair(
		std::unique_ptr<ImageRGBA8SRGB>(std::move(img)), std::string());
}

std::pair<std::unique_ptr<dr4::ImageRGBA8SRGB>, std::string>
dr4::readImageFromBytes(const uint8_t* buffer, int bufSize)
{
	using namespace std;

	int len = bufSize;
	int x, y, channelsInFile;
	if (!stbi_info_from_memory(buffer, len, &x, &y, &channelsInFile)) {
		return std::make_pair(nullptr, std::string("Could not read data in buffer as image:"));
	}

	//Sample4b full = {0xff, 0xaa, 0xaa, 0xff};
	SRGBA full = { 0xff, 0xaa, 0xaa, 0xff };

	auto img = make_unique<ImageRGBA8SRGB>((size_t)x, (size_t)y, full);

	stbi_uc* stbBuf = stbi_load_from_memory(buffer, len, &x, &y, &channelsInFile, 4);

	if (!stbBuf) {
		return std::make_pair(nullptr, std::string("Could not read data in buffer as image:"));
	}

	uint8_t* dataOut = (uint8_t*)img->asVector().data();
	size_t elemCount = img->elementCount() * 4;
	for (size_t i = 0; i < elemCount; i++) {
		dataOut[i] = (uint8_t)stbBuf[i];
	}

	stbi_image_free(stbBuf);

	return std::make_pair(
		std::unique_ptr<ImageRGBA8SRGB>(std::move(img)), std::string());
}
