#pragma once

#ifndef CIV4_GROUP_H
#define CIV4_GROUP_H

#include "LinkedList.h"
#include "CvPathGenerator.h"
#include "CvUnit.h"
#include "CvCity.h"
#include "idinfo_iterator_base.h"

class CvPlot;
class CvArea;
#ifdef USE_OLD_PATH_GENERATOR
class FAStarNode;
#endif

struct CachedEdgeCosts
{
	CachedEdgeCosts(int iCost = 0, int iBestMoveCost = 0, int iWorstMoveCost = 0, int iToPlotNodeCost = 0)
		: iCost(iCost)
		, iBestMoveCost(iBestMoveCost)
		, iWorstMoveCost(iWorstMoveCost)
		, iToPlotNodeCost(iToPlotNodeCost)
	{}

	int iCost;
	int iBestMoveCost;
	int iWorstMoveCost;
	int iToPlotNodeCost;
#ifdef _DEBUG
	CvPlot*	pFromPlot;
	CvPlot*	pToPlot;
#endif
};

struct StackCompare
{
	enum flags
	{
		None = 0,
		PotentialEnemy = 1 << 0,
		CheckCanAttack = 1 << 1,
		CheckCanMove = 1 << 2,
		Fast = 1 << 3
	};
};
DECLARE_FLAGS(StackCompare::flags);

class CvSelectionGroup
{

public:

	CvSelectionGroup();
	virtual ~CvSelectionGroup();

	DllExport void init(int iID, PlayerTypes eOwner);
	DllExport void uninit();			
	DllExport void reset(int iID = 0, PlayerTypes eOwner = NO_PLAYER, bool bConstructorCall = false);

	void kill();

	void doTurn();

	void resetHealing();

	bool showMoves() const;

	void updateTimers();
	bool doDelayedDeath();

	void playActionSound();

	bool pushMissionInternal(MissionTypes eMission, int iData1 = -1, int iData2 = -1, int iFlags = 0, bool bAppend = false, bool bManual = false, MissionAITypes eMissionAI = NO_MISSIONAI, CvPlot* pMissionAIPlot = NULL, CvUnit* pMissionAIUnit = NULL);		// Exposed to Python
	void pushMission(MissionTypes eMission, int iData1 = -1, int iData2 = -1, int iFlags = 0, bool bAppend = false, bool bManual = false, MissionAITypes eMissionAI = NO_MISSIONAI, CvPlot* pMissionAIPlot = NULL, CvUnit* pMissionAIUnit = NULL);		// Exposed to Python
	void popMission();																																										// Exposed to Python
	void autoMission();
	void updateMission();

	DllExport CvPlot* _lastMissionPlot();
	CvPlot* lastMissionPlot() const; // Exposed to Python

	bool canStartMission(int iMission, int iData1, int iData2, CvPlot* pPlot = NULL, bool bTestVisible = false, bool bUseCache = false) const;		// Exposed to Python
	bool startMission();
	bool continueMission(int iSteps = 0);
	bool checkMoveSafety(int iX, int iY, int iFlags);

	DllExport bool canDoInterfaceMode(InterfaceModeTypes eInterfaceMode);													// Exposed to Python
	DllExport bool canDoInterfaceModeAt(InterfaceModeTypes eInterfaceMode, CvPlot* pPlot);				// Exposed to Python

