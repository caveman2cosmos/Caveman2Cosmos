#pragma once

// unitAI.h

#ifndef CIV4_UNIT_AI_H
#define CIV4_UNIT_AI_H

#include "CvUnit.h"

class CvReachablePlotSet;
class CvCity;
class ConstructionNeeds;

typedef enum
{
	CONTRACTUAL_STATE_NONE,
	CONTRACTUAL_STATE_AWAITING_ANSWER,
	CONTRACTUAL_STATE_AWAITING_WORK,
	CONTRACTUAL_STATE_FOUND_WORK,
	CONTRACTUAL_STATE_NO_WORK_FOUND
} ContractualState;

class CvUnitAI : public CvUnit
{

public:

	CvUnitAI(bool bIsDummy = false);
	virtual ~CvUnitAI();

	void AI_init(UnitAITypes eUnitAI, int iBirthmark);
	void AI_uninit();
	void AI_reset(UnitAITypes eUnitAI = NO_UNITAI, bool bConstructorCall = false);

	bool AI_update();
	bool AI_follow();
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      04/05/10                                jdog5000      */
/*                                                                                              */
/* Unit AI                                                                                      */
/************************************************************************************************/
	bool AI_load(UnitAITypes eUnitAI, MissionAITypes eMissionAI, UnitAITypes eTransportedUnitAI = NO_UNITAI, int iMinCargo = -1, int iMinCargoSpace = -1, int iMaxCargoSpace = -1, int iMaxCargoOurUnitAI = -1, int iFlags = 0, int iMaxPath = MAX_INT, int iMaxTransportPath = MAX_INT);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	bool AI_upgrade(); // override

	bool AI_promote(); // override

#define	LEADER_PRIORITY_MIN 0
#define	LEADER_PRIORITY_MAX	100

	int AI_groupFirstVal();
	int AI_groupSecondVal();

	int AI_attackOdds(const CvPlot* pPlot, bool bPotentialEnemy, CvUnit** ppDefender = NULL, bool bAssassinate = false);
	//	Variant to test a specific defender AS IF it was in the specified plot
	int AI_attackOddsAtPlot(const CvPlot* pPlot, CvUnit* pDefender, bool modifyPredictedResults = false);
	int AI_attackOddsAtPlotInternal(const CvPlot* pPlot, CvUnit* pDefender, bool modifyPredictedResults);

	bool AI_bestCityBuild(CvCity* pCity, CvPlot** ppBestPlot = NULL, BuildTypes* peBestBuild = NULL, CvPlot* pIgnorePlot = NULL, CvUnit* pUnit = NULL);

	bool AI_isCityAIType() const;

	int AI_getBirthmark() const;
	void AI_setBirthmark(int iNewValue);

	UnitAITypes AI_getUnitAIType() const;
	void AI_setUnitAIType(UnitAITypes eNewValue);

	int AI_sacrificeValue(const CvPlot* pPlot) const;

	bool AI_isAwaitingContract() const;
	bool AI_isCityGarrison(const CvCity* pCity) const;
	void AI_setAsGarrison(const CvCity* pCity = NULL);

	BuildingTypes getIntendedConstructBuilding() const { return m_eIntendedConstructBuilding; };

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);

