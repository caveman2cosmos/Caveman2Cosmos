#include "CvGameCoreDLL.h"
#include "CyGameCoreUtils.h"
#include "CyCity.h"
#include "CyPlot.h"
#include "CyUnit.h"

int cyIntRange(int iNum, int iLow, int iHigh)
{
	return range(iNum, iLow, iHigh);
}

float cyFloatRange(float fNum, float fLow, float fHigh)
{
	return range(fNum, fLow, fHigh);
}

int cyDxWrap(int iDX)
{
	return dxWrap(iDX);
}

int cyDyWrap(int iDY)
{
	return dyWrap(iDY);
}

int cyPlotDistance(int iX, int iY, int iX2, int iY2)
{
	return plotDistance(iX, iY, iX2, iY2);
}

int cyStepDistance(int iX1, int iY1, int iX2, int iY2)
{
	return stepDistance(iX1, iY1, iX2, iY2);
}

CyPlot* cyPlotDirection(int iX, int iY, DirectionTypes eDirection)
{
	return new CyPlot(plotDirection(iX, iY, eDirection));
}

CyPlot* cyPlotCardinalDirection(int iX, int iY, CardinalDirectionTypes eCardDirection)
{
	return new CyPlot(plotCardinalDirection(iX, iY, eCardDirection));
}

CyPlot* cysPlotCardinalDirection(int iX, int iY, CardinalDirectionTypes eCardDirection)
{	static CyPlot plot;
plot.setPlot(plotCardinalDirection(iX, iY, eCardDirection));
return &plot;
}

CyPlot* cyPlotXY(int iX, int iY, int iDX, int iDY)
{
	return new CyPlot(plotXY(iX, iY, iDX, iDY));
}

CyPlot* cysPlotXY(int iX, int iY, int iDX, int iDY)
{
	static CyPlot plot;
	plot.setPlot(plotXY(iX, iY, iDX, iDY));
	return &plot;
}

DirectionTypes cyDirectionXYFromInt(int iDX, int iDY)
{
	return directionXY(iDX, iDY);
}

DirectionTypes cyDirectionXYFromPlot(CyPlot* pFromPlot, CyPlot* pToPlot)
{
	return directionXY(pFromPlot->getPlot(), pToPlot->getPlot());
}

CyPlot* cyPlotCity(int iX, int iY, int iIndex)
{
	return new CyPlot(plotCity(iX, iY, iIndex));
}

int cyPlotCityXYFromInt(int iDX, int iDY)
{
	return plotCityXY(iDX, iDY);
}

int cyPlotCityXYFromCity(CyCity* pCity, CyPlot* pPlot)
{
	return plotCityXY(pCity->getCity(), pPlot->getPlot());
}

CardinalDirectionTypes cyGetOppositeCardinalDirection(CardinalDirectionTypes eCardDirection)
{
	return getOppositeCardinalDirection(eCardDirection);
}

DirectionTypes cyCardinalDirectionToDirection(CardinalDirectionTypes eCard)
{
	return cardinalDirectionToDirection(eCard);
}

bool cyIsCardinalDirection(DirectionTypes eDirection)
{
	return isCardinalDirection(eDirection);
}

DirectionTypes cyEstimateDirection(int iDX, int iDY)
{
	return estimateDirection(iDX, iDY);
}

bool cyAtWar(int /*TeamTypes*/ eTeamA, int /*TeamTypes*/ eTeamB)
{
	return atWar((TeamTypes)eTeamA, (TeamTypes)eTeamB);
}

bool cyIsPotentialEnemy(int /*TeamTypes*/ eOurTeam, int /*TeamTypes*/ eTheirTeam)
{
	return isPotentialEnemy((TeamTypes)eOurTeam, (TeamTypes)eTheirTeam);
}

CyCity* cyGetCity(IDInfo city)
{
	return new CyCity(getCity(city));
}

CyUnit* cyGetUnit(IDInfo unit)
{
	return new CyUnit(getUnit(unit));
}

bool cyIsPromotionValid(int /*PromotionTypes*/ ePromotion, int /*UnitTypes*/ eUnit, bool bLeader)
{
	return isPromotionValid((PromotionTypes) ePromotion, (UnitTypes) eUnit, bLeader);
}

