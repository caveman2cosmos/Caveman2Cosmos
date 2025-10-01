#pragma once

// playerAI.h

#ifndef CIV4_PLAYER_AI_H
#define CIV4_PLAYER_AI_H

#include "FProfiler.h"

#include "CvPlayer.h"
#include "AI_defines.h"

class CvArea;
class CvCity;
class CvEventTriggerInfo;
class CvSelectionGroup;
class CvUnitSelectionCriteria;
class CvArmy;

/**
 * Stores information about a mission target for AI planning.
 * - Tracks number of units targeting, closest distance, and total volume.
 */
struct MissionTargetInfo
{
	MissionTargetInfo() : iCount(0), iClosest(0), iVolume(0) {}

	int	iCount;		//	Num units targeting the mission target
	int iClosest;	//	How close is the closest
	int iVolume;    //  How much volume among the units counted
};

//	Koshling - add caching to plot danger calculations
#define PLOT_DANGER_CACHING
#ifdef PLOT_DANGER_CACHING
/**
 * Caches plot danger calculations for performance.
 * - Stores coordinates, range, move test flag, result, and last use count.
 */
struct plotDangerCacheEntry
{
	plotDangerCacheEntry()
		: plotX(-1)
		, plotY(-1)
		, iRange(-1)
		, bTestMoves(false)
		, iResult(-1)
		, iLastUseCount(0)
	{}

	int plotX;
	int plotY;
	int iRange;
	bool bTestMoves;
	int iResult;
	int iLastUseCount;
};

#define PLOT_DANGER_CACHE_SIZE 24

/**
 * Manages a fixed-size cache of plot danger entries.
 * - Provides clear method and tracks usage counter.
 */
class plotDangerCache
{
public:
	plotDangerCache() : currentUseCounter(0) {}

	void clear()
	{
		PROFILE_EXTRA_FUNC();
		if ( currentUseCounter != 0 )
		{
			currentUseCounter = 0;

			foreach_(plotDangerCacheEntry& entry, entries)
			{
				entry.iLastUseCount = 0;
			}
		}
	}

	int currentUseCounter;
	struct plotDangerCacheEntry entries[PLOT_DANGER_CACHE_SIZE];
};
#endif

/**
 * AI player logic for managing all aspects of a civilization.
 * - Handles turn-based decisions, unit and city management, diplomacy, research, and victory strategies.
 * - Contains routines for evaluating yields, commerce, military, city sites, attitudes, and more.
 * - Integrates with city and unit AI, caching, and global game state.
 */
class CvPlayerAI : public CvPlayer
{

public:

	CvPlayerAI();
	virtual ~CvPlayerAI();

	// inlined for performance reasons
	static CvPlayerAI& getPlayer(PlayerTypes ePlayer)
	{
		FASSERT_BOUNDS(0, MAX_PLAYERS, ePlayer);
		return m_aPlayers[ePlayer];
	}

	DllExport static CvPlayerAI& getPlayerNonInl(PlayerTypes ePlayer);

	static void initStatics();
	static void freeStatics();
	DllExport static bool areStaticsInitialized();

	void AI_init();
	void AI_uninit();
	void AI_reset(bool bConstructor);

	int AI_getFlavorValue(FlavorTypes eFlavor) const;

	void AI_doTurnPre();
	void AI_doTurnPost();
	void AI_doTurnUnitsPre();
	void AI_doTurnUnitsPost();

	void AI_doPeace();

	void AI_updateFoundValues(bool bClear = false, const CvArea* area = NULL) const;
	void AI_updateAreaTargets();

	int AI_movementPriority(const CvSelectionGroup* pGroup) const;
	void AI_unitUpdate();

	void AI_makeAssignWorkDirty();
	
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
	void AI_updateAssignWork();

	void AI_makeProductionDirty();

	void AI_doCentralizedProduction();

	void AI_conquerCity(PlayerTypes eOldOwner, CvCity* pCity, bool bConquest, bool bTrade);

	bool AI_acceptUnit(const CvUnit* pUnit) const;

	DomainTypes AI_unitAIDomainType(UnitAITypes eUnitAI) const;

	int AI_yieldWeight(YieldTypes eYield) const;
	int AI_commerceWeight(CommerceTypes eCommerce, const CvCity* pCity = NULL) const;

	int AI_foundValue(int iX, int iY, int iMinRivalRange = -1, bool bStartingLoc = false) const;

	bool AI_isAreaAlone(const CvArea* pArea) const;
	bool AI_isCapitalAreaAlone() const;
	bool AI_isPrimaryArea(const CvArea* pArea) const;

	int AI_militaryWeight(const CvArea* pArea) const;

	int AI_targetCityValue(const CvCity* pCity, bool bRandomize, bool bIgnoreAttackers = false) const;
	CvCity* AI_findTargetCity(const CvArea* pArea) const;

	bool AI_isCommercePlot(const CvPlot* pPlot) const;

