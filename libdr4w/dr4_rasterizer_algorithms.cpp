
#include <dr4/dr4_rasterizer_algorithms.h>


#if 0
class Edge
{
public:
	Color Color1, Color2;
	int X1, Y1, X2, Y2;

	Edge(const Color& color1, int x1, int y1, const Color& color2, int x2, int y2);
};

class Span
{
public:
	Color Color1, Color2;
	int X1, X2;

	Span(const Color& color1, int x1, const Color& color2, int x2);
};
#endif

namespace dr4 {
	class Edge
	{
	public:
		SRGBA Color;
		int X1, Y1, X2, Y2;

		Edge(const SRGBA& color, int x1, int y1, int x2, int y2);
	};

	class Span
	{
	public:
		SRGBA Color;
		int X1, X2;

		Span(const SRGBA& color1, int x1, int x2);
	};
}

dr4::Edge::Edge(const SRGBA& color1, int x1, int y1, int x2, int y2)
{
	Color = color1;
	if (y1 < y2) {
		X1 = x1;
		Y1 = y1;
		X2 = x2;
		Y2 = y2;
	}
	else {
		X1 = x2;
		Y1 = y2;
		X2 = x1;
		Y2 = y1;
	}
}

dr4::Span::Span(const SRGBA& color1, int x1, int x2)
{
	Color = color1;
	if (x1 < x2) {
		X1 = x1;
		X2 = x2;
	}
	else {
		X1 = x2;
		X2 = x1;
	}
}

namespace {
	// perpdot
	static inline float Pd(float x0, float y0, float x1, float y1) {
		return x0 * y1 - y0 * x1;
	}

	void DrawSpan(dr4::Painter& p, const dr4::Span& span, int y)
	{
		int xdiff = span.X2 - span.X1;
		if (xdiff == 0)
			return;

		float factor = 0.0f;
		float factorStep = 1.0f / (float)xdiff;

		// draw each pixel in the span
		for (int x = span.X1; x < span.X2; x++) {
			p.SetPixeli(x, y, span.Color);
			factor += factorStep;
		}
	}

	void DrawSpansBetweenEdges(dr4::Painter& p, const dr4::Edge& e1, const dr4::Edge& e2)
	{
		// calculate difference between the y coordinates
		// of the first edge and return if 0
		float e1ydiff = (float)(e1.Y2 - e1.Y1);
		if (e1ydiff == 0.0f)
			return;

		// calculate difference between the y coordinates
		// of the second edge and return if 0
		float e2ydiff = (float)(e2.Y2 - e2.Y1);
		if (e2ydiff == 0.0f)
			return;

		// calculate differences between the x coordinates
		// and colors of the points of the edges
		float e1xdiff = (float)(e1.X2 - e1.X1);
		float e2xdiff = (float)(e2.X2 - e2.X1);

		// calculate factors to use for interpolation
		// with the edges and the step values to increase
		// them by after drawing each span
		float factor1 = (float)(e2.Y1 - e1.Y1) / e1ydiff;
		float factorStep1 = 1.0f / e1ydiff;
		float factor2 = 0.0f;
		float factorStep2 = 1.0f / e2ydiff;

		// loop through the lines between the edges and draw spans
		for (int y = e2.Y1; y < e2.Y2; y++) {
			// create and draw span
			dr4::Span span(e1.Color,
				e1.X1 + (int)(e1xdiff * factor1),
				e2.X1 + (int)(e2xdiff * factor2));
			DrawSpan(p, span, y);

			// increase factors
			factor1 += factorStep1;
			factor2 += factorStep2;
		}
	}
}

void
dr4::Razz::DrawTriangle(dr4::Painter& p, const SRGBA& color1, float x1, float y1,
	float x2, float y2,
	float x3, float y3)
{
	// create edges for the triangle
	Edge edges[3] = {
		Edge(color1, (int)x1, (int)y1, (int)x2, (int)y2),
		Edge(color1, (int)x2, (int)y2, (int)x3, (int)y3),
		Edge(color1, (int)x3, (int)y3, (int)x1, (int)y1)
	};

	int maxLength = 0;
	int longEdge = 0;

	// find edge with the greatest length in the y axis
	for (int i = 0; i < 3; i++) {
		int length = edges[i].Y2 - edges[i].Y1;
		if (length > maxLength) {
			maxLength = length;
			longEdge = i;
		}
	}

	int shortEdge1 = (longEdge + 1) % 3;
	int shortEdge2 = (longEdge + 2) % 3;

	// draw spans between edges; the long edge can be drawn
	// with the shorter edges to draw the full triangle
	DrawSpansBetweenEdges(p, edges[longEdge], edges[shortEdge1]);
	DrawSpansBetweenEdges(p, edges[longEdge], edges[shortEdge2]);
}

