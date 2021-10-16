#pragma once

// cityAI.h

#ifndef CIV4_CITY_AI_H
#define CIV4_CITY_AI_H

#include <vector>
#include <vector>
#include <vector>
#include <vector>

#include "CvCity.h"
#include "CvPlayerAI.h"

//	Possible focus flags to use when evaluating buildings
#define BUILDINGFOCUS_FOOD					(1 << 1)
#define BUILDINGFOCUS_PRODUCTION			(1 << 2)
#define BUILDINGFOCUS_GOLD					(1 << 3)
#define BUILDINGFOCUS_RESEARCH				(1 << 4)
#define BUILDINGFOCUS_CULTURE				(1 << 5)
#define BUILDINGFOCUS_DEFENSE				(1 << 6)
#define BUILDINGFOCUS_HAPPY					(1 << 7)
#define BUILDINGFOCUS_HEALTHY				(1 << 8)
#define BUILDINGFOCUS_EXPERIENCE			(1 << 9)
#define BUILDINGFOCUS_MAINTENANCE			(1 << 10)
#define BUILDINGFOCUS_SPECIALIST			(1 << 11)
#define BUILDINGFOCUS_ESPIONAGE				(1 << 12)
#define BUILDINGFOCUS_BIGCULTURE			(1 << 13)
#define BUILDINGFOCUS_WORLDWONDER			(1 << 14)
#define BUILDINGFOCUS_DOMAINSEA				(1 << 15)
#define BUILDINGFOCUS_WONDEROK				(1 << 16)
#define BUILDINGFOCUS_CAPITAL				(1 << 17) // Focus on cheap buildings
#define BUILDINGFOCUS_PROPERTY				(1 << 18) // Focus on property control buildings (crime/pollution/tourism etc.)
#define BUILDINGFOCUS_INVESTIGATION			(1 << 19)

#define BUILDINGFOCUS_CONSIDER_ANY			(1 << 20)

class BuildingValueCache; // KOSHLING Mod - pre-calculate and cache building values for all focuses
class CvArea;

//	Koshling - add caching to yield calculations
#ifdef YIELD_VALUE_CACHING
struct yieldValueCacheEntry
{
	yieldValueCacheEntry()
		: aiYields()
		, aiCommerceYields()
		, bAvoidGrowth(false)
		, bRemove(false)
		, bIgnoreFood(false)
		, bIgnoreGrowth(false)
		, bIgnoreStarvation(false)
		, bWorkerOptimization(false)
		, iResult(-1)
		, iLastUseCount(0)
	{}

	short aiYields[NUM_YIELD_TYPES];
	short aiCommerceYields[NUM_COMMERCE_TYPES];
	bool bAvoidGrowth;
	bool bRemove;
	bool bIgnoreFood;
	bool bIgnoreGrowth;
	bool bIgnoreStarvation;
	bool bWorkerOptimization;
	int iResult;
	int iLastUseCount;
};

#define YIELD_VALUE_CACHE_SIZE 16

class yieldCache
{
public:
	yieldCache() : currentUseCounter(0) {}
	struct yieldValueCacheEntry entries[YIELD_VALUE_CACHE_SIZE];
	int currentUseCounter;
};
#endif

typedef std::vector<std::pair<UnitAITypes, int> > UnitTypeWeightArray;

typedef struct
{
	UnitTypes	eUnit;
	int			iValue;
} UnitValueInfo;

class CvCityAI : public CvCity
{

public:

	CvCityAI();
	virtual ~CvCityAI();

	void AI_init();
	void AI_uninit();
	void AI_reset();
	void SendLog(CvWString function, CvWString message) const;

	void AI_doTurn();

	void AI_assignWorkingPlots();
	void AI_updateAssignWork();
	void AI_doContractFloatingDefenders();

	bool AI_avoidGrowth();
	bool AI_ignoreGrowth();
	int AI_specialistValue(SpecialistTypes eSpecialist, bool bAvoidGrowth, bool bRemove) const;

	//	KOSHLING - initialisation called on every city prior to performing unit mission allocation logic
	//	This allows caches that will remain valid for the processing of the current turn's units to be cleared
	virtual void AI_preUnitTurn();

	virtual void AI_noteUnitEscortNeeded();
	virtual void AI_trained(UnitTypes eUnitType, UnitAITypes eUnitAIType);

	void AI_chooseProduction();

