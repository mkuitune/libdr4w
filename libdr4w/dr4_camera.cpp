#include <dr4/dr4_camera.h>
#include <dr4/dr4_math.h>

float dr4::ViewSetup2D::getScale() const {
	auto diagonal = upperRight - lowerLeft;
	auto maxelem = MaximumElement2D(diagonal);
	return maxelem.value;
}

dr4::ViewSetup2D dr4::ViewSetup2D::CreateForView(const dr4::ViewPort port, const dr4::Camera2D cfg) {
	const float aspect = port.getAspectRatio();

	float viewwidth, viewheight;
	if (aspect > 1.0) {
		viewwidth = cfg.scale;
		viewheight = viewwidth / aspect;
	}
	else /* w <= h */ {
		viewheight = cfg.scale;
		viewwidth = viewheight * aspect;
	}

	ViewSetup2D view;
	view.lowerLeft.x = cfg.center.x - (0.5f * viewwidth);
	view.lowerLeft.y = cfg.center.y - (0.5f * viewheight);
	view.upperRight.x = cfg.center.x + (0.5f * viewwidth);
	view.upperRight.y = cfg.center.y + (0.5f * viewheight);
	return view;
}
