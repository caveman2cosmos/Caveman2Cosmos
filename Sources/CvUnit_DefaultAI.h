#pragma once

// unitAI.h

#ifndef CV_UNIT_DEFAULT_AI_H
#define CV_UNIT_DEFAULT_AI_H

#include "ICvUnitAI.h"
#include "CvUnit.h"
#include "GroupingParams.h"

class CvCity;
class CvPlot;
class CvReachablePlotSet;
class CvSelectionGroup;
class ConstructionNeeds;

///
/// Internal components of CvUnit (which should rather not be used outside it, unless you can't find another solution)
/// 
namespace CvUnitNS
{
	typedef enum
	{
		CONTRACTUAL_STATE_NONE,
		CONTRACTUAL_STATE_AWAITING_ANSWER,
		CONTRACTUAL_STATE_AWAITING_WORK,
		CONTRACTUAL_STATE_FOUND_WORK,
		CONTRACTUAL_STATE_NO_WORK_FOUND
	} ContractualState;

	class DefaultAI : public ICvUnitAI
	{
	public:
		DefaultAI(CvUnit* unit);
		virtual ~DefaultAI();

		DefaultAI& operator=(const DefaultAI& other);

		void init(UnitAITypes eUnitAI, int iBirthmark);
		void uninit();
		void reset(UnitAITypes eUnitAI = NO_UNITAI, bool bConstructorCall = false);
		void SendLog(CvWString function, CvWString message);

		bool update();
		bool follow();

		bool load(UnitAITypes eUnitAI, MissionAITypes eMissionAI, UnitAITypes eTransportedUnitAI = NO_UNITAI, int iMinCargo = -1, int iMinCargoSpace = -1, int iMaxCargoSpace = -1, int iMaxCargoOurUnitAI = -1, int iFlags = 0, int iMaxPath = MAX_INT, int iMaxTransportPath = MAX_INT);

		bool upgrade(); // override
		bool promote(); // override

#define	LEADER_PRIORITY_MIN 0
#define	LEADER_PRIORITY_MAX	100

		int groupFirstVal() const;
		int groupSecondVal() const;

		int attackOdds(const CvPlot* pPlot, bool bPotentialEnemy, CvUnit** ppDefender = NULL, bool bAssassinate = false);
		//	Variant to test a specific defender AS IF it was in the specified plot
		int attackOddsAtPlot(const CvPlot* pPlot, CvUnit* pDefender, bool modifyPredictedResults = false);
		int attackOddsAtPlotInternal(const CvPlot* pPlot, CvUnit* pDefender, bool modifyPredictedResults);

		bool bestCityBuild(const CvCity* pCity, CvPlot** ppBestPlot = NULL, BuildTypes* peBestBuild = NULL, const CvPlot* pIgnorePlot = NULL, const CvUnit* pUnit = NULL);

		bool isCityAIType() const;

		int getBirthmark() const;
		void setBirthmark(int iNewValue);

		UnitAITypes getUnitAIType() const;
		void setUnitAIType(UnitAITypes eNewValue);

		int sacrificeValue(const CvPlot* pPlot) const;

		bool isAwaitingContract() const;
		bool isCityGarrison(const CvCity* pCity) const;
		void setAsGarrison(const CvCity* pCity = NULL);
		int searchRange(int iRange = 1) const;

		BuildingTypes getIntendedConstructBuilding() const { return m_eIntendedConstructBuilding; };

		void read(FDataStreamBase* pStream);
		void write(FDataStreamBase* pStream);

	protected:

		CvUnit* m_unit;

		int m_iBirthmark;

		UnitAITypes m_eUnitAIType;

		int m_iAutomatedAbortTurn;

		int m_contractsLastEstablishedTurn;
		ContractualState m_contractualState;

		int m_iGarrisonCity;
		int m_iAffirmedGarrisonCity;

		BuildingTypes m_eIntendedConstructBuilding; // Used to coordinate subdued animal and great person builds
		static ConstructionNeeds* m_constructionNeeds;

