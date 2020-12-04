// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)

#define GLM_ENABLE_EXPERIMENTAL

#include <dr4/dr4_math.h>

#include <dr4/dr4_unitvector3f.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtx/quaternion.hpp>


dr4::GeometryTransform dr4::AngleAxisRotation(const UnitVector3f& axis, float angleRadians) {
	auto quat = glm::angleAxis(angleRadians, axis.v);
	glm::mat4 res = glm::toMat4(quat);
	return { res };
}
