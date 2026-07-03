#pragma once

// unitAI.h

#ifndef CIV4_UNIT_AI_H
#define CIV4_UNIT_AI_H

#include "CvUnit.h"
#include "GroupingParams.h"

typedef enum
{
	CONTRACTUAL_STATE_NONE,
	CONTRACTUAL_STATE_AWAITING_ANSWER,
	CONTRACTUAL_STATE_AWAITING_WORK,
	CONTRACTUAL_STATE_FOUND_WORK,
	CONTRACTUAL_STATE_NO_WORK_FOUND
} ContractualState;

class CvCity;
class CvPlot;
class CvReachablePlotSet;
class CvSelectionGroup;
class ConstructionNeeds;

/**
 * Main AI class for units.
 * - Handles behavior, missions, and decision logic for all unit types.
 * - Manages initialization, reset, save/load, and assignment of AI roles.
 * - Coordinates movement, attacks, promotions, grouping, and special missions.
 * - Integrates contract management, healing needs, property control, escort, and more.
 */
class CvUnitAI : public CvUnit
{
public:
    /**
     * Constructor for CvUnitAI.
     * - Initializes all variables and AI state for the unit.
     */
	CvUnitAI(bool bIsDummy = false);

    /**
     * Destructor for CvUnitAI.
     * - Frees memory and resets state.
     */
	virtual ~CvUnitAI();

    /**
     * Assignment operator.
     * - Copies all members from another CvUnitAI instance.
     */
	CvUnitAI& operator=(const CvUnitAI& other);

    /**
     * Initializes the unit AI with a type and birthmark.
     * - Resets state and sets the AI type.
     */
	void AI_init(UnitAITypes eUnitAI, int iBirthmark);

    /**
     * Uninitializes the unit AI.
     * - Frees resources and resets variables.
     */
	void AI_uninit();

    /**
     * Resets the unit AI.
     * - Resets all members and state, optionally during construction.
     */
	void AI_reset(UnitAITypes eUnitAI = NO_UNITAI, bool bConstructorCall = false);

    /**
     * Sends a log message for AI debugging.
     * - Uses the FLB logger for this class.
     */
	void SendLog(CvWString function, CvWString message);

    /**
     * Updates the unit AI each turn.
     * - Handles movement, missions, automation, and special states.
     * - Returns true if the unit should wait for the next turn.
     * @dot
     * digraph AI_update {
     *   node [shape=box];
     *   CheckCanMove -> HandleAutomate -> DoUnitAIMove -> HandleContracts -> FinalizeState;
     * }
     * @enddot
     */
	bool AI_update();

    /**
     * Follows a target or mission.
     * - Handles attacks, pillaging, and movement toward objectives.
     * - Returns true if an action is performed.
     */
	bool AI_follow();

    /**
     * Loads the unit onto a transport or performs a loading mission.
     * - Finds the best available transport based on criteria.
     * - Returns true if the mission is performed.
     */
	bool AI_load(UnitAITypes eUnitAI, MissionAITypes eMissionAI, UnitAITypes eTransportedUnitAI = NO_UNITAI, int iMinCargo = -1, int iMinCargoSpace = -1, int iMaxCargoSpace = -1, int iMaxCargoOurUnitAI = -1, int iFlags = 0, int iMaxPath = MAX_INT, int iMaxTransportPath = MAX_INT);

    /**
     * Attempts to upgrade the unit.
     * - Checks upgrade chain and conditions.
     * - Returns true if upgrade is performed.
     */
	bool AI_upgrade(); // override

    /**
     * Attempts to promote the unit.
     * - Evaluates available promotions and chooses the best.
     * - Returns true if a promotion is performed.
     */
	bool AI_promote(); // override

#define	LEADER_PRIORITY_MIN 0
#define	LEADER_PRIORITY_MAX	100

    /**
     * Returns the leader priority for this unit in a group.
     * - Used to determine group leader.
     */  
	int AI_groupFirstVal() const;

    /**
     * Returns the secondary group value (combat strength).
     * - Used for sorting units in groups.
     */
	int AI_groupSecondVal() const;

    /**
     * Calculates attack odds on a given plot.
     * - Considers defender, bonuses, and withdrawal chances.
     * - Returns odds out of 100.
     */
	int AI_attackOdds(const CvPlot* pPlot, bool bPotentialEnemy, CvUnit** ppDefender = NULL, bool bAssassinate = false);

    /**
     * Variant to test a specific defender as if it was in the specified plot.
     * - Returns attack odds.
     */
	int AI_attackOddsAtPlot(const CvPlot* pPlot, CvUnit* pDefender, bool modifyPredictedResults = false);

    /**
     * Internal variant for attack odds calculation.
     * - Used for caching and advanced calculations.
     */
	int AI_attackOddsAtPlotInternal(const CvPlot* pPlot, CvUnit* pDefender, bool modifyPredictedResults);

    /**
     * Determines the best build action in a city.
     * - Returns true if an action is found.
     */
	bool AI_bestCityBuild(const CvCity* pCity, CvPlot** ppBestPlot = NULL, BuildTypes* peBestBuild = NULL, const CvPlot* pIgnorePlot = NULL, const CvUnit* pUnit = NULL);

    /**
     * Returns true if the unit is a city AI type.
     */
	bool AI_isCityAIType() const;

    /**
     * Gets the birthmark (AI identifier) of the unit.
     */
	int AI_getBirthmark() const;

    /**
     * Sets the birthmark of the unit.
     */
	void AI_setBirthmark(int iNewValue);

    /**
     * Gets the AI type of the unit.
     */
	UnitAITypes AI_getUnitAIType() const;

    /**
     * Sets the AI type of the unit.
     */
	void AI_setUnitAIType(UnitAITypes eNewValue);

    /**
     * Calculates the sacrifice value of the unit on a plot.
     * - Used for combat or retreat decisions.
     */
	int AI_sacrificeValue(const CvPlot* pPlot) const;

    /**
     * Returns true if the unit is awaiting a contract.
     */
	bool AI_isAwaitingContract() const;

    /**
     * Returns true if the unit is a city garrison.
     */
	bool AI_isCityGarrison(const CvCity* pCity) const;

    /**
     * Sets the unit as a garrison for a city.
     */
	void AI_setAsGarrison(const CvCity* pCity = NULL);

    /**
     * Calculates the search range for the unit.
     * - Used for missions and movement.
     */
	int AI_searchRange(int iRange = 1) const;

    /**
     * Gets the intended building for construction by this unit.
     */
	BuildingTypes getIntendedConstructBuilding() const { return m_eIntendedConstructBuilding; }

    /**
     * Gets the intended heritage for construction by this unit.
     */
	HeritageTypes getIntendedHeritage() const { return m_eIntendedHeritage; }

    /**
     * Reads unit AI data from a stream (for loading).
     * - Loads all relevant AI state and counters.
     */
	void read(FDataStreamBase* pStream);

    /**
     * Writes unit AI data to a stream (for saving).
     * - Saves all relevant AI state and counters.
     */
	void write(FDataStreamBase* pStream);


    /**
     * Clears the cached generic value for this unit.
     * Steps:
     *   1. Resets cached value and flags.
     */
    void AI_flushValueCache();

    /**
     * Scores the healing need for a city for a given unit combat type and domain.
     * - Used to prioritize which city a healer unit should move to.
     * Steps:
     *   1. Evaluate city's wounded units of the given combat type and domain.
     *   2. Calculate total healing need based on damage and unit count.
     *   3. Return the score.
     */
    int scoreCityHealerNeed(const UnitCombatTypes eUnitCombat, const DomainTypes eDomain, const CvCity* city) const;

    /**
     * Returns the predicted hit points for the unit.
     * - Used for AI planning and combat evaluation.
     */
    virtual int AI_getPredictedHitPoints() const;

    /**
     * Sets the predicted hit points for the unit.
     * Steps:
     *   1. Updates the internal variable.
     *   2. Triggers any necessary recalculations.
     */
    virtual void AI_setPredictedHitPoints(int iPredictedHitPoints);

    /**
     * Returns true if the unit has attacked this turn.
     * - Used for AI planning and blitz logic.
     */
    virtual bool AI_getHasAttacked() const;

    /**
     * Calculates the beneficial property value this unit provides to a city.
     * - Used for property control and healing logic.
     * Steps:
     *   1. Evaluate all property sources from the unit.
     *   2. Sum up beneficial effects for the target city.
     *   3. Return the total value.
     */
    virtual int AI_beneficialPropertyValueToCity(const CvCity* pCity, PropertyTypes eProperty) const;

    /**
     * Notifies the AI that this unit was killed.
     * Steps:
     *   1. Updates internal counts and contract broker.
     *   2. Adjusts AI planning for unit loss.
     */
    virtual void AI_killed();


    /**
     * Fulfills healer need for a plot.
     * - Moves the unit to heal wounded units at the plot if needed.
     * Steps:
     *   1. Evaluate healing need at the plot.
     *   2. Move or stay to heal if required.
     *   3. Return true if action taken.
     */
    bool AI_fulfillHealerNeed(const CvPlot* pPlot);

    /**
     * Fulfills immediate healer need for a plot.
     * - Prioritizes urgent healing actions.
     * Steps:
     *   1. Check for critical healing need at the plot.
     *   2. Move or stay to heal if required.
     *   3. Return true if action taken.
     */
    bool AI_fulfillImmediateHealerNeed(const CvPlot* pPlot);

    /**
     * Fulfills healer need for a city.
     * - Moves the unit to heal wounded units in the city if needed.
     * Steps:
     *   1. Evaluate healing need for the city.
     *   2. Move or stay to heal if required.
     *   3. Return true if action taken.
     */
    bool AI_fulfillCityHealerNeed(const CvPlot* pPlot);

    /**
     * Fulfills property control need for the unit.
     * - Moves the unit to control properties (e.g. crime, disease) in cities.
     * Steps:
     *   1. Evaluate property control needs for all cities.
     *   2. Move or stay to control property if required.
     *   3. Return true if action taken.
     */
    bool AI_fulfillPropertyControlNeed();

    /**
     * Attempts to arrest units or criminals in the current plot.
     * Steps:
     *   1. Check for arrestable units in the plot.
     *   2. Perform arrest if possible.
     *   3. Return true if action taken.
     */
    bool AI_arrest();

    /**
     * Attempts to ambush enemy units in the current plot.
     * Steps:
     *   1. Evaluate odds of successful ambush.
     *   2. Perform ambush if odds are above threshold.
     *   3. Return true if action taken.
     */
    bool AI_ambush(int iOddsThreshold, bool bAssassinationOnly = false);

    /**
     * Activates a status effect (promotion) for the unit or stack.
     * Steps:
     *   1. Apply the status effect to the unit or stack.
     *   2. Return true if successful.
     */
    bool AI_activateStatus(bool bStack, PromotionTypes eStatus, CvUnit* pUnit);



    /**
     * Evaluates the value of a promotion for this unit.
     * @brief Returns a numeric score for a given promotion type, used to select the best promotion.
     * Steps:
     *   1. Calls the player's AI_promotionValue to get the value for this promotion and unit type.
     *   2. Returns the computed value.
     */
    int AI_promotionValue(PromotionTypes ePromotion) const;

    /**
     * Attempts to shadow another unit of a given AI type.
     * @brief Finds a suitable unit to follow and pushes a movement mission if possible.
     * Steps:
     *   1. Iterate over all units of the same owner.
     *   2. Check domain, AI type, movement, cargo, and city status.
     *   3. Select the best candidate based on criteria.
     *   4. Push a movement mission to follow the selected unit.
     *   5. Return true if a mission was pushed.
     */
    bool AI_shadow(UnitAITypes eUnitAI, int iMax = -1, int iMaxRatio = -1, bool bWithCargoOnly = true, bool bOutsideCityOnly = false, int iMaxPath = MAX_INT);

    /**
     * Attempts to group this unit with others based on parameters.
     * @brief Finds the best group to join and merges if beneficial.
     * Steps:
     *   1. Check if grouping is allowed (not cargo, not already grouped, etc.).
     *   2. Iterate over all groups of the owner.
     *   3. Evaluate group compatibility and size.
     *   4. Select the best group to join.
     *   5. Merge into the selected group and return true if successful.
     */
    bool AI_group(const GroupingParams& params);