	UnitTypes AI_bestUnit(int& iBestValue, int iNumSelectableTypes = -1, UnitAITypes* pSelectableTypes = NULL, bool bAsync = false, UnitAITypes* peBestUnitAI = NULL, bool bNoRand = false, bool bNoWeighting = false, const CvUnitSelectionCriteria* criteria = NULL);
	UnitTypes AI_bestUnitAI(UnitAITypes eUnitAI, int& iBestValue, bool bAsync = false, bool bNoRand = false, const CvUnitSelectionCriteria* criteria = NULL);

	BuildingTypes AI_bestBuildingThreshold(int iFocusFlags = 0, int iMaxTurns = MAX_INT, int iMinThreshold = 0, bool bAsync = false, AdvisorTypes eIgnoreAdvisor = NO_ADVISOR, bool bMaximizeFlaggedValue = false, PropertyTypes eProperty = NO_PROPERTY);
	const std::vector<ScoredBuilding> AI_bestBuildingsThreshold(int iFocusFlags = 0, int iMaxTurns = MAX_INT, int iMinThreshold = 0, bool bAsync = false, AdvisorTypes eIgnoreAdvisor = NO_ADVISOR, bool bMaximizeFlaggedValue = false, PropertyTypes eProperty = NO_PROPERTY);

	bool AI_scoreBuildingsFromListThreshold(std::vector<ScoredBuilding>& scoredBuildings, const std::vector<BuildingTypes>& possibles, int iFocusFlags = 0, int iMaxTurns = MAX_INT, int iMinThreshold = 0, bool bAsync = false, AdvisorTypes eIgnoreAdvisor = NO_ADVISOR, bool bMaximizeFlaggedValue = false, PropertyTypes eProperty = NO_PROPERTY);

	// Can a building be reasonably hurried by any method?
	bool AI_canRushBuildingConstruction(BuildingTypes building);

	int AI_buildingValue(BuildingTypes eBuilding, int iFocusFlags = 0, bool bForTech = false);
	int AI_buildingValueThreshold(BuildingTypes eBuilding, int iFocusFlags = 0, int iThreshold = 0, bool bMaximizeFlaggedValue = false, bool bIgnoreCanConstruct = false);
	int AI_buildingValueThresholdOriginal(BuildingTypes eBuilding, int iFocusFlags = 0, int iThreshold = 0, bool bMaximizeFlaggedValue = false, bool bIgnoreCanBuildReplacement = false, bool bForTech = false);
	int AI_buildingValueThresholdOriginalUncached(BuildingTypes eBuilding, int iFocusFlags = 0, int iThreshold = 0, bool bMaximizeFlaggedValue = false, bool bIgnoreCanBuildReplacement = false, bool bForTech = false);
	int getBuildingCommerceValue(BuildingTypes eBuilding, int iI, int* aiFreeSpecialistYield, int* aiFreeSpecialistCommerce, int* aiBaseCommerceRate, int* aiPlayerCommerceRate) const;

	ProjectTypes AI_bestProject() const;
	int AI_projectValue(ProjectTypes eProject) const;

	ProcessTypes AI_bestProcess(CommerceTypes eCommerceType = NO_COMMERCE, int64_t* commerceWeights = NULL) const;
	int64_t AI_processValue(ProcessTypes eProcess, CommerceTypes eCommerceType = NO_COMMERCE, int64_t* commerceWeights = NULL) const;
	bool AI_finalProcessSelection();

	int AI_neededSeaWorkers() const;

	bool AI_isDefended(int iExtra = 0, bool bAllowAnyDefenders = true);
	bool AI_isAirDefended(bool bCountLand = false, int iExtra = 0);
	bool AI_isDanger() const;
	int evaluateDanger();

	bool AI_isAdequateHappinessMilitary(int iExtra) const;
	int AI_neededHappinessDefenders() const;
	int AI_neededDefenders();
	int AI_neededAirDefenders() const;
	int AI_minDefenders() const;
	int AI_neededFloatingDefenders();
	void AI_updateNeededFloatingDefenders();

	int getGarrisonStrength(bool bAllowAnyDefenders = true) const;
	int AI_neededDefenseStrength();
	int AI_minDefenseStrength() const;

	int AI_getEmphasizeAvoidGrowthCount() const;
	bool AI_isEmphasizeAvoidGrowth() const;

	int AI_getEmphasizeGreatPeopleCount() const;
	bool AI_isEmphasizeGreatPeople() const;

	bool AI_isAssignWorkDirty() const;
	void AI_setAssignWorkDirty(bool bNewValue);

	bool AI_isChooseProductionDirty() const;
	void AI_setChooseProductionDirty(bool bNewValue);

	CvCity* AI_getRouteToCity() const;
	void AI_updateRouteToCity() const;