	void doCommand(CommandTypes eCommand, int iData1, int iData2);
	bool canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible = false, bool bUseCache = false, bool bAll = false) const;		// Exposed to Python
	bool canEverDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible, bool bUseCache) const;
	void setupActionCache();

	bool isHuman() const; // Exposed to Python

	DllExport bool _isBusy();
	bool isBusy() const;

	bool isCargoBusy() const;
	int baseMoves() const;																																										// Exposed to Python 
	bool isWaiting() const;																																							// Exposed to Python
	bool isFull() const;																																											// Exposed to Python
	bool hasCargo() const;																																										// Exposed to Python
	int getCargo(bool bVolume = false) const;

	DllExport bool _canAllMove();
	bool canAllMove() const; // Exposed to Python
	bool canAnyMove(bool bValidate = false) /* not const - Can set ACTIVITY_SLEEP if bValidate is true */; // Exposed to Python
	bool hasMoved() const;																																										// Exposed to Python
	bool canEnterTerritory(TeamTypes eTeam, bool bIgnoreRightOfPassage = false) const; // Exposed to Python
	bool canEnterArea(TeamTypes eTeam, const CvArea* pArea, bool bIgnoreRightOfPassage = false) const;									// Exposed to Python

	DllExport bool _canMoveInto(CvPlot* pPlot, bool bAttack = false);
	bool canMoveInto(const CvPlot* pPlot, bool bAttack = false) const; // Exposed to Python
	
	DllExport bool _canMoveOrAttackInto(CvPlot* pPlot, bool bDeclareWar = false);
	bool canMoveOrAttackInto(const CvPlot* pPlot, bool bDeclareWar = false) const; // Exposed to Python

	bool canMoveIntoWithWar(const CvPlot* pPlot, bool bAttack = false, bool bDeclareWar = false) const;

	bool canMoveThrough(const CvPlot* pPlot, bool bDeclareWar = false) const;																																	// Exposed to Python
	bool canFight() const;																																										// Exposed to Python 
	bool canDefend() const;																																										// Exposed to Python
	bool canBombard(const CvPlot* pPlot, bool bCheckCanReduceOnly = false) const;
	bool hasBombardCapability() const;
	bool hasCollateralDamage() const;
	bool canPillage(const CvPlot* pPlot) const;
	bool canBombardAtRanged(const CvPlot* pPlot, int iX, int iY) const;
	int getMinimumRBombardDamageLimit() const;
	int getMinimumRBombardRange() const;
	int getRBombardDamageMaxUnits() const;

	int getBombardTurns(const CvCity* pCity ) const;
	bool isHasPathToAreaPlayerCity( PlayerTypes ePlayer, int iFlags = 0, bool bGo = false ) /* not const - Can generate a mvoe to mission if bGo is true*/;
	bool isHasPathToAreaEnemyCity(int iFlags = 0, bool bGo = false ) /* not const - Can generate a mvoe to mission if bGo is true*/;
	bool isStranded();
	void invalidateIsStrandedCache();
	bool calculateIsStranded();
	bool canMoveAllTerrain() const;

	void unloadAll();
	bool alwaysInvisible() const;																																							// Exposed to Python
	bool isInvisible(TeamTypes eTeam) const;																								// Exposed to Python
	int countNumUnitAIType(UnitAITypes eUnitAI) const;																												// Exposed to Python
	bool hasWorker() const;																																										// Exposed to Python
	bool IsSelected() const;
	DllExport void NotifyEntity(MissionTypes eMission);
	void airCircle(bool bStart);
	void setBlockading(bool bStart);

	int getX() const;
	int getY() const;
	bool at(int iX, int iY) const;																																								// Exposed to Python
	bool atPlot(const CvPlot* pPlot) const;																																				// Exposed to Python
	CvPlot* plot() const;																																								// Exposed to Python
	DllExport CvPlot* plotExternal() const;																																								// Exposed to Python
	int getArea() const;
	CvArea* area() const;																																													// Exposed to Python
	DomainTypes getDomainType() const;

	RouteTypes getBestBuildRoute(const CvPlot* pPlot, BuildTypes* peBestBuild = NULL) const;	// Exposed to Python

	bool canIgnoreZoneofControl() const;

	bool groupDeclareWar(const CvPlot* pPlot, bool bForce = false);
	bool groupAttack(int iX, int iY, int iFlags, bool& bFailedAlreadyFighting, bool bStealth = false);
	void groupMove(CvPlot* pPlot, bool bCombat, CvUnit* pCombatUnit = NULL, bool bEndMove = false);
	bool groupPathTo(int iX, int iY, int iFlags);
	bool groupRoadTo(int iX, int iY, int iFlags);
	bool groupBuild(BuildTypes eBuild);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      04/18/10                                jdog5000      */