    /**
     * Attempts to merge this unit into a group within a given range.
     * @brief Finds the best unit to merge with and performs the merge.
     * Steps:
     *   1. Check if grouping is allowed.
     *   2. Iterate over plots within range.
     *   3. Evaluate units for merging based on AI type and group size.
     *   4. Merge with the best candidate and return true if successful.
     */
    bool AI_groupMergeRange(UnitAITypes eUnitAI, int iRange, bool bBiggerOnly = true, bool bAllowRegrouping = false, bool bIgnoreFaster = false);

    /**
     * Determines if the unit can group with a given AI type.
     * @brief Checks compatibility for grouping.
     * Steps:
     *   1. Compare AI types.
     *   2. Apply special rules for certain AI types.
     *   3. Return true if grouping is allowed.
     */
    bool AI_canGroupWithAIType(UnitAITypes eUnitAI) const;

    /**
     * Determines if the unit is allowed to join a group.
     * @brief Checks various conditions for group joining.
     * Steps:
     *   1. Check group head status, cargo, AI type, and mission type.
     *   2. Evaluate activity type and plot compatibility.
     *   3. Return true if allowed.
     */
    bool AI_allowGroup(const CvUnit* pUnit, UnitAITypes eUnitAI) const;

    /**
     * Calculates the number of workers needed for a plot and build.
     * @brief Estimates required workers based on build time and work rate.
     * Steps:
     *   1. Get work rate and build time.
     *   2. Calculate turns needed.
     *   3. Adjust for bonuses and return the number needed.
     */
    int AI_calculatePlotWorkersNeeded(const CvPlot* pPlot, BuildTypes eBuild) const;

    /**
     * Processes contracts for the unit.
     * @brief Handles contract fulfillment and assignment.
     * Steps:
     *   1. Check domain and contract state.
     *   2. Attempt to make or fulfill contracts.
     *   3. Return true if a contract was processed.
     */
    bool processContracts(int iMinPriority = 0);


    /**
     * Returns true if the worker needs a defender at the given plot.
     * @brief Evaluates danger and nearby enemies.
     * Steps:
     *   1. Check danger level at plot and current location.
     *   2. Check for nearby enemies in non-owned territory.
     *   3. Return true if a defender is needed.
     */
    bool AI_workerNeedsDefender(const CvPlot* pPlot) const;

    /**
     * Releases the worker's defender if not needed.
     * @brief Checks contract state and group size.
     * Steps:
     *   1. Check contract turn and group size.
     *   2. Release defender if conditions are met.
     */
    bool AI_workerReleaseDefenderIfNotNeeded() const;

protected:

	int m_iBirthmark;

	UnitAITypes m_eUnitAIType;

	int m_iAutomatedAbortTurn;

	int m_contractsLastEstablishedTurn;
	ContractualState m_contractualState;

	int m_iGarrisonCity;
	int m_iAffirmedGarrisonCity;

	BuildingTypes m_eIntendedConstructBuilding; // Used to coordinate subdued animal and great person builds
	HeritageTypes m_eIntendedHeritage; // Used to coordinate subdued animal and great person builds
	static ConstructionNeeds* m_constructionNeeds;

	int m_iGroupLeadOverride;
	int m_iPredictedHitPoints;
	bool m_bHasAttacked;
	bool m_bWaitingOnUnitAIAny;

	mutable int m_iGenericValue;
	mutable int m_eGenericValueFlagsCached;

	std::vector<int> m_aiWaitingOnUnitAITypes;

    /**
     * Moves the unit according to its AI type for this turn.
     * @brief Handles the main movement logic for the unit's AI.
     * Steps:
     *   1. Determines the unit's AI type and mission.
     *   2. Executes movement or mission logic based on AI type.
     *   3. Updates internal state after movement.
     */
	void doUnitAIMove();

    /**
     * Handles movement logic for animal units.
     * @brief Executes AI behavior specific to animal units.
     * Steps:
     *   1. Evaluates nearby targets or threats.
     *   2. Moves or acts according to animal AI rules.
     */
	void AI_animalMove();
	bool AI_SettleFirstCity();
	void AI_settleMove();
	int AI_minSettlerDefense() const;
	bool IsAbroad();
	int GetNumberOfUnitsInGroup();
	bool AI_upgradeWorker();
	void AI_workerMove();
	
    /**
     * Handles movement logic for barbarian attack units.
     * @brief Executes AI behavior for barbarian units focused on attacking and pillaging.
     * Steps:
     *   1. Selects status or mission if needed.
     *   2. Attempts attacks on nearby targets with varying odds thresholds.
     *   3. Moves to goody huts if available.
     *   4. Pillages nearby tiles if conditions are met.
     *   5. Evaluates strategic options based on game state (e.g., raging barbs, offensive area).
     *   6. Loads onto transports if appropriate.
     *   7. Heals if wounded.
     *   8. Patrols or retreats to city if threatened.
     *   9. Skips turn if no other actions are possible.
     */
	void AI_barbAttackMove();

    /**
     * Handles movement logic for attack units.
     * @brief Executes AI behavior for units assigned to attack roles.
     * Steps:
     *   1. Selects status or switches to construct if needed.
     *   2. Checks for regroup missions and danger in the current plot.
     *   3. Guards cities if needed, processes contracts, merges with city attack groups.
     *   4. Heals if wounded, airlifts if possible, joins city attacks.
     *   5. Evaluates offensive airlift and patrol options.
     *   6. Retreats to city or moves to safety if threatened.
     *   7. Skips turn if no other actions are possible.
     */
	void AI_attackMove();

    /**
     * Handles movement logic for city attack units.
     * @brief Executes AI behavior for units assigned to attack cities.
     * Steps:
     *   1. Selects status or switches to construct if needed.
     *   2. Evaluates area AI type and war status.
     *   3. Merges with city attack groups, finds target city, and moves to attack.
     *   4. Heals if wounded, bombards if possible, attacks infiltrators.
     *   5. Retreats to city or moves to safety if threatened.
     *   6. Skips turn if no other actions are possible.
     */
	void AI_attackCityMove();

    /**
     * Handles movement logic for city attack lemming units.
     * @brief Executes AI behavior for units assigned to follow city attack stacks.
     * Steps:
     *   1. Selects status if needed.
     *   2. Attempts city attacks with high odds, bombards, and moves to target city.
     *   3. Attacks any available targets.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_attackCityLemmingMove();

    /**
     * Handles movement logic for collateral damage units.
     * @brief Executes AI behavior for units specializing in collateral damage.
     * Steps:
     *   1. Leaves attack if odds are low, guards city if needed.
     *   2. Heals if wounded, attacks cities, bombards units.
     *   3. Attacks any available targets, protects if needed.
     *   4. Retreats to city or moves to safety if threatened.
     *   5. Skips turn if no other actions are possible.
     */
	void AI_collateralMove();

    /**
     * Handles movement logic for pillage units.
     * @brief Executes AI behavior for units assigned to pillage enemy territory.
     * Steps:
     *   1. Selects status if needed, heals if wounded.
     *   2. Joins city attacks, attacks cities, pillages nearby tiles.
     *   3. Chokes enemy cities, heals if needed, groups with other pillage units.
     *   4. Retreats to city or moves to safety if threatened.
     *   5. Skips turn if no other actions are possible.
     */
	void AI_pillageMove();

    /**
     * Handles movement logic for reserve units.
     * @brief Executes AI behavior for units assigned to reserve roles.
     * Steps:
     *   1. Checks for construct switch, evaluates danger in current plot.
     *   2. Leaves attack if threatened, guards city, processes contracts.
     *   3. Heals if wounded, protects if needed, retreats to city or moves to safety.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_reserveMove();


    /**
     * Handles movement logic for counter units.
     * @brief Executes AI behavior for units assigned to counter enemy threats.
     * Steps:
     *   1. Selects status if needed, checks for construct switch.
     *   2. Regroups if needed, merges with settler groups.
     *   3. Guards city, fort, or city site, heals if wounded.
     *   4. Groups with attack or city defense units, protects if needed.
     *   5. Retreats to city or moves to safety if threatened.
     *   6. Skips turn if no other actions are possible.
     */    
	void AI_counterMove();

    /**
     * Handles movement logic for pillage counter units.
     * @brief Executes AI behavior for units assigned to counter pillagers.
     * Steps:
     *   1. Establishes stack coverage for invisible units.
     *   2. Calls counterMove for standard counter logic.
     */
	void AI_pillageCounterMove();

    /**
     * Handles movement logic for paratrooper units.
     * @brief Executes AI behavior for units capable of paradrop missions.
     * Steps:
     *   1. Selects status if needed, checks for hostile territory.
     *   2. Attacks cities, performs paradrop if possible.
     *   3. Heals if wounded, protects if needed, retreats to city or moves to safety.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_paratrooperMove();

    /**
     * Handles movement logic for city defense units.
     * @brief Executes AI behavior for units assigned to defend cities.
     * Steps:
     *   1. Selects status if needed, checks for construct switch.
     *   2. Guards city, fort, or city site, heals if wounded.
     *   3. Bolsters national defenses, airlifts if needed.
     *   4. Retreats to city or moves to safety if threatened.
     *   5. Skips turn if no other actions are possible.
     */
	void AI_cityDefenseMove();

    /**
     * Handles movement logic for extra city defense units.
     * @brief Executes AI behavior for units assigned to special city defense roles.
     * Steps:
     *   1. Selects status if needed, checks for negative property unit.
     *   2. Leaves attack if odds are low, chokes enemy cities.
     *   3. Guards city, fort, or city site, heals if wounded.
     *   4. Retreats to city or moves to safety if threatened.
     *   5. Skips turn if no other actions are possible.
     */
	void AI_cityDefenseExtraMove();

    /**
     * Handles movement logic for explorer units.
     * @brief Executes AI behavior for units assigned to explore the map.
     * Steps:
     *   1. Selects status if needed, joins offensive stacks if in war.
     *   2. Explores nearby tiles, moves to goody huts, refreshes explore range.
     *   3. Retreats to city or moves to safety if threatened.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_exploreMove();

    /**
     * Handles movement logic for hunter escort units.
     * @brief Executes AI behavior for units assigned to escort hunters or subdued animals.
     * Steps:
     *   1. Selects status if needed, heals if wounded.
     *   2. Escorts hunter units, processes contracts, retreats to city or moves to safety.
     *   3. Skips turn if no other actions are possible.
     */
	void AI_hunterEscortMove();
	
    /**
     * Handles movement logic for missionary units.
     * @brief Executes AI behavior for units assigned to spread religion or corporation.
     * Steps:
     *   1. Selects status if needed, spreads religion or corporation.
     *   2. Performs inquisition if possible, retreats to city or moves to safety.
     *   3. Skips turn if no other actions are possible.
     */
	void AI_missionaryMove();

