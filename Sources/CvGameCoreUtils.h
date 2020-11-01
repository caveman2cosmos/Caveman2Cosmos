#pragma once

// utils.h

#ifndef CIV4_GAMECORE_UTILS_H
#define CIV4_GAMECORE_UTILS_H


#include "CvGlobals.h"
#include "CvMap.h"


class CvPlot;
class CvCity;
class CvUnit;
class CvString;
class CvRandom;
class FAStarNode;
class FAStar;
class CvInfoBase;


#ifndef SQR
#define SQR(x) ( (x)*(x))
#endif

#undef max
#undef min

//sign function taken from FirePlace - JW
template<class T> __forceinline T getSign( T x ) { return (( x < 0 ) ? T(-1) : x > 0 ? T(1) : T(0)); };

inline int range(int iNum, int iLow, int iHigh)
{
	FAssertMsg(iHigh >= iLow, "High should be higher than low");

	if (iNum < iLow)
	{
		return iLow;
	}
	if (iNum > iHigh)
	{
		return iHigh;
	}
	return iNum;
}

inline float range(float fNum, float fLow, float fHigh)
{
	FAssertMsg(fHigh >= fLow, "High should be higher than low");

	if (fNum < fLow)
	{
		return fLow;
	}
	if (fNum > fHigh)
	{
		return fHigh;
	}
	return fNum;
}

inline int coordDistance(int iFrom, int iTo, int iRange, bool bWrap)
{
	if (bWrap && abs(iFrom - iTo) > iRange/2)
	{
		return iRange - abs(iFrom - iTo);
	}
	return abs(iFrom - iTo);
}

inline int wrapCoordDifference(int iDiff, int iRange, bool bWrap)
{
	if (bWrap)
	{
		if (iDiff > iRange/2)
		{
			return iDiff - iRange;
		}
		if (iDiff < -iRange/2)
		{
			return iDiff + iRange;
		}
	}
	return iDiff;
}

inline int xDistance(int iFromX, int iToX)
{
	return coordDistance(iFromX, iToX, GC.getMap().getGridWidth(), GC.getMap().isWrapX());
}

inline int yDistance(int iFromY, int iToY)
{
	return coordDistance(iFromY, iToY, GC.getMap().getGridHeight(), GC.getMap().isWrapY());
}

inline int dxWrap(int iDX)																													// Exposed to Python
{
	return wrapCoordDifference(iDX, GC.getMap().getGridWidth(), GC.getMap().isWrapX());
}

inline int dyWrap(int iDY)																													// Exposed to Python
{
	return wrapCoordDifference(iDY, GC.getMap().getGridHeight(), GC.getMap().isWrapY());
}

// 4 | 4 | 3 | 3 | 3 | 4 | 4
// -------------------------
// 4 | 3 | 2 | 2 | 2 | 3 | 4
// -------------------------
// 3 | 2 | 1 | 1 | 1 | 2 | 3
// -------------------------
// 3 | 2 | 1 | 0 | 1 | 2 | 3
// -------------------------
// 3 | 2 | 1 | 1 | 1 | 2 | 3
// -------------------------
// 4 | 3 | 2 | 2 | 2 | 3 | 4
// -------------------------
// 4 | 4 | 3 | 3 | 3 | 4 | 4
//
// Returns the distance between plots according to the pattern above...
inline int plotDistance(int iX1, int iY1, int iX2, int iY2)													// Exposed to Python
{
	const int iDX = xDistance(iX1, iX2);
	const int iDY = yDistance(iY1, iY2);

	return std::max(iDX, iDY) + std::min(iDX, iDY) / 2;
}

// 3 | 3 | 3 | 3 | 3 | 3 | 3
// -------------------------
// 3 | 2 | 2 | 2 | 2 | 2 | 3
// -------------------------
// 3 | 2 | 1 | 1 | 1 | 2 | 3
// -------------------------
// 3 | 2 | 1 | 0 | 1 | 2 | 3
// -------------------------
// 3 | 2 | 1 | 1 | 1 | 2 | 3
// -------------------------
// 3 | 2 | 2 | 2 | 2 | 2 | 3
// -------------------------
// 3 | 3 | 3 | 3 | 3 | 3 | 3
//
// Returns the distance between plots according to the pattern above...
inline int stepDistance(int iX1, int iY1, int iX2, int iY2)													// Exposed to Python
{
	return std::max(xDistance(iX1, iX2), yDistance(iY1, iY2));
}

inline CvPlot* plotDirection(int iX, int iY, DirectionTypes eDirection)							// Exposed to Python
{
	if (eDirection == NO_DIRECTION)
	{
		return GC.getMap().plot(iX, iY);
	}
	return GC.getMap().plot(iX + GC.getPlotDirectionX()[eDirection], iY + GC.getPlotDirectionY()[eDirection]);
}

