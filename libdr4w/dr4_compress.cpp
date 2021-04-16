#include <dr4/dr4_compress.h>
#include <snappy/snappy.h>

bool dr4::Uncompress(const std::string& compressed, std::string& uncompressed){
	return snappy::Uncompress(compressed.data(), compressed.size(), &uncompressed);
}

bool dr4::UncompressLen(const std::string& compressed, size_t& out){
	return snappy::GetUncompressedLength(compressed.data(), compressed.size(), &out);
}

bool dr4::UncompressBytes(const std::string& compressed, char* bytes){
	return snappy::RawUncompress(compressed.data(), compressed.size(), bytes);
}

void dr4::Compress(const std::string& uncompressed, std::string& compressed){
	snappy::Compress(uncompressed.data(), uncompressed.length(), &compressed);
}

void dr4::Compress(const char* source, size_t srcLen, std::string& compressed)
{
	snappy::Compress(source, srcLen, &compressed);
}
