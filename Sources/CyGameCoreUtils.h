#pragma once

#ifndef CyGameCoreUtils_h
#define CyGameCoreUtils_h

//
// Python wrapper functions for DLL
//

class CyCity;
class CyPlot;
class CyUnit;

int cyIntRange(int iNum, int iLow, int iHigh);
float cyFloatRange(float fNum, float fLow, float fHigh);
int cyDxWrap(int iDX);
int cyDyWrap(int iDY);
int cyPlotDistance(int iX1, int iY1, int iX2, int iY2);
int cyStepDistance(int iX1, int iY1, int iX2, int iY2);
CyPlot* cyPlotDirection(int iX, int iY, DirectionTypes eDirection);
CyPlot* cyPlotCardinalDirection(int iX, int iY, CardinalDirectionTypes eCardDirection);
CyPlot* cysPlotCardinalDirection(int iX, int iY, CardinalDirectionTypes eCardDirection);
CyPlot* cyPlotXY(int iX, int iY, int iDX, int iDY);
CyPlot* cysPlotXY(int iX, int iY, int iDX, int iDY);
DirectionTypes cyDirectionXYFromInt(int iDX, int iDY);
DirectionTypes cyDirectionXYFromPlot(const CyPlot& kFromPlot, const CyPlot& kToPlot);
CyPlot* cyPlotCity(int iX, int iY, int iIndex);
int cyPlotCityXYFromInt(int iDX, int iDY);
int cyPlotCityXYFromCity(const CyCity& kCity, const CyPlot& kPlot);

DirectionTypes cyEstimateDirection(int iDX, int iDY);

bool cyAtWar(int /*TeamTypes*/ eTeamA, int /*TeamTypes*/ eTeamB);
bool cyIsPotentialEnemy(int /*TeamTypes*/ eOurPlayer, int /*TeamTypes*/ eTheirPlayer);

CyCity* cyGetCity(const IDInfo city);
CyUnit* cyGetUnit(const IDInfo unit);

bool cyIsPromotionValid(int /*PromotionTypes*/ ePromotion, int /*UnitTypes*/ eUnit, bool bLeader);
int cyGetTechScore(int /*TechTypes*/ eTech);
int cyGetWonderScore(int /*BuildingTypes*/ eWonder);
int /*ImprovementTypes*/ cyFinalImprovementUpgrade(int /*ImprovementTypes*/ eImprovement, int iCount);

int cyGetWorldSizeMaxConscript(int /*CivicTypes*/ eCivic);

bool cyIsReligionTech(int /*TechTypes*/ eTech);

bool cyIsTechRequiredForUnit(int /*TechTypes*/ eTech, int /*UnitTypes*/ eUnit);
bool cyIsTechRequiredForBuilding(int /*TechTypes*/ eTech, int /*BuildingTypes*/ eBuilding);
bool cyIsTechRequiredForProject(int /*TechTypes*/ eTech, int /*ProjectTypes*/ eProject);
bool cyIsWorldUnit(int /*UnitTypes*/ eUnit);
bool cyIsNationalUnit(int /*UnitTypes*/ eUnit);
bool cyIsLimitedUnit(int /*UnitTypes*/ eUnit);
bool cyIsWorldWonder(int /*BuildingTypes*/ eBuilding);
bool cyIsTeamWonder(int /*BuildingTypes*/ eBuilding);
bool cyIsNationalWonder(int /*BuildingTypes*/ eBuilding);
bool cyIsLimitedWonder(int /*BuildingTypes*/ eBuilding);
bool cyIsWorldProject(int /*ProjectTypes*/ eProject);
bool cyIsTeamProject(int /*ProjectTypes*/ eProject);
bool cyIsLimitedProject(int /*ProjectTypes*/ eProject);
int cyGetCombatOdds(const CyUnit& kAttacker, const CyUnit& kDefender);
int cyGetEspionageModifier(int /*TeamTypes*/ iOurTeam, int /*TeamTypes*/ iTargetTeam);

int cyCalculateExperience(int iLevel, int /*PlayerTypes*/ ePlayer);
int cyCalculateLevel(int iExperience, int /*PlayerTypes*/ ePlayer);

int64_t cyIntSqrt64(uint64_t iValue);

#endif	// CyGameCoreUtils_h
