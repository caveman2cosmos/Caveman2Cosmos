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
	int yieldValue;
	short yields[NUM_YIELD_TYPES];
	BonusTypes currentBonus;
	ImprovementTypes currentImprovement;
	FeatureTypes currentFeature;
	BuildTypes currentBuild;
};


#endif