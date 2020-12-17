#include <dr4/dr4_rasterizer.h>

namespace dr4 {




	class Rasterizer_vA : public IRasterizer {
	public:
		unsigned m_width;
		unsigned m_height;

		Rasterizer_vA(unsigned width, unsigned height):m_width(width), m_height(height) {
		}

		//
		// IRasterizer		
		//

		virtual ~Rasterizer_vA(){}

		virtual void draw(Camera2D camera, const Scene2D& scene) override {

		}
		virtual ImageRGBA8SRGB getColorAsSRGB() const override {
			ImageRGBA8SRGB res(m_width, m_height);
			return res;
		}


	};
}

std::shared_ptr<dr4::IRasterizer> dr4::CreateRasterizer(unsigned width, unsigned height){
	return std::make_shared<Rasterizer_vA>(width, height);
}
