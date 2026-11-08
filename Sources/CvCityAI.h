#pragma once

// cityAI.h

#ifndef CIV4_CITY_AI_H
#define CIV4_CITY_AI_H

#include <vector>

#include "CvCity.h"
#include "OutputRatios.h"
#include "PlotInfo.h"
#include "CvEnums.h"

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

#define MAX_REQUESTEDUNIT_PER_CITY 1
#define MAX_REQUESTEDBUILDING_PER_CITY 2
#define NB_MAX_BUILDINGS 2


class BuildingValueCache; // KOSHLING Mod - pre-calculate and cache building values for all focuses
class CvArea;
class CvUnitSelectionCriteria;

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

/**
 * @class CvCityAI
 * @brief AI logic for city management in Caveman2Cosmos.
 *
 * CvCityAI extends CvCity to provide advanced AI routines for city management, including:
 * - Assignment of citizens to plots and specialists, with optimization for yields and growth.
 * - Automated production selection, including units, buildings, projects, and processes, based on city and player strategy.
 * - Evaluation and scoring of buildings, units, and improvements using multiple focus flags and thresholds.
 * - Management of city defense, including calculation of needed defenders, air defense, and floating defenders.
 * - Calculation of city importance for economy, military, and victory strategies.
 * - Handling of property effects (crime, pollution, etc.) and property control units/buildings.
 * - Caching and optimization of yield and building value calculations for performance.
 * - Support for governor automation, emphasis settings, and special AI strategies (e.g., culture, conquest, turtle).
 * - Advanced routines for worker assignment, improvement selection, and plot value evaluation.
 * - Integration with global AI strategies, player traits, and game options.
 *
 * The class contains many methods whose implementations are complex (often >20 lines), including:
 * - AI_chooseProduction(), AI_assignWorkingPlots(), AI_specialistValue(), AI_bestUnit(), AI_bestUnitAI(), AI_bestBuildingsThreshold(), AI_scoreBuildingsFromListThreshold(), AI_buildingValue(), AI_buildingValueThreshold(), AI_buildingValueThresholdOriginal(), AI_buildingValueThresholdOriginalUncached(), AI_updateBestBuild(), AI_updateWorkersNeededHere(), AI_yieldValue(), AI_yieldValueInternal(), AI_plotValue(), AI_findBestImprovementForPlot(), AI_countGoodSpecialists(), AI_getCityImportance(), AI_getYieldMultipliers(), AI_getImprovementValue(), AI_bestAdvancedStartBuilding(), AI_getGoodTileCount(), AI_countWorkedPoorTiles(), AI_getTargetSize(), AI_getBestBuild(), AI_countBestBuilds(), AI_calculateWaterWorldPercent(), AI_yieldMultiplier(), AI_updateSpecialYieldMultiplier(), AI_specialYieldMultiplier(), AI_countNumBonuses(), AI_countNumImprovableBonuses(), AI_getPlotMagicValue(), AI_countGoodTiles(), AI_calculateTargetCulturePerTurn(), AI_stealPlots(), AI_cachePlayerCloseness(), AI_FlushBuildingValueCache(), getPropertySourceValue(), getPropertyDecay(), getPropertyNonBuildingSource(), AI_getBuildingYieldValue(), AI_getBuildingProductionValue(), AI_getBuildingCommerceValue(), buildingMayHaveAnyValue(), CalculateAllBuildingValues(), happynessValue(), healthValue(), worstWorkedPlotValue(), buildingPropertiesValue(), AI_establishSeeInvisibleCoverage(), AI_establishInvestigatorCoverage(), AI_isNegativePropertyUnit(), AI_meetsUnitSelectionCriteria().
 *
 * @note
 * - Many methods use internal caching and are optimized for performance.
 * - The class is tightly integrated with other AI systems (CvPlayerAI, CvTeamAI, CvContractBroker, etc.).
 * - Designed for extensibility and modding, with support for new properties, yields, and AI strategies.
 */
class CvCityAI : public CvCity
{

public:

	CvCityAI();
	virtual ~CvCityAI();

	void AI_init();
	void AI_uninit();
	void AI_reset();
	void SendLog(CvWString function, CvWString message) const;