/*                                                                                              */
/* General AI                                                                                   */
/************************************************************************************************/
	void setTransportUnit(CvUnit* pTransportUnit, CvSelectionGroup** pOtherGroup = NULL);
	void setRemoteTransportUnit(CvUnit* pTransportUnit);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	bool isAmphibPlot(const CvPlot* pPlot) const;																																		// Exposed to Python
	bool groupAmphibMove(CvPlot* pPlot, int iFlags);

	DllExport bool readyToSelect(bool bAny = false) /* not const - calls readyToMove */;																										// Exposed to Python
	bool readyToMove(bool bAny = false, bool bValidate = false) /* not const - calls canAnyMove */;																																// Exposed to Python
	bool readyToAuto() const;																																									// Exposed to Python 

	int getID() const;																																												// Exposed to Python
	void setID(int iID);																			

	int getMissionTimer() const;
	void setMissionTimer(int iNewValue);
	void changeMissionTimer(int iChange);
	void updateMissionTimer(int iSteps = 0);

	bool isForceUpdate() const;
	void setForceUpdate(bool bNewValue);

	bool isMidMove() const { return m_bIsMidMove; }

	bool isChoosingNewLeader() const { return m_bIsChoosingNewLeader; }

	inline PlayerTypes getOwner() const { return m_eOwner; } // Exposed to Python

	TeamTypes getTeam() const;																																					// Exposed to Python

	ActivityTypes getActivityType() const;																															// Exposed to Python
	void setActivityType(ActivityTypes eNewValue, MissionTypes eSleepType = NO_MISSION);																											// Exposed to Python

	AutomateTypes getAutomateType() const;																																		// Exposed to Python
	bool isAutomated() const;																																									// Exposed to Python
	void setAutomateType(AutomateTypes eNewValue);																											// Exposed to Python

#ifdef USE_OLD_PATH_GENERATOR
	FAStarNode* getPathLastNode() const;