    /**
     * Handles movement logic for prophet units.
     * @brief Executes AI behavior for units assigned to religious roles.
     * Steps:
     *   1. Selects status if needed, constructs buildings or founds religion.
     *   2. Discovers technology, performs heritage actions, evaluates golden age.
     *   3. Retreats to city or moves to safety if threatened.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_prophetMove();

    /**
     * Handles movement logic for artist units.
     * @brief Executes AI behavior for units assigned to cultural roles.
     * Steps:
     *   1. Selects status if needed, performs culture victory moves.
     *   2. Constructs buildings, discovers technology, performs great work.
     *   3. Evaluates golden age, retreats to city or moves to safety.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_artistMove();

    /**
     * Handles movement logic for scientist units.
     * @brief Executes AI behavior for units assigned to scientific roles.
     * Steps:
     *   1. Selects status if needed, discovers technology.
     *   2. Constructs buildings, joins cities, evaluates golden age.
     *   3. Retreats to city or moves to safety if threatened.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_scientistMove();

    /**
     * Handles movement logic for general units.
     * @brief Executes AI behavior for units assigned to military leadership roles.
     * Steps:
     *   1. Seeks escort if needed, commands units, leads legendary units.
     *   2. Joins military cities, constructs buildings, joins cities.
     *   3. Retreats to city or moves to safety if threatened.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_generalMove();

    /**
     * Handles movement logic for great hunter units.
     * @brief Executes AI behavior for units assigned to hunting and legendary roles.
     * Steps:
     *   1. Selects status if needed, leads legendary units.
     *   2. Constructs buildings, joins cities, performs heritage actions.
     *   3. Retreats to city or moves to safety if threatened.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_greatHunterMove();

    /**
     * Handles movement logic for great admiral units.
     * @brief Executes AI behavior for units assigned to naval leadership roles.
     * Steps:
     *   1. Selects status if needed, leads legendary units.
     *   2. Constructs buildings, joins cities, performs heritage actions.
     *   3. Retreats to city or moves to safety if threatened.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_greatAdmiralMove();

    /**
     * Handles movement logic for merchant units.
     * @brief Executes AI behavior for units assigned to trade and economic roles.
     * Steps:
     *   1. Selects status if needed, performs trade missions.
     *   2. Constructs buildings, discovers technology, performs caravan actions.
     *   3. Evaluates golden age, retreats to city or moves to safety.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_merchantMove();

    /**
     * Handles movement logic for engineer units.
     * @brief Executes AI behavior for units assigned to engineering and construction roles.
     * Steps:
     *   1. Selects status if needed, constructs buildings, switches hurry mode.
     *   2. Discovers technology, evaluates golden age, retreats to city or moves to safety.
     *   3. Skips turn if no other actions are possible.
     */
	void AI_engineerMove();
    
    /**
     * Handles movement logic for subdued animal units.
     * @brief Executes AI behavior for units assigned to animal roles.
     * Steps:
     *   1. Heals if wounded, constructs buildings, performs outcome missions.
     *   2. Upgrades if possible, scraps if surplus, retreats to city or moves to safety.
     *   3. Skips turn if no other actions are possible.
     */
	void AI_subduedAnimalMove();


    /**
     * Handles movement logic for spy units.
     * @brief Executes AI behavior for units assigned to espionage missions.
     * Steps:
     *   1. Evaluates espionage opportunities in enemy territory.
     *   2. Performs infiltration, sabotage, or city revolts if possible.
     *   3. Loads onto transports if needed.
     *   4. Retreats to city or moves to safety if threatened.
     *   5. Skips turn if no other actions are possible.
     */
	void AI_spyMove();

    /**
     * Handles movement logic for ICBM units.
     * @brief Executes AI behavior for nuclear missile units.
     * Steps:
     *   1. Evaluates possible nuclear strike targets.
     *   2. Launches attack if a valid target is found.
     *   3. Retreats or skips turn if no action is possible.
     */
	void AI_ICBMMove();

    /**
     * Handles movement logic for sea worker units.
     * @brief Executes AI behavior for units improving water tiles.
     * Steps:
     *   1. Selects status if needed.
     *   2. Improves bonuses and local plots.
     *   3. Retreats to city or moves to safety if threatened.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_workerSeaMove();

    /**
     * Handles movement logic for barbarian attack sea units.
     * @brief Executes AI behavior for barbarian ships focused on attacking and pillaging.
     * Steps:
     *   1. Selects status if needed.
     *   2. Attacks targets, pillages, heals, and patrols.
     *   3. Retreats to city or moves to safety if threatened.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_barbAttackSeaMove();

    /**
     * Handles movement logic for pirate sea units.
     * @brief Executes AI behavior for pirate ships focused on pillaging and blockading.
     * Steps:
     *   1. Selects status if needed.
     *   2. Pillages, blockades, heals, and explores.
     *   3. Retreats to city or moves to safety if threatened.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_pirateSeaMove();

    /**
     * Handles movement logic for attack sea units.
     * @brief Executes AI behavior for naval units assigned to attack roles.
     * Steps:
     *   1. Selects status if needed.
     *   2. Attacks, bombards, heals, and groups with other attack units.
     *   3. Pillages, protects, retreats, or moves to safety.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_attackSeaMove();

    /**
     * Handles movement logic for reserve sea units.
     * @brief Executes AI behavior for naval units assigned to reserve roles.
     * Steps:
     *   1. Guards bonuses, heals, attacks, and bombards.
     *   2. Shadows transports, groups with reserves, and protects.
     *   3. Retreats to city or moves to safety if threatened.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_reserveSeaMove();

    /**
     * Handles movement logic for escort sea units.
     * @brief Executes AI behavior for naval units assigned to escort transports.
     * Steps:
     *   1. Selects status if needed.
     *   2. Heals, attacks, groups with carriers or assault units.
     *   3. Escorts flotillas, retreats to city, or moves to safety.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_escortSeaMove();

    /**
     * Handles movement logic for explorer sea units.
     * @brief Executes AI behavior for naval units assigned to explore the map.
     * Steps:
     *   1. Selects status if needed.
     *   2. Explores, heals, patrols, and retreats to city if threatened.
     *   3. Skips turn if no other actions are possible.
     */
	void AI_exploreSeaMove();

    /**
     * Handles movement logic for assault sea units.
     * @brief Executes AI behavior for naval units assigned to launch invasions.
     * Steps:
     *   1. Selects status if needed.
     *   2. Loads cargo, attacks, reinforces, or retreats.
     *   3. Bombards, moves to safety, or skips turn if no other actions are possible.
     */
	void AI_assaultSeaMove();

    /**
     * Handles movement logic for settler sea units.
     * @brief Executes AI behavior for naval transports carrying settlers and workers.
     * Steps:
     *   1. Selects status if needed.
     *   2. Delivers settlers and workers to optimal city sites.
     *   3. Retreats to city or moves to safety if threatened.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_settlerSeaMove();

    /**
     * Handles movement logic for missionary sea units.
     * @brief Executes AI behavior for naval transports carrying missionaries.
     * Steps:
     *   1. Selects status if needed.
     *   2. Delivers missionaries to target cities.
     *   3. Explores, retreats to city, or moves to safety.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_missionarySeaMove();

    /**
     * Handles movement logic for spy sea units.
     * @brief Executes AI behavior for naval transports carrying spies.
     * Steps:
     *   1. Selects status if needed.
     *   2. Delivers spies to target cities.
     *   3. Retreats to city or moves to safety if threatened.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_spySeaMove();

    /**
     * Handles movement logic for carrier sea units.
     * @brief Executes AI behavior for naval carriers transporting air units.
     * Steps:
     *   1. Selects status if needed.
     *   2. Heals, groups with other carriers, and delivers air units.
     *   3. Retreats to city or moves to safety if threatened.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_carrierSeaMove();

    /**
     * Handles movement logic for missile carrier sea units.
     * @brief Executes AI behavior for naval carriers transporting missiles.
     * Steps:
     *   1. Selects status if needed.
     *   2. Delivers missiles to target locations.
     *   3. Retreats to city or moves to safety if threatened.
     *   4. Skips turn if no other actions are possible.
     */
	void AI_missileCarrierSeaMove();

    /**
     * Handles movement logic for attack air units.
     * @brief Executes AI behavior for air units assigned to attack missions.
     * Steps:
     *   1. Evaluates air strike, bombard, and recon opportunities.
     *   2. Supports ground attacks and defends base if needed.
     *   3. Retreats or skips turn if no action is possible.
     */
	void AI_attackAirMove();

    /**
     * Handles movement logic for defense air units.
     * @brief Executes AI behavior for air units assigned to defense missions.
     * Steps:
     *   1. Evaluates air defense, recon, and healing opportunities.
     *   2. Defends base city or moves to optimal defensive position.
     *   3. Retreats or skips turn if no action is possible.
     */
	void AI_defenseAirMove();

    /**
     * Handles movement logic for carrier air units.
     * @brief Executes AI behavior for air units assigned to carrier missions.
     * Steps:
     *   1. Delivers air units to carrier or base city.
     *   2. Defends base or moves to optimal position.
     *   3. Retreats or skips turn if no action is possible.
     */
	void AI_carrierAirMove();

    /**
     * Handles movement logic for missile air units.
     * @brief Executes AI behavior for air units assigned to missile missions.
     * Steps:
     *   1. Delivers missiles to target locations.
     *   2. Performs air strikes or bombards defenses.
     *   3. Retreats or skips turn if no action is possible.
     */
	void AI_missileAirMove();

    /**
     * Handles movement logic for healer units.
     * @brief Executes AI behavior for units assigned to heal other units.
     * Steps:
     *   1. Evaluates healing opportunities for nearby units.
     *   2. Moves to optimal healing location or retreats to city.
     *   3. Skips turn if no action is possible.
     */
	void AI_healerMove();

    /**
     * Handles movement logic for healer sea units.
     * @brief Executes AI behavior for naval units assigned to heal other units.
     * Steps:
     *   1. Evaluates healing opportunities for nearby units.
     *   2. Moves to optimal healing location or retreats to city.
     *   3. Skips turn if no action is possible.
     */
	void AI_healerSeaMove();

    /**
     * Handles movement logic for property control units.
     * @brief Executes AI behavior for units assigned to control negative properties (crime, disease, etc.).
     * Steps:
     *   1. Evaluates property control needs in cities.
     *   2. Moves to optimal location or performs arrest.
     *   3. Retreats to city or skips turn if no action is possible.
     */
	void AI_propertyControlMove();

    /**
     * Handles movement logic for property control sea units.
     * @brief Executes AI behavior for naval units assigned to control negative properties.
     * Steps:
     *   1. Evaluates property control needs in coastal cities.
     *   2. Moves to optimal location or performs arrest.
     *   3. Retreats to city or skips turn if no action is possible.
     */
	void AI_propertyControlSeaMove();

    /**
     * Handles movement logic for barbarian criminal units.
     * @brief Executes AI behavior for barbarian units focused on crime missions.
     * Steps:
     *   1. Attempts assassination or crime actions.
     *   2. Moves to enemy cities or hides from player units.
     *   3. Skips turn if no action is possible.
     */
	void AI_BarbCriminalMove();

    /**
     * Handles movement logic for investigator units.
     * @brief Executes AI behavior for units assigned to investigate and arrest criminals.
     * Steps:
     *   1. Evaluates arrest opportunities and property control needs.
     *   2. Moves to optimal location or performs arrest.
     *   3. Retreats to city or skips turn if no action is possible.
     */
	void AI_InvestigatorMove();

    /**
     * Handles movement logic for infiltrator units.
     * @brief Executes AI behavior for units assigned to infiltrate enemy territory.
     * Steps:
     *   1. Evaluates attack and infiltration opportunities.
     *   2. Moves to enemy cities or performs pillage actions.
     *   3. Retreats to city or skips turn if no action is possible.
     */
	void AI_InfiltratorMove();

    /**
     * Handles movement logic for see invisible units.
     * @brief Executes AI behavior for units assigned to detect invisible units.
     * Steps:
     *   1. Evaluates detection opportunities and property control needs.
     *   2. Moves to optimal location or performs arrest.
     *   3. Retreats to city or skips turn if no action is possible.
     */
	void AI_SeeInvisibleMove();

    /**
     * Handles movement logic for see invisible sea units.
     * @brief Executes AI behavior for naval units assigned to detect invisible units.
     * Steps:
     *   1. Evaluates detection opportunities and property control needs.
     *   2. Moves to optimal location or performs arrest.
     *   3. Retreats to city or skips turn if no action is possible.
     */
	void AI_SeeInvisibleSeaMove();

    /**
     * Handles movement logic for escort units.
     * @brief Executes AI behavior for units assigned to escort vulnerable units.
     * Steps:
     *   1. Evaluates escort opportunities for workers and settlers.
     *   2. Moves to optimal location or retreats to city.
     *   3. Skips turn if no action is possible.
     */
	void AI_EscortMove();

    /**
     * Retreats the unit if it cannot defend itself.
     * @brief Moves vulnerable units to safety if threatened.
     * Steps:
     *   1. Checks if the group can defend.
     *   2. Evaluates plot danger.
     *   3. Retreats to city if needed.
     */
	bool AI_retreatIfCantDefend();