	/**
	 * Performs the main AI logic for the city at the start of each turn.
	 *
	 * Main steps:
	 * 1. **Update Assignments**  
	 *    - Calls AI_updateAssignWork() to ensure citizens and specialists are assigned optimally.
	 * 2. **Handle Floating Defenders**  
	 *    - Calls AI_doContractFloatingDefenders() to manage city defense contracts.
	 * 3. **Production and Worker Management**  
	 *    - Calls AI_chooseProduction() to select what the city should build next.
	 *    - Updates worker needs and best build values for city plots.
	 * 4. **Property and Threat Updates**  
	 *    - Updates property values (crime, pollution, etc.) and city threat levels.
	 * 5. **Governor and Emphasis Handling**  
	 *    - Handles governor automation and city emphasis flags.
	 * 6. **Miscellaneous AI Tasks**  
	 *    - May include caching, logging, and other per-turn AI routines.
	 *
	 * @note
	 * - This method is called once per city per turn and coordinates all city-level AI decisions.
	 *
	 * @dot
	 * digraph AI_doTurn {
	 *   node [shape=box];
	 *   UpdateAssignments -> HandleDefenders -> ProductionWorkerMgmt -> PropertyThreatUpdates -> GovernorEmphasis -> MiscAITasks;
	 * }
	 * @enddot
	 */
	void AI_doTurn();

	/**
	 * Assigns working plots and specialists for the city.
	 * - Updates special yield multipliers.
	 * - Removes invalid plots and excess specialists.
	 * - Ensures home plot is always worked.
	 * - Removes worst citizens if over population limit.
	 * - Assigns unassigned population to plots or specialists.
	 * - If automated, optimizes assignments for best yields.
	 * - Updates UI if city is selected.
	 */
	void AI_assignWorkingPlots();

	/**
	 * Updates plot and specialist assignments if marked dirty.
	 * - Avoids changes if human player is in city screen.
	 * - Calls AI_assignWorkingPlots if needed.
	 */
	void AI_updateAssignWork();

	/**
	 * Handles contract requests for floating defenders in the city.
	 * - Evaluates city threat and defense needs.
	 * - Requests units from the contract broker if necessary.
	 * - Updates internal defender counters.
	 */
	void AI_doContractFloatingDefenders();

	/**
	 * Determines if the city should avoid growth.
	 * - Checks food production, human control, and emphasize flags.
	 * - Considers angry/unhealthy citizens and happiness/health levels.
	 * - Returns true if growth should be avoided.
	 */
	bool AI_avoidGrowth();

	/**
	 * Determines if the city should ignore growth.
	 * - Returns true if not emphasizing food or great people and food is not available.
	 */
	bool AI_ignoreGrowth();

	/**
	 * Calculates the value of a specialist for this city.
	 * - Computes yield and commerce from the specialist.
	 * - Adds value for great people points, scaling with civics and era.
	 * - Considers experience, health, happiness, and property effects.
	 * - Evaluates investigation and insidiousness for crime mechanics.
	 * - Adjusts value if the specialist is emphasized.
	 * - Returns a weighted score for use in citizen assignment.
	 */
	int AI_specialistValue(SpecialistTypes eSpecialist, bool bAvoidGrowth, bool bRemove) const;

	//	KOSHLING - initialisation called on every city prior to performing unit mission allocation logic
	//	This allows caches that will remain valid for the processing of the current turn's units to be cleared
	virtual void AI_preUnitTurn();

	/**
	 * Called when a unit is trained in the city.
	 * - Updates internal AI state for the trained unit.
	 * - May trigger additional AI routines depending on unit type.
	 */
	virtual void AI_trained(UnitTypes eUnitType, UnitAITypes eUnitAIType);

	/**
	 * Main AI production decision for the city.
	 * - Checks for anarchy, occupation, and danger states.
	 * - Clears production queue if needed.
	 * - Evaluates city status: war, economy, happiness, health, area type, etc.
	 * - Sets up flags for economic, military, and cultural priorities.
	 * - Handles special cases (barbarian, rebel, occupation, strike).
	 * - Prioritizes emergency needs (defense, happiness, health, property control).
	 * - Assigns workers, settlers, escorts, and special units as needed.
	 * - Evaluates and chooses buildings, units, projects, or processes based on city and player strategy.
	 * - Uses a priority system to order choices and fallback to processes if nothing else is suitable.
	 * - Contains many conditional branches for different game states and strategies.
	 */
	void AI_chooseProduction();