#endif
	CvPlot* getPathFirstPlot() const;																																		// Exposed to Python
	CvPlot* getPathEndTurnPlot() const;																														// Exposed to Python
	const CvPath& getPath() const;
	static CvPathGenerator* getPathGenerator();
	//TB OOS Fix
	bool generatePath(const CvPlot* pFromPlot, const CvPlot* pToPlot, int iFlags = 0, bool bReuse = false, int* piPathTurns = NULL, int iMaxPathLen = MAX_INT, int iOptimizationLimit = -1, bool bAsync = false) const;	// Exposed to Python
	void resetPath();	
	
	bool canPathDirectlyToInternal(const CvPlot* pFromPlot, const CvPlot* pToPlot, int movesRemaining) const;
	bool canPathDirectlyTo(const CvPlot* pFromPlot, const CvPlot* pToPlot) const;

	int movesRemainingAfterMovingTo(int iStartMoves, const CvPlot* pFromPlot, const CvPlot* pToPlot) const;
	int movesLeft() const;

	// Exposed to Python

	DllExport void clearUnits();
	DllExport bool addUnit(CvUnit* pUnit, bool bMinimalChange);
	void removeUnit(CvUnit* pUnit);
	void mergeIntoGroup(CvSelectionGroup* pSelectionGroup);
	bool containsUnit(const CvUnit* pUnit) const;
	CvSelectionGroup* splitGroup(int iSplitSize, CvUnit* pNewHeadUnit = NULL, CvSelectionGroup** ppOtherGroup = NULL);

	DllExport CLLNode<IDInfo>* deleteUnitNode(CLLNode<IDInfo>* pNode);
	DllExport CLLNode<IDInfo>* nextUnitNode(CLLNode<IDInfo>* pNode) const;
	DllExport int getNumUnits() const;
	int getNumUnitCargoVolumeTotal() const;
	int getLeastCargoVolume() const;
	DllExport int getUnitIndex(CvUnit* pUnit, int maxIndex = -1) const;
	DllExport CLLNode<IDInfo>* headUnitNode() const;
	DllExport CvUnit* getHeadUnit() const;
	CvUnit* getUnitAt(int index) const;
	UnitAITypes getHeadUnitAI() const;
	PlayerTypes getHeadOwner() const;
	TeamTypes getHeadTeam() const;

	// For iterating over units in a selection group
	DECLARE_IDINFO_ITERATOR(CvUnit, unit_iterator)

	unit_iterator beginUnits() const { return unit_iterator(&m_units); }
	unit_iterator endUnits() const { return unit_iterator(); }
	typedef bst::iterator_range<unit_iterator> unit_range;
	unit_range units() const { return unit_range(beginUnits(), endUnits()); }

	safe_unit_iterator beginUnitsSafe() const { return safe_unit_iterator(beginUnits(), endUnits()); }
	safe_unit_iterator endUnitsSafe() const { return safe_unit_iterator(); }
	typedef bst::iterator_range<safe_unit_iterator> safe_unit_range;
	safe_unit_range units_safe() const { return safe_unit_range(beginUnitsSafe(), endUnitsSafe()); }

	void clearMissionQueue();
	void setMissionPaneDirty();																																	// Exposed to Python
	int getLengthMissionQueue() const;																											// Exposed to Python
	MissionData* getMissionFromQueue(int iIndex) const;																							// Exposed to Python
	bool insertAtEndMissionQueue(MissionData mission, bool bStart = true);
	CLLNode<MissionData>* deleteMissionQueueNode(CLLNode<MissionData>* pNode);
	DllExport CLLNode<MissionData>* nextMissionQueueNode(CLLNode<MissionData>* pNode) const;
	CLLNode<MissionData>* prevMissionQueueNode(CLLNode<MissionData>* pNode) const;
	DllExport CLLNode<MissionData>* headMissionQueueNodeExternal() const;
	CLLNode<MissionData>* headMissionQueueNode() const;
	CLLNode<MissionData>* tailMissionQueueNode() const;
	int getMissionType(int iNode) const;																														// Exposed to Python
	int getMissionData1(int iNode) const;																														// Exposed to Python
	int getMissionData2(int iNode) const;																														// Exposed to Python

// BUG - All Units Actions - start
	bool allMatch(UnitTypes eUnit) const;
// BUG - All Units Actions - end

	// for serialization
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

	virtual void AI_init() = 0;
	virtual void AI_reset() = 0;
	virtual void AI_separate() = 0;
	virtual bool AI_update() = 0;
	virtual int AI_attackOdds(const CvPlot* pPlot, bool bPotentialEnemy, bool bForce = false, bool* bWin = NULL, int iTheshold = -1) const = 0;
	virtual CvUnit* AI_getBestGroupAttacker(const CvPlot* pPlot, bool bPotentialEnemy, int& iUnitOdds, bool bForce = false, bool bNoBlitz = false, CvUnit** pDefender = NULL, bool bAssassinate = false, bool bSurprise = false) const = 0;
	virtual CvUnit* AI_getBestGroupSacrifice(const CvPlot* pPlot, bool bPotentialEnemy, bool bForce = false, bool bNoBlitz = false, bool bSuprise = false) const = 0;

	virtual int AI_compareStacks(const CvPlot* pPlot, StackCompare::flags flags = StackCompare::None, int iRange = 0) const = 0;
	//virtual int AI_compareStacks(const CvPlot* pPlot, bool bPotentialEnemy, bool bCheckCanAttack = false, bool bCheckCanMove = false, int iRange = 0) const = 0;
	virtual int AI_sumStrength(const CvPlot* pAttackedPlot = NULL, DomainTypes eDomainType = NO_DOMAIN, StackCompare::flags flags = StackCompare::None) const = 0;
	virtual void AI_queueGroupAttack(int iX, int iY) = 0;
	virtual void AI_cancelGroupAttack() = 0;
	virtual bool AI_isGroupAttack() const = 0;

	virtual bool AI_isControlled() const = 0;
	virtual bool AI_isDeclareWar(const CvPlot* pPlot = NULL) const = 0;
	virtual CvPlot* AI_getMissionAIPlot() const = 0;
	virtual bool AI_isForceSeparate() const = 0;
	virtual void AI_makeForceSeparate() = 0;
	virtual MissionAITypes AI_getMissionAIType() const = 0;
	virtual void AI_setMissionAI(MissionAITypes eNewMissionAI, const CvPlot* pNewPlot, const CvUnit* pNewUnit) = 0;
	virtual void AI_noteSizeChange(int iChange, int iVolume) = 0;
	virtual CvUnit* AI_getMissionAIUnit() const = 0;
	virtual CvUnit* AI_ejectBestDefender(CvPlot* pTargetPlot, bool allowAllDefenders = false) = 0;
	virtual bool AI_hasBeneficialPropertyEffectForCity(CvCity* pCity) const = 0;
	virtual CvUnit* AI_ejectBestPropertyManipulator(CvCity* pTargetCity) = 0;
	virtual void AI_separateNonAI(UnitAITypes eUnitAI) = 0;
	virtual void AI_separateAI(UnitAITypes eUnitAI) = 0;
	virtual bool AI_isCityGarrison(const CvCity* pCity) const = 0;
	virtual void AI_setAsGarrison(const CvCity* pCity) = 0;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      06/02/09                                jdog5000      */
