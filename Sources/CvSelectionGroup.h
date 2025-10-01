#pragma once

#ifndef CIV4_GROUP_H
#define CIV4_GROUP_H

#include "LinkedList.h"
#include "copy_iterator.h"
#include "CvPathGenerator.h"
#include "idinfo_iterator_base.h"

class CvArea;
class CvCity;
class CvPlot;
class CvPath;
class CvUnit;
class CvUnitSelectionCriteria;
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
	: private bst::noncopyable
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

	bool pushMissionInternal(MissionTypes eMission, int iData1 = -1, int iData2 = -1, int iFlags = 0, bool bAppend = false, bool bManual = false, MissionAITypes eMissionAI = NO_MISSIONAI, const CvPlot* pMissionAIPlot = NULL, const CvUnit* pMissionAIUnit = NULL);
	void pushMission(MissionTypes eMission, int iData1 = -1, int iData2 = -1, int iFlags = 0, bool bAppend = false, bool bManual = false, MissionAITypes eMissionAI = NO_MISSIONAI, const CvPlot* pMissionAIPlot = NULL, const CvUnit* pMissionAIUnit = NULL);
	void popMission();
	void autoMission();
	void updateMission();

	DllExport CvPlot* lastMissionPlot() const;

	bool canStartMission(int iMission, int iData1, int iData2, CvPlot* pPlot = NULL, bool bTestVisible = false, bool bUseCache = false) const;
	bool startMission();
	bool continueMission(int iSteps = 0);

	DllExport bool canDoInterfaceMode(InterfaceModeTypes eInterfaceMode);
	DllExport bool canDoInterfaceModeAt(InterfaceModeTypes eInterfaceMode, CvPlot* pPlot);

	void doCommand(CommandTypes eCommand, int iData1, int iData2);
	bool canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible = false, bool bUseCache = false, bool bAll = false) const;
	bool canEverDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible, bool bUseCache) const;
	void setupActionCache();

	bool isHuman() const;

	DllExport bool isBusy() const;

	bool isCombat(const CvUnit* ignoreMe = NULL) const;

	bool isCargoBusy() const;
	int baseMoves() const;
	bool isWaiting() const;
	bool isFull() const;
	bool hasCargo() const;
	int getCargo(bool bVolume = false) const;

	int getArmyID() const { return m_iArmyID; }
	void setArmyID(int iArmyID) { m_iArmyID = iArmyID; }

	bool canAllSelectedMove() const;
	DllExport bool canAllMove() const;
	DllExport bool canMoveInto(CvPlot* pPlot, bool bAttack = false);
	DllExport bool canMoveOrAttackInto(CvPlot* pPlot, bool bDeclareWar = false);
	bool canEnterPlot(const CvPlot* pPlot, bool bAttack = false) const;
	bool canEnterOrAttackPlot(const CvPlot* pPlot, bool bDeclareWar = false) const;

	bool canAnyMove(bool bValidate = false) /* not const - Can set ACTIVITY_SLEEP if bValidate is true */;
	bool hasMoved() const;
	bool canEnterTerritory(TeamTypes eTeam, bool bIgnoreRightOfPassage = false) const;
	bool canEnterArea(TeamTypes eTeam, const CvArea* pArea, bool bIgnoreRightOfPassage = false) const;

	bool canMoveThrough(const CvPlot* pPlot, bool bDeclareWar = false) const;
	bool canFight() const;
	bool canAttackNow() const;
	bool canDefend() const;
	bool canBombard(const CvPlot* pPlot, bool bCheckCanReduceOnly = false) const;
	bool hasBombardCapability() const;
	bool hasCollateralDamage() const;
	bool canPillage(const CvPlot* pPlot) const;
	bool canBombardAtRanged(const CvPlot* pPlot, int iX, int iY) const;
	int getMinimumRBombardDamageLimit() const;
	int getMinimumRBombardRange() const;
	int getRBombardDamageMaxUnits() const;

	int getBombardTurns(const CvCity* pCity ) const;
	// Not const - Can generate a move-to mission if bGo is true
	bool isHasPathToAreaPlayerCity(const PlayerTypes ePlayer, const int iFlags=0, const bool bGo=false);
	bool isHasPathToAreaEnemyCity(const int iFlags=0, const bool bGo=false);
	// ! Not const
	bool isStranded();
	void invalidateIsStrandedCache();
	bool calculateIsStranded();
	bool canMoveAllTerrain() const;

	void unloadAll();
	bool alwaysInvisible() const;
	bool isInvisible(TeamTypes eTeam) const;
	int countNumUnitAIType(UnitAITypes eUnitAI) const;
	bool hasWorker() const;
	bool IsSelected() const;
	DllExport void NotifyEntity(MissionTypes eMission);
	void airCircle(bool bStart);
	void setBlockading(bool bStart);

	int getX() const;
	int getY() const;
	bool at(int iX, int iY) const;
	bool atPlot(const CvPlot* pPlot) const;
	CvPlot* plot() const;
	DllExport CvPlot* plotExternal() const;
	int getArea() const;
	CvArea* area() const;
	DomainTypes getDomainType() const;

	RouteTypes getBestBuildRoute(const CvPlot* pPlot, BuildTypes* peBestBuild = NULL) const;

	bool canIgnoreZoneofControl() const;

	bool groupDeclareWar(const CvPlot* pPlot, bool bForce = false);
	bool groupAttack(int iX, int iY, int iFlags, bool& bFailedAlreadyFighting);
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

	bool isAmphibPlot(const CvPlot* pPlot) const;
	bool groupAmphibMove(CvPlot* pPlot, int iFlags);

	DllExport bool readyToSelect(bool bAny = false) /* not const - calls readyToMove */;
	bool readyToMove(bool bAny = false, bool bValidate = false) /* not const - calls canAnyMove */;
	bool readyToAuto() const;

	int getID() const;
	void setID(int iID);

	int getMissionTimer() const;
	void setMissionTimer(int iNewValue);
	void changeMissionTimer(int iChange);
	void updateMissionTimer(int iSteps = 0);

	bool isForceUpdate() const;
	void setForceUpdate(bool bNewValue);

	bool isMidMove() const { return m_bIsMidMove; }

	bool isChoosingNewLeader() const { return m_bIsChoosingNewLeader; }

	inline PlayerTypes getOwner() const { return m_eOwner; }

	TeamTypes getTeam() const;

	ActivityTypes getActivityType() const;
	void setActivityType(ActivityTypes eNewValue, MissionTypes eSleepType = NO_MISSION);

	AutomateTypes getAutomateType() const;
	bool isAutomated() const;
	void setAutomateType(AutomateTypes eNewValue);