    /**
     * Handles automated network building for workers.
     * @brief Executes AI logic for building routes and connecting resources.
     * Steps:
     *   1. Connects bonuses and cities.
     *   2. Improves bonuses and builds routes.
     *   3. Retreats to city or moves to safety if threatened.
     *   4. Skips turn if no action is possible.
     */
	void AI_networkAutomated();

    /**
     * Handles automated city improvement for workers.
     * @brief Executes AI logic for improving city plots.
     * Steps:
     *   1. Improves city plots if possible.
     *   2. Retreats to city or moves to safety if threatened.
     *   3. Skips turn if no action is possible.
     */
	void AI_cityAutomated();

	//bool AI_load(UnitAITypes eUnitAI, MissionAITypes eMissionAI, UnitAITypes eTransportedUnitAI = NO_UNITAI, int iMinCargo = -1, int iMinCargoSpace = -1, int iMaxCargoSpace = -1, int iMaxCargoOurUnitAI = -1, int iFlags = 0, int iMaxPath = MAX_INT, int iMaxTransportPath = MAX_INT);

 /**
     * Determines if this unit is the best defender for a city and assigns it if appropriate.
     * @brief Assigns the unit as the city's best defender if needed.
     * Steps:
     *   1. Checks if the unit is in a city and owned by the player.
     *   2. Compares unit's defensive value to other units in the city.
     *   3. Assigns as best defender if criteria are met.
     *   4. Returns true if a mission was pushed.
     */
	bool AI_guardCityBestDefender();

    /**
     * Determines if this unit is needed as a minimum defender for a city.
     * @brief Assigns the unit as a minimum defender if required.
     * Steps:
     *   1. Checks if the unit is in a city and owned by the player.
     *   2. Evaluates city's minimum defender requirements.
     *   3. Assigns as defender if needed.
     *   4. Returns true if a mission was pushed.
     */
	bool AI_guardCityMinDefender(bool bSearch = true);

    /**
     * Assigns the unit to guard a city, optionally leaving or searching for a city.
     * @brief Moves the unit to defend a city based on parameters.
     * Steps:
     *   1. Checks if the unit can defend.
     *   2. Searches for nearby cities needing defense.
     *   3. Moves to city and assigns as defender.
     *   4. Returns true if a mission was pushed.
     */
	bool AI_guardCity(bool bLeave = false, bool bSearch = false, int iMaxPath = MAX_INT);

    /**
     * Attempts to airlift the unit to a city needing defense.
     * @brief Finds a city with airlift capacity and moves the unit there.
     * Steps:
     *   1. Checks if the unit is in a city with airlift capability.
     *   2. Searches for target cities needing defense.
     *   3. Airlifts the unit if possible.
     *   4. Returns true if a mission was pushed.
     */
	bool AI_guardCityAirlift();

    /**
     * Assigns the unit to guard a bonus resource.
     * @brief Moves the unit to defend a plot with a valuable resource.
     * Steps:
     *   1. Searches for resource plots needing defense.
     *   2. Evaluates value and path to each plot.
     *   3. Moves to the best plot and assigns as defender.
     *   4. Returns true if a mission was pushed.
     */
	bool AI_guardBonus(int iMinValue = 0);

    /**
     * Calculates the number of defenders needed for a plot.
     * @brief Evaluates threats and strategic value to determine required defenders.
     * Steps:
     *   1. Checks for resource, fort, and airbase value.
     *   2. Evaluates nearby hostiles and defense modifiers.
     *   3. Returns the calculated number of defenders needed.
     */
	int AI_getPlotDefendersNeeded(const CvPlot* pPlot, int iExtra) const;

    /**
     * Assigns the unit to guard a fort.
     * @brief Moves the unit to defend a fort if needed.
     * Steps:
     *   1. Searches for forts needing defense.
     *   2. Evaluates value and path to each fort.
     *   3. Moves to the best fort and assigns as defender.
     *   4. Returns true if a mission was pushed.
     */
	bool AI_guardFort(bool bSearch = true);

    /**
     * Ensures each fort has at least one defender.
     * @brief Moves the unit to a fort if it lacks a minimum defender.
     * Steps:
     *   1. Searches for forts with insufficient defenders.
     *   2. Evaluates value and path to each fort.
     *   3. Moves to the best fort and assigns as defender.
     *   4. Returns true if a mission was pushed.
     */
	bool AI_guardFortMinDefender(bool bSearch = true);

    /**
     * Assigns the unit to guard a city site.
     * @brief Moves the unit to defend a planned city location.
     * Steps:
     *   1. Searches for city sites needing defense.
     *   2. Evaluates value and path to each site.
     *   3. Moves to the best site and assigns as defender.
     *   4. Returns true if a mission was pushed.
     */
	bool AI_guardCitySite();

    /**
     * Assigns the unit to guard a city against spies.
     * @brief Moves the unit to defend a city from espionage.
     * Steps:
     *   1. Searches for cities vulnerable to spies.
     *   2. Evaluates value and path to each city.
     *   3. Moves to the best city and assigns as defender.
     *   4. Returns true if a mission was pushed.
     */
	bool AI_guardSpy(int iRandomPercent);

    /**
     * Assigns the unit to defend a city against choke attacks.
     * @brief Moves the unit to defend a city from enemy choke points.
     * Steps:
     *   1. Checks for cities needing choke defense.
     *   2. Evaluates defense needs and moves to city if required.
     *   3. Returns true if a mission was pushed.
     */
	bool AI_chokeDefend();

    /**
     * Heals the unit if damaged.
     * @brief Moves the unit to a safe location to heal if necessary.
     * Steps:
     *   1. Checks unit and group damage.
     *   2. Searches for safe plots to heal.
     *   3. Moves to healing location or skips turn.
     *   4. Returns true if a mission was pushed.
     */
	bool AI_heal(int iDamagePercent = 0, int iMaxPath = MAX_INT);

    /**
     * Handles post-attack actions for the unit.
     * @brief Determines next action after attacking (heal, retreat, etc.).
     * Steps:
     *   1. Checks if unit can attack again.
     *   2. Evaluates defensive needs and healing.
     *   3. Moves to safety or skips turn.
     *   4. Returns true if a mission was pushed.
     */
	bool AI_afterAttack();

    /**
     * Attempts to found a religion with this unit.
     * @brief Executes logic for founding a new religion if possible.
     * Steps:
     *   1. Checks if the unit can found a religion at its current location.
     *   2. Executes the founding action if conditions are met.
     *   3. Returns true if the religion was founded.
     */
	bool AI_foundReligion();
#ifdef OUTBREAKS_AND_AFFLICTIONS
    /**
     * Attempts to cure an affliction using this unit.
     * @brief Executes logic for curing a specific affliction line.
     * Steps:
     *   1. Checks if the unit has the ability to cure the specified affliction.
     *   2. Performs the cure action if possible.
     *   3. Returns true if the affliction was cured.
     */
	bool AI_cureAffliction(PromotionLineTypes eAfflictionLine);
#endif

    /**
     * Attempts to trigger a Golden Age with this unit.
     * @brief Executes logic for starting a Golden Age if possible.
     * Steps:
     *   1. Checks if the unit can trigger a Golden Age.
     *   2. Performs the Golden Age action if allowed.
     *   3. Returns true if a Golden Age was started.
     */
	bool AI_goldenAge();

    /**
     * Attempts to spread religion in a city.
     * @brief Executes logic for spreading religion to eligible cities.
     * Steps:
     *   1. Identifies target cities for spreading religion.
     *   2. Moves to the city and performs the spread action.
     *   3. Returns true if religion was spread.
     */
	bool AI_spreadReligion();

    /**
     * Attempts to spread a corporation in a city.
     * @brief Executes logic for spreading a corporation to eligible cities.
     * Steps:
     *   1. Identifies target cities for spreading a corporation.
     *   2. Moves to the city and performs the spread action.
     *   3. Returns true if corporation was spread.
     */
	bool AI_spreadCorporation();

    /**
     * Attempts to airlift and spread religion.
     * @brief Executes logic for airlifting the unit to a city and spreading religion.
     * Steps:
     *   1. Finds a city with airlift capability and a valid religion target.
     *   2. Airlifts the unit and performs the spread action.
     *   3. Returns true if religion was spread via airlift.
     */
	bool AI_spreadReligionAirlift();

    /**
     * Attempts to airlift and spread a corporation.
     * @brief Executes logic for airlifting the unit to a city and spreading a corporation.
     * Steps:
     *   1. Finds a city with airlift capability and a valid corporation target.
     *   2. Airlifts the unit and performs the spread action.
     *   3. Returns true if corporation was spread via airlift.
     */
	bool AI_spreadCorporationAirlift();

    /**
     * Attempts to discover a technology with this unit.
     * @brief Executes logic for using the unit to boost research or finish a technology.
     * Steps:
     *   1. Checks if the unit can discover a technology.
     *   2. Applies research points to the target technology.
     *   3. Returns true if the technology was discovered or boosted.
     */
	bool AI_discover(const bool bFirstResearchOnly = false);

    /**
     * Attempts to lead a legendary unit or stack.
     * @brief Executes logic for attaching this unit as a leader to a legendary unit.
     * Steps:
     *   1. Searches for eligible legendary units.
     *   2. Moves to the unit and attaches as a leader.
     *   3. Returns true if leadership was established.
     */
	bool AI_leadLegend();

    /**
     * Attempts to lead a group of units of specified AI types.
     * @brief Executes logic for attaching this unit as a leader to a group.
     * Steps:
     *   1. Searches for eligible units/groups of specified AI types.
     *   2. Moves to the group and attaches as a leader.
     *   3. Returns true if leadership was established.
     */
	bool AI_lead(std::vector<UnitAITypes>& aeAIUnitTypes);

    /**
     * Attempts to join a city as a specialist.
     * @brief Executes logic for settling the unit in a city to provide specialist benefits.
     * Steps:
     *   1. Searches for eligible cities.
     *   2. Moves to the city and joins as a specialist.
     *   3. Returns true if the unit joined a city.
     */
	bool AI_join(int iMaxCount = MAX_INT);

    /**
     * Attempts to perform an outcome mission.
     * @brief Executes logic for missions that have random or special outcomes.
     * Steps:
     *   1. Identifies possible outcome missions.
     *   2. Moves to the target location and performs the mission.
     *   3. Returns true if an outcome mission was performed.
     */
	bool AI_outcomeMission();

    /**
     * Determines if a subdued animal unit should be scrapped.
     * @brief Executes logic for disbanding surplus subdued animal units.
     * Steps:
     *   1. Checks financial trouble and surplus unit count.
     *   2. Scraps the unit if surplus exists.
     *   3. Returns true if the unit was scrapped.
     */
	bool AI_scrapSubdued();

    /**
     * Moves the unit to its own territory if abroad.
     * @brief Executes logic for returning the unit to friendly territory.
     * Steps:
     *   1. Searches for the nearest friendly plot.
     *   2. Moves to the plot if found.
     *   3. Returns true if the unit moved to friendly territory.
     */
	bool AI_moveToOurTerritory(int maxMoves);
	bool AI_switchHurry();
	bool AI_hurry(bool bAny = false);

    /**
     * Attempts to switch the unit to a construct role if appropriate.
     * @brief Executes logic for retiring the unit to construct buildings or heritage.
     * Steps:
     *   1. Checks if the unit is outdated or eligible for construction.
     *   2. Switches AI type and assigns construct mission.
     *   3. Returns true if the switch was made.
     */
	bool checkSwitchToConstruct();
    /**
     * Attempts to construct a building with this unit.
     * @brief Executes logic for finding and constructing the best available building.
     * Steps:
     *   1. Evaluates possible buildings and target plots.
     *   2. Moves to the plot and performs construction.
     *   3. Returns true if construction was performed.
     */
	bool AI_construct(int iMaxCount = MAX_INT, int iMaxSingleBuildingCount = MAX_INT, int iThreshold = 0, bool bDecayProbabilities = false, bool assumeSameValueEverywhere = false);

