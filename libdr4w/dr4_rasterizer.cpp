#include <dr4/dr4_rasterizer.h>

namespace dr4 {

	class RenderTile{
		unsigned sourcewidth;
		unsigned sourceheight;
	public:
		unsigned clipxstart;
		unsigned clipystart;
		unsigned clipwidth;
		unsigned clipheight;

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

		void apply(const RenderBuffer& src, RenderTile area) {
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
			Camera2D m_camera;
			//std::shared_ptr<Scene2D> m_scene;
			const Scene2D& m_scene; // do not modify, only read
			RenderBuffer m_buffer;
			RenderTile m_tile;
			DrawTask2D(
				Camera2D camera,
				//std::shared_ptr<Scene2D> scene,
				const Scene2D& scene,
				RenderTile tile
				) :m_camera(camera), m_scene(scene), m_tile(tile), m_buffer(m_tile.clipwidth, m_tile.clipheight) {}

			virtual ~DrawTask2D(){}
			virtual void doTask() override {
				// render scene
			}
		};

		//
		// IRasterizer		
		//

		virtual ~Rasterizer_vA(){}

		virtual void draw2D(Camera2D camera, const Scene2D& scene, FrameTasks& tasks) override {
			// Split to as many subparts as wanted, then draw
			std::shared_ptr<ITask> ptr(new DrawTask2D(camera, scene, RenderTile::Full(m_buffer.width, m_buffer.height)));
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