inline CvPlot* plotDirection(const CvPlot* pPlot, DirectionTypes eDirection)
{
	return plotDirection(pPlot->getX(), pPlot->getY(), eDirection);
}

inline CvPlot* plotCardinalDirection(int iX, int iY, CardinalDirectionTypes eCardinalDirection)	// Exposed to Python
{
	return GC.getMap().plot(iX + GC.getPlotCardinalDirectionX()[eCardinalDirection], iY + GC.getPlotCardinalDirectionY()[eCardinalDirection]);
}

inline CvPlot* plotXY(int iX, int iY, int iDX, int iDY)																// Exposed to Python
{
	return GC.getMap().plot(iX + iDX, iY + iDY);
}

inline DirectionTypes directionXY(int iDX, int iDY)																		// Exposed to Python
{
	if (abs(iDX) > DIRECTION_RADIUS || abs(iDY) > DIRECTION_RADIUS)
	{
		return NO_DIRECTION;
	}
	return GC.getXYDirection((iDX + DIRECTION_RADIUS), (iDY + DIRECTION_RADIUS));
}

inline DirectionTypes reverseDirection(DirectionTypes iDirection)																		// Exposed to Python
{
	return (DirectionTypes)((iDirection + 4)%NUM_DIRECTION_TYPES);
}

inline DirectionTypes directionXY(const CvPlot* pFromPlot, const CvPlot* pToPlot)			// Exposed to Python
{
	return directionXY(dxWrap(pToPlot->getX() - pFromPlot->getX()), dyWrap(pToPlot->getY() - pFromPlot->getY()));
}

inline DirectionTypes getAdjacentDirection(DirectionTypes eDirection, bool bClockwise)
{
	return (DirectionTypes)(((int)eDirection + (bClockwise ? 1 : -1) + NUM_DIRECTION_TYPES) % NUM_DIRECTION_TYPES);
}

CvPlot* plotCity(int iX, int iY, int iIndex);																			// Exposed to Python
int plotCityXY(int iDX, int iDY);																									// Exposed to Python
int plotCityXY(const CvCity* pCity, const CvPlot* pPlot);													// Exposed to Python

CardinalDirectionTypes getOppositeCardinalDirection(CardinalDirectionTypes eDir);	// Exposed to Python 
DirectionTypes cardinalDirectionToDirection(CardinalDirectionTypes eCard);				// Exposed to Python
DllExport bool isCardinalDirection(DirectionTypes eDirection);															// Exposed to Python
DirectionTypes estimateDirection(int iDX, int iDY);																// Exposed to Python
DllExport DirectionTypes estimateDirection(const CvPlot* pFromPlot, const CvPlot* pToPlot);
DllExport float directionAngle(DirectionTypes eDirection);

bool atWar(TeamTypes eTeamA, TeamTypes eTeamB);												// Exposed to Python
bool isPotentialEnemy(TeamTypes eOurTeam, TeamTypes eTheirTeam);			// Exposed to Python

DllExport CvCity* getCity(IDInfo city);	// Exposed to Python
DllExport CvUnit* getUnit(IDInfo unit);	// Exposed to Python

bool isBeforeUnitCycle(const CvUnit* pFirstUnit, const CvUnit* pSecondUnit);
bool isPromotionValid(PromotionTypes ePromotion, UnitTypes eUnit, bool bLeader);	// Exposed to Python

int getPopulationAsset(int iPopulation);								// Exposed to Python
int getLandPlotsAsset(int iLandPlots);									// Exposed to Python
int getPopulationPower(int iPopulation);								// Exposed to Python
int getPopulationScore(int iPopulation);								// Exposed to Python
int getLandPlotsScore(int iLandPlots);									// Exposed to Python
int getTechScore(TechTypes eTech);											// Exposed to Python
int getWonderScore(BuildingTypes eWonder);		// Exposed to Python

ImprovementTypes finalImprovementUpgrade(ImprovementTypes eImprovement, int iCount = 0);		// Exposed to Python

int getWorldSizeMaxConscript(CivicTypes eCivic);								// Exposed to Python

bool isReligionTech(TechTypes eTech);														// Exposed to Python

bool isTechRequiredForUnit(TechTypes eTech, UnitTypes eUnit);							// Exposed to Python
bool isTechRequiredForBuilding(TechTypes eTech, BuildingTypes eBuilding);	// Exposed to Python
bool isTechRequiredForProject(TechTypes eTech, ProjectTypes eProject);		// Exposed to Python

