#include "pch.h"

#include <dr4/dr4_distance.h>
#include <dr4/dr4_floatingpoint.h>

TEST(DR4Test, TestDistanceLine) {

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
	
	Pairf obliquepoint = {1.0f, 2.0f};
	float obliqueSignedDist = Distance2DSigned(obliquepoint, line);
	EXPECT_GE(obliqueSignedDist, 1.0);
	
	Pairf obliquepoint2 = {-1.0f, 2.0f};
	float obliqueSignedDist2 = Distance2DSigned(obliquepoint2, line);
	EXPECT_LE(obliqueSignedDist2, -1.0);

	Pairf ao = { 1.0f, -1.0f };
	Pairf bo = { -1.0f, 1.0f };
	Line2D lineOblique = { ao, bo };
	Pairf pointOut = { 0.f, 1.0f };
	float distOut = Distance2DSigned(pointOut, lineOblique);
	float sqrt2inv = 1.f / sqrtf(2.f);
	EXPECT_FLOAT_EQ(distOut, sqrt2inv);

}