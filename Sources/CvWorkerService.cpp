
#include "FProfiler.h"

#include "CvWorkerService.h"
#include "CvDefines.h"
#include "CvInfos.h"
#include "CvPlayerAI.h"
#include "CvBonusInfo.h"
#include "CvReachablePlotSet.h"
#include "CvImprovementInfo.h"
#include "CvGameAI.h"

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

bool CvWorkerService::ImproveBonus(CvUnitAI* unit, int allowedMovementTurns)
{
	PROFILE_EXTRA_FUNC();
	const CvPlot* unitPlot = unit->plot();
	const PlayerTypes unitOwner = unit->getOwner();
	const CvPlayerAI& ownerReference = GET_PLAYER(unitOwner);
	const int iBasePathFlags = MOVE_SAFE_TERRITORY | MOVE_AVOID_ENEMY_UNITS | MOVE_OUR_TERRITORY | MOVE_RECONSIDER_ON_LEAVING_OWNED;
	const bool gameOptionLeaveForests = ownerReference.isOption(PLAYEROPTION_LEAVE_FORESTS);
	const bool gameOptionSafeAutomation = ownerReference.isOption(PLAYEROPTION_SAFE_AUTOMATION);
	const bool gameOptionZoneOfControl = GC.getGame().isOption(GAMEOPTION_UNSUPPORTED_ZONE_OF_CONTROL);
	const bool bCanRoute = unit->canBuildRoute();
	const int maxDistanceFromBorder = unit->getGroup()->getNumUnits() > 1 && unit->getGroup()->canDefend() ? GC.getAI_WORKER_MAX_DISTANCE_FROM_CITY_OUT_BORDERS() / 2 + 1 : -1;
	BuildTypes overallBestBuild = NO_BUILD;
	int bestBonusValue = 0;
	int finalNumberOfMoveTurns = 0;
	CvPlot* bestPlot = NULL;

	CvReachablePlotSet plotSet(unit->getGroup(), iBasePathFlags, unit->AI_searchRange(allowedMovementTurns), true, maxDistanceFromBorder);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr) {

		CvPlot* loopedPlot = itr.plot();
		const PlayerTypes plotOwner = loopedPlot->getOwner();
		const ImprovementTypes currentImprovementOnPlot = loopedPlot->getImprovementType();
		// const CvImprovementInfo* currentImprovementInfo = currentImprovementOnPlot != NO_IMPROVEMENT ? &GC.getImprovementInfo(currentImprovementOnPlot) : NULL;

		if (currentImprovementOnPlot != NO_IMPROVEMENT) continue;

		if (unitOwner != plotOwner && plotOwner != NO_PLAYER) continue;

		if (!IsPlotValid(unit, loopedPlot) || loopedPlot->area() != unitPlot->area() || loopedPlot->getWorkingCity() != NULL) continue;

		BonusTypes nonObsoleteBonusType = loopedPlot->getNonObsoleteBonusType(unit->getTeam());

		if (nonObsoleteBonusType == NO_BONUS) continue;

		CvBonusInfo* plotBonusInfo = &GC.getBonusInfo(nonObsoleteBonusType);
		BuildTypes bestBuildForPlot = NO_BUILD;
		int bestDefenseValue = 0;

		// TODO: Improve this evaluation, but its a start.
		foreach_(const ImprovementTypes potentialImprovementType, plotBonusInfo->getProvidedByImprovementTypes()) {

			const BuildTypes tempPlotBuild = GetFastestBuildForImprovementType(ownerReference, potentialImprovementType, loopedPlot, unit, false);

			if (tempPlotBuild != NO_BUILD) {

				const CvImprovementInfo* potentialImprovement = &GC.getImprovementInfo(potentialImprovementType);

				const int tempDefenseValue = (
					1 // minimum 1 value for providing the bonus.
					+ potentialImprovement->getAirBombDefense() / 10
					+ potentialImprovement->getDefenseModifier() / 10
					+ (gameOptionZoneOfControl && potentialImprovement->isZOCSource() ? 3 : 0)
				);
				if (tempDefenseValue > bestDefenseValue) {
					bestDefenseValue = tempDefenseValue;
					bestBuildForPlot = tempPlotBuild;
				}
			}
		}
		if (bestBuildForPlot == NO_BUILD) continue;

		//const bool haveBonus = ownerReference.hasBonus(nonObsoleteBonusType);
		//const bool plotIsConnected = loopedPlot->isConnectedToCapital(unitOwner);

		int numberOfMoveTurns = 0;
		if (!unit->atPlot(loopedPlot) && !unit->generatePath(loopedPlot, iBasePathFlags, false, &numberOfMoveTurns)) continue;

		int tempBonusValue = std::max(1, ownerReference.AI_bonusVal(nonObsoleteBonusType));

		if (numberOfMoveTurns < 1)
		{
			tempBonusValue *= 3;
			tempBonusValue /= 2;
		}
		else tempBonusValue = std::max(1, ownerReference.AI_bonusVal(nonObsoleteBonusType) / numberOfMoveTurns);

		/*if (numberOfMoveTurns <= allowedMovementTurns)*/ {
			if (bestBonusValue < tempBonusValue) {
				bestBonusValue = tempBonusValue;
				overallBestBuild = bestBuildForPlot;
				bestPlot = loopedPlot;
				finalNumberOfMoveTurns = numberOfMoveTurns;
			}
		}
	}
	if (overallBestBuild == NO_BUILD) return false;
	MissionTypes eBestMission = MISSION_MOVE_TO;

	if (finalNumberOfMoveTurns >= stepDistance(unit->getX(), unit->getY(), bestPlot->getX(), bestPlot->getY())) {
		eBestMission = MISSION_ROUTE_TO;
	}

	if (unit->getGroup()->pushMissionInternal(eBestMission, bestPlot->getX(), bestPlot->getY(), (unit->isHuman() ? 0 : MOVE_WITH_CAUTION), false, false, MISSIONAI_BUILD, bestPlot))
	{
		unit->getGroup()->pushMission(MISSION_BUILD, overallBestBuild, -1, 0, (unit->getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, bestPlot);
		return true;
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