		int m_iGroupLeadOverride;
		int m_iPredictedHitPoints;
		bool m_bHasAttacked;
		bool m_bWaitingOnUnitAIAny;

		mutable int m_iGenericValue;
		mutable int m_eGenericValueFlagsCached;

		std::vector<int> m_aiWaitingOnUnitAITypes;

		void doUnitAIMove();

		void animalMove();
		bool SettleFirstCity();
		void settleMove();
		int minSettlerDefense() const;
		bool IsAbroad();
		int GetNumberOfUnitsInGroup();
		bool upgradeWorker();
		void workerMove();
		void barbAttackMove();
		void attackMove();
		void attackCityMove();
		void attackCityLemmingMove();
		void collateralMove();
		void pillageMove();
		void reserveMove();
		void counterMove();
		void pillageCounterMove();
		void paratrooperMove();
		void cityDefenseMove();
		void cityDefenseExtraMove();
		void exploreMove();
		void hunterEscortMove();
		void missionaryMove();
		void prophetMove();
		void artistMove();
		void scientistMove();
		void generalMove();
		void greatHunterMove();
		void greatAdmiralMove();
		void merchantMove();
		void engineerMove();
		void subduedAnimalMove();
		void spyMove();
		void ICBMMove();
		void workerSeaMove();
		void barbAttackSeaMove();
		void pirateSeaMove();
		void attackSeaMove();
		void reserveSeaMove();
		void escortSeaMove();
		void exploreSeaMove();
		void assaultSeaMove();
		void settlerSeaMove();
		void missionarySeaMove();
		void spySeaMove();
		void carrierSeaMove();
		void missileCarrierSeaMove();
		void attackAirMove();
		void defenseAirMove();
		void carrierAirMove();
		void missileAirMove();
		void healerMove();
		void healerSeaMove();
		void propertyControlMove();
		void propertyControlSeaMove();
		void BarbCriminalMove();
		void InvestigatorMove();
		void InfiltratorMove();
		void SeeInvisibleMove();
		void SeeInvisibleSeaMove();
		void EscortMove();
		bool retreatIfCantDefend();
		void networkAutomated();
		void cityAutomated();

		int promotionValue(PromotionTypes ePromotion) const;

		bool shadow(UnitAITypes eUnitAI, int iMax = -1, int iMaxRatio = -1, bool bWithCargoOnly = true, bool bOutsideCityOnly = false, int iMaxPath = MAX_INT);

		bool group(const GroupingParams& params);

		//bool AI_load(UnitAITypes eUnitAI, MissionAITypes eMissionAI, UnitAITypes eTransportedUnitAI = NO_UNITAI, int iMinCargo = -1, int iMinCargoSpace = -1, int iMaxCargoSpace = -1, int iMaxCargoOurUnitAI = -1, int iFlags = 0, int iMaxPath = MAX_INT, int iMaxTransportPath = MAX_INT);

		bool guardCityBestDefender();
		bool guardCityMinDefender(bool bSearch = true);
		bool guardCity(bool bLeave = false, bool bSearch = false, int iMaxPath = MAX_INT);
		bool guardCityAirlift();
		bool guardBonus(int iMinValue = 0);
		int getPlotDefendersNeeded(const CvPlot* pPlot, int iExtra) const;
		bool guardFort(bool bSearch = true);

		bool guardFortMinDefender(bool bSearch = true);

		bool guardCitySite();
		bool guardSpy(int iRandomPercent);

		bool chokeDefend();
		bool heal(int iDamagePercent = 0, int iMaxPath = MAX_INT);
		bool afterAttack();

		bool foundReligion();
#ifdef OUTBREAKS_AND_AFFLICTIONS
		bool cureAffliction(PromotionLineTypes eAfflictionLine);
#endif
		bool goldenAge();
		bool spreadReligion();
		bool spreadCorporation();
		bool spreadReligionAirlift();
		bool spreadCorporationAirlift();
		bool discover(const bool bFirstResearchOnly = false);

		bool leadLegend();

