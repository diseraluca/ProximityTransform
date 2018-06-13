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
// The output attribute "isVisible" can be connected to a visibility attribute to
// hide-show a node and is automaticallt��y connected to the visibility of the parent
// transform at creation
//
//Notes:
//ProximityLocator provides support for viewPort 2.0 only

#pragma once

#include <maya/MPxLocatorNode.h>
#include <maya/MCallbackIdArray.h>

class ProximityLocator : public MPxLocatorNode {
public:
	static void* creator();
	static MStatus initialize();
	virtual void postConstructor() override;
	virtual MStatus compute(const MPlug& plug, MDataBlock& data) override;

	~ProximityLocator();

private:
	void onNodeAdded(MObject& node, void* clientData);
	MObject & transformFromShape(const MObject& shapeNode);

public:
	static MTypeId id;

	static MObject dummyOutput;
	static MObject isVisible;

private:
	MCallbackId nodeAddedCbId; // Id of the callback that will connect the output to the transform visibility
};