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

	//
	// Generic compressor api for POD data stored in std::vector
	//
	template<class VEC>
	bool UncompressGeneric(const std::string& compressed, VEC& out){
			size_t len;
			bool succ = UncompressLen(compressed, len);
			if (!succ) {
				return false;
			}

			size_t nelem = len / sizeof(typename VEC::value_type);
			out = VEC(nelem);
			succ = UncompressBytes(compressed, (char*)&out[0]);
			if (!succ) {
				return false;
			}
	}
	template<class VEC>
	void CompressGeneric(const VEC& vec, std::string& compressed){
		const char* data = (const char*)&vec[0];
		size_t bytecount = vec.size() * sizeof(typename VEC::value_type);
		Compress(data, bytecount, compressed);
	}

}