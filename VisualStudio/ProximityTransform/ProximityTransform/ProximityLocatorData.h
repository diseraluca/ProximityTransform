#pragma once

#include <maya/MUserData.h>

// A commodity struct to represent a 2d point
// in view space
struct D2Point{
public:
	D2Point() :x{0}, y{0} {}
	D2Point(short x, short y) :x{ x }, y{ y } {}

	operator MPoint() const { return MPoint(x, y); }

public:
	short x;
	short y;
};

class ProximityLocatorData : public MUserData {
public:
	ProximityLocatorData() :MUserData(false) {}
	
public:
	MPoint worldPosition;
	D2Point position;
	double radius;
};