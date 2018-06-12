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

void * ProximityLocator::creator()
{
	return new ProximityLocator;
}

MStatus ProximityLocator::initialize()
{
	return MStatus();
}

MStatus ProximityLocator::compute(const MPlug & plug, MDataBlock & data)
{
	return MStatus();
}
