#pragma once
#include <string>
//
// Opaque wrapper for third party compression library (snappy)
//
namespace dr4 {
	// Snappy uses std::string as char buffer
	void Uncompress(const std::string& source,	std::string& output);
	void Compress(const std::string& source,	std::string& output);

}