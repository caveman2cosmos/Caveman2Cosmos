#include "CvGameCoreDLL.h"
#include "FProfiler.h"

#include "CvWorkerService.h"
#include "CvDefines.h"
#include "CvInfos.h"
#include "CvPlayerAI.h"
#include "CvImprovementInfo.h"

bool CvWorkerService::ShouldImproveCity(CvCity* targetCity)
{
	PROFILE_EXTRA_FUNC();
	if (targetCity == NULL) return false;
	foreach_(const CvPlot * pLoopPlot, targetCity->plots())
	{
		const int plotIndex = targetCity->getCityPlotIndex(pLoopPlot);
		if (pLoopPlot
		&& pLoopPlot->getWorkingCity() == targetCity
		&& pLoopPlot->getImprovementType() == NO_IMPROVEMENT
		&& targetCity->AI_getBestBuildValue(plotIndex) > 0)
		{
			const BuildTypes eBuild = (BuildTypes)targetCity->AI_getBestBuild(plotIndex);

			if (eBuild != NO_BUILD && GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT)
			{
				return true;
			}
		}
	}
	return false;
}

bool CvWorkerService::IsPlotValid(CvUnit* unit, CvPlot* plot)
{
	const CvUnitInfo& unitInfo = unit->getUnitInfo();
	if (unitInfo.isNoRevealMap() && unit->willRevealByMove(plot))
	{
		return false;
	}

	switch (unit->getDomainType())
	{
	case DOMAIN_SEA:
		if (plot->isWater() || unit->canMoveAllTerrain() || plot->isCanMoveSeaUnits()) return true;
		else if (plot->isFriendlyCity(*unit, true) && plot->isCoastalLand()) return true;
		break;

	case DOMAIN_LAND:
		if (!plot->isWater() || unit->canMoveAllTerrain() || plot->isSeaTunnel()) return true;
		break;

	case DOMAIN_IMMOBILE:
		break;

	case DOMAIN_AIR:
	default:
		FErrorMsg("error");
		break;
	}
	return false;
}

BuildTypes CvWorkerService::GetFastestBuildForImprovementType(const CvPlayer& player, const ImprovementTypes improvementType, const CvPlot* plot, const CvUnitAI* unit, bool includeCurrentImprovement)
{
	PROFILE_EXTRA_FUNC();
	int fastestTime = 10000;
	BuildTypes fastestBuild = NO_BUILD;
	const ImprovementTypes currentImprovementOnPlot = plot->getImprovementType();
	CvImprovementInfo* potentialImprovement = &GC.getImprovementInfo(improvementType);
	const bool checkUnitBuild = unit != NULL;


	foreach_(const BuildTypes eBuildType, potentialImprovement->getBuildTypes())
	{
		if (player.canBuild(plot, eBuildType, false, false) || (includeCurrentImprovement && improvementType == currentImprovementOnPlot))
		{
			if (!checkUnitBuild || unit->canBuild(plot, eBuildType, false)) {
				const int buildTime = GC.getBuildInfo(eBuildType).getTime();
				if (fastestTime > buildTime) {
					fastestTime = buildTime;
					fastestBuild = eBuildType;
				}
			}

		}
	}
	return fastestBuild;
}
