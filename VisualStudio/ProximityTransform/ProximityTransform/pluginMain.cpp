// Copyright 2018 Luca Di Sera
//		Contact: bloodtype.sigma@gmail.com
//				 https://github.com/diseraluca
//
// This code is licensed under the MIT License. 
// More informations can be found in the LICENSE file in the root folder of this repository
//
//
// File : pluginMain.cpp
//
// The ProximityTransform plugin adds to Maya a new locator node and a corresponding
// draw override for viewport 2.0 support

#include "ProximityLocator.h"
#include "ProximityLocatorDrawOverride.h"

#include <maya/MFnPlugin.h>
#include <maya/MDrawRegistry.h>

MStatus initializePlugin(MObject obj) {
	MStatus status{};
	MFnPlugin plugin{ obj, "Luca Di Sera", "1.0", "Any" };

	status = plugin.registerNode("ProximityLocator", ProximityLocator::id, ProximityLocator::creator, ProximityLocator::initialize, MPxNode::kLocatorNode, &ProximityLocator::drawDbClassification);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(ProximityLocator::drawDbClassification, ProximityLocator::drawRegistrantId, ProximityLocatorDrawOverride::creator);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MStatus::kSuccess;
}

MStatus uninitializePlugin(MObject obj) {
	MStatus status{};
	MFnPlugin plugin{ obj };

	status = MHWRender::MDrawRegistry::deregisterDrawOverrideCreator(ProximityLocator::drawDbClassification, ProximityLocator::drawRegistrantId);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.deregisterNode(ProximityLocator::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MStatus::kSuccess;
}