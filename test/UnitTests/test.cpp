#include "pch.h"

#include <dr4/dr4_distance.h>
#include <dr4/dr4_floatingpoint.h>

TEST(DR4Test, TestDistance) {

	using namespace dr4;

	Pairf a = { 0.0f, -1.0f };
	Pairf b = { 0.0f, 1.0f };
	Line2D line = { a, b };

	Pairf point = {1.0f, 0.0f};
	float signedDist = Distance2DSigned(point, line);

	EXPECT_FLOAT_EQ(signedDist, 1.0f);

	Pairf point2 = {-1.0f, 0.0f};
	float signedDist2 = Distance2DSigned(point2, line);
	
	EXPECT_FLOAT_EQ(signedDist2, -1.0f);

	float unsignedDist = Distance2DUnsigned(point, line);

	EXPECT_FLOAT_EQ(unsignedDist, 1.0);

  //EXPECT_EQ(1, 1);
  //EXPECT_TRUE(true);
}