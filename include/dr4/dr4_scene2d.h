//
// Scene management related definitions
//
#pragma once

#include <dr4/dr4_unitvector2f.h>
#include <dr4/dr4_tuples.h>
#include <dr4/dr4_math.h>
#include <dr4/dr4_color.h>
#include <dr4/dr4_shapes.h>

#include <memory>
#include <vector>
#include <variant>

namespace dr4 {

	struct Line2DCollection {
		std::vector<Line2D> lines;
		size_t material;
		void append(Line2D line) { lines.push_back(line); }
	};

	enum class BlendType { Default };
	struct Blend {
		BlendType type;
		Normalized<float> opacity;

		static Blend Default() {
			Blend b;
			b.type = BlendType::Default;
			b.opacity = 1.0;
			return b;
		}
	};

	struct Material2D {
		float linewidth = 1.0;
		RGBAFloat32 colorFill;
		RGBAFloat32 colorLine;

		static Material2D CreateDefault() { return {1.0f, RGBAFloat32::White(), RGBAFloat32::Black()}; }
	};

	struct ColorFill {
		RGBAFloat32 colorFill;
		
		static ColorFill CreateDefault() { return {RGBAFloat32::Red()}; }
	};

	enum class Content2D { Fill, Lines };

	struct Graphics2DElement {
		Content2D content; // refers content type
		size_t idx; /* Index to content type buffer of type specified by 'content'*/
	};

	struct Layer {
		std::vector<Graphics2DElement> graphics;
		Blend blend;
	};

	struct ScalarPresentation {
		enum class Unit { Cm, Mm, Meter };
		float scale;
		Unit dimension;

		// Scene data is always _unitless_. ScalarPresentation defines
		// conversion from scene data to scalar presentation with specific dimensions
		// as value-physical = value * scale \in <dimension> where value is the unitless
		// scene value.
	};

	class Scene2D {
	public:

		// todo should there be some bounds or specific origin (against which all the data id)
		// another option is to define a coordsys only on insert, otherwise everything is in world coordinates

		std::vector<Line2DCollection> lines;
		std::vector<ColorFill> colorFills;
		std::vector<Layer> layers;
		// todo add ordering of layers ... somewhere
		std::vector<Material2D> materials;

		ScalarPresentation scalarPresentation = { 1.0f, ScalarPresentation::Unit::Cm };

	};


	class Scene2DBuilder {
		Scene2D m_scene;
		size_t m_default_material;
	public:
		template<class T>
		static size_t lastOf(const T& cont) { return cont.size() - 1; }

		Scene2DBuilder() {
			Material2D defmat;
			defmat.linewidth = 1.0f;
			defmat.colorFill = RGBAFloat32::Red();
			defmat.colorLine = RGBAFloat32::Black();
			m_default_material = addMaterial(defmat);
		}

		size_t addMaterial(Material2D mat2d) {
			m_scene.materials.push_back(mat2d);
			return m_scene.materials.size() - 1;
		}

		size_t addLayer() {
			Layer lr;
			lr.blend = Blend::Default();
			m_scene.layers.push_back(lr);
			return lastOf(m_scene.layers);
		}

		void add(size_t layer, const Line2DCollection& lines) {
			m_scene.lines.push_back(lines);
			auto idx = lastOf(m_scene.lines);
			Graphics2DElement elem;
			elem.content = Content2D::Lines;
			elem.idx = idx;
			m_scene.layers[layer].graphics.push_back(elem);
		}

		void add(size_t layer, ColorFill fill) {
			m_scene.colorFills.push_back(fill);
			auto idx = lastOf(m_scene.colorFills);
			Graphics2DElement elem;
			elem.content = Content2D::Fill;
			elem.idx = idx;
			m_scene.layers[layer].graphics.push_back(elem);
		}

		Scene2D build() {
			return m_scene;
		}

	};
}
