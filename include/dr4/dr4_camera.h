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
#include <dr4/dr4_rasterizer_area.h>

namespace dr4 {
	struct ViewPort {
		unsigned width;
		unsigned height;

		float getAspectRatio() const noexcept{
			float w = (float)width;
			float h = (float)height;
			return w / h;
		}
	};

	struct LinearMap2D {
		float s;
		Pairf offset;
		Pairf origin;

		Pairf map(const Pairf& p) const {
			return ((p + offset) * s) + origin;
		}
	};
	// RasterConfig2D is a 'Camera2D' contains all related information for mapping between pixel- and scenespace
	// - not really a _camera_ 
	struct RasterConfig2D /* Could be as well ViewConfig2D but now naming follows 3D styling */{
		float scale;
		float iscale;
		// The 'up' is always y-up

		RasterDomain m_rasterDomain;
		SceneDomain m_sceneDomain;

		inline LinearMap2D rasterToScene() const {
			Pairf offset = {-1.f * ((float) m_rasterDomain.origin.x), -1.f * ((float)m_rasterDomain.origin.y)};
			Pairf origin = { m_sceneDomain.span.x.min, m_sceneDomain.span.y.min };
			return { iscale, offset, origin};
		}
		inline LinearMap2D sceneToRaster() const {
			Pairf offset = { -m_sceneDomain.span.x.min, -m_sceneDomain.span.y.min };
			float originx = (float)m_rasterDomain.origin.x;
			float originy = (float)m_rasterDomain.origin.y;
			Pairf origin = { originx, originy };
			return { scale, offset, origin };
		}

		static RasterConfig2D Create(
			RasterDomain rasterDomain,
			SceneDomain sceneDomain
		) {
			RasterConfig2D cfg;
			cfg.m_rasterDomain = rasterDomain;
			cfg.m_sceneDomain = sceneDomain;
			cfg.scale = ((float)cfg.m_rasterDomain.width) / cfg.m_sceneDomain.span.x.length();
			cfg.iscale = 1.f / cfg.scale;
			return cfg;
		}

#if 0
		static RasterConfig2D Default(){
			RasterConfig2D cfg;
			cam.scale = 1.0f;
			cam.center = {0.0f, 0.0f};
			return cam;
		}
#endif
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