    /**
     * Attempts to construct a heritage with this unit.
     * @brief Executes logic for finding and constructing the best available heritage.
     * Steps:
     *   1. Evaluates possible heritages and target plots.
     *   2. Moves to the plot and performs construction.
     *   3. Returns true if construction was performed.
     */
	int getBestConstructValue(int iMaxCount, int iMaxSingleBuildingCount, int iDecayProbabilityRate, int iThreshold, bool assumeSameValueEverywhere, CvPlot*& bestConstructPlot, CvPlot*& bestTurnPlot, CvUnitAI*& pBestTargetingUnit, BuildingTypes& eBestBuilding);

    /**
     * Attempts to construct a building or heritage at a target plot.
     * @brief Executes logic for moving to a plot and constructing the assigned building.
     * Steps:
     *   1. Validates target plot and building.
     *   2. Moves to the plot and performs construction.
     *   3. Returns true if construction was enacted.
     */
	bool enactConstruct(CvPlot* pBestConstructPlot, CvPlot* pBestPlot, CvUnitAI* eBestTargetingUnit, BuildingTypes eBestBuilding);


    /**
     * Attempts to construct a heritage with this unit.
     * @brief Executes logic for finding and constructing the best available heritage.
     * Steps:
     *   1. Evaluates possible heritages and target plots.
     *   2. Moves to the plot and performs construction.
     *   3. Returns true if construction was performed.
     */
	bool AI_heritage();
	
	int getBestHeritageValue(CvPlot*& pBestConstructPlot, CvPlot*& pBestPlot, CvUnitAI*& pTargetingUnit, HeritageTypes& eBest);

    /**
     * Attempts to perform an inquisition in a city.
     * @brief Executes logic for removing non-state religions from a city.
     * Steps:
     *   1. Identifies target cities for inquisition.
     *   2. Moves to the city and performs the inquisition.
     *   3. Returns true if inquisition was performed.
     */
	bool AI_doInquisition();

    /**
     * Attempts to perform a great work in a city.
     * @brief Executes logic for boosting culture in a city.
     * Steps:
     *   1. Identifies target cities for great work.
     *   2. Moves to the city and performs the great work.
     *   3. Returns true if great work was performed.
     */
	bool AI_greatWork();

    /**
     * Attempts to perform an offensive airlift.
     * @brief Executes logic for airlifting the unit to a target city for attack.
     * Steps:
     *   1. Identifies target cities for offensive airlift.
     *   2. Airlifts the unit and performs attack actions.
     *   3. Returns true if offensive airlift was performed.
     */
	bool AI_offensiveAirlift();

    /**
     * Attempts to perform a paradrop to a target plot.
     * @brief Executes logic for moving the unit via paradrop.
     * Steps:
     *   1. Identifies target plots for paradrop.
     *   2. Performs the paradrop action.
     *   3. Returns true if paradrop was performed.
     */
	bool AI_paradrop(int iRange);

    /**
     * Attempts to protect a plot or city.
     * @brief Executes logic for moving the unit to defend a location.
     * Steps:
     *   1. Identifies plots or cities needing protection.
     *   2. Moves to the location and defends.
     *   3. Returns true if protection was provided.
     */
	bool AI_protect(int iOddsThreshold, int iMaxPathTurns = MAX_INT);

    /**
     * Attempts to attack in a sea area.
     * @brief Executes logic for moving the unit to attack enemy units or cities in a sea area.
     * Steps:
     *   1. Identifies targets in the sea area.
     *   2. Moves to the target and attacks.
     *   3. Returns true if attack was performed.
     */
	bool AI_seaAreaAttack();

    /**
     * Attempts to patrol nearby plots.
     * @brief Executes logic for moving the unit to patrol and reveal fog of war.
     * Steps:
     *   1. Identifies adjacent plots for patrol.
     *   2. Moves to the best plot for patrol.
     *   3. Returns true if patrol was performed.
     */
	bool AI_patrol(bool bIgnoreDanger = false);

    /**
     * Attempts to defend a plot or city.
     * @brief Executes logic for moving the unit to defend a location.
     * Steps:
     *   1. Identifies plots or cities needing defense.
     *   2. Moves to the location and defends.
     *   3. Returns true if defense was provided.
     */
	bool AI_defend();
	
	void AI_safetyEval(const CvPlot* plotX, const int iPass, const int iRange, const bool bAnimalDanger, int &iBestValue, const CvPlot** pBestPlot);

    /**
     * Evaluates safety for the unit and moves to a safer location if needed.
     * @brief Executes logic for avoiding danger and seeking safe plots.
     * Steps:
     *   1. Evaluates danger in current and nearby plots.
     *   2. Moves to the safest available plot.
     *   3. Returns true if a safer location was found.
     */
	bool AI_safety(int iRange = 1);

    /**
     * Attempts to reach home territory.
     * @brief Executes logic for moving the unit to friendly territory or cities.
     * Steps:
     *   1. Identifies home plots or cities.
     *   2. Moves to the location if possible.
     *   3. Returns true if home was reached.
     */
	bool AI_reachHome(const bool bMockRun = false, int iRange = 0) const;

    /**
     * Attempts to hide the unit from enemies.
     * @brief Executes logic for moving the unit to a hidden or safe location.
     * Steps:
     *   1. Identifies plots suitable for hiding.
     *   2. Moves to the plot and hides.
     *   3. Returns true if hiding was performed.
     */
	bool AI_hide();

    /**
     * Attempts to move to and collect a goody hut.
     * @brief Executes logic for seeking and collecting goody huts.
     * Steps:
     *   1. Identifies goody huts within range.
     *   2. Moves to the hut and collects it.
     *   3. Returns true if a goody hut was collected.
     */
	bool AI_goody(int iRange);

    /**
     * Attempts to join offensive stacks as an explorer.
     * @brief Executes logic for explorers to join attacking stacks during war.
     * Steps:
     *   1. Checks if the unit is in an offensive war.
     *   2. Joins nearby offensive stacks if appropriate.
     *   3. Returns true if joined.
     */
	bool AI_explorerJoinOffensiveStacks();

    /**
     * Attempts to explore the map.
     * @brief Executes logic for moving the unit to unexplored areas.
     * Steps:
     *   1. Identifies unexplored plots.
     *   2. Moves to the best plot for exploration.
     *   3. Returns true if exploration was performed.
     */
	bool AI_explore();

    /**
     * Attempts to explore within a specified range.
     * @brief Executes logic for exploring nearby plots.
     * Steps:
     *   1. Identifies unexplored plots within range.
     *   2. Moves to the best plot for exploration.
     *   3. Returns true if exploration was performed.
     */
	bool AI_exploreRange(int iRange);

    /**
     * Attempts to refresh the exploration range.
     * @brief Executes logic for updating exploration targets and visibility.
     * Steps:
     *   1. Identifies new exploration targets.
     *   2. Moves to the best plot for exploration.
     *   3. Returns true if exploration was refreshed.
     */
	bool AI_refreshExploreRange(int iRange, bool bIncludeVisibilityRefresh = true);

    /**
     * Picks a target city for attack or mission.
     * @brief Executes logic for selecting the best city to target.
     * Steps:
     *   1. Evaluates all cities for strategic value.
     *   2. Selects the best city based on criteria.
     *   3. Returns a pointer to the target city.
     */
	CvCity* AI_pickTargetCity(int iFlags = 0, int iMaxPath = MAX_INT, bool bHuntBarbs = false);

    /**
     * Attempts to move to a target city.
     * @brief Executes logic for moving the unit to the selected city.
     * Steps:
     *   1. Calculates path to the target city.
     *   2. Moves to the city if possible.
     *   3. Returns true if movement was performed.
     */
	bool AI_goToTargetCity(int iFlags = 0, int iMaxPath = MAX_INT, const CvCity* pTargetCity = NULL);

    /**
     * Attempts to move to a barbarian city.
     * @brief Executes logic for moving the unit to a barbarian city.
     * Steps:
     *   1. Identifies barbarian cities within range.
     *   2. Moves to the best city if possible.
     *   3. Returns true if movement was performed.
     */
	bool AI_goToTargetBarbCity(int iMaxPath = 10);

    /**
     * Attempts to pillage around a target city.
     * @brief Executes logic for pillaging tiles near a city.
     * Steps:
     *   1. Identifies pillageable tiles around the city.
     *   2. Moves to and pillages the best tile.
     *   3. Returns true if pillaging was performed.
     */
	bool AI_pillageAroundCity(const CvCity* pTargetCity, int iBonusValueThreshold = 0, int iMaxPathTurns = MAX_INT);

    /**
     * Attempts to bombard a city.
     * @brief Executes logic for bombarding city defenses.
     * Steps:
     *   1. Checks if the unit can bombard the city.
     *   2. Performs bombardment if possible.
     *   3. Returns true if bombardment was performed.
     */
	bool AI_bombardCity();

    /**
     * Attempts to attack a city within range.
     * @brief Executes logic for attacking cities.
     * Steps:
     *   1. Identifies cities within attack range.
     *   2. Moves to and attacks the best city.
     *   3. Returns true if attack was performed.
     */
	bool AI_cityAttack(int iRange, int iOddsThreshold, bool bFollow = false);

    /**
     * Attempts any attack within range.
     * @brief Executes logic for attacking any valid target.
     * Steps:
     *   1. Identifies attackable plots within range.
     *   2. Moves to and attacks the best target.
     *   3. Returns true if attack was performed.
     */
	bool AI_anyAttack(int iRange, int iOddsThreshold, int iMinStack = 0, bool bAllowCities = true, bool bFollow = false);

    /**
     * Attempts a ranged bombardment on a plot.
     * @brief Executes logic for bombarding plots at range.
     * Steps:
     *   1. Identifies bombardable plots within range.
     *   2. Performs bombardment if possible.
     *   3. Returns true if bombardment was performed.
     * // RevolutionDCM
     */
    bool AI_RbombardPlot(int iRange, int iBonusValueThreshold);

    /**
     * Attempts a ranged bombardment on a unit.
     * @brief Executes logic for bombarding units at range.
     * Steps:
     *   1. Identifies units within bombardment range.
     *   2. Performs bombardment if possible.
     *   3. Returns true if bombardment was performed.
     */
	bool AI_RbombardUnit(int iRange, int iHighestOddsThreshold, int iMinStack, int iSeigeDiff, int iPowerThreshold, bool bCity = false);

    /**
     * Attempts a ranged bombardment on a city.
     * @brief Executes logic for bombarding city defenses at range.
     * Steps:
     *   1. Identifies cities within bombardment range.
     *   2. Performs bombardment if possible.
     *   3. Returns true if bombardment was performed.
     */
	bool AI_RbombardCity(const CvCity* pCity);

    /**
     * Attempts a naval ranged bombardment.
     * @brief Executes logic for bombarding naval targets at range.
     * Steps:
     *   1. Identifies naval targets within bombardment range.
     *   2. Performs bombardment if possible.
     *   3. Returns true if bombardment was performed.
     */
	bool AI_RbombardNaval();

    /**
     * Attempts a fighter engagement.
     * @brief Executes logic for engaging enemy air units.
     * Steps:
     *   1. Identifies enemy air units within range.
     *   2. Performs engagement if possible.
     *   3. Returns true if engagement was performed.
     */
	bool AI_FEngage();

    /**
     * Attempts a ranged attack within range.
     * @brief Executes logic for attacking targets at range.
     * Steps:
     *   1. Identifies targets within attack range.
     *   2. Performs attack if possible.
     *   3. Returns true if attack was performed.
     */
	bool AI_rangeAttack(int iRange);

    /**
     * Attempts to leave an attack if odds are unfavorable.
     * @brief Executes logic for retreating from an attack.
     * Steps:
     *   1. Evaluates attack odds and strength.
     *   2. Retreats if odds are below threshold.
     *   3. Returns true if retreat was performed.
     */
    bool AI_leaveAttack(int iRange, int iOddsThreshold, int iStrengthThreshold);

    /**
     * Attempts to blockade a plot or city.
     * @brief Executes logic for blockading enemy territory.
     * Steps:
     *   1. Identifies plots or cities for blockade.
     *   2. Moves to and performs blockade.
     *   3. Returns true if blockade was performed.
     */
	bool AI_blockade();