/*                                                                                              */
/* General AI                                                                                   */
/************************************************************************************************/
	virtual void AI_separateImpassable() = 0;
	virtual void AI_separateEmptyTransports() = 0;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	virtual bool AI_isFull() const = 0;
	virtual int AI_getGenericValueTimes100(UnitValueFlags eFlags) const = 0;
/************************************************************************************************/
/* DCM                                     04/19/09                                Johny Smith  */
/************************************************************************************************/
	// Dale - SA: Stack Attack START
	bool groupStackAttack(int iX, int iY, int iFlags, bool& bFailedAlreadyFighting, bool bStealth = false);
	// Dale - SA: Stack Attack END
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/
	int defensiveModifierAtPlot(const CvPlot* pPlot) const;
	bool meetsUnitSelectionCriteria(const CvUnitSelectionCriteria* criteria) const;
	int getStrength() const;
	bool hasCommander() const;
	bool hasUnitOfAI(UnitAITypes eUnitAI) const;
	int	getWorstDamagePercent(UnitCombatTypes eIgnoreUnitCombat = NO_UNITCOMBAT) const;
	void validateLocations(bool bFixup = false) const;
	bool findNewLeader(UnitAITypes eAIType);
	bool doMergeCheck() /* not const - does merge */;
	int getCargoSpace() const;
	int getCargoSpaceAvailable(SpecialUnitTypes eSpecialCargo, DomainTypes eDomainCargo) const;
	int countSeeInvisibleActive(UnitAITypes eUnitAI, InvisibleTypes eInvisibleType) const;
	void releaseUnitAIs(UnitAITypes eUnitAI);

protected:
	// WARNING: adding to this class will cause the civ4 exe to crash

	int m_iID;
	int m_iMissionTimer;

	PlayerTypes m_eOwner;
	ActivityTypes m_eActivityType;
	AutomateTypes m_eAutomateType;

	CLinkList<IDInfo> m_units;

	CLinkList<MissionData> m_missionQueue;
	std::vector<CvUnit *> m_aDifferentUnitCache;
	bool m_bIsBusyCache;

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/19/09                                jdog5000      */
/*                                                                                              */
/* General AI                                                                                   */
/************************************************************************************************/
	bool m_bIsStrandedCache;
	bool m_bIsStrandedCacheValid;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