#ifdef USE_OLD_PATH_GENERATOR
	FAStarNode* getPathLastNode() const;
#endif
	CvPlot* getPathFirstPlot() const;
	CvPlot* getPathEndTurnPlot() const;
	const CvPath& getPath() const;
	static CvPathGenerator* getPathGenerator();
	bool generatePath(const CvPlot* pFromPlot, const CvPlot* pToPlot, int iFlags = 0, bool bReuse = false, int* piPathTurns = NULL, int iMaxPathLen = MAX_INT, int iOptimizationLimit = -1) const;
	void resetPath();

	bool canPathDirectlyToInternal(const CvPlot* pFromPlot, const CvPlot* pToPlot, int movesRemaining) const;
	bool canPathDirectlyTo(const CvPlot* pFromPlot, const CvPlot* pToPlot) const;

	int movesRemainingAfterMovingTo(int iStartMoves, const CvPlot* pFromPlot, const CvPlot* pToPlot) const;
	int movesLeft() const;



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
	void setMissionPaneDirty();
	int getLengthMissionQueue() const;
	MissionData* getMissionFromQueue(int iIndex) const;
	bool insertAtEndMissionQueue(MissionData mission, bool bStart = true);
	CLLNode<MissionData>* deleteMissionQueueNode(CLLNode<MissionData>* pNode);
	DllExport CLLNode<MissionData>* nextMissionQueueNode(CLLNode<MissionData>* pNode) const;
	CLLNode<MissionData>* prevMissionQueueNode(CLLNode<MissionData>* pNode) const;
	DllExport CLLNode<MissionData>* headMissionQueueNodeExternal() const;
	CLLNode<MissionData>* headMissionQueueNode() const;
	CLLNode<MissionData>* tailMissionQueueNode() const;
	int getMissionType(int iNode) const;
	int getMissionData1(int iNode) const;
	int getMissionData2(int iNode) const;
	bool isMoveMission(CLLNode<MissionData>* node) const;

