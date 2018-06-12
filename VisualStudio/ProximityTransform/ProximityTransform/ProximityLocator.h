// Copyright 2018 Luca Di Sera
//		Contact: bloodtype.sigma@gmail.com
//				 https://github.com/diseraluca
//
// This code is licensed under the MIT License. 
// More informations can be found in the LICENSE file in the root folder of this repository
//
//
// File : ProximityLocator.h
//
// The ProximityLocator class defines a custom Maya Locator Node
// that hides or shows its transform depending on the mouse cursor
// position.

#pragma once

#include <maya/MPxLocatorNode.h>

class ProximityLocator : public MPxLocatorNode {
public:
	static void* creator();
	static MStatus initialize();
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
};