	/**
	 * Determines the best unit to build in the city.
	 * - Initializes weighting for each UnitAI type based on area, war status, and city needs.
	 * - Adjusts weights for military, worker, settler, and special units.
	 * - Multiplies weights for urgency (danger, war, etc.).
	 * - Iterates over all unit types, checking if they can be trained and meet criteria.
	 * - Selects the unit with the highest weighted value.
	 * - Optionally caches results for performance.
	 * - Returns the best unit type for current priorities.
	 */
	UnitTypes AI_bestUnit(int& iBestValue, int iNumSelectableTypes = -1, UnitAITypes* pSelectableTypes = NULL, bool bAsync = false, UnitAITypes* peBestUnitAI = NULL, bool bNoRand = false, bool bNoWeighting = false, const CvUnitSelectionCriteria* criteria = NULL);

	/**
	 * Determines the best unit to build for a specific UnitAI type.
	 * - Sets up selection criteria and checks cache for previous results.
	 * - Iterates over all units, filtering by criteria and trainability.
	 * - Scores each unit using player AI logic.
	 * - Selects the unit with the highest score.
	 * - Optionally caches the result for future calls.
	 * - Returns the best unit type for the given AI role.
	 */
	UnitTypes AI_bestUnitAI(UnitAITypes eUnitAI, int& iBestValue, bool bAsync = false, bool bNoRand = false, const CvUnitSelectionCriteria* criteria = NULL);

	BuildingTypes AI_bestBuildingThreshold(int iFocusFlags = 0, int iMaxTurns = MAX_INT, int iMinThreshold = 0, bool bAsync = false, AdvisorTypes eIgnoreAdvisor = NO_ADVISOR, bool bMaximizeFlaggedValue = false, PropertyTypes eProperty = NO_PROPERTY);
	
	/**
	 * Returns a list of the best buildings to construct, filtered by thresholds.
	 * - Gathers all possible buildings not maxed out for the player.
	 * - Scores each building using focus flags, turns, and thresholds.
	 * - Sorts the list by score, highest first.
	 * - Returns the sorted list for use in building selection.
	 */
	const std::vector<ScoredBuilding> AI_bestBuildingsThreshold(int iFocusFlags, int iMaxTurns, int iMinThreshold, bool bAsync, AdvisorTypes eIgnoreAdvisor, bool bMaximizeFlaggedValue, PropertyTypes eProperty);

	/**
	 * Scores a list of buildings for construction, using thresholds and focus flags.
	 * - For each building, checks if it can be constructed and matches advisor/focus.
	 * - Calculates a score based on city status, focus, and other parameters.
	 * - Adds scored buildings to the output vector.
	 * - Sorts the results by score.
	 * - Returns true if any buildings were scored.
	 */
	bool AI_scoreBuildingsFromListThreshold(std::vector<ScoredBuilding>& scoredBuildings, const std::vector<BuildingTypes>& possibles, int iFocusFlags, int iMaxTurns, int iMinThreshold, bool bAsync, AdvisorTypes eIgnoreAdvisor, bool bMaximizeFlaggedValue, PropertyTypes eProperty);

	/// Can a building be reasonably hurried by any method?
	bool AI_canRushBuildingConstruction(BuildingTypes building) const;

	/**
	 * Calculates the value of a building for the city, considering focus flags and thresholds.
	 * - Checks if the building can be constructed and if it is cached.
	 * - Uses cached values if available, otherwise computes from scratch.
	 * - Evaluates yields, commerce, happiness, health, and special effects.
	 * - Applies leader flavor modifiers for AI players.
	 * - Returns a weighted value for building selection.
	 */
	int AI_buildingValue(BuildingTypes eBuilding, int iFocusFlags = 0, bool bForTech = false, bool bDebug = false);

	/**
	 * Calculates the value of a building for the city, using focus flags and a threshold.
	 * - Checks cache for precomputed values.
	 * - If not cached, computes value and updates cache.
	 * - Evaluates building effects, yields, commerce, and modifiers.
	 * - Returns the computed value for building selection.
	 */
	int AI_buildingValueThreshold(BuildingTypes eBuilding, int iFocusFlags = 0, int iThreshold = 0, bool bMaximizeFlaggedValue = false, bool bIgnoreCanConstruct = false);