		bool lead(std::vector<UnitAITypes>& aeAIUnitTypes);
		bool join(int iMaxCount = MAX_INT);
		bool construct(int iMaxCount = MAX_INT, int iMaxSingleBuildingCount = MAX_INT, int iThreshold = 15,
			bool bDecayProbabilities = false, bool assumeSameValueEverywhere = false);
		bool outcomeMission();
		bool scrapSubdued();
		bool moveToOurTerritory(int maxMoves);
		bool switchHurry();
		bool hurry(bool bAny = false);
		bool checkSwitchToConstruct();
		int getBestConstructValue(
			int iMaxCount, int iMaxSingleBuildingCount, int iDecayProbabilityRate, int iThreshold,
			bool assumeSameValueEverywhere, CvPlot*& bestConstructPlot, CvPlot*& bestTurnPlot,
			CvUnit*& pBestTargetingUnit, BuildingTypes& eBestBuilding);
		bool enactConstruct(CvPlot* pBestConstructPlot, CvPlot* pBestPlot, CvUnit* eBestTargetingUnit,
			BuildingTypes eBestBuilding);

		bool doInquisition();

		bool greatWork();
		bool offensiveAirlift();
		bool paradrop(int iRange);

		bool protect(int iOddsThreshold, int iMaxPathTurns = MAX_INT);

		bool seaAreaAttack();
		bool patrol(bool bIgnoreDanger = false);
		bool defend();
		bool safety(int iRange = 1);
		bool reachHome(const bool bMockRun = false) const;
		bool hide();
		bool goody(int iRange);

		bool explorerJoinOffensiveStacks();
		bool explore();
		bool exploreRange(int iRange);
		bool refreshExploreRange(int iRange, bool bIncludeVisibilityRefresh = true);

		CvCity* pickTargetCity(int iFlags = 0, int iMaxPath = MAX_INT, bool bHuntBarbs = false);
		bool goToTargetCity(int iFlags = 0, int iMaxPath = MAX_INT, const CvCity* pTargetCity = NULL);
		bool goToTargetBarbCity(int iMaxPath = 10);
		bool pillageAroundCity(const CvCity* pTargetCity, int iBonusValueThreshold = 0, int iMaxPathTurns = MAX_INT);
		bool bombardCity();
		bool cityAttack(int iRange, int iOddsThreshold, bool bFollow = false);
		bool anyAttack(int iRange, int iOddsThreshold, int iMinStack = 0, bool bAllowCities = true, bool bFollow = false);

		bool RbombardPlot(int iRange, int iBonusValueThreshold); // RevolutionDCM
		bool RbombardUnit(int iRange, int iHighestOddsThreshold, int iMinStack, int iSeigeDiff, int iPowerThreshold, bool bCity = false);
		bool RbombardCity(const CvCity* pCity);
		bool RbombardNaval();

		bool FEngage();

		bool rangeAttack(int iRange);
		bool leaveAttack(int iRange, int iThreshold, int iStrengthThreshold);
		bool blockade();
		int  blockadeValue(const CvPlot* pLoopPlot, const CvCity* pCity, CvPlot*& endTurnPlot) const;
		bool pirateBlockade();
		bool seaBombardRange(int iMaxRange);
		bool pillage(int iBonusValueThreshold = 0);
		bool pillageRange(int iRange, int iBonusValueThreshold = 0);
		bool found();
		bool foundRange(int iRange, bool bFollow = false);
		bool assaultSeaTransport(bool bBarbarian = false);

		bool assaultSeaReinforce(bool bBarbarian = false);

