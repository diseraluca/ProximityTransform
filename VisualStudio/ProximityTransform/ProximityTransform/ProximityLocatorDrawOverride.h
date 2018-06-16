// Copyright 2018 Luca Di Sera
//		Contact: bloodtype.sigma@gmail.com
//				 https://github.com/diseraluca
//
// This code is licensed under the MIT License. 
// More informations can be found in the LICENSE file in the root folder of this repository
//
//
// File : ProximityLocatorDrawOverride.h
//
// The ProximityLocatorDrawOverride class defines a draw override
// to support the ProximityLocator class.
// A TimerCallback is used to dirty the output of the ProximityLocator
// to have the node compute and be drawn at fixed intervals to support
// a (mostly) real time hide-show beahviour.
//
//Notes:
//ProximityLocatorDrawOverrides provides support for viewPort 2.0 only

#pragma once

#include "ProximityLocator.h"

#include <maya/MPxDrawOverride.h>

class ProximityLocatorDrawOverride : public MHWRender::MPxDrawOverride {
public:
	ProximityLocatorDrawOverride(const MObject& obj);
	~ProximityLocatorDrawOverride();

	static MHWRender::MPxDrawOverride* creator(const MObject& obj);
	virtual MHWRender::DrawAPI supportedDrawAPIs() const override;
	virtual MUserData* prepareForDraw(const MDagPath& objPath, const MDagPath& cameraPath, const MHWRender::MFrameContext& frameContext, MUserData* oldData) override;
	virtual bool hasUIDrawables() const override;
	virtual void addUIDrawables(const MDagPath& objPath, MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext, const MUserData* data) override;

private:
	static void onDrawTimerCallback(float elapsedTime, float lastTime, void *clientData);

	static MColor colorFromPlug(const MPlug& colorPlug);

private:
	ProximityLocator * proximityLocator;
	MCallbackId drawTimerCbId;
};