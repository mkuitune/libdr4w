#pragma once

#include <dr4/dr4_unitvector2f.h>
#include <dr4/dr4_tuples.h>
#include <dr4/dr4_math.h>
#include <dr4/dr4_color.h>

#include <vector>
#include <variant>

namespace dr4 {

	struct Line2D {
		Tripletf fst;
		Tripletf snd;
	};

	struct ColorFill {
		RGBA color;
	};

	struct Blend {
		enum class Type {Default};
		Type type;
		Normalized<float> opacity;

		static Blend Default() {
			Blend b;
			b.type = Type::Default;
			b.opacity = 1.0;
			return b;
		}
	};

	struct Material2D {
		float linewidth = 1.0;
		RGBA colorFill;
		RGBA colorLine;
	};

	struct Graphics2D {
		std::vector<Line2D> lines;
	};

	enum class Content2D {Fill, Graphics};

	struct Layer {
		Content2D content; // refers color fill or graphics2d
		size_t idx;
		Blend blend;
	};

	class Scene2D {
	public:
		std::vector<std::shared_ptr<Graphics2D>> m_graphics;
		std::vector<ColorFill> m_colorFills;
		std::vector<Layer> m_layers;
		// todo add ordering of layers ... somewhere
	};

	class Scene2DBuilder {
		Scene2D m_scene;
	public:
		template<class T>
		static size_t lastOf(const T& cont) { return cont.size() - 1; }

		size_t addLayer(std::shared_ptr<Graphics2D> graphics, const Blend& blend) {
			m_scene.m_graphics.push_back(graphics);
			auto idx = lastOf(m_scene.m_graphics);
			m_scene.m_layers.push_back({Content2D::Graphics, idx, blend});
			return lastOf(m_scene.m_layers);
		}

		size_t addLayer(ColorFill fill, const Blend& blend) {
			m_scene.m_colorFills.push_back(fill);
			auto idx = lastOf(m_scene.m_colorFills);
			m_scene.m_layers.push_back({Content2D::Fill, idx, blend});
			return lastOf(m_scene.m_layers);
		}

		Scene2D build() {
			return m_scene;
		}

	};
}
