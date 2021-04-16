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
			CompressGeneric(vertices, out);
		}

		static CompressibleTrimesh Uncompress(const std::string& buffer) {
			CompressibleTrimesh mesh;
			bool succ = UncompressGeneric(buffer, mesh.vertices);
			if (!succ) {
				// TODO BETTER ERROR handling
				return {};
			}
			return mesh;
		}
	};
}