	/**
	 * Calculates the value of a building for the city, using original uncached logic.
	 * - Evaluates happiness, health, food, commerce, and other modifiers.
	 * - Considers population, area, and victory strategies.
	 * - Computes value in two passes for focus flags and general effects.
	 * - Returns the final weighted value.
	 */
	int AI_buildingValueThresholdOriginal(BuildingTypes eBuilding, int iFocusFlags = 0, int iThreshold = 0, bool bMaximizeFlaggedValue = false, bool bIgnoreCanBuildReplacement = false, bool bForTech = false);

	/**
	 * Calculates the value of a building for the city, without using cache.
	 * - Evaluates all effects: yields, commerce, happiness, health, specialists, and modifiers.
	 * - Considers population, area, and victory strategies.
	 * - Returns the computed value for building selection.
	 */
	int AI_buildingValueThresholdOriginalUncached(BuildingTypes eBuilding, int iFocusFlags = 0, int iThreshold = 0, bool bMaximizeFlaggedValue = false, bool bIgnoreCanBuildReplacement = false, bool bForTech = false);

	/**
	 * Returns the commerce value of a building for a specific commerce type.
	 * - Evaluates base commerce, modifiers, bonuses, and global effects.
	 * - Considers leader flavor and victory strategies.
	 * - Returns the weighted commerce value for building selection.
	 */
	int getBuildingCommerceValue(BuildingTypes eBuilding, int iI, int* aiFreeSpecialistYield, int* aiFreeSpecialistCommerce, int* aiBaseCommerceRate, int* aiPlayerCommerceRate) const;

	/**
	 * Calculates the best project to build in the city.
	 * - Iterates over all available projects.
	 * - Evaluates project value based on global effects, victory conditions, and modifiers.
	 * - Returns the best project type.
	 */
	ProjectTypes AI_bestProject() const;

	/**
	 * Calculates the value of a project for the city.
	 * - Evaluates nuke interception, tech sharing, victory conditions, happiness, health, and commerce modifiers.
	 * - Considers global and world effects.
	 * - Returns the weighted value for project selection.
	 */
	int AI_projectValue(ProjectTypes eProject) const;

	/**
	 * Calculates the best process to run in the city for a given commerce type.
	 * - Iterates over all available processes.
	 * - Evaluates value based on commerce conversion and weights.
	 * - Returns the best process type.
	 */
	ProcessTypes AI_bestProcess(CommerceTypes eCommerceType = NO_COMMERCE, int64_t* commerceWeights = NULL) const;

	/**
	 * Calculates the value of a process for the city.
	 * - Evaluates conversion rates for each commerce type.
	 * - Applies player commerce weights and average exchange rates.
	 * - Returns the weighted value for process selection.
	 */
	int64_t AI_processValue(ProcessTypes eProcess, CommerceTypes eCommerceType = NO_COMMERCE, int64_t* commerceWeights = NULL) const;

	/**
	 * Calculates the number of sea workers needed for the city.
	 * - Checks water areas for unimproved bonuses.
	 * - Sums up needed workers for all relevant water areas.
	 */
	int AI_neededSeaWorkers() const;

	bool AI_isDefended(int iExtra = 0, bool bAllowAnyDefenders = true);
	bool AI_isAirDefended(bool bCountLand = false, int iExtra = 0);
	bool AI_isDanger() const;
	int evaluateDanger();

	bool AI_isAdequateHappinessMilitary(int iExtra) const;
	int AI_neededHappinessDefenders() const;
	
	/**
	 * Calculates the number of defenders needed for the city.
	 * - Considers population, wonders, holy city status, war strategies, and recent acquisition.
	 * - Adjusts for happiness, revolution, and victory strategies.
	 * - Returns the required number of defenders.
	 */
	int AI_neededDefenders();

	/**
	 * Calculates the number of air defenders needed for the city.
	 * - Checks nearby plots for enemy and other teams.
	 * - Adjusts for coastal cities.
	 * - Returns the required number of air defenders.
	 */
	int AI_neededAirDefenders() const;
	int AI_minDefenders() const;
	int AI_neededFloatingDefenders();
	void AI_updateNeededFloatingDefenders();

	int getGarrisonStrength(bool bAllowAnyDefenders = true) const;
	int AI_neededDefenseStrength();
	int AI_minDefenseStrength() const;

	int AI_getEmphasizeAvoidGrowthCount() const;

	/**
	 * Checks if the city is emphasizing avoidance of growth.
	 * - Returns true if avoid growth is emphasized.
	 */
	bool AI_isEmphasizeAvoidGrowth() const;

