#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <stdint.h>

namespace dr4 {

	std::string preparePathForWriting(const char* pathstr);
	std::string writeBytesToPath(const std::vector<uint8_t>& bytes, const char* pathstr);

	typedef std::pair<std::unique_ptr<std::vector<uint8_t>>, std::string> array_result_t;

	array_result_t readBytesFromPath(const char* pathStr);

	typedef std::pair<std::string, std::string> string_result_t;
	string_result_t readFileFromPathToString(const std::string& pathStr);

	std::string FileNameStem(const std::string& path);
}
