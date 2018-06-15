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

#include <maya/MTimerMessage.h>
#include <maya/MFnDependencyNode.h>

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
	return NULL;
}

bool ProximityLocatorDrawOverride::hasUIDrawables() const
{
	return true;
}

void ProximityLocatorDrawOverride::addUIDrawables(const MDagPath & objPath, MHWRender::MUIDrawManager & drawManager, const MHWRender::MFrameContext & frameContext, const MUserData * data)
{
	drawManager.beginDrawable();

	drawManager.sphere(MPoint(0, 0, 0), 1, true);

	drawManager.endDrawable();
}

// ProximityLocatorDrawOverride::onDrawTimerCallback
//
// This function gets called at fixed intervals by a timer callback.
// When called onDrawTimerCallback access the dummyOutput attribute
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