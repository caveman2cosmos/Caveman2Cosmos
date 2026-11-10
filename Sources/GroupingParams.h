#pragma once

#ifndef CV_GROUPING_PARAMS
#define CV_GROUPING_PARAMS

#include "CvEnums.h"

struct GroupingParams
{
	GroupingParams()
		: eUnitAI(NO_UNITAI)
		, iMaxGroup(-1)
		, iMaxOwnUnitAI(-1)
		, iMinUnitAI(-1)
		, bIgnoreFaster(false)
		, bIgnoreOwnUnitType(false)
		, bStackOfDoom(false)
		, bBiggerGroupOnly(false)
		, bMergeWholeGroup(false)
		, iMaxPath(MAX_INT)
		, bAllowRegrouping(false)
		, bWithCargoOnly(false)
		, bInCityOnly(false)
		, eIgnoreMissionAIType(NO_MISSIONAI)
	{}
	GroupingParams& withUnitAI(UnitAITypes unitAI) { eUnitAI = unitAI; return *this; }
	GroupingParams& maxGroupSize(int maxGroup) { iMaxGroup = maxGroup; return *this; }
	GroupingParams& maxOwnUnitAI(int maxOwnUnitAI) { iMaxOwnUnitAI = maxOwnUnitAI; return *this; }
	GroupingParams& minUnitAI(int minUnitAI) { iMinUnitAI = minUnitAI; return *this; }
	GroupingParams& ignoreFaster(bool state = true) { bIgnoreFaster = state; return *this; }
	GroupingParams& ignoreOwnUnitType(bool state = true) { bIgnoreOwnUnitType = state; return *this; }
	GroupingParams& biggerGroupOnly(bool state = true) { bBiggerGroupOnly = state; return *this; }
	GroupingParams& mergeWholeGroup(bool state = true) { bBiggerGroupOnly = state; return *this; }
	GroupingParams& stackOfDoom(bool state = true) { bStackOfDoom = state; return *this; }
	GroupingParams& maxPathTurns(int maxPath) { iMaxPath = maxPath; return *this; }
	GroupingParams& allowRegrouping(bool state = true) { bAllowRegrouping = state; return *this; }
	GroupingParams& withCargoOnly(bool state = true) { bWithCargoOnly = state; return *this; }
	GroupingParams& inCityOnly(bool state = true) { bInCityOnly = state; return *this; }
	GroupingParams& ignoreMissionAIType(MissionAITypes ignoreMissionAIType) { eIgnoreMissionAIType = ignoreMissionAIType; return *this; }

	UnitAITypes eUnitAI;
	int iMaxGroup;
	int iMaxOwnUnitAI;
	int iMinUnitAI;
	bool bIgnoreFaster;
	bool bIgnoreOwnUnitType;
	bool bStackOfDoom;
	bool bBiggerGroupOnly;
	bool bMergeWholeGroup;
	int iMaxPath;
	bool bAllowRegrouping;
	bool bWithCargoOnly;
	bool bInCityOnly;
	MissionAITypes eIgnoreMissionAIType;
};
#endif