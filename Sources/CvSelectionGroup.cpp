// selectionGroup.cpp


#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvArea.h"
#include "CvArtFileMgr.h"
#include "CvCity.h"
#include "CvEventReporter.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvImprovementInfo.h"
#include "CvInfos.h"
#include "CvMap.h"
#include "CvPathGenerator.h"
#include "CvPlot.h"
#include "CvReachablePlotSet.h"
#include "CvPlayerAI.h"
#include "CvPopupInfo.h"
#include "CvSelectionGroup.h"
#include "CvUnitCombatInfo.h"
#include "CvSelectionGroupAI.h"
#include "CvTeamAI.h"
#include "CvUnit.h"
#include "CvUnitSelectionCriteria.h"
#include "CvViewport.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLUtilityIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"
#ifdef USE_OLD_PATH_GENERATOR
#include "FAStarNode.h"
#endif

const CvSelectionGroup* CvSelectionGroup::m_pCachedMovementGroup = nullptr;
bst::scoped_ptr<CvSelectionGroup::CachedPathGenerator> CvSelectionGroup::m_cachedPathGenerator;

//std::map<int,CachedEdgeCosts>* CvSelectionGroup::m_pCachedNonEndTurnEdgeCosts = nullptr;
//std::map<int,CachedEdgeCosts>* CvSelectionGroup::m_pCachedEndTurnEdgeCosts = nullptr;
//CvPathGenerator*	CvSelectionGroup::m_generator = nullptr;

// Public Functions...

CvSelectionGroup::CvSelectionGroup()
{
	reset(0, NO_PLAYER, true);
}


CvSelectionGroup::~CvSelectionGroup()
{
	uninit();
}


void CvSelectionGroup::init(int iID, PlayerTypes eOwner)
{
	//--------------------------------
	// Init saved data
	reset(iID, eOwner);

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data
	AI_reset();
}


void CvSelectionGroup::uninit()
{
	m_units.clear();
	m_missionQueue.clear();
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvSelectionGroup::reset(int iID, PlayerTypes eOwner, bool bConstructorCall)
{
	//--------------------------------
	// Uninit class
	uninit();

	m_iID = iID;
	m_iMissionTimer = 0;

	m_bForceUpdate = false;
	m_bIsMidMove = false;

	m_bIsChoosingNewLeader = false;

	m_eOwner = eOwner;

	m_eActivityType = ACTIVITY_AWAKE;
	m_eAutomateType = NO_AUTOMATE;
	m_bIsBusyCache = false;

	m_bIsStrandedCache = false;
	m_bIsStrandedCacheValid = false;

	m_bLastPathPlotChecked = false;
	m_bLastPlotVisible = false;
	m_bLastPlotRevealed = false;

	m_iArmyID = -1;

	if (!bConstructorCall)
	{
		AI_reset();
	}
}


void CvSelectionGroup::kill()
{
	FAssert(getOwner() != NO_PLAYER);
	FAssertMsg(getID() != FFreeList::INVALID_INDEX, "getID() is not expected to be equal with FFreeList::INVALID_INDEX");
	FAssertMsg(getNumUnits() == 0, "The number of units is expected to be 0");

	GET_PLAYER(getOwner()).removeGroupCycle(getID());

	GET_PLAYER(getOwner()).deleteSelectionGroup(getID());
}

bool CvSelectionGroup::sentryAlert() const
{
	PROFILE_EXTRA_FUNC();
	CvUnit* unit = NULL;
	int iMaxRange = 0;
	foreach_(CvUnit* unitX, units())
	{
		const int iRange = unitX->visibilityRange();
		if (iRange > iMaxRange)
		{
			iMaxRange = iRange;
			unit = unitX;
		}
	}

	if (unit)
	{
		const CvPlot* myPlot = unit->plot();

		foreach_(const CvPlot* plotX, myPlot->rect(iMaxRange, iMaxRange))
		{
			if (myPlot->canSeePlot(plotX, unit->getTeam()) && plotX->isVisibleEnemyUnit(unit))
			{
				return true;
			}
		}
	}
	return false;
}


#ifdef _MOD_SENTRY
/*
 * Similar to sentryAlert() except only checks water/land plots based on the domain type of the head unit.
 */
bool CvSelectionGroup::sentryAlertSameDomainType() const
{
	PROFILE_EXTRA_FUNC();
	CvUnit* unit = NULL;
	int iMaxRange = 0;

	foreach_(CvUnit* unitX, units())
	{
		const int iRange = unitX->visibilityRange();
		if (iRange > iMaxRange)
		{
			iMaxRange = iRange;
			unit = unitX;
		}
	}

	if (unit)
	{
		const CvPlot* myPlot = unit->plot();

		foreach_(const CvPlot* plotX, myPlot->rect(iMaxRange, iMaxRange))
		{
			if (myPlot->canSeePlot(plotX, unit->getTeam()) && plotX->isVisibleEnemyUnit(unit))
			{
				if (plotX->isWater())
				{
					if (getDomainType() == DOMAIN_SEA)
					{
						return true;
					}
				}
				else if (getDomainType() == DOMAIN_LAND)
				{
					return true;
				}
			}
		}
	}
	return false;
}
#endif


void CvSelectionGroup::doTurn()
{
	PROFILE("CvSelectionGroup::doTurn()");

	FAssert(getOwner() != NO_PLAYER);

	if (getNumUnits() > 0)
	{
		invalidateIsStrandedCache();

		bool bHurt = false;

		// do unit's turns (checking for damage)
		foreach_(CvUnit* pLoopUnit, units())
		{
			//	Units can be forced out of plots due to war/culture changes.  If a group
			//	has a mix of visible/invisible or nationality-hidden units this can act
			//	differently on different units within the group, effectively splitting it
			if ( pLoopUnit->plot() != plot() )
			{
				pLoopUnit->joinGroup(NULL);
			}

			pLoopUnit->doTurn();

			if (pLoopUnit->isHurt())
			{
				bHurt = true;
			}
		}

		ActivityTypes eActivityType = getActivityType();

		//	If the unit has a mission queued but not yet started set its activity
		//	to ACTIVITY_MISSION as that best reflects what it will be doign once its automation is run
		if ( eActivityType == ACTIVITY_HOLD && headMissionQueueNode() != NULL )
		{
			setActivityType(ACTIVITY_MISSION);
		}
		// wake unit if skipped last turn
		//		or healing and automated or no longer hurt (automated healing is one turn at a time)
		//		or on sentry and there is danger
		else if ((eActivityType == ACTIVITY_HOLD) ||
				((eActivityType == ACTIVITY_HEAL) && (AI_isControlled() || !bHurt)) ||
				((eActivityType == ACTIVITY_SENTRY) && (sentryAlert())))
		{
			setActivityType(ACTIVITY_AWAKE);
		}

		if (isHuman())
		{
#ifdef _MOD_SENTRY
			if (((eActivityType == ACTIVITY_SENTRY_NAVAL_UNITS) && (sentryAlertSameDomainType())) ||
				((eActivityType == ACTIVITY_SENTRY_LAND_UNITS) && (sentryAlertSameDomainType())) ||
				((eActivityType == ACTIVITY_SENTRY_WHILE_HEAL) && (sentryAlertSameDomainType() || AI_isControlled() || !bHurt)))
			{
				setActivityType(ACTIVITY_AWAKE);
			}
#endif

			if (isAutomated() && getAutomateType() == AUTOMATE_EXPLORE && getBugOptionBOOL("Actions__SentryHealing", true, "BUG_SENTRY_HEALING") && sentryAlert())
			{
				if (!(getBugOptionBOOL("Actions__SentryHealingOnlyNeutral", true, "BUG_SENTRY_HEALING_ONLY_NEUTRAL") && plot()->isOwned()))
				{
					setActivityType(ACTIVITY_AWAKE);
				}
			}
// AIAndy: This is pointless when there is the separate sentry while heal button
#ifndef _MOD_SENTRY
			if (eActivityType == ACTIVITY_HEAL && getBugOptionBOOL("Actions__SentryHealing", true, "BUG_SENTRY_HEALING") && sentryAlert())
			{
				if (!(getBugOptionBOOL("Actions__SentryHealingOnlyNeutral", true, "BUG_SENTRY_HEALING_ONLY_NEUTRAL") && plot()->isOwned()))
				{
					setActivityType(ACTIVITY_AWAKE);
				}
			}
#endif
		}

		// with improvements to launching air patrols, now can wake every turn
		if (eActivityType == ACTIVITY_INTERCEPT && !isHuman())
		{
			setActivityType(ACTIVITY_AWAKE);
		}

		if (AI_isControlled())
		{
			if ((getActivityType() != ACTIVITY_MISSION) || (!canDefend() && (GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 2))))
			{
				setForceUpdate(true);
			}
		}
		else if (getActivityType() == ACTIVITY_MISSION)
		{
			foreach_(const CvUnit* pLoopUnit, units() | filtered(!CvUnit::fn::isSpy()))
			{
				if (GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 2))
				{
					clearMissionQueue();
				}
				break;
			}
		}

		if (isHuman() && GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
		{
			const int iMinTimerDoubleMoves = GC.getMIN_TIMER_UNIT_DOUBLE_MOVES();

			int iBestWaitTurns = 0;
			foreach_(const CvUnit* pLoopUnit, units())
			{
				const int iWaitTurns = (iMinTimerDoubleMoves - (GC.getGame().getTurnSlice() - pLoopUnit->getLastMoveTurn()));
				if (iWaitTurns > iBestWaitTurns)
				{
					iBestWaitTurns = iWaitTurns;
				}
			}

			setMissionTimer(std::max(iBestWaitTurns, getMissionTimer()));

			if (iBestWaitTurns > 0)
			{
				// Cycle selection if the current group is selected
				CvUnit* pSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
				if (pSelectedUnit && pSelectedUnit->getGroup() == this)
				{
					gDLL->getInterfaceIFace()->selectGroup(pSelectedUnit, false, false, false);
				}
			}
		}
	}
	doDelayedDeath();
}

void CvSelectionGroup::resetHealing()
{
	PROFILE("CvSelectionGroup::resetHealing()");

	algo::for_each(units(), CvUnit::fn::setHealSupportUsed(0));
}

bool CvSelectionGroup::showMoves() const
{
	PROFILE_EXTRA_FUNC();
	if (GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS) || GC.getGame().isSimultaneousTeamTurns())
	{
		return false;
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const CvPlayer& pPlayer = GET_PLAYER((PlayerTypes)iI);
		if (pPlayer.isAlive() && pPlayer.isHumanPlayer())
		{
			const CvUnit* pHeadUnit = getHeadUnit();
			if (pHeadUnit && (pHeadUnit->isEnemy(pPlayer.getTeam()) ? pPlayer.isOption(PLAYEROPTION_SHOW_ENEMY_MOVES) : pPlayer.isOption(PLAYEROPTION_SHOW_FRIENDLY_MOVES)))
			{
				return true;
			}
		}
	}
	return false;
}


void CvSelectionGroup::updateTimers()
{
	PROFILE_EXTRA_FUNC();
	FAssert(getOwner() != NO_PLAYER);

	if (getNumUnits() > 0)
	{
		bool bCombat = false;
		bool bCombatFinished = true;

		foreach_(CvUnit* unitX, units())
		{
			if (unitX->getCombatTimer() > 0)
			{
				if (unitX->isAirCombat())
					unitX->updateAirCombat();
				else unitX->updateCombat();

				bCombat = true;
				if (bCombatFinished && unitX->getCombatTimer() > 0)
				{
					bCombatFinished = false;
				}
			}
		}

		if (!bCombat)
		{
			updateMission();
		}
		else if (bCombatFinished)
		{
			if (gDLL->getInterfaceIFace()->isCombatFocus())
			{
				gDLL->getInterfaceIFace()->releaseLockedCamera();
				gDLL->getInterfaceIFace()->setCombatFocus(false);
			}
			if (IsSelected())
			{
				if (canAnyMove())
				{
					foreach_(CvUnit* unitX, units())
					{
						if (unitX->IsSelected() && !unitX->canMove())
						{
							gDLL->getInterfaceIFace()->removeFromSelectionList(unitX);
						}
					}
				}
				else GC.getGame().updateSelectionListInternal();
			}
		}
	}
	doDelayedDeath();
}

namespace {
	bool canDelete(const CvPlayer& player, CvUnit* unit)
	{
		return !player.isTempUnit(unit) && unit->isDelayedDeath();
	}
}

// Returns true if group was killed...
bool CvSelectionGroup::doDelayedDeath()
{
	PROFILE_EXTRA_FUNC();
	FAssert(getOwner() != NO_PLAYER);

	if (isBusy())
	{
		return false;
	}

	// Get list of the units to delete *first* then delete them. Iterators would be invalidated
	// if we tried to delete while iterating over the group itself
	//std::vector<CvUnit*> toDelete = get_if(bind(canDelete, bst::ref(GET_PLAYER(getOwner())), _1));
	foreach_ (CvUnit* unit, units_safe() | filtered(bind(canDelete, bst::ref(GET_PLAYER(getOwner())), _1)))
		//std::vector<CvUnit*>::iterator itr = toDelete.begin(); itr != toDelete.end(); ++itr)
	{
		unit->doDelayedDeath();
	}
	if (getNumUnits() == 0)
	{
		kill();
		return true;
	}
	return false;
}


void CvSelectionGroup::playActionSound()
{
	// Pitboss should not be playing sounds!
#ifndef PITBOSS
	int iScriptId = -1;

	const CvUnit* pHeadUnit = getHeadUnit();
	if ( pHeadUnit )
	{
		iScriptId = pHeadUnit->getArtInfo(0, GET_PLAYER(getOwner()).getCurrentEra())->getActionSoundScriptId();
	}

	if ( (iScriptId == -1) && pHeadUnit )
	{
		CvCivilizationInfo* pCivInfo = &GC.getCivilizationInfo( pHeadUnit->getCivilizationType() );
		if ( pCivInfo )
		{
			iScriptId = pCivInfo->getActionSoundScriptId();
		}
	}

	if ( (iScriptId != -1) && pHeadUnit )
	{
		const CvPlot *pPlot = GC.getMap().plot(pHeadUnit->getX(), pHeadUnit->getY());
		if ( pPlot )
		{
			gDLL->Do3DSound( iScriptId, pPlot->getPoint() );
		}
	}
#endif // n PITBOSS
}


void CvSelectionGroup::pushMission(MissionTypes eMission, int iData1, int iData2, int iFlags, bool bAppend, bool bManual, MissionAITypes eMissionAI, const CvPlot* pMissionAIPlot, const CvUnit* pMissionAIUnit)
{
	if ( eMission == MISSION_SKIP && eMissionAI == NO_MISSIONAI)
	{
		//	No longer targeting any mission - make sure we don't keep record of the fact that we were previously
		((CvSelectionGroupAI*)this)->AI_setMissionAI(NO_MISSIONAI,NULL,NULL);
	}
	pushMissionInternal(eMission, iData1, iData2, iFlags, bAppend, bManual, eMissionAI, pMissionAIPlot, pMissionAIUnit);
}

bool CvSelectionGroup::pushMissionInternal(MissionTypes eMission, int iData1, int iData2, int iFlags, bool bAppend, bool bManual, MissionAITypes eMissionAI, const CvPlot* pMissionAIPlot, const CvUnit* pMissionAIUnit)
{
	PROFILE_FUNC();

	FAssert(getOwner() != NO_PLAYER);

	if (!bAppend)
	{
		if (isBusy())
		{
			return true;
		}
		clearMissionQueue();
	}

	if (getHeadUnit() == NULL)
		return false;

	if (bManual)
	{
		setAutomateType(NO_AUTOMATE);
	}

	MissionData mission;
	mission.eMissionType = eMission;
	mission.iData1 = iData1;
	mission.iData2 = iData2;
	mission.iFlags = iFlags;
	mission.iPushTurn = GC.getGame().getGameTurn();

	AI_setMissionAI(eMissionAI, pMissionAIPlot, pMissionAIUnit);

	const bool bResult = insertAtEndMissionQueue(mission, !bAppend);

	if (bManual)
	{
		if (getOwner() == GC.getGame().getActivePlayer())
		{
			if (isBusy() && GC.getMissionInfo(eMission).isSound())
			{
				playActionSound();
			}
			gDLL->getInterfaceIFace()->setHasMovedUnit(true);
		}
		CvEventReporter::getInstance().selectionGroupPushMission(this, eMission);

		doDelayedDeath();
	}
	return bResult;
}


void CvSelectionGroup::popMission()
{
	FAssert(getOwner() != NO_PLAYER);

	CLLNode<MissionData>* pTailNode = tailMissionQueueNode();

	if (pTailNode != NULL)
	{
		deleteMissionQueueNode(pTailNode);
	}
}


void CvSelectionGroup::autoMission()
{
	PROFILE_FUNC();

	FAssert(getOwner() != NO_PLAYER);

	if (getNumUnits() > 0 && headMissionQueueNode() && !isBusy())
	{
		if (isHuman()
		&& algo::any_of(units(), !CvUnit::fn::alwaysInvisible())
		&& GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 1))
		{
			clearMissionQueue();
		}
		else if (getActivityType() == ACTIVITY_MISSION)
		{
			continueMission();
		}
		else startMission();
	}
	doDelayedDeath();
}


void CvSelectionGroup::updateMission()
{
	FAssert(getOwner() != NO_PLAYER);

	if (getMissionTimer() > 0)
	{
		changeMissionTimer(-1);

		if (getMissionTimer() == 0 && getActivityType() == ACTIVITY_MISSION)
		{
			continueMission();
		}
	}
}

CvPlot* CvSelectionGroup::lastMissionPlot() const
{
	PROFILE_EXTRA_FUNC();
	CLLNode<MissionData>* pMissionNode = tailMissionQueueNode();

	while (pMissionNode != NULL)
	{
		switch (pMissionNode->m_data.eMissionType)
		{
			case MISSION_MOVE_TO:
#ifdef _MOD_SENTRY
			case MISSION_MOVE_TO_SENTRY:
#endif
			case MISSION_ROUTE_TO:
			{
				return GC.getMap().plot(pMissionNode->m_data.iData1, pMissionNode->m_data.iData2);
			}
			case MISSION_MOVE_TO_UNIT:
			{
				const CvUnit* pTargetUnit = GET_PLAYER((PlayerTypes)pMissionNode->m_data.iData1).getUnit(pMissionNode->m_data.iData2);
				if (pTargetUnit != NULL)
				{
					return pTargetUnit->plot();
				}
				break;
			}
		}
		pMissionNode = prevMissionQueueNode(pMissionNode);
	}
	return plot();
}


