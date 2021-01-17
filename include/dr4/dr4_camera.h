// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)
#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <dr4/dr4_unitvector3f.h>
#include <stdint.h>

namespace dr4 {

	// REMOVE THIS TEXT Rasterizer operation is decoupled from camera configuration


	struct ViewPort {
		unsigned width;
		unsigned height;

		float getAspectRatio() const noexcept{
			float w = (float)width;
			float h = (float)height;
			return w / h;
		}
	};

	// This is similar to camera in 3d
	struct Camera2D /* Could be as well ViewConfig2D but now naming follows 3D styling */{
		float scale;
		glm::vec2 center;
		// The 'up' is always y-up

		static Camera2D Default(){
			Camera2D cam;
			cam.scale = 1.0f;
			cam.center = {0.0f, 0.0f};
			return cam;
		}
	};

	struct ViewSetup2D {
		glm::vec2 lowerLeft;
		glm::vec2 upperRight;
		
		float getScale() const;
		static ViewSetup2D CreateForView(const ViewPort port, const Camera2D cfg);
	};


	struct ViewConfig {
		glm::vec3 position;
		UnitVector3f direction;
		// Note - to make sure the upreference is never colinear with direction one needs to
		// 1. When initializing camera, make sure upReference and direction are non-colinear
		// 2. When updating existing camera, make sure the change does not result in camera that has
		//    up and direction as colinear
		UnitVector3f upReference;
		float aspectRatio;
	};

	struct CameraOrtho {
		ViewConfig view;
		float scale = 1.0;
	};

	struct CameraPerspective {
		ViewConfig view;
		float fieldOfView = 45.0;
	};
}