protected:

	int m_iBirthmark;

	UnitAITypes m_eUnitAIType;

	int m_iAutomatedAbortTurn;

	int m_contractsLastEstablishedTurn;
	ContractualState m_contractualState;

	int m_iGarrisonCity;
	int m_iAffirmedGarrisonCity;

	BuildingTypes	m_eIntendedConstructBuilding;	//	Used to coordinate subdued animal and great person builds
	static ConstructionNeeds*	m_constructionNeeds;

	int m_iGroupLeadOverride;
	int m_iPredictedHitPoints;
	bool m_bHasAttacked;
	bool m_bWaitingOnUnitAIAny;

	mutable int m_iGenericValue;
	mutable int m_eGenericValueFlagsCached;

	std::vector<int> m_aiWaitingOnUnitAITypes;

	void doUnitAIMove();

	void AI_animalMove();
	void AI_settleMove();
	int AI_minSettlerDefense();
	void AI_workerMove();
	void AI_barbAttackMove();
	void AI_attackMove();
	void AI_attackCityMove();
	void AI_attackCityLemmingMove();
	void AI_collateralMove();
	void AI_pillageMove();
	void AI_reserveMove();
	void AI_counterMove();
	void AI_pillageCounterMove();
	void AI_paratrooperMove();
	void AI_cityDefenseMove();
	void AI_cityDefenseExtraMove();
	void AI_exploreMove();
	void AI_hunterEscortMove();
	void AI_missionaryMove();
	void AI_prophetMove();
	void AI_artistMove();
	void AI_scientistMove();
	void AI_generalMove();
	void AI_greatHunterMove();
	void AI_greatAdmiralMove();
	void AI_merchantMove();
	void AI_engineerMove();
	void AI_subduedAnimalMove();
	void AI_spyMove();
	void AI_ICBMMove();
	void AI_workerSeaMove();
	void AI_barbAttackSeaMove();
	void AI_pirateSeaMove();
	void AI_attackSeaMove();
	void AI_reserveSeaMove();
	void AI_escortSeaMove();
	void AI_exploreSeaMove();
	void AI_assaultSeaMove();
	void AI_settlerSeaMove();
	void AI_missionarySeaMove();
	void AI_spySeaMove();
	void AI_carrierSeaMove();
	void AI_missileCarrierSeaMove();
	void AI_attackAirMove();
	void AI_defenseAirMove();
	void AI_carrierAirMove();
	void AI_missileAirMove();
	void AI_healerMove();
	void AI_healerSeaMove();
	void AI_propertyControlMove();
	void AI_propertyControlSeaMove();
	void AI_BarbCriminalMove();
	void AI_InvestigatorMove();
	void AI_InfiltratorMove();
	void AI_SeeInvisibleMove();
	void AI_SeeInvisibleSeaMove();
	void AI_EscortMove();

	void AI_networkAutomated();
	void AI_cityAutomated();

	int AI_promotionValue(PromotionTypes ePromotion);

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      04/01/10                                jdog5000      */
/*                                                                                              */
/* Unit AI                                                                                      */
/************************************************************************************************/
/* original bts code
	bool AI_shadow(UnitAITypes eUnitAI, int iMax = -1, int iMaxRatio = -1, bool bWithCargoOnly = true);
*/
	bool AI_shadow(UnitAITypes eUnitAI, int iMax = -1, int iMaxRatio = -1, bool bWithCargoOnly = true, bool bOutsideCityOnly = false, int iMaxPath = MAX_INT);
	bool AI_group(UnitAITypes eUnitAI, int iMaxGroup = -1, int iMaxOwnUnitAI = -1, int iMinUnitAI = -1, bool bIgnoreFaster = false, bool bIgnoreOwnUnitType = false, bool bStackOfDoom = false, int iMaxPath = MAX_INT, bool bAllowRegrouping = false, bool bWithCargoOnly = false, bool bInCityOnly = false, MissionAITypes eIgnoreMissionAIType = NO_MISSIONAI);
	//bool AI_load(UnitAITypes eUnitAI, MissionAITypes eMissionAI, UnitAITypes eTransportedUnitAI = NO_UNITAI, int iMinCargo = -1, int iMinCargoSpace = -1, int iMaxCargoSpace = -1, int iMaxCargoOurUnitAI = -1, int iFlags = 0, int iMaxPath = MAX_INT, int iMaxTransportPath = MAX_INT);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	bool AI_guardCityBestDefender();
	bool AI_guardCityMinDefender(bool bSearch = true);
	bool AI_guardCity(bool bLeave = false, bool bSearch = false, int iMaxPath = MAX_INT);
	bool AI_guardCityAirlift();
	bool AI_guardBonus(int iMinValue = 0);
	int AI_getPlotDefendersNeeded(CvPlot* pPlot, int iExtra);
	bool AI_guardFort(bool bSearch = true);
	// Super Forts begin *AI_defense*
	bool AI_guardFortMinDefender(bool bSearch = true);
	// Super Forts end
	bool AI_guardCitySite();
	bool AI_guardSpy(int iRandomPercent);
	bool AI_destroySpy();
	bool AI_sabotageSpy();
	bool AI_pickupTargetSpy();
	bool AI_chokeDefend();
	bool AI_heal(int iDamagePercent = 0, int iMaxPath = MAX_INT);
	bool AI_afterAttack();
	/*TB Prophet Mod begin*/