	bool AI_isEmphasizeYield(const YieldTypes eIndex) const;
	bool AI_isEmphasizeCommerce(const CommerceTypes eIndex) const;
	bool AI_isAnyCommerceOrYieldEmphasis() const;

	bool AI_isEmphasize(EmphasizeTypes eIndex) const;
	void AI_setEmphasize(EmphasizeTypes eIndex, bool bNewValue);
	bool AI_isEmphasizeSpecialist(SpecialistTypes eIndex) const;
	void AI_setEmphasizeSpecialist(SpecialistTypes eIndex, bool bNewValue);
	void AI_forceEmphasizeCulture(bool bNewValue);

	void AI_markBestBuildValuesStale();
	void AI_updateBestBuildForPlots();
	int AI_getBestBuildValue(int iIndex) const;
	int AI_totalBestBuildValue(const CvArea* pArea) const;

	int AI_clearFeatureValue(int iIndex) const;

	int AI_getGoodTileCount() const;
	int AI_countWorkedPoorTiles() const;
	int AI_getTargetSize() const;

	BuildTypes AI_getBestBuild(int iIndex) const;
	int AI_countBestBuilds(const CvArea* pArea) const;
	void AI_updateBestBuild();

	virtual int AI_cityValue() const;

	int AI_calculateWaterWorldPercent() const;

	int AI_getCityImportance(bool bEconomy, bool bMilitary) const;

	int AI_yieldValue(short* piYields, short* piCommerceYields, bool bAvoidGrowth, bool bRemove, bool bIgnoreFood = false, bool bIgnoreGrowth = false, bool bIgnoreStarvation = false, bool bWorkerOptimization = false) const;
	int AI_yieldMultiplier(YieldTypes eYield) const;
	void AI_updateSpecialYieldMultiplier();
	int AI_specialYieldMultiplier(YieldTypes eYield) const;

	int AI_countNumBonuses(BonusTypes eBonus, bool bIncludeOurs, bool bIncludeNeutral, int iOtherCultureThreshold, bool bLand = true, bool bWater = true) const;
	int AI_countNumImprovableBonuses(bool bIncludeNeutral, TechTypes eExtraTech = NO_TECH, bool bLand = true, bool bWater = false) const;

	int AI_playerCloseness(PlayerTypes eIndex, int iMaxDistance);
	int AI_cityThreat(TeamTypes eTargetTeam = NO_TEAM, int* piThreatModifier = NULL);

	int AI_getWorkersHave() const;
	int AI_getWorkersNeeded() const;
	void AI_changeWorkersHave(int iChange);

	// Fuyu - Worker Counting 03.08.2010
	int AI_workingCityPlotTargetMissionAIs(PlayerTypes ePlayer, MissionAITypes eMissionAI, UnitAITypes eUnitAI = NO_UNITAI, bool bSameAreaOnly = false) const;
	// ! Fuyu

	BuildingTypes AI_bestAdvancedStartBuilding(int iPass);

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);

	bool AI_trainInquisitor();

	int AI_getEmphasizeAvoidAngryCitizensCount() const;
	bool AI_isEmphasizeAvoidAngryCitizens() const;
	int AI_getEmphasizeAvoidUnhealthyCitizensCount() const;
	bool AI_isEmphasizeAvoidUnhealthyCitizens() const;
	bool AI_buildCaravan();
	int AI_getPromotionValue(PromotionTypes ePromotion) const;
	int AI_calculateActualImprovementHealth(ImprovementTypes eImprovement) const;
	int AI_getMilitaryProductionRateRank() const;
	bool AI_isMilitaryProductionCity() const;
	void AI_setMilitaryProductionCity(bool bNewVal);
	int AI_getNavalMilitaryProductionRateRank() const;
	bool AI_isNavalMilitaryProductionCity() const;
	void AI_setNavalMilitaryProductionCity(bool bNewVal);
	int AI_getBuildPriority() const;

