#pragma once

#include <dr4/dr4_task.h>
#include <dr4/dr4_scene2d.h>
#include <dr4/dr4_camera.h>
#include <dr4/dr4_image.h>

namespace dr4 {

	class FrameTasks {
	public:
		ITask::Collection tasks;
	};

	// Rasterizer is an interface to rendering algorithm
	// Rendering itself is performed by ITask sequentially or in parallel
	class IRasterizer {
	public:
		virtual ~IRasterizer() {}
		// Append to task queue in frame tasks. Frame task results are to be applied to the render buffer in order
		virtual void draw2D(RasterConfig2D config, const Scene2D& scene, FrameTasks& tasks) = 0;
		virtual void applyResult(FrameTasks& tasks) = 0;
		//virtual void draw(Camera3D camera, const Scene3D& scene) = 0;
		virtual ImageRGBA8SRGB getColorAsSRGB() const = 0;
	};
	
	std::shared_ptr<IRasterizer> CreateRasterizer(unsigned width, unsigned height);

}