	/**
	 * Returns the count of emphasize great people flags.
	 * - Counts the number of times great people are emphasized.
	 */
	int AI_getEmphasizeGreatPeopleCount() const;
	

	/**
	 * Checks if the city is emphasizing great people.
	 * - Returns true if great people are emphasized.
	 */
	bool AI_isEmphasizeGreatPeople() const;

	/**
	 * Checks if plot assignment is marked dirty.
	 * - Returns true if assignments need updating.
	 */
	bool AI_isAssignWorkDirty() const;

	/**
	 * Sets the dirty flag for plot assignment.
	 * - Marks assignments as needing update.
	 */
	void AI_setAssignWorkDirty(bool bNewValue);

	/**
	 * Checks if production selection is marked dirty.
	 * - Returns true if production needs updating.
	 */
	bool AI_isChooseProductionDirty() const;

	/**
	 * Sets the dirty flag for production selection.
	 * - Marks production as needing update.
	 */
	void AI_setChooseProductionDirty(bool bNewValue);

	/**
	 * Returns the city to which a route is being established.
	 * - Used for AI route planning.
	 */
	CvCity* AI_getRouteToCity() const;

	/**
	 * Updates the route to another city.
	 * - Recalculates optimal route for trade or movement.
	 */
	void AI_updateRouteToCity() const;

	/**
	 * Checks if the city is emphasizing a specific yield type.
	 * - Returns true if the yield is emphasized.
	 */
	bool AI_isEmphasizeYield(const YieldTypes eIndex) const;

	/**
	 * Checks if the city is emphasizing a specific commerce type.
	 * - Returns true if the commerce is emphasized.
	 */
	bool AI_isEmphasizeCommerce(const CommerceTypes eIndex) const;

	/**
	 * Checks if any commerce or yield emphasis is active.
	 * - Returns true if any emphasis is set.
	 */
	bool AI_isAnyCommerceOrYieldEmphasis() const;

	/**
	 * Checks if the city is emphasizing a specific type.
	 * - Returns true if the emphasis is set.
	 */
	bool AI_isEmphasize(EmphasizeTypes eIndex) const;

	/**
	 * Sets emphasis for a specific type.
	 * - Updates internal flags for emphasis.
	 */
	void AI_setEmphasize(EmphasizeTypes eIndex, bool bNewValue);

	/**
	 * Checks if the city is emphasizing a specific specialist type.
	 * - Returns true if the specialist is emphasized.
	 */
	bool AI_isEmphasizeSpecialist(SpecialistTypes eIndex) const;

	/**
	 * Sets emphasis for a specific specialist type.
	 * - Updates internal flags for specialist emphasis.
	 */
	void AI_setEmphasizeSpecialist(SpecialistTypes eIndex, bool bNewValue);
	void AI_forceEmphasizeCulture(bool bNewValue);

	void AI_markBestBuildValuesStale();
	int AI_getBestBuildValue(int iIndex) const;
	int AI_totalBestBuildValue(const CvArea* pArea) const;

	int AI_clearFeatureValue(int iIndex) const;

	/**
	 * Returns the number of good tiles for working in the city.
	 * - Iterates over city plots.
	 * - Counts plots with sufficient yields and not being worked.
	 */
	int AI_getGoodTileCount() const;

	/**
	 * Returns the number of worked poor tiles in the city.
	 * - Iterates over city plots.
	 * - Counts worked plots with low yields.
	 */
	int AI_countWorkedPoorTiles() const;

	/**
	 * Calculates the target city size based on good tiles, happiness, and health.
	 * - Adjusts for current production, happiness, health, and population.
	 * - Considers advanced start points and other modifiers.
	 */
	int AI_getTargetSize() const;

	/**
	 * Returns the best build type for a given city plot.
	 * - Uses cached values from previous evaluations.
	 */
	BuildTypes AI_getBestBuild(int iIndex) const;

	/**
	 * Counts the number of best builds in the area.
	 * - Iterates over city plots in the area.
	 * - Counts plots with a valid best build assigned.
	 */
	int AI_countBestBuilds(const CvArea* pArea) const;

	/**
	 * Updates the best build values for all city plots.
	 * - Calculates optimal improvements for each plot.
	 * - Updates cached best build and value arrays.
	 */
	void AI_updateBestBuild();

	virtual int AI_cityValue() const;