#ifdef C2C_BUILD
	bool AI_foundReligion();
	bool AI_cureAffliction(PromotionLineTypes eAfflictionLine);
#endif
	/*TB Prophet Mod end*/
	bool AI_goldenAge();
	bool AI_spreadReligion();
	bool AI_spreadCorporation();
	bool AI_spreadReligionAirlift();
	bool AI_spreadCorporationAirlift();
	bool AI_discover(bool bThisTurnOnly = false, bool bFirstResearchOnly = false);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD & RevDCM                     09/03/10                        jdog5000      */
/*                                                                                phungus420    */
/* Great People AI, Unit AI                                                                     */
/************************************************************************************************/
	bool AI_leadLegend();
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	bool AI_lead(std::vector<UnitAITypes>& aeAIUnitTypes);
	bool AI_join(int iMaxCount = MAX_INT);
	bool AI_construct(int iMaxCount = MAX_INT, int iMaxSingleBuildingCount = MAX_INT, int iThreshold = 15, bool bDecayProbabilities = false, bool assumeSameValueEverywhere = false);
	bool AI_outcomeMission();
	bool AI_scrapSubdued();
	bool AI_moveToOurTerritory(int maxMoves);
	bool AI_switchHurry();
	bool AI_hurry(bool bAny = false);
	bool checkSwitchToConstruct();
	int getBestConstructValue(int iMaxCount, int iMaxSingleBuildingCount, int iDecayProbabilityRate, int iThreshold, bool assumeSameValueEverywhere, CvPlot*& bestConstructPlot, CvPlot*& bestTurnPlot, CvUnitAI*& eBestTargetingUnit, BuildingTypes& eBestBuilding);
	bool enactConstruct(CvPlot* pBestConstructPlot, CvPlot* pBestPlot, CvUnitAI* eBestTargetingUnit, BuildingTypes eBestBuilding);
/************************************************************************************************/
/* RevDCM                  Start		 5/2/09                                                 */
/*                                                                                              */
/* Inquisitions                                                                                 */
/************************************************************************************************/
	bool AI_doInquisition();
/************************************************************************************************/
/* Inquisitions	                     END                                                        */
/************************************************************************************************/
	bool AI_greatWork();
	bool AI_offensiveAirlift();
	bool AI_paradrop(int iRange);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      09/01/09                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	bool AI_protect(int iOddsThreshold, int iMaxPathTurns = MAX_INT);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	bool AI_seaAreaAttack();
	bool AI_patrol(bool bIgnoreDanger = false);
	bool AI_defend();
	bool AI_safety(int iRange = 1);
	bool AI_hide();
	bool AI_goody(int iRange);
	bool AI_explore();
	bool AI_exploreRange(int iRange);
	bool AI_refreshExploreRange(int iRange, bool bIncludeVisibilityRefresh = true);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/29/10                                jdog5000      */
/*                                                                                              */
/* War tactics AI                                                                               */
/************************************************************************************************/
	CvCity* AI_pickTargetCity(int iFlags = 0, int iMaxPath = MAX_INT, bool bHuntBarbs = false);
	bool AI_goToTargetCity(int iFlags = 0, int iMaxPath = MAX_INT, CvCity* pTargetCity = NULL);
	bool AI_goToTargetBarbCity(int iMaxPath = 10);
	bool AI_pillageAroundCity(CvCity* pTargetCity, int iBonusValueThreshold = 0, int iMaxPathTurns = MAX_INT);
	bool AI_bombardCity();
	bool AI_cityAttack(int iRange, int iOddsThreshold, bool bFollow = false);
	bool AI_anyAttack(int iRange, int iOddsThreshold, int iMinStack = 0, bool bAllowCities = true, bool bFollow = false);
	bool AI_attackTargets(int iRange, int iOddsThreshold, int iMinStack = 0, bool bAllowCities = true, bool bFollow = false);