bool isWorldUnit(UnitTypes eUnit);											// Exposed to Python
bool isNationalUnit(UnitTypes eUnit);									// Exposed to Python
bool isLimitedUnit(UnitTypes eUnit);										// Exposed to Python

bool isWorldWonder(BuildingTypes eBuilding);						// Exposed to Python
bool isTeamWonder(BuildingTypes eBuilding);						// Exposed to Python
bool isNationalWonder(BuildingTypes eBuilding);				// Exposed to Python
bool isNationalWonderGroup(BuildingTypes eBuilding);
bool isNationalWonderGroupSpecialBuilding(SpecialBuildingTypes eSpecialBuilding);
bool isLimitedWonder(BuildingTypes eBuilding);					// Exposed to Python
int limitedWonderLimit(BuildingTypes eBuilding);

bool isWorldProject(ProjectTypes eProject);														// Exposed to Python
bool isTeamProject(ProjectTypes eProject);														// Exposed to Python
bool isLimitedProject(ProjectTypes eProject);													// Exposed to Python

int64_t getBinomialCoefficient(int iN, int iK);
int getCombatOdds(const CvUnit* pAttacker, const CvUnit* pDefender);							// Exposed to Python

float getCombatOddsSpecific(const CvUnit* pAttacker, const CvUnit* pDefender, int n_A, int n_D);

int getEspionageModifier(TeamTypes eOurTeam, TeamTypes eTargetTeam);							// Exposed to Python

DllExport void setTradeItem(TradeData* pItem, TradeableItems eItemType = TRADE_ITEM_NONE, int iData = 0);

bool isPlotEventTrigger(EventTriggerTypes eTrigger);

TechTypes getDiscoveryTech(UnitTypes eUnit, PlayerTypes ePlayer);

void setListHelp(wchar_t* szBuffer, const wchar_t* szStart, const wchar_t* szItem, const wchar_t* szSeparator, bool bFirst);
void setListHelp(CvWString& szBuffer, const wchar_t* szStart, const wchar_t* szItem, const wchar_t* szSeparator, bool bFirst);
void setListHelp(CvWStringBuffer& szBuffer, const wchar_t* szStart, const wchar_t* szItem, const wchar_t* szSeparator, bool bFirst);