	/**
	 * Calculates the percentage of water world cities.
	 * - Counts team and other cities.
	 * - Adjusts percentage based on total cities and team cities.
	 */
	int AI_calculateWaterWorldPercent() const;

	/**
	 * Calculates the importance of the city for economy or military.
	 * - Considers victory strategies and culture rate rank.
	 * - Returns a value indicating city importance.
	 */
	int AI_getCityImportance(bool bEconomy, bool bMilitary) const;

	/**
	 * Calculates the yield value for a set of yields and commerce.
	 * - Uses caching for performance.
	 * - Considers growth, removal, food, starvation, and worker optimization.
	 * - Returns a weighted value for the yields.
	 */
	int AI_yieldValue(short* piYields, short* piCommerceYields, bool bAvoidGrowth, bool bRemove, bool bIgnoreFood = false, bool bIgnoreGrowth = false, bool bIgnoreStarvation = false, bool bWorkerOptimization = false) const;

	/**
	 * Calculates the yield multiplier for a given yield type.
	 * - Considers production, commerce, and modifiers from buildings and civics.
	 */
	int AI_yieldMultiplier(YieldTypes eYield) const;

	/**
	 * Updates special yield multipliers based on current production.
	 * - Adjusts multipliers for workers, settlers, wonders, and other priorities.
	 */
	void AI_updateSpecialYieldMultiplier();

	/**
	 * Returns the special yield multiplier for a given yield type.
	 */
	int AI_specialYieldMultiplier(YieldTypes eYield) const;

	/**
	 * Counts the number of bonuses in the city area.
	 * - Iterates over city plots.
	 * - Counts bonuses based on ownership, neutrality, and land/water.
	 */
	int AI_countNumBonuses(BonusTypes eBonus, bool bIncludeOurs, bool bIncludeNeutral, int iOtherCultureThreshold, bool bLand = true, bool bWater = true) const;
	int AI_countNumImprovableBonuses(bool bIncludeNeutral, TechTypes eExtraTech = NO_TECH, bool bLand = true, bool bWater = false) const;

	int AI_playerCloseness(PlayerTypes eIndex, int iMaxDistance);
	int AI_cityThreat(TeamTypes eTargetTeam = NO_TEAM, int* piThreatModifier = NULL);

	int AI_getWorkersNeeded() const { return m_iWorkersNeeded; }

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

	int	m_iBuildPriority;
	int	m_iTempBuildPriority;
	int m_iRequestedUnit;
	int m_iRequestedBuilding;

	void AI_doDraft();
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
	bool AI_choosePropertyControlBuildingAndUnit(int iTriggerPercentOfPropertyOpRange, PropertyTypes pProperty = NO_PROPERTY, int eMaxPropertyUnitsPercent = 20);
	bool AI_chooseHealerUnit(int iMinNeeded = 3);
	bool AI_chooseProject();
	bool AI_chooseProcess(CommerceTypes eCommerceType = NO_COMMERCE, int64_t* commerceWeights = NULL, bool bforce = false);

	int	getPlayerDangerPercentage(PlayerTypes ePlayer, int& iModifier) const;

	bool AI_bestSpreadUnit(bool bMissionary, bool bExecutive, int iBaseChance, UnitTypes* eBestSpreadUnit, int* iBestSpreadUnitValue) const;
	bool AI_addBestCitizen(bool bWorkers, bool bSpecialists, int* piBestPlot = NULL, SpecialistTypes* peBestSpecialist = NULL);
	bool AI_removeWorstCitizen(SpecialistTypes eIgnoreSpecialist = NO_SPECIALIST);
	void AI_juggleCitizens();

	bool AI_potentialPlot(short* piYields) const;
	bool AI_foodAvailable(int iExtra = 0) const;

	int AI_buildingYieldValue(YieldTypes eYield, BuildingTypes eBuilding, const CvBuildingInfo& kBuilding, bool bForeignTrade, int iFreeSpecialistYield) const;

	void AI_getBestPlotValue(OutputRatios& ratios, int iPlotCounter, const CvPlot* plot, std::vector<plotInfo>& optimalYieldList) const;

	int AI_evaluateMaxUnitSpending() const;

	/**
	 * Calculates the best improvement for a plot.
	 * - Checks for forced builds and current improvements.
	 * - Evaluates all possible improvements and builds.
	 * - Considers yields, bonuses, and feature removal.
	 * - Selects the improvement and build with the highest value.
	 */
	void AI_findBestImprovementForPlot(const CvPlot* pPlot, plotInfo* plotInfo, OutputRatios& ratios) const;

