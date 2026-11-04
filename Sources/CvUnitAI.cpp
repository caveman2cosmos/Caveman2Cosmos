// unitAI.cpp

#include "UnitTravelConnectivity.h"
#include "ReadOnlyLandReachability.h"
#include "ReadOnlyUnitPathFilter.h"
#include "CvGameCoreDLL.h"
#include "ConnectionRequests.h"
#include "TransportRequests.h"
#include "TransportRouting.h"
#include "DoctrinePromotions.h"
#include "NavalDecisionDiagnostics.h"
#include "RailRoadRevolution.h"
#include "AIPerf.h"
#include "WorkerRoutePathLimit.h"
#include "WorkerImprovementPathLimit.h"
#include "DefenderPathLimit.h"
#include "DefenderCityProximity.h"
#include "AssignmentSearchPerf.h"
#include "DefenseDemandPerf.h"
#include "DefenseNeedEvents.h"
#include "CvPlotGroup.h"
#include <map>
#include <queue>
#include "StackPromotions.h"
#include "StackPromotionMath.h"

#include "FProfiler.h"

#include "CvArea.h"
#include "CvBuildingInfo.h"
#include "CvBonusInfo.h"
#include "CvCity.h"
#include "CvCityAI.h"
#include "CvContractBroker.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvImprovementInfo.h"
#include "CvHeritageInfo.h"
#include "CvUnitCombatInfo.h"
#include "CvInfos.h"
#include "CvMap.h"
#include "CvPathGenerator.h"
#include "CvPlayerAI.h"
#include "CvPlot.h"
#include "CvPython.h"
#include "CvReachablePlotSet.h"
#include "CvSelectionGroup.h"
#include "CvSelectionGroupAI.h"
#include "CvTeamAI.h"
#include "CvUnitAI.h"
#include "WorkerRoutePerf.h"
#include "WorkerRouteQueryMemo.h"
#include "CityPropertyDispatch.h"
#include "IntExpr.h"
#include "DecisionCityValues.h"
#include "AttackCandidateMemo.h"
#include "PatrolCandidate.h"
#include "CityDefenderCount.h"
#include "OffensiveStackBalance.h"
#include "ConstructionPathBound.h"
#include "SubduedAnimalPolicy.h"
#include "AirInterceptionCandidates.h"
#include "NavalBombardTargets.h"
#include "MissionDiagnostics.h"
#include "LoadTransportCandidate.h"
#include "NavalRefugePathLimit.h"
#include "CityGuardDanger.h"
#include "CityGuardDefenseSnapshot.h"
#include "CvUnitSelectionCriteria.h"
#include "CvDLLFAStarIFaceBase.h"
#include "BetterBTSAI.h"
#ifdef USE_OLD_PATH_GENERATOR
#include "FAStarNode.h"
#endif
#include <CvWorkerService.h>
#include "SeaWorkerBonusCandidates.h"

PlayerTypes	CvUnitAI::m_cachedPlayer = NO_PLAYER;
CvReachablePlotSet* CvUnitAI::m_cachedMissionaryPlotset = NULL;
static PlayerTypes eCachedTargetCityPlayer = NO_PLAYER;
static PlayerTypes eCachedAttackOddsPlayer = NO_PLAYER;

namespace {
	bool investigatorNeededLocally(const CvUnitAI* unit);
	bool infiltratorWorkingLocally(const CvUnitAI* unit);
}

static bool plotOpaqueInfoMatches(int iOpaqueInfo, int activityId, int iValue)
{
	switch (activityId)
	{
	case ACTIVITY_ID_ANY_ATTACK:
	{
		return iOpaqueInfo != 0 && iValue > (iOpaqueInfo - 1) / 100;
	}
	case ACTIVITY_ID_EXPLORE:
	case ACTIVITY_ID_PILLAGE:
	{
		return iValue == iOpaqueInfo;
	}
	default:
	{
		return false;
	}
	}
}

static bool skipPeacefulCombatSeaPatrol(const CvUnitAI& unit, UnitAITypes role)
{
	// Useful attacks, protection, escort, healing and upgrades are tried before
	// the random patrol fallback. Idle combat ships should return to port in
	// peacetime; privateers and fleets preparing for war keep their old policy.
	return unit.getDomainType() == DOMAIN_SEA
		&& (role == UNITAI_ATTACK_SEA || role == UNITAI_RESERVE_SEA)
		&& !unit.isHuman() && !unit.isNPC()
		&& !unit.isHiddenNationality() && !unit.isAlwaysHostile(unit.plot())
		&& !unit.getGroup()->hasCargo()
		&& !GET_TEAM(unit.getTeam()).hasWarPlan(false)
		&& GET_TEAM(unit.getTeam()).getAtWarCount(false) == 0;
}

static int orderedWorkerFerryPassengers(const CvPlot* tile, const CvUnit* ship, bool includeNew)
{
    int count = 0;
    foreach_(const CvUnit* worker, tile->units())
        if (worker->getOwner() == ship->getOwner() && !worker->isCargo()
            && worker->AI_getUnitAIType() == UNITAI_WORKER
            && worker->getGroup()->AI_getMissionAIType() == MISSIONAI_LOAD_SETTLER
            && worker->getGroup()->AI_getMissionAIUnit() == ship
            && PUF_isAvailableUnitAITypeGroupie(worker, UNITAI_WORKER, -1, NULL)
            && (includeNew ? PUF_isFiniteRange(worker, -1, -1, NULL) : PUF_isFiniteRangeAndNotJustProduced(worker, -1, -1, NULL))) ++count;
    return count;
}

static bool requiresExplicitAssaultPickup(UnitAITypes transportRole, UnitAITypes cargoRole, AreaAITypes areaAI)
{
	// Land armies may wait at a coastal staging city without intending to
	// embark. Respect that assignment, but keep their explicit sea shortcuts.
	return transportRole == UNITAI_ASSAULT_SEA && cargoRole == UNITAI_ATTACK_CITY
		&& (areaAI == AREAAI_OFFENSIVE || areaAI == AREAAI_MASSING);
}

static bool PUF_isAvailableAssaultPickup(const CvUnit* unit, int role, int requireOrder, const CvUnit* other)
{
	return PUF_isAvailableUnitAITypeGroupie(unit, role, -1, other)
		&& (!requireOrder || unit->getGroup()->AI_getMissionAIType() == MISSIONAI_LOAD_ASSAULT);
}

static bool shouldAvoidLowOddsRiverAttack(const CvUnitAI& kAttacker, const CvPlot* pTargetPlot, int iAttackOdds)
{
	if (pTargetPlot == NULL || iAttackOdds >= 10)
	{
		return false;
	}
	if (kAttacker.getDomainType() != DOMAIN_LAND || kAttacker.isRiver())
	{
		return false;
	}
	const CvPlot* pFromPlot = kAttacker.plot();
	if (pFromPlot == NULL || pFromPlot == pTargetPlot)
	{
		return false;
	}
	if (stepDistance(pFromPlot->getX(), pFromPlot->getY(), pTargetPlot->getX(), pTargetPlot->getY()) != 1)
	{
		return false;
	}
	const DirectionTypes eDirection = directionXY(pFromPlot, pTargetPlot);
	return eDirection != NO_DIRECTION && pFromPlot->isRiverCrossing(eDirection);
}

void CvUnitAI::AI_clearCaches()
{
	m_cachedPlayer = NO_PLAYER;
	SAFE_DELETE(m_cachedMissionaryPlotset);

	eCachedTargetCityPlayer = NO_PLAYER;
	eCachedAttackOddsPlayer = NO_PLAYER;
}

#define	MAX_SEARCH_RANGE			25
#define	MAX_BARB_TARGET_CITY_RANGE	20

#define FOUND_RANGE					(7)

typedef struct
{
	CvPlot* plot;
	int		value;
} plotValue;

static bool plotValueSortPredicate(const plotValue& d1, const plotValue& d2)
{
	//	Sort largest first
	return d1.value < d2.value;
}

// Public Functions...

CvUnitAI::CvUnitAI(bool bIsDummy) : CvUnit(bIsDummy)
{
	AI_reset(NO_UNITAI, true);
}


CvUnitAI::~CvUnitAI()
{
	AI_uninit();
}


CvUnitAI& CvUnitAI::operator=(const CvUnitAI& other)
{
	m_iBirthmark = other.m_iBirthmark;
	m_eUnitAIType = other.m_eUnitAIType;
	m_iAutomatedAbortTurn = other.m_iAutomatedAbortTurn;
	m_iGroupLeadOverride = other.m_iGroupLeadOverride;
	m_contractsLastEstablishedTurn = other.m_contractsLastEstablishedTurn;
	m_contractualState = other.m_contractualState;
	m_iAwaitingContractStuckPasses = other.m_iAwaitingContractStuckPasses;
	m_eIntendedConstructBuilding = other.m_eIntendedConstructBuilding;
	m_eIntendedHeritage = other.m_eIntendedHeritage;
	m_iPredictedHitPoints = other.m_iPredictedHitPoints;
	m_bHasAttacked = other.m_bHasAttacked;
	m_bWaitingOnUnitAIAny = other.m_bWaitingOnUnitAIAny;
	m_iGarrisonCity = other.m_iGarrisonCity;
	m_iGenericValue = other.m_iGenericValue;
	m_aiWaitingOnUnitAITypes = other.m_aiWaitingOnUnitAITypes;
    std::copy(other.m_aiNavalCommitment, other.m_aiNavalCommitment + NavalCommitment::COUNT, m_aiNavalCommitment);
    m_civilianDecision = CivilianDecision::State();

	static_cast<CvUnit&>(*this) = static_cast<const CvUnit&>(other);

	return *this;
}


void CvUnitAI::AI_init(UnitAITypes eUnitAI, int iBirthmark)
{
	AI_reset(eUnitAI);

	//--------------------------------
	// Init other game data
	AI_setBirthmark(iBirthmark);

	FAssertMsg(AI_getUnitAIType() != NO_UNITAI, "AI_getUnitAIType() is not expected to be equal with NO_UNITAI");
	GET_PLAYER(getOwner()).AI_changeNumAIUnits(AI_getUnitAIType(), 1);
}


void CvUnitAI::AI_uninit()
{
}


void CvUnitAI::AI_reset(UnitAITypes eUnitAI, bool bConstructorCall)
{
    std::fill(m_aiNavalCommitment, m_aiNavalCommitment + NavalCommitment::COUNT, 0);
    m_civilianDecision = CivilianDecision::State();
	FAssert(bConstructorCall || eUnitAI != NO_UNITAI);

	AI_uninit();

	m_iBirthmark = 0;

	m_eUnitAIType = eUnitAI;

	m_iAutomatedAbortTurn = -1;

	m_iGroupLeadOverride = -1;

	m_contractsLastEstablishedTurn = -1;
	m_contractualState = CONTRACTUAL_STATE_NONE;
	m_iAwaitingContractStuckPasses = 0;

	m_eIntendedConstructBuilding = NO_BUILDING;
	m_eIntendedHeritage = NO_HERITAGE;

	m_iPredictedHitPoints = -1;
	m_bHasAttacked = false;
	m_bWaitingOnUnitAIAny = false;

	m_iGarrisonCity = -1;
	m_iGenericValue = -1;

	m_aiWaitingOnUnitAITypes.clear();
}
void CvUnitAI::SendLog(CvWString function, CvWString message)
{
	//WIP, wrapper of the new FLB logger, to create correct payload for this class
	CvWString aiType = "CvUnitAI";


	// logAIJson(aiType, this->getName(), function, message);

}

// AI_update returns true when we should abort the loop and wait until next slice
bool CvUnitAI::AI_update()
{
    StackPromotions::EvaluationScope aidEvaluation(isHuman() || !StackPromotions::enabled());
	AI_PERF_CONTEXT(getOwner(),getID());
	AI_PERF_UNIT_SCOPE(AI_getUnitAIType());
	AI_PERF_UNIT_STATE(getOwner(),getID(),getX(),getY(),movesLeft(),getGroup() ? getGroup()->AI_getMissionAIType() : NO_MISSIONAI);
	PROFILE_FUNC();

	LOG_UNIT_BLOCK(4, {
		UnitAITypes eUnitAi = AI_getUnitAIType();
		MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = m_szName;
		if (StrUnitName.length() == 0)
		{
			StrUnitName = getName(0).GetCString();
		}

		logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Starting AI_Update...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits());
	});

#ifdef _DEBUG
	getGroup()->validateLocations(true);
#endif

	//	If canMove() is false there is not much we can do.  Empirically this can happen
	//	Not sure why but suspect either:
	//	1) The loop in CvPlayer that calls AI_Update() for the active player but only ends turn
	//	   when there are no non-busy or movable units left.  Each time around this loop it's
	//	   possible for units to be left in this state due to unexecutable orders, and stack splits
	//	   which causes ALL units to be asked to reconsider their orders again the next time round the loop
	//	2) When not all units in the stack have the same movement allowance and the head unit can't move after
	//	   execution of the current order
	//	Whatever the cause, the safest thing to do is just push a SKIP without the expense of considering all
	//	the other possibilities it won't be able to execute anyway due to having no movement left
	if (DefenseDemandPerf::enabled() && AI_continueDefenseDemand())return false;
	if (!canMove())
	{
		if (!getGroup()->isBusy())
		{
			getGroup()->pushMission(MISSION_SKIP);
		}
		return false;
	}

	getGroup()->setActivityType(ACTIVITY_AWAKE);

	FAssertMsg(isGroupHead(), "isGroupHead is expected to be true"); // XXX is this a good idea???

	CvReachablePlotSet::ClearCache();

	if (getDomainType() == DOMAIN_LAND)
	{
		if (plot()->isWater() && !canMoveAllTerrain() && !plot()->isSeaTunnel())
		{
			getGroup()->pushMission(MISSION_SKIP);
			return false;
		}
		const CvUnit* pTransportUnit = getTransportUnit();

		if (pTransportUnit != NULL && (pTransportUnit->getGroup()->hasMoved() || pTransportUnit->getGroup()->headMissionQueueNode() != NULL))
		{
			getGroup()->pushMission(MISSION_SKIP);
			return false;
		}
	}

	if (AI_afterAttack())
	{
		return false;
	}

	if (getGroup()->isAutomated())
	{
		switch (getGroup()->getAutomateType())  // NOLINT(clang-diagnostic-switch-enum)
		{
		case AUTOMATE_BUILD:
		{
			if (AI_getUnitAIType() == UNITAI_WORKER)
			{
				AI_workerMove();
			}
			else if (AI_getUnitAIType() == UNITAI_WORKER_SEA)
			{
				AI_workerSeaMove();
			}
			else FErrorMsg("error");

			break;
		}
		case AUTOMATE_NETWORK:
		{
			AI_networkAutomated();
			break;
		}
		case AUTOMATE_CITY:
		{
			AI_cityAutomated();
			break;
		}
		case AUTOMATE_EXPLORE:
		{
			switch (getDomainType())
			{
			case DOMAIN_SEA:
			{
				AI_exploreSeaMove();
				break;
			}
			case DOMAIN_AIR:
			{
				// if we are cargo (on a carrier), hold if the carrier is not done moving yet
				const CvUnit* pTransportUnit = getTransportUnit();
				if (pTransportUnit != NULL
				&& pTransportUnit->isAutomated()
				&& pTransportUnit->canMove()
				&& pTransportUnit->getGroup()->getActivityType() != ACTIVITY_HOLD)
				{
					getGroup()->pushMission(MISSION_SKIP);
					break;
				}
				AI_exploreAirMove(); // Have air units explore like AI units do
				break;
			}
			case DOMAIN_LAND:
			{
				AI_exploreMove();
				break;
			}
			default: FErrorMsg("error");
			}
			// if we have air cargo (we are a carrier), and are done moving, explore with the aircraft as well
			if (hasCargo() && getDomainCargo() == DOMAIN_AIR
			&& (!canMove() || getGroup()->getActivityType() == ACTIVITY_HOLD))
			{
				std::vector<CvUnit*> aCargoUnits;
				getCargoUnits(aCargoUnits);
				if (!aCargoUnits.empty())
				{
					validateCargoUnits();
				}
				foreach_(const CvUnit * pCargoUnit, aCargoUnits)
				{
					FAssert(isAutomated())

						if (pCargoUnit->getDomainType() == DOMAIN_AIR && pCargoUnit->canMove())
						{
							pCargoUnit->getGroup()->setAutomateType(AUTOMATE_EXPLORE);
							pCargoUnit->getGroup()->setActivityType(ACTIVITY_AWAKE);
						}
				}
			}
			break;
		}
		case AUTOMATE_RELIGION:
		{
			if (AI_getUnitAIType() == UNITAI_MISSIONARY)
			{
				AI_missionaryMove();
			}
			else if (getGroup()->hasUnitOfAI(UNITAI_MISSIONARY))
			{
				CvSelectionGroup* pGroup = getGroup();
				joinGroup(NULL);
				joinGroup(pGroup);
				getGroup()->setAutomateType(AUTOMATE_RELIGION);
			}
			else
			{
				getGroup()->setAutomateType(NO_AUTOMATE);
			}
			break;
		}
		case AUTOMATE_HERD:
		{
			if (AI_getUnitAIType() == UNITAI_SUBDUED_ANIMAL)
			{
				if (!AI_construct(MAX_INT, MAX_INT, 0, false, true))
				{
					AI_herdMove();
				}
			}
			else if (getGroup()->hasUnitOfAI(UNITAI_SUBDUED_ANIMAL))
			{
				CvSelectionGroup* pGroup = getGroup();
				joinGroup(NULL);
				joinGroup(pGroup);
				getGroup()->setAutomateType(AUTOMATE_HERD);
			}
			else
			{
				getGroup()->setAutomateType(NO_AUTOMATE);
			}
			break;
		}
		case AUTOMATE_PILLAGE:
		{
			AI_AutomatedpillageMove();
			break;
		}
		case AUTOMATE_HUNT:
		{
			AI_SearchAndDestroyMove();
			break;
		}
		case AUTOMATE_CITY_DEFENSE:
		{
			AI_cityDefense();
			break;
		}
		case AUTOMATE_BORDER_PATROL:
		{
			AI_borderPatrol();
			break;
		}
		case AUTOMATE_PIRATE:
		{
			AI_pirateSeaMove();
			break;
		}
		case AUTOMATE_HURRY:
		{
			AI_merchantMove();
			break;
		}
		//Yes, these automations do the same thing, but they act differently for different units.
		case AUTOMATE_AIRSTRIKE:
		case AUTOMATE_AIRBOMB:
		{
			AI_autoAirStrike();
			break;
		}
		case AUTOMATE_AIR_RECON:
		{
			AI_exploreAirMove();
			break;
		}
		case AUTOMATE_UPGRADING:
		case AUTOMATE_CANCEL_UPGRADING:
		case AUTOMATE_PROMOTIONS:
		case AUTOMATE_CANCEL_PROMOTIONS:
		{
			FErrorMsg("SelectionGroup Should Not be Using These Automations!");
			break;
		}
		case AUTOMATE_SHADOW:
		{
			// If we've lost the unit we're shadowing,
			// not sure how this can happen but empirically it's been seen,
			// then lose the automation.
			if (getShadowUnit() == NULL)
			{
				getGroup()->setAutomateType(NO_AUTOMATE);
			}
			else
			{
				AI_shadowMove();
			}
			break;
		}
		default: FErrorMsg("error");
		}
		// if no longer automated, then we want to bail
		return !isDelayedDeath() && !getGroup()->isAutomated();
	}

    if (AI_continueNavalCommitment()) return false;

	// No confirmed garrison city until we reaffirm it with another set
	m_iAffirmedGarrisonCity = -1;

	if (isNPC())
	{
		PROFILE("CvUnitAI::AI_Update.NPC");

		doUnitAIMove();
	}
	else
	{
		PROFILE("CvUnitAI::AI_Update.civ");
		doUnitAIMove();
	}

	if (NULL != getGroup() && !isDelayedDeath() && m_iGarrisonCity != -1 && m_iAffirmedGarrisonCity == -1)
	{
		// This group has done something else (presumably of higher priority)
		//	so should no longer be considered part of the city's garrison
		AI_setAsGarrison();
	}

#ifdef _DEBUG
	if (NULL != getGroup() && !isDelayedDeath())
	{
		getGroup()->validateLocations(true);
	}
#endif

	if (!AI_isAwaitingContract())
	{
		m_iAwaitingContractStuckPasses = 0;
		return false;
	}

	if (isDelayedDeath())
	{
		return false;
	}

	// Every AI_update() pass that ends still awaiting a contract re-establishes
	// CONTRACTUAL_STATE_AWAITING_ANSWER/WORK for this turn (see processContracts()), which can
	// leave this unit (and its group, which excludes awaiting-contract groups from its
	// MISSION_SKIP safety net) re-offered indefinitely without ever finding one - "AI turn does
	// not end". If we've been stuck awaiting a contract for too many consecutive passes, give up
	// on it outright rather than just telling the caller to keep waiting.
	if (++m_iAwaitingContractStuckPasses > 20)
	{
		contractFulfilled();
		m_iAwaitingContractStuckPasses = 0;
		return false;
	}

	return true;
}

void CvUnitAI::doUnitAIMove()
{
	LOG_UNIT_BLOCK(4, {
		UnitAITypes eUnitAi = AI_getUnitAIType();
		MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = m_szName;
		if (StrUnitName.length() == 0)
		{
			StrUnitName = getName(0).GetCString();
		}

		logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], will perform its move. moves left : %d ...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits(), movesLeft());
	});

	//	If a unit has a contract mission for a unit that has just finished
	//	give the unit a chance to hook up with it by force-processing contracts
	//	before doing other AI activities
	if (getGroup()->AI_getMissionAIType() == MISSIONAI_CONTRACT_UNIT && processContracts(0))
	{
		return;
	}

	if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS)
	&& (!isAnimal() || isGatherHerd())
	&& getGroup()->doMergeCheck())
	{
		return;
	}

	switch (AI_getUnitAIType())
	{
	case UNITAI_UNKNOWN:
		getGroup()->pushMission(MISSION_SKIP);
		break;

	case UNITAI_ANIMAL:
		AI_animalMove();
		break;

	case UNITAI_SETTLE:
		AI_settleMove();
		break;

	case UNITAI_WORKER:
		AI_workerMove();
		break;

	case UNITAI_ATTACK:
		if (isNPC())
		{
			AI_barbAttackMove();
		}
		else
		{
			AI_attackMove();
		}
		break;

	case UNITAI_ATTACK_CITY:
		AI_attackCityMove();
		break;

	case UNITAI_COLLATERAL:
		AI_collateralMove();
		break;

	case UNITAI_PILLAGE:
		AI_pillageMove();
		break;

	case UNITAI_RESERVE:
		AI_reserveMove();
		break;

	case UNITAI_COUNTER:
		AI_counterMove();
		break;

	case UNITAI_PILLAGE_COUNTER:
		AI_pillageCounterMove();
		break;

	case UNITAI_PARADROP:
		AI_paratrooperMove();
		break;

	case UNITAI_CITY_DEFENSE:
		AI_cityDefenseMove();
		break;

	case UNITAI_CITY_COUNTER:
	case UNITAI_CITY_SPECIAL:
		AI_cityDefenseExtraMove();
		break;

	case UNITAI_EXPLORE:
		AI_exploreMove();
		break;

	case UNITAI_HUNTER:
		AI_SearchAndDestroyMove();
		break;

	case UNITAI_HUNTER_ESCORT:
		AI_hunterEscortMove();
		break;

	case UNITAI_MISSIONARY:
		AI_missionaryMove();
		break;

	case UNITAI_PROPHET:
		AI_prophetMove();
		break;

	case UNITAI_ARTIST:
		AI_artistMove();
		break;

	case UNITAI_SCIENTIST:
		AI_scientistMove();
		break;

	case UNITAI_GENERAL:
		AI_generalMove();
		break;

	case UNITAI_GREAT_HUNTER:
		AI_greatHunterMove();
		break;

	case UNITAI_GREAT_ADMIRAL:
		AI_greatAdmiralMove();
		break;

	case UNITAI_MERCHANT:
		AI_merchantMove();
		break;

	case UNITAI_SUBDUED_ANIMAL:
		AI_subduedAnimalMove();
		break;

	case UNITAI_ENGINEER:
		AI_engineerMove();
		break;

	case UNITAI_SPY:
		AI_spyMove();
		break;

	case UNITAI_ICBM:
		AI_ICBMMove();
		break;

	case UNITAI_WORKER_SEA:
		AI_workerSeaMove();
		break;

	case UNITAI_ATTACK_SEA:
		if (isNPC())
		{
			AI_barbAttackSeaMove();
		}
		else
		{
			AI_attackSeaMove();
		}
		break;

	case UNITAI_RESERVE_SEA:
		AI_reserveSeaMove();
		break;

	case UNITAI_ESCORT_SEA:
		AI_escortSeaMove();
		break;

	case UNITAI_EXPLORE_SEA:
		AI_exploreSeaMove();
		break;

	case UNITAI_ASSAULT_SEA:
		AI_assaultSeaMove();
		break;

	case UNITAI_SETTLER_SEA:
		AI_settlerSeaMove();
		break;

	case UNITAI_MISSIONARY_SEA:
		AI_missionarySeaMove();
		break;

	case UNITAI_SPY_SEA:
		AI_spySeaMove();
		break;

	case UNITAI_CARRIER_SEA:
		AI_carrierSeaMove();
		break;

	case UNITAI_MISSILE_CARRIER_SEA:
		AI_missileCarrierSeaMove();
		break;

	case UNITAI_PIRATE_SEA:
		AI_pirateSeaMove();
		break;

	case UNITAI_ATTACK_AIR:
		AI_attackAirMove();
		break;

	case UNITAI_DEFENSE_AIR:
		AI_defenseAirMove();
		break;

	case UNITAI_CARRIER_AIR:
		AI_carrierAirMove();
		break;

	case UNITAI_MISSILE_AIR:
		AI_missileAirMove();
		break;

	case UNITAI_ATTACK_CITY_LEMMING:
		AI_attackCityLemmingMove();
		break;

	case UNITAI_HEALER:
		AI_healerMove();
		break;

	case UNITAI_HEALER_SEA:
		AI_healerSeaMove();
		break;

	case UNITAI_PROPERTY_CONTROL:
		AI_propertyControlMove();
		break;

	case UNITAI_PROPERTY_CONTROL_SEA:
		AI_propertyControlSeaMove();
		break;

	case UNITAI_BARB_CRIMINAL:
		AI_BarbCriminalMove();
		break;

	case UNITAI_INVESTIGATOR:
		AI_InvestigatorMove();
		break;

	case UNITAI_INFILTRATOR:
		AI_InfiltratorMove();
		break;

	case UNITAI_SEE_INVISIBLE:
		AI_SeeInvisibleMove();
		break;

	case UNITAI_SEE_INVISIBLE_SEA:
		AI_SeeInvisibleSeaMove();
		break;

	case UNITAI_ESCORT:
		AI_EscortMove();
		break;

	default:
		FErrorMsg("error");
		break;
	}
}

//	Note death (or capture) of a unit
void CvUnitAI::AI_killed()
{
	if (gUnitLogLevel > 1)
	{
		//	Logging of death location and some mission info
		CvPlot* pMissionPlot = getGroup()->AI_getMissionAIPlot();
		MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(AI_getUnitAIType()).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = m_szName;
		if (StrUnitName.length() == 0)
		{
			StrUnitName = getName(0).GetCString();
		}

		logBBAI("Player %d Unit ID %d, %S, of Type %S, died at (%d,%d), mission was %S", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString());
		if (pMissionPlot != NULL)
		{
			logBBAI("	Mission plot was (%d,%d)", pMissionPlot->getX(), pMissionPlot->getY());
		}
	}
	logContractBroker(1, "%S's %S (%d) died, removing contract", GET_PLAYER(getOwner()).getCivilizationDescription(0), getName(0).GetCString(), m_iID);
	contractFulfilled();
	//	Increment the general danger count for this plot's vicinity
	if (getGroup()->AI_getMissionAIType() != MISSIONAI_DELIBERATE_KILL)
	{
		GET_PLAYER(getOwner()).addPlotDangerSource(plot(), GC.getGame().AI_combatValue(getUnitType()) + 100);

		//	Killing units may change danger evaluation so clear the plot danger cache
#ifdef PLOT_DANGER_CACHING
		CvPlayerAI::ClearPlotDangerCache();
#endif
	}
}

// Returns true if took an action or should wait to move later...
bool CvUnitAI::AI_follow()
{
	if (AI_followBombard())
	{
		return true;
	}
	// Pushing MISSION_MOVE_TO missions when not all units could move resulted in stack being
	// broken up on the next turn.  Also, if we can't attack now we don't want to queue up an
	// attack for next turn, better to re-evaluate.
	const bool bCanAllMove = getGroup()->canAllMove();

	if (bCanAllMove && AI_cityAttack(1, 65, true))
	{
		return true;
	}
	if (isEnemy(plot()->getTeam()) && getGroup()->canPillage(plot()))
	{
		getGroup()->pushMission(MISSION_PILLAGE);
		return true;
	}
	if (bCanAllMove && AI_anyAttack(1, 70, 2, true, true))
	{
		return true;
	}
	if (isFound() && area()->getBestFoundValue(getOwner()) > 0 && AI_foundRange(FOUND_RANGE, true))
	{
		return true;
	}
	return false;
}


// XXX what if a unit gets stuck due to its UnitAIType???
// XXX is this function costing us a lot? (it's recursive...)
bool CvUnitAI::AI_upgrade()
{
    StackPromotions::EvaluationScope aidEvaluation(isHuman() || !StackPromotions::enabled());
	PROFILE_FUNC();
	FAssertMsg(AI_getUnitAIType() != NO_UNITAI, "AI_getUnitAIType() is not expected to be equal with NO_UNITAI");

	// TB Note: New game theory being introduced here - a unitAI should NEVER change its AI when upgrading!
	// This was causing an infinite spam of units that the game couldn't get enough of a count of because
	// the AI kept upgrading their best pick for something and then upgrading it to another unit that couldn't be that AI type.
	// NOW, we should ALWAYS maintain the role a unit was designed for.
	// Watch for odd problems this might introduce elsewhere though.
	const CvUnitInfo& unitInfo = GC.getUnitInfo(getUnitType());

	const std::vector<int>& upgradeChain = unitInfo.getUnitUpgradeChain();
	if (gUnitLogLevel >= 4)
	{
		logBBAI("	%S at (%d,%d) have %d upgrades choices", getName(0).GetCString(), getX(), getY(), (int)upgradeChain.size());
	}

	if (!upgradeChain.empty())
	{
		const CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
		const UnitAITypes eUnitAI = AI_getUnitAIType();
		const CvArea* pArea = area();
		const int iCurrentValue = kPlayer.AI_unitValue(getUnitType(), eUnitAI, pArea);

		UnitTypes eBestUnit = NO_UNIT;
		int iBestValue = -1;

		foreach_(int iUnitX, upgradeChain)
		{
			const UnitTypes eUnitX = (UnitTypes)iUnitX;

			if (!GC.getUnitInfo(eUnitX).getNotUnitAIType(eUnitAI)
			&& canUpgrade(eUnitX)
			&& kPlayer.AI_unitValue(eUnitX, eUnitAI, pArea) >= iCurrentValue)
			{
				if (gUnitLogLevel >= 2)
				{
					logBBAI("	%S at (%d,%d) can upgrade to %S", getName(0).GetCString(), getX(), getY(), GC.getUnitInfo(eUnitX).getDescription());
				}
				const int iValue = GC.getGame().getSorenRandNum(1000, "AI Upgrade");

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestUnit = eUnitX;
				}
			}
		}
		if (eBestUnit != NO_UNIT)
		{
			if (gUnitLogLevel >= 2)
			{
				logBBAI("	Trying to upgrade %S at (%d,%d) to %S", getName(0).GetCString(), getX(), getY(), GC.getUnitInfo(eBestUnit).getDescription());
			}
			return upgrade(eBestUnit);
		}
	}
	return false;
}


bool CvUnitAI::AI_promote()
{
    StackPromotions::EvaluationScope aidEvaluation(isHuman() || !StackPromotions::enabled());
	PROFILE_FUNC();

	if (!isPromotionReady()) return false;

	int iBestValue = 0;
	PromotionTypes eBestPromotion = NO_PROMOTION;

	for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if (canPromote((PromotionTypes)iI, -1))
		{
			int iValue = AI_promotionValue((PromotionTypes)iI);

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestPromotion = (PromotionTypes)iI;
			}
		}
	}

	if (eBestPromotion != NO_PROMOTION)
	{
		if (gUnitLogLevel > 2)
		{
			CvWString StrunitAIType = GC.getUnitAIInfo(AI_getUnitAIType()).getType();
			CvWString StrUnitName = m_szName;
			if (StrUnitName.length() == 0)
			{
				StrUnitName = getName(0).GetCString();
			}
			logBBAI("	%S's %S chooses promotion %S", GET_PLAYER(getOwner()).getCivilizationDescription(0), getName(0).GetCString(), GC.getPromotionInfo(eBestPromotion).getDescription());
			logAiEvaluations(2,"	Player %S Unit %S of type %S - chooses promotion %S", GET_PLAYER(getOwner()).getCivilizationDescription(0), StrUnitName.GetCString(), StrunitAIType.GetCString(), GC.getPromotionInfo(eBestPromotion).getDescription());
		}

		if (promote(eBestPromotion, -1))
		{
			AI_promote();
			return true;
		}
		FErrorMsg(CvString::format("Couldn't apply promotion %S to %S", getName(0).GetCString(), GC.getPromotionInfo(eBestPromotion).getDescription()).c_str());
	}
	return false;
}

int CvUnitAI::AI_groupFirstVal() const
{
	if (m_iGroupLeadOverride != -1)
	{
		return m_iGroupLeadOverride;
	}

	switch (AI_getUnitAIType())
	{
	case UNITAI_UNKNOWN:
	case UNITAI_ANIMAL:
	case UNITAI_HEALER:
	case UNITAI_HEALER_SEA:
	case UNITAI_PROPERTY_CONTROL:
	case UNITAI_PROPERTY_CONTROL_SEA:
	case UNITAI_BARB_CRIMINAL:
	case UNITAI_INFILTRATOR:
		return LEADER_PRIORITY_MIN;

	case UNITAI_SETTLE:
		return 21;
	case UNITAI_WORKER:
		return 20;

		// Koshling - changed priorities of UNITAI_ATTACK and UNITAI_ATTACK_CITY
		//	to be constants with city attack higher.  Previously they overlapped, and were
		//	dependent on collateral damage and bombardment.  This is (a) no longer necessary
		//	since the AI routines now check the group capability (so the unit with the capability
		//	doesn't have to be the leader for it to be used); and (b) overlapping lead to
		//	bad behaviors where an ATTACK unit would join a city attack stack and become the lead,
		//	then realize there was a problem next turn and split the group, then rejoin the
		//	following turn, ...
	case UNITAI_ATTACK:
		return 14;
	case UNITAI_ATTACK_CITY:
		return 17;
	case UNITAI_COLLATERAL:
		return 7;
	case UNITAI_PILLAGE:
		return 12;
	case UNITAI_RESERVE:
		return 6;
	case UNITAI_COUNTER:
		return 5;
	case UNITAI_CITY_DEFENSE:
		return 3;

	case UNITAI_CITY_COUNTER:
	case UNITAI_PILLAGE_COUNTER:
	case UNITAI_SEE_INVISIBLE:
	case UNITAI_SEE_INVISIBLE_SEA:
		return 2;

	case UNITAI_CITY_SPECIAL:
	case UNITAI_INVESTIGATOR:
		return 1;

	case UNITAI_PARADROP:
		return 4;
	case UNITAI_SUBDUED_ANIMAL:
		return 7; // Must be less than the result for UNITAI_HUNTER

	case UNITAI_HUNTER_ESCORT:
	case UNITAI_EXPLORE:
		return 8;

	case UNITAI_HUNTER:
		return 9;
	case UNITAI_MISSIONARY:
		return 20;

	case UNITAI_PROPHET:
	case UNITAI_ARTIST:
	case UNITAI_SCIENTIST:
	case UNITAI_GENERAL:
	case UNITAI_GREAT_HUNTER:
	case UNITAI_GREAT_ADMIRAL:
	case UNITAI_MERCHANT:
	case UNITAI_ENGINEER:
		return 11;

	case UNITAI_SPY:
		return 9;

	case UNITAI_ICBM:
		break;

	case UNITAI_WORKER_SEA:
		return 8;
	case UNITAI_ATTACK_SEA:
		return 3;
	case UNITAI_RESERVE_SEA:
		return 2;

	case UNITAI_ESCORT_SEA:
	case UNITAI_ESCORT:
		return 1;

	case UNITAI_EXPLORE_SEA:
		return 5;
	case UNITAI_ASSAULT_SEA:
		return 11;
	case UNITAI_SETTLER_SEA:
		return 9;
	case UNITAI_MISSIONARY_SEA:
		return 9;
	case UNITAI_SPY_SEA:
		return 10;
	case UNITAI_CARRIER_SEA:
		return 7;
	case UNITAI_MISSILE_CARRIER_SEA:
		return 6;
	case UNITAI_PIRATE_SEA:
		return 4;

	case UNITAI_ATTACK_AIR:
	case UNITAI_DEFENSE_AIR:
	case UNITAI_CARRIER_AIR:
	case UNITAI_MISSILE_AIR:
		break;

	case UNITAI_ATTACK_CITY_LEMMING:
		return 1;

	default:
		FErrorMsg("error");
		break;
	}

	return 0;
}


int CvUnitAI::AI_groupSecondVal() const
{
	return ((getDomainType() == DOMAIN_AIR) ? airBaseCombatStr() : baseCombatStr());
}

int CvUnitAI::AI_getPredictedHitPoints() const
{
	return m_iPredictedHitPoints;
}

void CvUnitAI::AI_setPredictedHitPoints(int iPredictedHitPoints)
{
	m_iPredictedHitPoints = iPredictedHitPoints;

	if (iPredictedHitPoints == -1)
	{
		m_bHasAttacked = false; // This is a reset
	}
}

bool CvUnitAI::AI_getHasAttacked() const
{
	return m_bHasAttacked;
}

// Returns attack odds out of 100 (the higher, the better...)
// Withdrawal odds included in returned value
int CvUnitAI::AI_attackOdds(const CvPlot* pPlot, bool bPotentialEnemy, CvUnit** ppDefender, bool bAssassinate)
{
    StackPromotions::EvaluationScope aidEvaluation(true);
	PROFILE_FUNC();

	CvUnit* pDefender = pPlot->getBestDefender(NO_PLAYER, getOwner(), this, !bPotentialEnemy, bPotentialEnemy, false, bAssassinate);

	if (ppDefender != NULL)
	{
		*ppDefender = pDefender;
	}
	return AI_attackOddsAtPlot(pPlot, pDefender, (ppDefender != NULL));
}

// Returns attack odds out of 100 (the higher, the better...)
// Withdrawal odds included in returned value
int CvUnitAI::AI_attackOddsAtPlot(const CvPlot* pPlot, CvUnit* pDefender, bool modifyPredictedResults)
{
    StackPromotions::EvaluationScope aidEvaluation(true);
	PROFILE_FUNC();

	if (pDefender == NULL)
	{
		return 100;
	}


	static stdext::hash_map<int, int>* resultsCache = NULL;

	CvChecksum cacheKey;

	if (modifyPredictedResults)
	{
		m_bHasAttacked = true;
	}
	else
	{
		if (eCachedAttackOddsPlayer != getOwner() || resultsCache == NULL)
		{
			if (resultsCache == NULL)
			{
				resultsCache = new stdext::hash_map<int, int>();
			}
			else
			{
				resultsCache->clear();
			}

			eCachedAttackOddsPlayer = getOwner();
		}

		cacheKey.add(pPlot->getX());
		cacheKey.add(1024 * pPlot->getY());
		cacheKey.add(getID());
		cacheKey.add(pDefender->getID());
        // Never reuse approximate odds for an exact assessment, or vice versa.
        cacheKey.add(StackPromotions::detailed() ? 1 : 0);

		stdext::hash_map<int, int>::const_iterator itr = resultsCache->find(cacheKey.get());
		if (itr != resultsCache->end())
		{
			return itr->second;
		}
	}

	const int iResult = AI_attackOddsAtPlotInternal(pPlot, pDefender, modifyPredictedResults);

	if (!modifyPredictedResults)
	{
		(*resultsCache)[cacheKey.get()] = iResult;
	}

	return iResult;
}

int CvUnitAI::AI_attackOddsAtPlotInternal(const CvPlot* pPlot, CvUnit* pDefender, bool modifyPredictedResults)
{
    StackPromotions::EvaluationScope aidEvaluation(true);
	PROFILE_FUNC();

	int iOurStrength = ((getDomainType() == DOMAIN_AIR) ? airCurrCombatStr(NULL) : currCombatStr(NULL, NULL));
	if (iOurStrength == 0)
	{
		return 1;
	}
	int iOurFirepower = ((getDomainType() == DOMAIN_AIR) ? iOurStrength : currFirepower(NULL, NULL));

	const bool bSamePlot = pDefender->plot() == plot();

	int iDamageToUs;
	int iDamageToThem;
	int iTheirStrength;
	int iTheirOdds;
	getDefenderCombatValues(*pDefender, pPlot, iOurStrength, iOurFirepower, iTheirOdds, iTheirStrength, iDamageToUs, iDamageToThem, NULL, pDefender);

	int iBaseOdds = 100 - iTheirOdds / 10; // getDefenderCombatValues returns odds based on the combat die (which is 1000 sided)

	//int iTheirFirepower;
//#if 0
//	iTheirStrength = pDefender->currCombatStr(pPlot, this);
//	iTheirFirepower = pDefender->currFirepower(pPlot, this);
//
//
//	FAssert((iOurStrength + iTheirStrength) > 0);
//	FAssert((iOurFirepower + iTheirFirepower) > 0);
//
//	iBaseOdds = (100 * iOurStrength) / (iOurStrength + iTheirStrength);
//#endif
	if (iBaseOdds == 0)
	{
		return 1;
	}

	//#if 0
	//	const int iStrengthFactor = (iOurFirepower + iTheirFirepower + 1) / 2;
	//
	//	//TB Combat Mods Begin
	//	const int iUnmodifiedDefenderArmor = pDefender->armorTotal() - punctureTotal();
	//	const int iUnmodifiedAttackerArmor = armorTotal() - pDefender->punctureTotal();
	//
	//	const int iModifiedDefenderArmorZero = iUnmodifiedDefenderArmor < 0 ? 0 : iUnmodifiedDefenderArmor;
	//	const int iModifiedAttackerArmorZero = iUnmodifiedAttackerArmor < 0 ? 0 : iUnmodifiedAttackerArmor;
	//
	//	const int iModifiedDefenderArmor = (iModifiedDefenderArmorZero < 95 ? iModifiedDefenderArmorZero : 95);
	//	const int iModifiedAttackerArmor = (iModifiedAttackerArmorZero < 95 ? iModifiedAttackerArmorZero : 95);
	//
	//	const int iDefenderArmor = (100 - iModifiedDefenderArmor);
	//	const int iAttackerArmor = (100 - iModifiedAttackerArmor);
	//
	//	iDamageToUs = std::max(1, ((((GC.getCOMBAT_DAMAGE() * (iTheirFirepower + iStrengthFactor)) / (iOurFirepower + iStrengthFactor)) * iAttackerArmor) / 100));
	//	iDamageToThem = std::max(1, ((((GC.getCOMBAT_DAMAGE() * (iOurFirepower + iStrengthFactor)) / (iTheirFirepower + iStrengthFactor)) * iDefenderArmor) / 100));
	//#endif

	if (getDomainType() != DOMAIN_AIR)
	{
		int iOurFirstStrikesTimes2 = (pDefender->immuneToFirstStrikes() ? 0 : 2 * firstStrikes() + chanceFirstStrikes());
		int iTheirFirstStrikesTimes2 = (immuneToFirstStrikes() ? 0 : 2 * pDefender->firstStrikes() + pDefender->chanceFirstStrikes());

		// First assess damage from excess first strike rounds
		if (iOurFirstStrikesTimes2 > iTheirFirstStrikesTimes2)
		{
			pDefender->AI_setPredictedHitPoints(std::max(0, pDefender->getHP() - ((iOurFirstStrikesTimes2 - iTheirFirstStrikesTimes2) * iDamageToThem * iBaseOdds) / 200));

			getDefenderCombatValues(*pDefender, pPlot, iOurStrength, iOurFirepower, iTheirOdds, iTheirStrength, iDamageToUs, iDamageToThem, NULL, pDefender);
			iBaseOdds = 100 - iTheirOdds / 10;

			if (iTheirStrength == 0)
			{
				if (!modifyPredictedResults)
				{
					pDefender->AI_setPredictedHitPoints(-1);
				}
				return 99;
			}
		}
		else if (iOurFirstStrikesTimes2 < iTheirFirstStrikesTimes2)
		{
			AI_setPredictedHitPoints(std::max(0, getHP() - ((iTheirFirstStrikesTimes2 - iOurFirstStrikesTimes2) * iDamageToUs * (100 - iBaseOdds)) / 200));

			iOurStrength = ((getDomainType() == DOMAIN_AIR) ? airCurrCombatStr(NULL) : currCombatStr(NULL, NULL));
			iOurFirepower = ((getDomainType() == DOMAIN_AIR) ? iOurStrength : currFirepower(NULL, NULL));

			if (iOurStrength == 0)
			{
				if (!modifyPredictedResults)
				{
					AI_setPredictedHitPoints(-1);
				}
				return 1;
			}

			getDefenderCombatValues(*pDefender, pPlot, iOurStrength, iOurFirepower, iTheirOdds, iTheirStrength, iDamageToUs, iDamageToThem, NULL, pDefender);
			iBaseOdds = 100 - iTheirOdds / 10;
		}
	}
	//TB Combat Mods End

	const int iHitLimitThem = pDefender->getMaxHP() - combatLimit(pDefender);

	int iNeededRoundsUs = (std::max(0, pDefender->getHP() - iHitLimitThem) + iDamageToThem - 1) / iDamageToThem;
	int iNeededRoundsThem = (std::max(0, getHP()) + iDamageToUs - 1) / iDamageToUs;

	//#if 0
	//	if (getDomainType() != DOMAIN_AIR)
	//	{
	//		// From Mongoose SDK
	//		if (!pDefender->immuneToFirstStrikes())
	//		{
	//			iNeededRoundsUs -= ((iBaseOdds * firstStrikes()) + ((iBaseOdds * chanceFirstStrikes()) / 2)) / 100;
	//		}
	//		if (!immuneToFirstStrikes())
	//		{
	//			iNeededRoundsThem -= (((100 - iBaseOdds) * pDefender->firstStrikes()) + (((100 - iBaseOdds) * pDefender->chanceFirstStrikes()) / 2)) / 100;
	//		}
	//		iNeededRoundsUs = std::max(1, iNeededRoundsUs);
	//		iNeededRoundsThem = std::max(1, iNeededRoundsThem);
	//	}
	//#endif

	const int iRoundsDiff = iNeededRoundsUs - iNeededRoundsThem;

	if (iRoundsDiff > 0)
	{
		if (modifyPredictedResults)
		{
			AI_setPredictedHitPoints(0);

			//	Adjust the predicted damage to allow for the effect of strength reductions
			//	between the attack rounds
			//	Basically we know the loser is losing 100% of their starting strength over the coursemof the battle,
			//	so see how much the base prediction is for the attacker, then reduce the damage to the attacker by the
			//	amount they have left (as a percentage of where they started) since their attacks as rounds go on
			//	will be diluted less (down to around this amount)
			int iWinnerBaseLossPercent = 0;

			if (pDefender->getHP() != 0)
			{
				iWinnerBaseLossPercent += 100 - (100 * iNeededRoundsThem * iDamageToThem) / pDefender->getHP();
			}
			iDamageToThem -= (iDamageToThem * iWinnerBaseLossPercent) / 100;

			FAssert(pDefender->getHP() > iNeededRoundsThem * iDamageToThem);

			pDefender->AI_setPredictedHitPoints(pDefender->getHP() - iNeededRoundsThem * iDamageToThem);
		}

		iTheirStrength *= 100 + 100 * iRoundsDiff / std::max(1, iNeededRoundsThem);
		iTheirStrength /= 100;
	}
	else
	{
		if (modifyPredictedResults)
		{
			pDefender->AI_setPredictedHitPoints(0);

			if (iRoundsDiff != 0)
			{
				//	Adjust the predicted damage to allow for the effect of strength reductions
				//	between the attack rounds
				//	Basically we know the loser is losing 100% of their starting strength over the coursemof the battle,
				//	so see how much the base prediction is for the attacker, then reduce the damage to the attacker by the
				//	amount they have left (as a percentage of where they started) since their attacks as rounds go on
				//	will be diluted less (down to around this amount)
				const int iWinnerBaseLossPercent = 100 - 100 * iNeededRoundsUs * iDamageToUs / getHP();

				iDamageToUs -= iDamageToUs * iWinnerBaseLossPercent / 100;
			}
			// If iRoundsDiff == 0 both units can wind up at a notional 0 HP so cannot assert left over HP in that case
			AI_setPredictedHitPoints(std::max(0, getHP() - iNeededRoundsUs * iDamageToUs));
		}

		iOurStrength *= 100 - 100 * iRoundsDiff / std::max(1, iNeededRoundsUs);
		iOurStrength /= 100;
	}

	int iOdds = iOurStrength * 100 / (iOurStrength + iTheirStrength);

	//	Koshling - modify the calculated odds to account for withdrawal chances
	//	and the AI player's rose-tinted-spectacles value - this used to simply add
	//	to the odds, but that made it look like fights with hugely different strengths
	//	still had 5%ish chances of voctory which was a good expected gain so caused the
	//	AI to suicide weak units onto strong enemy ones, thinking that one in 20 or so would
	//	win againts a massively stronger opponent.  Changed it to be multiplicative instead
	//  TB Combat Mods (I'm hoping this calculates out right...):
	//  Determine Withdraw odds
	const int AdjustedWithdrawalstep1 = withdrawVSOpponentProbTotal(pDefender, pPlot) - pDefender->pursuitVSOpponentProbTotal(this);
	const int AdjustedWithdrawalstep2 = AdjustedWithdrawalstep1 > 100 ? 100 : AdjustedWithdrawalstep1;

	const int AdjustedWithdrawal = AdjustedWithdrawalstep2 < 0 ? 0 : AdjustedWithdrawalstep2;

	const int expectedRndCnt = std::min(iNeededRoundsUs, iNeededRoundsThem);
	const int timePercentage = earlyWithdrawTotal() > 100 ? 100 : earlyWithdrawTotal(); // InitialEarlyWithdrawPercentage

	const int expectedRnds = expectedRndCnt * timePercentage / 100; // Don't add 1 here since z is initialised to the result of the one certain round

	int y = AdjustedWithdrawal;
	int z = AdjustedWithdrawal;

	for (int Time = 0; Time < expectedRnds; ++Time)
	{
		z += AdjustedWithdrawal * y / 100;
		y = AdjustedWithdrawal * (100 - z) / 100; // Prob next round is prob per round times prob you haven't already
	}
	const int EvaluatedWithdrawOdds = z;

	// Figure out odds of repel
	const int iFortRepelWithOverrun = pDefender->fortifyRepelModifier() - overrunTotal();

	const int iFortRepelZero = iFortRepelWithOverrun < 0 ? 0 : iFortRepelWithOverrun;
	const int iFortRepelTotal = iFortRepelZero > 100 ? 100 : iFortRepelZero;

	const int iRepelWithUnyielding = pDefender->repelTotal() + iFortRepelTotal - unyieldingTotal();

	const int iRepelZero = iRepelWithUnyielding < 0 ? 0 : iRepelWithUnyielding;
	const int iRepelTotal = iRepelZero > 100 ? 100 : iRepelZero;

	y = iRepelTotal;
	z = iRepelTotal;

	for (int Time = 0; Time < expectedRndCnt; ++Time)
	{
		z += iRepelTotal * y / 100;
		y = iRepelTotal * (100 - z) / 100; // Prob next round is prob per round times prob you haven't already
	}
	const int EvaluatedRepelOdds = z;

	// Figure out odds of knockback
	const int iKnockbackVsUnyielding = knockbackVSOpponentProbTotal(pDefender) - pDefender->unyieldingTotal();

	const int iKnockbackZero = iKnockbackVsUnyielding < 0 ? 0 : iKnockbackVsUnyielding;
	const int iKnockbackTotal = iKnockbackZero > 100 ? 100 : iKnockbackZero;

	const int iAttackerKnockbackTries = knockbackRetriesTotal();
	y = iKnockbackTotal;
	z = iKnockbackTotal;

	for (int Time = 0; Time < iAttackerKnockbackTries; ++Time)
	{
		z += iKnockbackTotal * y / 100;
		y = iKnockbackTotal * (100 - z) / 100; // Prob next round is prob per round times prob you haven't already
	}
	const int EvaluatedKnockbackOdds = z;

	const int iOddsModifier = EvaluatedWithdrawOdds + EvaluatedKnockbackOdds - EvaluatedRepelOdds;

	//	Surviving is not winning - give survival by withdrawal/knockback half the value of a win
	iOdds += (iOdds * 2 * GET_PLAYER(getOwner()).AI_getAttackOddsChange() + (100 - iOdds) * iOddsModifier / 2) / 100;

	if (!modifyPredictedResults)
	{
		AI_setPredictedHitPoints(-1);
		pDefender->AI_setPredictedHitPoints(-1);
	}
	return range(iOdds, 1, 99);
}


//loops through best builds already generated by a city, and then finds the best one to do just now.
bool CvUnitAI::AI_bestCityBuild(const CvCity* pCity, CvPlot** ppBestPlot, BuildTypes* peBestBuild, const CvPlot* pIgnorePlot, const CvUnit* pUnit)
{
	AI_PERF_SCOPE(WORKER_BUILD_EVAL);
	PROFILE_FUNC();

	const CvPlayerAI& kOwner = GET_PLAYER(getOwner());
	const ImprovementTypes eRuins = GC.getIMPROVEMENT_CITY_RUINS();
	const bool bSafeAutomation = kOwner.isOption(PLAYEROPTION_SAFE_AUTOMATION);
	const bool bFromCity = pUnit != NULL && pUnit->plot()->isCity();

	BuildTypes eBestBuild = NO_BUILD;
	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;
	int iBestIndex = MAX_INT;
	std::vector<std::pair<int,int> > fallbackProposals;
	const CvPlot* pRejectedPlot = NULL;

	for (int iPass = 0; iPass < 2; iPass++)
	{
		for (int slot = 0; slot < pCity->getNumCityPlots(); ++slot)
		{
			const int iI = iPass == 0 ? slot : fallbackProposals[slot].second;
			const bool bWinsTie = iBestValue > 0 && iI < iBestIndex;
			// The city already caches proposal values. Reject non-contenders before
			// plot legality and canBuild; no world state changes during this search.
			int iValue = pCity->AI_getBestBuildValue(iI);
			if (iValue < iBestValue || (iValue == iBestValue && !bWinsTie))
			{
				continue;
			}
			CvPlot* pLoopPlot = plotCity(pCity->getX(), pCity->getY(), iI);

			if (pLoopPlot != NULL && pLoopPlot != pRejectedPlot && pLoopPlot->getWorkingCity() == pCity && AI_plotValid(pLoopPlot) && pLoopPlot != pIgnorePlot
				&& (!bSafeAutomation || pLoopPlot->getImprovementType() == NO_IMPROVEMENT || pLoopPlot->getImprovementType() == eRuins))
			{
				if (iValue > iBestValue || (bWinsTie && iValue == iBestValue))
				{
					const BuildTypes eBuild = pCity->AI_getBestBuild(iI);

					if (eBuild == NO_BUILD || !canBuild(pLoopPlot, eBuild))
					{
						continue;
					}
					if (iPass == 0)
					{
						iBestValue = iValue;
							iBestIndex = iI;
						pBestPlot = pLoopPlot;
						eBestBuild = eBuild;
						continue;
					}
					// Fallback must path to this candidate, not the failed first choice.
					// Exhausted movement can only lower the final score, so this is
					// an optimistic bound and cannot discard a winning assignment.
					int iMaxTurns = workerImprovementPathLimit(iValue, 1, iBestValue - (bWinsTie ? 1 : 0));
					const int iTargetWorkers = kOwner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD, getGroup());
					if (iTargetWorkers >= 1)
					{
						// Distant assignments allow just one worker. Only a trip
						// of at most one turn can gain the nearby stacking allowance.
						const int iNearbyWorkers = std::max(1, AI_calculatePlotWorkersNeeded(pLoopPlot, eBuild) + (bFromCity ? 10 : 0));
						iMaxTurns = iTargetWorkers < iNearbyWorkers ? std::min(iMaxTurns, 1) : -1;
					}
					int iPathTurns;
					if (iMaxTurns >= 0 && !pLoopPlot->isVisibleEnemyUnit(this)
						&& generatePath(pLoopPlot, isHuman() ? 0 : MOVE_WITH_CAUTION, true, &iPathTurns, iMaxTurns))
					{
						// XXX take advantage of range (warning... this could lead to some units doing nothing...)
						int iMaxWorkers = 1;
						if (getPathMovementRemaining() == 0)
						{
							iPathTurns++;
						}
						else if (iPathTurns <= 1)
						{
							iMaxWorkers = AI_calculatePlotWorkersNeeded(pLoopPlot, eBuild);
						}
						if (bFromCity && iPathTurns == 1 && getPathMovementRemaining() > 0)
						{
							iMaxWorkers += 10;
						}
						if (iTargetWorkers < iMaxWorkers)
						{
							// XXX - This could be improved greatly by looking at the real build time
							//	and other factors when deciding whether to stack.
							iValue /= 1 + iPathTurns;
							if (iValue > iBestValue || (bWinsTie && iValue == iBestValue))
							{
								iBestValue = iValue;
								iBestIndex = iI;
								pBestPlot = pLoopPlot;
								eBestBuild = eBuild;
							}
						}
					}
				}
			}
		}
		if (0 == iPass && eBestBuild == NO_BUILD) break;
		if (0 == iPass && eBestBuild != NO_BUILD)
		{
			const int iTargetWorkers = kOwner.AI_plotTargetMissionAIs(pBestPlot, MISSIONAI_BUILD, getGroup());
			const int iRemoteWorkers = bFromCity ? 11 : 1;
			int iMaxTurns = MAX_INT;
			if (iTargetWorkers >= iRemoteWorkers)
			{
				const int iNearbyWorkers = std::max(iRemoteWorkers, AI_calculatePlotWorkersNeeded(pBestPlot, eBestBuild) + (bFromCity ? 10 : 0));
				iMaxTurns = iTargetWorkers < iNearbyWorkers ? 1 : -1;
			}
			int iPathTurns;
			if (iMaxTurns >= 0 && !pBestPlot->isVisibleEnemyUnit(this)
				&& generatePath(pBestPlot, isHuman() ? 0 : MOVE_WITH_CAUTION, true, &iPathTurns, iMaxTurns))
			{
				int iMaxWorkers = 1;
				if (getPathMovementRemaining() == 0)
				{
					iPathTurns++;
				}
				else if (iPathTurns <= 1)
				{
					iMaxWorkers = AI_calculatePlotWorkersNeeded(pBestPlot, eBestBuild);
				}
				if (bFromCity)
				{
					iMaxWorkers += 10;
				}

				if (iTargetWorkers < iMaxWorkers)
				{
					break; // Good to go.
				}
			}
			// A first choice rejected by reachability, enemy or worker occupancy
			// cannot pass the fallback's equal or stricter occupancy allowance.
			pRejectedPlot = pBestPlot;
			eBestBuild = NO_BUILD;
			iBestValue = 0;
			iBestIndex = MAX_INT;
			// Establish useful score bounds before expensive fallback paths.
			// Keep original plot-index ties explicitly, regardless of sort order.
			for (int i = 0; i < pCity->getNumCityPlots(); ++i)
				fallbackProposals.push_back(std::make_pair(pCity->AI_getBestBuildValue(i), i));
			std::sort(fallbackProposals.rbegin(), fallbackProposals.rend());
		}
	}
	if (NO_BUILD != eBestBuild)
	{
		FAssert(NULL != pBestPlot);
		if (ppBestPlot != NULL)
		{
			*ppBestPlot = pBestPlot;
		}
		if (peBestBuild != NULL)
		{
			*peBestBuild = eBestBuild;
		}
	}
	return (NO_BUILD != eBestBuild);
}

bool CvUnitAI::AI_isCityAIType() const
{
	return (
			AI_getUnitAIType() == UNITAI_CITY_DEFENSE
		|| AI_getUnitAIType() == UNITAI_CITY_COUNTER
		|| AI_getUnitAIType() == UNITAI_CITY_SPECIAL
		|| AI_getUnitAIType() == UNITAI_RESERVE
		|| AI_getUnitAIType() == UNITAI_PILLAGE_COUNTER
		|| AI_getUnitAIType() == UNITAI_PROPERTY_CONTROL
	);
}


int CvUnitAI::AI_getBirthmark() const
{
	return m_iBirthmark;
}

void CvUnitAI::AI_setBirthmark(int iNewValue)
{
	m_iBirthmark = iNewValue;
}


UnitAITypes CvUnitAI::AI_getUnitAIType() const
{
	// A unit should never have no unitAI, so if that state is found to exist set it to its default AI.
	//	- Empirically it's been seen but the underlying cause has not yet been found. -
	if (m_eUnitAIType == NO_UNITAI)
	{
		FErrorMsg("Unit has no UnitAI!");

		((CvUnitAI*)this)->m_eUnitAIType = m_pUnitInfo->getDefaultUnitAIType();

		area()->changeNumAIUnits(getOwner(), m_eUnitAIType, 1);
		GET_PLAYER(getOwner()).AI_changeNumAIUnits(m_eUnitAIType, 1);
	}
	return m_eUnitAIType;
}


// XXX make sure this gets called...
void CvUnitAI::AI_setUnitAIType(UnitAITypes eNewValue)
{
	FAssertMsg(eNewValue != NO_UNITAI, "NewValue is not assigned a valid value");

	if (AI_getUnitAIType() != eNewValue)
	{
		area()->changeNumAIUnits(getOwner(), AI_getUnitAIType(), -1);
		GET_PLAYER(getOwner()).AI_changeNumAIUnits(AI_getUnitAIType(), -1);

		m_eUnitAIType = eNewValue;
        if(!isTempUnit())GET_PLAYER(getOwner()).getContractBroker().noteDefenseGroupChanged(getGroup());
        DefenseNeedEvents::changed();

		area()->changeNumAIUnits(getOwner(), AI_getUnitAIType(), 1);
		GET_PLAYER(getOwner()).AI_changeNumAIUnits(AI_getUnitAIType(), 1);

		joinGroup(NULL);
	}
}

int CvUnitAI::AI_sacrificeValue(const CvPlot* pPlot) const
{
	PROFILE_EXTRA_FUNC();
	if (getDomainType() == DOMAIN_AIR)
	{
		int iValue = 128 * (100 + currInterceptionProbability());
		if (m_pUnitInfo->getNukeRange() != -1)
		{
			iValue += 25000;
		}
		iValue /= std::max(1, (1 + m_pUnitInfo->getProductionCost()));
		iValue *= getMaxHP() - getDamage();
		return iValue / 100;
	}
	int iValue = 128 * currEffectiveStr(pPlot, ((pPlot == NULL) ? NULL : this));

	if (iValue > 0)
	{
		int iCollateralDamageValue = 0;
		if (pPlot != NULL)
		{
			const int iPossibleTargets = std::min(pPlot->getNumVisiblePotentialEnemyDefenders(this) - 1, collateralDamageMaxUnits());

			if (iPossibleTargets > 0)
			{
				iCollateralDamageValue = collateralDamage() + std::max(0, iCollateralDamageValue - 100);
				iCollateralDamageValue *= iPossibleTargets;
				iCollateralDamageValue /= 5;
			}
		}
		//  TB Combat Mods:
		//  Determine Withdraw odds
		const int AdjustedWithdrawalstep1 = withdrawalProbability();
		const int AdjustedWithdrawalstep2 = AdjustedWithdrawalstep1 > 100 ? 100 : AdjustedWithdrawalstep1;
		const int AdjustedWithdrawal = AdjustedWithdrawalstep2 < 0 ? 0 : AdjustedWithdrawalstep2;
		const int timePercentage = ((100 - earlyWithdrawTotal()) > 100 ? 100 : (100 - earlyWithdrawTotal())); // InitialEarlyWithdrawPercentage

		//TB Combat Mods (Pursuit and Early Withdraw)
		//first method employed - keeping simply commented out as backup
		//int AdjustedEarlyWithdraw = (((timePercentage < 0) ? 0 : timePercentage) * AdjustedWithdrawal);
		//int EvaluationalWithdrawOdds = AdjustedEarlyWithdraw + AdjustedWithdrawal;

		// Simplification: expected round count of 4 (4/100 = 1/25)
		const int expectedRnds = 1 + timePercentage / 25;

		int y = AdjustedWithdrawal;
		int z = AdjustedWithdrawal;
		for (int Time = 0; Time < expectedRnds; ++Time)
		{
			z += AdjustedWithdrawal * y / 100;
			y = AdjustedWithdrawal * (100 - z) / 100; // Prob next round is prob per round times prob you haven't already
		}
		const int EvaluatedWithdrawOdds = z;

		//* figure out odds of knockback
		const int iAttackerKnockback = knockbackTotal();
		const int iAttackerKnockbackTries = knockbackRetriesTotal();
		const int iKnockbackZero = iAttackerKnockback < 0 ? 0 : iAttackerKnockback;
		const int iKnockbackTotal = iKnockbackZero > 100 ? 100 : iKnockbackZero;

		y = iKnockbackTotal;
		z = iKnockbackTotal;
		for (int Time = 0; Time < iAttackerKnockbackTries; ++Time)
		{
			z += iKnockbackTotal * y / 100;
			y = iKnockbackTotal * (100 - z) / 100; // Prob next round is prob per round times prob you haven't already
		}
		const int EvaluatedKnockbackOdds = z;

		iValue *= 100 + iCollateralDamageValue;
		iValue /= 100 + cityDefenseModifier();
		iValue *= 100 + EvaluatedWithdrawOdds;
		iValue /= 100;
		iValue *= 100 + EvaluatedKnockbackOdds;
		iValue /= 100;
		//TB Combat Mods End (above EvaluationalWithdrawOdds replaces EvaluatedWithdrawOdds)

		iValue /= (10 + getExperience());

		// Value units which can't kill units later, also combat limits mean higher survival odds
		if (combatLimit() < 100)
		{
			iValue *= 150;
			iValue /= 100;

			iValue *= 100;
			iValue /= std::max(1, combatLimit());
		}
		iValue /= std::max(1, 1 + m_pUnitInfo->getProductionCost());
	}
	return iValue;
}

// Protected Functions...

void CvUnitAI::AI_animalMove()
{
	PROFILE_FUNC();

	const bool bReckless = GC.getGame().isOption(GAMEOPTION_ANIMAL_RECKLESS);

	if (canAttack())
	{
		if (bReckless)
		{
			if (AI_anyAttack(2, 0, 0, false))
			{
				return;
			}
		}
		// Recklessness based on animal aggression.
		// 1% odds assessment is there to account for some small understanding of likelihood of success even in an aggressive action.
		else if (
			GC.getGame().getSorenRandNum(10, "Animal Attack")
			<
			getMyAggression(GC.getHandicapInfo(GC.getGame().getHandicapType()).getAnimalAttackProb()))
		{
			if (AI_anyAttack(2, 1, 0, false))
			{
				return;
			}
		}
		// Animals are pretty good at assessing their chances of taking down prey, therefore 60% odds prereq.
		else if (
			GC.getGame().getSorenRandNum(100, "Animal Attack")
			<
			GC.getHandicapInfo(GC.getGame().getHandicapType()).getAnimalAttackProb())
		{
			if (AI_anyAttack(2, 60, 0, false))
			{
				return;
			}
		}
	}
	if (!bReckless && exposedToDanger(plot(), 60, true) && AI_safety())
	{
		return;
	}
	if (AI_heal() || AI_patrol(bReckless))
	{
		return;
	}
	LOG_UNIT_BLOCK(4, {
		UnitAITypes eUnitAi = AI_getUnitAIType();
		MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = m_szName;
		if (StrUnitName.length() == 0)
		{
			StrUnitName = getName(0).GetCString();
		}

		logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Skip reached for animalMove...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits());
	});
	getGroup()->pushMission(MISSION_SKIP);
}

bool CvUnitAI::AI_SettleFirstCity()
{
	PROFILE_EXTRA_FUNC();
	// Afforess & Fuyu - Check for Good City Sites Near Starting Location
	const int iGameSpeedPercent = GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
	const int iMaxFoundTurn = iGameSpeedPercent / 100;

	if (canMove() && !GET_PLAYER(getOwner()).AI_isPlotCitySite(plot()) && GC.getGame().getElapsedGameTurns() <= iMaxFoundTurn)
	{
		int iBestValue = 0;
		int iBestFoundTurn = 0;
		CvPlot* pBestPlot = NULL;

		for (int iCitySite = 0; iCitySite < GET_PLAYER(getOwner()).AI_getNumCitySites(); iCitySite++)
		{
			CvPlot* pCitySite = GET_PLAYER(getOwner()).AI_getCitySite(iCitySite);
			if (canFound(pCitySite) && (UnitTravelConnectivity::landUnitCanReach(this,pCitySite) || canMoveAllTerrain()))
			{
				int iPlotValue = pCitySite->getFoundValue(getOwner());
				if (iPlotValue > iBestValue)
				{
					//Can this unit reach the plot this turn? (getPathLastNode()->m_iData2 == 1)
					//Will this unit still have movement points left to found the city the same turn? (getPathLastNode()->m_iData1 > 0))
					int iPathTurns;
					if (generatePath(pCitySite, MOVE_NO_ENEMY_TERRITORY, false, &iPathTurns))
					{
						const int iFoundTurn = GC.getGame().getElapsedGameTurns() + iPathTurns - (getPathMovementRemaining() > 0 ? 1 : 0);
						if (iFoundTurn <= iMaxFoundTurn)
						{
							iPlotValue *= 100; //more precision
							//the slower the game speed, the less penalty the plotvalue gets for long walks towards it. On normal it's -18% per turn
							iPlotValue *= 100 - std::min(100, iFoundTurn * 1800 / iGameSpeedPercent);
							iPlotValue /= 100;
							if (iPlotValue > iBestValue)
							{
								iBestValue = iPlotValue;
								iBestFoundTurn = iFoundTurn;
								pBestPlot = pCitySite;
							}
						}
					}
				}
			}
		}

		if (pBestPlot != NULL)
		{
			//Don't give up coast or river, don't settle on bonus with food
			if (plot()->isRiver() && !pBestPlot->isRiver())
			{
				const int iOceanMinSize = GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize();

				if (plot()->isCoastalLand(iOceanMinSize) && !pBestPlot->isCoastalLand(iOceanMinSize)
					|| pBestPlot->getBonusType(NO_TEAM) != NO_BONUS
					&& pBestPlot->calculateNatureYield(YIELD_FOOD, getTeam(), true) > 0)
				{
					pBestPlot = NULL;
				}
			}
		}

		if (pBestPlot != NULL)
		{
			if (gUnitLogLevel >= 2)
			{
				logBBAI("	Settler not founding in place but moving %d, %d to nearby city site at %d, %d (%d turns away) with value %d)", (pBestPlot->getX() - getX()), (pBestPlot->getY() - getY()), pBestPlot->getX(), pBestPlot->getY(), iBestFoundTurn, iBestValue);
			}
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_SAFE_TERRITORY | MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_FOUND, pBestPlot);
			return true;
		}
	}
	// ! Afforess & Fuyu - Check for Good City Sites Near Starting Location

	if (canFound(plot()))
	{
		if (gUnitLogLevel >= 2)
		{
			logBBAI("	Settler (%d) founding in place due to no cities", getID());
		}
		getGroup()->pushMission(MISSION_FOUND);
		return true;
	}
	return false;
}


void CvUnitAI::AI_settleMove()
{
	PROFILE_FUNC();
	CvPlot* pEndTurnPlot = NULL;

	if (AI_selectStatus(true))
	{
		return;
	}

	if (isFound() && GET_PLAYER(getOwner()).getNumCities() == 0 && AI_SettleFirstCity())
	{
		return;
	}

	const int iDanger = GET_PLAYER(getOwner()).AI_getPlotDanger(plot(), 3);
	SendLog("SettlerAI", CvWString::format(L"iDanger check %I64d", iDanger));
	if (iDanger > 0)
	{
		if (!getGroup()->canDefend() && (plot()->getOwner() == getOwner() || iDanger > 2)
		|| GET_PLAYER(getOwner()).AI_getVisiblePlotDanger(plot(), 2, false, getGroup(), 70))
		{
			if (!getGroup()->canDefend() && AI_retreatToCity() || AI_safety())
			{
				return;
			}
			SendLog("SettlerAI", CvWString::format(L"already in city %I64d", iDanger));
			getGroup()->pushMission(MISSION_SKIP);
			return; // This safety decision has already spent the settler's turn.
		}
	}

	// Don't found new cities if that would cause more unhappiness when we already have happiness issues
	if (!isFound())
    {
        if(AI_retreatToCity())return;
        getGroup()->pushMission(MISSION_SKIP);return;
    }
	bool bInhibitFounding = false;


	if (GET_PLAYER(getOwner()).getCityLimit() > 0)
	{
		if (GET_PLAYER(getOwner()).getCityOverLimitUnhappy() > 0)
		{
			// Soft limit - If we already have unhappy cities don't create settlers
			//	that will increase overall unhappiness as they found new cities
			bInhibitFounding =
				(
					GET_PLAYER(getOwner()).getCityLimit() <= GET_PLAYER(getOwner()).getNumCities()
					&&
					GET_PLAYER(getOwner()).AI_getOverallHappyness(GET_PLAYER(getOwner()).getCityOverLimitUnhappy()) < 0
				);
		}
		else // Hard limit
		{
			bInhibitFounding = (GET_PLAYER(getOwner()).getCityLimit() <= GET_PLAYER(getOwner()).getNumCities());
		}
	}

	if (!bInhibitFounding)
	{
		bInhibitFounding = GET_PLAYER(getOwner()).AI_isFinancialTrouble();
		if (bInhibitFounding) SendLog("SettlerAI", CvWString::format(L"	Settler (%d) inhibited from founding due to financial difficulties", getID()));

		if (bInhibitFounding && gUnitLogLevel >= 2)
		{
			SendLog("SettlerAI", CvWString::format(L"	Settler (%d) inhibited from founding due to financial difficulties", getID()));
			logBBAI("	Settler (%d) inhibited from founding due to financial difficulties", getID());
		}
	}
	else if (gUnitLogLevel >= 2)
	{
		logBBAI("	Settler (%d) inhibited from founding due to city limit unhappyness", getID());
	}

	if (!bInhibitFounding)
	{
        if (AI_continueOverseasSettlement() || AI_continueSettlerEscort() || AI_continueFoundingMission()) return;
		int iAreaBestFoundValue = 0;
		int iOtherBestFoundValue = 0;
        CvPlot* pOverseasSite = NULL;
		SendLog("SettlerAI", CvWString::format(L"Number of City Sites %WS %I64d ID: %d X: %d Y: %d", GET_PLAYER(getOwner()).getCivilizationDescription(0), GET_PLAYER(getOwner()).AI_getNumCitySites(), getID(), getX(), getY()));
		for (int iI = 0; iI < GET_PLAYER(getOwner()).AI_getNumCitySites(); iI++)
		{
			CvPlot* pCitySitePlot = GET_PLAYER(getOwner()).AI_getCitySite(iI);

			// Only count city sites we can get to
			if (canFound(pCitySitePlot) && (UnitTravelConnectivity::landUnitCanReach(this,pCitySitePlot) || canMoveAllTerrain())
                && generatePath(pCitySitePlot, MOVE_SAFE_TERRITORY | MOVE_NO_ENEMY_TERRITORY, true))
			{
				if (plot() == pCitySitePlot && canFound(plot()))
				{
					SendLog("SettlerAI", CvWString::format(L"Settler %S (%d) founding in place since it's at a city site %d, %d", GET_PLAYER(getOwner()).getCivilizationDescription(0), getID(), getX(), getY()));
					if (gUnitLogLevel >= 2)
					{
						logBBAI("	Settler (%d) founding in place since it's at a city site %d, %d", getID(), getX(), getY());
					}
					getGroup()->pushMission(MISSION_FOUND);
					return;
				}
				// K-Mod. If we are already heading to this site, then keep going!
				// This change fixes a bug which prevented settlers from targeting the same site two turns in a row!
				else
				{
					CvPlot* pMissionPlot = getGroup()->AI_getMissionAIPlot();
					if (pMissionPlot == pCitySitePlot && getGroup()->AI_getMissionAIType() == MISSIONAI_FOUND)
					{
						// safety check. (cf. conditions in AI_found)
						if (getGroup()->canDefend() && getGroup()->getStrength() > (AI_minSettlerDefense() / 2))
						{
							if (gUnitLogLevel >= 2)
							{
								logBBAI("	Settler (%d) continuing mission to %d, %d, current group strength (%d)", getID(), pCitySitePlot->getX(), pCitySitePlot->getY(), getGroup()->getStrength());
							}
							pEndTurnPlot = getPathEndTurnPlot();
							if (pEndTurnPlot != NULL)
							{
								SendLog("SettlerAI", CvWString::format(L"Settler (%d) from %S continuing mission to %d, %d, current group strength (%d)", getID(), GET_PLAYER(getOwner()).getCivilizationDescription(0), pCitySitePlot->getX(), pCitySitePlot->getY(), getGroup()->getStrength()));
								getGroup()->pushMission(MISSION_MOVE_TO, pEndTurnPlot->getX(), pEndTurnPlot->getY(), MOVE_SAFE_TERRITORY | MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_FOUND, pCitySitePlot);
								return;
							}
							getGroup()->pushMission(MISSION_SKIP);
							return;
						}
					}
				}
				// K-Mod end
				iAreaBestFoundValue = std::max(iAreaBestFoundValue, pCitySitePlot->getFoundValue(getOwner()));
			}
			// An overseas site needs a ship even when the settler itself cannot
            // fight. A failed local path alone must not invent an overseas need
            // for an unescorted civilian (e.g. a dangerous land route).
            else if (AI_canPlanColony() && canFound(pCitySitePlot) && pCitySitePlot->isCoastalLand()
                && !pCitySitePlot->isVisibleEnemyUnit(this)
                && GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pCitySitePlot, MISSIONAI_LOAD_SETTLER, getGroup()) == 0
                && !UnitTravelConnectivity::landUnitCanReach(this, pCitySitePlot)
                && pCitySitePlot->getFoundValue(getOwner()) > iOtherBestFoundValue)
            {
                iOtherBestFoundValue = pCitySitePlot->getFoundValue(getOwner());
                pOverseasSite = pCitySitePlot;
            }
		}

		// No new settling of colonies when AI is in financial trouble
		if (plot()->isCity() && plot()->getOwner() == getOwner() && GET_PLAYER(getOwner()).AI_isFinancialTrouble())
		{
			iOtherBestFoundValue /= 4;
		}

		if (iAreaBestFoundValue == 0 && iOtherBestFoundValue == 0
		&& GC.getGame().getGameTurn() - getGameTurnCreated() > intSqrt(GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent()))
		{
			if (NULL != getTransportUnit())
			{
				getTransportUnit()->unloadAll();
			}

			if (NULL == getTransportUnit()
			&& GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(getGroup()->getHeadUnit(), MISSIONAI_PICKUP) == 0)
			{
				scrap(); //may seem wasteful, but settlers confuse the AI.
				return;
			}
		}

		if (iOtherBestFoundValue * 100 > iAreaBestFoundValue * 110 && plot()->getOwner() == getOwner()
		&& pOverseasSite && AI_loadForColony(pOverseasSite))
		{
			return;
		}

		if (iAreaBestFoundValue > 0 && plot()->isBestAdjacentFound(getOwner()) && canFound(plot()))
		{
			if (gUnitLogLevel >= 2)
			{
				logBBAI("	Settler (%d) founding in place due to best adjacent found", getID());
			}
			getGroup()->pushMission(MISSION_FOUND);
			return;
		}

		if (!getGroup()->canDefend() || getGroup()->getStrength() <= AI_minSettlerDefense())
		{
			//	If we're already in a city advertise for an escort
			if (plot()->isCity() && (plot()->getOwner() == getOwner()) && !isCargo())
			{
				std::ostringstream stringStream;
				stringStream << "Settler (" << getID() << ") for player" << getOwner() << "(" << GET_PLAYER(getOwner()).getCivilizationDescription(1) << ") at" << "(" << getX() << "," << getY() << ")" << "requesting defensive escort, current group strength (" << getGroup()->getStrength() << ")" << AI_minSettlerDefense() << std::endl;
				SendLog("SettlerAI", stringStream.str());
				if (m_contractsLastEstablishedTurn != GC.getGame().getGameTurn())
				{
					if (gUnitLogLevel >= 3)
					{
						logBBAI("	Settler (%d) for player %d (%S) at (%d,%d) requesting defensive escort, current group strength (%d)\n",
							getID(), getOwner(),
							GET_PLAYER(getOwner()).getCivilizationDescription(0),
							getX(), getY(),
							getGroup()->getStrength()
						);
					}
					//	For now advertise the work as being here since we'll be holding position
					//	while we wait for a defender.  It would be more optimal to 'meet' the defender
					//	on the way or at the target plot, but this might leave us exposed on the way so
					//	the calculations involved are somewhat harder and not attempted for now
					GET_PLAYER(getOwner()).getContractBroker().advertiseWork(

						HIGHEST_PRIORITY_ESCORT_PRIORITY,
						OFFENSIVE_UNITCAPABILITIES,
						getX(), getY(), this,
						UNITAI_ESCORT,
						AI_minSettlerDefense()
					);
					m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
					m_contractualState = CONTRACTUAL_STATE_AWAITING_ANSWER;
                    AI_rememberCivilianDecision(CivilianDecision::SETTLER_ESCORT, plot());
                    if (m_civilianDecision.kind == CivilianDecision::SETTLER_ESCORT)
                        getGroup()->AI_setMissionAI(MISSIONAI_WAIT_FOR_ESCORT, plot(), NULL);
					return;
				}
			}
		}

		if (plot()->isCity() && plot()->getOwner() == getOwner() && getGroup()->getNumUnits() < 3
		&& GC.getGame().getMaxCityElimination() > 0 && GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot()))
		{
			std::ostringstream stringStream;
			stringStream << "Settler (" << getID() << ") for player" << getOwner() << "(" << GET_PLAYER(getOwner()).getCivilizationDescription(0) << ") at" << "(" << getX() << "," << getY() << ")" << "skipping due to not enough units (" << getGroup()->getStrength() << ")" << GC.getGame().getMaxCityElimination() << GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot()) << std::endl;
			SendLog("SettlerAI", stringStream.str());
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}

		if (iAreaBestFoundValue > 0 && AI_found())
		{
			return;
		}

		if (plot()->getOwner() == getOwner())
		{
			if (pOverseasSite && AI_loadForColony(pOverseasSite))
			{
				SendLog("SettlerAI", "tries to board ship");
				return;
			}
			// BBAI TODO: Go to a good city (like one with a transport) ...
		}
	}

	if (!getGroup()->canDefend() && AI_retreatToCity())
	{
		return;
	}

	// Colonization transport is requested only for a selected legal city site.

	if (AI_safety())
	{
		return;
	}
	LOG_UNIT_BLOCK(4, {
		UnitAITypes eUnitAi = AI_getUnitAIType();
		MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = m_szName;
		if (StrUnitName.length() == 0)
		{
			StrUnitName = getName(0).GetCString();
		}

		logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Skip reached for settleMove...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits());
	});
	getGroup()->pushMission(MISSION_SKIP);
	return;
}


int CvUnitAI::AI_minSettlerDefense() const
{
	return (
		GET_PLAYER(getOwner()).strengthOfBestUnitAI(DOMAIN_LAND, UNITAI_CITY_DEFENSE)
		*
		(
			// One defender if game-start settler(s), else 2.
			1 /* + 1 * (getGameTurnCreated() > 0)*/
			+ // Two more if at war (ignore minor civ).
			1 * GET_TEAM(getTeam()).isAtWar()
			)
	);
}

bool CvUnitAI::IsAbroad() {
	return plot()->getOwner() != getOwner();
}

bool CvUnitAI::AI_upgradeWorker() {
	AI_PERF_SCOPE(WORKER_UPGRADE);

	const int64_t iGold = GET_PLAYER(getOwner()).getGold();
	const int64_t iTarget = GET_PLAYER(getOwner()).AI_goldTarget();
	return (
		iGold > iTarget
		// Whether we let it try to be comsumed by an upgrade depends on how much spare cash we have
		&& GC.getGame().getSorenRandNum(100, "AI upgrade worker") < 100 * (iGold - iTarget) / iGold
		// Try to upgrade this unit
		&& AI_upgrade()
	);
}

int CvUnitAI::GetNumberOfUnitsInGroup() {
	return getGroup()->getNumUnits();
}


void CvUnitAI::AI_workerMove()
{
	AI_PERF_SCOPE(WORKER_MOVE);
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}
	const bool bAbroad = IsAbroad();
	// If worker (or captive) is not defended, and is outside own borders.
	if (bAbroad)
	{
		if (getGroup()->canDefend())
		{
			if (!isHuman() && AI_workerReleaseDefenderIfNotNeeded())
			{
				OutputDebugString(CvString::format("%S (%d) AI_workerReleaseDefenderIfNotNeeded 1 (%d,%d)...\n", getDescription().c_str(), m_iID, m_iX, m_iY).c_str());

				if (AI_reachHome())
				{
					OutputDebugString(CvString::format("%S (%d) AI_reachHome 1 true (%d,%d)...\n", getDescription().c_str(), m_iID, m_iX, m_iY).c_str());
					return;
				}
			}
		}
		else if (AI_workerNeedsDefender(plot()))
		{
			// If I can reach safety on my own this turn then don't bother other units with our escape.
			if (AI_reachHome())
			{
				OutputDebugString(CvString::format("%S (%d) AI_reachHome 2 true (%d,%d)...\n", getDescription().c_str(), m_iID, m_iX, m_iY).c_str());
				return;
			}
			OutputDebugString(CvString::format("%S (%d) AI_reachHome 2 false (%d,%d)...\n", getDescription().c_str(), m_iID, m_iX, m_iY).c_str());

			// Prefer heading home over loitering for an escort. An undefended worker
            // is "abroad" the moment it strays onto another player's tile (IsAbroad ==
            // plot owner != us), e.g. when its path to an own city/bonus cut through a
            // neighbour's borders. Make for one of our own cities first; parking next
            // to the foreign city to wait for a passing military unit to adopt it is
            // what left AI workers stuck in other players' cities.
            if (AI_retreatToCity())
            {
                return;
            }

            // Only if we cannot path home at all do we fall back to grabbing a local
            // escort. Take control (LEADER_PRIORITY_MAX) so the escort comes to us
            // rather than dragging the worker even further from home.
			AI_setLeaderPriority(LEADER_PRIORITY_MAX);

			if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK).ignoreFaster().ignoreOwnUnitType().maxPathTurns(1)))
			{
				return;
			}
			if (AI_group(GroupingParams().withUnitAI(UNITAI_HUNTER).ignoreOwnUnitType().maxPathTurns(1)))
			{
				return;
			}
			if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK_CITY).ignoreFaster().ignoreOwnUnitType().maxPathTurns(1)))
			{
				return;
			}
			if (AI_group(GroupingParams().withUnitAI(UNITAI_COUNTER).ignoreFaster().ignoreOwnUnitType().maxPathTurns(1)))
			{
				return;
			}
			AI_setLeaderPriority(-1); // We didn't get to group so back to normal

		}
		// Toffer - After reaching this point the worker will always start making route from here to friendly territory... May be better things to do elsewhere.
		//	Should check if improving this neutal plot is worthwhile and if not just start going to a more worthwhile plot/job.
	}
	else if (!isHuman() && AI_workerReleaseDefenderIfNotNeeded())
	{
		OutputDebugString(CvString::format("%S (%d) AI_workerReleaseDefenderIfNotNeeded 2 (%d,%d)...\n", getDescription().c_str(), m_iID, m_iX, m_iY).c_str());
		return;
	}
	//ls612: Combat Worker Danger Evaluation
	const bool bWorkerDanger = bAbroad && GET_PLAYER(getOwner()).AI_isPlotThreatened(plot(), 2) || !bAbroad && exposedToDanger(plot(), 80, false);

	// Don't abandon an improvement we've already started just because an enemy unit came into view;
	// only flee danger when we don't have build progress on this plot invested yet.
	if (bWorkerDanger && (!canDefend() || GetNumberOfUnitsInGroup() == 1) && getBuildType() == NO_BUILD) {
		// in this order, either retreat to safety, or go into a city
		if (AI_safety() || AI_retreatToCity()) {
			return;
		}
	}

    if (AI_continueWorkerRoute()) return;

	// TODO: move out of workermove and into new unitai called UNITAI_MISSIONUNIT ?
	if (AI_heritage() || AI_construct(MAX_INT, MAX_INT, 0, true))
	{
		OutputDebugString(CvString::format("%S (%d) chooses to head off to construct\n", getDescription().c_str(), m_iID).c_str());
		return;
	}
	if (AI_upgradeWorker())
	{
		return;
	}

	if (AI_hurry(true))
	{
		return;
	}

	// TODO: move out of workermove and into new unitai called UNITAI_MISSIONUNIT ?
	if (AI_outcomeMission())
	{
		OutputDebugString(CvString::format("%S (%d) chooses to head off to do an outcome mission\n", getDescription().c_str(), m_iID).c_str());
		return;
	}

	if (!getGroup()->canDefend() && (isHuman() && GET_PLAYER(getOwner()).AI_isPlotThreatened(plot(), 2) || !isHuman() && AI_workerNeedsDefender(plot())) && AI_retreatToCity()) // XXX maybe not do this??? could be working productively somewhere else...
	{
		return;
	}

	if (!isHuman() && AI_bonusPlantMove()) return;

	bool bCanRoute = canBuildRoute();
	// Workboats don't build Sea Tunnels over Resources
	if (bCanRoute && !bAbroad && getDomainType() != DOMAIN_SEA)
	{
		const BonusTypes eNonObsoleteBonus = plot()->getNonObsoleteBonusType(getTeam());

		if (NO_BONUS != eNonObsoleteBonus
		&& !plot()->isConnectedToCapital()
		&& NO_IMPROVEMENT != plot()->getImprovementType()
		&& GC.getImprovementInfo(plot()->getImprovementType()).isImprovementBonusTrade(eNonObsoleteBonus)
		&& AI_connectPlot(plot()))
		{
			return;
		}
	}

	CvPlot* pBestBonusPlot = NULL;
	BuildTypes eBestBonusBuild = NO_BUILD;
	int iBestBonusValue = 0;

	// find bonuses within 2 moves to improve
	if (CvWorkerService::ImproveBonus(this, 2))
	{
		return;
	}

	// Afforess - worker financial trouble check
	if (!isHuman() && AI_getUnitAIType() == UNITAI_WORKER
	&& GET_PLAYER(getOwner()).AI_isFinancialTrouble()
	&& GET_PLAYER(getOwner()).getUnitUpkeepNet(isMilitaryBranch(), getUpkeep100()) > 0)
	{
		const int iWorkers = GET_PLAYER(getOwner()).AI_totalUnitAIs(UNITAI_WORKER);

		if (iWorkers > 3 && iWorkers > 2 * GET_PLAYER(getOwner()).getNumCities())
		{
			if (gUnitLogLevel > 2)
			{
				logBBAI(
					"%S's %S at (%d,%d) is disbanding itself due to large number of workers available, and financial trouble.",
					GET_PLAYER(getOwner()).getCivilizationDescription(0), getName(0).GetCString(), getX(), getY()
				);
			}
			scrap();
			return;
		}
	}

	CvCity* pCity = NULL;

	if (!bAbroad)
	{
		pCity = plot()->getPlotCity(); //get city on plot

		if (!pCity) //if city is not on same plot
		{
			pCity = plot()->getWorkingCity(); //get city that is owning this plot
		}

		if (pCity)
		{
			if (CvWorkerService::ShouldImproveCity(pCity) && AI_improveCity(pCity))
			{
				return;
			}

			if (pCity->AI_getWorkersNeeded() > 0 && (plot()->isCity() || pCity->AI_getWorkersNeeded() < (1 + pCity->getNumWorkers() * 2) / 3) && AI_improveCity(pCity))
			{
				return;
			}
		}
	}

	if (AI_improveLocalPlot(2, pCity))
	{
		return;
	}

	// find bonuses within 4 moves to improve
	if (CvWorkerService::ImproveBonus(this))
	{
		return;
	}

	{
		const bool bNextCity = pCity == NULL || pCity->AI_getWorkersNeeded() == 0 || pCity->getNumWorkers() > pCity->AI_getWorkersNeeded() + 1;

		if (bNextCity)
		{
			if (pBestBonusPlot != NULL && iBestBonusValue >= 15 && AI_improvePlot(pBestBonusPlot, eBestBonusBuild))
			{
				return;
			}

			if (AI_nextCityToImprove(pCity))
			{
				return;
			}
		}

		if (pBestBonusPlot != NULL && AI_improvePlot(pBestBonusPlot, eBestBonusBuild))
		{
			return;
		}

		if (pCity != NULL && AI_improveCity(pCity))
		{
			return;
		}

		if (!bNextCity && AI_nextCityToImprove(pCity))
		{
			return;
		}
	}

	if (bCanRoute)
	{
		if (AI_routeTerritory(true))
		{
			return;
		}

		if (AI_connectBonus(false))
		{
			return;
		}

		if (AI_routeCity())
		{
			return;
		}
	}

	if (AI_improveLocalPlot(3, NULL))
	{
		return;
	}

	if (AI_irrigateTerritory())
	{
		return;
	}

	if (bCanRoute && AI_routeTerritory())
	{
		return;
	}

	if (!isHuman() || isAutomated() && !GET_TEAM(getTeam()).isAtWar())
	{
		if (!isHuman() || getGameTurnCreated() < GC.getGame().getGameTurn())
		{
			if (AI_nextCityToImproveAirlift())
			{
				return;
			}
		}
		if (!isHuman())
		{
			// Fill up boats which already have workers
			if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_WORKER, -1, -1, -1, -1, MOVE_SAFE_TERRITORY))
			{
				return;
			}

			// Avoid filling a galley which has just a settler in it, reduce chances for other ships
			if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
			{
				if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, NO_UNITAI, -1, (2 * GET_PLAYER(getOwner()).getBestUnitTypeCargoVolume(UNITAI_WORKER)), -1, -1, MOVE_SAFE_TERRITORY))
				{
					return;
				}
			}
			else if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, NO_UNITAI, -1, 2, -1, -1, MOVE_SAFE_TERRITORY))
			{
				return;
			}
		}
	}


	if (!isHuman() && !isNPC())
	{
		if (!bAbroad && AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, 2, -1, -1, 0, MOVE_SAFE_TERRITORY))
		{
			return;
		}
	}

	if (!isHuman() && AI_getUnitAIType() == UNITAI_WORKER)
	{
		const int iWorkers = GET_PLAYER(getOwner()).AI_totalUnitAIs(UNITAI_WORKER);

		if (iWorkers > 5 && iWorkers > 5 * GET_PLAYER(getOwner()).getNumCities()
		&& GET_PLAYER(getOwner()).AI_isFinancialTrouble()
		&& GET_PLAYER(getOwner()).getUnitUpkeepNet(isMilitaryBranch(), getUpkeep100()) > 0)
		{
			if (gUnitLogLevel > 2)
			{
				logBBAI("	%S's %S at (%d,%d) is disbanding itself due to large number of workers available", GET_PLAYER(getOwner()).getCivilizationDescription(0), getName(0).GetCString(), getX(), getY());
			}
			scrap();
			return;
		}
	}

	if (AI_retreatToCity(false, true))
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (getGroup()->isStranded() && AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}
	LOG_UNIT_BLOCK(4, {
		UnitAITypes eUnitAi = AI_getUnitAIType();
		MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = m_szName;
		if (StrUnitName.length() == 0)
		{
			StrUnitName = getName(0).GetCString();
		}

		logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Skip reached for workerMove...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits());
	});
	getGroup()->pushMission(MISSION_SKIP);
}

void CvUnitAI::AI_barbAttackMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	if (AI_anyAttack(0, 60))
	{
		return;
	}

	if (AI_anyAttack(1, 70))
	{
		return;
	}

	if (plot()->isGoody())
	{
		if (AI_anyAttack(1, 80))
		{
			return;
		}

		if (plot()->plotCount(PUF_isUnitAIType, UNITAI_ATTACK, -1, NULL, getOwner()) == 1 && getGroup()->getNumUnits() == 1)
		{
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	if (GC.getGame().getSorenRandNum(3, "AI Barb") == 0 && AI_pillageRange(1))
	{
		return;
	}

	const bool bRaging = GC.getGame().isOption(GAMEOPTION_BARBARIAN_RAGING);

	if (bRaging || GC.getGame().getNumCivCities() > GC.getGame().countCivPlayersAlive() * 2)
	{
		if (AI_anyAttack(1, 20)
		|| AI_cityAttack(2, 15)
		|| AI_pillageRange(4)
		|| AI_cityAttack(3, 10))
		{
			return;
		}
	}
	else if (
		AI_anyAttack(1, 30)
	|| AI_pillageRange(2)
	|| AI_cityAttack(1, 20))
	{
		return;
	}

	const AreaAITypes eAreaAI = area()->getAreaAIType(getTeam());

	if (eAreaAI == AREAAI_OFFENSIVE)
	{
		if (AI_groupMergeRange(UNITAI_ATTACK, 2, true, true, true)
		|| AI_groupMergeRange(UNITAI_ATTACK_CITY, 4, true, true, true)
		|| AI_groupMergeRange(UNITAI_COLLATERAL, 4, true, true, true))
		{
			return;
		}
		if (AI_goToTargetCity(0, MAX_BARB_TARGET_CITY_RANGE)
		|| AI_goToTargetCity(0, MAX_INT, area()->getTargetCity(getOwner())))
		{
			return;
		}
	}
	else if (eAreaAI == AREAAI_ASSAULT)
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK_CITY, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
		{
			return;
		}
	}

	if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 1))
	{
		return;
	}

	if (AI_heal())
	{
		return;
	}

	if (AI_patrol(bRaging))
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}
	LOG_UNIT_BLOCK(4, {
		UnitAITypes eUnitAi = AI_getUnitAIType();
		MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = m_szName;
		if (StrUnitName.length() == 0)
		{
			StrUnitName = getName(0).GetCString();
		}

		logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Skip reached for barAttacklMove...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits());
	});
	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_attackMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true) || checkSwitchToConstruct())
	{
		return;
	}

	if (MISSIONAI_REGROUP == getGroup()->AI_getMissionAIType())
	{
		if (AI_group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(2).maxPathTurns(1).allowRegrouping()))
		{
			return;
		}
		if (AI_groupMergeRange(UNITAI_ATTACK_CITY, 5, false, true))
		{
			return;
		}
		if (AI_groupMergeRange(UNITAI_ATTACK, 5, false, true))
		{
			return;
		}
		if (AI_groupMergeRange(UNITAI_COLLATERAL, 5, false, true))
		{
			return;
		}
		if (AI_groupMergeRange(UNITAI_RESERVE, 5, false, true))
		{
			return;
		}

	}
	bool bDanger = (GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 3));

	if (bDanger)
	{
		if (plot()->isCity() && plot()->getOwner() == getOwner())
		{
			// Attack choking units
			const int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
			const int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

			if (iOurDefense < 3 * iEnemyOffense && AI_guardCity(true))
			{
				return;
			}
			if (iEnemyOffense > 0 && iOurDefense > 2 * iEnemyOffense && AI_anyAttack(2, 55))
			{
				return;
			}
			if (AI_groupMergeRange(UNITAI_ATTACK, 2, true, true, false))
			{
				return;
			}
			if (iEnemyOffense > 0 && iOurDefense > 2 * iEnemyOffense && AI_anyAttack(2, 30))
			{
				return;
			}
		}
	}
	else if (getGroup()->getNumUnits() > 2 && plot()->getOwner() == getOwner() && getGroup()->getHeadUnitAI() == AI_getUnitAIType())
	{ // No danger
		// Shouldn't have groups of > 2 attack units
		if (getGroup()->countNumUnitAIType(UNITAI_ATTACK) > 2)
		{
			getGroup()->AI_separate(); // will change group

			FAssert(getGroup()->getHeadUnitAI() == getGroup()->getHeadUnitAI());
		}
		// Should never have attack city group lead by attack unit
		if (getGroup()->countNumUnitAIType(UNITAI_ATTACK_CITY) > 0)
		{
			getGroup()->AI_separateAI(UNITAI_ATTACK_CITY); // will change group
			// Skip to let ATTACK_CITY units move to avoid hang since ATTACK can try to join ATTACK_CITY again.
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	{
		PROFILE("CvUnitAI::AI_attackMove() 1");

		// Guard a city we're in if it needs it
		if (AI_guardCity(true))
		{
			return;
		}

		//	No high priority actions to take so see if anyone need help
		if (processContracts())
		{
			return;
		}

		if (!plot()->isOwned() && AI_groupMergeRange(UNITAI_SETTLE, 2, true, false, false))
		{
			return;
		}

		if ((!plot()->isOwned() || plot()->getOwner() == getOwner())
		&& area()->getCitiesPerPlayer(getOwner()) > GET_PLAYER(getOwner()).AI_totalAreaUnitAIs(area(), UNITAI_CITY_DEFENSE)
		&& AI_guardCity(true, true, 3)) // Defend colonies in new world
		{
			return;
		}

		if (AI_heal(30, 1))
		{
			return;
		}

		if (!bDanger)
		{
			if (AI_group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(1).maxPathTurns(3).allowRegrouping()))
			{
				return;
			}

			if (AI_group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(2).maxPathTurns(3).allowRegrouping()))
			{
				return;
			}
		}

		if (AI_guardCityAirlift())
		{
			return;
		}

		if (AI_guardCity(false, true, 1))
		{
			return;
		}

		//join any city attacks in progress
		//	Koshling - changed this to happen unconditionally (used to only happen inside
		//	enemy territory) otherwise stacks massing on the borders didn't merge and reach
		//	a sufficient stack power threshold to actually start the city attack run
		if (AI_groupMergeRange(UNITAI_ATTACK_CITY, 5, false, true))
		{
			return;
		}


		AreaAITypes eAreaAIType = area()->getAreaAIType(getTeam());
		if (plot()->isCity() && plot()->getOwner() == getOwner()
		&& (eAreaAIType == AREAAI_ASSAULT || eAreaAIType == AREAAI_ASSAULT_ASSIST)
		&& AI_offensiveAirlift())
		{
			return;
		}

		if (bDanger)
		{
			if (AI_cityAttack(1, 55) || AI_anyAttack(1, 65))
			{
				return;
			}

			if (getGroup()->hasCollateralDamage())
			{
				// RevolutionDCM - ranged bombardment AI
				// In theory trebs can be set to UNITAI_ATTACK
				// Dale - RB: Field Bombard START
				if (AI_RbombardUnit(1, 50, 3, 1, 120))
				{
					return;
				}
				// RevolutionDCM - end

				if (AI_anyAttack(1, 45, 3))
				{
					return;
				}
			}
		}

		if (!noDefensiveBonus() && AI_guardCity(false, false))
		{
			return;
		}

		if (!bDanger && plot()->getOwner() == getOwner())
		{
			// Assault AI
			if ((eAreaAIType == AREAAI_ASSAULT || eAreaAIType == AREAAI_ASSAULT_MASSING || eAreaAIType == AREAAI_ASSAULT_ASSIST)

			&& AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK_CITY, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
			{
				return;
			}

			if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, -1, 1, MOVE_SAFE_TERRITORY, 3))
			{
				return;
			}

			// Not land war AI
			if (!(eAreaAIType == AREAAI_OFFENSIVE || eAreaAIType == AREAAI_DEFENSIVE || eAreaAIType == AREAAI_MASSING))
			{
				// Fill transports before starting new one, but not just full of our unit ai
				if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, 1, -1, -1, 1, MOVE_SAFE_TERRITORY, 4))
				{
					return;
				}

				// Pick new transport which has space for other unit ai types to join
				if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
				{
					if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, (2 * GET_PLAYER(getOwner()).getBestUnitTypeCargoVolume(UNITAI_ASSAULT_SEA)), -1, -1, MOVE_SAFE_TERRITORY, 4))
					{
						return;
					}
				}
				else if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, 2, -1, -1, MOVE_SAFE_TERRITORY, 4))
				{
					return;
				}
			}

			if (GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, MISSIONAI_GROUP) > 0)
			{
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}
		}

		// Allow larger groups if inside enemy territory
		if (getGroup()->getNumUnits() < 4 && plot()->isOwned() && GET_TEAM(getTeam()).isAtWar(plot()->getTeam())
		&& AI_groupMergeRange(UNITAI_ATTACK, 1, true, true, true))
		{
			return;
		}

		// Go for goody or an easy battle
		if (AI_goody(3) || AI_anyAttack(1, 70))
		{
			return;
		}
	}

	{
		PROFILE("CvUnitAI::AI_attackMove() 2");

		if (bDanger)
		{
			if (AI_pillageRange(1, 20)
			|| AI_cityAttack(1, 35)
			|| AI_anyAttack(1, 45)
			|| AI_pillageRange(3, 20)
			|| getGroup()->getNumUnits() < 4 && AI_choke(1))
			{
				return;
			}
		}
	}

	if (plot()->isCity() && AI_paradrop(getDropRange()))
	{
		return;
	}

	{
		PROFILE("CvUnitAI::AI_attackMove() 2");

		if (bDanger && (AI_cityAttack(4, 30) || AI_anyAttack(2, 40)))
		{
			return;
		}

		if (!isEnemy(plot()->getTeam()) && AI_heal())
		{
			return;
		}

		if (GET_PLAYER(getOwner()).AI_getNumAIUnits(UNITAI_CITY_DEFENSE) > 0 || GET_TEAM(getTeam()).isAtWar())
		{
			// BBAI TODO: If we're fast, maybe shadow an attack city stack and pillage off of it

			const bool bIgnoreFaster = GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_LAND_BLITZ) && area()->getAreaAIType(getTeam()) != AREAAI_ASSAULT;

			if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK_CITY).maxGroupSize(1).maxOwnUnitAI(1).ignoreFaster(bIgnoreFaster).ignoreOwnUnitType().stackOfDoom().maxPathTurns(5)))
			{
				return;
			}

			if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK).maxGroupSize(1).maxOwnUnitAI(1).ignoreFaster().ignoreOwnUnitType().maxPathTurns(4)))
			{
				return;
			}

			/* BBAI TODO: Need group to be fast, need to ignore slower groups

						if (GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_FASTMOVERS)
						&& AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK).maxGroupSize(4).maxOwnUnitAI(1).ignoreFaster().maxPathTurns(3)))
						{
							return;
						}
			*/
			if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK).maxGroupSize(1).maxOwnUnitAI(1).ignoreFaster().maxPathTurns(1)))
			{
				return;
			}
		}

		if (area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE)
		{
			if (getGroup()->getNumUnits() > 1 && AI_goToTargetCity(MOVE_AVOID_ENEMY_WEIGHT_2, MAX_BARB_TARGET_CITY_RANGE))
			{
				return;
			}
		}
		else if (area()->getAreaAIType(getTeam()) != AREAAI_DEFENSIVE
		&& (area()->getCitiesPerPlayer(BARBARIAN_PLAYER) > 0 || area()->getCitiesPerPlayer(NEANDERTHAL_PLAYER) > 0)
		&& getGroup()->getNumUnits() >= GC.getHandicapInfo(GC.getGame().getHandicapType()).getBarbarianInitialDefenders()
		&& AI_goToTargetBarbCity(10)) // Deal with Barbarian pest
		{
			return;
		}

		if (AI_guardCity(false, true, 3))
		{
			return;
		}

		if (GET_PLAYER(getOwner()).getNumCities() > 1 && getGroup()->getNumUnits() == 1 && area()->getAreaAIType(getTeam()) != AREAAI_DEFENSIVE
		&& area()->getNumUnrevealedTiles(getTeam()) > 0
		&& GET_PLAYER(getOwner()).AI_areaMissionAIs(area(), MISSIONAI_EXPLORE, getGroup()) < GET_PLAYER(getOwner()).AI_neededExplorers(area()) + 1
		&& (AI_exploreRange(3) || AI_explore()))
		{
			return;
		}

		if (AI_protect(35, 5))
		{
			return;
		}

		if (AI_offensiveAirlift())
		{
			return;
		}

		if (!bDanger && (area()->getAreaAIType(getTeam()) != AREAAI_DEFENSIVE) && plot()->getOwner() == getOwner())
		{
			if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, 1, -1, -1, 1, MOVE_SAFE_TERRITORY, 4))
			{
				return;
			}
			if (GET_TEAM(getTeam()).isAtWar() && !getGroup()->isHasPathToAreaEnemyCity()
			&& AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
			{
				return;
			}
		}

		if (AI_defend() || AI_travelToUpgradeCity())
		{
			return;
		}

		if (getGroup()->isStranded()
		&& AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}

		// If no other desireable actions, wait for pickup
		if (!bDanger && !isHuman() && plot()->isCoastalLand() && GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, MISSIONAI_PICKUP) > 0)
		{
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}

		if (getGroup()->getNumUnits() < 4 && AI_patrol())
		{
			return;
		}

		if (AI_retreatToCity() || AI_safety(3))
		{
			return;
		}
	}
	LOG_UNIT_BLOCK(4, {
		UnitAITypes eUnitAi = AI_getUnitAIType();
		MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = m_szName;
		if (StrUnitName.length() == 0)
		{
			StrUnitName = getName(0).GetCString();
		}

		logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Skip reached for attackMove...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits());
	});
	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_paratrooperMove()
{
	PROFILE_FUNC();
	if (AI_selectStatus(true))
	{
		return;
	}
	bool bHostile = (plot()->isOwned() && isPotentialEnemy(plot()->getTeam()));
	if (!bHostile)
	{
		if (AI_guardCity(true))
		{
			return;
		}

		if (plot()->getTeam() == getTeam())
		{
			if (plot()->isCity())
			{
				if (AI_heal(30, 1))
				{
					return;
				}
			}

			AreaAITypes eAreaAIType = area()->getAreaAIType(getTeam());
			bool bLandWar = ((eAreaAIType == AREAAI_OFFENSIVE) || (eAreaAIType == AREAAI_DEFENSIVE) || (eAreaAIType == AREAAI_MASSING));
			if (!bLandWar)
			{
				if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, 0, MOVE_SAFE_TERRITORY, 4))
				{
					return;
				}
			}
		}

		if (AI_guardCity(false, true, 1))
		{
			return;
		}
	}

	if (AI_cityAttack(1, 45))
	{
		return;
	}

	if (AI_anyAttack(1, 55))
	{
		return;
	}

	if (!bHostile)
	{
		if (AI_paradrop(getDropRange()))
		{
			return;
		}

		if (AI_offensiveAirlift())
		{
			return;
		}

		if (AI_moveToStagingCity())
		{
			return;
		}

		if (AI_guardFort(true))
		{
			return;
		}

		if (AI_guardCityAirlift())
		{
			return;
		}
	}

	if (getGroup()->hasCollateralDamage())
	{
		if (AI_anyAttack(1, 45, 3))
		{
			return;
		}
	}

	if (AI_pillageRange(1, 15))
	{
		return;
	}

	if (bHostile)
	{
		if (AI_choke(1))
		{
			return;
		}
	}

	if (AI_heal())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  09/18/09								jdog5000	  */
	/*																							  */
	/* Unit AI																					  */
	/************************************************************************************************/
		//if (AI_protect(35))
	if (AI_protect(35, 5))
	{
		return;
	}

	if (getGroup()->isStranded())
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/


	if (AI_safety(3))
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_attackCityMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}
	if (checkSwitchToConstruct())
	{
		return;
	}

	AreaAITypes eAreaAIType = area()->getAreaAIType(getTeam());
	bool bLandWar = eAreaAIType == AREAAI_OFFENSIVE || eAreaAIType == AREAAI_DEFENSIVE || eAreaAIType == AREAAI_MASSING;
	bool bAssault = eAreaAIType == AREAAI_ASSAULT || eAreaAIType == AREAAI_ASSAULT_ASSIST || eAreaAIType == AREAAI_ASSAULT_MASSING;

	bool bTurtle = GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_TURTLE);
	bool bAlert1 = GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_ALERT1);
	bool bIgnoreFaster = false;
	if (GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_LAND_BLITZ))
	{
		if (!bAssault && area()->getCitiesPerPlayer(getOwner()) > 0)
		{
			bIgnoreFaster = true;
		}
	}

	//	Super forts - allow stack to split after fort capture as well as city
	bool bInCity = plot()->isCity(false);

	if (bInCity && plot()->getOwner() == getOwner())
	{
		// force heal if we in our own city and damaged
		// can we remove this or call AI_heal here?
		if (AI_heal())
		{
			return;
		}

		if (plot()->getOwnershipDuration() <= 1)
		{
			CvSelectionGroup* pOldGroup = getGroup();

			pOldGroup->AI_separateNonAI(UNITAI_ATTACK_CITY);

			if (pOldGroup != getGroup())
			{
				return;
			}
		}

		if ((eAreaAIType == AREAAI_ASSAULT) || (eAreaAIType == AREAAI_ASSAULT_ASSIST))
		{
			if (AI_offensiveAirlift())
			{
				return;
			}
		}
	}

	bool bAtWar = isEnemy(plot()->getTeam());

	bool bHuntBarbs = false;

	if (!isNPC() && (area()->getCitiesPerPlayer(BARBARIAN_PLAYER) > 0 || area()->getCitiesPerPlayer(NEANDERTHAL_PLAYER) > 0))
	{
		if ((eAreaAIType != AREAAI_OFFENSIVE) && (eAreaAIType != AREAAI_DEFENSIVE) && !bAlert1 && !bTurtle)
		{
			bHuntBarbs = true;
		}
	}

	bool bReadyToAttack = false;
	if (isHominid())
	{
		bLandWar = area()->getNumCities() - area()->getCitiesPerPlayer(getOwner()) > 0;
		bReadyToAttack = getGroup()->getNumUnits() >= AI_stackOfDoomExtra();
	}
	else if (!bTurtle)
	{
		//Ready to Attack if the stack size is > to a Standard Value (Doom Extra)
		bReadyToAttack = getGroup()->getNumUnits() >= (bHuntBarbs ? 3 : AI_stackOfDoomExtra());
	}

	const bool bCity = plot()->isCity();

	if (bReadyToAttack)
	{
		// Check that stack has units which can capture cities
		bReadyToAttack = false;
		int iCityCaptureCount = 0;

		CLLNode<IDInfo>* pUnitNode = getGroup()->headUnitNode();
		while (pUnitNode != NULL && !bReadyToAttack)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = getGroup()->nextUnitNode(pUnitNode);

			if (!pLoopUnit->isOnlyDefensive())
			{
				if (!(pLoopUnit->isNoCapture()) && (pLoopUnit->combatLimit() >= 100))
				{
					iCityCaptureCount++;

					if (iCityCaptureCount > 5 || 3 * iCityCaptureCount > getGroup()->getNumUnits())
					{
						bReadyToAttack = true;
					}
				}
			}
		}

		//	Special case - if we have no attackers at all advertise for one urgently
		if (iCityCaptureCount < 2 && !isCargo())
		{
			GET_PLAYER(getOwner()).getContractBroker().advertiseWork(
				HIGH_PRIORITY_ESCORT_PRIORITY + 1,
				NO_UNITCAPABILITIES,
				getX(),
				getY(),
				this,
				UNITAI_ATTACK_CITY
			);
			m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
			m_contractualState = CONTRACTUAL_STATE_AWAITING_ANSWER;

			if (gUnitLogLevel > 2)
			{
				logContractBroker(1, "	%S's %S at (%d,%d) requests initial city-capture-capable attacker for attack stack at priority %d", GET_PLAYER(getOwner()).getCivilizationDescription(0), getName(0).GetCString(), getX(), getY(), HIGH_PRIORITY_ESCORT_PRIORITY + 1);
			}
		}

		// Do not infer medic coverage from canHeal(): that means this member
		// is wounded and can heal itself. The typed support assessment below
		// requests the healer combat class actually missing from this army.


	}

	//If it can Merge with Other Stack on the same plot
	if (AI_groupMergeRange(UNITAI_ATTACK_CITY, 0, true, true, bIgnoreFaster))
	{
		return;
	}

	CvCity* pTargetCity = NULL;
	if (isHominid())
	{
		pTargetCity = area()->getTargetCity(getOwner());
	}
	else if (getGroup()->getArmyID() != -1)
	{
		CvPlayer& player = GET_PLAYER(getOwner());
		CvArmy* kArmy = player.getArmy(getGroup()->getArmyID());
		if (kArmy != NULL)
		{
			CvPlot* pPlot = kArmy->getTargetPlot();
			if (pPlot != NULL)
			{
				pTargetCity = pPlot->getPlotCity();
				if (!AI_isCachedTargetCityValid(pTargetCity, bHuntBarbs)) pTargetCity = NULL;
			}
		}
	}

	if (pTargetCity == NULL)
	{
		pTargetCity = AI_pickTargetCity(MOVE_THROUGH_ENEMY, MAX_INT, bHuntBarbs);
	}

	if (pTargetCity != NULL)
	{
		int iStepDistToTarget = stepDistance(pTargetCity->getX(), pTargetCity->getY(), getX(), getY());
		int iAttackRatio = std::max(100, GC.getBBAI_ATTACK_CITY_STACK_RATIO());
		int iOurOffense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 1, false, false, true);
		int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(pTargetCity->plot(), 2, false, false);

		//	Koshling - use the difficulty level of the opponent's settings to estimate how
		//	much more prepared we really need to be - this should cause players on higher difficulty
		//	settings to get attacked by larger stacks rather than prematurely by not-quite-adequate ones
		//	Only do this if we're still in preparation mode either in our own territory or some way away from
		//	the target (before we lose the benefit of surprise)
		if (plot()->getOwner() == getOwner() || iStepDistToTarget > 3)
		{
			int iTargetUnitTrainModifier = GC.getHandicapInfo(GET_PLAYER(pTargetCity->getOwner()).getHandicapType()).getAITrainPercent();

			if (iTargetUnitTrainModifier < 100)
			{
				//	Difficulty above Noble, so a player on a higher settings
				//	Diety has 75% so squaring things means a diety player can expect to see
				//	the AI prepare attack stacks approximately 16/9 times more powerful
				//	then a noble player
				iAttackRatio *= 10000;
				int iSquared = iTargetUnitTrainModifier * iTargetUnitTrainModifier;
				iAttackRatio /= std::max(1, iSquared);
			}
		}

		if (isHominid())
		{
			iAttackRatio = std::max(80, iAttackRatio / 2);
		}

		int iComparePostBombard = 0;
		// AI gets a 1-tile sneak peak to compensate for lack of memory
		if (iStepDistToTarget <= 2 || pTargetCity->isVisible(getTeam(), false))
		{
			iComparePostBombard = getGroup()->AI_compareStacks(pTargetCity->plot(), StackCompare::CheckCanAttack | StackCompare::CheckCanMove | StackCompare::PotentialEnemy, std::min(2, iStepDistToTarget - 1));

			/* original BBAI code
			int iDefenseModifier = pTargetCity->getDefenseModifier(true);
			int iBombardTurns = getGroup()->getBombardTurns(pTargetCity);
			iDefenseModifier *= std::max(0, 20 - iBombardTurns);
			iDefenseModifier /= 20;
			iComparePostBombard *= 100 + std::max(0, iDefenseModifier);
			iComparePostBombard /= 100; */

			// K-Mod, appart from the fact that they got the defence reduction backwards; the defense modifier
			// is counted in AI_compareStacks. So if we add it again, we'd be double counting.
			// In fact, it's worse than that because it would compound.
			// I'm going to subtract defence, but unfortunately this will reduce based on the total rather than the base.
			int iDefenseModifier = pTargetCity->getDefenseModifier(false);
			int iBombardTurns = getGroup()->getBombardTurns(pTargetCity);
			int iReducedModifier = iDefenseModifier;
			iReducedModifier *= std::min(20, std::max(0, iBombardTurns - 12) + iBombardTurns / 2);
			iReducedModifier /= 20;
			//	Koshling - changed the sign on the following calculation as it was backwards!
			//	A small reducedModifier (the effective mod we EXPECT to be fighting against very quickly)
			//	with a large raw modifier will have already had the over-large raw modifier applied
			//	(reducing the compareRatio), and that is what we are seeking to reverse, not compound!
			iComparePostBombard *= 200 - iReducedModifier + iDefenseModifier;
			iComparePostBombard /= 200;
			// using 200 instead of 100 to offset the over-reduction from compounding.
			// With this, bombarding a defence bonus of 100% with reduce effective defence by 50%

			LOG_UNIT_BLOCK(3, {
				UnitAITypes eUnitAi = AI_getUnitAIType();
				MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
				CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
				CvWString MissionInfos = MissionAITypeToString(eMissionAI);
				CvWString StrUnitName = m_szName;
				if (StrUnitName.length() == 0)
				{
					StrUnitName = getName(0).GetCString();
				}

				logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Check Attack Ratio and Bombard possibility at (%d,%d)...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits(), pTargetCity->getX(), pTargetCity->getY());
				logBBAI("       Attack (estim after Bomb.) : %d, AttackRatio : %d", iComparePostBombard, iAttackRatio);
			});
			if (iComparePostBombard < iAttackRatio && !bAtWar)
			{
				//	Koshling - if we find we are not strong enough when we get target visibility
				//	don't start a war if we haven't already.
				//	Note that this is at best a very partial fix.  Changes are needed to determine
				//	whether we are likely to have enough strength much sooner.  Things to do:
				//	1)	Add some sort of history for the last-seen defense of each city and use
				//		the most recent value as an estimatre for cities we don't yet have visibility on
				//	2)	Use stealth units to frequently regain visibility on at least the area target city
				//	3)	When evaluating city defense take into account (visible) units in the neighbourhood
				//		which are not actually in the city itself - EDIT (3) now DONE
				bReadyToAttack = false;	//	Force it to gather more units first
			}
		}

		//	Grab some more units if we can

		UnitCombatTypes eMostNeeded = NO_UNITCOMBAT;
		int iNeedSupportCount = 0;
		int iMostNeededSupportCount = 0;

		for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		{
			if (GC.getUnitCombatInfo((UnitCombatTypes)iI).isHealsAs())
			{
				UnitCombatTypes eHealType = (UnitCombatTypes)iI;
				if (plot()->getUnitCombatsUnsupportedByHealer(getOwner(), eHealType, getDomainType()) > 4)
				{
					iNeedSupportCount = -4;
					foreach_(const CvUnit * pLoopUnit, getGroup()->units())
					{
						if (pLoopUnit->isHasUnitCombat(eHealType))
						{
							iNeedSupportCount++;
						}
						if (pLoopUnit->isHealsUnitCombat(eHealType))
						{
							iNeedSupportCount -= pLoopUnit->getNumHealSupportTotal();
							if (pLoopUnit->getNumHealSupportTotal())
							{
								iNeedSupportCount--;
							}
						}
					}
					iNeedSupportCount /= 4;
					if (iNeedSupportCount > 0 && iNeedSupportCount > iMostNeededSupportCount)
					{
						iMostNeededSupportCount = iNeedSupportCount;
						eMostNeeded = eHealType;
					}
				}
			}
		}

		if (m_contractsLastEstablishedTurn != GC.getGame().getGameTurn())
		{
			// Try to get a healer if we don't have enough
			if (!isCargo() && eMostNeeded != NO_UNITCOMBAT)
			{
				CvUnitSelectionCriteria healerCriteria;
				healerCriteria.m_bIsHealer = true;
				healerCriteria.m_eHealUnitCombat = eMostNeeded;

				GET_PLAYER(getOwner()).getContractBroker().advertiseWork(
					HIGH_PRIORITY_ESCORT_PRIORITY,
					HEALER_UNITCAPABILITIES,
					getX(), getY(),
					this, UNITAI_HEALER,
					-1, &healerCriteria
				);
				m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
				m_contractualState = CONTRACTUAL_STATE_AWAITING_ANSWER;
				if (gUnitLogLevel > 2)
				{
					CvWString szString;
					getUnitAIString(szString, AI_getUnitAIType());
					logBBAI("	%S's %S at (%d,%d) requests healer for attack stack", GET_PLAYER(getOwner()).getCivilizationDescription(0), getName(0).GetCString(), getX(), getY());
				}
			}

			// If we're still on home turf advertise for more attackers at a priority that is increased if we're not yet up to strength
			if (plot()->getOwner() == getOwner() && !isCargo())
			{
				//First cover the see invisible bases
				if (AI_establishStackSeeInvisibleCoverage())
				{
					//return; Test Calvitix. If it is possible to Contrat more than One Unit
				}
				GET_PLAYER(getOwner()).getContractBroker().advertiseWork(
					bReadyToAttack ? LOW_PRIORITY_ESCORT_PRIORITY : HIGH_PRIORITY_ESCORT_PRIORITY,
					NO_UNITCAPABILITIES,
					getX(), getY(),
					this, UNITAI_ATTACK_CITY
				);
				m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
				m_contractualState = CONTRACTUAL_STATE_AWAITING_ANSWER;
				if (gUnitLogLevel > 2)
				{
					CvWString szString;
					getUnitAIString(szString, AI_getUnitAIType());
					logBBAI("	%S's %S at (%d,%d) requests more attackers for attack stack at priority %d", GET_PLAYER(getOwner()).getCivilizationDescription(0), getName(0).GetCString(), getX(), getY(), bReadyToAttack ? LOW_PRIORITY_ESCORT_PRIORITY : HIGH_PRIORITY_ESCORT_PRIORITY);
				}

				//	Also try to get a great commander if we don't have one
				if (GC.getGame().isOption(GAMEOPTION_UNIT_GREAT_COMMANDERS) && !getGroup()->hasCommander())
				{
					GET_PLAYER(getOwner()).getContractBroker().advertiseWork(
						bReadyToAttack ? HIGH_PRIORITY_ESCORT_PRIORITY : LOW_PRIORITY_ESCORT_PRIORITY,
						NO_UNITCAPABILITIES,
						getX(), getY(),
						this, UNITAI_GENERAL
					);
					m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
					m_contractualState = CONTRACTUAL_STATE_AWAITING_ANSWER;
					if (gUnitLogLevel > 2)
					{
						CvWString szString;
						getUnitAIString(szString, AI_getUnitAIType());
						logBBAI("	%S's %S at (%d,%d) requests great commander for attack stack at priority %d", GET_PLAYER(getOwner()).getCivilizationDescription(0), getName(0).GetCString(), getX(), getY(), HIGH_PRIORITY_ESCORT_PRIORITY);
					}
				}
			}
		}

		if (iStepDistToTarget <= 2)
		{
			if (iComparePostBombard < iAttackRatio)
			{
				if (AI_groupMergeRange(UNITAI_ATTACK_CITY, 5, true, true, bIgnoreFaster))
				{
					return;
				}
				if (AI_groupMergeRange(UNITAI_ATTACK, 5, true, true, bIgnoreFaster))
				{
					return;
				}
				if (AI_groupMergeRange(UNITAI_COUNTER, 5, true, true, bIgnoreFaster))
				{
					return;
				}


				// If in danger, seek defensive ground
				if (4 * iOurOffense < 3 * iEnemyOffense)
				{
					if (AI_choke(1, true))
					{
						return;
					}
				}
			}

			if (iStepDistToTarget == 1)
			{
				// If next to target city and we would attack after bombarding down defenses,
				// or if defenses have crept up past half
				int iAttackOdds = getGroup()->AI_attackOdds(pTargetCity->plot(), /*bPotentialEnemy*/ true);
				if (iAttackOdds > 75)
				{
					logBBAI("    good odds (%d) to attack at (%d,%d)...", iAttackOdds, pTargetCity->plot()->getX(), pTargetCity->plot()->getY());
					if (AI_cityAttack(1, 80))
					{
						return;
					}
				}
				if ((iComparePostBombard >= iAttackRatio) || (pTargetCity->getDefenseDamage() < ((GC.getMAX_CITY_DEFENSE_DAMAGE() * 1) / 2)) || 20 * iOurOffense > 39 * (iEnemyOffense+1) || pTargetCity->plot()->getNumDefenders(pTargetCity->getOwner()) <= 2)
				{
					if (iComparePostBombard < std::max(150, GC.getBBAI_SKIP_BOMBARD_MIN_STACK_RATIO()) && (pTargetCity->isDirectAttackable() || canIgnoreNoEntryLevel()))
					{
						// Move to good tile to attack from unless we're way more powerful
						if (AI_goToTargetCity(0, 1, pTargetCity))
						{
							return;
						}
					}

					// Bombard may skip if stack is powerful enough
					if (AI_bombardCity())
					{
						//return;
					}

					//stack attack
					if (getGroup()->getNumUnits() > 1)
					{
						// BBAI TODO: What is right ratio?
						if (AI_stackAttackCity(1, iAttackRatio, true))
						{
							return;
						}
					}

					// If not strong enough alone, merge if another stack is nearby
					if (AI_groupMergeRange(UNITAI_ATTACK_CITY, 5, true, true, bIgnoreFaster))
					{
						return;
					}
					if (AI_groupMergeRange(UNITAI_COLLATERAL, 5, true, true, bIgnoreFaster))
					{
						return;
					}
					if (AI_groupMergeRange(UNITAI_ATTACK, 5, true, true, bIgnoreFaster))
					{
						return;
					}
					if (AI_groupMergeRange(UNITAI_COUNTER, 5, true, true, bIgnoreFaster))
					{
						return;
					}

					if (getGroup()->getNumUnits() == 1)
					{
						if (AI_cityAttack(1, 50))
						{
							return;
						}
					}
				}
			}

			if (bCity && AI_paradrop(getDropRange()))
			{
				return;
			}

			if (iComparePostBombard < iAttackRatio)
			{
				LOG_UNIT_BLOCK(3, {
					UnitAITypes eUnitAi = AI_getUnitAIType();
					MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
					CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
					CvWString MissionInfos = MissionAITypeToString(eMissionAI);
					CvWString StrUnitName = m_szName;
					if (StrUnitName.length() == 0)
					{
						StrUnitName = getName(0).GetCString();
					}

					logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Not Strong enough to Attack at (%d,%d)...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits(), pTargetCity->getX(), pTargetCity->getY());
					logBBAI("       Attack (estim after Bomb.) : %d, AttackRatio : %d", iComparePostBombard, iAttackRatio);
				});


				// If not strong enough, ether pillage around target city without exposing ourselves, or go back to own city
				if (GC.getGame().getSorenRandNum(3, "AI go Back") == 0)
				{
					if (AI_retreatToCity())
					{
						return;
					}
				}
				else if (GC.getGame().getSorenRandNum(3, "AI go Back") == 0)
				{
					if (AI_cityAttack(1, 70))
					{
						return;
					}
					// Move to good tile to attack from unless we're way more powerful
					if (AI_goToTargetCity(0, 1, pTargetCity))
					{
						return;
					}
				}

				if (AI_pillageRange(3))
				{
					return;
				}

				if (AI_anyAttack(1, 60, 0, false))
				{
					return;
				}

				if (AI_heal(((iComparePostBombard * 100) / iAttackRatio >= 80) ? 0 : 30, 1))
				{
					return;
				}

				// Pillage around enemy city
				if (AI_pillageAroundCity(pTargetCity, 11, 3))
				{
					return;
				}

				if (AI_pillageAroundCity(pTargetCity, 0, 5))
				{
					return;
				}

				if (AI_choke(1))
				{
					return;
				}
			}
			else if (AI_goToTargetCity(0, 4, pTargetCity))
			{
				return;
			}
		}

		if (!bHuntBarbs && iStepDistToTarget >= 10 && AI_rallyToStrongerGroup())
		{
			return;
		}
	}

	if (AI_groupMergeRange(UNITAI_ATTACK_CITY, 5, true, true, bIgnoreFaster))
	{
		return;
	}
	if (AI_groupMergeRange(UNITAI_COLLATERAL, 5, true, true, bIgnoreFaster))
	{
		return;
	}
	if (AI_groupMergeRange(UNITAI_ATTACK, 5, true, true, bIgnoreFaster))
	{
		return;
	}
	if (AI_groupMergeRange(UNITAI_COUNTER, 5, true, true, bIgnoreFaster))
	{
		return;
	}
	if (AI_groupMergeRange(UNITAI_RESERVE, 5, true, true, bIgnoreFaster))
	{
		return;
	}

	if (AI_heal(30, 1))
	{
		return;
	}

	// BBAI TODO: Stack v stack combat ... definitely want to do in own territory, but what about enemy territory?
	if (getGroup()->hasCollateralDamage() && plot()->getOwner() == getOwner())
	{
		if (AI_RbombardUnit(1, 50, 3, 2, 130))
		{
			return;
		}

		if (AI_anyAttack(1, 45, 3, false))
		{
			return;
		}

		if (!bReadyToAttack)
		{
			if (AI_RbombardUnit(getGroup()->getMinimumRBombardRange(), 30, 5, 2, 110))
			{
				return;
			}

			if (AI_anyAttack(1, 25, 5))
			{
				return;
			}
			// K-Mod; sometimes you just need to blast them with collateral damage before they can do it to you!
			// ... and please don't let them wipe out a massive stack just because we were too scared to fight back

			// look. I don't want to spend too much time messing around with this. So let me just try an experimental heuristic...
			if (getGroup()->getNumUnits() > 1 && AI_anyAttack(1, 60 / (getGroup()->getNumUnits() + 1), 3 + getGroup()->getNumUnits(), false))
			{
				return;
			}
		}
	}

	//	If we're still in our own territory attack infiltrators
	if (plot()->getOwner() == getOwner() && AI_anyAttack(2, 50))
	{
		return;
	}
	if (AI_anyAttack(1, 60, 0, false))
	{
		return;
	}

	if (bAtWar && (getGroup()->getNumUnits() <= 2))
	{
		if (AI_pillageRange(3, 11))
		{
			return;
		}

		if (AI_pillageRange(1))
		{
			return;
		}
	}

	if (plot()->getOwner() == getOwner())
	{
		if (!bLandWar)
		{
			if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
			{
				return;
			}
		}

		if (bReadyToAttack)
		{
			// Wait for units about to join our group
			MissionAITypes eMissionAIType = MISSIONAI_GROUP;
			int iJoiners = GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, &eMissionAIType, 1, getGroup(), 2);

			if ((iJoiners * 5) > getGroup()->getNumUnits())
			{
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}
		}
		else
		{
			if (!isNPC() && (eAreaAIType == AREAAI_DEFENSIVE))
			{
				// Use smaller attack city stacks on defense
				if (AI_guardCity(false, true, 3))
				{
					return;
				}
			}

			if (bTurtle)
			{
				if (AI_guardCity(false, true, 7))
				{
					return;
				}
			}

			int iTargetCount = GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, MISSIONAI_GROUP);
			if ((iTargetCount * 5) > getGroup()->getNumUnits())
			{
				MissionAITypes eMissionAIType = MISSIONAI_GROUP;
				int iJoiners = GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, &eMissionAIType, 1, getGroup(), 2);

				if ((iJoiners * 5) > getGroup()->getNumUnits() || isHominid())
				{
					getGroup()->pushMission(MISSION_SKIP);
					return;
				}

				if (AI_moveToStagingCity())
				{
					return;
				}
			}
		}
	}

	if (AI_heal(50, 3))
	{
		return;
	}

	if (!bAtWar)
	{
		if (AI_heal())
		{
			return;
		}

		if ((getGroup()->getNumUnits() == 1) && (getTeam() != plot()->getTeam()))
		{
			if (AI_retreatToCity())
			{
				return;
			}
		}
	}

	if (!bReadyToAttack && !noDefensiveBonus())
	{
		if (AI_guardCity(false, false))
		{
			return;
		}
	}

	const bool bAnyWarPlan = GET_TEAM(getTeam()).hasWarPlan(true);

	if (bReadyToAttack)
	{
		if (isHominid())
		{
			if (AI_goToTargetCity(MOVE_AVOID_ENEMY_WEIGHT_2, MAX_BARB_TARGET_CITY_RANGE)
			|| AI_goToTargetCity(MOVE_AVOID_ENEMY_WEIGHT_2, MAX_INT, pTargetCity))
			{
				return;
			}

			if (AI_pillageRange(3, 11))
			{
				return;
			}

			if (AI_pillageRange(1))
			{
				return;
			}
		}
		else if (bHuntBarbs && AI_goToTargetBarbCity((bAnyWarPlan ? 7 : 12)))
		{
			return;
		}
		else if (bLandWar && pTargetCity != NULL)
		{
			// Before heading out, check whether to wait to allow unit upgrades
			if (bInCity && plot()->getOwner() == getOwner())
			{
				if (!GET_PLAYER(getOwner()).AI_isFinancialTrouble())
				{
					// Check if stack has units which can upgrade
					int iNeedUpgradeCount = 0;

					foreach_(const CvUnit * pLoopUnit, getGroup()->units())
					{
						if (pLoopUnit->getUpgradeCity(false) != NULL)
						{
							iNeedUpgradeCount++;

							if (8 * iNeedUpgradeCount > getGroup()->getNumUnits())
							{
								getGroup()->pushMission(MISSION_SKIP);
								return;
							}
						}
					}
				}
			}

			if (AI_goToTargetCity(MOVE_AVOID_ENEMY_WEIGHT_2, 5, pTargetCity))
			{
				return;
			}

			if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 2, 2))
			{
				return;
			}

			if (AI_goToTargetCity(MOVE_AVOID_ENEMY_WEIGHT_2, 8, pTargetCity))
			{
				return;
			}

			// Load stack if walking will take a long time
			if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4, 3))
			{
				return;
			}

			if (AI_goToTargetCity(MOVE_AVOID_ENEMY_WEIGHT_2, 12, pTargetCity))
			{
				return;
			}

			if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4, 7))
			{
				return;
			}

			if (AI_goToTargetCity(MOVE_AVOID_ENEMY_WEIGHT_2, MAX_INT, pTargetCity))
			{
				return;
			}

			if (bAnyWarPlan)
			{
				const CvCity* pTargetCity = area()->getTargetCity(getOwner());

				if (pTargetCity != NULL && AI_solveBlockageProblem(pTargetCity->plot(), !GET_TEAM(getTeam()).isAtWar()))
				{
					return;
				}
			}
		}
	}
	else
	{
		const int iTargetCount = GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, MISSIONAI_GROUP);
		if (((iTargetCount * 4) > getGroup()->getNumUnits()) || ((getGroup()->getNumUnits() + iTargetCount) >= (bHuntBarbs ? 3 : AI_stackOfDoomExtra())))
		{
			MissionAITypes eMissionAIType = MISSIONAI_GROUP;
			const int iJoiners = GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, &eMissionAIType, 1, getGroup(), 2);

			if ((iJoiners * 6) > getGroup()->getNumUnits())
			{
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}

			if (AI_safety())
			{
				return;
			}
		}

		if (getGroup()->hasBombardCapability() && noDefensiveBonus())
		{
			// BBAI Notes: Add this stack lead by bombard unit to stack probably not lead by a bombard unit
			// BBAI TODO: Some sense of minimum stack size?  Can have big stack moving 10 turns to merge with tiny stacks
			if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK_CITY)
				.biggerGroupOnly()
				.mergeWholeGroup()
				.ignoreFaster(bIgnoreFaster)
				.ignoreOwnUnitType()
				.stackOfDoom()
				.maxPathTurns(10)
				.allowRegrouping()))
			{
				return;
			}
		}
		else if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK_CITY)
			.maxGroupSize(AI_stackOfDoomExtra() * 2)
			.biggerGroupOnly()
			.mergeWholeGroup()
			.ignoreFaster(bIgnoreFaster)
			.ignoreOwnUnitType()
			.stackOfDoom()
			.maxPathTurns(10)))
		{
			return;
		}
	}

	if (plot()->getOwner() == getOwner() && bLandWar && GET_TEAM(getTeam()).isAtWar())
	{
		// if no land path to enemy cities, try getting there another way
		if (AI_offensiveAirlift())
		{
			return;
		}

		if (pTargetCity == NULL)
		{
			if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
			{
				return;
			}
		}
	}

	if (!isNPC() && AI_moveToStagingCity())
	{
		return;
	}

	if (AI_offensiveAirlift())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (getGroup()->isStranded())
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}

		if (!isHuman() && plot()->isCoastalLand() && GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, MISSIONAI_PICKUP) > 0)
		{
			// If no other desireable actions, wait for pickup
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}

		if (AI_patrol())
		{
			return;
		}
	}

	if (AI_safety(3))
	{
		return;
	}

	LOG_UNIT_BLOCK(2, {
		UnitAITypes eUnitAi = AI_getUnitAIType();
		MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = m_szName;
		if (StrUnitName.length() == 0)
		{
			StrUnitName = getName(0).GetCString();
		}
		logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], nothing to do in attackCityMove, skip...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits());
	});

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_attackCityLemmingMove()
{
	if (AI_selectStatus(true))
	{
		return;
	}

	if (AI_cityAttack(1, 80))
	{
		return;
	}

	if (AI_bombardCity())
	{
		return;
	}

	if (AI_cityAttack(1, 40))
	{
		return;
	}

	if (AI_goToTargetCity(MOVE_THROUGH_ENEMY))
	{
		return;
	}

	if (AI_anyAttack(1, 70))
	{
		return;
	}

	if (AI_anyAttack(1, 0))
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
}

void CvUnitAI::AI_collateralMove()
{
	PROFILE_FUNC();

	// K-Mod!
	//if (AI_defensiveCollateral(51, 3))
	//	return;
	// K-Mod end
	if (AI_groupMergeRange(UNITAI_ATTACK_CITY, 10, false, true))
	{
		return;
	}

	if (AI_leaveAttack(1, 30, 100))
	{
		return;
	}

	if (AI_guardCity(false, true, 1))
	{
		return;
	}

	if (AI_heal(30, 1))
	{
		return;
	}

	if (AI_cityAttack(1, 35))
	{
		return;
	}

	if (AI_RbombardUnit(1, 50, 3, 1, 100))
	{
		return;
	}

	if (AI_anyAttack(1, 45, 3))
	{
		return;
	}

	if (AI_anyAttack(1, 55, 2))
	{
		return;
	}

	if (AI_RbombardUnit(1, 40, 3, 0, 100))
	{
		return;
	}

	if (AI_anyAttack(1, 35, 3))
	{
		return;
	}

	if (AI_anyAttack(1, 30, 4))
	{
		return;
	}

	if (AI_RbombardUnit(1, 25, 5, 0, 80))
	{
		return;
	}

	if (AI_anyAttack(1, 20, 5))
	{
		return;
	}

	if (AI_heal())
	{
		return;
	}

	if (!noDefensiveBonus())
	{
		if (AI_guardCity(false, false))
		{
			return;
		}
	}

	if (AI_RbombardUnit(getGroup()->getMinimumRBombardRange(), 40, 3, 0, 100))
	{
		return;
	}

	if (AI_anyAttack(2, 55, 3))
	{
		return;
	}

	if (AI_cityAttack(2, 50))
	{
		return;
	}

	if (AI_anyAttack(2, 60))
	{
		return;
	}

	if (AI_protect(50, 8))
	{
		return;
	}

	if (AI_RbombardPlot(getDCMBombRange(), 20))
	{
		return;
	}

	if (AI_guardCity(false, true, 3))
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (getGroup()->isStranded())
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (AI_safety(3))
	{
		return;
	}
	LOG_UNIT_BLOCK(4, {
		UnitAITypes eUnitAi = AI_getUnitAIType();
		MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = m_szName;
		if (StrUnitName.length() == 0)
		{
			StrUnitName = getName(0).GetCString();
		}

		logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Skip reached for collateralMove...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits());
	});
	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_pillageMove()
{
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  03/05/10								jdog5000	  */
	/*																							  */
	/* Unit AI																					  */
	/************************************************************************************************/
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	//if (AI_guardCity(false, true, 1))
	//{
	//	return;
	//}

	if (AI_heal(30, 1))
	{
		return;
	}

	// BBAI TODO: Shadow ATTACK_CITY stacks and pillage

	//join any city attacks in progress
	//if ( MISSIONAI_REGROUP == getGroup()->AI_getMissionAIType() || (plot()->isOwned() && plot()->getOwner() != getOwner()))
	//{
	//	if (AI_groupMergeRange(UNITAI_ATTACK_CITY, 1, false, true))
	//	{
	//		return;
	//	}
	//}

	if (AI_cityAttack(1, 55))
	{
		return;
	}

	if (AI_anyAttack(1, 65))
	{
		return;
	}

	//if (!noDefensiveBonus())
	//{
	//	if (AI_guardCity(false, false))
	//	{
	//		return;
	//	}
	//}

/*****************************************************************************************************/
/**  Author: TheLadiesOgre																		  **/
/**  Date: 16.09.2009																			   **/
/**  ModComp: TLOTags																			   **/
/**  Reason Added: Implement AI_paradrop for other UNITAI's now that it is potentially available	**/
/**  Notes: 14.10.2009-- Added bCity check														  **/
/*****************************************************************************************************/
	// Pillage-role villains (e.g. Cutthroat) can also be working through
	// harmful city properties supplied by BuildUp. Keep that useful hidden
	// assignment after urgent healing/attacks, before optional roaming.
	if (!isHuman() && infiltratorWorkingLocally(this))
	{
		getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_INFILTRATOR_MAINTAIN, plot());
		return;
	}

	bool bCity = plot()->isCity();
	if (bCity)
	{
		if (AI_paradrop(getDropRange()))
		{
			return;
		}
	}

	/*****************************************************************************************************/
	/**  TheLadiesOgre; 16.09.2009; TLOTags															 **/
	/*****************************************************************************************************/
	if (AI_pillageRange(3, 11))
	{
		return;
	}

	if (AI_choke(1))
	{
		return;
	}

	if (AI_pillageRange(1))
	{
		return;
	}

	if (plot()->getOwner() == getOwner())
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
		{
			return;
		}
	}

	if (AI_heal(50, 3))
	{
		return;
	}

	if (!isEnemy(plot()->getTeam()))
	{
		if (AI_heal())
		{
			return;
		}
	}

	if (AI_group(GroupingParams().withUnitAI(UNITAI_PILLAGE).maxGroupSize(1).maxOwnUnitAI(1).ignoreFaster().maxPathTurns(3)))
	{
		return;
	}

	if ((area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) || isEnemy(plot()->getTeam()))
	{
		if (AI_pillage(20))
		{
			return;
		}
	}

	if (AI_heal())
	{
		return;
	}

	//if (AI_guardCity(false, true, 3))
	//{
	//	return;
	//}

	if (AI_offensiveAirlift())
	{
		return;
	}

	if (AI_travelToUpgradeCity())
	{
		return;
	}

	if (!isHuman() && plot()->isCoastalLand() && GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, MISSIONAI_PICKUP) > 0)
	{
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (AI_patrol())
	{
		return;
	}

	//if (AI_retreatToCity())
	//{
	//	return;
	//}

	if (AI_safety(3))
	{
		return;
	}
	LOG_UNIT_BLOCK(4, {
		UnitAITypes eUnitAi = AI_getUnitAIType();
		MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = m_szName;
		if (StrUnitName.length() == 0)
		{
			StrUnitName = getName(0).GetCString();
		}

		logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Skip reached for pillageMove...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits());
	});

	getGroup()->pushMission(MISSION_SKIP);
	return;
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/
}

void CvUnitAI::AI_reserveMove()
{
	PROFILE_FUNC();

#ifdef OUTBREAKS_AND_AFFLICTIONS
	//TB Note: One big reason to split off healers into their own ai...
	for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
	{
		if (AI_cureAffliction((PromotionLineTypes)iI))
		{
			return;
		}
	}
#endif
	//TB OOS debug note: not terribly necessary to update into AND as this is not representing a fix but was adjusted to help with tracking
	bool bcheckSwitchToConstruct = checkSwitchToConstruct();
	if (bcheckSwitchToConstruct)
	{
		return;
	}

	bool bDanger = (GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 3));

	if (bDanger && AI_leaveAttack(2, 55, 130))
	{
		return;
	}

	if (plot()->getOwner() == getOwner())
	{
		if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
		{
			if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, ((SMCargoVolume() * 2) - 1), -1, MOVE_SAFE_TERRITORY))
			{
				return;
			}
			if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_WORKER, -1, -1, ((SMCargoVolume() * 2) - 1), -1, MOVE_SAFE_TERRITORY))
			{
				return;
			}
		}
		else
		{
			if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, 1, -1, MOVE_SAFE_TERRITORY))
			{
				return;
			}
			if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_WORKER, -1, -1, 1, -1, MOVE_SAFE_TERRITORY))
			{
				return;
			}
		}
	}

	//	No high priority actions to take so see if anyone requesting help
	if (processContracts())
	{
		return;
	}

	// NB: a RESERVE unit is no longer flipped to UNITAI_PROPERTY_CONTROL here. Property-control
    // units are produced as that role from creation by the need-based path in
    // CvCityAI::AI_chooseProduction (and the capability-checked AI_guardCity ejection); poaching
    // general reserve units caused a RESERVE<->PROPERTY_CONTROL oscillation (the handler bounced
    // non-capable / not-currently-needed units straight back). See AI_fulfillPropertyControlNeed.

	if (!plot()->isOwned())
	{
		if (AI_group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(1).maxPathTurns(1).allowRegrouping()))
		{
			return;
		}
	}

	if (AI_guardCity(true))
	{
		return;
	}

	if (!bDanger)
	{
		if (AI_group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(2).maxPathTurns(3).allowRegrouping()))
		{
			return;
		}
	}

	if (!noDefensiveBonus())
	{
		if (AI_guardFort(false))
		{
			return;
		}
	}

	if (AI_guardCityAirlift())
	{
		return;
	}

	if (AI_guardCity(false, true, 1))
	{
		return;
	}

	if (AI_guardCitySite())
	{
		return;
	}

	if (!noDefensiveBonus())
	{
		if (AI_guardFort(true))
		{
			return;
		}

		if (AI_guardBonus(15))
		{
			return;
		}
	}

	if (AI_heal(30, 1))
	{
		return;
	}

	if (bDanger)
	{
		if (AI_cityAttack(1, 55))
		{
			return;
		}

		if (AI_anyAttack(1, 60))
		{
			return;
		}
	}

	if (!noDefensiveBonus())
	{
		if (AI_guardCity(false, false))
		{
			return;
		}
	}

	if (bDanger)
	{
		if (AI_cityAttack(3, 45))
		{
			return;
		}

		if (AI_anyAttack(3, 50))
		{
			return;
		}
	}

	if (AI_protect(45, 8))
	{
		return;
	}

	if (AI_guardCity(false, true, 3))
	{
		return;
	}

	if (AI_defend())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (getGroup()->isStranded())
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (AI_safety(3))
	{
		return;
	}
	LOG_UNIT_BLOCK(4, {
		UnitAITypes eUnitAi = AI_getUnitAIType();
		MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = m_szName;
		if (StrUnitName.length() == 0)
		{
			StrUnitName = getName(0).GetCString();
		}

		logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Skip reached for reserveMove...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits());
	});

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_pillageCounterMove()
{
	PROFILE_FUNC();

	if (AI_establishStackSeeInvisibleCoverage())
	{
		return;
	}
	//	For now
	AI_counterMove();
}

void CvUnitAI::AI_counterMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	if (checkSwitchToConstruct())
	{
		return;
	}

	if (MISSIONAI_REGROUP == getGroup()->AI_getMissionAIType())
	{
		if (AI_group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(2).maxPathTurns(1).allowRegrouping()))
		{
			return;
		}
		if (AI_groupMergeRange(UNITAI_ATTACK_CITY, 1, false, true))
		{
			return;
		}
	}

	// Should never have group lead by counter unit
	if (getGroup()->getNumUnits() > 1 && getGroup()->getHeadUnitAI() == AI_getUnitAIType() && plot()->isCity() && plot()->getOwner() == getOwner())
	{
		//FErrorMsg("error"); // just interested in when this happens, not a problem
		getGroup()->AI_separate(); // will change group
		return;
	}

	if (!plot()->isOwned() && AI_groupMergeRange(UNITAI_SETTLE, 2, true, false, false))
	{
		return;
	}

	if (AI_guardCity(false, true, 1))
	{
		return;
	}

	// Super Forts begin *AI_defense*
	if (AI_guardFortMinDefender(true))
	{
		return;
	}
	// Super Forts end

	if (getSameTileHeal() + getHealUnitCombatCount() > 0)
	{
		if (!canAttack())
		{
			// Don't restrict to groups carrying cargo ... does this apply to any units in standard bts anyway?
			if (AI_shadow(UNITAI_ATTACK_CITY, -1, 21, false, false, 4))
			{
				return;
			}
		}
	}

	bool bDanger = (GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 3));
	AreaAITypes eAreaAIType = area()->getAreaAIType(getTeam());

	if (plot()->getOwner() == getOwner())
	{
		if (!bDanger)
		{
			if (plot()->isCity())
			{
				if ((eAreaAIType == AREAAI_ASSAULT) || (eAreaAIType == AREAAI_ASSAULT_ASSIST))
				{
					if (AI_offensiveAirlift())
					{
						return;
					}
				}
			}

			if ((eAreaAIType == AREAAI_ASSAULT) || (eAreaAIType == AREAAI_ASSAULT_ASSIST) || (eAreaAIType == AREAAI_ASSAULT_MASSING))
			{
				if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK_CITY, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
				{
					return;
				}

				if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
				{
					return;
				}
			}
		}

		if (!noDefensiveBonus())
		{
			if (AI_guardCity(false, false))
			{
				return;
			}
		}
	}

	//	No high priority actions to take so see if anyone requesting help
	if (processContracts())
	{
		return;
	}

	//join any city attacks in progress
	if (plot()->getOwner() != getOwner())
	{
		if (AI_groupMergeRange(UNITAI_ATTACK_CITY, 1, true, true))
		{
			return;
		}
	}

	if (bDanger)
	{
		if (AI_cityAttack(1, 35))
		{
			return;
		}

		if (AI_anyAttack(1, 40))
		{
			return;
		}
	}

	bool bIgnoreFasterStacks = false;
	if (GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_LAND_BLITZ))
	{
		if (area()->getAreaAIType(getTeam()) != AREAAI_ASSAULT)
		{
			bIgnoreFasterStacks = true;
		}
	}

	if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK_CITY).maxOwnUnitAI(2).ignoreFaster(bIgnoreFasterStacks).ignoreOwnUnitType().stackOfDoom().maxPathTurns(6)))
	{
		return;
	}

	bool bFastMovers = (GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_FASTMOVERS));

	if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK).maxGroupSize(2).ignoreFaster(bFastMovers).ignoreOwnUnitType().stackOfDoom().maxPathTurns(5)))
	{
		return;
	}

	// BBAI TODO: merge with nearby pillage

	if (AI_guardCity(false, true, 3))
	{
		return;
	}

	if (plot()->getOwner() == getOwner())
	{
		if (!bDanger)
		{
			if ((eAreaAIType != AREAAI_DEFENSIVE))
			{
				if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK_CITY, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
				{
					return;
				}

				if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
				{
					return;
				}
			}
		}
	}

	if (AI_heal())
	{
		return;
	}

	if (AI_offensiveAirlift())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety(3))
	{
		return;
	}
	LOG_UNIT_BLOCK(4, {
		UnitAITypes eUnitAi = AI_getUnitAIType();
		MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = m_szName;
		if (StrUnitName.length() == 0)
		{
			StrUnitName = getName(0).GetCString();
		}

		logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Skip reached for counterMove...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits());
	});

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


static bool canSpareCityFerryEscort(const CvUnit* unit)
{
    CvCity* city = unit->plot()->getPlotCity();
    if (city == NULL || city->getOwner() != unit->getOwner()
        || unit->isCargo() || unit->getGroup()->getNumUnits() != 1) return false;
    bool waiting = false;
    foreach_(const CvUnit* ship, unit->plot()->units())
        if (ship->getOwner() == unit->getOwner() && ship->AI_getUnitAIType() == UNITAI_SETTLER_SEA
            && ship->getUnitAICargo(UNITAI_SETTLE) > 0 && ship->getUnitAICargo(UNITAI_CITY_DEFENSE) == 0
            && unit->canLoadOntoUnit(ship, unit->plot())) { waiting = true; break; }
    if (!waiting) return false; // Avoid city-defense scans without an actual local boarding job.
    if (city->AI_isDanger()) return false;
    const int defenders = city->plot()->plotCount(PUF_canDefendGroupHead, -1, -1,
        NULL, unit->getOwner(), NO_TEAM, PUF_isCityAIType);
    if (defenders <= city->AI_neededDefenders()
        || !city->AI_isDefended(-unit->AI_genericUnitValueTimes100(UNITVALUE_FLAGS_DEFENSIVE) / 100)) return false;
    return !unit->isMilitaryHappiness() || city->AI_isAdequateHappinessMilitary(-1);
}

void CvUnitAI::AI_cityDefenseMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}
	if (checkSwitchToConstruct())
	{
		return;
	}

	if (AI_isNegativePropertyUnit())
	{
		AI_setUnitAIType(UNITAI_PILLAGE);
		return;
	}

	bool bDanger = (GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 3));

	if (MISSIONAI_REGROUP == getGroup()->AI_getMissionAIType())
	{
		if (AI_group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(5).maxOwnUnitAI(2).maxPathTurns(1).allowRegrouping()))
		{
			return;
		}
		if (AI_groupMergeRange(UNITAI_ATTACK_CITY, 1, false, true))
		{
			return;
		}
	}
	else if (!plot()->isOwned())
	{
		if (AI_group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(5).maxOwnUnitAI(2).maxPathTurns(2).allowRegrouping()))
		{
			return;
		}
	}

	if (bDanger)
	{
		//	Never remove the last unit from the city
		if (plot()->getPlotCity() == NULL || plot()->plotCount(PUF_canDefend /*PUF_canDefendGroupHead*/, -1, -1, NULL, getOwner()) != getGroup()->getNumUnits()) // XXX check for other team's units?
		{
			if (AI_leaveAttack(1, 70, 175))
			{
				return;
			}

			if (AI_chokeDefend())
			{
				return;
			}
		}
	}

	/************************************************************************************************/
	/* DCM									 04/19/09								Johny Smith  */
	/************************************************************************************************/
		// RevolutionDCM - new field bombard AI
		// Dale - RB: Field Bombard START
		//if (AI_RbombardCity())
		//{
		//	return;
		//}
		// Dale - RB: Field Bombard END

	/************************************************************************************************/
	/* DCM									 END												  */
	/************************************************************************************************/
	if (DefenseDemandPerf::enabled() && AI_guardFortMinDefender(false)) return;

	if (AI_guardCityBestDefender())
	{
		return;
	}

	// Safe-city garrisons should recover before optional outpost relocation.
	// Do not split mixed groups or suppress immediate defense.
	if (!bDanger && !isHuman() && !isCargo() && getDomainType() == DOMAIN_LAND
		&& getGroup()->getNumUnits() == 1 && getDamage() * 4 >= getMaxHP()
		&& !isAlwaysHeal() && plot()->getPlotCity() != NULL
		&& plot()->getOwner() == getOwner()
		&& plot()->getTotalTurnDamage(getGroup()) <= 0 && canHeal(plot()))
	{
		getGroup()->pushMission(MISSION_HEAL);
		return;
	}

	// Not needed as this city's top defender right now - if we're not in immediate
	//	danger, take the opportunity to head for wherever we can actually upgrade
	//	(e.g. Tribal Guardian -> Palace Guard needs the capital specifically).
	if (!bDanger && AI_travelToUpgradeCity())
	{
		return;
	}

	if (!bDanger && (isCargo() || canSpareCityFerryEscort(this)))
	{
		if (plot()->getOwner() == getOwner())
		{
			if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
			{
				if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, -1, 0, MOVE_SAFE_TERRITORY, 1))
				{
					return;
				}
			}
			else if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, -1, 0, MOVE_SAFE_TERRITORY, 1))
			{
				return;
			}
		}
	}

	if (AI_guardCityMinDefender(false))
	{
		return;
	}

	if (AI_guardCity(true))
	{
		return;
	}

	//	No high priority actions to take so see if anyone requesting help
	if (processContracts())
	{
		return;
	}

	// Super Forts begin *AI_defense*
	if (AI_guardFortMinDefender(true))
	{
		return;
	}
	// Super Forts end


	//TB: bolster national defenses please
	if (AI_guardCityMinDefender(true))
	{
		return;
	}

	if (!bDanger)
	{
		if (AI_group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(5).maxOwnUnitAI(2).maxPathTurns(2).allowRegrouping()))
		{
			return;
		}

		//if (AI_group(UNITAI_SETTLE, /*iMaxGroup*/ 2, -1, -1, false, false, false, /*iMaxPath*/ 2, /*bAllowRegrouping*/ true))
		//{
		//	return;
		//}

		if (plot()->getOwner() == getOwner())
		{
			if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
			{
				if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, ((SMCargoVolume() * 2) - 1), -1, MOVE_SAFE_TERRITORY))
				{
					return;
				}
			}
			else if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, 1, -1, MOVE_SAFE_TERRITORY))
			{
				return;
			}
		}
	}

	AreaAITypes eAreaAI = area()->getAreaAIType(getTeam());
	if ((eAreaAI == AREAAI_ASSAULT) || (eAreaAI == AREAAI_ASSAULT_MASSING) || (eAreaAI == AREAAI_ASSAULT_ASSIST))
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK_CITY, -1, -1, -1, 0, MOVE_SAFE_TERRITORY))
		{
			return;
		}
	}

	if ((AI_getBirthmark() % 4) == 0)
	{
		if (AI_guardFort())
		{
			return;
		}
	}

	if (AI_guardCityAirlift())
	{
		return;
	}

	if (AI_guardCity(false, true, 1))
	{
		return;
	}

	if (plot()->getOwner() == getOwner())
	{
		if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, 3, -1, -1, -1, MOVE_SAFE_TERRITORY))
		{
			// will enter here if in danger
			return;
		}
	}

	//join any city attacks in progress
	if (plot()->getOwner() != getOwner())
	{
		if (AI_groupMergeRange(UNITAI_ATTACK_CITY, 4, true, true))
		{
			return;
		}
	}

	if (AI_guardCity(false, true))
	{
		return;
	}

	if (!isNPC() && ((area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) || (area()->getAreaAIType(getTeam()) == AREAAI_MASSING)))
	{
		bool bIgnoreFaster = false;
		if (GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_LAND_BLITZ))
		{
			if (area()->getAreaAIType(getTeam()) != AREAAI_ASSAULT)
			{
				bIgnoreFaster = true;
			}
		}

		if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK_CITY).maxOwnUnitAI(2).minUnitAI(4).ignoreFaster(bIgnoreFaster)))
		{
			return;
		}
	}

	if (area()->getAreaAIType(getTeam()) == AREAAI_ASSAULT)
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK_CITY, 2, -1, -1, 1, MOVE_SAFE_TERRITORY))
		{
			return;
		}
	}

	// Ok, we're not needed locally - try to group up with others nearby
	// if we're not already in a city (preparatory to us all moving to one)
	if (plot()->getPlotCity() == NULL)
	{
		if (AI_groupMergeRange(UNITAI_CITY_DEFENSE, 3, false, true) ||
			AI_groupMergeRange(UNITAI_CITY_COUNTER, 3, false, true))
		{
			return;
		}

		if (AI_retreatToCity())
		{
			return;
		}
	}

	if (getGroup()->isStranded())
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (AI_safety())
	{
		return;
	}

	//This should make it so that if the AI accepts that the city is going to be overwhelmed then it starts splitting its units to buy time for more production and reinforcements.
	if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS) && plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			if (canSplit())
			{
				LOG_UNIT_BLOCK(3, {
					UnitAITypes eUnitAi = AI_getUnitAIType();
					MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
					CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
					CvWString MissionInfos = MissionAITypeToString(eMissionAI);
					CvWString StrUnitName = m_szName;
					if (StrUnitName.length() == 0)
					{
						StrUnitName = getName(0).GetCString();
					}

					logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], evade from City, Attacker too strong...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits());
				});
				doSplit();
			}
		}
		else
		{
			setInhibitMerge(false);
		}
	}

	if (getGroup()->getNumUnits() > 20)
	{
		if (canSplit())
		{
			LOG_UNIT_BLOCK(3, {
				UnitAITypes eUnitAi = AI_getUnitAIType();
				MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
				CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
				CvWString MissionInfos = MissionAITypeToString(eMissionAI);
				CvWString StrUnitName = m_szName;
				if (StrUnitName.length() == 0)
				{
					StrUnitName = getName(0).GetCString();
				}

				logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Stack too Big, Splitting...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits());
			});
			doSplit();
			return;
		}
		// An oversized selection group is not proof that a Size Matters
		// unit split is possible. With no action, use the normal skip below
		// instead of repeating this whole decision until the 50-pass limit.
	}

	LOG_UNIT_BLOCK(4, {
		UnitAITypes eUnitAi = AI_getUnitAIType();
		MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = m_szName;
		if (StrUnitName.length() == 0)
		{
			StrUnitName = getName(0).GetCString();
		}

		logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Skip reached for cityDefenseMove...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits());
	});

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_cityDefenseExtraMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	if (AI_isNegativePropertyUnit())
	{
		AI_setUnitAIType(UNITAI_PILLAGE);
		return;
	}

	if (!plot()->isOwned() && getGroup()->getHeadUnitAI() == UNITAI_CITY_COUNTER)
	{
		if (AI_group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(5).maxOwnUnitAI(2).maxPathTurns(4).allowRegrouping()))
		{
			return;
		}
	}

	if (AI_leaveAttack(2, 55, 150))
	{
		return;
	}

	if (AI_chokeDefend())
	{
		return;
	}

	if (getGroup()->getHeadUnitAI() == UNITAI_CITY_COUNTER)
	{
		if (AI_group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(5).maxOwnUnitAI(2).maxPathTurns(2).allowRegrouping()))
		{
			return;
		}
	}

	if (DefenseDemandPerf::enabled() && AI_guardFortMinDefender(false)) return;

	if (AI_guardCityBestDefender())
	{
		return;
	}

	if (plot()->getOwner() == getOwner())
	{
		if (getGroup()->getHeadUnitAI() == UNITAI_CITY_COUNTER && !GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 3))
		{
			if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
			{
				if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, ((SMCargoVolume() * 2) - 1), -1, MOVE_SAFE_TERRITORY, 1))
				{
					return;
				}
			}
			else if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, 1, -1, MOVE_SAFE_TERRITORY, 1))
			{
				return;
			}
		}

		if (plot()->getPlotCity()
		&& plot()->getPlotCity()->getOwner() == getOwner()
		&& plot()->plotCount(PUF_isUnitAIType, UNITAI_CITY_COUNTER, -1, NULL, getOwner()) < 2)
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, NULL);
			getGroup()->AI_setAsGarrison(plot()->getPlotCity());
			return;
		}
	}

	//	No high priority actions to take so see if anyone requesting help
	if (processContracts())
	{
		return;
	}

	// Super Forts begin *AI_defense*
	if (AI_guardFortMinDefender(true))
	{
		return;
	}

	if (AI_guardCity(true))
	{
		return;
	}

	if ((AI_getBirthmark() % 4) == 0)
	{
		if (AI_guardFort(true))
		{
			return;
		}
	}
	// Super Forts end

	if (processContracts())
	{
		return;
	}

	if (plot()->getPlotCity() && plot()->getPlotCity()->getOwner() == getOwner())
	{
		if (plot()->plotCount(PUF_canDefendGroupHead, -1, -1, NULL, getOwner(), NO_TEAM, PUF_isUnitAIType, AI_getUnitAIType()) == 1)
		{
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	if (AI_guardCityAirlift())
	{
		return;
	}

	if (AI_guardCity(false, true, 1))
	{
		return;
	}

	if (plot()->getOwner() == getOwner())
	{
		if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, 3, -1, -1, -1, MOVE_SAFE_TERRITORY, 3))
		{
			return;
		}
	}

	if (AI_guardCity(false, true))
	{
		return;
	}

	//	Ok, we're not needed locally - try to group up with others nearby
	//	if we're not already in a city (preparatory to us all moving to one)
	if (plot()->getPlotCity() == NULL)
	{
		if (AI_groupMergeRange(UNITAI_CITY_COUNTER, 3, false, true) ||
			AI_groupMergeRange(UNITAI_CITY_DEFENSE, 3, false, true))
		{
			return;
		}

		if (AI_retreatToCity())
		{
			return;
		}
	}

	if (AI_safety(3))
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_exploreMove()
{
	PROFILE_FUNC();

	if (AI_groupSelectStatus())
	{
		return;
	}
	MissionAITypes eMissionAIType = MISSIONAI_GROUP;

	if (!isHuman() && plot()->getOwner() == getOwner() && GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, &eMissionAIType, 1, getGroup(), 1) > 0)
	{
		// Wait for units which are joining our group this turn
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	//	If we have any animal hangers-on we should end the exploration and escort it home.
	if (plot()->getOwner() != getOwner() && getGroup()->countNumUnitAIType(UNITAI_SUBDUED_ANIMAL) > 0)
	{
		AI_setUnitAIType(UNITAI_HUNTER_ESCORT);
		return;
	}

	if (!isHuman() && canAttack())
	{
		if (AI_cityAttack(1, 60))
		{
			return;
		}

		if (AI_anyAttack(1, 70))
		{
			OutputDebugString(CvString::format("%S (%d) chooses to attack\n", getDescription().c_str(), m_iID).c_str());
			return;
		}
	}

	if (getDamage() > 0)
	{
		OutputDebugString(CvString::format("%S (%d) damaged at (%d,%d)...\n", getDescription().c_str(), m_iID, m_iX, m_iY).c_str());
		//	If there is an adjacent enemy seek safety before we heal
		if (exposedToDanger(plot(), 80))
		{
			OutputDebugString("	...plot is dangerous - seeking safety\n");
			if (AI_safety())
			{
				return;
			}
		}

		if (AI_heal())
		{
			OutputDebugString(CvString::format("	...healing at (%d,%d)\n", m_iX, m_iY).c_str());
			return;
		}
		else if (getDamagePercent() > 25)
		{
			//	Look for somewhere safer
			if (AI_safety(3))
			{
				return;
			}
		}
	}

	if (!isHuman())
	{
		if (AI_pillageRange(1))
		{
			return;
		}

		if (AI_cityAttack(3, 80))
		{
			return;
		}
	}

	if (AI_goody(4))
	{
		OutputDebugString(CvString::format("%S (%d) chooses to go for goody\n", getDescription().c_str(), m_iID).c_str());
		return;
	}

	if (AI_exploreRange(3))
	{
		return;
	}

	if (gUnitLogLevel > 2)
	{
		CvWString szString;
		getUnitAIString(szString, AI_getUnitAIType());
		logBBAI("	%S's %S at (%d,%d) finds nothing nearby as exploration target", GET_PLAYER(getOwner()).getCivilizationDescription(0), getName(0).GetCString(), getX(), getY());
	}

	if (!isHuman())
	{
		if (AI_pillageRange(3))
		{
			return;
		}
	}

	if (AI_explore())
	{
		return;
	}

	if (gUnitLogLevel > 2)
	{
		CvWString szString;
		getUnitAIString(szString, AI_getUnitAIType());
		logBBAI("	%S's %S at (%d,%d) finds no exploration target", GET_PLAYER(getOwner()).getCivilizationDescription(0), getName(0).GetCString(), getX(), getY());
	}

	if (!isHuman())
	{
		if (AI_pillage())
		{
			return;
		}
	}

	if (!isHuman())
	{
		if (AI_travelToUpgradeCity())
		{
			return;
		}
	}

	if (AI_refreshExploreRange(3))
	{
		return;
	}

	if (gUnitLogLevel > 2)
	{
		CvWString szString;
		getUnitAIString(szString, AI_getUnitAIType());
		logBBAI("	%S's %S at (%d,%d) finds nothing at all to explore", GET_PLAYER(getOwner()).getCivilizationDescription(0), getName(0).GetCString(), getX(), getY());
	}

	if (!isHuman() && AI_getUnitAIType() == UNITAI_EXPLORE
	&& GET_PLAYER(getOwner()).AI_totalAreaUnitAIs(area(), UNITAI_EXPLORE) > GET_PLAYER(getOwner()).AI_neededExplorers(area())
	&& GET_PLAYER(getOwner()).getFinalUnitUpkeep() > 0)
	{
		scrap();
		return;
	}

	if (!isHuman() && plot()->isCoastalLand() && GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, MISSIONAI_PICKUP) > 0)
	{
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (AI_travelToUpgradeCity())
	{
		return;
	}

	if (AI_patrol())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (getGroup()->isStranded())
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (AI_safety())
	{
		return;
	}
	LOG_UNIT_BLOCK(4, {
		UnitAITypes eUnitAi = AI_getUnitAIType();
		MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = m_szName;
		if (StrUnitName.length() == 0)
		{
			StrUnitName = getName(0).GetCString();
		}

		logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Skip reached for explorerMove...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits());
	});

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_missionaryMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	if (AI_spreadReligion())
	{
		return;
	}

	if (AI_spreadCorporation())
	{
		return;
	}

	if (AI_doInquisition())
	{
		return;
	}

	if (!isHuman() || isAutomated() && !GET_TEAM(getTeam()).isAtWar())
	{
		if (!isHuman() || (getGameTurnCreated() < GC.getGame().getGameTurn()))
		{
			if (AI_spreadReligionAirlift())
			{
				return;
			}
			if (AI_spreadCorporationAirlift())
			{
				return;
			}
		}

		if (!isHuman())
		{
			if (AI_load(UNITAI_MISSIONARY_SEA, MISSIONAI_LOAD_SPECIAL, NO_UNITAI, -1, -1, -1, 0, MOVE_SAFE_TERRITORY))
			{
				return;
			}

			if (AI_load(UNITAI_MISSIONARY_SEA, MISSIONAI_LOAD_SPECIAL, NO_UNITAI, -1, -1, -1, 0, MOVE_NO_ENEMY_TERRITORY))
			{
				return;
			}
		}
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (getGroup()->isStranded())
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (AI_safety())
	{
		return;
	}
	LOG_UNIT_BLOCK(4, {
		UnitAITypes eUnitAi = AI_getUnitAIType();
		MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = m_szName;
		if (StrUnitName.length() == 0)
		{
			StrUnitName = getName(0).GetCString();
		}

		logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Skip reached for missionaryMove...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits());
	});
	getGroup()->pushMission(MISSION_SKIP);
}

void CvUnitAI::AI_hunterEscortMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	if (getDamage() > 0)
	{
		OutputDebugString(CvString::format("%S (%d) damaged at (%d,%d)...\n", getDescription().c_str(), m_iID, m_iX, m_iY).c_str());
		//	If there is an adjacent enemy seek safety before we heal
		if (exposedToDanger(plot(), 80))
		{
			OutputDebugString("	...plot is dangerous - seeking safety\n");
			if (AI_safety())
			{
				return;
			}
		}

		if (AI_heal())
		{
			OutputDebugString(CvString::format("	...healing at (%d,%d)\n", m_iX, m_iY).c_str());
			return;
		}
		else if (getDamagePercent() > 25)
		{
			//	Look for somewhere safer
			if (AI_safety(3))
			{
				return;
			}
		}
	}

	//	If we have any animal hangers-on and are in our territory drop them off
	if (plot()->getOwner() == getOwner())
	{
		if (getGroup()->countNumUnitAIType(UNITAI_SUBDUED_ANIMAL) > 0)
		{
			getGroup()->AI_separateAI(UNITAI_SUBDUED_ANIMAL);
		}
		if (getGroup()->countNumUnitAIType(UNITAI_WORKER) > 0)
		{
			getGroup()->AI_separateAI(UNITAI_WORKER);
		}
	}
	else
	{
		if (AI_groupMergeRange(UNITAI_SUBDUED_ANIMAL, 1, false, true, true))
		{
			return;
		}
	}

	if (getGroup()->getNumUnits() == 1 && AI_groupMergeRange(UNITAI_HUNTER, 1, false, true, true))
	{
		return;
	}

	if (AI_huntRange(1, 90, false))
	{
		return;
	}

	if (getGroup()->countNumUnitAIType(UNITAI_SUBDUED_ANIMAL) > 0)
	{
		// If we have hangers-ons escort them back to our territory
		if (AI_reachHome(false, 3) || AI_retreatToCity())
		{
			return;
		}
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (AI_group(GroupingParams().withUnitAI(UNITAI_HUNTER).maxGroupSize(1).maxPathTurns(0)))
	{
		return;
	}

	if (processContracts())
	{
		return;
	}

	if (!isHuman() && plot()->isCoastalLand() && GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, MISSIONAI_PICKUP) > 0)
	{
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (m_contractualState == CONTRACTUAL_STATE_NO_WORK_FOUND)
	{
		if (m_pUnitInfo->getDefaultUnitAIType() == UNITAI_EXPLORE)
		{
			if (GET_PLAYER(getOwner()).AI_neededExplorers(area()) > GET_PLAYER(getOwner()).AI_totalAreaUnitAIs(area(), UNITAI_EXPLORE))
			{
				AI_setUnitAIType(UNITAI_EXPLORE);
				return;
			}
		}
		else if (m_pUnitInfo->getDefaultUnitAIType() != AI_getUnitAIType())
		{
			AI_setUnitAIType(m_pUnitInfo->getDefaultUnitAIType());
			return;
		}
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (getGroup()->isStranded())
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	//not needed anymore => RESERVE
	//#define MAX_HUNTER_ESCORT_PERCENT_NEEDS 5 //Calvitix Add a limit to the Join to PropControl Team (when already too much units)
	//const PlayerTypes eOwner = getOwner();
	//CvPlayerAI& player = GET_PLAYER(eOwner);
	//const CvArea* pArea = area();
	//int iHuntersInArea = player.AI_totalAreaUnitAIs(pArea, UNITAI_HUNTER_ESCORT);
	//int iUnitsInArea = player.getNumUnits();


	if (AI_safety())
	{
		return;
	}
	LOG_UNIT_BLOCK(4, {
		UnitAITypes eUnitAi = AI_getUnitAIType();
		MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = m_szName;
		if (StrUnitName.length() == 0)
		{
			StrUnitName = getName(0).GetCString();
		}

		logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Skip reached for hunterEscortMove...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits());
	});
	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_prophetMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	if (AI_construct(1))
	{
		return;
	}

	/*TB Prophet Mod begin*/
	if (AI_foundReligion())
	{
		return;
	}
	/*TB Prophet Mod end*/

	if (AI_discover(true))
	{
		return;
	}

	if (AI_heritage() || AI_construct())
	{
		return;
	}

	int iGoldenAgeValue = (GET_PLAYER(getOwner()).AI_calculateGoldenAgeValue() / (GET_PLAYER(getOwner()).unitsRequiredForGoldenAge()));
	int iDiscoverValue = std::max(1, getDiscoverResearch(NO_TECH));

	if (((iGoldenAgeValue * 100) / iDiscoverValue) > 60)
	{
		if (AI_goldenAge())
		{
			return;
		}

		if (iDiscoverValue > iGoldenAgeValue)
		{
			if (AI_discover())
			{
				return;
			}
			if (GET_PLAYER(getOwner()).getUnitCount(getUnitType()) > 1
			&& AI_join())
			{
				return;
			}
		}
	}
	else if (AI_discover() || AI_join())
	{
		return;
	}

	if (GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 2) || getGameTurnCreated() < GC.getGame().getGameTurn() - 25)
	{
		if (AI_discover())
		{
			return;
		}
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (getGroup()->isStranded())
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_artistMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	if (AI_artistCultureVictoryMove())
	{
		return;
	}

	if (AI_heritage() || AI_construct())
	{
		return;
	}

	if (AI_discover(true))
	{
		return;
	}

	if (AI_greatWork())
	{
		return;
	}

	int iGoldenAgeValue = (GET_PLAYER(getOwner()).AI_calculateGoldenAgeValue() / (GET_PLAYER(getOwner()).unitsRequiredForGoldenAge()));
	int iDiscoverValue = std::max(1, getDiscoverResearch(NO_TECH));

	if (((iGoldenAgeValue * 100) / iDiscoverValue) > 60)
	{
		if (AI_goldenAge())
		{
			return;
		}

		if (iDiscoverValue > iGoldenAgeValue)
		{
			if (AI_discover())
			{
				return;
			}
			if (GET_PLAYER(getOwner()).getUnitCount(getUnitType()) > 1
			&& AI_join())
			{
				return;
			}
		}
	}
	else if (AI_discover() || AI_join())
	{
		return;
	}

	if (GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 2) || getGameTurnCreated() < GC.getGame().getGameTurn() - 25)
	{
		if (AI_discover())
		{
			return;
		}
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (getGroup()->isStranded())
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_scientistMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	if (AI_discover(true))
	{
		return;
	}

	if (AI_heritage() || AI_construct(MAX_INT, 1))
	{
		return;
	}

	if (AI_join(2))
	{
		return;
	}

	if (AI_construct())
	{
		return;
	}

	int iGoldenAgeValue = (GET_PLAYER(getOwner()).AI_calculateGoldenAgeValue() / (GET_PLAYER(getOwner()).unitsRequiredForGoldenAge()));
	int iDiscoverValue = std::max(1, getDiscoverResearch(NO_TECH));

	if (((iGoldenAgeValue * 100) / iDiscoverValue) > 60)
	{
		if (AI_goldenAge())
		{
			return;
		}

		if (iDiscoverValue > iGoldenAgeValue)
		{
			if (AI_discover())
			{
				return;
			}
			if (GET_PLAYER(getOwner()).getUnitCount(getUnitType()) > 1
			&& AI_join())
			{
				return;
			}
		}
	}
	else if (AI_discover() || AI_join())
	{
		return;
	}


	if (GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 2) || getGameTurnCreated() < GC.getGame().getGameTurn() - 25)
	{
		if (AI_discover())
		{
			return;
		}
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (getGroup()->isStranded())
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_generalMove()
{
	PROFILE_FUNC();
	if (DoctrinePromotions::canFoundAny(*this) && AI_construct(MAX_INT, MAX_INT, 0, false, true, true)) return;

	std::vector<UnitAITypes> aeUnitAITypes;
	int iDanger = GET_PLAYER(getOwner()).AI_getPlotDanger(plot(), 2);

	bool bOffenseWar = (area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE);
	bool bCanDefend = getGroup()->canDefend();

	//	Generals always try to get an escort before moving away from a city
	if (!bCanDefend && m_contractsLastEstablishedTurn != GC.getGame().getGameTurn() && !isCargo())
	{
		//	For now advertise the work as being here since we'll be holding position
		//	while we wait for a defender.  It would be more optimal to 'meet' the defender
		//	on the way or at the target plot, but this might leave us exposed on the way so
		//	the calculations involved are somewhat harder and not attempted for now
		if (gUnitLogLevel >= 3)
		{
			logBBAI("	Great General for player %d (%S) at (%d,%d) requesting an escort",
					getOwner(),
					GET_PLAYER(getOwner()).getCivilizationDescription(0),
					getX(),
					getY());
		}

		GET_PLAYER(getOwner()).getContractBroker().advertiseWork(LOW_PRIORITY_ESCORT_PRIORITY, (unitCapabilities)(DEFENSIVE_UNITCAPABILITIES | OFFENSIVE_UNITCAPABILITIES), getX(), getY(), this);

		m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
		m_contractualState = CONTRACTUAL_STATE_AWAITING_ANSWER;

		//	Make sure we stay in charge when our escort joins us
		AI_setLeaderPriority(LEADER_PRIORITY_MAX);
	}

	if (isCommander())
	{
		//	Koshling - we cannot both advertise ourselves as available AND request help in the same turn, so alternate between
		//	for now (if we try to and happen to be last in the unit processing sequence out eventual request
		//	can wind up not being processed by any units so always forces a city build request - when built that unit may also
		//	not join us for exactly the same reason)
		const bool bLookForWork = ((GC.getGame().getGameTurn() % 2 == 0) && getLevel() < 10);

		//	If we're reasonably trained up then break off for any request, otherwise
		//	keep trying to train up unless there are high priority requests
		if (bLookForWork)
		{
			if (bCanDefend && processContracts((getLevel() < 4 ? LOW_PRIORITY_ESCORT_PRIORITY + 1 : 0)))
			{
				if (gUnitLogLevel >= 3 && m_contractualState == CONTRACTUAL_STATE_FOUND_WORK)
				{
					logBBAI("	Great General for player %d (%S) at (%d,%d) found contractual work",
							getOwner(),
							GET_PLAYER(getOwner()).getCivilizationDescription(0),
							getX(),
							getY());
				}
				return;
			}

			if (gUnitLogLevel >= 3)
			{
				logBBAI("	Great General for player %d (%S) at (%d,%d) found no urgent contractual work",
						getOwner(),
						GET_PLAYER(getOwner()).getCivilizationDescription(0),
						getX(),
						getY());
			}
		}

		if (bOffenseWar && getLevel() >= 4)
		{
			//try to join SoD (?)
			if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK).maxOwnUnitAI(1).minUnitAI(6).stackOfDoom().maxPathTurns(bCanDefend ? MAX_INT : 1).allowRegrouping()))
			{
				return;
			}
			if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK).maxOwnUnitAI(1).minUnitAI(4).stackOfDoom().maxPathTurns(bCanDefend ? MAX_INT : 1).allowRegrouping()))
			{
				return;
			}
			if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK_CITY).maxOwnUnitAI(1).minUnitAI(4).stackOfDoom().maxPathTurns(bCanDefend ? MAX_INT : 1).allowRegrouping()))
			{
				return;
			}
			if (AI_group(GroupingParams().withUnitAI(UNITAI_COLLATERAL).maxOwnUnitAI(1).minUnitAI(4).stackOfDoom().maxPathTurns(bCanDefend ? MAX_INT : 1).allowRegrouping()))
			{
				return;
			}
			if (AI_group(GroupingParams().withUnitAI(UNITAI_COUNTER).maxOwnUnitAI(1).minUnitAI(3).stackOfDoom().maxPathTurns(bCanDefend ? MAX_INT : 1).allowRegrouping()))
			{
				return;
			}
			//Koshling - remove attempts to join small stacks now that we have generals requesting escorts and
			//responding to explicit requests themselves
			//try to join attacking stack
			//if (AI_group(UNITAI_ATTACK, -1, 1, -1, false, false, false, bCanDefend ? MAX_INT : 1))
			//{
			//	return;
			//}

			//if (AI_group(UNITAI_ATTACK_CITY, -1, 1, -1, false))
			//{
			//	return;
			//}
			//try airlift
			if (AI_offensiveAirlift())
			{
				return;
			}
			//try load on transport
			if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
			{
				if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, (2 * GET_PLAYER(getOwner()).getBestUnitTypeCargoVolume(UNITAI_ASSAULT_SEA)), 0, ((bCanDefend ? MOVE_SAFE_TERRITORY : MOVE_OUR_TERRITORY) | MOVE_WITH_CAUTION)))
				{
					return;
				}
			}
			else if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, 2, -1, 0, ((bCanDefend ? MOVE_SAFE_TERRITORY : MOVE_OUR_TERRITORY) | MOVE_WITH_CAUTION)))
			{
				return;
			}
		}

		//	Try to hook up with a hunter and exp up - we always seek to have a stack of at least one
		//	general escort unit, the hunter, and the general
		bool bHasHunter = getGroup()->hasUnitOfAI(UNITAI_HUNTER);

		if (bHasHunter)
		{
			if (getGroup()->getNumUnits() > 2)
			{
				if (gUnitLogLevel > 2)
				{
					logBBAI("	%S's great commander (%d) at (%d,%d) [stack size %d, exp %d] executes hunter AI to exp up", GET_PLAYER(getOwner()).getCivilizationDescription(0), getID(), getX(), getY(), getGroup()->getNumUnits(), getExperience());
				}
				// If is high enough level, stop this behavior, get back to a city and release the hunter then become unavailable for this sort of assignment.
				if (getLevel() >= 10)
				{
					if (plot()->isCity(false, getTeam()))
					{
						getGroup()->AI_makeForceSeparate();
					}
					else if (AI_moveIntoNearestOwnedCity())
					{
						return;
					}
				}
				//	Already have hunter + another escort =- execute modified hunter AI
				AI_SearchAndDestroyMove(true);
				return;
			}
			else if (!bLookForWork && !isCargo())
			{
				//	Get another escort unit
				GET_PLAYER(getOwner()).getContractBroker().advertiseWork(

					HIGHEST_PRIORITY_ESCORT_PRIORITY,
					NO_UNITCAPABILITIES,
					getX(), getY(), this
				);

				if (gUnitLogLevel > 2)
				{
					logBBAI("	%S's great commander (%d) at (%d,%d) requests escort at priority %d", GET_PLAYER(getOwner()).getCivilizationDescription(0), getID(), getX(), getY(), HIGH_PRIORITY_ESCORT_PRIORITY);
				}
			}
		}
		else if (!bLookForWork && !isCargo())
		{
			GET_PLAYER(getOwner()).getContractBroker().advertiseWork(

				HIGHEST_PRIORITY_ESCORT_PRIORITY,
				NO_UNITCAPABILITIES,
				getX(), getY(), this,
				UNITAI_HUNTER
			);

			if (gUnitLogLevel > 2)
			{
				logBBAI("	%S's great commander (%d) at (%d,%d) [stack size %d] requests hunter at priority %d", GET_PLAYER(getOwner()).getCivilizationDescription(0), getID(), getX(), getY(), getGroup()->getNumUnits(), HIGH_PRIORITY_ESCORT_PRIORITY);
			}
		}

		if (AI_retreatToCity())
		{
			return;
		}
		if (AI_safety())
		{
			return;
		}
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}
	if (AI_command())
	{
		return;
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD & RevDCM					 09/03/10						jdog5000	  */
	/*																				phungus420	*/
	/* Great People AI, Unit AI																	 */
	/************************************************************************************************/
	if (AI_leadLegend())
	{
		return;
	}

	if (iDanger > 0)
	{
		aeUnitAITypes.clear();
		aeUnitAITypes.push_back(UNITAI_ATTACK);
		aeUnitAITypes.push_back(UNITAI_COUNTER);
		if (AI_lead(aeUnitAITypes))
		{
			return;
		}
	}

	if (AI_joinMilitaryCity())
	{
		return;
	}


	if (AI_construct(1))
	{
		return;
	}
	if (AI_join(1))
	{
		return;
	}

	if (bOffenseWar && (AI_getBirthmark() % 2 == 0))
	{
		aeUnitAITypes.clear();
		aeUnitAITypes.push_back(UNITAI_ATTACK_CITY);
		if (AI_lead(aeUnitAITypes))
		{
			return;
		}

		aeUnitAITypes.clear();
		aeUnitAITypes.push_back(UNITAI_ATTACK);
		if (AI_lead(aeUnitAITypes))
		{
			return;
		}
	}

	if (AI_heritage() || AI_join(2))
	{
		return;
	}

	if (AI_construct(2))
	{
		return;
	}
	if (AI_join(4))
	{
		return;
	}

	if (GC.getGame().getSorenRandNum(3, "AI General Construct") == 0)
	{
		if (AI_construct())
		{
			return;
		}
	}

	if (AI_join())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (getGroup()->isStranded())
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_greatHunterMove()
{
	PROFILE_FUNC();

	if (AI_groupSelectStatus())
	{
		return;
	}

	std::vector<UnitAITypes> aeUnitAITypes;

	int iGHrandom = GC.getGame().getSorenRandNum(5, "AI Great Hunter Decision");

	if (iGHrandom == 0 && AI_leadLegend())
	{
		return;
	}

	if (iGHrandom < 2)
	{
		AI_upgrade();
		if (isDelayedDeath())
		{
			//	Upgrade of original unit was successful
			return;
		}
	}

	if (iGHrandom < 4)
	{
		if (AI_joinMilitaryCity())
		{
			return;
		}
	}

	if (iGHrandom < 5)
	{
		aeUnitAITypes.clear();
		aeUnitAITypes.push_back(UNITAI_HUNTER);
		aeUnitAITypes.push_back(UNITAI_EXPLORE);
		if (AI_lead(aeUnitAITypes))
		{
			return;
		}
	}

	if (AI_construct(1))
	{
		return;
	}
	if (AI_join(1))
	{
		return;
	}

	if (AI_heritage() || AI_construct(2))
	{
		return;
	}

	if (AI_join(2))
	{
		return;
	}

	if (AI_construct())
	{
		return;
	}

	if (AI_join())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (getGroup()->isStranded())
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_greatAdmiralMove()
{
	PROFILE_FUNC();
	if (DoctrinePromotions::canFoundAny(*this) && AI_construct(MAX_INT, MAX_INT, 0, false, true, true)) return;

	std::vector<UnitAITypes> aeUnitAITypes;

	int iGArandom = GC.getGame().getSorenRandNum(4, "AI Great Admiral Decision");

	if (iGArandom == 0)
	{
		aeUnitAITypes.clear();
		aeUnitAITypes.push_back(UNITAI_ATTACK_SEA);
		aeUnitAITypes.push_back(UNITAI_RESERVE_SEA);
		aeUnitAITypes.push_back(UNITAI_CARRIER_SEA);
		if (AI_lead(aeUnitAITypes))
		{
			return;
		}
	}

	if (iGArandom < 2)
	{
		AI_upgrade();
		if (isDelayedDeath())
		{
			//	Upgrade of original unit was successful
			return;
		}
	}

	if (iGArandom < 4)
	{
		if (AI_joinMilitaryCity(true))
		{
			return;
		}
	}

	if (AI_heritage() || AI_construct())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (getGroup()->isStranded())
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_merchantMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	if (AI_heritage() || AI_construct())
	{
		return;
	}

	if (AI_discover(true))
	{
		return;
	}

	if (AI_caravan(false))
	{
		return;
	}
	if (AI_caravan(true))
	{
		return;
	}

	int iGoldenAgeValue = (GET_PLAYER(getOwner()).AI_calculateGoldenAgeValue() / (GET_PLAYER(getOwner()).unitsRequiredForGoldenAge()));
	int iDiscoverValue = std::max(1, getDiscoverResearch(NO_TECH));

	if (AI_trade(iGoldenAgeValue * 2))
	{
		return;
	}

	if (((iGoldenAgeValue * 100) / iDiscoverValue) > 60)
	{
		if (AI_goldenAge())
		{
			return;
		}

		if (AI_trade(iGoldenAgeValue))
		{
			return;
		}

		if (iDiscoverValue > iGoldenAgeValue)
		{
			if (AI_discover())
			{
				return;
			}
			if (GET_PLAYER(getOwner()).getUnitCount(getUnitType()) > 1
			&& AI_join())
			{
				return;
			}
		}
	}
	else if (AI_discover() || AI_join())
	{
		return;
	}


	if (GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 2) || getGameTurnCreated() < GC.getGame().getGameTurn() - 25)
	{
		if (AI_discover())
		{
			return;
		}
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (getGroup()->isStranded())
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_subduedAnimalMove()
{
	PROFILE_FUNC();

	//	Don't bother healing subdued animals in our own territory at least until after we test if they can construct
	if (plot()->getOwner() != getOwner())
	{
		// If they can get to our territory prefer that to healing
		if (AI_moveToOurTerritory(1))
		{
			return;
		}

		// Try to move to a nearby hunter escort unit if one is available to group with it
		if (AI_groupMergeRange(UNITAI_HUNTER_ESCORT, 1, false, true, true)
		// Try to move to a nearby explorer unit if one is available to group with it
		|| AI_groupMergeRange(UNITAI_EXPLORE, 1, false, true, true)
		// Try to move to a nearby hunter unit if one is available to group with it
		|| AI_groupMergeRange(UNITAI_HUNTER, 1, false, true, true))
		{
			return;
		}

		if (exposedToDanger(plot(), 80))
		{
			OutputDebugString("	...plot is dangerous - seeking safety\n");

			if (AI_safety())
			{
				return;
			}
		}

		if (!getGroup()->canDefend())
		{
			GET_PLAYER(getOwner()).getContractBroker().advertiseWork
			(
				HIGHEST_PRIORITY_ESCORT_PRIORITY,
				NO_UNITCAPABILITIES,
				getX(), getY(),
				this, UNITAI_HUNTER_ESCORT
			);
		}
		else if (AI_retreatToCity())
		{
			return;
		}
	}
	else if (getGroup()->getNumUnits() > 1)
	{
		//	Separate groups of subdued animals once they reach owned territory
		getGroup()->AI_separate();

		//	Will have changed group so the previous one no longer exists to deal with
		return;
	}

	if (AI_heritage() || AI_construct(MAX_INT, MAX_INT, 0, false, true))
	{
		OutputDebugString(CvString::format("%S (%d) chooses to head off to construct\n", getDescription().c_str(), m_iID).c_str());
		return;
	}

	if (AI_outcomeMission())
	{
		OutputDebugString(CvString::format("%S (%d) chooses to head off to do an outcome mission\n", getDescription().c_str(), m_iID).c_str());
		return;
	}

	AI_upgrade();

	if (isDelayedDeath())
	{
		//	Upgrade of original unit was successful
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_scrapSubdued())
	{
		OutputDebugString(CvString::format("%S (%d) chooses to disband\n", getDescription().c_str(), m_iID).c_str());
		return;
	}

	if (getGroup()->isStranded())
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (AI_safety())
	{
		return;
	}
	LOG_UNIT_BLOCK(4, {
		UnitAITypes eUnitAi = AI_getUnitAIType();
		MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = m_szName;
		if (StrUnitName.length() == 0)
		{
			StrUnitName = getName(0).GetCString();
		}

		logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Skip reached for subduedAnimalMove...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits());
	});

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

// Returns true if a mission was pushed...
// Seeks a plot where this unit can build a domesticated herd, preferring plots
// within the work radius of one of our cities (so the resulting bonus is of use),
// falling back to the nearest qualifying plot otherwise.
bool CvUnitAI::AI_herdMove()
{
	PROFILE_FUNC();

	const CvOutcomeList* pOutcomeList = getOutcomeListForMission(MISSION_BUILD_DOMESTICATED_HERD);

	if (pOutcomeList == NULL || !pOutcomeList->isPossibleSomewhere(*this))
	{
		// Nothing buildable and nothing spreadable right now (eg. missing tech, or no
		// qualifying city/plot this turn) -- stay automated and just try again next turn,
		// since AI_construct() may succeed later (new city founded, tech researched, etc.).
		getGroup()->pushMission(MISSION_SKIP);
		return true;
	}

	if (plot()->getOwner() != getOwner() && exposedToDanger(plot(), 80))
	{
		if (AI_safety())
		{
			return true;
		}
	}

	const int iBaseFlags = isHuman() ? MOVE_NO_ENEMY_TERRITORY : (MOVE_NO_ENEMY_TERRITORY | MOVE_IGNORE_DANGER);

	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestTargetPlot = NULL;
	int iBestValue = 0;

	CvReachablePlotSet plotSet(getGroup(), MOVE_NO_ENEMY_TERRITORY, MAX_INT);

	// Pass 1: plots within the work radius of one of our cities, so the resulting
	// bonus actually benefits a city.
	foreach_(const CvCity* pLoopCity, GET_PLAYER(getOwner()).cities())
	{
		for (int iIndex = 0; iIndex < NUM_CITY_PLOTS_2; iIndex++)
		{
			CvPlot* pCandidate = plotCity(pLoopCity->getX(), pLoopCity->getY(), iIndex);
			if (pCandidate == NULL || plotSet.find(pCandidate) == plotSet.end())
			{
				continue;
			}
			if (!pOutcomeList->isPossibleInPlot(*this, *pCandidate))
			{
				continue;
			}

			const int iValue = pOutcomeList->AI_getValueInPlot(*this, *pCandidate);
			if (iValue > iBestValue && generatePath(pCandidate, iBaseFlags, true))
			{
				iBestValue = iValue;
				pBestPlot = getPathEndTurnPlot();
				pBestTargetPlot = pCandidate;
			}
		}
	}

	// Pass 2: fall back to the nearest qualifying plot anywhere reachable.
	if (pBestTargetPlot == NULL)
	{
		for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
		{
			CvPlot* pCandidate = itr.plot();

			if (!pOutcomeList->isPossibleInPlot(*this, *pCandidate))
			{
				continue;
			}

			const int iValue = pOutcomeList->AI_getValueInPlot(*this, *pCandidate) * 1000 / (1 + itr.stepDistance());
			if (iValue > iBestValue && generatePath(pCandidate, iBaseFlags, true))
			{
				iBestValue = iValue;
				pBestPlot = getPathEndTurnPlot();
				pBestTargetPlot = pCandidate;
			}
		}
	}

	if (pBestPlot != NULL && pBestTargetPlot != NULL)
	{
		if (atPlot(pBestTargetPlot))
		{
			getGroup()->pushMission(MISSION_BUILD_DOMESTICATED_HERD);
			return true;
		}

		FAssert(!atPlot(pBestPlot));
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), iBaseFlags, false, false, NO_MISSIONAI, pBestTargetPlot);
	}

	getGroup()->pushMission(MISSION_SKIP);
	return true;
}

void CvUnitAI::AI_engineerMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	if (AI_heritage() || AI_construct())
	{
		OutputDebugString(CvString::format("%S (%d) chooses to head off to construct\n", getDescription().c_str(), m_iID).c_str());
		return;
	}

	if (AI_switchHurry())
	{
		return;
	}

	if (AI_hurry())
	{
		return;
	}

	if (AI_discover(true))
	{
		return;
	}

	int iGoldenAgeValue = (GET_PLAYER(getOwner()).AI_calculateGoldenAgeValue() / (GET_PLAYER(getOwner()).unitsRequiredForGoldenAge()));
	int iDiscoverValue = std::max(1, getDiscoverResearch(NO_TECH));

	if (((iGoldenAgeValue * 100) / iDiscoverValue) > 60)
	{
		if (AI_goldenAge())
		{
			return;
		}

		if (iDiscoverValue > iGoldenAgeValue)
		{
			if (AI_discover())
			{
				return;
			}
			if (GET_PLAYER(getOwner()).getUnitCount(getUnitType()) > 1
			&& AI_join())
			{
				return;
			}
		}
	}
	else if (AI_discover() || AI_join())
	{
		return;
	}


	if (GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 2) || getGameTurnCreated() < GC.getGame().getGameTurn() - 25)
	{
		if (AI_discover())
		{
			return;
		}
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (getGroup()->isStranded())
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_spyMove()
{
	PROFILE_FUNC();

	CvTeamAI& kTeam = GET_TEAM(getTeam());
	int iEspionageChance = 0;
	if (plot()->isOwned() && plot()->getTeam() != getTeam())
	{
		switch (GET_PLAYER(getOwner()).AI_getAttitude(plot()->getOwner()))
		{
		case ATTITUDE_FURIOUS:
			iEspionageChance = 100;
			break;

		case ATTITUDE_ANNOYED:
			iEspionageChance = 50;
			break;

		case ATTITUDE_CAUTIOUS:
			iEspionageChance = 20;
			break;

		case ATTITUDE_PLEASED:
			iEspionageChance = 7;
			break;

		case ATTITUDE_FRIENDLY:
			iEspionageChance = 0;
			break;

		default:
			FErrorMsg("error");
		}

		const WarPlanTypes eWarPlan = kTeam.AI_getWarPlan(plot()->getTeam());

		switch (eWarPlan)
		{
		case WARPLAN_LIMITED:
		case WARPLAN_TOTAL:
		{
			iEspionageChance += 50;
		}
		case NO_WARPLAN:
		{
			break;
		}
		default:
		{
			iEspionageChance += 20;
		}
		}

		if (plot()->isCity() && plot()->getTeam() != getTeam())
		{
			bool bTargetCity = false;

			// would we have more power if enemy defenses were down?
			int iOurPower = GET_PLAYER(getOwner()).AI_getOurPlotStrength(plot(), 1, false, true);
			int iEnemyPower = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 0, false, false);

			if (5 * iOurPower > 6 * iEnemyPower && eWarPlan != NO_WARPLAN)
			{
				bTargetCity = true;

				if (AI_revoltCitySpy())
				{
					return;
				}

				if (GC.getGame().getSorenRandNum(5, "AI Spy Skip Turn") > 0)
				{
					getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_ATTACK_SPY);
					return;
				}

				if (AI_cityOffenseSpy(5, plot()->getPlotCity()))
				{
					return;
				}
			}

			if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(plot(), MISSIONAI_ASSAULT, getGroup()) > 0)
			{
				bTargetCity = true;

				getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_ATTACK_SPY);
				return;
			}

			if (!bTargetCity)
			{
				// normal city handling
				if (getFortifyTurns() >= GC.getMAX_FORTIFY_TURNS())
				{
					if (AI_espionageSpy())
					{
						return;
					}
				}
				else if (GC.getGame().getSorenRandNum(100, "AI Spy Skip Turn") > 5)
				{
					// don't get stuck forever
					getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_ATTACK_SPY);
					return;
				}
			}
		}
		else if (iEspionageChance > 99 || GC.getGame().getSorenRandNum(100, "AI Spy Espionage") < iEspionageChance)
		{
			// This applies only when not in an enemy city, so for destroying improvements
			if (AI_espionageSpy())
			{
				return;
			}
		}
	}

	if (plot()->getTeam() == getTeam())
	{
		if (!kTeam.hasWarPlan(true)
		|| GET_PLAYER(getOwner()).AI_isDoVictoryStrategy(AI_VICTORY_SPACE4)
		|| GET_PLAYER(getOwner()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3))
		{
			if (GC.getGame().getSorenRandNum(10, "AI Spy defense") > 0)
			{
				if (AI_guardSpy(0))
				{
					return;
				}
			}
		}

		if (GC.getGame().getSorenRandNum(100, "AI Spy pillage improvement") < 25)
		{
			if (AI_bonusOffenseSpy(5))
			{
				return;
			}
		}
		else if (AI_cityOffenseSpy(10))
		{
			return;
		}
	}

	if (iEspionageChance > 0 && (plot()->isCity() || (plot()->getNonObsoleteBonusType(getTeam()) != NO_BONUS)))
	{
		if (GC.getGame().getSorenRandNum(7, "AI Spy Skip Turn") > 0)
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_ATTACK_SPY);
			return;
		}
	}

	if (area()->getNumCities() > area()->getCitiesPerPlayer(getOwner()))
	{
		if (GC.getGame().getSorenRandNum(4, "AI Spy Choose Movement") > 0)
		{
			if (AI_reconSpy(3))
			{
				return;
			}
		}
		else if (AI_cityOffenseSpy(10))
		{
			return;
		}
	}

	if (AI_load(UNITAI_SPY_SEA, MISSIONAI_LOAD_SPECIAL, NO_UNITAI, -1, -1, -1, 0, MOVE_NO_ENEMY_TERRITORY))
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_ICBMMove()
{
	PROFILE_FUNC();

	if (airRange() > 0)
	{
		if (AI_nukeRange(airRange()))
		{
			return;
		}
	}
	else if (AI_nuke())
	{
		return;
	}

	if (isCargo())
	{
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (airRange() > 0)
	{
		if (plot()->isCity(true))
		{
			const int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
			const int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

			if (4 * iEnemyOffense > iOurDefense || iOurDefense == 0)
			{
				// Too risky, pull back
				if (AI_airOffensiveCity())
				{
					return;
				}
			}
		}

		if (AI_missileLoad(UNITAI_MISSILE_CARRIER_SEA, 2, true)
		|| AI_missileLoad(UNITAI_MISSILE_CARRIER_SEA, 1, false))
		{
			return;
		}
		if ((AI_getBirthmark() % 3) == 0 && AI_missileLoad(UNITAI_ATTACK_SEA, 0, false))
		{
			return;
		}
		if (AI_airOffensiveCity())
		{
			return;
		}
	}
	getGroup()->pushMission(MISSION_SKIP);
}


void CvUnitAI::AI_workerSeaMove()
{
	AI_PERF_SCOPE(WORKER_MOVE);
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	if (!getGroup()->canDefend() && GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot()))
	{
		if (AI_retreatToCity())
		{
			return;
		}
	}

	// Finish a profitable complementary job on the current improved tile before
	// sending the boat elsewhere. This never replaces the primary improvement.
	const CvPlot* currentPlot = plot();
	CvCity* workingCity = currentPlot->getWorkingCity();
	if (CvAdditionalImprovements::enabled() && currentPlot->isWater()
	&& currentPlot->getImprovementType() != NO_IMPROVEMENT
	&& workingCity && workingCity->getOwner() == getOwner())
	{
		workingCity->AI_updateBestBuild();
		const BuildTypes localBuild = workingCity->AI_getBestBuild(workingCity->getCityPlotIndex(currentPlot));
		if (localBuild != NO_BUILD
		&& CvAdditionalImprovements::usesSecondaryLayer(GC.getBuildInfo(localBuild).getImprovement())
		&& workingCity->AI_getBestBuildValue(workingCity->getCityPlotIndex(currentPlot)) > 0
		&& canBuild(currentPlot, localBuild))
		{
			// We are already on the target. A travel/route mission can reject the
			// zero-length path or try a sea tunnel instead of starting construction.
			getGroup()->pushMission(MISSION_BUILD, localBuild, -1, 0, false, false, MISSIONAI_BUILD, currentPlot);
			return;
		}
	}

	std::vector<int> bonusCandidates;
	const bool boundedCandidates = SeaWorkerBonusCandidates::collect(*this, bonusCandidates);
	// iMinValue only controls the optional output arguments; all three old
	// no-output calls chose the same job. A failed attempt needs no immediate retry.
	if (AI_improveBonus(0, NULL, NULL, NULL, boundedCandidates ? &bonusCandidates : NULL))
	{
		return;
	}

	if (AI_improveLocalPlot(2, NULL))
	{
		return;
	}

	if (!isHuman() && AI_getUnitAIType() == UNITAI_WORKER_SEA)
	{
		const CvCity* pCity = plot()->getPlotCity();

		if (pCity && pCity->getOwner() == getOwner()
		&& (pCity->AI_neededSeaWorkers() == 0
			|| GET_PLAYER(getOwner()).AI_isFinancialTrouble()
			&& GET_PLAYER(getOwner()).getUnitUpkeepNet(isMilitaryBranch(), getUpkeep100()) > 0))
		{
			//Probably icelocked since it can't perform actions.
			scrap();
			return;
		}
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_barbAttackSeaMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	// Less suicide, always chase good targets
	if (AI_anyAttack(2, 51))
	{
		return;
	}

	if (AI_pillageRange(1))
	{
		return;
	}

	if (AI_anyAttack(1, 34))
	{
		return;
	}

	// We're easy to take out if wounded
	if (AI_heal())
	{
		return;
	}

	if (AI_pillageRange(3))
	{
		return;
	}

	// Barb ships will often hang out for a little while blockading before moving on
	if ((GC.getGame().getGameTurn() + getID()) % 12 > 5)
	{
		if (AI_pirateBlockade())
		{
			return;
		}
	}

	if (GC.getGame().getSorenRandNum(3, "AI Check trapped") == 0)
	{
		// If trapped in small hole in ice or around tiny island, disband to allow other units to be generated
		bool bScrap = true;
		const int iMaxRange = baseMoves() + 2;
		foreach_(const CvPlot * pLoopPlot, plot()->rect(iMaxRange, iMaxRange))
		{
			if (AI_plotValid(pLoopPlot))
			{
				int iPathTurns;
				if (generatePath(pLoopPlot, 0, true, &iPathTurns))
				{
					if (iPathTurns > 1)
					{
						bScrap = false;
						break;
					}
				}
			}
		}

		if (bScrap)
		{
			scrap();
		}
	}

	if (AI_patrol())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_pirateSeaMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	CvArea* pWaterArea;

	// heal in defended, unthreatened forts and cities
	if (plot()->isCity(true) && (GET_PLAYER(getOwner()).AI_getOurPlotStrength(plot(), 0, true, false) > 0) && !(GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 2, false)))
	{
		if (AI_heal())
		{
			return;
		}
	}

	if (plot()->isOwned() && (plot()->getTeam() == getTeam()))
	{
		if (AI_anyAttack(2, 40))
		{
			return;
		}

		//if (AI_protect(30))
		if (AI_protect(40, 3))
		{
			return;
		}


		if (((AI_getBirthmark() / 8) % 2) == 0)
		{
			// Previously code actually blocked grouping
			if (AI_group(GroupingParams().withUnitAI(UNITAI_PIRATE_SEA).maxOwnUnitAI(1).ignoreFaster().maxPathTurns(8)))
			{
				return;
			}
		}
	}
	else
	{
		if (AI_anyAttack(2, 51))
		{
			return;
		}
	}


	if (GC.getGame().getSorenRandNum(10, "AI Pirate Explore") == 0)
	{
		pWaterArea = plot()->waterArea();

		if (pWaterArea != NULL)
		{
			if (pWaterArea->getNumUnrevealedTiles(getTeam()) > 0)
			{
				if (GET_PLAYER(getOwner()).AI_areaMissionAIs(pWaterArea, MISSIONAI_EXPLORE, getGroup()) < (GET_PLAYER(getOwner()).AI_neededExplorers(pWaterArea)))
				{
					if (AI_exploreRange(2))
					{
						return;
					}
				}
			}
		}
	}

	if (GC.getGame().getSorenRandNum(11, "AI Pirate Pillage") == 0)
	{
		if (AI_pillageRange(1))
		{
			return;
		}
	}

	//Includes heal and retreat to sea routines.
	if (AI_pirateBlockade())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_attackSeaMove()
{
    NAVAL_DIAG_ROOT(GC.getGame().getGameTurn(),getOwner(),getID(),getGroup()->getID(),getX(),getY());

	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	if (plot()->isCity(true))
	{
		PROFILE("CvUnitAI::AI_attackSeaMove.City");

		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

		if (getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			if (AI_anyAttack(2, 50))
			{
				return;
			}

			if (AI_shadow(UNITAI_ASSAULT_SEA, 4, 34, false, true, 2))
			{
				return;
			}

			if (AI_protect(35, 3))
			{
				return;
			}

			if (AI_retreatToCity())
			{
				return;
			}

			if (AI_safety())
			{
				return;
			}
		}
	}

	if (AI_heal(30, 1))
	{
		return;
	}

	// RevolutionDCM - sea bombard AI formally DCM 1.7 AI_RbombardCity()
	// Dale - RB: Field Bombard START
	//if (AI_RbombardCity())
	//{
	//	return;
	//}
	//if (AI_RbombardNaval())
	//{
	//	return;
	//}
	// Dale - RB: Field Bombard END

	{
		PROFILE("CvUnitAI::AI_attackSeaMove.BasicAttacks");

		if (AI_anyAttack(1, 35))
		{
			return;
		}

		if (AI_seaBombardRange(1))
		{
			return;
		}

		if (AI_anyAttack(2, 40))
		{
			return;
		}

		if (AI_goody(2))
		{
			return;
		}

		if (AI_seaBombardRange(6))
		{
			return;
		}

		if (AI_heal(50, 3))
		{
			return;
		}

		if (AI_heal())
		{
			return;
		}
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  08/10/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
		// BBAI TODO: Turn this into a function, have docked escort ships do it to

		//Fuyu: search for more attackers, and when enough are found, always try to break through
	CvCity* pCity = plot()->getPlotCity();

	if (pCity != NULL)
	{
		if (pCity->isBlockaded())
		{
			PROFILE("CvUnitAI::AI_attackSeaMove.Blockaded");

			const int iBlockadeRange = GC.getSHIP_BLOCKADE_RANGE();
			// City under blockade
			// Attacker has low odds since anyAttack checks above passed, try to break if sufficient numbers

			int iAttackers = plot()->plotCount(PUF_isUnitAIType, UNITAI_ATTACK_SEA, -1, NULL, NO_PLAYER, getTeam(), PUF_isGroupHead, -1, -1);
			int iBlockaders = GET_PLAYER(getOwner()).AI_getWaterDanger(plot(), (iBlockadeRange + 1));
			//bool bBreakBlockade = (iAttackers > (iBlockaders + 2) || iAttackers >= 2*iBlockaders);

			const int iMaxRange = iBlockadeRange - 1;
			if (gUnitLogLevel > 2) logBBAI("	  Not enough attack fleet found in %S, searching for more in a %d-tile radius", pCity->getName().GetCString(), iMaxRange);

			foreach_(const CvPlot * pLoopPlot, plot()->rect(iMaxRange, iMaxRange))
			{
				if (pLoopPlot->isWater())
				{
					if (pLoopPlot->getBlockadedCount(getTeam()) > 0)
					{
						iAttackers += pLoopPlot->plotCount(PUF_isUnitAIType, UNITAI_ATTACK_SEA, -1, NULL, NO_PLAYER, getTeam(), PUF_isGroupHead, -1, -1);
					}
				}
			}
			//bBreakBlockade = (iAttackers > (iBlockaders + 2) || iAttackers >= 2*iBlockaders);

			//if (bBreakBlockade)
			if (iAttackers > iBlockaders + 2 || iAttackers >= 2 * iBlockaders)
			{
				if (gUnitLogLevel > 2) logBBAI("	  Found %d attackers and %d blockaders, proceeding to break blockade", iAttackers, iBlockaders);
				//if (iAttackers > GC.getGame().getSorenRandNum(2*iBlockaders + 1, "AI - Break blockade"))
				{
					// BBAI TODO: Make odds scale by # of blockaders vs number of attackers
					if (baseMoves() >= iBlockadeRange)
					{
						if (AI_anyAttack(1, 15))
						{
							return;
						}
					}
					else
					{
						//Fuyu: Even slow ships should attack
						//Assuming that every ship can reach a blockade with 2 moves
						if (AI_anyAttack(2, 15))
						{
							return;
						}
					}

					//If no mission was pushed yet and we have a lot of ships, try again with even lower odds
					if (iAttackers > 2 * iBlockaders)
					{
						if (AI_anyAttack(1, 10))
						{
							return;
						}
					}
				}
			}
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/
	if (!isHuman() && !isNPC())
	{
		if (AI_establishStackSeeInvisibleCoverage())
		{
			return;
		}
	}

	if (AI_group(GroupingParams().withUnitAI(UNITAI_CARRIER_SEA).maxGroupSize(4).maxOwnUnitAI(1).ignoreFaster().maxPathTurns(5)))
	{
		return;
	}

	if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK_SEA).maxGroupSize(1).ignoreFaster().maxPathTurns(3)))
	{
		return;
	}

	if (!plot()->isOwned() || !isEnemy(plot()->getTeam()))
	{
		PROFILE("CvUnitAI::AI_attackSeaMove.Neutral");

		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD					  01/11/09								jdog5000	  */
		/*																							  */
		/* Naval AI																					 */
		/************************************************************************************************/
		/* original bts code
				if (AI_shadow(UNITAI_ASSAULT_SEA, 4, 34))
				{
					return;
				}

				if (AI_shadow(UNITAI_CARRIER_SEA, 4, 51))
				{
					return;
				}

				if (AI_group(UNITAI_ASSAULT_SEA, -1, 4, -1, false, false, false))
				{
					return;
				}
			}

			if (AI_group(UNITAI_CARRIER_SEA, -1, 1, -1, false, false, false))
			{
				return;
			}
		*/
		if (AI_shadow(UNITAI_ASSAULT_SEA, 4, 34, true, false, 4))
		{
			return;
		}

		if (AI_shadow(UNITAI_CARRIER_SEA, 4, 51, true, false, 5))
		{
			return;
		}

		// Group with large flotillas first
		if (AI_group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).maxOwnUnitAI(4).minUnitAI(3).maxPathTurns(3).withCargoOnly()))
		{
			return;
		}

		if (AI_group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).maxOwnUnitAI(2).maxPathTurns(5).withCargoOnly()))
		{
			return;
		}
	}

	if (AI_group(GroupingParams().withUnitAI(UNITAI_CARRIER_SEA).maxOwnUnitAI(1).maxPathTurns(10)))
	{
		return;
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/


	if (plot()->isOwned() && (isEnemy(plot()->getTeam())))
	{
		if (AI_blockade())
		{
			return;
		}
	}
	if (AI_pillageRange(4))
	{
		return;
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  09/01/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
		//if (AI_protect(35))
	if (AI_protect(35, 3))
	{
		return;
	}

	if (AI_protect(35, 8))
		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD					   END												  */
		/************************************************************************************************/
	{
		return;
	}

	if (AI_travelToUpgradeCity())
	{
		return;
	}

	if (AI_seaAreaAttack())
	{
		return;
	}

	if (AI_patrol())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_reserveSeaMove()
{
	PROFILE_FUNC();

	if (plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

		if (getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0)  //prioritize getting outta there
		{
			if (AI_anyAttack(2, 60))
			{
				return;
			}

			//if (AI_protect(40))
			if (AI_protect(40, 3))
			{
				return;
			}

			if (AI_shadow(UNITAI_SETTLER_SEA, 2, -1, false, true, 4))
			{
				return;
			}

			if (AI_retreatToCity())
			{
				return;
			}

			if (AI_safety())
			{
				return;
			}
		}
	}

	if (!isHuman() && !isNPC())
	{
		if (AI_establishStackSeeInvisibleCoverage())
		{
			return;
		}
	}

	if (AI_guardBonus(30))
	{
		return;
	}

	if (AI_heal(30, 1))
	{
		return;
	}

	if (AI_anyAttack(1, 55))
	{
		return;
	}

	if (AI_seaBombardRange(6))
	{
		return;
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  09/01/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
		//if (AI_protect(40))
	if (AI_protect(40, 5))
		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD					   END												  */
		/************************************************************************************************/
	{
		return;
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  01/03/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
	/* original bts code
		if (AI_shadow(UNITAI_SETTLER_SEA, 1, -1, true))
		{
			return;
		}

		if (AI_group(UNITAI_RESERVE_SEA, 1))
		{
			return;
		}

		if (bombardRate() > 0)
		{
			if (AI_shadow(UNITAI_ASSAULT_SEA, 2, 30, true))
			{
				return;
			}
		}
	*/
	// Shadow any nearby settler sea transport out at sea
	if (AI_shadow(UNITAI_SETTLER_SEA, 2, -1, false, true, 5))
	{
		return;
	}

	if (AI_group(GroupingParams().withUnitAI(UNITAI_RESERVE_SEA).maxGroupSize(1).maxPathTurns(8)))
	{
		return;
	}

	if (getGroup()->hasBombardCapability())
	{
		if (AI_shadow(UNITAI_ASSAULT_SEA, 2, 30, true, false, 8))
		{
			return;
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (AI_heal(50, 3))
	{
		return;
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  09/01/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
	if (AI_protect(40))
	{
		return;
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (AI_anyAttack(3, 45))
	{
		return;
	}

	if (AI_heal())
	{
		return;
	}

	if (!isNeverInvisible())
	{
		if (AI_anyAttack(5, 35))
		{
			return;
		}
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  01/03/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					  */
	/************************************************************************************************/
		// Shadow settler transport with cargo
	if (AI_shadow(UNITAI_SETTLER_SEA, 1, -1, true, false, 10))
	{
		return;
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (AI_travelToUpgradeCity())
	{
		return;
	}

	if (AI_patrol())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_escortSeaMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}
	//	// if we have cargo, possibly convert to UNITAI_ASSAULT_SEA (this will most often happen with galleons)
	//	// note, this should not happen when we are not the group head, so escort galleons are fine joining a group, just not as head
	//	if (hasCargo() && (getUnitAICargo(UNITAI_ATTACK_CITY) > 0 || getUnitAICargo(UNITAI_ATTACK) > 0))
	//	{
	//		// non-zero AI_unitValue means that UNITAI_ASSAULT_SEA is valid for this unit (that is the check used everywhere)
	//		if (GET_PLAYER(getOwner()).AI_unitValue(getUnitType(), UNITAI_ASSAULT_SEA, NULL) > 0)
	//		{
	//			// save old group, so we can merge it back in
	//			CvSelectionGroup* pOldGroup = getGroup();
	//
	//			// this will remove this unit from the current group
	//			AI_setUnitAIType(UNITAI_ASSAULT_SEA);
	//
	//			// merge back the rest of the group into the new group
	//			CvSelectionGroup* pNewGroup = getGroup();
	//			if (pOldGroup != pNewGroup)
	//			{
	//				pOldGroup->mergeIntoGroup(pNewGroup);
	//			}
	//
	//			// perform assault sea action
	//			AI_assaultSeaMove();
	//			return;
	//		}
	//	}

	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						06/14/09	Solver & jdog5000	*/
	/* 																			*/
	/* 	Naval AI																*/
	/********************************************************************************/
	if (plot()->isCity(true)) //prioritize getting outta there
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

		if (getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0)
		{
			if (AI_anyAttack(1, 60))
			{
				return;
			}

			if (AI_group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).maxOwnUnitAI(1).ignoreFaster().maxPathTurns(1)))
			{
				return;
			}

			if (AI_retreatToCity())
			{
				return;
			}

			if (AI_safety())
			{
				return;
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	if (AI_heal(30, 1))
	{
		return;
	}

	if (AI_anyAttack(1, 55))
	{
		return;
	}

	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						9/14/08			jdog5000		*/
	/* 																			*/
	/* 	Naval AI																*/
	/********************************************************************************/
		// Galleons can get stuck with this AI type since they don't upgrade to any escort unit
		// Galleon escorts are much less useful once Frigates or later are available
	if (!isHuman() && !isNPC())
	{
		if (AI_establishStackSeeInvisibleCoverage())
		{
			return;
		}

		if (hasCargo() && (GC.getUnitInfo(getUnitType()).getSpecialCargo() == NO_SPECIALUNIT))
		{
			//Obsolete?
			int iValue = GET_PLAYER(getOwner()).AI_unitValue(getUnitType(), AI_getUnitAIType(), area());
			int iBestValue = GET_PLAYER(getOwner()).AI_bestAreaUnitAIValue(AI_getUnitAIType(), area());

			if (iValue < iBestValue)
			{
				if (GET_PLAYER(getOwner()).AI_unitValue(getUnitType(), UNITAI_ASSAULT_SEA, area()) > 0)
				{
					AI_setUnitAIType(UNITAI_ASSAULT_SEA);
					return;
				}

				if (GET_PLAYER(getOwner()).AI_unitValue(getUnitType(), UNITAI_SETTLER_SEA, area()) > 0)
				{
					AI_setUnitAIType(UNITAI_SETTLER_SEA);
					return;
				}

				scrap();
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	if (AI_group(GroupingParams().withUnitAI(UNITAI_CARRIER_SEA).maxOwnUnitAI(0).ignoreFaster()))
	{
		return;
	}

	if (AI_group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).maxOwnUnitAI(0).ignoreFaster().maxPathTurns(3)))
	{
		return;
	}

	if (AI_heal(50, 3))
	{
		return;
	}

	if (AI_pillageRange(2))
	{
		return;
	}

	if (AI_group(GroupingParams().withUnitAI(UNITAI_MISSILE_CARRIER_SEA).maxGroupSize(1).maxOwnUnitAI(1).minUnitAI(1)))
	{
		return;
	}

	if (AI_group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).maxGroupSize(1).maxOwnUnitAI(0).ignoreFaster()))
	{
		return;
	}

	if (AI_group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).maxOwnUnitAI(2).ignoreFaster()))
	{
		return;
	}

	if (AI_group(GroupingParams().withUnitAI(UNITAI_CARRIER_SEA).maxOwnUnitAI(2).ignoreFaster()))
	{
		return;
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  01/01/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					  */
	/************************************************************************************************/
	/* original bts code
		if (AI_group(UNITAI_ASSAULT_SEA, -1, 4, -1, true))
		{
			return;
		}
	*/
	// Group only with large flotillas first
	if (AI_group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).maxOwnUnitAI(4).minUnitAI(3).ignoreFaster()))
	{
		return;
	}

	if (AI_shadow(UNITAI_SETTLER_SEA, 2, -1, false, true, 4))
	{
		return;
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (AI_heal())
	{
		return;
	}

	if (AI_travelToUpgradeCity())
	{
		return;
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  04/18/10								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
		// If nothing else useful to do, escort nearby large flotillas even if they're faster
		// Gives Caravel escorts something to do during the Galleon/pre-Frigate era
	if (AI_group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).maxOwnUnitAI(4).minUnitAI(3).maxPathTurns(4).withCargoOnly()))
	{
		return;
	}

	if (AI_group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).maxOwnUnitAI(2).maxPathTurns(1).withCargoOnly()))
	{
		return;
	}

	// Pull back to primary area if it's not too far so primary area cities know you exist
	// and don't build more, unnecessary escorts
	if (AI_retreatToCity(true, false, 6))
	{
		return;
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_exploreSeaMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						10/21/08	Solver & jdog5000	*/
	/* 																			*/
	/* 	Naval AI																*/
	/********************************************************************************/
	if (plot()->isCity(true)) //prioritize getting outta there
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

		if (getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0)
		{
			if (!isHuman())
			{
				if (AI_anyAttack(1, 60))
				{
					return;
				}
			}

			if (AI_retreatToCity())
			{
				return;
			}

			if (AI_safety())
			{
				return;
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	CvArea* pWaterArea = plot()->waterArea();

	if (!isHuman())
	{
		if (AI_anyAttack(1, 75))
		{
			return;
		}
		if (!isNPC()) //XXX move some of this into a function? maybe useful elsewhere
		{
			//Obsolete?
			int iValue = GET_PLAYER(getOwner()).AI_unitValue(getUnitType(), AI_getUnitAIType(), area());
			int iBestValue = GET_PLAYER(getOwner()).AI_bestAreaUnitAIValue(AI_getUnitAIType(), area());

			if (iValue < iBestValue)
			{
				//Transform
				if (GET_PLAYER(getOwner()).AI_unitValue(getUnitType(), UNITAI_WORKER_SEA, area()) > 0)
				{
					AI_setUnitAIType(UNITAI_WORKER_SEA);
					return;
				}

				if (GET_PLAYER(getOwner()).AI_unitValue(getUnitType(), UNITAI_PIRATE_SEA, area()) > 0)
				{
					AI_setUnitAIType(UNITAI_PIRATE_SEA);
					return;
				}

				if (GET_PLAYER(getOwner()).AI_unitValue(getUnitType(), UNITAI_MISSIONARY_SEA, area()) > 0)
				{
					AI_setUnitAIType(UNITAI_MISSIONARY_SEA);
					return;
				}

				if (GET_PLAYER(getOwner()).AI_unitValue(getUnitType(), UNITAI_RESERVE_SEA, area()) > 0)
				{
					AI_setUnitAIType(UNITAI_RESERVE_SEA);
					return;
				}
				scrap();
			}
		}
	}

	if (AI_heal())
	{
		OutputDebugString("Chosen to heal\n");
		return;
	}

	if (!isHuman())
	{
		if (AI_pillageRange(1))
		{
			return;
		}
	}

	if (AI_exploreRange(4))
	{
		return;
	}

	if (!isHuman())
	{
		if (AI_pillageRange(4))
		{
			return;
		}
	}

	if (AI_explore())
	{
		return;
	}

	if (!isHuman())
	{
		if (AI_pillage())
		{
			return;
		}
	}

	if (!isHuman())
	{
		if (AI_travelToUpgradeCity())
		{
			return;
		}
	}

	if (!isHuman() && (AI_getUnitAIType() == UNITAI_EXPLORE_SEA))
	{
		pWaterArea = plot()->waterArea();

		if (pWaterArea != NULL
		&& GET_PLAYER(getOwner()).AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_EXPLORE_SEA) > GET_PLAYER(getOwner()).AI_neededExplorers(pWaterArea)
		&& GET_PLAYER(getOwner()).getFinalUnitUpkeep() > 0)
		{
			scrap();
			return;
		}
	}

	if (AI_patrol())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_assaultSeaMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}
	FAssert(AI_getUnitAIType() == UNITAI_ASSAULT_SEA);

	bool bEmpty = !getGroup()->hasCargo();
	bool bFull = (getGroup()->AI_isFull() && (getGroup()->getCargo() > 0));
	//I THINK a count of cargo is sufficient as it's only a verifier statement
	//MIGHT need to make this a little less demanding of being 'full' but the splitting of units to pack onto the boat will probably suffice.

	// Afforess 09/01/10
	// After a Barb Transport is done, set it to attack AI
	if (bEmpty && getOwner() == BARBARIAN_PLAYER)
	{
		AI_setUnitAIType(UNITAI_ATTACK_SEA);
		AI_barbAttackSeaMove();
		return;
	}

	if (plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

		if (getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		// An unguarded port/fort is not itself an enemy. Empty transports
		// must not abandon a pickup merely because both strengths are zero.
		if (iEnemyOffense > 0 && (iEnemyOffense > iOurDefense / 8 || iOurDefense == 0))
		{
			if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
			{
				if (!bEmpty)
				{
					getGroup()->unloadAll();
				}

				if (AI_anyAttack(1, 65))
				{
					return;
				}

				// Retreat to primary area first
				if (AI_retreatToCity(true))
				{
					return;
				}

				if (AI_retreatToCity())
				{
					return;
				}

				if (AI_safety())
				{
					return;
				}
			}

			if (!bFull && !bEmpty)
			{
				getGroup()->unloadAll();
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}
		}
	}

	if (bEmpty)
	{
		if (AI_anyAttack(1, 65))
		{
			return;
		}
		if (AI_anyAttack(1, 45))
		{
			return;
		}
	}

	bool bReinforce = false;
	bool bAttack = false;
	const bool bNoWarPlans = !GET_TEAM(getTeam()).hasWarPlan(true);
	bool bAttackBarbarian = false;
	bool bIsBarbarian = isNPC();

	// Count forts as cities
	bool bIsCity = plot()->isCity(true);

	// Cargo if already at war
	int iTargetReinforcementSize = (bIsBarbarian ? AI_stackOfDoomExtra() : 2);

	// Cargo to launch a new invasion
	int iTargetInvasionSize = 2 * iTargetReinforcementSize;

	int iCargoCount = getGroup()->getCargo();
	int iCargoVolume = getGroup()->getCargo(true);

	int iEscorts = getGroup()->countNumUnitAIType(UNITAI_ESCORT_SEA) + getGroup()->countNumUnitAIType(UNITAI_ATTACK_SEA);

	AreaAITypes eAreaAIType = area()->getAreaAIType(getTeam());
	bool bLandWar = !bIsBarbarian && ((eAreaAIType == AREAAI_OFFENSIVE) || (eAreaAIType == AREAAI_DEFENSIVE) || (eAreaAIType == AREAAI_MASSING));

	// Plot danger case handled above

	if (hasCargo() && (getUnitAICargo(UNITAI_SETTLE) > 0 || getUnitAICargo(UNITAI_WORKER) > 0))
	{
		// Dump inappropriate load at first oppurtunity after pick up
		if (bIsCity && (plot()->getOwner() == getOwner()))
		{
			getGroup()->unloadAll();
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
		else
		{
			if (!isFull())
			{
				if (AI_pickupStranded(NO_UNITAI, 1))
				{
					return;
				}
			}

			if (AI_retreatToCity(true))
			{
				return;
			}

			if (AI_retreatToCity())
			{
				return;
			}
		}
	}

	if (bIsCity)
	{
		CvCity* pCity = plot()->getPlotCity();

		if (pCity != NULL && (plot()->getOwner() == getOwner()))
		{
			if (!isHuman() && !isNPC())
			{
				if (AI_establishStackSeeInvisibleCoverage())
				{
					return;
				}
			}
			// split out galleys from stack of ocean capable ships
			if (GET_PLAYER(getOwner()).AI_unitImpassableCount(getUnitType()) == 0 && getGroup()->getNumUnits() > 1)
			{
				getGroup()->AI_separateImpassable();
			}

			// galleys with upgrade available should get that ASAP
			if (GET_PLAYER(getOwner()).AI_unitImpassableCount(getUnitType()) > 0)
			{
				CvCity* pUpgradeCity = getUpgradeCity(false);
				if (pUpgradeCity != NULL && pUpgradeCity == pCity)
				{
					// Wait for upgrade, this unit is top upgrade priority
					getGroup()->pushMission(MISSION_SKIP);
					return;
				}
			}
		}

		if (iCargoCount > 0)
		{
			if (pCity != NULL)
			{
				if ((GC.getGame().getGameTurn() - pCity->getGameTurnAcquired()) <= 1)
				{
					if (pCity->getPreviousOwner() != NO_PLAYER)
					{
						// Just captured city, probably from naval invasion.  If area targets, drop cargo and leave so as to not to be lost in quick counter attack
						if (GET_TEAM(getTeam()).countEnemyPowerByArea(plot()->area()) > 0)
						{
							getGroup()->unloadAll();

							if (iEscorts > 2)
							{
								if (getGroup()->countNumUnitAIType(UNITAI_ESCORT_SEA) > 1 && getGroup()->countNumUnitAIType(UNITAI_ATTACK_SEA) > 0)
								{
									getGroup()->AI_separateAI(UNITAI_ATTACK_SEA);
									getGroup()->AI_separateAI(UNITAI_RESERVE_SEA);

									iEscorts = getGroup()->countNumUnitAIType(UNITAI_ESCORT_SEA);
								}
							}
							iCargoCount = getGroup()->getCargo();
							iCargoVolume = getGroup()->getCargo(true);
							//TBHERE (count or volume?)
						}
					}
				}
			}
		}

		if ((iCargoCount > 0) && (iEscorts == 0))
		{
			if (AI_group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).ignoreFaster().maxPathTurns(1).withCargoOnly().ignoreMissionAIType(MISSIONAI_ASSAULT)))
			{
				return;
			}

			if (plot()->plotCount(PUF_isUnitAIType, UNITAI_ESCORT_SEA, -1, NULL, getOwner(), NO_TEAM, PUF_isGroupHead, -1, -1) > 0)
			{
				// Loaded but with no escort, wait for escorts in plot to join us
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}

			MissionAITypes eMissionAIType = MISSIONAI_GROUP;
			if ((GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, &eMissionAIType, 1, getGroup(), 3) > 0) || (GET_PLAYER(getOwner()).AI_getWaterDanger(plot(), 4) > 0))
			{
				// Loaded but with no escort, wait for others joining us soon or avoid dangerous waters
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}
		}

		if (bLandWar)
		{
			if (iCargoCount > 0)
			{
				if ((eAreaAIType == AREAAI_DEFENSIVE) || (pCity != NULL && pCity->AI_isDanger()))
				{
					// Unload cargo when on defense or if small load of troops and can reach enemy city over land (generally less risky)
					getGroup()->unloadAll();
					getGroup()->pushMission(MISSION_SKIP);
					return;
				}
			}

			if ((iCargoCount >= iTargetReinforcementSize))
			{
				getGroup()->AI_separateEmptyTransports();

				if (!(getGroup()->hasCargo()))
				{
					// this unit was empty group leader
					getGroup()->pushMission(MISSION_SKIP);
					return;
				}

				// Send ready transports
				if (AI_assaultSeaReinforce(false))
				{
					return;
				}

				if (iCargoCount >= iTargetInvasionSize)
				{
					if (AI_assaultSeaTransport(false))
					{
						return;
					}
				}
			}
		}
		else
		{
			if (eAreaAIType == AREAAI_ASSAULT && iCargoCount >= iTargetInvasionSize)
			{
				bAttack = true;
			}

			if (bFull
			&& (eAreaAIType == AREAAI_ASSAULT || eAreaAIType == AREAAI_ASSAULT_ASSIST)
			&& (iCargoVolume > cargoSpace() || iCargoCount >= iTargetReinforcementSize))
			{
				bReinforce = true;
			}
		}

		if (!bAttack && !bReinforce && (plot()->getTeam() == getTeam()))
		{
			if (iEscorts > 3 && iEscorts > (2 * getGroup()->countNumUnitAIType(UNITAI_ASSAULT_SEA)))
			{
				// If we have too many escorts, try freeing some for others
				getGroup()->AI_separateAI(UNITAI_ATTACK_SEA);
				getGroup()->AI_separateAI(UNITAI_RESERVE_SEA);

				iEscorts = getGroup()->countNumUnitAIType(UNITAI_ESCORT_SEA);
				if (iEscorts > 3 && iEscorts > (2 * getGroup()->countNumUnitAIType(UNITAI_ASSAULT_SEA)))
				{
					getGroup()->AI_separateAI(UNITAI_ESCORT_SEA);
				}
			}
		}

		MissionAITypes eMissionAIType = MISSIONAI_GROUP;
		if (GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, &eMissionAIType, 1, getGroup(), 1) > 0)
		{
			// Wait for units which are joining our group this turn
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}

		if (!bFull)
		{
			if (bAttack)
			{
				eMissionAIType = MISSIONAI_LOAD_ASSAULT;
				if (GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, &eMissionAIType, 1, getGroup(), 1) > 0)
				{
					// Wait for cargo which will load this turn
					getGroup()->pushMission(MISSION_SKIP);
					return;
				}
			}
			else if (GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, MISSIONAI_LOAD_ASSAULT) > 0)
			{
				// Wait for cargo which is on the way
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}
		}

		if (!bAttack && !bReinforce)
		{
			if (iCargoCount > 0)
			{
				if (AI_group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).ignoreFaster().maxPathTurns(5).withCargoOnly().ignoreMissionAIType(MISSIONAI_ASSAULT)))
				{
					return;
				}
			}
			else if (plot()->getTeam() == getTeam() && getGroup()->getNumUnits() > 1)
			{
				CvCity* pCity = plot()->getPlotCity();
				if (pCity != NULL && (GC.getGame().getGameTurn() - pCity->getGameTurnAcquired()) > 10)
				{
					if (pCity->plot()->plotCount(PUF_isAvailableUnitAITypeGroupie, UNITAI_ATTACK_CITY, -1, NULL, getOwner()) < iTargetReinforcementSize)
					{
						// Not attacking, no cargo so release any escorts, attack ships, etc and split transports
						getGroup()->AI_makeForceSeparate();
					}
				}
			}
		}
	}

	if (!bIsCity)
	{
		if (iCargoCount >= iTargetInvasionSize)
		{
			bAttack = true;
		}

		if (iCargoCount >= iTargetReinforcementSize
		|| bFull && iCargoVolume > cargoSpace())
		{
			bReinforce = true;
		}

		foreach_(CvPlot * pAdjacentPlot, plot()->adjacent())
		{
			if (iCargoCount > 0)
			{
				const CvCity* pAdjacentCity = pAdjacentPlot->getPlotCity();
				if (pAdjacentCity != NULL && pAdjacentCity->getOwner() == getOwner() && pAdjacentCity->getPreviousOwner() != NO_PLAYER)
				{
					if ((GC.getGame().getGameTurn() - pAdjacentCity->getGameTurnAcquired()) < 5)
					{
						// If just captured city and we have some cargo, dump units in city
						if (getGroup()->pushMissionInternal(MISSION_MOVE_TO, pAdjacentPlot->getX(), pAdjacentPlot->getY(), 0, false, false, MISSIONAI_ASSAULT, pAdjacentPlot))
						{
							return;
						}
					}
				}
			}
			else if (pAdjacentPlot->isOwned() && isEnemy(pAdjacentPlot->getTeam()))
			{
				if (pAdjacentPlot->getNumDefenders(getOwner()) > 2)
				{
					// if we just made a dropoff in enemy territory, release sea bombard units to support invaders
					if ((getGroup()->countNumUnitAIType(UNITAI_ATTACK_SEA) + getGroup()->countNumUnitAIType(UNITAI_RESERVE_SEA)) > 0)
					{
						bool bMissionPushed = false;

						//Covers city bombard and ranged assault on adjacent cities
						if (AI_seaBombardRange(1))
						{
							bMissionPushed = true;
						}

						CvSelectionGroup* pOldGroup = getGroup();

						//Release any Warships to finish the job.
						getGroup()->AI_separateAI(UNITAI_ATTACK_SEA);
						getGroup()->AI_separateAI(UNITAI_RESERVE_SEA);

						// Fixed bug in next line with checking unit type instead of unit AI
						if (pOldGroup == getGroup() && AI_getUnitAIType() == UNITAI_ASSAULT_SEA)
						{
							// Need to be sure all units can move
							if (getGroup()->canAllMove())
							{
								if (AI_retreatToCity(true))
								{
									bMissionPushed = true;
								}
							}
						}
						if (bMissionPushed)
						{
							return;
						}
					}
				}
			}
		}

		if (iCargoCount > 0)
		{
			MissionAITypes eMissionAIType = MISSIONAI_GROUP;
			if (GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, &eMissionAIType, 1, getGroup(), 1) > 0)
			{
				if (iEscorts < GET_PLAYER(getOwner()).AI_getWaterDanger(plot(), 2))
				{
					// Wait for units which are joining our group this turn (hopefully escorts)
					getGroup()->pushMission(MISSION_SKIP);
					return;
				}
			}
		}
	}
	//If all else fails
	if (AI_RbombardNaval())
	{
		return;
	}
	// Dale - RB: Field Bombard END

	if (bIsBarbarian)
	{
		if (getGroup()->isFull() || iCargoCount > iTargetInvasionSize)
		{
			if (AI_assaultSeaTransport(false))
			{
				return;
			}
		}
		else
		{
			if (AI_pickup(UNITAI_ATTACK_CITY, true, 5))
			{
				return;
			}

			if (AI_pickup(UNITAI_ATTACK, true, 5))
			{
				return;
			}

			//TBRETURN
			if (AI_retreatToCity())
			{
				return;
			}

			if (!(getGroup()->getCargo()))
			{//boolean call asking if there is cargo
				AI_barbAttackSeaMove();
				return;
			}

			if (AI_safety())
			{
				return;
			}

			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}
	else
	{
		if (bAttack || bReinforce)
		{
			if (bIsCity)
			{
				getGroup()->AI_separateEmptyTransports();
			}

			if (!(getGroup()->hasCargo()))
			{
				// this unit was empty group leader
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}

			FAssert(getGroup()->hasCargo());

			//BBAI TODO: Check that group has escorts, otherwise usually wait

			if (bAttack)
			{
				if (bReinforce && (AI_getBirthmark() % 2 == 0))
				{
					if (AI_assaultSeaReinforce())
					{
						return;
					}
					bReinforce = false;
				}

				if (AI_assaultSeaTransport())
				{
					return;
				}
			}

			// If not enough troops for own invasion,
			if (bReinforce)
			{
				if (AI_assaultSeaReinforce())
				{
					return;
				}
			}
		}

		if (bNoWarPlans && (iCargoCount >= iTargetReinforcementSize))
		{
			bAttackBarbarian = true;

			getGroup()->AI_separateEmptyTransports();

			if (!(getGroup()->hasCargo()))
			{
				// this unit was empty group leader
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}

			FAssert(getGroup()->hasCargo());
			if (AI_assaultSeaReinforce(bAttackBarbarian))
			{
				return;
			}

			FAssert(getGroup()->hasCargo());
			if (AI_assaultSeaTransport(bAttackBarbarian))
			{
				return;
			}
		}
	}

	bFull = (getGroup()->AI_isFull() && (getGroup()->getCargo() > 0));
	if ((bFull || bReinforce) && !bAttack)
	{
		// Group with nearby transports with units on board
		if (AI_group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).ignoreFaster().maxPathTurns(2).withCargoOnly().ignoreMissionAIType(MISSIONAI_ASSAULT)))
		{
			return;
		}

		if (AI_group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).ignoreFaster().maxPathTurns(10).withCargoOnly().ignoreMissionAIType(MISSIONAI_ASSAULT)))
		{
			return;
		}
	}
	else if (!bFull)
	{
		bool bHasOneLoad = false;
		if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
		{
			bHasOneLoad = (getGroup()->getCargo(true) >= cargoSpace());
		}
		else
		{
			bHasOneLoad = (getGroup()->getCargo() >= cargoSpace());
		}

		bool bHasCargo = getGroup()->hasCargo();
		if (AI_pickup(UNITAI_ATTACK_CITY, !bHasCargo, (bHasOneLoad ? 3 : 7)))
		{
			return;
		}

		if (AI_pickup(UNITAI_ATTACK, !bHasCargo, (bHasOneLoad ? 3 : 7)))
		{
			return;
		}

		if (AI_pickup(UNITAI_COUNTER, !bHasCargo, (bHasOneLoad ? 3 : 7)))
		{
			return;
		}

		if (AI_pickup(UNITAI_ATTACK_CITY, !bHasCargo))
		{
			return;
		}

		if (!bHasCargo)
		{
			if (AI_pickupStranded(UNITAI_ATTACK_CITY))
			{
				return;
			}

			if (AI_pickupStranded(UNITAI_ATTACK))
			{
				return;
			}

			if (AI_pickupStranded(UNITAI_COUNTER))
			{
				return;
			}

			if ((getGroup()->countNumUnitAIType(AI_getUnitAIType()) == 1))
			{
				// Try picking up any thing

				if (AI_pickupStranded())
				{
					return;
				}
			}
		}
	}

	if (bIsCity && bLandWar && getGroup()->hasCargo())
	{
		// Enemy units in this player's territory
		if (GET_PLAYER(getOwner()).AI_countNumAreaHostileUnits(area(), true, false, false, false, plot(), 15) > (getGroup()->getCargo() / 2))
		{//A count is sufficient.  Just asking if it should dropoff when threatened.
			getGroup()->unloadAll();
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	if (AI_retreatToCity(true))
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_settlerSeaMove()
{
	PROFILE_FUNC();

	bool bEmpty = !getGroup()->hasCargo();

	if (AI_selectStatus(true))
	{
		return;
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						10/21/08	Solver & jdog5000	*/
	/* 																			*/
	/* 	Naval AI																*/
	/********************************************************************************/
	if (plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

		if (getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			if (bEmpty)
			{
				if (AI_anyAttack(1, 65))
				{
					return;
				}
			}

			// Retreat to primary area first
			if (AI_retreatToCity(true))
			{
				return;
			}

			if (AI_retreatToCity())
			{
				return;
			}

			if (AI_safety())
			{
				return;
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	// Idle civilian transports answer posted rescue needs before speculative pickup.
	if (TransportRequests::enabled() && bEmpty && getGroup()->AI_getMissionAIType() == NO_MISSIONAI
		&& AI_pickupStranded()) return;

	if (bEmpty)
	{
		if (AI_anyAttack(1, 65))
		{
			return;
		}
		if (AI_anyAttack(1, 40))
		{
			return;
		}
	}

	int iSettlerCount = 0;
    foreach_(const CvUnit* passenger, plot()->units())
        if (passenger->getTransportUnit() == this && passenger->AI_getUnitAIType() == UNITAI_SETTLE
            && static_cast<const CvUnitAI*>(passenger)->AI_canPlanColony()) ++iSettlerCount;
	int iWorkerCount = getUnitAICargo(UNITAI_WORKER);

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  12/07/08								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
	if (hasCargo() && (iSettlerCount == 0) && (iWorkerCount == 0))
	{
		// Dump troop load at first oppurtunity after pick up
		if (plot()->isCity() && plot()->getOwner() == getOwner())
		{
			getGroup()->unloadAll();
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
		else
		{
			if (!(isFull()))
			{
				if (AI_pickupStranded(NO_UNITAI, 1))
				{
					return;
				}
			}

			if (AI_retreatToCity(true))
			{
				return;
			}

			if (AI_retreatToCity())
			{
				return;
			}
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  06/02/09								jdog5000	  */
	/*																							  */
	/* Settler AI																				   */
	/************************************************************************************************/
		// Don't send transport with settler and no defense
	if ((iSettlerCount > 0) && (iSettlerCount + iWorkerCount == cargoSpace()))
	{
		// No defenders for settler
		if (plot()->isCity() && plot()->getOwner() == getOwner())
		{
			getGroup()->unloadAll();
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	if ((iSettlerCount > 0) && (isFull() ||
		((getUnitAICargo(UNITAI_CITY_DEFENSE) > 0) &&
			(GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, MISSIONAI_LOAD_SETTLER) == 0))))
		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD					   END												  */
		/************************************************************************************************/
	{
		if (AI_settlerSeaTransport())
		{
			return;
		}
	}
	else if ((getTeam() != plot()->getTeam()) && bEmpty)
	{
		if (AI_pillageRange(3))
		{
			return;
		}
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  09/18/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
	if (plot()->isCity() && plot()->getOwner() == getOwner() && !hasCargo())
		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD					   END												  */
		/************************************************************************************************/
	{
		AreaAITypes eAreaAI = area()->getAreaAIType(getTeam());
		if ((eAreaAI == AREAAI_ASSAULT) || (eAreaAI == AREAAI_ASSAULT_MASSING))
		{
			CvArea* pWaterArea = plot()->waterArea();
			FAssert(pWaterArea != NULL);
			if (pWaterArea != NULL)
			{
				if (GET_PLAYER(getOwner()).AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_SETTLER_SEA) > 1)
				{
					if (GET_PLAYER(getOwner()).AI_unitValue(getUnitType(), UNITAI_ASSAULT_SEA, pWaterArea) > 0)
					{
						AI_setUnitAIType(UNITAI_ASSAULT_SEA);
						AI_assaultSeaMove();
						return;
					}
				}
			}
		}
	}

	if ((iWorkerCount > 0)
		&& GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, MISSIONAI_LOAD_SETTLER) == 0)
	{
		if (isFull() || (iSettlerCount == 0))
		{
			if (AI_settlerSeaFerry())
			{
				return;
			}
		}
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  09/18/09								jdog5000	  */
	/*																							  */
	/* Settler AI																				   */
	/************************************************************************************************/
	/* original bts code
		if (AI_pickup(UNITAI_SETTLE))
		{
			return;
		}
	*/
	if (!(getGroup()->hasCargo()))
	{
		if (AI_pickupStranded(UNITAI_SETTLE))
		{
			return;
		}
	}

	if (!getGroup()->isFull())
	{
		if (GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, MISSIONAI_LOAD_SETTLER) > 0)
		{
			// Wait for units on the way
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}

		if (iSettlerCount > 0)
		{
			if (AI_pickup(UNITAI_CITY_DEFENSE))
			{
				return;
			}
		}
		else if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
		{
			if (cargoSpace() - 2 * GET_PLAYER(getOwner()).getBestUnitTypeCargoVolume(UNITAI_WORKER) >= SMgetCargo() + iWorkerCount * GET_PLAYER(getOwner()).getBestUnitTypeCargoVolume(UNITAI_WORKER))
			{
				if (AI_pickup(UNITAI_SETTLE, true))
				{
					return;
				}
			}
		}
		else if (cargoSpace() - 2 >= getCargo() + iWorkerCount)
		{
			if (AI_pickup(UNITAI_SETTLE, true))
			{
				return;
			}
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if ((GC.getGame().getGameTurn() - getGameTurnCreated()) < 8)
	{
		if ((plot()->getPlotCity() == NULL) || GET_PLAYER(getOwner()).AI_totalAreaUnitAIs(plot()->area(), UNITAI_SETTLE) == 0)
		{
			if (AI_explore())
			{
				return;
			}
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  09/18/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
	/* original bts code
		if (AI_pickup(UNITAI_WORKER))
		{
			return;
		}
	*/
	if (!getGroup()->isFull())
	{
		// Rescue stranded non-settlers
		if (AI_pickupStranded())
		{
			return;
		}
	}

	//	Koshling - old condition here was broken for transports with a max capacity of 1 (canoes),
	//	and (after readiong the old code) I think more generally anyway.  Effect was it repeatedly went
	//	through here without ever actually wanting to load a settler (which it never does without an
	//	escort unit also) and wound up giving no orders at all to this unit, then looping over this 100 times
	//	a ta higher level before giving up and skipping moves for this unit.
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS)
	&& cargoSpace() > 1 && cargoSpace() - getCargo() < 2 && iWorkerCount > 0

	|| GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS)
	&& cargoSpace() > GET_PLAYER(getOwner()).getBestUnitTypeCargoVolume(UNITAI_SETTLE)
	&& cargoSpace() - SMgetCargo() < 2 * GET_PLAYER(getOwner()).getBestUnitTypeCargoVolume(UNITAI_WORKER)
	&& iWorkerCount > 0)
	{
		// If full of workers and not going anywhere, dump them if a settler is available
		if (iSettlerCount == 0 && plot()->plotCount(PUF_isAvailableUnitAITypeGroupie, UNITAI_SETTLE, -1, NULL, getOwner(), NO_TEAM, PUF_isFiniteRange) > 0)
		{
			getGroup()->unloadAll();

			if (!getGroup()->isFull())
			{
				AI_pickup(UNITAI_SETTLE, true);
			}
			return;
		}
	}

	if (!(getGroup()->isFull()))
	{
		if (AI_pickup(UNITAI_WORKER))
		{
			return;
		}
	}

	// Carracks cause problems for transport upgrades, galleys can't upgrade to them and they can't
	// upgrade to galleons.  Scrap galleys, switch unit AI for stuck Carracks.
	if (plot()->isCity() && plot()->getOwner() == getOwner())
	{
		UnitTypes eBestSettlerTransport = NO_UNIT;
		GET_PLAYER(getOwner()).AI_bestCityUnitAIValue(AI_getUnitAIType(), NULL, &eBestSettlerTransport);
		if (eBestSettlerTransport != NO_UNIT)
		{
			if (eBestSettlerTransport != getUnitType()
			&& GET_PLAYER(getOwner()).AI_unitImpassableCount(eBestSettlerTransport) == 0
			&& !upgradeAvailable(getUnitType(), eBestSettlerTransport))
			{
				// Keep a usable transport's passengers until a replacement role
				// is actually selected. Otherwise idle units reboard the same ship
				// and this fallback unloads them again without ever leaving port.
				if (GET_PLAYER(getOwner()).AI_unitImpassableCount(getUnitType()) > 0)
				{
					getGroup()->unloadAll();
					scrap();
					return;
				}
				else
				{
					CvArea* pWaterArea = plot()->waterArea();
					FAssert(pWaterArea != NULL);
					if (pWaterArea != NULL)
					{
						if (GET_PLAYER(getOwner()).AI_totalUnitAIs(UNITAI_EXPLORE_SEA) == 0)
						{
							if (GET_PLAYER(getOwner()).AI_unitValue(getUnitType(), UNITAI_EXPLORE_SEA, pWaterArea) > 0)
							{
								getGroup()->unloadAll();
								AI_setUnitAIType(UNITAI_EXPLORE_SEA);
								AI_exploreSeaMove();
								return;
							}
						}

						if (GET_PLAYER(getOwner()).AI_totalUnitAIs(UNITAI_SPY_SEA) == 0)
						{
							if (GET_PLAYER(getOwner()).AI_unitValue(getUnitType(), UNITAI_SPY_SEA, area()) > 0)
							{
								getGroup()->unloadAll();
								AI_setUnitAIType(UNITAI_SPY_SEA);
								AI_spySeaMove();
								return;
							}
						}

						if (GET_PLAYER(getOwner()).AI_totalUnitAIs(UNITAI_MISSIONARY_SEA) == 0)
						{
							if (GET_PLAYER(getOwner()).AI_unitValue(getUnitType(), UNITAI_MISSIONARY_SEA, area()) > 0)
							{
								getGroup()->unloadAll();
								AI_setUnitAIType(UNITAI_MISSIONARY_SEA);
								AI_missionarySeaMove();
								return;
							}
						}

						if (GET_PLAYER(getOwner()).AI_unitValue(getUnitType(), UNITAI_ATTACK_SEA, pWaterArea) > 0)
						{
							getGroup()->unloadAll();
							AI_setUnitAIType(UNITAI_ATTACK_SEA);
							AI_attackSeaMove();
							return;
						}
					}
				}
			}
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (AI_retreatToCity(true))
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_missionarySeaMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						10/21/08	Solver & jdog5000	*/
	/* 																			*/
	/* 	Naval AI																*/
	/********************************************************************************/
	if (plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

		if (getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			// Retreat to primary area first
			if (AI_retreatToCity(true))
			{
				return;
			}

			if (AI_retreatToCity())
			{
				return;
			}

			if (AI_safety())
			{
				return;
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	if (getUnitAICargo(UNITAI_MISSIONARY) > 0)
	{
		if (AI_specialSeaTransportMissionary())
		{
			return;
		}
	}
	else if (!(getGroup()->hasCargo()))
	{
		if (AI_pillageRange(4))
		{
			return;
		}
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  01/14/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
	if (!(getGroup()->isFull()))
	{
		if (GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, MISSIONAI_LOAD_SPECIAL) > 0)
		{
			// Wait for units on the way
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}

		if (AI_pickup(UNITAI_MISSIONARY, true))
		{
			return;
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (AI_explore())
	{
		return;
	}

	if (AI_retreatToCity(true))
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_spySeaMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}
	CvCity* pCity;

	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						10/21/08	Solver & jdog5000	*/
	/* 																			*/
	/* 	Naval AI																*/
	/********************************************************************************/
	if (plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

		if (getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			// Retreat to primary area first
			if (AI_retreatToCity(true))
			{
				return;
			}

			if (AI_retreatToCity())
			{
				return;
			}

			if (AI_safety())
			{
				return;
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	if (getUnitAICargo(UNITAI_SPY) > 0)
	{
		if (AI_specialSeaTransportSpy())
		{
			return;
		}

		pCity = plot()->getPlotCity();

		if (pCity != NULL)
		{
			if (pCity->getOwner() == getOwner())
			{
				getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_ATTACK_SPY, pCity->plot());
				return;
			}
		}
	}
	else if (!(getGroup()->hasCargo()))
	{
		if (AI_pillageRange(5))
		{
			return;
		}
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  01/14/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
	if (!(getGroup()->isFull()))
	{
		if (GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, MISSIONAI_LOAD_SPECIAL) > 0)
		{
			// Wait for units on the way
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}

		if (AI_pickup(UNITAI_SPY, true))
		{
			return;
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (AI_retreatToCity(true))
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_carrierSeaMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						10/21/08	Solver & jdog5000	*/
	/* 																			*/
	/* 	Naval AI																*/
	/********************************************************************************/
	if (plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

		if (getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			if (AI_retreatToCity(true))
			{
				return;
			}

			if (AI_retreatToCity())
			{
				return;
			}

			if (AI_safety())
			{
				return;
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	if (AI_heal(50))
	{
		return;
	}

	if (!isHuman() && !isNPC())
	{
		if (AI_establishStackSeeInvisibleCoverage())
		{
			return;
		}
	}

	if (!isEnemy(plot()->getTeam()))
	{
		if (GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, MISSIONAI_GROUP) > 0)
		{
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}
	else
	{
		if (AI_seaBombardRange(1))
		{
			return;
		}
	}

	if (AI_group(GroupingParams().withUnitAI(UNITAI_CARRIER_SEA).maxOwnUnitAI(1)))
	{
		return;
	}

	if (getGroup()->countNumUnitAIType(UNITAI_ATTACK_SEA) + getGroup()->countNumUnitAIType(UNITAI_ESCORT_SEA) == 0)
	{
		if (plot()->isCity() && plot()->getOwner() == getOwner())
		{
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
		if (AI_retreatToCity())
		{
			return;
		}
	}

	if (hasCargo())
	{
		if (AI_carrierSeaTransport())
		{
			return;
		}

		if (AI_blockade())
		{
			return;
		}

		if (AI_shadow(UNITAI_ASSAULT_SEA))
		{
			return;
		}
	}

	if (AI_travelToUpgradeCity())
	{
		return;
	}

	if (AI_retreatToCity(true))
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_missileCarrierSeaMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}
	bool bIsStealth = (getInvisibleType() != NO_INVISIBLE);

	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						06/14/09	Solver & jdog5000	*/
	/* 																			*/
	/* 	Naval AI																*/
	/********************************************************************************/
	if (plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

		if (getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			if (AI_shadow(UNITAI_ASSAULT_SEA, 1, 50, false, true, 1))
			{
				return;
			}

			if (AI_retreatToCity())
			{
				return;
			}

			if (AI_safety())
			{
				return;
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	if (plot()->isCity() && plot()->getTeam() == getTeam())
	{
		if (AI_heal())
		{
			return;
		}
	}

	if (((plot()->getTeam() != getTeam()) && getGroup()->hasCargo()) || getGroup()->AI_isFull())
	{
		if (bIsStealth)
		{
			if (AI_carrierSeaTransport())
			{
				return;
			}
		}
		else
		{
			/********************************************************************************/
			/* 	BETTER_BTS_AI_MOD						06/14/09		jdog5000		*/
			/* 																			*/
			/* 	Naval AI																*/
			/********************************************************************************/
			if (AI_shadow(UNITAI_ASSAULT_SEA, 1, 50, true, false, 12))
			{
				return;
			}
			/********************************************************************************/
			/* 	BETTER_BTS_AI_MOD						END								*/
			/********************************************************************************/

			if (AI_carrierSeaTransport())
			{
				return;
			}
		}
	}
	//	if (AI_pickup(UNITAI_ICBM))
	//	{
	//		return;
	//	}
	//
	//	if (AI_pickup(UNITAI_MISSILE_AIR))
	//	{
	//		return;
	//	}
	if (AI_retreatToCity())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
}


void CvUnitAI::AI_attackAirMove()
{
	AirInterceptionCandidates interceptors(*this);
	PROFILE_FUNC();

	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						10/21/08	Solver & jdog5000	*/
	/* 																			*/
	/* 	Air AI																	*/
	/********************************************************************************/
	CvCity* pCity = plot()->getPlotCity();
	bool bSkiesClear = true;

	// Check for sufficient defenders to stay
	int iDefenders = plot()->plotCount(PUF_canDefend, -1, -1, NULL, plot()->getOwner());

	int iAttackAirCount = plot()->plotCount(PUF_canAirAttack, -1, -1, NULL, NO_PLAYER, getTeam());
	iAttackAirCount += 2 * plot()->plotCount(PUF_isUnitAIType, UNITAI_ICBM, -1, NULL, NO_PLAYER, getTeam());

	if (plot()->isCoastalLand(GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize()))
	{
		iDefenders -= 1;
	}

	if (pCity != NULL)
	{
		if (pCity->getDefenseModifier(true) < 40)
		{
			iDefenders -= 1;
		}

		if (pCity->getOccupationTimer() > 1)
		{
			iDefenders -= 1;
		}
	}

	if (iAttackAirCount > iDefenders)
	{
		if (AI_airOffensiveCity())
		{
			return;
		}
	}

	// Check for direct threat to current base
	if (plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

		if (iEnemyOffense > iOurDefense || iOurDefense == 0)
		{
			// Too risky, pull back
			if (AI_airOffensiveCity())
			{
				return;
			}

			if (canAirDefend())
			{
				if (AI_airDefensiveCity())
				{
					return;
				}
			}
		}
		else if (iEnemyOffense > iOurDefense / 3)
		{
			if (getDamage() == 0)
			{
				if (!getGroup()->hasCollateralDamage() && canAirDefend())
				{
					if (pCity != NULL)
					{
						// Check for whether city needs this unit to air defend
						if (!(pCity->AI_isAirDefended(true, -1)))
						{
							getGroup()->pushMission(MISSION_AIRPATROL);
							return;
						}
					}
				}

				// Attack the invaders!
				if (AI_defendBaseAirStrike())
				{
					return;
				}

				if (AI_defensiveAirStrike())
				{
					return;
				}

				if (AI_airStrike())
				{
					return;
				}

				// If no targets, no sense staying in risky place
				if (AI_airOffensiveCity())
				{
					return;
				}

				if (canAirDefend())
				{
					if (AI_airDefensiveCity())
					{
						return;
					}
				}
			}

			if (healTurns(plot()) > 1)
			{
				// If very damaged, no sense staying in risky place
				if (AI_airOffensiveCity())
				{
					return;
				}

				if (canAirDefend())
				{
					if (AI_airDefensiveCity())
					{
						return;
					}
				}
			}

		}
	}

	if (getDamage() > 0)
	{
		if (100 * getHP() / getMaxHP() < 40)
		{
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
		const int iSearchRange = airRange();
		foreach_(const CvPlot * pLoopPlot, plot()->rect(iSearchRange, iSearchRange))
		{
			if (interceptors.bestInterceptor(pLoopPlot) != NULL)
			{
				bSkiesClear = false;
				break;
			}
		}

		if (!bSkiesClear)
		{
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	CvArea* pArea = area();
	int iAttackValue = kPlayer.AI_unitValue(getUnitType(), UNITAI_ATTACK_AIR, pArea);
	int iCarrierValue = kPlayer.AI_unitValue(getUnitType(), UNITAI_CARRIER_AIR, pArea);
	if (iCarrierValue > 0)
	{
		int iCarriers = kPlayer.AI_totalUnitAIs(UNITAI_CARRIER_SEA);
		if (iCarriers > 0)
		{
			UnitTypes eBestCarrierUnit = NO_UNIT;
			kPlayer.AI_bestAreaUnitAIValue(UNITAI_CARRIER_SEA, NULL, &eBestCarrierUnit);
			if (eBestCarrierUnit != NO_UNIT)
			{
				int iCarrierAirNeeded = iCarriers * GC.getUnitInfo(eBestCarrierUnit).getCargoSpace();
				if (kPlayer.AI_totalUnitAIs(UNITAI_CARRIER_AIR) < iCarrierAirNeeded)
				{
					AI_setUnitAIType(UNITAI_CARRIER_AIR);
					getGroup()->pushMission(MISSION_SKIP);
					return;
				}
			}
		}
	}

	int iDefenseValue = kPlayer.AI_unitValue(getUnitType(), UNITAI_DEFENSE_AIR, pArea);
	if (iDefenseValue > iAttackValue)
	{
		if (kPlayer.AI_bestAreaUnitAIValue(UNITAI_ATTACK_AIR, pArea) > iAttackValue)
		{
			AI_setUnitAIType(UNITAI_DEFENSE_AIR);
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						10/6/08			jdog5000		*/
	/* 																			*/
	/* 	Air AI																	*/
	/********************************************************************************/
		/* original BTS code
		if (AI_airBombDefenses())
		{
			return;
		}

		if (GC.getGame().getSorenRandNum(4, "AI Air Attack Move") == 0)
		{
			if (AI_airBombPlots())
			{
				return;
			}
		}

		if (AI_airStrike())
		{
			return;
		}

		if (canAirAttack())
		{
			if (AI_airOffensiveCity())
			{
				return;
			}
		}

		if (canRecon())
		{
			if (AI_exploreAir())
			{
				return;
			}
		}

		if (canAirDefend())
		{
			getGroup()->pushMission(MISSION_AIRPATROL);
			return;
		}
		*/
	bool bDefensive = false;
	if (pArea != NULL)
	{
		bDefensive = pArea->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE;
	}

	if (GC.getGame().getSorenRandNum(bDefensive ? 3 : 6, "AI Air Attack Move") == 0)
	{
		if (AI_defensiveAirStrike())
		{
			return;
		}
	}

	if (GC.getGame().getSorenRandNum(4, "AI Air Attack Move") == 0)
	{
		// only moves unit in a fort
		if (AI_travelToUpgradeCity())
		{
			return;
		}
	}

	// Support ground attacks
	if (AI_airBombDefenses())
	{
		return;
	}

	if (GC.getGame().getSorenRandNum(bDefensive ? 6 : 4, "AI Air Attack Move") == 0)
	{
		if (AI_airBombPlots())
		{
			return;
		}
	}

	if (AI_airStrike())
	{
		return;
	}

	if (canAirAttack())
	{
		if (AI_airOffensiveCity())
		{
			return;
		}
	}
	else
	{
		if (canAirDefend())
		{
			if (AI_airDefensiveCity())
			{
				return;
			}
		}
	}

	// BBAI TODO: Support friendly attacks on common enemies, if low risk?

	if (canAirDefend())
	{
		if (bDefensive || GC.getGame().getSorenRandNum(2, "AI Air Attack Move") == 0)
		{
			getGroup()->pushMission(MISSION_AIRPATROL);
			return;
		}
	}

	if (canRecon())
	{
		if (AI_exploreAir())
		{
			return;
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/


	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_defenseAirMove()
{
	PROFILE_FUNC();

	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						10/21/08	Solver & jdog5000	*/
	/* 																			*/
	/* 	Air AI																	*/
	/********************************************************************************/
	CvCity* pCity = plot()->getPlotCity();

	int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

	// includes forts
	if (plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);

		if (3 * iEnemyOffense > 4 * iOurDefense || iOurDefense == 0)
		{
			// Too risky, pull out
			// AI_airDefensiveCity will leave some air defense, pull extras out
			if (AI_airDefensiveCity())
			{
				return;
			}
		}
		else if (iEnemyOffense > iOurDefense / 3)
		{
			if (getDamage() > 0)
			{
				if (healTurns(plot()) > 1 + GC.getGame().getSorenRandNum(2, "AI Air Defense Move"))
				{
					// Can't help current situation, only risk losing unit
					if (AI_airDefensiveCity())
					{
						return;
					}
				}

				// Stay to defend in the future
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}

			if (canAirDefend() && pCity != NULL)
			{
				// Check for whether city needs this unit to air defend
				if (!(pCity->AI_isAirDefended(true, -1)))
				{
					getGroup()->pushMission(MISSION_AIRPATROL);
					return;
				}

				// Consider adding extra defenders
				if (!getGroup()->hasCollateralDamage() && (!pCity->AI_isAirDefended(false, -2)))
				{
					if (GC.getGame().getSorenRandNum(3, "AI Air Defense Move") == 0)
					{
						getGroup()->pushMission(MISSION_AIRPATROL);
						return;
					}
				}
			}

			// Attack the invaders!
			if (AI_defendBaseAirStrike())
			{
				return;
			}

			if (AI_defensiveAirStrike())
			{
				return;
			}

			if (AI_airStrike())
			{
				return;
			}

			if (AI_airDefensiveCity())
			{
				return;
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	if (getDamage() > 0)
	{
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (GC.getGame().getSorenRandNum(4, "AI Air Defense Move") == 0)
	{
		// only moves unit in a fort
		if (AI_travelToUpgradeCity())
		{
			return;
		}
	}

	if (canAirDefend())
	{
		// Check for whether city needs this unit for base air defenses
		int iBaseAirDefenders = 0;

		if (iEnemyOffense > 0)
		{
			iBaseAirDefenders++;
		}

		if (pCity != NULL)
		{
			iBaseAirDefenders += pCity->AI_neededAirDefenders() / 2;
		}

		if (plot()->countAirInterceptorsActive(getTeam()) < iBaseAirDefenders)
		{
			getGroup()->pushMission(MISSION_AIRPATROL);
			return;
		}
	}

	CvArea* pArea = area();
	bool bDefensive = false;
	bool bOffensive = false;

	if (pArea != NULL)
	{
		bDefensive = (pArea->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE);
		bOffensive = (pArea->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE);
	}

	if ((iEnemyOffense > 0) || bDefensive)
	{
		if (canAirDefend())
		{
			if (pCity != NULL)
			{
				// Consider adding extra defenders
				if (!(pCity->AI_isAirDefended(false, -1)))
				{
					if ((GC.getGame().getSorenRandNum((bOffensive ? 3 : 2), "AI Air Defense Move") == 0))
					{
						getGroup()->pushMission(MISSION_AIRPATROL);
						return;
					}
				}
			}
			else
			{
				if ((GC.getGame().getSorenRandNum((bOffensive ? 3 : 2), "AI Air Defense Move") == 0))
				{
					getGroup()->pushMission(MISSION_AIRPATROL);
					return;
				}
			}
		}

		if ((GC.getGame().getSorenRandNum(3, "AI Air Defense Move") > 0))
		{
			if (AI_defensiveAirStrike())
			{
				return;
			}

			if (AI_airStrike())
			{
				return;
			}
		}
	}
	else
	{
		if ((GC.getGame().getSorenRandNum(3, "AI Air Defense Move") > 0))
		{
			// Clear out any enemy fighters, support offensive units
			if (AI_airBombDefenses())
			{
				return;
			}

			if (GC.getGame().getSorenRandNum(3, "AI Air Defense Move") == 0)
			{
				// Hit enemy land stacks near our cities
				if (AI_defensiveAirStrike())
				{
					return;
				}
			}

			if (AI_airStrike())
			{
				return;
			}

			if (AI_getBirthmark() % 2 == 0 || bOffensive)
			{
				if (AI_airBombPlots())
				{
					return;
				}
			}
		}
	}

	if (AI_airDefensiveCity())
	{
		return;
	}

	// BBAI TODO: how valuable is recon information to AI in war time?
	if (canRecon())
	{
		if (GC.getGame().getSorenRandNum(bDefensive ? 6 : 3, "AI defensive air recon") == 0)
		{
			if (AI_exploreAir())
			{
				return;
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	if (canAirDefend())
	{
		getGroup()->pushMission(MISSION_AIRPATROL);
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_carrierAirMove()
{
	PROFILE_FUNC();

	// XXX maybe protect land troops?

	if (getDamage() > 0)
	{
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (isCargo())
	{
		int iRand = GC.getGame().getSorenRandNum(3, "AI Air Carrier Move");

		if (iRand == 2 && canAirDefend())
		{
			getGroup()->pushMission(MISSION_AIRPATROL);
			return;
		}
		else if (AI_airBombDefenses())
		{
			return;
		}
		else if (iRand == 1)
		{
			if (AI_airBombPlots())
			{
				return;
			}

			if (AI_airStrike())
			{
				return;
			}
		}
		else
		{
			if (AI_airStrike())
			{
				return;
			}

			if (AI_airBombPlots())
			{
				return;
			}
		}

		if (AI_travelToUpgradeCity())
		{
			return;
		}

		if (canAirDefend())
		{
			getGroup()->pushMission(MISSION_AIRPATROL);
			return;
		}
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (AI_airCarrier())
	{
		return;
	}

	if (AI_airDefensiveCity())
	{
		return;
	}

	if (canAirDefend())
	{
		getGroup()->pushMission(MISSION_AIRPATROL);
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_missileAirMove()
{
	PROFILE_FUNC();

	// includes forts
	if (!isCargo() && plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

		if (iEnemyOffense > (iOurDefense / 2) || iOurDefense == 0)
		{
			if (AI_airOffensiveCity())
			{
				return;
			}
		}
	}

	if (isCargo())
	{
		int iRand = GC.getGame().getSorenRandNum(3, "AI Air Missile plot bombing");
		if (iRand != 0)
		{
			if (AI_airBombPlots())
			{
				return;
			}
		}

		iRand = GC.getGame().getSorenRandNum(3, "AI Air Missile Carrier Move");
		if (iRand == 0)
		{
			if (AI_airBombDefenses())
			{
				return;
			}

			if (AI_airStrike())
			{
				return;
			}
		}
		else
		{
			if (AI_airStrike())
			{
				return;
			}

			if (AI_airBombDefenses())
			{
				return;
			}
		}

		if (AI_airBombPlots())
		{
			return;
		}

		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (AI_airStrike())
	{
		return;
	}

	if (AI_missileLoad(UNITAI_MISSILE_CARRIER_SEA))
	{
		return;
	}

	if (AI_missileLoad(UNITAI_RESERVE_SEA, 1))
	{
		return;
	}

	if (AI_missileLoad(UNITAI_ATTACK_SEA, 1))
	{
		return;
	}

	if (AI_airBombDefenses())
	{
		return;
	}

	if (!isCargo())
	{
		if (AI_airOffensiveCity())
		{
			return;
		}
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_healerMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	CvPlot* pPlot = plot();

	if (pPlot != NULL && exposedToDanger(pPlot, 80))
	{
		OutputDebugString("	...plot is dangerous - seeking safety\n");

		if (AI_safety())
		{
			return;
		}
	}

	AI_upgrade();
	if (isDelayedDeath())
	{
		//	Upgrade of original unit was successful
		return;
	}

	if (pPlot != NULL)
	{
		if (AI_fulfillImmediateHealerNeed(pPlot))
		{
			return;
		}

		if (processContracts())
		{
			return;
		}

		if (AI_fulfillCityHealerNeed(pPlot))
		{
			return;
		}

		if (AI_fulfillHealerNeed(pPlot))
		{
			return;
		}
	}

	if (AI_moveIntoNearestOwnedCity())
	{
		return;
	}

	if (pPlot != NULL)
	{
		if (getGroup()->pushMissionInternal(MISSION_SKIP, pPlot->getX(), pPlot->getY(), 0, false, false, MISSIONAI_HEAL_SUPPORT, pPlot))
			return;
	}
	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_healerSeaMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	CvPlot* pPlot = plot();
	if (pPlot != NULL && pPlot->isCity(true))
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

		if (getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			if (AI_retreatToCity(true))
			{
				return;
			}

			if (AI_retreatToCity())
			{
				return;
			}

			if (AI_safety())
			{
				return;
			}
		}
	}

	if (AI_heal(50))
	{
		return;
	}

	AI_upgrade();
	if (isDelayedDeath())
	{
		//	Upgrade of original unit was successful
		return;
	}

	if (AI_travelToUpgradeCity())
	{
		return;
	}

	if (pPlot != NULL)
	{
		if (AI_fulfillImmediateHealerNeed(pPlot))
		{
			return;
		}

		if (processContracts())
		{
			return;
		}

		if (AI_fulfillCityHealerNeed(pPlot))
		{
			return;
		}

		if (AI_fulfillHealerNeed(pPlot))
		{
			return;
		}
	}

	if (AI_moveIntoNearestOwnedCity())
	{
		return;
	}

	if (pPlot != NULL)
	{
		if (getGroup()->pushMissionInternal(MISSION_SKIP, pPlot->getX(), pPlot->getY(), 0, false, false, MISSIONAI_HEAL_SUPPORT, pPlot))
		{
			return;
		}
	}
	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_propertyControlMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	if (exposedToDanger(plot(), 80))
	{
		OutputDebugString("	...plot is dangerous - seeking safety\n");

		if (plot()->getOwner() != getOwner())
		{
			if (AI_safety())
			{
				return;
			}
		}
		else
		{
			if (AI_moveIntoNearestOwnedCity())
			{
				return;
			}
		}
	}

	AI_upgrade();
	if (isDelayedDeath())
	{
		//	Upgrade of original unit was successful
		return;
	}

	if (AI_artistCultureVictoryMove())
	{
		return;
	}

	if (AI_heritage() || AI_construct())
	{
		return;
	}

	if (AI_arrest())
	{
		return;
	}

	if (AI_fulfillPropertyControlNeed())
	{
		return;
	}

	if (processContracts())
	{
		return;
	}

	if (AI_moveIntoNearestOwnedCity())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_propertyControlSeaMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	if (plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

		if (getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			if (AI_retreatToCity(true))
			{
				return;
			}

			if (AI_retreatToCity())
			{
				return;
			}

			if (AI_safety())
			{
				return;
			}
		}
	}

	if (AI_heal(50))
	{
		return;
	}

	AI_upgrade();
	if (isDelayedDeath())
	{
		//	Upgrade of original unit was successful
		return;
	}

	if (AI_travelToUpgradeCity())
	{
		return;
	}

	if (AI_fulfillPropertyControlNeed())
	{
		return;
	}

	if (AI_moveIntoNearestOwnedCity())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_BarbCriminalMove()
{
	//consider assassination
	if (AI_anyAttack(0, 40))
	{
		return;
	}
	// Find enemy city if not already in a city
	if (!plot()->isCity(false) && getGroup()->isHasPathToAreaEnemyCity(MOVE_IGNORE_DANGER | MOVE_THROUGH_ENEMY, true))
	{
		return;
	}
	//consider if they are wanted to move out of sight of player who spotted them
	//consider to return to nearest non-barb city if not already in one
	//then just skip (pick a build up) and do as much crime damage as they can.
	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_InvestigatorMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	if (AI_isNegativePropertyUnit())
	{
		AI_setUnitAIType(UNITAI_PILLAGE);
		return;
	}

	if (exposedToDanger(plot(), 80))
	{
		OutputDebugString("	...plot is dangerous - seeking safety\n");

		if (plot()->getOwner() != getOwner())
		{
			if (AI_safety())
			{
				return;
			}
		}
		else
		{
			if (AI_moveIntoNearestOwnedCity())
			{
				return;
			}
		}
	}

	if (plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

		if (getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			if (AI_retreatToCity(true))
			{
				return;
			}

			if (AI_retreatToCity())
			{
				return;
			}

			if (AI_safety())
			{
				return;
			}
		}
	}

	if (AI_heal(50))
	{
		return;
	}

	AI_upgrade();
	if (isDelayedDeath())
	{
		//	Upgrade of original unit was successful
		return;
	}

	//Arrest if possible HERE
	if (AI_arrest())
	{
		return;
	}

	// Keep the city's established investigator before considering remote work.
	// Danger, healing, upgrades on site and arrests have already been handled.
	if (investigatorNeededLocally(this))
	{
		getGroup()->pushMission(MISSION_SKIP, getX(), getY(), 0, false, false, MISSIONAI_INVESTIGATOR_MAINTAIN, plot());
		return;
	}

	if (AI_travelToUpgradeCity())
	{
		return;
	}

	if (processContracts())
	{
		return;
	}

	if (AI_InvestigatorFulfillment())
	{
		return;
	}

	if (AI_moveIntoNearestOwnedCity())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP, getX(), getY(), 0, false, false, MISSIONAI_INVESTIGATOR_MAINTAIN, plot());
	return;
}

void CvUnitAI::AI_InfiltratorMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	AI_upgrade();
	if (isDelayedDeath())
	{
		// Upgrade of original unit was successful
		return;
	}

	const bool workingLocally = infiltratorWorkingLocally(this);
	// A concealed infiltrator already applying harmful BuildUp has a useful post.
	if (!workingLocally && getGroup()->getNumUnits() == 1 && !isWanted() && processContracts())
	{
		return;
	}

	if (AI_anyAttack(0, 75))
	{
		return;
	}

	if (!workingLocally && !isWanted() && GC.getGame().isOption(GAMEOPTION_NO_NATIONAL_UNIT_LIMIT) && !isNPC()
	&& plot() != NULL && plot()->isCity(false) && plot()->getOwner() != getOwner()
	&& !GET_PLAYER(getOwner()).AI_isFinancialTrouble())
	{
		int iTargetGroupSize = std::max(1, AI_getBirthmark() / 10);
		while (iTargetGroupSize > 9)
		{
			iTargetGroupSize = std::max(1, iTargetGroupSize / 10);
		}//get a number from 1-9.

		int iGroupSize = getGroup()->getNumUnits();
		int iMaxInfiltrationForceSize = 4 * (1 + (int)GET_PLAYER(getOwner()).getCurrentEra());
		int iExistingInfiltrationForceSize = GET_PLAYER(getOwner()).AI_totalAreaUnitAIs(area(), UNITAI_INFILTRATOR);
		if (iGroupSize < iTargetGroupSize && iExistingInfiltrationForceSize <= iMaxInfiltrationForceSize)
		{
			GET_PLAYER(getOwner()).getContractBroker().advertiseWork
			(
				HIGH_PRIORITY_ESCORT_PRIORITY,
				NO_UNITCAPABILITIES,
				getX(), getY(),
				this, UNITAI_INFILTRATOR
			);
			if (gUnitLogLevel > 2)
			{
				logBBAI("	%S's Infiltrator (%d) at (%d,%d) [stack size %d] requests another Infiltrator at priority %d", GET_PLAYER(getOwner()).getCivilizationDescription(0), getID(), getX(), getY(), getGroup()->getNumUnits(), HIGH_PRIORITY_ESCORT_PRIORITY);
			}
		}
	}

	// Then one by one they would end up hitting this point, theoretically, and possibly even the head unit immediately
	if (isWanted() && getGroup()->getNumUnits() == 1)
	{
		if (GET_PLAYER(getOwner()).AI_isFinancialTrouble() && canTrade(plot()))
		{
			getGroup()->pushMission(MISSION_TRADE);
			return;
		}
		else if (canInfiltrate(plot()))
		{
			getGroup()->pushMission(MISSION_INFILTRATE);
			return;
		}
		else if (AI_moveIntoNearestOwnedCity())
		{
			return;
		}
	}

	// Am I in rival land?
	const TeamTypes ePlotTeam = plot()->getTeam();
	if (ePlotTeam != NO_TEAM && ePlotTeam < MAX_PC_PLAYERS && ePlotTeam != getTeam()
	&& GET_TEAM(getTeam()).AI_getAttitudeWeight(ePlotTeam) < 0)
	{
		if (plot()->isCity(false))
		{
			// Toffer - Added gamespeed scaling to sabotage and pillage here.
			if (!workingLocally && getGroup()->isInvisible(ePlotTeam)
			&& (
				GC.getGame().getSorenRandNum(
					5 +
					7 * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent() / 100,
					"Leave city to pillage"
				) == 0
				) && AI_pillageRange(1, 20))
			{
				return;
			}
			//if inside a city, check to see if there's VERY good cause to sabotage production or steal plans.
			//Apparently the AI never steals plans soooo... leave that one alone.
			const int iProduction = plot()->getPlotCity()->getProductionProgress();
			if (iProduction > 0 && plot()->getPlotCity()->getProductionTurnsLeft() < 3)
			{
				// Only enact when it is a long build that has been heavily invested into already
				const int iEstTurns = plot()->getPlotCity()->getProductionNeeded() / iProduction;
				const int iLongBuild = 1 + 6 * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent() / 100;

				if (iEstTurns >= iLongBuild)
				{
					joinGroup(NULL); // do it alone
					if (getGroup()->pushMissionInternal(MISSION_SABOTAGE))
					{
						return;
					}
				}
			}

			//Otherwise sit there and cause crime rot.  Simple for now.
			getGroup()->pushMission(MISSION_SKIP, getX(), getY(), 0, false, false, MISSIONAI_INFILTRATOR_MAINTAIN, plot());
			return;
		}
		else if (getGroup()->isInvisible(ePlotTeam)
		&& (plot()->getImprovementType() != NO_IMPROVEMENT && AI_pillage() || AI_pillageRange(2, 20)))
		{
			return;
		}
	}

	if (AI_moveIntoCity(0, true))
	{
		return;
	}
	//if all this has failed, go more complicated and work like a pillage AI.
	AI_pillageMove();

	LOG_UNIT_BLOCK(3, {
		const CvWString szStringUnitAi = GC.getUnitAIInfo(m_eUnitAIType).getType();
		logBBAI("	Unit %S (%d) of Type (%S) for player %d (%S) at (%d,%d) - no job for Infiltrator left, end of AI_Infiltrator",
				getUnitInfo().getDescription(),
				getID(),
				szStringUnitAi.GetCString(),
				getOwner(),
				GET_PLAYER(getOwner()).getCivilizationDescription(0),
				getX(),
				getY());
	});
	return;
}


void CvUnitAI::AI_SeeInvisibleMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	if (AI_isNegativePropertyUnit())
	{
		AI_setUnitAIType(UNITAI_PILLAGE);
		return;
	}

	if (exposedToDanger(plot(), 80))
	{
		OutputDebugString("	...plot is dangerous - seeking safety\n");

		if (plot()->getOwner() != getOwner())
		{
			if (AI_safety())
			{
				return;
			}
		}
		else if (AI_moveIntoNearestOwnedCity())
		{
			return;
		}
	}

	if (plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

		if (getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			if (AI_retreatToCity(true))
			{
				return;
			}

			if (AI_retreatToCity())
			{
				return;
			}

			if (AI_safety())
			{
				return;
			}
		}
	}

	if (AI_heal(50))
	{
		return;
	}

	AI_upgrade();
	if (isDelayedDeath())
	{
		//	Upgrade of original unit was successful
		return;
	}

	if (AI_travelToUpgradeCity())
	{
		return;
	}

	if (processContracts())
	{
		return;
	}

	if (!plot()->isCity(true, getTeam()))
	{
		if (AI_moveIntoNearestOwnedCity())
		{
			return;
		}
	}

	getGroup()->pushMission(MISSION_SKIP, getX(), getY(), 0, false, false, MISSIONAI_SEE_INVISIBLE_MAINTAIN, plot());
	return;

}

void CvUnitAI::AI_SeeInvisibleSeaMove()
{
	PROFILE_FUNC();

	if (AI_selectStatus(true))
	{
		return;
	}

	if (AI_isNegativePropertyUnit())
	{
		AI_setUnitAIType(UNITAI_PIRATE_SEA);
		return;
	}

	if (exposedToDanger(plot(), 80))
	{
		OutputDebugString("	...plot is dangerous - seeking safety\n");

		if (plot()->getOwner() != getOwner())
		{
			if (AI_safety())
			{
				return;
			}
		}
		else
		{
			if (AI_moveIntoNearestOwnedCity())
			{
				return;
			}
		}
	}

	if (AI_heal(50))
	{
		return;
	}

	AI_upgrade();
	if (isDelayedDeath())
	{
		//	Upgrade of original unit was successful
		return;
	}

	if (AI_travelToUpgradeCity())
	{
		return;
	}

	if (processContracts())
	{
		return;
	}

	if (!plot()->isCity(true, getTeam()))
	{
		if (AI_moveIntoNearestOwnedCity())
		{
			return;
		}
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;

}

void CvUnitAI::AI_EscortMove()
{
	PROFILE_FUNC();
	//TBTask: These need to be inspired to go join workers and settlers that need escorting - the request from the workers have been removed to correct long periods of getting stuck before they could move on and do more.

	if (AI_selectStatus(true))
	{
		return;
	}

	if (AI_heal(50))
	{
		return;
	}

	AI_upgrade();
	if (isDelayedDeath())
	{
		//	Upgrade of original unit was successful
		return;
	}

	if (AI_travelToUpgradeCity())
	{
		return;
	}

	if (processContracts())
	{
		return;
	}

	if (AI_moveIntoNearestOwnedCity())
	{
		return;
	}

	this->getGroup()->pushMission(MISSION_SKIP);
}
bool CvUnitAI::AI_retreatIfCantDefend() {
	return (!getGroup()->canDefend() && GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot()) && AI_retreatToCity());
}

void CvUnitAI::AI_networkAutomated()
{
	FAssertMsg(canBuildRoute(), "canBuildRoute is expected to be true");

	if (AI_retreatIfCantDefend())
	{
		return;
	}
    // 1. Hook already-improved resources and cities into the trade network first --
    //    these give immediate trade value (AI_connectBonus defaults bTestTrade=true,
    //    i.e. only resources that already carry a trade improvement).
	if (AI_connectBonus(true) || AI_connectCity())
	{
		return;
	}

    // 2. Improve nearby resources so they become connectable. Without this an
    //    unimproved resource is invisible to AI_connectBonus and never gets hooked up.
	if (!GET_PLAYER(getOwner()).isModderOption(MODDEROPTION_INFRASTRUCTURE_IGNORES_IMPROVEMENTS)
	&& CvWorkerService::ImproveBonus(this, 2))
	{
		return;
	}

	// 3. Connect resources that aren't improved yet (bTestTrade=false drops the
    //    "already improved" requirement): road the tile now so it joins the network
    //    the moment it gets an improvement, instead of waiting a full improve+connect
    //    cycle. This is what makes "connect all your bonuses" actually reach the
    //    not-yet-developed ones.
    if (AI_connectBonus(false))
    {
        return;
    }

    // 4. Flesh out connective road: first plots whose improvement gains a route yield,
    //    then city-to-city links, then any remaining upgradeable territory. The final
    //    general AI_routeTerritory() pass is what lays/upgrades connective road across
    //    plain terrain rather than only on already-improved tiles.
	if (AI_routeTerritory(true) || AI_routeCity())
	{
		return;
	}

	if (AI_routeTerritory())
    {
    	return;
    }

	if (AI_retreatToCity() || AI_safety())
	{
		return;
	}
	getGroup()->pushMission(MISSION_SKIP);
}


void CvUnitAI::AI_cityAutomated()
{
	if (!getGroup()->canDefend() && GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot())
	// XXX maybe not do this??? could be working productively somewhere else...
	&& AI_retreatToCity())
	{
		return;
	}
	CvCity* pCity = plot()->getOwner() == getOwner() ? plot()->getWorkingCity() : NULL;

	if (pCity == NULL)
	{
		pCity = GC.getMap().findCity(getX(), getY(), getOwner()); // XXX do team???
	}

	if (pCity != NULL && AI_improveCity(pCity))
	{
		return;
	}

	if (AI_retreatToCity() || AI_safety())
	{
		return;
	}
	getGroup()->pushMission(MISSION_SKIP);
}


// XXX make sure we include any new UnitAITypes...
int CvUnitAI::AI_promotionValue(PromotionTypes ePromotion) const
{
    StackPromotions::EvaluationScope aidEvaluation(isHuman() || !StackPromotions::enabled());
	// Do not voluntarily turn an offensive soldier into a defend-only unit.
	// Existing defensive promotions and support roles remain valid.
	if (!isHuman() && OffensiveStackBalance::offensive(AI_getUnitAIType())
		&& !isOnlyDefensive() && GC.getPromotionInfo(ePromotion).getDefenseOnlyChange() > 0)
	{
		return 0;
	}
	return GET_PLAYER(getOwner()).AI_promotionValue(ePromotion, getUnitType(), this, AI_getUnitAIType());
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_shadow(UnitAITypes eUnitAI, int iMax, int iMaxRatio, bool bWithCargoOnly, bool bOutsideCityOnly, int iMaxPath)
{
    NAVAL_DIAG_PHASE(SHADOW,eUnitAI,iMax,iMaxRatio,iMaxPath);

	PROFILE_FUNC();

	const DomainTypes domain = getDomainType();
	const int iBaseMoves = getGroup()->baseMoves();

	CvUnit* pBestUnit = NULL;
	int iBestValue = 0;
	foreach_(CvUnit * pLoopUnit, GET_PLAYER(getOwner()).getUnitsByAIRole(eUnitAI))
	{
		if (pLoopUnit != this && pLoopUnit->isGroupHead()
		&& pLoopUnit->getDomainType() == domain && !pLoopUnit->isCargo()
		&& pLoopUnit->AI_getUnitAIType() == eUnitAI
		&& AI_plotValid(pLoopUnit->plot())
		&& pLoopUnit->getGroup()->baseMoves() <= iBaseMoves
		&& (!bWithCargoOnly || pLoopUnit->getGroup()->hasCargo())
		&& (!bOutsideCityOnly || !pLoopUnit->plot()->isCity()))
		{
			const int iShadowerCount = GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(pLoopUnit, MISSIONAI_SHADOW, getGroup());
			if ((-1 == iMax || iShadowerCount < iMax)
			&& (-1 == iMaxRatio || iShadowerCount == 0 || 100 * iShadowerCount / std::max(1, pLoopUnit->getGroup()->countNumUnitAIType(eUnitAI)) <= iMaxRatio)
			&& !pLoopUnit->plot()->isVisibleEnemyUnit(this))
			{
				int iPathTurns;
				if (generatePath(pLoopUnit->plot(), 0, true, &iPathTurns, iMaxPath) && iPathTurns <= iMaxPath)
				{
					const int iValue = (1 + pLoopUnit->getGroup()->getCargo()) * 1000 / (1 + iPathTurns);

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestUnit = pLoopUnit;
					}
				}
			}
		}
	}
	if (pBestUnit != NULL)
	{
		if (atPlot(pBestUnit->plot()))
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_SHADOW, NULL, pBestUnit);
			NAVAL_DIAG_PHASE_RETURN((true));
		}
		NAVAL_DIAG_PHASE_RETURN((getGroup()->pushMissionInternal(MISSION_MOVE_TO_UNIT, pBestUnit->getOwner(), pBestUnit->getID(), 0, false, false, MISSIONAI_SHADOW, NULL, pBestUnit)));
	}
	NAVAL_DIAG_PHASE_RETURN((false));
}


namespace {
	int unitImpassableCount(const CvUnit* unit)
	{
		return GET_PLAYER(unit->getOwner()).AI_unitImpassableCount(unit->getUnitType());
	}
}

#include "ArmyRallyRules.h"

bool CvUnitAI::AI_rallyToStrongerGroup()
{
	PROFILE_FUNC();
	CvSelectionGroup* ours = getGroup();
	std::vector<ArmyRallyCandidate> candidates;
	int order = 0;
	foreach_(CvSelectionGroup* other, GET_PLAYER(getOwner()).groups_non_empty())
	{
		CvUnit* head = other->getHeadUnit();
		if (other == ours || head == NULL || head->isCargo()
			|| head->getDomainType() != getDomainType()
			|| !armyRallyRole(head->AI_getUnitAIType())
			|| !armyRallyMission(other->AI_getMissionAIType())
			|| other->getActivityType() == ACTIVITY_HEAL
			|| isEnemy(head->plot()->getTeam())
			|| getDomainType() == DOMAIN_LAND && !canMoveAllTerrain() && !UnitTravelConnectivity::landUnitCanReach(this,head->plot(),true)) continue;
		candidates.push_back(ArmyRallyCandidate(other,
			plotDistance(getX(), getY(), head->getX(), head->getY()), order++));
	}
	std::sort(candidates.begin(), candidates.end());
	// A read-only selection: compute our aggregate once, not our best attacker
	// against our own plot for every other group. No data survives an order.
	const UnitValueFlags flags = (UnitValueFlags)(UNITVALUE_FLAGS_DEFENSIVE | UNITVALUE_FLAGS_OFFENSIVE);
	int strength = -1;
	for (size_t i = 0; i < candidates.size(); ++i)
	{
		CvSelectionGroup* other = candidates[i].group;
		CvUnit* head = other->getHeadUnit();
		if (!AI_plotValid(head->plot()) || !canJoinGroup(head->plot(), other, true)) continue;
		if (strength < 0) strength = ours->AI_getGenericValueTimes100(flags);
		const int otherStrength = other->AI_getGenericValueTimes100(flags);
		if (!armyRallyStronger(strength, ours->getID(), otherStrength, other->getID())) continue;
		if (ours->pushMissionInternal(MISSION_MOVE_TO_UNIT, head->getOwner(), head->getID(),
			0, false, false, MISSIONAI_GROUP, NULL, head)) return true;
	}
	return false;
}

bool CvUnitAI::AI_group(const GroupingParams& params)
{
    NAVAL_DIAG_PHASE(GROUP,params.eUnitAI,params.iMaxPath,params.iMaxGroup,params.iMaxOwnUnitAI);

	PROFILE_FUNC();

	// if we are on a transport, then do not regroup
	if (isCargo()
		|| !(params.bAllowRegrouping || params.bMergeWholeGroup) && getGroup()->getNumUnits() > 1
		|| getDomainType() == DOMAIN_LAND && !canMoveAllTerrain() && area()->getNumAIUnits(getOwner(), params.eUnitAI) == 0
		|| !AI_canGroupWithAIType(params.eUnitAI)
		|| GET_PLAYER(getOwner()).AI_getNumAIUnits(params.eUnitAI) == 0
		)
	{
		NAVAL_DIAG_PHASE_RETURN((false));
	}

	const int iOurImpassableCount = algo::max_element(getGroup()->units() | transformed(unitImpassableCount)).get_value_or(0);

	const bool localStackFirst = StackPromotions::eligible(*this);
	int iBestValue = MAX_INT;
	CvUnit* pBestUnit = NULL;

	const bool bCanDefend = getGroup()->canDefend();
	const int groupExtra = params.bStackOfDoom ? AI_stackOfDoomExtra() : 0;
	const int ourGroupSizeWithoutUs = getGroup()->getNumUnits() - 1;

	{
		// Group eligibility can update contract advertising. It does not move
		// units, so danger answers remain valid until orders are issued below.
		PlotDangerCacheScope candidateDangerCache(getOwner());
	CvReachablePlotSet plotSet(getGroup(), bCanDefend ? 0 : MOVE_OUR_TERRITORY);

	// Loop over groups, AI_allowGroup blocks non-head units anyway
	foreach_(CvSelectionGroup * group, GET_PLAYER(getOwner()).groups_non_empty())
	{
		CvUnit* unit = group->getHeadUnit();
		if ((params.iMaxPath > 0 || unit->plot() == plot())
			&& !isEnemy(unit->plot()->getTeam())
			&& (!params.bBiggerGroupOnly || group->getNumUnits() > ourGroupSizeWithoutUs)
			// AI_allowGroup can remove us from the contract worklist. Preserve
			// the original reachability gate before that observable side effect.
			&& plotSet.containsWithinRange(unit->plot(), AI_searchRange(params.iMaxPath))
			&& AI_allowGroup(unit, params.eUnitAI, params.bMergeWholeGroup)
			&& (params.iMaxOwnUnitAI == -1 || group->countNumUnitAIType(AI_getUnitAIType()) <= params.iMaxOwnUnitAI + groupExtra)
			&& (params.iMinUnitAI == -1 || group->countNumUnitAIType(params.eUnitAI) >= params.iMinUnitAI)
			&& (!params.bIgnoreFaster || group->baseMoves() <= baseMoves())
			&& (!params.bIgnoreOwnUnitType || unit->getUnitType() != getUnitType())
			&& (!params.bWithCargoOnly || unit->getGroup()->hasCargo())
			&& (!params.bInCityOnly || unit->plot()->isCity())
			&& (params.eIgnoreMissionAIType == NO_MISSIONAI || params.eIgnoreMissionAIType != unit->getGroup()->AI_getMissionAIType())
			&& !unit->plot()->isVisibleEnemyUnit(this)
			&& (
				(iOurImpassableCount == 0 && AI_getUnitAIType() != UNITAI_ASSAULT_SEA)
				||
				iOurImpassableCount == algo::max_element(group->units() | transformed(unitImpassableCount)).get_value_or(0)
				)
			&& (
				params.iMaxGroup == -1
				||
				group->getNumUnits() + GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(unit, MISSIONAI_GROUP, getGroup()) <= params.iMaxGroup + groupExtra
				)
			)
		{
			const bool local = atPlot(unit->plot());
			if (localStackFirst && pBestUnit && atPlot(pBestUnit->plot()) && !local) continue;
			int iPathTurns = 0;
			if ((localStackFirst && local) || generatePath(unit->plot(), bCanDefend ? 0 : MOVE_OUR_TERRITORY, true, &iPathTurns, params.iMaxPath))
			{
				if (iPathTurns <= params.iMaxPath)
				{
					int iValue = 1000 * (iPathTurns + 1);
					iValue *= 4 + group->getCargo();
					// just a count
					iValue /= group->getNumUnits();

					if (localStackFirst)
					{
						const int stackPreference = StackPromotions::mergePreference(*this, *group, params.bMergeWholeGroup);
						if (stackPreference != 0) iValue = iValue * 100 / (100 + stackPreference);
					}
					if (StackPromotionMath::preferLocalGroup(localStackFirst, local, pBestUnit && atPlot(pBestUnit->plot()), iValue, iBestValue))
					{
						iBestValue = iValue;
						pBestUnit = unit;
					}
				}
			}
		}
	}

	}

	if (pBestUnit != NULL)
	{
		if (atPlot(pBestUnit->plot()))
		{
			if (params.bMergeWholeGroup)
			{
				getGroup()->mergeIntoGroup(pBestUnit->getGroup());
			}
			else
			{
				joinGroup(pBestUnit->getGroup());
			}
			NAVAL_DIAG_PHASE_RETURN((true));
		}
		NAVAL_DIAG_PHASE_RETURN((getGroup()->pushMissionInternal(MISSION_MOVE_TO_UNIT, pBestUnit->getOwner(), pBestUnit->getID(), bCanDefend ? 0 : (MOVE_RECONSIDER_ON_LEAVING_OWNED | MOVE_OUR_TERRITORY | MOVE_WITH_CAUTION), false, false, MISSIONAI_GROUP, NULL, pBestUnit)));
	}
	NAVAL_DIAG_PHASE_RETURN((false));
}


bool CvUnitAI::AI_groupMergeRange(UnitAITypes eUnitAI, int iMaxRange, bool bBiggerOnly, bool bAllowRegrouping, bool bIgnoreFaster)
{
	PROFILE_FUNC();

	// if we are on a transport, then do not regroup
	if (isCargo())
	{
		return false;
	}

	if (!bAllowRegrouping)
	{
		if (getGroup()->getNumUnits() > 1)
		{
			return false;
		}
	}

	if ((getDomainType() == DOMAIN_LAND) && !canMoveAllTerrain())
	{
		if (area()->getNumAIUnits(getOwner(), eUnitAI) == 0)
		{
			return false;
		}
	}

	if (!AI_canGroupWithAIType(eUnitAI))
	{
		return false;
	}

	const CvPlot* pPlot = plot();
	CvSelectionGroup* pGroup = getGroup();

	// best match
	const CvUnit* pBestUnit = NULL;
	const bool localStackFirst = StackPromotions::eligible(*this);
	int iBestValue = MAX_INT;
	int iBestPathTurns = MAX_INT;
	// iterate over plots at each range
	foreach_(const CvPlot * pLoopPlot, pPlot->rect(iMaxRange, iMaxRange))
	{
		if (pLoopPlot->getArea() == pPlot->getArea() && AI_plotValid(pLoopPlot))
		{
			foreach_(const CvUnit * pLoopUnit, pLoopPlot->units())
			{
				const CvSelectionGroup* pLoopGroup = pLoopUnit->getGroup();

				if (AI_allowGroup(pLoopUnit, eUnitAI, true))
				{
					if (bIgnoreFaster || (pLoopUnit->getGroup()->baseMoves() <= baseMoves()))
					{
						if (!bBiggerOnly || (pLoopGroup->getNumUnits() > pGroup->getNumUnits()))
						{
							const bool local = pLoopPlot == pPlot;
							if (localStackFirst && pBestUnit && atPlot(pBestUnit->plot()) && !local) continue;
							int iPathTurns = 0;
							if ((localStackFirst && local) || generatePath(pLoopPlot, 0, true, &iPathTurns, iMaxRange + 2))
							{
								if (iPathTurns <= (iMaxRange + 2))
								{
									int iValue = 1000 * (iPathTurns + 1);
									iValue /= pLoopGroup->getNumUnits();

									if (localStackFirst)
									{
										const int stackPreference = StackPromotions::mergePreference(*this, *pLoopGroup, true);
										if (stackPreference != 0) iValue = iValue * 100 / (100 + stackPreference);
									}
									if (StackPromotionMath::preferLocalGroup(localStackFirst, local, pBestUnit && atPlot(pBestUnit->plot()), iValue, iBestValue))
									{
										iBestValue = iValue;
										pBestUnit = pLoopUnit;
										iBestPathTurns = iPathTurns;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if (pBestUnit != NULL)
	{
		if (atPlot(pBestUnit->plot()))
		{
			pBestUnit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, NO_MISSIONAI);
			pGroup->pushMission(MISSION_SKIP, -1, -1, 0, false, false, NO_MISSIONAI);
			pGroup->mergeIntoGroup(pBestUnit->getGroup());
			return true;
		}
		int idist = plotDistance(plot()->getX(), plot()->getY(), pBestUnit->plot()->getX(), pBestUnit->plot()->getY());
		LOG_UNIT_BLOCK(3, {
			UnitAITypes eUnitAi = AI_getUnitAIType();
			MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
			CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
			CvWString MissionInfos = MissionAITypeToString(eMissionAI);
			CvWString StrUnitName = m_szName;
			if (StrUnitName.length() == 0)
			{
				StrUnitName = getName(0).GetCString();
			}

			logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], search to merge at Range with another group at (%d, %d), distance %d...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits(), pBestUnit->plot()->getX(), pBestUnit->plot()->getY(), idist);
		});

		bool bSuccess = pGroup->pushMissionInternal(MISSION_MOVE_TO_UNIT, pBestUnit->getOwner(), pBestUnit->getID(), 0, false, false, MISSIONAI_GROUP, NULL, pBestUnit);
		if (bSuccess && idist == 1) //near it
		{
			pBestUnit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
		}
		if (bSuccess)
		{
			MissionAITypes eMissionAI = pBestUnit->getGroup()->AI_getMissionAIType();
			if (eMissionAI == NO_MISSIONAI || iBestPathTurns <= 2)
			{
				pBestUnit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
			}
			return true;
		}
	}
	return false;
}

namespace {
	bool canPathTo(const CvPlot* plot, const CvUnit* unit, int maxPathTurns)
	{
		int pathTurns = 0;
		if (unit->generatePath(plot, 0, true, &pathTurns, maxPathTurns + 1))
		{
			if (unit->getPathMovementRemaining() == 0)
			{
				pathTurns++;
			}

			if (pathTurns <= maxPathTurns)
			{
				return true;
			}
		}
		return false;
	}

	bool canTransportReachPlot(const CvPlot* loadingPlot, const CvUnit* targetUnit, const CvUnit* transportUnit, int maxTransportPath)
	{
		if (loadingPlot->isCoastalLand()
			&& loadingPlot->isRevealed(targetUnit->getTeam(), false)
			&& loadingPlot->isOwned()
			&& targetUnit->isPotentialEnemy(loadingPlot->getTeam(), loadingPlot)
			&& !targetUnit->isNPC()
			&& loadingPlot->area()->getCitiesPerPlayer(loadingPlot->getOwner()) > 0
			)
		{
			// Test water beside the enemy landing target, not beside the passenger origin.
			// Transport cannot enter land plot without cargo, so generate path only works properly if
			// land units are already loaded
			return algo::any_of(
				loadingPlot->adjacent() | filtered(CvPlot::fn::isWater()),
				bind(canPathTo, _1, transportUnit, maxTransportPath)
			);
		}
		return false;
	}
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					  04/18/10								jdog5000	  */
/*																							  */
/* War tactics AI, Unit AI																	  */
/************************************************************************************************/
// Returns true if we loaded onto a transport or a mission was pushed...
// it appears to me that this is fully handled at this point.
// still need to do a clean sweep search for all references to AI_load but I THINK it's taken care of at this point.
// the main thing to look for would be unadapted non -1 references to iMinCargo, iMinCargoSpace and iMaxCargoSpace
bool CvUnitAI::AI_load(UnitAITypes eUnitAI, MissionAITypes eMissionAI, UnitAITypes eTransportedUnitAI, int iMinCargo, int iMinCargoSpace, int iMaxCargoSpace, int iMaxCargoOurUnitAI, int iFlags, int iMaxPath, int iMaxTransportPath)
{
	PROFILE_FUNC();


	if (hasCargo())
	{
		return false;
	}

	if (isCargo())
	{
		getGroup()->pushMission(MISSION_SKIP);
		return true;
	}

	if ((getDomainType() == DOMAIN_LAND) && !canMoveAllTerrain())
	{
		if (area()->getNumAIUnits(getOwner(), eUnitAI) == 0)
		{
			return false;
		}
	}

if (!isHuman() && AI_getUnitAIType() == UNITAI_WORKER && eUnitAI == UNITAI_SETTLER_SEA && !getGroup()->isStranded())
    {
        bool destination = false;
        foreach_(const CvCity* city, GET_PLAYER(getOwner()).cities())
            if (!UnitTravelConnectivity::landUnitCanReach(this,city->plot()) && AI_workerFerryJob(city)) { destination = true; break; }
        if (!destination)
            foreach_(const CvUnit* ship, GET_PLAYER(getOwner()).getUnitsByAIRole(UNITAI_SETTLER_SEA))
                if (ship->getUnitAICargo(UNITAI_SETTLE) > 0) { destination = true; break; }
        if (!destination) return false;
    }

	const int iLoadMissionAICount = 4;
	// setting up a value so that we may loop through the following predefined array

	MissionAITypes aeLoadMissionAI[iLoadMissionAICount] = { MISSIONAI_LOAD_ASSAULT, MISSIONAI_LOAD_SETTLER, MISSIONAI_LOAD_SPECIAL, MISSIONAI_ATTACK_SPY };

	int iCurrentGroupSize = getGroup()->getNumUnits();
	// Under Size Matters we need the volume of the group
	if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
	{
		iCurrentGroupSize = getGroup()->getNumUnitCargoVolumeTotal();
	}

	LoadReservationCounts loadReservations(GET_PLAYER(getOwner()), getGroup(), aeLoadMissionAI, iLoadMissionAICount, GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS));

	CvUnit* bestTransportUnit = nullptr;
	int bestTransportUnitScore = MAX_INT;
	{
		PROFILE("CvUnitAI::AI_load.SearchTransport");

		CvReachablePlotSet plotSet(getGroup(), iFlags);
		bool reachabilityReady = false;

		foreach_(CvUnit * pLoopUnit, GET_PLAYER(getOwner()).getUnitsByAIRole(eUnitAI))
		{
			// Scores are non-negative and ties keep the first transport. Once
			// a zero-score rendezvous exists, no remaining ship can improve it.
			if (bestTransportUnitScore == 0) continue;

			// if the plot the unit under evaluation is on isn't the plot it was headed for?
			// if this unit can be loaded onto the looped unit
			if (pLoopUnit->AI_getUnitAIType() == eUnitAI && canLoadOntoUnit(pLoopUnit, pLoopUnit->plot()))
			{
                if (!isHuman() && AI_getUnitAIType() == UNITAI_WORKER && eUnitAI == UNITAI_SETTLER_SEA
                    && !AI_workerFerryDestination(static_cast<const CvUnitAI*>(pLoopUnit))) continue;
                if (!isHuman() && AI_getUnitAIType() == UNITAI_SETTLE && eUnitAI == UNITAI_SETTLER_SEA
                    && (!isFound() || (getGroup()->AI_getMissionAIType() == MISSIONAI_LOAD_SETTLER
                        && getGroup()->AI_getMissionAIPlot()
                        && !AI_colonyShipCanReach(pLoopUnit, getGroup()->AI_getMissionAIPlot())))) continue;
				// special case ASSAULT_SEA UnitAI, so that, if a unit is marked escort, but can load units, it will load them
				// transport units might have been built as escort, this most commonly happens with galleons
				UnitAITypes eLoopUnitAI = pLoopUnit->AI_getUnitAIType();

				// eloopunitai is the ai of the unit under evaluation.

				// if the unit under evaluation is the type we were wanting to get loaded through this command
				if (eLoopUnitAI == eUnitAI)// || (eUnitAI == UNITAI_ASSAULT_SEA && eLoopUnitAI == UNITAI_ESCORT_SEA))
				{
					int iCargoSpaceAvailable = pLoopUnit->cargoSpaceAvailable(getSpecialUnitType(), getDomainType());

					// iCargoSpaceAvailable refers to the space (in unit counts here) available to load units on the transport under eval
					if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
					{
						// here we make it refer to the space (in cargo volumes) available to load units on the transport instead
						iCargoSpaceAvailable -= loadReservations.reservedSpace(pLoopUnit);
						// here we must refer to things in a cargo volume manner rather than a unit count manner
						// This is asking to subtract out the number of units that already have planned to load onto this ship
						// Adapted the structure to enable me to ask not for the number of units but for the total cargo volume OF those units
						// iCargoSpaceAvailable now refers to actual cargo space by volume rather than by unit count
					}
					else
					{
						iCargoSpaceAvailable -= loadReservations.reservedSpace(pLoopUnit);
					}

					// if there is cargo space available after the units that have spoken for the transport are loaded then:
					if (iCargoSpaceAvailable > 0)
					{
						// note this may not be enough now because we're talking about cargo volumes that may still be greater than the amount available

						// if we're not already transporting any units
						if (eTransportedUnitAI == NO_UNITAI || pLoopUnit->getUnitAICargo(eTransportedUnitAI) > 0)
						{
							// or the transporting unit already is transporting unit(s) of the instructed ai type
							// (apparently seeks to ensure that transports only transport units with the same ai type?)
							if (iMinCargo == -1 || pLoopUnit->getCargo() >= iMinCargo)
							{
								// or the unit to be loaded onto has more or as much cargo as iMinCargo specified
								// looks like it's to enforce that settlers don't load alone
								// getCargo still has its ai uses under size matters as a basic count of the amount of UNITS that are loaded...
								// This is one example

								// Use existing count of cargo space available

								// Ok here we have a bigger issue.  iMinCargoSpace - minimum amount of units that must be loadable was what it DID mean
								if (iMinCargoSpace == -1 || iCargoSpaceAvailable >= iMinCargoSpace)
								{
									// NOW I'll need to search for it's bigger purpose
									// This will be harder to convert because I may need it to send through the minimum amount of sm cargo space in some cases
									// And for cases where I only need a 'count' it's impossible to say this generically.
									// Solved volumetrically - when option is on it takes the estimated best unit for the type it's trying to load and converts that unit's cargo volume * the iMinCargoSpace# previously defined.

									// max cargo space... what?  Don't load if there's too much space available on this ship???
									// Apparently its to keep counter units escorting settlers and workers from overloading themselves
									if (iMaxCargoSpace == -1 || iCargoSpaceAvailable <= iMaxCargoSpace)
									{
										// Resolved by conditionally making this value worth ((SMCargoVolume()*2)-1) -
										// twice the cargo volume of the evaluating unit but one less so a second one can't load after it does

										// I presume iMaxCargoOurUnitAI is intended to keep more than one settler from getting on the same boat.
										// Assuming I'm correct, this probably doesn't need to be addressed any differently as it's just going to be a count.
										if (iMaxCargoOurUnitAI == -1 || pLoopUnit->getUnitAICargo(AI_getUnitAIType()) <= iMaxCargoOurUnitAI)
										{
											// Don't block city defense from getting on board
											if (!pLoopUnit->plot()->isVisibleEnemyUnit(this))
											{
												CvPlot* pUnitTargetPlot = pLoopUnit->getGroup()->AI_getMissionAIPlot();
												if (pUnitTargetPlot == nullptr
													|| pUnitTargetPlot->getTeam() == getTeam()
													|| !pUnitTargetPlot->isOwned()
													|| !isPotentialEnemy(pUnitTargetPlot->getTeam(), pUnitTargetPlot))
												{
													int iPathTurns = 0;
													const int cargoPenalty = std::max(0, iCurrentGroupSize - iCargoSpaceAvailable) * 5;
													const int pathLimit = loadTransportPathLimit(iMaxPath, bestTransportUnitScore, cargoPenalty);
													// Apply all cargo/reservation/destination gates before building reachability.
													if (pathLimit >= 0
													&& reachableLoadTransport(this, pLoopUnit, plotSet, iMaxPath, reachabilityReady, eUnitAI)
													&& (atPlot(pLoopUnit->plot()) || generatePath(pLoopUnit->plot(), iFlags, true, &iPathTurns, pathLimit)))
													{
														// prefer a transport that can hold as much of our group as possible
														const int score = cargoPenalty + iPathTurns;

														if (score < bestTransportUnitScore)
														{
															bestTransportUnitScore = score;
															bestTransportUnit = pLoopUnit;
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if (bestTransportUnit != NULL && iMaxTransportPath < MAX_INT)
	{
		PROFILE("CvUnitAI::AI_load.SearchTarget");

		// Can transport reach enemy in requested time
		const int loadingRange = iMaxTransportPath * bestTransportUnit->baseMoves();
		if (!algo::any_of(
			plot()->rect(loadingRange, loadingRange),
			bind(canTransportReachPlot, _1, this, bestTransportUnit, iMaxTransportPath))
			)
		{
			bestTransportUnit = nullptr;
		}
	}

	if (bestTransportUnit != NULL)
	{
		if (atPlot(bestTransportUnit->plot()))
		{
			CvSelectionGroup* pOtherGroup = NULL;
			// TBNOTE: fixed setTransportUnit for Size Matters (hopefully correctly)
			getGroup()->setTransportUnit(bestTransportUnit, &pOtherGroup); // XXX is this dangerous (not pushing a mission...) XXX air units?

			// If part of large group loaded, then try to keep loading the rest
			if (eUnitAI == UNITAI_ASSAULT_SEA && (eMissionAI == MISSIONAI_LOAD_ASSAULT || eMissionAI == MISSIONAI_LOAD_SETTLER))
			{
				if (pOtherGroup != NULL && pOtherGroup->getNumUnits() > 0)
				{
					if (pOtherGroup->getHeadUnitAI() == AI_getUnitAIType())
					{
						pOtherGroup->getHeadUnit()->AI_load(eUnitAI, eMissionAI, eTransportedUnitAI, iMinCargo, iMinCargoSpace, iMaxCargoSpace, iMaxCargoOurUnitAI, iFlags, 0, iMaxTransportPath);
					}
					else if (eTransportedUnitAI == NO_UNITAI && iMinCargo < 0 && iMinCargoSpace < 0 && iMaxCargoSpace < 0 && iMaxCargoOurUnitAI < 0)
					{
						pOtherGroup->getHeadUnit()->AI_load(eUnitAI, eMissionAI, NO_UNITAI, -1, -1, -1, -1, iFlags, 0, iMaxTransportPath);
					}
				}
			}
			if (isCargo())
			{
                m_aiNavalCommitment[NavalCommitment::KIND] = NavalCommitment::NONE;
				return true;
			}
		}
		else
		{
			// BBAI TODO: To split or not to split?
			//Here is where it gets complicated again...
			const int iCargoSpaceAvailable = bestTransportUnit->cargoSpaceAvailable(getSpecialUnitType(), getDomainType());

			FAssertMsg(iCargoSpaceAvailable > 0, "best unit has no space");

			// split our group to fit on the transport
			CvSelectionGroup* pOtherGroup = NULL;
			//woooo... how to convert THIS???
			CvSelectionGroup* pSplitGroup = NULL;
			if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
			{
				pSplitGroup = getGroup()->splitGroup(iCargoSpaceAvailable, this, &pOtherGroup);
			}
			else
			{//we'll have to just split the group into individual units and let each process from there
				//This strikes me as having a high chance of disrupting the intentions of the units... with or without SM!
				pSplitGroup = getGroup()->splitGroup(1, this, &pOtherGroup);
			}
			FAssertMsg(pSplitGroup != NULL, "splitGroup failed");
			FAssertMsg(m_iGroupID == pSplitGroup->getID(), "splitGroup failed to put unit in the new group");

			if (pSplitGroup != NULL)
			{
                AI_rememberNavalCommitment(NavalCommitment::BOARD, bestTransportUnit->plot(), iFlags, eMissionAI, bestTransportUnit);
				CvPlot* pOldPlot = pSplitGroup->plot();
				pSplitGroup->pushMission(MISSION_MOVE_TO_UNIT, bestTransportUnit->getOwner(), bestTransportUnit->getID(), iFlags, false, false, eMissionAI, NULL, bestTransportUnit);
				bool bMoved = (pSplitGroup->plot() != pOldPlot);
				// Reaching a transport does not itself load a land unit in port.
				// Complete boarding before another AI update can choose guardCity
				// and release this ship's reservation for the next passenger.
				if (!isCargo() && atPlot(bestTransportUnit->plot())
					&& canLoadOntoUnit(bestTransportUnit, bestTransportUnit->plot()))
				{
					getGroup()->setTransportUnit(bestTransportUnit);
				}
				if (isCargo()) { m_aiNavalCommitment[NavalCommitment::KIND] = NavalCommitment::NONE; return true; }

				// A move may stop short and clear its queue (e.g. a changed next
				// step). Keep a still-valid reservation for the remainder of this
				// turn rather than letting every other defender chase that seat.
				if (bMoved && getGroup()->getLengthMissionQueue() == 0
					&& canLoadOntoUnit(bestTransportUnit, bestTransportUnit->plot()))
				{
					getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, eMissionAI, NULL, bestTransportUnit);
				}
				if (!bMoved && pOtherGroup != NULL)
				{
					joinGroup(pOtherGroup);
				}
				return bMoved;
			}
		}
	}
	//TBNOTE: A rather high likelihood that I may want to make this more advanced for SM - making the units that are too big to fit consider the option to split so they can get some of them onto the boats.  For now I think this might not be a good idea because it'll end up with weak units getting off the boat.  Better to just need more boats.
	return false;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					   END												  */
/************************************************************************************************/

// Returns true if a mission was pushed...
bool CvUnitAI::AI_guardCityBestDefender()
{
	CvCity* pCity;
	CvPlot* pPlot;

	pPlot = plot();
	pCity = pPlot->getPlotCity();

	if (pCity != NULL)
	{
		if (pCity->getOwner() == getOwner())
		{
			if (pPlot->isBestDefender(this))
			{
				getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, NULL);
				getGroup()->AI_setAsGarrison(pCity);
				return true;
			}
		}
	}

	return false;
}

bool CvUnitAI::AI_guardCityMinDefender(bool bSearch)
{


	PROFILE_FUNC();

	CvCity* pPlotCity = plot()->getPlotCity();
	if ((pPlotCity != NULL) && (pPlotCity->getOwner() == getOwner()))
	{
		const int iCityDefendersNeed = pPlotCity->AI_minDefenders();
		const int iCityDefenderCount = cityDefendersUpTo(pPlotCity->plot(), getOwner(), iCityDefendersNeed + 1);
		if ((iCityDefenderCount - 1) < iCityDefendersNeed)
		{
			//TB: Lets try it without this ankle cutting check for a bit and see if they can put up a fight when invaded
			/*if ((iCityDefenderCount <= 2) || (GC.getGame().getSorenRandNum(5, "AI shuffle defender") != 0))*/
			{

				getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, NULL);
				getGroup()->AI_setAsGarrison(pPlotCity);
				return true;
			}
		}
	}

	if (bSearch && (!DefenseDemandPerf::enabled() || isNPC()))
	{
		int iBestValue = 0;
		const CvPlot* pBestPlot = NULL;
		const CvPlot* pBestGuardPlot = NULL;

		const int iCurrentTurn = GC.getGame().getGameTurn();
		foreach_(const CvCity * pLoopCity, GET_PLAYER(getOwner()).cities())
		{
			if (AI_plotValid(pLoopCity->plot()))
			{
				/************************************************************************************************/
				/* BETTER_BTS_AI_MOD					  08/19/09								jdog5000	  */
				/*																							  */
				/* Unit AI, Efficiency																		  */
				/************************************************************************************************/
								// BBAI efficiency: check area for land units
				if ((getDomainType() == DOMAIN_LAND) && (!UnitTravelConnectivity::landUnitCanReach(this,pLoopCity->plot())) && !(getGroup()->canMoveAllTerrain()))
				{
					continue;
				}
				/************************************************************************************************/
				/* BETTER_BTS_AI_MOD					   END												  */
				/************************************************************************************************/

								//	Koshling - efficiency in handling babarins who have cities scattered all over
								//	the world - just search nearby ones
				if (isNPC() && stepDistance(pLoopCity->getX(), pLoopCity->getY(), getX(), getY()) > 10)
				{
					continue;
				}

				int iDefendersNeed = pLoopCity->AI_minDefenders();
				int iDefendersHave = cityDefendersUpTo(pLoopCity->plot(), getOwner(), iDefendersNeed);
				if (iDefendersHave < iDefendersNeed)
				{
					if (!pLoopCity->plot()->isVisibleEnemyUnit(this))
					{
						iDefendersHave += GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_GUARD_CITY, getGroup());
						if (iDefendersHave < iDefendersNeed + 1)
						{
							// Travel time can only subtract from this score. Keep the
							// broad search tree for contenders, but do not path to a
							// city that cannot win even with zero travel time.
							int iValue = (iDefendersNeed - iDefendersHave) * 20;
							iValue += 2 * std::min(15, iCurrentTurn - pLoopCity->getGameTurnAcquired());
							if (pLoopCity->isOccupation())
							{
								iValue += 5;
							}
							int iPathTurns;
							if (iValue > iBestValue && !atPlot(pLoopCity->plot()) && generatePath(pLoopCity->plot(), 0, true, &iPathTurns))
							{
								if (iPathTurns <= 10)
								{
									iValue -= iPathTurns;

									if (iValue > iBestValue)
									{
										iBestValue = iValue;
										pBestPlot = getPathEndTurnPlot();
										pBestGuardPlot = pLoopCity->plot();
									}
								}
							}
						}
					}
				}
			}
		}
		if (pBestPlot != NULL)
		{
			if (atPlot(pBestGuardPlot))
			{
				FAssert(pBestGuardPlot == pBestPlot);

				getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, NULL);
				return true;
			}
			FAssert(!atPlot(pBestPlot));
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_GUARD_CITY, pBestGuardPlot);
		}
	}

	return false;
}

// Returns true if a mission was pushed...
namespace {
	struct CityGuardPositionCandidate
	{
		CvPlot* plot;
		int value, order;
		CityGuardPositionCandidate(CvPlot* p, int v, int o) : plot(p), value(v), order(o) {}
		bool operator<(const CityGuardPositionCandidate& other) const { return value != other.value ? value > other.value : order < other.order; }
	};

	bool cityGuardReachableEdge(const CvSelectionGroup* group, const CvPlot* from, const CvPlot* to, bool endpoint)
	{
		bool terminus = false;
		const bool valid = ContextFreeNewPathValidFunc(group, from->getX(), from->getY(),
			to->getX(), to->getY(), MOVE_THROUGH_ENEMY, false, false, 0, NULL, &terminus);
		if (endpoint && !valid && !terminus)
		{
			bool dummy;
			terminus = NewPathDestValid(group, to->getX(), to->getY(), MOVE_THROUGH_ENEMY, dummy);
		}
		return valid || (endpoint && terminus);
	}

	// Positive certificate only: these are the same accepted edges as Populate(2).
	// A terminal-only or unrevealed intermediate plot must never extend a route.
	// Failure leaves the existing complete, cached flood as the authority.
	bool cityGuardHasShortPath(const CvSelectionGroup* group, const CvPlot* target)
	{
		const CvPlot* origin = group->plot();
		if (target == origin) return true;
		const int distance = stepDistance(origin->getX(), origin->getY(), target->getX(), target->getY());
		if (distance > 2) return false;
		PlotDangerCacheScope dangerCache(group->getOwner());
		if (distance == 1 && cityGuardReachableEdge(group, origin, target, true)) return true;
		foreach_(const CvPlot* via, origin->adjacent())
		{
			if (via == origin || via == target || !via->isRevealed(group->getTeam(), false)
				|| stepDistance(via->getX(), via->getY(), target->getX(), target->getY()) != 1) continue;
			if (cityGuardReachableEdge(group, origin, via, false)
				&& cityGuardReachableEdge(group, via, target, true)) return true;
		}
		return false;
	}

	bool guardCityReachable(CvReachablePlotSet& plotSet, const CvSelectionGroup* group, CvPlot* target, bool& populated)
	{
		// Populate always seeds the group origin. Staying in its own city needs
		// no movement flood; other cities retain the same two-step reach test.
		if (target == group->plot()) return true;
		if (!populated && plotSet.tryPopulateFromCache(2)) populated = true;
		if (!populated && cityGuardHasShortPath(group, target)) return true;
		if (!populated)
		{
			plotSet.Populate(2);
			populated = true;
		}
		return plotSet.find(target) != plotSet.end();
	}
}

bool CvUnitAI::AI_keepCityGuardPost(const CvCity* pCity)
{
	if (!AI_isCityAIType() || !AI_isCityGarrison(pCity)
		|| getGroup()->getNumUnits() != 1
		|| pCity->AI_isDanger() || plot() == pCity->plot()
		|| plot()->getOwner() != getOwner()
		|| plot()->getNumVisibleEnemyUnits(getOwner()) != 0
		|| plot()->defenseModifier(getTeam(), false) <= 0
		|| plot()->getDangerCount(getTeam()) != 0
		|| plot()->getTotalTurnDamage(getGroup()) > 0
		|| plot()->getFeatureTurnDamage() > 0
		|| exposedToDanger(plot(), 60, true)) return false;
	// A newly arrived guard has not fortified yet. Retargeting it here can
	// alternate between equally useful posts forever, before either is held.
	// AI_update clears an un-reaffirmed garrison after this decision.
	getGroup()->AI_setAsGarrison(pCity);
	getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, NULL);
	return true;
}

bool CvUnitAI::AI_guardCity(bool bLeave, bool bSearch, int iMaxPath)
{
	AI_PERF_SCOPE(CITY_GUARD);
	PROFILE_FUNC();
	FAssert(getDomainType() == DOMAIN_LAND || getDomainType() == DOMAIN_IMMOBILE);

	if (!getGroup()->canDefend())
	{
		return false;
	}
	CvReachablePlotSet plotSet(getGroup(), 0);
	bool bReachabilityReady = false;

	//	If we're already close to a city we may be considered part of it's defensive posture - check for those cases first
	for (int iPlotIndex = 0; iPlotIndex < NUM_CITY_PLOTS_2; iPlotIndex++)
	{
		CvPlot* pPlot = plotCity(getX(), getY(), iPlotIndex);

		if (pPlot != NULL && pPlot->getOwner() == getOwner())
		{
			CvCity* pCity = pPlot->getPlotCity();

			if (pCity != NULL && guardCityReachable(plotSet, getGroup(), pPlot, bReachabilityReady))
			{
				// NB: removed the property-control ejection/flip that used to re-type a garrisoned
                // unit to UNITAI_PROPERTY_CONTROL here. It used a broad capability test
                // (AI_hasBeneficialPropertyEffectForCity) that disagreed with the handler's narrow
                // one (getPropertyManipulators), so non-base-manipulator units got flipped and then
                // bounced straight back -> RESERVE<->PROPERTY_CONTROL oscillation. It also masked
                // the real need: flipped units made the property "getting better" and filled the 20%
                // quota, which blocked the need-based production path (CvCityAI::AI_chooseProduction
                // ~14458). Property-control units now come ONLY from that production path, assigned
                // the role at creation and never flipped.

				CityGuardDanger<CvPlayerAI, CvPlot> plotDanger(GET_PLAYER(getOwner()), pPlot);

				// Without leave permission, the existing rule always keeps city-AI
				// units on defense. Do not first assess a departure we cannot choose.
				bool bDefend = !bLeave && AI_isCityAIType();
				if (!bDefend)
				{
					int iExtra = 0;
					if (pCity->AI_isDanger()) iExtra -= plotDanger.get();
					//	Never remove the last unit
					if (atPlot(pPlot) && canDefend() && pPlot->plotCountUpTo(getGroup()->getNumUnits() < MAX_INT ? getGroup()->getNumUnits() + 1 : MAX_INT, PUF_canDefend /*PUF_canDefendGroupHead*/, -1, -1, NULL, getOwner()) == getGroup()->getNumUnits()) // XXX check for other team's units?
					{
						bDefend = true;
					}
					//	Check if it is adequately defended (allowing for this group leaving if it is thinking of doing so)
					//	and also that we don't need the units to maintain happiness
					else if (!(pCity->AI_isDefended(((!canDefend() || !AI_isCityGarrison(pCity)) ? 0 : -getGroup()->AI_getGenericValueTimes100(UNITVALUE_FLAGS_DEFENSIVE) / 100) + iExtra)) ||
							 (atPlot(pPlot) && isMilitaryHappiness() && !(pCity->AI_isAdequateHappinessMilitary(-getGroup()->getNumUnits()))))
					{
						bDefend = true;
					}
				}
				if (bDefend)
				{
					//	Need to defend city vicinity but not necessarily the city itself
					// Only subtract a prospective departure while we are in this city.
					// An outpost guard has already left its happiness garrison;
					// subtracting it again causes an immediate recall and repeat.
					bool bGuardInCity = !pCity->AI_isAdequateHappinessMilitary(atPlot(pPlot) ? -1 : 0);

					if (!bGuardInCity && canDefend())
					{
						//	Always leave at least half the defense actually in the city, and if there is
						//	danger around keep more (to match the enemy count)
						if (AI_isCityAIType())
						{
							bGuardInCity = cityAIGuardNeedsInCity(pPlot, getOwner(),
								atPlot(pPlot) ? getGroup()->getNumUnits() : 0, plotDanger);
						}
						else
						{
							const CityGuardDefenseSnapshot defenseCounts(pPlot, getOwner());
							const int currentAllDefenders = defenseCounts.all;
							const int currentAllCityAIDefenders = defenseCounts.cityAI;
							const int currentInCityDefenders = defenseCounts.inCity;
							int requiredInCityDefenders = std::max(plotDanger.get(), currentAllDefenders / 2);

							if (currentInCityDefenders <= (requiredInCityDefenders + (atPlot(pPlot) ? getGroup()->getNumUnits() : 0)))
							{
								//	Recall city AI types preferentially if there are enough
								if (currentAllCityAIDefenders < requiredInCityDefenders || AI_isCityAIType())
								{
									bGuardInCity = true;
								}
							}
						}
					}

					// Do not reconsider every city tile for an already useful outpost.
					if (!bGuardInCity && AI_keepCityGuardPost(pCity)) return true;

					CvPlot* pBestPlot = NULL;
					CvPlot* pBestGuardPlot = NULL;

					//	If we don't have to guard in the city figure out the best spot
					if (!bGuardInCity)
					{
						// Candidate scoring is read-only; release before ejecting or moving units.
						PlotDangerCacheScope positionDangerCache(getOwner());
						//	Guard a good spot outside the city but in its vicinity
						int iBestValue = 0;
						int bestOrder = MAX_INT, nextOrder = 0;
						// The city lookup above recognizes only this local footprint.
						// An outer-ring or detached settlement work plot would strand
						// its guard outside that lookup and trigger a retreat next update.
						const int guardPlots = std::min(pCity->getNumCityPlots(), NUM_CITY_PLOTS_2);
						std::vector<CityGuardPositionCandidate> positions;
						positions.reserve(guardPlots);

#ifdef C2C_AI_PERF_BASELINE
						foreach_(CvPlot * pLoopPlot, pPlot->rect(NUM_CITY_PLOTS_2, NUM_CITY_PLOTS_2))
#else
						// NUM_CITY_PLOTS_2 is a plot count (21), not a radius.
						foreach_(CvPlot * pLoopPlot, pCity->plots(guardPlots))
#endif
						{
							const int order = nextOrder++;
							if (AI_plotValid(pLoopPlot) && UnitTravelConnectivity::landUnitCanReach(this,pLoopPlot))
							{
								if (pLoopPlot->getOwner() == getOwner() &&
									pLoopPlot->getNumVisibleEnemyUnits(getOwner()) == 0)
								{
									//	Good defensive sites are way better
									int iValue = pLoopPlot->defenseModifier(getTeam(), false);

									//	Boost plots where there is known danger
									iValue = (iValue * (100 + pLoopPlot->getDangerCount(getTeam()))) / 100;

									// ls612: If the terrain is damaging, don't guard there
									bool	bHasTerrainDamage = (pLoopPlot->getTotalTurnDamage(getGroup()) > 0 || pLoopPlot->getFeatureTurnDamage() > 0);
									if (bHasTerrainDamage)
									{
										iValue = 0;
									}

									//	The city itself is considered only as a poor option since we already
									//	decided it wasn't really necessary to defend there in principal
									if (pLoopPlot == pPlot)
									{
										iValue += 2;	//	Make sure this is at least considered minimally viable

										//	Can guard in the city itself if no better candidates found, but down-grade it
										//	by a factor of 2
										iValue /= 2;
									}
									else
									{
										//	If there are already units guarding here reduce the value of another guard
										int iAlreadyGuarding = GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_GUARD_CITY, getGroup(), 0);
										if (iAlreadyGuarding > 0)
										{
											iValue /= (iAlreadyGuarding + 1);
										}
									}

									if (iValue > 0) positions.push_back(CityGuardPositionCandidate(pLoopPlot, iValue, order));
								}
							}
						}
						// Strong raw positions first tighten later path bounds. Retain the
						// original city-plot order for equal final scores, even when raw
						// values differ. Zero-turn arrivals are valid: their best possible
						// divisor is 2, not 3. Reservations still use the live world.
						std::sort(positions.begin(), positions.end());

						for (std::vector<CityGuardPositionCandidate>::const_iterator pos = positions.begin(); pos != positions.end(); ++pos)
						{
							const bool winsTie = pBestGuardPlot != NULL && pos->order < bestOrder;
							if (pos->value * 1000 / 2 < iBestValue || (pos->value * 1000 / 2 == iBestValue && !winsTie)) continue;
							const int maxWinningTurns = defenderPositionPathLimit(pos->value, iBestValue - (winsTie ? 1 : 0));
							int iPathTurns;
							if (maxWinningTurns >= 0 && generatePath(pos->plot, 0, true, &iPathTurns, maxWinningTurns))
							{
								const int score = pos->value * 1000 / (iPathTurns + 2);
								if (score > iBestValue || (score == iBestValue && winsTie))
								{
									if (!exposedToDanger(pos->plot, 60, true) && (getPathEndTurnPlot() == pos->plot || !exposedToDanger(getPathEndTurnPlot(), 60, true)))
									{
										iBestValue = score;
										bestOrder = pos->order;
										pBestPlot = getPathEndTurnPlot();
										pBestGuardPlot = pos->plot;
										FAssert(atPlot(pBestGuardPlot) || !atPlot(pBestPlot));
									}
								}
							}
						}

					}
					else
					{
						pBestPlot = pPlot;
						pBestGuardPlot = pPlot;
					}

					if ((pBestPlot != NULL) && (pBestGuardPlot != NULL))
					{
						CvSelectionGroup* pOldGroup = getGroup();
						CvUnit* pEjectedUnit = getGroup()->AI_ejectBestDefender(pBestGuardPlot);

						if (pEjectedUnit == NULL)
						{
							//	Mark this group as part of the city garrison
							getGroup()->AI_setAsGarrison(pCity);

							//	Not an ideal defensive candidate - is there a decent attack if the city
							//	really feels it needs us as part of it (active?) defense?
							if (AI_anyAttack(2, 55))
							{
								return true;
							}

							//	Ok, no attacks so I guess we'll defend as best we can
							pEjectedUnit = getGroup()->AI_ejectBestDefender(pBestGuardPlot, true);
						}
						if (pEjectedUnit != NULL)
						{
							CvPlot* missionPlot = (bGuardInCity ? pBestGuardPlot : pBestPlot);
							if (bGuardInCity)
							{
								if (pPlot->plotCount(PUF_isCityAIType, -1, -1, NULL, getOwner()) == 0)
								{
									if (pEjectedUnit->cityDefenseModifier() > 0)
									{
										FAssert(pEjectedUnit->AI_getUnitAIType() != UNITAI_HUNTER);
										pEjectedUnit->AI_setUnitAIType(UNITAI_CITY_DEFENSE);
									}
								}
							}
							if (atPlot(pBestGuardPlot))
							{
								//	Mark the ejected unit as part of the city garrison
								pEjectedUnit->getGroup()->AI_setAsGarrison(pCity);
								pEjectedUnit->AI_setUnitAIType(UNITAI_CITY_DEFENSE);
								pEjectedUnit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, NULL);
								return (pEjectedUnit->getGroup() == pOldGroup || pEjectedUnit == this);
							}
							else if (pEjectedUnit->generatePath(missionPlot, 0, true))
							{
								//	Mark the ejected unit as part of the city garrison
								pEjectedUnit->getGroup()->AI_setAsGarrison(pCity);
								pEjectedUnit->AI_setUnitAIType(UNITAI_CITY_DEFENSE);
								return (pEjectedUnit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, missionPlot->getX(), missionPlot->getY(), 0, false, false, MISSIONAI_GUARD_CITY, missionPlot));
							}
							else
							{
								//	If we can't move after all regroup
								pEjectedUnit->joinGroup(pOldGroup);
								return false;
							}
						}
						else
						{
							return false;
						}
					}
					else
					{
						//This unit is not suited for defense, skip the mission
						//to protect this city but encourage others to defend instead.
						getGroup()->pushMission(MISSION_SKIP);
						finishMoves();
					}
					return true;
				}
			}
		}
	}

	if (bSearch && (!DefenseDemandPerf::enabled() || isNPC()))
	{
		int iBestValue = MAX_INT;
		const CvPlot* pBestPlot = NULL;
		const CvPlot* pBestGuardPlot = NULL;

		foreach_(CvCity * pLoopCity, GET_PLAYER(getOwner()).cities())
		{
			if (AI_plotValid(pLoopCity->plot()))
			{
				// BBAI efficiency: check area for land units
				if ((getDomainType() == DOMAIN_LAND) && (!UnitTravelConnectivity::landUnitCanReach(this,pLoopCity->plot())) && !(getGroup()->canMoveAllTerrain()))
				{
					continue;
				}
				if (!(pLoopCity->AI_isDefended((!AI_isCityAIType()) ? pLoopCity->plot()->plotStrength(UNITVALUE_FLAGS_DEFENSIVE, PUF_canDefendGroupHead, -1, -1, getOwner(), NO_TEAM, PUF_isNotCityAIType) : 0)) ||
					(isMilitaryHappiness() && !(pLoopCity->AI_isAdequateHappinessMilitary())))
				{
					if (!pLoopCity->plot()->isVisibleEnemyUnit(this))
					{
						if ((GC.getGame().getGameTurn() - pLoopCity->getGameTurnAcquired() < 10) || GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_GUARD_CITY, getGroup()) < 2)
						{
							int iPathTurns;
							if (!atPlot(pLoopCity->plot()) && generatePath(pLoopCity->plot(), 0, true, &iPathTurns, defenderCityPathLimit(iMaxPath)))
							{
								if (iPathTurns <= iMaxPath && iPathTurns < iBestValue)
								{
									iBestValue = iPathTurns;
									pBestPlot = getPathEndTurnPlot();
									pBestGuardPlot = pLoopCity->plot();
									FAssert(!atPlot(pBestPlot));
								}
							}
						}
					}
				}

				if (pBestPlot != NULL)
				{
					break;
				}
			}
		}

		if ((pBestPlot != NULL) && (pBestGuardPlot != NULL))
		{
			FAssert(!atPlot(pBestPlot));
			CvSelectionGroup* pOldGroup = getGroup();
			CvUnit* pEjectedUnit = getGroup()->AI_ejectBestDefender(pBestGuardPlot);
			// If no member can fill the defensive role, moving the same group
			// between needy cities cannot satisfy either city. Leave other roles
			// to their own AI instead of issuing a fictitious reinforcement.
			if (pEjectedUnit == NULL) return false;
			const PlayerTypes defenderOwner = pEjectedUnit->getOwner();
			const int defenderId = pEjectedUnit->getID();
			const bool handledHere = pEjectedUnit == this || pEjectedUnit->getGroup() == pOldGroup;
			const bool dispatched = pEjectedUnit->getGroup()->pushMissionInternal(MISSION_MOVE_TO,
				pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_GUARD_CITY, pBestGuardPlot);
			// A synchronous move may kill its unit. Never dereference that old
			// pointer after issuing the order, including the failure path.
			CvUnit* defender = GET_PLAYER(defenderOwner).getUnit(defenderId);
			if (defender == NULL) return true;
			if (dispatched)
			{
				CvCity* destination = pBestGuardPlot->getPlotCity();
				if (destination != NULL && destination->getOwner() == defenderOwner)
				{
					defender->AI_setAsGarrison(destination);
					// Changing role rejoins a fresh group and discards its missions.
					// Preserve the route while travelling; convert only on arrival.
					if (defender->atPlot(pBestGuardPlot))
						defender->AI_setUnitAIType(UNITAI_CITY_DEFENSE);
				}
				return handledHere;
			}
			defender->joinGroup(pOldGroup);
			return false;
		}
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_guardCityAirlift()
{
	if (DefenseDemandPerf::enabled() && !isNPC()) return false;
	PROFILE_FUNC();

	if (getGroup()->getNumUnits() > 1)
	{
		return false;
	}

	const CvCity* pCity = plot()->getPlotCity();

	if (pCity == NULL)
	{
		return false;
	}

	if (pCity->getMaxAirlift() == 0)
	{
		return false;
	}

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(CvCity * pLoopCity, GET_PLAYER(getOwner()).cities())
	{
		if (pLoopCity != pCity)
		{
			if (canAirliftAt(pCity->plot(), pLoopCity->getX(), pLoopCity->getY()))
			{
				if (!(pLoopCity->AI_isDefended((!AI_isCityAIType()) ? pLoopCity->plot()->plotStrength(UNITVALUE_FLAGS_DEFENSIVE, PUF_canDefendGroupHead, -1, -1, getOwner(), NO_TEAM, PUF_isNotCityAIType) : 0)))	// XXX check for other team's units?
				{
					int iValue = pLoopCity->getPopulation();

					if (pLoopCity->AI_isDanger())
					{
						iValue *= 2;
					}

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = pLoopCity->plot();
						FAssert(pLoopCity != pCity);
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_AIRLIFT, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_guardBonus(int iMinValue)
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	CvPlot* pBestGuardPlot;
	BonusTypes eNonObsoleteBonus;
	int iPathTurns;
	int iValue;
	int iBestValue;
	int iI;

	iBestValue = 0;
	pBestPlot = NULL;
	pBestGuardPlot = NULL;

	const std::map<int, BonusTypes>& guardableResourcePlots = GET_PLAYER(getOwner()).getGuardableResourcePlots();
	std::multimap<int, int> sortedIndex;

	//	A valuable optimization is possible to avoid generating the paths for all candidate resource plots (which is the expensive part).
	//	If a plot couldn't beat the best one already evaluated even with a path distance of 1 there is no need to evaluate its path.
	//	In practise this happens a LOT because the AI sends units to guard resources, then re-evaluates every turn so they are mostly
	//	already in the right place.  To ensure an evaluation order that makes this optimization likely we sort on simple plot distance
	for (std::map<int, BonusTypes>::const_iterator itr = guardableResourcePlots.begin(); itr != guardableResourcePlots.end(); ++itr)
	{
		pLoopPlot = GC.getMap().plotByIndex(itr->first);

		sortedIndex.insert(std::pair<int, int>(stepDistance(getX(), getY(), pLoopPlot->getX(), pLoopPlot->getY()), itr->first));
	}

	for (std::multimap<int, int>::const_iterator itr = sortedIndex.begin(); itr != sortedIndex.end(); ++itr)
	{
		PROFILE("AI_guardBonus.LoopOuter");

		iI = itr->second;

		std::map<int, BonusTypes>::const_iterator searchItr = guardableResourcePlots.find(iI);
		FAssert(searchItr != guardableResourcePlots.end());
		eNonObsoleteBonus = searchItr->second;

		pLoopPlot = GC.getMap().plotByIndex(iI);

		if (AI_plotValid(pLoopPlot))
		{
			iValue = GET_PLAYER(getOwner()).AI_bonusVal(eNonObsoleteBonus);

			iValue += std::max(0, 200 * GC.getBonusInfo(eNonObsoleteBonus).getAIObjective());

			if (pLoopPlot->getPlotGroupConnectedBonus(getOwner(), eNonObsoleteBonus) == 1)
			{
				iValue *= 2;
			}

			if (iValue > iMinValue)
			{
				PROFILE("AI_guardBonus.HasValue");

				if (!(pLoopPlot->isVisibleEnemyUnit(this)))
				{
					PROFILE("AI_guardBonus.NoVisibleEnemy");

					// BBAI TODO: Multiple defenders for higher value resources?
					if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_GUARD_BONUS, getGroup()) == 0)
					{
						//	Can this possibly beat the best we already have?
						iValue *= 1000;

						//	Path length from a plot to itself (empirically) returns 1, hence the '3' rather than '2'
						if (iValue / 3 > iBestValue)
						{
							PROFILE("AI_guardBonus.TestPath");

							if (generatePath(pLoopPlot, 0, true, &iPathTurns))
							{
								OutputDebugString(CvString::format("Evaluate guard path from (%d,%d) to (%d,%d), turns=%d, value=%d\n",
									getX(), getY(),
									pLoopPlot->getX(),
									pLoopPlot->getY(),
									iPathTurns,
									iValue).c_str());

								iValue /= (iPathTurns + 1);

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestPlot = getPathEndTurnPlot();
									pBestGuardPlot = pLoopPlot;
								}
							}
						}
						else
						{
							OutputDebugString(CvString::format("Resource at (%d,%d) with value %d cannot beat best value so far of %d\n",
								pLoopPlot->getX(),
								pLoopPlot->getY(),
								iValue,
								iBestValue).c_str());

						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestGuardPlot != NULL))
	{
		if (atPlot(pBestGuardPlot))
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_BONUS, pBestGuardPlot);
			return true;
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_GUARD_BONUS, pBestGuardPlot);
		}
	}

	return false;
}

int CvUnitAI::AI_getPlotDefendersNeeded(const CvPlot* pPlot, int iExtra) const
{
	PROFILE_EXTRA_FUNC();
	int iNeeded = iExtra;
	const BonusTypes eNonObsoleteBonus = pPlot->getNonObsoleteBonusType(getTeam());
	if (eNonObsoleteBonus != NO_BONUS)
	{
		iNeeded += (GET_PLAYER(getOwner()).AI_bonusVal(eNonObsoleteBonus) + 10) / 19;
	}

	//	Super forts - PARTIAL merge here to encourage defense of choke points
	const CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	if (kPlayer.AI_getPlotDanger(pPlot) > 0)
	{
		++iNeeded;
		if ((kPlayer.AI_getPlotCanalValue(pPlot) > 0)
			|| (kPlayer.AI_getPlotChokeValue(pPlot) > 0)
			|| (kPlayer.AI_getPlotAirbaseValue(pPlot) > 0))
		{
			++iNeeded;
		}
	}
	//	Super forts end

	int iDefense = pPlot->defenseModifier(getTeam(), true);

	iNeeded += (iDefense + 25) / 50;

	if (pPlot->getMilitaryImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(pPlot->getMilitaryImprovementType()).getCulture() > 0)
	{
		iNeeded = std::max(1 + (GC.getImprovementInfo(pPlot->getMilitaryImprovementType()).isMilitaryStructure() ? 1 : 0), iNeeded);
	}

	if (iNeeded == 0)
	{
		return 0;
	}

	iNeeded += GET_PLAYER(getOwner()).AI_getPlotAirbaseValue(pPlot) / 50;

	int iNumHostiles = 0;
	int iNumPlots = 0;

	foreach_(const CvPlot * pLoopPlot, pPlot->rect(2, 2))
	{
		iNumHostiles += pLoopPlot->getNumVisiblePotentialEnemyDefenders(this);
		if (pLoopPlot->getTeam() != getTeam() || pLoopPlot->isCoastalLand())
		{
			iNumPlots++;
			if (isEnemy(pLoopPlot->getTeam()))
			{
				iNumPlots += 4;
			}
		}
	}

	if ((iNumHostiles == 0) && (iNumPlots < 4))
	{
		if (iNeeded > 1)
		{
			iNeeded = 1;
		}
		else
		{
			iNeeded = 0;
		}
	}

	return iNeeded;
}

namespace {
int fortDepartingCityDefenders(const CvSelectionGroup* group)
{
    int count = 0;
    foreach_(const CvUnit* unit, group->units())
    {
        if (PUF_isCityAIType(unit, -1, -1, NULL)) ++count;
    }
    return count;
}
}

bool CvUnitAI::AI_guardFort(bool bSearch)
{
	PROFILE_FUNC();

	if (plot()->getOwner() == getOwner())
	{
		const ImprovementTypes eImprovement = plot()->getMilitaryImprovementType();
		if (eImprovement != NO_IMPROVEMENT)
		{
			if (GC.getImprovementInfo(eImprovement).isMilitaryStructure())
			{
				// Super Forts begin *AI_defense* - just tweaked a number here (iExtra now 1 instead of 0)
				const int stationedDefenders = plot()->plotCount(PUF_isCityAIType, -1, -1, NULL, getOwner());
				const int neededDefenders = AI_getPlotDefendersNeeded(plot(), 1);
				// Optional regrouping must not take the whole minimum garrison away.
				if (stationedDefenders <= neededDefenders
					|| stationedDefenders - fortDepartingCityDefenders(getGroup()) < neededDefenders)
					// Super Forts end
				{
					getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_BONUS, plot());
					return true;
				}
			}
		}
	}

	if (!bSearch || (DefenseDemandPerf::enabled() && !isNPC()))
	{
		return false;
	}

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestGuardPlot = NULL;

	// Static membership is already indexed. Occupants, needs and reservations
	// remain live; expand reachability only for an otherwise useful post.
	CvReachablePlotSet plotSet(getGroup(), 0);
	bool reachabilityReady = false;
	bool offenseReady = false;
	int ownOffense = 0;

	const std::vector<int>& posts = GET_PLAYER(getOwner()).getDefensivePostPlotIndices();
	for (size_t post = 0; post < posts.size(); ++post)
	{
		const CvPlot* pLoopPlot = GC.getMap().plotByIndex(posts[post]);
		//	Koshling - AI_plotValid() call not needed when using a CvReachablePlotSet
		if (/*AI_plotValid(pLoopPlot) &&*/ !atPlot(pLoopPlot) && UnitTravelConnectivity::landUnitCanReach(this,pLoopPlot))
		{
			if (pLoopPlot->getOwner() == getOwner())
			{
				const ImprovementTypes eImprovement = pLoopPlot->getMilitaryImprovementType();
				if (eImprovement != NO_IMPROVEMENT)
				{
					if (GC.getImprovementInfo(eImprovement).isMilitaryStructure())
					{
						// Super Forts begin *AI_defense* - just tweaked a number here (iExtra now 1 instead of 0)
						const int stationedDefenders = pLoopPlot->plotCount(PUF_isCityAIType, -1, -1, NULL, getOwner());
						int iValue = AI_getPlotDefendersNeeded(pLoopPlot, 1) - stationedDefenders;
						// Super Forts end

						if (iValue > 0)
						{
							// Friendly strength at our origin is identical for every candidate.
							if (!offenseReady)
							{
								ownOffense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), true, true, false, true);
								offenseReady = true;
							}
							if (!(30 * ownOffense < 100 * GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(pLoopPlot, true, true, false)))
							{
								if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_GUARD_BONUS, getGroup()) < iValue)
								{
									if (!reachabilityReady) { plotSet.Populate(MAX_INT); reachabilityReady = true; }
									if (plotSet.find(const_cast<CvPlot*>(pLoopPlot)) == plotSet.end()) continue;
									int iPathTurns;
									// The existing score cannot beat the incumbent beyond this limit.
									const int pathLimit = defenderPositionPathLimit(iValue, iBestValue);
									if (pathLimit >= 0 && generatePath(pLoopPlot, 0, true, &iPathTurns, pathLimit))
									{
										iValue *= 1000;

										iValue /= (iPathTurns + 2);

										if (iValue > iBestValue)
										{
											iBestValue = iValue;
											pBestPlot = getPathEndTurnPlot();
											pBestGuardPlot = pLoopPlot;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestGuardPlot != NULL))
	{
		if (atPlot(pBestGuardPlot))
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_BONUS, pBestGuardPlot);
			return true;
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_GUARD_BONUS, pBestGuardPlot);
		}
	}

	return false;
}


// Super Forts begin *AI_defense* - this is designed to ensure each fort has one defender (or improvements that require a fortified unit to upgrade)
bool CvUnitAI::AI_guardFortMinDefender(bool bSearch)
{

	PROFILE_FUNC();

	if (plot()->getOwner() == getOwner())
	{
		const ImprovementTypes eImprovement = plot()->getMilitaryImprovementType();
		if (eImprovement != NO_IMPROVEMENT)
		{
			if (GC.getImprovementInfo(eImprovement).isMilitaryStructure() || GC.getImprovementInfo(eImprovement).isUpgradeRequiresFortify() || (DefenseDemandPerf::enabled() && GC.getImprovementInfo(eImprovement).isActsAsCity()))
			{
				const int stationedDefenders = plot()->plotCount(PUF_isCityAIType, -1, -1, NULL, getOwner());
				const int neededDefenders = DefenseDemandPerf::enabled()?AI_defenderDemandForPlot(plot()):1;
				// Optional regrouping must not take the whole minimum garrison away.
				if (stationedDefenders <= neededDefenders
					|| stationedDefenders - fortDepartingCityDefenders(getGroup()) < neededDefenders)
				{

					getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_BONUS, plot());
					return true;
				}
			}
		}
	}

	if (!bSearch)
	{
		return false;
	}

    // Forts publish staffing demands through the broker. A spare defender
    // no longer scans every post; the local minimum-garrison hold above stays.
    if(DefenseDemandPerf::enabled() && !isNPC())
    {DefenseDemandPerf::count(DefenseDemandPerf::LEGACY_SKIPS);return false;}

    int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestGuardPlot = NULL;

	const std::vector<int>& posts = GET_PLAYER(getOwner()).getDefensivePostPlotIndices();
	if (posts.empty()) return false;
	// A connected post is a useful first candidate, never proof of military
	// reachability. Disconnected/unknown posts remain eligible below.
	const CvPlotGroup* pNetwork = plot()->getPlotGroup(getOwner());
	std::vector<std::pair<std::pair<int, int>, int> > candidates;
	for (size_t post = 0; post < posts.size(); ++post)
	{
		const int iI = posts[post];
		const CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);
		// Filter non-posts before unit movement/reveal legality. Keep every
		// defender count and incoming-mission check for actual candidates.
		if (pLoopPlot->getOwner() != getOwner() || atPlot(pLoopPlot)) continue;
		const ImprovementTypes eImprovement = pLoopPlot->getMilitaryImprovementType();
		if (eImprovement == NO_IMPROVEMENT
			|| !(GC.getImprovementInfo(eImprovement).isMilitaryStructure() || GC.getImprovementInfo(eImprovement).isUpgradeRequiresFortify())
			|| !AI_plotValid(pLoopPlot)) continue;
		if (!pLoopPlot->isVisibleEnemyUnit(this))
		{
			if (pLoopPlot->plotCount(PUF_isCityAIType, -1, -1, NULL, getOwner()) == 0)
			{
				const int iMyDistance = stepDistance(getX(), getY(), pLoopPlot->getX(), pLoopPlot->getY());

				//	If another closer group is already headed there don't bother
				int iOtherGroupDistance;
				if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_GUARD_BONUS, getGroup(), 0, &iOtherGroupDistance) == 0 ||
					iMyDistance < iOtherGroupDistance)
				{
					const int iNetworkBand = pNetwork != NULL && pLoopPlot->getPlotGroup(getOwner()) == pNetwork ? 0 : 1;
					candidates.push_back(std::make_pair(std::make_pair(iNetworkBand, iMyDistance), iI));
				}
			}
		}
	}

	// Near candidates tend to establish a useful turn bound earlier. Still
	// test every possible winner; map index resolves score ties exactly as
	// the old full-map traversal did, regardless of this evaluation order.
	std::sort(candidates.begin(), candidates.end());
	int iBestIndex = MAX_INT;
	for (int i = 0; i < (int)candidates.size(); ++i)
	{
		const int iIndex = candidates[i].second;
		const CvPlot* pLoopPlot = GC.getMap().plotByIndex(iIndex);
		const bool bWinsTie = pBestPlot != NULL && iIndex < iBestIndex;
		// Minimum outpost staffing is a local assignment, not an empire-wide
		// redeployment. Keep nearby tunnels/hovercraft routes eligible; distant
		// posts are supplied by local forces or the existing transport AI.
		const int iCandidateMaxTurns = std::min(4, defenderFortPathLimit(iBestValue - (bWinsTie ? 1 : 0)));
		int iPathTurns;
		if (iCandidateMaxTurns >= 0 && generatePath(pLoopPlot, 0, true, &iPathTurns, iCandidateMaxTurns))
		{
			const int iValue = 1000 / (iPathTurns + 2);
			if (iValue > iBestValue || (bWinsTie && iValue == iBestValue))
			{
				iBestValue = iValue;
				iBestIndex = iIndex;
				pBestPlot = getPathEndTurnPlot();
				pBestGuardPlot = pLoopPlot;
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestGuardPlot != NULL))
	{
		if (atPlot(pBestGuardPlot))
		{

			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_BONUS, pBestGuardPlot);
			return true;
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			return (getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_GUARD_BONUS, pBestGuardPlot));
		}
	}

	return false;
}
// Super Forts end

// Returns true if a mission was pushed...
bool CvUnitAI::AI_guardCitySite()
{
	PROFILE_FUNC();

	CvPlot* pBestPlot = NULL;
	CvPlot* pBestGuardPlot = NULL;
	{
		const CvPlayerAI& owner = GET_PLAYER(getOwner());
		int iBestValue = 0;
		int iPathTurns;
		for (int iI = 0; iI < owner.AI_getNumCitySites(); iI++)
		{
			CvPlot* plotX = owner.AI_getCitySite(iI);

			if (owner.AI_plotTargetMissionAIs(plotX, MISSIONAI_GUARD_CITY, getGroup()) == 0 && generatePath(plotX, 0, true, &iPathTurns))
			{
				const int iValue = plotX->getFoundValue(getOwner());
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = getPathEndTurnPlot();
					pBestGuardPlot = plotX;
				}
			}
		}
	}

	if (pBestPlot != NULL && pBestGuardPlot != NULL)
	{
		if (atPlot(pBestGuardPlot))
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, pBestGuardPlot);
			return true;
		}
		FAssert(!atPlot(pBestPlot));
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_GUARD_CITY, pBestGuardPlot);
	}
	return false;
}



// Returns true if a mission was pushed...
bool CvUnitAI::AI_guardSpy(int iRandomPercent)
{
	PROFILE_FUNC();

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestGuardPlot = NULL;
	const bool bCultureVictoryValid = GC.getGame().culturalVictoryValid();

	foreach_(const CvCity * pLoopCity, GET_PLAYER(getOwner()).cities())
	{
		if (!AI_plotValid(pLoopCity->plot()) || pLoopCity->plot()->isVisibleEnemyUnit(this)
		|| getDomainType() == DOMAIN_LAND && !UnitTravelConnectivity::landUnitCanReach(this,pLoopCity->plot()) && !getGroup()->canMoveAllTerrain())
		{
			continue;
		}
		int iValue = 0;

		if (GET_PLAYER(getOwner()).AI_isDoVictoryStrategy(AI_VICTORY_SPACE4))
		{
			if (pLoopCity->isCapital())
			{
				iValue += 30;
			}
			else if (pLoopCity->isProductionProject())
			{
				iValue += 5;
			}
		}

		if (bCultureVictoryValid
		&& GET_PLAYER(getOwner()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3)
		&& pLoopCity->getCultureLevel() >= GC.getGame().culturalVictoryCultureLevel())
		{
			iValue += 10;
		}

		if (pLoopCity->isProductionUnit())
		{
			if (isLimitedUnit(pLoopCity->getProductionUnit()))
			{
				iValue += 4;
			}
		}
		else if (pLoopCity->isProductionBuilding())
		{
			if (isLimitedWonder(pLoopCity->getProductionBuilding()))
			{
				iValue += 5;
			}
		}
		else if (pLoopCity->isProductionProject())
		{
			if (isLimitedProject(pLoopCity->getProductionProject()))
			{
				iValue += 6;
			}
		}

		if (iValue > 0)
		{
			if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_GUARD_SPY, getGroup()) == 0)
			{
				int iPathTurns;
				if (generatePath(pLoopCity->plot(), 0, true, &iPathTurns))
				{
					iValue *= 100 + GC.getGame().getSorenRandNum(iRandomPercent, "AI Guard Spy");
					//iValue /= 100;
					iValue /= iPathTurns + 1;

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = getPathEndTurnPlot();
						pBestGuardPlot = pLoopCity->plot();
					}
				}
			}
		}
	}

	if (pBestPlot && pBestGuardPlot)
	{
		if (atPlot(pBestGuardPlot))
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_SPY, pBestGuardPlot);
			return true;
		}
		FAssert(!atPlot(pBestPlot));
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_GUARD_SPY, pBestGuardPlot);
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_chokeDefend()
{
	FAssert(AI_isCityAIType());

	// XXX what about amphib invasions?

	CvCity* pCity = plot()->getPlotCity();

	if (pCity != NULL && pCity->getOwner() == getOwner() && pCity->AI_neededDefenders() > 1)
	{
		// Reject an empty ordinary counterattack before valuing the garrison.
		// Keep ambush/ranged actions on the original path; canHunt is false in
		// a real city. A zero stack bound is conservative until danger is known.
		if (!isAssassin() && !canRangeStrike()
		&& !AI_hasLocalAttackCandidate(AI_searchRange(1), 65, 0, true)) return false;

		if (!pCity->AI_isDefended(pCity->plot()->plotStrength(UNITVALUE_FLAGS_DEFENSIVE,
			PUF_canDefendGroupHead, -1, -1, getOwner(), NO_TEAM, PUF_isNotCityAIType))) return false;

		const int iPlotDanger = GET_PLAYER(getOwner()).AI_getPlotDanger(plot(), 3);

		if (iPlotDanger <= 4 && AI_anyAttack(1, 65, std::max(0, iPlotDanger - 1)))
		{
			return true;
		}
	}
	return false;
}


// Returns true if a mission was pushed...
// Healing ships prefer a nearby refuge before considering the wider search.
// Keep the original full-range fallback and every safety/legality check.
bool CvUnitAI::AI_healingCity()
{
	if (!isHuman() && getDomainType() == DOMAIN_SEA)
	{
		// Prefer an owned refuge within the existing one-turn search before
		// scanning thousands of ocean plots for forts and foreign ports.
		// Failure retains the complete legacy city/fort fallback below.
		if (!plot()->isCity(true))
		{
			CvPlot* best = NULL;
            const CvPlot* refuge = NULL;
			int bestValue = 0;
			const int range = AI_searchRange(1);
			foreach_(const CvCity* city, GET_PLAYER(getOwner()).cities())
			{
				CvPlot* destination = city->plot();
				if (stepDistance(getX(), getY(), destination->getX(), destination->getY()) > range
					|| !AI_plotValid(destination) || !canEnterPlot(destination)) continue;
				const int value = 100 + 3 * city->getPopulation();
				const int limit = navalCityPathLimit(value, bestValue, 1);
				int turns = 0;
				if (limit < 0 || !generatePath(destination, 0, true, &turns, limit)) continue;
				const int score = value / (1 + turns);
				if (score > bestValue && !atPlot(getPathEndTurnPlot()))
				{
					bestValue = score;
					best = getPathEndTurnPlot();
                    refuge = destination;
				}
			}
            if (best != NULL) {
                AI_rememberNavalCommitment(NavalCommitment::REPAIR, refuge);
                return getGroup()->pushMissionInternal(MISSION_MOVE_TO, best->getX(), best->getY());
            }
		}
		if (AI_moveIntoCity(1)) return true;
	}
	return AI_moveIntoCity(2);
}

bool CvUnitAI::AI_healingSafety(int iMaxPath)
{
	if (!isHuman() && getDomainType() == DOMAIN_SEA && iMaxPath > 1 && AI_safety(1)) return true;
	return AI_safety(iMaxPath);
}

bool CvUnitAI::AI_heal(int iDamagePercent, int iMaxPath)
{
    NAVAL_DIAG_PHASE(HEAL,iDamagePercent,iMaxPath,0,0);

	PROFILE_FUNC();

	CvSelectionGroup* pGroup = getGroup();

	if (plot()->getTotalTurnDamage(pGroup) > 0)
	{

#ifdef _DEBUG
		OutputDebugString("AI_heal: denying heal due to terrain damage\n");
#endif
		NAVAL_DIAG_PHASE_RETURN((false));
	}
	if (iDamagePercent < 1)
	{
		iDamagePercent = 10;
	}

	if (pGroup->getNumUnits() == 1)
	{
		if (getDamage() < 1)
		{
			NAVAL_DIAG_PHASE_RETURN((false));
		}

#ifdef _DEBUG
		OutputDebugString("AI_heal: one unit stack\n");
#endif
		// canHeal() guard: MISSION_HEAL is a no-op when the engine can't actually start a
        // heal here (canStartMission(MISSION_HEAL) -> canHeal() is false, e.g. healTurns==0
        // or the unit isWaiting). Without this guard AI_heal still returned true, so the
        // move cascade thought the unit's turn was spent while it stayed readyToMove() --
        // the unit then re-decided "heal" every pass, spinning to the iTempHack=50 safety
        // each turn (and risking the rare never-ending turn). Only claim the heal when it
        // will actually take; otherwise fall through so the unit does something real / SKIPs.
		if ((plot()->isCity() || healTurns(plot()) == 1) && !isAlwaysHeal() && canHeal(plot()))
		{

#ifdef _DEBUG
			OutputDebugString("AI_heal: city or 1 turn heal\n");
#endif
// 			AI_logAct("heal", "healInCity", plot());
			pGroup->pushMission(MISSION_HEAL);
			NAVAL_DIAG_PHASE_RETURN((true));
		}
		if (plot()->getNumVisibleAdjacentEnemyDefenders(this) == 0 && !isAlwaysHeal()
		&& (plot()->getTeam() == NO_TEAM || !GET_TEAM(plot()->getTeam()).isAtWar(getTeam())))
		{
			if (!plot()->isCity() && AI_moveIntoCity(1))
			{

#ifdef _DEBUG
				OutputDebugString("AI_heal: one turn city move\n");
#endif
// 				AI_logAct("heal", "moveToCityToHeal", plot());
				NAVAL_DIAG_PHASE_RETURN((true));
			}
			if (canHeal(plot()))
            {

#ifdef _DEBUG
                OutputDebugString("AI_heal: healing\n");
#endif
//                 AI_logAct("heal", "healInField", plot());
                pGroup->pushMission(MISSION_HEAL);
                NAVAL_DIAG_PHASE_RETURN((true));
            }
		}

#ifdef _DEBUG
		OutputDebugString("AI_heal: denying heal\n");
#endif
	}

	iMaxPath = std::min(iMaxPath, 2);
	std::vector<CvUnit*> aeDamagedUnits;
	int iHurtUnitCount = 0;
	// The candidate vector is unused outside territory where splitting is
	// legal. Avoid per-unit heal-turn calculations there, without changing
	// the damaged count used to decide whether the group needs a refuge.
	const bool bCanClaimTerritory = !isHuman() && canClaimTerritory(plot());
	const bool bCanSplitForHealing = !isHuman() &&
		(bCanClaimTerritory || plot()->isCity() && plot()->getOwner() == getOwner());

	foreach_(CvUnit * unitX, pGroup->units())
	{
		if (unitX->getDamage() > 0)
		{
			iHurtUnitCount++;

			if (bCanSplitForHealing)
			{
				const int iThreshold = (
					unitX->getMaxHP() * iDamagePercent
					/
					(100 + 100 * (NO_UNIT != getLeaderUnitType()))
				);
				if (unitX->getDamage() > iThreshold
				&& !unitX->hasMoved() && !unitX->isAlwaysHeal()
				&& unitX->healTurns(unitX->plot()) <= iMaxPath)
				{
					aeDamagedUnits.push_back(unitX);
				}
			}
		}
	}
	if (iHurtUnitCount == 0)
	{
		NAVAL_DIAG_PHASE_RETURN((false));
	}

	if (!isHuman())
	{
		if (bCanSplitForHealing)
		{
			bool bPushedMission = false;

			for (unsigned int iI = 0; iI < aeDamagedUnits.size(); iI++)
			{
				CvUnit* pUnitToHeal = aeDamagedUnits[iI];
				pUnitToHeal->joinGroup(NULL);

				// note, removing the head unit from a group will force the group to be completely split if non-human
				if (pUnitToHeal == this)
				{
					pGroup = getGroup();
					bPushedMission = true;
// 					AI_logAct("heal", bCanClaimTerritory ? "claimAndHeal" : "healSplit", plot());

					if (bCanClaimTerritory)
					{
						pGroup->pushMission(MISSION_CLAIM_TERRITORY, -1, -1, 0, false, false, MISSIONAI_CLAIM_TERRITORY, plot());
						pGroup->pushMission(MISSION_HEAL, -1, -1, 0, true);
					}
					else pGroup->pushMission(MISSION_HEAL);
				}
				else pUnitToHeal->getGroup()->pushMission(MISSION_HEAL);

				iHurtUnitCount--;
			}
			if (bPushedMission)
			{
				NAVAL_DIAG_PHASE_RETURN((true));
			}
		}
	}

	// A fleet already in a friendly port must not leave merely because its
	// wounded ships are outnumbered by the unrelated land garrison below.
	// Outside-port recovery and emergency escape keep their existing policy.
	if (iHurtUnitCount > 0 && !isHuman() && getDomainType() == DOMAIN_SEA
	&& plot()->isCity(true) && plot()->getTeam() == getTeam()
	&& healRate(plot()) > 0 && pGroup->canStartMission(MISSION_HEAL, -1, -1))
	{
		AI_rememberNavalCommitment(NavalCommitment::REPAIR, plot());
		pGroup->pushMission(MISSION_HEAL);
		NAVAL_DIAG_PHASE_RETURN((true));
	}

	if (iHurtUnitCount * 2 >= plot()->getNumDefenders(getOwner()))
	{
		if (AI_healingCity())
		{
			NAVAL_DIAG_PHASE_RETURN((true));
		}
		const int iHealRateHere = healRate(plot());
		if (iHealRateHere > 10)
		{
			pGroup->pushMission(MISSION_HEAL);
			NAVAL_DIAG_PHASE_RETURN((true));
		}
		// Stuck somewhere we literally cannot heal at all (not friendly territory,
		//	no self-heal/battlefield medicine, no healer nearby) - head for one of
		//	our own cities instead of just parking here undamaged-but-unhealed.
		if (!isHuman() && 0 == iHealRateHere && AI_retreatToCity())
		{
			NAVAL_DIAG_PHASE_RETURN((true));
		}
		if (AI_healingSafety(iMaxPath))
		{
			NAVAL_DIAG_PHASE_RETURN((true));
		}
	}
	NAVAL_DIAG_PHASE_RETURN((false));
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_afterAttack()
{
	if (!isMadeAttack() || !canFight() || isBlitz())
	{
		return false;
	}
	// Can fight, but has used up its attack for this turn.

	if (getDomainType() == DOMAIN_LAND && AI_guardCity(false, true, 1))
	{
		return true;
	}

	if (AI_RbombardPlot(getDCMBombRange(), 60))
	{
		return true;
	}

	if (AI_pillageRange(1))
	{
		return true;
	}

	if (AI_retreatToCity(false, false, 1))
	{
		return true;
	}

	if (AI_hide())
	{
		return true;
	}

	if (AI_goody(1))
	{
		return true;
	}

	if (AI_RbombardPlot(getDCMBombRange(), 0))
	{
		return true;
	}

	if (AI_pillageRange(2))
	{
		return true;
	}

	if (AI_defend())
	{
		return true;
	}

	if (AI_safety())
	{
		return true;
	}

	return false;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_goldenAge()
{
	if (canGoldenAge())
	{
		getGroup()->pushMission(MISSION_GOLDEN_AGE);
		return true;
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_spreadReligion()
{
	PROFILE_FUNC();

	const CvPlayerAI& owner = GET_PLAYER(getOwner());
	const ReligionTypes eStateReligion = owner.getStateReligion();

	// More often than not, it is the state religion the AI spreads.
	ReligionTypes eReligion =
		(
			(eStateReligion != NO_RELIGION && m_pUnitInfo->getReligionSpreads(eStateReligion) > 0)
			?
			eStateReligion
			:
			NO_RELIGION
		);
	if (eReligion == NO_RELIGION)
	{
		for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			if (m_pUnitInfo->getReligionSpreads((ReligionTypes)iI) > 0)
			{
				eReligion = (ReligionTypes)iI;
				break;
			}
		}
	}
	if (eReligion == NO_RELIGION)
	{
		return false;
	}

	if (m_cachedPlayer != getOwner()
	|| m_cachedMissionaryPlotset == NULL
	|| m_cachedMissionaryPlotset->find(plot()) == m_cachedMissionaryPlotset->end())
	{
		SAFE_DELETE(m_cachedMissionaryPlotset);

		m_cachedMissionaryPlotset = new CvReachablePlotSet(getGroup(), MOVE_NO_ENEMY_TERRITORY, MAX_INT, false);

		m_cachedPlayer = getOwner();
	}

	int iBaseFlags = isHuman() ? MOVE_NO_ENEMY_TERRITORY : (MOVE_NO_ENEMY_TERRITORY | MOVE_IGNORE_DANGER);
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestSpreadPlot = NULL;

	std::multimap<int, CvCity*> targetValues;
	CvReachablePlotSet::const_iterator itr = m_cachedMissionaryPlotset->begin();

	// If we already has a chosen spread city targeted then start with a presumption we'll stick to it
	if (getGroup()->AI_getMissionAIType() == MISSIONAI_SPREAD)
	{
		PROFILE("CvUnitAI::AI_spreadReligion.AlreadyMissioned");

		CvPlot* pPlot = getGroup()->AI_getMissionAIPlot();

		CvCity* pLoopCity = pPlot->getPlotCity();

		//logBBAI("Missionary %d at (%d,%d) already targeting city at (%d,%d)\n", getID(), getX(), getY(), pPlot->getX(), pPlot->getY());

		if (pLoopCity != NULL
		&& m_cachedMissionaryPlotset->find(pLoopCity->plot()) != m_cachedMissionaryPlotset->end()
		&& generatePath(pLoopCity->plot(), iBaseFlags, true))
		{
			// If human, prefer to spread to the player where automated from.
			const bool bForceMove = isHuman() && plot()->getOwner() == pLoopCity->getOwner() && pLoopCity->isRevealed(getTeam(), false);

			pBestPlot = bForceMove ? pLoopCity->plot() : getPathEndTurnPlot();
			pBestSpreadPlot = pLoopCity->plot();

			// Check end of turn point is not in immediate danger
			if (!bForceMove && owner.AI_getVisiblePlotDanger(pBestPlot, 1, false))
			{
				//logBBAI("Danger at end turn plot - looking for alternate route...");

				// Try to find an alternate route taking danger into account in the pathing calculation
				if (generatePath(pLoopCity->plot(), MOVE_NO_ENEMY_TERRITORY, true))
				{
					//logBBAI("...found one");
					pBestPlot = getPathEndTurnPlot();

					iBaseFlags = MOVE_NO_ENEMY_TERRITORY;
				}
				else pBestSpreadPlot = NULL; // No way past
			}
		}
	}

	if (pBestSpreadPlot == NULL)
	{
		const bool bCultureVictory = owner.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2);
		const bool bHasHolyCity = GET_TEAM(getTeam()).hasHolyCity(eReligion);

		// BBAI TODO: Could also use CvPlayerAI::AI_missionaryValue to determine which player to target ...
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (!GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				continue;
			}
			int iPlayerMultiplierPercent = 0;

			if (GET_PLAYER((PlayerTypes)iI).getTeam() != getTeam() && canEnterTerritory(GET_PLAYER((PlayerTypes)iI).getTeam()))
			{
				if (bHasHolyCity)
				{
					iPlayerMultiplierPercent = 100;
					// BBAI TODO: If going for cultural victory, don't spread to other teams? Sure, this might decrease the chance of
					// someone else winning by culture, but at the cost of $$ in holy city and diplomatic conversions (ie future wars!).
					// Doesn't seem to up our odds of winning by culture really.  Also, no foreign spread after Free Religion?  Still get
					// gold for city count.
					if (!bCultureVictory || eReligion == eStateReligion)
					{
						if (GET_PLAYER((PlayerTypes)iI).getStateReligion() == NO_RELIGION)
						{
							if (1 > GET_PLAYER((PlayerTypes)iI).getNonStateReligionHappiness())
							{
								iPlayerMultiplierPercent += 600;
							}
						}
						else if (GET_PLAYER((PlayerTypes)iI).getStateReligion() == eReligion)
						{
							iPlayerMultiplierPercent += 300;
						}
						else if (GET_PLAYER((PlayerTypes)iI).hasHolyCity(GET_PLAYER((PlayerTypes)iI).getStateReligion()))
						{
							iPlayerMultiplierPercent += 50;
						}
						else iPlayerMultiplierPercent += 300;

						const int iCityCount = owner.getNumCities();
						//magic formula to produce normalized adjustment factor based on religious infusion
						int iAdjustment = 100 * (iCityCount + 1);
						iAdjustment /= iCityCount + 1 + GET_PLAYER((PlayerTypes)iI).countTotalHasReligion();
						iAdjustment = (iAdjustment - 25) * 4 / 3;

						iAdjustment = std::max(10, iAdjustment);

						iPlayerMultiplierPercent *= iAdjustment;
						iPlayerMultiplierPercent /= 100;
					}
				}
			}
			else if (iI == getOwner())
			{
				iPlayerMultiplierPercent = 100;
			}
			else if (bHasHolyCity && GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
			{
				iPlayerMultiplierPercent = 80;
			}
			else continue;

			if (iPlayerMultiplierPercent > 0)
			{
				foreach_(CvCity * cityX, GET_PLAYER((PlayerTypes)iI).cities())
				{
					if (AI_plotValid(cityX->plot())
					&& (itr = m_cachedMissionaryPlotset->find(cityX->plot())) != m_cachedMissionaryPlotset->end()
					&& canSpread(cityX->plot(), eReligion)
					&& !owner.AI_getAnyPlotDanger(cityX->plot())
					&& owner.AI_plotTargetMissionAIs(cityX->plot(), MISSIONAI_SPREAD, getGroup()) == 0)
					{
						int iValue = 7 + 4 * cityX->getPopulation();

						bool bOurCity = false;
						// BBAI TODO: Why not just use iPlayerMultiplier??
						if (cityX->getOwner() == getOwner())
						{
							iValue *= bCultureVictory ? 16 : 4;
							bOurCity = true;
						}
						else if (cityX->getTeam() == getTeam())
						{
							iValue *= 3;
							bOurCity = true;
						}
						else
						{
							iValue *= iPlayerMultiplierPercent;
							iValue /= 100;
						}

						const int iCityReligionCount = cityX->getReligionCount();

						if (!bOurCity)
						{
							// not our city, so prefer the lowest number of religions
							if (iCityReligionCount > 1)
							{
								iValue /= iCityReligionCount;
							}
							else if (iCityReligionCount == 0)
							{
								iValue *= 2; // absolutely prefer cities with zero religions
							}
						}
						else if (iCityReligionCount == 0)
						{
							iValue /= 2; // count cities with no religion the same as cities with 2 religions
						}
						else if (iCityReligionCount == 1)
						{
							iValue *= 2; // prefer a city with exactly 1 religion already
						}
						else iValue /= iCityReligionCount;

						//If human, prefer to spread to the player where automated from.
						if (isHuman() && plot()->getOwner() == cityX->getOwner())
						{
							iValue *= 5;
						}
						iValue *= 1000;

						targetValues.insert(std::pair<int, CvCity*>(-iValue / (1 + itr.stepDistance()), cityX));
					}
				}
			}
		}

		// Koshling - evaluate in likely best-first order to try to minimize pathing calculations
		for (std::multimap<int, CvCity*>::const_iterator itr = targetValues.begin(); itr != targetValues.end(); ++itr)
		{
			const CvCity* pLoopCity = itr->second;

			if (generatePath(pLoopCity->plot(), iBaseFlags, true))
			{
				// Reconstruct bForceMove value
				const bool bForceMove = isHuman() && plot()->getOwner() == pLoopCity->getOwner() && pLoopCity->isRevealed(getTeam(), false);

				pBestPlot = bForceMove ? pLoopCity->plot() : getPathEndTurnPlot();

				pBestSpreadPlot = pLoopCity->plot();
				//logBBAI("Potential target city at (%d,%d)", pBestSpreadPlot->getX(), pBestSpreadPlot->getY());

				// Check end of turn point is not in immediate danger
				if (!bForceMove && owner.AI_getVisiblePlotDanger(pBestPlot, 1, false))
				{
					//logBBAI("Danger at end turn plot - looking for alternate route...");

					// Try to find an alternate route taking danger into account in the pathing calculation
					if (generatePath(pLoopCity->plot(), MOVE_NO_ENEMY_TERRITORY, true))
					{
						pBestPlot = getPathEndTurnPlot();
						iBaseFlags = MOVE_NO_ENEMY_TERRITORY;
					}
					else continue; // No way past
				}
				// Ordering is very close to optimal in almost all conceivable cases
				break; // so just take the first that works
			}
		}
	}

	if (pBestPlot != NULL && pBestSpreadPlot != NULL)
	{
		//logBBAI("Missionary %d at (%d,%d) targeting spread in city at (%d,%d)\n", getID(), getX(), getY(), pBestSpreadPlot->getX(), pBestSpreadPlot->getY());
		if (atPlot(pBestSpreadPlot))
		{
			getGroup()->pushMission(MISSION_SPREAD, eReligion);
			return true;
		}
		FAssert(!atPlot(pBestPlot));
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), iBaseFlags, false, false, MISSIONAI_SPREAD, pBestSpreadPlot);
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_spreadCorporation()
{
	PROFILE_FUNC();

	CorporationTypes eCorporation = NO_CORPORATION;
	int iBaseFlags = (isHuman() ? MOVE_NO_ENEMY_TERRITORY : (MOVE_NO_ENEMY_TERRITORY | MOVE_IGNORE_DANGER));

	for (int iI = 0; iI < GC.getNumCorporationInfos(); ++iI)
	{
		if (m_pUnitInfo->getCorporationSpreads((CorporationTypes)iI) > 0)
		{
			eCorporation = ((CorporationTypes)iI);
			break;
		}
	}

	if (NO_CORPORATION == eCorporation)
	{
		return false;
	}

	bool bHasHQ = (GET_TEAM(getTeam()).hasHeadquarters(eCorporation));

	CvPlot* pBestPlot = NULL;
	CvPlot* pBestSpreadPlot = NULL;

	if (m_cachedPlayer != getOwner() ||
		 m_cachedMissionaryPlotset == NULL ||
		 m_cachedMissionaryPlotset->find(plot()) == m_cachedMissionaryPlotset->end())
	{
		SAFE_DELETE(m_cachedMissionaryPlotset);

		m_cachedMissionaryPlotset = new CvReachablePlotSet(getGroup(), MOVE_NO_ENEMY_TERRITORY, MAX_INT, false);

		m_cachedPlayer = getOwner();
	}

	//CvReachablePlotSet plotSet(getGroup(), MOVE_NO_ENEMY_TERRITORY, MAX_INT);
	std::multimap<int, CvCity*>	targetValues;
	CvReachablePlotSet::const_iterator itr = m_cachedMissionaryPlotset->begin();

	//	If we already has a chosen spread city targeted then start with a presumption
	//	we'll stick to it
	if (getGroup()->AI_getMissionAIType() == MISSIONAI_SPREAD_CORPORATION)
	{
		PROFILE("CvUnitAI::AI_spreadReligion.AlreadyMissioned");

		CvPlot* pPlot = getGroup()->AI_getMissionAIPlot();

		const CvCity* pLoopCity = pPlot->getPlotCity();

		//OutputDebugString(CvString::format("Missionary at (%d,%d) already targeting city at (%d,%d)\n", getX(), getY(), pPlot->getX(), pPlot->getY()).c_str());

		if (pLoopCity != NULL &&
			 m_cachedMissionaryPlotset->find(pLoopCity->plot()) != m_cachedMissionaryPlotset->end() &&
			 generatePath(pLoopCity->plot(), iBaseFlags, true))
		{
			bool bForceMove = false;
			if (isHuman())
			{
				//If human, prefer to spread to the player where automated from.
				if (plot()->getOwner() == pLoopCity->getOwner())
				{
					if (pLoopCity->isRevealed(getTeam(), false))
					{
						bForceMove = true;
					}
				}
			}

			pBestPlot = bForceMove ? pLoopCity->plot() : getPathEndTurnPlot();
			pBestSpreadPlot = pLoopCity->plot();

			//	Check end of turn point is not in immediate danger
			if (!bForceMove && GET_PLAYER(getOwner()).AI_getVisiblePlotDanger(pBestPlot, 1, false))
			{
				//logBBAI("Danger at end turn plot - looking for alternate route...");
				//	Try to find an alternate route taking danger into account in the
				//	apthing calculation
				if (generatePath(pLoopCity->plot(),
					MOVE_NO_ENEMY_TERRITORY,
					true))
				{
					//logBBAI("...found one");
					pBestPlot = getPathEndTurnPlot();

					iBaseFlags = MOVE_NO_ENEMY_TERRITORY;
				}
				else
				{
					//	No way past
					//logBBAI("...none found!");
					pBestSpreadPlot = NULL;
				}
			}
		}
	}

	if (pBestSpreadPlot == NULL)
	{
		const CvTeam& kTeam = GET_TEAM(getTeam());
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
			if (kLoopPlayer.isAlive() && ((bHasHQ && canEnterTerritory(GET_PLAYER((PlayerTypes)iI).getTeam())) || (getTeam() == kLoopPlayer.getTeam())))
			{
				const int iLoopPlayerCorpCount = kLoopPlayer.countCorporations(eCorporation);
				const CvTeam& kLoopTeam = GET_TEAM(kLoopPlayer.getTeam());
				foreach_(CvCity * pLoopCity, kLoopPlayer.cities())
				{
					if (AI_plotValid(pLoopCity->plot()) && (itr = m_cachedMissionaryPlotset->find(pLoopCity->plot())) != m_cachedMissionaryPlotset->end())
					{
						// BBAI efficiency: check same area
						if ( /*pLoopCity->area() == area() &&*/ canSpreadCorporation(pLoopCity->plot(), eCorporation))
						{
							if (!pLoopCity->plot()->isVisibleEnemyUnit(this))
							{
								if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_SPREAD_CORPORATION, getGroup()) == 0)
								{
									// BBAI TODO: Serious need for more intelligent self spread, keep certain corps from
									// enemies based on their victory pursuits (culture ...)
									int iValue = (10 + pLoopCity->getPopulation() * 2);

									if (pLoopCity->getOwner() == getOwner())
									{
										iValue *= 4;
									}
									else if (kLoopTeam.isVassal(getTeam()))
									{
										iValue *= 3;
									}
									else if (kTeam.isVassal(kLoopTeam.getID()))
									{
										if (iLoopPlayerCorpCount == 0)
										{
											iValue *= 10;
										}
										else
										{
											iValue *= 3;
											iValue /= 2;
										}
									}
									else if (pLoopCity->getTeam() == getTeam())
									{
										iValue *= 2;
									}

									if (iLoopPlayerCorpCount == 0)
									{
										//Generally prefer to heavily target one player
										iValue /= 2;
									}

									if (isHuman())
									{
										//If human, prefer to spread to the player where automated from.
										if (plot()->getOwner() == pLoopCity->getOwner())
										{
											iValue *= 10;
										}
									}

									iValue *= 1000;

									targetValues.insert(std::pair<int, CvCity*>(-iValue / (1 + itr.stepDistance()), pLoopCity));
								}
							}
						}
					}
				}
			}
		}

		// Koshling - evaluate in likely best-first order to try to minimize pathing calculations

		for (std::multimap<int, CvCity*>::const_iterator itr = targetValues.begin(); itr != targetValues.end(); ++itr)
		{
			const CvCity* pLoopCity = itr->second;

			if (generatePath(pLoopCity->plot(),
				iBaseFlags,
				true))
			{
				bool bForceMove = false;
				if (isHuman())
				{
					// Reconstruct bForceMove value
					if (plot()->getOwner() == pLoopCity->getOwner())
					{
						if (pLoopCity->isRevealed(getTeam(), false))
						{
							bForceMove = true;
						}
					}
				}

				pBestPlot = bForceMove ? pLoopCity->plot() : getPathEndTurnPlot();
				pBestSpreadPlot = pLoopCity->plot();

				//	Check end of turn point is not in immediate danger
				if (!bForceMove && GET_PLAYER(getOwner()).AI_getVisiblePlotDanger(pBestPlot, 1, false))
				{
					//logBBAI("Danger at end turn plot - looking for alternate route...");
					//	Try to find an alternate route taking danger into account in the
					//	apthing calculation
					if (generatePath(pLoopCity->plot(),
						MOVE_NO_ENEMY_TERRITORY,
						true))
					{
						//logBBAI("...found one");
						pBestPlot = getPathEndTurnPlot();

						iBaseFlags = MOVE_NO_ENEMY_TERRITORY;
					}
					else
					{
						//	No way past
						//logBBAI("...none found!");
						continue;
					}
				}

				//	Ordering is very close to optimal in almost all conceivable cases
				//	so just take the first that works
				break;
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestSpreadPlot != NULL))
	{
		if (atPlot(pBestSpreadPlot))
		{
			if (canSpreadCorporation(pBestSpreadPlot, eCorporation))
			{
				getGroup()->pushMission(MISSION_SPREAD_CORPORATION, eCorporation);
			}
			else
			{
				getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_SPREAD_CORPORATION, pBestSpreadPlot);
			}
			return true;
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD					  03/09/09								jdog5000	  */
			/*																							  */
			/* Unit AI																					  */
			/************************************************************************************************/
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), iBaseFlags, false, false, MISSIONAI_SPREAD_CORPORATION, pBestSpreadPlot);
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD					   END												  */
			/************************************************************************************************/
		}
	}

	return false;
}

bool CvUnitAI::AI_spreadReligionAirlift()
{
	PROFILE_FUNC();

	int iValue;
	int iI;

	if (getGroup()->getNumUnits() > 1)
	{
		return false;
	}

	const CvCity* pCity = plot()->getPlotCity();

	if (pCity == NULL)
	{
		return false;
	}

	if (pCity->getMaxAirlift() == 0)
	{
		return false;
	}

	//bool bCultureVictory = GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_CULTURE2);
	ReligionTypes eReligion = NO_RELIGION;

	if (eReligion == NO_RELIGION)
	{
		if (GET_PLAYER(getOwner()).getStateReligion() != NO_RELIGION)
		{
			if (m_pUnitInfo->getReligionSpreads(GET_PLAYER(getOwner()).getStateReligion()) > 0)
			{
				eReligion = GET_PLAYER(getOwner()).getStateReligion();
			}
		}
	}

	if (eReligion == NO_RELIGION)
	{
		for (iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			//if (bCultureVictory || GET_TEAM(getTeam()).hasHolyCity((ReligionTypes)iI))
			{
				if (m_pUnitInfo->getReligionSpreads((ReligionTypes)iI) > 0)
				{
					eReligion = ((ReligionTypes)iI);
					break;
				}
			}
		}
	}

	if (eReligion == NO_RELIGION)
	{
		return false;
	}

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getTeam()))
		{
			foreach_(const CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
			{
				if (canAirliftAt(pCity->plot(), pLoopCity->getX(), pLoopCity->getY()))
				{
					if (canSpread(pLoopCity->plot(), eReligion))
					{
						if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_SPREAD, getGroup()) == 0)
						{
							// Don't airlift where there's already one of our unit types (probably just airlifted)
							if (pLoopCity->plot()->plotCount(PUF_isUnitType, getUnitType(), -1, NULL, getOwner()) > 0)
							{
								continue;
							}
							iValue = (7 + (pLoopCity->getPopulation() * 4));

							int iCityReligionCount = pLoopCity->getReligionCount();
							int iReligionCountFactor = iCityReligionCount;

							// count cities with no religion the same as cities with 2 religions
							// prefer a city with exactly 1 religion already
							if (iCityReligionCount == 0)
							{
								iReligionCountFactor = 2;
							}
							else if (iCityReligionCount == 1)
							{
								iValue *= 2;
							}

							iValue /= iReligionCountFactor;
							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = pLoopCity->plot();
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_AIRLIFT, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_SPREAD, pBestPlot);
		return true;
	}

	return false;
}

bool CvUnitAI::AI_spreadCorporationAirlift()
{
	PROFILE_FUNC();

	if (getGroup()->getNumUnits() > 1)
	{
		return false;
	}

	CvCity* pCity = plot()->getPlotCity();

	if (pCity == NULL)
	{
		return false;
	}

	if (pCity->getMaxAirlift() == 0)
	{
		return false;
	}

	CorporationTypes eCorporation = NO_CORPORATION;

	for (int iI = 0; iI < GC.getNumCorporationInfos(); ++iI)
	{
		if (m_pUnitInfo->getCorporationSpreads((CorporationTypes)iI) > 0)
		{
			eCorporation = ((CorporationTypes)iI);
			break;
		}
	}

	if (NO_CORPORATION == eCorporation)
	{
		return false;
	}

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getTeam()))
		{
			foreach_(const CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
			{
				if (canAirliftAt(pCity->plot(), pLoopCity->getX(), pLoopCity->getY()))
				{
					if (canSpreadCorporation(pLoopCity->plot(), eCorporation))
					{
						if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_SPREAD_CORPORATION, getGroup()) == 0)
						{
							// Don't airlift where there's already one of our unit types (probably just airlifted)
							if (pLoopCity->plot()->plotCount(PUF_isUnitType, getUnitType(), -1, NULL, getOwner()) > 0)
							{
								continue;
							}

							int iValue = (pLoopCity->getPopulation() * 4);

							if (pLoopCity->getOwner() == getOwner())
							{
								iValue *= 4;
							}
							else
							{
								iValue *= 3;
							}

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = pLoopCity->plot();
							}
						}
					}
				}
			}
		}
	}
	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_AIRLIFT, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_SPREAD, pBestPlot);
		return true;
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_discover(const bool bFirstResearchOnly)
{
	if (!canDiscover())
	{
		return false;
	}
	const TechTypes eTech = getDiscoveryTech();
	const bool bIsFirstTech = GET_PLAYER(getOwner()).AI_isFirstTech(eTech);

	if (bFirstResearchOnly && !bIsFirstTech)
	{
		return false;
	}
	const int iPercentWasted = 100 * GET_TEAM(getTeam()).getResearchProgress(eTech) / GET_TEAM(getTeam()).getResearchCost(eTech);

	FAssert(iPercentWasted >= 0 && iPercentWasted <= 100);

	if (bIsFirstTech)
	{
		if (getDiscoverResearch(eTech) >= GET_TEAM(getTeam()).getResearchLeft(eTech))
		{
			if (iPercentWasted <= 30 || bFirstResearchOnly && iPercentWasted <= 50)
			{
				return discover(eTech);
			}
		}
		else if (bFirstResearchOnly)
		{
			return false;
		}
	}
	// Unit cannot finish the tech this turn, so why not speed it up some?
	if (getDiscoverResearch(eTech) <= GET_TEAM(getTeam()).getResearchLeft(eTech))
	{
		return discover(eTech);
	}
	// Unit can finish the tech this turn.

	if (GET_PLAYER(getOwner()).getResearchTurnsLeft(eTech, true)
	<= std::max(1, GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 100))
	{
		return false;
	}
	// Takes some time to invent, allow some wastage.
	if (iPercentWasted <= 5 || iPercentWasted <= 15 && GET_PLAYER(getOwner()).getCurrentResearch() == eTech)
	{
		return discover(eTech);
	}
	return false;
}


namespace {
	// Helper function to determine if a unit looks legendaryish
	bool isLegendary(const CvUnit* unit)
	{
		const CvUnitInfo& unitInfo = GC.getUnitInfo(unit->getUnitType());
		return (unitInfo.getMaxGlobalInstances() > 0 && unitInfo.getMaxGlobalInstances() <= 3)
			|| (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS) && unit->qualityRank() > 8);
	}
}

bool CvUnitAI::AI_leadLegend()
{
	PROFILE_FUNC();

	FAssertMsg(!isHuman(), "AI_leadLegend shouldn't be called for human players");
	FAssert(NO_PLAYER != getOwner());

	const CvPlayer& kOwner = GET_PLAYER(getOwner());
	CvUnit* pBestUnit = NULL;
	CvPlot* pBestPlot = NULL;
	int iBestStrength = 0;
	foreach_(CvUnit * pLoopUnit, kOwner.units())
	{
		if (isLegendary(pLoopUnit)
		&& canLead(pLoopUnit->plot(), pLoopUnit->getID()) > 0
		&& AI_plotValid(pLoopUnit->plot())
		&& !pLoopUnit->plot()->isVisibleEnemyUnit(this)
		&& pLoopUnit->combatLimit() >= 100
		&& generatePath(pLoopUnit->plot(), MOVE_AVOID_ENEMY_WEIGHT_3, true))
		{
			// pick the unit with the highest current strength
			int iCombatStrength = pLoopUnit->currCombatStr(NULL, NULL);
			iCombatStrength *= 10 + (pLoopUnit->getExperience() * 2);
			iCombatStrength /= 15;

			iCombatStrength *= 10 - GC.getUnitInfo(pLoopUnit->getUnitType()).getMaxGlobalInstances();
			iCombatStrength /= 3;

			if (iCombatStrength > iBestStrength)
			{
				iBestStrength = iCombatStrength;
				pBestUnit = pLoopUnit;
				// generatePath called just above actually updates our own current path, so here we save the end turn plot of that path so we can use it
				// as a move target
				pBestPlot = getPathEndTurnPlot();
			}
		}
	}

	if (pBestUnit && pBestPlot)
	{
		if (atPlot(pBestPlot))
		{
			if (gUnitLogLevel > 2)
			{
				CvWString szString;
				getUnitAIString(szString, pBestUnit->AI_getUnitAIType());

				logBBAI("	  Great general %d for %S chooses to lead %S Legend Unit", getID(), GET_PLAYER(getOwner()).getCivilizationDescription(0), pBestUnit->getName(0).GetCString());
			}

			getGroup()->pushMission(MISSION_LEAD, pBestUnit->getID());

			return true;
		}
		else
		{
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_WEIGHT_3);
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_lead(std::vector<UnitAITypes>& aeUnitAITypes)
{
	PROFILE_FUNC();

	FAssertMsg(!isHuman(), "isHuman did not return false as expected");
	FAssertMsg(AI_getUnitAIType() != NO_UNITAI, "AI_getUnitAIType() is not expected to be equal with NO_UNITAI");
	FAssert(NO_PLAYER != getOwner());

	const CvPlayer& kOwner = GET_PLAYER(getOwner());

	bool bNeedLeader = false;
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD & RevDCM					 09/03/10						jdog5000	  */
	/*																				phungus420	*/
	/* Great People AI, Unit AI																	 */
	/************************************************************************************************/
	bool bBestUnitLegend = false;

	CvUnit* pBestUnit = NULL;
	CvPlot* pBestPlot = NULL;

	// AI may use Warlords to create super-medic units
	CvUnit* pBestStrUnit = NULL;
	CvPlot* pBestStrPlot = NULL;

	CvUnit* pBestHealUnit = NULL;
	CvPlot* pBestHealPlot = NULL;

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (isEnemy((TeamTypes)iI))
		{
			if (GET_TEAM((TeamTypes)iI).countNumUnitsByArea(area()) > 0)
			{
				bNeedLeader = true;
				break;
			}
		}
	}

	if (bNeedLeader)
	{
		int iBestStrength = 0;
		int iBestHealing = 0;
		int iCombatStrength;

		foreach_(CvUnit * pLoopUnit, kOwner.units())
		{
			bool bValid = false;
			bool bLegend = false;

			if (GC.getUnitInfo(pLoopUnit->getUnitType()).getMaxGlobalInstances() > 0
			&& GC.getUnitInfo(pLoopUnit->getUnitType()).getMaxGlobalInstances() < 7)
			{
				if (canLead(pLoopUnit->plot(), pLoopUnit->getID()) > 0)
				{
					bValid = true;
					bLegend = true;
				}
			}

			if (!bValid)
			{
				for (uint iI = 0; iI < aeUnitAITypes.size(); iI++)
				{
					if (pLoopUnit->AI_getUnitAIType() == aeUnitAITypes[iI] || NO_UNITAI == aeUnitAITypes[iI])
					{
						if (canLead(pLoopUnit->plot(), pLoopUnit->getID()) > 0)
						{
							bValid = true;
							break;
						}
					}
				}
			}

			if (bValid)
			{
				if (AI_plotValid(pLoopUnit->plot()))
				{
					if (!pLoopUnit->plot()->isVisibleEnemyUnit(this))
					{
						if (pLoopUnit->combatLimit() >= 100)
						{
							if (generatePath(pLoopUnit->plot(), MOVE_AVOID_ENEMY_WEIGHT_3, true))
							{
								// pick the unit with the highest current strength
								iCombatStrength = pLoopUnit->currCombatStr(NULL, NULL);
								iCombatStrength *= 10 + (pLoopUnit->getExperience() * 2);
								iCombatStrength /= 15;

								if (bLegend)
								{
									iCombatStrength *= 10 - GC.getUnitInfo(pLoopUnit->getUnitType()).getMaxGlobalInstances();
									iCombatStrength /= 3;
								}

								if (iCombatStrength > iBestStrength)
								{
									iBestStrength = iCombatStrength;
									pBestStrUnit = pLoopUnit;
									pBestStrPlot = getPathEndTurnPlot();
									if (bLegend)
									{
										bBestUnitLegend = true;
									}
									else
									{
										bBestUnitLegend = false;
									}
								}

								// or the unit with the best healing ability
								int iHealing = pLoopUnit->getSameTileHeal() + (pLoopUnit->getHealUnitCombatCount() / 2) + pLoopUnit->getAdjacentTileHeal();
								if (iHealing > iBestHealing)
								{
									iBestHealing = iHealing;
									pBestHealUnit = pLoopUnit;
									pBestHealPlot = getPathEndTurnPlot();
								}
							}
						}
					}
				}
			}
		}
	}

	if (AI_getBirthmark() % 3 == 0 && pBestHealUnit != NULL)
	{
		pBestPlot = pBestHealPlot;
		pBestUnit = pBestHealUnit;
	}
	else
	{
		pBestPlot = pBestStrPlot;
		pBestUnit = pBestStrUnit;
	}

	if (pBestPlot)
	{
		if (atPlot(pBestPlot) && pBestUnit)
		{
			if (gUnitLogLevel > 2)
			{
				const CvWString szStringUnitAi = GC.getUnitAIInfo(pBestUnit->AI_getUnitAIType()).getType();

				if (bBestUnitLegend)
				{
					logBBAI("	  Great general %d for %S chooses to lead %S Legend Unit", getID(), GET_PLAYER(getOwner()).getCivilizationDescription(0), pBestUnit->getName(0).GetCString());
				}
				else
				{
					logBBAI("	  Great general %d for %S chooses to lead %S with UNITAI %S", getID(), GET_PLAYER(getOwner()).getCivilizationDescription(0), pBestUnit->getName(0).GetCString(), szStringUnitAi.GetCString());
				}
			}
			getGroup()->pushMission(MISSION_LEAD, pBestUnit->getID());
			return true;
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_WEIGHT_3);
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	return false;
}

// Returns true if a mission was pushed...
// iMaxCounts = 1 would mean join a city if there's no existing joined GP of that type.
bool CvUnitAI::AI_join(int iMaxCount)
{
	PROFILE_FUNC();

	if (GET_PLAYER(getOwner()).getNumCities() == 0)
	{
		return false;
	}

	if (iMaxCount && GC.getGame().getSorenRandNum(11, "Settle GG") < iMaxCount + 5)
	{
		return false;
	}
	const CvPlot* pBestPlot = NULL;
	SpecialistTypes eBestSpecialist = NO_SPECIALIST;

	{
		const int iThreatTreshold = (
			2
			*
			std::max(1, GET_PLAYER(getOwner()).AI_getTotalAreaCityThreat(area(), NULL))
			/
			GET_PLAYER(getOwner()).getNumCities()
		);
		int iBestValue = 0;

		foreach_(CvCity * cityX, GET_PLAYER(getOwner()).cities())
		{
			// BBAI efficiency: check same area
			if (UnitTravelConnectivity::landUnitCanReach(this,cityX->plot())
			&& AI_plotValid(cityX->plot())
			&& generateIgnoreDangerPath(cityX->plot()))
			{
				for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
				{
					const SpecialistTypes eType = static_cast<SpecialistTypes>(iI);

					if (m_pUnitInfo->getGreatPeoples(eType))
					{
						const int iCount = cityX->getSpecialistCount(eType);
						if (iCount >= iMaxCount)
						{
							continue;
						}
					}

					if (canJoin(cityX->plot(), eType)
					// Replaced local danger blocking of joins with more nuanced threat level evaluation.
					//	This reduces the chances of the join being blocked by local dnager counts from battles with animals and so forth
					&& cityX->AI_cityThreat() < iThreatTreshold)
					{
						const int iValue = cityX->AI_specialistValue(eType, cityX->AI_avoidGrowth(), false);
						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = cityX->plot();
							eBestSpecialist = eType;
						}
					}
				}
			}
		}
	}

	if (pBestPlot && eBestSpecialist != NO_SPECIALIST)
	{
		if (atPlot(pBestPlot))
		{
			getGroup()->pushMission(MISSION_JOIN, eBestSpecialist);
			return true;
		}

		if (generateSafePathforVulnerable(pBestPlot))
		{
			const CvPlot* plotX = getPathEndTurnPlot();
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, plotX->getX(), plotX->getY(), MOVE_WITH_CAUTION);
		}
		getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
		return true;
	}
	return false;
}

/*
CvUnitAI* CvUnitAI::AI_cityConstructionTargeted(const CvCity* pCity, BuildingTypes eBuilding, const CvSelectionGroup* omitGroup) const
{
	PROFILE_FUNC();

	foreach_(const CvSelectionGroup* pLoopSelectionGroup, GET_PLAYER(getOwner()).groups())
	{
		if ( pLoopSelectionGroup != omitGroup &&
			 pLoopSelectionGroup->AI_getMissionAIPlot() == pCity->plot() &&
			 pLoopSelectionGroup->AI_getMissionAIType() == MISSIONAI_CONSTRUCT )
		{
			CvUnitAI* targetingUnit = (CvUnitAI*)pLoopSelectionGroup->getHeadUnit();

			//	Have to handle the case of a group with no units.  This can occur after a foprce group
			//	split due to things like revokation of open borders that forces units to be unserimoniously
			//	moved to another plot (and ungroups them in doing so since it is possible that only a subset
			//	of a group has to move)
			if ( targetingUnit != NULL && targetingUnit->m_eIntendedConstructBuilding == eBuilding )
			{
				return targetingUnit;
			}
		}
	}

	return NULL;
}
*/

//	Should we disband this unit?
bool CvUnitAI::AI_scrapSubdued()
{
	PROFILE_FUNC();

	if (GET_PLAYER(getOwner()).AI_isFinancialTrouble())
	{
		//	Always scrap subdued animals if we're in financial trouble and they were unable to
		//	find construction targets (which is implied by having called this)
		scrap();
		return true;
	}

	//	Count how many units of this type we have that couldn't find construction missions (by
	//	implication of getting here no further units of this type could)
	int	iSurplass = 0;
	//	Hold a suplasss of each type of up to 2 (inside our borders) or 1 (outside), boosted by 1 if we have less then 4 cities
	int	iExtra = (plot()->getOwner() == getOwner() ? 2 : 1) + (GET_PLAYER(getOwner()).getNumCities() < 4 ? 1 : 0);

	foreach_(const CvUnit * pLoopUnit, GET_PLAYER(getOwner()).units())
	{
		if (pLoopUnit->getUnitType() == getUnitType() && pLoopUnit->getGroup()->AI_getMissionAIType() != MISSIONAI_CONSTRUCT)
		{
			iSurplass++;
		}
	}

	if (iSurplass > iExtra)
	{
		scrap();
		return true;
	}

	return false;
}

bool CvUnitAI::AI_moveToOurTerritory(int maxMoves)
{
	PROFILE_EXTRA_FUNC();
	const int iSearchRange = AI_searchRange(maxMoves);

	foreach_(const CvPlot * pLoopPlot, plot()->rect(iSearchRange, iSearchRange))
	{
		if (pLoopPlot->area() == plot()->area())
		{
			if (pLoopPlot->getOwner() == getOwner())
			{
				int iTurns;

				if (generatePath(pLoopPlot, MOVE_NO_ENEMY_TERRITORY, true, &iTurns, maxMoves))
				{
					const CvPlot* endTurnPlot = getPathEndTurnPlot();

					if (!exposedToDanger(endTurnPlot, 75))
					{
						return getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY(), MOVE_NO_ENEMY_TERRITORY);
					}
				}
			}
		}
	}

	return false;
}

// To cope with units like heroes, that are constructed under various unitAIs,
//	but which can construct buildings as an option,
//	this rouine determines whether a switch to doing so is appropriate, and if so performs it.
bool CvUnitAI::checkSwitchToConstruct()
{
	if (m_pUnitInfo->getNumBuildings() < 1 && m_pUnitInfo->getNumHeritage() < 1
	// Don't bail on a poorly defended city.
	|| !plot()->getPlotCity() && plot()->getNumDefenders(getOwner()) < 2)
	{
		return false;
	}
	const CvPlayerAI& player = GET_PLAYER(getOwner());

	// If this is a bit out-dated as a military unit consider constructive retirement.
	CvCity* pCapitalCity = GET_PLAYER(getOwner()).getCapitalCity();

	if (pCapitalCity)
	{
		const UnitTypes eBestUnit = GET_PLAYER(getOwner()).bestBuildableUnitForAIType(getDomainType(), AI_getUnitAIType());
		if (eBestUnit != NO_UNIT)
		{
			const int iMilitaryValue = GET_PLAYER(getOwner()).AI_unitValue(m_eUnitType, AI_getUnitAIType(), area());
			if (GET_PLAYER(getOwner()).AI_unitValue(eBestUnit, AI_getUnitAIType(), pCapitalCity->area()) > iMilitaryValue * 3 / 2)
			{
				if (AI_heritage() || AI_construct() || AI_join())
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool CvUnitAI::enactConstruct(CvPlot* pBestConstructPlot, CvPlot* pBestPlot, CvUnitAI* eBestTargetingUnit, BuildingTypes eBestBuilding)
{
	if (pBestPlot == NULL || pBestConstructPlot == NULL || eBestBuilding == NO_BUILDING)
	{
		return false;
	}
	GET_PLAYER(getOwner()).AI_changeNumBuildingsNeeded(eBestBuilding, -1);

	if (atPlot(pBestConstructPlot))
	{
		getGroup()->pushMission(MISSION_CONSTRUCT, eBestBuilding);
		return true;
	}
	FAssert(!atPlot(pBestPlot));

	//	Take over responsibility from any overridden targeting unit
	if (eBestTargetingUnit != NULL)
	{
		eBestTargetingUnit->m_eIntendedConstructBuilding = NO_BUILDING;
	}
	m_eIntendedConstructBuilding = eBestBuilding;

	// If we have to move outside our own territory heal first
	if (getDamage() > 0 && pBestPlot->getOwner() != getOwner())
	{
		// May not avoid healing on a bad plot though since it's not going through AI_heal() - watch for this
		getGroup()->pushMission(MISSION_HEAL, -1, -1, 0, false, false, NO_MISSIONAI, pBestConstructPlot);
		return true;
	}
	return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_NO_ENEMY_TERRITORY | MOVE_WITH_CAUTION | MOVE_AVOID_ENEMY_UNITS, false, false, MISSIONAI_CONSTRUCT, pBestConstructPlot);
}


namespace {
	// Helper class for recording what buildings are already planned for construction by units
	struct CityConstruct
	{
		CityConstruct(CvPlot* plot, BuildingTypes building) : plot(plot), building(building) {}

		CvPlot* plot;
		BuildingTypes building;

		operator size_t() const
		{
			return Cv::hash_combine(reinterpret_cast<size_t>(plot), static_cast<size_t>(building));
		}

		bool operator==(const CityConstruct& other) const
		{
			return plot == other.plot && building == other.building;
		}
	};
	typedef stdext::hash_map<CityConstruct, CvUnitAI*> CityConstructMap;

	// Get a map of buildings that units are planning on constructing (so we can ignore them when giving new construct orders)
	CityConstructMap getConstructBuildings(const CvPlayer& player, const CvSelectionGroup* ignoreGroup)
	{
		CityConstructMap constructions;
		foreach_(const CvSelectionGroup * pLoopSelectionGroup, player.groups())
		{
			if (pLoopSelectionGroup != ignoreGroup && pLoopSelectionGroup->AI_getMissionAIType() == MISSIONAI_CONSTRUCT)
			{
				CvUnitAI* targetingUnit = (CvUnitAI*)pLoopSelectionGroup->getHeadUnit();

				//	Have to handle the case of a group with no units.  This can occur after a force group
				//	split due to things like revocation of open borders that forces units to be unceremoniously
				//	moved to another plot (and ungroups them in doing so since it is possible that only a subset
				//	of a group has to move)
				if (targetingUnit != NULL)
				{
					constructions[CityConstruct(pLoopSelectionGroup->AI_getMissionAIPlot(), targetingUnit->getIntendedConstructBuilding())] = targetingUnit;
				}
			}
		}
		return constructions;
	}

	int getDecayedConstructProbability(int base, const int rate, const int iterations)
	{
		for (int i = 0; i < iterations; ++i)
		{
			base = base * rate / 100;
		}
		return base;
	}
}

namespace { bool propertyControllerConstructsLocally(const CvUnit* unit); }

int CvUnitAI::getBestConstructValue(int iMaxCount, int iMaxSingleBuildingCount, int iDecayProbabilityRate, int iThreshold, bool assumeSameValueEverywhere, CvPlot*& pBestConstructPlot, CvPlot*& pBestPlot, CvUnitAI*& pBestTargetingUnit, BuildingTypes& eBestBuilding, bool doctrinesOnly)
{
	PROFILE_FUNC();

	const int iContructRand = iDecayProbabilityRate == 0 ? 100 : GC.getGame().getSorenRandNum(100, "AI construction probability");
	const int iConstructBelow = 100;

	// Set outputs to default values
	pBestConstructPlot = nullptr;
	pBestPlot = nullptr;
	pBestTargetingUnit = nullptr;
	eBestBuilding = NO_BUILDING;

	int iBestValue = 0;

	typedef stdext::hash_map<BuildingTypes, int> PossibleBuildingsMap;
	PossibleBuildingsMap possibleBuildings;

	const CvPlayerAI& player = GET_PLAYER(getOwner());
	// Determine the list of building types we could make, based on what already exists
	for (int iI = 0; iI < m_pUnitInfo->getNumBuildings(); iI++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(m_pUnitInfo->getBuildings(iI));
		if (doctrinesOnly && !DoctrinePromotions::isFoundation(eBuilding)) continue;
		if (eBuilding != NO_BUILDING
		&& player.canConstruct(eBuilding, false, false, true)
		&& player.AI_getNumBuildingsNeeded(eBuilding, getDomainType() == DOMAIN_SEA) > 0)
		{
			const int numBuildings = algo::count_if(player.cities(), CvCity::fn::hasBuilding(eBuilding));
			const bool canBuild = numBuildings < iMaxCount &&
				(
					iDecayProbabilityRate == 0 || getDecayedConstructProbability(iConstructBelow, iDecayProbabilityRate, numBuildings) >= iContructRand
				);

			if (canBuild)
			{
				const int doctrineValue = DoctrinePromotions::isFoundation(eBuilding) ? DoctrinePromotions::foundingValue(*this, eBuilding) : -1;
				if (doctrineValue == 0) continue;
				possibleBuildings[eBuilding] = doctrineValue > 0 ? doctrineValue : 0;
			}
		}
	}

	if (possibleBuildings.size() == 0)
	{
		return 0;
	}

	const bool propertyController = AI_getUnitAIType() == UNITAI_PROPERTY_CONTROL || AI_getUnitAIType() == UNITAI_PROPERTY_CONTROL_SEA;
	const bool localConstructionOnly = propertyController && propertyControllerConstructsLocally(this);
	const int maxConstructionPathTurns = AI_getUnitAIType() == UNITAI_SUBDUED_ANIMAL
		? SUBDUED_ANIMAL_MAX_TRAVEL_TURNS : MAX_INT;
	CityConstructMap constructions;
	bool constructionTargetsReady = false;

	int iBestWeightedValue = 0;

	foreach_(CvCity * pLoopCity, player.cities())
	{
		// Preserve useful construction on site while an established controller
		// is needed here. Other roles and surplus controllers keep all targets.
		if (localConstructionOnly && pLoopCity->plot() != plot()) continue;
		if (pLoopCity->plot()->isVisibleEnemyUnit(this)) // Koshling - this line is questionable
			continue;
		// Every accepted target still needs the exact legal route below. A
		// preliminary whole-map flood duplicates it, even for idle animals with
		// no legal building destination. Reuse one exact route per city; a
		// reservation's MOVE_OUR_TERRITORY query may overwrite the group path.
		bool cityPathReady = false;
		bool cityHasPath = false;
		int cityPathLimit = -1;
		int cityPathTurns = 0;
		CvPlot* cityEndTurnPlot = NULL;

		for (PossibleBuildingsMap::iterator itr = possibleBuildings.begin(); itr != possibleBuildings.end(); ++itr)
		{
			const BuildingTypes buildingType = itr->first;
			const int doctrineValue = DoctrinePromotions::isFoundation(buildingType) ? itr->second : 0;

			// Animals reuse one value per building under the existing
			// same-value policy. Even zero travel must beat the incumbent
			// before paying legality/reservation/path costs for another city.
			int maxWinningTurns = maxConstructionPathTurns;
			if (assumeSameValueEverywhere && itr->second > 0)
			{
				if (itr->second <= iThreshold) continue;
				maxWinningTurns = std::min(maxConstructionPathTurns,
					constructionWinningPathLimit(itr->second, pLoopCity->getPopulation(), iBestWeightedValue));
				if (maxWinningTurns < 0) continue;
			}

			if (player.getBuildingCount(buildingType) >= iMaxSingleBuildingCount)
				continue;
			if (!canConstruct(pLoopCity->plot(), buildingType))
				continue;
			if (!constructionTargetsReady)
			{
				CityConstructMap targets = getConstructBuildings(player, getGroup());
				constructions.swap(targets);
				constructionTargetsReady = true;
			}

			// Check some other unit hasn't already got this city targeted to construct this building
			CityConstructMap::const_iterator foundUnit = constructions.find(CityConstruct(pLoopCity->plot(), buildingType));
			CvUnitAI* targetingUnit = foundUnit == constructions.end() ? NULL : foundUnit->second;
			// AI_cityConstructionTargeted(pLoopCity, buildingType, getGroup());

			const bool betterThanCurrentUnit =
				!targetingUnit ||
				(
					targetingUnit->plot()->getOwner() != getOwner()
					&& plot()->getOwner() == getOwner()
					&& generatePath(pLoopCity->plot(), MOVE_OUR_TERRITORY, true)
				);

			// If we're a better choice due to being already inside our own territory
			// TODO:	How about we just use straight line distance instead?
			//			Although it would probably be rare for targetingUnit to not be null so this check we be
			//			done infrequently.
			if (betterThanCurrentUnit)
			{
				// With per-city values the final rank ignores travel time. A value
				// that cannot win is not worth an exact route (or a map-wide flood).
				int propertyValue = 0;
				if (propertyController && !assumeSameValueEverywhere)
				{
					propertyValue = pLoopCity->AI_buildingValue(buildingType);
					if (propertyValue <= iThreshold || propertyValue <= iBestWeightedValue) continue;
				}

				// A non-positive local value cannot beat the zero incumbent.
				// Do not path there merely to discover that afterwards. Do not
				// cache it: another city may value this building positively.
				int sameValue = 0;
				if (assumeSameValueEverywhere)
				{
					sameValue = itr->second > 0 ? itr->second : pLoopCity->AI_buildingValue(buildingType);
					if (sameValue <= 0) continue;
				}

				PROFILE("CvUnitAI::getBestConstructValue.Pathing");

				if (!cityPathReady || (!cityHasPath && maxWinningTurns > cityPathLimit))
				{
					cityHasPath = generatePath(pLoopCity->plot(), MOVE_NO_ENEMY_TERRITORY, true, &cityPathTurns, maxWinningTurns);
					cityPathLimit = maxWinningTurns;
					cityPathReady = true;
					if (cityHasPath) cityEndTurnPlot = getPathEndTurnPlot();
				}
				if (cityHasPath)
				{
					//	When evaluating whether a hero unit should instead go and produce a building
					//	we want to avoid expensive calculations for every city, when in practise the kinds
					//	of things heroes can build have global effects and thus basically the same value
					//	everywhere that they have any value (note that this is a heuristic assumption that is true CURRENTLY,
					//	but may need to be revisited)
					//	Koshling - also extended this to subdued animals for performance reasons - generally for all cities
					//	and animal buiding CAN be built in this will be a good approximation
					int iValue = 0;
					if (assumeSameValueEverywhere && itr->second > 0)
					{
						iValue = itr->second;
					}
					else
					{
						iValue = doctrineValue > 0 ? doctrineValue : (assumeSameValueEverywhere ? sameValue : (propertyController ? propertyValue : pLoopCity->AI_buildingValue(buildingType)));

						itr->second = iValue;
					}

					//	Weight by ease of reaching and population slightly in the case of same-everywhere asserted
					//	builds
					int iWeightedValue = (iValue * 5) / (4 + cityPathTurns);

					if (assumeSameValueEverywhere)
					{
						iWeightedValue = (iWeightedValue * (5 + pLoopCity->getPopulation())) / 10;
					}

					if (iValue > iThreshold && iWeightedValue > iBestWeightedValue)
					{
						iBestValue = iValue;
						iBestWeightedValue = iWeightedValue;
						pBestPlot = cityEndTurnPlot;
						pBestConstructPlot = pLoopCity->plot();
						eBestBuilding = buildingType;
						pBestTargetingUnit = targetingUnit;
					}
				}
			}
		}
	}

	return iBestValue;
}

// Returns true if a mission was pushed...
// iMaxCount = 1 would mean construct only if there are no existing buildings
//   constructed by this GP type.
bool CvUnitAI::AI_construct(int iMaxCount, int iMaxSingleBuildingCount, int iThreshold, bool bDecayProbabilities, bool assumeSameValueEverywhere, bool doctrinesOnly)
{
	AI_PERF_SCOPE(UNIT_CONSTRUCT);
	PROFILE_FUNC();

	if (m_pUnitInfo->getNumBuildings() < 1)
	{
		return false;
	}
	CvPlot* pBestPlot;
	CvPlot* pBestConstructPlot;
	CvUnitAI* eBestTargetingUnit;
	BuildingTypes eBestBuilding;

	if (getBestConstructValue(iMaxCount, iMaxSingleBuildingCount, bDecayProbabilities ? 50 : 0, iThreshold, assumeSameValueEverywhere, pBestConstructPlot, pBestPlot, eBestTargetingUnit, eBestBuilding, doctrinesOnly) > 0)
	{
		if (gUnitLogLevel >= 2)
		{
			logBBAI("	%S at (%d,%d) going to construct %S at (%d,%d)", getName(0).GetCString(), getX(), getY(), GC.getBuildingInfo(eBestBuilding).getDescription(), pBestConstructPlot->getX(), pBestConstructPlot->getY());
		}
		return enactConstruct(pBestConstructPlot, pBestPlot, eBestTargetingUnit, eBestBuilding);
	}
	return false;
}


namespace {
	// Helper class for recording what heritages are already planned by units
	struct CityHeritage
	{
		CityHeritage(CvPlot* plot, HeritageTypes eType) : plot(plot), eType(eType) {}

		CvPlot* plot;
		HeritageTypes eType;

		operator size_t() const
		{
			return Cv::hash_combine(reinterpret_cast<size_t>(plot), static_cast<size_t>(eType));
		}

		bool operator==(const CityHeritage& other) const
		{
			return plot == other.plot && eType == other.eType;
		}
	};
	typedef stdext::hash_map<CityHeritage, CvUnitAI*> CityHeritageMap;

	// Get a map of buildings that units are planning on constructing (so we can ignore them when giving new construct orders)
	CityHeritageMap getHeritageMissionMap(const CvPlayer& player, const CvSelectionGroup* ignoreGroup)
	{
		CityHeritageMap heritageMap;
		foreach_(const CvSelectionGroup * groupX, player.groups())
		{
			if (groupX != ignoreGroup && groupX->AI_getMissionAIType() == MISSIONAI_CONSTRUCT)
			{
				CvUnitAI* targetingUnit = (CvUnitAI*)groupX->getHeadUnit();

				// Have to handle the case of a group with no units.  This can occur after a force group
				//	split due to things like revocation of open borders that forces units to be unceremoniously
				//	moved to another plot (and ungroups them in doing so since it is possible that only a subset
				//	of a group has to move)
				if (targetingUnit)
				{
					heritageMap[CityHeritage(groupX->AI_getMissionAIPlot(), targetingUnit->getIntendedHeritage())] = targetingUnit;
				}
			}
		}
		return heritageMap;
	}
}

int CvUnitAI::getBestHeritageValue(CvPlot*& pBestConstructPlot, CvPlot*& pBestPlot, CvUnitAI*& pTargetingUnit, HeritageTypes& eBest)
{
	PROFILE_FUNC();

	// Set outputs to default values
	pBestConstructPlot = nullptr;
	pBestPlot = nullptr;
	pTargetingUnit = nullptr;
	eBest = NO_HERITAGE;

	typedef stdext::hash_map<HeritageTypes, int> PossibleHeritageMap;
	PossibleHeritageMap possibleHeritage;

	const CvPlayerAI& player = GET_PLAYER(getOwner());
	// Determine the list of building types we could make, based on what already exists
	for (int iI = 0; iI < m_pUnitInfo->getNumHeritage(); iI++)
	{
		const HeritageTypes eTypeX = static_cast<HeritageTypes>(m_pUnitInfo->getHeritage(iI));
		if (player.canAddHeritage(eTypeX))
		{
			possibleHeritage[eTypeX] = 0;
		}
	}

	if (possibleHeritage.size() == 0)
	{
		return 0;
	}
	int iBestValue = 0;

	const CityHeritageMap heritageMap = getHeritageMissionMap(player, getGroup());
	const CvReachablePlotSet plotSet(getGroup(), MOVE_NO_ENEMY_TERRITORY, MAX_INT);

	int iBestWeightedValue = 0;

	foreach_(CvCity * cityX, player.cities())
	{
		if (plotSet.find(cityX->plot()) == plotSet.end())
			continue;

		for (PossibleHeritageMap::iterator itr = possibleHeritage.begin(); itr != possibleHeritage.end(); ++itr)
		{
			const HeritageTypes eTypeX = itr->first;

			// Check some other unit hasn't already got this city targeted to construct this building
			CityHeritageMap::const_iterator foundUnit = heritageMap.find(CityHeritage(cityX->plot(), eTypeX));
			CvUnitAI* targetingUnit = foundUnit == heritageMap.end() ? NULL : foundUnit->second;

			const bool bBetterThanCurrentUnit = (
					!targetingUnit
				|| !targetingUnit->generatePath(cityX->plot(), MOVE_NO_ENEMY_TERRITORY, true)
				|| targetingUnit->plot()->getOwner() != getOwner()
				&& plot()->getOwner() == getOwner()
			);

			// If we're a better choice due to being already inside our own territory
			// TODO:	How about we just use straight line distance instead?
			//			Although it would probably be rare for targetingUnit to not be null so this check we be
			//			done infrequently.
			if (bBetterThanCurrentUnit && canAddHeritage(cityX->plot(), eTypeX))
			{
				PROFILE("CvUnitAI::getBestConstructValue.Pathing");

				int iPathTurns;
				if (generatePath(cityX->plot(), MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns))
				{
					//	When evaluating whether a hero unit should instead go and produce a building
					//	we want to avoid expensive calculations for every city, when in practise the kinds
					//	of things heroes can build have global effects and thus basically the same value
					//	everywhere that they have any value (note that this is a heuristic assumption that is true CURRENTLY,
					//	but may need to be revisited)
					//	Koshling - also extended this to subdued animals for performance reasons - generally for all cities
					//	and animal buiding CAN be built in this will be a good approximation
					int iValue = 0;
					if (itr->second > 0)
					{
						iValue = itr->second;
					}
					else
					{
						iValue = player.AI_heritageValue(eTypeX);

						itr->second = iValue;
					}

					// Weight by ease of reaching and population slightly in the case of same-everywhere asserted builds
					const int iWeightedValue = iValue * 10 / (4 + iPathTurns);

					if (iWeightedValue > iBestWeightedValue)
					{
						iBestValue = iValue;
						iBestWeightedValue = iWeightedValue;
						pBestPlot = getPathEndTurnPlot();
						pBestConstructPlot = cityX->plot();
						eBest = eTypeX;
						pTargetingUnit = targetingUnit;
					}
				}
			}
		}
	}
	return iBestValue;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_heritage()
{
	AI_PERF_SCOPE(UNIT_CONSTRUCT);
	PROFILE_FUNC();

	if (m_pUnitInfo->getNumHeritage() < 1)
	{
		return false;
	}
	CvPlot* pBestPlot;
	CvPlot* pBestConstructPlot;
	CvUnitAI* pTargetingUnit;
	HeritageTypes eBest;

	if (getBestHeritageValue(pBestConstructPlot, pBestPlot, pTargetingUnit, eBest) > 0)
	{
		if (!pBestPlot || !pBestConstructPlot || eBest == NO_HERITAGE)
		{
			return false;
		}

		if (gUnitLogLevel >= 2)
		{
			logBBAI("	%S at (%d,%d) going to establish %S at (%d,%d)", getName(0).GetCString(), getX(), getY(), GC.getHeritageInfo(eBest).getDescription(), pBestConstructPlot->getX(), pBestConstructPlot->getY());
		}

		if (atPlot(pBestConstructPlot))
		{
			getGroup()->pushMission(MISSION_HERITAGE, eBest);
			return true;
		}
		FAssert(!atPlot(pBestPlot));

		//	Take over responsibility from any overridden targeting unit
		if (pTargetingUnit)
		{
			pTargetingUnit->m_eIntendedHeritage = NO_HERITAGE;
		}
		m_eIntendedHeritage = eBest;

		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_NO_ENEMY_TERRITORY | MOVE_WITH_CAUTION | MOVE_AVOID_ENEMY_UNITS, false, false, MISSIONAI_CONSTRUCT, pBestConstructPlot);
	}
	return false;
}

// Returns true if a mission was pushed...
// Try to get the best outcome mission done
bool CvUnitAI::AI_outcomeMission()
{
	AI_PERF_SCOPE(UNIT_OUTCOME);
	PROFILE_FUNC();

	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestMissionPlot = NULL;
	MissionTypes eBestMission = NO_MISSION;
	int iBestValue = 0;
	const bool localAnimal = AI_getUnitAIType() == UNITAI_SUBDUED_ANIMAL;
	int bestAnimalTurns = MAX_INT;
	const int cityPassCount = localAnimal && plot()->getPlotCity() ? 2 : 1;
	// Read-only decision scope: fixed group/flags, at most three turns.
	// Limits only tighten, so a failed earlier route cannot need a retry.
	// Several actions can share a city; retain its end plot, not group state.
	typedef std::vector<std::pair<const CvPlot*, std::pair<int, CvPlot*> > > AnimalUsePathMap;
	AnimalUsePathMap animalUsePaths;

	// favor the closest city
	CvCity* pClosestCity = plot()->getPlotCity();
	if (!pClosestCity)
		pClosestCity = GC.getMap().findCity(getX(), getY(), getOwner(), NO_TEAM, true, false);

	std::vector<std::pair<MissionTypes, const CvOutcomeList*> > aMissions;

	const CvUnitInfo& kInfo = getUnitInfo();

	for (int iI = 0; iI < kInfo.getNumActionOutcomes(); iI++)
	{
		aMissions.push_back(std::make_pair(kInfo.getActionOutcomeMission(iI), kInfo.getActionOutcomeList(iI)));
	}

	// check the unit combat types for outcome missions
	for (std::map<UnitCombatTypes, UnitCombatKeyedInfo>::const_iterator it = m_unitCombatKeyedInfo.begin(), end = m_unitCombatKeyedInfo.end(); it != end; ++it)
	{
		if (it->second.m_bHasUnitCombat)
		{
			const CvUnitCombatInfo& kCombatInfo = GC.getUnitCombatInfo(it->first);
			for (int iI = 0; iI < kCombatInfo.getNumActionOutcomes(); iI++)
			{
				aMissions.push_back(std::make_pair(kCombatInfo.getActionOutcomeMission(iI), kCombatInfo.getActionOutcomeList(iI)));
			}
		}
	}

	CvReachablePlotSet plotSet(getGroup(), MOVE_NO_ENEMY_TERRITORY, -1);
	bool reachabilityReady = false;

	// Compare every legal on-site action before searching any remote city.
	for (int cityPass = 0; cityPass < cityPassCount; ++cityPass)
	{
		for (std::vector<std::pair<MissionTypes, const CvOutcomeList*> >::iterator it = aMissions.begin(); it != aMissions.end(); ++it)
		{
			const MissionTypes eMission = it->first;
			const CvOutcomeList* pOutcomeList = it->second;

			if (eMission != NO_MISSION)
			{
				if (pOutcomeList->isPossibleSomewhere(*this))
				{
					if (pClosestCity)
					{
						foreach_(const CvCity * pLoopCity, GET_PLAYER(getOwner()).cities())
						{
							if (cityPassCount == 2 && ((cityPass == 0) != atPlot(pLoopCity->plot()))) continue;
							// Once an on-site use exists, remote uses cannot be nearer.
							if (localAnimal && pBestMissionPlot == plot() && !atPlot(pLoopCity->plot())) continue;
							if (pOutcomeList->isPossibleInPlot(*this, *(pLoopCity->plot())))
							{
								const int iValue = pOutcomeList->AI_getValueInPlot(*this, *(pLoopCity->plot()));
								if (iValue > 0 && (localAnimal || iValue > iBestValue))
								{
									// Do not flood the map for unavailable or worthless
									// outcomes. The origin is always seeded by Populate;
									// remote contenders retain the original reachability
									// filter and exact route before any mission is issued.
									if (!localAnimal && !atPlot(pLoopCity->plot()))
									{
										if (!reachabilityReady)
										{
											plotSet.Populate(MAX_INT);
											reachabilityReady = true;
										}
										if (plotSet.find(pLoopCity->plot()) == plotSet.end()) continue;
									}
									int pathTurns = 0;
									const bool onSite = atPlot(pLoopCity->plot());
									// A bounded exact route replaces the empire-wide flood only
									// for animals. Other outcome users retain their old policy.
									CvPlot* outcomeEnd = NULL;
									bool hasPath;
									if (localAnimal)
									{
										AnimalUsePathMap::iterator cached = animalUsePaths.begin();
										while (cached != animalUsePaths.end() && cached->first != pLoopCity->plot()) ++cached;
										if (cached == animalUsePaths.end())
										{
											hasPath = onSite || generatePath(pLoopCity->plot(), MOVE_NO_ENEMY_TERRITORY, true, &pathTurns, std::min(static_cast<int>(SUBDUED_ANIMAL_MAX_TRAVEL_TURNS), bestAnimalTurns));
											outcomeEnd = hasPath ? (onSite ? plot() : getPathEndTurnPlot()) : NULL;
											if (!onSite && (!outcomeEnd || outcomeEnd == plot())) hasPath = false;
											animalUsePaths.push_back(std::make_pair(pLoopCity->plot(), std::make_pair(hasPath ? pathTurns : -1, outcomeEnd)));
											cached = animalUsePaths.end();
											--cached;
										}
										pathTurns = cached->second.first;
										outcomeEnd = cached->second.second;
										hasPath = pathTurns >= 0;
									}
									else
									{
										hasPath = generatePath(pLoopCity->plot(), MOVE_NO_ENEMY_TERRITORY, true);
										if (hasPath) outcomeEnd = getPathEndTurnPlot();
									}
									if (hasPath && (!localAnimal || preferLocalAnimalOutcome(iValue, pathTurns, onSite, iBestValue, bestAnimalTurns, pBestMissionPlot == plot())))
									{
										iBestValue = iValue;
										bestAnimalTurns = pathTurns;
										pBestPlot = outcomeEnd;
										pBestMissionPlot = pLoopCity->plot();
										eBestMission = eMission;
									}
									if (plotSet.find(pLoopCity->plot()) == plotSet.end()) continue;
								}
								if (generatePath(pLoopCity->plot(), MOVE_NO_ENEMY_TERRITORY, true))
								{
									iBestValue = iValue;
									pBestPlot = getPathEndTurnPlot();
									pBestMissionPlot = pLoopCity->plot();
									eBestMission = eMission;
								}
							}
						}
					}
					else
					{
						// There is no city in this area, so try to execute an outcome mission at the current location
						if (pOutcomeList->isPossibleInPlot(*this, *(plot())))
						{
							const int iValue = pOutcomeList->AI_getValueInPlot(*this, *(plot()));
							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = localAnimal ? plot() : getPathEndTurnPlot();
								pBestMissionPlot = plot();
								eBestMission = eMission;
							}
						}
					}
				}
			}
		}
		if (localAnimal && pBestMissionPlot == plot()) break;
	}

	if ((pBestPlot != NULL) && (pBestMissionPlot != NULL) && (eBestMission != NO_MISSION))
	{
		if (gUnitLogLevel >= 2)
		{
			logBBAI("	%S at (%d,%d) going to enact mission %S at (%d,%d)", getName(0).GetCString(), getX(), getY(), GC.getMissionInfo(eBestMission).getDescription(), pBestMissionPlot->getX(), pBestMissionPlot->getY());
		}

		if (atPlot(pBestMissionPlot))
		{
			if (getGroup()->pushMissionInternal(eBestMission))
			{
				return true;
			}
		}
		else
		{
			FAssert(!atPlot(pBestPlot));

			//	If we have to move outside our own territory heal first
			if (getDamage() > 0 && pBestPlot->getOwner() != getOwner())
			{
				if (getGroup()->pushMissionInternal(MISSION_HEAL, -1, -1, 0, false, false, NO_MISSIONAI, pBestMissionPlot))
				{
					return true;
				}
			}

			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD					  03/09/09								jdog5000	  */
			/*																							  */
			/* Unit AI																					  */
			/************************************************************************************************/
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_NO_ENEMY_TERRITORY | MOVE_WITH_CAUTION | MOVE_AVOID_ENEMY_UNITS, false, false, NO_MISSIONAI, pBestMissionPlot);
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD					   END												  */
			/************************************************************************************************/
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_switchHurry()
{
	PROFILE_FUNC();

	CvCity* pCity = plot()->getPlotCity();

	if ((pCity == NULL) || (pCity->getOwner() != getOwner()))
	{
		return false;
	}

	int iBestValue = 0;
	BuildingTypes eBestBuilding = NO_BUILDING;

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iI);

		if (isWorldWonder(eBuilding))
		{
			if (pCity->canConstruct(eBuilding))
			{
				if (pCity->getProgressOnBuilding(eBuilding) == 0)
				{
					if (getMaxHurryProduction(pCity) >= pCity->getProductionNeeded(eBuilding))
					{
						const int iValue = pCity->AI_buildingValue(eBuilding);

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestBuilding = eBuilding;
						}
					}
				}
			}
		}
	}

	if (eBestBuilding != NO_BUILDING)
	{
		pCity->pushOrder(ORDER_CONSTRUCT, eBestBuilding, -1, false, false, false);

		if (pCity->getProductionBuilding() == eBestBuilding)
		{
			if (canHurry(plot()))
			{
				getGroup()->pushMission(MISSION_HURRY);
			}
			return true;
		}
		//This should always be true because just because the city can't hurry doesn't mean it hasn't successfully made its determination for this function.
		//Thus return true was moved outside of canHurry but kept within the check that ensures that the production building chosen is the next to be built confirmation.
		FErrorMsg("error");
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_hurry(bool bAny)
{
	PROFILE_FUNC();

	const CvPlayerAI& owner = GET_PLAYER(getOwner());
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvCity * cityX, owner.cities())
	{
		if (
			!UnitTravelConnectivity::landUnitCanReach(this,cityX->plot())
		|| !cityX->isProductionBuilding()
		|| !AI_plotValid(cityX->plot())
		|| !canHurry(cityX->plot())
		||
		   !bAny
		&& (
			owner.AI_plotTargetMissionAIs(cityX->plot(), MISSIONAI_HURRY, getGroup()) != 0
			||
			!isWorldWonder(cityX->getProductionBuilding())
			)
		)
		{
			continue;
		}

		int iPathTurns;
		if (generateIgnoreDangerPath(cityX->plot(), &iPathTurns))
		{
			const int iTurnsLeft = cityX->getProductionTurnsLeft() - iPathTurns;

			if (iTurnsLeft > (bAny ? 1 : 8))
			{
				const int iValue = iTurnsLeft;

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = cityX->plot();
				}
			}
		}
	}

	if (pBestPlot)
	{
		if (atPlot(pBestPlot))
		{
			getGroup()->pushMission(MISSION_HURRY);
			return true;
		}

		if (generateSafePathforVulnerable(pBestPlot))
		{
			const CvPlot* plotX = getPathEndTurnPlot();
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, plotX->getX(), plotX->getY(), MOVE_WITH_CAUTION, false, false, MISSIONAI_HURRY, pBestPlot);
		}
		getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
		return true;
	}
	return false;
}


namespace scoring {

	// Implementation details of the scoring
	namespace detail {
		// Traits class for items that can be scored (probably plots, cities, units, etc.)
		// It describes how to interact with the object (currently only how to get the plot the item is on)
		// The default implementation will work for CvUnit, CvCity, CvSelectionGroup at least
		template < class ItemTy_ >
		struct ItemTraits
		{
			typedef ItemTy_ item_value;

			static const CvPlot* plot(const ItemTy_* item) { return item->plot(); }
		};

		// Specialization of the traits for CvPlot as it can just return itself as its own plot!
		template <>
		struct ItemTraits<CvPlot>
		{
			typedef CvPlot item_value;

			static const CvPlot* plot(const CvPlot* item) { return item; }
		};
	}

	// A comparable container for an item along with its score
	template < class ItemTy_ >
	struct ItemScore
	{
		typedef detail::ItemTraits<ItemTy_> item_traits;

		ItemScore(ItemTy_* item = nullptr, int score = 0) : item(item), score(score) {}
		const CvPlot* plot() const { return item_traits::plot(item); }

		bool operator<(const ItemScore<ItemTy_>& other) const { return score < other.score; }
		bool operator>(const ItemScore<ItemTy_>& other) const { return score > other.score; }

		ItemTy_* item;
		int score;
	};

	struct GreatestScore {};
	struct LeastScore {};

	namespace detail {
		template < class ItemTy_ >
		bool validationFnUnwrap(bst::function<bool(const ItemTy_*)> validationFn, const ItemScore<ItemTy_>& itemScore)
		{
			return validationFn(itemScore.item);
		}

		template < class Ty_, class CompareTag_ >
		struct ScoringTraits {};

		template < class Ty_ >
		struct ScoringTraits<Ty_, GreatestScore>
		{
			typedef std::greater<Ty_> compare;
		};

		template < class Ty_ >
		struct ScoringTraits<Ty_, LeastScore>
		{
			typedef std::less<Ty_> compare;
		};
	}


	// Result of a findBestScore, containing a bool to indicate if any item was found along with the item and score
	template < class ItemTy_ >
	struct ScoreResult
	{
		ScoreResult(bool found, const ItemScore<ItemTy_>& result) : found(found), result(result) {}
		bool found;
		ItemScore<ItemTy_> result;
	};

	// Scores all items from begin to end using scoreFn, then selects the highest scoring item that passes the validationFn.
	// Template parameters:
	//   ItemTy_ : item type being scored against (e.g. CvCity, CvPlot, CvUnit, etc.)
	//   Compare_ : tag to indicate if we want bigger or smaller scores (GreatestScore or LeastScore)
	// Parameters:
	//   begin, end: Range of items to score, should dereference to ItemTy_*
	//   scoreFn: Function for scoring items. Signature: int score(const ItemTy_*)
	//   validationFn: Function for final item validation. After scoring the best scoring item that passes validation is returned. Signature: bool validate(const ItemTy_*)
	template < class ItemTy_, class Compare_, class Itr_>
	ScoreResult<ItemTy_> findBestScore(Itr_ begin, Itr_ end, bst::function<int(const ItemTy_*)> scoreFn, bst::function<bool(const ItemTy_*)> validationFn, bool positiveOnly = false)
	{
		using namespace detail;

		std::vector< ItemScore<ItemTy_> > scores;

		// Calculate scores
		for (Itr_ itr = begin; itr != end; ++itr)
		{
			const int score = scoreFn(*itr);
			if (!positiveOnly || score > 0)
				scores.push_back(ItemScore<ItemTy_>(*itr, score));
		}

		// Sort scores
		algo::sort(scores, ScoringTraits<ItemScore<ItemTy_>, Compare_>::compare());

		// Find the first city we can path to safely
		std::vector< ItemScore<ItemTy_> >::iterator foundItr = std::find_if(scores.begin(), scores.end(), bind(validationFnUnwrap<ItemTy_>, validationFn, _1));

		if (foundItr != scores.end())
		{
			return ScoreResult<ItemTy_>(true, *foundItr);
		}
		return ScoreResult<ItemTy_>(false, ItemScore<ItemTy_>());
	}

	// Validation function that will ensure a unit can reach an item
	bool canSafePathToCity(const CvUnitAI* unit, const CvCity* item)
	{
		FAssert(unit);
		FAssert(item);
		return unit->generateSafePathforVulnerable(item->plot());
	}

	// Match AI_healerMove's evacuation policy. Route caution alone does not
	// guarantee that a vulnerable support unit would remain at its destination.
	bool healerTargetSafe(const CvUnitAI* unit, const CvPlot* target)
	{
		return unit->atPlot(target) || !unit->exposedToDanger(target, 80);
	}

	bool canHealerPathToCity(const CvUnitAI* unit, const CvCity* city)
	{
		return healerTargetSafe(unit, city->plot()) && unit->generateSafePathforVulnerable(city->plot());
	}

	// Validation function that will ensure a unit can reach an item
	bool canPathToCity(const CvUnitAI* unit, const CvCity* item)
	{
		FAssert(unit != NULL);
		FAssert(item != NULL);
		return unit->generatePath(item->plot());
	}

	// Scoring helper function to attenuate score by distance, with optional boost for current plot
	int applyDistanceScoringFactor(int score, const CvPlot* sourcePlot, const CvPlot* targetPlot, const int currentPlotBoost)
	{
		FAssert(sourcePlot != NULL);
		FAssert(targetPlot != NULL);

		if (score > 0)
		{
			if (sourcePlot != targetPlot)
			{
				// Has to be > 0 because we aren't on the same plot...
				PlayerTypes Refplayer = sourcePlot->getOwner();
				int dist_break = 8;

				//if plot isn't owned, grab the first player (to get current era)
				if (Refplayer == NO_PLAYER)
				{
					for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
					{
						if (GET_PLAYER((PlayerTypes)iPlayer).isAlive())
						{
							Refplayer = (PlayerTypes)iPlayer;
							break;
						}
					}
				}
				dist_break += (int)GET_PLAYER(Refplayer).getCurrentEra();  //Calvitix TODO : dynamic, depending on map size ?
				const int dist = stepDistance(sourcePlot->getX(), sourcePlot->getY(), targetPlot->getX(), targetPlot->getY());
				//score = (int) (static_cast<float>(score) / sqrt(dist));// dist * dist;
				float d0 = 5.0f;
				float p = 0.4f;
				float expfact = 1.5f;
				if (dist <= dist_break)
				{
					score = static_cast<int>(score * pow(d0 / (d0 + dist), p));
				}
				else
				{
					float extra_dist = static_cast<float>(dist - dist_break);
					score = static_cast<int>(score * pow(d0 / (d0 + dist), p) * std::exp(-0.1 * pow(extra_dist, expfact)));
				}
			}
			else
			{
				score *= currentPlotBoost;
			}
		}
		return score;
	}
}

	namespace {
		// Returns lower score for more favorable target
		int scoreInquisitionTarget(const CvUnitAI* unit, const CvCity* targetCity)
		{
			FAssert(unit != NULL);
			FAssert(targetCity != NULL);

			int score = stepDistance(unit->plot()->getX(), unit->plot()->getY(), targetCity->plot()->getX(), targetCity->plot()->getY());

			const CvPlayer& player = GET_PLAYER(unit->getOwner());

			// Prefer NOT targeting our cities if we get bonuses from non-state religions
			if (targetCity->getOwner() == unit->getOwner())
			{
				if (player.isNonStateReligionCommerce())
				{
					score *= 2;
				}
			}
			// Prefer targeting our vassals
			else if (GET_PLAYER(targetCity->getOwner()).getTeam() != player.getTeam()
			&& GET_TEAM(GET_PLAYER(targetCity->getOwner()).getTeam()).isVassal(player.getTeam()))
			{
				score -= 12;
			}
			// Prefer NOT targeting cities where we have the holy city of some religions it has (we don't want to reduce our income from holy cities)
			const ReligionTypes eStateReligion = player.getStateReligion();

			for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
			{
				const ReligionTypes eType = static_cast<ReligionTypes>(iI);

				if (eType != eStateReligion && player.hasHolyCity(eType) && targetCity->isHasReligion(eType))
				{
					score += 13;
				}
			}
			return score;
		}

		// Return out preferred city target for religious activities
		CvCity* getReligiousVictoryTarget(const CvUnitAI* unit)
		{
			FAssert(unit != NULL);

			const CvPlayerAI& player = GET_PLAYER(unit->getOwner());
			if (!player.hasInquisitionTarget() || !player.isPushReligiousVictory() && !player.isConsiderReligiousVictory())
			{
				return NULL;
			}
			const TeamTypes eTeam = player.getTeam();
			const CvPlot* pUnitPlot = unit->plot();
			std::vector<CvCity*> viableCities;

			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				const CvPlayer& playerX = GET_PLAYER(static_cast<PlayerTypes>(iI));

				if (playerX.isAlive()
				// On my team or one of my vassals.
				&& (playerX.getTeam() == eTeam || GET_TEAM(playerX.getTeam()).isVassal(eTeam))
				&& pUnitPlot->isHasPathToPlayerCity(eTeam, static_cast<PlayerTypes>(iI)))
				{
					foreach_(CvCity * pLoopCity, playerX.cities())
					{
						if (pLoopCity->isInquisitionConditions())
						{
							viableCities.push_back(pLoopCity);
						}
					}
				}
			}

			using namespace scoring;
			ScoreResult<CvCity> bestCityScore =
				(
					findBestScore<CvCity, LeastScore>(
						player.beginCities(), player.endCities(),
						// scoring the city for inquisition attractiveness
						bind(scoreInquisitionTarget, unit, _1),
						// final validation is that we can actually path to the city
						bind(canSafePathToCity, unit, _1)
						)
				);
			return bestCityScore.found ? bestCityScore.result.item : nullptr;
		}
	}

bool CvUnitAI::AI_doInquisition()
{
	CvCity* pTargetCity = GET_PLAYER(getOwner()).getInquisitionRevoltCity(this, false, GC.getOC_MIN_REV_INDEX(), 0);
	if (!pTargetCity)
	{
		pTargetCity = GET_PLAYER(getOwner()).getTeamInquisitionRevoltCity(this, false, GC.getOC_MIN_REV_INDEX(), 0);
		if (!pTargetCity)
		{
			pTargetCity = getReligiousVictoryTarget(this);
		}
	}
	if (pTargetCity)
	{
		if (atPlot(pTargetCity->plot()))
		{
			return performInquisition();
		}
		if (generateSafePathforVulnerable(pTargetCity->plot()))
		{
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pTargetCity->getX(), pTargetCity->getY(), MOVE_WITH_CAUTION, false, false, MISSIONAI_INQUISITION, pTargetCity->plot());
		}
		return false;
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_greatWork()
{
	PROFILE_FUNC();

	CvPlot* pBestPlot = NULL;
	{
		int iBestValue = 0;
		foreach_(const CvCity * cityX, GET_PLAYER(getOwner()).cities())
		{
			if (UnitTravelConnectivity::landUnitCanReach(this,cityX->plot())
			&& AI_plotValid(cityX->plot())
			&& canGreatWork(cityX->plot())
			&& GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(cityX->plot(), MISSIONAI_GREAT_WORK, getGroup()) == 0
			&& generateIgnoreDangerPath(cityX->plot()))
			{
				const int iValue =
					(
						cityX->AI_calculateCulturePressure(true)
						-
						100 * cityX->getCulture(cityX->getOwner()) / std::max(1, getGreatWorkCulture())
					);
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = cityX->plot();
				}
			}
		}
	}

	if (pBestPlot)
	{
		if (atPlot(pBestPlot))
		{
			getGroup()->pushMission(MISSION_GREAT_WORK);
			return true;
		}

		if (generateSafePathforVulnerable(pBestPlot))
		{
			const CvPlot* plotX = getPathEndTurnPlot();
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, plotX->getX(), plotX->getY(), MOVE_WITH_CAUTION, false, false, MISSIONAI_GREAT_WORK, pBestPlot);
		}
		getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
		return true;
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_offensiveAirlift()
{
	PROFILE_FUNC();

	CvCity* pTargetCity;
	int iValue;

	if (getGroup()->getNumUnits() > 1)
	{
		return false;
	}

	if (area()->getTargetCity(getOwner()) != NULL)
	{
		return false;
	}

	const CvCity* pCity = plot()->getPlotCity();

	if (pCity == NULL)
	{
		return false;
	}

	if (pCity->getMaxAirlift() == 0)
	{
		return false;
	}

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvCity * pLoopCity, GET_PLAYER(getOwner()).cities())
	{
		if (pLoopCity->area() != pCity->area())
		{
			if (canAirliftAt(pCity->plot(), pLoopCity->getX(), pLoopCity->getY()))
			{
				pTargetCity = pLoopCity->area()->getTargetCity(getOwner());

				if (pTargetCity != NULL)
				{
					AreaAITypes eAreaAIType = pTargetCity->area()->getAreaAIType(getTeam());
					if (((eAreaAIType == AREAAI_OFFENSIVE) || (eAreaAIType == AREAAI_DEFENSIVE) || (eAreaAIType == AREAAI_MASSING))
						|| pTargetCity->AI_isDanger())
					{
						iValue = 10000;

						iValue *= (GET_PLAYER(getOwner()).AI_militaryWeight(pLoopCity->area()) + 10);
						iValue /= (GET_PLAYER(getOwner()).AI_totalAreaUnitAIs(pLoopCity->area(), AI_getUnitAIType()) + 10);

						iValue += std::max(1, ((GC.getMap().maxStepDistance() * 2) - GC.getMap().calculatePathDistance(pLoopCity->plot(), pTargetCity->plot())));

						if (AI_getUnitAIType() == UNITAI_PARADROP)
						{
							CvCity* pNearestEnemyCity = GC.getMap().findCity(pLoopCity->getX(), pLoopCity->getY(), NO_PLAYER, NO_TEAM, false, false, getTeam());

							if (pNearestEnemyCity != NULL)
							{
								int iDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pNearestEnemyCity->getX(), pNearestEnemyCity->getY());
								if (iDistance <= getDropRange())
								{
									iValue *= 5;
								}
							}
						}

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = pLoopCity->plot();
							FAssert(pLoopCity != pCity);
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_AIRLIFT, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_paradrop(int iRange)
{
	PROFILE_FUNC();

	if (getGroup()->getNumUnits() > 1)
	{
		return false;
	}

	const CvPlot* pPlot = plot();

	if (!canParadrop(pPlot))
	{
		return false;
	}

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	const int iParatrooperCount = plot()->plotCount(PUF_isParadrop, -1, -1, NULL, getOwner());
	FAssert(iParatrooperCount > 0);

	const int iSearchRange = AI_searchRange(iRange);

	foreach_(const CvPlot * pLoopPlot, plot()->rect(iSearchRange, iSearchRange))
	{
		if (isPotentialEnemy(pLoopPlot->getTeam(), pLoopPlot))
		{
			if (canParadropAt(pPlot, pLoopPlot->getX(), pLoopPlot->getY()))
			{
				int iValue = 0;

				const PlayerTypes eTargetPlayer = pLoopPlot->getOwner();
				FAssert(NO_PLAYER != eTargetPlayer);

				// Bonus values for bonuses the AI has are less than 10 for non-strategic resources... since this is
				// in the AI territory they probably have it
				if (NO_BONUS != pLoopPlot->getNonObsoleteBonusType(getTeam()))
				{
					iValue += std::max(1, GET_PLAYER(eTargetPlayer).AI_bonusVal(pLoopPlot->getBonusType()) - 10);
				}

				foreach_(const CvPlot * pAdjacentPlot, pLoopPlot->rect(1, 1))
				{
					const CvCity* pAdjacentCity = pAdjacentPlot->getPlotCity();

					if (NULL != pAdjacentCity)
					{
						if (pAdjacentCity->getOwner() == eTargetPlayer)
						{
							const int iAttackerCount = GET_PLAYER(getOwner()).AI_adjacentPotentialAttackers(pAdjacentPlot, true);
							const int iDefenderCount = pAdjacentPlot->getNumVisiblePotentialEnemyDefenders(this);
							iValue += 20 * (AI_attackOdds(pAdjacentPlot, true) - ((50 * iDefenderCount) / (iParatrooperCount + iAttackerCount)));
						}
					}
				}

				if (iValue > 0)
				{
					iValue += pLoopPlot->defenseModifier(getTeam(), ignoreBuildingDefense());

					const CvUnit* pInterceptor = bestInterceptor(pLoopPlot);
					if (NULL != pInterceptor)
					{
						int iInterceptProb = isSuicide() ? 100 : pInterceptor->currInterceptionProbability();

						iInterceptProb *= std::max(0, (100 - evasionProbability()));
						iInterceptProb /= 100;

						iValue *= std::max(0, 100 - iInterceptProb / 2);
						iValue /= 100;
					}
				}

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = pLoopPlot;

					FAssert(pBestPlot != pPlot);
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_PARADROP, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}

	return false;
}


/************************************************************************************************/
/* BETTER_BTS_AI_MOD					  09/01/09								jdog5000	  */
/*																							  */
/* Unit AI, Efficiency																		  */
/************************************************************************************************/
// Returns true if a mission was pushed...
bool CvUnitAI::AI_protect(int iOddsThreshold, int iMaxPathTurns)
{
    NAVAL_DIAG_PHASE(PROTECT,iOddsThreshold,iMaxPathTurns,0,0);

	PROFILE_FUNC();

	// Attack odds and peaceful entry do not prove that a group can fight.
	// A defend-only/immobile group must not chase one hostile stack, then
	// choose another and spend the turn walking between unattackable targets.
	bool hasMobileAttacker = false;
	foreach_(const CvUnit* attacker, getGroup()->units())
	{
		if (attacker->canAttack() && attacker->canMove())
		{
			hasMobileAttacker = true;
			break;
		}
	}
	if (!hasMobileAttacker) NAVAL_DIAG_PHASE_RETURN((false));

	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	CvPlot* endTurnPlot = NULL;
	int iValue;
	int iBestValue;

	iBestValue = 0;
	pBestPlot = NULL;

	CvReachablePlotSet plotSet(getGroup(), 0, AI_searchRange(iMaxPathTurns));

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		pLoopPlot = itr.plot();
		if (pLoopPlot->getOwner() == getOwner())
		{
			// Koshling - AI_plotValid() call not needed when using a CvReachablePlotSet
			if (/*AI_plotValid(pLoopPlot) &&*/ UnitTravelConnectivity::landUnitCanReach(this,pLoopPlot,true))
			{
				if (pLoopPlot->isVisibleEnemyUnit(this))
				{
					if (!atPlot(pLoopPlot))
					{
						// BBAI efficiency: Check area for land units
						if ((getDomainType() != DOMAIN_LAND) || (UnitTravelConnectivity::landUnitCanReach(this,pLoopPlot,true)) || getGroup()->canMoveAllTerrain())
						{
							// BBAI efficiency: Most of the time, path will exist and odds will be checked anyway.  When path doesn't exist, checking path
							// takes longer.  Therefore, check odds first.
							bool hasAttackTarget = false;
							foreach_(const CvUnit* attacker, getGroup()->units())
							{
								if (attacker->canAttack() && attacker->canMove()
									&& attacker->canEnterPlot(pLoopPlot, MoveCheck::Attack | MoveCheck::IgnoreLocation)
									&& pLoopPlot->hasDefender(true, NO_PLAYER, getOwner(), attacker, true))
								{
									hasAttackTarget = true;
									break;
								}
							}
							if (!hasAttackTarget) continue;

							iValue = getGroup()->AI_attackOdds(pLoopPlot, true);

							if ((iValue >= AI_finalOddsThreshold(pLoopPlot, iOddsThreshold)) && (iValue * 50 > iBestValue))
							{
								int iPathTurns;
								if (generatePath(pLoopPlot, 0, true, &iPathTurns, iMaxPathTurns))
								{
									// BBAI TODO: Other units targeting this already (if path turns > 1 or 0)?
									iValue *= 100;

									iValue /= (2 + iPathTurns);

									if (iValue > iBestValue)
									{
										endTurnPlot = getPathEndTurnPlot();

										if (!exposedToDanger(endTurnPlot, 100 - (100 - iOddsThreshold) / 2))
										{
											iBestValue = iValue;
											pBestPlot = endTurnPlot;
											FAssert(!atPlot(pBestPlot));
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));
		NAVAL_DIAG_PHASE_RETURN((getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY())));
	}

	NAVAL_DIAG_PHASE_RETURN((false));
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					   END												  */
/************************************************************************************************/

//	This routine effectively parallel the test in CvCityAI for building see attack/reserve units
//	in response to a sea invader in the same sea area (but not necessarily local)
bool CvUnitAI::AI_seaAreaAttack()
{
    NAVAL_DIAG_PHASE(SEA_AREA,0,0,0,0);

	PROFILE_FUNC();

	CvPlot* pBestPlot = NULL;
	int	iBestValue = 0;
	const CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	CvReachablePlotSet plotSet(getGroup(), 0);
	{
		PROFILE("CvUnitAI::AI_seaAreaAttack.Populate");
		plotSet.Populate(15);
	}
	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		CvPlot* pLoopPlot = itr.plot();
		// Relaxed (#sea-ai): pursue visible enemy ships anywhere in the sea area, not only within
        // plots we own. The generatePath() below still blocks entering territory we'd have to declare
        // war on, so this stays peace-safe while letting attack-sea units (incl. human-automated) leave
        // friendly waters to engage. Was: ... && pLoopPlot->getOwner() == getOwner().
		if (pLoopPlot->area() == area() && pLoopPlot->isVisible(getTeam(), false))
		{
			int iCount = pLoopPlot->plotCount(PUF_isEnemy, getOwner(), 0, NULL, NO_PLAYER, NO_TEAM, PUF_isVisible, getOwner());

			//	Refine this so that not EVERY unit heads for the same enemy incursion!
			if (iCount > 0)
			{
				int iValue = (10000 + GC.getGame().getSorenRandNum(5000, "AI sea area attack")) / (2 + kPlayer.AI_countNumLocalNavy(pLoopPlot, 4));

				PROFILE("CvUnitAI::AI_seaAreaAttack.Pathing");
				int iPathTurns;
				if (generatePath(pLoopPlot, 0, true, &iPathTurns, iBestValue == 0 ? MAX_INT : iValue / iBestValue))
				{
					PROFILE("CvUnitAI::AI_seaAreaAttack.Pathed");
					iValue /= (1 + iPathTurns);

					if (iValue > iBestValue)
					{
						CvPlot* endTurnPlot = getPathEndTurnPlot();

						if (!exposedToDanger(endTurnPlot, 60) && (!atPlot(endTurnPlot) || canAmbush(endTurnPlot)))
						{
							iBestValue = iValue;
							pBestPlot = endTurnPlot;
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (!atPlot(pBestPlot))
		{
			NAVAL_DIAG_PHASE_RETURN((getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY())));
		}
		// Sharing plot with the enemy
		FAssertMsg(canAmbush(pBestPlot), "Something is wrong. . .")
			NAVAL_DIAG_PHASE_RETURN((getGroup()->pushMissionInternal(MISSION_AMBUSH, pBestPlot->getX(), pBestPlot->getY())));
	}
	NAVAL_DIAG_PHASE_RETURN((false));
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_patrol(bool bIgnoreDanger)
{
	AI_PERF_SCOPE(PATROL);
	PROFILE_FUNC();

	if (skipPeacefulCombatSeaPatrol(*this, m_eUnitAIType))
	{
		return false;
	}

	const bool naval = getDomainType() == DOMAIN_SEA;
	// Real combat/protection/escort tasks are tried by the caller first. Do
	// not spend the rest of a turn repeatedly rolling adjacent idle patrols.
	// Unsafe positions still fall through to retreat/safety, not a forced hold.
	if (naval && hasMoved() && !isHuman() && !isNPC()
		&& (m_eUnitAIType == UNITAI_ATTACK_SEA || m_eUnitAIType == UNITAI_RESERVE_SEA || m_eUnitAIType == UNITAI_ESCORT_SEA)
		&& !getGroup()->hasCargo() && !bIgnoreDanger)
	{
		if (exposedToDanger(plot(), 60)) return false;
		return getGroup()->pushMissionInternal(MISSION_SKIP);
	}
	// A fallback patrol is not a travel mission. One idle step is enough for
	// combat troops at home; directed attacks, ranged fire, escorts, grouping
	// and BuildUp have already been considered by their role handler.
	// Hunters also try nearby/distant prey, goodies, exploration and border
	// positioning first. Do not bounce them around an idle home patrol.
	// A non-attacking group abroad cannot turn a random patrol into an
	// attack either. Keep its directed tasks, but do not burn its remaining
	// movement on repeated aimless steps outside our borders.
	if (!naval && getDomainType() == DOMAIN_LAND && hasMoved()
		&& !isHuman() && !isNPC() && !bIgnoreDanger && !getGroup()->hasCargo()
		&& (plot()->getTeam() == getTeam()
			|| !algo::any_of(getGroup()->units(), CvUnit::fn::canAttack()))
		&& (m_eUnitAIType == UNITAI_ATTACK || m_eUnitAIType == UNITAI_ATTACK_CITY
			|| m_eUnitAIType == UNITAI_RESERVE || m_eUnitAIType == UNITAI_COUNTER
			|| m_eUnitAIType == UNITAI_HUNTER))
	{
		if (exposedToDanger(plot(), 60)) return false;
		return getGroup()->pushMissionInternal(MISSION_SKIP);
	}
	PatrolCandidate navalCandidates[NUM_DIRECTION_TYPES];
	int navalCount = 0;
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pAdjacentPlot, plot()->adjacent())
	{
		if (AI_plotValid(pAdjacentPlot)
		&& (bIgnoreDanger || !pAdjacentPlot->isVisibleEnemyUnit(this))
		&& getGroup()->canEnterPlot(pAdjacentPlot, false))
		{
			int iValue = (1 + GC.getGame().getMapRandNum(10000, "AI Patrol"));

			if (isAnimal())
			{
				if (GC.getGame().isOption(GAMEOPTION_ANIMAL_DANGEROUS))
				{
					if (pAdjacentPlot->isVisibleEnemyUnit(this))
					{
						iValue += 5000;
					}
				}
				else if (!pAdjacentPlot->isOwned())
				{
					iValue += 5000;

					if (!pAdjacentPlot->isAdjacentOwned())
					{
						iValue += 2500;
					}
				}
			}
			else if (isHominid())
			{
				if (pAdjacentPlot->isVisibleEnemyUnit(this))
				{
					iValue += 5000;
				}
				if (pAdjacentPlot->isRevealedGoody(getTeam()))
				{
					iValue += 4000;
				}
				foreach_(const CvPlot * pAdjacentPlot2, pAdjacentPlot->adjacent())
				{
					if (pAdjacentPlot2->area() == pAdjacentPlot->area())
					{
						if (pAdjacentPlot2->isVisibleEnemyUnit(this))
						{
							iValue += 3000;
						}
						if (pAdjacentPlot->isRevealedGoody(getTeam()))
						{
							iValue += 2000;
						}
					}
				}
				if (pAdjacentPlot->isOwned())
				{
					iValue += 3000;

					if (pAdjacentPlot->getOwner() == getOwner())
					{
						iValue += 1000;
					}
				}
				else if (pAdjacentPlot->isAdjacentOwned())
				{
					iValue += 2000;
				}
			}

			if (naval)
			{
				PatrolCandidate& candidate = navalCandidates[navalCount];
				candidate.plot = pAdjacentPlot;
				candidate.value = iValue;
				candidate.order = navalCount++;
			}
			else if (iValue > iBestValue && (bIgnoreDanger || !exposedToDanger(pAdjacentPlot, 60)))
			{
				iBestValue = iValue;
				pBestPlot = pAdjacentPlot;
			}
		}
	}

	// All rolls and entry checks retain adjacent order. Only the read-only
	// exposure queries are reordered: the first safe maximum wins, including
	// the original first-on-tie rule. No map or unit cache survives this call.
	if (naval)
	{
		std::sort(navalCandidates, navalCandidates + navalCount);
		for (int i = 0; i < navalCount; ++i)
		{
			if (bIgnoreDanger || !exposedToDanger(navalCandidates[i].plot, 60))
			{
				pBestPlot = navalCandidates[i].plot;
				break;
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_defend()
{
	PROFILE_FUNC();

	int iPathTurns;

	if (AI_defendPlot(plot()))
	{
		getGroup()->pushMission(MISSION_SKIP);
		return true;
	}

	const int iSearchRange = AI_searchRange();

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, plot()->rect(iSearchRange, iSearchRange))
	{
		if (AI_plotValid(pLoopPlot))
		{
			if (AI_defendPlot(pLoopPlot))
			{
				if (!pLoopPlot->isVisibleEnemyUnit(this))
				{
					if (!atPlot(pLoopPlot) && generatePath(pLoopPlot, 0, true, &iPathTurns, 1))
					{
						const int iValue = (1 + GC.getGame().getSorenRandNum(10000, "AI Defend"));

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = pLoopPlot;
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (!(pBestPlot->isCity()) && (getGroup()->getNumUnits() > 1))
		{
			getGroup()->AI_makeForceSeparate();
		}

		FAssert(!atPlot(pBestPlot));
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}

	return false;
}


void CvUnitAI::AI_safetyEval(const CvPlot* plotX, const int iPass, const int iRange, const bool bAnimalDanger, int& iBestValue, const CvPlot** pBestPlot)
{
	AI_PERF_SCOPE(SAFETY_EVAL);
	AI_PERF_COUNT(SAFETY_EVALS, 1);
	if (plotX->isVisibleEnemyUnit(this))
	{
		AI_PERF_COUNT(SAFETY_ENEMY_SKIPS, 1);
		return;
	}
	int iCount = 0;

	foreach_(const CvUnit * unitX, plotX->units())
	{
		if (unitX->getOwner() == getOwner() && unitX->canDefend())
		{
			const CvUnit* pHeadUnit = unitX->getGroup()->getHeadUnit();
			FAssert(pHeadUnit);
			FAssert(getGroup()->getHeadUnit() == this);

			if (pHeadUnit != this && (pHeadUnit->isWaiting() || !pHeadUnit->canMove()))
			{
				FAssert(unitX != this);
				FAssert(pHeadUnit != getGroup()->getHeadUnit());
				iCount++;
			}
		}
	}
	int iValue = iCount * 100;

	// If the current danger includes animals then simply running to our
	//	borders is defense to at least the animal component of the current danger
	if (bAnimalDanger)
	{
		if (plotX->getOwner() == getOwner())
		{
			iValue += 75;
		}
		else if (plotX->getOwner() != NO_PLAYER && !atWar(plotX->getTeam(), getTeam()))
		{
			iValue += 50;
		}
	}
	iValue += plotX->defenseModifier(getTeam(), false);

	if (atPlot(plotX)) iValue += 50;
	const int iTerrainDamage = plotX->getTotalTurnDamage(getGroup());
	// Do not path to a tile that cannot beat the incumbent even with the
	// maximum random bonus and zero travel. Keep unbounded legacy requests.
	const int pathLimit = iRange < 0 ? iRange : safetyScorePathLimit(iValue,
		atPlot(plotX), getDamagePercent(), iTerrainDamage, iBestValue, iRange);
	if (iRange >= 0 && pathLimit < 0) return;

	int iPathTurns = 0;
	bool bReachable;
	{
		AI_PERF_SCOPE(SAFETY_PATH);
		AI_PERF_SAFETY_QUERY(plotX, iPass);
#ifdef C2C_AI_PERF_ENABLE
		if (stepDistance(getX(), getY(), plotX->getX(), plotX->getY()) > 3)
			AI_PERF_COUNT(SAFETY_DISTANT_REQUESTS, 1);
#endif
		bReachable = generatePath(plotX, (iPass > 0 ? MOVE_IGNORE_DANGER : 0), true, &iPathTurns, pathLimit);
	}
	if (!bReachable)
	{
		AI_PERF_COUNT(SAFETY_PATH_FAILURES, 1);
#ifdef C2C_AI_PERF_ENABLE
		if (stepDistance(getX(), getY(), plotX->getX(), plotX->getY()) > 3)
			AI_PERF_COUNT(SAFETY_DISTANT_FAILURES, 1);
#endif
		return;
	}
#if defined(C2C_AI_PERF_ENABLE) && defined(C2C_AI_SAFETY_BASELINE) && defined(C2C_AI_SAFETY_SHADOW)
	if (AIPerf::safetyUnexpectedReach())
		gDLL->logMsg(AIPerf::logFile(), CvString::format(
			"SAFETY_SHADOW_ERROR turn=%d owner=%d unit=%d originX=%d originY=%d targetX=%d targetY=%d pass=%d range=%d pathTurns=%d maxMoves=%d movesLeft=%d",
			GC.getGame().getGameTurn(), getOwner(), getID(), getX(), getY(), plotX->getX(), plotX->getY(), iPass, iRange, iPathTurns, maxMoves(), movesLeft()).c_str());
#endif
	AI_PERF_SCOPE(SAFETY_SCORE);

	if (!atPlot(plotX)) iValue += GC.getGame().getSorenRandNum(50, "AI Safety");
	{
		if (iTerrainDamage > 0)
		{
			iValue *= std::max(1, 100 - getDamagePercent() - iTerrainDamage);
			iValue /= 200;
		}
	}
	iValue = iValue * 5 / (iPathTurns + 4);

	if (iValue > iBestValue)
	{
		bool bDanger;
		{
			AI_PERF_SCOPE(SAFETY_DANGER);
			bDanger = GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plotX, 1, false);
		}
		if (bDanger) iValue /= 2;
		if (iValue > iBestValue)
		{
			//	If we have to pass through worse danger to get there it's not worth it
			CvPlot* endTurnPlot = getPathEndTurnPlot();

			if (endTurnPlot == plotX
			|| !GET_PLAYER(getOwner()).AI_getAnyPlotDanger(endTurnPlot, 1, false)
			|| endTurnPlot->defenseModifier(getTeam(), false) >= plot()->defenseModifier(getTeam(), false))
			{
				iBestValue = iValue;
				*pBestPlot = plotX;
			}
		}
	}
}

// Per-decision movement reachability, deliberately ignoring danger, borders and
// impassability. This superset can reject an impossible path without guessing
// which route the weighted path generator would prefer. Nothing persists after
// the decision or changes the generator's tree or invalidation policy.
class SafetyMovementReach
{
	struct Node
	{
		const CvPlot* plot;
		int turn, remaining;
	};
	struct Priority
	{
		bool operator()(const Node& a, const Node& b) const
		{
			return a.turn > b.turn || (a.turn == b.turn && a.remaining < b.remaining);
		}
	};
	typedef std::map<const CvPlot*, Node> Nodes;
	Nodes m_nodes;
	bool m_active;
public:
	SafetyMovementReach(const CvUnit* unit, int range, int searchRange) : m_active(false)
	{
		AI_PERF_SCOPE(SAFETY_BOUND);
#if !defined(C2C_AI_PERF_BASELINE) && !defined(USE_OLD_PATH_GENERATOR) && (!defined(C2C_AI_SAFETY_BASELINE) || defined(C2C_AI_SAFETY_SHADOW))
		if (range < 0 || range > 3 || unit->getGroup()->getNumUnits() != 1
		|| (unit->getDomainType() != DOMAIN_LAND && unit->getDomainType() != DOMAIN_SEA)) return;
		const int maxMoves = unit->maxMoves();
		if (maxMoves <= 0) return;
		const CvPlot* origin = unit->plot();
		Node first = { origin, 0, unit->movesLeft() };
		m_nodes[origin] = first;
		std::priority_queue<Node, std::vector<Node>, Priority> queue;
		queue.push(first);
		while (!queue.empty())
		{
			const Node node = queue.top();
			queue.pop();
			const Node& best = m_nodes.find(node.plot)->second;
			if (node.turn != best.turn || node.remaining != best.remaining) continue;
			AI_PERF_COUNT(SAFETY_FLOOD_PLOTS, 1);
			// Same zero-based turn and last-move overspending as NewPathCostFunc.
			const int nextTurn = node.turn + (node.remaining == 0 ? 1 : 0);
			if (nextTurn > range) continue;
			const int available = node.remaining == 0 ? maxMoves : node.remaining;
			foreach_(const CvPlot* adjacent, node.plot->adjacent())
			{
				Node next = { adjacent, nextTurn, std::max(0, available - adjacent->movementCost(unit, node.plot)) };
				Nodes::iterator found = m_nodes.find(adjacent);
				if (found != m_nodes.end() && (found->second.turn < next.turn
				|| (found->second.turn == next.turn && found->second.remaining >= next.remaining))) continue;
				// A valid detour can leave the candidate search area and return. If
				// movement allows leaving it, disable this filter rather than clip
				// the flood and incorrectly exclude such a route. Cap work as well.
				if (stepDistance(origin->getX(), origin->getY(), adjacent->getX(), adjacent->getY()) > searchRange
				|| (found == m_nodes.end() && m_nodes.size() >= 512))
				{
					AI_PERF_COUNT(SAFETY_FLOOD_FALLBACKS, 1);
					m_nodes.clear();
					return;
				}
				m_nodes[adjacent] = next;
				queue.push(next);
			}
		}
		m_active = true;
		AI_PERF_COUNT(SAFETY_BOUND_APPLIED, 1);
#endif
	}
	bool canReach(const CvPlot* plot) const
	{
		return !m_active || m_nodes.find(plot) != m_nodes.end();
	}
};

// Returns true if a mission was pushed...
bool CvUnitAI::AI_safety(int iRange)
{
    NAVAL_DIAG_PHASE(SAFETY,iRange,0,0,0);

	AI_PERF_SCOPE(SAFETY);
	AI_PERF_SAFETY_TRACE();
	PROFILE_FUNC();

	const int iSearchRange = AI_searchRange(iRange);
	SafetyMovementReach reachable(this, iRange, iSearchRange);
	const bool bAnimalDanger = GET_PLAYER(getOwner()).AI_getVisiblePlotDanger(plot(), 1, true);

	const CvPlot* pBestPlot = NULL;
	int iBestValue = 0;
	int iPass = 0;
	{
		// Candidate evaluation is read-only; end the cache scope before issuing orders.
		PlotDangerCacheScope candidateDangerCache(getOwner());
	for (; iPass < 2 && !pBestPlot; ++iPass)
	{
		if (iPass > 0) AI_PERF_COUNT(SAFETY_SECOND_PASSES, 1);
		if (!GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
		{
			CvReachablePlotSet plotSet(getGroup(), (iPass > 0) ? MOVE_IGNORE_DANGER : 0);
			{
				AI_PERF_SCOPE(SAFETY_SET);
				plotSet.Populate(iSearchRange);
			}

			for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
			{
				const CvPlot* plotX = itr.plot();
				AI_PERF_SAFETY_PREDICT(!reachable.canReach(plotX));
#ifndef C2C_AI_SAFETY_BASELINE
				if (!reachable.canReach(plotX))
				{
					AI_PERF_COUNT(SAFETY_CANDIDATES_PRUNED, 1);
					continue;
				}
#endif
				AI_safetyEval(plotX, iPass, iRange, bAnimalDanger, iBestValue, &pBestPlot);
			}
		}
		else
		{
			foreach_(const CvPlot * plotX, plot()->rect(iSearchRange, iSearchRange))
			{
				AI_PERF_SAFETY_PREDICT(!reachable.canReach(plotX));
#ifndef C2C_AI_SAFETY_BASELINE
				if (!reachable.canReach(plotX))
				{
					AI_PERF_COUNT(SAFETY_CANDIDATES_PRUNED, 1);
					continue;
				}
#endif
				if (!AI_plotValid(plotX))
				{
					continue;
				}
				AI_safetyEval(plotX, iPass, iRange, bAnimalDanger, iBestValue, &pBestPlot);
			}
		}
	}

	}

	if (pBestPlot)
	{
		if (atPlot(pBestPlot))
		{
			OutputDebugString(CvString::format("%S (%d) seeking safety stays put at (%d,%d)...\n", getDescription().c_str(), m_iID, m_iX, m_iY).c_str());
			getGroup()->pushMission(MISSION_SKIP);
			NAVAL_DIAG_PHASE_RETURN((true));
		}
		OutputDebugString(CvString::format("%S (%d) seeking safety moves to (%d,%d)\n", getDescription().c_str(), m_iID, pBestPlot->getX(), pBestPlot->getY()).c_str());
		// Check iPass > 1 here because we always increment it as we exit the loop, so it will be 1 higher than when pBestPlot was assigned a value
		NAVAL_DIAG_PHASE_RETURN((getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), ((iPass > 1) ? MOVE_IGNORE_DANGER : 0))));
	}
	NAVAL_DIAG_PHASE_RETURN((false));
}

// Returns true if a mission was pushed... unless it's a mock run.
bool CvUnitAI::AI_reachHome(const bool bMockRun, int iRange) const
{
	PROFILE_FUNC();

	OutputDebugString(CvString::format("%S (%d) AI_reachHome (%d,%d)...%d\n", getDescription().c_str(), m_iID, m_iX, m_iY, (int)bMockRun).c_str());

	if (bMockRun && plot()->getOwner() == getOwner())
	{
		return true;
	}
	if (iRange < baseMoves())
	{
		iRange = baseMoves();
	}
	const CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	foreach_(const CvPlot * plotX, plot()->rect(iRange, iRange))
	{
		if (!plotX || plotX->getOwner() != getOwner() || GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plotX, 1) || atPlot(plotX))
		{
			continue;
		}
		int iPathTurns;
		if (!generatePath(plotX, 0, true, &iPathTurns, 1) || iPathTurns > 1)
		{
			continue;
		}
		{
			bool bStranded = true;
			foreach_(CvCity * cityX, GET_PLAYER(getOwner()).cities())
			{
				if (plotX->isConnectedTo(cityX))
				{
					bStranded = false;
					break;
				}
			}
			if (bStranded)
			{
				continue;
			}
		}
		int iValue = 1000;

		{
			const CvCity* cityNear = GC.getMap().findCity(plotX->getX(), plotX->getY(), getOwner(), NO_TEAM, true);

			if (cityNear)
			{
				if (!plotX->isConnectedTo(cityNear))
				{
					iValue /= 10;
				}
				iValue /= 1 + plotDistance(plotX->getX(), plotX->getY(), cityNear->getX(), cityNear->getY());
			}
			else
			{
				iValue /= 100;
			}
		}

		foreach_(const CvUnit * unitX, plotX->units())
		{
			if (unitX->getOwner() == getOwner() && unitX->canDefend())
			{
				const CvUnit* pHeadUnit = unitX->getGroup()->getHeadUnit();

				if (pHeadUnit != this && (pHeadUnit->isWaiting() || !pHeadUnit->canMove()))
				{
					iValue += unitX->currEffectiveStr(plotX, NULL);;
				}
			}
		}
		// Diversify, don't always do the same in a given situation.
		iValue += GC.getGame().getSorenRandNum(iValue / 5, "AI Safety");

		if (iValue > iBestValue)
		{
			if (bMockRun) return true;

			iBestValue = iValue;
			pBestPlot = plotX;
		}
	}
	if (pBestPlot)
	{
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0);
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_hide()
{
	PROFILE_FUNC();

	if (getInvisibleType() == NO_INVISIBLE)
	{
		return false;
	}
	int iPathTurns;
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	const int iSearchRange = AI_searchRange();

	foreach_(const CvPlot * plotX, plot()->rect(iSearchRange, iSearchRange))
	{
		if (AI_plotValid(plotX))
		{
			{
				bool bValid = true;
				for (int iI = 0; iI < MAX_TEAMS; iI++)
				{
					if (GET_TEAM((TeamTypes)iI).isAlive()
					&& plotX->isSpotterInSight((TeamTypes)iI, getInvisibleType()))
					{
						bValid = false;
						break;
					}
				}
				if (!bValid) continue;
			}
			if (!plotX->isVisibleEnemyUnit(this) && generatePath(plotX, 0, true, &iPathTurns, 1))
			{
				int iCount = 1;

				foreach_(const CvUnit * unitX, plotX->units())
				{
					if (unitX->getOwner() == getOwner() && unitX->canDefend())
					{
						const CvUnit* pHeadUnit = unitX->getGroup()->getHeadUnit();
						FAssert(pHeadUnit != NULL);
						FAssert(getGroup()->getHeadUnit() == this);

						if (pHeadUnit != this && (pHeadUnit->isWaiting() || !pHeadUnit->canMove()))
						{
							FAssert(unitX != this);
							FAssert(pHeadUnit != getGroup()->getHeadUnit());
							iCount++;
						}
					}
				}

				const int iValue =
					(
						iCount * 100
						+ plotX->defenseModifier(getTeam(), false)
						+ (atPlot(plotX) ? 50 : GC.getGame().getSorenRandNum(50, "AI Hide"))
					);

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = plotX;
				}
			}
		}
	}
	if (pBestPlot != NULL)
	{
		if (atPlot(pBestPlot))
		{
			getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_goody(int iRange)
{
	// Stop dedicated hut/island searches from Industrial onward, including
	// scouts and explorers. Ordinary exploration keeps its existing behavior.
	const int industrialEra = GC.getInfoTypeForString("C2C_ERA_INDUSTRIAL");
	if (industrialEra >= 0 && GET_PLAYER(getOwner()).getCurrentEra() >= industrialEra)
	{
		return false;
	}

    NAVAL_DIAG_PHASE(GOODY,iRange,0,0,0);

	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	//	CvPlot* pAdjacentPlot;
	CvPlot* pBestPlot;
	int iSearchRange;
	int iPathTurns;
	int iValue;
	int iBestValue;
	//int iDX, iDY;
//	int iI;

	if (isNPC())
	{
		NAVAL_DIAG_PHASE_RETURN((false));
	}

	FAssert(canMove());

	iSearchRange = AI_searchRange(iRange);

	iBestValue = 0;
	pBestPlot = NULL;

	CvReachablePlotSet plotSet(getGroup(), 0, iSearchRange);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		pLoopPlot = itr.plot();
		if (pLoopPlot->isRevealedGoody(getTeam()))
		{
			if ((!canAttack() && !pLoopPlot->isVisibleEnemyUnit(this)) || !exposedToDanger(pLoopPlot, 60))
			{
				if (!atPlot(pLoopPlot) && generatePath(pLoopPlot, 0, true, &iPathTurns, iRange))
				{
					if (iPathTurns <= iRange)
					{
						iValue = (1 + GC.getGame().getSorenRandNum(10000, "AI Goody"));

						iValue /= (iPathTurns + 1);

						if (iValue > iBestValue)
						{
							//	Does this proposed move leave us exposed at the end of this
							//	turn?
							if (exposedToDanger(getPathEndTurnPlot(), 60))
							{
								//	Reject this option if we have less than 60% combat odds
								continue;
							}
							iBestValue = iValue;
							pBestPlot = getPathEndTurnPlot();
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));
		NAVAL_DIAG_PHASE_RETURN((getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY())));
	}

	NAVAL_DIAG_PHASE_RETURN((false));
}

bool CvUnitAI::AI_explorerJoinOffensiveStacks()
{
	//When in an offensive war, we are already sending stacks into enemy territory, so exploring
	//in addition to the stacks is counterproductive, and dangerous
	const bool bOffenseWar = area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE;
	if (bOffenseWar && !isHuman() && AI_getUnitAIType() != UNITAI_HUNTER && canAttack())	//	Exempt hunters from this behaviour and also defend-only explorers
	{
		//try to join SoD
		if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK).maxOwnUnitAI(1).minUnitAI(6).stackOfDoom().allowRegrouping()))
		{
			return true;
		}
		if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK).maxOwnUnitAI(1).minUnitAI(4).stackOfDoom().allowRegrouping()))
		{
			return true;
		}
		if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK_CITY).maxOwnUnitAI(1).minUnitAI(4).stackOfDoom().allowRegrouping()))
		{
			return true;
		}
		if (AI_group(GroupingParams().withUnitAI(UNITAI_COLLATERAL).maxOwnUnitAI(1).minUnitAI(4).stackOfDoom().allowRegrouping()))
		{
			return true;
		}
		if (AI_group(GroupingParams().withUnitAI(UNITAI_COUNTER).maxOwnUnitAI(1).minUnitAI(3).stackOfDoom().allowRegrouping()))
		{
			return true;
		}
		//try to join attacking stack
		if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK).maxOwnUnitAI(1)))
		{
			return true;
		}
		if (AI_group(GroupingParams().withUnitAI(UNITAI_ATTACK_CITY).maxOwnUnitAI(1)))
		{
			return true;
		}
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_explore()
{
	PROFILE_FUNC();

	if (AI_explorerJoinOffensiveStacks())
	{
		return true;
	}

	//	If we had previously selected a target make sure we include it in our evaluation this
	//	time around else dithering between different plots can occur
	CvPlot* pPreviouslySelectedPlot = NULL;
	if (getGroup()->AI_getMissionAIType() == MISSIONAI_EXPLORE)
	{
		pPreviouslySelectedPlot = getGroup()->AI_getMissionAIPlot();

		if (pPreviouslySelectedPlot != NULL && atPlot(pPreviouslySelectedPlot))
		{
			pPreviouslySelectedPlot = NULL;
		}
	}
	std::vector<plotValue> plotValues;
	const bool bNoContact = GC.getGame().countCivTeamsAlive() > GET_TEAM(getTeam()).getHasMetCivCount(true);

	CvReachablePlotSet plotSet(getGroup(), MOVE_NO_ENEMY_TERRITORY, MAX_INT);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		PROFILE("AI_explore 1");

		const CvPlot* plotX = itr.plot();

		// Only consider plots that are revealed or adjacent to something we already revealed
		//	ls612: Only consider plots that have no enemy defenders on them
		if (!plotX->isVisibleEnemyDefender(this) && plotX->area() == area() && (plotX->isRevealed(getTeam(), false) || plotX->isAdjacentRevealed(getTeam())))
		{
			int iValue = 0;

			if (plotX->isRevealedGoody(getTeam()))
			{
				iValue += 100000;
			}

			if (!plotX->isRevealed(getTeam(), false))
			{
				iValue += 10000;
			}

			if (iValue > 0 || plotX == pPreviouslySelectedPlot || GC.getGame().getMapRandNum(4, "AI make explore faster ;)") == 0)
			{
				PROFILE("AI_explore 2");

				foreach_(const CvPlot * plotY, plotX->adjacent())
				{
					if (!plotY->isRevealed(getTeam(), false))
					{
						iValue += 1000;
					}
					else if (bNoContact && plotY->getRevealedTeam(getTeam(), false) != plotY->getTeam())
					{
						iValue += 100;
					}
				}

				if (iValue < 1 || plotX->isVisibleEnemyUnit(this))
				{
					continue;
				}
				iValue += GC.getGame().getMapRandNum(250 * abs(xDistance(getX(), plotX->getX())) + abs(yDistance(getY(), plotX->getY())), "AI explore");

				if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(plotX, MISSIONAI_EXPLORE, getGroup(), 3) == 0)
				{
					//	Add some hysteresis to prevent switching between targets too much
					if (pPreviouslySelectedPlot == plotX)
					{
						iValue = (110 * iValue) / 100;
					}

					if (plotX->isAdjacentToLand())
					{
						iValue += 10000;
					}

					if (plotX->isOwned())
					{
						iValue += 5000;
					}

					//ls612: Make exploring AI aware of terrain damage
					if (plotX->getTotalTurnDamage(getGroup()) > 0 || plotX->getFeatureTurnDamage() > 0)
					{
						iValue /= 5;
					}
					iValue /= 3 + (std::max(1, itr.stepDistance()) / maxMoves());

					if (!atPlot(plotX) && iValue > 0)
					{
						plotValue thisPlotValue;

						thisPlotValue.plot = const_cast<CvPlot*>(plotX);
						thisPlotValue.value = iValue;

						plotValues.push_back(thisPlotValue);
					}
				}
			}
		}
	}
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestExplorePlot = NULL;
	algo::sort(plotValues, plotValueSortPredicate);

	for (int iI = plotValues.size() - 1; iI >= 0; iI--)
	{
		PROFILE("AI_explore 1.1");

		CvPlot* plotX = plotValues[iI].plot;
		int iPathTurns;
		if (generatePath(plotX, MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns))
		{
			PROFILE("AI_explore 1.1.1");
			// Does this proposed move leave us exposed at the end of this turn?
			if (exposedToDanger(getPathEndTurnPlot(), 60))
			{
				// Reject this option if we have less than 60% combat odds
				continue;
			}
			pBestPlot = plotX->isRevealedGoody(getTeam()) ? getPathEndTurnPlot() : plotX;
			pBestExplorePlot = plotX;
			break;
		}
	}

	if (pBestPlot && pBestExplorePlot)
	{
		FAssert(!atPlot(pBestPlot));
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_NO_ENEMY_TERRITORY | MOVE_AVOID_ENEMY_UNITS | MOVE_HEAL_AS_NEEDED25, false, false, MISSIONAI_EXPLORE, pBestExplorePlot);
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_exploreRange(int iRange)
{
	PROFILE_FUNC();

	if (AI_explorerJoinOffensiveStacks())
	{
		return true;
	}
	// If we had previously selected a target bias towards move in that direction
	CvPlot* pPreviouslySelectedPlot = NULL;
	if (getGroup()->AI_getMissionAIType() == MISSIONAI_EXPLORE)
	{
		pPreviouslySelectedPlot = getGroup()->AI_getMissionAIPlot();

		if (pPreviouslySelectedPlot && atPlot(pPreviouslySelectedPlot))
		{
			pPreviouslySelectedPlot = NULL;
		}
	}
	std::vector<plotValue> plotValues;
	const int iImpassableCount = GET_PLAYER(getOwner()).AI_unitImpassableCount(getUnitType());
	const int iSearchRange = AI_searchRange(iRange);
	CvReachablePlotSet plotSet(getGroup(), MOVE_NO_ENEMY_TERRITORY, iSearchRange);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		PROFILE("AI_exploreRange 1");

		//	Have we already considered this plot in a previous invocation?
		int iOpaqueInfo = itr.getOpaqueInfo(ACTIVITY_ID_EXPLORE);
		if (plotOpaqueInfoMatches(iOpaqueInfo, ACTIVITY_ID_EXPLORE, 1))
		{
			continue;
		}
		itr.setOpaqueInfo(ACTIVITY_ID_EXPLORE, 1);

		const CvPlot* plotX = itr.plot();

		// Only consider plots that are revealed or adjacent to something we already revealed
		if (atPlot(plotX) || !plotX->isRevealed(getTeam(), false) && !plotX->isAdjacentRevealed(getTeam()))
		{
			continue;
		}
		int iValue = 0;

		// Tend to keep heading to the same spot or else dithering and
		//	swapping of tragets between multiple explorers tends to occur
		if (plotX == pPreviouslySelectedPlot)
		{
			iValue += 10000;
		}

		if (plotX->isRevealedGoody(getTeam()))
		{
			iValue += 100000;
		}

		if (!plotX->isRevealed(getTeam(), false))
		{
			iValue += 10000;
		}

		PROFILE("AI_exploreRange 2");

		foreach_(const CvPlot * pAdjacentPlot, plotX->adjacent() | filtered(!CvPlot::fn::isRevealed(getTeam(), false)))
		{
			iValue += 1000;
		}

		if (iValue < 1
		|| !canEnterPlot(plotX)
		|| plotX->isVisibleEnemyUnit(this)
		|| GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(plotX, MISSIONAI_EXPLORE, getGroup(), 3) != 0)
		{
			continue;
		}
		PROFILE("AI_exploreRange 3");

		if (plotX->isAdjacentToLand())
		{
			iValue += 10000;
		}

		if (plotX->isOwned())
		{
			iValue += 5000;
		}

		if (!isHuman() && AI_getUnitAIType() == UNITAI_EXPLORE_SEA && iImpassableCount == 0)
		{
			int iDX = abs(getX() - plotX->getX());
			int iDY = abs(getY() - plotX->getY());
			int iDirectionModifier = 100 + 50 * (abs(iDX) + abs(iDY)) / iSearchRange;
			if (GC.getGame().circumnavigationAvailable())
			{
				if (GC.getMap().isWrapX())
				{
					if (iDX * ((AI_getBirthmark() % 2) == 0 ? 1 : -1) > 0)
					{
						iDirectionModifier *= 150 + ((iDX * 100) / iSearchRange);
					}
					else iDirectionModifier /= 2;
				}
				if (GC.getMap().isWrapY())
				{
					if (iDY * (((AI_getBirthmark() >> 1) % 2) == 0 ? 1 : -1) > 0)
					{
						iDirectionModifier *= 150 + ((iDY * 100) / iSearchRange);
					}
					else iDirectionModifier /= 2;
				}
			}
			iValue *= iDirectionModifier;
			iValue /= 100;
		}
		iValue += GC.getGame().getMapRandNum(10000, "AI Explore");

		//ls612: Make exploring AI aware of terrain damage
		if (plotX->getTotalTurnDamage(getGroup()) > 0 || plotX->getFeatureTurnDamage() > 0)
		{
			iValue /= 5;
		}
		iValue /= (1 + stepDistance(getX(), getY(), plotX->getX(), plotX->getY()));

		plotValue thisPlotValue;

		thisPlotValue.plot = const_cast<CvPlot*>(plotX);
		thisPlotValue.value = iValue;

		plotValues.push_back(thisPlotValue);
	}

	PROFILE("AI_exploreRange 4");
	bool candidatesRejectedForMoveSafety = false;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestExplorePlot = NULL;

	// Sort the plots found on their values
	while (plotValues.size() > 0)
	{
		algo::sort(plotValues, plotValueSortPredicate);

		int iValue = plotValues[plotValues.size() - 1].value;
		if (iValue == 0)
		{
			break;
		}
		CvPlot* plotX = plotValues[plotValues.size() - 1].plot;

		// Avoid the cost of path generation if this cannot possibly be the best result
		CvReachablePlotSet::const_iterator itr = plotSet.find(plotX);
		int iPathTurns;
		if (itr != plotSet.end() && generatePath(plotX, MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns, iRange))
		{
			// Don't limit by iPathTurns since the range param passed in is really
			//	advisory only and intended to limit the size of the search space

			// Does this proposed move leave us exposed at the end of this turn?
			// For general exploring moves we accept danger with 60% combat odds in our favour
			if (exposedToDanger(getPathEndTurnPlot(), 60))
			{
				// For now just reject this.  Might want to do a combat odds check ideally
				candidatesRejectedForMoveSafety = true;
			}
			else
			{
				if (getDomainType() == DOMAIN_LAND)
				{
					pBestPlot = getPathEndTurnPlot();
				}
				else pBestPlot = plotX;

				pBestExplorePlot = plotX;
				break;
			}
		}
		else
		{
			// It's actually out of range for this search so we have not really meaningfully checked it
			itr.setOpaqueInfo(ACTIVITY_ID_EXPLORE, 0);
			// Redistribute the score of the top plot (which we could not reach) to other candidates according to their distance from it.
			//	This creates a bias to explore towards the high value plot that we cannot yet find a path for
			for (int iI = 0; iI < (int)plotValues.size() - 1; iI++)
			{
				//	Can knock out candidates that are adjacent to the one we just tried to path to since we won't be able to reach them
				//	either (unless both are unrevealed potentially)
				if (stepDistance(plotX->getX(), plotX->getY(), plotValues[iI].plot->getX(), plotValues[iI].plot->getY()) == 1
				&& (plotX->isRevealed(getTeam(), false) || plotValues[iI].plot->isRevealed(getTeam(), false)))
				{
					plotValues[iI].value = 0;
				}
				else if (plotValues[iI].value > 0)
				{
					plotValues[iI].value += iValue / (5 * std::max(1, plotDistance(plotX->getX(), plotX->getY(), plotValues[iI].plot->getX(), plotValues[iI].plot->getY())));
				}
			}
		}
		plotValues.resize(plotValues.size() - 1);
	}

	if (pBestPlot && pBestExplorePlot)
	{
		OutputDebugString(CvString::format("%S (%d) chooses to explore move to (%d,%d)\n", getDescription().c_str(), m_iID, pBestPlot->getX(), pBestPlot->getY()).c_str());
		PROFILE("AI_exploreRange 5");

		FAssert(!atPlot(pBestPlot));
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_NO_ENEMY_TERRITORY | MOVE_HEAL_AS_NEEDED25, false, false, MISSIONAI_EXPLORE, pBestExplorePlot);
	}
	else if (candidatesRejectedForMoveSafety)
	{
		OutputDebugString(CvString::format("%S (%d) finds danger blocking explore move an seeks safety\n", getDescription().c_str(), m_iID).c_str());
		//	Just stay safe for a while
		return AI_safety();
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_refreshExploreRange(int iRange, bool bIncludeVisibilityRefresh)
{
	PROFILE_FUNC();

	if (AI_explorerJoinOffensiveStacks())
	{
		return true;
	}

	// If we had previously selected a target bias towards move in that direction
	CvPlot* pPreviouslySelectedPlot = NULL;
	if (getGroup()->AI_getMissionAIType() == MISSIONAI_EXPLORE)
	{
		pPreviouslySelectedPlot = getGroup()->AI_getMissionAIPlot();

		if (pPreviouslySelectedPlot != NULL && atPlot(pPreviouslySelectedPlot))
		{
			pPreviouslySelectedPlot = NULL;
		}
	}
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestExplorePlot = NULL;
	bool candidatesRejectedForMoveSafety = false;
	const int iSearchRange = AI_searchRange(iRange);
	CvReachablePlotSet plotSet(getGroup(), MOVE_NO_ENEMY_TERRITORY, iSearchRange);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		const CvPlot* plotX = itr.plot();
		PROFILE("AI_exploreRange 1");

		if (plotX && !atPlot(plotX))
		{
			int iAdjacentEnemies = 0;
			bool bValidAdjacentEnemyValue = false;

			int iValue = 0;

			if (plotX->isRevealedGoody(getTeam()))
			{
				iValue += 100000;
			}

			if (!plotX->isRevealed(getTeam(), false))
			{
				iValue += 10000;
			}

			if (bIncludeVisibilityRefresh && !plotX->isVisible(getTeam(), false))
			{
				iValue += (GC.getGame().getGameTurn() - plotX->getLastVisibleTurn(getTeam()));
			}

			foreach_(const CvPlot * pAdjacentPlot, plotX->adjacent())
			{
				PROFILE("AI_exploreRange 2");

				if (!(pAdjacentPlot->isRevealed(getTeam(), false)))
				{
					iValue += 1000;
				}

				//	If there is an enemy unit there add an extra value to the adjacent plot
				//	we are currently considering so as to consider moving there to tempt the enemy
				//	into an attack more favourable to us than us attacking them (which will already
				//	have been considered).  We will only actually consider this value if we can get
				//	there this turn however, since units move!
				if (pAdjacentPlot->isVisible(getTeam(), false) && AI_plotValid(pAdjacentPlot) && stepDistance(plotX->getX(), plotX->getY(), getX(), getY()) <= AI_searchRange())
				{
					foreach_(const CvUnit * pLoopUnit, pAdjacentPlot->units())
					{
						//	Animals won't attack into owned territory so don't count them
						if (isEnemy(pLoopUnit->getTeam()) && !pLoopUnit->isInvisible(getTeam(), false) &&
							(plotX->getOwner() == NO_PLAYER || !pLoopUnit->isAnimal() || (pLoopUnit->isAnimal() && pLoopUnit->canAnimalIgnoresBorders())))
						{
							iAdjacentEnemies++;

							//	Don't count extra value for might-be-attacked in owned territory.  This stops units
							//	getting stuck oscillating back and forth between two defensive tiles next to foreign
							//	(esp barbarian) cities when they should be hunting/exploring
							if (pAdjacentPlot->getOwner() == NO_PLAYER || pAdjacentPlot->getOwner() == getOwner())
							{
								bValidAdjacentEnemyValue = true;
							}
						}
					}
				}
			}

			if (iValue < 1 && !bValidAdjacentEnemyValue
			|| plotX->isVisibleEnemyUnit(this)
			|| GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(plotX, MISSIONAI_EXPLORE, getGroup(), 3) != 0)
			{
				continue;
			}
			PROFILE("AI_exploreRange 3");

			//	Avoid the cost of path generation if this cannot possibly be the best result
			int iPathTurns;
			if ((iValue > iBestValue - 50 - (bValidAdjacentEnemyValue ? 50 : 0)) && generatePath(plotX, MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns, iRange))
			{
				if (iPathTurns <= iRange)
				{
					iValue += GC.getGame().getMapRandNum(50, "AI Explore");

					if (bValidAdjacentEnemyValue && iPathTurns <= 1)
					{
						// Chance to prompt a favorable attack
						iValue += 50 + plotX->defenseModifier(getTeam(), false);
					}

					// Try to pick something that moves towards our eventual goal (if any)
					if (pPreviouslySelectedPlot)
					{
						iValue += (
							50
							*
							(
								stepDistance(pPreviouslySelectedPlot->getX(), pPreviouslySelectedPlot->getY(), getX(), getY())
								- stepDistance(pPreviouslySelectedPlot->getX(), pPreviouslySelectedPlot->getY(), plotX->getX(), plotX->getY())
								)
						);
					}

					//ls612: Make exploring AI aware of terrain damage
					if (plotX->getTotalTurnDamage(getGroup()) > 0 || plotX->getFeatureTurnDamage() > 0)
					{
						iValue /= 5;
					}

					if (iValue > iBestValue)
					{
						//	Does this proposed move leave us exposed at the end of this
						//	turn?  For general exploring moves we accept danger with 60%
						//	combat odds in our favour
						if (exposedToDanger(getPathEndTurnPlot(), 60))
						{
							//	For now just reject this.  Might want to do a combat odds check ideally
							candidatesRejectedForMoveSafety = true;
							continue;
						}
						iBestValue = iValue;
						if (getDomainType() == DOMAIN_LAND)
						{
							pBestPlot = getPathEndTurnPlot();
						}
						else pBestPlot = plotX;

						pBestExplorePlot = plotX;
					}
				}
			}
		}
	}

	if (pBestPlot && pBestExplorePlot)
	{
		PROFILE("AI_exploreRange 4");

		FAssert(!atPlot(pBestPlot));
		if (!atPlot(pBestPlot))
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_NO_ENEMY_TERRITORY | MOVE_HEAL_AS_NEEDED25, false, false, MISSIONAI_EXPLORE, pPreviouslySelectedPlot != NULL ? pPreviouslySelectedPlot : pBestExplorePlot);
		else if (!atPlot(pBestExplorePlot))
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestExplorePlot->getX(), pBestExplorePlot->getY(), MOVE_NO_ENEMY_TERRITORY | MOVE_HEAL_AS_NEEDED25, false, false, MISSIONAI_EXPLORE, pPreviouslySelectedPlot != NULL ? pPreviouslySelectedPlot : pBestExplorePlot);
	}
	if (candidatesRejectedForMoveSafety)
	{
		return AI_safety(); // Just stay safe for a while
	}
	return false;
}

//	Determine if there is a threatening unit at the spcified plot.
//	Return a pointer to the unit if so (NULL with a detected threat means multiple units)
bool CvUnitAI::getThreateningUnit(const CvPlot* pPlot, CvUnit*& pThreateningUnit, const CvPlot* pAttackPlot, int& iIndex, bool bReturnWorstOfMultiple) const
{
	PROFILE_EXTRA_FUNC();
	int iOdds = 0;
	int iWorstOdds = 0;
	int iI = 0;

	pThreateningUnit = NULL;

	if (bReturnWorstOfMultiple && iIndex != 0)
	{
		return false;
	}

	foreach_(CvUnit * pLoopUnit, pPlot->units())
	{
		//	Don't count animals if the target plot is owned since they cannot attack there
		if (isEnemy(pLoopUnit->getTeam(), NULL, pLoopUnit) && pLoopUnit->canAttack() && (!pLoopUnit->isAnimal() || (pLoopUnit->isAnimal() && pLoopUnit->canAnimalIgnoresBorders()) || pAttackPlot->getOwner() == NO_PLAYER))
		{
			if (bReturnWorstOfMultiple)
			{
				if (pPlot == pAttackPlot)
				{
					iOdds = 100 - ((CvUnitAI*)this)->AI_attackOddsAtPlot(pPlot, pLoopUnit);
				}
				else
				{
					iOdds = pLoopUnit->AI_attackOddsAtPlot(pPlot, (CvUnitAI*)this);
				}

				if (iOdds > iWorstOdds)
				{
					iWorstOdds = iOdds;
					pThreateningUnit = pLoopUnit;

					iIndex++;
				}
			}
			else
			{
				if (iI++ == iIndex)
				{
					iIndex++;

					pThreateningUnit = pLoopUnit;
					return true;
				}
			}
		}
	}

	return (pThreateningUnit != NULL);
}

bool CvUnitAI::exposedToDanger(const CvPlot* pPlot, int acceptableOdds, bool bConsiderOnlyWorstThreat) const
{
    StackPromotions::EvaluationScope aidEvaluation(true);
	PROFILE_EXTRA_FUNC();
	if (GET_PLAYER(getOwner()).AI_getPlotDanger(pPlot, 1, false))
	{
		//	What would the odds be?
		//	We use a cheapskate heuristic if there are multiple threatening
		//	units and just assume that any more than 1 is a no-no
		CvUnit* threateningUnit = NULL;
		int iOurOdds = 100;
		int iIndex;

		int iOurValue = 0;
		int iEnemyTotalValueExpectedLoss = 0;
		CvUnit* pOurAttacker = NULL;

		//	If we're not currently at the designated plot check it first on the assumption we'll want to
		//	attack into it if we are capable of so doing
		if (pPlot != plot() && pPlot->isVisibleEnemyUnit(this))
		{
			iIndex = 0;

			//	Evaluate based on our best attacker
			int dummy;
			pOurAttacker = getGroup()->AI_getBestGroupAttacker(pPlot, true, dummy);

			if (!pOurAttacker)
			{
				//	We cannot attack here
				return true;
			}
			iOurValue += pOurAttacker->AI_genericUnitValueTimes100(UNITVALUE_FLAGS_DEFENSIVE | UNITVALUE_FLAGS_OFFENSIVE);

			while (getThreateningUnit(pPlot, threateningUnit, pPlot, iIndex, bConsiderOnlyWorstThreat))
			{
				int iOdds = ((CvUnitAI*)pOurAttacker)->AI_attackOddsAtPlot(pPlot, threateningUnit);

				//	AI_attackOddsAtPlot returns a value capped artificially at 99, but for the ensuing calculation
				//	that causes nasty rounding errors so we just treat 99 as if it were certainty
				if (iOdds == 99)
				{
					iOdds = 100;
				}
				//	Survival probability is probability of surviving up to this (iOurOdds
				//	up to this point) * odds of surving this encounter, which we normalize
				//	from the base (100-iOdds) by a factor of iOurOdds to reflect likely damage taken
				iOurOdds = (iOurOdds * iOdds * iOurOdds) / 10000;

				if (iOurOdds > 50)
				{
					int iEnemyValue = threateningUnit->AI_genericUnitValueTimes100(UNITVALUE_FLAGS_DEFENSIVE | UNITVALUE_FLAGS_OFFENSIVE);

					//	Only count one kill - this routine is mostly geared towards single unit stacks so we'll
					//	only normally be actually attacking once
					if (iEnemyValue > iEnemyTotalValueExpectedLoss)
					{
						iEnemyTotalValueExpectedLoss = iEnemyValue;
					}
				}

				if (bConsiderOnlyWorstThreat)
				{
					break;
				}
			}
		}

		CvUnit* pOurDefender = ((CvSelectionGroupAI*)getGroup())->AI_findBestDefender(pPlot, true);

		if (pOurDefender != NULL)
		{
			if (pOurAttacker != pOurDefender)
			{
				iOurValue += pOurDefender->AI_genericUnitValueTimes100(UNITVALUE_FLAGS_DEFENSIVE | UNITVALUE_FLAGS_OFFENSIVE);
			}
		}

		for (int iI = 0; iI < NUM_DIRECTION_TYPES && iOurOdds >= acceptableOdds; iI++)
		{
			CvPlot* pAdjacentPlot;

			pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL)
			{
				if (AI_plotValid(pAdjacentPlot))
				{
					CvUnit* pUnit;

					iIndex = 0;

					while (getThreateningUnit(pAdjacentPlot, pUnit, pPlot, iIndex, bConsiderOnlyWorstThreat))
					{
						FAssert(pUnit != NULL);

						if (!isInvisible(pUnit->getTeam(), false))
						{
							int iOdds = pUnit->AI_attackOddsAtPlot(pPlot, (CvUnitAI*)pOurDefender);

							//	AI_attackOddsAtPlot returns a value capped artificially below at 1, but for the ensuing calculation
							//	that causes nasty rounding errors so we just treat 1 as if it were certainty of failure
							if (iOdds == 1)
							{
								iOdds = 0;
							}

							if (bConsiderOnlyWorstThreat)
							{
								if (threateningUnit != NULL)
								{
									if (iOdds > 100 - iOurOdds)
									{
										threateningUnit = pUnit;
										iOurOdds = 100 - iOdds;
									}
								}
								else
								{
									iOurOdds = 100 - iOdds;
								}

								break;
							}
							else
							{
								//	Survival probability is probability of surviving up to this (iOurOdds
								//	up to this point) * odds of surving this encounter, which we normalize
								//	from the base (100-iOdds) by a factor of iOurOdds to reflect likely damage taken
								iOurOdds = (iOurOdds * (100 - iOdds) * iOurOdds) / 10000;

								//	Since all enemies can attack us, count them all
								if (iOurOdds > 50)
								{
									iEnemyTotalValueExpectedLoss += pUnit->AI_genericUnitValueTimes100(UNITVALUE_FLAGS_DEFENSIVE | UNITVALUE_FLAGS_OFFENSIVE);
								}
							}
						}
					}
				}
			}
		}

		if (iOurOdds < acceptableOdds && iEnemyTotalValueExpectedLoss >= iOurValue)
		{
			//	If we would claim this is too dangerous adjust for relative losses so that
			//	things that leave us likely to die only if we take a lot of enemies with us
			//	are considered ok - note that we only apply this adjustment to single unit
			//	stacks, since use of this routine with  multi-unit stacks is limited to situations
			//	where escorts (of defenseless, but relatively important) units is involved
			if (getGroup()->getNumUnits() == 1)
			{
				iOurOdds += ((100 - iOurOdds) * (iEnemyTotalValueExpectedLoss - iOurValue)) / (2 * std::max(1, iOurValue));
			}
		}
		return (iOurOdds < acceptableOdds);
	}
	return false;
}


typedef struct
{
	int		iValue;
	CvCity* pCity;
} targetInfo;
#define	MAX_CLOSE_TARGET_DISTANCE	5

/************************************************************************************************/
/* BETTER_BTS_AI_MOD					  03/29/10								jdog5000	  */
/*																							  */
/* War tactics AI, Efficiency																   */
/************************************************************************************************/
// Returns target city
bool CvUnitAI::AI_isCachedTargetCityValid(const CvCity* city, bool huntBarbarians)
{
	// Shared candidate history is only a hint. A city can have been captured,
	// razed or made friendly since another army selected it this turn.
	return city != NULL && GET_PLAYER(city->getOwner()).isAlive()
		&& (huntBarbarians || city->getOwner() < MAX_PC_PLAYERS)
		&& ::isPotentialEnemy(getTeam(), city->getTeam())
		&& city->plot()->isRevealed(getTeam(), false) && !atPlot(city->plot())
		&& (getDomainType() != DOMAIN_LAND || UnitTravelConnectivity::landUnitCanReach(this,city->plot(),true)
			&& AI_potentialEnemy(city->getTeam(), city->plot()));
}

CvCity* CvUnitAI::AI_pickTargetCity(int iFlags, int iMaxPathTurns, bool bHuntBarbs)
{
	PROFILE_FUNC();
	// Scratch geometry dies before the selected mission can change the world.
	ReadOnlyLandReachability pathFeasibility;

	static std::map<CvPlot*, bool>* cachedTargets = NULL;

	/*
	CvCity* pTargetCity = area()->getTargetCity(getOwner());

	// Don't always go after area target ... don't know how far away it is
	if (pTargetCity != NULL
	&& AI_potentialEnemy(pTargetCity->getTeam(), pTargetCity->plot())
	&& !atPlot(pTargetCity->plot())
	&& generatePath(pTargetCity->plot(), iFlags, true))
	{
		pBestCity = pTargetCity;
	}
	*/
	if (cachedTargets == NULL)
	{
		cachedTargets = new std::map<CvPlot*, bool>();
	}

	if (eCachedTargetCityPlayer != getOwner())
	{
		eCachedTargetCityPlayer = getOwner();
		cachedTargets->clear();
	}

	CvCity* pBestCity = NULL;
	//Get the already calculated Cities, from the cache
	for (std::map<CvPlot*, bool>::const_iterator itr = cachedTargets->begin(); itr != cachedTargets->end(); ++itr)
	{
		CvCity* possibleTargetCity = itr->first->getPlotCity();
		int idist = stepDistance(getX(), getY(), itr->first->getX(), itr->first->getY());
		if (AI_isCachedTargetCityValid(possibleTargetCity, bHuntBarbs))
		{
				if(idist <= 2 || itr->first->area() == area() && idist < AI_searchRange(iMaxPathTurns)
				&& ReadOnlyUnitPathFilter::path(this,itr->first,iFlags,NULL,MAX_CLOSE_TARGET_DISTANCE,pathFeasibility))
				{
					pBestCity = possibleTargetCity;
					break;
				}
		}
	}

	CvReachablePlotSet plotSet(getGroup(), iFlags);

	if (iMaxPathTurns != MAX_INT)
	{
		plotSet.Populate(AI_searchRange(iMaxPathTurns));
	}

	std::multimap<int, targetInfo> possibleTargets;

	if (pBestCity == NULL)
	{
		int idist = MAX_INT;
		for (int iI = 0; iI < (bHuntBarbs ? MAX_PLAYERS : MAX_PC_PLAYERS); iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive() && ::isPotentialEnemy(getTeam(), GET_PLAYER((PlayerTypes)iI).getTeam()))
			{
				foreach_(CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
				{
					int idist = stepDistance(getX(), getY(), pLoopCity->getX(), pLoopCity->getY());
					if
						(
							(
								iMaxPathTurns == MAX_INT && idist < AI_searchRange(iMaxPathTurns)
								||
								iMaxPathTurns != MAX_INT && plotSet.find(pLoopCity->plot()) != plotSet.end()
								)
						&&
							pLoopCity->plot()->isRevealed(getTeam(), false)
						&&
							(getDomainType() != DOMAIN_LAND || UnitTravelConnectivity::landUnitCanReach(this,pLoopCity->plot(),true)
								&&
								AI_potentialEnemy(GET_PLAYER((PlayerTypes)iI).getTeam(), pLoopCity->plot())
								&&
								!atPlot(pLoopCity->plot()))
						)
					{
						int iValue =
							(
								AI_getUnitAIType() == UNITAI_ATTACK_CITY
								?
								GET_PLAYER(getOwner()).AI_targetCityValue(pLoopCity, false, false)
								:
								GET_PLAYER(getOwner()).AI_targetCityValue(pLoopCity, true, true)
							);

						if (area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE)
						{
							iValue *= 50 + pLoopCity->calculateCulturePercent(getOwner());
							iValue /= 50;
						}

						if (idist <= 2) //If very Close, Boost the value
						{
							iValue *= 3;
							iValue /= 2;
						}

						iValue *= 1000;

						// If city is minor civ, less interesting
						if (GET_PLAYER(pLoopCity->getOwner()).isMinorCiv() || GET_PLAYER(pLoopCity->getOwner()).isNPC())
						{
							iValue /= 2;
						}

						if (!pLoopCity->plot()->isCity(false))
						{
							iValue /= 4;  //Super fort are much less important
						}

						if (atPlot(pLoopCity->plot()))
						{
							iValue = 0;
						}
						else
						{
							iValue = applyDistanceScoringFactor(iValue, plot(), pLoopCity->plot(), 1);
						}

						if (iValue > 0)
						{
							targetInfo info;

							info.iValue = iValue;
							info.pCity = pLoopCity;

							const int iDistanceEstimate = (stepDistance(getX(), getY(), pLoopCity->getX(), pLoopCity->getY()) + 2) / 2;
							const int iOrderingValue = -iValue;

							possibleTargets.insert(std::make_pair(iOrderingValue, info));
						}
					}
				}
			}
		}
	}

	bool bVerifiedPathing = false;

	if (pBestCity == NULL)
	{
		// Following loop verifies
		bVerifiedPathing = true;
		int iBestValue = 0;

		for (std::multimap<int, targetInfo>::const_iterator itr = possibleTargets.begin(); itr != possibleTargets.end(); ++itr)
		{
			CvCity* pLoopCity = itr->second.pCity;
			int iMaxPath;

			iMaxPath = iMaxPathTurns;

			if (iMaxPath > 0)
			{
				PROFILE("AI_pickTargetCity.PrePathing");
				int iPathTurns = 0;
				int idist = stepDistance(pLoopCity->getX(), pLoopCity->getY(), getX(), getY());
				bool bpath = ReadOnlyUnitPathFilter::path(this,pLoopCity->plot(),iFlags,&iPathTurns,iMaxPath,pathFeasibility);
				if (idist <= 2) iPathTurns = 1;
				if (bpath || (idist == 1 && getGroup()->canBombard(plot(), true)))
				{
					PROFILE("AI_pickTargetCity.PostPathing");

					// If city is visible and our force already in position is dominantly powerful or we have a huge force
					// already on the way, pick a different target
					if (iPathTurns > 2 && pLoopCity->isVisible(getTeam(), false))
					{

						int iOurOffense = GET_TEAM(getTeam()).AI_getOurPlotStrength(pLoopCity->plot(),2,false,false,true);
						int iGroupStrength = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 1, false, false, true);
						int iEnemyDefense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(pLoopCity->plot(),1,true,false);

						//too weak
						if (2 * (iGroupStrength + iOurOffense) <= GC.getBBAI_SKIP_BOMBARD_BASE_STACK_RATIO() * iEnemyDefense)
						{
							continue;
						}


						if (GET_PLAYER(getOwner()).AI_cityTargetUnitsByPath(pLoopCity, getGroup(), iPathTurns) > std::max(6, 3 * pLoopCity->plot()->getNumVisiblePotentialEnemyDefenders(this)))
						{
							continue;
						}
					}

					// For barbarians, if the city is distant, build a larger stack first than we
					// would if it were close by
					if (isNPC() && getGroup()->getNumUnits() < 3 + iPathTurns / 5)
					{
						continue;
					}

					const int iValue = itr->second.iValue; // (4 + iPathTurns * iPathTurns);

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestCity = pLoopCity;
						iMaxPathTurns = iPathTurns * 2;
						// Candidates descend by this exact score; ties never replace
						// the first winner. No later path can improve the choice.
						break;
					}
				}
			}
		}
	}

	if (pBestCity != NULL)
	{
		if (!bVerifiedPathing && !ReadOnlyUnitPathFilter::path(this,pBestCity->plot(),iFlags,NULL,-1,pathFeasibility))
		{
			return NULL;
		}
		LOG_UNIT_BLOCK(3, {
			UnitAITypes eUnitAi = AI_getUnitAIType();
			MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
			CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
			CvWString MissionInfos = MissionAITypeToString(eMissionAI);
			CvWString StrUnitName = m_szName;
			if (StrUnitName.length() == 0)
			{
				StrUnitName = getName(0).GetCString();
			}

			logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], picks target city at (%d,%d)...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits(), pBestCity->getX(), pBestCity->getY());
		});

		(*cachedTargets)[pBestCity->plot()] = true;
	}
	return pBestCity;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_goToTargetCity(int iFlags, int iMaxPathTurns, const CvCity* pTargetCity)
{
	PROFILE_FUNC();
	// Scratch geometry dies before the selected mission can change the world.
	ReadOnlyLandReachability pathFeasibility;


	if (pTargetCity == NULL)
	{
		pTargetCity = AI_pickTargetCity(iFlags, iMaxPathTurns);
	}

	if (pTargetCity != NULL)
	{
		PROFILE("CvUnitAI::AI_targetCity plot attack");
		int iBestValue = 0;
		const CvPlot* pBestPlot = NULL;

		if (0 == (iFlags & MOVE_THROUGH_ENEMY))
		{
			bool bAttemptWithoutMovingThroughEnemy = true;
			bool bNeedToPathThroughEnemy = false;

			do
			{
				foreach_(const CvPlot * pAdjacentPlot, pTargetCity->plot()->adjacent())
				{
					int iPathTurns;
					if (AI_plotValid(pAdjacentPlot) && !pAdjacentPlot->isVisibleEnemyUnit(this)
					&& ReadOnlyUnitPathFilter::path(this,pAdjacentPlot,iFlags,&iPathTurns,-1,pathFeasibility))
					{
						int iValue = std::max(0, pAdjacentPlot->defenseModifier(getTeam(), false) + 100);

						if (!pAdjacentPlot->isRiverCrossing(directionXY(pAdjacentPlot, pTargetCity->plot())))
						{
							iValue -= 12 * GC.getRIVER_ATTACK_MODIFIER();
						}

						if (!isEnemy(pAdjacentPlot->getTeam(), pAdjacentPlot))
						{
							iValue += 100;
						}

						if (atPlot(pAdjacentPlot))
						{
							iValue += 50;
						}

						iValue = std::max(1, iValue);

						iValue *= 1000;

						iValue /= (iPathTurns + 1);

						if (iValue > iBestValue)
						{
							const CvPlot* endTurnPlot = getPathEndTurnPlot();
							bool bAcceptable;

							if (pAdjacentPlot == endTurnPlot)
							{
								bAcceptable = true;
							}
							else if (getGroup()->getNumUnits() < 3)
							{
								bAcceptable = !exposedToDanger(endTurnPlot, 60);
							}
							else
							{
								const int iTotalEnemyStrength = endTurnPlot->getVisibleEnemyStrength(getOwner(), 1);
								const int iOurStrength = plot()->plotStrength((UnitValueFlags)(UNITVALUE_FLAGS_OFFENSIVE | UNITVALUE_FLAGS_DEFENSIVE), NULL, -1, -1, getOwner());

								bAcceptable = (iOurStrength > 2 * iTotalEnemyStrength);
							}

							if (bAcceptable)
							{
								iBestValue = iValue;
								pBestPlot = endTurnPlot;
							}
						}
					}
				}

				//	If we failed to find any path without moving through enemy units retry
				//	accepting we might have to
				bAttemptWithoutMovingThroughEnemy = !bAttemptWithoutMovingThroughEnemy;
				if (pBestPlot == NULL)
				{
					iFlags |= MOVE_THROUGH_ENEMY;
					bNeedToPathThroughEnemy = true;
				}
			} while (!bAttemptWithoutMovingThroughEnemy && pBestPlot == NULL);

			if (bNeedToPathThroughEnemy && pBestPlot != NULL)
			{
				//	TODO - add some sort of sanity check that we can reasonably expect
				//	to get through them!
				const CvPath& kPath = getGroup()->getPath();

				for (CvPath::const_iterator itr = kPath.begin(); itr != kPath.end() && itr.plot() != pTargetCity->plot(); ++itr)
				{
					const CvPlot* pPathPlot = itr.plot();

					if (pPathPlot->isVisibleEnemyUnit(getOwner()))
					{
						bool bWin = false;
						int iExpectedGainOdds = getGroup()->AI_attackOdds(pPathPlot, true, false, &bWin);

						if (iExpectedGainOdds < 50)
						{
							if (!bWin)
							{
								//	Blockade is too strong to beat
								pBestPlot = NULL;
							}
							else
							{
								const int iStackRatio = getGroup()->AI_compareStacks(pPathPlot, StackCompare::PotentialEnemy);

								//	If we won, but with low expected gain odds it might still be worthwhile
								//	to break the bottleneck - renormalize by the ratio of starting stack
								//	strengths.  This will allow a very numerous stack to plough through
								//	a small, but strong garrison even though doing so is locally a loss
								iExpectedGainOdds *= iStackRatio;
								iExpectedGainOdds /= 100;

								if (iExpectedGainOdds < 50)
								{
									//	Really not worth it
									pBestPlot = NULL;
								}
							}
						}
					}
				}
			}
		}
		else
		{
			pBestPlot = pTargetCity->plot();
		}

		if (pBestPlot != NULL)
		{
			FAssert(!(pTargetCity->at(pBestPlot)) || 0 != (iFlags & MOVE_THROUGH_ENEMY)); // no suicide missions...
			if (!atPlot(pBestPlot))
			{
				LOG_UNIT_BLOCK(3, {
					UnitAITypes eUnitAi = AI_getUnitAIType();
					MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
					CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
					CvWString MissionInfos = MissionAITypeToString(eMissionAI);
					CvWString StrUnitName = m_szName;
					if (StrUnitName.length() == 0)
					{
						StrUnitName = getName(0).GetCString();
					}

					logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], move to target city at (%d,%d)...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits(), pTargetCity->getX(), pTargetCity->getY());
				});
				return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), iFlags);
			}
			else
			{
				if (pBestPlot->isCity() && (!pBestPlot->getPlotCity()->AI_isDefended()))
				{
					CvUnit* pEjectedUnit = getGroup()->AI_ejectBestDefender(pBestPlot);

					if (pEjectedUnit != NULL)
					{
						pEjectedUnit->getGroup()->pushMission(MISSION_SKIP);
						pEjectedUnit->AI_setUnitAIType(UNITAI_CITY_DEFENSE);
						pEjectedUnit->AI_setAsGarrison(pBestPlot->getPlotCity());
					}
				}


			}
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_goToTargetBarbCity(int iMaxPathTurns)
{
	PROFILE_FUNC();
	// Scratch geometry dies before the selected mission can change the world.
	ReadOnlyLandReachability pathFeasibility;

	int iPathTurns;
	int iBestValue = 0;
	const CvCity* pBestCity = NULL;
	const PlayerTypes eOwner = getOwner();

	for (int iI = MAX_PC_PLAYERS; iI < MAX_PLAYERS; iI++)
	{
		if (iI == eOwner)
		{
			continue;
		}
		foreach_(const CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
		{
			if (AI_plotValid(pLoopCity->plot()))
			{
				// BBAI efficiency: check area for land units before generating path
				if (getDomainType() == DOMAIN_LAND && !UnitTravelConnectivity::landUnitCanReach(this,pLoopCity->plot(),true) && !getGroup()->canMoveAllTerrain())
				{
					continue;
				}
				if (pLoopCity->isRevealed(getTeam(), false)
				&& !atPlot(pLoopCity->plot())
				&& ReadOnlyUnitPathFilter::path(this,pLoopCity->plot(),0,&iPathTurns,-1,pathFeasibility)
				&& iPathTurns < iMaxPathTurns)
				{
					const int iValue = 1000 * GET_PLAYER(eOwner).AI_targetCityValue(pLoopCity, false) / (iPathTurns + 1);

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestCity = pLoopCity;
					}
				}
			}
		}
	}

	if (pBestCity != NULL)
	{
		iBestValue = 0;
		const CvPlot* pBestPlot = NULL;

		foreach_(const CvPlot * pAdjacentPlot, pBestCity->plot()->adjacent())
		{
			if (AI_plotValid(pAdjacentPlot))
			{
				if (!pAdjacentPlot->isVisibleEnemyUnit(this))
				{
					if (generatePath(pAdjacentPlot, 0, true, &iPathTurns, iMaxPathTurns))
					{
						int iValue = std::max(0, (pAdjacentPlot->defenseModifier(getTeam(), false) + 100));

						if (!(pAdjacentPlot->isRiverCrossing(directionXY(pAdjacentPlot, pBestCity->plot()))))
						{
							iValue += (10 * -(GC.getRIVER_ATTACK_MODIFIER()));
						}

						iValue = std::max(1, iValue);

						iValue *= 1000;

						iValue /= (iPathTurns + 1);

						if (iValue > iBestValue)
						{
							CvPlot* endTurnPlot = getPathEndTurnPlot();

							if (!exposedToDanger(endTurnPlot, 60))
							{
								iBestValue = iValue;
								pBestPlot = endTurnPlot;
							}
						}
					}
				}
			}
		}

		if (pBestPlot != NULL)
		{
			FAssert(!(pBestCity->at(pBestPlot))); // no suicide missions...
			if (atPlot(pBestPlot))
			{
				getGroup()->pushMission(MISSION_SKIP);
				return true;
			}
			else
			{
				return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
			}
		}
	}
	return false;
}

bool CvUnitAI::AI_pillageAroundCity(const CvCity* pTargetCity, int iBonusValueThreshold, int iMaxPathTurns)
{
	PROFILE_FUNC();

	int iBestValue = 0;
	CvPlot* endTurnPlot = NULL;
	CvPlot* pBestPlot = NULL;
	const CvPlot* pBestPillagePlot = NULL;

	foreach_(const CvPlot * pLoopPlot, pTargetCity->plots(NUM_CITY_PLOTS))
	{
		if (AI_plotValid(pLoopPlot) && !(pLoopPlot->isNPC()))
		{
			if (potentialWarAction(pLoopPlot) && (pLoopPlot->getTeam() == pTargetCity->getTeam()))
			{
				if (getGroup()->canPillage(pLoopPlot))
				{
					if (!pLoopPlot->isVisibleEnemyUnit(this))
					{
						if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_PILLAGE, getGroup()) == 0)
						{
							int iPathTurns;
							if (generatePath(pLoopPlot, 0, true, &iPathTurns, iMaxPathTurns))
							{
								int iValue = AI_pillageValue(pLoopPlot, iBonusValueThreshold);

								iValue *= 1000 + 30 * (pLoopPlot->defenseModifier(getTeam(), false));

								iValue /= (iPathTurns + 1);

								// if not at war with this plot owner, then devalue plot if we already inside this owner's borders
								// (because declaring war will pop us some unknown distance away)
								if (!isEnemy(pLoopPlot->getTeam()) && plot()->getTeam() == pLoopPlot->getTeam())
								{
									iValue /= 10;
								}

								if (iValue > iBestValue)
								{
									endTurnPlot = getPathEndTurnPlot();

									if (!exposedToDanger(endTurnPlot, 60))
									{
										iBestValue = iValue;
										pBestPlot = endTurnPlot;
										pBestPillagePlot = pLoopPlot;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestPillagePlot != NULL))
	{
		if (atPlot(pBestPillagePlot) && !isEnemy(pBestPillagePlot->getTeam()))
		{
			//getGroup()->groupDeclareWar(pBestPillagePlot, true);
			// rather than declare war, just find something else to do, since we may already be deep in enemy territory
			return false;
		}

		if (atPlot(pBestPillagePlot))
		{
			if (isEnemy(pBestPillagePlot->getTeam()))
			{
				getGroup()->pushMission(MISSION_PILLAGE, -1, -1, 0, false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
				return true;
			}
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
		}
	}

	return false;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_bombardCity()
{
	PROFILE_FUNC();

	//	Replaced REV implementation with K-mod simplified one
	//	[FUTURE] - may want to use contract brokerage to get bombers and ships to bombard first
	if (getGroup()->canBombard(plot()))
	{
		CvCity* pBombardCity = bombardTarget(plot());

		if (pBombardCity != NULL)
		{
			int iAttackOdds = getGroup()->AI_attackOdds(pBombardCity->plot(), true);
			int iBase = GC.getBBAI_SKIP_BOMBARD_BASE_STACK_RATIO();
			int iMin = GC.getBBAI_SKIP_BOMBARD_MIN_STACK_RATIO();
			int iBombardTurns = getGroup()->getBombardTurns(pBombardCity);
			int iThreshold = (iBase * (100 - iAttackOdds) + (1 + iBombardTurns / 2) * iMin * iAttackOdds) / (100 + (iBombardTurns / 2) * iAttackOdds);
			int iComparison = getGroup()->AI_compareStacks(pBombardCity->plot(), StackCompare::CheckCanAttack | StackCompare::CheckCanMove | StackCompare::PotentialEnemy);

			if (iComparison > iThreshold && (pBombardCity->isDirectAttackable() || canIgnoreNoEntryLevel()))
			{
				if (gUnitLogLevel > 2)
				{
					logBBAI("	  Stack skipping bombard of %S with compare %d, starting odds %d, bombard turns %d, threshold %d", pBombardCity->getName().GetCString(), iComparison, iAttackOdds, iBombardTurns, iThreshold);
				}
			}
			else
			{
				LOG_UNIT_BLOCK(3, {
					UnitAITypes eUnitAi = AI_getUnitAIType();
					MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
					CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
					CvWString MissionInfos = MissionAITypeToString(eMissionAI);
					CvWString StrUnitName = m_szName;
					if (StrUnitName.length() == 0)
					{
						StrUnitName = getName(0).GetCString();
					}

					logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], will perform bombard at (%d, %d)...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits(), pBombardCity->plot()->getX(), pBombardCity->plot()->getY());
				});
				return getGroup()->pushMissionInternal(MISSION_BOMBARD);
			}
		}
		//	Super forts - handle fort improvements
		else
		{
			CvPlot* pTargetPlot = bombardImprovementTarget(plot());

			if (pTargetPlot != NULL)
			{
				// do not bombard forts with no defenders
				int iDefenderStrength = pTargetPlot->AI_sumStrength(NO_PLAYER, getOwner(), DOMAIN_LAND, StrengthFlags::DefensiveBonuses | StrengthFlags::TestAtWar);
				if (iDefenderStrength == 0)
				{
					return false;
				}

				// do not bombard cities if we have overwelming odds
				int iAttackOdds = getGroup()->AI_attackOdds(pTargetPlot, /*bPotentialEnemy*/ true);
				if (iAttackOdds > 95)
				{
					return false;
				}

				// could also do a compare stacks call here if we wanted, the downside of that is that we may just have a lot more units
				// we may not want to suffer high casualties just to save a turn
				//getGroup()->AI_compareStacks(pBombardCity->plot(), /*bPotentialEnemy*/ true, /*bCheckCanAttack*/ true, /*bCheckCanMove*/ true);
				//int iOurStrength = pBombardCity->plot()->AI_sumStrength(getOwner(), NO_PLAYER, DOMAIN_LAND, false, false, false)

				if (pTargetPlot->getDefenseDamage() < ((GC.getImprovementInfo(pTargetPlot->getMilitaryImprovementType()).getDefenseModifier() * 3) / 4))
				{
					return getGroup()->pushMissionInternal(MISSION_BOMBARD);
				}
			}
		}
		// Super Forts end
	}

	return false;

	//#if 0
	//	CvCity* pBombardCity;
	//
	//	if (canBombard(plot()))
	//	{
	//		pBombardCity = bombardTarget(plot());
	//		FAssertMsg(pBombardCity != NULL, "BombardCity is not assigned a valid value");
	//
	//		// do not bombard cities with no defenders
	//		int iDefenderStrength = pBombardCity->plot()->AI_sumStrength(NO_PLAYER, getOwner(), DOMAIN_LAND, /*bDefensiveBonuses*/ true, /*bTestAtWar*/ true, false);
	//		if (iDefenderStrength == 0)
	//		{
	//			return false;
	//		}
	//
	//		// do not bombard cities if we have overwelming odds
	//		int iAttackOdds = getGroup()->AI_attackOdds(pBombardCity->plot(), /*bPotentialEnemy*/ true);
	//		if ((iAttackOdds > 95))
	//		{
	//			return false;
	//		}
	//
	//		// If we have reasonable odds, check for attacking without waiting for bombards
	//		if ((iAttackOdds >= getBBAI_SKIP_BOMBARD_BEST_ATTACK_ODDS())
	//		{
	//			int iBase = GC.getBBAI_SKIP_BOMBARD_BASE_STACK_RATIO();
	//				int iComparison = getGroup()->AI_compareStacks(pBombardCity->plot(), /*bPotentialEnemy*/ true, /*bCheckCanAttack*/ true, /*bCheckCanMove*/ true);
	//
	//				// Big troop advantage plus pretty good starting odds, don't wait to allow reinforcements
	//				if (iComparison > (iBase - 4 * iAttackOdds))
	//				{
	//					if (gUnitLogLevel > 2) logBBAI("	  Stack skipping bombard of %S with compare %d and starting odds %d", pBombardCity->getName().GetCString(), iComparison, iAttackOdds);
	//						return false;
	//				}
	//
	//			int iMin = GC.getBBAI_SKIP_BOMBARD_MIN_STACK_RATIO();
	//			bool bHasWaited = algo::any_of(getGroup()->units(), CvUnit::fn::getFortifyTurns() > 0)
	//
	//				// Bombard at least one turn to allow bombers/ships to get some shots in too
	//				if (bHasWaited && (pBombardCity->getDefenseDamage() > 0))
	//				{
	//					int iBombardTurns = getGroup()->getBombardTurns(pBombardCity);
	//					if (iComparison > std::max(iMin, iBase - 3 * iAttackOdds - 3 * iBombardTurns))
	//					{
	//						if (gUnitLogLevel > 2) logBBAI("	  Stack skipping bombard of %S with compare %d, starting odds %d, and bombard turns %d", pBombardCity->getName().GetCString(), iComparison, iAttackOdds, iBombardTurns);
	//						return false;
	//					}
	//				}
	//		}
	//
	//		//getGroup()->pushMission(MISSION_PILLAGE);
	//		getGroup()->pushMission(MISSION_BOMBARD);
	//		return true;
	//		/************************************************************************************************/
	//		/* REVOLUTIONDCM							05/24/08								Glider1	 */
	//		/*																							  */
	//		/*																							  */
	//		/************************************************************************************************/
	//				// RevolutionDCM - ranged bombard AI wraps standard bombard
	//		if (!AI_RbombardCity(pBombardCity))
	//		{
	//			// vanilla behaviour
	//			getGroup()->pushMission(MISSION_BOMBARD);
	//			return true;
	//		}
	//		// RevolutionDCM - end
	///************************************************************************************************/
	///* REVOLUTIONDCM							END									 Glider1	 */
	///************************************************************************************************/
	//	}
	//
	//	return false;
	//#endif
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					   END												  */
/************************************************************************************************/


// Returns true if a mission was pushed...
#include "CityAssaultOutcome.h"

bool CvUnitAI::AI_cityAttack(int iRange, int iOddsThreshold, bool bFollow)
{
	PROFILE_FUNC();
	FAssert(canMove());

	// Reaching an approach plot does not make a group capable of assault.
	// A defensive head may lead real attackers, so test members, not the head.
	bool hasMobileAttacker = false;
	foreach_(const CvUnit* unit, getGroup()->units())
	{
		if (unit->canAttack() && unit->canMove())
		{
			hasMobileAttacker = true;
			break;
		}
	}
	if (!hasMobileAttacker) return false;

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	const int iSearchRange = bFollow ? 1 : AI_searchRange(iRange);

	foreach_(const CvPlot * plotX, plot()->rect(iSearchRange, iSearchRange))
	{
		if (AI_plotValid(plotX)
		&& plotX->isCity(true)
		&& !atPlot(plotX)
		&& AI_potentialEnemy(plotX->getTeam(), plotX))
		{
			// Do not path towards a city this group cannot legally assault.
			// Attack entry also respects capture/domain/visible-defender rules;
			// ordinary peaceful entry is not evidence that an attack is possible.
			bool hasCityAttacker = false;
			foreach_(const CvUnit* unit, getGroup()->units())
			{
				if (unit->canAttack() && unit->canMove()
				&& cityAssaultHasOutcome(*unit, *plotX, GET_PLAYER(unit->getOwner()).isAnimal())
					&& unit->canEnterPlot(plotX, MoveCheck::Attack | MoveCheck::DeclareWar | MoveCheck::IgnoreLocation))
				{
					hasCityAttacker = true;
					break;
				}
			}
			if (!hasCityAttacker) continue;

			int iPathTurns;
			if (bFollow && canEnterOrAttackPlot(plotX, true)
			|| !bFollow && generatePath(plotX, 0, true, &iPathTurns, iRange) && iPathTurns <= iRange)
			{
				const int iValue = getGroup()->AI_attackOdds(plotX, true);

				if (shouldAvoidLowOddsRiverAttack(*this, plotX, iValue))
				{
					continue;
				}

				if (iValue >= AI_finalOddsThreshold(plotX, iOddsThreshold) && iValue > iBestValue)
				{
					const CvPlot* endTurnPlot = getPathEndTurnPlot();

					if (endTurnPlot == plotX || !exposedToDanger(bFollow ? plotX : endTurnPlot, iOddsThreshold))
					{
						iBestValue = iValue;
						pBestPlot = (bFollow ? plotX : endTurnPlot);
						FAssert(!atPlot(pBestPlot));
					}
				}
			}
		}
	}

	if (pBestPlot)
	{
		LOG_UNIT_BLOCK(3, {
			UnitAITypes eUnitAi = AI_getUnitAIType();
			MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
			CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
			CvWString MissionInfos = MissionAITypeToString(eMissionAI);
			CvWString StrUnitName = m_szName;
			if (StrUnitName.length() == 0)
			{
				StrUnitName = getName(0).GetCString();
			}

			logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Consider Attacking City from plot (%d,%d), with Odds %d...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits(), pBestPlot->getX(), pBestPlot->getY(), iOddsThreshold);
		});
		FAssert(!atPlot(pBestPlot));
		bool iAttackResult = getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), ((bFollow) ? MOVE_DIRECT_ATTACK : 0));

		if (iAttackResult)
		{
			if (plot()->isCity() && (!plot()->getPlotCity()->AI_isDefended()))
			{
				CvUnit* pEjectedUnit = getGroup()->AI_ejectBestDefender(pBestPlot);

				if (pEjectedUnit != NULL)
				{
					pEjectedUnit->getGroup()->pushMission(MISSION_SKIP);
					pEjectedUnit->AI_setUnitAIType(UNITAI_CITY_DEFENSE);
					pEjectedUnit->AI_setAsGarrison(pBestPlot->getPlotCity());
				}
			}
		}

		return iAttackResult;
	}
	return false;
}


bool CvUnitAI::AI_hasLocalAttackCandidate(int iSearchRange, int iOddsThreshold, int iMinStack, bool bAllowCities, AttackCandidateMemo* memo)
{
	TARGET_DIAG_ACCUM(preflightPlots, ATTACK_PREFLIGHT_PLOTS);
	TARGET_DIAG_ACCUM(preflightEmpty, ATTACK_PREFLIGHT_EMPTY);
	// Preflight is read-only and runs after ranged fire / ambush. Ordinary
	// land attacks need a mobile attacker in the whole group, not just entry
	// permission for a defensive leader or a unit able to coexist with enemies.
	const bool landGroup = getDomainType() == DOMAIN_LAND;
	bool hasMobileAttacker = true;
	const bool canClaim = landGroup && canClaimTerritory(NULL);
	if (landGroup)
	{
		hasMobileAttacker = false;
		foreach_(const CvUnit* attacker, getGroup()->units())
		{
			if (attacker->canMove() && attacker->canAttack())
			{
				hasMobileAttacker = true;
				break;
			}
		}
		// No ordinary attack or fort claim is possible. Ambush and ranged
		// fire already ran in AI_anyAttack before entering this read-only scan.
		if (!hasMobileAttacker && !canClaim) return false;
	}
	foreach_(CvPlot* candidate, plot()->rect(iSearchRange, iSearchRange))
	{
		TARGET_DIAG_VISIT(preflightPlots);
		// A definitely empty non-city cannot be a naval target. Do not
		// inspect visibility, thresholds or group entry on these plots.
		if (getDomainType() == DOMAIN_SEA && candidate->getNumUnits() == 0 && !candidate->isCity(true))
		{
			TARGET_DIAG_VISIT(preflightEmpty);
			continue;
		}
		// Apply the selector's exclusions before building any reachability flood,
		// for land groups as well as ships. Fort claims remain independent below.
		if (hasMobileAttacker && (bAllowCities || !candidate->isCity(true))
			&& (candidate->isVisibleEnemyUnit(this)
				|| (candidate->isCity(true) && AI_potentialEnemy(candidate->getTeam())))
			&& (iMinStack <= 0 || candidate->getNumVisibleEnemyTargetUnits(this) >= iMinStack)
			&& getGroup()->canEnterOrAttackPlot(candidate, true))
		{
			if (getDomainType() != DOMAIN_SEA)
			{
				foreach_(const CvUnit* attacker, getGroup()->units())
				{
					if (attacker->canMove() && attacker->canAttack()
						&& attacker->canEnterPlot(candidate, MoveCheck::Attack | MoveCheck::DeclareWar | MoveCheck::IgnoreLocation)) return true;
				}
			}
			else
			{
				// Use the same stack odds and threshold as the final attack selector.
				// A visible but hopeless target does not justify flooding the ocean.
				int value = 0, threshold = 1;
				bool winLikely = true;
				if (candidate->isVisiblePotentialEnemyDefender(this))
				{
					winLikely = false;
					value = getGroup()->AI_attackOdds(candidate, true, false, &winLikely, iOddsThreshold);
					threshold = AI_finalOddsThreshold(candidate, iOddsThreshold);
					if (shouldAvoidLowOddsRiverAttack(*this, candidate, value))
					{
						if (memo) memo->remember(candidate, -1, 0);
						continue;
					}
					if (winLikely && candidate->isCity(false)) value += iOddsThreshold;
				}
				else value = 100 * candidate->getNumVisiblePotentialEnemyDefenderless(this);
				if (candidate->getOwner() == getOwner() && winLikely) value *= 2;
				if (memo) memo->remember(candidate, value, threshold);
				if (value > 0 && value >= threshold) return true;
			}
		}
		if (canClaim && candidate->area() == area())
		{
			const ImprovementTypes improvement = candidate->getImprovementType();
			if (improvement != NO_IMPROVEMENT && GC.getImprovementInfo(improvement).getCulture() > 0
				&& (candidate->getOwner() == NO_PLAYER || potentialWarAction(candidate))
				&& canClaimTerritory(candidate)) return true;
		}
	}
	return false;
}

bool CvUnitAI::AI_anyAttack(int iRange, int iOddsThreshold, int iMinStack, bool bAllowCities, bool bFollow)
{
	TARGET_DIAG_ACCUM(selectorPlots, ATTACK_SELECTOR_PLOTS);
	TARGET_DIAG_ACCUM(selectorEmpty, ATTACK_SELECTOR_EMPTY);
    NAVAL_DIAG_PHASE(ATTACK,iRange,iOddsThreshold,iMinStack,(bAllowCities?1:0)|(bFollow?2:0));

	AI_PERF_SCOPE(ANY_ATTACK);
	PROFILE_FUNC();
	FAssert(canMove());

	if (AI_ambush(iOddsThreshold))
	{
		NAVAL_DIAG_PHASE_RETURN((true));
	}

	if (iRange < 1)
	{
		NAVAL_DIAG_PHASE_RETURN((false));
	}

	if (AI_rangeAttack(iRange))
	{
		NAVAL_DIAG_PHASE_RETURN((true));
	}

	const int iSearchRange = bFollow ? 1 : AI_searchRange(iRange);
	// A city garrison cannot stage a multi-turn offensive: its role will
	// recall it before the next attack update. Only commit to an actual
	// same-turn counterattack; field armies keep their existing staging.
	const CvCity* defendedCity = plot()->getPlotCity();
	const bool immediateGarrisonAttack = getDomainType() == DOMAIN_LAND
		&& defendedCity != NULL && defendedCity->getOwner() == getOwner()
		&& getGroup()->AI_isCityGarrison(defendedCity);

	// Do not build a flood for an empty or non-actionable combat search.
	// Ranged attacks/ambushes have already had their opportunity above.
	AttackCandidateMemo attackMemo;
	if ((AI_isCityAIType() || getDomainType() == DOMAIN_SEA || getDomainType() == DOMAIN_LAND) && !AI_hasLocalAttackCandidate(iSearchRange, iOddsThreshold, iMinStack, bAllowCities, &attackMemo)) NAVAL_DIAG_PHASE_RETURN((false));

	// Do not build a flood or evaluate combat for an empty defensive/naval search.
	// Ranged attacks/ambushes have already had their opportunity above.
	if ((AI_isCityAIType() || getDomainType() == DOMAIN_SEA) && !AI_hasLocalAttackCandidate(iSearchRange, iOddsThreshold, iMinStack, bAllowCities)) return false;

	CvReachablePlotSet plotSet(getGroup(), MOVE_THROUGH_ENEMY, iSearchRange);

	if (getDomainType() == DOMAIN_LAND && AI_claimForts(&plotSet, 0, iRange + 1))
	{
		NAVAL_DIAG_PHASE_RETURN((true));
	}
	int iPathTurns;
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		FAssert(itr.stepDistance() <= iSearchRange);

		if (iMinStack == 0 && bAllowCities && !bFollow
		// Have we already considered this plot in a previous invocation?
		&& plotOpaqueInfoMatches(itr.getOpaqueInfo(ACTIVITY_ID_ANY_ATTACK), ACTIVITY_ID_ANY_ATTACK, iOddsThreshold))
		{
			continue;
		}

		CvPlot* plotX = itr.plot();
		TARGET_DIAG_VISIT(selectorPlots);
		if (getDomainType() == DOMAIN_SEA && plotX->getNumUnits() == 0 && !plotX->isCity(true))
		{
			TARGET_DIAG_VISIT(selectorEmpty);
			continue;
		}

		// Most reachable plots are empty. Do not evaluate group movement/cargo
		// legality or consult route metadata until a visible target is eligible.
		if (!bAllowCities && plotX->isCity(true)
		|| !plotX->isVisibleEnemyUnit(this)
		&& (!plotX->isCity(true) || !AI_potentialEnemy(plotX->getTeam()))
		|| (iMinStack > 0 && plotX->getNumVisibleEnemyTargetUnits(this) < iMinStack)
		|| CvSelectionGroup::getPathGenerator()->haveRouteLength(plotX, getGroup(), 0, iPathTurns) && iPathTurns > iRange
		|| (!attackMemo.contains(plotX) && !getGroup()->canEnterOrAttackPlot(plotX, true)))
		{
			continue;
		}
		int iValue = 0;
		bool bWinLikely = false;
		int iAdjustedOddsThreshold = 0;

		PROFILE("CvUnitAI::AI_anyAttack.FoundTarget");

		if (attackMemo.get(plotX, iValue, iAdjustedOddsThreshold))
		{
			// Same group, thresholds and unchanged world since the preflight.
			if (iValue < 0) continue;
		}
		else if (plotX->isVisiblePotentialEnemyDefender(this))
		{
			iValue += getGroup()->AI_attackOdds(plotX, true, false, &bWinLikely, iOddsThreshold);
			iAdjustedOddsThreshold = AI_finalOddsThreshold(plotX, iOddsThreshold);

			if (shouldAvoidLowOddsRiverAttack(*this, plotX, iValue))
			{
				continue;
			}

			if (bWinLikely && plotX->isCity(false))
			{
				iValue += iOddsThreshold;
			}
		}
		else
		{
			iValue += 100 * plotX->getNumVisiblePotentialEnemyDefenderless(this);
			iAdjustedOddsThreshold = 1;
			bWinLikely = true;
		}

		// Increase value on our territory since we really want to
		//	get rid of enemies there even if it costs us a few losses
		if (!attackMemo.contains(plotX) && plotX->getOwner() == getOwner() && bWinLikely)
		{
			iValue *= 2;
		}

		if (iMinStack == 0 && bAllowCities && !bFollow && iAdjustedOddsThreshold != 0)
		{
			itr.setOpaqueInfo(ACTIVITY_ID_ANY_ATTACK, 1 + 100 * iValue * iOddsThreshold / iAdjustedOddsThreshold);
		}

		if (iValue > iBestValue && iValue >= iAdjustedOddsThreshold)
		{
			PROFILE("CvUnitAI::AI_anyAttack.SearchPath");

			if (!atPlot(plotX) && (bFollow ? getGroup()->canEnterPlot(plotX, true) : generatePath(plotX, 0, true, &iPathTurns, iRange)))
			{
				PROFILE("CvUnitAI::AI_anyAttack.SuccessfulPath");

				CvPlot* endTurnPlot = getPathEndTurnPlot();
				if (immediateGarrisonAttack && !bFollow && endTurnPlot != plotX) continue;

				if (endTurnPlot == plotX || !exposedToDanger(bFollow ? plotX : endTurnPlot, iOddsThreshold))
				{
					iBestValue = iValue;
					pBestPlot = bFollow ? plotX : endTurnPlot;
					FAssert(!atPlot(pBestPlot));

					if (iValue > iAdjustedOddsThreshold + (100 - iAdjustedOddsThreshold) / 2)
					{
						// Result is significantly better than we've been asked to find so cut off the search now
						break;
					}
				}
			}
		}
	}

#ifdef VALIDITY_CHECK_NEW_ATTACK_SEARCH
	{
		CvPlot* pNewAlgorithmBestPlot = pBestPlot;
		int iNewAlgorithmBestValue = iBestValue;
		int iDX, iDY;

		iBestValue = 0;
		pBestPlot = NULL;

		for (iDX = -(iSearchRange); iDX <= iSearchRange; iDX++)
		{
			for (iDY = -(iSearchRange); iDY <= iSearchRange; iDY++)
			{
				CvPlot* plotX = plotXY(getX(), getY(), iDX, iDY);

				if (plotX
				&& AI_plotValid(plotX)
				&& (bAllowCities || !plotX->isCity(false))
				&& (plotX->isVisibleEnemyUnit(this) || plotX->isCity() && AI_potentialEnemy(plotX->getTeam()))
				&& plotX->getNumVisiblePotentialEnemyDefenders(this) >= iMinStack)
				{
					PROFILE("CvUnitAI::AI_anyAttack.FoundTarget");
					const int iValue = getGroup()->AI_attackOdds(plotX, true);

					if (iValue > iBestValue && iValue >= AI_finalOddsThreshold(plotX, iOddsThreshold))
					{
						PROFILE("CvUnitAI::AI_anyAttack.SearchPath");
						if (!atPlot(plotX) && (bFollow ? getGroup()->canEnterPlot(plotX, true) : (generatePath(plotX, 0, true, &iPathTurns))))
						{
							PROFILE("CvUnitAI::AI_anyAttack.SuccessfulPath");
							if (iPathTurns <= iRange)
							{
								PROFILE("CvUnitAI::AI_anyAttack.SuccessfulPath.InRange");

								iBestValue = iValue;
								pBestPlot = (bFollow ? plotX : getPathEndTurnPlot());
								FAssert(!atPlot(pBestPlot));
							}
						}
					}
				}
			}
		}
		FAssert((pNewAlgorithmBestPlot == NULL) == (pBestPlot == NULL) || getDomainType() != DOMAIN_SEA);
		FAssert(iNewAlgorithmBestValue == iBestValue || getDomainType() != DOMAIN_SEA);
	}
#endif

	if (pBestPlot)
	{
		FAssert(!atPlot(pBestPlot));
		LOG_UNIT_BLOCK(3, {
			UnitAITypes eUnitAi = AI_getUnitAIType();
			MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
			CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
			CvWString MissionInfos = MissionAITypeToString(eMissionAI);
			CvWString StrUnitName = m_szName;
			if (StrUnitName.length() == 0)
			{
				StrUnitName = getName(0).GetCString();
			}

			logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Move to Attack at (%d,%d)...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits(), pBestPlot->getX(), pBestPlot->getY());
		});
		NAVAL_DIAG_PHASE_RETURN((getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), ((bFollow) ? MOVE_DIRECT_ATTACK : 0))));
	}
	NAVAL_DIAG_PHASE_RETURN((false));
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_rangeAttack(int iRange)
{
	PROFILE_FUNC();
	FAssert(canMove());

	if (!canRangeStrike())
	{
		return false;
	}

	// Movement speed does not extend ranged-fire distance. canRangeStrikeAt
	// remains authoritative for line of sight, visibility and a real defender.
	const int iSearchRange = std::min(AI_searchRange(iRange), airRange());
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * plotX, plot()->rect(iSearchRange, iSearchRange))
	{
		// Super Forts - *AI_offense* - modified if statement so forts will be attacked too
		if (plotX->isVisibleEnemyUnit(this) || plotX->isCity(true) && AI_potentialEnemy(plotX->getTeam()))
		{
			if (!atPlot(plotX) && canRangeStrikeAt(plot(), plotX->getX(), plotX->getY()))
			{
				const int iValue = getGroup()->AI_attackOdds(plotX, true);

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = plotX;
				}
			}
		}
	}
	if (pBestPlot)
	{
		FAssert(!atPlot(pBestPlot));
		getGroup()->pushMission(MISSION_RANGE_ATTACK, pBestPlot->getX(), pBestPlot->getY(), 0);
		return true;
	}
	return false;
}

bool CvUnitAI::AI_leaveAttack(int iRange, int iOddsThreshold, int iStrengthThreshold)
{
	PROFILE_EXTRA_FUNC();
	FAssert(canMove());

	// Collect actionable hostile targets in the same plot order. An
	// empty neighbourhood cannot produce a sortie, so it needs no strength
	// scans. Keep the live city/odds/path checks for every surviving target.
	std::vector<const CvPlot*> targets;
	foreach_(const CvPlot* plotX, plot()->rect(iRange, iRange))
	{
		if (AI_plotValid(plotX) && !atPlot(plotX)
		&& (plotX->isVisibleEnemyUnit(this) || plotX->isCity(true) && AI_potentialEnemy(plotX->getTeam(), plotX))
		&& getGroup()->canEnterOrAttackPlot(plotX, false))
		{
			// Entering an enemy-occupied tile need not attack its occupants:
			// coexistence, amnesty and domain rules may permit peaceful entry.
			// Require a real victim for a mobile member, or a capturable city.
			foreach_(const CvUnit* attacker, getGroup()->units())
			{
				if (attacker->canAttack() && attacker->canMove() && attacker->canEnterPlot(plotX, MoveCheck::Attack | MoveCheck::IgnoreLocation)
				&& (plotX->hasDefender(true, NO_PLAYER, getOwner(), attacker, true)
					|| plotX->isEnemyCity(*attacker) && !attacker->isNoCapture()
					&& attacker->canEnterPlot(plotX, MoveCheck::Attack)))
				{
					targets.push_back(plotX);
					break;
				}
			}
		}
	}
	if (targets.empty()) return false;

	const CvCity* pCity = plot()->getPlotCity();

	if (pCity && pCity->getOwner() == getOwner())
	{
		if (canDefend() && plot()->plotCount(PUF_canDefend, -1, -1, NULL, getOwner()) <= 1)
		{
			return false;
		}

		const int iEnemyStrength = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);
		if (iEnemyStrength > 0)
		{
			const int iOurStrength = GET_PLAYER(getOwner()).AI_getOurPlotStrength(plot(), 0, false, false);
			if (iOurStrength * 100 / iEnemyStrength < iStrengthThreshold) return false;
		}
	}
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot* plotX, targets)
	{
		int iPathTurns;
		if (generatePath(plotX, 0, true, &iPathTurns, iRange))
		{
			const int iValue = getGroup()->AI_attackOdds(plotX, true);

			if (iValue >= AI_finalOddsThreshold(plotX, iOddsThreshold))
			{
				if (iValue > iBestValue)
				{
					const CvPlot* endTurnPlot = getPathEndTurnPlot();

					// A defensive sortie must reach the actual attack this turn.
					// An empty staging tile is immediately abandoned by guardCity,
					// causing repeated leave/return orders without attacking.
					if (endTurnPlot == plotX)
					{
						iBestValue = iValue;
						pBestPlot = endTurnPlot;
						FAssert(!atPlot(pBestPlot));
					}
				}
			}
		}
	}
	if (pBestPlot)
	{
		int idist = plotDistance(plot()->getX(), plot()->getY(), pBestPlot->getX(), pBestPlot->getY());
		LOG_UNIT_BLOCK(3, {
			UnitAITypes eUnitAi = AI_getUnitAIType();
			MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
			CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
			CvWString MissionInfos = MissionAITypeToString(eMissionAI);
			CvWString StrUnitName = m_szName;
			if (StrUnitName.length() == 0)
			{
				StrUnitName = getName(0).GetCString();
			}

			logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], leaveAttack to (%d, %d), distance %d ...", getOwner(), m_iID, StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getGroup()->getNumUnits(), pBestPlot->getX(), pBestPlot->getY(), idist);
		});
		FAssert(!atPlot(pBestPlot));
		if (!atPlot(pBestPlot))
		{
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0);
		}
	}
	return false;
}

int CvUnitAI::AI_blockadeValue(const CvPlot* pLoopPlot, const CvCity* pCity, CvPlot*& endTurnPlot, int iMinValue, int iCityValue) const
{
	// The one-turn arrival multiplier is at most 3, after division by 2.
	// At zero turns the score is unmultiplied; all later arrivals are lower.
	// An upper bound cannot reject a candidate able to beat the current best.
	int iBaseValue = iCityValue;
	if (iBaseValue < 0)
	{
		iBaseValue = 1 + std::min(pCity->getPopulation(), pCity->countNumWaterPlots());
		iBaseValue += GET_PLAYER(getOwner()).AI_adjacentPotentialAttackers(pCity->plot());
		iBaseValue += GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pCity->plot(), MISSIONAI_ASSAULT, getGroup(), 2) * 3;
	}
	if (getGroup()->canBombard(pLoopPlot)) iBaseValue *= 2;
	iBaseValue *= 1000;
	if ((__int64)iBaseValue * 3 / 2 <= iMinValue) return 0;

	// At two or more turns score <= floor(base/(turns+1)). Only a strict
	// improvement matters to the selector. Keep the special one-turn arrival
	// multiplier eligible, but do not search beyond a provably losing arrival.
	const int iMaxPathTurns = iMinValue > 0
		? std::max(1, (int)((__int64)iBaseValue / ((__int64)iMinValue + 1)) - 1)
		: MAX_INT;
	int iValue = 0;
	int iPathTurns;
	if (generatePath(pLoopPlot, 0, true, &iPathTurns, iMaxPathTurns))
	{
		iValue = iBaseValue;
		iValue /= (iPathTurns + 1);

		if (iPathTurns == 1)
		{
			//Prefer to have movement remaining to Bombard + Plunder
			iValue *= 1 + std::min(2, (getPathMovementRemaining() + GC.getMOVE_DENOMINATOR() - 1) / GC.getMOVE_DENOMINATOR());
		}

		// if not at war with this plot owner, then devalue plot if we already inside this owner's borders
		// (because declaring war will pop us some unknown distance away)
		if (!isEnemy(pLoopPlot->getTeam()) && plot()->getTeam() == pLoopPlot->getTeam())
		{
			iValue /= 10;
		}

		endTurnPlot = getPathEndTurnPlot();
	}

	return iValue;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_blockade()
{
    NAVAL_DIAG_PHASE(BLOCKADE,0,0,0,0);

	PROFILE_FUNC();

	int iBestRange;
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	const CvPlot* pBestBlockadePlot = NULL;

	CvReachablePlotSet plotSet(getGroup(), 0);

	std::vector<CvPlot*> targets;
	std::vector<const CvCity*> targetCities;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (!GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getTeam(), false))
		{
			continue;
		}
		const TeamTypes eTeamX = GET_PLAYER((PlayerTypes)iI).getTeam();

		if (!isEnemy(eTeamX) && GET_TEAM(getTeam()).AI_getWarPlan(eTeamX) == NO_WARPLAN)
		{
			continue;
		}

		foreach_(const CvCity * pCity, GET_PLAYER((PlayerTypes)iI).cities())
		{
			if (!pCity->isRevealed(getTeam(), false)
			|| !pCity->isCoastal(GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize()))
			{
				continue;
			}
			foreach_(CvPlot * plotX, pCity->plot()->adjacent())
			{
				if (plotX->isRevealed(getTeam(), false)
				&& plotX->getWorkingCity() == pCity
				&& !plotX->isVisibleEnemyUnit(this)
				&& canPlunder(plotX)
				&& GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(plotX, MISSIONAI_BLOCKADE, getGroup(), 2) == 0)
				{
					targets.push_back(plotX);
					targetCities.push_back(pCity);
				}
			}
		}
	}
	if (targets.empty()) NAVAL_DIAG_PHASE_RETURN((false));
	plotSet.PopulateForTargets(targets);

	// Reuse the original city/adjacent order and reservations. Nothing mutates
	// between collection and selection; no second empire scan is necessary.
	const CvCity* valuedCity = NULL;
	int cityValue = 0;
	for (size_t i = 0; i < targets.size(); ++i)
	{
		CvPlot* plotX = targets[i];
		CvReachablePlotSet::const_iterator itr = plotSet.find(plotX);
		if (itr == plotSet.end()) continue;
		if (iBestValue > 0 && itr.stepDistance() > 2 * iBestRange) continue;
		if (potentialWarAction(plotX))
		{
			// Adjacent candidates share the city's read-only strategic score.
			// Keep it only for this call; reservations and units may change later.
			if (valuedCity != targetCities[i])
			{
				valuedCity = targetCities[i];
				cityValue = 1 + std::min(valuedCity->getPopulation(), valuedCity->countNumWaterPlots());
				cityValue += GET_PLAYER(getOwner()).AI_adjacentPotentialAttackers(valuedCity->plot());
				cityValue += GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(valuedCity->plot(), MISSIONAI_ASSAULT, getGroup(), 2) * 3;
			}
			CvPlot* endTurnPlot;
			const int iValue = AI_blockadeValue(plotX, valuedCity, endTurnPlot, iBestValue, cityValue);
			if (iValue > iBestValue && (endTurnPlot == plotX || !exposedToDanger(endTurnPlot, 70)))
			{
				iBestValue = iValue;
				iBestRange = itr.stepDistance();
				pBestPlot = endTurnPlot;
				pBestBlockadePlot = plotX;
			}
		}
	}

	if (pBestPlot != NULL && pBestBlockadePlot != NULL)
	{
		FAssert(canPlunder(pBestBlockadePlot));
		if (atPlot(pBestBlockadePlot) && !isEnemy(pBestBlockadePlot->getTeam(), pBestBlockadePlot))
		{
			getGroup()->groupDeclareWar(pBestBlockadePlot, true);
		}

		if (atPlot(pBestBlockadePlot))
		{
			if (getGroup()->canBombard(plot()))
			{
				getGroup()->pushMission(MISSION_BOMBARD, -1, -1, 0, false, false, MISSIONAI_BLOCKADE, pBestBlockadePlot);
			}
			getGroup()->pushMission(MISSION_PLUNDER, -1, -1, 0, (getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BLOCKADE, pBestBlockadePlot);

			NAVAL_DIAG_PHASE_RETURN((true));
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			NAVAL_DIAG_PHASE_RETURN((getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_BLOCKADE, pBestBlockadePlot)));
		}
	}
	NAVAL_DIAG_PHASE_RETURN((false));
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_pirateBlockade()
{
	PROFILE_FUNC();

	std::vector<int> aiDeathZone(GC.getMap().numPlots(), 0);

	CvReachablePlotSet plotSet(getGroup(), 0, MAX_INT);

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* plotX = GC.getMap().plotByIndex(iI);

		if (plotSet.find(plotX) != plotSet.end() && plotX->isOwned() && plotX->getTeam() != getTeam())
		{
			int iBestHostileMoves = 0;
			foreach_(CvUnit * unitX, plotX->units())
			{
				if (isEnemy(unitX->getTeam(), unitX->plot(), unitX)
				&& unitX->getDomainType() == DOMAIN_SEA
				&& !unitX->isInvisible(getTeam(), false)
				&& unitX->canAttack()
				&& unitX->currEffectiveStr(NULL, NULL, NULL) > currEffectiveStr(plotX, unitX, NULL))
				{
					//Fuyu: No (rail)roads on water, always movement cost 1. Rounding up of course
					iBestHostileMoves = std::max(iBestHostileMoves, (unitX->getMoves() + GC.getMOVE_DENOMINATOR() - 1) / GC.getMOVE_DENOMINATOR());
				}
			}
			if (iBestHostileMoves > 0)
			{
				foreach_(const CvPlot * pRangePlot, plotX->rect(iBestHostileMoves, iBestHostileMoves))
				{
					aiDeathZone[GC.getMap().plotNum(pRangePlot->getX(), pRangePlot->getY())]++;
				}
			}
		}
	}

	const bool bIsInDanger = aiDeathZone[GC.getMap().plotNum(getX(), getY())] > 0;
	const bool bHasTerrainDamage = plot()->getTotalTurnDamage(getGroup()) > 0 || plot()->getFeatureTurnDamage() > 0;
	const bool bNeedsHeal = getDamage() > 0;
	const bool bHurtAndInWildernessOrInBadShape = bNeedsHeal && (!plot()->isOwned() && !plot()->isAdjacentOwned() || getDamagePercent() > 25);

	if (!bIsInDanger && bHurtAndInWildernessOrInBadShape)
	{
		// If we only have damage explained by the current plot for one turn
		//	don't use that as an excuse to immediately turn around and go hide in a city!
		if (!bHasTerrainDamage || getDamage() > (plot()->getTotalTurnDamage(getGroup()) + plot()->getFeatureTurnDamage()))
		{
			if (AI_retreatToCity(false, false, 1 + getDamagePercent() / 20))
			{
				return true;
			}
		}

		if (!bHasTerrainDamage)
		{
			getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
	}
	int iPathTurns;
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestBlockadePlot = NULL;
	bool bBestIsForceMove = false;
	bool bBestIsMove = false;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (plotSet.find(pLoopPlot) != plotSet.end())
		{
			if (!pLoopPlot->isVisible(getTeam(), false) && !pLoopPlot->isVisibleEnemyUnit(this) && canPlunder(pLoopPlot))
			{
				if (GC.getGame().getSorenRandNum(4, "AI Pirate Blockade") == 0)
				{
					if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BLOCKADE, getGroup(), 3) == 0)
					{
						int iBlockadedCount = 0;
						int iPopulationValue = 0;
						int iRange = GC.getSHIP_BLOCKADE_RANGE() - 1;
						foreach_(const CvPlot * pRangePlot, pLoopPlot->rect(iRange, iRange))
						{
							bool bPlotBlockaded = false;
							if (pRangePlot->isWater() && pRangePlot->isOwned() && isEnemy(pRangePlot->getTeam(), pLoopPlot))
							{
								bPlotBlockaded = true;
								iBlockadedCount += pRangePlot->getBlockadedCount(pRangePlot->getTeam());
							}

							if (!bPlotBlockaded)
							{
								const CvCity* pPlotCity = pRangePlot->getPlotCity();
								if (pPlotCity != NULL)
								{
									if (isEnemy(pPlotCity->getTeam(), pLoopPlot))
									{
										int iCityValue = 3 + pPlotCity->getPopulation();
										iCityValue *= (atWar(getTeam(), pPlotCity->getTeam()) ? 1 : 3);
										if (GET_PLAYER(pPlotCity->getOwner()).isNoForeignTrade())
										{
											iCityValue /= 2;
										}
										iPopulationValue += iCityValue;
									}
								}
							}
						}
						int iValue = 1000 * iPopulationValue;

						if (atPlot(pLoopPlot))
						{
							iValue *= 3;
						}
						iValue /= 16 + iBlockadedCount;

						const int iDeath = aiDeathZone[GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY())];

						if (iDeath)
						{
							iValue /= 10;
						}

						int iBestEstimateValue = iValue;

						CvReachablePlotSet::const_iterator itr = plotSet.find(pLoopPlot);

						FAssert(itr != plotSet.end());
						const int iMinTurns = itr.stepDistance() / baseMoves() + 1;

						if (iMinTurns <= 2 && !pLoopPlot->isAdjacentOwned() && bIsInDanger && iPopulationValue == 0)
						{
							iBestEstimateValue += 5000;
						}
						else
						{
							iBestEstimateValue /= iMinTurns + 1;
						}

						if (iMinTurns == 1)
						{
							iBestEstimateValue *= 2;
						}

						if (iBestEstimateValue > iBestValue && generatePath(pLoopPlot, MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns))
						{
							bool bForceMove = false;

							if (!iDeath && bIsInDanger
							&& iPathTurns < 3 && 0 == iPopulationValue
							&& getPathMovementRemaining() == 0
							&& !pLoopPlot->isAdjacentOwned())
							{
								const int iRand = GC.getGame().getSorenRandNum(2500, "AI Pirate Retreat");
								iValue += iRand;
								if (iRand > 1000)
								{
									iValue += GC.getGame().getSorenRandNum(2500, "AI Pirate Retreat");
									bForceMove = true;
								}
							}

							if (!bForceMove)
							{
								iValue /= iPathTurns + 1;
							}
							const bool bMove = iPathTurns == 1 && getPathMovementRemaining() > 0;

							if (bMove && !atPlot(pLoopPlot))
							{
								iValue *= 2;
							}
							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = bForceMove ? pLoopPlot : getPathEndTurnPlot();
								pBestBlockadePlot = pLoopPlot;
								bBestIsForceMove = bForceMove;
								bBestIsMove = bMove;
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL && pBestBlockadePlot != NULL)
	{
		FAssert(canPlunder(pBestBlockadePlot));

		if (atPlot(pBestBlockadePlot))
		{
			getGroup()->pushMission(MISSION_PLUNDER, -1, -1, 0, (getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BLOCKADE, pBestBlockadePlot);
			return true;
		}
		FAssert(!atPlot(pBestPlot));

		if (bNeedsHeal)
		{
			CvPlot* pBestStopAndHealPlot = pBestPlot;

			// Examine plots we move through in this turn and see if any is suitable to stop and heal in
			generatePath(pBestPlot, MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns);
			CvPath::const_iterator itr = getGroup()->getPath().begin();

			while (itr != getGroup()->getPath().end())
			{
				CvPlot* pPlot = itr.plot();

				// If an intermediary plot is one that the heal decsion logic (near the start of this method)
				//	would choose to heal in, then just stop there on our way
				if (bHurtAndInWildernessOrInBadShape
				&& aiDeathZone[GC.getMap().plotNum(pPlot->getX(), pPlot->getY())] < 1
				&& pPlot->getTotalTurnDamage(getGroup()) < 1
				&& pPlot->getFeatureTurnDamage() < 1)
				{
					pBestStopAndHealPlot = pPlot;
				}

				if (pPlot == pBestPlot)
				{
					break;
				}
				++itr;
			}
			pBestPlot = pBestStopAndHealPlot;
		}

		if (bBestIsForceMove)
		{
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_WEIGHT_3);
		}

		if (getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_WEIGHT_3, false, false, MISSIONAI_BLOCKADE, pBestBlockadePlot))
		{
			if (bBestIsMove)
			{
				getGroup()->pushMission(MISSION_PLUNDER, -1, -1, 0, (getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BLOCKADE, pBestBlockadePlot);
			}
			return true;
		}
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_seaBombardRange(int iMaxRange)
{
    NAVAL_DIAG_PHASE(BOMBARD,iMaxRange,0,0,0);

	PROFILE_FUNC();

	const CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	const CvPlot* pPlot = plot();
	const CvSelectionGroup* pGroup = getGroup();
	// Equal benefit is not a reason to abandon an existing bombard position.
	const CvPlot* pPreviousBombardPlot = pGroup->AI_getMissionAIType()==MISSIONAI_BLOCKADE
	    ? pGroup->AI_getMissionAIPlot() : NULL;

	// can any unit in this group bombard?
	bool bHasBombardUnit = false;
	bool bBombardUnitCanBombardNow = false;
	foreach_(const CvUnit * pLoopUnit, pGroup->units())
	{
		if (pLoopUnit->getBombardRate() > 0)
		{
			bHasBombardUnit = true;

			if (pLoopUnit->canMove() && !pLoopUnit->isMadeAttack())
			{
				bBombardUnitCanBombardNow = true;
				break;
			}
		}
	}

	if (!bHasBombardUnit)
	{
		NAVAL_DIAG_PHASE_RETURN((false));
	}

	if (!hasPotentialNavalBombardCity(*this,iMaxRange))
	{
		NAVAL_DIAG_PHASE_RETURN((false));
	}

	// best match
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestBombardPlot = NULL;
	const CvPlot* endTurnPlot = NULL;
	int iBestValue = 0;

	CvReachablePlotSet plotSet(getGroup(), 0, iMaxRange);

	// Both passes already reject arrivals beyond this horizon.
	const int bombardMaxTurns = 1 + iMaxRange / std::max(1, baseMoves());
	std::vector<std::pair<const CvCity*, int> > supportValues;
	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		const CvPlot* pLoopPlot = itr.plot();
		if (pLoopPlot != NULL /*&& AI_plotValid(pLoopPlot)*/)
		{
			CvCity* pBombardCity = bombardTarget(pLoopPlot);

			if (pBombardCity != NULL && isEnemy(pBombardCity->getTeam(), pLoopPlot) && pBombardCity->getDefenseDamage() < GC.getMAX_CITY_DEFENSE_DAMAGE())
			{
				// Supporting ground forces is a prerequisite for this pass.
				// Counts are independent of the route: reject zero support first.
				int supportValue;
				if (!findDecisionCityValue(supportValues, pBombardCity, supportValue))
				{
					supportValue = kPlayer.AI_plotTargetMissionAIs(pBombardCity->plot(), MISSIONAI_ASSAULT, NULL, 2) * 3
						+ kPlayer.AI_adjacentPotentialAttackers(pBombardCity->plot(), true);
					supportValues.push_back(std::make_pair(pBombardCity, supportValue));
				}
				if (supportValue <= 0) continue;
				int iPathTurns;
				if (generatePath(pLoopPlot, 0, true, &iPathTurns, bombardMaxTurns))
				{
					// Loop construction doesn't guarantee we can get there anytime soon, could be on other side of narrow continent
					if (iPathTurns <= bombardMaxTurns)
					{
						// Check only for supporting our own ground troops first, if none will look for another target
						int iValue = supportValue;

						if (iValue > 0)
						{
							iValue *= 1000;

							iValue /= (iPathTurns + 1);

							if (iPathTurns == 1)
							{
								//Prefer to have movement remaining to Bombard + Plunder
								iValue *= 1 + std::min(2, (getPathMovementRemaining() + GC.getMOVE_DENOMINATOR() - 1) / GC.getMOVE_DENOMINATOR());
							}

							if (iValue > iBestValue || (iValue == iBestValue && pLoopPlot == pPreviousBombardPlot))
							{
								endTurnPlot = getPathEndTurnPlot();

								if (endTurnPlot == pLoopPlot || !exposedToDanger(endTurnPlot, 70))
								{
									iBestValue = iValue;
									pBestPlot = endTurnPlot;
									pBestBombardPlot = pLoopPlot;
								}
							}
						}
					}
				}
			}
		}
	}

	// If no troops of ours to support, check for other bombard targets
	if ((pBestPlot == NULL) && (pBestBombardPlot == NULL))
	{
		if (AI_getUnitAIType() != UNITAI_ASSAULT_SEA)
		{
			std::vector<std::pair<const CvCity*, int> > blockadeValues;
			foreach_(const CvPlot * pLoopPlot, pPlot->rect(iMaxRange, iMaxRange))
			{
				if (AI_plotValid(pLoopPlot))
				{
					CvCity* pBombardCity = bombardTarget(pLoopPlot);

					// Consider city even if fully bombarded, causes ship to camp outside blockading instead of twitching between
					// cities after bombarding to 0
					if (pBombardCity != NULL && isEnemy(pBombardCity->getTeam(), pLoopPlot) && pBombardCity->getTotalDefense(false) > 0)
					{
						int iPathTurns;
						if (generatePath(pLoopPlot, 0, true, &iPathTurns, bombardMaxTurns))
						{
							// Loop construction doesn't guarantee we can get there anytime soon, could be on other side of narrow continent
							if (iPathTurns <= bombardMaxTurns)
							{
								int iValue;
								if (!findDecisionCityValue(blockadeValues, pBombardCity, iValue))
								{
									iValue = std::min(20, pBombardCity->getDefenseModifier(false) / 2);

									// Inclination to support attacks by others
									if (GET_PLAYER(pBombardCity->getOwner()).AI_getAnyPlotDanger(pBombardCity->plot(), 2, false))
									{
										iValue += 60;
									}

									// Inclination to bombard a different nearby city to extend the reach of blockade
									if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pBombardCity->plot(), MISSIONAI_BLOCKADE, getGroup(), 3) == 0)
									{
										iValue += 35 + pBombardCity->getPopulation();
									}

									// Small inclination to bombard area target, not too large so as not to tip our hand
									if (pBombardCity == pBombardCity->area()->getTargetCity(getOwner()))
									{
										iValue += 10;
									}
									blockadeValues.push_back(std::make_pair(pBombardCity, iValue));
								}

								if (iValue > 0)
								{
									iValue *= 1000;

									iValue /= (iPathTurns + 1);

									if (iPathTurns == 1)
									{
										//Prefer to have movement remaining to Bombard + Plunder
										iValue *= 1 + std::min(2, (getPathMovementRemaining() + GC.getMOVE_DENOMINATOR() - 1) / GC.getMOVE_DENOMINATOR());
									}

									if (iValue > iBestValue || (iValue == iBestValue && pLoopPlot == pPreviousBombardPlot))
									{
										iBestValue = iValue;
										pBestPlot = getPathEndTurnPlot();
										pBestBombardPlot = pLoopPlot;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestBombardPlot != NULL))
	{
		if (atPlot(pBestBombardPlot))
		{
			// if we are at the plot from which to bombard, and we have a unit that can bombard this turn, do it
			if (bBombardUnitCanBombardNow && pGroup->canBombard(pBestBombardPlot))
			{
				if (AI_RbombardNaval())
				{
					NAVAL_DIAG_PHASE_RETURN((true));
				}
				getGroup()->pushMission(MISSION_BOMBARD, -1, -1, 0, false, false, MISSIONAI_BLOCKADE, pBestBombardPlot);

				// if city bombarded enough, wake up any units that were waiting to bombard this city
				CvCity* pBombardCity = bombardTarget(pBestBombardPlot); // is NULL if city cannot be bombarded any more
				if (pBombardCity == NULL || pBombardCity->getDefenseDamage() >= ((GC.getMAX_CITY_DEFENSE_DAMAGE() * 5) / 6))
				{
					kPlayer.AI_wakePlotTargetMissionAIs(pBestBombardPlot, MISSIONAI_BLOCKADE, getGroup());
				}
			}
			// otherwise, skip until next turn, when we will surely bombard
			else if (canPlunder(pBestBombardPlot))
			{
				getGroup()->pushMission(MISSION_PLUNDER, -1, -1, 0, false, false, MISSIONAI_BLOCKADE, pBestBombardPlot);
			}
			else
			{
				getGroup()->pushMission(MISSION_SKIP);
			}
			NAVAL_DIAG_PHASE_RETURN((true));
		}
		FAssert(!atPlot(pBestPlot));
		NAVAL_DIAG_PHASE_RETURN((getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_BLOCKADE, pBestBombardPlot)));
	}
	NAVAL_DIAG_PHASE_RETURN((false));
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_pillage(int iBonusValueThreshold)
{
	PROFILE_FUNC();

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestPillagePlot = NULL;

	CvReachablePlotSet plotSet(getGroup(), 0, MAX_INT);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		CvPlot* plotX = itr.plot();

		if (UnitTravelConnectivity::landUnitCanReach(this,plotX,true)
		&& plotX->isOwned()
		&& isEnemy(plotX->getTeam(), plotX))
		{
			CvCity* pWorkingCity = plotX->getWorkingCity();

			if (pWorkingCity
			&& pWorkingCity != area()->getTargetCity(getOwner())
			&& getGroup()->canPillage(plotX)
			&& plotX->isRevealed(getTeam(), false)
			&& !plotX->isVisibleEnemyUnit(this)
			&& GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(plotX, MISSIONAI_PILLAGE, getGroup(), 1) == 0)
			{
				int iValue = 1000 * AI_pillageValue(plotX, iBonusValueThreshold);

				// if not at war with this plot owner, then devalue plot if we already inside this owner's borders
				// (because declaring war will pop us some unknown distance away)
				if (getCombatOwner(plotX->getTeam(), plotX) == getOwner()
				&& !isEnemy(plotX->getTeam())
				&& plot()->getTeam() == plotX->getTeam())
				{
					iValue /= 10;
				}

				if (iValue > iBestValue)
				{
					int iPathTurns;
					const int pathLimit = isHuman() ? MAX_INT : navalCityPathLimit(iValue, iBestValue, MAX_INT);
					if (pathLimit >= 0 && generatePath(plotX, 0, true, &iPathTurns, pathLimit))
					{
						iValue /= iPathTurns + 1;

						if (iValue > iBestValue)
						{
							CvPlot* endTurnPlot = getPathEndTurnPlot();

							if (endTurnPlot == plotX || !exposedToDanger(endTurnPlot, 70))
							{
								iBestValue = iValue;
								pBestPlot = endTurnPlot;
								pBestPillagePlot = plotX;
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL && pBestPillagePlot != NULL)
	{
		if (atPlot(pBestPillagePlot) && !isEnemy(pBestPillagePlot->getTeam()))
		{
			//getGroup()->groupDeclareWar(pBestPillagePlot, true);
			// rather than declare war, just find something else to do, since we may already be deep in enemy territory
			return false;
		}

		if (atPlot(pBestPillagePlot))
		{
			if (isEnemy(pBestPillagePlot->getTeam()))
			{
				getGroup()->pushMission(MISSION_PILLAGE, -1, -1, 0, false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
				return true;
			}
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
		}
	}

	return false;
}

bool CvUnitAI::AI_canPillage(const CvPlot& kPlot) const
{
	PROFILE_EXTRA_FUNC();
	if (isEnemy(kPlot.getTeam(), &kPlot))
	{
		return true;
	}

	if (!kPlot.isOwned())
	{
		bool bPillageUnowned = true;

		for (int iPlayer = 0; iPlayer < MAX_PLAYERS && bPillageUnowned; ++iPlayer)
		{
			const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			if (!isEnemy(kLoopPlayer.getTeam(), &kPlot))
			{
				foreach_(const CvCity * pCity, kLoopPlayer.cities())
				{
					if (kPlot.getPlotGroup((PlayerTypes)iPlayer) == pCity->plot()->getPlotGroup((PlayerTypes)iPlayer))
					{
						bPillageUnowned = false;
						break;
					}

				}
			}
		}

		if (bPillageUnowned)
		{
			return true;
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_pillageRange(int iRange, int iBonusValueThreshold)
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	CvPlot* pBestPillagePlot;
	CvPlot* endTurnPlot = NULL;
	int iSearchRange;
	int iPathTurns;
	int iValue;
	int iBestValue;

	iSearchRange = AI_searchRange(iRange);

	iBestValue = 0;
	pBestPlot = NULL;
	pBestPillagePlot = NULL;

	// Populate even when no pillage target exists. Later selectors reuse this
	// shared set and its activity metadata; skipping a negative search changes
	// candidate order and the RNG consumed by AI_safety (see cache sequence test).
	CvReachablePlotSet plotSet(getGroup(), 0, iSearchRange);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		pLoopPlot = itr.plot();

		//	Have we already considered this plot in a previous invocation?
		int iOpaqueInfo = itr.getOpaqueInfo(ACTIVITY_ID_PILLAGE);
		if (plotOpaqueInfoMatches(iOpaqueInfo, ACTIVITY_ID_PILLAGE, iBonusValueThreshold + 1))
		{
			continue;
		}

		itr.setOpaqueInfo(ACTIVITY_ID_PILLAGE, iBonusValueThreshold + 1);

		if ((!(pLoopPlot->isNPC()) || !GET_PLAYER(getOwner()).isModderOption(MODDEROPTION_AUTO_PILLAGE_AVOID_BARBARIAN_CITIES)))
		{
			if (potentialWarAction(pLoopPlot))
			{
				CvCity* pWorkingCity = pLoopPlot->getWorkingCity();

				if (pWorkingCity != NULL)
				{
					if (!(pWorkingCity == area()->getTargetCity(getOwner())) && getGroup()->canPillage(pLoopPlot))
					{
						if (!pLoopPlot->isVisibleEnemyUnit(this))
						{
							if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_PILLAGE, getGroup()) == 0)
							{
								iValue = 1000 * AI_pillageValue(pLoopPlot, iBonusValueThreshold);

								// if not at war with this plot owner, then devalue plot if we already inside this owner's borders
								// (because declaring war will pop us some unknown distance away)
								if (!isEnemy(pLoopPlot->getTeam()) && plot()->getTeam() == pLoopPlot->getTeam())
								{
									iValue /= 10;
								}

								//	TODO - should really evaluate how dangerous the target plot is and take acount of that
								//	in both the value and whether to even consider it.  Factors to take account of:
								//		Defense modifier
								//		Adjacency damage
								//		Enemy stacks in strenght relationship to our own

								const int pathLimit = isHuman() ? iRange : navalCityPathLimit(iValue, iBestValue, iRange);
								if (iValue > iBestValue && pathLimit >= 0 && generatePath(pLoopPlot, 0, true, &iPathTurns, pathLimit))
								{
									if (getPathMovementRemaining() == 0)
									{
										iPathTurns++;
									}

									if (iPathTurns <= iRange)
									{
										iValue /= (iPathTurns + 1);

										if (iValue > iBestValue)
										{
											endTurnPlot = getPathEndTurnPlot();

											if (endTurnPlot == pLoopPlot || !exposedToDanger(endTurnPlot, 70))
											{
												iBestValue = iValue;
												pBestPlot = endTurnPlot;
												pBestPillagePlot = pLoopPlot;

												FAssert(pBestPlot == pBestPillagePlot || !atPlot(pBestPlot));
											}
										}
									}
								}
								else
								{
									//	Actually out of range so not really markable as searched (if we'd already
									//	found a pathable candidate it wont matter anyway since this routine will
									//	push a mission)
									// Score-bounded failures must remain uncached as well.
									itr.setOpaqueInfo(ACTIVITY_ID_PILLAGE, 0);
								}
							}
						}
					}
				}
			}
		}
	}

#ifdef VALIDITY_CHECK_NEW_ATTACK_SEARCH
	CvPlot* pNewAlgorithmBestPlot = pBestPlot;
	int iNewAlgorithmBestValue = iBestValue;
	int iDX, iDY;

	iBestValue = 0;
	pBestPlot = NULL;

	for (iDX = -(iSearchRange); iDX <= iSearchRange; iDX++)
	{
		for (iDY = -(iSearchRange); iDY <= iSearchRange; iDY++)
		{
			pLoopPlot = plotXY(getX(), getY(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				if (AI_plotValid(pLoopPlot) && (!(pLoopPlot->isNPC()) || !GET_PLAYER(getOwner()).isModderOption(MODDEROPTION_AUTO_PILLAGE_AVOID_BARBARIAN_CITIES)))
				{
					if (potentialWarAction(pLoopPlot))
					{
						CvCity* pWorkingCity = pLoopPlot->getWorkingCity();

						if (pWorkingCity != NULL)
						{
							if (!(pWorkingCity == area()->getTargetCity(getOwner())) && canPillage(pLoopPlot))
							{
								if (!pLoopPlot->isVisibleEnemyUnit(this))
								{
									if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_PILLAGE, getGroup()) == 0)
									{
										if (generatePath(pLoopPlot, 0, true, &iPathTurns))
										{
											if (getPathLastNode()->m_iData1 == 0)
											{
												iPathTurns++;
											}

											if (iPathTurns <= iRange)
											{
												iValue = AI_pillageValue(pLoopPlot, iBonusValueThreshold);

												iValue *= 1000;

												iValue /= (iPathTurns + 1);

												// if not at war with this plot owner, then devalue plot if we already inside this owner's borders
												// (because declaring war will pop us some unknown distance away)
												if (!isEnemy(pLoopPlot->getTeam()) && plot()->getTeam() == pLoopPlot->getTeam())
												{
													iValue /= 10;
												}

												if (iValue > iBestValue)
												{
													iBestValue = iValue;
													pBestPlot = getPathEndTurnPlot();
													pBestPillagePlot = pLoopPlot;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	FAssert((pNewAlgorithmBestPlot == NULL) == (pBestPlot == NULL) || getDomainType() != DOMAIN_SEA);
	FAssert(iNewAlgorithmBestValue == iBestValue || getDomainType() != DOMAIN_SEA);
#endif

	if (pBestPlot != NULL && pBestPillagePlot != NULL)
	{
		if (atPlot(pBestPillagePlot) && !isEnemy(pBestPillagePlot->getTeam()))
		{
			//getGroup()->groupDeclareWar(pBestPillagePlot, true);
			// rather than declare war, just find something else to do, since we may already be deep in enemy territory
			return false;
		}

		if (!atPlot(pBestPillagePlot))
		{
			FAssert(!atPlot(pBestPlot));
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
		}

		if (isEnemy(pBestPillagePlot->getTeam()))
		{
			return getGroup()->pushMissionInternal(MISSION_PILLAGE, pBestPillagePlot->getX(), pBestPillagePlot->getY(), 0, false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
		}
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_found()
{
	PROFILE_FUNC();

	int iPathTurns, iBestFoundValue = 0;
	const CvPlot* pBestPlot = NULL;

	for (int iI = 0; iI < GET_PLAYER(getOwner()).AI_getNumCitySites(); iI++)
	{
		CvPlot* pCitySitePlot = GET_PLAYER(getOwner()).AI_getCitySite(iI);

		if (canFound(pCitySitePlot)
		&& (UnitTravelConnectivity::landUnitCanReach(this,pCitySitePlot,true) || canMoveAllTerrain())
		&& !pCitySitePlot->isVisibleEnemyUnit(this)
		&& GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pCitySitePlot, MISSIONAI_FOUND, getGroup()) == 0
		&& getGroup()->canDefend()
		&& getGroup()->getStrength() >= AI_minSettlerDefense()
		&& generatePath(pCitySitePlot, MOVE_IGNORE_DANGER | MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns))
		{
			const int iValue = 1000 * pCitySitePlot->getFoundValue(getOwner()) / (iPathTurns + 1);

			if (iValue > iBestFoundValue)
			{
				iBestFoundValue = iValue;
				pBestPlot = pCitySitePlot;
			}
		}
	}

	if (pBestPlot)
	{
		if (atPlot(pBestPlot))
		{
			if (gUnitLogLevel >= 2)
			{
				logBBAI("	Settler (%d) founding at best found plot %d, %d", getID(), pBestPlot->getX(), pBestPlot->getY());
			}
			getGroup()->pushMission(MISSION_FOUND, -1, -1, 0, false, false, MISSIONAI_FOUND, pBestPlot);
			return true;
		}

		if (gUnitLogLevel >= 2)
		{
			logBBAI("	Settler (%d) heading for best found plot %d, %d, current group strength (%d)", getID(), pBestPlot->getX(), pBestPlot->getY(), getGroup()->getStrength());
		}

		if (generatePath(pBestPlot, MOVE_WITH_CAUTION | MOVE_NO_ENEMY_TERRITORY, true))
		{
			const CvPlot* plotX = getPathEndTurnPlot();
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, plotX->getX(), plotX->getY(), MOVE_WITH_CAUTION | MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_FOUND, pBestPlot);
		}
		getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
		return true;
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_foundRange(int iRange, bool bFollow)
{
	PROFILE_FUNC();

	const CvPlot* pBestPlot = NULL;

	{
		const int iSearchRange = AI_searchRange(iRange);
		int iBestValue = 0;
		foreach_(CvPlot * plotX, plot()->rect(iSearchRange, iSearchRange))
		{
			if (AI_plotValid(plotX)
			&& canFound(plotX)
			&& (
				plotX != plot()
				||
				GET_PLAYER(getOwner()).AI_getPlotDanger(plotX, 1)
				<=
				plotX->plotCount(PUF_canDefend, -1, -1, NULL, getOwner())
				)
			&& !plotX->isVisibleEnemyUnit(this)
			&& GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(plotX, MISSIONAI_FOUND, getGroup(), 3) == 0)
			{
				const int iValue = plotX->getFoundValue(getOwner());

				if (iValue > iBestValue)
				{
					int iPathTurns;
					if (generatePath(plotX, MOVE_IGNORE_DANGER | MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns) && iPathTurns <= iRange)
					{
						iBestValue = iValue;
						pBestPlot = plotX;
					}
				}
			}
		}
	}

	if (pBestPlot)
	{
		if (atPlot(pBestPlot))
		{
			getGroup()->pushMission(MISSION_FOUND, -1, -1, 0, false, false, MISSIONAI_FOUND, pBestPlot);
			return true;
		}
		if (!bFollow)
		{
			if (generatePath(pBestPlot, MOVE_WITH_CAUTION | MOVE_NO_ENEMY_TERRITORY, true))
			{
				const CvPlot* plotX = getPathEndTurnPlot();
                return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_UNITS | MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_FOUND, pBestPlot);
			}
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
			return true;
		}
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_assaultSeaTransport(bool bBarbarian)
{
	PROFILE_FUNC();

	bool bIsAttackCity = (getUnitAICargo(UNITAI_ATTACK_CITY) > 0);

	FAssert(getGroup()->hasCargo());
	//FAssert(bIsAttackCity || getGroup()->getUnitAICargo(UNITAI_ATTACK) > 0);

	if (!canCargoAllMove())
	{
		return false;
	}

	std::vector<CvUnit*> aGroupCargo;
	foreach_(CvUnit * pLoopUnit, plot()->units())
	{
		const CvUnit* pTransport = pLoopUnit->getTransportUnit();
		if (pTransport != NULL && pTransport->getGroup() == getGroup())
		{
			aGroupCargo.push_back(pLoopUnit);
		}
	}

	int iCargoCount = getGroup()->getCargo();
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestAssaultPlot = NULL;
	CvPlot* endTurnPlot = NULL;
	CvReachablePlotSet plotSet(getGroup(), MOVE_AVOID_ENEMY_WEIGHT_3 | MOVE_ALLOW_ADJACENT_COASTAL, MAX_INT);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		CvPlot* pLoopPlot = itr.plot();
		if (pLoopPlot->isCoastalLand() && pLoopPlot->isOwned()
		&& (bBarbarian || !pLoopPlot->isNPC() || GET_PLAYER(getOwner()).isMinorCiv())
		&& isPotentialEnemy(pLoopPlot->getTeam(), pLoopPlot))
		{
			PROFILE("CvUnitAI::AI_assaultSeaTransport.PotentialPlot");

			const int iTargetCities = pLoopPlot->area()->getCitiesPerPlayer(pLoopPlot->getOwner());
			if (iTargetCities > 0)
			{
				bool bCanCargoAllUnload = true;
				const int iVisibleEnemyDefenders = pLoopPlot->getNumVisiblePotentialEnemyDefenders(this);

				if (iVisibleEnemyDefenders > 0 || pLoopPlot->isCity())
				{
					for (uint i = 0; i < aGroupCargo.size(); ++i)
					{
						CvUnit* pAttacker = aGroupCargo[i];
						if (iVisibleEnemyDefenders > 0)
						{
							CvUnit* pDefender = pLoopPlot->getBestDefender(NO_PLAYER, pAttacker->getOwner(), pAttacker, true);
							if (pDefender == NULL || !pAttacker->canAttack(*pDefender))
							{
								bCanCargoAllUnload = false;
								break;
							}
						}
						else if (pLoopPlot->isCity() && !pLoopPlot->isVisible(getTeam(), false))
						{
							// Assume city is defended, artillery can't naval invade
							if (pAttacker->combatLimit() < 100)
							{
								bCanCargoAllUnload = false;
								break;
							}
						}
					}
				}

				if (bCanCargoAllUnload)
				{
					PROFILE("CvUnitAI::AI_assaultSeaTransport.CanUnload");

					int iValue = 1;

					if (!bIsAttackCity)
					{
						iValue += AI_pillageValue(pLoopPlot, 15) * 10;
					}

					const int iAssaultsHere = GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_ASSAULT, getGroup());

					iValue += iAssaultsHere * 100;

					CvCity* pCity = pLoopPlot->getPlotCity();

					if (pCity == NULL)
					{
						foreach_(const CvPlot * pAdjacentPlot, pLoopPlot->adjacent())
						{
							pCity = pAdjacentPlot->getPlotCity();

							if (pCity != NULL)
							{
								if (pCity->getOwner() == pLoopPlot->getOwner())
								{
									break;
								}
								pCity = NULL;
							}
						}
					}

					if (pCity != NULL)
					{
						PROFILE("CvUnitAI::AI_assaultSeaTransport.EvalCity");

						FAssert(isPotentialEnemy(pCity->getTeam(), pLoopPlot));

						if (!pLoopPlot->isRiverCrossing(directionXY(pLoopPlot, pCity->plot())))
						{
							iValue += (50 * -(GC.getRIVER_ATTACK_MODIFIER()));
						}

						iValue += 15 * (pLoopPlot->defenseModifier(getTeam(), false));
						iValue += 1000;
						iValue += (GET_PLAYER(getOwner()).AI_adjacentPotentialAttackers(pCity->plot()) * 200);

						// Continue attacking in area we have already captured cities
						if (pCity->area()->getCitiesPerPlayer(getOwner()) > 0 && pCity->AI_playerCloseness(getOwner()) > 5)
						{
							iValue *= 3;
							iValue /= 2;
						}
					}

					int iThisTurnValueIncrement = 0;

					if (pCity != NULL)
					{
						iThisTurnValueIncrement = GC.getGame().getSorenRandNum(50, "AI Assault");

						if (pCity->area()->getNumCities() > 1)
						{
							iThisTurnValueIncrement *= 2;
							iThisTurnValueIncrement += iValue;
						}
					}

					iValue *= 1000;
					iThisTurnValueIncrement *= 1000;

					if (iTargetCities <= iAssaultsHere)
					{
						iValue /= 2;
						iThisTurnValueIncrement /= 2;
					}

					if (iTargetCities == 1)
					{
						if (iCargoCount > 7)
						{
							iValue *= 3;
							iValue /= iCargoCount - 4;

							iThisTurnValueIncrement *= 3;
							iThisTurnValueIncrement /= iCargoCount - 4;
						}
					}

					if (pLoopPlot->isCity())
					{
						if (iVisibleEnemyDefenders * 3 > iCargoCount)
						{
							iValue /= 10;
							iThisTurnValueIncrement /= 10;
						}
						else
						{
							// Assume non-visible city is properly defended
							const int iDivisor = std::max(pLoopPlot->getPlotCity()->AI_neededDefenders(), (iVisibleEnemyDefenders * 3));

							iValue *= iCargoCount;
							iValue /= iDivisor;
							iThisTurnValueIncrement *= iCargoCount;
							iThisTurnValueIncrement /= iDivisor;
						}
					}
					else if (0 == iVisibleEnemyDefenders)
					{
						iValue *= 4;
						iValue /= 3;
						iThisTurnValueIncrement *= 4;
						iThisTurnValueIncrement /= 3;
					}
					else
					{
						iValue /= iVisibleEnemyDefenders;
						iThisTurnValueIncrement /= iVisibleEnemyDefenders;
					}

					int iPathTurns;
					if (iValue > 0 && generatePath(pLoopPlot, MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns, iBestValue == 0 ? MAX_INT : (iValue + iThisTurnValueIncrement) / iBestValue))
					{
						PROFILE("CvUnitAI::AI_assaultSeaTransport.ValidPath");

						if (iPathTurns <= 1)
						{
							iValue += iThisTurnValueIncrement;
						}

						iValue /= (iPathTurns + 1);

						if (iValue > iBestValue)
						{
							endTurnPlot = getPathEndTurnPlot();

							if (endTurnPlot == pLoopPlot || !exposedToDanger(endTurnPlot, 80))
							{
								iBestValue = iValue;
								pBestPlot = endTurnPlot;
								pBestAssaultPlot = pLoopPlot;
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestAssaultPlot != NULL))
	{
		PROFILE("CvUnitAI::AI_assaultSeaTransport.ProcessTarget");

		FAssert(!(pBestPlot->isImpassable(getTeam())));

		// Cancel missions of all those coming to join departing transport
		const CvPlayer& kPlayer = GET_PLAYER(getOwner());

		foreach_(CvSelectionGroup * pLoopGroup, kPlayer.groups())
		{
			if (pLoopGroup != getGroup() && pLoopGroup->AI_getMissionAIType() == MISSIONAI_GROUP && pLoopGroup->getHeadUnitAI() == AI_getUnitAIType())
			{
				const CvUnit* pMissionUnit = pLoopGroup->AI_getMissionAIUnit();

				if (pMissionUnit != NULL && pMissionUnit->getGroup() == getGroup())
				{
					pLoopGroup->clearMissionQueue();
				}
			}
		}

		if (pBestPlot == pBestAssaultPlot || stepDistance(pBestPlot->getX(), pBestPlot->getY(), pBestAssaultPlot->getX(), pBestAssaultPlot->getY()) == 1)
		{
			if (atPlot(pBestAssaultPlot))
			{
				getGroup()->unloadAll(); // XXX is this dangerous (not pushing a mission...) XXX air units?
				return true;
			}
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestAssaultPlot->getX(), pBestAssaultPlot->getY(), MOVE_AVOID_ENEMY_WEIGHT_3, false, false, MISSIONAI_ASSAULT, pBestAssaultPlot);
		}
		FAssert(!atPlot(pBestPlot));
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_WEIGHT_3, false, false, MISSIONAI_ASSAULT, pBestAssaultPlot);
	}

	return false;
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD					  02/07/10								jdog5000	  */
/*																							  */
/* Naval AI, Efficiency																		 */
/************************************************************************************************/
// Returns true if a mission was pushed...
bool CvUnitAI::AI_assaultSeaReinforce(bool bBarbarian)
{
	PROFILE_FUNC();

	FAssert(getGroup()->hasCargo());

	if (!canCargoAllMove() || !getGroup()->canAllMove())
	{
		return false;
	}

	std::vector<CvUnit*> aGroupCargo;
	foreach_(CvUnit * pLoopUnit, plot()->units())
	{
		const CvUnit* pTransport = pLoopUnit->getTransportUnit();
		if (pTransport != NULL && pTransport->getGroup() == getGroup())
		{
			aGroupCargo.push_back(pLoopUnit);
		}
	}

	//Never Called
	//TBHERE (Group Count or Group Volume?)
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* endTurnPlot = NULL;
	CvPlot* pBestAssaultPlot = NULL;
	CvPlot* pEndTurnPlot = NULL;
	CvArea* pWaterArea = plot()->waterArea();
	bool bCity = plot()->isCity(true, getTeam());
	bool bCanMoveAllTerrain = getGroup()->canMoveAllTerrain();

	int iValue;

	// Loop over nearby plots for groups in enemy territory to reinforce
	{
		PROFILE("AI_assaultSeaReinforce.Nearby");

		const int iRange = 2 * baseMoves();
		foreach_(CvPlot * pLoopPlot, plot()->rect(iRange, iRange))
		{
			if (pLoopPlot->isOwned() && isEnemy(pLoopPlot->getTeam(), pLoopPlot))
			{
				if (bCanMoveAllTerrain || (pWaterArea != NULL && pLoopPlot->isAdjacentToArea(pWaterArea)))
				{
					const int iTargetCities = pLoopPlot->area()->getCitiesPerPlayer(pLoopPlot->getOwner());

					if (iTargetCities > 0)
					{
						const int iOurFightersHere = pLoopPlot->getNumDefenders(getOwner());

						if (iOurFightersHere > 2)
						{
							int iPathTurns;
							if (generatePath(pLoopPlot, MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns))
							{
								if (iPathTurns <= 2)
								{
									pEndTurnPlot = getPathEndTurnPlot();

									iValue = 10 * iTargetCities;
									iValue += 8 * iOurFightersHere;
									iValue += 3 * GET_PLAYER(getOwner()).AI_adjacentPotentialAttackers(pLoopPlot);

									iValue *= 100;

									iValue /= (iPathTurns + 1);

									if (iValue > iBestValue)
									{
										iBestValue = iValue;
										pBestPlot = pEndTurnPlot;
										pBestAssaultPlot = pLoopPlot;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot == NULL) && (pBestAssaultPlot == NULL))
	{
		// Loop over other transport groups, looking for synchronized landing
		{
			PROFILE("AI_assaultSeaReinforce.Sync");

			foreach_(const CvSelectionGroup * pLoopSelectionGroup, GET_PLAYER(getOwner()).groups())
			{
				if (pLoopSelectionGroup != getGroup())
				{
					if (pLoopSelectionGroup->AI_getMissionAIType() == MISSIONAI_ASSAULT)
					{
						CvPlot* pLoopPlot = pLoopSelectionGroup->AI_getMissionAIPlot();

						if (pLoopPlot != NULL)
						{
							if (pLoopPlot->isOwned())
							{
								if (isPotentialEnemy(pLoopPlot->getTeam(), pLoopPlot))
								{
									if (bCanMoveAllTerrain || (pWaterArea != NULL && pLoopPlot->isAdjacentToArea(pWaterArea)))
									{
										const int iTargetCities = pLoopPlot->area()->getCitiesPerPlayer(pLoopPlot->getOwner());
										if (iTargetCities > 0)
										{
											const int iAssaultsHere = pLoopSelectionGroup->getCargo();//A count but flirting with the need to look deeper

											if (iAssaultsHere > 2)
											{
												//	Use approximate path lengths here as switching path calculations between stacks
												//	throws out all cached pathing data and is very expensive performance-wise
												int	iStepPathTurns = (10 * GC.getMap().calculatePathDistance(plot(), pLoopPlot) + 5) / (10 * baseMoves());
												int	iOtherStepPathTurns = (10 * GC.getMap().calculatePathDistance(pLoopSelectionGroup->plot(), pLoopPlot) + 5) / (10 * pLoopSelectionGroup->getHeadUnit()->baseMoves());

												if ((iStepPathTurns > iOtherStepPathTurns) && (iStepPathTurns < iOtherStepPathTurns + 6))
												{
													int iPathTurns;
													if (generatePath(pLoopPlot, MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns))
													{
														iValue = (iAssaultsHere * 5);
														iValue += iTargetCities * 10;

														iValue *= 100;

														// if more than 3 turns to get there, then put some randomness into our preference of distance
														// +/- 33%
														if (iPathTurns > 3)
														{
															int iPathAdjustment = GC.getGame().getSorenRandNum(67, "AI Assault Target");

															iPathTurns *= 66 + iPathAdjustment;
															iPathTurns /= 100;
														}

														iValue /= (iPathTurns + 1);

														if (iValue > iBestValue)
														{
															iBestValue = iValue;
															pBestPlot = getPathEndTurnPlot();
															pBestAssaultPlot = pLoopPlot;
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// Reinforce our cities in need
	if ((pBestPlot == NULL) && (pBestAssaultPlot == NULL))
	{
		{
			PROFILE("AI_assaultSeaReinforce.Cities");

			foreach_(CvCity * pLoopCity, GET_PLAYER(getOwner()).cities())
			{
				if (bCanMoveAllTerrain || (pWaterArea != NULL && (pLoopCity->waterArea(true) == pWaterArea || pLoopCity->secondWaterArea() == pWaterArea)))
				{
					iValue = 0;
					if (pLoopCity->area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE)
					{
						iValue = 3;
					}
					else if (pLoopCity->area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE)
					{
						iValue = 2;
					}
					else if (pLoopCity->area()->getAreaAIType(getTeam()) == AREAAI_MASSING)
					{
						iValue = 1;
					}
					else if (bBarbarian && (pLoopCity->area()->getCitiesPerPlayer(BARBARIAN_PLAYER) > 0))
					{
						iValue = 1;
					}
					else if (bBarbarian && (pLoopCity->area()->getCitiesPerPlayer(NEANDERTHAL_PLAYER) > 0))
					{
						iValue = 1;
					}

					if (iValue > 0)
					{
						bool bCityDanger = pLoopCity->AI_isDanger();
						if ((bCity && pLoopCity->area() != area()) || bCityDanger || ((GC.getGame().getGameTurn() - pLoopCity->getGameTurnAcquired()) < 10 && pLoopCity->getPreviousOwner() != NO_PLAYER))
						{
							int iOurPower = std::max(1, pLoopCity->area()->getPower(getOwner()));
							// Enemy power includes barb power
							int iEnemyPower = GET_TEAM(getTeam()).countEnemyPowerByArea(pLoopCity->area());

							// Don't send troops to areas we are dominating already
							// Don't require presence of enemy cities, just a dangerous force
							if (iOurPower < (3 * iEnemyPower))
							{
								int iPathTurns;
								if (generatePath(pLoopCity->plot(), MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns))
								{
									iValue *= 10 * pLoopCity->AI_cityThreat();

									iValue += 20 * GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_ASSAULT, getGroup());

									iValue *= std::min(iEnemyPower, 3 * iOurPower);
									iValue /= iOurPower;

									iValue *= 100;

									// if more than 3 turns to get there, then put some randomness into our preference of distance
									// +/- 33%
									if (iPathTurns > 3)
									{
										int iPathAdjustment = GC.getGame().getSorenRandNum(67, "AI Assault Target");

										iPathTurns *= 66 + iPathAdjustment;
										iPathTurns /= 100;
									}

									iValue /= (iPathTurns + 6);

									if (iValue > iBestValue)
									{
										iBestValue = iValue;
										pBestPlot = (bCityDanger ? getPathEndTurnPlot() : pLoopCity->plot());
										pBestAssaultPlot = pLoopCity->plot();
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot == NULL) && (pBestAssaultPlot == NULL))
	{
		if (bCity)
		{
			if (GET_TEAM(getTeam()).isAVassal())
			{
				PROFILE("AI_assaultSeaReinforce.Vassal");

				TeamTypes eMasterTeam = NO_TEAM;

				for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
				{
					if (GET_TEAM(getTeam()).isVassal((TeamTypes)iI))
					{
						eMasterTeam = (TeamTypes)iI;
					}
				}

				if ((eMasterTeam != NO_TEAM) && GET_TEAM(getTeam()).isOpenBorders(eMasterTeam))
				{
					for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).getTeam() == eMasterTeam)
						{
							foreach_(CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
							{
								if (pLoopCity->area() != area())
								{
									iValue = 0;
									if (pLoopCity->area()->getAreaAIType(eMasterTeam) == AREAAI_OFFENSIVE)
									{
										iValue = 2;
									}
									else if (pLoopCity->area()->getAreaAIType(eMasterTeam) == AREAAI_MASSING)
									{
										iValue = 1;
									}

									if (iValue > 0)
									{
										if (bCanMoveAllTerrain || (pWaterArea != NULL && (pLoopCity->waterArea(true) == pWaterArea || pLoopCity->secondWaterArea() == pWaterArea)))
										{
											int iOurPower = std::max(1, pLoopCity->area()->getPower(getOwner()));
											iOurPower += GET_TEAM(eMasterTeam).countPowerByArea(pLoopCity->area());
											// Enemy power includes barb power
											int iEnemyPower = GET_TEAM(eMasterTeam).countEnemyPowerByArea(pLoopCity->area());

											// Don't send troops to areas we are dominating already
											// Don't require presence of enemy cities, just a dangerous force
											if (iOurPower < (2 * iEnemyPower))
											{
												int iPathTurns;
												if (generatePath(pLoopCity->plot(), MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns))
												{
													iValue *= pLoopCity->AI_cityThreat();

													iValue += 10 * GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_ASSAULT, getGroup());

													iValue *= std::min(iEnemyPower, 3 * iOurPower);
													iValue /= iOurPower;

													iValue *= 100;

													// if more than 3 turns to get there, then put some randomness into our preference of distance
													// +/- 33%
													if (iPathTurns > 3)
													{
														int iPathAdjustment = GC.getGame().getSorenRandNum(67, "AI Assault Target");

														iPathTurns *= 66 + iPathAdjustment;
														iPathTurns /= 100;
													}

													iValue /= (iPathTurns + 1);

													if (iValue > iBestValue)
													{
														endTurnPlot = getPathEndTurnPlot();

														if (endTurnPlot == pLoopCity->plot() || !exposedToDanger(endTurnPlot, 60))
														{
															iBestValue = iValue;
															pBestPlot = endTurnPlot;
															pBestAssaultPlot = pLoopCity->plot();
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestAssaultPlot != NULL))
	{
		FAssert(!(pBestPlot->isImpassable(getTeam())));

		// Cancel missions of all those coming to join departing transport
		const CvPlayer& kPlayer = GET_PLAYER(getOwner());

		foreach_(CvSelectionGroup * pLoopGroup, kPlayer.groups())
		{
			if (pLoopGroup != getGroup())
			{
				if (pLoopGroup->AI_getMissionAIType() == MISSIONAI_GROUP && pLoopGroup->getHeadUnitAI() == AI_getUnitAIType())
				{
					const CvUnit* pMissionUnit = pLoopGroup->AI_getMissionAIUnit();

					if (pMissionUnit != NULL && pMissionUnit->getGroup() == getGroup())
					{
						pLoopGroup->clearMissionQueue();
					}
				}
			}
		}

		if ((pBestPlot == pBestAssaultPlot) || (stepDistance(pBestPlot->getX(), pBestPlot->getY(), pBestAssaultPlot->getX(), pBestAssaultPlot->getY()) == 1))
		{
			if (atPlot(pBestAssaultPlot))
			{
				getGroup()->unloadAll(); // XXX is this dangerous (not pushing a mission...) XXX air units?
				return true;
			}
			else
			{
				return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestAssaultPlot->getX(), pBestAssaultPlot->getY(), MOVE_AVOID_ENEMY_WEIGHT_3, false, false, MISSIONAI_ASSAULT, pBestAssaultPlot);
			}
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_WEIGHT_3, false, false, MISSIONAI_ASSAULT, pBestAssaultPlot);
		}
	}

	return false;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					   END												  */
/************************************************************************************************/



// Returns true if a mission was pushed...
bool CvUnitAI::AI_settlerSeaTransport()
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	CvPlot* pBestFoundPlot;
	CvPlot* endTurnPlot = NULL;
	bool bValid;
	int iValue;
	int iBestValue;
	int iI;

	FAssert(hasCargo());
	FAssert(getUnitAICargo(UNITAI_SETTLE) > 0);

	if (!canCargoAllMove())
	{
		return false;
	}

	//New logic should allow some new tricks like
	//unloading settlers when a better site opens up locally
	//and delivering settlers
	//to inland sites

	const CvArea* pWaterArea = plot()->waterArea();
	FAssertMsg(pWaterArea != NULL, "Ship out of water?");

	CvUnit* pSettlerUnit = NULL;
	const CvPlot* pPlot = plot();
	foreach_(CvUnit * pLoopUnit, pPlot->units())
	{
		if (pLoopUnit->getTransportUnit() == this)
		{
			if (pLoopUnit->AI_getUnitAIType() == UNITAI_SETTLE
                && static_cast<const CvUnitAI*>(pLoopUnit)->AI_canPlanColony())
			{
				pSettlerUnit = pLoopUnit;
				break;
			}
		}
	}

	if (!pSettlerUnit) return false;
    const CvPlot* colony = pSettlerUnit->getGroup()->AI_getMissionAIType() == MISSIONAI_LOAD_SETTLER
        ? pSettlerUnit->getGroup()->AI_getMissionAIPlot() : NULL;
    if (colony && !pSettlerUnit->canFound(colony)) {
        pSettlerUnit->getGroup()->AI_setMissionAI(NO_MISSIONAI, NULL, NULL);
        return false;
    }

	int iAreaBestFoundValue = 0;
	CvPlot* pAreaBestPlot = NULL;

	int iOtherAreaBestFoundValue = 0;
	CvPlot* pOtherAreaBestPlot = NULL;

	for (iI = 0; iI < GET_PLAYER(getOwner()).AI_getNumCitySites(); iI++)
	{
		CvPlot* pCitySitePlot = GET_PLAYER(getOwner()).AI_getCitySite(iI);
        if (!pSettlerUnit->canFound(pCitySitePlot) || (colony && colony != pCitySitePlot)) continue;
		if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pCitySitePlot, MISSIONAI_FOUND, getGroup()) == 0)
		{
			iValue = pCitySitePlot->getFoundValue(getOwner());
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD					  01/13/09								jdog5000	  */
			/*																							  */
			/* Settler AI																				   */
			/************************************************************************************************/
			/* original bts code
						if (pCitySitePlot->getArea() == getArea())
						{
							if (iValue > iAreaBestFoundValue)
							{
			*/
			// Only count city sites we can get to
			if (pCitySitePlot->getArea() == getArea() && pSettlerUnit->generatePath(pCitySitePlot, MOVE_SAFE_TERRITORY, true))
			{
				if (iValue > iAreaBestFoundValue)
				{
					/************************************************************************************************/
					/* BETTER_BTS_AI_MOD					   END												  */
					/************************************************************************************************/
					iAreaBestFoundValue = iValue;
					pAreaBestPlot = pCitySitePlot;
				}
			}
			else
			{
				if (iValue > iOtherAreaBestFoundValue)
				{
					iOtherAreaBestFoundValue = iValue;
					pOtherAreaBestPlot = pCitySitePlot;
				}
			}
		}
	}
	if ((0 == iAreaBestFoundValue) && (0 == iOtherAreaBestFoundValue))
	{
		return false;
	}

	if (iAreaBestFoundValue > iOtherAreaBestFoundValue)
	{
		//let the settler walk.
		getGroup()->unloadAll();
		getGroup()->pushMission(MISSION_SKIP);
		return true;
	}

	iBestValue = 0;
	pBestPlot = NULL;
	pBestFoundPlot = NULL;

	for (iI = 0; iI < GET_PLAYER(getOwner()).AI_getNumCitySites(); iI++)
	{
		CvPlot* pCitySitePlot = GET_PLAYER(getOwner()).AI_getCitySite(iI);
        if (!pSettlerUnit->canFound(pCitySitePlot) || (colony && colony != pCitySitePlot)) continue;
		if (!pCitySitePlot->isVisibleEnemyUnit(this))
		{
			if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pCitySitePlot, MISSIONAI_FOUND, getGroup(), 4) == 0)
			{
				int iPathTurns;
				// BBAI TODO: Nearby plots too if much shorter (settler walk from there)
				// also, if plots are in area player already has cities, then may not be coastal ... (see Earth 1000 AD map for Inca)
				if (generatePath(pCitySitePlot, MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns))
				{
					iValue = pCitySitePlot->getFoundValue(getOwner());
					iValue *= 1000;
					iValue /= (2 + iPathTurns);

					if (iValue > iBestValue)
					{
						endTurnPlot = getPathEndTurnPlot();

						if (endTurnPlot == pCitySitePlot || !exposedToDanger(endTurnPlot, 80))
						{
							iBestValue = iValue;
							pBestPlot = endTurnPlot;
							pBestFoundPlot = pCitySitePlot;
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestFoundPlot != NULL))
	{
		FAssert(!(pBestPlot->isImpassable(getTeam()))); // added getTeam()

		if ((pBestPlot == pBestFoundPlot) || (stepDistance(pBestPlot->getX(), pBestPlot->getY(), pBestFoundPlot->getX(), pBestFoundPlot->getY()) == 1))
		{
			if (atPlot(pBestFoundPlot))
			{
				unloadAll(); // XXX is this dangerous (not pushing a mission...) XXX air units?
				return true;
			}
			else
			{
				return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestFoundPlot->getX(), pBestFoundPlot->getY(), MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_FOUND, pBestFoundPlot);
			}
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_FOUND, pBestFoundPlot);
		}
	}

	//Try original logic
	//(sometimes new logic breaks)
	pPlot = plot();

	iBestValue = 0;
	pBestPlot = NULL;
	pBestFoundPlot = NULL;

	int iMinFoundValue = GET_PLAYER(getOwner()).AI_getMinFoundValue();

	for (iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndex(iI);
        if (!pSettlerUnit->canFound(pLoopPlot) || (colony && colony != pLoopPlot)) continue;

		if (pLoopPlot->isCoastalLand())
		{
			iValue = pLoopPlot->getFoundValue(getOwner());

			if ((iValue > iBestValue) && (iValue >= iMinFoundValue))
			{
				bValid = false;

				foreach_(const CvUnit * pLoopUnit, pPlot->units())
				{
					if (pLoopUnit->getTransportUnit() == this)
					{
						if (pLoopUnit->canFound(pLoopPlot))
						{
							bValid = true;
							break;
						}
					}
				}

				if (bValid)
				{
					if (!pLoopPlot->isVisibleEnemyUnit(this))
					{
						if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_FOUND, getGroup(), 4) == 0)
						{
							if (generatePath(pLoopPlot, MOVE_NO_ENEMY_TERRITORY, true))
							{
								iBestValue = iValue;
								pBestPlot = getPathEndTurnPlot();
								pBestFoundPlot = pLoopPlot;
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestFoundPlot != NULL))
	{
		FAssert(!(pBestPlot->isImpassable(getTeam()))); // added getTeam()

		if ((pBestPlot == pBestFoundPlot) || (stepDistance(pBestPlot->getX(), pBestPlot->getY(), pBestFoundPlot->getX(), pBestFoundPlot->getY()) == 1))
		{
			if (atPlot(pBestFoundPlot))
			{
				unloadAll(); // XXX is this dangerous (not pushing a mission...) XXX air units?
				return true;
			}
			else
			{
				return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestFoundPlot->getX(), pBestFoundPlot->getY(), MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_FOUND, pBestFoundPlot);
			}
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_FOUND, pBestFoundPlot);
		}
	}
	return false;
}


// Returns true if a mission was pushed...
// A ferry needs a job for this passenger, not merely a worker-role vacancy.
// Resource planters have graphical-only builds which the normal city improvement
// cache does not propose. Resolve their existing XML/Python build convention;
// canBuild remains authoritative, including resource access, cost and options.
bool CvUnitAI::AI_bonusPlantBuild(const CvCity* city, CvPlot** job, BuildTypes* build) const
{
    const CvPlayerAI& owner = GET_PLAYER(getOwner());
    int best = 0;
    foreach_(BuildTypes candidate, getUnitInfo().getBuilds())
    {
        const CvBuildInfo& info = GC.getBuildInfo(candidate);
        const ImprovementTypes improvement = (ImprovementTypes)info.getImprovement();
        if (improvement == NO_IMPROVEMENT || !GC.getImprovementInfo(improvement).isPlacesBonus()) continue;
        const char* name = info.getType();
        if (strncmp(name, "BUILD_BONUS_", 12) != 0) continue;
        const BonusTypes bonus = (BonusTypes)GC.getInfoTypeForString(name + 6, true);
        if (bonus == NO_BONUS) continue;
        // Planting cannot introduce a resource the empire does not possess.
        if (owner.getNumAvailableBonuses(bonus) <= 0) continue;
        // Master shares availability through the plot group; use the existing
        // raw-vicinity cache to avoid replicating a resource already here.
        if (city->hasRawVicinityBonus(bonus)) continue;
        const int value = 1 + std::max(0, owner.AI_bonusVal(bonus, 1));
        if (value <= best) continue;
        for (int i = 1; i < city->getNumCityPlots(); ++i)
        {
            CvPlot* tile = city->getCityIndexPlot(i);
            if (!tile || tile->getOwner() != getOwner() || tile->getWorkingCity() != city
                || tile->isWater() || tile->area() != city->area() || !tile->isRevealed(getTeam(), false)
                || tile->getBonusType(getTeam()) != NO_BONUS
                || tile->isVisibleEnemyUnit(this)) continue;
            const ImprovementTypes current = tile->getImprovementType();
            // Do not destroy forts, settlements or unrelated productive works.
            if (current != NO_IMPROVEMENT && !GC.getImprovementInfo(current).isImprovementBonusTrade(bonus)) continue;
            if (owner.AI_plotTargetMissionAIs(tile, MISSIONAI_BUILD, getGroup()) > 0 || !canBuild(tile, candidate)) continue;
            best = value; *job = tile; *build = candidate;
            break;
        }
    }
    return best > 0;
}

bool CvUnitAI::AI_bonusPlantMove()
{
    // Ordinary workers pay only a walk through their short XML build list.
    bool planter = false;
    foreach_(BuildTypes build, getUnitInfo().getBuilds())
    {
        const ImprovementTypes improvement = (ImprovementTypes)GC.getBuildInfo(build).getImprovement();
        if (improvement != NO_IMPROVEMENT && GC.getImprovementInfo(improvement).isPlacesBonus()) { planter = true; break; }
    }
    if (!planter || isCargo()) return false;
    std::vector<std::pair<int, CvCity*> > cities;
    foreach_(CvCity* city, GET_PLAYER(getOwner()).cities())
        if (UnitTravelConnectivity::landUnitCanReach(this,city->plot())) cities.push_back(std::make_pair(stepDistance(getX(), getY(), city->getX(), city->getY()), city));
    // Stable ties preserve city iteration order; do not compare raw pointers.
    for (size_t i = 1; i < cities.size(); ++i)
    {
        const std::pair<int, CvCity*> entry = cities[i]; size_t j = i;
        while (j > 0 && cities[j-1].first > entry.first) { cities[j] = cities[j-1]; --j; }
        cities[j] = entry;
    }
    for (size_t i = 0; i < cities.size(); ++i)
    {
        CvPlot* job = NULL; BuildTypes build = NO_BUILD; int turns;
        if (AI_bonusPlantBuild(cities[i].second, &job, &build)
            && generatePath(job, MOVE_SAFE_TERRITORY, true, &turns))
        {
            if (!atPlot(job) && !getGroup()->pushMissionInternal(MISSION_MOVE_TO, job->getX(), job->getY(), MOVE_SAFE_TERRITORY, false, false, MISSIONAI_BUILD, job)) continue;
            getGroup()->pushMission(MISSION_BUILD, build, -1, 0, getGroup()->getLengthMissionQueue() > 0, false, MISSIONAI_BUILD, job);
            return true;
        }
    }
    return false;
}

bool CvUnitAI::AI_workerFerryJob(const CvCity* city, bool* planterJob) const
{
    CvPlot* job = NULL; BuildTypes build = NO_BUILD;
    if (planterJob) *planterJob = false;
    if (AI_bonusPlantBuild(city, &job, &build)) { if (planterJob) *planterJob = true; return true; }
    if (city->AI_getWorkersNeeded() <= city->getNumWorkers()) return false;
    // Cached city proposals are cheap; revalidate this worker's capabilities and
    // current legality. No land path is inferred from trade connectivity.
    for (int i = 1; i < city->getNumCityPlots(); ++i)
    {
        if (city->AI_getBestBuildValue(i) <= 0) continue;
        const BuildTypes candidate = city->AI_getBestBuild(i);
        CvPlot* tile = city->getCityIndexPlot(i);
        if (candidate != NO_BUILD && tile && tile->getOwner() == getOwner() && tile->getWorkingCity() == city
            && tile->area() == city->area() && tile->isRevealed(getTeam(), false)
            && !tile->isVisibleEnemyUnit(this)
            && GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(tile, MISSIONAI_BUILD, getGroup()) == 0
            && canBuild(tile, candidate)) return true;
    }
    return false;
}

bool CvUnitAI::AI_workerFerryDestination(const CvUnitAI* ship) const
{
    // Explicit rescue remains available. Otherwise land work is tried before
    // embarking and a ferry must lead to another landmass with compatible work.
    if (getGroup()->isStranded()) return true;
    // An ordinary builder may accompany a settler to a future city. A resource
    // planter is not an interchangeable construction worker for that mission.
    if (ship->getUnitAICargo(UNITAI_SETTLE) > 0)
        foreach_(BuildTypes build, getUnitInfo().getBuilds())
        {
            const CvBuildInfo& info = GC.getBuildInfo(build);
            const ImprovementTypes improvement = (ImprovementTypes)info.getImprovement();
            if (info.getRoute() != NO_ROUTE || improvement != NO_IMPROVEMENT && !GC.getImprovementInfo(improvement).isPlacesBonus()) return true;
        }
    foreach_(const CvCity* city, GET_PLAYER(getOwner()).cities())
    {
        if (!UnitTravelConnectivity::landUnitCanReach(this,city->plot()) && UnitTravelConnectivity::seaUnitCanReach(ship,city->plot()) && AI_workerFerryJob(city) && ship->AI_plotValid(city->plot()))
        {
            int turns;
            if (ship->generatePath(city->plot(), 0, true, &turns)) return true;
        }
    }
    return false;
}

// Returns true if a mission was pushed or a passenger reached a useful port.
bool CvUnitAI::AI_settlerSeaFerry()
{
    PROFILE_FUNC();
    FAssert(hasCargo());
    FAssert(getUnitAICargo(UNITAI_WORKER) > 0);
    if (!canCargoAllMove()) return false;
    const CvPlot* accepted = m_aiNavalCommitment[NavalCommitment::KIND] == NavalCommitment::DELIVERY
        ? GC.getMap().plot(m_aiNavalCommitment[NavalCommitment::X], m_aiNavalCommitment[NavalCommitment::Y]) : NULL;
    int best = 0;
    CvPlot* destination = NULL;
    CvPlot* waypoint = NULL;
    foreach_(const CvCity* city, GET_PLAYER(getOwner()).cities())
    {
        if (accepted && city->plot() != accepted) continue;
        int iValue = std::max(0, city->AI_getWorkersNeeded() - city->getNumWorkers());
        bool useful = false;
        bool ordinaryWorker = false;
        foreach_(const CvUnit* cargo, plot()->units())
        {
            if (cargo->getOwner() != getOwner() || cargo->getTransportUnit() != this || cargo->AI_getUnitAIType() != UNITAI_WORKER) continue;
            bool planterJob = false;
            if (static_cast<const CvUnitAI*>(cargo)->AI_workerFerryJob(city, &planterJob))
            { useful = true; if (!planterJob) { ordinaryWorker = true; break; } }
        }
        if (!useful) continue;
        // A specialist's concrete planting job is useful even if ordinary
        // worker staffing is full. Ordinary passengers still require a deficit.
        iValue = ordinaryWorker ? std::max(1, iValue) : 1;
        iValue -= GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(city->plot(), MISSIONAI_FOUND, getGroup());
        int turns;
        if (iValue > 0 && UnitTravelConnectivity::seaUnitCanReach(this,city->plot()) && generatePath(city->plot(), 0, true, &turns))
        {
            iValue = iValue * 1000 / (4 + turns);
            if (atPlot(city->plot())) iValue += 100;
            else if (getGroup()->AI_getMissionAIType() == MISSIONAI_FOUND && getGroup()->AI_getMissionAIPlot() == city->plot()) iValue += 100000;
            if (iValue > best)
            {
                CvPlot* end = getPathEndTurnPlot();
                if (end && (end == city->plot() || !atPlot(end) && !exposedToDanger(end, 80)))
                { best = iValue; destination = city->plot(); waypoint = end; }
            }
        }
    }
    // A rescued worker without a remaining job should be landed safely, not
    // carried forever. Jobless ordinary boarding is gated in AI_load.
    if (!destination && plot()->getPlotCity() && plot()->getOwner() == getOwner()) destination = plot();
    if (!destination) return false;
    if (atPlot(destination))
    {
        // Leave settlers/escorts and incompatible workers aboard. Snapshot IDs:
        // unloading changes group membership and may change the plot unit list.
        std::vector<int> passengers;
        foreach_(const CvUnit* cargo, plot()->units())
            if (cargo->getOwner() == getOwner() && cargo->getTransportUnit() == this && cargo->AI_getUnitAIType() == UNITAI_WORKER)
                passengers.push_back(cargo->getID());
        bool unloaded = false;
        int slots = std::max(0, destination->getPlotCity()->AI_getWorkersNeeded() - destination->getPlotCity()->getNumWorkers());
        for (size_t i = 0; i < passengers.size(); ++i)
        {
            CvUnitAI* worker = static_cast<CvUnitAI*>(GET_PLAYER(getOwner()).getUnit(passengers[i]));
            if (worker && worker->getTransportUnit() == this && worker->canUnload())
            {
                CvPlot* job = NULL; BuildTypes build = NO_BUILD;
                const bool planterJob = worker->AI_bonusPlantBuild(destination->getPlotCity(), &job, &build);
                if (waypoint && !planterJob && (slots <= 0 || !worker->AI_workerFerryJob(destination->getPlotCity()))) continue;
                worker->setTransportUnit(NULL); unloaded = true;
                if (!planterJob) --slots;
            }
        }
        return unloaded;
    }
    AI_rememberNavalCommitment(NavalCommitment::DELIVERY, destination, 0, MISSIONAI_FOUND);
    return getGroup()->pushMissionInternal(MISSION_MOVE_TO, waypoint->getX(), waypoint->getY(), 0, false, false, MISSIONAI_FOUND, destination);
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_specialSeaTransportMissionary()
{
	PROFILE_FUNC();

	CvCity* pCity;
	CvUnit* pMissionaryUnit;
	CvPlot* pLoopPlot;
	CvPlot* pPlot;
	CvPlot* pBestPlot;
	CvPlot* pBestSpreadPlot;
	int iPathTurns;
	int iValue;
	int iCorpValue;
	int iBestValue;
	int iJ;
	bool bExecutive = false;

	FAssert(hasCargo());
	FAssert(getUnitAICargo(UNITAI_MISSIONARY) > 0);

	if (!canCargoAllMove())
	{
		return false;
	}

	pPlot = plot();

	pMissionaryUnit = NULL;

	foreach_(CvUnit * pLoopUnit, pPlot->units())
	{
		if (pLoopUnit->getTransportUnit() == this)
		{
			if (pLoopUnit->AI_getUnitAIType() == UNITAI_MISSIONARY)
			{
				pMissionaryUnit = pLoopUnit;
				break;
			}
		}
	}

	if (pMissionaryUnit == NULL)
	{
		return false;
	}

	iBestValue = 0;
	pBestPlot = NULL;
	pBestSpreadPlot = NULL;

	// XXX what about non-coastal cities?
	CvReachablePlotSet plotSet(getGroup(), MOVE_ALLOW_ADJACENT_COASTAL, MAX_INT);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		pLoopPlot = itr.plot();

		if (pLoopPlot->isCoastalLand())
		{
			pCity = pLoopPlot->getPlotCity();

			if (pCity != NULL)
			{
				iValue = 0;
				iCorpValue = 0;

				for (iJ = 0; iJ < GC.getNumReligionInfos(); iJ++)
				{
					if (pMissionaryUnit->canSpread(pLoopPlot, ((ReligionTypes)iJ)))
					{
						if (GET_PLAYER(getOwner()).getStateReligion() == ((ReligionTypes)iJ))
						{
							iValue += 3;
						}

						if (GET_PLAYER(getOwner()).hasHolyCity((ReligionTypes)iJ))
						{
							iValue++;
						}
					}
				}

				for (iJ = 0; iJ < GC.getNumCorporationInfos(); iJ++)
				{
					if (pMissionaryUnit->canSpreadCorporation(pLoopPlot, ((CorporationTypes)iJ)))
					{
						if (GET_PLAYER(getOwner()).hasHeadquarters((CorporationTypes)iJ))
						{
							iCorpValue += 3;
						}
					}
				}

				if (iValue > 0)
				{
					if (!pLoopPlot->isVisibleEnemyUnit(this))
					{
						if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_SPREAD, getGroup()) == 0)
						{
							if (generatePath(pLoopPlot, 0, true, &iPathTurns))
							{
								iValue *= pCity->getPopulation();

								if (pCity->getOwner() == getOwner())
								{
									iValue *= 4;
								}
								else if (pCity->getTeam() == getTeam())
								{
									iValue *= 3;
								}

								if (pCity->getReligionCount() == 0)
								{
									iValue *= 2;
								}

								iValue /= (pCity->getReligionCount() + 1);

								if (iPathTurns == 1)
								{
									iValue *= 2;
								}

								iValue *= 1000;

								iValue /= (iPathTurns + 1);

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestPlot = getPathEndTurnPlot();
									pBestSpreadPlot = pLoopPlot;
									bExecutive = false;
								}
							}
						}
					}
				}

				if (iCorpValue > 0)
				{
					if (!pLoopPlot->isVisibleEnemyUnit(this))
					{
						if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_SPREAD_CORPORATION, getGroup()) == 0)
						{
							if (generatePath(pLoopPlot, 0, true, &iPathTurns))
							{
								iCorpValue *= pCity->getPopulation();

								FAssert(iPathTurns > 0);

								if (iPathTurns == 1)
								{
									iCorpValue *= 2;
								}
								iCorpValue *= 1000;

								iCorpValue /= (iPathTurns + 1);

								if (iCorpValue > iBestValue)
								{
									iBestValue = iCorpValue;
									pBestPlot = getPathEndTurnPlot();
									pBestSpreadPlot = pLoopPlot;
									bExecutive = true;
								}
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestSpreadPlot != NULL))
	{
		FAssert(!(pBestPlot->isImpassable(getTeam())) || canMoveImpassable()); // added getTeam()

		if ((pBestPlot == pBestSpreadPlot) || (stepDistance(pBestPlot->getX(), pBestPlot->getY(), pBestSpreadPlot->getX(), pBestSpreadPlot->getY()) == 1))
		{
			if (atPlot(pBestSpreadPlot))
			{
				unloadAll(); // XXX is this dangerous (not pushing a mission...) XXX air units?
				return true;
			}
			else
			{
				return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestSpreadPlot->getX(), pBestSpreadPlot->getY(), 0, false, false, bExecutive ? MISSIONAI_SPREAD_CORPORATION : MISSIONAI_SPREAD, pBestSpreadPlot);
			}
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, bExecutive ? MISSIONAI_SPREAD_CORPORATION : MISSIONAI_SPREAD, pBestSpreadPlot);
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_specialSeaTransportSpy()
{
	PROFILE_FUNC();

	FAssert(hasCargo());
	FAssert(getUnitAICargo(UNITAI_SPY) > 0);

	if (!canCargoAllMove())
	{
		return false;
	}
	const CvPlayerAI& owner = GET_PLAYER(getOwner());
	int iBestValue = 0;
	PlayerTypes eBestPlayer = NO_PLAYER;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getTeam(), false)
		&& owner.AI_getAttitude((PlayerTypes)iI) <= ATTITUDE_ANNOYED)
		{
			const int iValue = GET_PLAYER((PlayerTypes)iI).getTotalPopulation();

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestPlayer = ((PlayerTypes)iI);
			}
		}
	}
	if (eBestPlayer == NO_PLAYER)
	{
		return false;
	}
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestSpyPlot = NULL;

	CvReachablePlotSet plotSet(getGroup(), MOVE_ALLOW_ADJACENT_COASTAL, MAX_INT);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		CvPlot* pLoopPlot = itr.plot();
		if (pLoopPlot->isCoastalLand() && pLoopPlot->getOwner() == eBestPlayer)
		{
			int iValue = 1000 * pLoopPlot->area()->getCitiesPerPlayer(eBestPlayer);

			if (iValue > iBestValue && owner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_ATTACK_SPY, getGroup(), 4) == 0)
			{
				int iPathTurns;
				if (generatePath(pLoopPlot, 0, true, &iPathTurns))
				{
					iValue /= iPathTurns + 1;

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = getPathEndTurnPlot();
						pBestSpyPlot = pLoopPlot;
					}
				}
			}
		}
	}
	if (pBestPlot != NULL && pBestSpyPlot != NULL)
	{
		FAssert(!pBestPlot->isImpassable(getTeam()));

		if (pBestPlot == pBestSpyPlot || 1 == stepDistance(pBestPlot->getX(), pBestPlot->getY(), pBestSpyPlot->getX(), pBestSpyPlot->getY()))
		{
			if (atPlot(pBestSpyPlot))
			{
				unloadAll(); // XXX is this dangerous (not pushing a mission...) XXX air units?
				return true;
			}
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestSpyPlot->getX(), pBestSpyPlot->getY(), 0, false, false, MISSIONAI_ATTACK_SPY, pBestSpyPlot);
		}
		FAssert(!atPlot(pBestPlot));
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_ATTACK_SPY, pBestSpyPlot);
	}
	return false;
}


// Returns true if a mission was pushed...
int CvUnitAI::AI_carrierPositionValue(const CvPlot* station, int airRange,
    std::vector<std::pair<int,int> >& targetValues) const
{
    // CARRIER_PROBE_SCORE
    int value = 0;
    foreach_(const CvPlot* target, station->rect(airRange, airRange)) {
        const int distance = plotDistance(station->getX(), station->getY(), target->getX(), target->getY());
        if (distance > airRange) continue;
        // Empty water and ineligible plots have a cheap existing rejection.
        // Memoize the costly support/defender counts only after that gate.
        if (target->isNPC() || !potentialWarAction(target)) continue;
        if (targetValues.empty()) targetValues.assign(GC.getMap().numPlots(), std::make_pair(-1,-1));
        std::pair<int,int>& cached = targetValues[GC.getMap().plotNum(target->getX(), target->getY())];
        if (cached.first == -1) {
            cached.first = target->isCity()
                ? 3 + GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(target, MISSIONAI_ASSAULT, getGroup(), 2) * 6 : 0;
            if (target->getImprovementType() != NO_IMPROVEMENT) cached.first += 2;
        } else { // CARRIER_PROBE_TARGET_HIT
        }
        value += cached.first;
        if (distance <= airRange / 2) {
            if (cached.second == -1)
                cached.second = target->plotCount(PUF_canDefend, -1, -1, NULL, getOwner());
            value += cached.second;
        }
    }
    return value;
}

bool CvUnitAI::AI_carrierSeaTransport()
{
	PROFILE_FUNC();

	int iPathTurns;

	int iMaxAirRange = 0;

	std::vector<CvUnit*> aCargoUnits;
	getCargoUnits(aCargoUnits);
	if (!aCargoUnits.empty())
	{
		validateCargoUnits();
	}
	foreach_(const CvUnit * pCargoUnit, aCargoUnits)
	{
		iMaxAirRange = std::max(iMaxAirRange, pCargoUnit->airRange());
	}

	if (iMaxAirRange == 0)
	{
		return false;
	}

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestCarrierPlot = NULL;

    // CARRIER_PROBE_SCAN
    std::vector<std::pair<int,int> > targetValues;
	CvReachablePlotSet plotSet(getGroup(), 0, MAX_INT);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		const CvPlot* pLoopPlot = itr.plot();

		//if (AI_plotValid(pLoopPlot))
		{
			if (pLoopPlot->isWater() && pLoopPlot->isAdjacentToLand())
			{
				if (!pLoopPlot->isVisibleEnemyUnit(this))
				{
                    int iValue = AI_carrierPositionValue(pLoopPlot, iMaxAirRange, targetValues);
					if (iValue > 0)
					{
						iValue *= 1000;

						foreach_(const CvPlot * pDirectionPlot, pLoopPlot->adjacent())
						{
							if (pDirectionPlot->isCity() && isEnemy(pDirectionPlot->getTeam(), pLoopPlot))
							{
								iValue /= 2;
								break;
							}
						}

						if (iValue > iBestValue)
						{
							const bool bStealth = (getInvisibleType() != NO_INVISIBLE);
							if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_CARRIER, getGroup(), bStealth ? 5 : 3) <= (bStealth ? 0 : 3))
							{
								if (generatePath(pLoopPlot, 0, true, &iPathTurns))
								{
									iValue /= (iPathTurns + 1);

									if (iValue > iBestValue)
									{
										const CvPlot* endTurnPlot = getPathEndTurnPlot();

										if (endTurnPlot == pLoopPlot || !exposedToDanger(endTurnPlot, 70))
										{
											iBestValue = iValue;
											pBestPlot = endTurnPlot;
											pBestCarrierPlot = pLoopPlot;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestCarrierPlot != NULL))
	{
		if (atPlot(pBestCarrierPlot))
		{
			if (getGroup()->hasCargo())
			{
				const CvPlot* pPlot = plot();

				const int iNumUnits = pPlot->getNumUnits();

				for (int i = 0; i < iNumUnits; ++i)
				{
					bool bDone = true;
					foreach_(const CvUnit * pCargoUnit, pPlot->units())
					{
						if (pCargoUnit->isCargo())
						{
							FAssert(pCargoUnit->getTransportUnit() != NULL);
							if (pCargoUnit->getOwner() == getOwner() && (pCargoUnit->getTransportUnit()->getGroup() == getGroup()) && (pCargoUnit->getDomainType() == DOMAIN_AIR))
							{
								if (pCargoUnit->canMove() && pCargoUnit->isGroupHead())
								{
									// careful, this might kill the cargo group
									if (pCargoUnit->getGroup()->AI_update())
									{
										bDone = false;
										break;
									}
								}
							}
						}
					}

					if (bDone)
					{
						break;
					}
				}
			}

			if (canPlunder(pBestCarrierPlot))
			{
				getGroup()->pushMission(MISSION_PLUNDER, -1, -1, 0, false, false, MISSIONAI_CARRIER, pBestCarrierPlot);
			}
			else
			{
				getGroup()->pushMission(MISSION_SKIP);
			}
			return true;
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_CARRIER, pBestCarrierPlot);
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_connectPlot(CvPlot* pPlot, int iRange)
{
	AI_PERF_SCOPE(WORKER_CONNECT);
	PROFILE_FUNC();

	FAssert(canBuildRoute());

	if (getDomainType() == DOMAIN_LAND
	&& !UnitTravelConnectivity::landUnitCanReach(this,pPlot)
	&& !getGroup()->canMoveAllTerrain())
	{
		return false;
	}
	int iBasePathFlags = MOVE_SAFE_TERRITORY | MOVE_AVOID_ENEMY_UNITS | MOVE_IGNORE_DANGER | MOVE_RECONSIDER_ON_LEAVING_OWNED;

	if (!pPlot->isVisibleEnemyUnit(this)
	&& GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pPlot, MISSIONAI_BUILD, getGroup(), iRange) == 0
	&& generatePath(pPlot, MOVE_IGNORE_DANGER | MOVE_NO_ENEMY_TERRITORY, true))
	{
		bool bHasPossibleTargetCity = false;

		if (atPlot(pPlot))
		{
			foreach_(const CvCity * cityX, GET_PLAYER(getOwner()).cities())
			{
				if (pPlot->isConnectedTo(cityX))
				{
					continue;
				}
				const CvPlot* cityPlot = cityX->plot();

				if (cityPlot->isVisibleEnemyUnit(this))
				{
					continue;
				}
				bHasPossibleTargetCity = true;

				FAssertMsg(pPlot->getPlotCity() != cityX, "pPlot->getPlotCity() is not expected to be equal with cityX");

				if (plot()->getPlotGroup(getOwner()) == cityPlot->getPlotGroup(getOwner()))
				{
					PROFILE("CvUnitAI::AI_connectPlot.StrangeCase");

					if (generatePath(cityPlot, MOVE_WITH_CAUTION | MOVE_NO_ENEMY_TERRITORY, true))
					{
						return getGroup()->pushMissionInternal(MISSION_ROUTE_TO, cityPlot->getX(), cityPlot->getY(), MOVE_WITH_CAUTION | MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_BUILD, cityPlot);
					}
				}
			}
			if (!bHasPossibleTargetCity)
			{
				return false;
			}
			CvReachablePlotSet plotSet(getGroup(), iBasePathFlags);
			plotSet.Populate(MAX_INT);

			foreach_(const CvCity * cityX, GET_PLAYER(getOwner()).cities())
			{
				if (pPlot->isConnectedTo(cityX))
				{
					continue;
				}
				CvPlot* cityPlot = cityX->plot();
				if (plotSet.find(cityPlot) == plotSet.end())
				{
					continue;
				}
				FAssertMsg(pPlot->getPlotCity() != cityX, "pPlot->getPlotCity() is not expected to be equal with cityX");

				if (!cityPlot->isVisibleEnemyUnit(this) && generatePath(cityPlot, MOVE_WITH_CAUTION | MOVE_NO_ENEMY_TERRITORY, true))
				{
					return getGroup()->pushMissionInternal(MISSION_ROUTE_TO, cityPlot->getX(), cityPlot->getY(), MOVE_WITH_CAUTION | MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_BUILD, cityPlot);
				}
			}
		}
		else if (generatePath(pPlot, MOVE_WITH_CAUTION | MOVE_NO_ENEMY_TERRITORY, true))
		{
			return getGroup()->pushMissionInternal(MISSION_ROUTE_TO, pPlot->getX(), pPlot->getY(), MOVE_WITH_CAUTION | MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_BUILD, pPlot);
		}

		// We have an ignore-danger path (the gate above proved it) but no danger-free
        // one. Parking on WAIT_FOR_ESCORT only makes sense if an escort can actually
        // arrive. Two cases where it never will, so the worker would stall forever:
        //   - a human's automated worker (the player's military isn't auto-dispatched
        //     to babysit it) -- the "Automate Network does nothing" bug; and
        //   - any worker stranded ABROAD (on another player's soil): our military won't
        //     path into foreign territory to reach it, which is exactly how Portuguese
        //     workers ended up sitting in England's borders for many turns.
        // In both cases, commit to the work via the ignore-danger path instead (which
        // for an abroad worker also walks it back off the foreign tile toward an own
        // target); if we're already on the plot with no reachable city, return false so
        // the caller falls through to its next option (improve/route/retreat/safety).
        if (isHuman() || IsAbroad())
        {
            if (!atPlot(pPlot))
            {
                return getGroup()->pushMissionInternal(MISSION_ROUTE_TO, pPlot->getX(), pPlot->getY(), MOVE_IGNORE_DANGER | MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_BUILD, pPlot);
            }
            return false;
        }
		if (!atPlot(pPlot)) AI_rememberCivilianDecision(CivilianDecision::WORKER_CONNECTION, pPlot, iRange);
        getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT, pPlot);
		return true;
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_improveCity(CvCity* pCity)
{
	AI_PERF_SCOPE(WORKER_CITY);
	PROFILE_FUNC();

	CvPlot* pBestPlot;
	BuildTypes eBestBuild;

	if (AI_bestCityBuild(pCity, &pBestPlot, &eBestBuild, NULL, this))
	{
		FAssertMsg(pBestPlot != NULL, "BestPlot is not assigned a valid value");
		FASSERT_BOUNDS(0, GC.getNumBuildInfos(), eBestBuild);

		MissionTypes eMission;
		if ((plot()->getWorkingCity() != pCity) || (GC.getBuildInfo(eBestBuild).getRoute() != NO_ROUTE))
		{
			eMission = MISSION_ROUTE_TO;
		}
		else
		{
			int iPathTurns;

			eMission = MISSION_MOVE_TO;
			if (NULL != pBestPlot && generatePath(pBestPlot, MOVE_WITH_CAUTION, false, &iPathTurns) && (iPathTurns == 1) && (getPathMovementRemaining() == 0))
			{
				if (pBestPlot->getRouteType() != NO_ROUTE)
				{
					eMission = MISSION_ROUTE_TO;
				}
			}
			else if (plot()->getRouteType() == NO_ROUTE)
			{
				int iPlotMoveCost = GC.getTerrainInfo(plot()->getTerrainType()).getMovementCost();

				if (plot()->getFeatureType() != NO_FEATURE)
				{
					iPlotMoveCost += GC.getFeatureInfo(plot()->getFeatureType()).getMovementCost();
				}

				if (plot()->isHills())
				{
					iPlotMoveCost += GC.getHILLS_EXTRA_MOVEMENT();
				}

				if (plot()->isAsPeak())
				{
					if (!GET_TEAM(getTeam()).isMoveFastPeaks())
					{
						iPlotMoveCost += GC.getPEAK_EXTRA_MOVEMENT();
					}
					iPlotMoveCost += 3;
				}

				if (iPlotMoveCost > 1)
				{
					eMission = MISSION_ROUTE_TO;
				}
			}
			if (getGroup()->pushMissionInternal(eMission, pBestPlot->getX(), pBestPlot->getY(), isHuman() ? 0 : MOVE_WITH_CAUTION, false, false, MISSIONAI_BUILD, pBestPlot))
			{
				getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, (getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);

				return true;
			}
		}

		eBestBuild = AI_betterPlotBuild(pBestPlot, eBestBuild);

		if (getGroup()->pushMissionInternal(eMission, pBestPlot->getX(), pBestPlot->getY(), isHuman() ? 0 : MOVE_WITH_CAUTION, false, false, MISSIONAI_BUILD, pBestPlot))
		{
			getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, (getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);

			return true;
		}
	}

	return false;
}

bool CvUnitAI::AI_improveLocalPlot(int iRange, const CvCity* pIgnoreCity)
{
	AI_PERF_SCOPE(WORKER_LOCAL);
	PROFILE_EXTRA_FUNC();
	const CvPlayerAI& kOwner = GET_PLAYER(getOwner());
	const ImprovementTypes eRuins = GC.getIMPROVEMENT_CITY_RUINS();
	const bool bSafeAutomation = kOwner.isOption(PLAYEROPTION_SAFE_AUTOMATION);

	int iPathTurns;
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	bool plotIsInCity = false;
	BuildTypes eBestBuild = NO_BUILD;

	foreach_(const CvPlot * pLoopPlot, plot()->rect(iRange, iRange))
	{
		if (pLoopPlot->isCityRadius())
		{
			CvCity* pCity = pLoopPlot->getWorkingCity();
			if (NULL != pCity && pCity->getOwner() == getOwner() && (NULL == pIgnoreCity || pCity != pIgnoreCity) && AI_plotValid(pLoopPlot))
			{
				plotIsInCity = true;
				const int iIndex = pCity->getCityPlotIndex(pLoopPlot);
				const BuildTypes bestBuildForCurrentCity = pCity->AI_getBestBuild(iIndex);
				const ImprovementTypes proposedImprovement = bestBuildForCurrentCity == NO_BUILD ? NO_IMPROVEMENT : GC.getBuildInfo(bestBuildForCurrentCity).getImprovement();
				const ImprovementTypes currentImprovementOnPlot = pLoopPlot->getImprovementInLayer(proposedImprovement);
				const int candidateValue = pCity->AI_getBestBuildValue(iIndex);
				const int boostedValue = atPlot(pLoopPlot) ? candidateValue*3/2 : candidateValue;
				const int maxWinningTurns = workerImprovementPathLimit((__int64)boostedValue*1000,1,iBestValue);

				if (iIndex != CITY_HOME_PLOT && bestBuildForCurrentCity != NO_BUILD
				&& (!bSafeAutomation || currentImprovementOnPlot == NO_IMPROVEMENT || currentImprovementOnPlot == eRuins
					|| pLoopPlot->canAddAdditionalImprovement(proposedImprovement))
				&& (currentImprovementOnPlot == NO_IMPROVEMENT || GC.getBuildInfo(bestBuildForCurrentCity).getImprovement() != NO_IMPROVEMENT)
				&& (NULL == pIgnoreCity || pCity->AI_getWorkersNeeded() > 0 && pCity->getNumWorkers() < 1 + pCity->AI_getWorkersNeeded() * 2 / 3)
				&& maxWinningTurns>=0
				&& canBuild(pLoopPlot, bestBuildForCurrentCity)
				&& generatePath(pLoopPlot, isHuman() ? 0 : MOVE_WITH_CAUTION, true, &iPathTurns,maxWinningTurns))
				{
					int iValue = candidateValue;
					int iMaxWorkers = 1;
					if (plot() == pLoopPlot)
					{
						iValue *= 3;
						iValue /= 2;
					}
					else if (getPathMovementRemaining() == 0)
					{
						iPathTurns++;
					}
					else if (iPathTurns <= 1)
					{
						iMaxWorkers = AI_calculatePlotWorkersNeeded(pLoopPlot, bestBuildForCurrentCity);
					}

					if (kOwner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD, getGroup()) < iMaxWorkers)
					{
						iValue *= 1000;
						iValue /= 1 + iPathTurns;

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = pLoopPlot;
							eBestBuild = bestBuildForCurrentCity;
						}
					}
				}
			}
		}
	}
	if (pBestPlot != NULL)
	{
		FASSERT_BOUNDS(0, GC.getNumBuildInfos(), eBestBuild);

		FAssert(pBestPlot->getWorkingCity() != NULL);

		MissionTypes eMission = MISSION_MOVE_TO;

		if (generatePath(pBestPlot, 0, true, &iPathTurns) && iPathTurns == 1 && getPathMovementRemaining() == 0)
		{
			if (pBestPlot->getRouteType() != NO_ROUTE)
			{
				eMission = MISSION_ROUTE_TO;
			}
		}
		else if (plot()->getRouteType() == NO_ROUTE)
		{
			int iPlotMoveCost = GC.getTerrainInfo(plot()->getTerrainType()).getMovementCost();

			if (plot()->getFeatureType() != NO_FEATURE)
			{
				iPlotMoveCost += GC.getFeatureInfo(plot()->getFeatureType()).getMovementCost();
			}
			if (plot()->isHills())
			{
				iPlotMoveCost += GC.getHILLS_EXTRA_MOVEMENT();
			}
			if (plot()->isAsPeak())
			{
				if (!GET_TEAM(getTeam()).isMoveFastPeaks())
				{
					iPlotMoveCost += GC.getPEAK_EXTRA_MOVEMENT();
				}
				iPlotMoveCost += 3;
			}
			if (iPlotMoveCost > 1)
			{
				eMission = MISSION_ROUTE_TO;
			}
		}
		// this _should_ eliminate forts within city limits...
		if (!plotIsInCity && eBestBuild == NO_BUILD)
		{
			// AI_betterplotbuild needs to be refactored...
			eBestBuild = AI_betterPlotBuild(pBestPlot, eBestBuild);
		}

		if (getGroup()->pushMissionInternal(eMission, pBestPlot->getX(), pBestPlot->getY(), (isHuman() ? 0 : MOVE_WITH_CAUTION), false, false, MISSIONAI_BUILD, pBestPlot))
		{
			getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, (getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);
			return true;
		}
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_nextCityToImprove(CvCity* pCity)
{
	AI_PERF_SCOPE(WORKER_RELOCATE);
	PROFILE_FUNC();

	int iBasePathFlags = MOVE_SAFE_TERRITORY | MOVE_AVOID_ENEMY_UNITS | (isHuman() ? MOVE_OUR_TERRITORY : MOVE_WITH_CAUTION | MOVE_RECONSIDER_ON_LEAVING_OWNED);

	int iBestPlotValue = 0;
	int iBestCityValue = 0;
	BuildTypes eBestBuild = NO_BUILD;
	const CvPlot* pBestPlot = NULL;
	const CvCity* pBestCity = NULL;
	const int iWorkerArea = getArea();


	{
		// Candidate evaluation is read-only; end the cache scope before issuing orders.
		PlotDangerCacheScope candidateDangerCache(getOwner());
	foreach_(const CvCity * pLoopCity, GET_PLAYER(getOwner()).cities())
	{
		if (pLoopCity != pCity)
		{
			//limit to cities in Area (otherwise, cause slowdown, in space maps in particular
			if (pLoopCity->getArea() == iWorkerArea)
			{

				int iWorkersNeeded = pLoopCity->AI_getWorkersNeeded();
				int iWorkersHave = pLoopCity->getNumWorkers();

				int iValue = 100 * std::max(0, iWorkersNeeded - iWorkersHave) + 10 * iWorkersNeeded;

				iValue *= (iWorkersNeeded + 1);
				iValue /= (iWorkersHave + 1);

				// Even a zero-turn trip cannot beat either incumbent below this
				// upper bound. Keep plot and city fallbacks separate in the test.
				iValue *= 1000;
				if (pLoopCity->isCapital())
				{
					iValue *= 2;
				}
				// A selected work plot takes precedence over the fallback city.
				// Do not search this city's work when even a zero-turn trip loses.
				// The fallback remains available if the final mission fails; skipping
				// an inferior city may change that emergency choice, not the best job.
				if (pBestPlot != NULL && iValue <= iBestPlotValue) continue;
				if (iValue > iBestPlotValue || iValue > iBestCityValue)
				{
					CvPlot* pPlot;
					BuildTypes eBuild;
					if (AI_bestCityBuild(pLoopCity, &pPlot, &eBuild, NULL, this))
					{
						FAssert(pPlot != NULL);
						FAssert(eBuild != NO_BUILD);

						if (AI_plotValid(pPlot))
						{
							if (iValue > iBestPlotValue)
							{
								PROFILE("CvUnitAI::AI_nextCityToImprove.Pathing");
								int iPathTurns;
								const int iMaxTurns = workerImprovementPathLimit(iValue, 1, iBestPlotValue);
								if (generatePath(pPlot, iBasePathFlags, true, &iPathTurns, iMaxTurns))
								{
									PROFILE("CvUnitAI::AI_nextCityToImprove.Pathed");
									iValue /= (iPathTurns + 1);

									if (iValue > iBestPlotValue)
									{
										iBestPlotValue = iValue;
										eBestBuild = eBuild;
										pBestPlot = pPlot;
										pBestCity = pLoopCity;
										//CvPlot* pEndTurnPlot = getPathEndTurnPlot();
										FAssert(!atPlot(pBestPlot) || NULL == pCity || pCity->AI_getWorkersNeeded() == 0 || pCity->getNumWorkers() > pCity->AI_getWorkersNeeded() + 1);
									}
								}
							}
						}
					}
					else if (isHuman())
					{ // Human automation may stage workers at a city without a current job.
						if (AI_plotValid(pLoopCity->plot()))
						{
							if (iValue > iBestCityValue)
							{
								PROFILE("CvUnitAI::AI_nextCityToImprove.Pathing");
								int iPathTurns;
								const int iMaxTurns = workerImprovementPathLimit(iValue, 1, iBestCityValue);
								if (generatePath(pLoopCity->plot(), iBasePathFlags, true, &iPathTurns, iMaxTurns))
								{
									PROFILE("CvUnitAI::AI_nextCityToImprove.Pathed");
									iValue /= (iPathTurns + 1);

									if (iValue > iBestCityValue)
									{
										iBestCityValue = iValue;
										//pBestPlot = pPlot;
										pBestCity = pLoopCity;
										//CvPlot* pEndTurnPlot = getPathEndTurnPlot();
										FAssert(!(NULL == pBestCity || pBestCity->AI_getWorkersNeeded() == 0 || pBestCity->getNumWorkers() > pBestCity->AI_getWorkersNeeded() + 1));
									}
								}
							}
						}
					}
				}
			}
		}
	}

	}

	if (pBestPlot != NULL && pBestCity != NULL)
	{
		FASSERT_BOUNDS(0, GC.getNumBuildInfos(), eBestBuild);

		eBestBuild = AI_betterPlotBuild(pBestPlot, eBestBuild);

		if (getGroup()->pushMissionInternal(MISSION_ROUTE_TO, pBestPlot->getX(), pBestPlot->getY(), ((isHuman() ? 0 : MOVE_WITH_CAUTION) | MOVE_SAFE_TERRITORY), false, false, MISSIONAI_BUILD, pBestPlot))
		{
			getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, (getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);

			CvWString StrUnitName;
			if (gUnitLogLevel > 2)
			{
				const CvWString StrunitAIType = GC.getUnitAIInfo(AI_getUnitAIType()).getType();
				const CvWString StrBuildAIType = GC.getBuildInfo(eBestBuild).getType();
				StrUnitName = m_szName;
				if (StrUnitName.length() == 0)
				{
					StrUnitName = getName(0).GetCString();
				}
				logAiEvaluations(3, "	Player %S Unit %S of type %S - Moving to Plot near %S to perform Build there %S", GET_PLAYER(getOwner()).getCivilizationDescription(0), StrUnitName.GetCString(), StrunitAIType.GetCString(), pBestCity->getName().GetCString(), StrBuildAIType.GetCString());
			}


			return true;
		}
		else
		{
			CvWString StrUnitName;
			if (gUnitLogLevel > 2)
			{
				const CvWString StrunitAIType = GC.getUnitAIInfo(AI_getUnitAIType()).getType();
				const CvWString StrBuildAIType = GC.getBuildInfo(eBestBuild).getType();
				StrUnitName = m_szName;
				if (StrUnitName.length() == 0)
				{
					StrUnitName = getName(0).GetCString();
				}
				logAiEvaluations(3, "	Player %S Unit %S of type %S - Want to Move to Plot near %S to perform Build there %S, but Pathing error", GET_PLAYER(getOwner()).getCivilizationDescription(0), StrUnitName.GetCString(), StrunitAIType.GetCString(), pBestCity->getName().GetCString(), StrBuildAIType.GetCString());
			}



		}
	}

	// AI relocation requires a concrete build, not just a WorkersNeeded score.
	// Arrival changes assignment counts and used to send idle workers straight back.
	if (isHuman() && pBestCity != NULL)
	{
		if (atPlot(pBestCity->plot())) //already in the city that need workers
		{

			CvWString StrunitAIType;
			CvWString StrUnitName;
			if (gUnitLogLevel > 2)
			{
				StrunitAIType = GC.getUnitAIInfo(AI_getUnitAIType()).getType();
				StrUnitName = m_szName;
				if (StrUnitName.length() == 0)
				{
					StrUnitName = getName(0).GetCString();
				}
				logAiEvaluations(3, "	Player %S Unit %S of type %S - Staying in %S to perform as Worker there when needed", GET_PLAYER(getOwner()).getCivilizationDescription(0), StrUnitName.GetCString(), StrunitAIType.GetCString(), pBestCity->getName().GetCString());
			}
			// Claim success only after actually ending the idle worker decision.
			return getGroup()->pushMissionInternal(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_REGROUP, pBestCity->plot());

		}


		if (generateSafePathforVulnerable(pBestCity->plot()))
		{
			CvWString StrunitAIType;
			CvWString StrUnitName;
			if (gUnitLogLevel > 2)
			{
				StrunitAIType = GC.getUnitAIInfo(AI_getUnitAIType()).getType();
				StrUnitName = m_szName;
				if (StrUnitName.length() == 0)
				{
					StrUnitName = getName(0).GetCString();
				}
				logAiEvaluations(3, "	Player %S Unit %S of type %S - Moving to %S to perform as Worker there", GET_PLAYER(getOwner()).getCivilizationDescription(0), StrUnitName.GetCString(), StrunitAIType.GetCString(), pBestCity->getName().GetCString());
			}
			const CvPlot* endTurnPlot = getPathEndTurnPlot();
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_REGROUP, pBestCity->plot());
		}
		else
		{
			int iPathTurns;
			//Calvitix. To reduce the amount of PropControl units moving, RNG 50% only will move
			const int iMotivValue = GC.getGame().getSorenRandNum(10, "Should Move unsafe to Dest");

			if (iMotivValue <= 5) //50%
			{

				if (generatePath(pBestCity->plot(), iBasePathFlags, true, &iPathTurns))
				{
					CvWString StrUnitName;
					if (gUnitLogLevel > 2)
					{
						const CvWString StrunitAIType = GC.getUnitAIInfo(AI_getUnitAIType()).getType();
						StrUnitName = m_szName;
						if (StrUnitName.length() == 0)
						{
							StrUnitName = getName(0).GetCString();
						}
						logAiEvaluations(3, "	Player %S Unit %S of type %S - Moving (unsafe) to %S to perform as Worker", GET_PLAYER(getOwner()).getCivilizationDescription(0), StrUnitName.GetCString(), StrunitAIType.GetCString(), pBestCity->getName().GetCString());
					}




					const CvPlot* endTurnPlot = getPathEndTurnPlot();
					return getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_REGROUP, pBestCity->plot());
				}
			}
			CvWString StrunitAIType;
			CvWString StrUnitName;
			if (gUnitLogLevel > 2)
			{
				StrunitAIType = GC.getUnitAIInfo(AI_getUnitAIType()).getType();
				StrUnitName = m_szName;
				if (StrUnitName.length() == 0)
				{
					StrUnitName = getName(0).GetCString();
				}
				logAiEvaluations(3, "	Player %S Unit %S of type %S - Want to Move to %S to perform as Worker there, but pathing error", GET_PLAYER(getOwner()).getCivilizationDescription(0), StrUnitName.GetCString(), StrunitAIType.GetCString(), pBestCity->getName().GetCString());
			}
		}
	}


	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_nextCityToImproveAirlift()
{
	AI_PERF_SCOPE(WORKER_RELOCATE);
	PROFILE_FUNC();

	if (getGroup()->getNumUnits() > 1)
	{
		return false;
	}
	const CvCity* pCity = plot()->getPlotCity();

	if (pCity == NULL || pCity->getMaxAirlift() == 0)
	{
		return false;
	}
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvCity * pLoopCity, GET_PLAYER(getOwner()).cities())
	{
		if (pLoopCity != pCity && canAirliftAt(pCity->plot(), pLoopCity->getX(), pLoopCity->getY()))
		{
			const int iValue = pLoopCity->AI_totalBestBuildValue(pLoopCity->area());

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestPlot = pLoopCity->plot();
			}
		}
	}
	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_AIRLIFT, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_irrigateTerritory()
{
	PROFILE_FUNC();

	const CvPlayerAI& kOwner = GET_PLAYER(getOwner());
	const bool bLeaveForests = kOwner.isOption(PLAYEROPTION_LEAVE_FORESTS);
	const bool bSafeAutomation = kOwner.isOption(PLAYEROPTION_SAFE_AUTOMATION);
	const ImprovementTypes eRuins = GC.getIMPROVEMENT_CITY_RUINS();

	BuildTypes eBestBuild = NO_BUILD;
	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (AI_plotValid(pLoopPlot) && UnitTravelConnectivity::landUnitCanReach(this,pLoopPlot) && pLoopPlot->getOwner() == getOwner() && pLoopPlot->getWorkingCity() == NULL)
		{
			const ImprovementTypes eImprovement = pLoopPlot->getImprovementType();

			if (eImprovement == NO_IMPROVEMENT
			|| (!bSafeAutomation || eImprovement == eRuins) && !GC.getImprovementInfo(eImprovement).isCarriesIrrigation())
			{
				const BonusTypes eNonObsoleteBonus = pLoopPlot->getNonObsoleteBonusType(getTeam());

				if ((eImprovement == NO_IMPROVEMENT || eNonObsoleteBonus == NO_BONUS || !GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eNonObsoleteBonus))
				&& pLoopPlot->isIrrigationAvailable(true))
				{
					int iBestTempBuildValue = 0;
					BuildTypes eBestTempBuild = NO_BUILD;

					for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
					{
						const BuildTypes eBuild = (BuildTypes)iJ;

						if (GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT
						&& GC.getImprovementInfo(GC.getBuildInfo(eBuild).getImprovement()).isCarriesIrrigation()
						&& canBuild(pLoopPlot, eBuild))
						{
							const int iValue = 10000 / (GC.getBuildInfo(eBuild).getTime() + 1);

							if (iValue > iBestTempBuildValue)
							{
								iBestTempBuildValue = iValue;
								eBestTempBuild = eBuild;
							}
						}
					}

					if (eBestTempBuild != NO_BUILD)
					{
						bool bValid = true;

						if (bLeaveForests && pLoopPlot->getFeatureType() != NO_FEATURE
						&& GC.getBuildInfo(eBestTempBuild).isFeatureRemove(pLoopPlot->getFeatureType())
						&& GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange(YIELD_PRODUCTION) > 0)
						{
							bValid = false;
						}

						if (bValid && !pLoopPlot->isVisibleEnemyUnit(this)
						&& kOwner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD, getGroup(), 1) == 0)
						{
							int iPathTurns;
							// XXX should this actually be at the top of the loop? (with saved paths and all...)
							if (generatePath(pLoopPlot, isHuman() ? 0 : MOVE_WITH_CAUTION, true, &iPathTurns))
							{
								const int iValue = 10000 / (iPathTurns + 1);

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									eBestBuild = eBestTempBuild;
									pBestPlot = pLoopPlot;
								}
							}
						}
					}
				}
			}
		}
	}
	if (pBestPlot != NULL)
	{
		FASSERT_BOUNDS(0, GC.getNumBuildInfos(), eBestBuild);

		if (getGroup()->pushMissionInternal(MISSION_ROUTE_TO, pBestPlot->getX(), pBestPlot->getY(), (isHuman() ? 0 : MOVE_WITH_CAUTION), false, false, MISSIONAI_BUILD, pBestPlot))
		{
			getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, (getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);
			return true;
		}
	}
	return false;
}


bool CvUnitAI::AI_fortTerritory(bool bCanal, bool bAirbase)
{
	PROFILE_FUNC();

	//if (AI_StrategicForts())
	//{
	//	return true;
	//}
	BuildTypes eBestBuild = NO_BUILD;
	const CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	int iMaxDistFromBorder = -1;

	if (!isHuman() || (getGroup()->getNumUnits() > 1 && getGroup()->canDefend()))
	{
		iMaxDistFromBorder = GC.getAI_WORKER_MAX_DISTANCE_FROM_CITY_OUT_BORDERS();
	}

	const PlayerTypes ePlayer = getOwner();
	const CvPlayerAI& kOwner = GET_PLAYER(ePlayer);
	const bool bLeaveForests = kOwner.isOption(PLAYEROPTION_LEAVE_FORESTS);
	const int iBasePathFlags = MOVE_SAFE_TERRITORY | MOVE_AVOID_ENEMY_UNITS | (isHuman() ? MOVE_OUR_TERRITORY : MOVE_WITH_CAUTION | MOVE_RECONSIDER_ON_LEAVING_OWNED);

	CvReachablePlotSet plotSet(getGroup(), isHuman() ? 0 : MOVE_WITH_CAUTION, MAX_INT);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		const CvPlot* pLoopPlot = itr.plot();

		if (/*AI_plotValid(pLoopPlot) &&*/ UnitTravelConnectivity::landUnitCanReach(this,pLoopPlot)
		&& (pLoopPlot->getOwner() == ePlayer || pLoopPlot->getOwner() == NO_PLAYER && pLoopPlot->isRevealed(getTeam(), false))
		&& (CvAdditionalImprovements::enabled() || pLoopPlot->getImprovementType() == NO_IMPROVEMENT || !pLoopPlot->isCityRadius()))
		{
			int iValue = bCanal ? kOwner.AI_getPlotCanalValue(pLoopPlot) : 0;
			iValue += bAirbase ? kOwner.AI_getPlotAirbaseValue(pLoopPlot) : 0;

			iValue += kOwner.AI_getPlotChokeValue(pLoopPlot); // Super Forts

			int iMinAcceptableValue = 0;
			if (pLoopPlot->getOwner() == NO_PLAYER)
			{	// Don't go outside borders for low values
				iMinAcceptableValue += 150;
			}

			if (iValue > iMinAcceptableValue)
			{
				int iBestBuildValue = 0;
				BuildTypes eBestTempBuild = NO_BUILD;

				for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
				{
					const BuildTypes eBuild = ((BuildTypes)iJ);

					if (GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT)
					{
						const CvImprovementInfo& kImprovement = GC.getImprovementInfo(GC.getBuildInfo(eBuild).getImprovement());

						if ((bCanal && kImprovement.isCanMoveSeaUnits() || kImprovement.isActsAsCity() && kImprovement.getDefenseModifier() > 0)
						&& canBuild(pLoopPlot, eBuild))
						{
							int iBuildValue = 100 * kImprovement.getDefenseModifier();

							if (kImprovement.isCanMoveSeaUnits() && bCanal)
							{
								iBuildValue += 2000;
							}
							if (kImprovement.isZOCSource())
							{
								iBuildValue += 5000;
							}
							iBuildValue /= (GC.getBuildInfo(eBuild).getTime() + 1);

							if (iBuildValue > iBestBuildValue)
							{
								iBestBuildValue = iBuildValue;
								eBestTempBuild = eBuild;
							}
						}
					}
				}

				if (eBestTempBuild != NO_BUILD && !pLoopPlot->isVisibleEnemyUnit(this))
				{
					bool bValid = true;

					if (bLeaveForests && pLoopPlot->getFeatureType() != NO_FEATURE
					&& GC.getBuildInfo(eBestTempBuild).isFeatureRemove(pLoopPlot->getFeatureType())
					&& GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange(YIELD_PRODUCTION) > 0)
					{
						bValid = false;
					}
					if (bValid && kOwner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD, getGroup(), 1/*3*/) == 0)
					{
						int iPathTurns;
						if (generatePath(pLoopPlot, iBasePathFlags, true, &iPathTurns))
						{
							if (pLoopPlot->getOwner() == NO_PLAYER)
							{
								const CvCity* pNearestCity = GC.getMap().findCity(pLoopPlot->getX(), pLoopPlot->getY(), ePlayer, NO_TEAM, false);
								if (pNearestCity == NULL || plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pNearestCity->getX(), pNearestCity->getY()) > iMaxDistFromBorder || iPathTurns > iMaxDistFromBorder / 2)
								{
									continue;
								}
							}
							iValue += bCanal ? 0 : kOwner.AI_getPlotCanalValue(pLoopPlot) / 4;
							iValue += bAirbase ? 0 : kOwner.AI_getPlotAirbaseValue(pLoopPlot) / 4;
							iValue *= 100;
							iValue /= (iPathTurns + 1);

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								eBestBuild = eBestTempBuild;
								pBestPlot = pLoopPlot;
							}
						}
					}
				}
			}
		}
	}
	if (pBestPlot != NULL)
	{
		FASSERT_BOUNDS(0, GC.getNumBuildInfos(), eBestBuild);

		if (getGroup()->pushMissionInternal(MISSION_ROUTE_TO, pBestPlot->getX(), pBestPlot->getY(), (isHuman() ? 0 : MOVE_WITH_CAUTION), false, false, MISSIONAI_BUILD, pBestPlot))
		{
			getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, (getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);
			return true;
		}
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_improveBonus(int iMinValue, CvPlot** ppBestPlot, BuildTypes* peBestBuild, int* piBestValue, const std::vector<int>* bonusCandidates)
{
	AI_PERF_SCOPE(WORKER_BONUS);
	PROFILE_FUNC();

	int iMaxDistFromBorder = -1;

	if (getGroup()->getNumUnits() > 1 && getGroup()->canDefend())
	{
		iMaxDistFromBorder = GC.getAI_WORKER_MAX_DISTANCE_FROM_CITY_OUT_BORDERS();
	}
	const int iBasePathFlags = MOVE_SAFE_TERRITORY | MOVE_AVOID_ENEMY_UNITS | (isHuman() ? MOVE_OUR_TERRITORY : MOVE_WITH_CAUTION | MOVE_RECONSIDER_ON_LEAVING_OWNED);
	const ImprovementTypes eRuins = GC.getIMPROVEMENT_CITY_RUINS();
	const PlayerTypes ePlayer = getOwner();
	const CvPlayerAI& kOwner = GET_PLAYER(ePlayer);
	const bool bLeaveForests = kOwner.isOption(PLAYEROPTION_LEAVE_FORESTS);
	const bool bSafeAutomation = kOwner.isOption(PLAYEROPTION_SAFE_AUTOMATION);
	const bool bCanRoute = canBuildRoute();

	int iPathTurns = 0;
	int iBestValue = 0;
	int iBestResourceValue = 0;
	bool bBestBuildIsRoute = false;
	BuildTypes eBestBuild = NO_BUILD;
	CvPlot* pBestPlot = NULL;

	CvReachablePlotSet plotSet(getGroup(), iBasePathFlags, -1, true, iMaxDistFromBorder == -1 ? -1 : iMaxDistFromBorder / 2 + 1);
	bool bPlotSetPopulated = false;

	const int candidateCount = bonusCandidates != NULL ? (int)bonusCandidates->size() : GC.getMap().numPlots();
	for (int iI = 0; iI < candidateCount; iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(bonusCandidates != NULL ? (*bonusCandidates)[iI] : iI);

		if (bPlotSetPopulated && plotSet.find(pLoopPlot) == plotSet.end())
		{
			continue;
		}
		PROFILE("CvUnitAI::AI_improveBonus.ConsiderPlot");
		const PlayerTypes ePlotOwner = pLoopPlot->getOwner();

		if ((ePlotOwner == ePlayer || ePlotOwner == NO_PLAYER && pLoopPlot->isRevealed(getTeam(), false)) && AI_plotValid(pLoopPlot)
		&& (UnitTravelConnectivity::landUnitCanReach(this,pLoopPlot) || DOMAIN_SEA == getDomainType() && pLoopPlot->isWater() && plot()->isAdjacentToArea(pLoopPlot->area())))
		{
			BonusTypes eNonObsoleteBonus = pLoopPlot->getNonObsoleteBonusType(getTeam());

			if (eNonObsoleteBonus != NO_BONUS)
			{
				const bool bIsConnected = pLoopPlot->isConnectedToCapital(ePlayer);
				// Super Forts - This section makes sure the plot is "close enough" to a city to be worth building on
				bool bCloseEnough = false;

				if (ePlotOwner == ePlayer)
				{
					bCloseEnough = true;
				}
				// Automated human workers will not look outside borders to build forts on bonuses the player already has
				else if (!isHuman() || !kOwner.hasBonus(eNonObsoleteBonus))
				{
					PROFILE("CvUnitAI::AI_improveBonus.CheckConnectivity");

					if (!bPlotSetPopulated)
					{
						plotSet.Populate(MAX_INT);
						bPlotSetPopulated = true;

						if (plotSet.find(pLoopPlot) == plotSet.end())
						{
							continue;
						}
					}

					const CvCity* pNearestCity = GC.getMap().findCity(pLoopPlot->getX(), pLoopPlot->getY(), ePlayer, NO_TEAM, false);

					if (pNearestCity != NULL && generatePath(pLoopPlot, 0, true, &iPathTurns, iMaxDistFromBorder / 2)

					&& plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pNearestCity->getX(), pNearestCity->getY()) * (kOwner.hasBonus(eNonObsoleteBonus) ? 2 : 1) <= iMaxDistFromBorder)
					{
						bCloseEnough = true;
					}
				}

				if ((pLoopPlot->getWorkingCity() || bIsConnected || bCanRoute) && bCloseEnough && !pLoopPlot->isVisibleEnemyUnit(this))
				{
					PROFILE("CvUnitAI::AI_improveBonus.CheckImprovement");

					const bool bCityRadius = pLoopPlot->isCityRadius();
					const ImprovementTypes eImprovement = pLoopPlot->getImprovementType();

					bool bDoImprove = false;

					if (eImprovement == NO_IMPROVEMENT)
					{
						bDoImprove = true;
					}
					/*
					else if (!bCityRadius && (GC.getImprovementInfo(eImprovement).isActsAsCity() || GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eNonObsoleteBonus)))
					{
						//TB Super Forts debug: I wanted to allow a civ to re-evaluate whether a fort should be or continue to be used on a given plot as the best way to gather this resource.
						//The way this is programmed they won't remove forts when cities move in and they won't remove forts when war is over and they won't replace a plantation or whatever with a fort if war is afoot.
						bDoImprove = true;
					}
					*/
					else if (bCityRadius && (GC.getImprovementInfo(eImprovement).isActsAsCity() || GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eNonObsoleteBonus)))
					{
						//TB Super Forts debug: I wanted to allow a civ to re-evaluate whether a fort should be or continue to be used on a given plot as the best way to gather this resource.
						//The way this is programmed they won't remove forts when cities move in and they won't remove forts when war is over and they won't replace a plantation or whatever with a fort if war is afoot.
						bDoImprove = true;
					}

					if (!bDoImprove)
					{
						bDoImprove = eImprovement == eRuins;
					}
					else if (bSafeAutomation && eImprovement != NO_IMPROVEMENT && eImprovement != eRuins)
					{
						bDoImprove = false;
					}

					const FeatureTypes eFeature = pLoopPlot->getFeatureType();
					BuildTypes eBestTempBuild = NO_BUILD;

					if (bDoImprove)
					{
						PROFILE("CvUnitAI::AI_improveBonus.CheckBuild");

						int iBestTempBuildValue = MAX_INT;

						for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
						{
							const BuildTypes eBuild = (BuildTypes)iJ;

							if (GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT)
							{
								const CvImprovementInfo& kImprovementX = GC.getImprovementInfo(GC.getBuildInfo(eBuild).getImprovement());

								if ((!bLeaveForests || eFeature == NO_FEATURE || !GC.getBuildInfo(eBuild).isFeatureRemove(eFeature))

								&& (ePlotOwner == ePlayer || kImprovementX.getCulture() > 0 && kImprovementX.isOutsideBorders())

								&& (kImprovementX.isImprovementBonusTrade(eNonObsoleteBonus) || !bCityRadius && kImprovementX.getCulture() > 0)

								&& canBuild(pLoopPlot, eBuild))
								{
									const int iValue = 10000 / (GC.getBuildInfo(eBuild).getTime() + 1);

									if (iValue < iBestTempBuildValue)
									{
										iBestTempBuildValue = iValue;
										eBestTempBuild = eBuild;
									}
								}
							}
						}
					}
					if (eBestTempBuild == NO_BUILD)
					{
						bDoImprove = false;
					}
					// Super Forts begin *AI_worker* (if not building an improvement and you don't own the plot then continue so the AI doesn't consider building a route)
					if (!bDoImprove && ePlotOwner != ePlayer)
					{
						continue;
					}
					// Super Forts end

					if (eBestTempBuild != NO_BUILD || bCanRoute && !bIsConnected)
					{
						if (!bPlotSetPopulated)
						{
							plotSet.Populate(MAX_INT);

							bPlotSetPopulated = true;
							if (plotSet.find(pLoopPlot) == plotSet.end())
							{
								continue;
							}
						}

						PROFILE("CvUnitAI::AI_improveBonus.Evaluate");

						// Estimate an upper bound before pathfinding. Omit positive countervalue;
						// a lazily initialized countervalue is left to the existing reachable path.
						int maxWinningTurns=MAX_INT;
						if(iBestValue>0)
						{
							bool canBound=true;
							__int64 optimisticValue=kOwner.AI_bonusVal(eNonObsoleteBonus);
							if(bDoImprove)
							{
								const CvImprovementInfo& candidate=GC.getImprovementInfo(GC.getBuildInfo(eBestTempBuild).getImprovement());
								if(bCityRadius)for(int y=0;y<NUM_YIELD_TYPES;++y)
								{
									optimisticValue+=20*pLoopPlot->calculateImprovementYieldChange(GC.getBuildInfo(eBestTempBuild).getImprovement(),(YieldTypes)y,ePlayer,false);
									optimisticValue+=10*pLoopPlot->calculateNatureYield((YieldTypes)y,getTeam(),eFeature==NO_FEATURE ? true : GC.getBuildInfo(eBestTempBuild).isFeatureRemove(eFeature));
								}
								if(candidate.isActsAsCity())
								{
									int defense=candidate.getAirBombDefense()/10+candidate.getDefenseModifier()/10+(candidate.isZOCSource()?3:0);
									if(!bCityRadius)optimisticValue+=defense+candidate.getCulture();
									else if(pLoopPlot->getWorkingCity()!=NULL)optimisticValue+=defense*pLoopPlot->getWorkingCity()->AI_cityThreat()/100;
								}
								if(eImprovement!=NO_IMPROVEMENT)
								{
									const int counter=pLoopPlot->getImprovementCurrentValue();
									if(counter==0)canBound=false; // Preserve the original lazy initialization.
									else if(counter<0)optimisticValue-=counter;
								}
								optimisticValue+=std::max(0,100*GC.getBonusInfo(eNonObsoleteBonus).getAIObjective());
								if(kOwner.getNumTradeableBonuses(eNonObsoleteBonus)==0)optimisticValue*=2;
							}
							if(canBound)
							{
								const int afterDivision=bDoImprove && bCityRadius ? 2 : 1;
								const int onPlot=bDoImprove && atPlot(pLoopPlot) ? 3 : 1;
								maxWinningTurns=workerImprovementPathLimit(optimisticValue*1000*onPlot,afterDivision,iBestValue);
								if(maxWinningTurns<0){AI_PERF_COUNT(TARGET_PATHS_PRUNED,1);continue;}
							}
						}

						//	Use the same caution here as the mission that will actually be pushed for this plot,
						//	so a plot judged reachable now doesn't get its move aborted mid-route by a stricter
						//	danger check next turn (that mismatch caused automated workers to bounce back and
						//	forth between re-evaluations instead of settling on a build).
						if (generatePath(pLoopPlot, iBasePathFlags, true, &iPathTurns,maxWinningTurns))
						{
							int iValue = kOwner.AI_bonusVal(eNonObsoleteBonus);

							if (bDoImprove)
							{
								const ImprovementTypes eImprovementX = GC.getBuildInfo(eBestTempBuild).getImprovement();
								FAssert(eImprovementX != NO_IMPROVEMENT);

								if (bCityRadius)
								{
									for (int iK = 0; iK < NUM_YIELD_TYPES; iK++)
									{
										iValue += 20 * pLoopPlot->calculateImprovementYieldChange(eImprovementX, (YieldTypes)iK, ePlayer, false);
										iValue += 10 * pLoopPlot->calculateNatureYield((YieldTypes)iK, getTeam(), eFeature == NO_FEATURE ? true : GC.getBuildInfo(eBestTempBuild).isFeatureRemove(eFeature));
									}
								}
								//TB Super Forts AI fix: here we are evaluating the improvement but not previously giving any concession that a *fort* is a horrible thing to build within a city radius unless there's good cause.
								if (GC.getImprovementInfo(eImprovementX).isActsAsCity())
								{
									int iDefenseValue = GC.getImprovementInfo(eImprovementX).getAirBombDefense() / 10;
									iDefenseValue += GC.getImprovementInfo(eImprovementX).getDefenseModifier() / 10;
									iDefenseValue += (GC.getImprovementInfo(eImprovementX).isZOCSource() ? 3 : 0);
									if (!bCityRadius)
									{
										iValue += iDefenseValue;
										iValue += GC.getImprovementInfo(eImprovementX).getCulture();
									}
									else if (pLoopPlot->getWorkingCity() != NULL)
									{
										//if the city is under threat then the priority to replace with forts grows
										int iThreat = iDefenseValue;
										iThreat *= pLoopPlot->getWorkingCity()->AI_cityThreat();
										iThreat /= 100;
										iValue += iThreat;
									}
								}

								if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
								{
									/*
									int iCountervalue = kOwner.AI_bonusVal(eNonObsoleteBonus);
									if (bCityRadius)
									{
										for (int iK = 0; iK < NUM_YIELD_TYPES; iK++)
										{
											iCountervalue += 20 * pLoopPlot->calculateImprovementYieldChange(eImprovement, (YieldTypes)iK, ePlayer, false);
											iCountervalue += 10 * pLoopPlot->calculateNatureYield((YieldTypes)iK, getTeam(), eFeature == NO_FEATURE ? true : GC.getBuildInfo(eBestTempBuild).isFeatureRemove(eFeature));
										}
									}
									if (GC.getImprovementInfo(eImprovement).isMilitaryStructure())
									{
										int iCounterDefenseValue = GC.getImprovementInfo(eImprovement).getAirBombDefense()/10;
										iCounterDefenseValue += GC.getImprovementInfo(eImprovement).getDefenseModifier()/10;
										iCounterDefenseValue += (GC.getImprovementInfo(eImprovement).isZOCSource() ? 3 : 0);
										if (!bCityRadius)
										{
											iCountervalue += iCounterDefenseValue;
											iCountervalue += GC.getImprovementInfo(eImprovement).getCulture();
										}
										else if (pLoopPlot->getWorkingCity() != NULL)
										{
											//if the city is under threat then the priority to replace with forts grows
											int iCounterThreat = iCounterDefenseValue;
											iCounterThreat *= pLoopPlot->getWorkingCity()->AI_cityThreat();
											iCounterThreat /= 100;
											iCountervalue += iCounterThreat;
										}
									}
									*/
									//TB Note:establishes countervalue in a much processing cheaper fashion.
									//Value gets set once when improvement is built and that's that.
									//Defensive values based on threats when built will linger but that's a little interesting and could help to keep those odd forts from being so quickly rebuilt over.
									int iCountervalue = pLoopPlot->getImprovementCurrentValue();
									if (iCountervalue == 0)
									{
										//Compatibility check - if the game hasn't assigned a current value to the plot yet, assign one then recheck.
										pLoopPlot->setImprovementCurrentValue();
										iCountervalue = pLoopPlot->getImprovementCurrentValue();
									}

									iValue -= iCountervalue;
								}
								if (iValue <= 0)
								{
									bDoImprove = false;
								}
							}
							if (bDoImprove)
							{

								iValue += std::max(0, 100 * GC.getBonusInfo(eNonObsoleteBonus).getAIObjective());

								if (kOwner.getNumTradeableBonuses(eNonObsoleteBonus) == 0)
								{
									iValue *= 2;
								}
							}

							int iMaxWorkers = 1;

							if (eBestTempBuild != NO_BUILD && GC.getBuildInfo(eBestTempBuild).getTime() > 0)
							{
								//allow teaming.
								iMaxWorkers = AI_calculatePlotWorkersNeeded(pLoopPlot, eBestTempBuild);
								if (getPathMovementRemaining() == 0)
								{
									iMaxWorkers = std::min((iMaxWorkers + 1) / 2, 1 + kOwner.AI_baseBonusVal(eNonObsoleteBonus) / 20);
								}
							}

							if (kOwner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD, getGroup()) < iMaxWorkers
							&& (!bDoImprove || pLoopPlot->getBuildTurnsLeft(eBestTempBuild, 0, 0) > 2 * iPathTurns - 1))
							{
								if (bDoImprove)
								{
									iValue *= 1000;

									if (atPlot(pLoopPlot))
									{
										iValue *= 3;
									}

									iValue /= (iPathTurns + 1);

									if (bCityRadius)
									{
										iValue *= 2;
									}

									if (iValue > iBestValue)
									{
										iBestValue = iValue;
										eBestBuild = eBestTempBuild;
										pBestPlot = pLoopPlot;
										bBestBuildIsRoute = false;
										iBestResourceValue = iValue;
									}
								}
								else if (bCanRoute && !bIsConnected)
								{
									FAssert(bCanRoute && !bIsConnected);

									if (eImprovement != NO_IMPROVEMENT && GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eNonObsoleteBonus))
									{
										iValue *= 1000;
										iValue /= (iPathTurns + 1);

										if (iValue > iBestValue)
										{
											iBestValue = iValue;
											eBestBuild = NO_BUILD;
											pBestPlot = pLoopPlot;
											bBestBuildIsRoute = true;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if (iBestValue < iMinValue && NULL != ppBestPlot)
	{
		FAssert(NULL != peBestBuild);
		FAssert(NULL != piBestValue);

		*ppBestPlot = pBestPlot;
		*peBestBuild = eBestBuild;
		*piBestValue = iBestResourceValue;
	}
	if (pBestPlot != NULL)
	{
		if (eBestBuild != NO_BUILD)
		{
			FAssertMsg(!bBestBuildIsRoute, "BestBuild should not be a route");
			FAssertMsg(eBestBuild < GC.getNumBuildInfos(), "BestBuild is assigned a corrupt value");

			MissionTypes eBestMission = MISSION_MOVE_TO;

			if ((pBestPlot->getWorkingCity() == NULL) || !pBestPlot->getWorkingCity()->isConnectedToCapital())
			{
				eBestMission = MISSION_ROUTE_TO;
			}
			else
			{
				int iPathTurns;
				if (generatePath(pBestPlot, iBasePathFlags, false, &iPathTurns)
				&& iPathTurns >= stepDistance(getX(), getY(), pBestPlot->getX(), pBestPlot->getY()))
				{
					eBestMission = MISSION_ROUTE_TO;
				}
			}
			eBestBuild = AI_betterPlotBuild(pBestPlot, eBestBuild);

			// Moving to the tile we already occupy can complete without accepting
			// a move mission. Start the selected work instead of abandoning it.
			// A workboat may be consumed by this build: do not access it afterwards.
			if (atPlot(pBestPlot))
			{
				getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, false, false, MISSIONAI_BUILD, pBestPlot);
				return true;
			}

			if (getGroup()->pushMissionInternal(eBestMission, pBestPlot->getX(), pBestPlot->getY(), (isHuman() ? 0 : MOVE_WITH_CAUTION), false, false, MISSIONAI_BUILD, pBestPlot))
			{
				getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, (getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);
				return true;
			}
		}
		else if (bBestBuildIsRoute)
		{
			if (AI_connectPlot(pBestPlot))
			{
				return true;
			}
		}
		else
		{
			FErrorMsg("error");
		}
	}
	return false;
}


bool CvUnitAI::AI_isAwaitingContract() const
{
	return (m_contractsLastEstablishedTurn == GC.getGame().getGameTurn() &&
			(m_contractualState == CONTRACTUAL_STATE_AWAITING_ANSWER || m_contractualState == CONTRACTUAL_STATE_AWAITING_WORK)
			&& !CvContractBroker::isTransportCommitted(this));
}

bool CvUnitAI::processContracts(int iMinPriority)
{
	AI_PERF_SCOPE(CONTRACT_PROCESS);
	PROFILE_FUNC();

	//	Currently not implemented for domains other than land
	// The transport controls landing; contracts must not unload passengers
	// or overwrite a still valid embarkation order.
	if (getDomainType() != DOMAIN_LAND || CvContractBroker::isTransportCommitted(this))
	{
		return false;
	}

	bool bContractAlreadyEstablished = (m_contractsLastEstablishedTurn == GC.getGame().getGameTurn());

	// Have we advertised ourselves as available yet?
	if (!bContractAlreadyEstablished)
	{
		GET_PLAYER(getOwner()).getContractBroker().lookingForWork(this, iMinPriority);

		m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
		m_contractualState = CONTRACTUAL_STATE_AWAITING_WORK;
		GET_PLAYER(getOwner()).getContractBroker().noteDefenderAvailability(this);
		if (gUnitLogLevel >= 3)
		{
			const CvWString szStringUnitAi = GC.getUnitAIInfo(m_eUnitAIType).getType();
			logContractBroker(1, "	Unit %S (%d) of Type %S for player %d (%S) at (%d,%d) advertising for work",
					getUnitInfo().getDescription(),
					getID(),
					szStringUnitAi.GetCString(),
					getOwner(),
					GET_PLAYER(getOwner()).getCivilizationDescription(0),
					getX(),
					getY());
		}
	}
	int iAtX = INVALID_PLOT_COORD;
	int iAtY = INVALID_PLOT_COORD;
	CvUnit* pJoinUnit = NULL;

    bool defenseDemand=false;int defenseMaxTurns=MAX_INT;
    if (GET_PLAYER(getOwner()).getContractBroker().makeContract(this, iAtX, iAtY, pJoinUnit, !bContractAlreadyEstablished,&defenseDemand,&defenseMaxTurns))
	{
        if(defenseDemand)return AI_fulfillDefenseDemand(GC.getMap().plot(iAtX,iAtY),defenseMaxTurns);
        // Work found
		if (gUnitLogLevel >= 3)
		{
			CvString JoinInfos = "";
			CvString MissionInfos = "";
			if (pJoinUnit)
			{
				const CvSelectionGroup* pGroup = pJoinUnit->getGroup();
				JoinInfos = CvString::format("[to join ] %d",pJoinUnit->getID());
				if (pGroup)
				{
					MissionInfos = MissionAITypeToString(pGroup->AI_getMissionAIType());
				}
				//GC.getUnitAIInfo(AI_getUnitAIType())->getType();

			}
			logContractBroker(1, "	Unit %S (%d) for player %d (%S) at (%d,%d) found work (%S) at (%d,%d) %S\n",
					getUnitInfo().getDescription(),
					getID(),
					getOwner(),
					GET_PLAYER(getOwner()).getCivilizationDescription(0),
					getX(),
					getY(),
					MissionInfos.GetCString(),
					(pJoinUnit == NULL ? iAtX : pJoinUnit->getX()),
					(pJoinUnit == NULL ? iAtY : pJoinUnit->getY()),
					JoinInfos.GetCString());
		}

		//	In rare circumstances it is possible that the unit that advertised for us may have itself actively
		//	joined us, in which case we're done with this contract
		if (pJoinUnit != NULL && pJoinUnit->getGroup() == getGroup())
		{
			m_contractualState = CONTRACTUAL_STATE_FOUND_WORK;

			//	Remove ourselves from advertising for work and set our status back to no contract
			contractFulfilled();

			return true;
		}

		//	Must ungroup ourselves since it's just this unit that is answering the work
		//	request (unless it's a no-defend unit in which case it will go with any escorts it has)
		if (getGroup()->getNumUnits() > 1 && canDefend())
		{
			//	Also check we are not the ONLY unit that can defend - if we are we'll drag the rest along
			bool bHasOtherDefender = false;

			foreach_(const CvUnit * pLoopUnit, getGroup()->units())
			{
				if (pLoopUnit != this && pLoopUnit->canDefend())
				{
					bHasOtherDefender = true;
					break;
				}
			}

			if (bHasOtherDefender)
			{
				joinGroup(NULL);
			}
		}
		//	Try to enact the contracted work
		CvPlot* pTargetPlot;

		if (pJoinUnit != NULL)
		{
			pTargetPlot = pJoinUnit->plot();
		}
		else
		{
			pTargetPlot = GC.getMap().plot(iAtX, iAtY);
		}

		if (atPlot(pTargetPlot))
		{
			m_contractualState = CONTRACTUAL_STATE_FOUND_WORK;

			if (pJoinUnit != NULL && atPlot(pJoinUnit->plot()))
			{
				if (gUnitLogLevel >= 3)
				{
					logContractBroker(1, "	...already at target plot - merging into requesting unit's group.");
				}
				getGroup()->setTransportUnit(NULL);
				pJoinUnit->getGroup()->setTransportUnit(NULL);
				getGroup()->mergeIntoGroup(pJoinUnit->getGroup());

				//	Remove ourselves from advertising for work and set our status back to no contract
				contractFulfilled();
			}
			else
			{
				if (gUnitLogLevel >= 3)
				{
					logContractBroker(1, "	...already at target plot.");
				}
				contractFulfilled();
				getGroup()->pushMission(MISSION_SKIP);
			}
		}
		else
		{
			if (pJoinUnit != NULL)
			{
				if (getGroup()->pushMissionInternal(MISSION_MOVE_TO_UNIT, pJoinUnit->getOwner(), pJoinUnit->getID(), MOVE_SAFE_TERRITORY | MOVE_AVOID_ENEMY_UNITS, false, false, MISSIONAI_CONTRACT_UNIT, NULL, pJoinUnit))
				{
					if (gUnitLogLevel >= 3)
					{
						logContractBroker(1, "	...first try, move to unit mission !");
					}
					contractFulfilled();
					return true;
				}
			}
			if (gUnitLogLevel >= 3)
			{
				logContractBroker(1, "	...second try, move to plot mission !");
			}
			if (!getGroup()->pushMissionInternal(MISSION_MOVE_TO, pTargetPlot->getX(), pTargetPlot->getY(), MOVE_SAFE_TERRITORY | MOVE_AVOID_ENEMY_UNITS, false, false, (pJoinUnit == NULL ? MISSIONAI_CONTRACT : MISSIONAI_CONTRACT_UNIT), pTargetPlot))
			{
				if (gUnitLogLevel >= 3)
				{
					logContractBroker(1, "	...safe path don't work, try unsafe one !");
				}
				if (!getGroup()->pushMissionInternal(MISSION_MOVE_TO, pTargetPlot->getX(), pTargetPlot->getY(), MOVE_IGNORE_DANGER , false, false, (pJoinUnit == NULL ? MISSIONAI_CONTRACT : MISSIONAI_CONTRACT_UNIT), pTargetPlot))
				{
					if (gUnitLogLevel >= 3)
					{
						logContractBroker(1, "	...unexpectedly unable to enact the work!");
					}
					//	Handle as if we found no work this turn since we cannot safely path to it
					m_contractualState = CONTRACTUAL_STATE_NO_WORK_FOUND;
					GET_PLAYER(getOwner()).getContractBroker().removeUnit(this);
					return false;
				}
			}
			//	We only reset the contract establishment if we actively move to
			//	fulfill it.  This is so that:
			//	1)	In the move case, if the move doesn't use up all our movement allowance we can
			//		potentially recontract
			//	2)	In the non-move case we retain the knowledge that we've already looked and not found
			//		anything in particular to do - this prevents other units grouping with us and setting us back
			//		awake
			contractFulfilled();
		}
		return true;
	}

	if (bContractAlreadyEstablished)
	{
		m_contractualState = CONTRACTUAL_STATE_NO_WORK_FOUND;

		//	No work available
		LOG_UNIT_BLOCK(4, {
			const CvWString szStringUnitAi = GC.getUnitAIInfo(m_eUnitAIType).getType();
			logBBAI("	Unit %S (%d) of Type (%S) for player %d (%S) at (%d,%d) - no work available",
					getUnitInfo().getDescription(),
					getID(),
					szStringUnitAi.GetCString(),
					getOwner(),
					GET_PLAYER(getOwner()).getCivilizationDescription(0),
					getX(),
					getY());
		});
		return false;
	}
	return true;
}

void CvUnitAI::contractFulfilled()
{
	m_contractsLastEstablishedTurn = -1;	//	Contract fulfilled
	m_contractualState = CONTRACTUAL_STATE_NONE;

	GET_PLAYER(getOwner()).getContractBroker().removeUnit(this);
}

bool CvUnitAI::AI_workerNeedsDefender(const CvPlot* pPlot) const
{
	PROFILE_FUNC();

	if (isNPC())
	{
		return false;
	}

	//	Check danger level both where we are now and where we are headed
	const int iDanger = std::max(pPlot->getDangerCount(getOwner()), plot()->getDangerCount(getOwner()));

	//	Need to adjust this threshold based on experience with AI testing - 25 is an initial good guess
	if (iDanger > 20)
	{
		// Afforess - don't just return true, check for actual danger first!
		// return true;
		return GET_PLAYER(getOwner()).AI_isPlotThreatened(plot(), 2);
	}

	//	Also for non-owned territory check for nearby enemies
	if (pPlot->getOwner() != getOwner() && GET_PLAYER(getOwner()).AI_getVisiblePlotDanger(pPlot, 2, false))
	{
		return true;
	}

	return false;
}

bool CvUnitAI::AI_workerReleaseDefenderIfNotNeeded() const
{
	if (isNPC() || getGroup()->getNumUnits() < 2)
	{
		// NPC rarely has anything better to do with military unit than be a hinder against other players capturing workers from them.
		// NPC always return false on AI_workerNeedsDefender, so may get stuck in an infinite loop of grouping/ungrouping with defenders.
		return false;
	}
	// Never release on the same turn we contracted or an infinite loop can result
	if (m_contractsLastEstablishedTurn != GC.getGame().getGameTurn())
	{
		if (getDomainType() != DOMAIN_LAND)
		{
			return false;
		}

		if (getGroup()->canDefend() && !AI_workerNeedsDefender(plot()) && AI_reachHome(true))
		{
			if (gUnitLogLevel >= 3)
			{
				logBBAI(
					"	Worker for player %d (%S) at (%d,%d) releasing escort",
					getOwner(),
					GET_PLAYER(getOwner()).getCivilizationDescription(0),
					getX(),
					getY()
				);
			}
			getGroup()->AI_makeForceSeparate();

			return true;
		}
	}

	return false;
}

//returns true if a mission is pushed
//if eBuild is NO_BUILD, assumes a route is desired.
bool CvUnitAI::AI_improvePlot(CvPlot* pPlot, BuildTypes eBuild)
{
	PROFILE_FUNC();

	FASSERT_BOUNDS(NO_BUILD, GC.getNumBuildInfos(), eBuild);
	FAssert(pPlot != NULL);

	if (eBuild != NO_BUILD)
	{
		eBuild = AI_betterPlotBuild(pPlot, eBuild);
		if (!atPlot(pPlot))
		{
			if (!getGroup()->pushMissionInternal(MISSION_MOVE_TO, pPlot->getX(), pPlot->getY(), 0, false, false, MISSIONAI_BUILD, pPlot))
			{
				return false;
			}
		}
		getGroup()->pushMission(MISSION_BUILD, eBuild, -1, 0, (getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pPlot);

		return true;
	}
	else if (canBuildRoute() && AI_connectPlot(pPlot))
	{
		return true;
	}

	return false;

}

BuildTypes CvUnitAI::AI_betterPlotBuild(const CvPlot* pPlot, BuildTypes eBuild) const
{
	PROFILE_FUNC();

	FASSERT_BOUNDS(0, GC.getNumBuildInfos(), eBuild);
	FAssert(pPlot != NULL);

	bool bBuildRoute = false;
	bool bClearFeature = false;

	const CvBuildInfo& kOriginalBuildInfo = GC.getBuildInfo(eBuild);

	if (kOriginalBuildInfo.getRoute() != NO_ROUTE)
	{
		return eBuild;
	}

	int iWorkersNeeded = AI_calculatePlotWorkersNeeded(pPlot, eBuild);

	if ((pPlot->getNonObsoleteBonusType(getTeam()) == NO_BONUS) && (pPlot->getWorkingCity() != NULL))
	{
		iWorkersNeeded = std::max(1, std::min(iWorkersNeeded, pPlot->getWorkingCity()->getNumWorkers()));
	}

	const FeatureTypes eFeature = pPlot->getFeatureType();
	if (eFeature != NO_FEATURE)
	{
		const CvFeatureInfo& kFeatureInfo = GC.getFeatureInfo(eFeature);
		if (kOriginalBuildInfo.isFeatureRemove(eFeature))
		{
			if (kOriginalBuildInfo.getImprovement() == NO_IMPROVEMENT || !pPlot->isBeingWorked() || kFeatureInfo.getYieldChange(YIELD_FOOD) + kFeatureInfo.getYieldChange(YIELD_PRODUCTION) <= 0)
			{
				bClearFeature = true;
			}
		}

		if ((kFeatureInfo.getMovementCost() > 1) && (iWorkersNeeded > 1))
		{
			bBuildRoute = true;
		}
	}

	if (pPlot->getNonObsoleteBonusType(getTeam()) != NO_BONUS)
	{
		bBuildRoute = true;
	}
	else if (pPlot->isHills())
	{
		if ((GC.getHILLS_EXTRA_MOVEMENT() > 0) && (iWorkersNeeded > 1))
		{
			bBuildRoute = true;
		}
	}
	else if (pPlot->isAsPeak() && GC.getPEAK_EXTRA_MOVEMENT() > 0 && iWorkersNeeded > 1)
	{
		bBuildRoute = true;
	}

	if (pPlot->getRouteType() != NO_ROUTE)
	{
		bBuildRoute = false;
	}

	BuildTypes eBestBuild = NO_BUILD;
	int iBestValue = 0;
	for (int iBuild = 0; iBuild < GC.getNumBuildInfos(); iBuild++)
	{
		const BuildTypes eBuild = ((BuildTypes)iBuild);
		const CvBuildInfo& kBuildInfo = GC.getBuildInfo(eBuild);

		const RouteTypes eRoute = (RouteTypes)kBuildInfo.getRoute();
		if ((bBuildRoute && (eRoute != NO_ROUTE)) || (bClearFeature && kBuildInfo.isFeatureRemove(eFeature)))
		{
			if (canBuild(pPlot, eBuild))
			{
				int iValue = 10000;

				if (bBuildRoute && (eRoute != NO_ROUTE))
				{
					iValue *= (1 + GC.getRouteInfo(eRoute).getValue());
					iValue /= 2;

					if (pPlot->getNonObsoleteBonusType(getTeam()) != NO_BONUS)
					{
						iValue *= 2;
					}

					if (pPlot->getWorkingCity() != NULL)
					{
						iValue *= 2 + iWorkersNeeded + ((pPlot->isHills() && (iWorkersNeeded > 1)) ? 2 * GC.getHILLS_EXTRA_MOVEMENT() : 0);
						iValue /= 3;
					}
					const ImprovementTypes eImprovement = kOriginalBuildInfo.getImprovement();
					if (eImprovement != NO_IMPROVEMENT)
					{
						int iRouteMultiplier = ((GC.getImprovementInfo(eImprovement).getRouteYieldChanges(eRoute, YIELD_FOOD)) * 100);
						iRouteMultiplier += ((GC.getImprovementInfo(eImprovement).getRouteYieldChanges(eRoute, YIELD_PRODUCTION)) * 100);
						iRouteMultiplier += ((GC.getImprovementInfo(eImprovement).getRouteYieldChanges(eRoute, YIELD_COMMERCE)) * 60);
						iValue *= 100 + iRouteMultiplier;
						iValue /= 100;
					}

					int iPlotGroupId = -1;
					foreach_(const CvPlot * pLoopPlot, pPlot->adjacent())
					{
						if (pPlot->isRiver() || (pLoopPlot->getRouteType() != NO_ROUTE))
						{
							const CvPlotGroup* pLoopGroup = pLoopPlot->getPlotGroup(getOwner());
							if (pLoopGroup != NULL && pLoopGroup->getID() != -1)
							{
								if (pLoopGroup->getID() != iPlotGroupId)
								{
									//This plot bridges plot groups, so route it.
									iValue *= 4;
									break;
								}
								else
								{
									iPlotGroupId = pLoopGroup->getID();
								}
							}
						}
					}
				}

				iValue /= (kBuildInfo.getTime() + 1);

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestBuild = eBuild;
				}
			}
		}
	}

	if (eBestBuild == NO_BUILD)
	{
		return eBuild;
	}
	return eBestBuild;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_connectBonus(bool bTestTrade)
{
	AI_PERF_SCOPE(WORKER_CONNECT);
	PROFILE_FUNC();
	const CvMap& map = GC.getMap();
	const CvArea* pArea = area();
	const TeamTypes eTeam = getTeam();
	const PlayerTypes ePlayer = getOwner();

	for (int iI = map.numPlots() - 1; iI > -1; iI--)
	{
		CvPlot* plotX = map.plotByIndex(iI);

		if (plotX->getOwner() == ePlayer
		&& AI_plotValid(plotX)
		&& plotX->area() == pArea
		&& plotX->getNonObsoleteBonusType(eTeam) != NO_BONUS
		&& !plotX->isConnectedToCapital()
		&& (
			!bTestTrade
			|| plotX->getImprovementType() != NO_IMPROVEMENT
			&& GC.getImprovementInfo(plotX->getImprovementType()).isImprovementBonusTrade(plotX->getNonObsoleteBonusType(eTeam))
			)
		&& AI_connectPlot(plotX))
		{
			return true;
		}
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_connectCity()
{
	AI_PERF_SCOPE(WORKER_CONNECT);
	PROFILE_FUNC();

	CvCity* cityX = plot()->getWorkingCity();
	if (cityX
	&& cityX->getNumWorkers() <= 2
	&& AI_plotValid(cityX->plot())
	&& !cityX->isConnectedToCapital()
	&& AI_connectPlot(cityX->plot(), 1))
	{
		return true;
	}

	foreach_(const CvCity * cityX, GET_PLAYER(getOwner()).cities())
	{
		if (cityX->getNumWorkers() <= 2
		&& AI_plotValid(cityX->plot())
		&& !cityX->isConnectedToCapital()
		&& AI_connectPlot(cityX->plot(), 1))
		{
			return true;
		}
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_routeCity()
{
    AI_PERF_SCOPE(CIV_ROUTE_CITY);


	AI_PERF_SCOPE(WORKER_ROUTE);
	PROFILE_FUNC();
	FAssert(canBuildRoute());

	if (!canBuildRoute())
	{
		return false;
	}
	const int iBasePathFlags = MOVE_SAFE_TERRITORY | MOVE_AVOID_ENEMY_UNITS | MOVE_OUR_TERRITORY | MOVE_RECONSIDER_ON_LEAVING_OWNED | MOVE_NO_ENEMY_TERRITORY;

	const CvPlayerAI& player = GET_PLAYER(getOwner());
    CivilianRefinement::RouteFailures failedRoutes;

	const std::vector<ConnectionRequests::Request> requests=ConnectionRequests::candidates(*this);
	for(size_t job=0;job<requests.size();++job)
	{
        const ConnectionRequests::Request& request=requests[job];
        CvPlot* source=GC.getMap().plotByIndex(request.source);
        CvPlot* target=GC.getMap().plotByIndex(request.target);
		if (!ConnectionRequests::active(getOwner(),source,target)
        || !AI_plotValid(source) || !AI_plotValid(target)
        || !UnitTravelConnectivity::landUnitCanReach(this,source)
        || !UnitTravelConnectivity::landUnitCanReach(this,target)
        || source->isVisibleEnemyUnit(this) || target->isVisibleEnemyUnit(this)
        || player.AI_plotTargetMissionAIs(source,MISSIONAI_BUILD,getGroup())!=0
        || player.AI_plotTargetMissionAIs(target,MISSIONAI_BUILD,getGroup())!=0
        || !ConnectionRequests::needed(getOwner(),source,target)
        || (!atPlot(source)&&!failedRoutes.path(*this,source,request.source,iBasePathFlags))
        || !ConnectionRequests::canWork(*this,source,target,iBasePathFlags))
		{
			continue;
		}
        // An order can execute immediately; failed native paths must not
        // survive route construction, ownership or other callback changes.
		failedRoutes.clear();
        ConnectionRequests::trace("CLAIM",getOwner(),getID(),request);
        CvSelectionGroup* group=getGroup();
        group->clearMissionQueue();
        const int flags=MOVE_SAFE_TERRITORY|MOVE_WITH_CAUTION|MOVE_NO_ENEMY_TERRITORY;
        // Queue the complete job before activation can finish its first leg.
        // ROUTE_TO(current tile) can legally refuse an already completed
        // road; that empty approach must not discard the actual connection.
        if(!atPlot(source))group->pushMission(MISSION_ROUTE_TO,source->getX(),source->getY(),flags,true,false,MISSIONAI_BUILD,target);
        group->pushMission(MISSION_ROUTE_TO,target->getX(),target->getY(),flags,true,false,MISSIONAI_BUILD,target);
        return group->startMission();
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_routeTerritory(bool bImprovementOnly)
{
	AI_PERF_SCOPE(WORKER_ROUTE);
	PROFILE_FUNC();
	FAssert(canBuildRoute());

	const CvMap& map = GC.getMap();
	const PlayerTypes ePlayer = getOwner();

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	// This scan does not change the world until its final mission is pushed.
	// Reuse only immutable improvement metadata here, not unit build legality.
	std::vector<RouteTypes> yieldRoutes;
	std::vector<char> yieldPotential;
	std::vector<int> maximumYields;
	if (bImprovementOnly)
	{
		yieldPotential.assign(GC.getNumImprovementInfos(), 0);
		maximumYields.assign(GC.getNumImprovementInfos() * NUM_YIELD_TYPES, MIN_INT);
		for (int build = 0; build < GC.getNumBuildInfos(); ++build)
		{
			const CvBuildInfo& info = GC.getBuildInfo((BuildTypes)build);
			if (info.getRoute() != NO_ROUTE && hasBuild((BuildTypes)build)
			&& !info.isDisabled() && info.getTime() >= 0
			&& (info.getTechPrereq() == NO_TECH || GET_TEAM(getTeam()).isHasTech(info.getTechPrereq()))
			&& (info.getObsoleteTech() == NO_TECH || !GET_TEAM(getTeam()).isHasTech(info.getObsoleteTech())))
				yieldRoutes.push_back((RouteTypes)info.getRoute());
		}
	}

	const std::vector<int>& ownedPlots = GET_PLAYER(ePlayer).getOwnedPlotIndices();
	// Establish a useful nearby incumbent before considering empire-wide jobs.
	// Keep the old reverse-index order within each band; no persistent cache.
	std::vector<int> routePlots;
	routePlots.reserve(ownedPlots.size());
	for (int band = 0; band < 2; ++band)
		for (size_t owned = ownedPlots.size(); owned > 0; --owned)
		{
			const int index = ownedPlots[owned - 1];
			const CvPlot* candidate = map.plotByIndex(index);
			const bool nearby = stepDistance(getX(), getY(), candidate->getX(), candidate->getY()) <= 2;
			if (nearby == (band == 0)) routePlots.push_back(index);
		}
	const int maximumPriority = 100;
	for (size_t owned = 0; owned < routePlots.size(); ++owned)
	{
		if (workerRoutePathLimit(maximumPriority, iBestValue) < 0) break;
		const int iI = routePlots[owned];
		CvPlot* plotX = map.plotByIndex(iI);

		if (plotX->getOwner() != ePlayer
		|| !UnitTravelConnectivity::landUnitCanReach(this, plotX)

		|| !AI_plotValid(plotX))
		{
			continue;
		}
		if (bImprovementOnly)
		{
			// Route yields on an idle tile are not a present economic benefit.
			// The later general-network pass can still build its connective road.
			if (!plotX->isBeingWorked()) continue;
			const ImprovementTypes improvement = plotX->getImprovementType();
			if (improvement == NO_IMPROVEMENT) continue;
			if (!yieldPotential[improvement])
			{
				yieldPotential[improvement] = 1;
				for (size_t route = 0; route < yieldRoutes.size(); ++route)
					for (int yield = 0; yield < NUM_YIELD_TYPES; ++yield)
						maximumYields[improvement * NUM_YIELD_TYPES + yield] = std::max(
							maximumYields[improvement * NUM_YIELD_TYPES + yield],
							GC.getImprovementInfo(improvement).getRouteYieldChanges(yieldRoutes[route], yield));
			}
			bool possible = false;
			for (int yield = 0; yield < NUM_YIELD_TYPES; ++yield)
			{
				const int oldYield = plotX->getRouteType() == NO_ROUTE ? 0
					: GC.getImprovementInfo(improvement).getRouteYieldChanges(plotX->getRouteType(), yield);
				if (maximumYields[improvement * NUM_YIELD_TYPES + yield] > oldYield) possible = true;
			}
			if (!possible) continue;
		}
		// Best route depends on Tech, Plot(Terrain/Feature, and Improvement in case of eRail v Highway)
		const RouteTypes eBestRoute = GET_PLAYER(ePlayer).getBestRoute(plotX, false, this);

		if (eBestRoute == NO_ROUTE || eBestRoute == plotX->getRouteTypeForBuild(eBestRoute))
		{
			continue;
		}
		bool bValid = !bImprovementOnly;

		if (!bValid)
		{
			if (!plotX->isBeingWorked()) continue;
			const ImprovementTypes eType = plotX->getImprovementType();

			if (eType == NO_IMPROVEMENT)
			{
				continue;
			}
			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				// An existing +1 is not another +1 from upgrading this route.
				const int oldYield = plotX->getRouteType() == NO_ROUTE ? 0
					: GC.getImprovementInfo(eType).getRouteYieldChanges(plotX->getRouteType(), iJ);
				if (GC.getImprovementInfo(eType).getRouteYieldChanges(eBestRoute, iJ) > oldYield)
				{
					bValid = true;
					break;
				}
			}
		}
		const int maxWinningTurns = workerRoutePathLimit(100, iBestValue);
		if (bValid && maxWinningTurns < 0) { AI_PERF_COUNT(TARGET_PATHS_PRUNED,1);continue; }
		if (bValid && GET_PLAYER(ePlayer).AI_plotTargetMissionAIs(plotX, MISSIONAI_BUILD, getGroup(), 1) == 0)
		{
			int iPathTurns;
			if (generatePath(plotX, MOVE_IGNORE_DANGER | MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns, maxWinningTurns))
			{
				const int iValue = 10000 / (iPathTurns + 1);

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = plotX;
				}
			}
		}
	}

	if (pBestPlot)
	{

        AI_rememberCivilianDecision(CivilianDecision::WORKER_ROUTE, pBestPlot, 1);
		if (atPlot(pBestPlot) || generatePath(pBestPlot, MOVE_WITH_CAUTION | MOVE_NO_ENEMY_TERRITORY, true))
		{
			return getGroup()->pushMissionInternal(MISSION_ROUTE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_WITH_CAUTION | MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_BUILD, pBestPlot);
		}
		// No danger-free path to the route plot. Don't stall on WAIT_FOR_ESCORT when no
        // escort can come: a human's automated worker is never assigned one, and a
        // worker stranded ABROAD won't be reached by our military across foreign soil.
        // Commit via the ignore-danger path (proved to exist above) -- for an abroad
        // worker this routes it back toward our own territory. A non-abroad AI worker
        // keeps the old wait, since its military can still escort it at home.
        if (isHuman() || IsAbroad())
        {
            return getGroup()->pushMissionInternal(MISSION_ROUTE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_IGNORE_DANGER | MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_BUILD, pBestPlot);
        }
		getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT, pBestPlot);
		return true;
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_travelToUpgradeCity()
{
    NAVAL_DIAG_PHASE(UPGRADE,0,0,0,0);

	PROFILE_FUNC();

	// Check if unit is not a dead end, that there exist a better unit in the game that it one day can upgrade into.
	if (GC.getUnitInfo(getUnitType()).getNumUnitUpgrades() <= 0)
	{
		NAVAL_DIAG_PHASE_RETURN((false));
	}
	SendLog("AI_ExploreMove", CvWString::format(L"Unit: %S is not a dead end unit, and can potentially upgrade to something one day", this->getName().c_str()));
	// is there a city which can upgrade us?
	CvCity* pUpgradeCity = getUpgradeCity(/*bSearch*/ true);
	if (pUpgradeCity != NULL)
	{
		// cache some stuff
		CvPlot* pPlot = plot();

		// if we at the upgrade city, stop, wait to get upgraded
		if (pUpgradeCity->plot() == pPlot)
		{
			if (isHuman())
			{
				// Wake up and notify player!
				const CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_UNIT_UPGRADE_READY", getNameKey(), pUpgradeCity->getNameKey());
				AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, NULL, GC.getCOLOR_GREY(), getX(), getY(), true, true);

				getGroup()->setAutomateType(NO_AUTOMATE);
				//getGroup()->clearMissionQueue();
				//getGroup()->setActivityType(ACTIVITY_AWAKE);
				NAVAL_DIAG_PHASE_RETURN((true));
			}
			if (getDomainType() == DOMAIN_AIR)
			{
				NAVAL_DIAG_PHASE_RETURN((false));
			}
			if (getGroup()->pushMissionInternal(MISSION_SKIP))
			{
				NAVAL_DIAG_PHASE_RETURN((true));
			}
		}

		if (DOMAIN_AIR == getDomainType())
		{
			FAssert(!atPlot(pUpgradeCity->plot()));
			NAVAL_DIAG_PHASE_RETURN((getGroup()->pushMissionInternal(MISSION_MOVE_TO, pUpgradeCity->getX(), pUpgradeCity->getY())));
		}

		// find the closest city
		CvCity* pClosestCity = pPlot->getPlotCity();
		bool bAtClosestCity = (pClosestCity != NULL);
		if (pClosestCity == NULL)
		{
			pClosestCity = pPlot->getWorkingCity();
		}
		if (pClosestCity == NULL)
		{
			pClosestCity = GC.getMap().findCity(getX(), getY(), NO_PLAYER, getTeam(), true, getDomainType() == DOMAIN_SEA);
		}

		// can we path to the upgrade city?
		int iUpgradeCityPathTurns;
		CvPlot* pThisTurnPlot = NULL;
		bool bCanPathToUpgradeCity = generatePath(pUpgradeCity->plot(), 0, true, &iUpgradeCityPathTurns);
		if (bCanPathToUpgradeCity)
		{
			pThisTurnPlot = getPathEndTurnPlot();
		}

		// if we close to upgrade city, head there
		if (NULL != pThisTurnPlot && NULL != pClosestCity
		&& (pClosestCity == pUpgradeCity || iUpgradeCityPathTurns < 4)
		&& !exposedToDanger(pThisTurnPlot, 70))
		{
			FAssert(!atPlot(pThisTurnPlot));
			if (getGroup()->pushMissionInternal(MISSION_MOVE_TO, pThisTurnPlot->getX(), pThisTurnPlot->getY()))
			{
				NAVAL_DIAG_PHASE_RETURN((true));
			}
		}

		// check for better airlift choice
		if (getDomainType() == DOMAIN_LAND && NULL != pClosestCity && pClosestCity->getMaxAirlift() > 0)
		{
			// if we at the closest city, then do the airlift, or wait
			if (bAtClosestCity)
			{
				// can we do the airlift this turn?
				if (canAirliftAt(pClosestCity->plot(), pUpgradeCity->getX(), pUpgradeCity->getY()))
				{
					if (getGroup()->pushMissionInternal(MISSION_AIRLIFT, pUpgradeCity->getX(), pUpgradeCity->getY()))
					{
						NAVAL_DIAG_PHASE_RETURN((true));
					}
				}
				// wait to do it next turn
				else if (getGroup()->pushMissionInternal(MISSION_SKIP))
				{
					NAVAL_DIAG_PHASE_RETURN((true));
				}
			}
			int iClosestCityPathTurns;
			const CvPlot* pThisTurnPlotForAirlift =
				(
					generatePath(pClosestCity->plot(), 0, true, &iClosestCityPathTurns)
					?
					getPathEndTurnPlot()
					:
					NULL
				);
			// is the closest city closer pathing? If so, move toward closest city
			if (NULL != pThisTurnPlotForAirlift
			&& (!bCanPathToUpgradeCity || iClosestCityPathTurns < iUpgradeCityPathTurns)
			&& !exposedToDanger(pThisTurnPlotForAirlift, 70))
			{
				FAssert(!atPlot(pThisTurnPlotForAirlift));
				if (getGroup()->pushMissionInternal(MISSION_MOVE_TO, pThisTurnPlotForAirlift->getX(), pThisTurnPlotForAirlift->getY()))
				{
					NAVAL_DIAG_PHASE_RETURN((true));
				}
			}
		}

		// did not have better airlift choice, go ahead and path to the upgrade city
		if (NULL != pThisTurnPlot)
		{
			if (!exposedToDanger(pThisTurnPlot, 70))
			{
				FAssert(!atPlot(pThisTurnPlot));
				NAVAL_DIAG_PHASE_RETURN((getGroup()->pushMissionInternal(MISSION_MOVE_TO, pThisTurnPlot->getX(), pThisTurnPlot->getY())));
			}
		}
	}
	NAVAL_DIAG_PHASE_RETURN((false));
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_retreatToCity(bool bPrimary, bool bAirlift, int iMaxPath)
{
    NAVAL_DIAG_PHASE(RETREAT,bPrimary,bAirlift,iMaxPath,0);

	AI_PERF_SCOPE(RETREAT_CITY);
	PROFILE_FUNC();

	CvPlot* pBestPlot = NULL;
    const CvPlot* refuge = NULL;
	CvPlot* endTurnPlot = NULL;
	int iBestValue = MAX_INT;
	const int iCurrentDanger = GET_PLAYER(getOwner()).AI_getPlotDanger(plot());

	CvCity* pCity = plot()->getPlotCity();

	// Friendly defenders can make numeric danger negative for every domain.
	// An AI unit already in an acceptable owned refuge must not leave merely
	// because the balance is below zero. Preserve human automation policy.
	const bool aiUnit = !isHuman();
	if ((iCurrentDanger == 0 || (aiUnit && iCurrentDanger < 0))
	&& pCity && pCity->getOwner() == getOwner()
	&& (!bPrimary || GET_PLAYER(getOwner()).AI_isPrimaryArea(pCity->area()))
	&& (!bAirlift || pCity->getMaxAirlift() > 0)
	&& !pCity->plot()->isVisibleEnemyUnit(this))
	{
		// If we've successfully retreated and have an escort release it for now,
		//	the exceptions are generals who build up multiple escorts while in cities
		if (!canDefend() && getGroup()->canDefend() && AI_getUnitAIType() != UNITAI_GENERAL)
		{
			joinGroup(NULL);
		}
		getGroup()->pushMission(MISSION_SKIP);
		NAVAL_DIAG_PHASE_RETURN((true));
	}
	const int iSearchRange = AI_searchRange(iMaxPath);

    // Once a ship has reached an acceptable port, another retreat must
    // actually improve safety. Otherwise two equally threatened ports keep
    // sending the fleet back and forth during the same turn.
    const bool comparePortSafety = !isHuman() && getDomainType() == DOMAIN_SEA
        && pCity && pCity->getOwner() == getOwner()
        && (!bPrimary || GET_PLAYER(getOwner()).AI_isPrimaryArea(pCity->area()))
        && (!bAirlift || pCity->getMaxAirlift() > 0)
        && !pCity->plot()->isVisibleEnemyUnit(this);
    std::vector<const CvCity*> refuges;
    std::vector<CvPlot*> targets;
    foreach_(const CvCity* city, GET_PLAYER(getOwner()).cities())
    {
        if ((!bPrimary || GET_PLAYER(getOwner()).AI_isPrimaryArea(city->area()))
            && (!bAirlift || city->getMaxAirlift()>0)
            && UnitTravelConnectivity::seaUnitCanReach(this,city->plot())
            && !city->plot()->isVisibleEnemyUnit(this)
            && (bAirlift || getDomainType()!=DOMAIN_LAND || UnitTravelConnectivity::landUnitCanReach(this,city->plot()) || getGroup()->canMoveAllTerrain())
            && !atPlot(city->plot())
            && (!comparePortSafety || GET_PLAYER(getOwner()).AI_getPlotDanger(city->plot()) < iCurrentDanger))
        {
            targets.push_back(city->plot());
            refuges.push_back(city);
        }
    }
    // Candidate evaluation does not change the world. The two passes for
    // each flag set can share their private target view, including failures.
    CvReachablePlotSet cautiousView(getGroup(),0);
    CvReachablePlotSet permissiveView(getGroup(),MOVE_IGNORE_DANGER);
    bool cautiousReady=false,permissiveReady=false;
	int iPass = 0;
	for (; iPass < 4; iPass++)
	{
        CvReachablePlotSet& plotSet=iPass>1?permissiveView:cautiousView;
        bool& ready=iPass>1?permissiveReady:cautiousReady;
        if (!ready) {plotSet.PopulateForTargets(targets,iSearchRange);ready=true;}

		foreach_(const CvCity * cityX, refuges)
		{
			if (bPrimary && !GET_PLAYER(getOwner()).AI_isPrimaryArea(cityX->area())
			|| bAirlift && cityX->getMaxAirlift() < 1
			|| cityX->plot()->isVisibleEnemyUnit(this)
			|| !bAirlift && getDomainType() == DOMAIN_LAND && !UnitTravelConnectivity::landUnitCanReach(this,cityX->plot()) && !getGroup()->canMoveAllTerrain()
			|| atPlot(cityX->plot())
			|| plotSet.find(cityX->plot()) == plotSet.end())
			{
				continue;
			}
			int iPathTurns;

			// Ships must satisfy this existing turn limit anyway. Bound the
			// search itself instead of discovering an arbitrarily long route.
			const int pathLimit = !isHuman() && getDomainType() == DOMAIN_SEA
				? navalRetreatPathLimit(iMaxPath, iPass, iBestValue) : MAX_INT;
			if (pathLimit >= 0 && generatePath(cityX->plot(), ((iPass > 1) ? MOVE_IGNORE_DANGER : 0), true, &iPathTurns, pathLimit)
			&& iPathTurns <= (iPass == 2 ? 1 : iMaxPath))
			{
				// Water units can't defend a city.
				// Any unthreatened city acceptable on 0th pass, solves problem where sea units would
				// oscillate in and out of threatened city because they had iCurrentDanger = 0 on turns outside city

				// A warship can fight at sea, but does not make a threatened
				// port safe. Apply the refuge safety test to every naval group.
				bool bCheck = iPass > 0 || (getDomainType() != DOMAIN_SEA && getGroup()->canDefend());
				if (!bCheck)
				{
					const int iLoopDanger = GET_PLAYER(getOwner()).AI_getPlotDanger(cityX->plot());
					bCheck = (
						(iLoopDanger == 0 || (aiUnit && iLoopDanger < 0))
						|| (
							iLoopDanger < iCurrentDanger
							&& //Fuyu: try to avoid doomed cities
							iLoopDanger < 2 * (cityX->plot()->getNumDefenders(getOwner()))
							)
					);
				}
				if (bCheck)
				{
					int iValue = iPathTurns;

					if (AI_getUnitAIType() == UNITAI_SETTLER_SEA)
					{
						iValue *= 1 + std::max(0, GET_PLAYER(getOwner()).AI_totalAreaUnitAIs(cityX->area(), UNITAI_SETTLE) - GET_PLAYER(getOwner()).AI_totalAreaUnitAIs(cityX->area(), UNITAI_SETTLER_SEA));
					}

					if (iValue < iBestValue)
					{
						endTurnPlot = getPathEndTurnPlot();

						if (endTurnPlot == cityX->plot() || !exposedToDanger(endTurnPlot, 60))
						{
							iBestValue = iValue;
							pBestPlot = endTurnPlot;
                            refuge = cityX->plot();

							if (atPlot(pBestPlot))
							{
								pBestPlot = getGroup()->getPathFirstPlot();
								FAssert(!atPlot(pBestPlot));
							}
						}
					}
				}
			}
		}

		if (pBestPlot != NULL)
		{
			break;
		}
		if (iPass == 0 && pCity && pCity->getOwner() == getOwner()
		&& (!bPrimary || GET_PLAYER(getOwner()).AI_isPrimaryArea(pCity->area()))
		&& (!bAirlift || pCity->getMaxAirlift() > 0)
		&& !pCity->plot()->isVisibleEnemyUnit(this))
		{
			// If we've successfully retreated and have an escort release it for now
			if (!canDefend() && getGroup()->canDefend())
			{
				joinGroup(NULL);
			}
			getGroup()->pushMission(MISSION_SKIP);
			NAVAL_DIAG_PHASE_RETURN((true));
		}
		if (getGroup()->alwaysInvisible())
		{
			break;
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));
        AI_rememberNavalCommitment(NavalCommitment::REFUGE, refuge, iPass > 1 ? MOVE_IGNORE_DANGER : 0);
		// Execute the same danger policy that selected this path (passes 0/1 are cautious).
		NAVAL_DIAG_PHASE_RETURN((getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), iPass > 1 ? MOVE_IGNORE_DANGER : 0)));
	}

	if (pCity && pCity->getTeam() == getTeam())
	{
		// If we've successfully retreated and have an escort release it for now
		if (!canDefend() && getGroup()->canDefend())
		{
			joinGroup(NULL);
		}
		getGroup()->pushMission(MISSION_SKIP);
		NAVAL_DIAG_PHASE_RETURN((true));
	}
	NAVAL_DIAG_PHASE_RETURN((false));
}


// Returns true if a mission was pushed...
#include "TransportPickupPathLimit.h"

bool CvUnitAI::AI_pickup(UnitAITypes eUnitAI, bool bCountProduction, int iMaxPath)
{
	PROFILE_FUNC();

	CvCity* pCity;
	CvPlot* pBestPlot;
	CvPlot* pBestPickupPlot;
	CvPlot* endTurnPlot = NULL;
	int iPathTurns;
	int iValue;
	int iBestValue;

	bool bFull = (getGroup()->AI_isFull() && (getGroup()->getCargo() > 0));
	if (bFull)
	{
		FErrorMsg("Unit is not expected to be full when AI_pickup is called");
		return false;
	}

	pCity = plot()->getPlotCity();

	if (pCity != NULL)
	{
		if (pCity->getOwner() == getOwner())
		{
			if (((GC.getGame().getGameTurn() - pCity->getGameTurnAcquired()) > 15 || (GET_TEAM(getTeam()).countEnemyPowerByArea(pCity->area()) == 0))
				&& !(AI_getUnitAIType() == UNITAI_ASSAULT_SEA && pCity->area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE))
			{
				// only count units which are available to load
				const bool requireOrder = requiresExplicitAssaultPickup(AI_getUnitAIType(), eUnitAI, pCity->area()->getAreaAIType(getTeam()));
				int iCount = pCity->plot()->plotCount(PUF_isAvailableAssaultPickup, eUnitAI, requireOrder, NULL, getOwner(), NO_TEAM, PUF_isFiniteRange);
                    if (eUnitAI == UNITAI_WORKER && AI_getUnitAIType() == UNITAI_SETTLER_SEA)
                        iCount = orderedWorkerFerryPassengers(pCity->plot(), this, true);
                    if (eUnitAI == UNITAI_SETTLE && AI_getUnitAIType() == UNITAI_SETTLER_SEA)
                        iCount = AI_colonyPassengers(pCity->plot());

				if (!((eUnitAI == UNITAI_WORKER || eUnitAI == UNITAI_SETTLE) && AI_getUnitAIType() == UNITAI_SETTLER_SEA) && !requireOrder && bCountProduction && (pCity->getProductionUnitAI() == eUnitAI))
				{
					if (pCity->getProductionTurnsLeft() < 4)
					{
						const CvUnitInfo& kUnitInfo = GC.getUnitInfo(pCity->getProductionUnit());
						if ((kUnitInfo.getDomainType() != DOMAIN_AIR) || kUnitInfo.getAirRange() > 0)
						{
							iCount++;
						}
					}
				}

				// No passenger/eligible production means no garrison or route evaluation.
				bool bConsider = false;
				if (iCount > 0)
				{

					if (AI_getUnitAIType() == UNITAI_ASSAULT_SEA)
					{
						// Improve island hopping
						if (pCity->area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE)
						{
							bConsider = false;
						}
						else if (eUnitAI == UNITAI_ATTACK_CITY && !(pCity->AI_isDanger()))
						{
							bConsider = (pCity->plot()->plotCount(PUF_canDefend, -1, -1, NULL, getOwner(), NO_TEAM, PUF_isDomainType, DOMAIN_LAND) > pCity->AI_neededDefenders());
						}
						else
						{
							bConsider = pCity->AI_isDefended(-GET_PLAYER(getOwner()).strengthOfBestUnitAI(DOMAIN_LAND, UNITAI_CITY_DEFENSE));
						}
					}
					else if (AI_getUnitAIType() == UNITAI_SETTLER_SEA)
					{
						if (eUnitAI == UNITAI_CITY_DEFENSE)
						{
							bConsider = (pCity->plot()->plotCount(PUF_canDefendGroupHead, -1, -1, NULL, getOwner(), NO_TEAM, PUF_isCityAIType) > 1);
						}
						else
						{
							bConsider = true;
						}
					}
					else
					{
						bConsider = true;
					}

				}

				if (bConsider)
				{
					if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
					{
						if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pCity->plot(), MISSIONAI_PICKUP, getGroup()) < ((iCount + (getGroup()->getCargoSpace() - 1)) / std::max(1, getGroup()->getCargoSpace())))
						{
							getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_PICKUP, pCity->plot());
							return true;
						}
					}
					else
					{
						if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIsinCargoVolume(pCity->plot(), MISSIONAI_PICKUP, getGroup()) < (((iCount * 100) + (getGroup()->getCargoSpace() - 100)) / std::max(1, getGroup()->getCargoSpace())))
						{
							getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_PICKUP, pCity->plot());
							return true;
						}
					}
				}
			}
		}
	}

	iBestValue = 0;
	pBestPlot = NULL;
	pBestPickupPlot = NULL;

	foreach_(CvCity * pLoopCity, GET_PLAYER(getOwner()).cities())
	{
		if (AI_plotValid(pLoopCity->plot()) && UnitTravelConnectivity::seaUnitCanReach(this,pLoopCity->plot()))
		{
			if (((GC.getGame().getGameTurn() - pLoopCity->getGameTurnAcquired()) > 15 || (GET_TEAM(getTeam()).countEnemyPowerByArea(pLoopCity->area()) == 0))
				&& !(AI_getUnitAIType() == UNITAI_ASSAULT_SEA && pLoopCity->area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE))
			{
				// only count units which are available to load, have had a chance to move since being built
				const bool requireOrder = requiresExplicitAssaultPickup(AI_getUnitAIType(), eUnitAI, pLoopCity->area()->getAreaAIType(getTeam()));
				int iCount = pLoopCity->plot()->plotCount(PUF_isAvailableAssaultPickup, eUnitAI, requireOrder, NULL, getOwner(), NO_TEAM, (bCountProduction ? PUF_isFiniteRange : PUF_isFiniteRangeAndNotJustProduced));
                    if (eUnitAI == UNITAI_WORKER && AI_getUnitAIType() == UNITAI_SETTLER_SEA)
                        iCount = orderedWorkerFerryPassengers(pLoopCity->plot(), this, bCountProduction);
                    if (eUnitAI == UNITAI_SETTLE && AI_getUnitAIType() == UNITAI_SETTLER_SEA)
                        iCount = AI_colonyPassengers(pLoopCity->plot());

				iValue = iCount * 10;

				if (!((eUnitAI == UNITAI_WORKER || eUnitAI == UNITAI_SETTLE) && AI_getUnitAIType() == UNITAI_SETTLER_SEA) && !requireOrder && bCountProduction && (pLoopCity->getProductionUnitAI() == eUnitAI))
				{
					const CvUnitInfo& kUnitInfo = GC.getUnitInfo(pLoopCity->getProductionUnit());
					if ((kUnitInfo.getDomainType() != DOMAIN_AIR) || kUnitInfo.getAirRange() > 0)
					{
						iValue++;
						iCount++;
					}
				}

				// Passenger availability is cheaper than the city defense calculation.
				if (iValue <= 0) continue;

                // Upper bound at zero travel turns: a losing port needs neither
                // a garrison calculation nor reservation/danger checks.
                int pickupValue = iValue + pLoopCity->getPopulation();
                if (AI_getUnitAIType() == UNITAI_ASSAULT_SEA)
                {
                    const AreaAITypes areaAI = pLoopCity->area()->getAreaAIType(getTeam());
                    if (areaAI == AREAAI_ASSAULT) pickupValue *= 4;
                    else if (areaAI == AREAAI_ASSAULT_ASSIST) pickupValue *= 2;
                }
                const int pickupMaxTurns = transportPickupPathLimit(pickupValue, iBestValue, iMaxPath);
                if (pickupMaxTurns < 0) continue;

				bool bConsider = false;

				if (AI_getUnitAIType() == UNITAI_ASSAULT_SEA)
				{
					if (pLoopCity->area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE)
					{
						bConsider = false;
					}
					else if (eUnitAI == UNITAI_ATTACK_CITY && !(pLoopCity->AI_isDanger()))
					{
						// Improve island hopping
						bConsider = (pLoopCity->plot()->plotCount(PUF_canDefend, -1, -1, NULL, getOwner(), NO_TEAM, PUF_isDomainType, DOMAIN_LAND) > pLoopCity->AI_neededDefenders());
					}
					else
					{
						bConsider = pLoopCity->AI_isDefended(GET_PLAYER(getOwner()).strengthOfBestUnitAI(DOMAIN_LAND, UNITAI_CITY_DEFENSE));
					}
				}
				else if (AI_getUnitAIType() == UNITAI_SETTLER_SEA)
				{
					if (eUnitAI == UNITAI_CITY_DEFENSE)
					{
						bConsider = (pLoopCity->plot()->plotCount(PUF_canDefendGroupHead, -1, -1, NULL, getOwner(), NO_TEAM, PUF_isCityAIType) > 1);
					}
					else
					{
						bConsider = true;
					}
				}
				else
				{
					bConsider = true;
				}

				if (bConsider)
				{
					if (iValue > 0)
					{
						iValue += pLoopCity->getPopulation();

						if (!pLoopCity->plot()->isVisibleEnemyUnit(this))
						{
							bool bCheck = false;
							if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
							{
								bCheck = (GET_PLAYER(getOwner()).AI_plotTargetMissionAIsinCargoVolume(pLoopCity->plot(), MISSIONAI_PICKUP, getGroup()) < (((iCount * 100) + (getGroup()->getCargoSpace() - 100)) / std::max(1, getGroup()->getCargoSpace())));
							}
							else
							{
								bCheck = (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_PICKUP, getGroup()) < ((iCount + (getGroup()->getCargoSpace() - 1)) / std::max(1, getGroup()->getCargoSpace())));
							}
							if (bCheck)
							{
								if (pickupMaxTurns >= 0 && !(pLoopCity->AI_isDanger()))
								{
									if (!atPlot(pLoopCity->plot()) && generatePath(pLoopCity->plot(), 0, true, &iPathTurns, pickupMaxTurns))
									{
										iValue = pickupValue * 1000;

										iValue /= (iPathTurns + 3);

										if ((iValue > iBestValue))
										{
											endTurnPlot = getPathEndTurnPlot();

											if (endTurnPlot == pLoopCity->plot() || !exposedToDanger(endTurnPlot, 60))
											{
												iBestValue = iValue;
												// Do one turn along path, then reevaluate
												// Causes update of destination based on troop movement
												//pBestPlot = pLoopCity->plot();
												pBestPlot = endTurnPlot;
												pBestPickupPlot = pLoopCity->plot();

												if (pBestPlot == NULL || atPlot(pBestPlot))
												{
													//FErrorMsg("error");
													pBestPlot = pBestPickupPlot;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestPickupPlot != NULL))
	{
		if (!isWaitingOnUnitAIAny())
		{
			FAssert(!atPlot(pBestPlot));
            AI_rememberNavalCommitment(NavalCommitment::PICKUP, pBestPickupPlot, MOVE_AVOID_ENEMY_WEIGHT_3, MISSIONAI_PICKUP);
            m_aiNavalCommitment[NavalCommitment::TARGET_OWNER] = eUnitAI;
            m_aiNavalCommitment[NavalCommitment::TARGET_UNIT] = bCountProduction ? 1 : 0;
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_WEIGHT_3, false, false, MISSIONAI_PICKUP, pBestPickupPlot);
		}
		else
		{
			return getGroup()->pushMissionInternal(MISSION_SKIP);
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_pickupStranded(UnitAITypes eUnitAI, int iMaxPath)
{
    AI_PERF_SCOPE(TRANSPORT_PICKUP);
	//TB TRANSPORTNOTES: potentially big problems here with checking for the amount of cargo not being possible to check in SM perspective.
	PROFILE_FUNC();

	if (isNPC())
	{
		return false;
	}

	if (getGroup()->AI_isFull() && getGroup()->getCargo() > 0)
	{
		FErrorMsg("Unit is not expected to be full when AI_pickupStranded is called");
		return false;
	}

	const bool bSM = GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS);

	const CvPlayerAI& player = GET_PLAYER(getOwner());

	int iBestValue = 0;
	CvUnit* pBestUnit = NULL;
	CvPlot* pBestPickupPlot = NULL;

	CvReachablePlotSet plotSet(getGroup(), MOVE_ALLOW_ADJACENT_COASTAL, -1);
	bool reachabilityReady = false;

	std::vector<CvSelectionGroup*> rescueGroups;
	if (TransportRequests::enabled()) rescueGroups = TransportRequests::candidates(getOwner(), getID());
	else foreach_(CvSelectionGroup* group, player.groups()) rescueGroups.push_back(group);
	foreach_(CvSelectionGroup * groupX, rescueGroups)
	{
		if (groupX->isStranded())
		{
			CvUnit* pHeadUnit = groupX->getHeadUnit();
			if (pHeadUnit == NULL || eUnitAI != NO_UNITAI && pHeadUnit->AI_getUnitAIType() != eUnitAI)
			{
				continue;
			}
			CvPlot* plotX = pHeadUnit->plot();
			if (plotX == NULL || !plotX->isCoastalLand() && !canMoveAllTerrain())
			{
				continue;
			}
			if (CivilianRefinement::rescue() && !UnitTravelConnectivity::seaUnitCanReachPickup(this, plotX))
			{
				continue;
			}
			if (TransportRequests::enabled() && !TransportRequests::nearestAvailable(getOwner(),groupX->getID(),getID()))continue;
			// Units are stranded, attempt rescue
			// TB TRANSPORTNOTES: iCount should be shifted to getting the cargo volume of groupX.
			int iCount = bSM ? groupX->getNumUnitCargoVolumeTotal() : groupX->getNumUnits();

			if (1000 * iCount > iBestValue)
			{
				// Reject cargo that cannot fit before expanding reachable sea/coast plots.
                if ((bSM || TransportRequests::enabled() || CivilianRefinement::rescue())
                    && getGroup()->getCargoSpaceAvailable((SpecialUnitTypes)pHeadUnit->getSpecialUnitType(),
                    (DomainTypes)pHeadUnit->getDomainType()) < (bSM ? iCount : 1)) continue;
                PROFILE("CvUnitAI::AI_pickupStranded.EvaluateTargetUnit");

				CvPlot* pPickupPlot = NULL;
				int pickupTurns = -1;
				if (TransportRequests::enabled())
				{
					MissionAITypes pickup = MISSIONAI_PICKUP;
					iCount -= player.AI_unitTargetMissionAIs(pHeadUnit, &pickup, 1, getGroup(), iMaxPath == MAX_INT ? -1 : iMaxPath) * cargoSpace();
					const int value = 1000 * iCount;
					if (value <= iBestValue) continue;
					const int limit = iBestValue == 0 ? iMaxPath : std::min(iMaxPath, value / iBestValue);
					std::vector<CvPlot*> endpoints;
					endpoints.push_back(plotX);
					foreach_(CvPlot* neighbor, plotX->adjacent()) endpoints.push_back(neighbor);
					std::stable_sort(endpoints.begin(),endpoints.end(),TransportRouting::EndpointOrder(this));
					for (size_t endpoint = 0; endpoint < endpoints.size(); ++endpoint)
					{
						CvPlot* tile = endpoints[endpoint];
						if (tile->isVisibleEnemyUnit(this)) continue;
						if (!atPlot(tile) && !getGroup()->canMoveAllTerrain()
							&& !tile->isWater() && !tile->isFriendlyCity(*this, true)) continue;
						int turns = 0;
						if (atPlot(tile) || generatePath(tile, 0, true, &turns,limit))
						{
							pickupTurns = turns;pPickupPlot = tile;break;
						}
					}
				}
				else if (atPlot(plotX))
				{
					pPickupPlot = plotX;
				}
				else
				{
					// Most rescue queries have no eligible stranded group. Do not
					// expand the map until a remote candidate needs this answer.
					if (!reachabilityReady)
					{
						plotSet.Populate(AI_searchRange(iMaxPath));
						reachabilityReady = true;
					}
					if (plotSet.find(plotX) != plotSet.end())
					{
						pPickupPlot = plotX;
					}
					else foreach_(CvPlot * plotY, plotX->adjacent())
					{
						if (plotSet.find(plotY) != plotSet.end()
						// If we can move into the target plot in one turn take account of enemy units there
						&& (stepDistance(plotY->getX(), plotY->getY(), getX(), getY()) > getMoves()
						|| !plotY->isVisibleEnemyUnit(this)))
						{
							pPickupPlot = plotY;
							break;
						}
					}
				}

				if (TransportRequests::enabled() && pPickupPlot == NULL)
					TransportRequests::rejected(getOwner(),groupX->getID(),getID(),iMaxPath);
				if (pPickupPlot != NULL)
				{
					MissionAITypes eMissionAIType = MISSIONAI_PICKUP;
					if (!TransportRequests::enabled()) iCount -= (
						player.AI_unitTargetMissionAIs(pHeadUnit, &eMissionAIType, 1, getGroup(), iMaxPath == MAX_INT ? -1 : iMaxPath)
						*
						cargoSpace()
					);
					int iValue = 1000 * iCount;
                    // Division by (arrival + 1) cannot improve this score.
                    if (iValue <= iBestValue) continue;

					const int iMaxValuePath = (iBestValue == 0 ? MAX_INT : iValue / iBestValue);


					{
						int iPathTurns = pickupTurns < 0 ? 0 : pickupTurns;
						if (pickupTurns >= 0 || generatePath(pPickupPlot, 0, true, &iPathTurns, std::min(iMaxPath, iMaxValuePath)))
						{
							iValue /= iPathTurns + 1;

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestUnit = pHeadUnit;
								pBestPickupPlot = pPickupPlot;
							}
						}
					}
				}
			}
			if (TransportRequests::enabled() && pBestUnit != NULL)break;
		}
	}

	if (pBestUnit != NULL)
	{
		if (TransportRequests::enabled())
		{
			const int owner = getOwner(), ship = getID(), request = pBestUnit->getGroupID();
			const bool issued = getGroup()->pushMissionInternal(MISSION_MOVE_TO_UNIT,
				pBestUnit->getOwner(), pBestUnit->getID(), 0, false, false,
				MISSIONAI_PICKUP, pBestPickupPlot, pBestUnit);
			if (issued) TransportRequests::assign(owner, request, ship);
			return issued;
		}
		if (atPlot(pBestPickupPlot))
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_PICKUP, pBestUnit->plot());
			return true;
		}
		FAssert(!atPlot(pBestPickupPlot));
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPickupPlot->getX(), pBestPickupPlot->getY(), 0, false, false, MISSIONAI_PICKUP, pBestPickupPlot);
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_airOffensiveCity()
{
	PROFILE_EXTRA_FUNC();
	//PROFILE_FUNC();

	FAssert(canAirAttack() || nukeRange() >= 0);

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		const CvPlot* plotX = GC.getMap().plotByIndex(iI);

		// Limit to cities and forts, true for any city but only this team's forts
		if (plotX->isCity(true, getTeam())
		&&
		(
			plotX->getTeam() == getTeam()
			||
			plotX->isOwned() && GET_TEAM(plotX->getTeam()).isVassal(getTeam())
			)
		&& (atPlot(plotX) || canEnterPlot(plotX)))
		{
			const int iValue = AI_airOffenseBaseValue(plotX);

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestPlot = plotX;
			}
		}
	}

	if (pBestPlot != NULL && !atPlot(pBestPlot))
	{
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_SAFE_TERRITORY);
	}
	return false;
}


// Function for ranking the value of a plot as a base for offensive air units
int CvUnitAI::AI_airOffenseBaseValue(const CvPlot* pPlot) const
{
	PROFILE_EXTRA_FUNC();
	if (pPlot == NULL)
	{
		return 0;
	}
	const CvArea* baseArea = pPlot->area();

	if (baseArea == NULL)
	{
		return 0;
	}
	CvCity* pCity = pPlot->getPlotCity();

	int iDefenders = pPlot->plotCount(PUF_canDefend, -1, -1, NULL, pPlot->getOwner());
	const TeamTypes eTeam = getTeam();

	int iAttackAirCount = pPlot->plotCount(PUF_canAirAttack, -1, -1, NULL, NO_PLAYER, eTeam);
	iAttackAirCount += 2 * pPlot->plotCount(PUF_isUnitAIType, UNITAI_ICBM, -1, NULL, NO_PLAYER, eTeam);
	if (atPlot(pPlot))
	{
		iAttackAirCount += canAirAttack() ? -1 : 0;
		iAttackAirCount += (nukeRange() >= 0) ? -2 : 0;
	}

	if (pPlot->isCoastalLand(GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize()))
	{
		iDefenders -= 1;
	}

	if (pCity != NULL)
	{
		if (pCity->getDefenseModifier(true) < 40)
		{
			iDefenders -= 1;
		}

		if (pCity->getOccupationTimer() > 1)
		{
			iDefenders -= 1;
		}
	}

	// Consider threat from nearby enemy territory
	int iBorderDanger = 1;

	foreach_(const CvPlot * plotX, pPlot->adjacent())
	{
		if (plotX->area() == baseArea && plotX->isOwned() && plotX->getTeam() != eTeam
		&& !GET_TEAM(plotX->getTeam()).isVassal(eTeam))
		{
			iBorderDanger++;
			// Toffer - Will iDistance ever be more than 1 in this loop?
			const int iDistance = stepDistance(pPlot->getX(), pPlot->getY(), plotX->getX(), plotX->getY());
			if (iDistance == 1)
			{
				iBorderDanger++;
			}

			if (atWar(plotX->getTeam(), eTeam))
			{
				if (iDistance == 1)
				{
					iBorderDanger += 2;
				}
				else if (iDistance == 2 && plotX->isRoute())
				{
					iBorderDanger += 2;
				}
			}
		}
	}

	iDefenders -= std::min(3, iBorderDanger / 3);

	// Don't put more attack air units on plot than effective land defenders ... too large a risk
	if (iAttackAirCount >= iDefenders || iDefenders <= 0)
	{
		return 0;
	}

	int iValue = 0;

	if (GET_TEAM(eTeam).hasWarPlan(true))
	{
		// Don't count assault assist, don't want to weight defending colonial coasts when homeland might be under attack
		const bool bAssault = baseArea->getAreaAIType(eTeam) == AREAAI_ASSAULT || baseArea->getAreaAIType(eTeam) == AREAAI_ASSAULT_MASSING;

		// Loop over operational range
		const int iRange = airRange();

		foreach_(const CvPlot * plotX, pPlot->rect(iRange, iRange))
		{
			if (plotX->area() != NULL)
			{
				const int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), plotX->getX(), plotX->getY());

				if (iDistance <= iRange)
				{
					// Value system is based around 1 enemy military unit in our territory = 10 pts
					int iTempValue = 0;

					if (plotX->isWater())
					{
						if (plotX->isVisible(eTeam, false) && !plotX->area()->isLake())
						{
							// Defend ocean
							iTempValue = 1;

							if (plotX->isOwned())
							{
								if (plotX->getTeam() == eTeam)
								{
									iTempValue += 1;
								}
								else if (plotX->getTeam() != eTeam && GET_TEAM(eTeam).AI_getWarPlan(plotX->getTeam()) != NO_WARPLAN)
								{
									iTempValue += 1;
								}
							}

							// Low weight for visible ships cause they will probably move
							iTempValue += 2 * plotX->getNumVisiblePotentialEnemyDefenders(this);

							if (bAssault)
							{
								iTempValue *= 2;
							}
						}
					}
					else if (!plotX->isOwned())
					{
						if (iDistance < iRange - 2)
						{
							// Target enemy troops in neutral territory
							iTempValue += 4 * plotX->getNumVisiblePotentialEnemyDefenders(this);
						}
					}
					else if (plotX->getTeam() == eTeam)
					{
						iTempValue = 0;

						if (iDistance < iRange - 2)
						{
							// Target enemy troops in our territory
							iTempValue += 5 * plotX->getNumVisiblePotentialEnemyDefenders(this);

							if (plotX->getOwner() == getOwner())
							{
								if (GET_PLAYER(getOwner()).AI_isPrimaryArea(plotX->area()))
								{
									iTempValue *= 3;
								}
								else iTempValue *= 2;
							}

							if (plotX->area()->getAreaAIType(eTeam) == AREAAI_DEFENSIVE)
							{
								iTempValue *= 2;
							}
						}
					}
					else if (plotX->getTeam() != eTeam && GET_TEAM(eTeam).AI_getWarPlan(plotX->getTeam()) != NO_WARPLAN)
					{
						// Attack opponents land territory
						iTempValue = 3;

						const CvCity* cityX = plotX->getPlotCity();

						if (cityX != NULL)
						{
							// Target enemy cities
							iTempValue += 3 * cityX->getPopulation() + 30;

							if (canAirBomb() && cityX->isBombardable(this))
							{
								iTempValue *= 2;
							}

							if (plotX->area()->getTargetCity(getOwner()) == cityX)
							{
								iTempValue *= 2;
							}

							if (cityX->AI_isDanger())
							{
								// Multiplier for nearby troops, ours, teammate's, and any other enemy of city
								iTempValue *= 3;
							}
						}
						else
						{
							if (iDistance < iRange - 2)
							{
								// Support our troops in enemy territory
								iTempValue += 15 * plotX->getNumDefenders(getOwner());

								// Target enemy troops adjacent to our territory
								if (plotX->isAdjacentTeam(eTeam, true))
								{
									iTempValue += 7 * plotX->getNumVisiblePotentialEnemyDefenders(this);
								}
							}

							// Weight resources
							if (canAirBombAt(pPlot, plotX->getX(), plotX->getY())
							&& plotX->getBonusType(eTeam) != NO_BONUS)
							{
								iTempValue += 8 * std::max(2, GET_PLAYER(plotX->getOwner()).AI_bonusVal(plotX->getBonusType(eTeam)) / 10);
							}
						}

						if (plotX->area()->getAreaAIType(eTeam) == AREAAI_OFFENSIVE)
						{
							// Extra weight for enemy territory in offensive areas
							iTempValue *= 2;
						}

						if (GET_PLAYER(getOwner()).AI_isPrimaryArea(plotX->area()))
						{
							iTempValue *= 3;
							iTempValue /= 2;
						}

						if (plotX->isNPC())
						{
							iTempValue /= 2;
						}
					}
					iValue += iTempValue;
				}
			}
		}

		// Consider available defense, direct threat to potential base
		const int iOurDefense = GET_TEAM(eTeam).AI_getOurPlotStrength(pPlot, 0, true, false, true);
		const int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(pPlot, 2, false, false);

		if (3 * iEnemyOffense > iOurDefense || iOurDefense == 0)
		{
			iValue *= iOurDefense;
			iValue /= std::max(1, 3 * iEnemyOffense);
		}

		// Value forts less, they are generally riskier bases
		if (pCity == NULL)
		{
			iValue *= 2;
			iValue /= 3;
		}
	}
	else
	{
		if (pPlot->getOwner() != getOwner())
		{
			// Keep planes at home when not in real wars
			return 0;
		}

		// If no wars, use prior logic with added value to keeping planes safe from sneak attack
		if (pCity != NULL)
		{
			iValue = 20 + pCity->getPopulation() + pCity->AI_cityThreat();
		}
		else if (iDefenders > 0)
		{
			iValue = (pCity != NULL) ? 0 : GET_PLAYER(getOwner()).AI_getPlotAirbaseValue(pPlot);
			iValue /= 6;
		}
		iValue += std::min(24, 3 * (iDefenders - iAttackAirCount));

		if (GET_PLAYER(getOwner()).AI_isPrimaryArea(baseArea))
		{
			iValue *= 4;
			iValue /= 3;
		}

		// No real enemies, check for minor civ or barbarian cities where attacks could be supported
		const CvCity* pNearestEnemyCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY(), NO_PLAYER, NO_TEAM, false, false, eTeam);

		if (pNearestEnemyCity != NULL)
		{
			const int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pNearestEnemyCity->getX(), pNearestEnemyCity->getY());
			if (iDistance > airRange())
			{
				iValue /= 10 * (2 + airRange());
			}
			else iValue /= 2 + iDistance;
		}
	}

	if (pPlot->getOwner() == getOwner())
	{
		// Bases in our territory better than teammate's
		iValue *= 2;
	}
	else if (pPlot->getTeam() == eTeam)
	{
		// Our team's bases are better than vassal plots
		iValue *= 3;
		iValue /= 2;
	}

	return iValue;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_airDefensiveCity()
{
	PROFILE_EXTRA_FUNC();
	//PROFILE_FUNC();

	FAssert(getDomainType() == DOMAIN_AIR);
	FAssert(canAirDefend());

	if (canAirDefend() && getDamage() == 0)
	{
		CvCity* pCity = plot()->getPlotCity();

		if (pCity != NULL && pCity->getOwner() == getOwner())
		{
			if (!pCity->AI_isAirDefended(false, 1))
			{
				// Stay if very short on planes, regardless of situation
				getGroup()->pushMission(MISSION_AIRPATROL);
				return true;
			}

			if (!pCity->AI_isAirDefended(true, -1))
			{
				// Stay if city is threatened but not seriously threatened
				const int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);

				if (iEnemyOffense > 0)
				{
					const int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);
					if (3 * iEnemyOffense < 4 * iOurDefense)
					{
						getGroup()->pushMission(MISSION_AIRPATROL);
						return true;
					}
				}
			}
		}
	}

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	foreach_(CvCity * cityX, GET_PLAYER(getOwner()).cities())
	{
		CvPlot* plotX = cityX->plot();
		if (canAirDefend(plotX) && (atPlot(plotX) || canEnterPlot(plotX)))
		{
			const int iExistingAirDefenders =
				(
					plotX->plotCount
					(
						PUF_canAirDefend, -1, -1, NULL,
						cityX->getOwner(), NO_TEAM,
						PUF_isDomainType, DOMAIN_AIR
					)
					// Subtract self if already there
					- atPlot(plotX)
				);
			const int iNeedAirDefenders = cityX->AI_neededAirDefenders();

			if (iNeedAirDefenders > iExistingAirDefenders)
			{
				int iValue = cityX->getPopulation() + cityX->AI_cityThreat();

				const int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plotX, 0, true, false, true);
				const int iEnemyOffense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plotX, 2, false, false);

				iValue *= 100;

				// Increase value of cities needing air defense more
				iValue *= std::max(1, 3 + iNeedAirDefenders - iExistingAirDefenders);

				if (GET_PLAYER(getOwner()).AI_isPrimaryArea(cityX->area()))
				{
					iValue *= 4;
					iValue /= 3;
				}

				// Reduce value of endangered city, it may be too late to help
				if (3 * iEnemyOffense > iOurDefense || iOurDefense == 0)
				{
					iValue *= iOurDefense;
					iValue /= std::max(1, 3 * iEnemyOffense);
				}

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = plotX;
				}
			}
		}
	}

	if (pBestPlot != NULL && !atPlot(pBestPlot))
	{
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_airCarrier()
{
	PROFILE_EXTRA_FUNC();
	//PROFILE_FUNC();

	if (hasCargo())
	{
		return false;
	}

	if (isCargo())
	{
		if (canAirDefend())
		{
			getGroup()->pushMission(MISSION_AIRPATROL);
			return true;
		}
		getGroup()->pushMission(MISSION_SKIP);
		return true;
	}

	int iBestValue = 0;
	CvUnit* pBestUnit = NULL;

	foreach_(CvUnit * unitX, GET_PLAYER(getOwner()).units())
	{
		if (canLoadOntoUnit(unitX, unitX->plot()))
		{
			int iValue = 10;

			if (!unitX->plot()->isCity())
			{
				iValue += 20;
			}

			if (!unitX->plot()->isOwned())
			{
				iValue += 10;
			}
			else if (isEnemy(unitX->plot()->getTeam(), unitX->plot()))
			{
				iValue += 20;
			}

			if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
			{
				iValue /= (unitX->SMgetCargo() + GC.getGame().getBaseAirUnitIncrementsbyCargoVolume()) / GC.getGame().getBaseAirUnitIncrementsbyCargoVolume();
			}
			else iValue /= unitX->getCargo() + 1;

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestUnit = unitX;
			}
		}
	}

	if (pBestUnit != NULL)
	{
		if (atPlot(pBestUnit->plot()))
		{
			setTransportUnit(pBestUnit); // XXX is this dangerous (not pushing a mission...) XXX air units?
			return true;
		}
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestUnit->getX(), pBestUnit->getY());
	}
	return false;
}

bool CvUnitAI::AI_missileLoad(UnitAITypes eTargetUnitAI, int iMaxOwnUnitAI, bool bStealthOnly)
{
	PROFILE_EXTRA_FUNC();
	//PROFILE_FUNC();

	CvUnit* pBestUnit = NULL;
	int iBestValue = 0;

	const bool bSM = GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS);
	const int iCargoValueFactorSM =
		(
			bSM ? GC.getGame().getBaseMissileUnitIncrementsbyCargoVolume() : 0
		);
	FAssert(!bSM || iCargoValueFactorSM > 0);

	foreach_(CvUnit * unitX, GET_PLAYER(getOwner()).units())
	{
		if ((!bStealthOnly || unitX->getInvisibleType() != NO_INVISIBLE)
		&& unitX->AI_getUnitAIType() == eTargetUnitAI
		&& (iMaxOwnUnitAI == -1 || unitX->getUnitAICargo(AI_getUnitAIType()) <= iMaxOwnUnitAI)
		&& canLoadOntoUnit(unitX, unitX->plot()))
		{
			const int iValue =
				(
					(100 + GC.getGame().getSorenRandNum(100, "AI missile load"))
					*
					(
						bSM
						?
						iCargoValueFactorSM + unitX->SMgetCargo() / iCargoValueFactorSM
						:
						1 + unitX->getCargo()
						)
				);
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestUnit = unitX;
			}
		}
	}

	if (pBestUnit != NULL)
	{
		if (atPlot(pBestUnit->plot()))
		{
			setTransportUnit(pBestUnit); // XXX is this dangerous (not pushing a mission...) XXX air units?
			return true;
		}
		if (getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestUnit->getX(), pBestUnit->getY()))
		{
			setTransportUnit(pBestUnit);
			return true;
		}
	}
	return false;

}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_airStrike()
{
	AirInterceptionCandidates interceptors(*this);
	PROFILE_EXTRA_FUNC();

	const int iSearchRange = airRange();

	int iBestValue = (isSuicide() && m_pUnitInfo->getProductionCost() > 0) ? m_pUnitInfo->getProductionCost() * 5 / 6 : 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * plotX, plot()->rect(iSearchRange, iSearchRange))
	{
		if (!canEnterPlot(plotX, MoveCheck::Attack))
		{
			continue;
		}
		const int iPotentialAttackers =
			(
				GET_PLAYER(getOwner()).AI_adjacentPotentialAttackers(plotX)
				+
				(
					plotX->isCity()
					?
					2 * GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(plotX, MISSIONAI_ASSAULT, getGroup(), 1)
					:
					0
					)
			);
		// Bombers will always consider striking units adjacent to this team's territory
		// to soften them up for potential attack.  This situation doesn't apply if this team's adjacent
		// territory is water, land units won't be able to reach easily for attack
		if (plotX->isWater() || iPotentialAttackers > 0 || plotX->isAdjacentTeam(getTeam(), true))
		{
			CvUnit* pDefender = plotX->getBestDefender(NO_PLAYER, getOwner(), this, true);

			if (!pDefender) continue;

			if (pDefender->canDefend())
			{
				const int iDamage = airCombatDamage(pDefender);

				int iValue =
					(
						std::max(0, std::min(pDefender->getDamage() + iDamage, airCombatLimit(pDefender)) - pDefender->getDamage())
						+
						(iDamage * collateralDamage() / 100)
						*
						std::min(plotX->getNumVisiblePotentialEnemyDefenders(this) - 1, collateralDamageMaxUnits())
						/
						2
					);
				iValue *= 3 + iPotentialAttackers;
				iValue /= 4;

				const CvUnit* unitX = interceptors.bestInterceptor(plotX);

				if (unitX)
				{
					iValue =
						(
							iValue
							*
							std::max
							(
								0,
								100
								-
								(isSuicide() ? 100 : unitX->currInterceptionProbability())
								*
								std::max(0, 100 - evasionProbability())
								/
								200
							)
							/
							100
						);
				}

				if (plotX->isWater())
				{
					iValue *= 3;
				}

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = plotX;
					FAssert(!atPlot(pBestPlot));
				}
			}
			else FErrorMsg("Best Defender is expected to be able to defend!");
		}
	}

	if (pBestPlot)
	{
		FAssert(!atPlot(pBestPlot));
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}
	return false;
}

/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						9/16/08			jdog5000		*/
/* 																			*/
/* 	Air AI																	*/
/********************************************************************************/
// Returns true if a mission was pushed...
// Air strike focused on weakening enemy stacks threatening our cities
bool CvUnitAI::AI_defensiveAirStrike()
{
	AirInterceptionCandidates interceptors(*this);
	PROFILE_FUNC();

	const int iSearchRange = airRange();

	int iBestValue = (isSuicide() && m_pUnitInfo->getProductionCost() > 0) ? (60 * m_pUnitInfo->getProductionCost()) : 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, plot()->rect(iSearchRange, iSearchRange))
	{
		if (canEnterPlot(pLoopPlot, MoveCheck::Attack)) // Only true of plots this unit can airstrike
		{
			// Only attack enemy land units near our cities
			if (pLoopPlot->isPlayerCityRadius(getOwner()) && !pLoopPlot->isWater())
			{
				const CvCity* pClosestCity = GC.getMap().findCity(pLoopPlot->getX(), pLoopPlot->getY(), getOwner(), getTeam(), true, false);

				if (pClosestCity != NULL)
				{
					// City and pLoopPlot forced to be in same area, check they're still close
					int iStepDist = plotDistance(pClosestCity->getX(), pClosestCity->getY(), pLoopPlot->getX(), pLoopPlot->getY());

					if (iStepDist < 3)
					{
						const CvUnit* pDefender = pLoopPlot->getBestDefender(NO_PLAYER, getOwner(), this, true);

						if (pDefender != NULL && pDefender->canDefend())
						{
							const int iDamage = airCombatDamage(pDefender);

							int iValue = std::max(0, (std::min((pDefender->getDamage() + iDamage), airCombatLimit(pDefender)) - pDefender->getDamage()));

							iValue += ((((iDamage * collateralDamage()) / 100) * std::min((pLoopPlot->getNumVisiblePotentialEnemyDefenders(this) - 1), collateralDamageMaxUnits())) / 2);

							iValue *= GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(pClosestCity->plot(), 2, false, false);
							iValue /= std::max(1, GET_TEAM(getTeam()).AI_getOurPlotStrength(pClosestCity->plot(), 0, true, false, true));

							if (iStepDist == 1)
							{
								iValue *= 5;
								iValue /= 4;
							}

							const CvUnit* pInterceptor = interceptors.bestInterceptor(pLoopPlot);

							if (pInterceptor != NULL)
							{
								int iInterceptProb = isSuicide() ? 100 : pInterceptor->currInterceptionProbability();

								iInterceptProb *= std::max(0, (100 - evasionProbability()));
								iInterceptProb /= 100;

								iValue *= std::max(0, 100 - iInterceptProb / 2);
								iValue /= 100;
							}

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = pLoopPlot;
								FAssert(!atPlot(pBestPlot));
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}

	return false;
}

// Air strike around base city
// Returns true if a mission was pushed...
bool CvUnitAI::AI_defendBaseAirStrike()
{
	AirInterceptionCandidates interceptors(*this);
	PROFILE_FUNC();

	// Only search around base
	int iSearchRange = 2;

	int iBestValue = (isSuicide() && m_pUnitInfo->getProductionCost() > 0) ? (15 * m_pUnitInfo->getProductionCost()) : 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, plot()->rect(iSearchRange, iSearchRange))
	{
		if (canEnterPlot(pLoopPlot, MoveCheck::Attack) && !pLoopPlot->isWater()) // Only true of plots this unit can airstrike
		{
			if (plot()->area() == pLoopPlot->area())
			{
				const CvUnit* pDefender = pLoopPlot->getBestDefender(NO_PLAYER, getOwner(), this, true);

				FAssert(pDefender != NULL && pDefender->canDefend());

				if (pDefender != NULL && pDefender->canDefend())
				{

					const int iDamage = airCombatDamage(pDefender);

					int iValue = std::max(0, (std::min((pDefender->getDamage() + iDamage), airCombatLimit(pDefender)) - pDefender->getDamage()));

					iValue += ((iDamage * collateralDamage()) * std::min((pLoopPlot->getNumVisiblePotentialEnemyDefenders(this) - 1), collateralDamageMaxUnits())) / (2 * 100);

					// Weight towards stronger units
					iValue *= (pDefender->currCombatStr(NULL, NULL, NULL) + 2000);
					iValue /= 2000;

					// Weight towards adjacent stacks
					if (plotDistance(getX(), getY(), pLoopPlot->getX(), pLoopPlot->getY()) == 1)
					{
						iValue *= 5;
						iValue /= 4;
					}

					const CvUnit* pInterceptor = interceptors.bestInterceptor(pLoopPlot);

					if (pInterceptor != NULL)
					{
						int iInterceptProb = isSuicide() ? 100 : pInterceptor->currInterceptionProbability();

						iInterceptProb *= std::max(0, (100 - evasionProbability()));
						iInterceptProb /= 100;

						iValue *= std::max(0, 100 - iInterceptProb / 2);
						iValue /= 100;
					}

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = pLoopPlot;
						FAssert(!atPlot(pBestPlot));
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}

	return false;
}
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						END								*/
/********************************************************************************/

bool CvUnitAI::AI_airBombPlots()
{
	AirInterceptionCandidates interceptors(*this);
	PROFILE_EXTRA_FUNC();
	//PROFILE_FUNC();

	const int iSearchRange = airRange();

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, plot()->rect(iSearchRange, iSearchRange))
	{
		if (!pLoopPlot->isCity() && pLoopPlot->isOwned() && pLoopPlot != plot())
		{
			if (canAirBombAt(plot(), pLoopPlot->getX(), pLoopPlot->getY()))
			{
				int iValue = 0;

				if (pLoopPlot->getBonusType(pLoopPlot->getTeam()) != NO_BONUS)
				{
					iValue += AI_pillageValue(pLoopPlot, 15);

					iValue += GC.getGame().getSorenRandNum(10, "AI Air Bomb");
				}
				else if (isSuicide())
				{
					//This should only be reached when the unit is desperate to die
					iValue += AI_pillageValue(pLoopPlot);
					// Guided missiles lean towards destroying resource-producing tiles as opposed to improvements like Towns
					if (pLoopPlot->getBonusType(pLoopPlot->getTeam()) != NO_BONUS)
					{
						//and even more so if it's a resource
						iValue += GET_PLAYER(pLoopPlot->getOwner()).AI_bonusVal(pLoopPlot->getBonusType(pLoopPlot->getTeam()));
					}
				}

				if (iValue > 0)
				{
					const CvUnit* pInterceptor = interceptors.bestInterceptor(pLoopPlot);

					if (pInterceptor != NULL)
					{
						int iInterceptProb = isSuicide() ? 100 : pInterceptor->currInterceptionProbability();

						iInterceptProb *= std::max(0, (100 - evasionProbability()));
						iInterceptProb /= 100;

						iValue *= std::max(0, 100 - iInterceptProb / 2);
						iValue /= 100;
					}

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = pLoopPlot;
						FAssert(!atPlot(pBestPlot));
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_AIRBOMB, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}

	return false;
}


bool CvUnitAI::AI_airBombDefenses()
{
	AirInterceptionCandidates interceptors(*this);
	PROFILE_EXTRA_FUNC();
	//PROFILE_FUNC();

	const int iSearchRange = airRange();

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, plot()->rect(iSearchRange, iSearchRange))
	{
		const CvCity* pCity = pLoopPlot->getPlotCity();
		if (pCity != NULL)
		{
			int iValue = 0;

			if (canAirBombAt(plot(), pLoopPlot->getX(), pLoopPlot->getY()))
			{
				int iPotentialAttackers = GET_PLAYER(getOwner()).AI_adjacentPotentialAttackers(pLoopPlot);
				iPotentialAttackers += std::max(0, GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pCity->plot(), NO_MISSIONAI, getGroup(), 2) - 4);

				if (iPotentialAttackers > 1)
				{
					iValue += std::max(0, (std::min((pCity->getDefenseDamage() + getAirBombCurrRate()), GC.getMAX_CITY_DEFENSE_DAMAGE()) - pCity->getDefenseDamage()));

					iValue *= 4 + iPotentialAttackers;

					if (pCity->AI_isDanger())
					{
						iValue *= 2;
					}

					if (pCity == pCity->area()->getTargetCity(getOwner()))
					{
						iValue *= 2;
					}
				}

				if (iValue > 0)
				{
					const CvUnit* pInterceptor = interceptors.bestInterceptor(pLoopPlot);

					if (pInterceptor != NULL)
					{
						int iInterceptProb = isSuicide() ? 100 : pInterceptor->currInterceptionProbability();

						iInterceptProb *= std::max(0, (100 - evasionProbability()));
						iInterceptProb /= 100;

						iValue *= std::max(0, 100 - iInterceptProb / 2);
						iValue /= 100;
					}

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = pLoopPlot;
						FAssert(!atPlot(pBestPlot));
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_AIRBOMB, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}

	return false;

}

bool CvUnitAI::AI_exploreAir()
{
	PROFILE_FUNC();

	const CvPlayer& player = GET_PLAYER(getOwner());
	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getTeam(), false))
		{
			const bool bEnemy = isEnemy(GET_PLAYER((PlayerTypes)iI).getTeam());

			foreach_(const CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
			{
				if (!pLoopCity->isVisible(getTeam(), false)
				&& canReconAt(plot(), pLoopCity->getX(), pLoopCity->getY()))
				{
					int iValue = 1 + GC.getGame().getSorenRandNum(15, "AI explore air");
					if (bEnemy)
					{
						iValue += 10;
						iValue += std::min(10, pLoopCity->area()->getNumAIUnits(getOwner(), UNITAI_ATTACK_CITY));
						iValue += 10 * player.AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_ASSAULT);
					}
					iValue *= plotDistance(getX(), getY(), pLoopCity->getX(), pLoopCity->getY());

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = pLoopCity->plot();
					}
				}
			}
		}
	}
	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_RECON, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}
	return false;
}


int CvUnitAI::AI_exploreAirPlotValue(const CvPlot* pPlot) const
{
	int iValue = 0;
	if (pPlot->isVisible(getTeam(), false))
	{
		iValue++;

		if (!pPlot->isOwned())
		{
			iValue++;
		}

		if (!pPlot->isImpassable(getTeam())) // added getTeam()
		{
			iValue *= 4;

			if (pPlot->isWater() || pPlot->getArea() == getArea())
			{
				iValue *= 2;
			}
		}
	}

	return iValue;
}

bool CvUnitAI::AI_exploreAir2()
{
	PROFILE_FUNC();

	const CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	const int iSearchRange = airRange();
	foreach_(const CvPlot * pLoopPlot, plot()->rect(iSearchRange, iSearchRange)
	| filtered(!CvPlot::fn::isVisible(getTeam(), false)))
	{
		if (canReconAt(plot(), pLoopPlot->getX(), pLoopPlot->getY()))
		{
			int iValue = AI_exploreAirPlotValue(pLoopPlot);

			foreach_(const CvPlot * pAdjacentPlot, plot()->adjacent()
			| filtered(CvPlot::fn::isVisible(getTeam(), false)))
			{
				iValue += AI_exploreAirPlotValue(pAdjacentPlot);
			}

			iValue += GC.getGame().getSorenRandNum(25, "AI explore air");
			iValue *= std::min(7, plotDistance(getX(), getY(), pLoopPlot->getX(), pLoopPlot->getY()));

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestPlot = pLoopPlot;
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_RECON, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}

	return false;
}

void CvUnitAI::AI_exploreAirMove()
{
	if (AI_exploreAir() || AI_exploreAir2())
	{
		return;
	}
	if (canAirDefend())
	{
		getGroup()->pushMission(MISSION_AIRPATROL);
		return;
	}
	getGroup()->pushMission(MISSION_SKIP);

	return;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_nuke()
{
	PROFILE_FUNC();

	const CvCity* pBestCity = NULL;
	int iBestValue = 0;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && isEnemy(GET_PLAYER((PlayerTypes)iI).getTeam())
		&& GET_PLAYER(getOwner()).AI_getAttitude((PlayerTypes)iI) == ATTITUDE_FURIOUS)
		{
			foreach_(const CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
			{
				if (canNukeAt(plot(), pLoopCity->getX(), pLoopCity->getY()))
				{
					const int iValue = AI_nukeValue(pLoopCity);

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestCity = pLoopCity;
						FAssert(pBestCity->getTeam() != getTeam());
					}
				}
			}
		}
	}
	if (pBestCity != NULL)
	{
		getGroup()->pushMission(MISSION_NUKE, pBestCity->getX(), pBestCity->getY());
		return true;
	}
	return false;
}

bool CvUnitAI::AI_nukeRange(int iRange)
{
	PROFILE_EXTRA_FUNC();
	const CvPlot* pBestPlot = NULL;
	int iBestValue = 0;
	foreach_(const CvPlot * pLoopPlot, plot()->rect(iRange, iRange))
	{
		if (canNukeAt(plot(), pLoopPlot->getX(), pLoopPlot->getY()))
		{
			int iValue = -99;

			foreach_(const CvPlot * pLoopPlot2, pLoopPlot->rect(nukeRange(), nukeRange()))
			{
				int iEnemyCount = 0;
				int iTeamCount = 0;
				int iNeutralCount = 0;
				int iDamagedEnemyCount = 0;

				foreach_(const CvUnit * pLoopUnit, pLoopPlot2->units()
				| filtered(!CvUnit::fn::isNukeImmune()))
				{
					if (pLoopUnit->getTeam() == getTeam())
					{
						iTeamCount++;
					}
					else if (!pLoopUnit->isInvisible(getTeam(), false))
					{
						if (isEnemy(pLoopUnit->getTeam()))
						{
							iEnemyCount++;
							if (pLoopUnit->getDamage() * 2 > pLoopUnit->getMaxHP())
							{
								iDamagedEnemyCount++;
							}
						}
						else
						{
							iNeutralCount++;
						}
					}
				}

				iValue += (iEnemyCount + iDamagedEnemyCount) * (pLoopPlot2->isWater() ? 25 : 12);
				iValue -= iTeamCount * 15;
				iValue -= iNeutralCount * 20;


				int iMultiplier = 1;
				if (pLoopPlot2->getTeam() == getTeam())
				{
					iMultiplier = -2;
				}
				else if (isEnemy(pLoopPlot2->getTeam()))
				{
					iMultiplier = 1;
				}
				else if (!pLoopPlot2->isOwned())
				{
					iMultiplier = 0;
				}
				else
				{
					iMultiplier = -10;
				}

				if (pLoopPlot2->getImprovementType() != NO_IMPROVEMENT)
				{
					iValue += iMultiplier * 10;
				}

				if (pLoopPlot2->getNonObsoleteBonusType(getTeam()) != NO_BONUS)
				{
					iValue += iMultiplier * 20;
				}

				if (pLoopPlot2->isCity())
				{
					iValue += std::max(0, iMultiplier * (-20 + 15 * pLoopPlot2->getPlotCity()->getPopulation()));
				}
			}

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestPlot = pLoopPlot;
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_NUKE, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}

	return false;
}

bool CvUnitAI::AI_trade(int iValueThreshold)
{
	PROFILE_EXTRA_FUNC();
	int iPathTurns;
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getTeam(), false))
		{
			foreach_(const CvCity * cityX, GET_PLAYER((PlayerTypes)iI).cities())
			{
				if (AI_plotValid(cityX->plot()))
				{
					int iValue = getTradeGold(cityX->plot());

					if (iValue >= iValueThreshold && canTrade(cityX->plot(), true)
					&& generateIgnoreDangerPath(cityX->plot(), &iPathTurns))
					{
						iValue /= 4 + iPathTurns;

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = cityX->plot();
						}
					}
				}
			}
		}
	}

	if (pBestPlot)
	{
		if (atPlot(pBestPlot))
		{
			return getGroup()->pushMissionInternal(MISSION_TRADE, pBestPlot->getX(), pBestPlot->getY());
		}
		if (generateSafePathforVulnerable(pBestPlot))
		{
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_WITH_CAUTION);
		}
		getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
		return true;
	}
	return false;
}


bool CvUnitAI::AI_infiltrate()
{
	PROFILE_FUNC();

	if (canInfiltrate(plot()))
	{
		getGroup()->pushMission(MISSION_INFILTRATE);
		return true;
	}
	if (canInfiltrate())
	{
		CvPlot* pBestPlot = NULL;
		int iBestValue = 0;
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getTeam(), false))
			{
				foreach_(const CvCity * cityX, GET_PLAYER((PlayerTypes)iI).cities())
				{
					if (isInvisible(cityX->getTeam(), false, true))
					{
						// BBAI efficiency: check area for land units before generating path
						if (getDomainType() == DOMAIN_LAND && !UnitTravelConnectivity::landUnitCanReach(this,cityX->plot(),true) && !getGroup()->canMoveAllTerrain())
						{
							continue;
						}
						int iValue = getEspionagePoints();
						int iPathTurns;

						if (iValue > iBestValue && generatePath(cityX->plot(), 0, true, &iPathTurns))
						{
							FAssert(iPathTurns > 0);

							if (getPathMovementRemaining() == 0)
							{
								iPathTurns++;
							}
							iValue /= 1 + iPathTurns;

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = cityX->plot();
							}
						}
					}
				}
			}
		}
		if (pBestPlot != NULL)
		{
			if (atPlot(pBestPlot))
			{
				getGroup()->pushMission(MISSION_INFILTRATE);
				return true;
			}
			if (getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY()))
			{
				getGroup()->pushMission(MISSION_INFILTRATE, -1, -1, 0, (getGroup()->getLengthMissionQueue() > 0));
				return true;
			}
		}
	}
	return false;
}


bool CvUnitAI::AI_reconSpy(int iRange)
{
	PROFILE_FUNC();

	const CvPlot* pBestPlot = NULL;
	CvPlot* pBestTargetPlot = NULL;
	int iBestValue = 0;

	int iSearchRange = AI_searchRange(iRange);

	for (int iX = -iSearchRange; iX <= iSearchRange; iX++)
	{
		for (int iY = -iSearchRange; iY <= iSearchRange; iY++)
		{
			const CvPlot* pLoopPlot = plotXY(getX(), getY(), iX, iY);
			int iDistance = stepDistance(0, 0, iX, iY);
			if ((iDistance > 0) && (pLoopPlot != NULL) && AI_plotValid(pLoopPlot))
			{
				int iValue = 0;
				if (pLoopPlot->getPlotCity() != NULL)
				{
					iValue += GC.getGame().getSorenRandNum(4000, "AI Spy Scout City");
				}

				if (pLoopPlot->getBonusType(getTeam()) != NO_BONUS)
				{
					iValue += GC.getGame().getSorenRandNum(1000, "AI Spy Recon Bonus");
				}

				foreach_(const CvPlot * pAdjacentPlot, pLoopPlot->adjacent())
				{
					if (!pAdjacentPlot->isRevealed(getTeam(), false))
					{
						iValue += 500;
					}
					else if (!pAdjacentPlot->isVisible(getTeam(), false))
					{
						iValue += 200;
					}
				}


				if (iValue > 0)
				{
					int iPathTurns;
					if (generatePath(pLoopPlot, 0, true, &iPathTurns, iRange))
					{
						if (iPathTurns <= iRange)
						{
							// don't give each and every plot in range a value before generating the patch (performance hit)
							iValue += GC.getGame().getSorenRandNum(250, "AI Spy Scout Best Plot");

							iValue *= iDistance;

							/* Can no longer perform missions after having moved
							if (getPathLastNode()->m_iData2 == 1)
							{
								if (getPathLastNode()->m_iData1 > 0)
								{
									//Prefer to move and have movement remaining to perform a kill action.
									iValue *= 2;
								}
							} */

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestTargetPlot = getPathEndTurnPlot();
								pBestPlot = pLoopPlot;
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestTargetPlot != NULL))
	{
		if (atPlot(pBestTargetPlot))
		{
			getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
		else
		{
			if (getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestTargetPlot->getX(), pBestTargetPlot->getY()))
			{
				getGroup()->pushMission(MISSION_SKIP);
				return true;
			}
		}
	}

	return false;
}

/// \brief Spy decision on whether to cause revolt in besieged city
///
/// Have spy breakdown city defenses if we have troops in position to capture city this turn.
bool CvUnitAI::AI_revoltCitySpy()
{
	PROFILE_FUNC();

	CvCity* pCity = plot()->getPlotCity();


	if (pCity == NULL)
	{
		FErrorMsg("City is not expected to be NULL");
		return false;
	}

	if (!(GET_TEAM(getTeam()).isAtWar(pCity->getTeam())))
	{
		return false;
	}

	if (pCity->isDisorder())
	{
		return false;
	}

	int iOurPower = GET_PLAYER(getOwner()).AI_getOurPlotStrength(plot(), 1, false, true);
	int iEnemyDefensePower = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 0, true, false);
	int iEnemyPostPower = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 0, false, false);

	if (iOurPower > 2 * iEnemyDefensePower)
	{
		return false;
	}

	if (iOurPower < iEnemyPostPower)
	{
		return false;
	}

	if (10 * iEnemyDefensePower < 11 * iEnemyPostPower)
	{
		return false;
	}

	for (int iMission = 0; iMission < GC.getNumEspionageMissionInfos(); ++iMission)
	{
		const CvEspionageMissionInfo& kMissionInfo = GC.getEspionageMissionInfo((EspionageMissionTypes)iMission);
		if ((kMissionInfo.getCityRevoltCounter() > 0) || (kMissionInfo.getPlayerAnarchyCounter() > 0))
		{
			if (!GET_PLAYER(getOwner()).canDoEspionageMission((EspionageMissionTypes)iMission, pCity->getOwner(), pCity->plot(), -1, this))
			{
				continue;
			}

			if (!espionage((EspionageMissionTypes)iMission, -1))
			{
				continue;
			}

			return true;
		}
	}

	return false;
}

int CvUnitAI::AI_getEspionageTargetValue(const CvPlot* pPlot, int iMaxPath, int iMinUsefulValue) const
{
	PROFILE_FUNC();

	const CvTeamAI& kTeam = GET_TEAM(getTeam());
	int iValue = 0;

	if (pPlot->isOwned() && pPlot->getTeam() != getTeam() && !GET_TEAM(getTeam()).isVassal(pPlot->getTeam()))
	{
		if (AI_plotValid(pPlot))
		{
			const CvCity* pCity = pPlot->getPlotCity();
			if (pCity != NULL)
			{
				iValue += pCity->getPopulation();
				iValue += pCity->plot()->calculateCulturePercent(getOwner()) / 8;

				// BBAI TODO: Should go to cities where missions will be cheaper ...

				const int iRand = GC.getGame().getSorenRandNum(6, "AI spy choose city");
				iValue += iRand * iRand;

				if (area()->getTargetCity(getOwner()) == pCity)
				{
					iValue += 30;
				}

				if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pPlot, MISSIONAI_ASSAULT, getGroup()) > 0)
				{
					iValue += 30;
				}

				// BBAI TODO: What else?  If can see production, go for wonders and space race ...
			}
			else
			{
				const BonusTypes eBonus = pPlot->getNonObsoleteBonusType(getTeam());
				if (eBonus != NO_BONUS)
				{
					iValue += GET_PLAYER(pPlot->getOwner()).AI_baseBonusVal(eBonus) - 10;
				}
			}

			int iPathableValue = iValue;
			if (kTeam.AI_getWarPlan(pPlot->getTeam()) != NO_WARPLAN)
			{
				if (kTeam.AI_isSneakAttackPreparing(pPlot->getTeam()))
				{
					iPathableValue *= (pPlot->isCity()) ? 15 : 10;
				}
				else
				{
					iPathableValue *= 3;
				}
			}

			iPathableValue *= 3;
			iPathableValue /= (3 + GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pPlot, MISSIONAI_ATTACK_SPY, getGroup()));

			int iPathTurns;
			if (iPathableValue > iMinUsefulValue && generatePath(pPlot, 0, true, &iPathTurns, iMaxPath))
			{
				iValue = iPathableValue;
			}
			else
			{
				iValue = 0;
			}
		}
	}

	return (iValue > iMinUsefulValue) ? iValue : 0;
}


bool CvUnitAI::AI_cityOffenseSpy(const int iMaxPath, const CvCity* pSkipCity)
{
	PROFILE_FUNC();

	const TeamTypes eTeam = getTeam();
	const CvTeamAI& team = GET_TEAM(eTeam);
	const CvPlayerAI& player = GET_PLAYER(getOwner());
	const CvArea* pArea = area();

	const bool bValid = team.getBestKnownTechScorePercent() < 85;

	// Every candidate already receives an exact, iMaxPath-bounded path test.
	// Reuse a complete reachable view if present; do not flood the whole map
	// merely as a preflight for a small number of city destinations.
	CvReachablePlotSet plotSet(getGroup(), 0);
	const bool hasCachedReachable = plotSet.tryPopulateFromCache(MAX_INT);
	const CvPlot* pBestPlot = NULL;
	int iBestValue = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; ++iI)
	{
		const CvPlayer& playerX = GET_PLAYER((PlayerTypes)iI);

		if (playerX.isAliveAndTeam(eTeam, false) && !team.isVassal(playerX.getTeam()))
		{
			// Only move to cities where we will run missions
			if (bValid || team.AI_getWarPlan(playerX.getTeam()) != NO_WARPLAN
			|| player.AI_getAttitudeWeight((PlayerTypes)iI) < 51)
			{
				foreach_(const CvCity * cityX, playerX.cities())
				{
					if (cityX == pSkipCity)
					{
						continue;
					}
					if (cityX->area() == pArea || canMoveAllTerrain())
					{
						CvPlot* plotX = cityX->plot();

						if (!hasCachedReachable || plotSet.find(plotX) != plotSet.end())
						{
							const int iValue = AI_getEspionageTargetValue(plotX, iMaxPath, iBestValue);
							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = plotX;
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (atPlot(pBestPlot))
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_ATTACK_SPY);
			return true;
		}
		else if (getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_ATTACK_SPY))
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_ATTACK_SPY);
			return true;
		}
	}
	return false;
}

bool CvUnitAI::AI_bonusOffenseSpy(int iRange)
{
	PROFILE_FUNC();

	const CvPlot* pBestPlot = NULL;
	int iBestValue = 10;

	const int iSearchRange = AI_searchRange(iRange);

	foreach_(const CvPlot * pLoopPlot, plot()->rect(iSearchRange, iSearchRange))
	{
		if (pLoopPlot->getBonusType(getTeam()) != NO_BONUS
		&& pLoopPlot->isOwned() && pLoopPlot->getTeam() != getTeam())
		{
			// Only move to plots where we will run missions
			if (GET_PLAYER(getOwner()).AI_getAttitudeWeight(pLoopPlot->getOwner()) < 51
			|| GET_TEAM(getTeam()).AI_getWarPlan(pLoopPlot->getTeam()) != NO_WARPLAN)
			{
				const int iValue = AI_getEspionageTargetValue(pLoopPlot, iRange, iBestValue);
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = pLoopPlot;
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (atPlot(pBestPlot))
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_ATTACK_SPY);
			return true;
		}
		else if (getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_ATTACK_SPY))
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_ATTACK_SPY);
			return true;
		}
	}

	return false;
}

//Returns true if the spy performs espionage.
bool CvUnitAI::AI_espionageSpy()
{
	PROFILE_FUNC();

	if (!canEspionage(plot()))
	{
		return false;
	}

	CvPlot* pTargetPlot = NULL;
	PlayerTypes eTargetPlayer = NO_PLAYER;
	int iExtraData = -1;

	EspionageMissionTypes eBestMission = GET_PLAYER(getOwner()).AI_bestPlotEspionage(plot(), eTargetPlayer, pTargetPlot, iExtraData);
	if (NO_ESPIONAGEMISSION == eBestMission)
	{
		return false;
	}

	if (!GET_PLAYER(getOwner()).canDoEspionageMission(eBestMission, eTargetPlayer, pTargetPlot, iExtraData, this))
	{
		return false;
	}

	if (!espionage(eBestMission, iExtraData))
	{
		return false;
	}

	return true;
}

bool CvUnitAI::AI_moveToStagingCity()
{
	PROFILE_FUNC();

	CvCity* pBestCity = NULL;
	CvPlot* endTurnPlot = NULL;

	int iPathTurns;
	int iValue;

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	int iWarCount = 0;
	TeamTypes eTargetTeam = NO_TEAM;
	CvTeam& kTeam = GET_TEAM(getTeam());
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != getTeam() && GET_TEAM((TeamTypes)iI).isAlive() && kTeam.AI_isSneakAttackPreparing((TeamTypes)iI))
		{
			//OutputDebugString(CvString::format("AI_moveToStagingCity - potential target team %d\n", iI).c_str());
			eTargetTeam = (TeamTypes)iI;
			iWarCount++;
		}
	}
	if (iWarCount > 1)
	{
		PROFILE("CvUnitAI::AI_moveToStagingCity.MultiTargetTeams");

		//OutputDebugString("AI_moveToStagingCity - no target team\n");
		eTargetTeam = NO_TEAM;
	}

	CvReachablePlotSet plotSet(getGroup(), 0);
    std::vector<CvPlot*> targets;
    foreach_(CvCity* city, GET_PLAYER(getOwner()).cities())
        if (UnitTravelConnectivity::landUnitCanReach(this,city->plot(),true)) targets.push_back(city->plot());
    plotSet.PopulateForTargets(targets);


	//	Do a rough pathing to arrange the cities into a heuristically optimal order so
	//	that the detailed pathing is most likely to get the best cities first, and hence generate lower
	//	cutoffs for max path on later ones (and hence execute faster)
	std::multimap<int, std::pair<CvCity*, int> > orderedTargets;
	foreach_(CvCity * pLoopCity, GET_PLAYER(getOwner()).cities())
	{
		PROFILE("AI_moveToStagingCity.Valuation");

		// BBAI efficiency: check same area
		if (UnitTravelConnectivity::landUnitCanReach(this,pLoopCity->plot(),true))
		{
			CvReachablePlotSet::const_iterator itr = plotSet.find(pLoopCity->plot());

			if (itr != plotSet.end())
			{
				//	If we're not at war with anyone and we are not inside our own territory
				//	then we're just trying to return home - consider all cities equal
				if (iWarCount == 0 && plot()->getOwner() != getOwner())
				{
					iValue = 100;
				}
				else
				{
					// BBAI TODO: Need some knowledge of whether this is a good city to attack from ... only get that
					// indirectly from threat.
					iValue = pLoopCity->AI_cityThreat(eTargetTeam);
				}

				// Have attack stacks in assault areas move to coastal cities for faster loading
				if ((area()->getAreaAIType(getTeam()) == AREAAI_ASSAULT) || (area()->getAreaAIType(getTeam()) == AREAAI_ASSAULT_MASSING))
				{
					CvArea* pWaterArea = pLoopCity->waterArea();
					if (pWaterArea != NULL && GET_TEAM(getTeam()).AI_isWaterAreaRelevant(pWaterArea))
					{
						// BBAI TODO:  Need a better way to determine which cities should serve as invasion launch locations

						// Inertia so units don't just chase transports around the map
						iValue = iValue / 2;
						if (pLoopCity->area()->getAreaAIType(getTeam()) == AREAAI_ASSAULT)
						{
							// If in assault, transports may be at sea ... tend to stay where they left from
							// to speed reinforcement
							iValue += pLoopCity->plot()->plotCount(PUF_isAvailableUnitAITypeGroupie, UNITAI_ATTACK_CITY, -1, NULL, getOwner());
						}

						// Attraction to cities which are serving as launch/pickup points
						iValue += 3 * pLoopCity->plot()->plotCount(PUF_isUnitAIType, UNITAI_ASSAULT_SEA, -1, NULL, getOwner());
						iValue += 2 * pLoopCity->plot()->plotCount(PUF_isUnitAIType, UNITAI_ESCORT_SEA, -1, NULL, getOwner());
						if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
						{
							iValue += 5 * GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_PICKUP);
						}
						else
						{
							iValue += 5 * (GET_PLAYER(getOwner()).AI_plotTargetMissionAIsinCargoVolume(pLoopCity->plot(), MISSIONAI_PICKUP) / 100);
						}
					}
					else
					{
						iValue = iValue / 8;
					}
				}

				if ((pLoopCity->plot() != plot()) && pLoopCity->isVisible(eTargetTeam, false))
				{
					iValue /= 2;
				}

				iValue *= 1000;

				//	Do a first-stage cut-off even on the rough paths to eliminate particualrly unlikely
				//	candidates.  The +4 in this calculation (lack of -4 relative to the vrsion in the final
				//	detailed evaluation) is to allow a little leeway since the optimal
				//	paths may vary somewhat from the rough ones.  The 2* is because the plotSet step distance
				//	is route-blind
				int iMaxTurns = (iBestValue == 0 ? MAX_INT : 2 * iValue / iBestValue);

				//OutputDebugString(CvString::format("Generate rough path to (%d,%d) [value %d] with maxTurns=%d\n", pLoopCity->getX(), pLoopCity->getY(), iValue, iMaxTurns).c_str());
				//if ( generatePath(pLoopCity->plot(), MOVE_IGNORE_DANGER | MOVE_THROUGH_ENEMY, true, &iPathTurns, iMaxTurns, 0) )
				if (itr.stepDistance() <= iMaxTurns)
				{
					iPathTurns = itr.stepDistance();

					//OutputDebugString(CvString::format("\tSuccess.  PathTurns=%d\n", iPathTurns).c_str());
					int iPriority = iValue / (5 + iPathTurns);

					if (iPriority > iBestValue)
					{
						OutputDebugString(CvString::format("\tNew best value: %d\n", iPriority).c_str());
						iBestValue = iPriority;
					}

					orderedTargets.insert(std::make_pair(iPriority, std::make_pair(pLoopCity, iValue)));
				}
				//else
				//{
				//	OutputDebugString("\tNot pathable\n");
				//}

				//OutputDebugString(CvString::format("City @(%d,%d), distance %d, value %d - add at priority %d\n",pLoopCity->getX(),pLoopCity->getY(),iDistance,iValue,iPriority).c_str());
			}
		}
	}

	iBestValue = 0;

	for (std::multimap<int, std::pair<CvCity*, int> >::const_iterator itr = orderedTargets.begin(); itr != orderedTargets.end(); ++itr)
	{
		CvCity* pLoopCity = itr->second.first;
		iValue = itr->second.second;

		//	For barbarians we can assume they won't be using routes so we can make
		//	a decent estimate of their minimal possible path length
		int iMinPath;

		if (isNPC())
		{
			iMinPath = std::max(1, stepDistance(m_iX, m_iY, pLoopCity->getX(), pLoopCity->getY()) / baseMoves());
		}
		else
		{
			iMinPath = 1;
		}

		//OutputDebugString(CvString::format("City (%d,%d), value=%d, minPath=%d\n", pLoopCity->getX(), pLoopCity->getY(), iValue).c_str());
		if (iValue / (5 + iMinPath) > iBestValue)
		{
			PROFILE("CvUnitAI::AI_moveToStagingCity.Pathing");

			int iMaxTurns = (iBestValue == 0 ? MAX_INT : iValue / iBestValue - 4);

			//OutputDebugString(CvString::format("Attempt pathing with maxPath=%d\n", iMaxTurns).c_str());
			if (generatePath(pLoopCity->plot(), MOVE_IGNORE_DANGER, true, &iPathTurns, iMaxTurns))
			{
				PROFILE("CvUnitAI::AI_moveToStagingCity.DangerCheck");
				OutputDebugString(CvString::format("Pathing suceeded with path len %d\n", iPathTurns).c_str());

				iValue /= (5 + iPathTurns);

				if (iValue > iBestValue)
				{
					endTurnPlot = getPathEndTurnPlot();

					if (endTurnPlot == pLoopCity->plot() || !exposedToDanger(endTurnPlot, 60))
					{
						iBestValue = iValue;
						pBestPlot = endTurnPlot;
						pBestCity = pLoopCity;
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (atPlot(pBestPlot))
		{
			getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_GROUP, pBestCity->plot());
	}
	return false;
}

/*
bool CvUnitAI::AI_seaRetreatFromCityDanger()
{
	if (plot()->isCity(true) && GET_PLAYER(getOwner()).AI_getPlotDanger(plot(), 2) > 0) //prioritize getting outta there
	{
		if (AI_anyAttack(2, 40))
		{
			return true;
		}

		if (AI_anyAttack(4, 50))
		{
			return true;
		}

		if (AI_retreatToCity())
		{
			return true;
		}

		if (AI_safety())
		{
			return true;
		}
	}
	return false;
}

bool CvUnitAI::AI_airRetreatFromCityDanger()
{
	if (plot()->isCity(true))
	{
		CvCity* pCity = plot()->getPlotCity();
		if (GET_PLAYER(getOwner()).AI_getPlotDanger(plot(), 2) > 0 || (pCity != NULL && !pCity->AI_isDefended()))
		{
			if (AI_airOffensiveCity())
			{
				return true;
			}

			if (canAirDefend() && AI_airDefensiveCity())
			{
				return true;
			}
		}
	}
	return false;
}

bool CvUnitAI::AI_airAttackDamagedSkip()
{
	if (getDamage() == 0)
	{
		return false;
	}

	bool bSkip = (getHP() * 100 / getMaxHP() < 40);
	if (!bSkip)
	{
		int iSearchRange = airRange();
		bool bSkiesClear = true;
		for (int iDX = -iSearchRange; iDX <= iSearchRange && bSkiesClear; iDX++)
		{
			for (int iDY = -iSearchRange; iDY <= iSearchRange && bSkiesClear; iDY++)
			{
				CvPlot* pLoopPlot = plotXY(getX(), getY(), iDX, iDY);
				if (pLoopPlot != NULL)
				{
					if (bestInterceptor(pLoopPlot) != NULL)
					{
						bSkiesClear = false;
						break;
					}
				}
			}
		}
		bSkip = !bSkiesClear;
	}

	if (bSkip)
	{
		getGroup()->pushMission(MISSION_SKIP);
		return true;
	}

	return false;
}
*/

// Returns true if a mission was pushed or we should wait for another unit to bombard...
bool CvUnitAI::AI_followBombard()
{
	PROFILE_EXTRA_FUNC();
	/************************************************************************************************/
	/* REVOLUTIONDCM							05/24/08								Glider1	 */
	/*																							  */
	/*																							  */
	/************************************************************************************************/

	if (getGroup()->canBombard(plot()))
	{
		// RevolutionDCM - ranged bombard AI wraps standard bombard
		// TBHERE : This is opposite.  I like the solution above to limit the units that will try this
		// and keep attackers otherwise for city attacks.(sorta)
		// But it's opposite... it should check mission bombard first
		// then check rbombard
		//if (!AI_RbombardCity(bombardTarget(plot())))
		//{
		//	// vanilla behaviour
		//	getGroup()->pushMission(MISSION_BOMBARD);
		//	return true;
		//}
		if (AI_bombardCity())
		{
			return true;
		}

		if (AI_RbombardCity(bombardTarget(plot())))
		{
			return true;
		}
		// RevolutionDCM - end
	}
	/************************************************************************************************/
	/* REVOLUTIONDCM							END									 Glider1	 */
	/************************************************************************************************/
	if (getDomainType() == DOMAIN_LAND)
	{
		foreach_(const CvPlot * pAdjacentPlot1, plot()->adjacent()
		| filtered(CvPlot::fn::isCity(false, NO_TEAM)))
		{
			if (AI_potentialEnemy(pAdjacentPlot1->getTeam(), pAdjacentPlot1))
			{
				foreach_(const CvPlot * pAdjacentPlot2, pAdjacentPlot1->adjacent())
				{
					foreach_(const CvUnit * pLoopUnit, pAdjacentPlot2->units())
					{
						if (pLoopUnit->getOwner() == getOwner())
						{
							if (pLoopUnit->canBombard(pAdjacentPlot2))
							{
								if (pLoopUnit->isGroupHead())
								{
									if (pLoopUnit->getGroup() != getGroup())
									{
										if (pLoopUnit->getGroup()->readyToMove())
										{
											return true;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return false;
}


// Returns true if the unit has found a potential enemy...
bool CvUnitAI::AI_potentialEnemy(TeamTypes eTeam, const CvPlot* pPlot)
{
	PROFILE_FUNC();

	if (getGroup()->AI_isDeclareWar(pPlot))
	{
		return isPotentialEnemy(eTeam, pPlot);
	}
	else
	{
		return isEnemy(eTeam, pPlot);
	}
}


// Returns true if this plot needs some defense...
bool CvUnitAI::AI_defendPlot(const CvPlot* pPlot) const
{
	if (!canDefend(pPlot))
	{
		return false;
	}

	const bool bHasTerrainDamage = (pPlot->getTotalTurnDamage(getGroup()) > 0 || pPlot->getFeatureTurnDamage() > 0);

	const CvCity* pCity = pPlot->getPlotCity();

	if (bHasTerrainDamage && !pPlot->isRoute() && pCity == NULL)
	{
		return false;
	}

	if (pCity != NULL)
	{
		if (pCity->getOwner() == getOwner() && pCity->AI_isDanger())
		{
			return true;
		}
	}
	else
	{
		if (pPlot->plotCount(PUF_canDefendGroupHead, -1, -1, NULL, getOwner()) <= ((atPlot(pPlot)) ? 1 : 0))
		{
			if (pPlot->plotCount(PUF_cannotDefend, -1, -1, NULL, getOwner()) > 0)
			{
				return true;
			}

			//			if (pPlot->defenseModifier(getTeam(), false) >= 50 && pPlot->isRoute() && pPlot->getTeam() == getTeam())
			//			{
			//				return true;
			//			}
		}
	}

	return false;
}


int CvUnitAI::AI_pillageValue(const CvPlot* pPlot, int iBonusValueThreshold) const
{
	PROFILE_EXTRA_FUNC();
	FAssert(getGroup()->canPillage(pPlot) || canAirBombAt(plot(), pPlot->getX(), pPlot->getY()) || (getGroup()->getCargo() > 0));
	//A count is all that's necessary here
	if (!pPlot->isOwned())
	{
		return 0;
	}

	int iBonusValue = 0;
	const BonusTypes eNonObsoleteBonus = pPlot->getNonObsoleteBonusType(pPlot->getTeam());
	if (eNonObsoleteBonus != NO_BONUS)
	{
		iBonusValue = GET_PLAYER(pPlot->getOwner()).AI_bonusVal(eNonObsoleteBonus);
	}

	if (iBonusValueThreshold > 0
	&& (eNonObsoleteBonus == NO_BONUS || iBonusValue < iBonusValueThreshold))
	{
		return 0;
	}
	int iValue = 0;

	if (pPlot->isRoute())
	{
		iValue++;
		if (eNonObsoleteBonus != NO_BONUS)
		{
			iValue += iBonusValue * 4;
		}

		foreach_(const CvPlot * pAdjacentPlot, plot()->adjacent() | filtered(CvPlot::fn::getTeam() == pPlot->getTeam()))
		{
			if (pAdjacentPlot->isCity())
			{
				iValue += 10;
			}

			if (!pAdjacentPlot->isRoute() && !pAdjacentPlot->isWater() && !pAdjacentPlot->isImpassable(getTeam()))
			{
				iValue += 2;
			}
		}
	}
	const ImprovementTypes eImprovement = pPlot->getRevealedImprovementType(getTeam());

	if (eImprovement != NO_IMPROVEMENT)
	{
		if (pPlot->getWorkingCity() != NULL)
		{
			iValue += 5 * pPlot->calculateImprovementYieldChange(eImprovement, YIELD_FOOD, pPlot->getOwner());
			iValue += 4 * pPlot->calculateImprovementYieldChange(eImprovement, YIELD_PRODUCTION, pPlot->getOwner());
			iValue += 3 * pPlot->calculateImprovementYieldChange(eImprovement, YIELD_COMMERCE, pPlot->getOwner());
		}
		iValue += GC.getImprovementInfo(eImprovement).getPillageGold();

		if (eNonObsoleteBonus != NO_BONUS && GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eNonObsoleteBonus))
		{
			if (pPlot->isConnectedToCapital() && pPlot->getPlotGroupConnectedBonus(pPlot->getOwner(), eNonObsoleteBonus) == 1)
			{
				iValue += iBonusValue * 8;
			}
			else iValue += iBonusValue * 4;
		}
	}
	return iValue;
}

int CvUnitAI::AI_nukeValue(const CvCity* pCity) const
{
	PROFILE_FUNC();
	FAssertMsg(pCity, "City is not assigned a valid value");

	const CvPlot* nukePlot = pCity->plot();

	const int iNukeRange = nukeRange();

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (isNukeVictim(nukePlot, (TeamTypes)iI, iNukeRange) && !isEnemy((TeamTypes)iI))
		{
			return 0; // Don't start wars with neutrals
		}
	}

	int iValue = (
			std::max(1, pCity->getPopulation() - 9)
		+ GC.getGame().getSorenRandNum(1 + pCity->getPopulation(), "AI Nuke City Value")
		+ pCity->getPopulation() * (100 + pCity->calculateCulturePercent(pCity->getOwner())) / 100
		- GET_PLAYER(getOwner()).AI_getAttitudeVal(pCity->getOwner()) / 3
	);

	bool bHitOwnLand = false;

	foreach_(const CvPlot * plotX, nukePlot->rect(iNukeRange, iNukeRange))
	{
		if (plotX->getTeam() != NO_TEAM)
		{
			if (plotX->getTeam() == getTeam())
			{
				iValue--;
				bHitOwnLand = true;
			}
			else
			{
				if (plotX->getImprovementType() != NO_IMPROVEMENT)
				{
					iValue++;
				}
				if (plotX->getNonObsoleteBonusType(getTeam()) != NO_BONUS)
				{
					iValue++;
				}
			}
		}
	}

	if (!pCity->isEverOwned(getOwner()))
	{
		iValue *= 3;
		iValue /= 2;
	}

	if (!GET_TEAM(pCity->getTeam()).isAVassal())
	{
		iValue *= 2;
	}

	if (nukePlot->isVisible(getTeam(), false))
	{
		iValue += 2 * nukePlot->getNumVisiblePotentialEnemyDefenders(this);
	}
	else
	{
		iValue += 6;
	}

	if (bHitOwnLand)
	{
		iValue /= 2;
	}
	return iValue;
}


int CvUnitAI::AI_searchRange(int iRange) const
{
	if (iRange < 1)
	{
		return 1;
	}
	if (iRange >= MAX_SEARCH_RANGE)
	{
		return MAX_SEARCH_RANGE;
	}

	if (flatMovementCost() || getDomainType() == DOMAIN_AIR)
	{
		return std::min(MAX_SEARCH_RANGE, iRange * baseMoves());
	}
	return std::min(MAX_SEARCH_RANGE, (iRange + 1) * (baseMoves() + 1));
}


// XXX at some point test the game with and without this function...
bool CvUnitAI::AI_plotValid(const CvPlot* pPlot) const
{
	PROFILE_FUNC();

	if (m_pUnitInfo->isNoRevealMap() && willRevealByMove(pPlot))
	{
		return false;
	}

	switch (getDomainType())
	{
	case DOMAIN_SEA:
	{
		return (
			canMoveAllTerrain()
			|| pPlot->isWater()
			|| pPlot->isCanMoveSeaUnits()
			|| pPlot->isFriendlyCity(*this, true) && pPlot->isCoastalLand()
		);
	}
	case DOMAIN_LAND:
	{
		return !pPlot->isWater() || canMoveAllTerrain() || pPlot->isSeaTunnel();
	}
	case DOMAIN_IMMOBILE: break;
	default:
	{
		FErrorMsg("error");
		break;
	}
	}
	return false;
}


int CvUnitAI::AI_finalOddsThreshold(const CvPlot* pPlot, int iOddsThreshold) const
{
	PROFILE_FUNC();
	int iFinalOddsThreshold = iOddsThreshold;
	const CvCity* pCity = pPlot->getPlotCity();

	if (pCity && pCity->getDefenseDamage() < GC.getMAX_CITY_DEFENSE_DAMAGE() * 3 / 4)
	{
		iFinalOddsThreshold += std::max(0, pCity->getDefenseDamage() - pCity->getLastDefenseDamage() - 2 * GC.getCITY_DEFENSE_DAMAGE_HEAL_RATE());
	}

	// More aggressive if only one enemy defending city or
	//	if there are units in the plot that are special targets of the attacker (such as animals for hunters)
	if (pCity && pPlot->getNumVisiblePotentialEnemyDefenders(this) == 1
	|| pPlot->getNumVisibleEnemyTargetUnits(this) > 0)
	{
		iFinalOddsThreshold *= 2;
		iFinalOddsThreshold /= 3;
	}

	if (getDomainType() == DOMAIN_SEA && getGroup()->hasCargo())
	{
		iFinalOddsThreshold = iFinalOddsThreshold * 3 / 2; // don't risk cargo without better odds
	}
	else
	{
		iFinalOddsThreshold *= 6;
		int iDivisor = 5;
		iDivisor += (AI_isCityAIType() ? 2 : 0);
		iDivisor += ((AI_getUnitAIType() == UNITAI_ATTACK_CITY || AI_getUnitAIType() == UNITAI_ATTACK) ? 2 : 0);
		iFinalOddsThreshold /= iDivisor;
	}
	return range(iFinalOddsThreshold, 0, 100);
}


int CvUnitAI::AI_stackOfDoomExtra() const
{
	return ((AI_getBirthmark() % (1 + GET_PLAYER(getOwner()).getCurrentEra())*2) + 5);
}

bool CvUnitAI::AI_stackAttackCity(int iRange, int iPowerThreshold, bool bFollow)
{
	PROFILE_FUNC();

	FAssert(canMove());

	int iSearchRange = bFollow ? 1 : AI_searchRange(iRange);
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	foreach_(const CvPlot * pLoopPlot, plot()->rect(iSearchRange, iSearchRange))
	{
		int iPathTurns;
		if (AI_plotValid(pLoopPlot)
			// Super Forts begin *AI_offense* - modified if statement so forts are attacked too
			&& pLoopPlot->isCity(true)
			&& AI_potentialEnemy(pLoopPlot->getTeam(), pLoopPlot)
			&& !atPlot(pLoopPlot)
			//	Allow for empty cities (so the move in is strictly not an attack)
			&& (
				(bFollow && getGroup()->canEnterPlot(pLoopPlot, true) || getGroup()->canEnterPlot(pLoopPlot, false))
				||
				(!bFollow && generatePath(pLoopPlot, 0, true, &iPathTurns, iRange) && (iPathTurns <= iRange))
				)
			)
		{
			if (bFollow)
			{
				const int iAttackOdds = getGroup()->AI_attackOdds(pLoopPlot, true);
				if (shouldAvoidLowOddsRiverAttack(*this, pLoopPlot, iAttackOdds))
				{
					continue;
				}
			}

			const int iValue = getGroup()->AI_compareStacks(pLoopPlot, StackCompare::CheckCanAttack | StackCompare::CheckCanMove | StackCompare::PotentialEnemy);

			if (iValue >= iPowerThreshold
				&& iValue > iBestValue)
			{
				const CvPlot* endTurnPlot = getPathEndTurnPlot();

				if (bFollow || endTurnPlot == pLoopPlot || !exposedToDanger(endTurnPlot, 60))
				{
					iBestValue = iValue;
					pBestPlot = bFollow ? pLoopPlot : endTurnPlot;
					FAssert(!atPlot(pBestPlot));
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (gUnitLogLevel >= 1 && pBestPlot->getPlotCity() != NULL)
		{
			logBBAI("	Stack for player %d (%S) decides to attack city %S with stack ratio %d", getOwner(), GET_PLAYER(getOwner()).getCivilizationDescription(0), pBestPlot->getPlotCity()->getName(0).GetCString(), iBestValue);
			logBBAI("	City %S has defense modifier %d, %d with ignore building", pBestPlot->getPlotCity()->getName(0).GetCString(), pBestPlot->getPlotCity()->getDefenseModifier(false), pBestPlot->getPlotCity()->getDefenseModifier(true));
		}
		FAssert(!atPlot(pBestPlot));
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), ((bFollow) ? MOVE_DIRECT_ATTACK : 0));
	}

	return false;
}

bool CvUnitAI::AI_moveIntoCity(int iRange, bool bOpponentOnly)
{
	PROFILE_FUNC();
	FAssert(canMove());

	if (plot()->isCity(true))
	{
		return false;
	}
	const CvPlot* pBestPlot = NULL;
    const CvPlot* refuge = NULL;
	{
		const int iSearchRange = AI_searchRange(iRange);
		int iBestValue = 0;

		foreach_(const CvPlot * plotX, plot()->rect(iSearchRange, iSearchRange))
		{
			if (!plotX->isCity(true)
			|| bOpponentOnly && getTeam() == plotX->getTeam()
			|| !AI_plotValid(plotX)
			|| !UnitTravelConnectivity::landUnitCanReach(this,plotX,bOpponentOnly)
            || !UnitTravelConnectivity::seaUnitCanReach(this,plotX,bOpponentOnly)
			|| !canEnterPlot(plotX))
			{
				continue;
			}
			int iValue = 100;
			if (plotX->getPlotCity()) // Could be a fort
			{
				iValue += 3 * plotX->getPlotCity()->getPopulation();
			}
			if (bOpponentOnly && !plotX->isNPC())
			{
				iValue -= 9 * GET_TEAM(getTeam()).AI_getAttitudeWeight(plotX->getTeam());
			}
			// The same strict integer-score bound applies to land refuges too.
			// Keep the first winner and all original entry/attitude checks.
			const int pathLimit = !isHuman()
				? navalCityPathLimit(iValue, iBestValue, iRange) : iRange;
			int iPathTurns = 0;
			if (pathLimit >= 0 && generatePath(plotX, 0, true, &iPathTurns, pathLimit))
			{
				//Reduce by amount of turns to get there
				if (iPathTurns > 0)
				{
					iValue = iValue / (1 + iPathTurns);
				}

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = getPathEndTurnPlot();
                    refuge = plotX;
					FAssert(!atPlot(pBestPlot));
				}
			}
		}
	}
	if (pBestPlot)
	{
        AI_rememberNavalCommitment(NavalCommitment::REPAIR, refuge);
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}
	return false;
}

bool CvUnitAI::AI_moveIntoNearestOwnedCity()
{
	PROFILE_FUNC();
	FAssert(canMove());

	if (plot()->isCity())
	{
		return false;
	}
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestCityPlot = NULL;
	const bool healer = AI_getUnitAIType() == UNITAI_HEALER || AI_getUnitAIType() == UNITAI_HEALER_SEA;

	foreach_(const CvCity * cityX, GET_PLAYER(getOwner()).cities())
	{
		if (UnitTravelConnectivity::landUnitCanReach(this,cityX->plot()) && AI_plotValid(cityX->plot()) && UnitTravelConnectivity::seaUnitCanReach(this,cityX->plot()))
		{
			const CvPlot* plotX = cityX->plot();
			if (plotX
			&& UnitTravelConnectivity::landUnitCanReach(this,plotX)
			&& plotX->isCity(true)
			&& canEnterPlot(plotX)
			&& (!healer || scoring::healerTargetSafe(this, plotX)))
			{
				int iPathTurns;
				if (generatePath(plotX, 0, true, &iPathTurns))
				{
					int iValue = 1000;
					if (plotX->getPlotCity())
					{
						iValue -= iPathTurns;
					}

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = getPathEndTurnPlot();
						pBestCityPlot = plotX;
						FAssert(!atPlot(pBestPlot));
					}
				}
			}
		}
	}

	if (pBestPlot && pBestCityPlot)
	{
		FAssert(!atPlot(pBestCityPlot));
		if (AI_getUnitAIType() == UNITAI_HEALER)
		{
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_UNITS, false, false, MISSIONAI_HEAL_SUPPORT, pBestCityPlot);
		}
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_UNITS);
	}
	return false;
}

//bolsters the culture of the weakest city.
//returns true if a mission is pushed.
bool CvUnitAI::AI_artistCultureVictoryMove()
{
	PROFILE_EXTRA_FUNC();
	if (!GET_PLAYER(getOwner()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE1))
	{
		return false;
	}
	FAssertMsg(GC.getGame().culturalVictoryNumCultureCities() > 0, "AI_VICTORY_CULTURE1 Strategy should not be true");

	bool bJoin = true;
	bool bGreatWork = GET_PLAYER(getOwner()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3);

	SpecialistTypes eBestSpecialist = NO_SPECIALIST;
	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;
	int iTargetCultureRank = GC.getGame().culturalVictoryNumCultureCities();

	while (iTargetCultureRank > 0 && !pBestPlot)
	{
		foreach_(CvCity * cityX, GET_PLAYER(getOwner()).cities())
		{
			// BBAI efficiency: check same area
			if (UnitTravelConnectivity::landUnitCanReach(this,cityX->plot()) && AI_plotValid(cityX->plot())
			// instead of commerce rate rank should use the culture on tile...
			&& cityX->findCommerceRateRank(COMMERCE_CULTURE) == iTargetCultureRank)
			{
				// if the city is a fledgling, probably building culture, try next higher city
				if (cityX->getCultureLevel() < 2
				// if we cannot path there, try the next higher culture city
				|| !generateIgnoreDangerPath(cityX->plot()))
				{
					continue;
				}
				pBestPlot = cityX->plot();

				if (bGreatWork && canGreatWork(pBestPlot))
				{
					break;
				}
				for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
				{
					if (canJoin(pBestPlot, (SpecialistTypes)iI))
					{
						const int iValue = cityX->AI_specialistValue(((SpecialistTypes)iI), cityX->AI_avoidGrowth(), false);

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestSpecialist = (SpecialistTypes)iI;
						}
					}
				}
				if (eBestSpecialist == NO_SPECIALIST)
				{
					bJoin = false;
					if (canGreatWork(pBestPlot))
					{
						bGreatWork = true;
						break;
					}
					bGreatWork = false;
				}
				break;
			}
		}
		iTargetCultureRank--;
	}

	if (!pBestPlot)
	{
		return false; // should try to airlift there...
	}

	if (!atPlot(pBestPlot))
	{
		if (generateSafePathforVulnerable(pBestPlot))
		{
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_WITH_CAUTION);
		}
		getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
		return true;
	}

	if (bGreatWork)
	{
		getGroup()->pushMission(MISSION_GREAT_WORK);
		return true;
	}
	if (bJoin)
	{
		getGroup()->pushMission(MISSION_JOIN, eBestSpecialist);
		return true;
	}
	return false;
}


bool CvUnitAI::AI_choke(int iRange, bool bDefensive)
{
	PROFILE_FUNC();

	bool bNoDefensiveBonus = noDefensiveBonus();

	if (getGroup()->getNumUnits() > 1)
	{
		foreach_(const CvUnit * pLoopUnit, getGroup()->units())
		{
			bNoDefensiveBonus = (bNoDefensiveBonus && pLoopUnit->noDefensiveBonus());
		}
	}
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestChokePlot = NULL;
	int iBestValue = 0;

	foreach_(const CvPlot * pLoopPlot, plot()->rect(iRange, iRange))
	{
		if (isEnemy(pLoopPlot->getTeam()) && !pLoopPlot->isVisibleEnemyUnit(this))
		{
			const CvCity* pWorkingCity = pLoopPlot->getWorkingCity();

			if (pWorkingCity != NULL && pWorkingCity->getTeam() == pLoopPlot->getTeam()
			// and no terrain damage
			&& pLoopPlot->getTotalTurnDamage(getGroup()) <= 0 && pLoopPlot->getFeatureTurnDamage() <= 0)
			{
				int iValue = (bDefensive ? pLoopPlot->defenseModifier(getTeam(), false) : -15);

				if (pLoopPlot->getBonusType(getTeam()) != NO_BONUS)
				{
					iValue += GET_PLAYER(pLoopPlot->getOwner()).AI_bonusVal(pLoopPlot->getBonusType(), 0);
				}
				iValue += pLoopPlot->getYield(YIELD_PRODUCTION) * 10;
				iValue += pLoopPlot->getYield(YIELD_FOOD) * 10;
				iValue += pLoopPlot->getYield(YIELD_COMMERCE) * 5;

				if (bNoDefensiveBonus)
				{
					iValue *= std::max(0, ((baseCombatStrNonGranular() * 120) - GC.getGame().getBestLandUnitCombat()));
				}
				else iValue *= pLoopPlot->defenseModifier(getTeam(), false);

				if (iValue > 0)
				{
					if (!bDefensive)
					{
						iValue *= 10;
						iValue /= std::max(1, (pLoopPlot->getNumDefenders(getOwner()) + ((pLoopPlot == plot()) ? 0 : getGroup()->getNumUnits())));
					}
					// Score first: worse fields cannot improve this blockade. On an
					// equal score keep an already useful position instead of shuffling
					// between fields solely because of enumeration order.
					if (iValue <= 0 || iValue < iBestValue || (iValue == iBestValue && !atPlot(pLoopPlot))) continue;
					if (generatePath(pLoopPlot, 0, true, NULL, iRange))
					{
						const CvPlot* endTurnPlot = getPathEndTurnPlot();

						if (!exposedToDanger(endTurnPlot, 60))
						{
							pBestPlot = endTurnPlot;
							iBestValue = iValue;
							pBestChokePlot = pLoopPlot;
						}
					}
				}
			}
		}
	}
	if (pBestPlot != NULL && pBestChokePlot != NULL)
	{
		if (atPlot(pBestChokePlot))
		{
			//if(getGroup()->canPillage(plot()))
			//{
			//	getGroup()->pushMission(MISSION_PILLAGE);
			//}
			if (!isInGroup())//Somehow the pillage mission caused a case where a unit lost it's group definition.  I could not spot how as I walked through the event.
			{
				joinGroup(NULL, true);
			}
			getGroup()->pushMission(MISSION_SKIP);

			return true;
		}
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}
	return false;
}


bool CvUnitAI::AI_solveBlockageProblem(const CvPlot* pDestPlot, bool bDeclareWar)
{
	PROFILE_FUNC();

	FAssert(pDestPlot != NULL);


	if (pDestPlot != NULL)
	{
		const CvPlot* pSourcePlot = plot();

		if (gDLL->getFAStarIFace()->GeneratePath(&GC.getStepFinder(), pSourcePlot->getX(), pSourcePlot->getY(), pDestPlot->getX(), pDestPlot->getY(), false, -1, true))
		{
			const FAStarNode* pStepNode = gDLL->getFAStarIFace()->GetLastNode(&GC.getStepFinder());

			while (pStepNode != NULL)
			{
				int iPathTurns;
				const CvPlot* pStepPlot = GC.getMap().plotSorenINLINE(pStepNode->m_iX, pStepNode->m_iY);
				if (canEnterOrAttackPlot(pStepPlot) && generatePath(pStepPlot, 0, false, &iPathTurns))
				{
					if (bDeclareWar && pStepNode->m_pPrev != NULL)
					{
						const CvPlot* pPlot = GC.getMap().plotSorenINLINE(pStepNode->m_pPrev->m_iX, pStepNode->m_pPrev->m_iY);
						if (pPlot->getTeam() != NO_TEAM
						&& !getGroup()->canEnterPlot(pPlot, true)
						&& !isPotentialEnemy(pPlot->getTeam(), pPlot))
						{
							CvTeamAI& kTeam = GET_TEAM(getTeam());
							if (kTeam.canDeclareWar(pPlot->getTeam()))
							{
								WarPlanTypes eWarPlan = WARPLAN_LIMITED;
								const WarPlanTypes eExistingWarPlan = kTeam.AI_getWarPlan(pDestPlot->getTeam());
								if (eExistingWarPlan != NO_WARPLAN)
								{
									if ((eExistingWarPlan == WARPLAN_TOTAL) || (eExistingWarPlan == WARPLAN_PREPARING_TOTAL))
									{
										eWarPlan = WARPLAN_TOTAL;
									}

									if (!kTeam.isAtWar(pDestPlot->getTeam()))
									{
										kTeam.AI_setWarPlan(pDestPlot->getTeam(), NO_WARPLAN);
									}
								}
								kTeam.AI_setWarPlan(pPlot->getTeam(), eWarPlan, true);
								return (AI_goToTargetCity(MOVE_AVOID_ENEMY_WEIGHT_2));
							}
						}
					}
					if (pStepPlot->isVisibleEnemyUnit(this))
					{
						FAssert(canAttack());
						const CvPlot* pBestPlot = pStepPlot;
						//To prevent puppeteering attempt to barge through
						//if quite close
						if (iPathTurns > 3)
						{
							pBestPlot = getPathEndTurnPlot();
						}

						FAssert(!atPlot(pBestPlot));
						return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_DIRECT_ATTACK);
					}
				}
				pStepNode = pStepNode->m_pParent;
			}
		}
	}

	return false;
}

int CvUnitAI::AI_calculatePlotWorkersNeeded(const CvPlot* pPlot, BuildTypes eBuild) const
{
	const int iWorkRate = workRate(true);

	if (iWorkRate <= 0)
	{
		FErrorMsg("error");
		return 1;
	}
	const int iBuildTime = pPlot->getBuildTime(eBuild) - pPlot->getBuildProgress(eBuild);
	int iTurns = iBuildTime / iWorkRate;

	if (iBuildTime > (iTurns * iWorkRate))
	{
		iTurns++;
	}

	int iNeeded = std::max(1, (iTurns + 2) / 3);

	if (pPlot->getNonObsoleteBonusType(getTeam()) != NO_BONUS)
	{
		iNeeded *= 2;
	}

	return iNeeded;

}

bool CvUnitAI::AI_canGroupWithAIType(UnitAITypes eUnitAI, const CvPlot* at) const
{
	if (!at) at = plot();
	if (eUnitAI != AI_getUnitAIType())
	{
		switch (eUnitAI)
		{
		case (UNITAI_ATTACK_CITY):
			if (at->isCity() && (GC.getGame().getGameTurn() - at->getPlotCity()->getGameTurnAcquired()) <= 1)
			{
				return false;
			}
			break;
		default:
			break;
		}
	}

	return true;
}



bool CvUnitAI::AI_allowGroup(const CvUnit* pUnit, UnitAITypes eUnitAI, bool bMergeWholeGroup) const
{
	// Another army must not recruit a reinforcement and overwrite its guard
	// destination with a rendezvous or WAIT_FOR_ESCORT mission.
	if(!AI_canRegroupDefense(this,pUnit))return false;
	CvSelectionGroup* pGroup = pUnit->getGroup();
	CvPlot* pPlot = pUnit->plot();
	bool bStateNoWork = false;
	// Do not travel to a rendezvous that the same policy will refuse on
	// arrival (e.g. a cross-role merge in a just-captured city).
	if (StackPromotions::enabled() && !AI_canGroupWithAIType(eUnitAI, pPlot)) return false;
	if (pUnit == this)
	{
		return false;
	}

	if (!pUnit->isGroupHead())
	{
		return false;
	}

	//	Don't join a unit that was itself wondering what to do this turn, unless Being Sure it is possible
	if ((static_cast<const CvUnitAI*>(pUnit))->m_contractsLastEstablishedTurn == GC.getGame().getGameTurn() &&
		 (m_contractualState == CONTRACTUAL_STATE_AWAITING_WORK || m_contractualState == CONTRACTUAL_STATE_NO_WORK_FOUND))
	{
		bStateNoWork = true;
	}

	if (pGroup == getGroup())
	{
		return false;
	}

	if (pUnit->isCargo())
	{
		return false;
	}

	if (pUnit->AI_getUnitAIType() != eUnitAI)
	{
		return false;
	}

	switch (pGroup->AI_getMissionAIType())
	{
	case MISSIONAI_GUARD_CITY:
		// do not join groups that are guarding cities
		// intentional fallthrough
	case MISSIONAI_LOAD_SETTLER:
	case MISSIONAI_LOAD_ASSAULT:
	case MISSIONAI_LOAD_SPECIAL:
		// do not join groups that are loading into transports (we might not fit and get stuck in loop forever)
		return false;
		break;
	default:
		break;
	}

	if (pGroup->getActivityType() == ACTIVITY_HEAL)
	{
		// do not attempt to join groups which are healing this turn
		// (healing is cleared every turn for automated groups, so we know we pushed a heal this turn)
		return false;
	}

	if (!canJoinGroup(pPlot, pGroup, bMergeWholeGroup))
	{
		return false;
	}

	if (eUnitAI == UNITAI_SETTLE)
	{
		if (GET_PLAYER(getOwner()).AI_getAnyPlotDanger(pPlot, 3))
		{
			return false;
		}
	}
	else if (eUnitAI == UNITAI_ASSAULT_SEA)
	{
		if (!pGroup->hasCargo())
		{
			return false;
		}
	}

	if ((getGroup()->getHeadUnitAI() == UNITAI_CITY_DEFENSE))
	{
		if (plot()->isCity() && (plot()->getTeam() == getTeam()) && plot()->getBestDefender(getOwner())->getGroup() == getGroup())
		{
			return false;
		}
	}

	if (plot()->getOwner() == getOwner())
	{
		CvPlot* pTargetPlot = pGroup->AI_getMissionAIPlot();

		if (pTargetPlot != NULL)
		{
			if (pTargetPlot->isOwned())
			{
				if (isPotentialEnemy(pTargetPlot->getTeam(), pTargetPlot))
				{
					//Do not join groups which have debarked on an offensive mission
					return false;
				}
			}
		}
	}

	if (pUnit->getInvisibleType() != NO_INVISIBLE)
	{
		if (getInvisibleType() == NO_INVISIBLE)
		{
			return false;
		}
	}

	//Calvitix If searching for Work, perhaps better to group than have an hypothetical job
	if (bStateNoWork)
	{
		GET_PLAYER(getOwner()).getContractBroker().removeUnit(this);
	}
	return true;
}


void CvUnitAI::read(FDataStreamBase* pStream)
{
	CvUnit::read(pStream);

	pStream->Read(&m_iBirthmark);

	pStream->Read((int*)&m_eUnitAIType);
	pStream->Read(&m_iAutomatedAbortTurn);

	//	Translate the unit AI id - done this way because unit AI types were
	//	not originally considered to be an enum type and so older saves have them
	//	as raw ints
	UnitAITypes eTranslatedUnitAI = (UnitAITypes)CvTaggedSaveFormatWrapper::getSaveFormatWrapper().getNewClassEnumValue(REMAPPED_CLASS_TYPE_UNITAIS, m_eUnitAIType, true);
	//	If the save has no translation table for unitAIs assume the ids are identical - this
	//	is normally the case, and we can't do any better with an older save
	if (eTranslatedUnitAI != NO_UNITAI)
	{
		m_eUnitAIType = eTranslatedUnitAI;
	}
	//	Animals should never have a non-animal AI (old assets had a bug that allowed this
	//	so correct it here as a patch up)
	// TB Note: This is potentially a little constrictive now to some concepts.
	//if ( isAnimal() && m_eUnitAIType != UNITAI_ANIMAL )
	//{
	//	m_eUnitAIType = UNITAI_ANIMAL;

	//	GET_PLAYER(getOwner()).AI_noteUnitRecalcNeeded();
	//}
	//	Invalid unitAIs cannot be coped with - just delete them - better than
	//	killing the entire game when this corruption is found
	if (m_eUnitAIType >= NUM_UNITAI_TYPES)
	{
		kill(true);
	}

	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);
	WRAPPER_READ(wrapper, "CvUnitAI", &m_iGarrisonCity);	// which city does this unit belong to the garrison of (if any)
	WRAPPER_READ(wrapper, "CvUnitAI", &m_iGroupLeadOverride);
	WRAPPER_READ(wrapper, "CvUnitAI", &m_bWaitingOnUnitAIAny);
    WRAPPER_READ_ARRAY(wrapper, "CvUnitAI", NavalCommitment::COUNT, m_aiNavalCommitment);

	//m_aiWaitingOnUnitAITypes.clear();
	//uint iSize;
	//WRAPPER_READ_DECORATED(wrapper, "CvUnitAI", &iSize, "numWaitingOnUnitAITypes");
	//for (uint i = 0; i < iSize; i++)
	//{
	//	int iIndex;
	//	WRAPPER_READ(wrapper, "CvUnitAI", &iIndex);
	//	m_aiWaitingOnUnitAITypes.push_back(iIndex);
	//}

	//Example of how to Skip Element
	//WRAPPER_SKIP_ELEMENT(wrapper, "CvUnitAI", m_iLastUnitUpdated, SAVE_VALUE_TYPE_INT);
}


void CvUnitAI::write(FDataStreamBase* pStream)
{
	CvUnit::write(pStream);

	pStream->Write(m_iBirthmark);

	pStream->Write(m_eUnitAIType);
	pStream->Write(m_iAutomatedAbortTurn);

	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);
	WRAPPER_WRITE(wrapper, "CvUnitAI", m_iGarrisonCity);	// which city does this unit belong to the garrison of (if any)
	WRAPPER_WRITE(wrapper, "CvUnitAI", m_iGroupLeadOverride);
	WRAPPER_WRITE(wrapper, "CvUnitAI", m_bWaitingOnUnitAIAny);
    WRAPPER_WRITE_ARRAY(wrapper, "CvUnitAI", NavalCommitment::COUNT, m_aiNavalCommitment);

	//uint iSize = m_aiWaitingOnUnitAITypes.size();
	//WRAPPER_WRITE_DECORATED(wrapper, "CvUnitAI", iSize, "numWaitingOnUnitAITypes");
	//for (uint iI = 0; iI < iSize; iI++)
	//{
	//	WRAPPER_WRITE(wrapper, "CvUnitAI", m_aiWaitingOnUnitAITypes[iI]);
	//}
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD					  02/21/10								jdog5000	  */
/*																							  */
/* Lead From Behind																			 */
/************************************************************************************************/
// Dale - RB: Field Bombard START
// RevolutionDCM - ranged bombardment
// returns true if a mission was pushed...
bool CvUnitAI::AI_RbombardPlot(int iRange, int iBonusValueThreshold)
{
	PROFILE_FUNC();

	if (!GC.isDCM_RANGE_BOMBARD())
	{
		return false;
	}
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * plotX, plot()->rect(iRange, iRange))
	{
		if (AI_plotValid(plotX) && !plotX->isCity() && plotX->getTeam() != getTeam()
		&& plotX->getImprovementType() != NO_IMPROVEMENT
		&& getGroup()->canBombardAtRanged(plot(), plotX->getX(), plotX->getY()))
		{
			int iValue = 0;
			if (iBonusValueThreshold > 0)
			{
				const ImprovementTypes eImprovement = plotX->getRevealedImprovementType(getTeam());

				if (eImprovement != NO_IMPROVEMENT)
				{
					iValue = std::max(0, GC.getImprovementInfo(eImprovement).getPillageGold() - 10); // cottages = 0, hamlets = 5, villages = 10, towns = 15
					iValue *= 100;
					iValue /= 15; // cottages = 0, hamlets = 33, villages = 67, towns = 100
					if (plotX->getWorkingCity() == NULL)
					{
						iValue *= 50;
						iValue /= 100;
					}
					if (iValue < iBonusValueThreshold) iValue = 0;
				}
			}
			else iValue = AI_pillageValue(plotX, 0); // returns any improvement with highest pillage value


			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestPlot = plotX;
			}
		}
	}
	if (pBestPlot != NULL)
	{
		FAssertMsg(pBestPlot->getNumVisibleEnemyCombatUnits(getOwner()) > 0, "Can't bombard without units in target plot");
		getGroup()->pushMission(MISSION_RBOMBARD, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_RbombardUnit(int iRange, int iHighestOddsThreshold, int iMinStack, int iSeigeDiff, int iPowerThreshold, bool bCity)
{
	// iRange = bombard range to consider
	// iHighestOddsThreshold = the highest chance of successful attack
	// iMinStack = the minimum stack size to bombard
	// iSeigeDiff = the difference in range bombard capable seige, us to them
	// iPowerThreshold = comparison of stack strengths as a percent ration with 100 being equality

	PROFILE_FUNC();

	int iPrelimValue;

	if (!GC.isDCM_RANGE_BOMBARD())
	{
		return false;
	}

	const int iSearchRange = AI_searchRange(iRange);

	int iBestValue = 999; // looking for the odds at or below iHighestOddsThreshold
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, plot()->rect(iSearchRange, iSearchRange))
	{
		if (AI_plotValid(pLoopPlot) && (!pLoopPlot->isCity() || bCity))
		{
			if (getGroup()->canBombardAtRanged(plot(), pLoopPlot->getX(), pLoopPlot->getY()))
			{
				if (pLoopPlot->isVisibleEnemyUnit(this) || AI_potentialEnemy(pLoopPlot->getTeam()))
				{
					if (pLoopPlot->getNumVisiblePotentialEnemyDefenders(this) >= iMinStack)
					{
						for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
						{
							const TeamTypes eEnemyTeam = GET_PLAYER((PlayerTypes)iPlayer).getTeam();
							if (GET_TEAM(eEnemyTeam).isAtWar(getTeam()))
							{
								if (pLoopPlot->getAverageEnemyDamage(eEnemyTeam) < getGroup()->getMinimumRBombardDamageLimit())
								{
									int iEnemyCount = pLoopPlot->getNumVisibleEnemyCombatUnits((PlayerTypes)iPlayer);

									iPrelimValue = pLoopPlot->getAverageEnemyStrength(eEnemyTeam);
									iPrelimValue *= pLoopPlot->getAverageEnemyDamage(eEnemyTeam);
									iPrelimValue /= 100;
									iPrelimValue += std::min(getGroup()->getRBombardDamageMaxUnits(), iEnemyCount);
									iPrelimValue *= iEnemyCount;
								}
							}
						}
						const CvUnit* pDefender = pLoopPlot->getBestDefender(NO_PLAYER, getOwner(), this, true);
						if (pDefender != NULL && pDefender->isRbombardable(iMinStack))
						{
							int iValue = AI_attackOdds(pLoopPlot, true);
							if (iValue <= iHighestOddsThreshold)
							{
								int ourSeige = getRbombardSeigeCount(plot());
								int theirSeige = pDefender->getRbombardSeigeCount(pLoopPlot);
								if ((ourSeige - theirSeige) >= iSeigeDiff)
								{
									int ourStrength = GET_PLAYER(getOwner()).AI_getOurPlotStrength(plot(), iRange, false, false) * 100;
									int theirStrength = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), iRange, false, false);
									if (theirStrength <= 0)
									{
										theirStrength = 1;
									}
									int strengthRatio = (ourStrength / theirStrength);
									if ((strengthRatio >= iPowerThreshold) && (theirStrength > 1))
									{
										iValue -= iPrelimValue;
										if (iValue < iBestValue)
										{
											iBestValue = iValue;
											pBestPlot = pLoopPlot;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if (pBestPlot != NULL)
	{
		FAssertMsg(pBestPlot->getNumVisibleEnemyCombatUnits(getOwner()) > 0, "Can't bombard without units in target plot");
		getGroup()->pushMission(MISSION_RBOMBARD, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_RbombardCity(const CvCity* pCity)
{
	PROFILE_FUNC();

	if (!GC.isDCM_RANGE_BOMBARD() || pCity == NULL)
	{
		return false;
	}

	const CvPlot* pPlot = pCity->plot();
	if (pPlot == NULL)
	{
		return false; // ok will never happen but anyway...
	}

	if (!getGroup()->canBombardAtRanged(plot(), pPlot->getX(), pPlot->getY()))
	{
		return false;
	}

	if (AI_RbombardUnit(std::min(2, getGroup()->getMinimumRBombardRange()), 65, 2, 0, 0, true))
	{
		return true;
	}
	/*pDefender = pPlot->getBestDefender(NO_PLAYER, getOwner(), this, true);
	if (pDefender != NULL)
	{
		if (collateralDamageMaxUnits() > pPlot->getNumUnits())
		{
			if (pDefender->isRbombardable(3) && getGroup()->getBombardTurns(pCity) < 3)
			{
				getGroup()->pushMission(MISSION_RBOMBARD, pPlot->getX(),pPlot->getY());
				return true;
			}
		}
	}*/
	return false;
}

// RevolutionDCM - sea bombard AI identical to DCM 1.7 AI_RbombardCity() AI
// Returns true if a mission was pushed...
bool CvUnitAI::AI_RbombardNaval()
{
	PROFILE_FUNC();

	if (!canRBombard())
	{
		return false;
	}

	const int iSearchRange = getGroup()->getMinimumRBombardRange();
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, plot()->rect(iSearchRange, iSearchRange))
	{
		if (!atPlot(pLoopPlot) && getGroup()->canBombardAtRanged(plot(), pLoopPlot->getX(), pLoopPlot->getY()))
		{
			const int iEnemyCount = pLoopPlot->getNumVisibleEnemyCombatUnits(getOwner());

			// We definitely can't do a bombard mission unless there is enemy present, and we won't do one unless the targets are worth it
			if (iEnemyCount > 0 && pLoopPlot->getAverageEnemyDamage(getTeam()) < getGroup()->getMinimumRBombardDamageLimit())
			{
				int iValue = pLoopPlot->getAverageEnemyStrength(getTeam());
				iValue *= pLoopPlot->getAverageEnemyDamage(getTeam());
				iValue /= 100;
				iValue += std::min(getGroup()->getRBombardDamageMaxUnits(), iEnemyCount);
				iValue *= iEnemyCount;

				CvCity* pCity = pLoopPlot->getPlotCity();
				if (pCity != NULL)
				{
					//iValue += std::max(0, (std::min((pCity->getDefenseDamage() + airBombCurrRate()), GC.getMAX_CITY_DEFENSE_DAMAGE()) - pCity->getDefenseDamage()));
					iValue *= 5;
					if (pCity->AI_isDanger())
					{
						iValue *= 2;
					}
					if (pCity == pCity->area()->getTargetCity(getOwner()))
					{
						iValue *= 3;
					}
				}

				//TBHERE
				int iPotentialAttackers = GET_PLAYER(getOwner()).AI_adjacentPotentialAttackers(pLoopPlot);//pLoopPlot->getNumVisibleEnemyDefenders(NO_PLAYER);
				if (iPotentialAttackers > 0 || pLoopPlot->isAdjacentTeam(getTeam()))
				{
					const CvUnit* pDefender = pLoopPlot->getBestDefender(NO_PLAYER, getOwner(), this, true);

					if (pDefender != NULL && pDefender->canDefend() && iValue > 0)
					{
						//iDamage = GC.getGame().getSorenRandNum(rBombardDamage(), "AI Bombard");
						//iValue = std::max(0, (std::min((pDefender->getDamage() + iDamage), rBombardDamage()) - pDefender->getDamage()));
						//iValue += ((((iDamage * collateralDamage()) / 100) * std::min((pLoopPlot->getNumVisibleEnemyDefenders(this) - 1), collateralDamageMaxUnits())) / 2);
						iValue *= (3 + iPotentialAttackers);
						iValue /= 4;
					}
				}
				iValue *= GC.getGame().getSorenRandNum(20, "AI Ranged Bombard Selection(Naval)");
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = pLoopPlot;
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssertMsg(pBestPlot->getNumVisibleEnemyCombatUnits(getOwner()) > 0, "Can't bombard without units in target plot");
		getGroup()->pushMission(MISSION_RBOMBARD, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}
	return false;
}
// Dale - RB: Field Bombard END
// RevolutionDCM - end


// Dale - FE: Fighters START
bool CvUnitAI::AI_FEngage()
{
	PROFILE_FUNC();

	if (!canFEngage())
	{
		return false;
	}
	if (currFirepower(NULL, NULL) <= 0)
	{
		return false;
	}
	if (GC.getGame().getSorenRandNum(10, "Randomness") < 5)
	{
		return false;
	}
	if (GC.isDCM_FIGHTER_ENGAGE())
	{
		const int iSearchRange = airRange();
		int iBestValue = 0;
		const CvPlot* pBestPlot = NULL;
		foreach_(const CvPlot * pLoopPlot, plot()->rect(iSearchRange, iSearchRange))
		{
			if (canFEngageAt(plot(), pLoopPlot->getX(), pLoopPlot->getY()))
			{
				int iValue = 0;
				const int iCount = algo::count_if(pLoopPlot->units(), CvUnit::fn::getDomainType() == DOMAIN_AIR);
				const int iPotentialAttackers = iCount;
				iValue += iCount;
				if (iPotentialAttackers > 0 || pLoopPlot->isAdjacentTeam(getTeam()))
				{
					const CvUnit* pDefender = pLoopPlot->getBestDefender(NO_PLAYER, getOwner(), this, true);
					const int iDamage = std::max(1, ((GC.getCOMBAT_DAMAGE() * (currFirepower(NULL, NULL) + ((currFirepower(NULL, NULL) + pDefender->currFirepower(NULL, NULL) + 1) / 2))) / (pDefender->currFirepower(pLoopPlot, this) + ((currFirepower(NULL, NULL) + pDefender->currFirepower(NULL, NULL) + 1) / 2))));
					iValue += (iDamage * pLoopPlot->getNumVisiblePotentialEnemyDefenders(this));
				}
				iValue *= GC.getGame().getSorenRandNum(5, "AI FEngage");
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = pLoopPlot;
					FAssert(!atPlot(pBestPlot));
				}
			}
		}
		if (pBestPlot != NULL)
		{
			getGroup()->pushMission(MISSION_FENGAGE, pBestPlot->getX(), pBestPlot->getY());
			return true;
		}
	}
	return false;
}
// Dale - FE: Fighters END
/************************************************************************************************/
/* DCM							END											   Glider1	 */
/************************************************************************************************/

/**** Dexy - Fixed Borders START ****/
// Returns true if a mission was pushed...
bool CvUnitAI::AI_claimForts(CvReachablePlotSet* pReachablePlots, int iMinValue, int iMaxPath)
{
	PROFILE_FUNC();

	int iBestValue = iMinValue;
	const CvPlot* endTurnPlot = NULL;
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pFortPlot = NULL;


	for (CvReachablePlotSet::const_iterator itr = pReachablePlots->begin(); itr != pReachablePlots->end(); ++itr)
	{
		const CvPlot* pLoopPlot = itr.plot();
		if (pLoopPlot->getMilitaryImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(pLoopPlot->getMilitaryImprovementType()).getCulture() > 0)
		{
			if (/*AI_plotValid(pLoopPlot) &&*/ UnitTravelConnectivity::landUnitCanReach(this,pLoopPlot))
			{
				if (potentialWarAction(pLoopPlot) || pLoopPlot->getOwner() == NO_PLAYER)
				{
					if (canClaimTerritory(pLoopPlot))
					{
						if (!AI_isPlotWellDefended(pLoopPlot, true, 35))
						{
							if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_CLAIM_TERRITORY, getGroup(), 1) == 0)
							{
								int iPathTurns;
								if (generatePath(pLoopPlot, 0, true, &iPathTurns, iMaxPath))
								{
									int iValue = 1000;

									foreach_(const CvPlot * pAdjacentPlot, pLoopPlot->adjacent()
									| filtered(CvPlot::fn::getBonusType(getTeam()) != NO_BONUS))
									{
										iValue *= 2;
									}

									iValue /= (iPathTurns + 1);

									if (iValue > iBestValue)
									{
										endTurnPlot = getPathEndTurnPlot();

										if (endTurnPlot == pLoopPlot || !exposedToDanger(endTurnPlot, 60))
										{
											iBestValue = iValue;
											pBestPlot = endTurnPlot;
											pFortPlot = pLoopPlot;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pFortPlot != NULL))
	{
		if (atPlot(pFortPlot))
		{
			if (isEnemy(pFortPlot->getTeam()))
			{
				getGroup()->pushMission(MISSION_CLAIM_TERRITORY, -1, -1, 0, false, false, MISSIONAI_CLAIM_TERRITORY, pFortPlot);
				return true;
			}
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_CLAIM_TERRITORY, pFortPlot);
		}
	}

	return false;
}

BuildTypes CvUnitAI::AI_findBestFort(const CvPlot* pPlot) const
{
	PROFILE_FUNC();
	int iBestTempBuildValue = 0;
	BuildTypes eBestTempBuild = NO_BUILD;

	for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		const BuildTypes eBuild = static_cast<BuildTypes>(iI);

		if (GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT)
		{
			const CvImprovementInfo& kImprovement = GC.getImprovementInfo(GC.getBuildInfo(eBuild).getImprovement());
			// Watchtower can claim land if fort can't
			if (kImprovement.isMilitaryStructure() && canBuild(pPlot, eBuild))
			{
				int iValue =
					(
						kImprovement.getDefenseModifier() * 100
						+
						kImprovement.getVisibilityChange() * 1000 // Each visibility equals 10% defense mod
					);
				if (kImprovement.isActsAsCity())
				{
					iValue += 5000; // Equals 50% defense mod
				}
				if (kImprovement.isZOCSource())
				{
					iValue += 5000; // Equals 50% defense mod
				}
				if (iValue > 0)
				{
					iValue /= (GC.getBuildInfo(eBuild).getTime() + 1);

					if (iValue > iBestTempBuildValue)
					{
						iBestTempBuildValue = iValue;
						eBestTempBuild = eBuild;
					}
				}
			}
		}
	}

	return eBestTempBuild;
}

bool CvUnitAI::AI_StrategicForts()
{
	PROFILE_FUNC();

	if (isNPC())
	{
		return false;
	}
	const CvPlayerAI& kOwner = GET_PLAYER(getOwner());
	const TeamTypes eTeam = getTeam();
	const bool bWarPlan = GET_TEAM(eTeam).hasWarPlan(true);

	CvReachablePlotSet plotSet(getGroup(), 0, MAX_INT);
	BuildTypes eBestBuild = NO_BUILD;
	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;
	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		CvPlot* pLoopPlot = itr.plot();

		if (/*AI_plotValid(pLoopPlot) &&*/ UnitTravelConnectivity::landUnitCanReach(this,pLoopPlot)
		&& (pLoopPlot->getOwner() == NO_PLAYER || pLoopPlot->getTeam() == eTeam)
		&& (CvAdditionalImprovements::enabled() || pLoopPlot->getImprovementType() == NO_IMPROVEMENT))
		{
			const BuildTypes eBestTempBuild = AI_findBestFort(pLoopPlot);

			if (eBestTempBuild != NO_BUILD
			&& !pLoopPlot->isVisibleEnemyUnit(this)
			&& kOwner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD, getGroup(), 3) == 0)
			{
				int iPathTurns = 0;
				if (generateIgnoreDangerPath(pLoopPlot, &iPathTurns))
				{
					//	Koshling - adjusted a bit to take advantage of super forts choke calculations
					const int iValue =
						(
							1000
							*
							std::max(100, pLoopPlot->getFoundValue(getOwner()) / 2 + kOwner.AI_getPlotChokeValue(pLoopPlot))
							*
							(pLoopPlot->getBorderPlotCount() + 1)
							*
							(pLoopPlot->getEnemyBorderPlotCount(getOwner()) + 1) * (bWarPlan ? 3 : 1)
							/
							((GC.getBuildInfo(eBestTempBuild).getTime() + 1) * (iPathTurns + 1))
						);
					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						eBestBuild = eBestTempBuild;
						pBestPlot = pLoopPlot;
					}
				}
			}
		}
	}
	// No real value or plot
	if (iBestValue < 250 || !pBestPlot)
	{
		return false;
	}
	FASSERT_BOUNDS(0, GC.getNumBuildInfos(), eBestBuild);

	if (atPlot(pBestPlot))
	{
		getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, (getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);
		return true;
	}
	if (generateSafePathforVulnerable(pBestPlot))
	{
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_WITH_CAUTION, false, false, MISSIONAI_BUILD, pBestPlot);
	}
	getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
	return true;
}

bool CvUnitAI::AI_caravan(bool bAnyCity)
{
	PROFILE_FUNC();

	if (getUnitInfo().getProductionCost() < 0)
	{
		return false; // Avoid using Great People
	}
	const int iNumCities = GET_PLAYER(getOwner()).getNumCities();
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	foreach_(const CvCity * pLoopCity, GET_PLAYER(getOwner()).cities())
	{
		if (UnitTravelConnectivity::landUnitCanReach(this,pLoopCity->plot()) && AI_plotValid(pLoopCity->plot()) && canHurry(pLoopCity->plot())
		&& !pLoopCity->plot()->isVisibleEnemyUnit(this))
		{
			int iPathTurns;
			if (generateIgnoreDangerPath(pLoopCity->plot(), &iPathTurns))
			{
				if (bAnyCity
				|| pLoopCity->findPopulationRank() >= iNumCities * 2 / 3
				&& pLoopCity->getPopulation() < GET_PLAYER(getOwner()).getTotalPopulation() * 2 / (3 * iNumCities))
				{
					const int iTurnsLeft = pLoopCity->getProductionTurnsLeft() - iPathTurns;

					const int iMinTurns = GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 50;

					if (iTurnsLeft > iMinTurns && iTurnsLeft > iBestValue)
					{
						iBestValue = iTurnsLeft;
						pBestPlot = pLoopCity->plot();
					}
				}
			}
		}
	}

	if (!pBestPlot)
	{
		return false;
	}
	if (atPlot(pBestPlot))
	{
		getGroup()->pushMission(MISSION_HURRY);
		return true;
	}
	if (generateSafePathforVulnerable(pBestPlot))
	{
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_WITH_CAUTION, false, false, MISSIONAI_HURRY, pBestPlot);
	}
	getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
	return true;
}

bool CvUnitAI::AI_hurryFood()
{
	PROFILE_FUNC();

	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	foreach_(const CvCity * cityX, GET_PLAYER(getOwner()).cities())
	{
		if (UnitTravelConnectivity::landUnitCanReach(this,cityX->plot())
		&& AI_plotValid(cityX->plot())
		// Hurry is possible
		&& canHurry(cityX->plot())
		&& !cityX->plot()->isVisibleEnemyUnit(this)
		// Nobody already hurrying
		&& GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(cityX->plot(), MISSIONAI_HURRY_FOOD, getGroup()) == 0
		&& (
			cityX->foodDifference() < 0
			||
			GC.getGame().getSorenRandNum(100, "AI hurry with food") > 75
			&&
			cityX->findPopulationRank() > GC.getWorldInfo(GC.getMap().getWorldSize()).getTargetNumCities()
			)
		)
		{
			int iPathTurns;
			// Get a safe path
			if (generateIgnoreDangerPath(cityX->plot(), &iPathTurns))
			{
				const int iTurnsLeft = cityX->getFoodTurnsLeft() - iPathTurns;

				// Worth actually going
				if (iTurnsLeft > (1 + GC.getGame().getGameSpeedType()) * 2 / 3 && iTurnsLeft > iBestValue)
				{
					iBestValue = iTurnsLeft;
					pBestPlot = cityX->plot();
				}
			}
		}
	}

	if (pBestPlot)
	{
		if (atPlot(pBestPlot))
		{
			getGroup()->pushMission(MISSION_HURRY_FOOD);
			return true;
		}
		if (generateSafePathforVulnerable(pBestPlot))
		{
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_WITH_CAUTION, false, false, MISSIONAI_HURRY_FOOD, pBestPlot);
		}
		getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
		return true;
	}
	return false;
}

bool CvUnitAI::AI_command()
{
	PROFILE_EXTRA_FUNC();

	if (!GC.getGame().isOption(GAMEOPTION_UNIT_GREAT_COMMANDERS)
	|| !getUnitInfo().isGreatGeneral()
	|| isCommander())
	{
		return false;
	}
	const int iNumCommanders = GET_PLAYER(getOwner()).getCommanders().size();

	bool bCommand = iNumCommanders < 2 + GET_PLAYER(getOwner()).getNumCities() / 5;

	if (!bCommand)
	{
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
		{
			if (GET_TEAM(GET_PLAYER((PlayerTypes)iPlayer).getTeam()).isAtWar(getTeam())
			&& iNumCommanders < static_cast<int>(GET_PLAYER((PlayerTypes)iPlayer).getCommanders().size()))
			{
				bCommand = true;
				break;
			}
		}
	}

	if (!bCommand && GET_TEAM(getTeam()).getAnyWarPlanCount(true) > iNumCommanders)
	{
		bCommand = true;
	}

	if (bCommand && plot()->getPlotCity())
	{
		const int iTotalThreat = std::max(1, GET_PLAYER(getOwner()).AI_getTotalAreaCityThreat(area(), NULL));
		const int iOurThreat = plot()->getPlotCity()->AI_cityThreat();

		// Unlikely to level the commander fast enough to be useful, leading troops will bring more immediate benefits
		const int iNumCities = std::max(1, GET_PLAYER(getOwner()).getNumCities());
		if (iOurThreat > 2 * iTotalThreat / iNumCities)
		{
			bCommand = false;
		}
	}
	if (bCommand)
	{
		setCommander(true);
		return true;
	}
	return false;
}

//bool CvUnitAI::AI_commod()
//{
//	PROFILE_EXTRA_FUNC();
//
//	if (!GC.getGame().isOption(GAMEOPTION_UNIT_GREAT_COMMODORES)
//	|| !getUnitInfo().isGreatAdmiral()
//	|| isCommmodore())
//	{
//		return false;
//	}
//	const int iNumCommodores = GET_PLAYER(getOwner()).getCommodores().size();
//
//	bool bCommand = iNumCommodores < 2 + GET_PLAYER(getOwner()).getNumCities() / 5;
//
//	if (!bCommand)
//	{
//		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
//		{
//			if (GET_TEAM(GET_PLAYER((PlayerTypes)iPlayer).getTeam()).isAtWar(getTeam())
//			&& iNumCommodores < static_cast<int>(GET_PLAYER((PlayerTypes)iPlayer).getCommodores().size()))
//			{
//				bCommand = true;
//				break;
//			}
//		}
//	}
//
//	if (!bCommand && GET_TEAM(getTeam()).getAnyWarPlanCount(true) > iNumCommodores)
//	{
//		bCommand = true;
//	}
//
//	if (bCommand && plot()->getPlotCity())
//	{
//		const int iTotalThreat = std::max(1, GET_PLAYER(getOwner()).AI_getTotalAreaCityThreat(area(), NULL));
//		const int iOurThreat = plot()->getPlotCity()->AI_cityThreat();
//
//		// Unlikely to level the commodore fast enough to be useful, leading troops will bring more immediate benefits
//		const int iNumCities = std::max(1, GET_PLAYER(getOwner()).getNumCities());
//		if (iOurThreat > 2 * iTotalThreat / iNumCities)
//		{
//			bCommand = false;
//		}
//	}
//	if (bCommand)
//	{
//		setCommodore(true);
//		return true;
//	}
//	return false;
//}

bool CvUnitAI::AI_AutomatedPillage(int iBonusValueThreshold)
{
	PROFILE_FUNC();

	FAssertMsg(isHuman(), "should not be called for AI's");

	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	CvPlot* pBestPillagePlot;
	CvPlot* endTurnPlot = NULL;
	int iPathTurns;
	int iValue;
	int iBestValue;
	int iI;

	iBestValue = 0;
	pBestPlot = NULL;
	pBestPillagePlot = NULL;

	for (iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndex(iI);

		if (AI_plotValid(pLoopPlot) && getGroup()->canPillage(pLoopPlot) && UnitTravelConnectivity::landUnitCanReach(this,pLoopPlot,true))
		{
			if (!pLoopPlot->isNPC() || !GET_PLAYER(getOwner()).isModderOption(MODDEROPTION_AUTO_PILLAGE_AVOID_BARBARIAN_CITIES))
			{
				if (potentialWarAction(pLoopPlot))
				{
					if (GET_PLAYER(getOwner()).isModderOption(MODDEROPTION_AUTO_PILLAGE_AVOID_ENEMY_UNITS) || !pLoopPlot->isVisibleEnemyUnit(this))
					{
						if (GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_PILLAGE, getGroup(), 1) == 0)
						{
							if (generatePath(pLoopPlot, 0, true, &iPathTurns))
							{
								iValue = AI_pillageValue(pLoopPlot, iBonusValueThreshold);

								iValue *= 1000;

								iValue /= (iPathTurns + 1);

								// if not at war with this plot owner, then devalue plot if we already inside this owner's borders
								// (because declaring war will pop us some unknown distance away)
								if (!isEnemy(pLoopPlot->getTeam()) && plot()->getTeam() == pLoopPlot->getTeam())
								{
									iValue /= 10;
								}

								if (iValue > iBestValue)
								{
									endTurnPlot = getPathEndTurnPlot();

									if (endTurnPlot == pLoopPlot || !exposedToDanger(endTurnPlot, 60))
									{
										iBestValue = iValue;
										pBestPlot = endTurnPlot;
										pBestPillagePlot = pLoopPlot;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestPillagePlot != NULL))
	{
		if (atPlot(pBestPillagePlot) && !isEnemy(pBestPillagePlot->getTeam()))
		{
			return false;
		}

		if (atPlot(pBestPillagePlot))
		{
			if (isEnemy(pBestPillagePlot->getTeam()))
			{
				getGroup()->pushMission(MISSION_PILLAGE, -1, -1, 0, false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
				return true;
			}
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
		}
	}

	return false;
}


void CvUnitAI::AI_SearchAndDestroyMove(bool bWithCommander)
{
	TARGET_DIAG_TIMER(SEARCH_DESTROY);
	TARGET_DIAG_COUNT(SEARCH_DESTROY_CALLS, 1);
	TARGET_DIAG_SEGMENTS(searchParts, SD_STATUS);
	PROFILE_FUNC();

	if (AI_groupSelectStatus())
	{
		return;
	}

	TARGET_DIAG_NEXT(searchParts, SD_WAIT);
	CvPlayerAI& player = GET_PLAYER(getOwner());
	MissionAITypes eMissionAIType = MISSIONAI_GROUP;

	if (!isHuman() && plot()->getOwner() == getOwner() && player.AI_unitTargetMissionAIs(this, &eMissionAIType, 1, getGroup(), 2) > 0)  //Calvitix, set to 2 turns, to facilitate escort to joint its hunter
	{
		// Wait for units which are joining our group this turn
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	TARGET_DIAG_NEXT(searchParts, SD_GROUP_DAMAGE);
	//	If we have any animal hangers-on and are in our territory drop them off
	if (getGroup()->countNumUnitAIType(UNITAI_SUBDUED_ANIMAL) > 0)
	{
		if (plot()->getOwner() == getOwner())
		{
			getGroup()->AI_separateAI(UNITAI_SUBDUED_ANIMAL);
		}
		else if (getGroup()->countNumUnitAIType(UNITAI_HUNTER_ESCORT) > 0)
		{
			getGroup()->AI_separateAI(UNITAI_HUNTER);
			if (getGroup()->countNumUnitAIType(UNITAI_HUNTER) == 0)
			{
				return; // The hunter wasn't the stack leader?
			}
		}
	}


	if (getGroup()->getWorstDamagePercent() > 0)
	{
		C2C_MISSION_TRACE(CvString::format("%S (%d) damaged at (%d,%d)...\n", getDescription().c_str(), m_iID, m_iX, m_iY).c_str());
		//	If there is an adjacent enemy seek safety before we heal
		if (exposedToDanger(plot(), 75))
		{
			C2C_MISSION_TRACE("	...plot is dangerous - seeking safety\n");
			if (AI_safety())
			{
				return;
			}
		}

		C2C_MISSION_TRACE("	...Try to heal\n");
		if (AI_heal())
		{
			C2C_MISSION_TRACE(CvString::format("	...healing at (%d,%d)\n", m_iX, m_iY).c_str());
			return;
		}

		if (getGroup()->getWorstDamagePercent() > 25)
		{
			//	Look for somewhere safer
			if (AI_safety(3))
			{
				return;
			}
		}
	}

	TARGET_DIAG_NEXT(searchParts, SD_CONTRACTS);
	//	If we have more than 3 animal hangers-on escort them back to our territory
	if (getGroup()->countNumUnitAIType(UNITAI_WORKER) + getGroup()->countNumUnitAIType(UNITAI_SUBDUED_ANIMAL) > 3 && plot()->getOwner() != getOwner())
	{
		if (AI_retreatToCity())
		{
			return;
		}
	}
	const bool bLookForWork = !bWithCommander && (GC.getGame().getGameTurn() % 2 == 0);

	if (bLookForWork && !isHuman() && getGroup()->getNumUnits() == 1)
	{
		//If is able to merge right now with an escort
		if (AI_groupMergeRange(UNITAI_HUNTER_ESCORT, 1, false, true, true))
		{
			//return;
			LOG_UNIT_BLOCK(3, {
				CvWString StrunitAIType = GC.getUnitAIInfo(AI_getUnitAIType()).getType();
				CvWString StrUnitName = m_szName;
				if (StrUnitName.length() == 0)
				{
					StrUnitName = getName(0).GetCString();
				}
				logBBAI("	Player %d Unit ID %d, %S of Type %S at (%d,%d) [stack size %d] merge with a Hunter Escort", getOwner(), getID(), StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), getGroup()->getNumUnits());
			});

			//if Hunter if not the group Head, trick to obtain it
			if (getGroup()->getNumUnits() == 2)
			{
				if (this != getGroup()->getHeadUnit())
				{
					CvSelectionGroup* myGroup = getGroup();
					this->joinGroup(NULL);
					this->joinGroup(myGroup);

					FAssert(this == getGroup()->getHeadUnit())
				}
			}

		}
		else
		{
			// If anyone is actively asking for a hunter that takes priority
			if (processContracts(HIGHEST_PRIORITY_ESCORT_PRIORITY))
			{
				if (gUnitLogLevel >= 3 && m_contractualState == CONTRACTUAL_STATE_FOUND_WORK)
				{
					logContractBroker(1, "	Hunter for player %d (%S) at (%d,%d) found contractual work",
						getOwner(),
						player.getCivilizationDescription(0),
						getX(),
						getY());
				}
				return;
			}
			if (gUnitLogLevel >= 3)
			{
				logContractBroker(1, "	Hunter for player %d (%S) at (%d,%d) found no urgent contractual work",
					getOwner(),
					player.getCivilizationDescription(0),
					getX(),
					getY());
			}
		}
	}

	TARGET_DIAG_NEXT(searchParts, SD_EARLY_HUNT);
	//Apparently minimum odds is a maximum odds threshold rather than minimum
	const int iMinimumOdds = isHuman() ? player.getModderOption(MODDEROPTION_AUTO_HUNT_MIN_COMBAT_ODDS) : (bWithCommander ? 90 : 70);

	if (AI_huntRange(1, iMinimumOdds, false))
	{
		return;
	}

	// Toffer - Non-optimal hunter is temporary, phase them out when appropriate.
	if (!bWithCommander && !isHuman() && m_pUnitInfo->getDefaultUnitAIType() != UNITAI_HUNTER)
	{
		const int iOwnedHunters = player.AI_totalAreaUnitAIs(area(), UNITAI_HUNTER);
		if (iOwnedHunters > 5)
		{
			AI_setUnitAIType(m_pUnitInfo->getDefaultUnitAIType());
			return;
		}
		if (iOwnedHunters > 1)
		{
			const int iNeededHunters = player.AI_neededHunters(area());
			const int iHunterDeficitPercent = (iNeededHunters <= iOwnedHunters) ? 0 : (iNeededHunters - iOwnedHunters) * 100 / iNeededHunters;

			if (iHunterDeficitPercent <= 80)
			{
				AI_setUnitAIType(m_pUnitInfo->getDefaultUnitAIType());
				return;
			}
		}
	}

	if (!bWithCommander && (!isHuman() || player.isModderOption(MODDEROPTION_AUTO_HUNT_RETURN_FOR_UPGRADES)))
	{
		if (AI_travelToUpgradeCity())
		{
			return;
		}
	}

	{
		TARGET_DIAG_NEXT(searchParts, SD_ESCORT);
		// Get the proper accompaniment
		const bool bContractEscort = (
			!bLookForWork && !isHuman() && !isCargo()
			&& getGroup()->countNumUnitAIType(UNITAI_HUNTER_ESCORT) < 1
			&& player.hasTrainableUnitForAI(UNITAI_HUNTER_ESCORT)
		);
		if (bContractEscort)
		{
			player.getContractBroker().advertiseWork
			(
				HIGHEST_PRIORITY_ESCORT_PRIORITY,
				NO_UNITCAPABILITIES,
				getX(), getY(),
				this, UNITAI_HUNTER_ESCORT
			);

			LOG_UNIT_BLOCK(3, {
				CvWString StrunitAIType = GC.getUnitAIInfo(AI_getUnitAIType()).getType();
				CvWString StrUnitName = m_szName;
				if (StrUnitName.length() == 0)
				{
					StrUnitName = getName(0).GetCString();
				}
				logBBAI("	Player %d Unit ID %d, %S of Type %S at (%d,%d) [stack size %d] requests hunter escort at priority %d", getOwner(), getID(), StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), getGroup()->getNumUnits(), HIGHEST_PRIORITY_ESCORT_PRIORITY);
			});
			// Limited operations gravitating close to borders while waiting.
			if (exposedToDanger(plot(), 90))
			{
				if (AI_safety())
				{
					return;
				}
			}
			else if (plot()->getOwner() == getOwner())
			{
				if (AI_huntRange(4, iMinimumOdds, false))
				{
					return;
				}
				if (AI_moveToBorders())
				{
					return;
				}
			}
			else
			{
				if (AI_huntRange(1, iMinimumOdds, false))
				{
					return;
				}
				if (GC.getGame().getSorenRandNum(10, "keep close to home") < 4)
				{
					if (AI_reachHome(false, 6))
					{
						return;
					}
				}
			}
			if (AI_explore())
			{
				return;
			}
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	TARGET_DIAG_NEXT(searchParts, SD_LATE_HUNT);
	//	If we have animal hangers-on escort them back to our territory if it is not too far
	if ((getGroup()->countNumUnitAIType(UNITAI_SUBDUED_ANIMAL) > 0 || getGroup()->countNumUnitAIType(UNITAI_WORKER) > 0) && plot()->getOwner() != getOwner())
	{
		if (AI_reachHome(false, 4))
		{
			return;
		}
	}

	if (AI_huntRange(1, iMinimumOdds, false))
	{
		return;
	}

	if (AI_goody(4))
	{
		return;
	}

	if (AI_huntRange(5, iMinimumOdds, false))
	{
		return;
	}

	// Toffer - Should change this to scrap the lowest level hunters first, perhaps in the player objects doTurn() routine.
	if (!bWithCommander && !isHuman()
	&& player.AI_isFinancialTrouble()
	&& player.getUnitUpkeepNet(isMilitaryBranch(), getUpkeep100()) > 0)
	{
		const int iNeededHunters = player.AI_neededHunters(area());
		const int iHasHunters = player.AI_totalAreaUnitAIs(area(), UNITAI_HUNTER);

		if (iHasHunters > iNeededHunters)
		{
			if (gUnitLogLevel >= 2)
			{
				logBBAI("%S has %d hunters (%d needed) - scrapping", player.getCivilizationDescription(0), iHasHunters, iNeededHunters);
			}
			scrap();
			return;
		}
	}

	TARGET_DIAG_NEXT(searchParts, SD_EXPLORE);
	if (AI_refreshExploreRange(3, true))
	{
		return;
	}

	if (AI_moveToBorders())
	{
		return;
	}

	if (AI_patrol())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

bool CvUnitAI::AI_huntRange(int iRange, int iOddsThreshold, bool bStayInBorders, int iMinValue)
{
	PROFILE_FUNC();

	iOddsThreshold = range(iOddsThreshold, 1, 99);

	const bool bCanCaptureCities =
		getGroup()->getAutomateType() == AUTOMATE_HUNT && !GET_PLAYER(getOwner()).isModderOption(MODDEROPTION_AUTO_HUNT_NO_CITY_CAPTURING) ||
		getGroup()->getAutomateType() == AUTOMATE_BORDER_PATROL && !GET_PLAYER(getOwner()).isModderOption(MODDEROPTION_AUTO_PATROL_NO_CITY_CAPTURING);

	int bestScore = iMinValue;
	CvPlot* bestTargetPlot = NULL;

	foreach_(const CvPlot * plotX, plot()->rect(iRange, iRange))
	{

        if (AI_getUnitAIType() == UNITAI_HUNTER && getGroup()->getAutomateType() == AUTOMATE_HUNT)
        {
            bool hasAnimal = false;
            foreach_(CvUnit * pUnit, plotX->units())
            {
                if (pUnit->isAnimal())
                {
                    hasAnimal = true;
                    break;
                }
            }
            if (!hasAnimal)
                continue;
        }

		if (!atPlot(plotX)
            && (!bStayInBorders || plotX->getOwner() == getOwner())
            && (!plotX->isCity() || bCanCaptureCities)
            && AI_plotValid(plotX)
            && plotX->isVisibleEnemyUnit(this)
            && getGroup()->canEnterPlot(plotX, true)
            && (getGroup()->getAutomateType() != AUTOMATE_HUNT
                   || plotX->getOwner() == getOwner()
                   || plotX->getOwner() == NO_PLAYER)
            && (!isAnimal() || AI_getUnitAIType() == UNITAI_HUNTER)
        )
		{
			const int attackOdds = getGroup()->AI_attackOdds(plotX, true);

			// Do not search paths to forbidden territory, poor odds or a target that
			// cannot beat a previously reachable choice. Visibility and the search
			// rectangle remain unchanged; hunters gain no extra map knowledge.
			if (attackOdds > bestScore
				&& attackOdds >= (isHuman() ? iOddsThreshold : AI_finalOddsThreshold(plotX, iOddsThreshold))
				&& generatePath(plotX, 0, true, nullptr, iRange))
			{
				bestScore = attackOdds;
				bestTargetPlot = getPathEndTurnPlot();
			}
		}
	}

	if (bestTargetPlot)
	{
		FAssert(!atPlot(bestTargetPlot));
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, bestTargetPlot->getX(), bestTargetPlot->getY(), MOVE_DIRECT_ATTACK, false, false);
	}
	return false;
}

void CvUnitAI::AI_cityDefense()
{
	int iMinimumOdds = GET_PLAYER(getOwner()).getModderOption(MODDEROPTION_AUTO_DEFENSE_MIN_COMBAT_ODDS);
	bool bCanLeaveCity = GET_PLAYER(getOwner()).isModderOption(MODDEROPTION_AUTO_DEFENSE_CAN_LEAVE_CITY);

	if (AI_returnToBorders())
	{
		return;
	}

	if (AI_guardCityBestDefender())
	{
		return;
	}

	if (AI_guardCityMinDefender(false))
	{
		return;
	}

	if (AI_leaveAttack(2, iMinimumOdds, 100))
	{
		return;
	}

	if (AI_leaveAttack(3, iMinimumOdds + 10, 130))
	{
		return;
	}

	if (AI_guardCity(bCanLeaveCity, bCanLeaveCity))
	{
		return;
	}

	if (AI_heal())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


// AI don't use this, only human automated units does.
void CvUnitAI::AI_borderPatrol()
{
	PROFILE_FUNC();

	if (!(isMadeAttack() && canAttackNow()) && AI_returnToBorders())
	{
		return;
	}

	if (AI_heal())
	{
		return;
	}
	const bool bStayInBorders = !GET_PLAYER(getOwner()).isModderOption(MODDEROPTION_AUTO_PATROL_CAN_LEAVE_BORDERS);
	const int iMinimumOdds = GET_PLAYER(getOwner()).getModderOption(MODDEROPTION_AUTO_PATROL_MIN_COMBAT_ODDS);

	if (AI_huntRange(1, iMinimumOdds, bStayInBorders))
	{
		return;
	}

	if (AI_huntRange(2, iMinimumOdds, bStayInBorders))
	{
		return;
	}

	if (AI_huntRange(3, iMinimumOdds, true))
	{
		return;
	}

	if (AI_huntRange(5, iMinimumOdds + 5, true))
	{
		return;
	}

	if (AI_patrolBorders())
	{
		return;
	}

	if (AI_huntRange(7, iMinimumOdds + 10, true))
	{
		return;
	}

	if (AI_huntRange(12, iMinimumOdds + 15, true))
	{
		return;
	}

	if (!bStayInBorders)
	{
		if (AI_patrol())
		{
			return;
		}
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

bool CvUnitAI::AI_returnToBorders()
{
	PROFILE_FUNC();

	//Allows the unit to be a maximum of 2 tiles from our borders before ordering him back
	if (plot()->getOwner() == getOwner())
	{
		return false;
	}
	foreach_(const CvPlot * pAdjacentPlot, plot()->adjacent())
	{
		if (pAdjacentPlot->getOwner() == getOwner())
		{
			return false;
		}
		if (algo::any_of(pAdjacentPlot->adjacent(), CvPlot::fn::getOwner() == getOwner()))
		{
			return false;
		}
	}
	int iPathTurns;
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	CvReachablePlotSet plotSet(getGroup(), 0, MAX_INT);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		const CvPlot* plotX = itr.plot();
		if (UnitTravelConnectivity::landUnitCanReach(this,plotX,true)
		&& plotX->getOwner() == getOwner()
		&& !plotX->isVisibleEnemyUnit(this)
		&& generatePath(plotX, 0, true, &iPathTurns))
		{
			const int iValue = 1000 / (iPathTurns + 1);

			if (iValue > iBestValue)
			{
				const CvPlot* endTurnPlot = getPathEndTurnPlot();

				if (endTurnPlot == plotX || !exposedToDanger(endTurnPlot, 65))
				{
					iBestValue = iValue;
					pBestPlot = endTurnPlot;
				}
			}
		}
	}
	if (pBestPlot)
	{
		if (!isWaitingOnUnitAIAny())
		{
			FAssert(!atPlot(pBestPlot));
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
		}
		return getGroup()->pushMissionInternal(MISSION_SKIP);
	}
	return false;
}


//	AI_headToBorder is used to cause a unit within our borders to move towards them.  It will
//	prefer borders with players we are at war with, over borders to neutral, over borders
//	to players we are not at war with
bool CvUnitAI::AI_moveToBorders()
{
	PROFILE_FUNC();

	const CvPlot* pBestPlot = NULL;
	const CvPlot* endTurnPlot = NULL;
	int		iDistance = 1;
	int		iBestValue = 0;
	bool	bValid;

	//	Just intended for units inside our territory to head out
	if (plot()->getOwner() != getOwner())
	{
		return false;
	}

	// Already at a land border: there is no outward positioning task left.
	// The perimeter scan starts at radius one and cannot select this tile.
	foreach_(const CvPlot* borderPlot, plot()->adjacent())
	{
		if (!borderPlot->isWater() && borderPlot->getOwner() != getOwner()) return false;
	}

	do
	{
		bValid = false;

		//	Examine the ring of plots iDistance away from us (this is a square)
		for (int iPerimeter = -iDistance; iPerimeter < iDistance; iPerimeter++)
		{
			for (int iSide = 0; iSide < 4; iSide++)
			{
				int iX = getX();
				int iY = getY();

				switch (iSide)
				{
				case 0:
					//	North side
					iX = iX - iPerimeter;
					iY = iY + iDistance;
					break;
				case 1:
					//	East side
					iX = iX + iDistance;
					iY = iY + iPerimeter;
					break;
				case 2:
					//	South side
					iX = iX + iPerimeter;
					iY = iY - iDistance;
					break;
				case 3:
					//	West side
					iX = iX - iDistance;
					iY = iY - iPerimeter;
					break;
				}

				const CvPlot* pLoopPlot = GC.getMap().plot(iX, iY);

				if (pLoopPlot != NULL)
				{
					if (AI_plotValid(pLoopPlot) && UnitTravelConnectivity::landUnitCanReach(this,pLoopPlot,true))
					{
						if (pLoopPlot->getOwner() == getOwner())
						{
							//	Still within our territory somewhere on this ring
							bValid = true;

							int iValue = 0;

							foreach_(const CvPlot * pAdjacentPlot, pLoopPlot->adjacent())
							{
								if (!pAdjacentPlot->isWater() && pAdjacentPlot->getOwner() != getOwner())
								{
									iValue += GC.getGame().getSorenRandNum(300, "AI Move to Border");

									if (NO_PLAYER == pAdjacentPlot->getOwner())
									{
										iValue += 100;
									}
									else if (GET_TEAM(getTeam()).isAtWar(pAdjacentPlot->getTeam()))
									{
										iValue += 200 + GC.getGame().getSorenRandNum(100, "AI Move to Border2");
									}
								}
							}

							if (iValue * 10 > iBestValue)
							{
								int iPathTurns;

								if (generatePath(pLoopPlot, 0, true, &iPathTurns))
								{
									iValue = (iValue * 50) / (iPathTurns + 5);

									if (iValue > iBestValue)
									{
										endTurnPlot = getPathEndTurnPlot();
										pBestPlot = pLoopPlot;
										iBestValue = iValue;
									}
								}
							}
						}
					}
				}
			}
		}

		if (iDistance++ > std::max(GC.getMap().getGridHeight(), GC.getMap().getGridWidth()))
		{
			break;
		}
	} while (bValid);

	//	If we're already at the best border plot this mission isn't useful
	if (pBestPlot != NULL && !atPlot(pBestPlot) && endTurnPlot != NULL)
	{
		if (!isWaitingOnUnitAIAny())
		{
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY());
		}
		return getGroup()->pushMissionInternal(MISSION_SKIP);
	}
	return false;
}

//AI_patrolBorders relys heavily on the units current facing direction to determine where the next
//move should be. For example, units facing north should not turn around south, or any southerly
//direction (southwest, southeast) to patrol, since that would cause them to move back and forth
//in a nonsensical manner. Instead, they should appear to be intelligent, and move around
//the players borders in a circuit, without turning back or leaving the boundries of
//the cultural borders. This is not in fact the most optimal method of patroling, but it
//produces results that make sense to the average human, which is the actual goal, since
//the AI actually never use this function, only automated human units do.
bool CvUnitAI::AI_patrolBorders()
{
	PROFILE_FUNC();

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	const int iSearchRange = baseMoves();

	foreach_(const CvPlot * pLoopPlot, plot()->rect(iSearchRange, iSearchRange))
	{
		if (canEnterPlot(pLoopPlot, MoveCheck::IgnoreLoad))
		{
			const DirectionTypes eNewDirection = estimateDirection(plot(), pLoopPlot);
			int iValue = GC.getGame().getSorenRandNum(10000, "AI Border Patrol");
			if (pLoopPlot->isBorder(true))
			{
				iValue += GC.getGame().getSorenRandNum(10000, "AI Border Patrol");
			}
			else if (pLoopPlot->isBorder(false))
			{
				iValue += GC.getGame().getSorenRandNum(5000, "AI Border Patrol");
			}
			//Avoid heading backwards, we want to circuit our borders, if possible.
			if (eNewDirection == getOppositeDirection(getFacingDirection(false)))
			{
				iValue /= 25;
			}
			else if (isAdjacentDirection(getOppositeDirection(getFacingDirection(false)), eNewDirection))
			{
				iValue /= 10;
			}
			if (pLoopPlot->getOwner() != getOwner())
			{
				if (GET_PLAYER(getOwner()).isModderOption(MODDEROPTION_AUTO_PATROL_CAN_LEAVE_BORDERS))
				{
					iValue = -1;
				}
				else
				{
					iValue /= 10;
				}
			}
			if (getDomainType() == DOMAIN_LAND && pLoopPlot->isWater() || getDomainType() == DOMAIN_SEA && !pLoopPlot->isWater())
			{
				iValue /= 10;
			}
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestPlot = pLoopPlot;
			}
		}
	}
	if (pBestPlot != NULL)
	{
		if (!isWaitingOnUnitAIAny())
		{
			FAssert(!atPlot(pBestPlot));
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
		}
		return getGroup()->pushMissionInternal(MISSION_SKIP);
	}
	return false;
}

void CvUnitAI::AI_AutomatedpillageMove()
{
	PROFILE_FUNC();

	if (AI_heal(30, 1))
	{
		return;
	}
	if (AI_pillageRange(6, 20))
	{
		return;
	}

	if (AI_pillageRange(3, 11))
	{
		return;
	}

	if (AI_pillageRange(1))
	{
		return;
	}

	if (AI_heal(50, 3))
	{
		return;
	}

	if (!isEnemy(plot()->getTeam()))
	{
		if (AI_heal())
		{
			return;
		}
	}

	if ((area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) || isEnemy(plot()->getTeam()))
	{
		if (AI_pillage(20))
		{
			return;
		}
	}

	if (AI_heal())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_autoAirStrike()
{
	PROFILE_FUNC();

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	CvArea* pArea = area();

	//Heal
	if (getDamage() > 0 && 100 * getHP() / getMaxHP() < 50)
	{
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	// Attack the invaders!
	if (AI_defendBaseAirStrike())
	{
		return;
	}

	//Attack enemies in range
	if (AI_defensiveAirStrike())
	{
		return;
	}

	//Attack anyone
	if (AI_airStrike())
	{
		return;
	}

	if (kPlayer.isModderOption(MODDEROPTION_AUTO_AIR_CAN_REBASE))
	{
		// If no targets, no sense staying in risky place
		if (AI_airOffensiveCity())
		{
			return;
		}

		if (canAirDefend())
		{
			if (AI_airDefensiveCity())
			{
				return;
			}
		}

		if (healTurns(plot()) > 1)
		{
			// If very damaged, no sense staying in risky place
			if (AI_airOffensiveCity())
			{
				return;
			}

			if (canAirDefend())
			{
				if (AI_airDefensiveCity())
				{
					return;
				}
			}
		}
	}

	if (kPlayer.isModderOption(MODDEROPTION_AUTO_AIR_CAN_DEFEND))
	{
		int iAttackValue = kPlayer.AI_unitValue(getUnitType(), UNITAI_ATTACK_AIR, pArea);
		int iDefenseValue = kPlayer.AI_unitValue(getUnitType(), UNITAI_DEFENSE_AIR, pArea);
		if (iDefenseValue > iAttackValue)
		{
			if (kPlayer.AI_bestAreaUnitAIValue(UNITAI_ATTACK_AIR, pArea) > iAttackValue)
			{
				AI_setUnitAIType(UNITAI_DEFENSE_AIR);
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}
		}
	}


	bool bDefensive = false;
	if (pArea != NULL)
	{
		bDefensive = pArea->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE;
	}
	if (kPlayer.isModderOption(MODDEROPTION_AUTO_AIR_CAN_REBASE) && kPlayer.isModderOption(MODDEROPTION_AUTO_AIR_CAN_DEFEND))
	{
		if (GC.getGame().getSorenRandNum(bDefensive ? 3 : 6, "AI Air Attack Move") == 0)
		{
			if (AI_defensiveAirStrike())
			{
				return;
			}
		}
	}

	if (kPlayer.isModderOption(MODDEROPTION_AUTO_AIR_CAN_REBASE))
	{
		if (GC.getGame().getSorenRandNum(4, "AI Air Attack Move") == 0)
		{
			// only moves unit in a fort
			if (AI_travelToUpgradeCity())
			{
				return;
			}
		}
	}

	// Support ground attacks
	if (canAirBomb())
	{
		if (AI_airBombDefenses())
		{
			return;
		}

		if (AI_airBombPlots())
		{
			return;
		}
	}

	if (AI_airStrike())
	{
		return;
	}

	if (AI_airBombCities())
	{
		return;
	}

	if (canAirDefend() && kPlayer.isModderOption(MODDEROPTION_AUTO_AIR_CAN_DEFEND))
	{
		if (bDefensive || GC.getGame().getSorenRandNum(2, "AI Air Attack Move") == 0)
		{
			getGroup()->pushMission(MISSION_AIRPATROL);
			return;
		}
	}

	if (canRecon())
	{
		if (AI_exploreAir())
		{
			return;
		}
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_airBombCities()
{
	AirInterceptionCandidates interceptors(*this);
	PROFILE_EXTRA_FUNC();
	//PROFILE_FUNC();

	const int iSearchRange = airRange();

	int iBestValue = (isSuicide() && m_pUnitInfo->getProductionCost() > 0) ? (5 * m_pUnitInfo->getProductionCost()) / 6 : 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, plot()->rect(iSearchRange, iSearchRange))
	{
		if (canEnterPlot(pLoopPlot, MoveCheck::Attack))
		{
			int iValue = 0;
			const int iPotentialAttackers = pLoopPlot->getNumVisiblePotentialEnemyDefenders(this);

			if (iPotentialAttackers > 0)
			{
				const CvUnit* pDefender = pLoopPlot->getBestDefender(NO_PLAYER, getOwner(), this, true);

				FAssert(pDefender != NULL && pDefender->canDefend());

				if (pDefender != NULL && pDefender->canDefend())
				{
					// XXX factor in air defenses...

					const int iDamage = airCombatDamage(pDefender);

					iValue = std::max(0, (std::min((pDefender->getDamage() + iDamage), airCombatLimit(pDefender)) - pDefender->getDamage()));

					iValue += ((((iDamage * collateralDamage()) / 100) * std::min((pLoopPlot->getNumVisiblePotentialEnemyDefenders(this) - 1), collateralDamageMaxUnits())) / 2);

					iValue *= (3 + iPotentialAttackers);
					iValue /= 4;

					const CvUnit* pInterceptor = interceptors.bestInterceptor(pLoopPlot);

					if (pInterceptor != NULL)
					{
						int iInterceptProb = isSuicide() ? 100 : pInterceptor->currInterceptionProbability();

						iInterceptProb *= std::max(0, (100 - evasionProbability()));
						iInterceptProb /= 100;

						iValue *= std::max(0, 100 - iInterceptProb / 2);
						iValue /= 100;
					}

					if (pLoopPlot->isWater())
					{
						iValue *= 2;
					}
					if (pLoopPlot->isCity())
					{
						iValue *= 2;
					}

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = pLoopPlot;
						FAssert(!atPlot(pBestPlot));
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));
		return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}

	return false;
}


void CvUnitAI::AI_shadowMove()
{
	PROFILE_FUNC();

	CvUnit* pTarget = getShadowUnit();
	FAssertMsg(pTarget != NULL, "Should be Shading a Unit!");

	if (AI_protectTarget(pTarget))
	{
		return;
	}

	if (AI_moveToTarget(pTarget))
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

bool CvUnitAI::AI_moveToTarget(const CvUnit* pTarget)
{
	PROFILE_FUNC();

	if (atPlot(pTarget->plot()))
	{
		return false;
	}

	int iPathTurns;
	if (!isWaitingOnUnitAIAny() && generatePath(pTarget->plot(), 0, true, &iPathTurns))
	{
		const CvPlot* endTurnPlot = getPathEndTurnPlot();
		if (endTurnPlot != NULL)
		{
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY());
		}
	}

	return false;
}

bool CvUnitAI::AI_protectTarget(const CvUnit* pTarget)
{
	PROFILE_FUNC();

	//const CvPlot* endTurnPlot = NULL;
	int iPathTurns;

	const int iSearchRange = baseMoves();

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	// The signed balance can be negative (friendly defenders), or even zero
	// while an enemy remains nearby. Ask for actual danger before either
	// scanning attack targets or skipping protection. Respect visibility.
	if (!GET_PLAYER(getOwner()).AI_getAnyPlotDanger(pTarget->plot(), 1, false))
	{
		return false;
	}

	const int iDanger = GET_PLAYER(getOwner()).AI_getPlotDanger(pTarget->plot(), 1, false);

	//Lots of Danger, Move Ontop of Target to protect it
	if (iDanger > getGroup()->getNumUnits())
	{
		if (generatePath(pTarget->plot(), 0, true, &iPathTurns))
		{
			const CvPlot* endTurnPlot = getPathEndTurnPlot();
			getGroup()->pushMission(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY());
			return true;
		}
	}

	//Only minimal enemy targets, move to kill them if possible
	else
	{
		foreach_(const CvPlot * pLoopPlot, plot()->rect(iSearchRange, iSearchRange))
		{
			if (AI_plotValid(pLoopPlot))
			{
				if (pLoopPlot->isVisibleEnemyUnit(this))
				{
					if (!atPlot(pLoopPlot) && canEnterPlot(pLoopPlot, MoveCheck::Attack) && generatePath(pLoopPlot, 0, true, &iPathTurns, 1))
					{
						if (pLoopPlot->getNumVisiblePotentialEnemyDefenders(this) <= getGroup()->getNumUnits())
						{
							if (pLoopPlot->getNumVisibleAdjacentEnemyDefenders(this) <= ((getGroup()->getNumUnits() * 3) / 2))
							{
								const int iValue = getGroup()->AI_attackOdds(pLoopPlot, true);

								if (iValue >= AI_finalOddsThreshold(pLoopPlot, 65))
								{
									if (iValue > iBestValue)
									{
										iBestValue = iValue;
										pBestPlot = getPathEndTurnPlot();
										FAssert(!atPlot(pBestPlot));
									}
								}
							}
						}
					}
				}
			}
		}
	}

	//Not possible to kill enemies, retreat to target
	if (pBestPlot == NULL)
	{
		if (atPlot(pTarget->plot()))
		{
			getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
		else if (!isWaitingOnUnitAIAny() && generatePath(pTarget->plot(), 0, true, &iPathTurns))
		{
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, pTarget->getX(), pTarget->getY());
		}
		else
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
			return true;
		}
	}
	else
	{
		getGroup()->pushMission(MISSION_SKIP);
		return true;
	}
}

bool CvUnitAI::AI_joinMilitaryCity(bool bNaval)
{
	PROFILE_FUNC();

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	SpecialistTypes eBestSpecialist = NO_SPECIALIST;

	foreach_(CvCity * pLoopCity, GET_PLAYER(getOwner()).cities())
	{
		if (UnitTravelConnectivity::landUnitCanReach(this,pLoopCity->plot()))
		{
			const CvPlot* plot = pLoopCity->plot();

			if (AI_plotValid(plot)
			&& (bNaval ? pLoopCity->AI_isNavalMilitaryProductionCity() : pLoopCity->AI_isMilitaryProductionCity())
			&& !plot->isVisibleEnemyUnit(this)
			&& generateIgnoreDangerPath(plot))
			{
				for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
				{
					if (canJoin(plot, (SpecialistTypes)iI))
					{
						const int iValue = pLoopCity->AI_specialistValue((SpecialistTypes)iI, pLoopCity->AI_avoidGrowth(), false);
						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = getPathEndTurnPlot();
							eBestSpecialist = (SpecialistTypes)iI;
						}
					}
				}
			}
		}
	}

	if (pBestPlot)
	{
		if (atPlot(pBestPlot))
		{
			getGroup()->pushMission(MISSION_JOIN, eBestSpecialist);
			return true;
		}
		if (generateSafePathforVulnerable(pBestPlot))
		{
			const CvPlot* plotX = getPathEndTurnPlot();
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, plotX->getX(), plotX->getY(), MOVE_WITH_CAUTION);
		}
		getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
		return true;
	}

	return false;
}

bool CvUnitAI::AI_isPlotWellDefended(const CvPlot* pPlot, bool bIncludeAdjacent, int iOddsOfDefeat) const
{
	PROFILE_FUNC();

	const int iOurOffense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), bIncludeAdjacent, true, false, true);
	const int iEnemyDefense = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(pPlot, bIncludeAdjacent, true, false);

	return iOddsOfDefeat * iOurOffense < 100 * iEnemyDefense;
}


void CvUnitAI::AI_flushValueCache()
{
	DefenseNeedEvents::changed();
	if(getOwner()!=NO_PLAYER && !isTempUnit())GET_PLAYER(getOwner()).getContractBroker().noteDefenseGroupChanged(getGroup());
	m_iGenericValue = -1;
}

//	Assess the value of a unit without the context of any specific battle/scenario
//	This is its max strength, modified by promotions
int	CvUnitAI::AI_genericUnitValueTimes100(UnitValueFlags eFlags) const
{
	PROFILE_FUNC();

	if (m_iGenericValue == -1 || m_eGenericValueFlagsCached != eFlags)
	{
		int	iResult = 100 * baseCombatStrNonGranular();

		foreach_(const STD_PAIR(PromotionTypes, PromotionKeyedInfo) & keyedInfo, getPromotionKeyedInfo())
		{
			if (keyedInfo.second.m_bHasPromotion)
			{
				const CvPromotionInfo& kPromotion = GC.getPromotionInfo(keyedInfo.first);
				bool bPromotionHasAccountedValue = false;

				//	Generic strength multiplier
				if (kPromotion.getCombatPercent() != 0)
				{
					iResult = (iResult * (100 + kPromotion.getCombatPercent())) / 100;

					bPromotionHasAccountedValue |= (kPromotion.getCombatPercent() > 0);
				}

				//	Unit combat modifiers
				// Null modifier arrays are already indexed by the info object.
				if (kPromotion.isAnyUnitCombatModifierPercent())
				{
					for (int iJ = 0; iJ < GC.getNumUnitCombatInfos(); iJ++)
					{
						if (kPromotion.getUnitCombatModifierPercent((UnitCombatTypes)iJ) != 0)
						{
							//	Future - make this adaptive to the area's known enemy units
							//	Note - the scaling factor of 5 (implicit in the 500) is because
							//	although there are actually 40 or so unit combat types only a small
							//	fraction are in wide use or appear in promotions in practice.  Once this
							//	is normalized for actual occurance of units that fudge factor won't be needed
							int	iUnitCombatWeight = 500 / GC.getNumUnitCombatInfos();

							iResult = (iResult * (100 + (iUnitCombatWeight * kPromotion.getUnitCombatModifierPercent((UnitCombatTypes)iJ)) / 100)) / 100;

							bPromotionHasAccountedValue |= (kPromotion.getUnitCombatModifierPercent((UnitCombatTypes)iJ) > 0);
						}
					}
				}

				//	First strikes
				if (kPromotion.getFirstStrikesChange() != 0)
				{
					int iFirstStrikesWeight = 15;

					iResult = (iResult * (100 + iFirstStrikesWeight * kPromotion.getFirstStrikesChange())) / 100;

					bPromotionHasAccountedValue |= (kPromotion.getFirstStrikesChange() > 0);
				}

				//	Tile and adjacent tile healing are considered with any flags
				int iUCHeal = 0;
				int iUCAdjHeal = 0;
				for (int iK = 0; iK < kPromotion.getNumHealUnitCombatChangeTypes(); iK++)
				{
					iUCHeal += kPromotion.getHealUnitCombatChangeType(iK).iHeal;
					iUCAdjHeal += kPromotion.getHealUnitCombatChangeType(iK).iAdjacentHeal;
				}
				if (kPromotion.getSameTileHealChange() != 0 || iUCHeal > 0)
				{

					int iSameTileHealWeight = 75;

					iResult = ((iResult * (100 + (iSameTileHealWeight * ((kPromotion.getSameTileHealChange() + iUCHeal) / 100)) / 100)));

					bPromotionHasAccountedValue |= ((kPromotion.getSameTileHealChange() + iUCHeal) > 0);
				}
				if (kPromotion.getAdjacentTileHealChange() != 0 || iUCAdjHeal > 0)
				{
					int iAdjacentTileHealWeight = 200;

					iResult = (iResult * (100 + (iAdjacentTileHealWeight * (kPromotion.getAdjacentTileHealChange() + iUCAdjHeal)) / 100)) / 100;

					bPromotionHasAccountedValue |= ((kPromotion.getAdjacentTileHealChange() + iUCAdjHeal) > 0);
				}

				//Team Project (2)
								//	Survival Chance multiplier
				if (kPromotion.getSurvivorChance() != 0)
				{
					iResult = (iResult * (100 + kPromotion.getSurvivorChance())) / 100;

					bPromotionHasAccountedValue |= (kPromotion.getSurvivorChance() > 0);
				}

				//Team Project (3)
								//	Capture Modifier
				if (kPromotion.getCaptureProbabilityModifierChange() != 0)
				{
					iResult = (iResult * (100 + (kPromotion.getCaptureProbabilityModifierChange() * 4))) / 100;

					bPromotionHasAccountedValue |= (kPromotion.getCaptureProbabilityModifierChange() > 0);
				}

				//	Capture Resistance Modifier
				if (kPromotion.getCaptureResistanceModifierChange() != 0)
				{
					iResult = (iResult * (100 + (kPromotion.getCaptureResistanceModifierChange() * 3))) / 100;

					bPromotionHasAccountedValue |= (kPromotion.getCaptureResistanceModifierChange() > 0);
				}

				if ((eFlags & UNITVALUE_FLAGS_DEFENSIVE) != 0)
				{
					//	Terrain defense
					// Null modifier arrays are already indexed by the info object.
					if (kPromotion.isAnyTerrainDefensePercent() || kPromotion.getIgnoreTerrainDamage() >= 0)
					{
						for (int iJ = 0; iJ < GC.getNumTerrainInfos(); iJ++)
						{
							if (kPromotion.getTerrainDefensePercent((TerrainTypes)iJ) != 0 ||
								 kPromotion.getIgnoreTerrainDamage() == iJ)
							{
								int iNumRevealedAreaTiles = std::max(1, area()->getNumRevealedTiles(getTeam()));
								int	iNumRevealedAreaThisTerrain = area()->getNumRevealedTerrainTiles(getTeam(), (TerrainTypes)iJ);

								//	Note - used 1000 multiplier here for better granularity because there are
								//	a lot of terrain types.  We multiply the actual ratio by 2 because in practise the AI will
								//	deploy the units to appropriate terrains/features when it can so the the effective value is
								//	higher than the raw proportion would apply
								int	iTerrainWeight = (2 * 1000 * iNumRevealedAreaThisTerrain) / iNumRevealedAreaTiles;

								iResult = (iResult * (10000 + (kPromotion.getTerrainDefensePercent((TerrainTypes)iJ) * iTerrainWeight) / 10)) / 10000;

								if (kPromotion.getIgnoreTerrainDamage() == iJ)
								{
									iResult = (iResult * (10000 + (50 * iTerrainWeight) / 10)) / 10000;
									bPromotionHasAccountedValue = true;
								}
							}
						}
					}
					//	Feature defense
					// Null modifier arrays are already indexed by the info object.
					if (kPromotion.isAnyFeatureDefensePercent())
					{
						for (int iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
						{
							if (kPromotion.getFeatureDefensePercent((FeatureTypes)iJ) != 0)
							{
								int iNumRevealedAreaTiles = std::max(1, area()->getNumRevealedTiles(getTeam()));
								int	iNumRevealedAreaThisFeature = area()->getNumRevealedFeatureTiles(getTeam(), (FeatureTypes)iJ);

								//	Note - used 1000 multiplier here for better granularity because there are
								//	a lot of feature types.  We multiply the actual ratio by 2 because in practise the AI will
								//	deploy the units to appropriate terrains/features wehn it can so the the effective value is
								//	higher than the raw proportion would apply
								int	iFeatureWeight = (2 * 1000 * iNumRevealedAreaThisFeature) / iNumRevealedAreaTiles;

								iResult = (iResult * (10000 + (kPromotion.getFeatureDefensePercent((FeatureTypes)iJ) * iFeatureWeight) / 10)) / 10000;

								bPromotionHasAccountedValue |= (kPromotion.getFeatureDefensePercent((FeatureTypes)iJ) > 0);
							}
						}
					}
					//	Hills defense
					if (kPromotion.getHillsDefensePercent() != 0)
					{
						int iHillsWeight = 30;	//	Crudely assume 30% hills for now

						iResult = (iResult * (100 + (kPromotion.getHillsDefensePercent() * iHillsWeight) / 100)) / 100;

						bPromotionHasAccountedValue |= (kPromotion.getHillsDefensePercent() > 0);
					}
					//	City defense
					if (kPromotion.getCityDefensePercent() != 0)
					{
						//	Always value city defence as half since units with it are often going to be stationed in cities
						iResult = (iResult * (100 + kPromotion.getCityDefensePercent() / 2)) / 100;

						bPromotionHasAccountedValue |= (kPromotion.getCityDefensePercent() > 0);
					}

					//	First strike immunity
					if (kPromotion.isImmuneToFirstStrikes())
					{
						int		iFirstStrikeWeight = 120;	//	Future - make this adaptive to known enemy units

						iResult = (iResult * iFirstStrikeWeight) / 100;

						bPromotionHasAccountedValue = true;
					}

					//	Consider healing as part of defense
					if (kPromotion.isAlwaysHeal())
					{
						int		iAlwaysHealWeight = 140;

						iResult = (iResult * iAlwaysHealWeight) / 100;

						bPromotionHasAccountedValue = true;
					}

					//Team Project (2)
					if (kPromotion.getVictoryHeal() > 0)
					{
						iResult = (iResult * (100 + (kPromotion.getVictoryHeal() * 10))) / 100;

						bPromotionHasAccountedValue |= (kPromotion.getVictoryHeal() > 0);
					}

					if (kPromotion.getVictoryStackHeal() > 0)
					{
						iResult = (iResult * (100 + (kPromotion.getVictoryStackHeal() * 20))) / 100;

						bPromotionHasAccountedValue |= (kPromotion.getVictoryStackHeal() > 0);
					}

					if (kPromotion.getVictoryAdjacentHeal() > 0)
					{
						iResult = (iResult * (100 + (kPromotion.getVictoryAdjacentHeal() * 20))) / 100;

						bPromotionHasAccountedValue |= (kPromotion.getVictoryAdjacentHeal() > 0);
					}

					//Extra Lives (should double the value of the unit...)
					if (kPromotion.isOneUp())
					{
						int iOneUpWeight = 200;

						iResult = (iResult * iOneUpWeight) / 100;

						bPromotionHasAccountedValue = true;
					}

					// Defensive Victory Moves
					if (kPromotion.isDefensiveVictoryMove())
					{
						int iDefensiveVictoryMoveWeight = 140;

						iResult = (iResult * iDefensiveVictoryMoveWeight) / 100;

						bPromotionHasAccountedValue = true;
					}

					//	For defense consider extra healing in friendly terrioty
					if (kPromotion.getFriendlyHealChange() != 0)
					{
						int		iHealWeight = 25;

						iResult = (iResult * (1000 + (kPromotion.getFriendlyHealChange() * iHealWeight) / 1000)) / 1000;

						bPromotionHasAccountedValue |= (kPromotion.getFriendlyHealChange() > 0);
					}
				}

				if ((eFlags & UNITVALUE_FLAGS_OFFENSIVE) != 0)
				{
					//Team Project (2)
						// Offensive Victory Moves
					if (kPromotion.isOffensiveVictoryMove())
					{
						//may want to include Blitz into this factor somehow
						int iOffensiveVictoryMoveWeight = 140;

						iResult = (iResult * iOffensiveVictoryMoveWeight) / 100;

						bPromotionHasAccountedValue = true;
					}

					//	Terrain attack
					// Null modifier arrays are already indexed by the info object.
					if (kPromotion.isAnyTerrainAttackPercent() || kPromotion.getIgnoreTerrainDamage() >= 0)
					{
						for (int iJ = 0; iJ < GC.getNumTerrainInfos(); iJ++)
						{
							if (kPromotion.getTerrainAttackPercent((TerrainTypes)iJ) != 0 ||
								 kPromotion.getIgnoreTerrainDamage() == iJ)
							{
								int iNumRevealedAreaTiles = std::max(1, area()->getNumRevealedTiles(getTeam()));
								int	iNumRevealedAreaThisTerrain = area()->getNumRevealedTerrainTiles(getTeam(), (TerrainTypes)iJ);

								//	Note - used 1000 multiplier here for better granularity because there are
								//	a lot of terrain types.  We multiply the actual ratio by 2 because in practise the AI will
								//	deploy the units to appropriate terrains/features when it can so the the effective value is
								//	higher than the raw proportion would apply
								int	iTerrainWeight = (2 * 1000 * iNumRevealedAreaThisTerrain) / iNumRevealedAreaTiles;

								iResult = (iResult * (10000 + (kPromotion.getTerrainAttackPercent((TerrainTypes)iJ) * iTerrainWeight) / 10)) / 10000;

								bPromotionHasAccountedValue |= (kPromotion.getTerrainAttackPercent((TerrainTypes)iJ) > 0);

								if (kPromotion.getIgnoreTerrainDamage() == iJ)
								{
									iResult = (iResult * (10000 + (50 * iTerrainWeight) / 10)) / 10000;
									bPromotionHasAccountedValue = true;
								}
							}
						}
					}
					//	Feature attack
					// Null modifier arrays are already indexed by the info object.
					if (kPromotion.isAnyFeatureAttackPercent())
					{
						for (int iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
						{
							if (kPromotion.getFeatureAttackPercent((FeatureTypes)iJ) != 0)
							{
								int iNumRevealedAreaTiles = std::max(1, area()->getNumRevealedTiles(getTeam()));
								int	iNumRevealedAreaThisFeature = area()->getNumRevealedFeatureTiles(getTeam(), (FeatureTypes)iJ);

								//	Note - used 1000 multiplier here for better granularity because there are
								//	a lot of feature types.  We multiply the actual ratio by 2 because in practise the AI will
								//	deploy the units to appropriate terrains/features wehn it can so the the effective value is
								//	higher than the raw proportion would apply
								int	iFeatureWeight = (2 * 1000 * iNumRevealedAreaThisFeature) / iNumRevealedAreaTiles;

								iResult = (iResult * (10000 + (kPromotion.getFeatureAttackPercent((FeatureTypes)iJ) * iFeatureWeight) / 10)) / 10000;

								bPromotionHasAccountedValue |= (kPromotion.getFeatureAttackPercent((FeatureTypes)iJ) > 0);
							}
						}
					}
					//	Hills attack
					if (kPromotion.getHillsAttackPercent() != 0)
					{
						int iHillsWeight = 30;	//	Crudely assume 30% hills for now

						iResult = (iResult * (100 + (kPromotion.getHillsAttackPercent() * iHillsWeight) / 100)) / 100;

						bPromotionHasAccountedValue |= (kPromotion.getHillsAttackPercent() > 0);
					}
					//	City attack
					if (kPromotion.getCityAttackPercent() != 0)
					{
						//	Always value city attack as half since units with it are often going to be used in attacks
						iResult = (iResult * (100 + kPromotion.getCityAttackPercent() / 2)) / 100;

						bPromotionHasAccountedValue |= (kPromotion.getCityAttackPercent() > 0);
					}

					//	For attack consider extra healing in neutral and enemy territory
					if (kPromotion.getNeutralHealChange() != 0)
					{
						int		iNeutralHealWeight = 20;

						iResult = (iResult * (1000 + (kPromotion.getNeutralHealChange() * iNeutralHealWeight) / 10)) / 1000;

						bPromotionHasAccountedValue |= (kPromotion.getNeutralHealChange() > 0);
					}
					if (kPromotion.getEnemyHealChange() != 0)
					{
						int		iEnemyHealWeight = 30;

						iResult = (iResult * (1000 + (kPromotion.getEnemyHealChange() * iEnemyHealWeight) / 10)) / 1000;

						bPromotionHasAccountedValue |= (kPromotion.getEnemyHealChange() > 0);
					}
					//Team Project (2)
										//	Collateral Damage Limit
					if (kPromotion.getCollateralDamageLimitChange() != 0)
					{
						iResult = (iResult * (100 + (kPromotion.getCollateralDamageLimitChange()))) / 100;

						bPromotionHasAccountedValue |= (kPromotion.getCollateralDamageLimitChange() > 0);
					}
					//	Collateral Damage Limit
					if (kPromotion.getCollateralDamageMaxUnitsChange() != 0)
					{
						iResult = (iResult * (100 + (kPromotion.getCollateralDamageMaxUnitsChange() * 5))) / 100;

						bPromotionHasAccountedValue |= (kPromotion.getCollateralDamageMaxUnitsChange() > 0);
					}
					//	Combat Limit Change
					if (kPromotion.getCombatLimitChange() != 0)
					{
						iResult = (iResult * (100 + (kPromotion.getCombatLimitChange()))) / 100;

						bPromotionHasAccountedValue |= (kPromotion.getCombatLimitChange() > 0);
					}
				}

				if ((eFlags & UNITVALUE_FLAGS_UTILITY) != 0)
				{
					//Team Project (2)
										// Free Drop (takes no movement to perform an air drop)
					if (kPromotion.isFreeDrop())
					{
						int iFreeDropWeight = 110;

						iResult = (iResult * iFreeDropWeight) / 100;

						bPromotionHasAccountedValue = true;
					}
					// Pillage Espionage
					if (kPromotion.isPillageEspionage())
					{
						int iPillageEspionageWeight = 110;

						iResult = (iResult * iPillageEspionageWeight) / 100;

						bPromotionHasAccountedValue = true;
					}
					// Pillage Research
					if (kPromotion.isPillageResearch())
					{
						int iPillageResearchWeight = 110;

						iResult = (iResult * iPillageResearchWeight) / 100;

						bPromotionHasAccountedValue = true;
					}
					// Pillage Marauder
					if (kPromotion.isPillageMarauder())
					{
						int iPillageMarauderWeight = 115;

						iResult = (iResult * iPillageMarauderWeight) / 100;

						bPromotionHasAccountedValue = true;
					}
					// PillageOnMove
					if (kPromotion.isPillageOnMove())
					{
						int iPillageOnMoveWeight = 115;

						iResult = (iResult * iPillageOnMoveWeight) / 100;

						bPromotionHasAccountedValue = true;
					}
					// PillageOnVictory
					if (kPromotion.isPillageOnVictory())
					{
						int iPillageOnVictoryWeight = 115;

						iResult = (iResult * iPillageOnVictoryWeight) / 100;

						bPromotionHasAccountedValue = true;
					}
					//	Celebrity Happy
					if (kPromotion.getCelebrityHappy() != 0)
					{
						iResult = (iResult * (100 + (kPromotion.getCelebrityHappy() * 10))) / 100;

						bPromotionHasAccountedValue |= (kPromotion.getCelebrityHappy() > 0);
					}
					//	Drop Range
					if (kPromotion.getExtraDropRange() != 0)
					{
						iResult = (iResult * (100 + (kPromotion.getExtraDropRange() * 3))) / 100;

						bPromotionHasAccountedValue |= (kPromotion.getExtraDropRange() > 0);
					}

				}

				if ((eFlags & UNITVALUE_FLAGS_ALL) == UNITVALUE_FLAGS_ALL && !bPromotionHasAccountedValue)
				{
					//	We don't know what this promotion is for but it must be worth something
					int	iUnknownPromotionWeight = 110;

					iResult = (iResult * iUnknownPromotionWeight) / 100;
				}
			}
		}

		m_iGenericValue = iResult;
		m_eGenericValueFlagsCached = eFlags;
	}

	return m_iGenericValue;
}

/*
bool CvUnitAI::AI_approximatePath(CvPlot* pToPlot, int iFlags, int* piPathTurns) const
{
	PROFILE_FUNC();

#define CLOSE_THRESHOLD 10
	CvPlot*	start = plot();
int	iStepPathLen = GC.getMap().calculatePathDistance(start, pToPlot);

OutputDebugString(CvString::format("Approx path from (%d,%d) to (%d,%d), step pathLen: %d\n", start->getX(), start->getY(), pToPlot->getX(), pToPlot->getY(), iStepPathLen).c_str());

if (iStepPathLen == -1)
{
	OutputDebugString("Not pathable\n");
	return false;
}
else if (iStepPathLen < CLOSE_THRESHOLD)
{
	PROFILE("AI_approximatePath.Close");
	bool bResult = generatePath(pToPlot, iFlags, true, piPathTurns);

	if (bResult)
	{
		OutputDebugString(CvString::format("Actual close path evaluation yielded length of %d\n", *piPathTurns).c_str());
	}
	else
	{
		OutputDebugString("Not pathable on close path verification!!\n");
	}

	return bResult;
}
else
{
	PROFILE("AI_approximatePath.Distant");
	//	Find actual path length of a starting subpath of the step path to
	//	use to generate a normalization factor to estimate an actual cost
	FAStarNode* pNode = gDLL->getFAStarIFace()->GetLastNode(&GC.getStepFinder());

	while (pNode->m_iData1 >= CLOSE_THRESHOLD)
	{
		pNode = pNode->m_pParent;
	}

	//	Make sure we can path this stack to here, else shrink it back until we can
	FAStarNode* pCandidateSubPathEndNode = pNode;

	while (pNode->m_pParent != NULL)
	{
		if (!canEnterPlot(GC.getMap().plot(pNode->m_iX, pNode->m_iY)))
		{
			pCandidateSubPathEndNode = pNode->m_pParent;
		}

		pNode = pNode->m_pParent;
	}

	CvPlot* pSubPathTerminus = GC.getMap().plot(pCandidateSubPathEndNode->m_iX, pCandidateSubPathEndNode->m_iY);
	int iSubPathTurns;

	{
		PROFILE("AI_approximatePath.DistantSubPathCalc");
		if (!generatePath(pSubPathTerminus, iFlags, true, &iSubPathTurns))
		{
			OutputDebugString("Unpathable sub-path found!!\n");

			//	This should never happen - if it does something has goen wrong - just evaluate the
			//	entire path
			return generatePath(pToPlot, iFlags, true, piPathTurns);
		}

		//	Now normalise the step path length by the ratio of the subpath step length to its actual length
		*piPathTurns = (iStepPathLen * iSubPathTurns) / std::max(1, pCandidateSubPathEndNode->m_iData1);

		OutputDebugString(CvString::format("Sub path evaluation yielded length of %d vs %d, normalising total to %d\n", iSubPathTurns, pCandidateSubPathEndNode->m_iData1, *piPathTurns).c_str());
	}
}

return true;
}
*/

bool CvUnitAI::AI_isCityGarrison(const CvCity* pCity) const
{
	if (pCity == NULL) return false;
	return (m_iGarrisonCity != -1 && m_iGarrisonCity == pCity->getID());
}

void CvUnitAI::AI_setAsGarrison(const CvCity* pCity)
{
	int iGarrisonCity = (pCity == NULL ? -1 : pCity->getID());

	m_iAffirmedGarrisonCity = iGarrisonCity;

	if (iGarrisonCity != m_iGarrisonCity)
	{
		if (pCity == NULL)
		{
			pCity = GET_PLAYER(getOwner()).getCity(m_iGarrisonCity);
		}

		m_iGarrisonCity = iGarrisonCity;
        DefenseNeedEvents::changed();
        if(getOwner()!=NO_PLAYER && !isTempUnit())GET_PLAYER(getOwner()).getContractBroker().noteDefenseGroupChanged(getGroup());

		if (gUnitLogLevel >= 3)
		{
			if (iGarrisonCity == -1)
			{
				if (pCity != NULL)
				{
					logBBAI("	Unit %S (group %d with %d units) for player %d (%S) at (%d,%d) leaves garrison for city %S\n",
							getUnitInfo().getDescription(),
							getGroup()->getID(),
							getGroup()->getNumUnits(),
							getOwner(),
							GET_PLAYER(getOwner()).getCivilizationDescription(0),
							getX(),
							getY(),
							pCity->getName().GetCString());
				}
			}
			else
			{
				FAssert(pCity != NULL);
				logBBAI("	Unit %S (group %d with %d units) for player %d (%S) at (%d,%d) joins garrison for city %S\n",
						getUnitInfo().getDescription(),
						getGroup()->getID(),
						getGroup()->getNumUnits(),
						getOwner(),
						GET_PLAYER(getOwner()).getCivilizationDescription(0),
						getX(),
						getY(),
						pCity->getName().GetCString());
			}
		}
	}
}


/*TB Prophet Mod begin*/
bool CvUnitAI::AI_foundReligion()
{
	PROFILE_FUNC();

	ReligionTypes eReligion;
	int iI;
	int iJ;
	int value;
	int bestValue;

	ReligionTypes eBestReligion = NO_RELIGION;
	int iProphetCount = GET_PLAYER(getOwner()).AI_getNumAIUnits(UNITAI_PROPHET);

	const ReligionTypes eFavorite = (ReligionTypes)GC.getLeaderHeadInfo(GET_PLAYER(getOwner()).getLeaderType()).getFavoriteReligion();
	if (GC.getGame().isOption(GAMEOPTION_RELIGION_DIVINE_PROPHETS))
	{
		if (eFavorite != NO_RELIGION && !GC.getGame().isReligionFounded(eFavorite))
		{
			if (canSpread(plot(), eFavorite))
			{
				//if favorite religion of current player was not founded:
		//	push mission 'found religion' with parameter 'favorite religion' and return true
				getGroup()->pushMission(MISSION_SPREAD, eFavorite);
				return true;
			}
		}
		//if favorite religion was not founded and CAN'T yet be, hold on to one prophet.			   GC.getLeaderHeadInfo(GET_PLAYER(getOwner()).getLeaderType()).getFavoriteReligion()(GC.getTechInfo((TechTypes)(GC.getBonusInfo((BonusTypes)iK).getTechCityTrade())).getEra()
		if (eFavorite != NO_RELIGION &&
			!GC.getGame().isReligionFounded(eFavorite) &&
			(((int)(GC.getTechInfo(GC.getReligionInfo(eFavorite).getTechPrereq()).getEra()) - (int)(GET_PLAYER(getOwner()).getCurrentEra()) < 2)) &&
			(iProphetCount == 1))
		{
			getGroup()->pushMission(MISSION_SKIP);
			return true;
		}

		value = 0;
		bestValue = 0;
		eBestReligion = NO_RELIGION;

		//go over all religions that can be founded:
		if (!GC.getGame().isOption(GAMEOPTION_RELIGION_LIMITED))
		{
			for (iI = 0; iI < GC.getNumReligionInfos(); iI++)
			{
				if (canSpread(plot(), (ReligionTypes)iI))
				{
					if (!GC.getGame().isReligionFounded((ReligionTypes)iI))
					{
						for (iJ = 0; iJ < GC.getNumFlavorTypes(); iJ++)
						{
							value += std::max(1, GET_PLAYER(getOwner()).AI_getFlavorValue((FlavorTypes)iJ)) * std::max(1, GC.getReligionInfo((ReligionTypes)iI).getFlavorValue((FlavorTypes)iJ));
						}
						if (value > bestValue)
						{
							eBestReligion = ((ReligionTypes)iI);
							bestValue = value;
						}
					}
				}
			}

			eReligion = eBestReligion;
			if (eReligion != NO_RELIGION)
			{
				//	push mission 'found religion' with parameter 'eReligion' and return true
				getGroup()->pushMission(MISSION_SPREAD, eReligion);
				return true;
			}
		}

		if (GC.getGame().isOption(GAMEOPTION_RELIGION_LIMITED) && GC.getGame().isReligionFounded(eFavorite))
		{
			for (iI = 0; iI < GC.getNumReligionInfos(); iI++)
			{
				if (canSpread(plot(), (ReligionTypes)iI))
				{
					if (!GC.getGame().isReligionFounded((ReligionTypes)iI))
					{
						for (iJ = 0; iJ < GC.getNumFlavorTypes(); iJ++)
						{
							value += (GET_PLAYER(getOwner()).AI_getFlavorValue((FlavorTypes)iJ) + 1) * (GC.getReligionInfo((ReligionTypes)iI).getFlavorValue((FlavorTypes)iJ) + 1);
							if (value > bestValue)
							{
								eBestReligion = ((ReligionTypes)iI);
								bestValue = value;
							}
						}
					}
				}
			}

			eReligion = eBestReligion;
			if (eReligion != NO_RELIGION)
			{
				//	push mission 'found religion' with parameter 'eReligion' and return true
				getGroup()->pushMission(MISSION_SPREAD, eReligion);
				return true;
			}
		}
	}
	return false;
}

#ifdef OUTBREAKS_AND_AFFLICTIONS
bool CvUnitAI::AI_cureAffliction(PromotionLineTypes eAfflictionLine)
{
	PROFILE_FUNC();

	if (canCure(plot(), eAfflictionLine))
	{
		getGroup()->pushMission(MISSION_CURE, eAfflictionLine);
		return true;
	}
	return false;
}
#endif
/*TB Prophet Mod end*/

void unitSourcesValueToCity(const CvGameObject* pObject, const CvPropertyManipulators* pMani, const CvUnit* pUnit, const CvCityAI* pCity, int* iValue, PropertyTypes eProperty)
{
	PROFILE_EXTRA_FUNC();
	if (pCity == NULL)
	{
		pCity = static_cast<CvCityAI*>(GET_PLAYER(pUnit->getOwner()).getCapitalCity());
	}

	if (pCity != NULL)
	{
		foreach_(const CvPropertySource * pSource, pMani->getSources())
		{
			if ((eProperty == NO_PROPERTY || pSource->getProperty() == eProperty)

			// Sources that deliver to the city or the plot are both considered since the city plot diffuses to the city for most properties anyway
			&& pSource->getType() == PROPERTYSOURCE_CONSTANT && (pSource->getObjectType() == GAMEOBJECT_CITY || pSource->getObjectType() == GAMEOBJECT_PLOT))
			{
				const PropertyTypes eProperty = pSource->getProperty();
				const int iCurrentSourceSize = pCity->getTotalBuildingSourcedProperty(eProperty) + pCity->getTotalUnitSourcedProperty(eProperty) + pCity->getPropertyNonBuildingSource(eProperty);
				const int iNewSourceSize = iCurrentSourceSize + static_cast<const CvPropertySourceConstant*>(pSource)->getAmountPerTurn(pCity->getGameObject());
				const int iDecayPercent = pCity->getPropertyDecay(eProperty);

				//	Steady state occurs at a level where the decay removes as much per turn as the sources add
				//	Decay can be 0 if the current level is below the threshold at which decay cuts in, so for the
				//	purposes of calculation just treat this as very slow decay
				const int iCurrentSteadyStateLevel = (100 * iCurrentSourceSize) / std::max(1, iDecayPercent);
				const int iNewSteadyStateLevel = (100 * iNewSourceSize) / std::max(1, iDecayPercent);

				*iValue += pCity->getPropertySourceValue(eProperty, iNewSteadyStateLevel - iCurrentSteadyStateLevel);
			}
		}
	}
}

//	Find the total beneficial bnet value to a given city of this unit's property sources
int CvUnitAI::AI_beneficialPropertyValueToCity(const CvCity* pCity, PropertyTypes eProperty) const
{
	PROFILE_EXTRA_FUNC();
	int iValue = 0;
	//void unitSourcesValueToCity(CvGameObject * pObject, CvPropertyManipulators * pMani, const CvUnit * pUnit, const CvCityAI * pCity, int* iValue, PropertyTypes eProperty)

	this->getGameObject()->foreachManipulator(
		bind(unitSourcesValueToCity, this->getGameObject(), _1, this, static_cast<const CvCityAI*>(pCity), &iValue, eProperty)
	);

	return iValue;
}

//	Set an override to the group head priority for this unit, to allow a unit that
//	would not nomally lead a stack to do so - priority will be preserved when other groups
//	joing one lead by a unit with an override, but reset if we join another group
void CvUnitAI::AI_setLeaderPriority(int iPriority)	//	 -1 means reset to default
{
	m_iGroupLeadOverride = iPriority;
}

bool CvUnitAI::AI_fulfillHealerNeed(const CvPlot* pPlot)
{
	PROFILE_EXTRA_FUNC();
	const CvPlot* pBestPlot = NULL;
	int iBestValue = 0;
	const UnitCombatTypes eUnitCombat = getBestHealingType();
	if (eUnitCombat == NO_UNITCOMBAT)
	{
		return false;
	}

	const PlayerTypes ePlayer = getOwner();
	const DomainTypes eDomain = getDomainType();

	foreach_(const CvPlot * plotX, pPlot->rect(25, 25))
	{
		// Rejected unsafe targets must not lend their value to a failed path.
		// Empty plots cannot contain patients or resident healers. Do not
		// query connectivity/support for thousands of zero-score candidates.
		if (plotX->getNumUnits() == 0) continue;
		if(!UnitTravelConnectivity::landUnitCanReach(this,plotX))continue;
		int iValue = 0;
		const int iCheck = plotX->getUnitCombatsUnsupportedByHealer(ePlayer, eUnitCombat, eDomain);

		if (iCheck > 0)
		{
			int iPathTurns = 0;
			if (atPlot(plotX))
			{
				iValue = iCheck;
				iValue += getNumHealSupportTotal();
			}
			else if (generateIgnoreDangerPath(plotX, &iPathTurns))
			{
				iValue = iCheck;
				iValue -= iPathTurns / 4;
			}
		}
		else
		{
			iValue = std::min(plotX->plotCount(PUF_isHealUnitCombatType, eUnitCombat, eDomain, NULL, ePlayer), getNumHealSupportTotal());
		}
		if (iValue > iBestValue && scoring::healerTargetSafe(this, plotX))
		{
			iBestValue = iValue;
			pBestPlot = plotX;
		}
	}

	if (pBestPlot)
	{
		if (atPlot(pBestPlot))
		{
			return getGroup()->pushMissionInternal(MISSION_SKIP, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_HEAL_SUPPORT, pBestPlot);
		}
		if (generateSafePathforVulnerable(pBestPlot))
		{
			const CvPlot* plotX = getPathEndTurnPlot();
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, plotX->getX(), plotX->getY(), MOVE_WITH_CAUTION, false, false, MISSIONAI_HEAL_SUPPORT, pBestPlot);
		}
		getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
		return true;
	}
	return false;
}

bool CvUnitAI::AI_fulfillImmediateHealerNeed(const CvPlot* pPlot)
{
	PROFILE_EXTRA_FUNC();

	const UnitCombatTypes eUnitCombat = getBestHealingType();
	if (eUnitCombat == NO_UNITCOMBAT)
	{
		return false;
	}
	const CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	const PlayerTypes ePlayer = getOwner();
	const DomainTypes eDomain = getDomainType();


	foreach_(const CvPlot * plotX, pPlot->rect(4, 4))
	{
		// Rejected unsafe targets must not lend their value to a failed path.
		if (plotX->getNumUnits() == 0) continue;
		if(!UnitTravelConnectivity::landUnitCanReach(this,plotX))continue;
		int iValue = 0;
		const int iCheck = plotX->getInjuredUnitCombatsUnsupportedByHealer(ePlayer, eUnitCombat, eDomain);

		if (iCheck > 0)
		{
			int iPathTurns;
			if (atPlot(plotX))
			{
				iValue = iCheck;
				iValue += getNumHealSupportTotal();
			}
			else if (generateIgnoreDangerPath(plotX, &iPathTurns))
			{
				iValue = iCheck;
				iValue -= iPathTurns;
			}
		}
		else
		{
			iValue = std::min(plotX->plotCount(PUF_isInjuredUnitCombatType, eUnitCombat, eDomain, NULL, ePlayer), getNumHealSupportTotal());
		}
		if (iValue > iBestValue && scoring::healerTargetSafe(this, plotX))
		{
			iBestValue = iValue;
			pBestPlot = plotX;
		}
	}

	if (pBestPlot)
	{
		if (atPlot(pBestPlot))
		{
			return getGroup()->pushMissionInternal(MISSION_SKIP, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_HEAL_SUPPORT, pBestPlot);
		}
		if (generateSafePathforVulnerable(pBestPlot))
		{
			const CvPlot* plotX = getPathEndTurnPlot();
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, plotX->getX(), plotX->getY(), MOVE_WITH_CAUTION, false, false, MISSIONAI_HEAL_SUPPORT, pBestPlot);
		}
		getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
		return true;
	}
	return false;
}

int CvUnitAI::scoreCityHealerNeed(const UnitCombatTypes eUnitCombat, const DomainTypes eDomain, const CvCity* city) const
{
	if (!UnitTravelConnectivity::landUnitCanReach(this,city->plot()) || !AI_plotValid(city->plot()))
		return 0;
	CvPlot* targetPlot = city->plot();
	int score = 0;
	if (targetPlot != NULL && UnitTravelConnectivity::landUnitCanReach(this,targetPlot))
	{
		int unsupportedUnits = targetPlot->getUnitCombatsUnsupportedByHealer(getOwner(), eUnitCombat, eDomain);

		if (!atPlot(targetPlot))
		{
			score = unsupportedUnits;
		}
		else if (unsupportedUnits > 0)
		{
			score = unsupportedUnits + getNumHealSupportTotal();
		}
		else
		{
			int healersOnPlot = targetPlot->plotCount(PUF_isHealUnitCombatType, eUnitCombat, eDomain, NULL, getOwner());
			score = std::min(healersOnPlot, getNumHealSupportTotal());
		}
	}
	// Scale up (fairly arbitrarily) so that the distance factor that is applied will have the
	// appropriate effect (not overwhelming the base score, nor having no effect)
	return scoring::applyDistanceScoringFactor(score * 10, plot(), targetPlot, 2);
}

bool CvUnitAI::AI_fulfillCityHealerNeed(const CvPlot* pPlot)
{
	const UnitCombatTypes eUnitCombat = getBestHealingType();
	if (eUnitCombat == NO_UNITCOMBAT)
	{
		return false;
	}
	const DomainTypes eDomain = getDomainType();
	const CvPlayer& player = GET_PLAYER(getOwner());

	using namespace scoring;
	ScoreResult<CvCity> bestCityScore = findBestScore<CvCity, GreatestScore>(
		player.beginCities(), player.endCities(),
		bind(&CvUnitAI::scoreCityHealerNeed, this, eUnitCombat, eDomain, _1), // scoring the city for healing need
		bind(canHealerPathToCity, this, _1), true // Only cities with a real support need may request this healer.
	);

	if (bestCityScore.found)
	{
		CvCity* bestCity = bestCityScore.result.item;
		if (atPlot(bestCity->plot()))
		{
			return getGroup()->pushMissionInternal(MISSION_SKIP, bestCity->getX(), bestCity->getY(), 0, false, false, MISSIONAI_HEAL_SUPPORT, bestCity->plot());
		}
		if (generateSafePathforVulnerable(bestCity->plot()))
		{
			const CvPlot* endTurnPlot = getPathEndTurnPlot();
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY(), MOVE_WITH_CAUTION, false, false, MISSIONAI_HEAL_SUPPORT, bestCity->plot());
		}
		getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
		return true;
	}
	return false;
}

namespace {
	// Read current effects once per decision, using sparse active promotions rather
	// than scanning every promotion or repeating the sources for every property.
	void addPropertyControlSources(const CvPropertyManipulators* manipulators, const CvGameObject* object, std::vector<int>& amounts)
	{
		if (manipulators == NULL) return;
		foreach_(const CvPropertySource* source, manipulators->getSources())
		{
			const PropertyTypes property = source->getProperty();
			if (source->getType() == PROPERTYSOURCE_CONSTANT && source->getObjectType() == GAMEOBJECT_CITY
				&& property >= 0 && property < (int)amounts.size())
			{
				const __int64 amount = (__int64)amounts[property] + static_cast<const CvPropertySourceConstant*>(source)->getAmountPerTurn(object);
				amounts[property] = (int)std::max<__int64>(-1000000, std::min<__int64>(1000000, amount));
			}
		}
	}

	bool investigatorNeededLocally(const CvUnitAI* unit)
	{
		const CvPlot* plot = unit->plot();
		const CvCity* city = plot->getPlotCity();
		if (city == NULL || city->getOwner() != unit->getOwner()) return false;
		// Same minimum-investigator rule used by AI_InvestigatorFulfillment.
		const int ownMission = unit->getGroup()->AI_getMissionAIType() == MISSIONAI_INVESTIGATOR_MAINTAIN;
		return 0 < 1 + ownMission - GET_PLAYER(unit->getOwner()).AI_plotTargetMissionAIs(
			plot, MISSIONAI_INVESTIGATOR_MAINTAIN, NULL, 0);
	}

	bool infiltratorWorkingLocally(const CvUnitAI* unit)
	{
		const CvPlot* plot = unit->plot();
		const TeamTypes host = plot->getTeam();
		if (!unit->isBuildUp() || unit->isWanted() || plot->getPlotCity() == NULL
			|| host == NO_TEAM || host >= MAX_PC_PLAYERS || host == unit->getTeam()
			|| GET_TEAM(unit->getTeam()).AI_getAttitudeWeight(host) >= 0
			|| !unit->getGroup()->isInvisible(host)) return false;

		// Use actual city effects, including active promotions/BuildUp. No unit
		// names or property IDs are assumed, and this decision has no lasting cache.
		std::vector<int> amounts(GC.getNumPropertyInfos(), 0);
		addPropertyControlSources(unit->getUnitInfo().getPropertyManipulators(), unit->getGameObject(), amounts);
		const std::map<PromotionTypes, PromotionKeyedInfo>& promotions = unit->getPromotionKeyedInfo();
		for (std::map<PromotionTypes, PromotionKeyedInfo>::const_iterator it = promotions.begin(); it != promotions.end(); ++it)
		{
			if (it->second.m_bHasPromotion)
				addPropertyControlSources(GC.getPromotionInfo(it->first).getPropertyManipulators(), unit->getGameObject(), amounts);
		}
		for (int i = 0; i < (int)amounts.size(); ++i)
		{
			if ((__int64)amounts[i] * GC.getPropertyInfo((PropertyTypes)i).getAIWeight() < 0) return true;
		}
		return false;
	}

	PropertyTypes propertyControlCapabilities(const CvUnit* unit, std::vector<int>& current, std::vector<int>& mobile)
	{
		current.assign(GC.getNumPropertyInfos(), 0);
		mobile.assign(GC.getNumPropertyInfos(), 0);
		addPropertyControlSources(unit->getUnitInfo().getPropertyManipulators(), unit->getGameObject(), current);
		mobile = current;
		const std::map<PromotionTypes, PromotionKeyedInfo>& promotions = unit->getPromotionKeyedInfo();
		for (std::map<PromotionTypes, PromotionKeyedInfo>::const_iterator it = promotions.begin(); it != promotions.end(); ++it)
		{
			if (!it->second.m_bHasPromotion) continue;
			const CvPromotionInfo& promotion = GC.getPromotionInfo(it->first);
			addPropertyControlSources(promotion.getPropertyManipulators(), unit->getGameObject(), current);
			const PromotionLineTypes line = promotion.getPromotionLine();
			// Fortification/BuildUp promotions are removed when the unit moves.
			if (line == NO_PROMOTIONLINE || !GC.getPromotionLineInfo(line).isBuildUp())
			{
				addPropertyControlSources(promotion.getPropertyManipulators(), unit->getGameObject(), mobile);
			}
		}
		PropertyTypes best = NO_PROPERTY;
		int bestScore = 0;
		for (int i = 0; i < (int)current.size(); ++i)
		{
			const int weight = GC.getPropertyInfo((PropertyTypes)i).getAIWeight() / 50;
			current[i] = (int)std::max<__int64>(0, std::min<__int64>(1000000, (__int64)current[i] * weight));
			mobile[i] = (int)std::max<__int64>(0, std::min<__int64>(1000000, (__int64)mobile[i] * weight));
			if (current[i] > bestScore) { bestScore = current[i]; best = (PropertyTypes)i; }
		}
		return best;
	}

	bool propertyControllerConstructsLocally(const CvUnit* unit)
	{
		const CvCity* city = unit->plot()->getPlotCity();
		if (city == NULL || city->getOwner() != unit->getOwner()) return false;
		std::vector<int> current, mobile;
		propertyControlCapabilities(unit, current, mobile);
		for (int property = 0; property < (int)current.size(); ++property)
		{
			if (current[property] <= 0) continue;
			const PropertyTypes type = (PropertyTypes)property;
			if (city->getPropertyNeed(type) > 0) return true;
			// getPropertyNeed clamps a satisfied need to -1, even when this
			// controller is what keeps it satisfied. Reuse the city's stored
			// level/change and the same ten-turn horizon, without this unit.
			const CvPropertyInfo& info = GC.getPropertyInfo(type);
			const int era = GET_PLAYER(unit->getOwner()).getCurrentEra();
			const int target = info.isTargetLevelbyEraType(era)
				? info.getTargetLevelbyEraType(era) : info.getTargetLevel();
			const CvProperties* properties = city->getPropertiesConst();
			const __int64 projected = (__int64)properties->getValueByProperty(type)
				+ (__int64)properties->getChangeByProperty(type) * 10;
			const __int64 needWithoutController = ((__int64)target - projected)
				* (info.getAIWeight() / 50) + (__int64)current[property] * 10;
			if (needWithoutController > 0) return true;
		}
		return false;
	}

	int scorePropertyControlNeed(const CvUnit* unit, const CvCity* city, PropertyTypes currentProperty, PropertyTypes mobileProperty, PropertyTypes crimeProperty, int buildUpRetention = 0)
	{
		const bool current = unit->atPlot(city->plot());
		const PropertyTypes property = current ? currentProperty : mobileProperty;
		if (property == NO_PROPERTY) return 0;
		int value = city->getPropertyNeed(property); // Existing per-city, per-turn cache.
		if (value < 0 && !current) return 0;
		int minimum = GC.getC2C_MIN_PROP_CONTROL();
		// Preserve the legacy criminal-presence rule, resolving its XML ID rather than assuming index zero.
		if (property == crimeProperty) minimum += city->plot()->getNumCriminals() * 2;
		const CvPlayer& player = GET_PLAYER(unit->getOwner());
		// These mission counts already have their own invalidated player cache.
		const int responders = player.AI_plotTargetMissionAIs(city->plot(), MISSIONAI_PROPERTY_CONTROL_RESPONSE, NULL, 0);
		const int existing = player.AI_plotTargetMissionAIs(city->plot(), MISSIONAI_PROPERTY_CONTROL_MAINTAIN, NULL, 0);
		if (responders > 0 || existing > 0) value /= responders + existing;
		if (current)
		{
			// Need already includes this established unit. A move also destroys BuildUp;
			// price that loss on the same ten-turn horizon used by getPropertyNeed.
			value = (int)std::min<__int64>(1000000, (__int64)value + (__int64)buildUpRetention * 10);
			if (existing < minimum) value += 1000;
			value = std::max(100, value);
		}
		const int score = std::max(0, value);
		return std::min(1000000, score + scoring::applyDistanceScoringFactor(score, unit->plot(), city->plot(), 1) / 10);
	}

	struct PropertyControlCandidate
	{
		PropertyControlCandidate(CvCity* city, int score, bool current) : city(city), score(score), current(current) {}
		CvCity* city;
		int score;
		bool current;
		bool operator<(const PropertyControlCandidate& other) const
		{
			if (score != other.score) return score > other.score;
			return current && !other.current; // A tie does not justify moving an established controller.
		}
	};

	bool propertyControllerNeededLocally(const CvUnit* unit, PropertyTypes property)
	{
		const CvCity* city = unit->plot()->getPlotCity();
		return property != NO_PROPERTY && city != NULL && city->getOwner() == unit->getOwner()
			&& city->getPropertyNeed(property) > 0;
	}

	bool propertyRelocationPreferred(const std::vector<PropertyControlCandidate>& candidates)
	{
		int local = 0, remote = 0;
		for (size_t i = 0; i < candidates.size(); ++i)
		{
			if (candidates[i].current) local = std::max(local, candidates[i].score);
			else remote = std::max(remote, candidates[i].score);
		}
		return remote > local;
	}

	int selectPropertyControlCandidate(const CvUnitAI* unit, std::vector<PropertyControlCandidate>& candidates)
	{
		std::stable_sort(candidates.begin(), candidates.end());
		// No commands or world changes in this scope; repeated path checks can reuse danger answers.
		PlotDangerCacheScope dangerCache(unit->getOwner());
		for (int i = 0; i < (int)candidates.size(); ++i)
		{
			if (candidates[i].score <= 0) break;
			if (candidates[i].current || unit->generateSafePathforVulnerable(candidates[i].city->plot())) return i;
		}
		return -1;
	}

};

namespace
{
    // Only the constant, same-tile sources understood by the existing controller
    // AI participate. Unmodelled interactions/expressions keep the legacy policy.
    bool dispatchPropertySources(const CvPropertyManipulators* mani, const CvCity* city,
        std::vector<int>& current, std::vector<int>& mobile, bool buildUp)
    {
        if (mani == NULL) return true;
        if (mani->getNumInteractions() || mani->getNumPropagators()) return false;
        foreach_(const CvPropertySource* source, mani->getSources())
        {
            if (source->getObjectType() != GAMEOBJECT_CITY && source->getObjectType() != GAMEOBJECT_PLOT) continue;
            if (source->getType() != PROPERTYSOURCE_CONSTANT || source->getRelation() != RELATION_SAME_PLOT) return false;
            const CvPropertySourceConstant* constant = static_cast<const CvPropertySourceConstant*>(source);
            if (dynamic_cast<const IntExprConstant*>(constant->getAmountPerTurnExpr()) == NULL) return false;
            const int p = source->getProperty();
            if (p < 0 || p >= (int)current.size()) return false;
            const CvGameObject* object = source->getObjectType() == GAMEOBJECT_CITY
                ? static_cast<const CvGameObject*>(city->getGameObject()) : static_cast<const CvGameObject*>(city->plot()->getGameObject());
            if (!source->isActive(object)) continue;
            const int value = (int)std::max<__int64>(-1000000, std::min<__int64>(1000000,
                (__int64)constant->getAmountPerTurn(object) * (GC.getPropertyInfo((PropertyTypes)p).getAIWeight() / 50)));
            // Plot diffusion is not instant. Charge the donor the full helpful
            // loss, but promise only direct city help and all possible harm.
            current[p] = std::min(1000000, current[p] + std::max(0, value));
            if (!buildUp && (source->getObjectType() == GAMEOBJECT_CITY || value < 0))
                mobile[p] = std::max(-1000000, std::min(1000000, mobile[p] + value));
        }
        return true;
    }

    bool dispatchPropertyCapabilities(const CvUnit* unit, const CvCity* city, std::vector<int>& current, std::vector<int>& mobile)
    {
        current.assign(GC.getNumPropertyInfos(), 0);
        mobile.assign(GC.getNumPropertyInfos(), 0);
        if (!dispatchPropertySources(unit->getUnitInfo().getPropertyManipulators(), city, current, mobile, false)) return false;
        const std::map<PromotionTypes, PromotionKeyedInfo>& promotions = unit->getPromotionKeyedInfo();
        for (std::map<PromotionTypes, PromotionKeyedInfo>::const_iterator i = promotions.begin(); i != promotions.end(); ++i)
        {
            if (!i->second.m_bHasPromotion) continue;
            const CvPromotionInfo& promotion = GC.getPromotionInfo(i->first);
            const PromotionLineTypes line = promotion.getPromotionLine();
            if (!dispatchPropertySources(promotion.getPropertyManipulators(), city, current, mobile,
                line != NO_PROMOTIONLINE && GC.getPromotionLineInfo(line).isBuildUp())) return false;
        }
        for (size_t p = 0; p < current.size(); ++p)
            if (current[p] > 0 || mobile[p] > 0) return true;
        return false;
    }

    CityPropertyDispatch::Budget dispatchCityBudget(const CvCity* city)
    {
        CityPropertyDispatch::Budget budget(GC.getNumPropertyInfos());
        for (int p = 0; p < GC.getNumPropertyInfos(); ++p)
        {
            const PropertyTypes property = (PropertyTypes)p;
            const CvPropertyInfo& info = GC.getPropertyInfo(property);
            const int era = GET_PLAYER(city->getOwner()).getCurrentEra();
            const int target = info.isTargetLevelbyEraType(era) ? info.getTargetLevelbyEraType(era) : info.getTargetLevel();
            const CvProperties* properties = city->getPropertiesConst();
            const __int64 projected = (__int64)properties->getValueByProperty(property) + (__int64)properties->getChangeByProperty(property) * 10;
            budget.need[p] = (int)std::max<__int64>(-1000000000, std::min<__int64>(1000000000,
                ((__int64)target - projected) * (info.getAIWeight() / 50)));
            budget.minimum[p] = GC.getC2C_MIN_PROP_CONTROL();
            if (info.getType() == CvString("PROPERTY_CRIME")) budget.minimum[p] += city->plot()->getNumCriminals() * 2;
        }
        return budget;
    }

    bool splitDispatchController(CvUnit* unit)
    {
        // joinGroup(NULL, ..., false) deliberately leaves a unit WITHOUT a group.
        // Allocate the singleton explicitly, also avoiding upgrade regroup hints.
        CvPlayer& player = GET_PLAYER(unit->getOwner());
        CvSelectionGroup* fresh = player.addSelectionGroup();
        if (fresh == NULL) return false;
        const int id = fresh->getID();
        fresh->init(id, unit->getOwner());
        unit->joinGroup(fresh);
        if (unit->getGroup() == fresh) return true;
        player.deleteSelectionGroup(id);
        return false;
    }

    struct DispatchController
    {
        int id, source;
        bool available;
        std::vector<int> current, mobile;
        DispatchController(int id, int source, bool available) : id(id), source(source), available(available) {}
    };

    struct DispatchCity
    {
        CvCity* city;
        CityPropertyDispatch::Budget budget, retention;
        std::vector<int> controllers;
        int happinessSpent, happinessBudget;
        DispatchCity(CvCity* city) : city(city), budget(dispatchCityBudget(city)), retention(budget), happinessSpent(0), happinessBudget(city->happyLevel() - city->unhappyLevel(0)) {}
    };

    struct DispatchNeed
    {
        int city, value;
        DispatchNeed(int city, int value) : city(city), value(value) {}
        bool operator<(const DispatchNeed& other) const { return value > other.value; }
    };

    void propertyDispatchTrace(const CvPlayerAI& player, const char* event, int city, int donor, int count, int turns)
    {
#ifdef C2C_PROPERTY_DISPATCH_STATS
        const char* output = getenv("C2C_AI_BENCH_OUTPUT");
        if (output == NULL) return;
        static FILE* file = fopen(CvString::format("%s/CityPropertyDispatch.log", output).c_str(), "w");
        if (file != NULL)
        {
            fprintf(file, "turn=%d player=%d event=%s city=%d donor=%d count=%d turns=%d\n",
                GC.getGame().getGameTurn(), player.getID(), event, city, donor, count, turns);
            fflush(file);
        }
#endif
    }
}

// Called once after this player's unit turn reset/promotions/upgrades, before
// movement updates. IDs survive grouping; no saved state or cross-turn pointers.
void CvUnitAI::AI_dispatchCityPropertyRequests(CvPlayerAI& player)
{
    PROFILE_FUNC();
    if (player.isHumanPlayer() || player.isNPC()) return;
    const int maxHelpTurns = 9;
    // Dissolve homogeneous support batches after arrival. Otherwise a batch
    // would become permanently unavailable merely because it has several members.
    std::vector<int> arrived;
    foreach_(CvUnit* base, player.units())
    {
        if (base->AI_getUnitAIType() != UNITAI_PROPERTY_CONTROL || base->getDomainType() != DOMAIN_LAND
            || base->isDead() || base->isDelayedDeath() || base->isCargo() || base->isHurt()) continue;
        CvCity* city = base->plot()->getPlotCity();
        CvSelectionGroup* group = base->getGroup();
        if (group == NULL || group->getArmyID() >= 0) continue;
        const CvUnitAI* actor = static_cast<const CvUnitAI*>(base);
        if (actor->m_contractualState == CONTRACTUAL_STATE_FOUND_WORK || actor->m_contractualState == CONTRACTUAL_STATE_AWAITING_ANSWER) continue;
        const MissionAITypes mission = group->AI_getMissionAIType();
        if (city == NULL || city->getOwner() != player.getID() || group->isBusy()
            || (mission != MISSIONAI_PROPERTY_CONTROL_MAINTAIN && mission != MISSIONAI_PROPERTY_CONTROL_RESPONSE)
            || group->AI_getMissionAIPlot() != base->plot()) continue;
        bool homogeneous = true;
        foreach_(const CvUnit* member, group->units())
        {
            if (member->AI_getUnitAIType() != UNITAI_PROPERTY_CONTROL || member->isCargo() || member->isHurt()) homogeneous = false;
        }
        if (homogeneous) arrived.push_back(base->getID());
    }
    for (size_t i = 0; i < arrived.size(); ++i)
    {
        CvUnit* unit = player.getUnit(arrived[i]);
        if (unit == NULL) continue;
        if (unit->getGroup() == NULL || unit->getGroup()->getNumUnits() > 1)
            if (!splitDispatchController(unit)) continue;
        unit->getGroup()->AI_setMissionAI(MISSIONAI_PROPERTY_CONTROL_MAINTAIN, unit->plot(), NULL);
    }
    std::vector<DispatchCity> cities;
    std::map<int, int> index;
    foreach_(CvCity* city, player.cities())
    {
        index[city->getID()] = (int)cities.size();
        cities.push_back(DispatchCity(city));
    }
    if (cities.empty()) return;
    std::vector<DispatchController> controllers;
    int incoming = 0, protectedUnits = 0, unsupported = 0, offCity = 0;
    std::map<int, int> incomingTurns;
    foreach_(CvUnit* base, player.units())
    {
        if (base->AI_getUnitAIType() != UNITAI_PROPERTY_CONTROL || base->getDomainType() != DOMAIN_LAND
            || base->isDead() || base->isDelayedDeath() || base->isCargo()) continue;
        CvUnitAI* unit = static_cast<CvUnitAI*>(base);
        CvCity* source = unit->plot()->getPlotCity();
        const int from = source != NULL && source->getOwner() == player.getID() ? index[source->getID()] : -1;
        CvSelectionGroup* group = unit->getGroup();
        CvPlot* targetPlot = (group->AI_getMissionAIType() == MISSIONAI_PROPERTY_CONTROL_RESPONSE
            || group->AI_getMissionAIType() == MISSIONAI_CONTRACT) ? group->AI_getMissionAIPlot() : NULL;
        CvCity* target = targetPlot != NULL ? targetPlot->getPlotCity() : NULL;
        DispatchController record(unit->getID(), from, false);
        const CvCity* evaluation = from >= 0 ? source : (target != NULL && target->getOwner() == player.getID() ? target : cities[0].city);
        const bool supported = dispatchPropertyCapabilities(unit, evaluation, record.current, record.mobile);
        if (!supported) ++unsupported;
        if (from < 0) ++offCity;
        if (from >= 0)
            for (int p = 0; p < (int)record.current.size(); ++p)
                if (record.current[p] > 0) { ++cities[from].budget.present[p]; ++cities[from].retention.present[p]; }
        if (target != NULL && target->getOwner() == player.getID() && !unit->atPlot(targetPlot))
        {
            // Validate one whole-group route, then credit every member at its
            // actual ETA. Unknown/unreachable arrivals receive no credit.
            if (incomingTurns.find(group->getID()) == incomingTurns.end())
            {
                int turns = 10;
                if (!unit->generatePath(targetPlot, MOVE_WITH_CAUTION, true, &turns, maxHelpTurns)) turns = 10;
                incomingTurns[group->getID()] = turns;
            }
            std::vector<int> targetCurrent, targetMobile;
            if (dispatchPropertyCapabilities(unit, target, targetCurrent, targetMobile))
            {
                CityPropertyDispatch::reserve(cities[index[target->getID()]].budget, targetMobile, incomingTurns[group->getID()]);
                ++incoming;
            }
            continue;
        }
        const MissionAITypes mission = group->AI_getMissionAIType();
        record.available = supported && from >= 0 && group->getNumUnits() == 1 && group->getArmyID() < 0
            && !group->isBusy() && !group->AI_isForceSeparate() && unit->canMove() && !unit->hasMoved()
            && !unit->isHurt() && !unit->isWanted() && !unit->AI_isCityGarrison(source)
            && (mission == NO_MISSIONAI || mission == MISSIONAI_PROPERTY_CONTROL_MAINTAIN)
            && unit->m_contractualState != CONTRACTUAL_STATE_FOUND_WORK
            && unit->m_contractualState != CONTRACTUAL_STATE_AWAITING_ANSWER;
        if (!record.available) ++protectedUnits;
        if (from >= 0) cities[from].controllers.push_back((int)controllers.size());
        controllers.push_back(record);
    }
    // Release feasibility only becomes stricter during this pass: incoming
    // promises never improve retention. Remove unavailable donors once, rather
    // than revisiting every stationed unit for each requesting city.
    int eligible = 0;
    for (size_t c = 0; c < cities.size(); ++c)
    {
        std::vector<int> available;
        bool dangerChecked = false, danger = false;
        for (size_t u = 0; u < cities[c].controllers.size(); ++u)
        {
            const int i = cities[c].controllers[u];
            DispatchController& record = controllers[i];
            if (!record.available || !CityPropertyDispatch::canRelease(cities[c].retention, record.current)) continue;
            if (!dangerChecked) { danger = cities[c].city->AI_isDanger(); dangerChecked = true; }
            if (danger) { record.available = false; continue; }
            available.push_back(i); ++eligible;
        }
        cities[c].controllers.swap(available);
    }
    // Local training is already chosen by the existing city/contract scheduler.
    // Reserve its base capability only when completion is close, in the same
    // ten-turn units as incoming assistance; do not replace important production.
    std::vector<DispatchNeed> needs;
    for (int c = 0; c < (int)cities.size(); ++c)
    {
        CvCity* city = cities[c].city;
        const UnitTypes training = city->getProductionUnit();
        if (training != NO_UNIT && city->getProductionUnitAI() == UNITAI_PROPERTY_CONTROL && city->getProductionTurnsLeft() <= 3)
        {
            std::vector<int> amounts(GC.getNumPropertyInfos(), 0), ignored(GC.getNumPropertyInfos(), 0);
            if (dispatchPropertySources(GC.getUnitInfo(training).getPropertyManipulators(), city, ignored, amounts, false))
                CityPropertyDispatch::reserve(cities[c].budget, amounts, std::max(1, city->getProductionTurnsLeft()));
        }
        int urgent = 0;
        for (int p = 0; p < GC.getNumPropertyInfos(); ++p) urgent = std::max(urgent, cities[c].budget.need[p]);
        if (urgent > 0) needs.push_back(DispatchNeed(c, urgent));
    }
    std::stable_sort(needs.begin(), needs.end());
    int pathChecks = 0, failed = 0, assigned = 0, batches = 0;
    for (size_t n = 0; n < needs.size(); ++n)
    {
        const int receiver = needs[n].city;
        DispatchCity& destination = cities[receiver];
        std::vector<std::pair<int, int> > nearby;
        for (int c = 0; c < (int)cities.size(); ++c)
            if (c != receiver && !cities[c].controllers.empty() && cities[c].city->area() == destination.city->area())
                nearby.push_back(std::make_pair(stepDistance(cities[c].city->getX(), cities[c].city->getY(), destination.city->getX(), destination.city->getY()), c));
        std::sort(nearby.begin(), nearby.end());
        for (size_t d = 0; d < nearby.size(); ++d)
        {
            DispatchCity& donor = cities[nearby[d].second];
            CityPropertyDispatch::Budget tentativeDonor = donor.retention, tentativeReceiver = destination.budget;
            std::vector<int> selected;
            CvUnitAI* leader = NULL;
            int happiness = 0;
            for (size_t u = 0; u < donor.controllers.size(); ++u)
            {
                const int recordIndex = donor.controllers[u];
                DispatchController& record = controllers[recordIndex];
                if (!record.available) continue;
                CvUnitAI* unit = static_cast<CvUnitAI*>(player.getUnit(record.id));
                if (unit == NULL || unit->isDelayedDeath() || !unit->atPlot(donor.city->plot()) || unit->getGroup()->getNumUnits() != 1) continue;
                if (leader != NULL && (unit->getUnitType() != leader->getUnitType() || !unit->canJoinGroup(unit->plot(), leader->getGroup()))) continue;
                const int happyCost = unit->isMilitaryHappiness() ? std::max(0, player.getHappyPerMilitaryUnit()) : 0;
                if (happyCost > 0 && donor.happinessBudget < donor.happinessSpent + happiness + happyCost) continue;
                if (!CityPropertyDispatch::canRelease(tentativeDonor, record.current)) continue;
                std::vector<int> targetCurrent, targetMobile;
                if (!dispatchPropertyCapabilities(unit, destination.city, targetCurrent, targetMobile)) continue;
                if (!CityPropertyDispatch::useful(tentativeReceiver, targetMobile)
                    || !CityPropertyDispatch::canRelease(tentativeDonor, record.current)) continue;
                // Do not improve one property by creating a new deficit in another.
                bool harmful = false;
                for (int p = 0; p < GC.getNumPropertyInfos(); ++p)
                    if (targetMobile[p] < 0 && (__int64)tentativeReceiver.need[p] - (__int64)targetMobile[p] * 10 > 0) harmful = true;
                if (harmful) continue;
                if (leader == NULL) leader = unit;
                else { unit->joinGroup(leader->getGroup()); if (unit->getGroup() != leader->getGroup()) continue; }
                record.mobile = targetMobile;
                CityPropertyDispatch::release(tentativeDonor, record.current);
                CityPropertyDispatch::reserve(tentativeReceiver, targetMobile);
                happiness += happyCost;
                selected.push_back(recordIndex);
            }
            if (leader == NULL) continue;
            ++pathChecks;
            int turns = 0;
            bool success = leader->generatePath(destination.city->plot(), MOVE_WITH_CAUTION, true, &turns, maxHelpTurns);
            if (success && turns <= maxHelpTurns)
            {
                // Endpoint and whole-group path are checked by the real pathfinder.
                // pushMission may execute callbacks: never dereference leader/group afterwards.
                success = leader->getGroup()->pushMissionInternal(MISSION_MOVE_TO, destination.city->getX(), destination.city->getY(),
                    MOVE_WITH_CAUTION, false, false, MISSIONAI_PROPERTY_CONTROL_RESPONSE, destination.city->plot());
            }
            else success = false;
            if (!success)
            {
                ++failed;
                for (size_t u = 0; u < selected.size(); ++u)
                {
                    CvUnit* unit = player.getUnit(controllers[selected[u]].id);
                    if (unit != NULL && unit->atPlot(donor.city->plot()))
                    {
                        if (unit->getGroup() == NULL || unit->getGroup()->getNumUnits() > 1)
                            if (!splitDispatchController(unit)) continue;
                        unit->getGroup()->clearMissionQueue();
                        unit->getGroup()->AI_setMissionAI(MISSIONAI_PROPERTY_CONTROL_MAINTAIN, donor.city->plot(), NULL);
                    }
                }
                propertyDispatchTrace(player, "failed", destination.city->getID(), donor.city->getID(), (int)selected.size(), turns);
                continue; // Tentative budgets and reservations were not published.
            }
            for (size_t u = 0; u < selected.size(); ++u)
            {
                DispatchController& record = controllers[selected[u]];
                record.available = false;
                // A reserved responder is no longer an idle contract bidder.
                player.getContractBroker().removeUnit(record.id);
                CityPropertyDispatch::release(donor.budget, record.current);
                CityPropertyDispatch::release(donor.retention, record.current);
                CityPropertyDispatch::reserve(destination.budget, record.mobile, turns);
            }
            donor.happinessSpent += happiness;
            assigned += (int)selected.size(); ++batches;
            propertyDispatchTrace(player, "sent", destination.city->getID(), donor.city->getID(), (int)selected.size(), turns);
        }
    }
    int retired = 0;
    for (size_t c = 0; c < cities.size(); ++c)
        retired += player.getContractBroker().fulfillCityPropertyRequests(cities[c].city->plot(), cities[c].budget.need);
    propertyDispatchTrace(player, "retired-requests", 0, 0, retired, 0);
    propertyDispatchTrace(player, "summary", (int)needs.size(), protectedUnits, assigned, pathChecks);
    propertyDispatchTrace(player, "batches", incoming, failed, batches, (int)controllers.size());
    propertyDispatchTrace(player, "coverage", (int)cities.size(), offCity, unsupported, (int)controllers.size());
    propertyDispatchTrace(player, "eligible", (int)cities.size(), 0, eligible, 0);
#ifdef C2C_PROPERTY_DISPATCH_STATS
    size_t bytes = cities.capacity() * sizeof(DispatchCity) + controllers.capacity() * sizeof(DispatchController);
    for (size_t c = 0; c < cities.size(); ++c)
        bytes += (cities[c].budget.need.capacity() + cities[c].budget.present.capacity() + cities[c].budget.minimum.capacity()
            + cities[c].retention.need.capacity() + cities[c].retention.present.capacity() + cities[c].retention.minimum.capacity()
            + cities[c].controllers.capacity()) * sizeof(int);
    for (size_t u = 0; u < controllers.size(); ++u) bytes += (controllers[u].current.capacity() + controllers[u].mobile.capacity()) * sizeof(int);
    propertyDispatchTrace(player, "snapshot-bytes", (int)cities.size(), (int)controllers.size(), (int)bytes, GC.getNumPropertyInfos());
#endif
}

bool CvUnitAI::AI_followCityPropertyRequest()
{
    PROFILE_FUNC();
    if (getDomainType() != DOMAIN_LAND || GET_PLAYER(getOwner()).isHumanPlayer() || GET_PLAYER(getOwner()).isNPC()) return false;
    const MissionAITypes mission = getGroup()->AI_getMissionAIType();
    if (mission != NO_MISSIONAI && mission != MISSIONAI_PROPERTY_CONTROL_MAINTAIN && mission != MISSIONAI_PROPERTY_CONTROL_RESPONSE) return false;
    if (m_contractualState == CONTRACTUAL_STATE_FOUND_WORK || m_contractualState == CONTRACTUAL_STATE_AWAITING_ANSWER) return false;
    const CvCity* local = plot()->getPlotCity();
    const CvPlot* target = getGroup()->AI_getMissionAIType() == MISSIONAI_PROPERTY_CONTROL_RESPONSE ? getGroup()->AI_getMissionAIPlot() : NULL;
    const CvCity* city = target != NULL ? target->getPlotCity() : NULL;
    const CvCity* evaluation = local != NULL && local->getOwner() == getOwner() ? local : city;
    if (evaluation == NULL || evaluation->getOwner() != getOwner()) return false;
    std::vector<int> current, mobile;
    if (!dispatchPropertyCapabilities(this, evaluation, current, mobile)) return false;
    if (city != NULL && city->getOwner() == getOwner() && !atPlot(target))
    {
        if (generateSafePathforVulnerable(target))
        {
            CvSelectionGroup* group = getGroup();
            return group->pushMissionInternal(MISSION_MOVE_TO, target->getX(), target->getY(), MOVE_WITH_CAUTION,
                false, false, MISSIONAI_PROPERTY_CONTROL_RESPONSE, target);
        }
        getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
        return true;
    }
    if (local != NULL && local->getOwner() == getOwner())
        return getGroup()->pushMissionInternal(MISSION_SKIP, getX(), getY(), 0, false, false, MISSIONAI_PROPERTY_CONTROL_MAINTAIN, plot());
    return false;
}

bool CvUnitAI::AI_fulfillPropertyControlNeed()
{
	PROFILE_EXTRA_FUNC();
	if (AI_followCityPropertyRequest()) return true;
	std::vector<int> currentControl, mobileControl;
	const PropertyTypes currentProperty = propertyControlCapabilities(this, currentControl, mobileControl);
	PropertyTypes mobileProperty = NO_PROPERTY;
	int mobileScore = 0;
	for (int i = 0; i < (int)mobileControl.size(); ++i)
	{
		if (mobileControl[i] > mobileScore) { mobileScore = mobileControl[i]; mobileProperty = (PropertyTypes)i; }
	}
	PropertyTypes BestProperty = currentProperty != NO_PROPERTY ? currentProperty : mobileProperty;

	//No property has been found
	// If it doesn't change properties then it can't fulfill control needs
	// Calvitix test : assign to another task
	if (BestProperty == NO_PROPERTY)
	{
		AI_setUnitAIType(UNITAI_RESERVE);
		getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, NO_MISSIONAI);
		CvWString StrunitAIType;
		CvWString StrUnitName;
		if (gUnitLogLevel > 2)
		{
			StrunitAIType = GC.getUnitAIInfo(AI_getUnitAIType()).getType();
			StrUnitName = m_szName;
			if (StrUnitName.length() == 0)
			{
				StrUnitName = getName(0).GetCString();
			}
			logAiEvaluations(3, "	Player %S Unit %S of type %S - No property Improv. found - Set to UNITAI_RESERVE", GET_PLAYER(getOwner()).getCivilizationDescription(0), StrUnitName.GetCString(), StrunitAIType.GetCString());
		}
		return true; // Role change and SKIP already handled this update.
	}

	// A city still reporting need already includes this controller's contribution.
	// Keep its active help (including BuildUp) instead of evaluating departure.
	if (propertyControllerNeededLocally(this, currentProperty))
	{
		return getGroup()->pushMissionInternal(MISSION_SKIP, getX(), getY(), 0, false, false, MISSIONAI_PROPERTY_CONTROL_MAINTAIN, plot());
	}

	// A city still reporting need already includes this controller's contribution.
	// Keep its active help (including BuildUp) instead of evaluating departure.
	if (propertyControllerNeededLocally(this, currentProperty))
	{
		return getGroup()->pushMissionInternal(MISSION_SKIP, getX(), getY(), 0, false, false, MISSIONAI_PROPERTY_CONTROL_MAINTAIN, plot());
	}

	const CvPlayer& player = GET_PLAYER(getOwner());


	const PropertyTypes crimeProperty = (PropertyTypes)GC.getInfoTypeForString("PROPERTY_CRIME", true);
	const int buildUpRetention = currentProperty == NO_PROPERTY ? 0 : std::max(0, currentControl[currentProperty] - mobileControl[currentProperty]);
	std::vector<PropertyControlCandidate> candidates;
	foreach_(CvCity* city, player.cities())
	{
		const int score = scorePropertyControlNeed(this, city, currentProperty, mobileProperty, crimeProperty, buildUpRetention);
		if (score > 0) candidates.push_back(PropertyControlCandidate(city, score, atPlot(city->plot())));
	}
	// This AI already elects to remain in place on six of ten relocation rolls.
	// For controllers stationed in our city, make that choice before paying for
	// remote paths. Score/BuildUp retention still decide whether relocation is
	// even worth considering. Reuse this roll if a remote target is selected.
	int relocationRoll = -1;
	const CvCity* stationedCity = plot()->getPlotCity();
	if (stationedCity != NULL && stationedCity->getOwner() == getOwner() && propertyRelocationPreferred(candidates))
	{
		relocationRoll = GC.getGame().getSorenRandNum(10, "Should Move for PropControl");
		if (relocationRoll <= 5)
		{
			return getGroup()->pushMissionInternal(MISSION_SKIP, getX(), getY(), 0, false, false, MISSIONAI_PROPERTY_CONTROL_MAINTAIN, plot());
		}
	}
	const int selected = selectPropertyControlCandidate(this, candidates);
	// Capture the successful validation path before any later callbacks. No second search is needed.
	const CvPlot* selectedEndTurn = selected >= 0 && !candidates[selected].current ? getPathEndTurnPlot() : NULL;
	const scoring::ScoreResult<CvCity> bestCityScore(selected >= 0,
		scoring::ItemScore<CvCity>(selected >= 0 ? candidates[selected].city : NULL, selected >= 0 ? candidates[selected].score : 0));

	if (bestCityScore.found)
	{
		CvCity* bestCity = bestCityScore.result.item;
		BestProperty = atPlot(bestCity->plot()) ? currentProperty : mobileProperty;
		CvWString StrunitAIType;
		CvWString StrUnitName;

		//Calvitix test
		#define MAX_TARGET_CONTROL_MAINTAIN 10
		if (bestCityScore.result.score <= 110)
		{  // No need. Set to Reserve

			int iNbControlForces = player.AI_plotTargetMissionAIs(bestCity->plot(), MISSIONAI_PROPERTY_CONTROL_MAINTAIN, NULL, 0);
			const CvPlayerAI& playerAI = GET_PLAYER(getOwner());
			if (playerAI.AI_getTotalProperty(BestProperty) < -2000) //Global Property is OK
			{
				iNbControlForces -= 3;
			}

			if (iNbControlForces > MAX_TARGET_CONTROL_MAINTAIN)
			{
                // Not needed right now: hold in place AS a property-control unit (the dedicated
                // pool waits in-role) instead of flipping to RESERVE. That flip, paired with the
                // (now removed) reserve-side conversion, caused the RESERVE<->PROPERTY_CONTROL
                // oscillation. Property units keep their role from creation and are never flipped.
                getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_PROPERTY_CONTROL_MAINTAIN, plot());
                return true;
            }


		}


		if (gUnitLogLevel > 2)
		{
			StrunitAIType = GC.getUnitAIInfo(AI_getUnitAIType()).getType();
			StrUnitName = m_szName;
			if (StrUnitName.length() == 0)
			{
				StrUnitName = getName(0).GetCString();
			}
			logBBAI("	Player %S Unit %S of type %S - City that need the most Prop Control Help : %S  (Value %d)", GET_PLAYER(getOwner()).getCivilizationDescription(0), StrUnitName.GetCString(), StrunitAIType.GetCString(), bestCity->getName().GetCString(), bestCityScore.result.score);
		}


		if (atPlot(bestCity->plot()))
		{
			if (gUnitLogLevel > 2)
			{
				logBBAI("	Player %S Unit %S of type %S - Staying in City %S", GET_PLAYER(getOwner()).getCivilizationDescription(0), StrUnitName.GetCString(), StrunitAIType.GetCString(), bestCity->getName().GetCString());
			}
			return getGroup()->pushMissionInternal(MISSION_SKIP, bestCity->getX(), bestCity->getY(), 0, false, false, MISSIONAI_PROPERTY_CONTROL_MAINTAIN, bestCity->plot());
		}

		//Calvitix. To reduce the amount of PropControl units moving, RNG 50% only will move
		const int iValue = relocationRoll >= 0 ? relocationRoll : GC.getGame().getSorenRandNum(10, "Should Move for PropControl");

		if (iValue <= 5) //50%
		{
			if (gUnitLogLevel > 2)
			{
				logBBAI("	Player %S Unit %S of type %S - Could move, but Staying in City %S due to RNG", GET_PLAYER(getOwner()).getCivilizationDescription(0), StrUnitName.GetCString(), StrunitAIType.GetCString(), bestCity->getName().GetCString());
			}
			return getGroup()->pushMissionInternal(MISSION_SKIP, getX(), getY(), 0, false, false, MISSIONAI_PROPERTY_CONTROL_MAINTAIN, plot());
		}
		else
		{

			if (selectedEndTurn != NULL)
			{
				if (gUnitLogLevel > 2)
				{
					logBBAI("	Player %S Unit %S of type %S - Is moving, and vulnerable. Safe Move path generated to City %S", GET_PLAYER(getOwner()).getCivilizationDescription(0), StrUnitName.GetCString(), StrunitAIType.GetCString(), bestCity->getName().GetCString());
				}
				const CvPlot* endTurnPlot = selectedEndTurn;
				return getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY(), MOVE_WITH_CAUTION, false, false, MISSIONAI_PROPERTY_CONTROL_RESPONSE, bestCity->plot());
			}
			if (gUnitLogLevel > 2)
			{
				logBBAI("	Player %S Unit %S of type %S - Generate Escort request for move to City %S  (Value %d)", GET_PLAYER(getOwner()).getCivilizationDescription(0), StrUnitName.GetCString(), StrunitAIType.GetCString(), bestCity->getName().GetCString(), bestCityScore.result.score);
			}

			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
			return true;
		}
	}
	else
	{   //No city needs control right now: hold in place as a property-control unit (the dedicated
        //pool waits in-role); do NOT flip to RESERVE (that caused the oscillation).
        getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_PROPERTY_CONTROL_MAINTAIN, plot());
        return true;
    }
	return false;
}

bool CvUnitAI::AI_arrest()
{
	if (!canArrest())
	{
		return false;
	}
	const int iMoves = getMoves();

	doArrest();

	return getMoves() > iMoves;
}


bool CvUnitAI::AI_ambush(int iOddsThreshold, bool bAssassinationOnly)
{
	PROFILE_FUNC();

	const CvPlot* pPlot = getGroup()->plot();
	if (!pPlot) return false;

	if (!bAssassinationOnly && canHunt(pPlot))
	{
		return getGroup()->pushMissionInternal(MISSION_HUNT, pPlot->getX(), pPlot->getY());
	}

	const bool bAssassin = isAssassin();

	if (pPlot->isCity(true) && !bAssassin)
	{
		return false;
	}
	const bool bCanAssassinate = canAmbush(pPlot, bAssassin);
	if (bAssassinationOnly && !bCanAssassinate)
	{
		return false;
	}
	// Getting 1 here which is to say there is a target but the target doesn't validate later.
	const int iVisDef = pPlot->getNumVisiblePotentialEnemyDefenders(this);
	// No point to count this if there are visible defenders as it is only relevant when there are none.
	const int iVisDefless = iVisDef < 1 ? pPlot->getNumVisiblePotentialEnemyDefenderless(this) : 0;

	if (iVisDef < 1 && iVisDefless < 1)
	{
		return false;
	}
	bool bWinLikely = false;
	int iAdjustedOddsThreshold;
	int iValue;

	PROFILE("CvUnitAI::AI_anyAttack.FoundTargetforAmbush");
	if (iVisDef > 0)
	{
		iValue = getGroup()->AI_attackOdds(pPlot, true, true, &bWinLikely, iOddsThreshold);
		iAdjustedOddsThreshold = AI_finalOddsThreshold(pPlot, iOddsThreshold);
		if (bWinLikely)
		{
			iValue += iOddsThreshold;
		}
	}
	else
	{
		iValue = 100 * iVisDefless;
		iAdjustedOddsThreshold = 1;
		bWinLikely = true;
	}

	// Increase value on our territory since we really want to get rid of enemies there even if it costs us a few losses
	if (pPlot->getOwner() == getOwner() && bWinLikely)
	{
		iValue *= 2;
	}

	if (iValue > 0 && iValue >= iAdjustedOddsThreshold)
	{
		if (bCanAssassinate)
		{
			return getGroup()->pushMissionInternal(MISSION_ASSASSINATE, pPlot->getX(), pPlot->getY());
		}
		return getGroup()->pushMissionInternal(MISSION_AMBUSH, pPlot->getX(), pPlot->getY());
	}
	return false;
}


bool CvUnitAI::AI_activateStatus(bool bStack, PromotionTypes eStatus, CvUnit* pUnit)
{
	if (eStatus == NO_PROMOTION)
	{
		return false;
	}
	//function to turn on or off status on whole group or just unit
	if (!bStack)
	{
		if (!pUnit)
		{
			return false;
		}
		if (pUnit->canDoCommand(COMMAND_STATUS, eStatus, 0))
		{
			pUnit->doCommand(COMMAND_STATUS, eStatus, 0);
			return true;
		}
	}
	else if (getGroup()->canDoCommand(COMMAND_STATUS, eStatus, 0, false, false, true))
	{
		getGroup()->doCommand(COMMAND_STATUS, eStatus, 0);
		return true;
	}
	return false;
}

bool CvUnitAI::AI_selectStatus(bool bStack, CvUnit* pUnit)
{
    NAVAL_DIAG_PHASE(STATUS,bStack,pUnit!=NULL,0,0);

	PROFILE_EXTRA_FUNC();
	if (bStack)
	{
		if (!getGroup())
		{
			NAVAL_DIAG_PHASE_RETURN((false));
		}
	}
	else if (!pUnit)
	{
		NAVAL_DIAG_PHASE_RETURN((false));
	}
	int iBestValue = 0;
	int iWorstValue = 0;
	PromotionTypes eBestStatus = NO_PROMOTION;
	PromotionTypes eWorstStatus = NO_PROMOTION;
	const MissionAITypes eMissionAI = getGroup() ? getGroup()->AI_getMissionAIType() : NO_MISSIONAI;
	const UnitAITypes eUnitAI = bStack ? AI_getUnitAIType() : pUnit->AI_getUnitAIType();

	const PromotionLineTypes promotionLineStandout = GC.getPROMOTIONLINE_STANDOUT();

	const int iNumStatusPromotions = GC.getNumStatusPromotions();
	int iWorstDamagePercent = -1;
	int iCombatDanger = -1; // This evaluation is read-only until the final activation.

	for (int iI = 0; iI < iNumStatusPromotions; iI++)
	{
		int iValue = 0;
		const PromotionTypes eStatus = GC.getStatusPromotion(iI);
		// Permanent schools are chosen by train(), which evaluates the whole line.
		if (DoctrinePromotions::isBase(eStatus)) continue;
		const CvPromotionInfo& kPromotion = GC.getPromotionInfo(eStatus);
		const PromotionTypes eRemoveStatus = GC.getStatusDefaultPromotion(iI);

		if (kPromotion.getLinePriority() != 1)
		{
			if (bStack)
			{
				if (getGroup()->canDoCommand(COMMAND_STATUS, eStatus, 0, false, false, true) || isHasPromotion(eStatus))
				{
					//Keep things as thin as possible here - program in as new statuses are introduced
					//Stay the Hand
					int iTemp = kPromotion.getDefenseOnlyChange();
					if (iTemp != 0)
					{
						if (eMissionAI == MISSIONAI_SPREAD ||
							eMissionAI == MISSIONAI_SPREAD_CORPORATION ||
							eMissionAI == MISSIONAI_CONSTRUCT ||
							eMissionAI == MISSIONAI_HURRY ||
							eMissionAI == MISSIONAI_GREAT_WORK ||
							eMissionAI == MISSIONAI_FOUND ||
							eMissionAI == MISSIONAI_BUILD ||
							eMissionAI == MISSIONAI_ASSAULT ||
							eMissionAI == MISSIONAI_CARRIER ||
							eMissionAI == MISSIONAI_PICKUP ||
							eMissionAI == MISSIONAI_INQUISITION ||
							eMissionAI == MISSIONAI_HURRY_FOOD ||
							eMissionAI == MISSIONAI_HEAL_SUPPORT)
						{
							iTemp *= 10;
						}
						else
						{
							iTemp *= -10;
						}
						iValue += iTemp;
					}

					//Stealth Defense
					iTemp = kPromotion.getStealthDefenseChange();
					if (iTemp != 0)
					{
						if (eUnitAI == UNITAI_SETTLE ||
							eUnitAI == UNITAI_WORKER ||
							eUnitAI == UNITAI_COUNTER ||
							eUnitAI == UNITAI_CITY_COUNTER ||
							eUnitAI == UNITAI_CITY_SPECIAL ||
							eUnitAI == UNITAI_ESCORT_SEA ||
							eUnitAI == UNITAI_HUNTER_ESCORT ||
							eUnitAI == UNITAI_MISSIONARY ||
							eUnitAI == UNITAI_PROPHET ||
							eUnitAI == UNITAI_SCIENTIST ||
							eUnitAI == UNITAI_GENERAL ||
							eUnitAI == UNITAI_MERCHANT ||
							eUnitAI == UNITAI_ENGINEER ||
							eUnitAI == UNITAI_WORKER_SEA ||
							eUnitAI == UNITAI_SETTLER_SEA ||
							eUnitAI == UNITAI_MISSIONARY_SEA ||
							eUnitAI == UNITAI_HUNTER ||
							eUnitAI == UNITAI_GREAT_HUNTER)
						{
							iTemp *= 100;
						}
						iValue += iTemp;
					}

					//Standout
					if (hasInvisibleAbility())
					{
						iTemp = kPromotion.getNumNegatesInvisibilityTypes();

						if (iTemp != 0)
						{
							if (promotionLineStandout != NO_PROMOTIONLINE && kPromotion.getPromotionLine() == promotionLineStandout)
							{
								if ((eMissionAI == MISSIONAI_GUARD_CITY ||
									eMissionAI == MISSIONAI_GUARD_BONUS ||
									eMissionAI == MISSIONAI_GUARD_TRADE_NET ||
									eMissionAI == MISSIONAI_CLAIM_TERRITORY) ||
									(eUnitAI == UNITAI_ATTACK_CITY ||
										eUnitAI == UNITAI_CITY_DEFENSE ||
										eUnitAI == UNITAI_PROPERTY_CONTROL ||
										eUnitAI == UNITAI_PROPERTY_CONTROL_SEA ||
										eUnitAI == UNITAI_ESCORT) ||
									m_iGarrisonCity != -1)
								{
									iTemp *= 10;
								}
								else if ((eMissionAI == MISSIONAI_SPREAD ||
									eMissionAI == MISSIONAI_SPREAD_CORPORATION ||
									eMissionAI == MISSIONAI_CONSTRUCT ||
									eMissionAI == MISSIONAI_HURRY ||
									eMissionAI == MISSIONAI_GREAT_WORK ||
									eMissionAI == MISSIONAI_EXPLORE ||
									eMissionAI == MISSIONAI_BLOCKADE ||
									eMissionAI == MISSIONAI_PILLAGE ||
									eMissionAI == MISSIONAI_FOUND ||
									eMissionAI == MISSIONAI_BUILD ||
									eMissionAI == MISSIONAI_ASSAULT ||
									eMissionAI == MISSIONAI_CARRIER ||
									eMissionAI == MISSIONAI_PICKUP ||
									eMissionAI == MISSIONAI_INQUISITION ||
									eMissionAI == MISSIONAI_HURRY_FOOD ||
									eMissionAI == MISSIONAI_HEAL_SUPPORT) ||
									(eUnitAI == UNITAI_ANIMAL ||
										eUnitAI == UNITAI_ATTACK ||
										eUnitAI == UNITAI_PILLAGE ||
										eUnitAI == UNITAI_ATTACK_SEA ||
										eUnitAI == UNITAI_RESERVE_SEA ||
										eUnitAI == UNITAI_EXPLORE_SEA ||
										eUnitAI == UNITAI_ASSAULT_SEA ||
										eUnitAI == UNITAI_SPY_SEA ||
										eUnitAI == UNITAI_CARRIER_SEA ||
										eUnitAI == UNITAI_MISSILE_CARRIER_SEA ||
										eUnitAI == UNITAI_PIRATE_SEA ||
										eUnitAI == UNITAI_PARADROP ||
										eUnitAI == UNITAI_HUNTER ||
										eUnitAI == UNITAI_GREAT_HUNTER ||
										eUnitAI == UNITAI_HEALER ||
										eUnitAI == UNITAI_HEALER_SEA ||
										eUnitAI == UNITAI_GREAT_HUNTER ||
										eUnitAI == UNITAI_INFILTRATOR))
								{
									iTemp *= -10;
								}
							}
							else
							{
								//if this Promotion isn't on the Standout Promotionline NegatesInvisibility is a bad thing
								iTemp *= -100;
							}
							iValue += iTemp;
						}
					}

					//Enhanced March
					//Moves
					iTemp = kPromotion.getMovesChange();
					if (iTemp != 0)
					{
						if ((eMissionAI == MISSIONAI_SPREAD ||
							eMissionAI == MISSIONAI_SPREAD_CORPORATION ||
							eMissionAI == MISSIONAI_CONSTRUCT ||
							eMissionAI == MISSIONAI_HURRY ||
							eMissionAI == MISSIONAI_GREAT_WORK ||
							eMissionAI == MISSIONAI_BUILD ||
							eMissionAI == MISSIONAI_INQUISITION ||
							eMissionAI == MISSIONAI_HURRY_FOOD ||
							eMissionAI == MISSIONAI_PROPERTY_CONTROL_RESPONSE) ||
							(eUnitAI == UNITAI_MISSIONARY ||
								eUnitAI == UNITAI_PROPHET ||
								eUnitAI == UNITAI_ARTIST ||
								eUnitAI == UNITAI_SCIENTIST ||
								eUnitAI == UNITAI_GENERAL ||
								eUnitAI == UNITAI_MERCHANT ||
								eUnitAI == UNITAI_ENGINEER))
						{
							iTemp *= 20;
						}
						else if (eUnitAI != UNITAI_HUNTER && eUnitAI != UNITAI_HUNTER_ESCORT)
						{
							iTemp *= -10;
						}
						else if (plot() != NULL && plot()->getOwner() == getOwner())
						{
							iTemp *= 5;
						}
						iValue += iTemp;
					}
					//combat%
					iTemp = kPromotion.getCombatPercent();
					if (plot() != NULL && iTemp != 0)
					{
						const int iRange = visibilityRange(plot()) + baseMoves();
						if (iCombatDanger < 0) iCombatDanger = GET_PLAYER(getOwner()).AI_getPlotDanger(plot(), iRange) > 0 ? 1 : 0;
						if (iCombatDanger > 0)
						{
							iTemp *= 2;
						}
						if (plot()->getOwner() != getOwner())
						{
							iTemp *= 4;
						}
						iValue += iTemp;
					}
					//selfheal
					iTemp = kPromotion.getSelfHealModifier();
					if (iTemp != 0)
					{
						// No status is activated until all candidates have been evaluated.
						if (iWorstDamagePercent < 0) iWorstDamagePercent = getGroup()->getWorstDamagePercent();
						iTemp *= iWorstDamagePercent;
					}
					iTemp /= 10;
					iValue += iTemp;

				}
				// If any can remove
				if (iValue < 0 && !getGroup()->canDoCommand(COMMAND_STATUS, (int)eRemoveStatus, 0, false, false, false))
				{
					iValue = 0;
				}
			}
			else
			{
				if (pUnit->canDoCommand(COMMAND_STATUS, eStatus, 0) || pUnit->isHasPromotion(eStatus))
				{
					//conditions to add to ivalue and subtract from iValue

					//Standout
					if (pUnit->hasInvisibleAbility())
					{
						int iTemp = 0;
						//only care about the NegatesInvisibilityTypes which have an actual effect
						for (int iN = 0; iN < kPromotion.getNumNegatesInvisibilityTypes(); iN++)
						{
							InvisibleTypes invsibleType = (InvisibleTypes)kPromotion.getNegatesInvisibilityType(iN);
							if (pUnit->getInvisibleType() == invsibleType || pUnit->hasInvisibilityType(invsibleType))
							{
								iTemp++;
							}
						}
						if (iTemp != 0)
						{
							if (promotionLineStandout != NO_PROMOTIONLINE && kPromotion.getPromotionLine() == promotionLineStandout)
							{
								if (eUnitAI == UNITAI_SETTLE ||
									eUnitAI == UNITAI_WORKER ||
									eUnitAI == UNITAI_COUNTER ||
									eUnitAI == UNITAI_CITY_COUNTER ||
									eUnitAI == UNITAI_CITY_SPECIAL ||
									eUnitAI == UNITAI_ESCORT_SEA ||
									eUnitAI == UNITAI_HUNTER_ESCORT ||
									eUnitAI == UNITAI_MISSIONARY ||
									eUnitAI == UNITAI_PROPHET ||
									eUnitAI == UNITAI_SCIENTIST ||
									eUnitAI == UNITAI_GENERAL ||
									eUnitAI == UNITAI_MERCHANT ||
									eUnitAI == UNITAI_ENGINEER ||
									eUnitAI == UNITAI_WORKER_SEA ||
									eUnitAI == UNITAI_SETTLER_SEA ||
									eUnitAI == UNITAI_MISSIONARY_SEA ||
									eUnitAI == UNITAI_ESCORT)
								{
									iTemp *= 100;
								}
								else if ((eMissionAI == MISSIONAI_GUARD_CITY ||
									eMissionAI == MISSIONAI_GUARD_BONUS ||
									eMissionAI == MISSIONAI_GUARD_TRADE_NET ||
									eMissionAI == MISSIONAI_CLAIM_TERRITORY) ||
									(eUnitAI == UNITAI_ATTACK_CITY ||
										eUnitAI == UNITAI_CITY_DEFENSE ||
										eUnitAI == UNITAI_PROPERTY_CONTROL ||
										eUnitAI == UNITAI_PROPERTY_CONTROL_SEA ||
										eUnitAI == UNITAI_ESCORT) ||
									m_iGarrisonCity != -1)
								{
									iTemp *= 10;
								}
								else if ((eMissionAI == MISSIONAI_SPREAD ||
									eMissionAI == MISSIONAI_SPREAD_CORPORATION ||
									eMissionAI == MISSIONAI_CONSTRUCT ||
									eMissionAI == MISSIONAI_HURRY ||
									eMissionAI == MISSIONAI_GREAT_WORK ||
									eMissionAI == MISSIONAI_EXPLORE ||
									eMissionAI == MISSIONAI_BLOCKADE ||
									eMissionAI == MISSIONAI_PILLAGE ||
									eMissionAI == MISSIONAI_FOUND ||
									eMissionAI == MISSIONAI_BUILD ||
									eMissionAI == MISSIONAI_ASSAULT ||
									eMissionAI == MISSIONAI_CARRIER ||
									eMissionAI == MISSIONAI_PICKUP ||
									eMissionAI == MISSIONAI_INQUISITION ||
									eMissionAI == MISSIONAI_HURRY_FOOD ||
									eMissionAI == MISSIONAI_HEAL_SUPPORT) ||
									(eUnitAI == UNITAI_ANIMAL ||
										eUnitAI == UNITAI_ATTACK ||
										eUnitAI == UNITAI_PILLAGE ||
										eUnitAI == UNITAI_ATTACK_SEA ||
										eUnitAI == UNITAI_RESERVE_SEA ||
										eUnitAI == UNITAI_EXPLORE_SEA ||
										eUnitAI == UNITAI_ASSAULT_SEA ||
										eUnitAI == UNITAI_SPY_SEA ||
										eUnitAI == UNITAI_CARRIER_SEA ||
										eUnitAI == UNITAI_MISSILE_CARRIER_SEA ||
										eUnitAI == UNITAI_PIRATE_SEA ||
										eUnitAI == UNITAI_PARADROP ||
										eUnitAI == UNITAI_HUNTER ||
										eUnitAI == UNITAI_HUNTER_ESCORT ||
										eUnitAI == UNITAI_GREAT_HUNTER ||
										eUnitAI == UNITAI_HEALER ||
										eUnitAI == UNITAI_HEALER_SEA ||
										eUnitAI == UNITAI_GREAT_HUNTER ||
										eUnitAI == UNITAI_INFILTRATOR))
								{
									iTemp *= -10;
								}
							}
							else
							{
								//if this Promotion isn't on the Standout Promotionline NegatesInvisibility is a bad thing
								iTemp *= -100;
							}
							iValue += iTemp;
						}
					}

					//Enhanced March
					//Moves
					int iTemp = kPromotion.getMovesChange();
					if (iTemp != 0)
					{
						if ((eMissionAI == MISSIONAI_SPREAD ||
							eMissionAI == MISSIONAI_SPREAD_CORPORATION ||
							eMissionAI == MISSIONAI_CONSTRUCT ||
							eMissionAI == MISSIONAI_HURRY ||
							eMissionAI == MISSIONAI_GREAT_WORK ||
							eMissionAI == MISSIONAI_BUILD ||
							eMissionAI == MISSIONAI_INQUISITION ||
							eMissionAI == MISSIONAI_HURRY_FOOD ||
							eMissionAI == MISSIONAI_PROPERTY_CONTROL_RESPONSE) ||
							(eUnitAI == UNITAI_MISSIONARY ||
								eUnitAI == UNITAI_PROPHET ||
								eUnitAI == UNITAI_ARTIST ||
								eUnitAI == UNITAI_SCIENTIST ||
								eUnitAI == UNITAI_GENERAL ||
								eUnitAI == UNITAI_MERCHANT ||
								eUnitAI == UNITAI_ENGINEER))
						{
							iTemp *= 20;
						}
						else if (eUnitAI != UNITAI_HUNTER && eUnitAI != UNITAI_HUNTER_ESCORT)
						{
							iTemp *= -10;
						}
						else if (pUnit->plot() != NULL && pUnit->plot()->getOwner() == pUnit->getOwner())
						{
							iTemp *= 5;
						}
						iValue += iTemp;
					}
					//combat%
					iTemp = kPromotion.getCombatPercent();
					if (pUnit->plot() != NULL && iTemp != 0)
					{
						const int iRange = pUnit->visibilityRange(plot()) + pUnit->baseMoves();
						/*if (GET_PLAYER(getOwner()).AI_getVisiblePlotDanger(plot(), iRange, false, getGroup(), 70))*/
						if (iCombatDanger < 0) iCombatDanger = GET_PLAYER(getOwner()).AI_getPlotDanger(plot(), iRange) > 0 ? 1 : 0;
						if (iCombatDanger > 0)
						{
							iTemp *= 2;
						}
						if (pUnit->plot()->getOwner() != pUnit->getOwner())
						{
							iTemp *= 4;
						}
						iValue += iTemp;
					}
					//selfheal
					iTemp = kPromotion.getSelfHealModifier();
					iTemp *= pUnit->getDamage();
					iTemp /= 10;
					iValue += iTemp;
				}

				if (iValue < 0 && !pUnit->canDoCommand(COMMAND_STATUS, (int)eRemoveStatus, 0))
				{
					iValue = 0;
				}
			}
		}
		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			eBestStatus = eStatus;
		}
		if (iValue < iWorstValue)
		{
			iWorstValue = iValue;
			eWorstStatus = eRemoveStatus;
		}
	}
	const __int64 iCompare = (__int64)iBestValue + iWorstValue; // Negative scores are costs, not extra benefits.

	if (iCompare < 0 && eWorstStatus != NO_PROMOTION)
	{
		NAVAL_DIAG_PHASE_RETURN((AI_activateStatus(bStack, eWorstStatus, pUnit) && !GC.getPromotionInfo(eWorstStatus).isQuick()));
		//setWorstStatus
	}

	if (iCompare > 0 && eBestStatus != NO_PROMOTION && !(bStack ? isHasPromotion(eBestStatus) : pUnit->isHasPromotion(eBestStatus)))
	{
		NAVAL_DIAG_PHASE_RETURN((AI_activateStatus(bStack, eBestStatus, pUnit) && !GC.getPromotionInfo(eBestStatus).isQuick()));
		//setBestStatus
	}
	NAVAL_DIAG_PHASE_RETURN((false));
}

bool CvUnitAI::AI_groupSelectStatus()
{
	PROFILE_EXTRA_FUNC();
	foreach_(CvUnit * pLoopUnit, getGroup()->units())
	{
		if (AI_selectStatus(false, pLoopUnit))
		{
			return true;
		}
	}
	return false;
}

bool CvUnitAI::AI_InvestigatorFulfillment()
{
	PROFILE_EXTRA_FUNC();
	if (investigatorNeededLocally(this))
	{
		return getGroup()->pushMissionInternal(MISSION_SKIP, getX(), getY(), 0, false, false, MISSIONAI_INVESTIGATOR_MAINTAIN, plot());
	}

	const CvPlayerAI& player = GET_PLAYER(getOwner());
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvCity * cityX, player.cities())
	{
		const CvPlot* plotX = cityX->plot();

		const int iResponders = player.AI_plotTargetMissionAIs(plotX, MISSIONAI_INVESTIGATOR_RESPONSE, NULL, 0);
		// At this point iNumCriminals is more of a subjective need base
		const int iNumCriminals = cityX->plot()->getNumCriminals() - iResponders;

		// Each criminal makes things exponentially more valuable.
		int iValue = 100 * iNumCriminals * iNumCriminals;

		if (iValue <= 0) continue;

		if (atPlot(plotX))
		{
			iValue *= 2;
		}
		else
		{
			int iPathTurns = 0;
			if (generateIgnoreDangerPath(plotX, &iPathTurns))
			{
				if (iPathTurns > 0)
				{
					iValue /= iPathTurns; // Reduce value by distance
				}
				if (iResponders > 0)
				{
					iValue /= iResponders; // Reduce value by # of other units already on the way
				}
			}
			else
			{
				iValue = 0;
			}
		}
		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			pBestPlot = plotX;
		}
	}

	if (pBestPlot)
	{
		if (atPlot(pBestPlot))
		{
			return getGroup()->pushMissionInternal(MISSION_SKIP, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_INVESTIGATOR_MAINTAIN, pBestPlot);
		}
		if (generateSafePathforVulnerable(pBestPlot))
		{
			const CvPlot* endTurnPlot = getPathEndTurnPlot();
			return getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY(), MOVE_WITH_CAUTION, false, false, MISSIONAI_INVESTIGATOR_RESPONSE, pBestPlot);
		}
		getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
		return true;
	}
	return false;
}

//in progress
bool CvUnitAI::AI_establishStackSeeInvisibleCoverage()
{
	PROFILE_EXTRA_FUNC();
	if (isHuman())
	{
		return false;
	}
	UnitTypes	eBestUnit = NO_UNIT;
	int iUnitValue = 0;
	CvUnitSelectionCriteria criteria;

	InvisibleTypes eVisible = NO_INVISIBLE;
	if (m_contractsLastEstablishedTurn != GC.getGame().getGameTurn())
	{
		for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
		{
			eBestUnit = NO_UNIT;
			iUnitValue = 0;
			eVisible = (InvisibleTypes)iI;
			criteria.m_eVisibility = eVisible;
			criteria.m_bNoNegativeProperties = true;

			if (getDomainType() == DOMAIN_LAND)
			{
				int iNbSeeInvUnits = getGroup()->countSeeInvisibleActive(UNITAI_SEE_INVISIBLE, eVisible);
				//Calvitix (as there are Dogs with other roles, that can see inv - or other units with ATTACKS, ATTACK_CITY roles)
				iNbSeeInvUnits += getGroup()->countSeeInvisibleActive(UNITAI_ATTACK, eVisible);
				iNbSeeInvUnits += getGroup()->countSeeInvisibleActive(UNITAI_ATTACK_CITY, eVisible);
				iNbSeeInvUnits += getGroup()->countSeeInvisibleActive(UNITAI_COUNTER, eVisible);
				iNbSeeInvUnits += getGroup()->countSeeInvisibleActive(UNITAI_COLLATERAL, eVisible);
				if (iNbSeeInvUnits < 1)
				{
					iUnitValue = GET_PLAYER(getOwner()).AI_bestAreaUnitAIValue(UNITAI_SEE_INVISIBLE, area(), &eBestUnit, &criteria);
					if (eBestUnit != NO_UNIT)
					{
						GET_PLAYER(getOwner()).getContractBroker().advertiseWork(HIGHEST_PRIORITY_ESCORT_PRIORITY,
							NO_UNITCAPABILITIES,
							getX(),
							getY(),
							this,
							UNITAI_SEE_INVISIBLE,
							-1,
							&criteria);

						m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
						m_contractualState = CONTRACTUAL_STATE_AWAITING_ANSWER;
						setToWaitOnUnitAI(UNITAI_SEE_INVISIBLE, true);

						if (gUnitLogLevel > 2)
						{
							logBBAI("	%S (%d) at (%d,%d) [stack size %d] requests UNITAI_SEE_INVISIBLE Unit at priority %d", GET_PLAYER(getOwner()).getCivilizationDescription(0), getID(), getX(), getY(), getGroup()->getNumUnits(), HIGH_PRIORITY_ESCORT_PRIORITY);
						}

						//getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_SEE_INVISIBLE);
						return true;
					}
				}
			}
			else if (getDomainType() == DOMAIN_SEA)
			{
				int iNbSeeInvUnits = getGroup()->countSeeInvisibleActive(UNITAI_SEE_INVISIBLE_SEA, eVisible);
				iNbSeeInvUnits += getGroup()->countSeeInvisibleActive(UNITAI_ATTACK_SEA, eVisible);
				iNbSeeInvUnits += getGroup()->countSeeInvisibleActive(UNITAI_CARRIER_SEA, eVisible);
				iNbSeeInvUnits += getGroup()->countSeeInvisibleActive(UNITAI_ASSAULT_SEA, eVisible);
				iNbSeeInvUnits += getGroup()->countSeeInvisibleActive(UNITAI_PIRATE_SEA, eVisible);
				//Calvitix TODO - Other Roles can see inv. too
				if (iNbSeeInvUnits < 1)
				{
					iUnitValue = GET_PLAYER(getOwner()).AI_bestAreaUnitAIValue(UNITAI_SEE_INVISIBLE_SEA, area(), &eBestUnit, &criteria);
					if (eBestUnit != NO_UNIT)
					{
						GET_PLAYER(getOwner()).getContractBroker().advertiseWork(HIGHEST_PRIORITY_ESCORT_PRIORITY,
							NO_UNITCAPABILITIES,
							getX(),
							getY(),
							this,
							UNITAI_SEE_INVISIBLE_SEA,
							-1,
							&criteria);

						m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
						m_contractualState = CONTRACTUAL_STATE_AWAITING_ANSWER;
						setToWaitOnUnitAI(UNITAI_SEE_INVISIBLE_SEA, true);

						if (gUnitLogLevel > 2)
						{
							logBBAI("	%S (%d) at (%d,%d) [stack size %d] requests UNITAI_SEE_INVISIBLE Sea Unit at priority %d", GET_PLAYER(getOwner()).getCivilizationDescription(0), getID(), getX(), getY(), getGroup()->getNumUnits(), HIGH_PRIORITY_ESCORT_PRIORITY);
						}

						//Calvitix Test getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_SEE_INVISIBLE);
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool CvUnitAI::generateSafePathforVulnerable(const CvPlot* pToPlot, int* piPathTurns) const
{
	return generatePath(pToPlot, MOVE_WITH_CAUTION, true, piPathTurns);
}

bool CvUnitAI::generateIgnoreDangerPath(const CvPlot* pToPlot, int* piPathTurns) const
{
	return generatePath(pToPlot, MOVE_IGNORE_DANGER, true, piPathTurns);
}

void CvUnitAI::setToWaitOnUnitAI(UnitAITypes eUnitAI, bool bAdd)
{
	PROFILE_EXTRA_FUNC();
	int iIndex = (int)eUnitAI;

	if (bAdd)
	{
		m_aiWaitingOnUnitAITypes.push_back(iIndex);
	}
	else
	{
		std::vector<int> m_aiTempWaitingOnUnitAITypes;
		m_aiTempWaitingOnUnitAITypes.clear();
		int iCount = m_aiWaitingOnUnitAITypes.size();
		bool bFound = false;
		for (int iI = 0; iI < iCount; iI++)
		{
			if (iIndex == m_aiWaitingOnUnitAITypes[iI] && !bFound)
			{
				bFound = true;
			}
			else
			{
				m_aiTempWaitingOnUnitAITypes.push_back(m_aiWaitingOnUnitAITypes[iI]);
			}
		}
		m_aiWaitingOnUnitAITypes.clear();
		iCount = m_aiTempWaitingOnUnitAITypes.size();
		for (int iI = 0; iI < iCount; iI++)
		{
			m_aiWaitingOnUnitAITypes.push_back(m_aiTempWaitingOnUnitAITypes[iI]);
		}
	}
	setWaitingOnUnitAIAny();
}

bool CvUnitAI::isWaitingOnUnitAI(int iIndex) const
{
	return !isHuman() && algo::any_of_equal(m_aiWaitingOnUnitAITypes, iIndex);
}

bool CvUnitAI::isWaitingOnUnitAIAny() const
{
	return m_bWaitingOnUnitAIAny;
}

void CvUnitAI::setWaitingOnUnitAIAny()
{
	PROFILE_EXTRA_FUNC();
	m_bWaitingOnUnitAIAny = false;
	if (isHuman())
	{
		return;
	}
	for (int iI = 0; iI < NUM_UNITAI_TYPES; iI++)
	{
		if (isWaitingOnUnitAI(iI))
		{
			m_bWaitingOnUnitAIAny = true;
			break;
		}
	}
}

bool CvUnitAI::AI_isNegativePropertyUnit() const
{
	PROFILE_EXTRA_FUNC();
	const CvPropertyManipulators* propertyManipulators = GC.getUnitInfo(getUnitType()).getPropertyManipulators();
	if (propertyManipulators != NULL)
	{
		foreach_(const CvPropertySource * pSource, propertyManipulators->getSources())
		{
			//	We have a source for a property - value is crudely just the AIweight of that property times the source size (which is expected to only depend on the player)
			if (pSource->getType() == PROPERTYSOURCE_CONSTANT)
			{
				if (GC.getPropertyInfo(pSource->getProperty()).getAIWeight() < 0)
				{
					if (static_cast<const CvPropertySourceConstant*>(pSource)->getAmountPerTurn(getGameObject()) > 0)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

int CvUnitAI::getMyAggression(int iAttackProb) const
{
	int iAggression = m_pUnitInfo->getAggression() + getLevel() - 1;
	iAggression *= getMaxHP() - getDamage();
	iAggression /= 100;
	iAggression *= iAttackProb;
	return iAggression / 100;
}

// K-Mod. Defend nearest city against invading attack stacks.
/*bool CvUnitAI::AI_defensiveCollateral(int iThreshold, int iSearchRange)
{
	PROFILE_FUNC();
	FAssert(collateralDamage() > 0);

	CvPlayerAI const& kOwner = GET_PLAYER(getOwner());

	CvPlot const* pDefencePlot = NULL;
	//if (getPlot().isCity(false, getTeam()))
	// advc.001: Looks like karadoc misinterpreted the eForTeam parameter
	if (this->getPlot().isCity() && getPlot().getOwner() == getOwner())
		pDefencePlot = plot();
	else
	{
		int iClosest = MAX_INT;
		for (SquareIter it(*this, iSearchRange); it.hasNext(); ++it)
		{
			CvPlot const& p = *it;
			//if (p.isCity(false, getTeam()))
			// advc.001: (see comment above)
			if (p.isCity() && p.getOwner() == getOwner())
			{
				if (kOwner.AI_isAnyPlotDanger(p))
				{
					pDefencePlot = &p;
					break;
				}
				int iDist = it.currStepDist();
				if (iDist < iClosest)
				{
					iClosest = iDist;
					pDefencePlot = &p;
				}
			}
		}
	}

	if (pDefencePlot == NULL)
		return false;

	int const iEnemyAttack = kOwner.AI_localAttackStrength(pDefencePlot, NO_TEAM,
			getDomainType(), iSearchRange);
	int const iOurDefence = kOwner.AI_localDefenceStrength(pDefencePlot, getTeam(),
			getDomainType(), 0);
	bool const bDanger = (iEnemyAttack > iOurDefence);

	CvPlot* pBestPlot = NULL; // advc (note): Maximizing iThreshold
	for (SquareIter it(*this, iSearchRange, false); it.hasNext(); ++it)
	{
		CvPlot const& p = *it;
		if (!isArea(p.getArea())) // advc.opt
			continue;
		int const iEnemies = p.getNumVisibleEnemyDefenders(this);
		int iPathTurns;
		if (iEnemies > 0 && generatePath(p, NO_MOVEMENT_FLAGS, true, &iPathTurns, 1))
		{
			//int iValue = getGroup()->AI_attackOdds(pLoopPlot, false);
			int iValue = AI_getGroup()->AI_getWeightedOdds(&p);
			if (iValue > 0 && iEnemies >= std::min(4, collateralDamageMaxUnits()))
			{
				int iOurAttack = kOwner.AI_localAttackStrength(&p, getTeam(),
						getDomainType(), iSearchRange, true, true, true);
				int iEnemyDefence = kOwner.AI_localDefenceStrength(&p, NO_TEAM,
						getDomainType(), 0);
				iValue += std::max(0, (bDanger ? 75 : 45) * (3 * iOurAttack - iEnemyDefence) /
						std::max(1, 3 * iEnemyDefence));
				// note: the scale is choosen to be around +50% when attack == defence, while in danger.
				if (bDanger && it.currStepDist() <= 1)
				{
					// enemy is ready to attack, and strong enough to win. We might as well hit them.
					iValue += 20;
				}
			}
			if (iValue >= iThreshold)
			{
				iThreshold = iValue;
				pBestPlot = &getPathEndTurnPlot();
			}
		}
	}

	if (pBestPlot != NULL)
	{
		pushGroupMoveTo(*pBestPlot);
		return true;
	}

	return false;
}*/

#include "CvNavalCommitment.inl"
#include "CvCivilianDecision.inl"
#include "CvSettlerColonization.inl"

bool CvUnitAI::AI_isAvailableForDefenseDemand() const
{
    return m_contractsLastEstablishedTurn==GC.getGame().getGameTurn()
        && (m_contractualState==CONTRACTUAL_STATE_AWAITING_WORK || m_contractualState==CONTRACTUAL_STATE_NO_WORK_FOUND)
        && !isDelayedDeath();
}

int CvUnitAI::AI_defenderDemandForPlot(const CvPlot* target) const
{
    const CvCity* city=target?target->getPlotCity():NULL;
    if(city)return city->AI_minDefenders();
    return target?std::max(1,AI_getPlotDefendersNeeded(target,1)):1;
}

bool CvUnitAI::AI_fulfillDefenseDemand(const CvPlot* target,int maxTurns)
{
    CvContractBroker& broker=GET_PLAYER(getOwner()).getContractBroker();
    if(!target || target->getOwner()!=getOwner())
    {DefenseDemandPerf::count(DefenseDemandPerf::CANCELLED);broker.cancelDefenseAssignment(getID());contractFulfilled();return false;}
    const MissionAITypes mission=target->getPlotCity()?MISSIONAI_GUARD_CITY:MISSIONAI_GUARD_BONUS;
    if(atPlot(target))
    {
        DefenseDemandPerf::count(DefenseDemandPerf::ARRIVED);contractFulfilled();
        if(target->getPlotCity())getGroup()->AI_setAsGarrison(target->getPlotCity());
        getGroup()->pushMission(MISSION_SKIP,-1,-1,0,false,false,mission,target);return true;
    }
    if(!broker.defenseNeedsDefender(target) || !broker.canReleaseDefender(this))
    {DefenseDemandPerf::count(DefenseDemandPerf::CANCELLED);broker.cancelDefenseAssignment(getID());contractFulfilled();return false;}
    const bool airlift=target->getPlotCity() && canAirliftAt(plot(),target->getX(),target->getY());
    const int flags=MOVE_NO_ENEMY_TERRITORY|MOVE_AVOID_ENEMY_UNITS;int turns;
    if(!airlift && !generatePath(target,flags,true,&turns,maxTurns))
    {DefenseDemandPerf::count(DefenseDemandPerf::EXECUTION_FAILED);broker.cancelDefenseAssignment(getID());contractFulfilled();return false;}
    if(!airlift)broker.groupDefenseReinforcements(this,target,maxTurns);
    const PlayerTypes owner=getOwner();const int id=getID();
    getGroup()->AI_setAsGarrison(NULL);
    contractFulfilled();bool result=true;
    // Keep the final requested place as the destination across turns.
    if(airlift)getGroup()->pushMission(MISSION_AIRLIFT,target->getX(),target->getY(),0,false,false,mission,target);
    else result=getGroup()->pushMissionInternal(MISSION_MOVE_TO,target->getX(),target->getY(),flags,false,false,mission,target);
    // Orders can execute synchronously, move or kill their unit. Resolve it
    // again before affirming a city garrison; never count it before arrival.
    CvUnit* arrived=GET_PLAYER(owner).getUnit(id);
    if(arrived && !arrived->isDelayedDeath() && arrived->getGroup() && arrived->atPlot(target) && target->getPlotCity())arrived->getGroup()->AI_setAsGarrison(target->getPlotCity());
    if(!result && arrived && !arrived->isDelayedDeath() && arrived->getGroup() && !arrived->getGroup()->isBusy())
    {DefenseDemandPerf::count(DefenseDemandPerf::TRAVEL_BLOCKED);arrived->getGroup()->pushMission(MISSION_SKIP,-1,-1,0,false,false,mission,target);}
    return true;
}

// A guard mission belongs to its group, including after a forced AI wakeup or
// an interrupted queue. Do not replace an accepted reinforcement order with
// healing, upgrades, regrouping or a search for a different destination.
bool CvUnitAI::AI_hasDefenseDemandDestination() const
{
    if(!DefenseDemandPerf::enabled() || isHuman() || isNPC() || getDomainType()!=DOMAIN_LAND
        || isCargo() || isDelayedDeath() || !getGroup())return false;
    const CvSelectionGroup* group=getGroup();const CvUnit* head=group->getHeadUnit();
    if(!head || !head->AI_isCityAIType())return false;
    const MissionAITypes mission=group->AI_getMissionAIType();const CvPlot* target=group->AI_getMissionAIPlot();
    if((mission!=MISSIONAI_GUARD_CITY && mission!=MISSIONAI_GUARD_BONUS) || !target
        || target->getOwner()!=getOwner() || group->atPlot(target))return false;
    if(target->getPlotCity())return true;
    const ImprovementTypes type=target->getMilitaryImprovementType();
    return type!=NO_IMPROVEMENT && (GC.getImprovementInfo(type).isMilitaryStructure()
        || GC.getImprovementInfo(type).isUpgradeRequiresFortify() || GC.getImprovementInfo(type).isActsAsCity());
}

bool CvUnitAI::AI_canRegroupDefense(const CvUnit* source,const CvUnit* target)
{
    if(!DefenseDemandPerf::enabled())return true;
    if((source && static_cast<const CvUnitAI*>(source)->AI_hasDefenseDemandDestination())
        || (target && static_cast<const CvUnitAI*>(target)->AI_hasDefenseDemandDestination()))
    {DefenseDemandPerf::count(DefenseDemandPerf::DEFENSE_ORDER_SKIPS);return false;}
    return true;
}

bool CvUnitAI::AI_continueDefenseDemand()
{
    if(isHuman() || isNPC() || !AI_isCityAIType() || getDomainType()!=DOMAIN_LAND
        || isCargo() || isDelayedDeath() || !getGroup() || getGroup()->getHeadUnit()!=this)return false;
    CvSelectionGroup* group=getGroup();const MissionAITypes mission=group->AI_getMissionAIType();
    if(mission!=MISSIONAI_GUARD_CITY && mission!=MISSIONAI_GUARD_BONUS)return false;
    const CvPlot* target=group->AI_getMissionAIPlot();
    if(!target)return false;
    if(atPlot(target))
    {if(target->getPlotCity())group->AI_setAsGarrison(target->getPlotCity());return false;}
    const ImprovementTypes improvement=target->getMilitaryImprovementType();
    if(target->getOwner()!=getOwner() || (!target->getPlotCity()
        && (improvement==NO_IMPROVEMENT || (!GC.getImprovementInfo(improvement).isMilitaryStructure()
            && !GC.getImprovementInfo(improvement).isUpgradeRequiresFortify()
            && !GC.getImprovementInfo(improvement).isActsAsCity()))))
    {group->AI_setMissionAI(NO_MISSIONAI,NULL,NULL);return false;}
    if(group->isBusy())return true;
    if(!group->canAllMove())
    {group->pushMission(MISSION_SKIP,-1,-1,0,false,false,mission,target);return true;}
    DefenseDemandPerf::count(DefenseDemandPerf::RESUMED);
    const PlayerTypes owner=getOwner();const int id=getID();
    group->AI_setAsGarrison(NULL);
    const bool result=group->pushMissionInternal(MISSION_MOVE_TO,target->getX(),target->getY(),
        MOVE_NO_ENEMY_TERRITORY|MOVE_AVOID_ENEMY_UNITS,false,false,mission,target);
    // A temporary obstruction keeps the destination but ends this turn's
    // retry loop. The group may try the same order on its next turn.
    CvUnit* alive=GET_PLAYER(owner).getUnit(id);
    if(!result && alive && !alive->isDelayedDeath() && alive->getGroup() && !alive->getGroup()->isBusy())
    {DefenseDemandPerf::count(DefenseDemandPerf::TRAVEL_BLOCKED);alive->getGroup()->pushMission(MISSION_SKIP,-1,-1,0,false,false,mission,target);}
    return true;
}

bool CvUnitAI::AI_defenseDemandAdjacentAttack()
{
    if(!DefenseDemandPerf::enabled() || isHuman() || isNPC() || !AI_isCityAIType() || getDomainType()!=DOMAIN_LAND
        || isCargo() || isDelayedDeath() || !getGroup() || getGroup()->getHeadUnit()!=this
        || !canAttack() || (isMadeAttack() && !isBlitz()))return false;
    CvSelectionGroup* group=getGroup();
    const MissionAITypes mission=group->AI_getMissionAIType();
    const CvPlot* target=group->AI_getMissionAIPlot();
    const MissionData* order=group->getMissionFromQueue(0);
    if((mission!=MISSIONAI_GUARD_CITY && mission!=MISSIONAI_GUARD_BONUS)
        || !target || target->getOwner()!=getOwner() || atPlot(target) || group->isBusy()
        || !group->canAllMove() || !order || order->eMissionType!=MISSION_MOVE_TO
        || (order->iFlags&MOVE_DIRECT_ATTACK))return false;
    const CvPlot* best=NULL;int bestOdds=94;
    foreach_(const CvPlot* nearby,plot()->adjacent())
    {
        if(!nearby->isVisibleEnemyUnit(this) || !group->canEnterOrAttackPlot(nearby,false))continue;
        // Detection alone does not permit combat: a criminal blending into a
        // city can be visible yet coexist with this guard. Chasing it used to
        // send reinforcements back to the same city every other turn forever.
        const CvUnit* defender=nearby->getBestDefender(NO_PLAYER,getOwner(),this,true);
        if(!defender || !canAttack(*defender))continue;
        const int odds=group->AI_attackOdds(nearby,false);
        if(odds<=bestOdds)continue;
        int turns;
        if(!generatePath(nearby,MOVE_DIRECT_ATTACK,true,&turns,1))continue;
        best=nearby;bestOdds=odds;
    }
    if(!best)return false;
    // Queue both orders before executing combat, which can invalidate this
    // unit or its group. Mission AI continues to name the original target.
    DefenseDemandPerf::count(DefenseDemandPerf::ADJACENT_ATTACKS);
    group->clearMissionQueue();
    group->pushMission(MISSION_MOVE_TO,best->getX(),best->getY(),MOVE_DIRECT_ATTACK,true,false,mission,target);
    group->pushMission(MISSION_MOVE_TO,target->getX(),target->getY(),MOVE_NO_ENEMY_TERRITORY|MOVE_AVOID_ENEMY_UNITS,true,false,mission,target);
    group->startMission();
    return true;
}
