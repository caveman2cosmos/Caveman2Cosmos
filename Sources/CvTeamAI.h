#pragma once

// teamAI.h

#ifndef CIV4_TEAM_AI_H
#define CIV4_TEAM_AI_H

#include "CvTeam.h"

class CvArea;
class CvCity;

class CvTeamAI : public CvTeam
{
public:
	CvTeamAI();
	virtual ~CvTeamAI();

	DllExport static CvTeamAI& getTeam(TeamTypes eTeam);

	static void initStatics();
	static void freeStatics();

	void AI_init();
	void AI_uninit();
	void AI_reset(bool bConstructor);

	void AI_doTurnPre();

	/**
	 * \brief Perform post-turn AI actions for the team.
	 *
	 * This function is called at the end of the team's turn to update
	 * strategic evaluations and possibly execute war actions.
	 *
	 * The sequence of operations is as follows:
	 * 1. Update the team's worst enemy based on current game state.
	 * 2. Update area strategies, without forcing updates on all areas.
	 * 3. If the team is human-controlled, NPC, or a minor civilization, exit early.
	 * 4. Execute war-related AI decisions if applicable.
	 *
	 * \note
	 * - Calls `AI_updateWorstEnemy()` to refresh the enemy evaluation.
	 * - Calls `AI_updateAreaStragies(false)` to update area-specific strategies.
	 * - Calls `AI_doWar()` to carry out attacks or military moves for the team.
	 *
	 * \see CvTeamAI::AI_updateWorstEnemy()
	 * \see CvTeamAI::AI_updateAreaStragies()
	 * \see CvTeamAI::AI_doWar()
	 *
	 * \dot
		digraph AI_doTurnPost {
			rankdir=TB;  // vertical layout
			bgcolor="#ffffff";  // white background

			// Default node style
			node [shape=box, style=filled, fontname="Arial", fontsize=10, color="#555555", fillcolor="#f9f9f9", penwidth=1.5];

			// Default edge style
			edge [color="#5555aa", penwidth=1.5, arrowsize=1];

			// Nodes
			Start [label="Start AI_doTurnPost", fillcolor="#cde8ff"];
			UpdateWorstEnemy [label="AI_updateWorstEnemy()"];
			UpdateAreaStrategies [label="AI_updateAreaStragies(false)"];
			CheckHumanNPCMinor [label="isHuman() || isNPC() || isMinorCiv?", shape=diamond, fillcolor="#ffd7d7"];
			ReturnIfHuman [label="Return (skip AI war)", fillcolor="#eeeeee"];
			DoWar [label="AI_doWar()", fillcolor="#d7ffd7"];
			End [label="End AI_doTurnPost", fillcolor="#cde8ff"];

			// Flow
			Start -> UpdateWorstEnemy -> UpdateAreaStrategies -> CheckHumanNPCMinor;
			CheckHumanNPCMinor -> ReturnIfHuman [label="Yes"];
			CheckHumanNPCMinor -> DoWar [label="No"];
			ReturnIfHuman -> End;
			DoWar -> End;
		}
	 * \enddot
	 */
	void AI_doTurnPost();

	void AI_makeAssignWorkDirty();

	int AI_getOurPlotStrength(const CvPlot* pPlot, const int iRange, const bool bDefensiveBonuses, const bool bTestMoves, const bool bIncludeVassals = false) const;

	void AI_updateAreaStragies(const bool bTargets = true);
	void AI_updateAreaTargets();

	int AI_countMilitaryWeight(const CvArea* pArea) const;

	bool AI_isAnyCapitalAreaAlone() const;
	bool AI_isPrimaryArea(const CvArea* pArea) const;
	bool AI_hasCitiesInPrimaryArea(const TeamTypes eTeam) const;
	AreaAITypes AI_calculateAreaAIType(const CvArea* pArea, const bool bPreparingTotal = false) const;

	int AI_calculateAdjacentLandPlots(const TeamTypes eTeam) const;
	int AI_calculateCapitalProximity(TeamTypes eTeam) const;
	int AI_calculatePlotWarValue(const TeamTypes eTeam) const;
	int AI_calculateBonusWarValue(TeamTypes eTeam) const;

	bool AI_isLandTarget(TeamTypes eTeam, bool bNeighborsOnly = false) const;
	bool AI_isAllyLandTarget(TeamTypes eTeam) const;
	bool AI_shareWar(TeamTypes eTeam) const;

	AttitudeTypes AI_getAttitude(TeamTypes eTeam, bool bForced = true) const;
	int AI_getAttitudeVal(TeamTypes eTeam, bool bForced = true) const;
	int AI_getMemoryCount(TeamTypes eTeam, MemoryTypes eMemory) const;

	int AI_chooseElection(const VoteSelectionData& kVoteSelectionData) const;