		bool settlerSeaTransport();
		bool settlerSeaFerry();
		bool specialSeaTransportMissionary();
		bool specialSeaTransportSpy();
		bool carrierSeaTransport();
		bool connectPlot(CvPlot* pPlot, int iRange = 0);
		bool improveCity(CvCity* pCity);
		bool improveLocalPlot(int iRange, const CvCity* pIgnoreCity);
		bool nextCityToImprove(CvCity* pCity);
		bool nextCityToImproveAirlift();
		bool irrigateTerritory();
		bool fortTerritory(bool bCanal, bool bAirbase);
		bool improveBonus(int iMinValue = 0, CvPlot** ppBestPlot = NULL, BuildTypes* peBestBuild = NULL, int* piBestValue = NULL);
		bool improvePlot(CvPlot* pPlot, BuildTypes eBuild);
		BuildTypes betterPlotBuild(const CvPlot* pPlot, BuildTypes eBuild) const;
		bool connectBonus(bool bTestTrade = true);
		bool connectCity();
		bool routeCity();
		bool routeTerritory(bool bImprovementOnly = false);
		bool travelToUpgradeCity();
		bool retreatToCity(bool bPrimary = false, bool bAirlift = false, int iMaxPath = MAX_INT);

		bool pickup(UnitAITypes eUnitAI, bool bCountProduction = false, int iMaxPath = MAX_INT);
		bool pickupStranded(UnitAITypes eUnitAI = NO_UNITAI, int iMaxPath = MAX_INT);

		bool airOffensiveCity();
		bool airDefensiveCity();
		bool airCarrier();
		bool missileLoad(UnitAITypes eTargetUnitAI, int iMaxOwnUnitAI = -1, bool bStealthOnly = false);
		bool airStrike();

		int airOffenseBaseValue(const CvPlot* pPlot) const;
		bool defensiveAirStrike();
		bool defendBaseAirStrike();

		bool airBombPlots();
		bool airBombDefenses();
		bool exploreAir();

		int exploreAirPlotValue(const CvPlot* pPlot) const;
		bool exploreAir2();
		void exploreAirMove();

		bool nuke();
		bool nukeRange(int iRange);
		bool trade(int iValueThreshold);
		bool infiltrate();
		bool reconSpy(int iRange);
		bool revoltCitySpy();
		bool bonusOffenseSpy(int iMaxPath);
		bool cityOffenseSpy(const int iRange, const CvCity* pSkipCity = NULL);
		bool espionageSpy();
		bool moveToStagingCity();
		//bool AI_seaRetreatFromCityDanger();
		//bool AI_airRetreatFromCityDanger();
		//bool AI_airAttackDamagedSkip();

		bool followBombard();

		bool potentialEnemy(TeamTypes eTeam, const CvPlot* pPlot = NULL);

		bool defendPlot(const CvPlot* pPlot) const;
		int pillageValue(const CvPlot* pPlot, int iBonusValueThreshold = 0) const;
		int nukeValue(const CvCity* pCity) const;
		bool canPillage(const CvPlot& kPlot) const;

		bool plotValid(const CvPlot* pPlot) const;

		int finalOddsThreshold(const CvPlot* pPlot, int iOddsThreshold) const;

		int stackOfDoomExtra() const;

		bool stackAttackCity(int iRange, int iPowerThreshold, bool bFollow = true);
		bool moveIntoCity(int iRange, bool bOpponentOnly = false);
		bool moveIntoNearestOwnedCity();

		bool groupMergeRange(UnitAITypes eUnitAI, int iRange, bool bBiggerOnly = true, bool bAllowRegrouping = false, bool bIgnoreFaster = false);

		bool artistCultureVictoryMove();

		bool choke(int iRange = 1, bool bDefensive = false);

		bool solveBlockageProblem(const CvPlot* pDestPlot, bool bDeclareWar);

		int calculatePlotWorkersNeeded(const CvPlot* pPlot, BuildTypes eBuild) const;

		int getEspionageTargetValue(const CvPlot* pPlot, int iMaxPath, int iMinUsefulValue) const;

		bool canGroupWithAIType(UnitAITypes eUnitAI) const;
		bool allowGroup(const CvUnit* pUnit, UnitAITypes eUnitAI) const;

		bool workerNeedsDefender(const CvPlot* pPlot) const;
		bool workerReleaseDefenderIfNotNeeded() const;
		bool processContracts(int iMinPriority = 0);
		void contractFulfilled();

