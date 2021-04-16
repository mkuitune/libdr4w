#include <dr4/dr4_compress.h>
#include <snappy/snappy.h>

void dr4::Uncompress(const std::string& compressed, std::string& uncompressed){
	snappy::Uncompress(compressed.data(), compressed.size(), &uncompressed);
}

void dr4::Compress(const std::string& uncompressed, std::string& compressed){
	snappy::Compress(uncompressed.data(), uncompressed.length(), &compressed);
}