	int AI_startWarVal(TeamTypes eTeam) const;
	int AI_endWarVal(TeamTypes eTeam) const;

	bool AI_minorPeaceOut(TeamTypes eTeam) const;
	int AI_getBarbarianCivWarVal(TeamTypes eTeam, int iMaxDistance = 10) const;

	int AI_techTradeVal(TechTypes eTech, TeamTypes eTeam);
	DenialTypes AI_techTrade(const TechTypes eTech, const TeamTypes eTeam) const;

	int AI_mapTradeVal(TeamTypes eTeam) const;
	DenialTypes AI_mapTrade(TeamTypes eTeam) const;

	int AI_vassalTradeVal(TeamTypes eTeam) const;
	DenialTypes AI_vassalTrade(TeamTypes eTeam) const;

	int AI_surrenderTradeVal(TeamTypes eTeam) const;
	DenialTypes AI_surrenderTrade(TeamTypes eTeam, int iPowerMultiplier = 100) const;

	bool AI_isAnyMemberDoVictoryStrategy( int iVictoryStrategy ) const;
	bool AI_isAnyMemberDoVictoryStrategyLevel4() const;
	bool AI_isAnyMemberDoVictoryStrategyLevel3() const;

	int AI_getWarSuccessCapitulationRatio() const;
	int AI_getEnemyPowerPercent( bool bConsiderOthers = false ) const;
	int AI_getRivalAirPower( ) const;
	bool AI_acceptSurrender(const TeamTypes eSurrenderTeam) const;
	bool AI_isOkayVassalTarget(const TeamTypes eTeam) const;

	void AI_getWarRands( int &iMaxWarRand, int &iLimitedWarRand, int &iDogpileWarRand ) const;
	void AI_getWarThresholds( int &iMaxWarThreshold, int &iLimitedWarThreshold, int &iDogpileWarThreshold ) const;
	int AI_getTotalWarOddsTimes100( ) const;

	int AI_makePeaceTradeVal(TeamTypes ePeaceTeam, TeamTypes eTeam) const;
	DenialTypes AI_makePeaceTrade(TeamTypes ePeaceTeam, TeamTypes eTeam) const;

	int AI_declareWarTradeVal(TeamTypes eWarTeam, TeamTypes eTeam) const;
	DenialTypes AI_declareWarTrade(TeamTypes eWarTeam, TeamTypes eTeam, bool bConsiderPower = true) const;

	int AI_openBordersTradeVal(TeamTypes eTeam) const;
	DenialTypes AI_openBordersTrade(TeamTypes eTeam) const;

	int AI_defensivePactTradeVal(TeamTypes eTeam) const;
	DenialTypes AI_defensivePactTrade(TeamTypes eTeam) const;

	DenialTypes AI_permanentAllianceTrade(TeamTypes eTeam) const;

	TeamTypes AI_getWorstEnemy() const;
	void AI_updateWorstEnemy();

	int AI_getWarPlanStateCounter(TeamTypes eIndex) const;
	void AI_setWarPlanStateCounter(TeamTypes eIndex, int iNewValue);
	void AI_changeWarPlanStateCounter(TeamTypes eIndex, int iChange);

	int AI_getAtWarCounter(TeamTypes eIndex) const;
	void AI_setAtWarCounter(TeamTypes eIndex, int iNewValue);
	void AI_changeAtWarCounter(TeamTypes eIndex, int iChange);

	int AI_getAtPeaceCounter(TeamTypes eIndex) const;
	void AI_setAtPeaceCounter(TeamTypes eIndex, int iNewValue);
	void AI_changeAtPeaceCounter(TeamTypes eIndex, int iChange);

	int AI_getHasMetCounter(TeamTypes eIndex) const;
	void AI_setHasMetCounter(TeamTypes eIndex, int iNewValue);
	void AI_changeHasMetCounter(TeamTypes eIndex, int iChange);

	int AI_getOpenBordersCounter(TeamTypes eIndex) const;
	void AI_setOpenBordersCounter(TeamTypes eIndex, int iNewValue);
	void AI_changeOpenBordersCounter(TeamTypes eIndex, int iChange);

	int AI_getDefensivePactCounter(TeamTypes eIndex) const;
	void AI_setDefensivePactCounter(TeamTypes eIndex, int iNewValue);
	void AI_changeDefensivePactCounter(TeamTypes eIndex, int iChange);

	int AI_getShareWarCounter(TeamTypes eIndex) const;
	void AI_setShareWarCounter(TeamTypes eIndex, int iNewValue);
	void AI_changeShareWarCounter(TeamTypes eIndex, int iChange);

	int AI_getWarSuccess(TeamTypes eIndex) const;
	void AI_setWarSuccess(TeamTypes eIndex, int iNewValue);
	void AI_changeWarSuccess(TeamTypes eIndex, int iChange);