/************************************************************************************************/
/* DCM                                     04/19/09                                Johny Smith  */
/************************************************************************************************/
// Dale - RB: Field Bombard START
	bool AI_RbombardPlot(int iRange, int iBonusValueThreshold); // RevolutionDCM
	bool AI_RbombardUnit(int iRange, int iHighestOddsThreshold, int iMinStack, int iSeigeDiff, int iPowerThreshold, bool bCity = false);
	bool AI_RbombardCity(CvCity* pCity);
	bool AI_RbombardNaval();
// Dale - RB: Field Bombard END
// Dale - ARB: Archer Bombard START
	bool AI_Abombard();
// Dale - ARB: Archer Bombard END
// Dale - FE: Fighters START
	bool AI_FEngage();
// Dale - FE: Fighters END
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/
	bool AI_rangeAttack(int iRange);
	bool AI_leaveAttack(int iRange, int iThreshold, int iStrengthThreshold);
	bool AI_blockade();
	int  AI_blockadeValue(CvPlot* pLoopPlot, CvCity* pCity, CvPlot*& endTurnPlot) const;
	bool AI_pirateBlockade();
	bool AI_seaBombardRange(int iMaxRange);
	bool AI_pillage(int iBonusValueThreshold = 0);
	bool AI_pillageRange(int iRange, int iBonusValueThreshold = 0);
	bool AI_found();
	bool AI_foundRange(int iRange, bool bFollow = false);
	bool AI_assaultSeaTransport(bool bBarbarian = false);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      01/04/09                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	bool AI_assaultSeaReinforce(bool bBarbarian = false);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	bool AI_settlerSeaTransport();
	bool AI_settlerSeaFerry();
	bool AI_specialSeaTransportMissionary();
	bool AI_specialSeaTransportSpy();
	bool AI_carrierSeaTransport();
	bool AI_connectPlot(CvPlot* pPlot, int iRange = 0);
	bool AI_improveCity(CvCity* pCity);
	bool AI_improveLocalPlot(int iRange, CvCity* pIgnoreCity);
	bool AI_nextCityToImprove(CvCity* pCity);
	bool AI_nextCityToImproveAirlift();
	bool AI_irrigateTerritory();
	bool AI_fortTerritory(bool bCanal, bool bAirbase);
	bool AI_improveBonus(int iMinValue = 0, CvPlot** ppBestPlot = NULL, BuildTypes* peBestBuild = NULL, int* piBestValue = NULL);
	bool AI_improvePlot(CvPlot* pPlot, BuildTypes eBuild);
	BuildTypes AI_betterPlotBuild(CvPlot* pPlot, BuildTypes eBuild);
	bool AI_connectBonus(bool bTestTrade = true);
	bool AI_connectCity();
	bool AI_routeCity();
	bool AI_routeTerritory(bool bImprovementOnly = false);
	bool AI_travelToUpgradeCity();
	bool AI_retreatToCity(bool bPrimary = false, bool bAirlift = false, int iMaxPath = MAX_INT);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      01/15/09                                jdog5000      */
