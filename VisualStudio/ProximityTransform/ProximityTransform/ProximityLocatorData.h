// Copyright 2018 Luca Di Sera
//		Contact: bloodtype.sigma@gmail.com
//				 https://github.com/diseraluca
//
// This code is licensed under the MIT License. 
// More informations can be found in the LICENSE file in the root folder of this repository
//
//
// File : ProximityLocatorData.h

#pragma once

#include <maya/MUserData.h>

// A commodity struct to represent a 2d point
// in view space that is able to convert to an MPoint
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
	D2Point position;
	double radius;
};