// PlotUnitFunc's...
bool PUF_isGroupHead( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isPlayer( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isTeam( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isCombatTeam(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis = NULL);
bool PUF_isOtherPlayer( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isOtherTeam( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isEnemy( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isEnemyTarget( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isParadrop(const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isNonAlly(const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isVisible( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isVisibleDebug( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_canSiege( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isPotentialEnemy( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_canDeclareWar( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_canDefend( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_canAttack( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isCriminal(const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isWantedCriminal(const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isCityGarrison(const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_cannotDefend( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_canDefendGroupHead( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_canDefendEnemy( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_canDefendEnemyNoAnimal( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_canDefendPotentialEnemy( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_canDefendPotentialEnemyAgainst(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis = NULL);
bool PUF_canDefenselessPotentialEnemyAgainst(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis);
bool PUF_canAirAttack( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_canAirDefend( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isFighting( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isAnimal( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isMilitaryHappiness( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isInvestigate( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isCounterSpy( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isSpy( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isUnitType( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isHealUnitCombatType(const CvUnit* pUnit, /*UnitCombatType*/int iData1, /*DomainType*/int iData2, const CvUnit* pThis = NULL);
bool PUF_isMountainLeader(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis = NULL);
bool PUF_isInjuredUnitCombatType(const CvUnit* pUnit, /*UnitCombatType*/int iData1, /*DomainType*/int iData2, const CvUnit* pThis = NULL);
bool PUF_isDomainType( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isUnitAIType( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isCityAIType( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isNotCityAIType( const CvUnit* pUnit, int iData1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isSelected( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_makeInfoBarDirty(CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isNoMission(const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isMissionAI(const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);

/********************************************************************************/
/* 	Worker Counting						03.08.2010				Fuyu			*/
/********************************************************************************/
bool PUF_isNoMissionAI(const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
/********************************************************************************/
/* 	Worker Counting												END 			*/
/********************************************************************************/
bool PUF_isFiniteRange(const CvUnit* pUnit, int iData1 = -1, int iData2 = -1, const CvUnit* pThis = NULL);
bool PUF_isAvailableUnitAITypeGroupie(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis = NULL);
bool PUF_isUnitAITypeGroupie(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis = NULL);
bool PUF_isFiniteRangeAndNotJustProduced(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis = NULL);
bool PUF_isMissionary(const CvUnit* pUnit, int /*ReligionTypes*/ iData1, int /*PlayerTypes*/ iData2, const CvUnit* pThis = NULL);
bool PUF_isAfflicted(const CvUnit* pUnit, int /*PromotionLineTypes*/ iData1, int iData2, const CvUnit* pThis = NULL);
bool PUF_isTunneledEnemy( const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis = NULL);
bool PUF_isNonTunneledEnemy(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis = NULL);

// FAStarFunc...
int potentialIrrigation(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int checkFreshWater(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int changeIrrigated(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int pathDestValid(int iToX, int iToY, const void* pointer, FAStar* finder);
int pathHeuristic(int iFromX, int iFromY, int iToX, int iToY);
int pathCost(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int pathValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int pathAdd(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int stepDestValid(int iToX, int iToY, const void* pointer, FAStar* finder);
int stepHeuristic(int iFromX, int iFromY, int iToX, int iToY);
int stepValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int stepCost(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int stepAdd(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int teamStepValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int routeValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int borderValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int areaValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int joinArea(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int plotGroupValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int countPlotGroup(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int countRegion(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);

bool moveToValid(const CvSelectionGroup* pSelectionGroup, const CvPlot* pToPlot, int iFlags);

//	Koshling - new pathing generator callback functions
int	NewPathHeuristicFunc(const CvSelectionGroup* pGroup, int iFromX, int iFromY, int iToX, int iToY, int& iLimitCost);
int	NewPathCostFunc(const CvPathGeneratorBase* generator, const CvSelectionGroup* pGroup, int iFromX, int iFromY, int iToX, int iToY, int iFlags, int& iMovementRemaining, int iPathTurns, int& iToNodeCost, bool bIsTerminalNode);
bool ContextFreeNewPathValidFunc(const CvSelectionGroup* pGroup, int iFromX, int iFromY, int iToX, int iToY, int iFlags, bool isTerminus, bool bMoveTerminationChecksOnly, int iPathTurns, bool* pbToNodeInvalidity, bool* pbValidAsTerminus);
bool NewPathValidFunc(const CvSelectionGroup* pGroup, int iFromX, int iFromY, int iToX, int iToY, int iFlags, bool isTerminus, bool bMoveTerminationChecksOnly, int iPathTurns, bool& bToNodeInvalidity);
bool NewPathDestValid(const CvSelectionGroup* pSelectionGroup, int iToX, int iToY, int iFlags, bool& bRequiresWar);
bool NewPathTurnEndValidityCheckRequired(const CvSelectionGroup* pGroup, int iFlags);

int baseYieldToSymbol(int iNumYieldTypes, int iYieldStack);

bool isPickableName(const TCHAR* szName);

DllExport int* shuffle(int iNum, CvRandom& rand);
void shuffleArray(int* piShuffle, int iNum, CvRandom& rand);

int getTurnMonthForGame(int iGameTurn, int iStartYear, CalendarTypes eCalendar, GameSpeedTypes eSpeed);
int getTurnYearForGame(int iGameTurn, int iStartYear, CalendarTypes eCalendar, GameSpeedTypes eSpeed);

void getDirectionTypeString(CvWString& szString, DirectionTypes eDirectionType);
void getCardinalDirectionTypeString(CvWString& szString, CardinalDirectionTypes eDirectionType);
void getActivityTypeString(CvWString& szString, ActivityTypes eActivityType);
void getMissionTypeString(CvWString& szString, MissionTypes eMissionType);
void getMissionAIString(CvWString& szString, MissionAITypes eMissionAI);
void getUnitAIString(CvWString& szString, UnitAITypes eUnitAI);

int calcBaseExpNeeded(const int iLevel, const PlayerTypes ePlayer);
int calculateLevel(const int iExperience, const PlayerTypes ePlayer);

DirectionTypes getOppositeDirection(DirectionTypes eDirection);
bool isAdjacentDirection(DirectionTypes eFacingDirection, DirectionTypes eOtherDirection);

//	Koshling - abstract treaty length from the define int to allow scaling
int getTreatyLength();

void AddDLLMessage(PlayerTypes ePlayer, bool bForce, int iLength, CvWString szString, LPCTSTR pszSound = NULL,
		InterfaceMessageTypes eType = MESSAGE_TYPE_INFO, LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		int iFlashX = -1, int iFlashY = -1, bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false);

//	Koshling - better checksum algorithm that can be used when reasonably high quality
//	hashes are needed
class CvChecksum {
public:
	CvChecksum() { clear(); }
	void clear() { sum = 0; r = 55665; c1 = 52845; c2 = 22719;}
	void add(int i);
	void add(uint8_t b);
	int get() { return (int)sum; }
protected:
	WORD r;
	WORD c1;
	WORD c2;
	DWORD sum;
}; 

#endif