bool CvSelectionGroup::canStartMission(int iMission, int iData1, int iData2, CvPlot* pPlot, bool bTestVisible, bool bUseCache) const
{
	PROFILE_FUNC();

	//cache isBusy
	if (bUseCache ? m_bIsBusyCache : isBusy())
	{
		return false;
	}

	if (!pPlot)
	{
		pPlot = plot();
	}
#ifdef NOMADIC_START
	const TechTypes sedentaryLifestyle = GC.getTECH_SEDENTARY_LIFESTYLE();
#endif
	foreach_(CvUnit* unitX, units())
	{
		switch (iMission)
		{
#ifdef _MOD_SENTRY
			case MISSION_MOVE_TO_SENTRY:
			{
				if (!unitX->canSentry(NULL))
				{
					return false;
				}
				// fall through to next case
			}
#endif
			case MISSION_MOVE_TO:
			{
				return !pPlot->at(iData1, iData2);
			}
			case MISSION_ROUTE_TO:
			{
				return !pPlot->at(iData1, iData2) || getBestBuildRoute(pPlot) != NO_ROUTE;
			}
			case MISSION_MOVE_TO_UNIT:
			{
				FAssertMsg(iData1 != NO_PLAYER, "iData1 should be a valid Player");
				CvUnit* pTargetUnit = GET_PLAYER((PlayerTypes)iData1).getUnit(iData2);
				return pTargetUnit && !pTargetUnit->atPlot(pPlot);
			}
			case MISSION_SKIP:
			{
				if (unitX->canHold())
				{
					return true;
				}
				break;
			}
			case MISSION_SLEEP:
			{
				if (unitX->canSleep())
				{
					return true;
				}
				break;
			}
			case MISSION_FORTIFY:
			{
				if (unitX->canFortify())
				{
					return true;
				}
				break;
			}
			case MISSION_BUILDUP:
			{
				if (unitX->canBuildUp())
				{
					return true;
				}
				break;
			}
			case MISSION_AUTO_BUILDUP:
			{
				if (unitX->canBuildUp() || unitX->canFortify() || unitX->canSleep())
				{
					return true;
				}
				break;
			}
			case MISSION_HEAL_BUILDUP:
			{
				if (unitX->canHeal(pPlot) && pPlot->getTotalTurnDamage(this) <= 0)
				{
					return true;
				}
				break;
			}
			case MISSION_AIRPATROL:
			{
				if (unitX->canAirPatrol(pPlot))
				{
					return true;
				}
				break;
			}
			case MISSION_SEAPATROL:
			{
				if (unitX->canSeaPatrol(pPlot))
				{
					return true;
				}
				break;
			}
			case MISSION_HEAL:
			{
				//ls612: Fix for Terrain damage, apparently that wasn't factored in anywhere else.
				if (unitX->canHeal(pPlot) && pPlot->getTotalTurnDamage(this) <= 0)
				{
					return true;
				}
				break;
			}
			case MISSION_SENTRY:
			{
				if (unitX->canSentry(pPlot))
				{
					return true;
				}
				break;
			}
#ifdef _MOD_SENTRY
			case MISSION_SENTRY_WHILE_HEAL:
			{
				if ((unitX->canSentry(pPlot)) && (unitX->canHeal(pPlot)))
				{
					return true;
				}
				break;
			}
			case MISSION_SENTRY_NAVAL_UNITS:
			{
				if ((getDomainType() == DOMAIN_SEA) && (unitX->canSentry(pPlot)))
				{
					return true;
				}
				break;
			}
			case MISSION_SENTRY_LAND_UNITS:
			{
				if ((getDomainType() == DOMAIN_LAND) && (unitX->canSentry(pPlot)))
				{
					return true;
				}
				break;
			}
#endif
			case MISSION_AIRLIFT:
			{
				if (unitX->canAirliftAt(pPlot, iData1, iData2))
				{
					return true;
				}
				break;
			}
			case MISSION_NUKE:
			{
				if (unitX->canNukeAt(pPlot, iData1, iData2))
				{
					return true;
				}
				break;
			}
			case MISSION_RECON:
			{
				if (unitX->canReconAt(pPlot, iData1, iData2))
				{
					return true;
				}
				break;
			}
			case MISSION_PARADROP:
			{
				if (unitX->canParadropAt(pPlot, iData1, iData2))
				{
					return true;
				}
				break;
			}
			case MISSION_AIRBOMB:
			{
				if (unitX->canAirBombAt(pPlot, iData1, iData2))
				{
					return true;
				}
				break;
			}
			case MISSION_BOMBARD:
			{
				if (unitX->canBombard(pPlot))
				{
					return true;
				}
				break;
			}
			case MISSION_RBOMBARD:
			{
				if (GC.isDCM_RANGE_BOMBARD() && unitX->canBombardAtRanged(pPlot, iData1, iData2))
				{
					return true;
				}
				break;
			}
			case MISSION_RANGE_ATTACK:
			{
				if (unitX->canRangeStrikeAt(pPlot, iData1, iData2))
				{
					return true;
				}
				break;
			}
			case MISSION_PLUNDER:
			{
				if (unitX->canPlunder(pPlot, bTestVisible))
				{
					return true;
				}
				break;
			}
			case MISSION_PILLAGE:
			{
				if (unitX->canPillage(pPlot))
				{
					return true;
				}
				break;
			}
			case MISSION_SABOTAGE:
			{
				if (unitX->canSabotage(pPlot, bTestVisible))
				{
					return true;
				}
				break;
			}
			case MISSION_DESTROY:
			{
				if (unitX->canDestroy(pPlot, bTestVisible))
				{
					return true;
				}
				break;
			}
			case MISSION_STEAL_PLANS:
			{
				if (unitX->canStealPlans(pPlot, bTestVisible))
				{
					return true;
				}
				break;
			}
			case MISSION_FOUND:
			{
#ifdef NOMADIC_START
				if (!GET_TEAM(unitX->getTeam()).isHasTech(sedentaryLifestyle))
				{
					return false;
				}
#endif
				if (unitX->canFound(pPlot, bTestVisible))
				{
					return true;
				}
				break;
			}
			case MISSION_SPREAD:
			{
				if (unitX->canSpread(pPlot, ((ReligionTypes)iData1), bTestVisible))
				{
					return true;
				}
				break;
			}
			case MISSION_SPREAD_CORPORATION:
			{
				if (unitX->canSpreadCorporation(pPlot, ((CorporationTypes)iData1), bTestVisible))
				{
					return true;
				}
				break;
			}
			case MISSION_JOIN:
			{
				if (unitX->canJoin(pPlot, ((SpecialistTypes)iData1)))
				{
					return true;
				}
				break;
			}
			case MISSION_CONSTRUCT:
			{
				if (unitX->canConstruct(pPlot, (BuildingTypes)iData1, bTestVisible))
				{
					return true;
				}
				break;
			}
			case MISSION_HERITAGE:
			{
				if (unitX->canAddHeritage(pPlot, (HeritageTypes)iData1, bTestVisible))
				{
					return true;
				}
				break;
			}
			case MISSION_DISCOVER:
			{
				if (unitX->canDiscover())
				{
					return true;
				}
				break;
			}
			case MISSION_HURRY:
			{
				if (unitX->canHurry(pPlot, bTestVisible))
				{
					return true;
				}
				break;
			}
			case MISSION_TRADE:
			{
				if (unitX->canTrade(pPlot, bTestVisible))
				{
					return true;
				}
				break;
			}
			case MISSION_GREAT_WORK:
			{
				if (unitX->canGreatWork(pPlot))
				{
					return true;
				}
				break;
			}
			case MISSION_INFILTRATE:
			{
				if (unitX->canInfiltrate(pPlot, bTestVisible))
				{
					return true;
				}
				break;
			}
			case MISSION_GOLDEN_AGE:
			{
				//this means to play the animation only
				if (iData1 != -1)
				{
					return true;
				}

				if (unitX->canGoldenAge(bTestVisible))
				{
					return true;
				}
				break;
			}
			case MISSION_BUILD:
			{
				FASSERT_BOUNDS(0, GC.getNumBuildInfos(), iData1);

				if (unitX->canBuild(pPlot, (BuildTypes)iData1, bTestVisible && !GET_PLAYER(unitX->getOwner()).isModderOption(MODDEROPTION_HIDE_UNAVAILBLE_BUILDS)))
				{
					return true;
				}
				break;
			}
			case MISSION_LEAD:
			{
				if (unitX->canLead(pPlot, iData1))
				{
					return true;
				}
				break;
			}
			case MISSION_ESPIONAGE:
			{
				if (unitX->canEspionage(pPlot, bTestVisible))
				{
					return true;
				}
				break;
			}
			case MISSION_CURE:
			{
#ifdef OUTBREAKS_AND_AFFLICTIONS
				if (unitX->canCure(pPlot, ((PromotionLineTypes)iData1)))
				{
					return true;
				}
#endif
				break;
			}
			case MISSION_GOTO:
			{
				if (unitX->getDomainType() == DOMAIN_LAND || unitX->getDomainType() == DOMAIN_SEA)
				{
					return true;
				}
				break;
			}
			case MISSION_DIE_ANIMATION:
			{
				return false;
			}
			case MISSION_BEGIN_COMBAT:
			case MISSION_END_COMBAT:
			case MISSION_AIRSTRIKE:
			{
				if (unitX->canAirStrike(pPlot))
				{
					return true;
				}
				break;
			}
			case MISSION_SURRENDER:
			case MISSION_IDLE:
			case MISSION_DIE:
			case MISSION_DAMAGE:
			case MISSION_MULTI_SELECT:
			case MISSION_MULTI_DESELECT:
			{
				break;
			}
			case MISSION_AIRBOMB1:
			{
				if (unitX->canAirBomb1At(pPlot, iData1, iData2))
				{
					return true;
				}
				break;
			}
			case MISSION_AIRBOMB2:
			{
				if (unitX->canAirBomb2At(pPlot, iData1, iData2))
				{
					return true;
				}
				break;
			}
			case MISSION_AIRBOMB3:
			{
				if (unitX->canAirBomb3At(pPlot, iData1, iData2))
				{
					return true;
				}
				break;
			}
			case MISSION_AIRBOMB4:
			{
				if (unitX->canAirBomb4At(pPlot, iData1, iData2))
				{
					return true;
				}
				break;
			}
			case MISSION_AIRBOMB5:
			{
				if (unitX->canAirBomb5At(pPlot, iData1, iData2))
				{
					return true;
				}
				break;
			}
			case MISSION_FENGAGE:
			{
				if (unitX->canFEngageAt(pPlot, iData1, iData2))
				{
					return true;
				}
				break;
			}
			case MISSION_CLAIM_TERRITORY:
			{
				if (unitX->canClaimTerritory(pPlot))
				{
					return true;
				}
				break;
			}
			case MISSION_HURRY_FOOD:
			{
				if (unitX->canHurryFood(pPlot))
				{
					return true;
				}
				break;
			}
			case MISSION_INQUISITION:
			{
				if (unitX->canPerformInquisition(pPlot))
				{
					return true;
				}
				break;
			}
			case MISSION_ESPIONAGE_SLEEP:
			{
				if (unitX->plot()->isCity() && unitX->canSleep() && unitX->canEspionage(pPlot, true) && unitX->getFortifyTurns() != GC.getMAX_FORTIFY_TURNS())
				{
					return true;
				}
				break;
			}
			case MISSION_SHADOW:
			{
				{
					const CvPlot* pShadowPlot = GC.getMap().plot(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2);

					if (pShadowPlot != NULL)
					{
						const int iValidShadowUnits = algo::count_if(pShadowPlot->units(),
							bind(&CvUnit::canShadowAt, unitX, pShadowPlot, _1));

						if (iValidShadowUnits > 0)
						{
							return true;
						}
					}
				}
				break;
			}
			case MISSION_GREAT_COMMANDER:
			{
				if (GC.getGame().isOption(GAMEOPTION_UNIT_GREAT_COMMANDERS) && unitX->getUnitInfo().isGreatGeneral() && !unitX->isCommander() && !unitX->isCommodore())
				{
					return true;
				}
				break;
			}
			case MISSION_GREAT_COMMODORE:
			{
            	if (GC.getGame().isOption(GAMEOPTION_UNIT_GREAT_COMMODORES) && unitX->getUnitInfo().isGreatGeneral() && !unitX->isCommodore() && !unitX->isCommander())
            	{
            		return true;
            	}
            	break;
            }
			case MISSION_ASSASSINATE:
			{
				if (unitX->canAmbush(pPlot, true))
				{
					return true;
				}
				break;
			}
			case MISSION_AMBUSH:
			{
				if (unitX->canAmbush(pPlot, false))
				{
					return true;
				}
				break;
			}
			default: // AIAndy: Assumed to be an outcome mission
			{
				const CvOutcomeMission* pOutcomeMission = unitX->getUnitInfo().getOutcomeMissionByMission((MissionTypes)iMission);
				if (pOutcomeMission && pOutcomeMission->isPossible(unitX, bTestVisible))
				{
					return true;
				}
				// Outcome missions on unit combats
				for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
				{
					if (unitX->isHasUnitCombat((UnitCombatTypes)iI))
					{
						const CvOutcomeMission* pOutcomeMission = GC.getUnitCombatInfo((UnitCombatTypes)iI).getOutcomeMissionByMission((MissionTypes)iMission);
						if (pOutcomeMission && pOutcomeMission->isPossible(unitX, bTestVisible))
						{
							return true;
						}
					}
				}
				break;
			}
		}
	}
	return false;
}