void
dr4::Razz::DrawTriangle2(dr4::Painter& ptr, const SRGBA& color1, float x1, float y1,
	float x2, float y2,
	float x3, float y3)
{
	// TODO CLIP BOUNDS
	int minx = (int)std::min(std::min(x1, x2), x3);
	int miny = (int)std::min(std::min(y1, y2), y3);
	int maxx = (int)std::max(std::max(x1, x2), x3);
	int maxy = (int)std::max(std::max(y1, y2), y3);

	float ex1 = x2 - x1;
	float ey1 = y2 - y1;
	
	float ex2 = x3 - x2;
	float ey2 = y3 - y2;
	
	float ex3 = x1 - x3;
	float ey3 = y1 - y3;

	for (int y = miny; y <= maxy; y++) {
		for (int x = minx; x <= maxx; x++){
			float p1 = Pd(ex1, ey1, x - x1, y - y1);
			float p2 = Pd(ex2, ey2, x - x2, y - y2);
			float p3 = Pd(ex3, ey3, x - x3, y - y3);
			if (p1 >= 0.f && p2 >= 0.f && p3 >= 0.f)
				ptr.SetPixeli(x, y, color1);
		}
	}
}

void
dr4::Razz::DrawTriangle3(dr4::Painter& ptr, const SRGBA& color1, float x1, float y1,
	float x2, float y2,
	float x3, float y3)
{
	// TODO CLIP BOUNDS
	int minx = (int)std::min(std::min(x1, x2), x3);
	int miny = (int)std::min(std::min(y1, y2), y3);
	int maxx = (int)std::max(std::max(x1, x2), x3);
	int maxy = (int)std::max(std::max(y1, y2), y3);

	float ex1 = x2 - x1;
	float ey1 = y2 - y1;
	
	float ex2 = x3 - x2;
	float ey2 = y3 - y2;
	
	float ex3 = x1 - x3;
	float ey3 = y1 - y3;

	for (int y = miny; y <= maxy; y++) {
		for (int x = minx; x <= maxx; x++){
			float p1 = Pd(ex1, ey1, x - x1, y - y1);
			float p2 = Pd(ex2, ey2, x - x2, y - y2);
			float p3 = Pd(ex3, ey3, x - x3, y - y3);
			if (p1 >= 0.f && p2 >= 0.f && p3 >= 0.f)
				ptr.SetPixeli(x, y, color1);
		}
	}
}


void dr4::Razz::DrawLine(Painter& ptr, SRGBA color, float x1, float y1, float x2, float y2) {
	float xdiff = (x2 - x1);
	float ydiff = (y2 - y1);

	if (xdiff == 0.0f && ydiff == 0.0f) {
		ptr.SetPixel(x1, y1, color);
		return;
	}

	if (fabs(xdiff) > fabs(ydiff)) {
		float xmin, xmax;

		// set xmin to the lower x value given
		// and xmax to the higher value
		if (x1 < x2) {
			xmin = x1;
			xmax = x2;
		}
		else {
			xmin = x2;
			xmax = x1;
		}

		// draw line in terms of y slope
		float slope = ydiff / xdiff;
		for (float x = xmin; x <= xmax; x += 1.0f) {
			float y = y1 + ((x - x1) * slope);
			//Color color = color1 + ((color2 - color1) * ((x - x1) / xdiff));
			ptr.SetPixel(x, y, color);
		}
	}
	else {
		float ymin, ymax;

		// set ymin to the lower y value given
		// and ymax to the higher value
		if (y1 < y2) {
			ymin = y1;
			ymax = y2;
		}
		else {
			ymin = y2;
			ymax = y1;
		}

		// draw line in terms of x slope
		float slope = xdiff / ydiff;
		for (float y = ymin; y <= ymax; y += 1.0f) {
			float x = x1 + ((y - y1) * slope);
			//Color color = color1 + ((color2 - color1) * ((y - y1) / ydiff));
			ptr.SetPixel(x, y, color);
		}
	}

}
