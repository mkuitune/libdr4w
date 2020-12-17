#pragma once

#include<memory>
#include <dr4/dr4_scene2d.h>
#include <dr4/dr4_camera.h>
#include <dr4/dr4_image.h>

namespace dr4 {

	class IRasterizer {
	public:
		virtual ~IRasterizer() {}
		virtual void draw(Camera2D camera, const Scene2D& scene) = 0;
		virtual ImageRGBA8SRGB getColorAsSRGB() const = 0;
	};
	
	std::shared_ptr<IRasterizer> CreateRasterizer(unsigned width, unsigned height);

}