// BUG - All Units Actions - start
	bool allMatch(UnitTypes eUnit) const;
// BUG - All Units Actions - end

	// for serialization
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

	virtual void AI_reset() = 0;
	virtual void AI_separate() = 0;
	virtual bool AI_update() = 0;
	virtual int AI_attackOdds(const CvPlot* pPlot, bool bPotentialEnemy, bool bForce = false, bool* bWin = NULL, int iTheshold = -1) const = 0;

	virtual CvUnit* AI_getBestGroupAttacker
	(
		const CvPlot* pPlot,
		bool bPotentialEnemy,
		int& iUnitOdds,
		bool bForce = false,
		CvUnit** pDefender = NULL,
		bool bAssassinate = false,
		bool bSurprise = false,
		bool bIgnoreMadeAttack = false,
		const std::set<int>& ignoreUnitID = std::set<int>()
	)
	const = 0;

	virtual CvUnit* AI_getBestGroupSacrifice(const CvPlot* pPlot, bool bForce, bool bNoBlitz) const = 0;

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
	virtual CvUnit* AI_ejectBestDefender(const CvPlot* pTargetPlot, bool allowAllDefenders = false) = 0;
	virtual bool AI_hasBeneficialPropertyEffectForCity(const CvCity* pCity, PropertyTypes pProperty) const = 0;
	virtual CvUnit* AI_ejectBestPropertyManipulator(const CvCity* pTargetCity) = 0;
	virtual void AI_separateNonAI(UnitAITypes eUnitAI) = 0;
	virtual void AI_separateAI(UnitAITypes eUnitAI) = 0;
	virtual bool AI_isCityGarrison(const CvCity* pCity) const = 0;
	virtual void AI_setAsGarrison(const CvCity* pCity) = 0;
	virtual void AI_separateImpassable() = 0;
	virtual void AI_separateEmptyTransports() = 0;
	virtual bool AI_isFull() const = 0;
	virtual int AI_getGenericValueTimes100(UnitValueFlags eFlags) const = 0;

	int defensiveModifierAtPlot(const CvPlot* pPlot) const;
	bool meetsUnitSelectionCriteria(const CvUnitSelectionCriteria* criteria) const;
	int getStrength() const;
	bool hasCommander() const;
	bool hasCommodore() const;
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
	int m_iArmyID;

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
		DECLARE_MAP_FUNCTOR(CvSelectionGroup, void, autoMission);
		DECLARE_MAP_FUNCTOR(CvSelectionGroup, void, resetHealing);
		DECLARE_MAP_FUNCTOR(CvSelectionGroup, bool, doDelayedDeath);
		DECLARE_MAP_FUNCTOR(CvSelectionGroup, void, updateTimers);

		DECLARE_MAP_FUNCTOR_2(CvSelectionGroup, bool, readyToMove, bool, bool);

		DECLARE_MAP_FUNCTOR_CONST(CvSelectionGroup, bool, isBusy);
		DECLARE_MAP_FUNCTOR_CONST(CvSelectionGroup, bool, readyToAuto);
		DECLARE_MAP_FUNCTOR_CONST(CvSelectionGroup, bool, isAutomated);
		DECLARE_MAP_FUNCTOR_CONST(CvSelectionGroup, const CvUnit*, getHeadUnit);
		DECLARE_MAP_FUNCTOR_CONST(CvSelectionGroup, UnitAITypes, getHeadUnitAI);
		DECLARE_MAP_FUNCTOR_CONST(CvSelectionGroup, PlayerTypes, getHeadOwner);
		DECLARE_MAP_FUNCTOR_CONST(CvSelectionGroup, TeamTypes, getHeadTeam);
		DECLARE_MAP_FUNCTOR_CONST(CvSelectionGroup, AutomateTypes, getAutomateType);
	};
};

#endif