	std::vector<int> AI_calculateOutputRatio(int food, int production, int commerce) const;

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

	int AI_getImprovementValue(const CvPlot* pPlot, ImprovementTypes eImprovement, int iFoodPriority, int iProductionPriority, int iCommercePriority, int iFoodChange) const;
	void AI_getYieldMultipliers(int &iFoodMultiplier, int &iProductionMultiplier, int &iCommerceMultiplier, int &iDesiredFoodChange) const;

	int tradeRouteValue(const CvBuildingInfo& kBuilding, YieldTypes eYield, bool bForeignTrade) const;

	void AI_buildGovernorChooseProduction();

	int AI_getPlotMagicValue(const CvPlot* pPlot, bool bHealthy, bool bWorkerOptimization = false) const;
	int AI_countGoodTiles(bool bHealthy, bool bUnworkedOnly, int iThreshold = 50, bool bWorkerOptimization = false) const;
	
	/**
	 * Calculates the number of good specialists in the city.
	 * - Iterates over all specialist types.
	 * - Sums up specialists with high yield and commerce values.
	 * - Adjusts for health and happiness.
	 */	
	int AI_countGoodSpecialists(bool bHealthy) const;
	int AI_calculateTargetCulturePerTurn() const;

	void AI_stealPlots();

	void AI_cachePlayerCloseness(int iMaxDistance);
	void AI_updateWorkersNeededHere();

	// added so under cheat mode we can call protected functions for testing
	friend class CvGameTextMgr;

public:
	//	KOSHLING MOD - pre-calculate building values for all buildings/focuses
	/**
	 * Pre-calculates and flushes the building value cache for all buildings and focuses.
	 * - Clears or retains cached values depending on the parameter.
	 * - Ensures up-to-date building evaluations for city AI decisions.
	 */
	void AI_FlushBuildingValueCache(bool bRetainValues = false);

	/**
	 * Computes the property source value for a given property and source value.
	 * - Normalizes property change based on operational range.
	 * - Scales value according to city, area, player, or team importance.
	 * - Adjusts for positive or negative property changes.
	 */
	int getPropertySourceValue(PropertyTypes eProperty, int iSourceValue) const;

	/**
	 * Calculates the decay rate of a property in the city.
	 * - Iterates over all property sources affecting decay.
	 * - Sums up decay contributions from features, buildings, and other sources.
	 */
	int getPropertyDecay(PropertyTypes eProperty) const;

	/**
	 * Computes the non-building source value for a property.
	 * - Sums up contributions from specialists, population, traits, and global sources.
	 * - Ignores building-based property sources.
	 */
	int getPropertyNonBuildingSource(PropertyTypes eProperty) const;

private:

	int AI_getBuildingYieldValue
	(
		BuildingTypes eBuilding, const CvBuildingInfo& kBuilding, bool bIsLimitedWonder, bool bForeignTrade, bool bFinancialTrouble,
		int* aiFreeSpecialistYield, int* aiYieldRank, int iLimitedWonderLimit, const CvArea* pArea, int iTotalPopulation, int iFoodDifference
	)
	const;

	int AI_getBuildingProductionValue
	(
		BuildingTypes eBuilding, const CvBuildingInfo& kBuilding, bool bIsLimitedWonder,
		bool bForeignTrade, int iFreeSpecialistYield, int iYieldRank, int iLimitedWonderLimit
	)
	const;

	int AI_getBuildingCommerceValue
	(
		BuildingTypes eBuilding, const CvBuildingInfo& kBuilding, bool bIsLimitedWonder, bool bFinancialTrouble, bool bCulturalVictory1,
		int* aiFreeSpecialistYield, int* aiFreeSpecialistCommerce, int* aiBaseCommerceRate, int* aiPlayerCommerceRate, int* aiCommerceRank,
		int iLimitedWonderLimit, int iTotalPopulation
	)
	const;

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
	bool AI_meetsUnitSelectionCriteria(UnitTypes eUnit, const CvUnitSelectionCriteria* criteria) const;

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

	int m_iCityThreat;
	int m_iCityThreatModifier;

	std::map<int, UnitValueInfo> m_bestUnits;

	std::map<int, int> m_buildValueCache;
};

#endif
