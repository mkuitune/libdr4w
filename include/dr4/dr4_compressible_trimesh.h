#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <dr4/dr4_compress.h>
#include <dr4/dr4_tuples.h>

namespace dr4 {
	class CompressibleTrimesh {
		std::vector<Tripletf> vertices; //each 


		//
		// Compression API
		//
		void compress(std::string& out) const {
			// size, bytes
			const char* data = (const char*)&vertices[0];
			size_t bytecount = vertices.size() * sizeof(Tripletf);
			Compress(data, bytecount, out);
		}

		static CompressibleTrimesh Uncompress(const std::string& buffer) {
			size_t len;
			bool succ = UncompressLen(buffer, len);
			if (!succ) {
				// TODO BETTER ERROR	
				return {};
			}

			size_t nvertices = len / sizeof(Tripletf);
			CompressibleTrimesh mesh;
			mesh.vertices = std::vector<Tripletf>(nvertices);
			succ = UncompressBytes(buffer, (char*)&mesh.vertices[0]);
			if (!succ) {
				// TODO BETTER ERROR	
				return {};
			}
			return mesh;
		}
	};
}