/*                                                                                              */
/* Naval AI                                                                                     */
/************************************************************************************************/
	bool AI_pickup(UnitAITypes eUnitAI, bool bCountProduction = false, int iMaxPath = MAX_INT);
	bool AI_pickupStranded(UnitAITypes eUnitAI = NO_UNITAI, int iMaxPath = MAX_INT);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	bool AI_airOffensiveCity();
	bool AI_airDefensiveCity();
	bool AI_airCarrier();
	bool AI_missileLoad(UnitAITypes eTargetUnitAI, int iMaxOwnUnitAI = -1, bool bStealthOnly = false);
	bool AI_airStrike();
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						9/26/08				jdog5000	    */
/* 																			    */
/* 	Air AI																	    */
/********************************************************************************/
	int AI_airOffenseBaseValue( CvPlot* pPlot );
	bool AI_defensiveAirStrike();
	bool AI_defendBaseAirStrike();
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						END								    */
/********************************************************************************/
	bool AI_airBombPlots();
	bool AI_airBombDefenses();	
	bool AI_exploreAir();
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      01/12/09                                jdog5000      */
/*                                                                                              */
/* Player Interface                                                                             */
/************************************************************************************************/
	int AI_exploreAirPlotValue( CvPlot* pPlot );
	bool AI_exploreAir2();
	void AI_exploreAirMove();
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	bool AI_nuke();
	bool AI_nukeRange(int iRange);
	bool AI_trade(int iValueThreshold);
	bool AI_infiltrate();
	bool AI_reconSpy(int iRange);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      10/20/09                                jdog5000      */
/*                                                                                              */
/* Espionage AI                                                                                 */
/************************************************************************************************/
	bool AI_revoltCitySpy();
	bool AI_bonusOffenseSpy(int iMaxPath);
	bool AI_cityOffenseSpy(int iRange, CvCity* pSkipCity = NULL);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	bool AI_espionageSpy();
	bool AI_moveToStagingCity();
	//bool AI_seaRetreatFromCityDanger();
	//bool AI_airRetreatFromCityDanger();
	//bool AI_airAttackDamagedSkip();

	bool AI_followBombard();

	bool AI_potentialEnemy(TeamTypes eTeam, const CvPlot* pPlot = NULL);

	bool AI_defendPlot(const CvPlot* pPlot) const;
	int AI_pillageValue(const CvPlot* pPlot, int iBonusValueThreshold = 0) const;
	int AI_nukeValue(const CvCity* pCity) const;
	bool AI_canPillage(const CvPlot& kPlot) const;

	int AI_searchRange(int iRange) const;
	bool AI_plotValid(const CvPlot* pPlot) const;

	int AI_finalOddsThreshold(const CvPlot* pPlot, int iOddsThreshold) const;

	int AI_stackOfDoomExtra() const;

	bool AI_stackAttackCity(int iRange, int iPowerThreshold, bool bFollow = true);
	bool AI_moveIntoCity(int iRange, bool bOpponentOnly = false);
	bool AI_moveIntoNearestOwnedCity();

	bool AI_groupMergeRange(UnitAITypes eUnitAI, int iRange, bool bBiggerOnly = true, bool bAllowRegrouping = false, bool bIgnoreFaster = false);
	
	bool AI_artistCultureVictoryMove();

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/31/10                              jdog5000        */
/*                                                                                              */
/* War tactics AI                                                                               */
/************************************************************************************************/
	bool AI_choke(int iRange = 1, bool bDefensive = false);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	bool AI_solveBlockageProblem(CvPlot* pDestPlot, bool bDeclareWar);
	
	int AI_calculatePlotWorkersNeeded(CvPlot* pPlot, BuildTypes eBuild);

	int AI_getEspionageTargetValue(CvPlot* pPlot, int iMaxPath, int iMinUsefulValue);

	bool AI_canGroupWithAIType(UnitAITypes eUnitAI) const;
	bool AI_allowGroup(const CvUnit* pUnit, UnitAITypes eUnitAI) const;

	bool AI_workerNeedsDefender(CvPlot* pPlot) const;
	bool AI_workerReleaseDefenderIfNotNeeded() const;
	bool processContracts(int iMinPriority = 0);
	void contractFulfilled();


	bool AI_approximatePath(CvPlot* pToPlot, int iFlags, int* piPathTurns) const;
	CvUnitAI* AI_cityConstructionTargeted(CvCity* pCity, BuildingTypes eBuilding, CvSelectionGroup* omitGroup) const;

	// added so under cheat mode we can call protected functions for testing
	friend class CvGameTextMgr;