protected:

	int m_iEmphasizeAvoidAngryCitizensCount;
	int m_iEmphasizeAvoidUnhealthyCitizensCount;
	bool m_bMilitaryProductionCity;
	bool m_bNavalMilitaryProductionCity;

	int m_iEmphasizeAvoidGrowthCount;
	int m_iEmphasizeGreatPeopleCount;

	int m_iUnitSummonedLastEstablishedTurn;

	bool m_bAssignWorkDirty;
	bool m_bChooseProductionDirty;

	mutable IDInfo m_routeToCity;
	mutable bool m_routeToCityUpdated;

	int* m_aiEmphasizeYieldCount;
	int* m_aiEmphasizeCommerceCount;
	bool m_bForceEmphasizeCulture;

	bool m_bestBuildValuesStale;
	int m_aiBestBuildValue[NUM_CITY_PLOTS];

	BuildTypes m_aeBestBuild[NUM_CITY_PLOTS];

	bool* m_pbEmphasize;
	bool* m_pbEmphasizeSpecialist;

	int* m_aiSpecialYieldMultiplier;

	int m_iCachePlayerClosenessTurn;
	int m_iCachePlayerClosenessDistance;
	int* m_aiPlayerCloseness;

	int m_iNeededFloatingDefenders;
	int m_iNeededFloatingDefendersCacheTurn;

	int m_iWorkersNeeded;
	int m_iWorkersHave;

	int	m_iBuildPriority;
	int	m_iTempBuildPriority;
	bool m_bRequestedUnit;
	bool m_bRequestedBuilding;

	void AI_doDraft(bool bForce = false);
	void AI_doHurry(bool bForce = false);
	void AI_doEmphasize();
	int AI_getHappyFromHurry(HurryTypes eHurry) const;
	int AI_getHappyFromHurry(HurryTypes eHurry, UnitTypes eUnit) const;
	int AI_getHappyFromHurry(HurryTypes eHurry, BuildingTypes eBuilding) const;
	int AI_getHappyFromHurry(int iHurryPopulation) const;
	bool AI_doPanic();
	int AI_calculateCulturePressure(bool bGreatWork = false) const;

	bool AI_chooseUnit(const char* reason, UnitAITypes eUnitAI /*= NO_UNITAI*/, int iOdds = -1, int iUnitStrength = -1, int iPriorityOverride = -1, const CvUnitSelectionCriteria* criteria = NULL);
	bool AI_chooseUnitImmediate(const char* reason, UnitAITypes eUnitAI, const CvUnitSelectionCriteria* criteria = NULL, UnitTypes eUnitType = NO_UNIT);
	bool AI_chooseUnit(UnitTypes eUnit, UnitAITypes eUnitAI);
	bool AI_chooseDefender(const char* reason);
	bool AI_chooseLeastRepresentedUnit(const char* reason, UnitTypeWeightArray &allowedTypes, int iOdds = -1);
	bool AI_chooseBuilding(int iFocusFlags = 0, int iMaxTurns = MAX_INT, int iMinThreshold = 0, int iOdds = -1, bool bMaximizePerTurnValue = false, PropertyTypes eProperty = NO_PROPERTY);
	bool AI_chooseExperienceBuilding(const UnitAITypes eUnitAI, const int iUnitProductionLossesFactor);
	bool AI_choosePropertyControlUnit(int iTriggerPercentOfPropertyOpRange, PropertyTypes pProperty = NO_PROPERTY);
	bool AI_chooseHealerUnit(int iMinNeeded = 3);
	bool AI_chooseProject();
	bool AI_chooseProcess(CommerceTypes eCommerceType = NO_COMMERCE, int64_t* commerceWeights = NULL);

	int	getPlayerDangerPercentage(PlayerTypes ePlayer, int& iModifier) const;

	bool AI_bestSpreadUnit(bool bMissionary, bool bExecutive, int iBaseChance, UnitTypes* eBestSpreadUnit, int* iBestSpreadUnitValue) const;
	bool AI_addBestCitizen(bool bWorkers, bool bSpecialists, int* piBestPlot = NULL, SpecialistTypes* peBestSpecialist = NULL);
	bool AI_removeWorstCitizen(SpecialistTypes eIgnoreSpecialist = NO_SPECIALIST);
	void AI_juggleCitizens();

	bool AI_potentialPlot(short* piYields) const;
	bool AI_foodAvailable(int iExtra = 0) const;

	int AI_buildingYieldValue(YieldTypes eYield, BuildingTypes eBuilding, const CvBuildingInfo& kBuilding, bool bForeignTrade, int iFreeSpecialistYield) const;

	void AI_getCurrentPlotValue(int iPlotCounter, const CvPlot* plot, std::vector<plotInfo>& currentYieldList) const;
	void AI_getBestPlotValue(const std::vector<int>& ratios, int iPlotCounter, const CvPlot* plot, std::vector<plotInfo>& optimalYieldList, int iDesiredFoodChange) const;
	void AI_newbestPlotBuild(const CvPlot* pPlot, plotInfo* plotInfo, int iFoodPriority, int iProductionPriority, int iCommercePriority) const;
	const std::vector<int> AI_calculateOutputRatio(int food, int production, int commerce) const;
	BuildTypes GetShortestBuildTimeOnPlot(const CvPlot* plot) const;

