#pragma once
#include<stdint.h>
namespace dr4 {

	enum class CoreTypes{
		// Aliases to float buffer
		VertexBuffer,
		Image2D,
		// Aliases to 
	};

	struct TypeInfo {
		uint64_t type : 32;
		uint64_t idx : 32;
	};
}