bool CvSelectionGroup::startMission()
{
	PROFILE_FUNC();

	FAssert(!isBusy());
	FAssert(getOwner() != NO_PLAYER);
	FAssert(headMissionQueueNode() != NULL);

	if (!GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS) && !GET_PLAYER(getOwner()).isTurnActive())
	{
		return false;
	}
	setActivityType(canAllMove() ? ACTIVITY_MISSION : ACTIVITY_HOLD);

	bool bDelete = false;
	bool bAction = false;
	bool bNuke = false;
	bool bNotify = false;
	bool bResult = true;
	bool bCycle = true;

	if (!canStartMission(headMissionQueueNode()->m_data.eMissionType, headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2, plot()))
	{
		bDelete = true;
		bResult = false;
		bCycle = false;
	}
	else
	{
		FAssertMsg(GET_PLAYER(getOwner()).isTurnActive() || GET_PLAYER(getOwner()).isHumanPlayer(), "It's expected that either the turn is active for this player or the player is human");

		switch (headMissionQueueNode()->m_data.eMissionType)
		{
			case MISSION_MOVE_TO:
#ifdef _MOD_SENTRY
			case MISSION_MOVE_TO_SENTRY:
#endif
			{
				// if player is human, save the visibility and reveal state of the last plot of the move path from the initial plot
				if (isHuman())
				{
					checkLastPathPlot(GC.getMap().plot(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2));
				}
				break;
			}
			case MISSION_SKIP:
			{
				setActivityType(ACTIVITY_HOLD);
				bDelete = true;
				break;
			}
			case MISSION_ESPIONAGE_SLEEP:
			case MISSION_SLEEP:
			{
				setActivityType(ACTIVITY_SLEEP, MISSION_SLEEP);
				bNotify = true;
				bDelete = true;
				break;
			}
			case MISSION_FORTIFY:
			{
				setActivityType(ACTIVITY_SLEEP, MISSION_FORTIFY);
				bNotify = true;
				bDelete = true;
				break;
			}
			case MISSION_BUILDUP:
			{
				setActivityType(ACTIVITY_SLEEP, MISSION_BUILDUP);
				bNotify = true;
				bDelete = true;
				bCycle = false;
				break;
			}
			case MISSION_AUTO_BUILDUP:
			{
				setActivityType(ACTIVITY_SLEEP, MISSION_AUTO_BUILDUP);
				bNotify = true;
				bDelete = true;
				break;
			}
			case MISSION_HEAL_BUILDUP:
			{
				setActivityType(ACTIVITY_HEAL, MISSION_HEAL_BUILDUP);
				bNotify = true;
				bDelete = true;
				break;
			}
			case MISSION_PLUNDER:
			{
				setActivityType(ACTIVITY_PLUNDER);
				bNotify = true;
				bDelete = true;
				break;
			}
			case MISSION_AIRPATROL:
			{
				setActivityType(ACTIVITY_INTERCEPT);
				bDelete = true;
				break;
			}
			case MISSION_SEAPATROL:
			{
				setActivityType(ACTIVITY_PATROL);
				bDelete = true;
				break;
			}
			case MISSION_HEAL:
			{
				setActivityType(ACTIVITY_HEAL, MISSION_SLEEP);
				bNotify = true;
				bDelete = true;
				break;
			}
			case MISSION_SENTRY:
			{
				setActivityType(ACTIVITY_SENTRY);
				bNotify = true;
				bDelete = true;
				break;
			}
#ifdef _MOD_SENTRY
			case MISSION_SENTRY_WHILE_HEAL:
			{
				setActivityType(ACTIVITY_SENTRY_WHILE_HEAL);
				bNotify = true;
				bDelete = true;
				break;
			}
			case MISSION_SENTRY_NAVAL_UNITS:
			{
				setActivityType(ACTIVITY_SENTRY_NAVAL_UNITS);
				bNotify = true;
				bDelete = true;
				break;
			}
			case MISSION_SENTRY_LAND_UNITS:
			{
				setActivityType(ACTIVITY_SENTRY_LAND_UNITS);
				bNotify = true;
				bDelete = true;
				break;
			}
#endif
			case MISSION_AMBUSH:
			case MISSION_ASSASSINATE:
			{
				bDelete = true;
				bCycle = false;
				break;
			}
		}

		if (bNotify)
		{
			NotifyEntity(headMissionQueueNode()->m_data.eMissionType);
		}

		if (headMissionQueueNode()->m_data.eMissionType == MISSION_PILLAGE)
		{
			// Fast units pillage first
			int iMaxMovesLeft = 1;

			foreach_(const CvUnit* pLoopUnit, units())
			{
				if (pLoopUnit->canMove() && pLoopUnit->canPillage(pLoopUnit->plot()))
				{
					int iMovesLeft = pLoopUnit->movesLeft();
					if (pLoopUnit->getBombardRate() > 0)
					{
						iMovesLeft /= 2;
					}
					iMovesLeft /= 100;

					iMaxMovesLeft = std::max(iMaxMovesLeft, iMovesLeft);
				}
			}

			bool bDidPillage = false;
			while (iMaxMovesLeft > 0 && !bDidPillage)
			{
				int iNextMaxMovesLeft = 0;
				foreach_(CvUnit* pLoopUnit, units())
				{
					if (pLoopUnit->canMove() && pLoopUnit->canPillage(pLoopUnit->plot()))
					{
						int iMovesLeft = pLoopUnit->movesLeft();
						if (pLoopUnit->getBombardRate() > 0)
						{
							iMovesLeft /= 2;
						}
						iMovesLeft /= 100;

						iMovesLeft = std::max(iMaxMovesLeft, iMovesLeft);

						if (iMovesLeft >= iMaxMovesLeft && pLoopUnit->pillage())
						{
							bAction = true;
							if( isHuman() || canAllMove() )
							{
								bDidPillage = true;
								iMovesLeft -= 1;
								break;
							}
						}
						iNextMaxMovesLeft = std::min( iNextMaxMovesLeft, iMovesLeft );
					}
				}

				iMaxMovesLeft = iNextMaxMovesLeft;
			}
		}
		else
		{
			CvUnit* pBestUnit = NULL;
			int iBestUnitValue = 0;
			foreach_(CvUnit* pLoopUnit, units())
			{
				if (pLoopUnit->canMove())
				{
					switch (headMissionQueueNode()->m_data.eMissionType)
					{
					case MISSION_ASSASSINATE:
						if (pLoopUnit->canAmbush(plot(), true))
						{
							const int iUnitValue1 = pLoopUnit->AI_attackOdds(plot(), false, 0, true);
							if (iUnitValue1 >= iBestUnitValue)
							{
								pBestUnit = pLoopUnit;
								iBestUnitValue = iUnitValue1;
							}
						}
						break;

					case MISSION_AMBUSH:
						if (pLoopUnit->canAmbush(plot(), false))
						{
							const int iUnitValue2 = pLoopUnit->AI_attackOdds(plot(), false, 0, false);
							if (iUnitValue2 >= iBestUnitValue)
							{
								pBestUnit = pLoopUnit;
								iBestUnitValue = iUnitValue2;
							}
						}
						break;

					default:
						break;
					}
				}
			}

			CLLNode<IDInfo>* pUnitNode = headUnitNode();
			while (pUnitNode)
			{
				CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

				if (pLoopUnit->canMove())
				{
					switch (headMissionQueueNode()->m_data.eMissionType)
					{
						case MISSION_MOVE_TO:
						case MISSION_ROUTE_TO:
						case MISSION_MOVE_TO_UNIT:
						case MISSION_SKIP:
						case MISSION_SLEEP:
						case MISSION_FORTIFY:
						case MISSION_BUILDUP:
						case MISSION_AUTO_BUILDUP:
						case MISSION_HEAL_BUILDUP:
						case MISSION_AIRPATROL:
						case MISSION_SEAPATROL:
						case MISSION_HEAL:
						case MISSION_SENTRY:
						case MISSION_BUILD:
#ifdef _MOD_SENTRY
						case MISSION_MOVE_TO_SENTRY:
						case MISSION_SENTRY_WHILE_HEAL:
						case MISSION_SENTRY_NAVAL_UNITS:
						case MISSION_SENTRY_LAND_UNITS:
#endif
						{
							break;
						}
						case MISSION_AIRLIFT:
						{
							if (pLoopUnit->airlift(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
							{
								bAction = true;
							}
							break;
						}
						case MISSION_NUKE:
						{
							if (pLoopUnit->nuke(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
							{
								bAction = true;

								if (GC.getMap().plot(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2)->isVisibleToWatchingHuman())
								{
									bNuke = true;
								}
							}
							break;
						}
						case MISSION_RECON:
						{
							if (pLoopUnit->recon(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
							{
								bAction = true;
							}
							break;
						}
						case MISSION_PARADROP:
						{
							if (pLoopUnit->paradrop(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
							{
								bAction = true;
							}
							break;
						}
						case MISSION_AIRBOMB:
						{
							if (pLoopUnit->airBomb(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
							{
								bAction = true;
							}
							break;
						}
						case MISSION_BOMBARD:
						{
							if (pLoopUnit->bombard())
							{
								bAction = true;
							}
							break;
						}
						case MISSION_RBOMBARD:
						{
							if(GC.isDCM_RANGE_BOMBARD())
							{
								if (pLoopUnit->bombardRanged(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
								{
									bAction = true;
								}
							}
							break;
						}
						case MISSION_RANGE_ATTACK:
						{
							if (pLoopUnit->rangeStrike(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
							{
								bAction = true;
							}
							break;
						}
						case MISSION_PILLAGE:
						{
							if (pLoopUnit->pillage())
							{
								bAction = true;
							}
							break;
						}
						case MISSION_PLUNDER:
						{
							if (pLoopUnit->plunder())
							{
								bAction = true;
							}
							break;
						}
						case MISSION_SABOTAGE:
						{
							if (pLoopUnit->sabotage())
							{
								bAction = true;
							}
							break;
						}
						case MISSION_DESTROY:
						{
							if (pLoopUnit->destroy())
							{
								bAction = true;
							}
							break;
						}
						case MISSION_STEAL_PLANS:
						{
							if (pLoopUnit->stealPlans())
							{
								bAction = true;
							}
							break;
						}
						case MISSION_FOUND:
						{
							if (pLoopUnit->found())
							{
								bAction = true;
							}
							break;
						}
						case MISSION_SPREAD:
						{
							if (pLoopUnit->spread((ReligionTypes)(headMissionQueueNode()->m_data.iData1)))
							{
								bAction = true;
							}
							break;
						}
						case MISSION_SPREAD_CORPORATION:
						{
							if (pLoopUnit->spreadCorporation((CorporationTypes)(headMissionQueueNode()->m_data.iData1)))
							{
								bAction = true;
							}
							break;
						}
						case MISSION_CURE:
						{
#ifdef OUTBREAKS_AND_AFFLICTIONS
							if (pLoopUnit->CureAffliction((PromotionLineTypes)(headMissionQueueNode()->m_data.iData1)))
							{
								bAction = true;
							}
#endif
							break;
						}
						case MISSION_JOIN:
						{
							if (pLoopUnit->join((SpecialistTypes)(headMissionQueueNode()->m_data.iData1)))
							{
								bAction = true;
							}
							break;
						}
						case MISSION_CONSTRUCT:
						{
							if (pLoopUnit->construct((BuildingTypes)headMissionQueueNode()->m_data.iData1))
							{
								bAction = true;
								pUnitNode = NULL; // City can only have one of each building, only one unit needs to add it.
							}
							break;
						}
						case MISSION_HERITAGE:
						{
							if (pLoopUnit->addHeritage((HeritageTypes)headMissionQueueNode()->m_data.iData1))
							{
								bAction = true;
								pUnitNode = NULL; // Heritages are unique, only one unit needs to add it.
							}
							break;
						}
						case MISSION_DISCOVER:
						{
							if (GET_PLAYER(pLoopUnit->getOwner()).getUnit(headMissionQueueNode()->m_data.iData1)->discover((TechTypes)headMissionQueueNode()->m_data.iData2))
							{
								bAction = true;
							}
							break;
						}
						case MISSION_HURRY:
						{
							if (pLoopUnit->hurry())
							{
								bAction = true;
							}
							break;
						}
						case MISSION_TRADE:
						{
							if (pLoopUnit->trade())
							{
								bAction = true;
							}
							break;
						}
						case MISSION_GREAT_WORK:
						{
							if (pLoopUnit->greatWork())
							{
								bAction = true;
							}
							break;
						}
						case MISSION_INFILTRATE:
						{
							if (pLoopUnit->infiltrate())
							{
								bAction = true;
							}
							break;
						}
						case MISSION_GOLDEN_AGE:
						{
							//just play animation, not golden age - JW
							if (headMissionQueueNode()->m_data.iData1 != -1)
							{
								pLoopUnit->addMission(CvMissionDefinition(MISSION_GOLDEN_AGE, pLoopUnit->plot(), pLoopUnit));
								pLoopUnit->NotifyEntity(MISSION_GOLDEN_AGE);
								bAction = true;
							}
							else if (pLoopUnit->goldenAge())
							{
								bAction = true;
							}
							break;
						}
						case MISSION_LEAD:
						{
							if (pLoopUnit->lead(headMissionQueueNode()->m_data.iData1))
							{
								bAction = true;
							}
							break;
						}
						case MISSION_ESPIONAGE:
						{
							if (pLoopUnit->espionage((EspionageMissionTypes)headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
							{
								bAction = true;
								pUnitNode = NULL; // allow one unit at a time to do espionage
							}
							break;
						}
						case MISSION_AIRBOMB1:
						{
							if (pLoopUnit->airBomb1(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
							{
								bAction = true;
							}
							break;
						}
						case MISSION_AIRBOMB2:
						{
							if (pLoopUnit->airBomb2(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
							{
								bAction = true;
							}
							break;
						}
						case MISSION_AIRBOMB3:
						{
							if (pLoopUnit->airBomb3(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
							{
								bAction = true;
							}
							break;
						}
						case MISSION_AIRBOMB4:
						{
							if (pLoopUnit->airBomb4(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
							{
								bAction = true;
							}
							break;
						}
						case MISSION_AIRBOMB5:
						{
							if (pLoopUnit->airBomb5(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
							{
								bAction = true;
							}
							break;
						}
						case MISSION_FENGAGE:
						{
							if (GC.isDCM_FIGHTER_ENGAGE()
							&& pLoopUnit->fighterEngage(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
							{
								bAction = true;
							}
							break;
						}
						case MISSION_CLAIM_TERRITORY:
						{
							if (pLoopUnit->claimTerritory())
							{
								bAction = true;
								pUnitNode = NULL; // allow only one unit to claim territory (no need for more)
							}
							break;
						}
						case MISSION_HURRY_FOOD:
						{
							if (pLoopUnit->hurryFood())
							{
								bAction = true;
							}
							break;
						}
						case MISSION_INQUISITION:
						{
							if (pLoopUnit->performInquisition())
							{
								bAction = true;
							}
							break;
						}
						case MISSION_ESPIONAGE_SLEEP:
						{
							pLoopUnit->sleepForEspionage();
							break;
						}
						case MISSION_GREAT_COMMANDER:
						{
							if (pLoopUnit->getUnitInfo().isGreatGeneral() && !pLoopUnit->isCommander())
							{
								pLoopUnit->setCommander(true);
								bAction = true;
							}
							break;
						}
						case MISSION_GREAT_COMMODORE:
                        {
                        	if (pLoopUnit->getUnitInfo().isGreatGeneral() && !pLoopUnit->isCommodore())
                        	{
                        		pLoopUnit->setCommodore(true);
                        		bAction = true;
                        	}
                        	break;
                        }
						case MISSION_SHADOW:
						{
							CvPlot* pShadowPlot = GC.getMap().plot(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2);
							if (pShadowPlot != NULL)
							{
								//if (pLoopUnit->canShadowAt(pShadowPlot))
								{
									//Check for multiple valid units
									const int iValidShadowUnits = algo::count_if(pShadowPlot->units(),
										bind(CvUnit::canShadowAt, _1, pShadowPlot, _1)
									);
									//Strange Handling to ensure MP works
									if (headMissionQueueNode()->m_data.iFlags == 0 && iValidShadowUnits > 1)
									{
										CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_SELECT_UNIT, pLoopUnit->getID(), pShadowPlot->getX(), pShadowPlot->getY());
										if (pInfo)
										{
											gDLL->getInterfaceIFace()->addPopup(pInfo, pLoopUnit->getOwner(), true);
										}
									}
									else if (iValidShadowUnits > 0)
									{
										if (iValidShadowUnits == 1)
										{
											pLoopUnit->setShadowUnit(pShadowPlot->getCenterUnit(false));
										}
										else
										{
											pLoopUnit->setShadowUnit(GET_PLAYER(pLoopUnit->getOwner()).getUnit(headMissionQueueNode()->m_data.iFlags));
										}
										setAutomateType(AUTOMATE_SHADOW);
										bAction = true;
									}
								}
							}
							break;
						}
						case MISSION_DIE_ANIMATION:
						{
							bAction = true;
							break;
						}
						case MISSION_ASSASSINATE:
						{
							if (pLoopUnit == pBestUnit && pLoopUnit->doAmbush(true))
							{
								bAction = true;
							}
							break;
						}
						case MISSION_AMBUSH:
						{
							if (pLoopUnit == pBestUnit && pLoopUnit->doAmbush(false))
							{
								bAction = true;
							}
							break;
						}
						default: // AIAndy: Assumed to be an outcome mission
						{
							if (pLoopUnit->doOutcomeMission(headMissionQueueNode()->m_data.eMissionType))
							{
								bAction = true;
							}
							break;
						}
					}

					if (getNumUnits() == 0 || headMissionQueueNode() == NULL)
					{
						break;
					}
				}
			}
		}
	}

	if (getNumUnits() > 0 && headMissionQueueNode())
	{
		if (bAction && isHuman() && plot()->isVisibleToWatchingHuman())
		{
			updateMissionTimer();
		}

		if (bNuke)
		{
			setMissionTimer(GC.getMissionInfo(MISSION_NUKE).getTime());
		}
		OutputDebugString(CvString::format("%S startMission endish mission=%d...\n", getHeadUnit()->getDescription().c_str(), headMissionQueueNode()->m_data.eMissionType).c_str());

		if (!isBusy())
		{
			OutputDebugString(CvString::format("%S Not Busy...\n", getHeadUnit()->getDescription().c_str()).c_str());
			if (bDelete)
			{
				deleteMissionQueueNode(headMissionQueueNode());
			}
			else if (getActivityType() == ACTIVITY_MISSION)
			{
				return continueMission();
			}
			OutputDebugString(CvString::format("%S Not continueMission...\n", getHeadUnit()->getDescription().c_str()).c_str());

			if (IsSelected() && (bCycle || !canAnyMove()))
			{
				GC.getGame().updateSelectionListInternal();
			}
		}
	}
	return bResult;
}


bool CvSelectionGroup::continueMission(int iSteps)
{
	PROFILE_FUNC();

	CLLNode<MissionData>* missionNode = headMissionQueueNode();

	OutputDebugString(CvString::format("%S part 1 continueMission %d...\n", getHeadUnit()->getDescription().c_str(), missionNode == NULL ? -1 : missionNode->m_data.eMissionType).c_str());

	FAssert(!isBusy());
	FAssert(missionNode);
	FAssert(getOwner() != NO_PLAYER);

	if (getActivityType() == ACTIVITY_AWAKE)
	{
		return false;
	}
	FAssert(getActivityType() == ACTIVITY_MISSION);

	// just in case...
	if (!missionNode || isMoveMission(missionNode) && (missionNode->m_data.iData1 == -1 || missionNode->m_data.iData2 == -1))
	{
		FErrorMsg("Should not occur");
		setActivityType(ACTIVITY_AWAKE);
		return false;
	}

	bool bDone = false;
	bool bAction = false;
	bool bFailed = false;
	bool bCombat = false;

	if ((missionNode->m_data.iPushTurn == GC.getGame().getGameTurn() || (missionNode->m_data.iFlags & MOVE_THROUGH_ENEMY))
	&& missionNode->m_data.eMissionType == MISSION_MOVE_TO
	&& canAllMove())
	{
		bool bFailedAlreadyFighting;
		if (groupAttack(missionNode->m_data.iData1, missionNode->m_data.iData2, missionNode->m_data.iFlags, bFailedAlreadyFighting))
		{
			bDone = true;
			bCombat = true;
		}
		missionNode = headMissionQueueNode();
		FAssert(getHeadUnit());
		FAssert(getNumUnits() > 0);
		OutputDebugString(CvString::format("%S continueMission %d after attack...\n", getHeadUnit() ? getHeadUnit()->getDescription().c_str() : L"Empty group", missionNode == NULL ? -1 : missionNode->m_data.eMissionType).c_str());

		// extra crash protection, should never happen (but a previous bug in groupAttack was causing a NULL here)
		// while that bug is fixed, no reason to not be a little more careful
		if (!missionNode)
		{
			FErrorMsg("missionNode == NULL after groupAttack");
			if (IsSelected() && !canAnyMove())
			{
				GC.getGame().updateSelectionListInternal();
			}
			return true; // It did something so the mission was not a failure
		}
	}

	if (!bDone && getNumUnits() > 0 && canAllMove())
	{
		switch (missionNode->m_data.eMissionType)
		{
			case MISSION_MOVE_TO:
#ifdef _MOD_SENTRY
			case MISSION_MOVE_TO_SENTRY:
#endif
			{
				// if player is human, save the visibility and reveal state of the last plot of the move path from the initial plot
				// if it hasn't been saved already to handle units in motion when loading a game
				if (isHuman() && !isLastPathPlotChecked())
				{
					checkLastPathPlot(GC.getMap().plot(missionNode->m_data.iData1, missionNode->m_data.iData2));
				}

				if (getDomainType() == DOMAIN_AIR)
				{
					groupPathTo(missionNode->m_data.iData1, missionNode->m_data.iData2, missionNode->m_data.iFlags);
					bDone = true;
				}
				else if (groupPathTo(missionNode->m_data.iData1, missionNode->m_data.iData2, missionNode->m_data.iFlags))
				{
					if (getNumUnits() > 0
					&& !canAllMove()
					&& headMissionQueueNode()
					&& groupAmphibMove(GC.getMap().plot(missionNode->m_data.iData1, missionNode->m_data.iData2), missionNode->m_data.iFlags))
					{
						bAction = false;
						bDone = true;
					}
					else bAction = true;
				}
				else if (getNumUnits() < 1 || missionNode == NULL
				|| !groupAmphibMove(GC.getMap().plot(missionNode->m_data.iData1, missionNode->m_data.iData2), missionNode->m_data.iFlags))
				{
					bFailed = true; // Can't move there! (e.g. - breaches stacking limit)
					bDone = true;
				}
				else
				{
					bAction = false;
					bDone = true;
				}
				break;
			}
			case MISSION_ROUTE_TO:
			{
				if (groupRoadTo(missionNode->m_data.iData1, missionNode->m_data.iData2, missionNode->m_data.iFlags))
				{
					bAction = true;
				}
				else
				{
					bFailed = true; // Can't move there! (e.g. - breaches stacking limit)
					bDone = true;
				}
				break;
			}
			case MISSION_MOVE_TO_UNIT:
			{
				if (getHeadUnitAI() == UNITAI_CITY_DEFENSE && plot()->isCity() && (plot()->getTeam() == getTeam())
				&& plot()->getBestDefender(getOwner())->getGroup() == this)
				{
					bAction = false;
					bDone = true;
					break;
				}
				CvUnit* pTargetUnit = GET_PLAYER((PlayerTypes)missionNode->m_data.iData1).getUnit(missionNode->m_data.iData2);
				if (pTargetUnit)
				{
					// Handling for mission to retrieve a unit
					if (AI_getMissionAIType() == MISSIONAI_PICKUP)
					{
						if (!pTargetUnit->getGroup()->isStranded() || isFull() || !pTargetUnit->plot())
						{
							bDone = true;
							bAction = false;
							break;
						}
						CvPlot* pPickupPlot = NULL;
						int iPathTurns;
						int iBestPathTurns = MAX_INT;

						if ((canMoveAllTerrain() || pTargetUnit->plot()->isWater() || pTargetUnit->plot()->isFriendlyCity(*getHeadUnit(), true))
						&& generatePath(plot(), pTargetUnit->plot(), 0, false, &iPathTurns))
						{
							pPickupPlot = pTargetUnit->plot();
						}
						else
						{
							foreach_(CvPlot* pAdjacentPlot, pTargetUnit->plot()->adjacent())
							{
								if (atPlot(pAdjacentPlot))
								{
									pPickupPlot = pAdjacentPlot;
									break;
								}
								if ((pAdjacentPlot->isWater() || pAdjacentPlot->isFriendlyCity(*getHeadUnit(), true))
								&& generatePath(plot(), pAdjacentPlot, 0, true, &iPathTurns)
								&& iPathTurns < iBestPathTurns)
								{
									pPickupPlot = pAdjacentPlot;
									iBestPathTurns = iPathTurns;
								}
							}
						}

						if (pPickupPlot != NULL)
						{
							if (atPlot(pPickupPlot))
							{
								foreach_(CvUnit* pLoopUnit, units() | filtered(!CvUnit::fn::isFull()))
								{
									pTargetUnit->getGroup()->setRemoteTransportUnit(pLoopUnit);
								}
								bAction = true;
								bDone = true;
							}
							else if (groupPathTo(pPickupPlot->getX(), pPickupPlot->getY(), missionNode->m_data.iFlags))
							{
								bAction = true;
							}
							else bDone = true;
						}
						else bDone = true;

						break;
					}

					if (AI_getMissionAIType() != MISSIONAI_SHADOW
					&&  AI_getMissionAIType() != MISSIONAI_GROUP
					&& (!plot()->isOwned() || plot()->getOwner() == getOwner()))
					{
						const CvPlot* pMissionPlot = pTargetUnit->getGroup()->AI_getMissionAIPlot();
						if (pMissionPlot != NULL
						&& NO_TEAM != pMissionPlot->getTeam()
						&& pMissionPlot->isOwned()
						&& pTargetUnit->isPotentialEnemy(pMissionPlot->getTeam(), pMissionPlot))
						{
							bAction = false;
							bDone = true;
							break;
						}
					}

					if (groupPathTo(pTargetUnit->getX(), pTargetUnit->getY(), missionNode->m_data.iFlags))
					{
						bAction = true;
					}
					else
					{
						bFailed = true;	//	Can't move there! (e.g. - breaches stacking limit)
						bDone = true;
					}
				}
				else bDone = true;

				break;
			}
			case MISSION_SKIP:
			case MISSION_ESPIONAGE_SLEEP:
			case MISSION_SLEEP:
			case MISSION_FORTIFY:
			case MISSION_BUILDUP:
			case MISSION_AUTO_BUILDUP:
			case MISSION_HEAL_BUILDUP:
			case MISSION_PLUNDER:
			case MISSION_AIRPATROL:
			case MISSION_SEAPATROL:
			case MISSION_HEAL:
			case MISSION_SENTRY:
#ifdef _MOD_SENTRY
			case MISSION_SENTRY_WHILE_HEAL:
			case MISSION_SENTRY_NAVAL_UNITS:
			case MISSION_SENTRY_LAND_UNITS:
#endif
			{
				FErrorMsg("error");
				break;
			}
			case MISSION_BUILD:
			{
				OutputDebugString(CvString::format("%S MISSION_BUILD...\n", getHeadUnit()->getDescription().c_str()).c_str());
				if (!groupBuild((BuildTypes)(missionNode->m_data.iData1)))
				{
					OutputDebugString(CvString::format("%S Failed...\n", getHeadUnit()->getDescription().c_str()).c_str());
					bFailed = true;
					bDone = true;
				}
				break;
			}
		}
	}
	missionNode = headMissionQueueNode();

	if (missionNode && getNumUnits() > 0)
	{
		OutputDebugString(CvString::format("%S part 3 continueMission %d...\n", getHeadUnit()->getDescription().c_str(), missionNode == NULL ? -1 : missionNode->m_data.eMissionType).c_str());

		if (!bDone)
		{
			switch (missionNode->m_data.eMissionType)
			{
				case MISSION_MOVE_TO:
#ifdef _MOD_SENTRY
				case MISSION_MOVE_TO_SENTRY:
#endif
				{
					if (at(missionNode->m_data.iData1, missionNode->m_data.iData2))
					{
						clearLastPathPlot();
						bDone = true;
					}
					break;
				}
				case MISSION_ROUTE_TO:
				{
					if (at(missionNode->m_data.iData1, missionNode->m_data.iData2) && getBestBuildRoute(plot()) == NO_ROUTE)
					{
						bDone = true;
					}
					break;
				}
				case MISSION_MOVE_TO_UNIT:
				{
					CvUnit* pTargetUnit = GET_PLAYER((PlayerTypes)missionNode->m_data.iData1).getUnit(missionNode->m_data.iData2);
					if (!pTargetUnit || atPlot(pTargetUnit->plot()))
					{
						bDone = true;
					}
					break;
				}
				case MISSION_SKIP:
				case MISSION_ESPIONAGE_SLEEP:
				case MISSION_SLEEP:
				case MISSION_FORTIFY:
				case MISSION_BUILDUP:
				case MISSION_AUTO_BUILDUP:
				case MISSION_HEAL_BUILDUP:
				case MISSION_PLUNDER:
				case MISSION_AIRPATROL:
				case MISSION_SEAPATROL:
				case MISSION_HEAL:
				case MISSION_SENTRY:
#ifdef _MOD_SENTRY
				case MISSION_SENTRY_WHILE_HEAL:
				case MISSION_SENTRY_NAVAL_UNITS:
				case MISSION_SENTRY_LAND_UNITS:
#endif
				{
					FErrorMsg("error");
					break;
				}
				case MISSION_BUILD:
				{
					break;
				}
				default:
				{
					bDone = true;
					break;
				}
			}
		}

		if (bAction && (bDone || !canAllMove()) && plot()->isVisibleToWatchingHuman())
		{
			updateMissionTimer(iSteps);

			if (showMoves()
			&& GC.getGame().getActivePlayer() != NO_PLAYER
			&& GC.getGame().getActivePlayer() != getOwner()
			&& plot()->isActiveVisible(false)
			&& !isInvisible(GC.getGame().getActiveTeam())
			&& plot()->isInViewport())
			{
				gDLL->getInterfaceIFace()->lookAt(plot()->getPoint(), CAMERALOOKAT_NORMAL);
			}
		}

		if (bDone)
		{
			OutputDebugString(CvString::format("%S Done...\n", getHeadUnit()->getDescription().c_str()).c_str());

			if (IsSelected() && !bCombat && !canAllSelectedMove())
			{
				GC.getGame().updateSelectionListInternal();
			}

			if (!isBusy())
			{
				OutputDebugString(CvString::format("%S Not Busy...\n", getHeadUnit()->getDescription().c_str()).c_str());

				if (!isHuman() || missionNode->m_data.eMissionType != MISSION_MOVE_TO)
				{
					deleteMissionQueueNode(missionNode);

					// We executed this one ok but if we have no moves left don't leave
					//	it with ACTIVITY_AWAKE as its state as that'll just cause the AI to spin wheels.
					//	Preserve the missionAI and target plot/unit though as other units may search on them.
					if (!isHuman() && !canAllMove() && !missionNode)
					{
						pushMission(MISSION_SKIP, -1, -1, 0, false, false, AI_getMissionAIType(), AI_getMissionAIPlot(), AI_getMissionAIUnit());
					}
				}
				else if (canAllMove() || !nextMissionQueueNode(missionNode))
				{
					deleteMissionQueueNode(missionNode);
				}
			}
			else if (bCombat && isHuman() && !nextMissionQueueNode(missionNode))
			{
				clearMissionQueue();
			}
		}
		else if (canAllMove())
		{
			// If the recursion fails that is not an overall failure, since this step did something
			continueMission(iSteps + 1);
		}
	}
	return !bFailed;
}


void CvSelectionGroup::doCommand(CommandTypes eCommand, int iData1, int iData2)
{
	PROFILE_EXTRA_FUNC();
	foreach_(CvUnit* unit, units())
	{
		unit->doCommand(eCommand, iData1, iData2);
	}
}

bool CvSelectionGroup::canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible, bool bUseCache, bool bAll) const
{
	PROFILE_FUNC();

	if ((bUseCache ? m_bIsBusyCache : isBusy())
	|| !canEverDoCommand(eCommand, iData1, iData2, bTestVisible, bUseCache))
	{
		return false;
	}

	foreach_(const CvUnit* pLoopUnit, units())
	{
		if (pLoopUnit->canDoCommand(eCommand, iData1, iData2, bTestVisible, false))
		{
			if (!bAll)
			{
				return true;
			}
		}
		else if (bAll)
		{
			return false;
		}
	}
	if (bAll)
	{
		return true;
	}
	return false;
}

bool CvSelectionGroup::canEverDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible, bool bUseCache) const
{
	PROFILE_EXTRA_FUNC();
	if (eCommand == COMMAND_LOAD)
	{
		return algo::any_of(plot()->units(), !CvUnit::fn::isFull());
	}
	if (eCommand == COMMAND_UNLOAD)
	{
		return algo::any_of(units(), CvUnit::fn::isCargo());
	}
	if (eCommand == COMMAND_UPGRADE && bUseCache)
	{
		//see if any of the different units can upgrade to this unit type
		for(int i=0;i<(int)m_aDifferentUnitCache.size();i++)
		{
			const CvUnit *unit = m_aDifferentUnitCache[i];
			if(unit->canDoCommand(eCommand, iData1, iData2, bTestVisible, false))
				return true;
		}
		return false;
	}
	return true;
}

void CvSelectionGroup::setupActionCache()
{
	PROFILE_EXTRA_FUNC();
	//cache busy calculation
	m_bIsBusyCache = isBusy();

	//cache different unit types
	m_aDifferentUnitCache.erase(m_aDifferentUnitCache.begin(), m_aDifferentUnitCache.end());

	foreach_(CvUnit* unit, units())
	{
		if (unit->isReadyForUpgrade())
		{
			const UnitTypes unitType = unit->getUnitType();
			bool bFound = false;
			for (int i = 0; i < (int)m_aDifferentUnitCache.size(); i++)
			{
				if (unitType == m_aDifferentUnitCache[i]->getUnitType())
				{
					bFound = true;
					break;
				}
			}
			if (!bFound) m_aDifferentUnitCache.push_back(unit);
		}
	}
}

// Returns true if one of the units can support the interface mode...
bool CvSelectionGroup::canDoInterfaceMode(InterfaceModeTypes eInterfaceMode)
{
	PROFILE_FUNC();

	FAssertMsg(eInterfaceMode != NO_INTERFACEMODE, "InterfaceMode is not assigned a valid value");

	if (isBusy())
	{
		return false;
	}

	foreach_(const CvUnit* pLoopUnit, units())
	{
		switch (eInterfaceMode)
		{
#ifdef _MOD_SENTRY
			case INTERFACEMODE_GO_TO_SENTRY:
			{
				if (sentryAlertSameDomainType())
				{
					return false;
				}
				// fall through to next case
			}
#endif
			case INTERFACEMODE_GO_TO:
			{
				if (getDomainType() != DOMAIN_AIR && getDomainType() != DOMAIN_IMMOBILE)
				{
					return true;
				}
				break;
			}
			case INTERFACEMODE_GO_TO_TYPE:
			{
				if (getDomainType() != DOMAIN_AIR && getDomainType() != DOMAIN_IMMOBILE)
				{
					if (pLoopUnit->plot()->plotCount(PUF_isUnitType, pLoopUnit->getUnitType(), -1, NULL, pLoopUnit->getOwner()) > 1)
					{
						return true;
					}
				}
				break;
			}
			case INTERFACEMODE_GO_TO_ALL:
			{
				if (getDomainType() != DOMAIN_AIR && getDomainType() != DOMAIN_IMMOBILE)
				{
					if (pLoopUnit->plot()->plotCount(NULL, -1, -1, NULL, pLoopUnit->getOwner()) > 1)
					{
						return true;
					}
				}
				break;
			}
			case INTERFACEMODE_ROUTE_TO:
			{
				if (pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER || pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER_SEA)
				{
					if (pLoopUnit->canBuildRoute())
					{
						return true;
					}
				}
				break;
			}
			case INTERFACEMODE_AIRLIFT:
			{
				if (pLoopUnit->canAirlift(pLoopUnit->plot()))
				{
					return true;
				}
				break;
			}
			case INTERFACEMODE_NUKE:
			{
				if (pLoopUnit->canNuke() && pLoopUnit->canMove())
				{
					return true;
				}
				break;
			}
			case INTERFACEMODE_RECON:
			{
				if (pLoopUnit->canRecon())
				{
					return true;
				}
				break;
			}
			case INTERFACEMODE_PARADROP:
			{
				if (pLoopUnit->canParadrop(pLoopUnit->plot()))
				{
					return true;
				}
				break;
			}
			case INTERFACEMODE_AIRBOMB:
			{
				if (pLoopUnit->canAirBomb())
				{
					return true;
				}
				break;
			}
			case INTERFACEMODE_RANGE_ATTACK:
			{
				if (pLoopUnit->canRangeStrike())
				{
					return true;
				}
				break;
			}
			case INTERFACEMODE_AIRSTRIKE:
			{
				if (pLoopUnit->getDomainType() == DOMAIN_AIR)
				{
					if (pLoopUnit->canAirAttack())
					{
						return true;
					}
				}
				break;
			}
			case INTERFACEMODE_REBASE:
			{
				if (pLoopUnit->getDomainType() == DOMAIN_AIR)
				{
					return true;
				}
				break;
			}
			case INTERFACEMODE_AIRBOMB1:
			{
				if (pLoopUnit->canAirBomb1())
				{
					return true;
				}
				break;
			}
			case INTERFACEMODE_AIRBOMB2:
			{
				if (pLoopUnit->canAirBomb2())
				{
					return true;
				}
				break;
			}
			case INTERFACEMODE_AIRBOMB3:
			{
				if (pLoopUnit->canAirBomb3())
				{
					return true;
				}
				break;
			}
			case INTERFACEMODE_AIRBOMB4:
			{
				if (pLoopUnit->canAirBomb4())
				{
					return true;
				}
				break;
			}
			case INTERFACEMODE_AIRBOMB5:
			{
				if (pLoopUnit->canAirBomb5())
				{
					return true;
				}
				break;
			}
			case INTERFACEMODE_BOMBARD:
			{
				if (pLoopUnit->canRBombard())
				{
					return true;
				}
				break;
			}
			case INTERFACEMODE_FENGAGE:
			{
				if (pLoopUnit->canFEngage() && GC.isDCM_FIGHTER_ENGAGE())
				{
					return true;
				}
				break;
			}
			case INTERFACEMODE_SHADOW_UNIT:
			{
				if (pLoopUnit->canShadow())
				{
					return true;
				}
				break;
			}
		}
	}
	return false;
}


// Returns true if one of the units can execute the interface mode at the specified plot...
bool CvSelectionGroup::canDoInterfaceModeAt(InterfaceModeTypes eInterfaceMode, CvPlot* pPlot)
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(eInterfaceMode != NO_INTERFACEMODE, "InterfaceMode is not assigned a valid value");

	foreach_(const CvUnit* pLoopUnit, units())
	{
		FAssertMsg(pLoopUnit != NULL, "TestAssert, if this never happen then remove the following if statement");

		if (pLoopUnit == NULL) return false;

		switch (eInterfaceMode)
		{
			case INTERFACEMODE_AIRLIFT:
			{
				return pLoopUnit->canAirliftAt(pLoopUnit->plot(), pPlot->getX(), pPlot->getY());
			}
			case INTERFACEMODE_NUKE:
			{
				return pLoopUnit->canNukeAt(pLoopUnit->plot(), pPlot->getX(), pPlot->getY());
			}
			case INTERFACEMODE_RECON:
			{
				return pLoopUnit->canReconAt(pLoopUnit->plot(), pPlot->getX(), pPlot->getY());
			}
			case INTERFACEMODE_PARADROP:
			{
				return pLoopUnit->canParadropAt(pLoopUnit->plot(), pPlot->getX(), pPlot->getY());
			}
			case INTERFACEMODE_AIRBOMB:
			{
				return pLoopUnit->canAirBombAt(pLoopUnit->plot(), pPlot->getX(), pPlot->getY());
			}
			case INTERFACEMODE_RANGE_ATTACK:
			{
				return pLoopUnit->canRangeStrikeAt(pLoopUnit->plot(), pPlot->getX(), pPlot->getY());
			}
			case INTERFACEMODE_AIRSTRIKE:
			{
				return pLoopUnit->canEnterPlot(pPlot, MoveCheck::Attack);
			}
			case INTERFACEMODE_REBASE:
			{
				return pLoopUnit->canEnterPlot(pPlot);
			}
			case INTERFACEMODE_AIRBOMB1:
			{
				return GC.isDCM_AIR_BOMBING() && pLoopUnit->canAirBomb1At(pLoopUnit->plot(), pPlot->getX(), pPlot->getY());
			}
			case INTERFACEMODE_AIRBOMB2:
			{
				return GC.isDCM_AIR_BOMBING() && pLoopUnit->canAirBomb2At(pLoopUnit->plot(), pPlot->getX(), pPlot->getY());
			}
			case INTERFACEMODE_AIRBOMB3:
			{
				return GC.isDCM_AIR_BOMBING() && pLoopUnit->canAirBomb3At(pLoopUnit->plot(), pPlot->getX(), pPlot->getY());
			}
			case INTERFACEMODE_AIRBOMB4:
			{
				return GC.isDCM_AIR_BOMBING() && pLoopUnit->canAirBomb4At(pLoopUnit->plot(), pPlot->getX(), pPlot->getY());
			}
			case INTERFACEMODE_AIRBOMB5:
			{
				return GC.isDCM_AIR_BOMBING() && pLoopUnit->canAirBomb5At(pLoopUnit->plot(), pPlot->getX(), pPlot->getY());
			}
			case INTERFACEMODE_BOMBARD:
			{
				return GC.isDCM_RANGE_BOMBARD() && pLoopUnit->canBombardAtRanged(pLoopUnit->plot(), pPlot->getX(), pPlot->getY());
			}
			case INTERFACEMODE_FENGAGE:
			{
				return GC.isDCM_FIGHTER_ENGAGE() && pLoopUnit->canFEngageAt(pLoopUnit->plot(), pPlot->getX(), pPlot->getY());
			}
			case INTERFACEMODE_SHADOW_UNIT:
			{
				foreach_(CvUnit* pLoopUnit, pPlot->units())
				{
					if (pLoopUnit->canShadowAt(pPlot, pLoopUnit))
					{
						return true;
					}
				}
				return false;
			}
			default: return true;
		}
	}
	return false;
}


bool CvSelectionGroup::isHuman() const
{
	return (getOwner() != NO_PLAYER) ? GET_PLAYER(getOwner()).isHumanPlayer() : true;
}

bool CvSelectionGroup::isBusy() const
{
	return getMissionTimer() > 0 || isCombat();
}

bool CvSelectionGroup::isCombat(const CvUnit* ignoreMe) const
{
	foreach_(const CvUnit* unitX, units())
	{
		if ((!ignoreMe || ignoreMe != unitX) && unitX->isCombat())
		{
			return true;
		}
	}
	return false;
}

bool CvSelectionGroup::isCargoBusy() const
{
	PROFILE_EXTRA_FUNC();
	const CvPlot* pPlot = plot();
	foreach_(const CvUnit* pLoopUnit1, units() | filtered(CvUnit::fn::hasCargo()))
	{
		foreach_(const CvUnit* pLoopUnit2, pPlot->units())
		{
			if (pLoopUnit2->getTransportUnit() == pLoopUnit1 && pLoopUnit2->getGroup()->isBusy())
			{
				return true;
			}
		}
	}

	return false;
}


int CvSelectionGroup::baseMoves() const
{
	PROFILE_EXTRA_FUNC();
	int iBestValue = MAX_INT;

	foreach_(const CvUnit* pLoopUnit, units())
	{
		const int iValue = pLoopUnit->baseMoves();
		if (iValue < iBestValue)
		{
			iBestValue = iValue;
		}
	}

	return iBestValue;
}


bool CvSelectionGroup::isWaiting() const
{
	return ((getActivityType() == ACTIVITY_HOLD) ||
			  (getActivityType() == ACTIVITY_SLEEP) ||
					(getActivityType() == ACTIVITY_HEAL) ||
					(getActivityType() == ACTIVITY_SENTRY) ||
#ifdef _MOD_SENTRY
					(getActivityType() == ACTIVITY_SENTRY_WHILE_HEAL) ||
					(getActivityType() == ACTIVITY_SENTRY_NAVAL_UNITS) ||
					(getActivityType() == ACTIVITY_SENTRY_LAND_UNITS) ||
#endif
					(getActivityType() == ACTIVITY_PATROL) ||
					(getActivityType() == ACTIVITY_PLUNDER) ||
					(getActivityType() == ACTIVITY_INTERCEPT));
}


bool CvSelectionGroup::isFull() const
{
	PROFILE_EXTRA_FUNC();
	// do two passes, the first pass, we ignore units with speical cargo
	int iSpecialCargoCount = 0;
	int iCargoCount = 0;

	// first pass, count but ignore special cargo units
	foreach_(const CvUnit* pLoopUnit, units())
	{
		if (pLoopUnit->cargoSpace() > 0)
		{
			iCargoCount++;
		}

		if (pLoopUnit->getSpecialCargo() != NO_SPECIALUNIT)
		{
			iSpecialCargoCount++;
		}
		else if (!pLoopUnit->isFull())
		{
			return false;
		}
	}

	// if every unit in the group has special cargo, then check those, otherwise, consider ourselves full
	if (iSpecialCargoCount >= iCargoCount)
	{
		if (!algo::all_of(units(), CvUnit::fn::isFull()))
		{
			return false;
		}
	}

	return true;
}


bool CvSelectionGroup::hasCargo() const
{
	return algo::any_of(units(), CvUnit::fn::hasCargo());
}

//Call for volume when you want the total cargo volume held by the group
int CvSelectionGroup::getCargo(bool bVolume) const
{
	if (bVolume && GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
	{
		return algo::accumulate(units() | transformed(CvUnit::fn::SMgetCargo()), 0);
	}
	return algo::accumulate(units() | transformed(CvUnit::fn::getCargo()), 0);
}

bool CvSelectionGroup::canAllMove() const
{
	return getNumUnits() > 0 && algo::all_of(units(), CvUnit::fn::canMove());
}

bool CvSelectionGroup::canAllSelectedMove() const
{
	PROFILE_EXTRA_FUNC();
	FAssert(isHuman());
	if (!IsSelected()) return false;

	foreach_(const CvUnit* unitX, units())
	{
		if (unitX->IsSelected() && !unitX->canMove())
		{
			return false;
		}
	}
	return true;
}

bool CvSelectionGroup::canAnyMove(bool bValidate)
{
	if (!bValidate)
	{
		return algo::any_of(units(), CvUnit::fn::canMove());
	}
	const int canMoveCount = algo::count_if(units(), CvUnit::fn::canMove());

	if (canMoveCount > 0 && canMoveCount < getNumUnits() && !isBusy() && getActivityType() == ACTIVITY_AWAKE)
	{
		// Selection group has both units that can and cannot move further.
		//	If we're validating the state then such groups should be busy (else a WFoC will result),
		//	so if they are not set them to sleep for this turn
		setActivityType(ACTIVITY_SLEEP);
	}
	return canMoveCount > 0;
}

bool CvSelectionGroup::hasMoved() const
{
	return algo::any_of(units(), CvUnit::fn::hasMoved());
}


bool CvSelectionGroup::canEnterTerritory(TeamTypes eTeam, bool bIgnoreRightOfPassage) const
{
	return getNumUnits() > 0
		&& algo::all_of(units(), bind(&CvUnit::canEnterTerritory, _1, eTeam, bIgnoreRightOfPassage));
}

bool CvSelectionGroup::canEnterArea(TeamTypes eTeam, const CvArea* pArea, bool bIgnoreRightOfPassage) const
{
	return getNumUnits() > 0
		&& algo::all_of(units(), bind(&CvUnit::canEnterArea, _1, eTeam, pArea, bIgnoreRightOfPassage));
}

bool CvSelectionGroup::canEnterPlot(const CvPlot* pPlot, bool bAttack) const
{
	return getNumUnits() > 0 && algo::any_of(units(), bind(&CvUnit::canEnterPlot, _1, pPlot, bAttack ? MoveCheck::Attack : MoveCheck::None, nullptr));
}
/*DllExport*/ bool CvSelectionGroup::canMoveInto(CvPlot* pPlot, bool bAttack)
{
#ifdef _DEBUG
	OutputDebugString("exe is asking if group can move into a plot\n");
#endif
	return canEnterPlot(pPlot, bAttack);
}

bool CvSelectionGroup::canEnterOrAttackPlot(const CvPlot* pPlot, bool bDeclareWar) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvUnit* unitX, units())
	{
		if (unitX->canEnterOrAttackPlot(pPlot, bDeclareWar))
		{
			return true;
		}
	}
	return false;
}
/*DllExport*/ bool CvSelectionGroup::canMoveOrAttackInto(CvPlot* pPlot, bool bDeclareWar)
{
#ifdef _DEBUG
	OutputDebugString("exe is asking if group can move into or attack a plot\n");
#endif
	return canEnterOrAttackPlot(pPlot, bDeclareWar);
}

bool CvSelectionGroup::canMoveThrough(const CvPlot* pPlot, bool bDeclareWar) const
{
	PROFILE_EXTRA_FUNC();
#define	MAX_UNIQUE_UNIT_CATEGORIES_CONSIDERED	3
	int	unitCharacteristics[MAX_UNIQUE_UNIT_CATEGORIES_CONSIDERED];
	int numUniqueUnitCategories = 0;

	FAssert(getNumUnits() > 0);

	foreach_(const CvUnit* pLoopUnit, units())
	{
		int unitMovementCharacteristics = pLoopUnit->getMovementCharacteristicsHash();

		//	If we've already considred a unit with these characteristics no need to
		//	check this one too
		for(int iI = 0; iI < numUniqueUnitCategories; iI++)
		{
			if ( unitMovementCharacteristics == unitCharacteristics[iI] )
			{
				break;
			}
		}

//#define TEST_NEW_ALGORITHM_VALIDITY
#ifndef TEST_NEW_ALGORITHM_VALIDITY
		if ( iI == numUniqueUnitCategories )
#endif
		{
			if (!(pLoopUnit->canMoveThrough(pPlot, bDeclareWar)))
			{
				FAssert( iI == numUniqueUnitCategories );
				return false;
			}

#ifdef TEST_NEW_ALGORITHM_VALIDITY
			if ( iI == numUniqueUnitCategories )
#endif
			if ( numUniqueUnitCategories < MAX_UNIQUE_UNIT_CATEGORIES_CONSIDERED )
			{
				unitCharacteristics[numUniqueUnitCategories++] = unitMovementCharacteristics;
			}
		}
	}

	return true;
}


bool CvSelectionGroup::canFight() const
{
	return algo::any_of(units(), CvUnit::fn::canFight());
}

bool CvSelectionGroup::canAttackNow() const
{
	return algo::any_of(units(), CvUnit::fn::canAttackNow());
}

bool CvSelectionGroup::canDefend() const
{
	return algo::any_of(units(), CvUnit::fn::canDefend());
}

bool CvSelectionGroup::hasBombardCapability() const
{
	return algo::any_of(units(), CvUnit::fn::getBombardRate() > 0);
}

bool CvSelectionGroup::hasCollateralDamage() const
{
	return algo::any_of(units(), CvUnit::fn::collateralDamage() > 0);
}

bool CvSelectionGroup::canBombard(const CvPlot* pPlot, bool bCheckCanReduceOnly) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvUnit* pLoopUnit, units())
	{
		if (pLoopUnit->canBombard(pPlot, bCheckCanReduceOnly))
		{
			return true;
		}
		// Dale - RB: Field Bombard
		if (!bCheckCanReduceOnly && pLoopUnit->canRBombard())
		{
			return true;
		}
	}

	return false;
}

bool CvSelectionGroup::canPillage(const CvPlot* pPlot) const
{
	return algo::any_of(units(), CvUnit::fn::canPillage(pPlot));
}

bool CvSelectionGroup::canBombardAtRanged(const CvPlot* pPlot, int iX, int iY) const
{
	return algo::any_of(units(), CvUnit::fn::canBombardAtRanged(pPlot, iX, iY));
}

int CvSelectionGroup::getMinimumRBombardDamageLimit() const
{
	PROFILE_EXTRA_FUNC();
	int iLowest = MAX_INT;
	foreach_(const CvUnit* pLoopUnit, units())
	{
		const int iTemp = pLoopUnit->rBombardDamageLimit();
		if (iTemp > 0 && iTemp < iLowest)
		{
			iLowest = iTemp;
		}
	}

	return (iLowest != MAX_INT) ? iLowest : 0;
}

int CvSelectionGroup::getMinimumRBombardRange() const
{
	PROFILE_EXTRA_FUNC();
	int iLowest = MAX_INT;
	foreach_(const CvUnit* pLoopUnit, units())
	{
		const int iTemp = pLoopUnit->rBombardDamageLimit();
		if (iTemp > 0 && iTemp < iLowest)
		{
			iLowest = pLoopUnit->getDCMBombRange();
		}
	}

	return (iLowest != MAX_INT) ? iLowest : 0;
}

int CvSelectionGroup::getRBombardDamageMaxUnits() const
{
	PROFILE_EXTRA_FUNC();
	int iHighest = 0;
	foreach_(const CvUnit* pLoopUnit, units())
	{
		if (pLoopUnit->rBombardDamageLimit() > iHighest)
		{
			iHighest = pLoopUnit->rBombardDamageMaxUnits();
		}
	}

	return iHighest;
}

//
// Approximate how many turns this group would take to reduce pCity's defense modifier to zero
//
int CvSelectionGroup::getBombardTurns(const CvCity* pCity) const
{
	PROFILE_FUNC();

	const bool bHasBomber = (getOwner() != NO_PLAYER ? (GET_PLAYER(getOwner()).AI_calculateTotalBombard(DOMAIN_AIR) > 0) : false);
	bool bIgnoreBuildingDefense = bHasBomber;
	int iTotalBombardRate = (bHasBomber ? 16 : 0);

	foreach_(const CvUnit* pLoopUnit, units())
	{
		int iUnitBombardRate = pLoopUnit->getBombardRate();
		if (iUnitBombardRate > 0)
		{
			if( pLoopUnit->ignoreBuildingDefense() )
			{
				bIgnoreBuildingDefense = true;
			}
			else
			{
				iUnitBombardRate *= std::max(25, (100 - pCity->getBuildingBombardDefense()));
				iUnitBombardRate /= 100;
			}

			iTotalBombardRate += iUnitBombardRate;
		}
	}

	if( pCity->getTotalDefense(bIgnoreBuildingDefense) == 0 )
	{
		return 0;
	}

	int iBombardTurns = pCity->getTotalDefense(bIgnoreBuildingDefense);

	if( iTotalBombardRate > 0 )
	{
		iBombardTurns = (GC.getMAX_CITY_DEFENSE_DAMAGE() - pCity->getDefenseDamage());
		iBombardTurns *= pCity->getTotalDefense(false);
		iBombardTurns += (GC.getMAX_CITY_DEFENSE_DAMAGE() * iTotalBombardRate) - 1;
		iBombardTurns /= std::max(1, (GC.getMAX_CITY_DEFENSE_DAMAGE() * iTotalBombardRate));
	}

	//if( gUnitLogLevel > 2 ) logBBAI("      Bombard of %S will take %d turns at rate %d and current damage %d with bombard def %d", pCity->getName().GetCString(), iBombardTurns, iTotalBombardRate, pCity->getDefenseDamage(), (bIgnoreBuildingDefense ? 0 : pCity->getBuildingBombardDefense()));

	return iBombardTurns;
}

bool CvSelectionGroup::isHasPathToAreaPlayerCity(const PlayerTypes ePlayer, const int iFlags, const bool bGo)
{
	PROFILE_FUNC();

	CvReachablePlotSet plotSet(this, iFlags, MAX_INT, false);

	const CvUnit* headUnit = getHeadUnit();
	const CvPlot* plot = headUnit->plot();

	const bool bIgnoreDanger =
	(
		(iFlags & MOVE_IGNORE_DANGER) != 0
		||
		headUnit->canAttack()
		||
		headUnit->isBlendIntoCity()
	);
	foreach_(const CvCity* cityX, GET_PLAYER(ePlayer).cities())
	{
		if (cityX->area() == area() && plotSet.find(cityX->plot()) != plotSet.end())
		{
			if (generatePath(plot, cityX->plot(), iFlags, true))
			{
				if (bIgnoreDanger && bGo)
				{
					return pushMissionInternal(MISSION_MOVE_TO, cityX->getX(), cityX->getY(), iFlags);
				}
				return true;
			}
			// Toffer - This assert is not super accurate, it occur when there's a discrepancy between plotSet and generatePath,
			//	usually caused by incorrect flags used as input for this function,
			//	it does however indicate that plotSet and generatePath disagree about plot accessability under some specific flag combinations.
			FErrorMsg
			(
				CvString::format
				(
					"Pathing of player %d unit-group of %d (led by %S) from plot <%d, %d> to supposedly reachable city %S, owned by player %d at <%d, %d>, failed.",
					getOwner(), getNumUnits(), headUnit->getDescription().c_str(), getX(), getY(), cityX->getName().c_str(), ePlayer, cityX->getX(), cityX->getY()
				).c_str()
			);
		}
	}
	return false;
}

bool CvSelectionGroup::isHasPathToAreaEnemyCity(const int iFlags, const bool bGo)
{
	PROFILE_FUNC();

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.isAlive() && isPotentialEnemy(getTeam(), kLoopPlayer.getTeam())
		&& isHasPathToAreaPlayerCity((PlayerTypes)iI, iFlags, bGo))
		{
			return true;
		}
	}
	return false;
}

bool CvSelectionGroup::isStranded()
{
	PROFILE_FUNC();

	if (!m_bIsStrandedCacheValid)
	{
		m_bIsStrandedCache = calculateIsStranded();
		m_bIsStrandedCacheValid = true;
	}
	return m_bIsStrandedCache;
}

void CvSelectionGroup::invalidateIsStrandedCache()
{
	m_bIsStrandedCacheValid = false;
}

bool CvSelectionGroup::calculateIsStranded()
{
	PROFILE_FUNC();

	if (getNumUnits() <= 0)
	{
		return false;
	}

	if (plot() == NULL)
	{
		return false;
	}

	if (getDomainType() != DOMAIN_LAND)
	{
		return false;
	}

	if (getActivityType() != ACTIVITY_AWAKE && getActivityType() != ACTIVITY_HOLD)
	{
		return false;
	}

	if (AI_getMissionAIType() != NO_MISSIONAI)
	{
		return false;
	}

	if (getLengthMissionQueue() > 0)
	{
		return false;
	}

	if (!canAllMove())
	{
		return false;
	}

	const CvUnit* headUnit = getHeadUnit();
	if (headUnit->isCargo())
	{
		return false;
	}

	if (plot()->area()->getNumUnrevealedTiles(getTeam()) > 0
	&& (getHeadUnitAI() == UNITAI_ATTACK || getHeadUnitAI() == UNITAI_EXPLORE))
	{
		return false;
	}

	{
		int iBestValue;
		if (getHeadUnitAI() == UNITAI_SETTLE
		&& GET_PLAYER(getOwner()).AI_getNumAreaCitySites(getArea(), iBestValue) > 0)
		{
			return false;
		}

		if (plot()->area()->getCitiesPerPlayer(getOwner()) == 0
		&& plot()->area()->getNumAIUnits(getOwner(), UNITAI_SETTLE) > 0
		&& GET_PLAYER(getOwner()).AI_getNumAreaCitySites(getArea(), iBestValue) > 0)
		{
			return false;
		}
	}

	if (plot()->area()->getNumCities() > 0)
	{
		if (headUnit->AI_getUnitAIType() == UNITAI_SPY)
		{
			return false;
		}

		if (plot()->getImprovementType() != NO_IMPROVEMENT
		&& GC.getImprovementInfo(plot()->getImprovementType()).isActsAsCity()
		&& canDefend())
		{
			return false;
		}

		if (plot()->isCity() && plot()->getOwner() == getOwner())
		{
			return false;
		}

		if (headUnit->canAttack() || headUnit->isBlendIntoCity())
		{
			//	Since we are considering a reachable enemy city as not stranded it follows
			//	that we expect situations where we can only attack are not considered stranded,
			//	so include the MOVE_THROUGH_ENEMY flag for consistency
			if (isHasPathToAreaPlayerCity(getOwner(), MOVE_THROUGH_ENEMY))
			{
				return false;
			}

			if (isHasPathToAreaEnemyCity(MOVE_IGNORE_DANGER | MOVE_THROUGH_ENEMY))
			{
				return false;
			}
		}
		else if (isHasPathToAreaPlayerCity(getOwner(), MOVE_IGNORE_DANGER | MOVE_NO_ENEMY_TERRITORY))
		{
			return false;
		}
	}
	return true;
}

bool CvSelectionGroup::canMoveAllTerrain() const
{
	return algo::all_of(units(), CvUnit::fn::canMoveAllTerrain());
}

void CvSelectionGroup::unloadAll()
{
	algo::for_each(units(), CvUnit::fn::unloadAll());
}

bool CvSelectionGroup::alwaysInvisible() const
{
	return algo::all_of(units(), CvUnit::fn::alwaysInvisible());
}


bool CvSelectionGroup::isInvisible(TeamTypes eTeam) const
{
	return algo::all_of(units(), CvUnit::fn::isInvisible(eTeam, false));
}


int CvSelectionGroup::countNumUnitAIType(UnitAITypes eUnitAI) const
{
	return NO_UNITAI == eUnitAI ? getNumUnits() : algo::count_if(units(), CvUnit::fn::AI_getUnitAIType() == eUnitAI);
}


bool CvSelectionGroup::hasWorker() const
{
	return ((countNumUnitAIType(UNITAI_WORKER) > 0) || (countNumUnitAIType(UNITAI_WORKER_SEA) > 0));
}


bool CvSelectionGroup::IsSelected() const
{
	return algo::any_of(units(), CvUnit::fn::IsSelected());
}


void CvSelectionGroup::NotifyEntity(MissionTypes eMission)
{
	algo::for_each(units(), CvUnit::fn::NotifyEntity(eMission));
}


void CvSelectionGroup::airCircle(bool bStart)
{
	algo::for_each(units(), CvUnit::fn::airCircle(bStart));
}


void CvSelectionGroup::setBlockading(bool bStart)
{
	algo::for_each(units(), CvUnit::fn::setBlockading(bStart));
}


int CvSelectionGroup::getX() const
{
	const CvUnit* pHeadUnit = getHeadUnit();
	return pHeadUnit ? pHeadUnit->getX() : INVALID_PLOT_COORD;
}


int CvSelectionGroup::getY() const
{
	const CvUnit* pHeadUnit = getHeadUnit();
	return pHeadUnit ? pHeadUnit->getY() : INVALID_PLOT_COORD;
}


bool CvSelectionGroup::at(int iX, int iY) const
{
	return getX() == iX && getY() == iY;
}


bool CvSelectionGroup::atPlot(const CvPlot* pPlot) const
{
	return plot() == pPlot;
}


CvPlot* CvSelectionGroup::plot() const
{
	const CvUnit* pHeadUnit = getHeadUnit();
	return pHeadUnit ? pHeadUnit->plot() : NULL;
}


/*DllExport*/ CvPlot* CvSelectionGroup::plotExternal() const
{
#ifdef _DEBUG
	OutputDebugString("exe is asking for the plot of this group\n");
#endif
	CvPlot* pResult = plot();

	FAssert(!pResult || pResult->isInViewport());

	return pResult;
}


int CvSelectionGroup::getArea() const
{
	const CvUnit* pHeadUnit = getHeadUnit();
	return pHeadUnit ? pHeadUnit->getArea() : NULL;
}

CvArea* CvSelectionGroup::area() const
{
	const CvUnit* pHeadUnit = getHeadUnit();
	return pHeadUnit ? pHeadUnit->area() : NULL;
}


DomainTypes CvSelectionGroup::getDomainType() const
{
	const CvUnit* pHeadUnit = getHeadUnit();
	return pHeadUnit ? pHeadUnit->getDomainType() : NO_DOMAIN;
}


RouteTypes CvSelectionGroup::getBestBuildRoute(const CvPlot* pPlot, BuildTypes* peBestBuild) const
{
	PROFILE_FUNC();

	if (peBestBuild != NULL)
	{
		*peBestBuild = NO_BUILD;
	}

	int iBestValue = 0;
	RouteTypes eBestRoute = NO_ROUTE;

	// This really should not be arbitrarily duplicated code!
	// Somehow merge with CvPlayer::getBestRoute! Use getBestRouteInternal, save value, for value comp between units.
	// This is actually what stumped Blaze from getting non-unique value routes working; don't want to copy-paste code...
	foreach_(const CvUnit* pLoopUnit, units())
	{
		for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
		{
			const RouteTypes eRoute = (RouteTypes)GC.getBuildInfo((BuildTypes)iI).getRoute();
			if (eRoute != NO_ROUTE && pLoopUnit->canBuild(pPlot, (BuildTypes)iI))
			{
				const int iValue = GC.getRouteInfo(eRoute).getValue();
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestRoute = eRoute;
					if (peBestBuild != NULL)
					{
						*peBestBuild = ((BuildTypes)iI);
					}
				}
			}
		}
	}
	return eBestRoute;
}

bool CvSelectionGroup::canIgnoreZoneofControl() const
{
	FAssert(getNumUnits() > 0);

	return algo::all_of(units(), CvUnit::fn::canIgnoreZoneofControl());
}

// Returns true if group was bumped...
bool CvSelectionGroup::groupDeclareWar(const CvPlot* pPlot, bool bForce)
{
	if (!AI_isDeclareWar(pPlot))
	{
		return false;
	}
	TeamTypes ePlotTeam = pPlot->getTeam();
	const int iNumUnits = getNumUnits();

	if (bForce || !canEnterArea(ePlotTeam, pPlot->area(), true))
	{
		CvTeamAI& kTeam = GET_TEAM(getTeam());
		if (ePlotTeam != NO_TEAM
		&& kTeam.AI_isSneakAttackReady(ePlotTeam)
		&& kTeam.canDeclareWar(ePlotTeam))
		{
			kTeam.declareWar(ePlotTeam, true, NO_WARPLAN);
		}
	}
	return iNumUnits != getNumUnits();
}

// Returns true if attack was made...
bool CvSelectionGroup::groupAttack(int iX, int iY, int iFlags, bool& bFailedAlreadyFighting)
{
	PROFILE_FUNC();

	bFailedAlreadyFighting = false;
	CvPlot* pDestPlot = GC.getMap().plot(iX, iY);

	// Can attack on the same plot
	if (plot() != pDestPlot && (iFlags & MOVE_THROUGH_ENEMY) && generatePath(plot(), pDestPlot, iFlags))
	{
		pDestPlot = getPathFirstPlot();
	}
	FAssertMsg(pDestPlot, "DestPlot is not assigned a valid value");

	// CvSelectionGroup has a valid plot, but units don't always
	if (getNumUnits() < 1
	|| getDomainType() != DOMAIN_AIR && stepDistance(getX(), getY(), pDestPlot->getX(), pDestPlot->getY()) > 1
	|| !(iFlags & MOVE_DIRECT_ATTACK) && getDomainType() != DOMAIN_AIR && !(iFlags & MOVE_THROUGH_ENEMY) && (!generatePath(plot(), pDestPlot, iFlags) || getPathFirstPlot() != pDestPlot))
	{
		return false;
	}

	const bool bHuman = isHuman();

	if (bHuman && !isLastPathPlotVisible() && getDomainType() != DOMAIN_AIR)
	{
		return false;
	}

	bool bStealthDefense = false;
	int iAttackOdds = 0;
	CvUnit* pBestAttackUnit = AI_getBestGroupAttacker(pDestPlot, true, iAttackOdds, bStealthDefense, NULL, false, bStealthDefense);

	if (!pBestAttackUnit) return false;

	bool bAffixFirstAttacker = false;

	if (!pDestPlot->hasDefender(false, NO_PLAYER, getOwner(), pBestAttackUnit, true, false, false, true))
	{
		// Reveals the unit if true
		if (pDestPlot->hasStealthDefender(pBestAttackUnit, true))
		{
			bStealthDefense = true;
			bAffixFirstAttacker = true;
		}
	}

	if (groupDeclareWar(pDestPlot))
	{
		return true;
	}
	CvUnit* pBestDefender = pDestPlot->getBestDefender(NO_PLAYER, getOwner(), pBestAttackUnit, true, false, false, false, bStealthDefense);

	if (!pBestDefender) return false;

	const bool bStack = bHuman && (getDomainType() == DOMAIN_AIR || GET_PLAYER(getOwner()).isOption(PLAYEROPTION_STACK_ATTACK));

	std::set<int> alreadyAttacked;
	bool bAttack = false;
	bool bBombardExhausted = false;
	bool bLoopStealthDefense = false;
	bool bAffixFirstDefender = true;
	while (true)
	{
		PROFILE("CvSelectionGroup::groupAttack.StackLoop");

		if (bLoopStealthDefense) bStealthDefense = false;
		else if (bStealthDefense) bLoopStealthDefense = true;

		if (!bAffixFirstAttacker)
		{
			pBestAttackUnit = AI_getBestGroupAttacker(pDestPlot, false, iAttackOdds, bLoopStealthDefense, NULL, false, bLoopStealthDefense, false, alreadyAttacked);

			if (!pBestAttackUnit) break;
		}

		if (!bAffixFirstAttacker
		&& !pDestPlot->hasDefender(false, NO_PLAYER, getOwner(), pBestAttackUnit, true, false, false, true))
		{
			// Reveals the unit if true
			if (pDestPlot->hasStealthDefender(pBestAttackUnit, true))
			{
				bLoopStealthDefense = true;
			}
		}
		if (!bAffixFirstDefender)
		{
			pBestDefender = pDestPlot->getBestDefender(NO_PLAYER, getOwner(), pBestAttackUnit, true, false, false, false, true);
		}
		// if there are no defenders, do not attack
		if (!pBestDefender) break;

		bAffixFirstAttacker = false;
		bAffixFirstDefender = false;

		if (iAttackOdds < 68 && !bHuman && !bLoopStealthDefense)
		{
			if (bBombardExhausted)
			{
				CvUnit* pBestSacrifice = AI_getBestGroupSacrifice(pDestPlot, false, bHuman);
				if (pBestSacrifice && pBestSacrifice->canEnterPlot(pDestPlot, MoveCheck::Attack))
				{
					pBestAttackUnit = pBestSacrifice;
				}
			}
			else
			{
				bool bFoundBombard = false;

				OutputDebugString("Attempting to bombard tough plot\n");
				foreach_(CvUnit* pLoopUnit, units())
				{
					if (pLoopUnit->canBombardAtRanged(plot(), pDestPlot->getX(), pDestPlot->getY()))
					{
						bFoundBombard = true;
						pLoopUnit->bombardRanged(pDestPlot->getX(), pDestPlot->getY(), false);
					}
				}
				bBombardExhausted = !bFoundBombard;
				continue;
			}
		}
		// Toffer - Human player expect units with blitz to only attack once in a stack attack
		// if not the player has no control of the situation as e.g.
		//	2 units are selected, if one of the units have 4 moves and blitz it might completly suicide
		//	with 4 attacks against an enemy stack; AI would stop to reconsider depending on the outcome of each attack,
		//	the human player would expect an even expenditure of movement points in the selected group for one attack command.
		if (bHuman || !pBestAttackUnit->isBlitz())
		{
			FAssert(alreadyAttacked.find(pBestAttackUnit->getID()) == alreadyAttacked.end());
			alreadyAttacked.insert(pBestAttackUnit->getID());
		}
		bAttack = true;

		if (getNumUnits() < 2 || (bHuman && !bStack))
		{
			pBestAttackUnit->attack(pDestPlot, bLoopStealthDefense);
			break;
		}

		if (!bStack && (pBestAttackUnit->plot()->isBattle() || pDestPlot->isBattle()))
		{
			bFailedAlreadyFighting = true;

			// If this is AI stack, follow through with the attack to the end.
			// Allow Automated Units to Stack Attack
			if (!bHuman || isAutomated())
			{
				AI_queueGroupAttack(iX, iY);
			}
			break;
		}
		else pBestAttackUnit->attack(pDestPlot, bLoopStealthDefense);
	}
	return bAttack;
}


void CvSelectionGroup::groupMove(CvPlot* pPlot, bool bCombat, CvUnit* pCombatUnit, bool bEndMove)
{
	PROFILE_FUNC();

	// Inhibit recalculation of the appropriate center unit to display on the start
	// and end plots until after all units have moved, so it only has to be done once.
	CvPlot* pStartPlot = plot();

	pStartPlot->enableCenterUnitRecalc(false);
	pPlot->enableCenterUnitRecalc(false);

	const int iX = pPlot->getX();
	const int iY = pPlot->getY();

	m_bIsMidMove = true;

	foreach_(CvUnit* unitX, units_safe())
	{
		if (unitX->at(iX,iY))
		{
			continue;
		}

		//TBNote: Need to make this an option perhaps.
		//	Groups probably shouldn't be automatically splitting up to continue the planned move, particularly for human players.
		//	Would check if the whole group can move into the plot first. This warrants more study before acting on this.
		if (unitX == pCombatUnit
		|| unitX->canMove()
#ifdef _MOD_SENTRY
		&& (
				!isHuman()
				||
				!headMissionQueueNode()
				||
				headMissionQueueNode()->m_data.eMissionType != MISSION_MOVE_TO_SENTRY
				||
				!sentryAlertSameDomainType()
			)
#endif
		&& (bCombat ? unitX->canEnterOrAttackPlot(pPlot) : unitX->canEnterPlot(pPlot)))
		{
			unitX->move(pPlot, true);
		}
		else
		{
			unitX->joinGroup(NULL, true);
			unitX->ExecuteMove(((float)(GC.getMissionInfo(MISSION_MOVE_TO).getTime() * gDLL->getMillisecsPerTurn())) / 1000.0f, false);

			// Afforess - Units Seem to be getting stuck here
			if (GC.iStuckUnitID != unitX->getID())
			{
				GC.iStuckUnitID = unitX->getID();
				GC.iStuckUnitCount = 0;
			}
			else
			{
				GC.iStuckUnitCount++;
				if (GC.iStuckUnitCount > 5)
				{
					FErrorMsg("Unit Stuck in Loop!");
					const CvUnit* pHeadUnit = getHeadUnit();
					if (NULL != pHeadUnit)
					{
						char szOut[1024];
						CvWString szTempString;
						getUnitAIString(szTempString, pHeadUnit->AI_getUnitAIType());
						sprintf(szOut, "Unit stuck in loop: %S(%S)[%d, %d] (%S)\n", pHeadUnit->getName().GetCString(), GET_PLAYER(pHeadUnit->getOwner()).getName(),
							pHeadUnit->getX(), pHeadUnit->getY(), szTempString.GetCString());
						gDLL->messageControlLog(szOut);
					}
					unitX->finishMoves();
				}
			}
		}
	}
	pStartPlot->enableCenterUnitRecalc(true);
	pPlot->enableCenterUnitRecalc(true);

	//execute move
	if (bEndMove || !canAllMove())
	{
		foreach_(CvUnit* unitX, units())
		{
			FAssertDeclareScope(CvSelectionGroup_CvUnit_LOOP);
			unitX->ExecuteMove(((float)(GC.getMissionInfo(MISSION_MOVE_TO).getTime() * gDLL->getMillisecsPerTurn())) / 1000.0f, false);
		}
	}

	if (bCombat ? !canAnyMove() : !canAllMove())
	{
		GC.getGame().updateSelectionListInternal(getBugOptionINT("MainInterface__CycleDelayMove", 2));
	}
	m_bIsMidMove = false;
}


// Returns true if move was made...
bool CvSelectionGroup::groupPathTo(int iX, int iY, int iFlags)
{
	PROFILE_FUNC();

	CvPlot* pPathPlot;

	if (at(iX, iY))
	{
		return false; // XXX is this necessary?
	}

	FAssert(!isBusy());
	FAssert(getOwner() != NO_PLAYER);
	FAssert(headMissionQueueNode());

	CvPlot* pDestPlot = GC.getMap().plot(iX, iY);
	FAssertMsg(pDestPlot, "DestPlot is not assigned a valid value");

	FAssertMsg(canAllMove(), "canAllMove is expected to be true");

	if (getDomainType() == DOMAIN_AIR)
	{
		if (!canEnterOrAttackPlot(pDestPlot))
		{
			return false;
		}
		pPathPlot = pDestPlot;
	}
	else
	{
		if (!generatePath(plot(), pDestPlot, iFlags))
		{
			return false;
		}

		pPathPlot = getPathFirstPlot();

		//	If the first plot IS the destination it's possible that generatePath
		//	can succeed, but the group cannot actually move there because the move
		//	would require an attack
		if (!canEnterPlot(pPathPlot))
		{
			return false;
		}

		if (groupAmphibMove(pPathPlot, iFlags))
		{
			return false;
		}
	}

	bool bForce = false;
	const MissionAITypes eMissionAI = AI_getMissionAIType();

	/*** Dexy - Fixed Borders START ****/
	if (eMissionAI == MISSIONAI_BLOCKADE || eMissionAI == MISSIONAI_PILLAGE || eMissionAI == MISSIONAI_CLAIM_TERRITORY)
//	OLD CODE
//	if (eMissionAI == MISSIONAI_BLOCKADE || eMissionAI == MISSIONAI_PILLAGE)
	/*** Dexy - Fixed Borders  END  ****/
	{
		bForce = true;
	}

	if (groupDeclareWar(pPathPlot, bForce))
	{
		return false;
	}

	const bool bEndMove = (pPathPlot == pDestPlot);

	groupMove(pPathPlot, iFlags & MOVE_THROUGH_ENEMY, NULL, bEndMove);

	return true;
}


// Returns true if move was made...
bool CvSelectionGroup::groupRoadTo(int iX, int iY, int iFlags)
{
	if (!AI_isControlled() || !at(iX, iY) || (getLengthMissionQueue() == 1))
	{
		BuildTypes eBestBuild;
		getBestBuildRoute(plot(), &eBestBuild);

		if (eBestBuild != NO_BUILD)
		{
			groupBuild(eBestBuild);
			return true;
		}
	}

	return groupPathTo(iX, iY, iFlags);
}


// Returns true if build should continue...
bool CvSelectionGroup::groupBuild(BuildTypes eBuild)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_PLAYERS, getOwner());
	FASSERT_BOUNDS(0, GC.getNumBuildInfos(), eBuild);

	const CvPlot* pPlot = plot();

	const CvBuildInfo& kBuildInfo = GC.getBuildInfo(eBuild);
	const ImprovementTypes eImprovement = kBuildInfo.getImprovement();

	if (eImprovement != NO_IMPROVEMENT)
	{
		if (AI_isControlled())
		{
			if (GET_PLAYER(getOwner()).isOption(PLAYEROPTION_SAFE_AUTOMATION))
			{
				if ((pPlot->getImprovementType() != NO_IMPROVEMENT) && (pPlot->getImprovementType() != GC.getIMPROVEMENT_CITY_RUINS()))
				{
					const BonusTypes eBonus = (BonusTypes)pPlot->getNonObsoleteBonusType(GET_PLAYER(getOwner()).getTeam());
					if ((eBonus == NO_BONUS) || !GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eBonus))
					{
						if (GC.getImprovementInfo(eImprovement).getImprovementPillage() != NO_IMPROVEMENT)
						{
							return false;
						}
					}
				}
			}
			/*
			if (AI_getMissionAIType() == MISSION_BUILD)
			{
				CvCity* pWorkingCity = pPlot->getWorkingCity();
				if ((pWorkingCity != NULL) && (AI_getMissionAIPlot() == pPlot))
				{
					if (pWorkingCity->AI_getBestBuild(pWorkingCity->getCityPlotIndex(pPlot)) != eBuild)
					{
						return false;
					}
				}
			}
			*/
		}
	}

	bool bContinue = false;
	bool bCheckChop = false;
	bool bStopOtherWorkers = false;

	const FeatureTypes eFeature = pPlot->getFeatureType();

	if (eFeature != NO_FEATURE && isHuman() && kBuildInfo.isFeatureRemove(eFeature) && kBuildInfo.getFeatureProduction(eFeature) != 0)
	{
		if (eImprovement == NO_IMPROVEMENT)
		{
			// clearing a forest or jungle
			if (getBugOptionBOOL("Actions__PreChopForests", true, "BUG_PRECHOP_FORESTS"))
			{
				bCheckChop = true;
			}
		}
		else if (getBugOptionBOOL("Actions__PreChopImprovements", true, "BUG_PRECHOP_IMPROVEMENTS"))
		{
			bCheckChop = true;
		}
	}

	foreach_(CvUnit* unitX, units())
	{
		FAssertMsg(unitX->atPlot(pPlot), "unitX is expected to be at pPlot");

		if (unitX->canBuild(pPlot, eBuild))
		{
			bContinue = true;

			if (unitX->build(eBuild))
			{
				bContinue = false;
				break;
			}

			if (bCheckChop && pPlot->getBuildTurnsLeft(eBuild, getOwner()) == 1)
			{
				// TODO: stop other worker groups
				CvCity* pCity;
				const int iProduction = plot()->getFeatureProduction(eBuild, getTeam(), &pCity);

				if (iProduction > 0)
				{
					AddDLLMessage(
						getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText(
							"TXT_KEY_BUG_PRECLEARING_FEATURE_BONUS",
							GC.getFeatureInfo(eFeature).getTextKeyWide(), iProduction, pCity->getNameKey()
						),
						ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), MESSAGE_TYPE_INFO,
						GC.getFeatureInfo(eFeature).getButton(), GC.getCOLOR_WHITE(), getX(), getY(), true, true
					);
				}
				bContinue = false;
				bStopOtherWorkers = true;
				break;
			}
		}
	}

	if (bStopOtherWorkers)
	{
		foreach_(const CvUnit* unitX, pPlot->units())
		{
			CvSelectionGroup* groupX = unitX->getGroup();

			if (groupX != NULL
			&&  groupX != this
			&&  groupX->getOwner() == getOwner()
			&&  groupX->getActivityType() == ACTIVITY_MISSION
			&&  groupX->getLengthMissionQueue() > 0
			&&  groupX->getMissionType(0) == kBuildInfo.getMissionType()
			&&  groupX->getMissionData1(0) == eBuild)
			{
				groupX->deleteMissionQueueNode(groupX->headMissionQueueNode());
			}
		}
	}
	return bContinue;
}


void CvSelectionGroup::setTransportUnit(CvUnit* pTransportUnit, CvSelectionGroup** pOtherGroup)
{
	PROFILE_EXTRA_FUNC();
	// if we are loading
	if (pTransportUnit != NULL)
	{
		CvUnit* pHeadUnit = getHeadUnit();
		if (pHeadUnit == NULL)
		{
			FErrorMsg("non-zero group without head unit");
			return;
		}

		const int iCargoSpaceAvailable = pTransportUnit->cargoSpaceAvailable(pHeadUnit->getSpecialUnitType(), pHeadUnit->getDomainType());

		// if no space at all, give up
		if (iCargoSpaceAvailable < 1)
		{
			return;
		}

		// if there is space, but not enough to fit whole group, then split us, and set on the new group
		//Definitely only applicable in mechanism to non-sm
		if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
		{
			if (iCargoSpaceAvailable < getNumUnits())
			{
				CvSelectionGroup* pSplitGroup = splitGroup(iCargoSpaceAvailable, NULL, pOtherGroup);
				if (pSplitGroup != NULL)
				{
					pSplitGroup->setTransportUnit(pTransportUnit);
				}
				return;
			}

			FAssertMsg(iCargoSpaceAvailable >= getNumUnits(), "cargo size too small");
		}
		else if (iCargoSpaceAvailable < getNumUnitCargoVolumeTotal()
			/*pTransportUnit->cargoSpaceAvailable(pHeadUnit->getSpecialUnitType(), pHeadUnit->getDomainType())*/
			)
		{
			// If we can't fit the whole group but we can fit the first unit then split it off and load it on its own.
			if (getNumUnits() > 1 && iCargoSpaceAvailable >= getHeadUnit()->SMCargoVolume())
			{
				CvSelectionGroup* pSplitGroup = splitGroup(1, NULL, pOtherGroup);
				FAssertMsg(pSplitGroup, "Split failed");
				pSplitGroup->setTransportUnit(pTransportUnit);
			}
			// If the head unit can be split then try it
			else if (getHeadUnit()->canSplit())
			{
				// TODO: https://github.com/caveman2cosmos/Caveman2Cosmos/issues/329 (we need to calculate if splitting unit will work first, only do it if it will)
				getHeadUnit()->doSplit();
				// Recurse and try again
				setTransportUnit(pTransportUnit);
			}
			// We can't load anything
			else
			{
				return;
			}
		}

		// setTransportUnit removes the unit from the current group so we copy the unit list from the group first (group being modified while being iterated can cause problems).
		std::vector<CvUnit*> units(beginUnits(), endUnits());
		foreach_(CvUnit* pLoopUnit, units)
		{
			// just in case implementation of setTransportUnit changes, check to make sure this unit is not already loaded
			FAssertMsg(pLoopUnit->getTransportUnit() != pTransportUnit, "Unit is already changed");

			// if there is room, load the unit
			if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
			{
				if (pTransportUnit->cargoSpaceAvailable(pLoopUnit->getSpecialUnitType(), pLoopUnit->getDomainType()) >= pLoopUnit->SMCargoVolume())
				{
					pLoopUnit->setTransportUnit(pTransportUnit);
				}
				// We should continue on the loop because another unit might be able to fit
				// todo: Should we perhaps consider all unit volumes before we start the loop? Perhaps do a packing algorithm to get the most in?
			}
			else if (pTransportUnit->cargoSpaceAvailable(pLoopUnit->getSpecialUnitType(), pLoopUnit->getDomainType()) > 0)
			{
				pLoopUnit->setTransportUnit(pTransportUnit);
			}
			else
			{
				// If there is no room and we aren't using size matters then we may aswell abort the rest of the loop as no more units will fit
				break;
			}
		}
	}
	// otherwise we are unloading
	else
	{
		// loop over all the units, unloading them
		foreach_(CvUnit* unit, units_safe())
		{
			// unload unit
			unit->setTransportUnit(NULL);
		}
	}
}


/// \brief Function for loading stranded units onto an offshore transport
///
void CvSelectionGroup::setRemoteTransportUnit(CvUnit* pTransportUnit)
{
	PROFILE_EXTRA_FUNC();
	// if we are loading
	if (pTransportUnit != NULL)
	{
		const CvUnit* pHeadUnit = getHeadUnit();
		if (pHeadUnit == NULL)
		{
			return;
		}
		FAssertMsg(pHeadUnit != NULL, "non-zero group without head unit");

		const int iCargoSpaceAvailable = pTransportUnit->cargoSpaceAvailable(pHeadUnit->getSpecialUnitType(), pHeadUnit->getDomainType());

		// if no space at all, give up
		if (iCargoSpaceAvailable < 1)
		{
			return;
		}

		// if there is space, but not enough to fit whole group, then split us, and set on the new group
		if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
		{
			if (iCargoSpaceAvailable < getNumUnitCargoVolumeTotal())
			{
				CvSelectionGroup* pSplitGroup = splitGroup(1);
				if (pSplitGroup != NULL)
				{
					if (iCargoSpaceAvailable > pSplitGroup->getNumUnitCargoVolumeTotal())
					{
						pSplitGroup->setRemoteTransportUnit(pTransportUnit);
					}
					else if (pSplitGroup->getHeadUnit()->canSplit())
					{
						pSplitGroup->getHeadUnit()->doSplit();
						setRemoteTransportUnit(pTransportUnit);
					}
					//I may be really needing to develop out a lot more to splitting and merging to help transfer ai data...
					//Maybe simply at the beginning of a split or merge, record the mission of the original group to which the unit was assigned then transfer that data to the new unit(s) after the merge/split...
				}
				return;
			}
		}
		else if (iCargoSpaceAvailable < getNumUnits())
		{
			CvSelectionGroup* pSplitGroup = splitGroup(iCargoSpaceAvailable);
			if (pSplitGroup != NULL)
			{
				pSplitGroup->setRemoteTransportUnit(pTransportUnit);
			}
			return;
		}

		FAssertMsg(iCargoSpaceAvailable >= getNumUnits(), "cargo size too small");

		bool bLoadedOne;
		do
		{
			bLoadedOne = false;

			// loop over all the units on the plot, looping through this selection group did not work
			foreach_(CvUnit* pLoopUnit, units())
			{
				if (pLoopUnit->getTransportUnit() != pTransportUnit && pLoopUnit->getOwner() == pTransportUnit->getOwner())
				{
					bool bSpaceAvailable = false;
					if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
					{
						bSpaceAvailable = pTransportUnit->cargoSpaceAvailable(pLoopUnit->getSpecialUnitType(), pLoopUnit->getDomainType()) > pLoopUnit->SMCargoVolume();
					}
					else
					{
						bSpaceAvailable = pTransportUnit->cargoSpaceAvailable(pLoopUnit->getSpecialUnitType(), pLoopUnit->getDomainType()) > 0;
					}
					if (bSpaceAvailable)
					{
						if( !(pLoopUnit->atPlot(pTransportUnit->plot())) )
						{
							// Putting a land unit on water automatically loads it
							//GC.getGame().logOOSSpecial(12, pLoopUnit->getID(), pTransportUnit->getX(), pTransportUnit->getY());
							pLoopUnit->setXY(pTransportUnit->getX(),pTransportUnit->getY());
						}

						if( pLoopUnit->getTransportUnit() != pTransportUnit )
						{
							pLoopUnit->setTransportUnit(pTransportUnit);
						}

						bLoadedOne = true;
						break;
					}
					else if (getHeadUnit()->canSplit())
					{
						getHeadUnit()->doSplit();
						setRemoteTransportUnit(pTransportUnit);
					}
				}
			}
		}
		while (bLoadedOne);
	}
	else
	{
		// loop over all the units, unloading them
		algo::for_each(units(), CvUnit::fn::setTransportUnit(NULL));
	}
}

bool CvSelectionGroup::isAmphibPlot(const CvPlot* pPlot) const
{
	bool bFriendly = true;
	CvUnit* pUnit = getHeadUnit();
	if (NULL != pUnit)
	{
		bFriendly = pPlot->isFriendlyCity(*pUnit, true);
	}

	//return ((getDomainType() == DOMAIN_SEA) && pPlot->isCoastalLand() && !bFriendly && !canMoveAllTerrain());

	if (getDomainType() == DOMAIN_SEA)
	{
		if (pPlot->isCity() && !bFriendly && (pPlot->isCoastalLand() || pPlot->isWater() || canMoveAllTerrain()))
		{
			return true;
		}
		return (pPlot->isCoastalLand() && !bFriendly && !canMoveAllTerrain());
	}
	return false;
}

// Returns true if attempted an amphib landing...
bool CvSelectionGroup::groupAmphibMove(CvPlot* pPlot, int iFlags)
{
	PROFILE_EXTRA_FUNC();
	FAssert(getOwner() != NO_PLAYER);

	if (groupDeclareWar(pPlot))
	{
		return true;
	}

	if (!isAmphibPlot(pPlot))
	{
		return false;
	}

// BUG - Safe Move - start
	// don't perform amphibious landing on plot that was unrevealed when goto order was issued
	if (isHuman() && !isLastPathPlotRevealed())
	{
		return false;
	}
// BUG - Safe Move - end

	if (stepDistance(getX(), getY(), pPlot->getX(), pPlot->getY()) != 1)
	{
		return false;
	}

	bool bLanding = false;

	// BBAI TODO: Bombard with warships if invading
	foreach_(const CvUnit* unit, units())
	{
		if (!unit->hasCargo() || unit->getDomainCargo() != DOMAIN_LAND)
		{
			continue;
		}

		std::vector<CvUnit*> aCargoUnits;
		unit->getCargoUnits(aCargoUnits);

		std::vector<CvSelectionGroup*> aCargoGroups;
		foreach_(const CvUnit* pCargoUnit, aCargoUnits)
		{
			if (algo::none_of_equal(aCargoGroups, pCargoUnit->getGroup()))
			{
				aCargoGroups.push_back(pCargoUnit->getGroup());
			}
		}

		foreach_(CvSelectionGroup* pGroup, aCargoGroups)
		{
			if (pGroup->canAllMove())
			{
				FAssert(!pGroup->at(pPlot->getX(), pPlot->getY()));
				bLanding = pGroup->pushMissionInternal(MISSION_MOVE_TO, pPlot->getX(), pPlot->getY(), (MOVE_IGNORE_DANGER | iFlags));
			}
		}
	}

	return bLanding;
}


bool CvSelectionGroup::readyToSelect(bool bAny)
{
	return (readyToMove(bAny) && !isAutomated());
}


bool CvSelectionGroup::readyToMove(bool bAny, bool bValidate)
{
	return ((bAny ? canAnyMove(bValidate) : canAllMove()) && (headMissionQueueNode() == NULL) && (getActivityType() == ACTIVITY_AWAKE) && !isBusy() && !isCargoBusy());
}


bool CvSelectionGroup::readyToAuto() const
{
	return (canAllMove() && (headMissionQueueNode() != NULL));
}


int CvSelectionGroup::getID() const
{
	return m_iID;
}


void CvSelectionGroup::setID(int iID)
{
	m_iID = iID;
}


TeamTypes CvSelectionGroup::getTeam() const
{
	return (getOwner() != NO_PLAYER) ? GET_PLAYER(getOwner()).getTeam() : NO_TEAM;
}


int CvSelectionGroup::getMissionTimer() const
{
	return m_iMissionTimer;
}


void CvSelectionGroup::setMissionTimer(int iNewValue)
{
	FAssert(getOwner() != NO_PLAYER);

	m_iMissionTimer = iNewValue;
	FASSERT_NOT_NEGATIVE(getMissionTimer());
}


void CvSelectionGroup::changeMissionTimer(int iChange)
{
	setMissionTimer(getMissionTimer() + iChange);
}


void CvSelectionGroup::updateMissionTimer(int iSteps)
{
	PROFILE_FUNC();

	int iTime = 0;
	CLLNode<MissionData>* missionNode = headMissionQueueNode();

	if (missionNode && (isHuman() || showMoves()))
	{
		iTime = GC.getMissionInfo((MissionTypes)missionNode->m_data.eMissionType).getTime();

		if (isMoveMission(missionNode))
		{
			CvPlot* pTargetPlot = NULL;
			if (missionNode->m_data.eMissionType == MISSION_MOVE_TO_UNIT)
			{
				CvUnit* pTargetUnit = GET_PLAYER((PlayerTypes)missionNode->m_data.iData1).getUnit(missionNode->m_data.iData2);

				if (pTargetUnit)
				{
					pTargetPlot = pTargetUnit->plot();
				}
			}
			else
			{
				pTargetPlot = GC.getMap().plot(missionNode->m_data.iData1, missionNode->m_data.iData2);
			}

			if (atPlot(pTargetPlot))
			{
				iTime += iSteps;
			}
			else
			{
				iTime = std::min(iTime, 2);
			}
		}

		if (isHuman())
		{
			if (isAutomated() && getBugOptionBOOL("MainInterface__MinimizeAITurnSlices", false)
			|| GET_PLAYER((GC.getGame().isNetworkMultiPlayer()) ? getOwner() : GC.getGame().getActivePlayer()).isOption(PLAYEROPTION_QUICK_MOVES))
			{
				if (!GC.getGame().isGameMultiPlayer())
				{
					iTime = 0;
				}
				else iTime = std::min(iTime, 1);
			}
		}
		else if (!GC.getGame().isGameMultiPlayer() && getBugOptionBOOL("MainInterface__MinimizeAITurnSlices", false))
		{
			iTime = 0;
		}
	}
	setMissionTimer(iTime);
}


bool CvSelectionGroup::isForceUpdate() const
{
	return m_bForceUpdate;
}


void CvSelectionGroup::setForceUpdate(bool bNewValue)
{
	m_bForceUpdate = bNewValue;
}


ActivityTypes CvSelectionGroup::getActivityType() const
{
	return m_eActivityType;
}


void CvSelectionGroup::setActivityType(ActivityTypes eNewValue, MissionTypes eSleepType)
{
	PROFILE_EXTRA_FUNC();
	FAssert(getOwner() != NO_PLAYER);

	if (eNewValue == m_eActivityType)
	{
		return;
	}
	if (m_eActivityType == ACTIVITY_INTERCEPT)
	{
		airCircle(false);
	}
	setBlockading(false);

	m_eActivityType = eNewValue;

	if (eNewValue == ACTIVITY_INTERCEPT)
	{
		airCircle(true);
	}
	CvPlot* pPlot = plot();

	if (eNewValue != ACTIVITY_MISSION)
	{
		if (eNewValue != ACTIVITY_INTERCEPT)
		{
			//don't idle intercept animation
			foreach_(CvUnit* pLoopUnit, units())
			{
				pLoopUnit->NotifyEntity(MISSION_IDLE);
				if (pLoopUnit->isDead()) continue;

				// Determine proper Sleep type
				if (!isHuman() || eSleepType != NO_MISSION && (eNewValue == ACTIVITY_SLEEP || eNewValue == ACTIVITY_HEAL))
				{
					if (
						pLoopUnit->isBuildUpable()
					&&	(
								eSleepType == MISSION_BUILDUP
							||	eSleepType == MISSION_AUTO_BUILDUP
							||	eSleepType == MISSION_HEAL_BUILDUP
							||	eSleepType == NO_MISSION
						)
					) pLoopUnit->setBuildUpType(NO_PROMOTIONLINE, eSleepType);
					
					pLoopUnit->setSleepType(pLoopUnit->isFortifyable() ? MISSION_FORTIFY : MISSION_SLEEP);
				}
			}
		}

		if (getTeam() == GC.getGame().getActiveTeam() && pPlot != NULL)
		{
			pPlot->setFlagDirty(true);
		}
	}

	if (pPlot == gDLL->getInterfaceIFace()->getSelectionPlot())
	{
		gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
	}
}


AutomateTypes CvSelectionGroup::getAutomateType() const
{
	return m_eAutomateType;
}


bool CvSelectionGroup::isAutomated() const
{
	return (getAutomateType() != NO_AUTOMATE);
}


void CvSelectionGroup::setAutomateType(AutomateTypes eNewValue)
{
	PROFILE_EXTRA_FUNC();
	FAssert(getOwner() != NO_PLAYER);

	if (getAutomateType() != eNewValue)
	{
		m_eAutomateType = eNewValue;

		clearMissionQueue();
		setActivityType(ACTIVITY_AWAKE);

		// if canceling automation, cancel on cargo as well
		if (eNewValue == NO_AUTOMATE)
		{
			const CvPlot* pPlot = plot();
			if (pPlot != NULL)
			{
				foreach_(const CvUnit* pCargoUnit, pPlot->units())
				{
					const CvUnit* pTransportUnit = pCargoUnit->getTransportUnit();
					if (pTransportUnit != NULL && pTransportUnit->getGroup() == this)
					{
						pCargoUnit->getGroup()->setAutomateType(NO_AUTOMATE);
						pCargoUnit->getGroup()->setActivityType(ACTIVITY_AWAKE);
					}
				}
			}
		}
	}
}

#ifdef USE_OLD_PATH_GENERATOR
FAStarNode* CvSelectionGroup::getPathLastNode() const
{
	return gDLL->getFAStarIFace()->GetLastNode(&GC.getPathFinder());
}
#endif


CvPlot* CvSelectionGroup::getPathFirstPlot() const
{
	PROFILE_EXTRA_FUNC();
#ifdef USE_OLD_PATH_GENERATOR
	FAStarNode* pNode = getPathLastNode();

	if (pNode->m_pParent == NULL)
	{
		return GC.getMap().plotSorenINLINE(pNode->m_iX, pNode->m_iY);
	}

	while (pNode != NULL)
	{
		if (pNode->m_pParent->m_pParent == NULL)
		{
			return GC.getMap().plotSorenINLINE(pNode->m_iX, pNode->m_iY);
		}
		pNode = pNode->m_pParent;
	}

	FErrorMsg("error");

	return NULL;
#else
	CvPath::const_iterator itr = getPath().begin();

	//	CvPath stores the node the unit started on first, but the 'first plot' required is the
	//	first one moved to
	FAssertMsg(itr != getPath().end(), "getPathFirstPlot called without path being calculated (a valid path always contains the first plot at least)");
	if (itr != getPath().end() && ++itr != getPath().end() )
	{
		return itr.plot();
	}
	return NULL;
#endif
}

const CvPath& CvSelectionGroup::getPath() const
{
	return getPathGenerator()->getLastPath();
}

CvPlot* CvSelectionGroup::getPathEndTurnPlot() const
{
	PROFILE_EXTRA_FUNC();
#ifdef USE_OLD_PATH_GENERATOR
	FAStarNode* pNode = getPathLastNode();

	if (NULL != pNode)
	{
		if (pNode->m_pParent == NULL || pNode->m_iData2 == 1)
		{
			return GC.getMap().plotSorenINLINE(pNode->m_iX, pNode->m_iY);
		}

		while (pNode->m_pParent != NULL)
		{
			if (pNode->m_pParent->m_iData2 == 1)
			{
				return GC.getMap().plotSorenINLINE(pNode->m_pParent->m_iX, pNode->m_pParent->m_iY);
			}
			pNode = pNode->m_pParent;
		}
	}
	FErrorMsg("error");

	return NULL;
#else
	CvPath::const_iterator itr = getPath().begin();

	// CvPath stores the node the unit started on first, but the 'first plot' required is the first one moved to
	if (itr != getPath().end())
	{
		const int iStart = itr.turn();
		CvPlot*	pPlot = itr.plot();

		while (itr.turn() == iStart && itr != getPath().end())
		{
			pPlot = itr.plot();
			++itr;
		}
		FAssertMsg(
			pPlot != getPath().begin().plot() || pPlot == getPath().lastPlot(),
			CvString::format(
				"Bad path movement calc on path from (%d,%d) to (%d,%d) via (%d,%d) for %S (%d) at plot (%d,%d)\n",
				getPath().begin().plot()->getX(), getPath().begin().plot()->getY(),
				getPath().lastPlot()->getX(), getPath().lastPlot()->getY(),
				pPlot->getX(), pPlot->getY(),
				getHeadUnit()->getDescription().c_str(), getHeadUnit()->getID(),
				plot()->getX(), plot()->getY()
			).c_str()
		);
		return pPlot;
	}
	return NULL;
#endif
}

bool CvSelectionGroup::generatePath(const CvPlot* pFromPlot, const CvPlot* pToPlot, int iFlags, bool bReuse, int* piPathTurns, int iMaxPathLen, int iOptimizationLimit) const
{
	bool bSuccess;

	PROFILE("CvSelectionGroup::generatePath()");

#ifdef USE_OLD_PATH_GENERATOR

	if (pFromPlot == NULL || pToPlot == NULL)
	{
		return false;
	}

	gDLL->getFAStarIFace()->SetData(&GC.getPathFinder(), this);

	//	Note - we NEVER allow the pathing engine to cache  any more, since that reuses end-of-turn costs/validity
	//	in non-end-of-turn considerations of inter-tile moves.  This appears to always have been a bug, but with
	//	the new AI pathing it became more significant.  The callbacks we use in path generation now do much more
	//	extensive caching of their own, which works out about performance neutral
	switch(getHeadUnit()->getDomainType())
	{
		case DOMAIN_LAND:
		{
			PROFILE("generatePath.Land");
			bSuccess = gDLL->getFAStarIFace()->GeneratePath(&GC.getPathFinder(), pFromPlot->getX(), pFromPlot->getY(), pToPlot->getX(), pToPlot->getY(), false, iFlags, false);
			break;
		}
		case DOMAIN_AIR:
		{
			PROFILE("generatePath.Air");
			bSuccess = gDLL->getFAStarIFace()->GeneratePath(&GC.getPathFinder(), pFromPlot->getX(), pFromPlot->getY(), pToPlot->getX(), pToPlot->getY(), false, iFlags, false);
			break;
		}
		case DOMAIN_SEA:
		{
			PROFILE("generatePath.Sea");
			bSuccess = gDLL->getFAStarIFace()->GeneratePath(&GC.getPathFinder(), pFromPlot->getX(), pFromPlot->getY(), pToPlot->getX(), pToPlot->getY(), false, iFlags, bReuse);
			break;
		}
		default:
		{
			PROFILE("generatePath.Other");
			bSuccess = gDLL->getFAStarIFace()->GeneratePath(&GC.getPathFinder(), pFromPlot->getX(), pFromPlot->getY(), pToPlot->getX(), pToPlot->getY(), false, iFlags, bReuse);
			break;
		}
	}

	if (piPathTurns != NULL)
	{
		*piPathTurns = MAX_INT;

		if (bSuccess)
		{
			FAStarNode* pNode = getPathLastNode();

			if (pNode != NULL)
			{
				*piPathTurns = pNode->m_iData2;
			}
		}
	}
#else
	bSuccess = getPathGenerator()->generatePath(pFromPlot, pToPlot, (CvSelectionGroup*)this, iFlags, iMaxPathLen, iOptimizationLimit);

	if (piPathTurns != NULL)
	{
		*piPathTurns = MAX_INT;

		if (bSuccess)
		{
			*piPathTurns = getPathGenerator()->getLastPath().length();
		}
	}

#endif

	return bSuccess;
}

CvPathGenerator* CvSelectionGroup::getPathGenerator()
{
	return getCachedPathGenerator().get();
}

CvSelectionGroup::CachedPathGenerator::CachedPathGenerator(CvMap* map)
	: m_pCachedNonEndTurnEdgeCosts(256)
	, m_pCachedEndTurnEdgeCosts(32)
	, m_pathGenerator(map)
{
	m_pCachedNonEndTurnEdgeCosts.set_empty_key(-1);
	m_pCachedEndTurnEdgeCosts.set_empty_key(-1);
	m_pathGenerator.Initialize(NewPathHeuristicFunc, NewPathCostFunc, NewPathValidFunc, NewPathDestValid, NewPathTurnEndValidityCheckRequired);
}

void CvSelectionGroup::CachedPathGenerator::clear()
{
	m_pCachedNonEndTurnEdgeCosts.clear();
	m_pCachedEndTurnEdgeCosts.clear();
}

CvSelectionGroup::CachedPathGenerator& CvSelectionGroup::getCachedPathGenerator()
{
	if (!m_cachedPathGenerator)
	{
		m_cachedPathGenerator.reset(new CachedPathGenerator(&GC.getMap()));
	}
	return *m_cachedPathGenerator;
}

namespace {
	int get_path_cache_key(const CvPlot* pFromPlot, const CvPlot* pToPlot)
	{
		return GC.getMap().plotNum(pFromPlot->getX(), pFromPlot->getY()) + (GC.getMap().plotNum(pToPlot->getX(), pToPlot->getY()) << 16);
	}
}
bool CvSelectionGroup::CachedPathGenerator::HaveCachedPathEdgeCosts(const CvPlot* pFromPlot, const CvPlot* pToPlot, bool bIsEndTurnElement, int& iResult, int& iBestMoveCost, int& iWorstMoveCost, int& iToPlotNodeCost)
{
	//	Could use Zobrist hashes of the plots, but actually since we're only combining two sets of coordinates we can
	//	fit it all in an int for any reasonable map
	const int cacheKey = get_path_cache_key(pFromPlot, pToPlot);
	bool result;

	if ( bIsEndTurnElement )
	{
		CacheMapType::const_iterator itr = m_pCachedEndTurnEdgeCosts.find(cacheKey);

		if ( itr == m_pCachedEndTurnEdgeCosts.end() )
		{
			result = false;
		}
		else
		{
			iResult = (itr->second).iCost;
			iBestMoveCost = (itr->second).iBestMoveCost;
			iWorstMoveCost = (itr->second).iWorstMoveCost;
			iToPlotNodeCost = (itr->second).iToPlotNodeCost;
#ifdef _DEBUG
			FAssert((itr->second).pFromPlot == pFromPlot);
			FAssert((itr->second).pToPlot == pToPlot);
#endif
			result = true;
		}
	}
	else
	{
		const CacheMapType::const_iterator itr = m_pCachedNonEndTurnEdgeCosts.find(cacheKey);

		if ( itr == m_pCachedNonEndTurnEdgeCosts.end() )
		{
			result = false;
		}
		else
		{
			iResult = (itr->second).iCost;
			iBestMoveCost = (itr->second).iBestMoveCost;
			iWorstMoveCost = (itr->second).iWorstMoveCost;
			iToPlotNodeCost = (itr->second).iToPlotNodeCost;
#ifdef _DEBUG
			FAssert((itr->second).pFromPlot == pFromPlot);
			FAssert((itr->second).pToPlot == pToPlot);
#endif
			result = true;
		}
	}

	return result;
}

void CvSelectionGroup::CachedPathGenerator::CachePathEdgeCosts(const CvPlot* pFromPlot, const CvPlot* pToPlot, bool bIsEndTurnElement, int iCost, int iBestMoveCost, int iWorstMoveCost, int iToPlotNodeCost)
{
	//	Could use Zobrist hashes of the plots, but actually since we're only combining two sets of coordinates we can
	//	fit it all in an int for any reasonable map
	FAssert(GC.getMap().getGridHeight()*GC.getMap().getGridHeight()*GC.getMap().getGridWidth()*(GC.getMap().getGridWidth()/2) < MAX_INT);

	const int cacheKey = get_path_cache_key(pFromPlot, pToPlot);

	CachedEdgeCosts costs(iCost, iBestMoveCost, iWorstMoveCost, iToPlotNodeCost);
#ifdef _DEBUG
	costs.pFromPlot = const_cast<CvPlot*>(pFromPlot);
	costs.pToPlot = const_cast<CvPlot*>(pToPlot);
#endif

	if ( bIsEndTurnElement )
	{
		m_pCachedEndTurnEdgeCosts[cacheKey] = costs;
	}
	else
	{
		m_pCachedNonEndTurnEdgeCosts[cacheKey] = costs;
	}
}

void CvSelectionGroup::setGroupToCacheFor(CvSelectionGroup* group)
{
	if (m_pCachedMovementGroup != group)
	{
		m_pCachedMovementGroup = group;
		getCachedPathGenerator().clear();
	}
}

bool CvSelectionGroup::HaveCachedPathEdgeCosts(const CvPlot* pFromPlot, const CvPlot* pToPlot, bool bIsEndTurnElement, int& iResult, int& iBestMoveCost, int& iWorstMoveCost, int& iToPlotNodeCost) const
{
	if (m_pCachedMovementGroup != this)
	{
		return false;
	}
	return getCachedPathGenerator().HaveCachedPathEdgeCosts(pFromPlot, pToPlot, bIsEndTurnElement, iResult, iBestMoveCost, iWorstMoveCost, iToPlotNodeCost);
}

void CvSelectionGroup::CachePathEdgeCosts(const CvPlot* pFromPlot, const CvPlot* pToPlot, bool bIsEndTurnElement, int iCost, int iBestMoveCost, int iWorstMoveCost, int iToPlotNodeCost) const
{

	if (this == m_pCachedMovementGroup)
	{
		getCachedPathGenerator().CachePathEdgeCosts(pFromPlot, pToPlot, bIsEndTurnElement, iCost, iBestMoveCost, iWorstMoveCost, iToPlotNodeCost);
	}
}

void CvSelectionGroup::resetPath()
{
	gDLL->getFAStarIFace()->ForceReset(&GC.getPathFinder());
}

bool CvSelectionGroup::canPathDirectlyTo(const CvPlot* pFromPlot, const CvPlot* pToPlot) const
{
	return canPathDirectlyToInternal(pFromPlot, pToPlot, -1);
}

bool CvSelectionGroup::canPathDirectlyToInternal(const CvPlot* pFromPlot, const CvPlot* pToPlot, int movesRemaining) const
{
	PROFILE_FUNC();

	if ( movesRemaining == 0 )
	{
		return false;
	}

	//	Avoid path searching - just test paths that monotonically move towards the destination
	foreach_(CvPlot* pAdjacentPlot, pFromPlot->adjacent())
	{
		if ( stepDistance(pAdjacentPlot->getX(), pAdjacentPlot->getY(), pToPlot->getX(), pToPlot->getY()) <
			 stepDistance(pFromPlot->getX(), pFromPlot->getY(), pToPlot->getX(), pToPlot->getY()) )
		{
			if (canEnterPlot(pAdjacentPlot, (pAdjacentPlot == pToPlot)))
			{
				return pAdjacentPlot == pToPlot || canPathDirectlyToInternal(pAdjacentPlot, pToPlot, movesRemainingAfterMovingTo(movesRemaining, pFromPlot, pAdjacentPlot));
			}
		}
	}
	return false;
}

int CvSelectionGroup::movesRemainingAfterMovingTo(int iStartMoves, const CvPlot* pFromPlot, const CvPlot* pToPlot) const
{
	PROFILE_EXTRA_FUNC();
	int iResult = MAX_INT;

	foreach_(const CvUnit* pLoopUnit, units())
	{
		int iUnitMoves = (iStartMoves == -1 ? pLoopUnit->maxMoves() : iStartMoves);
		iUnitMoves -= pToPlot->movementCost(pLoopUnit, pFromPlot);
		iUnitMoves = std::max(iUnitMoves, 0);

		iResult = std::min(iResult, iUnitMoves);
	}

	return iResult;
}

int CvSelectionGroup::movesLeft() const
{
	PROFILE_EXTRA_FUNC();
	int iResult = MAX_INT;

	foreach_(const CvUnit* pLoopUnit, units())
	{
		iResult = std::min(iResult, pLoopUnit->movesLeft());
	}

	return iResult;
}


void CvSelectionGroup::clearUnits()
{
	PROFILE_EXTRA_FUNC();
	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pUnitNode = deleteUnitNode(pUnitNode);
	}
}

bool CvSelectionGroup::hasUnitOfAI(UnitAITypes eUnitAI) const
{
	return algo::any_of(units(), CvUnit::fn::AI_getUnitAIType() == eUnitAI);
}

int	CvSelectionGroup::getWorstDamagePercent(UnitCombatTypes eIgnoreUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	int iWorstDamage = 0;

	foreach_(const CvUnit* pLoopUnit, units())
	{
		if (eIgnoreUnitCombat == NO_UNITCOMBAT || !pLoopUnit->isHasUnitCombat(eIgnoreUnitCombat))
		{
			if (pLoopUnit->getDamage() > iWorstDamage)
			{
				iWorstDamage = pLoopUnit->getDamagePercent();
			}
		}
	}

	return iWorstDamage;
}

// Returns true if the unit is added...
bool CvSelectionGroup::addUnit(CvUnit* pUnit, bool bMinimalChange)
{
	PROFILE_EXTRA_FUNC();
	//PROFILE_FUNC();

	CvUnit* pOldHeadUnit = getHeadUnit();
	CvPlot* pPlot = pUnit->plot();

	if (pPlot != NULL && !pUnit->canJoinGroup(pPlot, this))
	{
		return false;
	}

#ifdef _DEBUG
	if (pPlot != NULL)
	{
		validateLocations();
	}
#endif

	bool bAdded = false;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);

		if (!bAdded && ((pUnit->AI_groupFirstVal() > pLoopUnit->AI_groupFirstVal()) ||
			  ((pUnit->AI_groupFirstVal() == pLoopUnit->AI_groupFirstVal()) &&
				 (pUnit->AI_groupSecondVal() > pLoopUnit->AI_groupSecondVal()))))
		{
			m_units.insertBefore(pUnit->getIDInfo(), pUnitNode);
			bAdded = true;
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}

	if (!bAdded)
	{
		m_units.insertAtEnd(pUnit->getIDInfo());
	}

	AI_noteSizeChange(1, pUnit->getCargoVolume());

	if (!bMinimalChange && getOwner() == NO_PLAYER && getNumUnits() > 0)
	{
		algo::for_each(units(), CvUnit::fn::NotifyEntity(MISSION_MULTI_SELECT));
	}

	if (pOldHeadUnit != getHeadUnit() && GC.getENABLE_DYNAMIC_UNIT_ENTITIES())
	{
		if (pOldHeadUnit)
		{
			pOldHeadUnit->reloadEntity();
		}
		getHeadUnit()->reloadEntity();
	}

#ifdef _DEBUG
	if (pPlot)
	{
		validateLocations();
	}
#endif
	return true;
}

bool CvSelectionGroup::containsUnit(const CvUnit* pUnit) const
{
	return algo::any_of_equal(units(), pUnit);
}

void CvSelectionGroup::removeUnit(CvUnit* pUnit)
{
	PROFILE_EXTRA_FUNC();
	CvUnit* pOldHeadUnit = getHeadUnit();

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode)
	{
		if (::getUnit(pUnitNode->m_data) == pUnit)
		{
			deleteUnitNode(pUnitNode);
			break;
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}

	if (pOldHeadUnit != getHeadUnit() && GC.getENABLE_DYNAMIC_UNIT_ENTITIES())
	{
		if (pOldHeadUnit)
		{
			pOldHeadUnit->reloadEntity();
		}

		if (getHeadUnit())
		{
			getHeadUnit()->reloadEntity();
		}
	}
}


CLLNode<IDInfo>* CvSelectionGroup::deleteUnitNode(CLLNode<IDInfo>* pNode)
{
	FAssertNotInScope(CvSelectionGroup_CvUnit_LOOP);

	if (getOwner() != NO_PLAYER)
	{
		CLLNode<MissionData>* headQueueNode = headMissionQueueNode();

		if (headQueueNode == NULL || headQueueNode->m_data.eMissionType != MISSION_MOVE_TO)
		{
			clearMissionQueue();

			switch (getActivityType())
			{
				case ACTIVITY_SLEEP:
				case ACTIVITY_INTERCEPT:
				case ACTIVITY_PATROL:
				case ACTIVITY_PLUNDER:
					break;
				default: setActivityType(ACTIVITY_AWAKE);
			}
		}
	}
	const CvUnit* pLoopUnit = ::getUnit(pNode->m_data);

	AI_noteSizeChange(-1, pLoopUnit ? pLoopUnit->getCargoVolume() : 0);

	return m_units.deleteNode(pNode);
}


CLLNode<IDInfo>* CvSelectionGroup::nextUnitNode(CLLNode<IDInfo>* pNode) const
{
	return m_units.next(pNode);
}


int CvSelectionGroup::getNumUnits() const
{
	return m_units.getLength();
}

int CvSelectionGroup::getNumUnitCargoVolumeTotal() const
{
	return algo::accumulate(units() | transformed(CvUnit::fn::SMCargoVolume()), 0);
}

int CvSelectionGroup::getLeastCargoVolume() const
{
	PROFILE_EXTRA_FUNC();
	int iLowest = MAX_INT;
	foreach_(const CvUnit* pLoopUnit, units())
	{
		const int iVolume = pLoopUnit->SMCargoVolume();
		if (iVolume < iLowest)
		{
			iLowest = iVolume;
		}
	}
	return iLowest;
}


bool CvSelectionGroup::meetsUnitSelectionCriteria(const CvUnitSelectionCriteria* criteria) const
{
	return algo::any_of(units(), CvUnit::fn::meetsUnitSelectionCriteria(criteria));
}

void CvSelectionGroup::mergeIntoGroup(CvSelectionGroup* pSelectionGroup)
{
	PROFILE_EXTRA_FUNC();
	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	// merge groups, but make sure we do not change the head unit AI
	// this means that if a new unit is going to become the head, change its AI to match, if possible
	// AI_setUnitAIType removes the unit from the current group (at least currently), so we have to be careful in the loop here
	// so, loop until we have not changed unit AIs
	bool bChangedUnitAI = true;
	do
	{
		bChangedUnitAI = false;

		// loop over all the units, moving them to the new group,
		// stopping if we had to change a unit AI, because doing so removes that unit from our group, so we have to start over
		CLLNode<IDInfo>* pUnitNode = headUnitNode();
		while (pUnitNode != NULL && !bChangedUnitAI)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);

			if (pLoopUnit != NULL)
			{
				const UnitAITypes eUnitAI = pLoopUnit->AI_getUnitAIType();

				// if the unitAIs are different, and the loop unit has a higher val, then the group unitAI would change
				// change this UnitAI to the old group UnitAI if possible
				CvUnit* pNewHeadUnit = pSelectionGroup->getHeadUnit();
				UnitAITypes eNewHeadUnitAI = pSelectionGroup->getHeadUnitAI();
				if (pNewHeadUnit!= NULL && eUnitAI != eNewHeadUnitAI && pLoopUnit->AI_groupFirstVal() > pNewHeadUnit->AI_groupFirstVal())
				{
					// non-zero AI_unitValue means that this UnitAI is valid for this unit (that is the check used everywhere)
					if (kPlayer.AI_unitValue(pLoopUnit->getUnitType(), eNewHeadUnitAI, NULL) > 0)
					{
						FAssert(pLoopUnit->AI_getUnitAIType() != UNITAI_HUNTER);
						// this will remove pLoopUnit from the current group
						pLoopUnit->AI_setUnitAIType(eNewHeadUnitAI);

						bChangedUnitAI = true;
					}
				}
				if (pLoopUnit->canJoinGroup(pLoopUnit->plot(), pSelectionGroup))
				{
					pLoopUnit->joinGroup(pSelectionGroup);
					pUnitNode = headUnitNode();
				}
				else
				{
					pUnitNode = nextUnitNode(pUnitNode);
				}
			}
		}
	}
	while (bChangedUnitAI);
}

namespace {
	bool isValidHeadUnit(const CvUnit* ignoreUnit, UnitAITypes requiredAI, const CvUnit* unit)
	{
		return unit != ignoreUnit && unit->AI_getUnitAIType() == requiredAI;
	}
}

// split this group into two groups, one of iSplitSize, the other the remaining units
// this group can survive this function as the remainder group.
// split up each unit AI type as evenly as possible, mirrors origninal group composition.
CvSelectionGroup* CvSelectionGroup::splitGroup(int iSplitSize, CvUnit* pNewHeadUnit, CvSelectionGroup** ppOtherGroup)
{
	PROFILE_EXTRA_FUNC();
	if (iSplitSize <= 0)
	{
		FErrorMsg("splitGroup size must be > 0");
		return NULL;
	}

	// are we already small enough?
	if (getNumUnits() <= iSplitSize)
	{
		return this;
	}

	CvUnit* pOldHeadUnit = getHeadUnit();
	if (pOldHeadUnit == NULL)
	{
		FErrorMsg("Non-zero size group without head unit");
		return NULL;
	}

	const UnitAITypes eOldHeadAI = pOldHeadUnit->AI_getUnitAIType();

	// if pNewHeadUnit NULL, then we will use our current head to head the new split group of target size
	if (pNewHeadUnit == NULL)
	{
		pNewHeadUnit = pOldHeadUnit;
	}

	// pRemainderHeadUnit is the head unit of the group that contains the remainder of units
	CvUnit* pRemainderHeadUnit = NULL;

	if (pOldHeadUnit == pNewHeadUnit)
	{
		// try to find remainder head with same AI as head, if we cannot find one, we will leave the remaining units in this group
		unit_iterator fitr = std::find_if(beginUnits(), endUnits(), bind(isValidHeadUnit, pNewHeadUnit, eOldHeadAI, _1));
		if (fitr != endUnits())
		{
			pRemainderHeadUnit = *fitr;
		}
	}
	else // if the new head is not the old head, then make the old head the remainder head
	{
		pRemainderHeadUnit = pOldHeadUnit;
	}

	// Default to leaving remaining units in this group
	CvSelectionGroup* pRemainderGroup = this;

	// make the new group for the new head
	pNewHeadUnit->joinGroup(NULL);
	CvSelectionGroup* pSplitGroup = pNewHeadUnit->getGroup();
	FAssertMsg(pSplitGroup != NULL, "join resulted in NULL group");

	// make a new group for the remainder, if non-null
	if (pRemainderHeadUnit != NULL)
	{
		pRemainderHeadUnit->joinGroup(NULL);
		pRemainderGroup = pRemainderHeadUnit->getGroup();
		FAssertMsg(pRemainderGroup != NULL, "join resulted in NULL group");
		FAssertMsg(pRemainderGroup != this, "join resulted in same group");
	}

	// split units by AI type
	typedef stdext::hash_map< UnitAITypes, std::vector<CvUnit*> > UnitGrouping;
	UnitGrouping unitGroups;
	foreach_(CvUnit* unit, units())
	{
		unitGroups[unit->AI_getUnitAIType()].push_back(unit);
	}

	const int sourceGroupSize = getNumUnits();

	// Interlace units into the two groups, new group takes its fair share from each AI type proportionally to orignial group
	int iUnitsSplitOffCount = 1;
	foreach_(const std::vector<CvUnit*>& unitsOfType, unitGroups | map_values)
	{
		int idx = 0;
		if (iSplitSize - iUnitsSplitOffCount > 0)
		{
			// We want to take a proportion of the units equal to the proportional size of iSplitSize relative to the original group.
			// i.e. we going to take our fair share (+1 so we don't suffer rounding errors)
			int countForThisAIType = std::min(1 + iSplitSize * unitsOfType.size() / sourceGroupSize, unitsOfType.size());
			// Make sure we don't exceed the requested units for the split group (this might not really matter)
			countForThisAIType = std::min(countForThisAIType, iSplitSize - iUnitsSplitOffCount);

			for (; idx < countForThisAIType; ++idx)
			{
				unitsOfType[idx]->joinGroup(pSplitGroup);
				iUnitsSplitOffCount++;
			}
		}
		if (pRemainderGroup != this) // Toffer, no point for a unit in this group to join this group...
		{
			for (; idx < static_cast<int>(unitsOfType.size()); ++idx)
			{
				unitsOfType[idx]->joinGroup(pRemainderGroup);
			}
		}
		else if (iSplitSize <= iUnitsSplitOffCount)
		{
			break; // Toffer - Job done.
		}
	}
	FAssertMsg(pRemainderGroup == this || getNumUnits() == 0, "Source group in split action wasn't fully emptied");
	FAssertMsg(pSplitGroup->getNumUnits() == iSplitSize, "New split group didn't meet requested size");
	FAssertMsg(
		pRemainderGroup->getNumUnits() == (getNumUnits() == 0 ? 2 : 1) + sourceGroupSize - iSplitSize,
		CvString::format(
			"%S (%d) - New remainder group of %d didn't meet expected size of %d",
			pRemainderGroup->getHeadUnit()->getDescription().c_str(),
			pRemainderGroup->getHeadUnit()->getID(),
			pRemainderGroup->getNumUnits(),
			(getNumUnits() == 0 ? 2 : 1) + sourceGroupSize - iSplitSize
		).c_str()
	);
	if (ppOtherGroup != NULL)
	{
		*ppOtherGroup = pRemainderGroup;
	}
	return pSplitGroup;
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvSelectionGroup::getUnitIndex
//! \brief      Returns the index of the given unit in the selection group
//! \param      pUnit The unit to find the index of within the group
//! \retval     The zero-based index of the unit within the group, or -1 if it is not in the group.
//------------------------------------------------------------------------------------------------
/*DllExport*/ int CvSelectionGroup::getUnitIndex(CvUnit* pUnit, int maxIndex /* = -1 */) const
{
	PROFILE_EXTRA_FUNC();
#ifdef _DEBUG
	OutputDebugString("exe is asking for a units' index within this group\n");
#endif
	FAssertMsg(this != NULL, "CTD incoming");
	int iIndex = 0;

	foreach_(const CvUnit* unitX, units())
	{
		if (unitX == pUnit)
		{
			return iIndex;
		}
		iIndex++;

		if (maxIndex >= 0 && iIndex >= maxIndex)
		{
			return -1;
		}
	}
	return -1;
}

CLLNode<IDInfo>* CvSelectionGroup::headUnitNode() const
{
	return m_units.head();
}


CvUnit* CvSelectionGroup::getHeadUnit() const
{
	const CLLNode<IDInfo>* pUnitNode = headUnitNode();
	return pUnitNode ? ::getUnit(pUnitNode->m_data) : NULL;
}

CvUnit* CvSelectionGroup::getUnitAt(int index) const
{
	PROFILE_EXTRA_FUNC();
	if (index >= getNumUnits())
	{
		FErrorMsg("[Jason] Selectiongroup unit index out of bounds.");
		return NULL;
	}

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	for (int i = 0; i < index; i++)
		pUnitNode = nextUnitNode(pUnitNode);

	CvUnit *pUnit = ::getUnit(pUnitNode->m_data);
	return pUnit;
}


UnitAITypes CvSelectionGroup::getHeadUnitAI() const
{
	const CvUnit* pHeadUnit = getHeadUnit();
	return pHeadUnit ? pHeadUnit->AI_getUnitAIType() : NO_UNITAI;
}


PlayerTypes CvSelectionGroup::getHeadOwner() const
{
	const CvUnit* pHeadUnit = getHeadUnit();
	return pHeadUnit ? pHeadUnit->getOwner() : NO_PLAYER;
}


TeamTypes CvSelectionGroup::getHeadTeam() const
{
	const CvUnit* pHeadUnit = getHeadUnit();
	return pHeadUnit ? pHeadUnit->getTeam() : NO_TEAM;
}

//std::vector<const CvUnit*> CvSelectionGroup::get_if(bst::function<bool(const CvUnit*)> predicateFn) const
//{
//	std::vector<const CvUnit*> units;
//	for (unit_iterator itr = beginUnits(); itr != endUnits(); ++itr)
//	{
//		if (predicateFn(*itr))
//		{
//			units.push_back(*itr);
//		}
//	}
//	return units;
//}
//
//std::vector<CvUnit*> CvSelectionGroup::get_if(bst::function<bool(CvUnit*)> predicateFn)
//{
//	std::vector<CvUnit*> units;
//	for (unit_iterator itr = beginUnits(); itr != endUnits(); ++itr)
//	{
//		if (predicateFn(*itr))
//		{
//			units.push_back(*itr);
//		}
//	}
//	return units;
//}

void CvSelectionGroup::clearMissionQueue()
{
	PROFILE_FUNC();
	FAssert(getOwner() != NO_PLAYER);
	FAssert(getHeadUnit());

	OutputDebugString(CvString::format("%S clearMissionQueue...\n", getHeadUnit() ? getHeadUnit()->getDescription().c_str() : L"Empty group").c_str());
	deactivateHeadMission();

	m_missionQueue.clear();

	if (getOwner() == GC.getGame().getActivePlayer() && IsSelected())
	{
		gDLL->getInterfaceIFace()->setDirty(Waypoints_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}
}

void CvSelectionGroup::setMissionPaneDirty()
{
	FAssert(getOwner() != NO_PLAYER);

	if (IsSelected())
	{
		gDLL->getInterfaceIFace()->setDirty(Waypoints_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}
}


int CvSelectionGroup::getLengthMissionQueue() const
{
	return m_missionQueue.getLength();
}


MissionData* CvSelectionGroup::getMissionFromQueue(int iIndex) const
{
	CLLNode<MissionData>* pMissionNode = m_missionQueue.nodeNum(iIndex);
	return pMissionNode ? &(pMissionNode->m_data) : NULL;
}


bool CvSelectionGroup::insertAtEndMissionQueue(MissionData mission, bool bStart)
{
	PROFILE_FUNC();

	FAssert(getOwner() != NO_PLAYER);

	m_missionQueue.insertAtEnd(mission);

	OutputDebugString(CvString::format("%S insertAtEndMissionQueue %d...\n", getHeadUnit()->getDescription().c_str(), mission.eMissionType).c_str());

	if (getLengthMissionQueue() == 1 && bStart && !activateHeadMission())
	{
		return false;
	}

	if ((getOwner() == GC.getGame().getActivePlayer()) && IsSelected())
	{
		gDLL->getInterfaceIFace()->setDirty(Waypoints_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}

	return true;
}


CLLNode<MissionData>* CvSelectionGroup::deleteMissionQueueNode(CLLNode<MissionData>* pNode)
{
	FAssertMsg(pNode != NULL, "Node is not assigned a valid value");
	FAssert(getOwner() != NO_PLAYER);
	OutputDebugString(CvString::format("%S deleteMissionQueueNode %d...\n", getHeadUnit()->getDescription().c_str(), pNode->m_data.eMissionType).c_str());

	if (pNode == headMissionQueueNode())
	{
		deactivateHeadMission();
	}

	CLLNode<MissionData>* pNextMissionNode = m_missionQueue.deleteNode(pNode);

	if (pNextMissionNode == headMissionQueueNode())
	{
		activateHeadMission();
	}

	if ((getOwner() == GC.getGame().getActivePlayer()) && IsSelected())
	{
		gDLL->getInterfaceIFace()->setDirty(Waypoints_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}

	return pNextMissionNode;
}


CLLNode<MissionData>* CvSelectionGroup::nextMissionQueueNode(CLLNode<MissionData>* pNode) const
{
	return m_missionQueue.next(pNode);
}


CLLNode<MissionData>* CvSelectionGroup::prevMissionQueueNode(CLLNode<MissionData>* pNode) const
{
	return m_missionQueue.prev(pNode);
}


CLLNode<MissionData>* CvSelectionGroup::headMissionQueueNodeExternal() const
{
	static CLLNode<MissionData> transformedMission;
	CLLNode<MissionData>* pHeadMissionData = m_missionQueue.head();

	if (pHeadMissionData == NULL)
	{
		return NULL;
	}
	transformedMission = *pHeadMissionData;

	switch (transformedMission.m_data.eMissionType)
	{
		case MISSION_MOVE_TO:
		case MISSION_ROUTE_TO:
		case MISSION_RANGE_ATTACK:
		{
			CvViewport* pCurrentViewport = GC.getCurrentViewport();

			transformedMission.m_data.iData1 = pCurrentViewport->getViewportXFromMapX(transformedMission.m_data.iData1);
			transformedMission.m_data.iData2 = pCurrentViewport->getViewportYFromMapY(transformedMission.m_data.iData2);
			break;
		}
	}
	return &transformedMission;
}

CLLNode<MissionData>* CvSelectionGroup::headMissionQueueNode() const
{
	return m_missionQueue.head();
}


CLLNode<MissionData>* CvSelectionGroup::tailMissionQueueNode() const
{
	return m_missionQueue.tail();
}


int CvSelectionGroup::getMissionType(int iNode) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	CLLNode<MissionData>* pMissionNode = headMissionQueueNode();

	while (pMissionNode != NULL)
	{
		if ( iNode == iCount )
		{
			return pMissionNode->m_data.eMissionType;
		}

		iCount++;

		pMissionNode = nextMissionQueueNode(pMissionNode);
	}
	return -1;
}

int CvSelectionGroup::getMissionData1(int iNode) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	CLLNode<MissionData>* pMissionNode = headMissionQueueNode();

	while (pMissionNode != NULL)
	{
		if (iNode == iCount)
		{
			return pMissionNode->m_data.iData1;
		}
		iCount++;

		pMissionNode = nextMissionQueueNode(pMissionNode);
	}
	return -1;
}

int CvSelectionGroup::getMissionData2(int iNode) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	CLLNode<MissionData>* pMissionNode = headMissionQueueNode();

	while (pMissionNode != NULL)
	{
		if (iNode == iCount)
		{
			return pMissionNode->m_data.iData2;
		}
		iCount++;

		pMissionNode = nextMissionQueueNode(pMissionNode);
	}
	return -1;
}


bool CvSelectionGroup::isMoveMission(CLLNode<MissionData>* node) const
{
	if (node)
	{
		switch(node->m_data.eMissionType)
		{
			case MISSION_MOVE_TO:
			case MISSION_MOVE_TO_UNIT:
#ifdef _MOD_SENTRY
			case MISSION_MOVE_TO_SENTRY:
#endif
			case MISSION_ROUTE_TO:
			{
				return true;
			}
		}
	}
	return false;
}


void CvSelectionGroup::read(FDataStreamBase* pStream)
{
	PROFILE_EXTRA_FUNC();
	// Init saved data
	reset();

	pStream->Read(&m_iID);
	pStream->Read(&m_iMissionTimer);

	pStream->Read(&m_bForceUpdate);

	pStream->Read((int*)&m_eOwner);
	pStream->Read((int*)&m_eActivityType);
	pStream->Read((int*)&m_eAutomateType);

	m_units.Read(pStream);
	m_missionQueue.Read(pStream);

	//	The order queue itself is not a streamable type so is serialized in raw
	//	binary image, which means we need to do some explicit translation on load
	//	if we are using the tagged format
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	if ( wrapper.isUsingTaggedFormat() )
	{
		CLLNode<MissionData>* pNode = headMissionQueueNode();
		while (pNode != NULL)
		{
			switch(pNode->m_data.eMissionType)
			{
			case MISSION_BUILD:
				if ( (pNode->m_data.iData1 = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDS,pNode->m_data.iData1)) == (int)NO_BUILD )
				{
					//	This build has been removed - delete this queue element
					pNode = deleteMissionQueueNode(pNode);
				}
				else
				{
					pNode = nextMissionQueueNode(pNode);
				}
				break;
			case MISSION_MOVE_TO:
			case MISSION_ROUTE_TO:
			case MISSION_NUKE:
			case MISSION_RECON:
			case MISSION_PARADROP:
			case MISSION_AIRBOMB:
			case MISSION_RANGE_ATTACK:
			case MISSION_AIRBOMB1:
			case MISSION_AIRBOMB2:
			case MISSION_AIRBOMB3:
			case MISSION_AIRBOMB4:
			case MISSION_AIRBOMB5:
			case MISSION_RBOMBARD:
			case MISSION_FENGAGE:
			case MISSION_CLAIM_TERRITORY:
				//	Fixup for viewports, old versions of which can leave things un-normalized
				pNode->m_data.iData1 = (pNode->m_data.iData1 + GC.getMap().getGridWidth()) % GC.getMap().getGridWidth();
				pNode->m_data.iData2 = (pNode->m_data.iData2 + GC.getMap().getGridHeight()) % GC.getMap().getGridHeight();
				// Drop through
			default:
				pNode = nextMissionQueueNode(pNode);
				break;
			}
		}
	}
}


void CvSelectionGroup::write(FDataStreamBase* pStream)
{
	pStream->Write(m_iID);
	pStream->Write(m_iMissionTimer);

	pStream->Write(m_bForceUpdate);

	pStream->Write(m_eOwner);
	pStream->Write(m_eActivityType);
	pStream->Write(m_eAutomateType);

	m_units.Write(pStream);
	m_missionQueue.Write(pStream);
}


bool CvSelectionGroup::activateHeadMission()
{
	FAssert(getOwner() != NO_PLAYER);
	return headMissionQueueNode() == NULL || isBusy() || startMission();
}


void CvSelectionGroup::deactivateHeadMission()
{
	FAssert(getOwner() != NO_PLAYER);

	if (headMissionQueueNode() != NULL)
	{
		if (getActivityType() == ACTIVITY_MISSION)
		{
			setActivityType(ACTIVITY_AWAKE);
		}
		setMissionTimer(0);
	}
}


bool CvSelectionGroup::allMatch(UnitTypes eUnit) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);

	return algo::all_of(units(), CvUnit::fn::getUnitType() == eUnit);
}

// BUG - Safe Move - start
void CvSelectionGroup::checkLastPathPlot(const CvPlot* pPlot)
{
	m_bLastPathPlotChecked = true;
	if (pPlot != NULL)
	{
		m_bLastPlotVisible = pPlot->isVisible(getTeam(), false);
		m_bLastPlotRevealed = pPlot->isRevealed(getTeam(), false);
	}
	else
	{
		m_bLastPlotVisible = false;
		m_bLastPlotRevealed = false;
	}
}

void CvSelectionGroup::clearLastPathPlot()
{
	m_bLastPathPlotChecked = false;
}

bool CvSelectionGroup::isLastPathPlotChecked() const
{
	return m_bLastPathPlotChecked;
}

bool CvSelectionGroup::isLastPathPlotVisible() const
{
	return m_bLastPathPlotChecked ? m_bLastPlotVisible : false;
}

bool CvSelectionGroup::isLastPathPlotRevealed() const
{
	return m_bLastPathPlotChecked ? m_bLastPlotRevealed : false;
}
// BUG - Safe Move - end

int CvSelectionGroup::defensiveModifierAtPlot(const CvPlot* pPlot) const
{
	PROFILE_EXTRA_FUNC();
	const int iModifier = pPlot->defenseModifier(getTeam(), false);

	int iBestStrength = 0;
	int iBestExtraModifier = 0;

	foreach_(const CvUnit* pLoopUnit, units())
	{
		if (pLoopUnit->canDefend())
		{
			int iUnitModifier = 0;

			if (pPlot->isHills())
			{
				iUnitModifier += pLoopUnit->hillsDefenseModifier();
			}

			if (pPlot->getFeatureType() != NO_FEATURE)
			{
				iUnitModifier += pLoopUnit->featureDefenseModifier(pPlot->getFeatureType());
			}

			iUnitModifier += pLoopUnit->terrainDefenseModifier(pPlot->getTerrainType());

			const int iStrength = (pLoopUnit->getHP() * (100 + iUnitModifier))/100;

			if (iStrength > iBestStrength)
			{
				iBestStrength = iStrength;
				iBestExtraModifier = iUnitModifier;
			}
		}
	}

	return iModifier + iBestExtraModifier;
}

int CvSelectionGroup::getStrength() const
{
	PROFILE_EXTRA_FUNC();
	int iStrength = 0;

	foreach_(const CvUnit* pLoopUnit, units())
	{
		iStrength += pLoopUnit->AI_genericUnitValueTimes100(UNITVALUE_FLAGS_DEFENSIVE | UNITVALUE_FLAGS_OFFENSIVE);
	}

	return iStrength/100;
}

bool CvSelectionGroup::hasCommander() const
{
	return algo::any_of(units(), CvUnit::fn::isCommander());
}

bool CvSelectionGroup::hasCommodore() const
{
	return algo::any_of(units(), CvUnit::fn::isCommodore());
}

void CvSelectionGroup::validateLocations(bool bFixup) const
{
	PROFILE_EXTRA_FUNC();
	CvPlot* pPlot = NULL;
	CvUnit* pTransportUnit;
	bool bTransportUnitSet = false;

	foreach_(CvUnit* pLoopUnit, units())
	{
		if (!bTransportUnitSet)
		{
			bTransportUnitSet = true;
			pTransportUnit = pLoopUnit->getTransportUnit();
		}

		if (pTransportUnit != pLoopUnit->getTransportUnit())
		{
			if (bFixup)
			{
				FErrorMsg("Group split in cargo state - fixing");

				pLoopUnit->joinGroup(NULL);
			}
			else
			{
				FAssertMsg(pLoopUnit->isHuman(), "Group split in cargo state");
			}
		}

		if (pPlot == NULL)
		{
			pPlot = pLoopUnit->plot();
		}
		else if (!bFixup)
		{
			FAssertMsg(pLoopUnit->isHuman() || pPlot == pLoopUnit->plot(), "Incorrect plot on unit of group");
		}
		else if (pPlot != pLoopUnit->plot())
		{
			FErrorMsg("Incorrect plot on unit of group - fixing");

			pLoopUnit->joinGroup(NULL); // Drop the errant unit from the group
		}
	}
}

bool CvSelectionGroup::findNewLeader(UnitAITypes eAIType)
{
	PROFILE_EXTRA_FUNC();
	CvUnit* pBestUnit = NULL;
	const CvPlayer& kPlayer = GET_PLAYER(getOwner());

	foreach_(CvUnit* pLoopUnit, units())
	{
		if (kPlayer.AI_unitValue(pLoopUnit->getUnitType(), eAIType, NULL) > 0)
		{
			pBestUnit = pLoopUnit;
			break;
		}
		else if (eAIType == UNITAI_ATTACK_CITY && pLoopUnit->canAttack())
		{
			//	If there is no unit that explicitly supports the attack city role
			//	accept any that can attack so as to keep the stack's intent
			pBestUnit = pLoopUnit;
		}
	}

	if (pBestUnit != NULL)
	{
		m_bIsChoosingNewLeader = true;

		pBestUnit->AI_setUnitAIType(eAIType);

		m_bIsChoosingNewLeader = false;

		pBestUnit->joinGroup(this);

		return true;
	}
	return false;
}

namespace {
	bool isMergable(const CvUnit* unit)
	{
		// Inhibit workers from this function since some sacrifice themselves
		// (which would require much more programming to manage for the ai)
		// and worker merging is only a way for players to manage less units really.
		return unit->AI_getUnitAIType() != UNITAI_WORKER && unit->canMerge(true);
	}
}


// TB Notes: I'd like to add that a unit should only merge with units of the same AI to avoid problems here.
bool CvSelectionGroup::doMergeCheck()
{
	PROFILE_EXTRA_FUNC();
	bool anyMerged = false;

	while (true)
	{
		bst::optional<CvUnit*> mergable = algo::find_if(units(), isMergable);

		if (mergable)
		{
			anyMerged = true;
			(*mergable)->doMerge();
		}
		else break;
	}
	return anyMerged;
}

int CvSelectionGroup::getCargoSpace() const
{
	PROFILE_EXTRA_FUNC();
	FAssert(getNumUnits() > 0);

	const UnitAITypes eUnitAI = getHeadUnitAI();
	int iCargoCount = 0;

	foreach_(const CvUnit* unitX, units())
	{
		if (unitX->AI_getUnitAIType() == eUnitAI)
		{
			iCargoCount += unitX->cargoSpace();
		}
	}
	return iCargoCount;
}

int CvSelectionGroup::getCargoSpaceAvailable(SpecialUnitTypes eSpecialCargo, DomainTypes eDomainCargo) const
{
	PROFILE_EXTRA_FUNC();
	FAssert(getNumUnits() > 0);

	const UnitAITypes eUnitAI = getHeadUnitAI();
	int iCargoCount = 0;

	foreach_(const CvUnit* unitX, units())
	{
		if (unitX->AI_getUnitAIType() == eUnitAI)
		{
			iCargoCount += unitX->cargoSpaceAvailable(eSpecialCargo, eDomainCargo);
		}
	}
	return iCargoCount;
}

int CvSelectionGroup::countSeeInvisibleActive(UnitAITypes eUnitAI, InvisibleTypes eInvisibleType) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	foreach_(const CvUnit* pLoopUnit, units())
	{
		if (pLoopUnit->AI_getUnitAIType() == eUnitAI)
		{
			if (GC.getGame().isOption(GAMEOPTION_COMBAT_HIDE_SEEK))
			{
				if (pLoopUnit->visibilityIntensityTotal(eInvisibleType) > 0)
				{
					iCount++;
				}
			}
			else
			{
				for (int iI = 0; iI < pLoopUnit->getNumSeeInvisibleTypes(); ++iI)
				{
					if (pLoopUnit->getSeeInvisibleType(iI) == (int)eInvisibleType)
					{
						iCount++;
					}
				}
			}
		}
	}

	return iCount;
}

void CvSelectionGroup::releaseUnitAIs(UnitAITypes eUnitAI)
{
	algo::for_each(units() | filtered(CvUnit::fn::AI_getUnitAIType() == eUnitAI),
		CvUnit::fn::joinGroup(NULL)
	);
}

CvUnit* CvSelectionGroup::unit_iterator::resolve(const IDInfo& info) const
{
	return ::getUnit(info);
}

//const CvUnit* CvSelectionGroup::const_unit_iterator::resolve(const IDInfo& info) const
//{
//	return ::getUnit(info);
//}