	int AI_plotDangerUnitCheck
	(
		const CvPlot* plot0, const CvPlot* plotX,
		const CvUnit* unitX, const TeamTypes eTeam,
		const int iDistance = 0, const bool bTestMoves = false
	)
	const;
	bool AI_getAnyPlotDanger(const CvPlot* pPlot, int iRange = -1, bool bTestMoves = true) const;
	bool AI_getVisiblePlotDanger(const CvPlot* pPlot, int iRange, bool bAnimalOnly, CvSelectionGroup* group = NULL, int acceptableOdds = -1) const;
	int AI_getPlotDanger(const CvPlot* pPlot, int iRange = -1, bool bTestMoves = true) const;
	//	Koshling - internal bypasses cache
	int AI_getPlotDangerInternal(const CvPlot* pPlot, int iRange, bool bTestMoves) const;

	int AI_getWaterDanger(const CvPlot* pPlot, int iRange) const;
	int AI_countNumLocalNavy(const CvPlot* pPlot, int iRange) const;

	bool AI_avoidScience() const;
	bool AI_hasCriticalGold() const;
	bool AI_isFinancialTrouble() const;
	short AI_fundingHealth(int iExtraExpense = 0, int iExtraExpenseMod = 0) const;
	short AI_safeFunding() const;
	int64_t AI_goldTarget() const;
	int AI_goldValueAssessmentModifier() const;

	TechTypes AI_bestTech(int iMaxPathLength = 1, bool bIgnoreCost = false, bool bAsync = false, TechTypes eIgnoreTech = NO_TECH, AdvisorTypes eIgnoreAdvisor = NO_ADVISOR);


	bool AI_canTrainSettler();

	int AI_techValue(TechTypes eTech, int iPathLength, bool bIgnoreCost, bool bAsync);
	int AI_techBuildingValue( TechTypes eTech, int iPathLength, bool &bEnablesWonder);
	int AI_techUnitValue( TechTypes eTech, int iPathLength, bool &bEnablesUnitWonder);
	int AI_heritageValue(const HeritageTypes eType) const;

	int  AI_TechValueCached(TechTypes eTech, bool bAsync, bool considerFollowOns = false);
	int AI_averageCurrentTechValue(TechTypes eRelativeTo, bool bAsync);
	void AI_chooseFreeTech();
	void AI_chooseResearch();
	void AI_startGoldenAge();

	DllExport DiploCommentTypes AI_getGreeting(PlayerTypes ePlayer) const;
	bool AI_isWillingToTalk(PlayerTypes ePlayer) const;
	bool AI_demandRebukedSneak(PlayerTypes ePlayer) const;
	bool AI_demandRebukedWar(PlayerTypes ePlayer) const;
	bool AI_hasTradedWithTeam(TeamTypes eTeam) const;

	AttitudeTypes AI_getAttitude(PlayerTypes ePlayer, bool bForced = true) const;
	int AI_getAttitudeVal(PlayerTypes ePlayer, bool bForced = true) const;
	static AttitudeTypes AI_getAttitudeFromValue(int iAttitudeVal);

	int AI_calculateStolenCityRadiusPlots(PlayerTypes ePlayer) const;
	int AI_getCloseBordersAttitude(PlayerTypes ePlayer) const;

	int AI_getWarAttitude(PlayerTypes ePlayer) const;
	int AI_getPeaceAttitude(PlayerTypes ePlayer) const;
	int AI_getSameReligionAttitude(PlayerTypes ePlayer) const;
	int AI_getDifferentReligionAttitude(PlayerTypes ePlayer) const;
	int AI_getBonusTradeAttitude(PlayerTypes ePlayer) const;
	int AI_getOpenBordersAttitude(PlayerTypes ePlayer) const;
	int AI_getDefensivePactAttitude(PlayerTypes ePlayer) const;
	int AI_getRivalDefensivePactAttitude(PlayerTypes ePlayer) const;
	int AI_getRivalVassalAttitude(PlayerTypes ePlayer) const;
	int AI_getShareWarAttitude(PlayerTypes ePlayer) const;
	int AI_getFavoriteCivicAttitude(PlayerTypes ePlayer) const;
	int AI_getTradeAttitude(PlayerTypes ePlayer) const;
	int AI_getRivalTradeAttitude(PlayerTypes ePlayer) const;
	int AI_getMemoryAttitude(PlayerTypes ePlayer, MemoryTypes eMemory) const;
	int AI_getColonyAttitude(PlayerTypes ePlayer) const;

	// BEGIN: Show Hidden Attitude Mod 01/22/2010
	int AI_getFirstImpressionAttitude(PlayerTypes ePlayer) const;
	int AI_getTeamSizeAttitude(PlayerTypes ePlayer) const;
	int AI_getBetterRankDifferenceAttitude(PlayerTypes ePlayer) const;
	int AI_getWorseRankDifferenceAttitude(PlayerTypes ePlayer) const;
	int AI_getLowRankAttitude(PlayerTypes ePlayer) const;
	int AI_getLostWarAttitude(PlayerTypes ePlayer) const;
	int AI_getKnownPlayerRank(PlayerTypes ePlayer) const;
	int AI_getTraitAttitude(PlayerTypes ePlayer) const;
	// END: Show Hidden Attitude Mod