    /**
     * Calculates the blockade value for a plot.
     * @brief Returns a numeric score for blockading a plot or city.
     * Steps:
     *   1. Evaluates strategic value and path turns.
     *   2. Returns the computed blockade value.
     */
	int  AI_blockadeValue(const CvPlot* pLoopPlot, const CvCity* pCity, CvPlot*& endTurnPlot) const;

    /**
     * Attempts a pirate blockade.
     * @brief Executes logic for blockading with pirate units.
     * Steps:
     *   1. Identifies optimal blockade locations.
     *   2. Moves to and performs blockade.
     *   3. Returns true if blockade was performed.
     */
	bool AI_pirateBlockade();

    /**
     * Attempts a sea bombardment within range.
     * @brief Executes logic for bombarding sea targets.
     * Steps:
     *   1. Identifies sea targets within bombardment range.
     *   2. Performs bombardment if possible.
     *   3. Returns true if bombardment was performed.
     */
	bool AI_seaBombardRange(int iMaxRange);

    /**
     * Attempts to pillage a plot.
     * @brief Executes logic for pillaging enemy improvements.
     * Steps:
     *   1. Identifies pillageable plots.
     *   2. Moves to and pillages the best plot.
     *   3. Returns true if pillaging was performed.
     */
	bool AI_pillage(int iBonusValueThreshold = 0);

    /**
     * Attempts to pillage within a specified range.
     * @brief Executes logic for pillaging enemy improvements in range.
     * Steps:
     *   1. Identifies pillageable plots within range.
     *   2. Moves to and pillages the best plot.
     *   3. Returns true if pillaging was performed.
     */
	bool AI_pillageRange(int iRange, int iBonusValueThreshold = 0);

    /**
     * Attempts to found a city.
     * @brief Executes logic for founding a city at the best location.
     * Steps:
     *   1. Identifies optimal city sites.
     *   2. Moves to and founds the city.
     *   3. Returns true if a city was founded.
     */
	bool AI_found();

    /**
     * Attempts to found a city within a specified range.
     * @brief Executes logic for founding a city in range.
     * Steps:
     *   1. Identifies optimal city sites within range.
     *   2. Moves to and founds the city.
     *   3. Returns true if a city was founded.
     */
	bool AI_foundRange(int iRange, bool bFollow = false);

    /**
     * Attempts an assault sea transport mission.
     * @brief Executes logic for launching a naval invasion.
     * Steps:
     *   1. Identifies target locations for assault.
     *   2. Loads cargo and moves to target.
     *   3. Performs assault if possible.
     *   4. Returns true if assault was performed.
     */
	bool AI_assaultSeaTransport(bool bBarbarian = false);

    /**
     * Attempts to reinforce a sea assault.
     * @brief Executes logic for reinforcing naval invasions.
     * Steps:
     *   1. Identifies friendly stacks or cities needing reinforcement.
     *   2. Moves to and reinforces the target.
     *   3. Returns true if reinforcement was performed.
     */
	bool AI_assaultSeaReinforce(bool bBarbarian = false);

    /**
     * Attempts a settler sea transport mission.
     * @brief Executes logic for transporting settlers by sea.
     * Steps:
     *   1. Identifies optimal city sites for settlers.
     *   2. Loads settlers and moves to target.
     *   3. Unloads and founds city if possible.
     *   4. Returns true if transport was performed.
     */
	bool AI_settlerSeaTransport();

    /**
     * Attempts a settler sea ferry mission.
     * @brief Executes logic for ferrying workers by sea.
     * Steps:
     *   1. Identifies cities needing workers.
     *   2. Loads workers and moves to target.
     *   3. Unloads workers if possible.
     *   4. Returns true if ferry was performed.
     */
	bool AI_settlerSeaFerry();

    /**
     * Attempts a special sea transport mission for missionaries.
     * @brief Executes logic for transporting missionaries by sea.
     * Steps:
     *   1. Identifies cities needing missionaries.
     *   2. Loads missionaries and moves to target.
     *   3. Unloads missionaries if possible.
     *   4. Returns true if transport was performed.
     */
	bool AI_specialSeaTransportMissionary();

    /**
     * Attempts a special sea transport mission for spies.
     * @brief Executes logic for transporting spies by sea.
     * Steps:
     *   1. Identifies cities needing spies.
     *   2. Loads spies and moves to target.
     *   3. Unloads spies if possible.
     *   4. Returns true if transport was performed.
     */
	bool AI_specialSeaTransportSpy();

    /**
     * Attempts a carrier sea transport mission.
     * @brief Executes logic for transporting air units by carrier.
     * Steps:
     *   1. Identifies air units needing transport.
     *   2. Loads air units and moves to target.
     *   3. Unloads air units if possible.
     *   4. Returns true if transport was performed.
     */
	bool AI_carrierSeaTransport();

    /**
     * Attempts to connect a plot with a route.
     * @brief Executes logic for building routes to connect resources or cities.
     * Steps:
     *   1. Identifies plots needing connection.
     *   2. Moves to and builds route.
     *   3. Returns true if connection was performed.
     */
	bool AI_connectPlot(CvPlot* pPlot, int iRange = 0);

    /**
     * Attempts to improve a city.
     * @brief Executes logic for building improvements in a city.
     * Steps:
     *   1. Identifies plots in the city needing improvement.
     *   2. Moves to and builds improvement.
     *   3. Returns true if improvement was performed.
     */
	bool AI_improveCity(CvCity* pCity);

    /**
     * Attempts to improve a local plot.
     * @brief Executes logic for building improvements on nearby plots.
     * Steps:
     *   1. Identifies nearby plots needing improvement.
     *   2. Moves to and builds improvement.
     *   3. Returns true if improvement was performed.
     */
	bool AI_improveLocalPlot(int iRange, const CvCity* pIgnoreCity);

    /**
     * Attempts to improve the next city.
     * @brief Executes logic for moving to and improving the next city in need.
     * Steps:
     *   1. Identifies the next city needing improvement.
     *   2. Moves to and builds improvement.
     *   3. Returns true if improvement was performed.
     */
	bool AI_nextCityToImprove(CvCity* pCity);

    /**
     * Attempts to improve the next city via airlift.
     * @brief Executes logic for airlifting to and improving the next city in need.
     * Steps:
     *   1. Identifies the next city needing improvement and with airlift capability.
     *   2. Airlifts and builds improvement.
     *   3. Returns true if improvement was performed.
     */
	bool AI_nextCityToImproveAirlift();

    /**
     * Attempts to irrigate territory.
     * @brief Executes logic for building irrigation improvements.
     * Steps:
     *   1. Identifies plots needing irrigation.
     *   2. Moves to and builds irrigation.
     *   3. Returns true if irrigation was performed.
     */
	bool AI_irrigateTerritory();

    /**
     * Attempts to build a fort in territory.
     * @brief Executes logic for building forts, canals, or airbases.
     * Steps:
     *   1. Identifies plots suitable for forts.
     *   2. Moves to and builds fort, canal, or airbase.
     *   3. Returns true if construction was performed.
     */
	bool AI_fortTerritory(bool bCanal, bool bAirbase);

    /**
     * Attempts to improve a bonus resource.
     * @brief Executes logic for building improvements on resource tiles.
     * Steps:
     *   1. Identifies resource tiles needing improvement.
     *   2. Moves to and builds improvement.
     *   3. Returns true if improvement was performed.
     */
	bool AI_improveBonus(int iMinValue = 0, CvPlot** ppBestPlot = NULL, BuildTypes* peBestBuild = NULL, int* piBestValue = NULL);

    /**
     * Attempts to improve a plot.
     * @brief Executes logic for building a specific improvement on a plot.
     * Steps:
     *   1. Validates the plot and improvement.
     *   2. Moves to and builds improvement.
     *   3. Returns true if improvement was performed.
     */
	bool AI_improvePlot(CvPlot* pPlot, BuildTypes eBuild);

    /**
     * Determines the best build for a plot.
     * @brief Returns the optimal build type for a given plot.
     * Steps:
     *   1. Evaluates all possible builds for the plot.
     *   2. Selects the best build based on value.
     *   3. Returns the best build type.
     */
	BuildTypes AI_betterPlotBuild(const CvPlot* pPlot, BuildTypes eBuild) const;

    /**
     * Attempts to connect a bonus resource.
     * @brief Executes logic for building routes to connect resources.
     * Steps:
     *   1. Identifies resources needing connection.
     *   2. Moves to and builds route.
     *   3. Returns true if connection was performed.
     */
	bool AI_connectBonus(bool bTestTrade = true);

    /**
     * Attempts to connect a city.
     * @brief Executes logic for building routes to connect cities.
     * Steps:
     *   1. Identifies cities needing connection.
     *   2. Moves to and builds route.
     *   3. Returns true if connection was performed.
     */
	bool AI_connectCity();

    /**
     * Attempts to build a route to a city.
     * @brief Executes logic for building routes to connect cities.
     * Steps:
     *   1. Identifies cities needing routes.
     *   2. Moves to and builds route.
     *   3. Returns true if route was built.
     */
	bool AI_routeCity();

    /**
     * Attempts to build routes in territory.
     * @brief Executes logic for building routes in owned territory.
     * Steps:
     *   1. Identifies plots needing routes.
     *   2. Moves to and builds route.
     *   3. Returns true if route was built.
     */
	bool AI_routeTerritory(bool bImprovementOnly = false);

    /**
     * Attempts to travel to a city for upgrading.
     * @brief Executes logic for moving the unit to a city where it can be upgraded.
     * Steps:
     *   1. Identifies cities with upgrade facilities.
     *   2. Moves to the city.
     *   3. Returns true if movement was performed.
     */
	bool AI_travelToUpgradeCity();

    /**
     * Attempts to retreat to a city.
     * @brief Executes logic for moving the unit to a safe city.
     * Steps:
     *   1. Identifies safe cities for retreat.
     *   2. Moves to the city.
     *   3. Returns true if retreat was performed.
     */
	bool AI_retreatToCity(bool bPrimary = false, bool bAirlift = false, int iMaxPath = MAX_INT);

    /**
     * Attempts to pick up units of a given AI type.
     * @brief Executes logic for loading units onto transports.
     * Steps:
     *   1. Identifies units needing pickup.
     *   2. Moves to and loads units.
     *   3. Returns true if pickup was performed.
     */
	bool AI_pickup(UnitAITypes eUnitAI, bool bCountProduction = false, int iMaxPath = MAX_INT);

    /**
     * Attempts to pick up stranded units.
     * @brief Executes logic for rescuing units stranded on land or sea.
     * Steps:
     *   1. Identifies stranded units.
     *   2. Moves to and loads units.
     *   3. Returns true if rescue was performed.
     */
	bool AI_pickupStranded(UnitAITypes eUnitAI = NO_UNITAI, int iMaxPath = MAX_INT);

    /**
     * Attempts to move air units to an offensive city.
     * @brief Executes logic for relocating air units for attack.
     * Steps:
     *   1. Identifies cities needing offensive air units.
     *   2. Moves to the city.
     *   3. Returns true if relocation was performed.
     */
	bool AI_airOffensiveCity();

    /**
     * Attempts to move air units to a defensive city.
     * @brief Executes logic for relocating air units for defense.
     * Steps:
     *   1. Identifies cities needing defensive air units.
     *   2. Moves to the city.
     *   3. Returns true if relocation was performed.
     */
	bool AI_airDefensiveCity();

    /**
     * Attempts to move air units to a carrier.
     * @brief Executes logic for loading air units onto carriers.
     * Steps:
     *   1. Identifies carriers needing air units.
     *   2. Moves to and loads onto carrier.
     *   3. Returns true if loading was performed.
     */
	bool AI_airCarrier();

    /**
     * Attempts to load missiles onto a carrier or transport.
     * @brief Executes logic for loading missile units.
     * Steps:
     *   1. Identifies carriers or transports needing missiles.
     *   2. Moves to and loads missiles.
     *   3. Returns true if loading was performed.
     */
	bool AI_missileLoad(UnitAITypes eTargetUnitAI, int iMaxOwnUnitAI = -1, bool bStealthOnly = false);

    /**
     * Attempts to perform an air strike.
     * @brief Executes logic for attacking targets with air units.
     * Steps:
     *   1. Identifies targets within air range.
     *   2. Performs air strike if possible.
     *   3. Returns true if air strike was performed.
     */
	bool AI_airStrike();

