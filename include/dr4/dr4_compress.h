#pragma once
#include <string>
//
// Opaque wrapper for third party compression library (snappy)
//
namespace dr4 {
	// Snappy uses std::string as char buffer
	bool Uncompress(const std::string& compressed,	std::string& uncompressed);
	bool UncompressLen(const std::string& compressed, size_t& out);
	bool UncompressBytes(const std::string& compressed, char* bytes);
	void Compress(const std::string& uncompressed,	std::string& compressed);
	void Compress(const char* source,size_t srcLen,	std::string& compresssed);

}