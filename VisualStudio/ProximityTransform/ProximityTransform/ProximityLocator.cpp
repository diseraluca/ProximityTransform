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

MString ProximityLocator::drawDbClassification{ "drawdb/geometry/ProximityLocator" };
MString ProximityLocator::drawRegistrantId{ "ProximityLocatorPlugin" };

MObject ProximityLocator::dummyInput;
MObject ProximityLocator::isVisible;

void * ProximityLocator::creator()
{
	return new ProximityLocator;
}

MStatus ProximityLocator::initialize()
{
	MStatus status{};

	MFnNumericAttribute nAttr;

	dummyInput = nAttr.create("dummyInput", "dip", MFnNumericData::kBoolean, true, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	CHECK_MSTATUS(nAttr.setWritable(false));
	CHECK_MSTATUS(addAttribute(dummyInput));
	
	isVisible = nAttr.create("isVisible", "isv", MFnNumericData::kBoolean, true, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setWritable(false));
	CHECK_MSTATUS(addAttribute(isVisible));

	attributeAffects(dummyInput, isVisible);

	return MStatus::kSuccess;
}

void ProximityLocator::postConstructor()
{
	MFnDagNode proximityLocatorNodeFn{ thisMObject() };
	proximityLocatorNodeFn.setName("ProximityLocator#");

	nodeAddedCbId = MDGMessage::addNodeAddedCallback(ProximityLocator::onNodeAdded, "ProximityLocator");
	if (nodeAddedCbId == NULL) {
		MGlobal::displayError("Couldn't create the nodeAdded callback");
	}
}

MStatus ProximityLocator::compute(const MPlug & plug, MDataBlock & data)
{
	// As of now the compute method is a proxy that
	// inverts isVisible and nothing more.
	//TODO-DO Add the real computation

	if (plug != isVisible ) {
		return MStatus::kUnknownParameter;
	}
	
	bool dummyValue{ data.inputValue(dummyInput).asBool() };
	bool isVisbleValue{ data.outputValue(isVisible).asBool() };

	data.outputValue(isVisible).setBool(!isVisbleValue);
	data.outputValue(isVisible).setClean();

	return MStatus::kSuccess;
}

ProximityLocator::~ProximityLocator()
{
	if (nodeAddedCbId != NULL) {
		CHECK_MSTATUS(MDGMessage::removeCallback(nodeAddedCbId));
	}
}

// ProximityLocator::onNodeAdded
//
// This function gets called by a callback when a node of ProximityLocator
// type is created.
// When called onNodeAdded connects the "isVisible" output attribute
// of the ProximityLocator instance to the "visibility" attribute of
// the corresponding transform.
void ProximityLocator::onNodeAdded(MObject & node, void * clientData)
{
	MStatus status{};
	MFnDagNode transformFn{ ProximityLocator::transformFromShape(node) };


	MPlug proximityLocatorIsVisiblePlug{ node, isVisible };
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