    /**
     * Calculates the base value for offensive air units at a plot.
     * @brief Returns a numeric score for placing offensive air units.
     * Steps:
     *   1. Evaluates plot for offensive air unit placement.
     *   2. Returns the computed value.
     */
	int AI_airOffenseBaseValue(const CvPlot* pPlot) const;

    /**
     * Attempts a defensive air strike.
     * @brief Executes logic for defending cities with air units.
     * Steps:
     *   1. Identifies enemy stacks threatening cities.
     *   2. Performs air strike if possible.
     *   3. Returns true if defensive air strike was performed.
     */
	bool AI_defensiveAirStrike();

    /**
     * Attempts a base air strike.
     * @brief Executes logic for attacking around base cities with air units.
     * Steps:
     *   1. Identifies targets near base city.
     *   2. Performs air strike if possible.
     *   3. Returns true if base air strike was performed.
     */
	bool AI_defendBaseAirStrike();

    /**
     * Attempts to bomb plots with air units.
     * @brief Executes logic for bombing enemy improvements.
     * Steps:
     *   1. Identifies plots to bomb.
     *   2. Performs bombing if possible.
     *   3. Returns true if bombing was performed.
     */
	bool AI_airBombPlots();

    /**
     * Attempts to bomb defenses with air units.
     * @brief Executes logic for bombing city defenses.
     * Steps:
     *   1. Identifies cities with defenses to bomb.
     *   2. Performs bombing if possible.
     *   3. Returns true if bombing was performed.
     */
	bool AI_airBombDefenses();

    /**
     * Attempts to explore with air units.
     * @brief Executes logic for revealing fog of war with air units.
     * Steps:
     *   1. Identifies unexplored plots within air range.
     *   2. Performs exploration if possible.
     *   3. Returns true if exploration was performed.
     */
	bool AI_exploreAir();

    /**
     * Calculates the value of exploring a plot with air units.
     * @brief Returns a numeric score for air exploration.
     * Steps:
     *   1. Evaluates plot for exploration value.
     *   2. Returns the computed value.
     */
	int AI_exploreAirPlotValue(const CvPlot* pPlot) const;

    /**
     * Attempts to explore with air units (alternative logic).
     * @brief Executes logic for revealing fog of war with air units.
     * Steps:
     *   1. Identifies unexplored plots within air range.
     *   2. Performs exploration if possible.
     *   3. Returns true if exploration was performed.
     */
	bool AI_exploreAir2();

    /**
     * Handles movement logic for air exploration.
     * @brief Executes AI behavior for air units assigned to explore.
     * Steps:
     *   1. Attempts standard and alternative air exploration.
     *   2. Performs air patrol if possible.
     *   3. Skips turn if no action is possible.
     */
	void AI_exploreAirMove();

    /**
     * Attempts to use a nuke on a city.
     * @brief Executes logic for nuclear attacks.
     * Steps:
     *   1. Identifies target cities for nuking.
     *   2. Performs nuclear attack if possible.
     *   3. Returns true if attack was performed.
     */
	bool AI_nuke();

    /**
     * Attempts to use a nuke within a specified range.
     * @brief Executes logic for nuclear attacks in range.
     * Steps:
     *   1. Identifies target plots for nuking.
     *   2. Performs nuclear attack if possible.
     *   3. Returns true if attack was performed.
     */
	bool AI_nukeRange(int iRange);

    /**
     * Attempts to perform a trade mission.
     * @brief Executes logic for trading with other civilizations.
     * Steps:
     *   1. Identifies trade targets.
     *   2. Moves to and performs trade.
     *   3. Returns true if trade was performed.
     */
	bool AI_trade(int iValueThreshold);

	/**
	 * Attempts to perform a spy mission.
	 * @brief Executes logic for espionage activities.
	 * Steps:
	 *   1. Identifies espionage targets.
	 *   2. Moves to and performs spy mission.
	 *   3. Returns true if mission was performed.
	 */
	bool AI_infiltrate();

    /**
     * Performs a reconnaissance mission with a spy unit.
     * @brief Moves the spy within a given range to gather intelligence.
     * Steps:
     *   1. Searches for valuable plots within the specified range.
     *   2. Moves to the best target plot for reconnaissance.
     *   3. Returns true if a recon mission was performed.
     */
	bool AI_reconSpy(int iRange);

    /**
     * Attempts to cause a revolt in a besieged city using a spy.
     * @brief Executes logic for breaking down city defenses if troops are ready to attack.
     * Steps:
     *   1. Checks if the spy is in a city and at war with its owner.
     *   2. Evaluates if friendly troops are present and can capture the city.
     *   3. Performs the revolt mission if conditions are met.
     *   4. Returns true if the revolt was triggered.
     */
	bool AI_revoltCitySpy();

    /**
     * Performs a bonus offense mission with a spy.
     * @brief Moves the spy to attack or sabotage bonus resources within range.
     * Steps:
     *   1. Identifies bonus resources in range.
     *   2. Moves to the best target and performs sabotage.
     *   3. Returns true if the mission was performed.
     */
	bool AI_bonusOffenseSpy(int iMaxPath);

    /**
     * Performs a city offense mission with a spy.
     * @brief Moves the spy to attack or sabotage enemy cities within range.
     * Steps:
     *   1. Identifies enemy cities within range, skipping specified city if needed.
     *   2. Moves to the best target city and performs sabotage.
     *   3. Returns true if the mission was performed.
     */
	bool AI_cityOffenseSpy(const int iRange, const CvCity* pSkipCity = NULL);

    /**
     * Performs an espionage mission with a spy.
     * @brief Executes logic for performing espionage actions in enemy territory.
     * Steps:
     *   1. Identifies possible espionage missions at the current plot.
     *   2. Executes the best available espionage mission.
     *   3. Returns true if the mission was performed.
     */
	bool AI_espionageSpy();

    /**
     * Moves the unit to a staging city for military operations.
     * @brief Finds the best city to gather units before an attack.
     * Steps:
     *   1. Identifies staging cities based on war plans and strategic value.
     *   2. Moves to the best staging city.
     *   3. Returns true if movement was performed.
     */
	bool AI_moveToStagingCity();
	//bool AI_seaRetreatFromCityDanger();
	//bool AI_airRetreatFromCityDanger();
	//bool AI_airAttackDamagedSkip();

    /**
     * Follows a bombardment mission.
     * @brief Coordinates with other units to perform or wait for bombardment.
     * Steps:
     *   1. Checks if the group can bombard the current plot.
     *   2. Waits for other units to finish bombardment if necessary.
     *   3. Returns true if a bombardment mission was performed or waited for.
     */
	bool AI_followBombard();

    /**
     * Determines if a given team is a potential enemy at a plot.
     * @brief Checks diplomatic status and war plans.
     * Steps:
     *   1. Evaluates team relations and war status.
     *   2. Returns true if the team is a potential enemy.
     */
	bool AI_potentialEnemy(TeamTypes eTeam, const CvPlot* pPlot = NULL);

    /**
     * Determines if a plot needs defense.
     * @brief Evaluates if the unit should defend the specified plot.
     * Steps:
     *   1. Checks for terrain damage and city presence.
     *   2. Evaluates defense needs based on threats and bonuses.
     *   3. Returns true if defense is needed.
     */
	bool AI_defendPlot(const CvPlot* pPlot) const;

    /**
     * Calculates the value of pillaging a plot.
     * @brief Returns a numeric score for pillaging based on resources and improvements.
     * Steps:
     *   1. Evaluates bonus resources and improvements on the plot.
     *   2. Calculates value based on strategic importance.
     *   3. Returns the computed pillage value.
     */
	int AI_pillageValue(const CvPlot* pPlot, int iBonusValueThreshold = 0) const;

    /**
     * Calculates the value of nuking a city.
     * @brief Returns a numeric score for using a nuke on a city.
     * Steps:
     *   1. Evaluates population, culture, and strategic importance.
     *   2. Considers collateral damage and diplomatic consequences.
     *   3. Returns the computed nuke value.
     */
	int AI_nukeValue(const CvCity* pCity) const;

    /**
     * Determines if the unit can pillage a plot.
     * @brief Checks if pillaging is allowed based on ownership and enemy status.
     * Steps:
     *   1. Checks if the plot is owned by an enemy.
     *   2. Returns true if pillaging is possible.
     */
	bool AI_canPillage(const CvPlot& kPlot) const;

    /**
     * Validates if a plot is suitable for the unit to move to.
     * @brief Checks domain, terrain, and movement restrictions.
     * Steps:
     *   1. Evaluates domain type and terrain compatibility.
     *   2. Returns true if the plot is valid for movement.
     */
	bool AI_plotValid(const CvPlot* pPlot) const;

    /**
     * Calculates the final odds threshold for an attack.
     * @brief Adjusts attack odds based on city defenses and special targets.
     * Steps:
     *   1. Evaluates city defense status and enemy presence.
     *   2. Modifies odds threshold accordingly.
     *   3. Returns the final odds threshold.
     */
	int AI_finalOddsThreshold(const CvPlot* pPlot, int iOddsThreshold) const;

    /**
     * Calculates extra stack size for "stack of doom" logic.
     * @brief Returns a value used to determine optimal stack size.
     * Steps:
     *   1. Uses birthmark and era to calculate extra units.
     *   2. Returns the computed value.
     */
	int AI_stackOfDoomExtra() const;

    /**
     * Attempts a stack attack on a city within range.
     * @brief Moves the unit stack to attack a city if conditions are met.
     * Steps:
     *   1. Identifies cities within range and evaluates stack power.
     *   2. Moves to attack if power threshold is met.
     *   3. Returns true if attack was performed.
     */
	bool AI_stackAttackCity(int iRange, int iPowerThreshold, bool bFollow = true);

    /**
     * Moves the unit into a city within range.
     * @brief Finds the nearest city and moves the unit there.
     * Steps:
     *   1. Searches for cities within the specified range.
     *   2. Moves to the best city found.
     *   3. Returns true if movement was performed.
     */
	bool AI_moveIntoCity(int iRange, bool bOpponentOnly = false);

    /**
     * Moves the unit into the nearest owned city.
     * @brief Finds the closest friendly city and moves the unit there.
     * Steps:
     *   1. Searches for owned cities.
     *   2. Moves to the nearest city.
     *   3. Returns true if movement was performed.
     */
	bool AI_moveIntoNearestOwnedCity();

    /**
     * Performs a culture victory move for artist units.
     * @brief Boosts culture in the weakest city or performs a great work.
     * Steps:
     *   1. Identifies target city for culture boost.
     *   2. Moves to the city and performs the action.
     *   3. Returns true if the mission was performed.
     */
	bool AI_artistCultureVictoryMove();

    /**
     * Attempts to choke enemy units or cities within range.
     * @brief Moves the unit to block or disrupt enemy movement.
     * Steps:
     *   1. Identifies choke points within range.
     *   2. Moves to the best choke point.
     *   3. Returns true if choke was performed.
     */
	bool AI_choke(int iRange = 1, bool bDefensive = false);

    /**
     * Attempts to solve a blockage problem en route to a destination plot.
     * @brief Handles obstacles and declares war if necessary.
     * Steps:
     *   1. Evaluates path to destination.
     *   2. Handles blockages and diplomatic actions.
     *   3. Returns true if blockage was resolved.
     */
	bool AI_solveBlockageProblem(const CvPlot* pDestPlot, bool bDeclareWar);


    /**
     * Calculates the espionage target value for a plot.
     * @brief Returns a score for performing espionage at the specified plot.
     * Steps:
     *   1. Evaluates plot ownership and strategic value.
     *   2. Returns the computed espionage value.
     */
	int AI_getEspionageTargetValue(const CvPlot* pPlot, int iMaxPath, int iMinUsefulValue) const;




    /**
     * Marks the contract as fulfilled for this unit.
     * @brief Resets contract state and removes from broker.
     * Steps:
     *   1. Resets contract turn and state.
     *   2. Removes unit from contract broker.
     */
	void contractFulfilled();

