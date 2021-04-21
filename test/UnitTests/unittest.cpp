#include "pch.h"

#include <dr4/dr4_distance.h>
#include <dr4/dr4_floatingpoint.h>

#include <dr4/dr4_handlemanager.h>

#include <string>

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

TEST(DR4Test, TestLineDistanceDistance) {

	using namespace dr4;

	Pairf a = { 0.0f, -1.0f };
	Pairf b = { 0.0f, 1.0f };
	Line2D lineab = { a, b };
	LineDistance2D line(lineab);

	Pairf point = {1.0f, 0.0f};
	float signedDist = line.signedDistance(point);
	EXPECT_FLOAT_EQ(signedDist, 1.0f);

	Pairf point2 = {-1.0f, 0.0f};
	float signedDist2 = line.signedDistance(point2);
	EXPECT_FLOAT_EQ(signedDist2, -1.0f);

	float unsignedDist = line.unsignedDistance(point);
	EXPECT_FLOAT_EQ(unsignedDist, 1.0);
	
	Pairf obliquepoint = {1.0f, 2.0f};
	float obliqueSignedDist = line.signedDistance(obliquepoint);
	EXPECT_GE(obliqueSignedDist, 1.0);
	
	Pairf obliquepoint2 = {-1.0f, 2.0f};
	float obliqueSignedDist2 = line.signedDistance(obliquepoint2);
	EXPECT_LE(obliqueSignedDist2, -1.0);

	float topFarpointDist = line.signedDistance({0.f, 3.000f});
	EXPECT_FLOAT_EQ(topFarpointDist , -2.0f);
	
	float bottomFarpointDist = line.signedDistance({0.f, -3.000f});
	EXPECT_FLOAT_EQ(bottomFarpointDist , -2.0f);

	Pairf ao = { 1.0f, -1.0f };
	Pairf bo = { -1.0f, 1.0f };
	Line2D lineObliqueab = { ao, bo };
	LineDistance2D lineOblique(lineObliqueab);

	float distOut = lineOblique.signedDistance({ 0.f, 1.0f });
	float sqrt2inv = 1.f / sqrtf(2.f);
	EXPECT_FLOAT_EQ(distOut, sqrt2inv);
	
	float distOutFar = lineOblique.signedDistance({ -2.f, 2.0f });
	EXPECT_FLOAT_EQ(distOutFar, -sqrtf(2.0f));
}

TEST(DR4Test, TestHandles) {

	using namespace dr4;
	using namespace std;
	enum class localtypes_t : int {type_string = 1};
	const int strtype = (int)localtypes_t::type_string;
	//HandleBuffer<string, static_cast<int>(localtype_t::type_string)> stringBuffer(256);
	HandleBuffer<string> stringBuffer(4);

	auto h = stringBuffer.create("Hello");
	map<string, decltype(h)> ref;
	ref["Hello"] = h;

	auto mkword = [&](const string& wrd) {
		auto hndl = stringBuffer.create(wrd);
		ref[wrd] = hndl;
	};

	mkword("a");
	mkword("b");
	mkword("c");
	mkword("d");
	mkword("foo foo foo");
	mkword("Asinine rabbit");
	mkword("fancy bear");
	
	stringBuffer.release(ref["c"]);
	ref.erase("c");
	
	mkword("hello");

	for (auto r : ref) {
		auto s = stringBuffer.get(r.second);
		if(s)
			ASSERT_EQ(*s, r.first);
	}

}