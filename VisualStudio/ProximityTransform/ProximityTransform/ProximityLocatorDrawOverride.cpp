// Copyright 2018 Luca Di Sera
//		Contact: bloodtype.sigma@gmail.com
//				 https://github.com/diseraluca
//
// This code is licensed under the MIT License. 
// More informations can be found in the LICENSE file in the root folder of this repository
//
//
// File : ProximityLocatorDrawOverride.cpp

#include "ProximityLocatorDrawOverride.h"
#include "ProximityLocatorData.h"

#include <maya/MTimerMessage.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnDagNode.h>
#include <maya/MGlobal.h>

ProximityLocatorDrawOverride::ProximityLocatorDrawOverride(const MObject & obj)
	:MHWRender::MPxDrawOverride(obj, NULL, false)
{
	MStatus status{};

	drawTimerCbId = MTimerMessage::addTimerCallback(0.5f, ProximityLocatorDrawOverride::onDrawTimerCallback, this, &status);
	CHECK_MSTATUS(status);

	MFnDependencyNode proximityLocatorNode{ obj, &status };
	proximityLocator = status ? dynamic_cast<ProximityLocator*>(proximityLocatorNode.userNode()) : NULL;
}

ProximityLocatorDrawOverride::~ProximityLocatorDrawOverride()
{
	proximityLocator = NULL;

	if (drawTimerCbId != 0) {
		MTimerMessage::removeCallback(drawTimerCbId);
		drawTimerCbId = 0;
	}
}

MHWRender::MPxDrawOverride * ProximityLocatorDrawOverride::creator(const MObject & obj)
{
	return new ProximityLocatorDrawOverride(obj);
}

MHWRender::DrawAPI ProximityLocatorDrawOverride::supportedDrawAPIs() const
{
	return (MHWRender::kOpenGL | MHWRender::kOpenGLCoreProfile | MHWRender::kDirectX11);
}

MUserData * ProximityLocatorDrawOverride::prepareForDraw(const MDagPath & objPath, const MDagPath & cameraPath, const MHWRender::MFrameContext & frameContext, MUserData * oldData)
{
	ProximityLocatorData* data = dynamic_cast<ProximityLocatorData*>(oldData);
	if (!data) {
		data = new ProximityLocatorData;
	}

	MStatus status{};
	MObject proximityLocatorNode{ objPath.node(&status) };
	if (status) {
		MPlug proximityLocatorProximityRadiusPlug{ proximityLocatorNode, ProximityLocator::proximityRadius };
		data->radius = proximityLocatorProximityRadiusPlug.asDouble();
	}
	else {
		data->radius = 1.0;
	}

	MFnDagNode proximityLocatorTransformFn{ ProximityLocator::transformFromShape(proximityLocatorNode), &status };
	if (status) {
		MTransformationMatrix proximityLocatorTransformMatrix{ proximityLocatorTransformFn.transformationMatrix() };
		MPoint worldPosition{ proximityLocatorTransformMatrix.getTranslation(MSpace::kWorld, &status) };
		CHECK_MSTATUS(status);

		M3dView activeView{ M3dView::active3dView(&status) };
		CHECK_MSTATUS(status);
		activeView.worldToView(worldPosition, data->position.x, data->position.y, &status);
		CHECK_MSTATUS(status);
	}
	else {
		data->position = D2Point();
	}

	MPlug proximityLocatorDrawColorPlug{ proximityLocatorNode, ProximityLocator::drawColor };
	data->drawColor = colorFromPlug(proximityLocatorDrawColorPlug);

	return data;
}

bool ProximityLocatorDrawOverride::hasUIDrawables() const
{
	return true;
}

void ProximityLocatorDrawOverride::addUIDrawables(const MDagPath & objPath, MHWRender::MUIDrawManager & drawManager, const MHWRender::MFrameContext & frameContext, const MUserData * data)
{
	ProximityLocatorData* proximityLocatorData = (ProximityLocatorData*)(data);
	if (!proximityLocatorData) {
		return;
	}

	drawManager.beginDrawInXray();
	drawManager.setColor(proximityLocatorData->drawColor);
	drawManager.circle2d(proximityLocatorData->position, proximityLocatorData->radius, false);
	drawManager.endDrawInXray();
}

// ProximityLocatorDrawOverride::onDrawTimerCallback
//
// This function gets called at fixed intervals by a timer callback.
// When called onDrawTimerCallback access the dummyInput attribute
// to force the ProximityLocator node instance to compute its isVisible output.
// Then it dirties the DrawGeometry to launch the draw.
void ProximityLocatorDrawOverride::onDrawTimerCallback(float elapsedTime, float lastTime, void * clientData)
{
	ProximityLocatorDrawOverride* proximityLocatorDrawOverride = static_cast<ProximityLocatorDrawOverride*>(clientData);
	if (proximityLocatorDrawOverride && proximityLocatorDrawOverride->proximityLocator) {
		MPlug proximityLocatorDummyInputPlug{ proximityLocatorDrawOverride->proximityLocator->thisMObject(), ProximityLocator::dummyInput };
		bool dummyInputValue = proximityLocatorDummyInputPlug.asBool();
		proximityLocatorDummyInputPlug.setBool(!dummyInputValue);

		MHWRender::MRenderer::setGeometryDrawDirty(proximityLocatorDrawOverride->proximityLocator->thisMObject());
	}
}

//ProximityLocatorDrawOverride::colorFromPlug
//
// Extracts an MColor for the given plug. If the plug is not a suitable color attribute
// returns a default MColor
MColor ProximityLocatorDrawOverride::colorFromPlug(const MPlug & colorPlug)
{
	if (!colorPlug.isCompound() || colorPlug.numChildren() != 3) {
		MGlobal::displayWarning("The given plug is not a valid color plug");
		return MColor{};
	}

	MColor resultColor{};
	resultColor.r = colorPlug.child(0).asDouble();
	resultColor.g = colorPlug.child(1).asDouble();
	resultColor.b = colorPlug.child(2).asDouble();

	return resultColor;
}
