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

#include <Windows.h>
#include <qtcore/qpoint>
#include <qtcore/qmetatype.h>
#include <qtcore/qbytearray.h>
#include <qtgui/qcursor>
#include <qtwidgets/qwidget.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnDagNode.h>
#include <maya/MDagPath.h>
#include <maya/MGlobal.h>
#include <maya/MDagModifier.h>
#include <maya/MDGMessage.h>
#include <maya/MPoint.h>
#include <maya/MMatrix.h>

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

	// We use Qt to get the cursor position because maya  
	// can't give it to us if we are not in an MContext
	// Right now the tool works with global coordinates
	// ( e.g the mouse cursor show control in range even
	// if it is outside the viewport )
	QPoint QcursorPosition{ QCursor::pos() };
	MPoint cursorPosition{ double(QcursorPosition.x() / 1000), double(QcursorPosition.y() / 1000) };

	short proximityLocatorCoordinates[2];
	CHECK_MSTATUS(dagObjectToViewCoordinates(thisMObject(), proximityLocatorCoordinates[0], proximityLocatorCoordinates[1]));
	MPoint proximityLocatorViewCoordinates{double(proximityLocatorCoordinates[0]/1000), double(proximityLocatorCoordinates[1]/1000)};

	MVector cursorLocatorVector{ cursorPosition - proximityLocatorViewCoordinates };
	bool result = (cursorLocatorVector.length() <= 0.1);

	// We access dummyInput to clean the dg 
	bool dummyValue{ data.inputValue(dummyInput).asBool() };

	data.outputValue(isVisible).setBool(result);
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

MObject  ProximityLocator::transformFromShape(const MObject & shapeNode)
{
	MStatus status{};

	MFnDagNode shapeNodeFn{ shapeNode };
	MDagPath shapeNodePath{};
	CHECK_MSTATUS(shapeNodeFn.getPath(shapeNodePath));

	MObject transform{ shapeNodePath.transform(&status) };
	CHECK_MSTATUS(status);

	return transform;
}

MStatus ProximityLocator::dagObjectToViewCoordinates(const MObject & dagNode, short & x_pos, short & y_pos) const
{
	MStatus status{};

	// Find the world space position of the node
	MFnDagNode dagNodeFn{ dagNode, &status };
	CHECK_MSTATUS_AND_RETURN_IT(status);
	MTransformationMatrix dagNodeMatrix{ dagNodeFn.transformationMatrix(&status) };
	CHECK_MSTATUS_AND_RETURN_IT(status);
	MPoint dagNodePosition{ dagNodeMatrix.getTranslation(MSpace::kWorld, &status) };
	CHECK_MSTATUS_AND_RETURN_IT(status);

	M3dView activeView{ M3dView::active3dView() };
	activeView.worldToView(dagNodePosition, x_pos, y_pos, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MStatus::kSuccess;
}