int cyGetPopulationAsset(int iPopulation)
{
	return getPopulationAsset(iPopulation);
}

int cyGetLandPlotsAsset(int iLandPlots)
{
	return getLandPlotsAsset(iLandPlots);
}

int cyGetPopulationPower(int iPopulation)
{
	return getPopulationPower(iPopulation);
}

int cyGetPopulationScore(int iPopulation)
{
	return getPopulationScore(iPopulation);
}

int cyGetLandPlotsScore(int iPopulation)
{
	return getLandPlotsScore(iPopulation);
}

int cyGetTechScore(int /*TechTypes*/ eTech)
{
	return getTechScore((TechTypes)eTech);
}

int cyGetWonderScore(int /*BuildingTypes*/ eWonder)
{
	return getWonderScore((BuildingTypes)eWonder);
}

int /*ImprovementTypes*/ cyFinalImprovementUpgrade(int /*ImprovementTypes*/ eImprovement, int iCount)
{
	return finalImprovementUpgrade((ImprovementTypes) eImprovement, iCount);
}

int cyGetWorldSizeMaxConscript(int /*CivicTypes*/ eCivic)
{
	return getWorldSizeMaxConscript((CivicTypes) eCivic);
}

bool cyIsReligionTech(int /*TechTypes*/ eTech)
{
	return isReligionTech((TechTypes) eTech);
}

bool cyIsTechRequiredForUnit(int /*TechTypes*/ eTech, int /*UnitTypes*/ eUnit)
{
	return isTechRequiredForUnit((TechTypes)eTech, (UnitTypes)eUnit);
}

bool cyIsTechRequiredForBuilding(int /*TechTypes*/ eTech, int /*BuildingTypes*/ eBuilding)
{
	return isTechRequiredForBuilding((TechTypes)eTech, (BuildingTypes)eBuilding);
}

bool cyIsTechRequiredForProject(int /*TechTypes*/ eTech, int /*ProjectTypes*/ eProject)
{
	return isTechRequiredForProject((TechTypes)eTech, (ProjectTypes)eProject);
}

bool cyIsWorldUnit(int /*UnitTypes*/ eUnit)
{
	return isWorldUnit((UnitTypes)eUnit);
}

bool cyIsNationalUnit(int /*UnitTypes*/ eUnit)
{
	return isNationalUnit((UnitTypes)eUnit);
}

bool cyIsLimitedUnit(int /*UnitTypes*/ eUnit)
{
	return isLimitedUnit((UnitTypes)eUnit);
}

bool cyIsWorldWonder(int /*BuildingTypes*/ eBuilding)
{
	return isWorldWonder((BuildingTypes)eBuilding);
}

bool cyIsTeamWonder(int /*BuildingTypes*/ eBuilding)
{
	return isTeamWonder((BuildingTypes)eBuilding);
}

bool cyIsNationalWonder(int /*BuildingTypes*/ eBuilding)
{
	return isNationalWonder((BuildingTypes)eBuilding);
}

bool cyIsLimitedWonder(int /*BuildingTypes*/ eBuilding)
{
	return isLimitedWonder((BuildingTypes)eBuilding);
}

bool cyIsWorldProject(int /*ProjectTypes*/ eProject)
{
	return isWorldProject((ProjectTypes)eProject);
}

bool cyIsTeamProject(int /*ProjectTypes*/ eProject)
{
	return isTeamProject((ProjectTypes)eProject);
}

bool cyIsLimitedProject(int /*ProjectTypes*/ eProject)
{
	return isLimitedProject((ProjectTypes)eProject);
}

int cyGetCombatOdds(CyUnit* pAttacker, CyUnit* pDefender)
{
	return getCombatOdds(pAttacker->getUnit(), pDefender->getUnit());
}

int cyGetEspionageModifier(int iOurTeam, int iTargetTeam)
{
	return getEspionageModifier((TeamTypes)iOurTeam, (TeamTypes)iTargetTeam);
}

// BUG - Unit Experience - start
int cyCalculateExperience(int iLevel, int ePlayer)
{
	return calculateExperience(iLevel, (PlayerTypes)ePlayer);
}

int cyCalculateLevel(int iExperience, int ePlayer)
{
	return calculateLevel(iExperience, (PlayerTypes)ePlayer);
}
// BUG - Unit Experience - end