	PlayerVoteTypes AI_diploVote(const VoteSelectionSubData& kVoteData, VoteSourceTypes eVoteSource, bool bPropose);

	int AI_dealVal(PlayerTypes ePlayer, const CLinkList<TradeData>* pList, bool bIgnoreAnnual = false, int iExtra = 1) const;
	bool AI_goldDeal(const CLinkList<TradeData>* pList) const;
	bool AI_considerOffer(PlayerTypes ePlayer, const CLinkList<TradeData>* pTheirList, const CLinkList<TradeData>* pOurList, int iChange = 1) const;
	bool AI_counterPropose(PlayerTypes ePlayer, const CLinkList<TradeData>* pTheirList, const CLinkList<TradeData>* pOurList, CLinkList<TradeData>* pTheirInventory, CLinkList<TradeData>* pOurInventory, CLinkList<TradeData>* pTheirCounter, CLinkList<TradeData>* pOurCounter) const;

	int AI_maxGoldTrade(PlayerTypes ePlayer) const;
	int AI_maxGoldPerTurnTrade(PlayerTypes ePlayer) const;

	int AI_getGoldValue(const int iGold, const int iValuePercent) const;
	int AI_getGoldFromValue(const int iValue, const int iValuePercent) const;

	int AI_bonusVal(BonusTypes eBonus, int iChange = 1, bool bForTrade = false) const;
	int AI_baseBonusVal(BonusTypes eBonus, bool bForTrade = false) const;
	int AI_bonusTradeVal(BonusTypes eBonus, PlayerTypes ePlayer, int iChange) const;
	DenialTypes AI_bonusTrade(BonusTypes eBonus, PlayerTypes ePlayer) const;
	int AI_corporationBonusVal(BonusTypes eBonus) const;

	int AI_cityTradeVal(CvCity* pCity) const;
	DenialTypes AI_cityTrade(CvCity* pCity, PlayerTypes ePlayer) const;

	int AI_stopTradingTradeVal(TeamTypes eTradeTeam, PlayerTypes ePlayer) const;
	DenialTypes AI_stopTradingTrade(TeamTypes eTradeTeam, PlayerTypes ePlayer) const;

	int AI_civicTradeVal(CivicTypes eCivic, PlayerTypes ePlayer) const;
	DenialTypes AI_civicTrade(CivicTypes eCivic, PlayerTypes ePlayer) const;

	int AI_religionTradeVal(ReligionTypes eReligion, PlayerTypes ePlayer) const;
	DenialTypes AI_religionTrade(ReligionTypes eReligion, PlayerTypes ePlayer) const;

	int AI_unitImpassableCount(UnitTypes eUnit) const;

	int AI_unitValue(UnitTypes eUnit, UnitAITypes eUnitAI, const CvArea* pArea, const CvUnitSelectionCriteria* criteria = NULL) const;

	int AI_unitHealerValue(UnitTypes eUnit, UnitCombatTypes eUnitCombat = NO_UNITCOMBAT) const;
	int AI_unitPropertyValue(UnitTypes eUnit, PropertyTypes eProperty = NO_PROPERTY) const;
	int AI_totalUnitAIs(UnitAITypes eUnitAI) const;
	int AI_totalAreaUnitAIs(const CvArea* pArea, UnitAITypes eUnitAI) const;
	int AI_totalWaterAreaUnitAIs(const CvArea* pArea, UnitAITypes eUnitAI) const;
	int AI_countCargoSpace(UnitAITypes eUnitAI) const;

	int AI_neededExplorers(const CvArea* pArea) const;
	int AI_neededHunters(const CvArea* pArea) const;
	int AI_neededWorkers(const CvArea* pArea) const;
	int AI_neededMissionaries(const CvArea* pArea, ReligionTypes eReligion) const;
	int AI_neededExecutives(const CvArea* pArea, CorporationTypes eCorporation) const;

	int AI_missionaryValue(const CvArea* pArea, ReligionTypes eReligion, PlayerTypes* peBestPlayer = NULL) const;
	int AI_executiveValue(const CvArea* pArea, CorporationTypes eCorporation, PlayerTypes* peBestPlayer = NULL) const;

	int AI_corporationValue(CorporationTypes eCorporation, const CvCity* pCity = NULL) const;