		//bool AI_approximatePath(CvPlot* pToPlot, int iFlags, int* piPathTurns) const;
		//CvUnitAI* AI_cityConstructionTargeted(const CvCity* pCity, BuildingTypes eBuilding, const CvSelectionGroup* omitGroup) const;

		// added so under cheat mode we can call protected functions for testing
		friend class CvGameTextMgr;

	public:
		bool hurryFood();
		bool claimForts(CvReachablePlotSet* pReachablePlots, int iMinValue = 0, int iMaxPath = -1);
		BuildTypes findBestFort(const CvPlot* pPlot) const;
		bool StrategicForts();
		bool caravan(bool bAnyCity = false);
		bool command();
		bool AutomatedPillage(int iBonusValueThreshold);
		void SearchAndDestroyMove(bool bWithCommander = false);
		bool huntRange(int iRange, int iOddsThreshold, bool bStayInBorders = false, int iMinValue = 0);
		void cityDefense();
		void borderPatrol();
		bool returnToBorders();
		bool patrolBorders();
		bool moveToBorders();
		void AutomatedpillageMove();
		void autoAirStrike();
		bool airBombCities();
		bool moveToTarget(const CvUnit* pTarget);
		void shadowMove();
		bool protectTarget(const CvUnit* pTarget);
		bool joinMilitaryCity(bool bNaval = false);
		bool isPlotWellDefended(const CvPlot* pPlot, bool bIncludeAdjacent, int iOddsOfDefeat) const;

		int genericUnitValueTimes100(UnitValueFlags eFlags) const;
		void flushValueCache();

		int scoreCityHealerNeed(const UnitCombatTypes eUnitCombat, const DomainTypes eDomain, const CvCity* city) const;

	public:
		virtual int getPredictedHitPoints() const;
		virtual void setPredictedHitPoints(int iPredictedHitPoints);
		virtual bool getHasAttacked() const;
		virtual int beneficialPropertyValueToCity(const CvCity* pCity, PropertyTypes eProperty) const;

		//	KOSHLING - inform the AI of unit losses so that it can adjust internal counts
		virtual void killed();

		//	Set an override to the group head priority for this unit, to allow a unit that
		//	would not nomally lead a stack to do so - priority will be preserved when other groups
		//	joing one lead by a unit with an override, but reset if we join another group
		virtual void setLeaderPriority(int iPriority);	//	 -1 means reset to default

		bool fulfillHealerNeed(const CvPlot* pPlot);
		bool fulfillImmediateHealerNeed(const CvPlot* pPlot);
		bool fulfillCityHealerNeed(const CvPlot* pPlot);

		bool fulfillPropertyControlNeed();

		bool arrest();
		bool ambush(int iOddsThreshold, bool bAssassinationOnly = false);

	private:
		bool activateStatus(bool bStack, PromotionTypes eStatus, CvUnit* pUnit);
	public:
		bool selectStatus(bool bStack, CvUnit* pUnit = NULL);
		bool groupSelectStatus();

		bool InvestigatorFulfillment();
		bool establishStackSeeInvisibleCoverage();

		bool generateSafePathforVulnerable(const CvPlot* pToPlot, int* piPathTurns = NULL) const;

		void setToWaitOnUnitAI(UnitAITypes eUnitAI, bool bAdd);
		bool isWaitingOnUnitAI(int iIndex) const;
		bool isWaitingOnUnitAIAny() const;
		void setWaitingOnUnitAIAny();
		bool isNegativePropertyUnit() const;
		int getMyAggression(int iAttackProb) const;

		bool exposedToDanger(const CvPlot* pPlot, int acceptableOdds, bool bConsiderOnlyWorstThreat = false) const;
		bool getThreateningUnit(const CvPlot* pPlot, CvUnit*& pThreateningUnit, const CvPlot* pAttackPlot, int& iIndex, bool bReturnWorstOfMultiple = false) const;

		static void clearCaches();

		static PlayerTypes			m_cachedPlayer;
		static CvReachablePlotSet* m_cachedMissionaryPlotset;
	};

}

#endif
