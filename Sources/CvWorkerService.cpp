#include "CvWorkerService.h"
#include "CvDefines.h"
#include "CvInfos.h"
#include "CvPlayerAI.h"
#include "CvBonusInfo.h"
#include "CvReachablePlotSet.h"
#include "CvImprovementInfo.h"

bool CvWorkerService::ShouldImproveCity(CvCity* targetCity)
{
	if (targetCity == NULL) return false;
	foreach_(const CvPlot * pLoopPlot, targetCity->plots())
	{
		const int plotIndex = targetCity->getCityPlotIndex(pLoopPlot);
		if (pLoopPlot != NULL
		&& pLoopPlot->getWorkingCity() == targetCity
		&& pLoopPlot->isBeingWorked()
		&& pLoopPlot->getImprovementType() == NO_IMPROVEMENT
		&& targetCity->AI_getBestBuildValue(plotIndex) > 0
		&& GC.getBuildInfo((BuildTypes)targetCity->AI_getBestBuild(plotIndex)).getImprovement() != NO_IMPROVEMENT)
		{
			return true;
		}
	}
	return false;
}

bool CvWorkerService::ImproveBonus(CvUnitAI* unit, CvPlot* plot, int allowedMovementTurns)
{

	const PlayerTypes unitOwner = unit->getOwner();
	const CvPlayerAI& ownerReference = GET_PLAYER(unitOwner);
	const int iBasePathFlags = MOVE_SAFE_TERRITORY | MOVE_AVOID_ENEMY_UNITS | (unit->isHuman() ? MOVE_OUR_TERRITORY : MOVE_IGNORE_DANGER | MOVE_RECONSIDER_ON_LEAVING_OWNED);
	const bool gameOptionLeaveForests = ownerReference.isOption(PLAYEROPTION_LEAVE_FORESTS);
	const bool gameOptionSafeAutomation = ownerReference.isOption(PLAYEROPTION_SAFE_AUTOMATION);
	const bool gameOptionZoneOfControl = GC.getGame().isOption(GAMEOPTION_ZONE_OF_CONTROL);
	const bool bCanRoute = unit->canBuildRoute();
	const int maxDistanceFromBorder = unit->getGroup()->getNumUnits() > 1 && unit->getGroup()->canDefend() ? GC.getAI_WORKER_MAX_DISTANCE_FROM_CITY_OUT_BORDERS() / 2 + 1 : -1;
	BuildTypes overallBestBuild = NO_BUILD;
	int bestBonusValue = 0;
	int numberOfMoveTurns = 0;
	CvPlot* bestPlot = NULL;

	CvReachablePlotSet plotSet(unit->getGroup(), iBasePathFlags, MAX_INT, true, maxDistanceFromBorder);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr) {

		CvPlot* loopedPlot = itr.plot();
		const PlayerTypes plotOwner = loopedPlot->getOwner();
		const ImprovementTypes currentImprovementOnPlot = loopedPlot->getImprovementType();
		const CvImprovementInfo* currentImprovementInfo = currentImprovementOnPlot != (ImprovementTypes)NO_IMPROVEMENT ? &GC.getImprovementInfo(currentImprovementOnPlot) : NULL;

		if (!IsPlotValid(unit, plot) || loopedPlot->area() != plot->area() || plot->getWorkingCity() != NULL) continue;

		if (unitOwner != plotOwner && plotOwner != NO_PLAYER) continue;

		BonusTypes nonObsoleteBonusType = loopedPlot->getNonObsoleteBonusType(unit->getTeam());

		if (nonObsoleteBonusType == NO_BONUS) continue;

		CvBonusInfo* plotBonusInfo = &GC.getBonusInfo(nonObsoleteBonusType);
		CvImprovementInfo* potentialImprovement = NULL;
		BuildTypes bestBuildForPlot = NO_BUILD;
		int bestDefenseValue = 0;

		// TODO: Improve this evaluation, but its a start.
		foreach_(const ImprovementTypes potentialImprovementType, plotBonusInfo->getProvidedByImprovementTypes()) {

			potentialImprovement = &GC.getImprovementInfo(potentialImprovementType);

			BuildTypes tempPlotBuild = GetFastestBuildForImprovementType(ownerReference, potentialImprovementType, plot, false);

			const int tempDefenseValue = (
				1
				+ potentialImprovement->getAirBombDefense() / 10
				+ potentialImprovement->getDefenseModifier() / 10
				+ (gameOptionZoneOfControl && potentialImprovement->isZOCSource() ? 3 : 0)
			);
			if (tempPlotBuild != NO_BUILD) {
				if (tempDefenseValue > bestDefenseValue) {
					bestDefenseValue = tempDefenseValue;
					bestBuildForPlot = tempPlotBuild;
				}
			}
		}
		if (bestBuildForPlot == NO_BUILD) continue;

		//const bool haveBonus = ownerReference.hasBonus(nonObsoleteBonusType);
		//const bool plotIsConnected = loopedPlot->isConnectedToCapital(unitOwner);

		if (!unit->generatePath(loopedPlot, iBasePathFlags, false, &numberOfMoveTurns)) continue;

		const int tempBonusValue = std::max(1, (ownerReference.AI_bonusVal(nonObsoleteBonusType) + bestDefenseValue) / std::max(1, numberOfMoveTurns));

		/*if (numberOfMoveTurns <= allowedMovementTurns)*/ {
			if (bestBonusValue < tempBonusValue) {
				bestBonusValue = tempBonusValue;
				overallBestBuild = bestBuildForPlot;
				bestPlot = loopedPlot;
			}
		}
	}
	if (overallBestBuild == NO_BUILD) return false;
	MissionTypes eBestMission = MISSION_MOVE_TO;

	if (numberOfMoveTurns >= stepDistance(unit->getX(), unit->getY(), bestPlot->getX(), bestPlot->getY())) {
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

BuildTypes CvWorkerService::GetFastestBuildForImprovementType(const CvPlayer& player,const ImprovementTypes improvementType, const CvPlot* plot, bool includeCurrentImprovement)
{
	int fastestTime = 10000;
	BuildTypes fastestBuild = NO_BUILD;
	const ImprovementTypes currentImprovementOnPlot = plot->getImprovementType();
	CvImprovementInfo* potentialImprovement = &GC.getImprovementInfo(improvementType);

	foreach_(const BuildTypes eBuildType, potentialImprovement->getBuildTypes())
	{
		if (player.canBuild(plot, eBuildType, false, false) || (includeCurrentImprovement && improvementType == currentImprovementOnPlot))
		{
			const int buildTime = GC.getBuildInfo(eBuildType).getTime();
			if (fastestTime > buildTime) {
				fastestTime = buildTime;
				fastestBuild = eBuildType;
			}
		}
	}
	return fastestBuild;
}