	int AI_adjacentPotentialAttackers(const CvPlot* pPlot, bool bTestCanMove = false) const;
	//int AI_PotentialEnemyAttackers(CvPlot* pPlot, bool bTestCanMove = false, bool bTestVisible = true) const;
	//int AI_PotentialDefenders(CvPlot* pPlot, bool bTestVisible = true) const;
	int AI_totalMissionAIs(MissionAITypes eMissionAI, const CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_areaMissionAIs(const CvArea* pArea, MissionAITypes eMissionAI, const CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_plotTargetMissionAIsInternal(const CvPlot* pPlot, MissionAITypes eMissionAI, int iRange = 0, int* piClosest = NULL) const;
	int AI_plotTargetMissionAIsInternalinCargoVolume(const CvPlot* pPlot, MissionAITypes eMissionAI, int iRange = 0, int* piClosest = NULL) const;
	int AI_plotTargetMissionAIs(const CvPlot* pPlot, MissionAITypes eMissionAI, const CvSelectionGroup* pSkipSelectionGroup = NULL, int iRange = 0, int* piClosest = NULL) const;
	int AI_plotTargetMissionAIsinCargoVolume(const CvPlot* pPlot, MissionAITypes eMissionAI, const CvSelectionGroup* pSkipSelectionGroup = NULL, int iRange = 0, int* piClosest = NULL) const;
	void AI_noteMissionAITargetCountChange(MissionAITypes eMissionAI, const CvPlot* pPlot, int iChange, const CvPlot* pUnitPlot, int iVolume);
	int AI_unitTargetMissionAIs(const CvUnit* pUnit, MissionAITypes eMissionAI, const CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_unitTargetMissionAIs(const CvUnit* pUnit, MissionAITypes* aeMissionAI, int iMissionAICount, const CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_enemyTargetMissionAIs(MissionAITypes eMissionAI, const CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_enemyTargetMissionAIs(MissionAITypes* aeMissionAI, int iMissionAICount, const CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_wakePlotTargetMissionAIs(const CvPlot* pPlot, MissionAITypes eMissionAI, const CvSelectionGroup* pSkipSelectionGroup = NULL) const;

	int AI_cityTargetUnitsByPath(const CvCity* pCity, const CvSelectionGroup* pSkipSelectionGroup, int iMaxPathTurns) const;
	int AI_enemyTargetMissions(TeamTypes eTargetTeam, const CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_enemyTargetMissionAIs(TeamTypes eTargetTeam, MissionAITypes* aeMissionAI, int iMissionAICount, const CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_unitTargetMissionAIs(const CvUnit* pUnit, MissionAITypes* aeMissionAI, int iMissionAICount, const CvSelectionGroup* pSkipSelectionGroup, int iMaxPathTurns, bool bCargo = false) const;
	CivicTypes AI_bestCivic(CivicOptionTypes eCivicOption, int* iBestValue) const;

	CivicTypes AI_bestCivic(CivicOptionTypes eCivicOption, int* iBestValue, bool bCivicOptionVacuum = false, CivicTypes* paeSelectedCivics = NULL) const;
	CivicTypes AI_bestCivic(CivicOptionTypes eCivicOption) const;
	int AI_civicValue(CivicTypes eCivic, bool bCivicOptionVacuum = false, CivicTypes* paeSelectedCivics = NULL) const;
	//	Provide a measure of overall happyness (weighted appropriately by city)
	int AI_getOverallHappyness(int iExtraUnhappy = 0) const;

	int AI_RevCalcCivicRelEffect(CivicTypes eCivic) const;

	ReligionTypes AI_bestReligion() const;
	int AI_religionValue(ReligionTypes eReligion) const;

	ReligionTypes AI_findHighestHasReligion();

	EspionageMissionTypes AI_bestPlotEspionage(CvPlot* pSpyPlot, PlayerTypes& eTargetPlayer, CvPlot*& pPlot, int& iData) const;
	int AI_espionageVal(PlayerTypes eTargetPlayer, EspionageMissionTypes eMission, const CvPlot* pPlot, int iData) const;

	int AI_getPeaceWeight() const;
	void AI_setPeaceWeight(int iNewValue);

	int AI_getEspionageWeight() const;
	void AI_setEspionageWeight(int iNewValue);

	int AI_getAttackOddsChange() const;
	void AI_setAttackOddsChange(int iNewValue);

	int AI_getCivicTimer() const;
	void AI_setCivicTimer(int iNewValue);
	void AI_changeCivicTimer(int iChange);

	int AI_getReligionTimer() const;
	void AI_setReligionTimer(int iNewValue);
	void AI_changeReligionTimer(int iChange);

	int AI_getExtraGoldTarget() const;
	void AI_setExtraGoldTarget(int iNewValue);

	int AI_getNumTrainAIUnits(UnitAITypes eIndex) const;
	void AI_changeNumTrainAIUnits(UnitAITypes eIndex, int iChange);

	int AI_getNumAIUnits(UnitAITypes eIndex) const;
	void AI_changeNumAIUnits(UnitAITypes eIndex, int iChange);

	int AI_getSameReligionCounter(PlayerTypes eIndex) const;
	void AI_changeSameReligionCounter(PlayerTypes eIndex, int iChange);

	int AI_getDifferentReligionCounter(PlayerTypes eIndex) const;
	void AI_changeDifferentReligionCounter(PlayerTypes eIndex, int iChange);

	int AI_getFavoriteCivicCounter(PlayerTypes eIndex) const;
	void AI_changeFavoriteCivicCounter(PlayerTypes eIndex, int iChange);

	int AI_getBonusTradeCounter(PlayerTypes eIndex) const;
	void AI_changeBonusTradeCounter(PlayerTypes eIndex, int iChange);

	int AI_getPeacetimeTradeValue(PlayerTypes eIndex) const;
	void AI_changePeacetimeTradeValue(PlayerTypes eIndex, int iChange);

	int AI_getPeacetimeGrantValue(PlayerTypes eIndex) const;
	void AI_changePeacetimeGrantValue(PlayerTypes eIndex, int iChange);

	int AI_getGoldTradedTo(PlayerTypes eIndex) const;
	void AI_changeGoldTradedTo(PlayerTypes eIndex, int iChange);

	int AI_getAttitudeExtra(const PlayerTypes ePlayer) const;
	void AI_setAttitudeExtra(const PlayerTypes ePlayer, const int iNewValue);
	void AI_changeAttitudeExtra(const PlayerTypes ePlayer, const int iChange);

	bool AI_isFirstContact(PlayerTypes eIndex) const;
	void AI_setFirstContact(PlayerTypes eIndex, bool bNewValue);

	int AI_getContactTimer(PlayerTypes eIndex1, ContactTypes eIndex2) const;
	void AI_changeContactTimer(PlayerTypes eIndex1, ContactTypes eIndex2, int iChange);

	int AI_getMemoryCount(PlayerTypes eIndex1, MemoryTypes eIndex2) const;
	void AI_changeMemoryCount(PlayerTypes eIndex1, MemoryTypes eIndex2, int iChange);

	int AI_calculateGoldenAgeValue() const;

	void AI_doCommerce();

	EventTypes AI_chooseEvent(int iTriggeredId, int* pValue = NULL) const;
	virtual void AI_launch(VictoryTypes eVictory);

	int AI_getCultureVictoryStage() const;
	int AI_getSpaceVictoryStage() const;
	int AI_getConquestVictoryStage() const;
	int AI_getDominationVictoryStage() const;
	int AI_getDiplomacyVictoryStage() const;
	bool AI_isDoVictoryStrategy(int iVictoryStrategy) const;
	bool AI_isDoVictoryStrategyLevel4() const;
	bool AI_isDoVictoryStrategyLevel3() const;
	void AI_forceUpdateVictoryStrategies();
	int AI_getVictoryStrategyHash() const;
	int AI_getStrategyRand(int iShift) const;
	void AI_calculateStrategyRand();

	int AI_cultureVictoryTechValue(TechTypes eTech) const;

	bool AI_isDoStrategy(int iStrategy) const;
	void AI_forceUpdateStrategies();

	void AI_nowHasTech(TechTypes eTech);

	int AI_countDeadlockedBonuses(const CvPlot* pPlot) const;

	int AI_getOurPlotStrength(const CvPlot* pPlot, int iRange, bool bDefensiveBonuses, bool bTestMoves) const;
	int AI_getEnemyPlotStrength(const CvPlot* pPlot, int iRange, bool bDefensiveBonuses, bool bTestMoves) const;

	int AI_goldToUpgradeAllUnits(int iExpThreshold = 0) const;

	int AI_goldTradeValuePercent() const;

	int AI_averageYieldMultiplier(YieldTypes eYield) const;
	int AI_averageCommerceMultiplier(CommerceTypes eCommerce) const;
	int AI_averageGreatPeopleMultiplier() const;
	int AI_averageCommerceExchange(CommerceTypes eCommerce) const;

	int AI_playerCloseness(PlayerTypes eIndex, int iMaxDistance) const;

	int AI_getTotalCityThreat() const;
	int AI_getTotalFloatingDefenseNeeded() const;
	int AI_getTotalProperty(PropertyTypes eProperty) const;


	int AI_getTotalAreaCityThreat(const CvArea* pArea, int* piLargestThreat = NULL) const;
	int AI_countNumAreaHostileUnits(const CvArea* pArea, bool bPlayer, bool bTeam, bool bNeutral, bool bHostile, const CvPlot* pPlot, int iMaxDistance) const;
	int AI_getTotalFloatingDefendersNeeded(const CvArea* pArea) const;
	int AI_getTotalFloatingDefenders(const CvArea* pArea) const;

	RouteTypes AI_bestAdvancedStartRoute(const CvPlot* pPlot, int* piYieldValue = NULL) const;
	UnitTypes AI_bestAdvancedStartUnitAI(const CvPlot* pPlot, UnitAITypes eUnitAI) const;
	CvPlot* AI_advancedStartFindCapitalPlot() const;

	bool AI_advancedStartPlaceExploreUnits(bool bLand);
	void AI_advancedStartRevealRadius(const CvPlot* pPlot, int iRadius);
	bool AI_advancedStartPlaceCity(const CvPlot* pPlot);
	bool AI_advancedStartDoRoute(const CvPlot* pFromPlot, const CvPlot* pToPlot);
	void AI_advancedStartRouteTerritory();
	void AI_doAdvancedStart(bool bNoExit = false);

	int AI_getMinFoundValue() const;

	void AI_recalculateFoundValues(int iX, int iY, int iInnerRadius, int iOuterRadius) const;

	int AI_getCitySitePriorityFactor(const CvPlot* pPlot) const;
	void AI_updateCitySites(int iMinFoundValueThreshold, int iMaxSites) const;
	void calculateCitySites() const;

	bool AI_isPlotCitySite(const CvPlot* pPlot) const;
	int AI_getNumAreaCitySites(int iAreaID, int& iBestValue) const;
	int AI_getNumAdjacentAreaCitySites(int iWaterAreaID, int iExcludeArea, int& iBestValue) const;

	int AI_getNumCitySites() const;
	CvPlot* AI_getCitySite(int iIndex) const;

	int AI_bestAreaUnitAIValue(UnitAITypes eUnitAI, const CvArea* pArea, UnitTypes* peBestUnitType = NULL, const CvUnitSelectionCriteria* criteria = NULL) const;
	int AI_bestCityUnitAIValue(UnitAITypes eUnitAI, const  CvCity* pCity, UnitTypes* peBestUnitType = NULL, const CvUnitSelectionCriteria* criteria = NULL) const;

	int AI_calculateTotalBombard(DomainTypes eDomain) const;

	int AI_getUnitWeight(UnitTypes eUnit) const;
	int AI_getUnitCombatWeight(UnitCombatTypes eUnitCombat) const;

#ifdef CVARMY_BREAKSAVE
	void AI_formArmies();   // Nouvelle fonction de création des armées
#endif

	/**
	 * \brief Calculate the relative viability of a unit type for a given AI role.
	 *
	 * This function evaluates all units of the specified domain and determines
	 * how strong the best unit of the requested AI type is compared to other
	 * available units in the same domain.
	 *
	 * \param eUnitAI The Unit AI role to evaluate (e.g., UNITAI_ATTACK, UNITAI_CITY_DEFENSE).
	 * \param eDomain The domain of units to consider (DOMAIN_LAND, DOMAIN_SEA, DOMAIN_AIR).
	 *
	 * \return An integer representing the relative strength of the best unit
	 *         for the requested AI type compared to the strongest other unit
	 *         in the same domain. The value is scaled by 100.
	 *
	 * \note
	 * - Units are considered only if they are available to the player's team
	 *   (weight > 0 or prerequisite tech known).
	 * - Compares combat strength (`getCombat()`) of units.
	 *
	 * \see CvUnitInfo::getUnitAIType()
	 * \see GET_TEAM()
	 * \see CvPlayerAI::m_aiUnitWeights
	 *
	 * \dot
	 * digraph AI_calculateUnitAIViability {
	 *   rankdir=LR;
	 *   node [shape=box, style=filled, color=lightblue];
	 *   Start [label="Start"];
	 *   ForEachUnit [label="For each unit in domain"];
	 *   CheckAvailability [label="Check if player can use unit"];
	 *   CheckUnitAI [label="Does unit match eUnitAI?"];
	 *   UpdateBestAI [label="Update iBestUnitAIStrength"];
	 *   UpdateBestOther [label="Update iBestOtherStrength"];
	 *   ReturnValue [label="Return (100 * iBestUnitAIStrength) / iBestOtherStrength"];
	 *
	 *   Start -> ForEachUnit;
	 *   ForEachUnit -> CheckAvailability;
	 *   CheckAvailability -> CheckUnitAI [label="Yes"];
	 *   CheckAvailability -> UpdateBestOther [label="No"];
	 *   CheckUnitAI -> UpdateBestAI [label="Yes"];
	 *   CheckUnitAI -> UpdateBestOther [label="No"];
	 *   UpdateBestAI -> ForEachUnit;
	 *   UpdateBestOther -> ForEachUnit;
	 *   ForEachUnit -> ReturnValue [label="End loop"];
	 * }
	 * \enddot
	 */
	int AI_calculateUnitAIViability(UnitAITypes eUnitAI, DomainTypes eDomain) const;

	void AI_updateBonusValue();
	void AI_updateBonusValue(BonusTypes eBonus);

	int AI_getAttitudeWeight(PlayerTypes ePlayer) const;

	ReligionTypes AI_chooseReligion();

	int AI_getPlotAirbaseValue(const CvPlot* pPlot) const;
	int AI_getPlotCanalValue(const CvPlot* pPlot) const;
	// Super Forts *choke*
	int AI_getPlotChokeValue(const CvPlot* pPlot) const;
	// Super Forts end

	bool AI_isPlotThreatened(const CvPlot* pPlot, int iRange = -1, bool bTestMoves = true) const;

	bool AI_isFirstTech(TechTypes eTech) const;

	CvCity* getInquisitionRevoltCity(const CvUnit *pUnit, const bool bNoUnit, int iRevIndexThreshold, const int iTrendThreshold) const;
	CvCity* getTeamInquisitionRevoltCity(const CvUnit *pUnit, const bool bNoUnit, int iRevIndexThreshold, const int iTrendThreshold) const;
	CvCity* getReligiousVictoryTarget(const CvUnit* pUnit, const bool bNoUnit) const;

	bool isPushReligiousVictory() const;
	void AI_setPushReligiousVictory();

	bool isConsiderReligiousVictory() const;
	void AI_setConsiderReligiousVictory();

	bool hasInquisitionTarget() const;
	void AI_setHasInquisitionTarget();

	int countCityReligionRevolts() const;

	int AI_workerTradeVal(const CvUnit* pUnit) const;
	int AI_militaryUnitTradeVal(const CvUnit* pUnit) const;

	void resetBonusClassTallyCache(const int iTurn = -1, const bool bFull = true);

private:
	// Toffer - Transient Caches
	int* m_bonusClassRevealed;
	int* m_bonusClassUnrevealed;
	int* m_bonusClassHave;
	int m_iBonusClassTallyCachedTurn;

	// will be set to true first time every game load when settler can be built. (since it never happens that you can no longer build settler unit
	bool m_canTrainSettler;

	int AI_corporationTradeVal(CorporationTypes eCorporation) const;

public:

	int AI_pledgeVoteTradeVal(const VoteTriggeredData* kData, PlayerVoteTypes ePlayerVote, PlayerTypes ePlayer) const;
	int AI_secretaryGeneralTradeVal(VoteSourceTypes eVoteSource, PlayerTypes ePlayer) const;

	int AI_getEmbassyAttitude(PlayerTypes ePlayer) const;
	TeamTypes AI_bestJoinWarTeam(PlayerTypes ePlayer) const;
	TeamTypes AI_bestMakePeaceTeam(PlayerTypes ePlayer) const;
	TeamTypes AI_bestStopTradeTeam(PlayerTypes ePlayer) const;
	int AI_militaryBonusVal(BonusTypes eBonus);
	int AI_getCivicShareAttitude(PlayerTypes ePlayer) const;
	int AI_getCivicAttitudeChange(PlayerTypes ePlayer) const;
	int AI_promotionValue(PromotionTypes ePromotion, UnitTypes eUnit, const CvUnit* pUnit = NULL, UnitAITypes eUnitAI = NO_UNITAI, bool bForBuildUp = false) const;
	int AI_unitCombatValue(UnitCombatTypes eUnitCombat, UnitTypes eUnit, const CvUnit* pUnit = NULL, UnitAITypes eUnitAI = NO_UNITAI) const;
	TechTypes AI_bestReligiousTech(int iMaxPathLength, TechTypes eIgnoreTech, AdvisorTypes eIgnoreAdvisor);
	int AI_religiousTechValue(TechTypes eTech) const;
	int AI_ourCityValue(CvCity* pCity) const;

	void AI_noteWarStatusChange(TeamTypes eTeam, bool bAtWar);

	void AI_doMilitaryProductionCity();
	int AI_getMilitaryProductionCityCount() const;
	int AI_getNavalMilitaryProductionCityCount() const;

	CvCity* findBestCoastalCity() const;
	UnitTypes bestBuildableUnitForAIType(DomainTypes eDomain, UnitAITypes eUnitAIType, const CvUnitSelectionCriteria* criteria = NULL) const;
	int strengthOfBestUnitAI(DomainTypes eDomain, UnitAITypes eUnitAIType) const;

	mutable int m_iMilitaryProductionCityCount;
	mutable int m_iNavalMilitaryProductionCityCount;
	mutable int* m_aiCivicValueCache;

	void AI_invalidateAttitudeCache(PlayerTypes ePlayer);
	void AI_invalidateAttitudeCache();
	void AI_changeAttitudeCache(const PlayerTypes ePlayer, const int iChange);

	int	AI_getNumBuildingsNeeded(BuildingTypes eBuilding, bool bCoastal) const;
	void AI_changeNumBuildingsNeeded(BuildingTypes eBuilding, int iChange);

	// for serialization
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

	void AI_noteUnitRecalcNeeded();
	void AI_recalculateUnitCounts();
	int AI_calculateAverageLocalInstability() const;
	int AI_calculateAverageCityDistance() const;

protected:

	static CvPlayerAI* m_aPlayers;

	int m_iPeaceWeight;
	int m_iEspionageWeight;
	int m_iAttackOddsChange;
	int m_iCivicTimer;
	int m_iReligionTimer;
	int m_iExtraGoldTarget;

	mutable int m_iStrategyHash;
	mutable int m_iStrategyHashCacheTurn;

	mutable int m_iStrategyRand;
	mutable int m_iVictoryStrategyHash;
	mutable int m_iVictoryStrategyHashCacheTurn;

	bool m_bPushReligiousVictory;
	bool m_bConsiderReligiousVictory;
	bool m_bHasInquisitionTarget;

	mutable TechTypes m_eBestResearchTarget; // Koshling - retain beeline target to minmimize need for recalculation

	mutable volatile int m_iAveragesCacheTurn;

	mutable int m_iAverageGreatPeopleMultiplier;

	mutable int *m_aiAverageYieldMultiplier;
	mutable int *m_aiAverageCommerceMultiplier;
	mutable int *m_aiAverageCommerceExchange;

	mutable int m_iUpgradeUnitsCacheTurn;
	mutable int m_iUpgradeUnitsCachedExpThreshold;
	mutable int m_iUpgradeUnitsCachedGold;

	int *m_aiNumTrainAIUnits;
	int *m_aiNumAIUnits;
	int* m_aiSameReligionCounter;
	int* m_aiDifferentReligionCounter;
	int* m_aiFavoriteCivicCounter;
	int* m_aiBonusTradeCounter;
	int* m_aiPeacetimeTradeValue;
	int* m_aiPeacetimeGrantValue;
	int* m_aiGoldTradedTo;
	int* m_aiAttitudeExtra;
	int* m_aiBonusValue;
	int* m_aiTradeBonusValue;
	bool* m_abNonTradeBonusCalculated;
	int* m_aiUnitWeights;
	int* m_aiUnitCombatWeights;

	mutable int* m_aiCloseBordersAttitudeCache;

	mutable int* m_aiAttitudeCache;

	bool* m_abFirstContact;

	int** m_aaiContactTimer;
	int** m_aaiMemoryCount;

	mutable std::vector<int> m_aiAICitySites;
	mutable bool m_bCitySitesNotCalculated;

	bool m_bWasFinancialTrouble;
	int m_iTurnLastProductionDirty;


	void AI_doCounter();
	void AI_doMilitary();

	int heritagePropertiesValue(const CvHeritageInfo& heritage) const;

public:
	void AI_doCivics();
protected:
	void AI_doReligion();
	void AI_doDiplo();

	void AI_beginDiplomacy(CvDiploParameters* pDiploParams, PlayerTypes ePlayer);

	void AI_doSplit();
	void AI_doCheckFinancialTrouble();

	bool AI_disbandUnit(int iExpThreshold);

	int AI_getStrategyHash() const;
	void AI_calculateAverages() const;

	bool AI_isCivicCanChangeOtherValues(CivicTypes eCivicSelected, ReligionTypes eAssumedReligion = NO_RELIGION) const;
	bool AI_isCivicValueRecalculationRequired(CivicTypes eCivic, CivicTypes eCivicSelected, ReligionTypes eAssumedReligion = NO_RELIGION) const;

	int AI_getHappinessWeight(int iHappy, int iExtraPop) const;
	int AI_getHealthWeight(int iHealth, int iExtraPop) const;

	void AI_convertUnitAITypesForCrush();
	int AI_eventValue(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const;

	void AI_doEnemyUnitData();
	void AI_invalidateCloseBordersAttitudeCache();

	friend class CvGameTextMgr;

#ifdef PLOT_DANGER_CACHING
public:
	static void ClearPlotDangerCache()
	{
		plotDangerCache.clear();
	}
private:
	static plotDangerCache plotDangerCache;
	static int plotDangerCacheHits;
	static int plotDangerCacheReads;
#endif

	techPath* findBestPath(TechTypes eTech, int& valuePerUnitCost, bool bIgnoreCost, bool bAsync);
	int	 techPathValuePerUnitCost(techPath* path, TechTypes eTech, bool bIgnoreCost, bool bAsync);
	TechTypes findStartTech(techPath* path) const;

	typedef stdext::hash_map<TechTypes, int> TechTypesValueMap;
	mutable TechTypesValueMap m_cachedTechValues;
	typedef stdext::hash_map<BuildingTypes, int> BuildingCountMap;
	mutable BuildingCountMap m_numBuildingsNeeded;

	int m_iCityGrowthValueBase;
	int m_turnsSinceLastRevolution;
	int m_iCivicSwitchMinDeltaThreshold;
	bool bUnitRecalcNeeded;

	typedef stdext::hash_map<const CvPlot*, MissionTargetInfo> PlotMissionTargetMap;
	typedef stdext::hash_map<MissionAITypes, PlotMissionTargetMap> MissionPlotTargetMap;
	mutable MissionPlotTargetMap m_missionTargetCache;
};

// helper for accessing static functions
#define GET_PLAYER CvPlayerAI::getPlayer

#endif
