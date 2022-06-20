#pragma once

#ifndef STRUCT_PLOTINFO
#define STRUCT_PLOTINFO

#include <string>

#include "CvEnums.h"

struct plotInfo
{
	plotInfo();
	std::string ToJSON();

	int index;
	bool worked;
	bool owned;
	bool bonusImproved;
	int value;
	int newValue;
	short yields[NUM_YIELD_TYPES];
	short newYields[NUM_YIELD_TYPES];
	BonusTypes bonus;
	ImprovementTypes currentImprovement;
	ImprovementTypes newImprovement;
	FeatureTypes currentFeature;
	FeatureTypes newFeature;
	BuildTypes currentBuild;
	BuildTypes newBuild;
};

#endif