	//bool AI_approximatePath(CvPlot* pToPlot, int iFlags, int* piPathTurns) const;
	//CvUnitAI* AI_cityConstructionTargeted(const CvCity* pCity, BuildingTypes eBuilding, const CvSelectionGroup* omitGroup) const;

	// added so under cheat mode we can call protected functions for testing
	friend class CvGameTextMgr;

public:
    /**
     * Triggers a hurry action for food.
     * - Used for automated units.
     */
	bool AI_hurryFood();

    /**
     * Attempts to claim strategic forts.
     * - Used for defense and territory control.
     */
	bool AI_claimForts(CvReachablePlotSet* pReachablePlots, int iMinValue = 0, int iMaxPath = -1);

    /**
     * Finds the best fort to build on a plot.
     * - Returns the optimal build type.
     */
	BuildTypes AI_findBestFort(const CvPlot* pPlot) const;

    /**
     * Attempts to claim strategic forts for defense or territory control.
     * @brief Moves the unit to occupy or defend forts based on strategic value.
     * Steps:
     *   1. Identifies forts needing occupation or defense.
     *   2. Evaluates strategic value and path to each fort.
     *   3. Moves to the best fort and claims it.
     *   4. Returns true if a fort was claimed.
     */
	bool AI_StrategicForts();

    /**
     * Performs a caravan mission for trade or resource delivery.
     * @brief Moves the unit to trade with cities or deliver resources.
     * Steps:
     *   1. Identifies target cities for caravan missions.
     *   2. Moves to the city and performs trade or delivery.
     *   3. Returns true if the caravan mission was performed.
     */
	bool AI_caravan(bool bAnyCity = false);

    /**
     * Executes a command mission for special units.
     * @brief Performs command actions such as leading troops or activating abilities.
     * Steps:
     *   1. Identifies command opportunities.
     *   2. Executes the command action.
     *   3. Returns true if the command was performed.
     */
	bool AI_command();

    /**
     * Performs automated pillaging of enemy territory.
     * @brief Moves the unit to pillage valuable tiles automatically.
     * Steps:
     *   1. Identifies pillageable tiles based on bonus value threshold.
     *   2. Moves to and pillages the best tile.
     *   3. Returns true if pillaging was performed.
     */
	bool AI_AutomatedPillage(int iBonusValueThreshold);

    /**
     * Executes search and destroy movement for hunter units.
     * @brief Moves the unit to seek and attack enemy units or animals.
     * Steps:
     *   1. Searches for targets in range.
     *   2. Moves to and attacks the best target.
     *   3. Returns true if an attack was performed.
     */
	void AI_SearchAndDestroyMove(bool bWithCommander = false);

    /**
     * Hunts targets within a specified range.
     * @brief Moves the unit to hunt enemies or animals in range.
     * Steps:
     *   1. Identifies targets within range and odds threshold.
     *   2. Moves to and attacks the best target.
     *   3. Returns true if hunting was performed.
     */
	bool AI_huntRange(int iRange, int iOddsThreshold, bool bStayInBorders = false,  int iMinValue = 0);

    /**
     * Performs city defense actions.
     * @brief Moves the unit to defend cities and respond to threats.
     * Steps:
     *   1. Identifies cities needing defense.
     *   2. Moves to and defends the city.
     *   3. Returns true if defense was performed.
     */
	void AI_cityDefense();

    /**
     * Performs border patrol actions.
     * @brief Moves the unit to patrol borders and prevent incursions.
     * Steps:
     *   1. Identifies border plots needing patrol.
     *   2. Moves to and patrols the border.
     *   3. Returns true if patrol was performed.
     */
	void AI_borderPatrol();

    /**
     * Returns the unit to national borders if abroad.
     * @brief Moves the unit to the nearest border plot.
     * Steps:
     *   1. Identifies the nearest border plot.
     *   2. Moves to the border.
     *   3. Returns true if movement was performed.
     */
	bool AI_returnToBorders();

    /**
     * Patrols borders to prevent enemy incursions.
     * @brief Moves the unit along borders in a circuit.
     * Steps:
     *   1. Identifies optimal patrol route along borders.
     *   2. Moves to and patrols border plots.
     *   3. Returns true if patrol was performed.
     */
	bool AI_patrolBorders();

    /**
     * Moves the unit to the border of owned territory.
     * @brief Moves the unit towards the edge of national borders.
     * Steps:
     *   1. Identifies border plots.
     *   2. Moves to the best border plot.
     *   3. Returns true if movement was performed.
     */
	bool AI_moveToBorders();

    /**
     * Performs automated pillage movement.
     * @brief Moves the unit to pillage tiles automatically.
     * Steps:
     *   1. Identifies pillageable tiles.
     *   2. Moves to and pillages the best tile.
     *   3. Returns true if pillaging was performed.
     */
	void AI_AutomatedpillageMove();

    /**
     * Performs automated air strike actions.
     * @brief Moves the unit to perform air strikes automatically.
     * Steps:
     *   1. Identifies targets for air strike.
     *   2. Performs air strike if possible.
     *   3. Returns true if air strike was performed.
     */
	void AI_autoAirStrike();


    /**
     * Performs air bombardment of cities.
     * @brief Moves the unit to bomb enemy cities.
     * Steps:
     *   1. Identifies cities for air bombardment.
     *   2. Performs bombardment if possible.
     *   3. Returns true if bombardment was performed.
     */
	bool AI_airBombCities();

    /**
     * Moves the unit to a target unit.
     * @brief Moves the unit to follow or protect a specified target.
     * Steps:
     *   1. Calculates path to the target unit.
     *   2. Moves to the target if possible.
     *   3. Returns true if movement was performed.
     */
	bool AI_moveToTarget(const CvUnit* pTarget);

    /**
     * Performs shadow movement for following other units.
     * @brief Moves the unit to shadow another unit or group.
     * Steps:
     *   1. Identifies units to shadow.
     *   2. Moves to follow the target unit.
     *   3. Returns true if movement was performed.
     */
	void AI_shadowMove();

    /**
     * Protects a target unit from danger.
     * @brief Moves the unit to defend or escort a specified target.
     * Steps:
     *   1. Identifies threats to the target unit.
     *   2. Moves to protect the target.
     *   3. Returns true if protection was provided.
     */
	bool AI_protectTarget(const CvUnit* pTarget);

    /**
     * Joins a military city for defense or attack.
     * @brief Moves the unit to a city for military operations.
     * Steps:
     *   1. Identifies military cities needing units.
     *   2. Moves to the city and joins its defense or attack.
     *   3. Returns true if joined.
     */
	bool AI_joinMilitaryCity(bool bNaval = false);

    /**
     * Determines if a plot is well defended.
     * @brief Evaluates the defensive strength of a plot and adjacent plots.
     * Steps:
     *   1. Checks for defenders in the plot and adjacent plots.
     *   2. Compares odds of defeat.
     *   3. Returns true if the plot is well defended.
     */
	bool AI_isPlotWellDefended(const CvPlot* pPlot, bool bIncludeAdjacent, int iOddsOfDefeat) const;

	int	AI_genericUnitValueTimes100(UnitValueFlags eFlags) const;


public:


	/** 	Set an override to the group head priority for this unit, to allow a unit that
	//	would not nomally lead a stack to do so - priority will be preserved when other groups
	//	joing one lead by a unit with an override, but reset if we join another group */
	virtual void AI_setLeaderPriority(int iPriority);	//	 -1 means reset to default

    /**
     * Selects a status effect (promotion) for the unit or stack.
     * @brief Chooses and applies the best status effect for the unit or group.
     * Steps:
     *   1. Evaluates available status effects.
     *   2. Applies the most beneficial status to the unit or stack.
     *   3. Returns true if a status was applied.
     */
	bool AI_selectStatus(bool bStack, CvUnit* pUnit = NULL);

    /**
     * Selects a status effect for the group.
     * @brief Applies the best status effect to the group if appropriate.
     * Steps:
     *   1. Evaluates group needs for status effects.
     *   2. Applies the best status to the group.
     *   3. Returns true if a status was applied.
     */
	bool AI_groupSelectStatus();

    /**
     * Fulfills investigator needs such as crime investigation.
     * @brief Moves the unit to investigate and arrest criminals.
     * Steps:
     *   1. Identifies plots or cities with crime or criminals.
     *   2. Moves to the location and performs investigation or arrest.
     *   3. Returns true if an action was performed.
     */
	bool AI_InvestigatorFulfillment();

    /**
     * Establishes stack coverage for seeing invisible units.
     * @brief Moves the unit to provide detection coverage for invisible units.
     * Steps:
     *   1. Identifies areas lacking invisible unit detection.
     *   2. Moves to the area to provide coverage.
     *   3. Returns true if coverage was established.
     */
	bool AI_establishStackSeeInvisibleCoverage();

    //bool AI_defensiveCollateral(int iThreshold, int iSearchRange);

    /**
     * Generates a safe path for vulnerable units to a target plot.
     * @brief Calculates a path that avoids danger for vulnerable units.
     * Steps:
     *   1. Evaluates possible paths to the target plot.
     *   2. Selects the safest path based on enemy presence and terrain.
     *   3. Returns true if a safe path was found.
     */
	bool generateSafePathforVulnerable(const CvPlot* pToPlot, int* piPathTurns = NULL) const;

    /**
     * Generates a path to a target plot ignoring danger.
     * @brief Calculates a path to the target plot without considering threats.
     * Steps:
     *   1. Evaluates possible paths to the target plot.
     *   2. Selects the shortest or most efficient path.
     *   3. Returns true if a path was found.
     */
	bool generateIgnoreDangerPath(const CvPlot* pToPlot, int* piPathTurns = NULL) const;

    /**
     * Sets the unit to wait on a specific UnitAI type.
     * @brief Adds or removes the specified UnitAI type from the waiting list.
     * Steps:
     *   1. Updates the waiting list for the unit.
     */
	void setToWaitOnUnitAI(UnitAITypes eUnitAI, bool bAdd);

    /**
     * Returns true if the unit is waiting on a specific UnitAI type.
     * @brief Checks if the unit is waiting for another unit of the given AI type.
     */
	bool isWaitingOnUnitAI(int iIndex) const;

    /**
     * Returns true if the unit is waiting on any UnitAI type.
     * @brief Checks if the unit is waiting for any other unit.
     */
	bool isWaitingOnUnitAIAny() const;

    /**
     * Sets the unit to wait on any UnitAI type.
     * @brief Marks the unit as waiting for any other unit.
     */
	void setWaitingOnUnitAIAny();

    /**
     * Returns true if the unit is a negative property unit (e.g. criminal).
     * @brief Checks if the unit has negative properties affecting cities.
     */
	bool AI_isNegativePropertyUnit() const;

    /**
     * Returns the aggression value of the unit for attack probability.
     * @brief Calculates aggression based on unit type and AI settings.
     * Steps:
     *   1. Evaluates unit's base aggression and modifiers.
     *   2. Returns the computed aggression value.
     */
	int getMyAggression(int iAttackProb) const;

    /**
     * Returns true if the unit is exposed to danger at a plot.
     * @brief Evaluates the danger level of the plot for the unit.
     * Steps:
     *   1. Checks for enemy units and unfavorable odds.
     *   2. Returns true if the plot is dangerous.
     */
	bool exposedToDanger(const CvPlot* pPlot, int acceptableOdds, bool bConsiderOnlyWorstThreat = false) const;

    /**
     * Finds a threatening unit at a plot.
     * @brief Identifies the most dangerous enemy unit at the plot.
     * Steps:
     *   1. Iterates through units at the plot.
     *   2. Evaluates threat level for each unit.
     *   3. Returns pointer to the most threatening unit found.
     */
	bool getThreateningUnit(const CvPlot* pPlot, CvUnit*& pThreateningUnit, const CvPlot* pAttackPlot, int& iIndex, bool bReturnWorstOfMultiple = false) const;

    /**
     * Clears static AI caches for all units.
     * @brief Resets cached player and plot sets for AI calculations.
     * Steps:
     *   1. Clears cached player and missionary plot set.
     */
	static void	AI_clearCaches();

	static PlayerTypes			m_cachedPlayer;
	static CvReachablePlotSet*	m_cachedMissionaryPlotset;
};

#endif
