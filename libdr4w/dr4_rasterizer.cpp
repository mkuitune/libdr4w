#include <dr4/dr4_rasterizer.h>
#include <dr4/dr4_rasterizer_algorithms.h>

namespace dr4 {

	struct Range2D {
		unsigned x0,y0, xmax, ymax;

		size_t rowlength() const { return xmax - x0; }
	};

	class RenderTile{
		unsigned sourcewidth; // dimensions of the result buffer, not the render tile
		unsigned sourceheight;
	public:
		unsigned clipxstart; // render tile positioning into the result buffer
		unsigned clipystart;
		unsigned clipwidth;
		unsigned clipheight;

		Range2D getRange() {
			unsigned clipxstop = clipxstart + clipwidth;
			unsigned clipystop = clipystart + clipheight;
			return { clipxstart, clipystart, clipxstop, clipystop};
		}

		static RenderTile Full(unsigned width, unsigned height) {
			RenderTile tile;
			tile.sourcewidth = width;
			tile.sourceheight = height;
			tile.clipxstart = 0;
			tile.clipystart = 0;
			tile.clipwidth = tile.sourcewidth;
			tile.clipheight= tile.sourcewidth;
			return tile;
		}
	};

	struct RenderBuffer {
		ImageRGBA32Linear color;
		unsigned width;
		unsigned height;

		RenderBuffer(unsigned w, unsigned h):width(w), height(h), color(width, height) {
		}

		void apply(const RenderBuffer& src, RenderTile area /* target space, not source*/) {
			// copy result - maybe want to blend but do that later
			// RenderTile should be created based on buffer dimensions so shoudl fit already
			auto range = area.getRange();
			unsigned srcY = 0;
			for (unsigned y = range.y0; y < range.ymax; y++, srcY++){
				color.copyRowFrom(src.color, { range.x0, y }, { 0, srcY }, range.rowlength());
			}
		}

		ImageRGBA8SRGB getColorAsSRGBA() const {
			return convertRBGA32LinearToSrgb(color);
		}
	};


	class Rasterizer_vA : public IRasterizer {
	public:
		unsigned m_width;
		unsigned m_height;

		RenderBuffer m_buffer;

		Rasterizer_vA(unsigned width, unsigned height):
			m_width(width), m_height(height), m_buffer(width, height) {
		}

		//
		// Render tasks
		//

		class DrawTask2D : public ITask {
		public:
			RasterConfig2D m_config;
			//std::shared_ptr<Scene2D> m_scene;
			const Scene2D& m_scene; // do not modify, only read
			RenderBuffer m_buffer;
			RenderTile m_tile;
			Painter m_painter;

			DrawTask2D(
				RasterConfig2D config,
				//std::shared_ptr<Scene2D> scene,
				const Scene2D& scene,
				RenderTile tile) 
				:m_config(config), m_scene(scene), m_tile(tile),
					m_buffer(m_tile.clipwidth, m_tile.clipheight),m_painter(m_buffer.color) {}

			virtual ~DrawTask2D(){}

			void drawGraphicsLinesOnlyDBG(const Graphics2DElement& g, Blend blend) {
				// todo maybe have a separate drawtask class for debug output
				auto sceneToRaster = m_config.sceneToRaster();
				if (g.content == Content2D::Fill) {
					auto fill = m_scene.m_colorFills[g.idx];
					m_painter.fill(fill.colorFill);
				}
				else if (g.content == Content2D::Lines) {
					auto lines = m_scene.m_lines[g.idx];
					auto material = m_scene.m_materials[lines.material];
					for (auto line : lines.lines) {
						// line coords in scene coordsystem. Rasterize in tile pixel coordinates
						// transform coords (from->to) scene(s) -> framebuffer(p) ->tile(p)
						auto rFst = sceneToRaster.map(line.fst);
						auto rSnd = sceneToRaster.map(line.snd);
						Razz::DrawLine(m_painter, material.colorLine, rFst.x, rFst.y, rSnd.x, rSnd.y);
					}
				}
			}

			virtual void doTask() override {
				// render scene
				// render layers front to back
				for (auto layer : m_scene.m_layers) {
					for (auto g : layer.graphics) {
						//drawGraphics(g, layer.blend);
						drawGraphicsLinesOnlyDBG(g, layer.blend);
					}
				}
			}
		};

		//
		// IRasterizer		
		//

		virtual ~Rasterizer_vA(){}

		virtual void draw2D(RasterConfig2D config, const Scene2D& scene, FrameTasks& tasks) override {
			// Split to as many subparts as wanted, then draw
			std::shared_ptr<ITask> ptr(new DrawTask2D(config, scene, RenderTile::Full(m_buffer.width, m_buffer.height)));
			tasks.tasks.push_back(ptr);
		}
		
		virtual void applyResult(FrameTasks& tasks) {
			for (auto& t : tasks.tasks) {
				DrawTask2D* taskIn = dynamic_cast<DrawTask2D*>(t.get());
				// Apply buffer to buffer
				m_buffer.apply(taskIn->m_buffer, taskIn->m_tile);
			}
		}

		virtual ImageRGBA8SRGB getColorAsSRGB() const override {
			return m_buffer.getColorAsSRGBA();
		}

	};
}

std::shared_ptr<dr4::IRasterizer> dr4::CreateRasterizer(unsigned width, unsigned height){
	return std::make_shared<Rasterizer_vA>(width, height);
}
