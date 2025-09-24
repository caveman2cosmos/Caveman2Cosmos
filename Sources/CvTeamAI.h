#pragma once

// teamAI.h

#ifndef CIV4_TEAM_AI_H
#define CIV4_TEAM_AI_H

#include "CvTeam.h"
#include "CvGameCoreDLL.h"

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
	
	/**
     * Returns true if the area is the team's primary area.
     * - Checks if any team member considers the area as primary.
     */
	bool AI_isPrimaryArea(const CvArea* pArea) const;

    /**
     * Returns true if the specified team has cities in our primary area.
     * - Used for war planning and land target evaluation.
     */
	bool AI_hasCitiesInPrimaryArea(const TeamTypes eTeam) const;

    /**
     * Calculates the AI type for a given area (offensive, defensive, neutral, etc.).
     * - Considers recent attacks, targets, military weight, and strategic options.
     */
	AreaAITypes AI_calculateAreaAIType(const CvArea* pArea, const bool bPreparingTotal = false) const;

    /**
     * Calculates the number of adjacent land plots between this team and another.
     * - Used for evaluating land targets and proximity.
     */
	int AI_calculateAdjacentLandPlots(const TeamTypes eTeam) const;

    /**
     * Calculates the proximity between this team's capital and another team's capital.
     * - Used for war value and strategic planning.
     */
	int AI_calculateCapitalProximity(TeamTypes eTeam) const;

    /**
     * Calculates the war value of plots owned by another team.
     * - Considers adjacency and bonus resources.
     */
	int AI_calculatePlotWarValue(const TeamTypes eTeam) const;

    /**
     * Calculates the war value based on bonuses available in another team's territory.
     * - Used to prioritize targets with valuable resources.
     */
	int AI_calculateBonusWarValue(TeamTypes eTeam) const;

    /**
     * Returns true if the specified team is a land target for this team.
     * - Considers adjacency and shared primary areas.
     */
	bool AI_isLandTarget(TeamTypes eTeam, bool bNeighborsOnly = false) const;

    /**
     * Returns true if the specified team or its allies are land targets.
     * - Used for evaluating war and alliance strategies.
     */
	bool AI_isAllyLandTarget(TeamTypes eTeam) const;

    /**
     * Returns true if this team shares a war with the specified team against a third party.
     * - Used for diplomatic and war planning logic.
     */
	bool AI_shareWar(TeamTypes eTeam) const;

    /**
     * Returns the attitude of this team towards another team.
     * - Used for diplomacy and trade decisions.
     */
	AttitudeTypes AI_getAttitude(TeamTypes eTeam, bool bForced = true) const;

    /**
     * Returns the numeric attitude value towards another team.
     * - Used for detailed diplomatic calculations.
     */
	int AI_getAttitudeVal(TeamTypes eTeam, bool bForced = true) const;

    /**
     * Returns the memory count for a specific diplomatic event with another team.
     * - Used for attitude and trade refusal logic.
     */
	int AI_getMemoryCount(TeamTypes eTeam, MemoryTypes eMemory) const;

    /**
     * Chooses the best election option for the team.
     * - Evaluates vote options and team preferences.
     */
	int AI_chooseElection(const VoteSelectionData& kVoteSelectionData) const;

    /**
     * Computes the value of starting a war against another team.
     * - Considers proximity, bonuses, victory strategies, and attitude.
     */
	int AI_startWarVal(TeamTypes eTeam) const;

    /**
     * Computes the value of ending a war with another team.
     * - Considers cities, population, war success, and strategic factors.
     */
	int AI_endWarVal(TeamTypes eTeam) const;

	/**
     * Returns true if the team should make peace with a minor civ.
     * - Checks if the minor civ has cities in our primary area.
     * - Evaluates team closeness and war success.
     * - Returns true if financial trouble or lack of strategic interest.
     * Steps:
     *   1. Check if minor civ is a valid target (cities in area, closeness, war success).
     *   2. If any team member is in financial trouble, return true.
     *   3. Otherwise, return false.
     */
	bool AI_minorPeaceOut(TeamTypes eTeam) const;

    /**
     * Calculates the war value against a barbarian civ.
     * - Considers attitude, proximity, plot war value, capital proximity, and war success.
     * Steps:
     *   1. Check attitude and cities in primary area.
     *   2. Calculate team closeness.
     *   3. Add values for plot war, capital proximity, and war success.
     *   4. Adjust value based on attitude and power ratios.
     */
	int AI_getBarbarianCivWarVal(TeamTypes eTeam, int iMaxDistance = 10) const;

    /**
     * Calculates the value of trading a technology to another team.
     * - Uses cached values for efficiency.
     * - Considers tech cost, relative value, known teams, and trade modifiers.
     * Steps:
     *   1. Check cache for value.
     *   2. If not cached, calculate base cost and adjust for tech value.
     *   3. Adjust for known teams and monopoly.
     *   4. Store in cache and return.
     */
	int AI_techTradeVal(TechTypes eTech, TeamTypes eTeam);

    /**
     * Determines if a tech trade should be denied.
     * - Checks attitude, tech brokering, war plans, and monopoly.
     * Steps:
     *   1. Check if human, vassal, or at war.
     *   2. Check worst enemy and attitude thresholds.
     *   3. Check tech brokering and monopoly.
     *   4. Check for world units, wonders, and victory projects.
     *   5. Return appropriate denial type.
     */
	DenialTypes AI_techTrade(const TechTypes eTech, const TeamTypes eTeam) const;

    /**
     * Calculates the value of trading a map to another team.
     * - Considers revealed plots, war plans, era, and vassal status.
     * Steps:
     *   1. Loop over all plots and count value for revealed/unrevealed tiles.
     *   2. Adjust value for war plans and era.
     *   3. Adjust for vassal status.
     *   4. Return final value.
     */
	int AI_mapTradeVal(TeamTypes eTeam) const;

    /**
     * Determines if a map trade should be denied.
     * - Checks war plans, attitude, and recent cancellations.
     * Steps:
     *   1. Check if human, vassal, or at war.
     *   2. Check worst enemy and war plans.
     *   3. Check attitude thresholds.
     *   4. Return appropriate denial type.
     */
	DenialTypes AI_mapTrade(TeamTypes eTeam) const;

    /**
     * Calculates the value of trading vassal status.
     * - Uses surrender trade value.
     */
	int AI_vassalTradeVal(TeamTypes eTeam) const;

    /**
     * Determines if a vassal trade should be denied.
     * - Checks for war/peace possibilities and attitude.
     * Steps:
     *   1. Check for war/peace with other teams.
     *   2. Check attitude and victory conditions.
     *   3. Return appropriate denial type.
     */
	DenialTypes AI_vassalTrade(TeamTypes eTeam) const;

    /**
     * Calculates the value of surrendering to another team.
     * - Returns 0 (placeholder).
     */
	int AI_surrenderTradeVal(TeamTypes eTeam) const;

    /**
     * Determines if a surrender trade should be denied.
     * - Checks for war/peace, attitude, and victory conditions.
     * Steps:
     *   1. Check for war/peace with other teams.
     *   2. Check attitude and population/land thresholds.
     *   3. Return appropriate denial type.
     */
    DenialTypes AI_surrenderTrade(TeamTypes eMasterTeam, int iPowerMultiplier = 100) const;

    /**
     * Returns true if any team member is pursuing a specific victory strategy.
     * - Loops through all players and checks strategy.
     */
    bool AI_isAnyMemberDoVictoryStrategy(int iVictoryStrategy) const;

    /**
     * Returns true if any team member is pursuing a level 4 victory strategy.
     */
	bool AI_isAnyMemberDoVictoryStrategyLevel4() const;

    /**
     * Returns true if any team member is pursuing a level 3 victory strategy.
     */
	bool AI_isAnyMemberDoVictoryStrategyLevel3() const;

    /**
     * Computes the war success capitulation ratio.
     * - Sums war success values against all enemies.
     * - Returns a ratio indicating net war success.
     */
	int AI_getWarSuccessCapitulationRatio() const;

    /**
     * Computes the enemy power percent compared to our own.
     * - Sums up enemy and rival power.
     * - Returns percentage.
     */
    int AI_getEnemyPowerPercent(bool bConsiderOthers = false) const;

    /**
     * Sums up rival air power.
     * - Counts enemy and rival air units.
     * - Returns total air power.
     */
    int AI_getRivalAirPower() const;

    /**
     * Determines if the team should accept surrender from another team.
     * - Checks victory strategies, valuable cities, war success, and war weariness.
     * Steps:
     *   1. Check if human or not at war.
     *   2. Check victory strategies and valuable cities.
     *   3. Check war success ratio and war weariness.
     *   4. Return true if surrender should be accepted.
     */
    bool AI_acceptSurrender(TeamTypes eSurrenderTeam) const;

    /**
     * Returns true if the team is a valid vassal target.
     * - Checks if the vassal shares a primary area with the master.
     * Steps:
     *   1. Check if team is a vassal.
     *   2. Check cities in primary area and adjacent land plots.
     *   3. Return true if valid target.
     */
	bool AI_isOkayVassalTarget(const TeamTypes eTeam) const;


    /**
     * Returns the war random values for total war, limited war, and dogpile war.
     * - Used to determine the likelihood of declaring different types of war.
     * Steps:
     *   1. Get base values from team members' personalities.
     *   2. Adjust values based on victory strategies and vassal count.
     *   3. Further adjust for "final war" strategy.
     */
    void AI_getWarRands(int &iMaxWarRand, int &iLimitedWarRand, int &iDogpileWarRand) const;

    /**
     * Returns the war threshold values for total war, limited war, and dogpile war.
     * - Used to compare against war random values to decide on war plans.
     * Steps:
     *   1. Calculate average unit cost percent for the team.
     *   2. Adjust thresholds based on aggressive strategies and victory strategies.
     *   3. Normalize and finalize threshold values.
     */
    void AI_getWarThresholds(int &iMaxWarThreshold, int &iLimitedWarThreshold, int &iDogpileWarThreshold) const;

    /**
     * Returns the odds (times 100) of declaring total war.
     * - Used for AI war planning.
     * Steps:
     *   1. Get war randoms and thresholds.
     *   2. Calculate odds as threshold divided by random value.
     */
    int AI_getTotalWarOddsTimes100() const;

    /**
     * Calculates the value of making peace between two teams.
     * - Used for trade evaluation.
     * Steps:
     *   1. Base value on turn number and city count.
     *   2. Adjust for attitude between teams.
     *   3. Adjust for war duration and other modifiers.
     */
	int AI_makePeaceTradeVal(TeamTypes ePeaceTeam, TeamTypes eTeam) const;

    /**
     * Determines if a peace trade should be denied.
     * - Checks for human, vassal status, war plan, and land ratio.
     * Steps:
     *   1. Check if peace is possible.
     *   2. Compare endWarVal between teams.
     *   3. Check land ratio for domination.
     *   4. Return denial type.
     */
	DenialTypes AI_makePeaceTrade(TeamTypes ePeaceTeam, TeamTypes eTeam) const;

    /**
     * Calculates the value of declaring war on another team for trade purposes.
     * - Used for diplomatic trade evaluation.
     * Steps:
     *   1. Base value on city and population count.
     *   2. Adjust for attitude, power, and war status.
     *   3. Add modifiers for nukes and defensive pacts.
     *   4. Finalize value with turn and game progress.
     */
	int AI_declareWarTradeVal(TeamTypes eWarTeam, TeamTypes eTeam) const;

    /**
     * Determines if a declare war trade should be denied.
     * - Checks for vassal/defensive pacts, power, attitude, and nuclear deterrent.
     * Steps:
     *   1. Check for vassal/defensive pact relationships.
     *   2. Check if war can be declared.
     *   3. Evaluate attitude and power ratios.
     *   4. Check for nuclear deterrent.
     *   5. Return denial type.
     */
	DenialTypes AI_declareWarTrade(TeamTypes eWarTeam, TeamTypes eTeam, bool bConsiderPower = true) const;

    /**
     * Calculates the value of opening borders with another team.
     * - Used for trade evaluation.
     * Steps:
     *   1. Double value if planning war (not against eTeam).
     *   2. Otherwise, sum cities of both teams.
     */
	int AI_openBordersTradeVal(TeamTypes eTeam) const;

    /**
     * Determines if an open borders trade should be denied.
     * - Checks for human, vassal, war plans, attitude, and recent cancellations.
     * Steps:
     *   1. Check for human/vassal/war sharing.
     *   2. Check for worst enemy and war plans.
     *   3. Check for recent cancellations and attitude thresholds.
     *   4. Return denial type.
     */
	DenialTypes AI_openBordersTrade(TeamTypes eTeam) const;

    /**
     * Calculates the value of a defensive pact with another team.
     * - Used for trade evaluation.
     * Steps:
     *   1. Multiply sum of cities by 3.
     */
	int AI_defensivePactTradeVal(TeamTypes eTeam) const;

    /**
     * Determines if a defensive pact trade should be denied.
     * - Checks for human, worst enemy, attitude, and allied status.
     * Steps:
     *   1. Check for human player and number of alive civs.
     *   2. Check for worst enemy and attitude thresholds.
     *   3. Return denial type.
     */
	DenialTypes AI_defensivePactTrade(TeamTypes eTeam) const;

    /**
     * Determines if a permanent alliance trade should be denied.
     * - Checks for human, worst enemy, power, and attitude.
     * Steps:
     *   1. Check for human and worst enemy.
     *   2. Compare combined power to total civ power.
     *   3. Check for sufficient allied status and attitude.
     *   4. Return denial type.
     */
	DenialTypes AI_permanentAllianceTrade(TeamTypes eTeam) const;

    /**
     * Returns the worst enemy team.
     * - Used for diplomatic and war planning.
     * Steps:
     *   1. Iterate over all teams.
     *   2. Find the team with lowest attitude value and highest rank.
     */
	TeamTypes AI_getWorstEnemy() const;

    /**
     * Updates the worst enemy team.
     * - Used for AI diplomatic recalculation.
     * Steps:
     *   1. Iterate over all teams.
     *   2. Evaluate attitude and rank.
     *   3. Set m_eWorstEnemy to the best candidate.
     */
	void AI_updateWorstEnemy();

	/**
     * Gets the war plan state counter for a given team.
     * - Returns the number of turns since the current war plan was set.
     * Steps:
     *   1. Return the value from m_aiWarPlanStateCounter for the given team index.
     */
	int AI_getWarPlanStateCounter(TeamTypes eIndex) const;

    /**
     * Sets the war plan state counter for a given team.
     * - Updates the number of turns since the current war plan was set.
     * Steps:
     *   1. Set m_aiWarPlanStateCounter for the given team index.
     *   2. Ensure the value is not negative.
     */
	void AI_setWarPlanStateCounter(TeamTypes eIndex, int iNewValue);

    /**
     * Changes the war plan state counter for a given team by a delta.
     * Steps:
     *   1. Add iChange to the current counter value.
     *   2. Set the new value using AI_setWarPlanStateCounter.
     */
	void AI_changeWarPlanStateCounter(TeamTypes eIndex, int iChange);

    /**
     * Gets the at-war counter for a given team.
     * - Returns the number of turns at war with the team.
     */
	int AI_getAtWarCounter(TeamTypes eIndex) const;

    /**
     * Sets the at-war counter for a given team.
     * - Updates the number of turns at war.
     * Steps:
     *   1. Set m_aiAtWarCounter for the given team index.
     *   2. Ensure the value is not negative.
     */
	void AI_setAtWarCounter(TeamTypes eIndex, int iNewValue);

    /**
     * Changes the at-war counter for a given team by a delta.
     * Steps:
     *   1. Add iChange to the current counter value.
     *   2. Set the new value using AI_setAtWarCounter.
     */
	void AI_changeAtWarCounter(TeamTypes eIndex, int iChange);

    /**
     * Gets the at-peace counter for a given team.
     * - Returns the number of turns at peace with the team.
     */
	int AI_getAtPeaceCounter(TeamTypes eIndex) const;

    /**
     * Sets the at-peace counter for a given team.
     * - Updates the number of turns at peace.
     * Steps:
     *   1. Set m_aiAtPeaceCounter for the given team index.
     *   2. Ensure the value is not negative.
     */
	void AI_setAtPeaceCounter(TeamTypes eIndex, int iNewValue);

    /**
     * Changes the at-peace counter for a given team by a delta.
     * Steps:
     *   1. Add iChange to the current counter value.
     *   2. Set the new value using AI_setAtPeaceCounter.
     */
	void AI_changeAtPeaceCounter(TeamTypes eIndex, int iChange);

    /**
     * Gets the has-met counter for a given team.
     * - Returns the number of turns since meeting the team.
     */
	int AI_getHasMetCounter(TeamTypes eIndex) const;

    /**
     * Sets the has-met counter for a given team.
     * - Updates the number of turns since meeting the team.
     * Steps:
     *   1. Set m_aiHasMetCounter for the given team index.
     *   2. Ensure the value is not negative.
     */
	void AI_setHasMetCounter(TeamTypes eIndex, int iNewValue);

    /**
     * Changes the has-met counter for a given team by a delta.
     * Steps:
     *   1. Add iChange to the current counter value.
     *   2. Set the new value using AI_setHasMetCounter.
     */
	void AI_changeHasMetCounter(TeamTypes eIndex, int iChange);

    /**
     * Gets the open borders counter for a given team.
     * - Returns the number of turns with open borders.
     */
	int AI_getOpenBordersCounter(TeamTypes eIndex) const;

    /**
     * Sets the open borders counter for a given team.
     * - Updates the number of turns with open borders.
     * Steps:
     *   1. Set m_aiOpenBordersCounter for the given team index.
     *   2. Ensure the value is not negative.
     */
	void AI_setOpenBordersCounter(TeamTypes eIndex, int iNewValue);

    /**
     * Changes the open borders counter for a given team by a delta.
     * Steps:
     *   1. Add iChange to the current counter value.
     *   2. Set the new value using AI_setOpenBordersCounter.
     */
	void AI_changeOpenBordersCounter(TeamTypes eIndex, int iChange);

    /**
     * Gets the defensive pact counter for a given team.
     * - Returns the number of turns with a defensive pact.
     */
	int AI_getDefensivePactCounter(TeamTypes eIndex) const;

    /**
     * Sets the defensive pact counter for a given team.
     * - Updates the number of turns with a defensive pact.
     * Steps:
     *   1. Set m_aiDefensivePactCounter for the given team index.
     *   2. Ensure the value is not negative.
     */
	void AI_setDefensivePactCounter(TeamTypes eIndex, int iNewValue);

    /**
     * Changes the defensive pact counter for a given team by a delta.
     * Steps:
     *   1. Add iChange to the current counter value.
     *   2. Set the new value using AI_setDefensivePactCounter.
     */
	void AI_changeDefensivePactCounter(TeamTypes eIndex, int iChange);

    /**
     * Gets the share war counter for a given team.
     * - Returns the number of turns sharing a war with the team.
     */
	int AI_getShareWarCounter(TeamTypes eIndex) const;

    /**
     * Sets the share war counter for a given team.
     * - Updates the number of turns sharing a war.
     * Steps:
     *   1. Set m_aiShareWarCounter for the given team index.
     *   2. Ensure the value is not negative.
     */
	void AI_setShareWarCounter(TeamTypes eIndex, int iNewValue);

    /**
     * Changes the share war counter for a given team by a delta.
     * Steps:
     *   1. Add iChange to the current counter value.
     *   2. Set the new value using AI_setShareWarCounter.
     */
	void AI_changeShareWarCounter(TeamTypes eIndex, int iChange);

    /**
     * Gets the war success value for a given team.
     * - Returns the accumulated war success points.
     */
	int AI_getWarSuccess(TeamTypes eIndex) const;

    /**
     * Sets the war success value for a given team.
     * - Updates the accumulated war success points.
     * Steps:
     *   1. Set m_aiWarSuccess for the given team index.
     *   2. Invalidate attitude caches for all players involved.
     *   3. Ensure the value is not negative.
     */
	void AI_setWarSuccess(TeamTypes eIndex, int iNewValue);

    /**
     * Changes the war success value for a given team by a delta.
     * Steps:
     *   1. Add iChange to the current value.
     *   2. Apply special scaling for rebels.
     *   3. Set the new value using AI_setWarSuccess.
     */
	void AI_changeWarSuccess(TeamTypes eIndex, int iChange);

    /**
     * Gets the enemy peacetime trade value for a given team.
     * - Returns the accumulated trade value.
     */
	int AI_getEnemyPeacetimeTradeValue(TeamTypes eIndex) const;

    /**
     * Sets the enemy peacetime trade value for a given team.
     * - Updates the accumulated trade value.
     * Steps:
     *   1. Set m_aiEnemyPeacetimeTradeValue for the given team index.
     *   2. Ensure the value is not negative.
     */
	void AI_setEnemyPeacetimeTradeValue(TeamTypes eIndex, int iNewValue);

    /**
     * Changes the enemy peacetime trade value for a given team by a delta.
     * Steps:
     *   1. Add iChange to the current value.
     *   2. Set the new value using AI_setEnemyPeacetimeTradeValue.
     */
	void AI_changeEnemyPeacetimeTradeValue(TeamTypes eIndex, int iChange);

    /**
     * Gets the enemy peacetime grant value for a given team.
     * - Returns the accumulated grant value.
     */
	int AI_getEnemyPeacetimeGrantValue(TeamTypes eIndex) const;

    /**
     * Sets the enemy peacetime grant value for a given team.
     * - Updates the accumulated grant value.
     * Steps:
     *   1. Set m_aiEnemyPeacetimeGrantValue for the given team index.
     *   2. Ensure the value is not negative.
     */
	void AI_setEnemyPeacetimeGrantValue(TeamTypes eIndex, int iNewValue);

    /**
     * Changes the enemy peacetime grant value for a given team by a delta.
     * Steps:
     *   1. Add iChange to the current value.
     *   2. Set the new value using AI_setEnemyPeacetimeGrantValue.
     */
	void AI_changeEnemyPeacetimeGrantValue(TeamTypes eIndex, int iChange);

    /**
     * Gets the current war plan for a given team.
     * - Returns the WarPlanTypes enum value.
     */
	WarPlanTypes AI_getWarPlan(TeamTypes eIndex) const;

    /**
     * Returns true if the current war plan is a chosen war (not defensive).
     * - Checks the war plan type.
     */
	bool AI_isChosenWar(TeamTypes eIndex) const;

    /**
     * Returns true if the current war plan is preparing a sneak attack.
     * - Checks for WARPLAN_PREPARING_LIMITED or WARPLAN_PREPARING_TOTAL.
     */
	bool AI_isSneakAttackPreparing(TeamTypes eIndex) const;

    /**
     * Returns true if the current war plan is ready for a sneak attack.
     * - Checks for chosen war and not preparing.
     */
	bool AI_isSneakAttackReady(TeamTypes eIndex) const;

    /**
     * Sets the war plan for a given team.
     * - Updates the war plan type and resets counters.
     * Steps:
     *   1. If the new value differs, set m_aeWarPlan for the team.
     *   2. If bInFull is true, reset war plan state counter and update area strategies.
     *   3. Mark production dirty for all non-human team members.
     */
	void AI_setWarPlan(TeamTypes eIndex, WarPlanTypes eNewValue, bool bWar = true, bool bInFull = true);

    /**
     * Calculates the closeness value between this team and another.
     * - Sums up player closeness scores for all team members.
     */
	int AI_teamCloseness(TeamTypes eIndex, int iMaxDistance = -1) const;

    /**
     * Performs random rolls to veto war declaration.
     * - Returns true if war is vetoed by random chance or attitude probability.
     */
	bool AI_performNoWarRolls(TeamTypes eTeam);

    /**
     * Gets the attitude weight for a given team.
     * - Returns a numeric value based on attitude type.
     */
	int AI_getAttitudeWeight(const TeamTypes eTeam) const;

    /**
     * Gets the lowest victory countdown among all victory types.
     * - Returns -1 if no countdown is active.
     */
	int AI_getLowestVictoryCountdown() const;

    /**
     * Calculates the monopoly value of a technology for trade.
     * - Sums up values for units, buildings, and projects unlocked by the tech.
     */
	int AI_getTechMonopolyValue(TechTypes eTech, TeamTypes eTeam) const;

    /**
     * Returns true if the water area is relevant for the team.
     * - Checks for cities adjacent to the area and strategic importance.
     */
	bool AI_isWaterAreaRelevant(const CvArea* pArea) const;

    /**
     * Reads team AI data from a stream.
     * - Loads all counters, war plans, and cached values.
     */
	virtual void read(FDataStreamBase* pStream);

    /**
     * Writes team AI data to a stream.
     * - Saves all counters, war plans, and cached values.
     */
	virtual void write(FDataStreamBase* pStream);

    /**
     * Calculates the value of an embassy trade with another team.
     * - Based on number of cities.
     */
	int AI_embassyTradeVal(TeamTypes eTeam) const;

    /**
     * Calculates the value of a limited borders trade with another team.
     * - Based on number of cities.
     */
	int AI_LimitedBordersTradeVal(TeamTypes eTeam) const;

    /**
     * Calculates the value of a free trade agreement with another team.
     * - Based on number of cities and trade status.
     */
	int AI_FreeTradeAgreementVal(TeamTypes eTeam) const;

    /**
     * Calculates the value of a contact trade (sharing contact info) with another team.
     * - Based on attitude, met teams, and trade options.
     */
    int AI_contactTradeVal(TeamTypes eContactTeam, TeamTypes eTeamBuyer) const;

    /**
     * Determines if an embassy trade should be denied.
     * - Checks attitude, memory, and recent cancellations.
     */
	DenialTypes AI_embassyTrade(TeamTypes eTeam) const;

    /**
     * Determines if a limited borders trade should be denied.
     * - Uses open borders trade denial logic.
     */
	DenialTypes AI_LimitedBordersTrade(TeamTypes eTeam) const;

    /**
     * Determines if a contact trade should be denied.
     * - Checks attitude, war plans, and worst enemy status.
     */
	DenialTypes AI_contactTrade(TeamTypes eContactTeam, TeamTypes eTeam) const;

    /**
     * Determines if a free trade agreement should be denied.
     * - Checks attitude, war plans, and connection between capitals.
     */
	DenialTypes AI_FreeTradeAgreement(TeamTypes eTeam) const;

    /**
     * Updates cached diplomatic values for the team.
     * - Recalculates thresholds and random values for tech and war planning.
     */
	void AI_updateDiploCache();

    /**
     * Returns true if the given team has adjacent land plots to this team.
     * - Used for evaluating proximity and land targets.
     */
	bool AI_hasAdjacentLandPlots(TeamTypes eTeam) const;

#ifdef ENABLE_FOGWAR_DECAY
    /**
     * Returns an number that will be the base for the plot decay. The higher the number is, the lastest the plot will disappear.
     * The number is by default 1, and will be set to 3,6,9,12... depending on which Tech the player already has
     */
    short getVisibilityDecay(bool pSeaPlot = false);
#endif

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
#ifdef ENABLE_FOGWAR_DECAY
    bool m_bPermanentMapLand;
    bool m_bPermanentMapSea;
    short m_iDefaultDecay;
#endif


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

#define NO_DECAY -99
#define DEFAULT_DECAY 0
#define REMOVE_PLOT_DECAY -10
#define MAX_DECAY 99

#endif
