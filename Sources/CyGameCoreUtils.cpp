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
{
	static CyPlot plot;
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

DirectionTypes cyDirectionXYFromPlot(const CyPlot& kFromPlot, const CyPlot& kToPlot)
{
	return directionXY(kFromPlot.getPlot(), kToPlot.getPlot());
}

CyPlot* cyPlotCity(int iX, int iY, int iIndex)
{
	return new CyPlot(plotCity(iX, iY, iIndex));
}

int cyPlotCityXYFromInt(int iDX, int iDY)
{
	return plotCityXY(iDX, iDY);
}

int cyPlotCityXYFromCity(const CyCity& kCity, const CyPlot& kPlot)
{
	return plotCityXY(kCity.getCity(), kPlot.getPlot());
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

CyCity* cyGetCity(const IDInfo city)
{
	return new CyCity(getCity(city));
}

CyUnit* cyGetUnit(const IDInfo unit)
{
	return new CyUnit(getUnit(unit));
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

int cyGetCombatOdds(const CyUnit& kAttacker, const CyUnit& kDefender)
{
	return getCombatOdds(kAttacker.getUnit(), kDefender.getUnit());
}

int cyGetEspionageModifier(int iOurTeam, int iTargetTeam)
{
	return getEspionageModifier((TeamTypes)iOurTeam, (TeamTypes)iTargetTeam);
}

int64_t cyIntSqrt64(uint64_t iValue)
{
	return intSqrt64(iValue);
}
