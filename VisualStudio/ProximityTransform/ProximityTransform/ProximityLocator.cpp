// Copyright 2018 Luca Di Sera
//		Contact: bloodtype.sigma@gmail.com
//				 https://github.com/diseraluca
//
// This code is licensed under the MIT License. 
// More informations can be found in the LICENSE file in the root folder of this repository
//
//
// File : ProximityLocator.cpp

#include "ProximityLocator.h"

#include <maya/MFnNumericAttribute.h>
#include <maya/MFnDagNode.h>
#include <maya/MDagPath.h>
#include <maya/MGlobal.h>
#include <maya/MDagModifier.h>
#include <maya/MDGMessage.h>

MTypeId ProximityLocator::id{ 0x0012d3005 };

MObject ProximityLocator::dummyOutput;
MObject ProximityLocator::isVisible;

void * ProximityLocator::creator()
{
	return new ProximityLocator;
}

MStatus ProximityLocator::initialize()
{
	MStatus status{};

	MFnNumericAttribute nAttr;

	dummyOutput = nAttr.create("dummyOutput", "dop", MFnNumericData::kBoolean, true, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setWritable(false));
	CHECK_MSTATUS(addAttribute(dummyOutput));
	
	isVisible = nAttr.create("isVisible", "isv", MFnNumericData::kBoolean, true, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setWritable(false));
	CHECK_MSTATUS(addAttribute(isVisible));

	return MStatus::kSuccess;
}

void ProximityLocator::postConstructor()
{


	MStatus status{};
	MFnDagNode transformFn{ transformFromShape(thisMObject()) };


	MPlug proximityLocatorIsVisiblePlug{ thisMObject(), isVisible };
	if (proximityLocatorIsVisiblePlug.isNull()) {
		MGlobal::displayError("Couldn't find a plug to isVisible");
		return;
	}

	MPlug transformVisibilityPlug{ transformFn.findPlug("visibility", &status) };
	CHECK_MSTATUS(status);

	MDagModifier dagModifier{};
	dagModifier.connect(proximityLocatorIsVisiblePlug, transformVisibilityPlug);
	dagModifier.doIt();
}

MStatus ProximityLocator::compute(const MPlug & plug, MDataBlock & data)
{
	return MStatus();
}

ProximityLocator::~ProximityLocator()
{
}

void ProximityLocator::onNodeAdded(MObject & node, void * clientData)
{
}

MObject & ProximityLocator::transformFromShape(const MObject & shapeNode)
{
	MStatus status{};

	MFnDagNode shapeNodeFn{ shapeNode };
	MDagPath shapeNodePath{};
	CHECK_MSTATUS(shapeNodeFn.getPath(shapeNodePath));

	MObject transform{ shapeNodePath.transform(&status) };
	CHECK_MSTATUS(status);

	return transform;
}