#ifdef YIELD_VALUE_CACHING
	virtual void AI_NoteWorkerChange();
	virtual void AI_NoteSpecialistChange();
#endif
	int AI_yieldValueInternal(short* piYields, short* piCommerceYields, bool bAvoidGrowth, bool bRemove, bool bIgnoreFood = false, bool bIgnoreGrowth = false, bool bIgnoreStarvation = false, bool bWorkerOptimization = false) const;
	int AI_plotValue(const CvPlot* pPlot, bool bAvoidGrowth, bool bRemove, bool bIgnoreFood = false, bool bIgnoreGrowth = false, bool bIgnoreStarvation = false) const;

	int AI_experienceWeight() const;
	int AI_buildUnitProb() const;
	bool AI_checkIrrigationSpread(const CvPlot* pPlot) const;

	void AI_bestPlotBuild(CvPlot* pPlot, int& piBestValue, BuildTypes& peBestBuild, int iFoodPriority, int iProductionPriority, int
	                      iCommercePriority, bool bChop, int iHappyAdjust, int iHealthAdjust, int iFoodChange);

	int AI_getImprovementValue(const CvPlot* pPlot, ImprovementTypes eImprovement, int iFoodPriority, int iProductionPriority, int iCommercePriority, int iFoodChange) const;
	void AI_getYieldMultipliers(int &iFoodMultiplier, int &iProductionMultiplier, int &iCommerceMultiplier, int &iDesiredFoodChange) const;

	int tradeRouteValue(const CvBuildingInfo& kBuilding, YieldTypes eYield, bool bForeignTrade) const;

	void AI_buildGovernorChooseProduction();

	int AI_getYieldMagicValue(const int* piYieldsTimes100, bool bHealthy) const;
	int AI_getPlotMagicValue(const CvPlot* pPlot, bool bHealthy, bool bWorkerOptimization = false) const;
	int AI_countGoodTiles(bool bHealthy, bool bUnworkedOnly, int iThreshold = 50, bool bWorkerOptimization = false) const;
	int AI_countGoodSpecialists(bool bHealthy) const;
	//int AI_calculateTargetCulturePerTurn() const;

	void AI_stealPlots();

	void AI_cachePlayerCloseness(int iMaxDistance);
	void AI_updateWorkersNeededHere();

	// added so under cheat mode we can call protected functions for testing
	friend class CvGameTextMgr;

public:
	//	KOSHLING MOD - pre-calculate building values for all buildings/focuses
	void AI_FlushBuildingValueCache(bool bRetainValues = false);
	int getPropertySourceValue(PropertyTypes eProperty, int iSourceValue) const;
	int getPropertyDecay(PropertyTypes eProperty) const;
	int getPropertyNonBuildingSource(PropertyTypes eProperty) const;

private:
	int	GetBuildingValue(BuildingTypes eType, int iFocusFlags, int threshold, bool bMaximizeFlaggedValue, bool bIgnoreCanConstruct = false);
	bool buildingMayHaveAnyValue(BuildingTypes eBuilding, int iFocusFlags) const;
	void CalculateAllBuildingValues(int iFocusFlags);
	int happynessValue(int iAddedHappyness, int iBaseHappinessLevel, int iBaseHealthLevel) const;
	int healthValue(int iAddedHealth, int iUseHappinessLevel, int iBaseHealthLevel, int iBaseFoodDifference) const;
	int worstWorkedPlotValue() const;
	//	Evaluate a building we are considering building here in terms of its
	//	effect on properties
	int buildingPropertiesValue(const CvBuildingInfo& kBuilding) const;

	bool AI_establishSeeInvisibleCoverage();
	bool AI_establishInvestigatorCoverage();
	bool AI_isNegativePropertyUnit(UnitTypes eUnit) const;
	bool AI_meetsUnitSelectionCriteria(UnitTypes eUnit, const CvUnitSelectionCriteria* criteria = NULL);

	//	The cache itself
	BuildingValueCache* cachedBuildingValues;

#ifdef YIELD_VALUE_CACHING
	virtual void ClearYieldValueCache();
	void ClearYieldValueCacheImpl();
	virtual void CheckYieldValueCache(char* label);

	mutable yieldCache yieldValueCache;
	static int yieldValueCacheHits;
	static int yieldValueCacheReads;
#endif

	int m_neededDefenders;	//	Cached at the start of unit mission calculations each turn
	int m_neededDefendersVicinity;
	int m_requestedEscorts;

	int m_iCityThreat;
	int m_iCityThreatModifier;

	std::map<int, UnitValueInfo> m_bestUnits;

	std::map<int, int> m_buildValueCache;
};

#endif