public:
/************************************************************************************************/
/* Afforess	                  Start		 6/20/10                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	bool AI_hurryFood();
	bool AI_claimForts(CvReachablePlotSet* pReachablePlots, int iMinValue = 0, int iMaxPath = -1);
	BuildTypes AI_findBestFort(CvPlot* pPlot) const;
	bool AI_StrategicForts();
	bool AI_caravan(bool bAnyCity = false);
	bool AI_command();
	bool AI_AutomatedPillage(int iBonusValueThreshold);
	void AI_SearchAndDestroyMove(bool bWithCommander = false);
	bool AI_huntRange(int iRange, int iOddsThreshold, bool bStayInBorders = false,  int iMinValue = 0);
	void AI_cityDefense();
	void AI_borderPatrol();
	bool AI_returnToBorders();
	bool AI_patrolBorders();
	bool AI_moveToBorders();
	void AI_AutomatedpillageMove();
	void AI_autoAirStrike();
	bool AI_airBombCities();
	bool AI_moveToTarget(CvUnit* pTarget);
	void AI_shadowMove();
	bool AI_protectTarget(CvUnit* pTarget);
	bool AI_joinMilitaryCity(bool bNaval = false);
	bool AI_isPlotWellDefended(CvPlot* pPlot, bool bIncludeAdjacent, int iOddsOfDefeat);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	int	AI_genericUnitValueTimes100(UnitValueFlags eFlags) const;
	void AI_flushValueCache();

	int scoreCityHealerNeed(const UnitCombatTypes eUnitCombat, const DomainTypes eDomain, const CvCity* city) const;

public:
	virtual int AI_getPredictedHitPoints() const;
	virtual void AI_setPredictedHitPoints(int iPredictedHitPoints);
	virtual bool AI_getHasAttacked();
	virtual int AI_beneficialPropertyValueToCity(CvCity* pCity, PropertyTypes eProperty) const;

	//	KOSHLING - inform the AI of unit losses so that it can adjust internal counts
	virtual void AI_killed();

	//	Set an override to the group head priority for this unit, to allow a unit that
	//	would not nomally lead a stack to do so - priority will be preserved when other groups
	//	joing one lead by a unit with an override, but reset if we join another group
	virtual void AI_setLeaderPriority(int iPriority);	//	 -1 means reset to default

	bool AI_fulfillHealerNeed(CvPlot* pPlot);
	bool AI_fulfillImmediateHealerNeed(CvPlot* pPlot);
	bool AI_fulfillCityHealerNeed(CvPlot* pPlot);

	bool AI_fulfillPropertyControlNeed();

	bool AI_arrest();
	bool AI_ambush(int iOddsThreshold, bool bAssassinationOnly = false);

	bool AI_activateStatus(bool bChange, bool bStack, PromotionTypes eStatus, CvUnit* pUnit = NULL);
	bool AI_selectStatus(bool bStack, CvUnit* pUnit = NULL);
	bool AI_groupSelectStatus();

	bool AI_InvestigatorFulfillment();
	bool AI_establishStackSeeInvisibleCoverage();

	bool generateSafePathforVulnerable(const CvPlot* pToPlot, int* piPathTurns = NULL) const;

	void setToWaitOnUnitAI(UnitAITypes eUnitAI, bool bAdd);
	bool isWaitingOnUnitAI(int iIndex);
	bool isWaitingOnUnitAIAny();
	void setWaitingOnUnitAIAny();
	bool AI_isNegativePropertyUnit();
	int getMyAggression(int iAttackProb) const;

	//	Check whether a plot is dangerous for the unit (alone) with provided acceptable
	//	survival odds
	bool exposedToDanger(const CvPlot* pPlot, int acceptableOdds, bool bConsiderOnlyWorstThreat = false) const;
	bool getThreateningUnit(const CvPlot* pPlot, CvUnit*& pThreateningUnit, const CvPlot* pAttackPlot, int& iIndex, bool bReturnWorstOfMultiple = false) const;

	static void	AI_clearCaches();

	static PlayerTypes			m_cachedPlayer;
	static CvReachablePlotSet*	m_cachedMissionaryPlotset;

public:

	//
	// Algorithm/range helpers
	//
	struct fn {
	}; 
};

#endif