// BUG - Safe Move - start
	bool m_bLastPathPlotChecked;
	bool m_bLastPlotVisible;
	bool m_bLastPlotRevealed;
	bool m_bForceUpdate;
	bool m_bIsMidMove;

	bool m_bIsChoosingNewLeader;

	void checkLastPathPlot(const CvPlot* pPlot);
	void clearLastPathPlot();
	bool isLastPathPlotChecked() const;
	bool isLastPathPlotVisible() const;
	bool isLastPathPlotRevealed() const;
// BUG - Safe Move - end

	bool activateHeadMission();
	void deactivateHeadMission();
	
	bool sentryAlert() const;


// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
	bool sentryAlertSameDomainType() const;
#endif
// BUG - Sentry Actions - end

//	KOSHLING Mod - add path validity results cache
private:
	struct CachedPathGenerator
	{
		CachedPathGenerator(CvMap* map);

		void clear();

		bool HaveCachedPathEdgeCosts(const CvPlot* pFromPlot, const CvPlot* pToPlot, bool bIsEndTurnElement, int& iResult, int& iBestMoveCost, int& iWorstMoveCost, int& iToPlotNodeCost);
		void CachePathEdgeCosts(const CvPlot* pFromPlot, const CvPlot* pToPlot, bool bIsEndTurnElement, int iCost, int iBestMoveCost, int iWorstMoveCost, int iToPlotNodeCost);

		CvPathGenerator* get() { return &m_pathGenerator; }
		const CvPathGenerator* get() const { return &m_pathGenerator; }

	private:
		typedef google::dense_hash_map<int, CachedEdgeCosts> CacheMapType;
		CacheMapType m_pCachedNonEndTurnEdgeCosts;
		CacheMapType m_pCachedEndTurnEdgeCosts;
		CvPathGenerator m_pathGenerator;
	};

	// These have to be static due to some assumptions the game engine seems to make about
	// this class which presumably relates to the comment earlier that adding to this class causes
	// a crash in the main engine.  This is a bit untidy, but essentially fine due to the
	// single threaded nature of the application and the fact that cache validity is only
	// required across a single path generation call, which cannot interleave
	static const CvSelectionGroup* m_pCachedMovementGroup;
	static bst::scoped_ptr<CachedPathGenerator> m_cachedPathGenerator;
	static CachedPathGenerator& getCachedPathGenerator();

public:
	static void setGroupToCacheFor(CvSelectionGroup* group);

	bool HaveCachedPathEdgeCosts(const CvPlot* pFromPlot, const CvPlot* pToPlot, bool bIsEndTurnElement, int& iResult, int& iBestMoveCost, int& iWorstMoveCost, int &iToPlotNodeCost) const;
	void CachePathEdgeCosts(const CvPlot* pFromPlot, const CvPlot* pToPlot, bool bIsEndTurnElement, int iCost, int iBestMoveCost, int iWorstMoveCost, int iToPlotNodeCost) const;

public:
	//
	// Algorithm/range helpers
	// Pass these to the filtered/transformed range adapters like:
	// foreach_(CvUnit* headUnit, groups() | transformed(CvSelectionGroup::fn::getHeadUnit())
	//									   | filtered(_1 != nullptr)) {}
	// or algorithms like:
	// fn::find_if(groups(), CvSelectionGroup::fn::getHeadUnitAI() != UNITAI_SETTLER)
	//
	struct fn {
		DECLARE_MAP_FUNCTOR(CvSelectionGroup, bool, readyToAuto);
		DECLARE_MAP_FUNCTOR(CvSelectionGroup, bool, isAutomated);
		DECLARE_MAP_FUNCTOR(CvSelectionGroup, const CvUnit*, getHeadUnit);
		DECLARE_MAP_FUNCTOR(CvSelectionGroup, UnitAITypes, getHeadUnitAI);
		DECLARE_MAP_FUNCTOR(CvSelectionGroup, PlayerTypes, getHeadOwner);
		DECLARE_MAP_FUNCTOR(CvSelectionGroup, TeamTypes, getHeadTeam);
		DECLARE_MAP_FUNCTOR(CvSelectionGroup, AutomateTypes, getAutomateType);
	};
};

#endif