	int AI_getEnemyPeacetimeTradeValue(TeamTypes eIndex) const;
	void AI_setEnemyPeacetimeTradeValue(TeamTypes eIndex, int iNewValue);
	void AI_changeEnemyPeacetimeTradeValue(TeamTypes eIndex, int iChange);

	int AI_getEnemyPeacetimeGrantValue(TeamTypes eIndex) const;
	void AI_setEnemyPeacetimeGrantValue(TeamTypes eIndex, int iNewValue);
	void AI_changeEnemyPeacetimeGrantValue(TeamTypes eIndex, int iChange);

	WarPlanTypes AI_getWarPlan(TeamTypes eIndex) const;
	bool AI_isChosenWar(TeamTypes eIndex) const;
	bool AI_isSneakAttackPreparing(TeamTypes eIndex) const;
	bool AI_isSneakAttackReady(TeamTypes eIndex) const;
	void AI_setWarPlan(TeamTypes eIndex, WarPlanTypes eNewValue, bool bWar = true, bool bInFull = true);

	int AI_teamCloseness(TeamTypes eIndex, int iMaxDistance = -1) const;

	bool AI_performNoWarRolls(TeamTypes eTeam);

	int AI_getAttitudeWeight(const TeamTypes eTeam) const;

	int AI_getLowestVictoryCountdown() const;

	int AI_getTechMonopolyValue(TechTypes eTech, TeamTypes eTeam) const;

	bool AI_isWaterAreaRelevant(const CvArea* pArea) const;

	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

	int AI_embassyTradeVal(TeamTypes eTeam) const;
	int AI_LimitedBordersTradeVal(TeamTypes eTeam) const;
	int AI_FreeTradeAgreementVal(TeamTypes eTeam) const;
    int AI_contactTradeVal(TeamTypes eContactTeam, TeamTypes eTeam) const;

	DenialTypes AI_embassyTrade(TeamTypes eTeam) const;
	DenialTypes AI_LimitedBordersTrade(TeamTypes eTeam) const;
	DenialTypes AI_contactTrade(TeamTypes eContactTeam, TeamTypes eTeam) const;
	DenialTypes AI_FreeTradeAgreement(TeamTypes eTeam) const;

	void AI_updateDiploCache();

	bool AI_hasAdjacentLandPlots(TeamTypes eTeam) const;


protected:

	static CvTeamAI* m_aTeams;

	TeamTypes m_eWorstEnemy;

	int* m_aiWarPlanStateCounter;
	int* m_aiAtWarCounter;
	int* m_aiAtPeaceCounter;
	int* m_aiHasMetCounter;
	int* m_aiOpenBordersCounter;
	int* m_aiDefensivePactCounter;
	int* m_aiShareWarCounter;
	int* m_aiWarSuccess;
	int* m_aiEnemyPeacetimeTradeValue;
	int* m_aiEnemyPeacetimeGrantValue;

	WarPlanTypes* m_aeWarPlan;

	int AI_noWarAttitudeProb(AttitudeTypes eAttitude) const;

	void AI_doCounter();
	void AI_doWar();

	int m_iNoTechTradeThreshold;
	int m_iTechTradeKnownPercent;
	int m_iMaxWarRand;
	int m_iMaxWarNearbyPowerRatio;
	int m_iMaxWarDistantPowerRatio;
	int m_iMaxWarMinAdjacentLandPercent;
	int m_iLimitedWarRand;
	int m_iLimitedWarPowerRatio;
	int m_iDogpileWarRand;
	int m_iMakePeaceRand;

	int AI_noTechTradeThreshold(bool bRecalculate = false) const;
	int AI_techTradeKnownPercent(bool bRecalculate = false) const;
	int AI_maxWarRand(bool bRecalculate = false) const;
	int AI_maxWarNearbyPowerRatio(bool bRecalculate = false) const;
	int AI_maxWarDistantPowerRatio(bool bRecalculate = false) const;
	int AI_maxWarMinAdjacentLandPercent(bool bRecalculate = false) const;
	int AI_limitedWarRand(bool bRecalculate = false) const;
	int AI_limitedWarPowerRatio(bool bRecalculate = false) const;
	int AI_dogpileWarRand(bool bRecalculate = false) const;
	int AI_makePeaceRand(bool bRecalculate = false) const;

	// added so under cheat mode we can call protected functions for testing
	friend class CvGameTextMgr;
	friend class CvDLLWidgetData;

	mutable int m_endWarValueCache[MAX_TEAMS];

	mutable stdext::hash_map<int,int> m_tradeTechValueCache;
	mutable int	m_tradeTechValuesCachedTurn;
};

// helper for accessing static functions
#define GET_TEAM CvTeamAI::getTeam

#endif
