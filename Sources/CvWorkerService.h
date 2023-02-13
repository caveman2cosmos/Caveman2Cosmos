#pragma once

#include "CvPlot.h"
#include "CvCity.h"

class CvBuildInfo;

#ifndef CV_WORKER_SERVICE
#define CV_WORKER_SERVICE

class CvWorkerService
{
public:
	static bool ShouldImproveCity(CvCity* targetCity);
	static bool ImproveBonus(CvUnitAI* unit, int allowedMovementTurns = 4);
	static bool IsPlotValid(CvUnit* unit, CvPlot* plot);
	static BuildTypes GetFastestBuildForImprovementType(const CvPlayer& player, const ImprovementTypes improvementType, const CvPlot* plot,const CvUnitAI* unit = NULL, bool includeCurrentImprovement = true);
};

#endif