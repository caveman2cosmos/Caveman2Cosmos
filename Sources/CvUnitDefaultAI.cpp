// unitAI.cpp

#include "CvGameCoreDLL.h"
#include "CvArea.h"
#include "CvBuildingInfo.h"
#include "CvBonusInfo.h"
#include "CvCity.h"
#include "CvCityAI.h"
#include "CvContractBroker.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvImprovementInfo.h"
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
#include "CvUnit.h"
#include "CvUnitSelectionCriteria.h"
#include "CvDLLFAStarIFaceBase.h"
#ifdef USE_OLD_PATH_GENERATOR
#include "FAStarNode.h"
#endif
#include <CvWorkerService.h>

PlayerTypes	CvUnitDefaultAI::m_cachedPlayer = NO_PLAYER;
CvReachablePlotSet* CvUnitDefaultAI::m_cachedMissionaryPlotset = NULL;
static PlayerTypes eCachedTargetCityPlayer = NO_PLAYER;
static PlayerTypes eCachedAttackOddsPlayer = NO_PLAYER;

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

void CvUnitDefaultAI::clearCaches()
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

CvUnitDefaultAI::CvUnitDefaultAI(CvUnit* unit)
{
	m_unit = unit;
	reset(NO_UNITAI, true);
}


CvUnitDefaultAI::~CvUnitDefaultAI()
{
	uninit();
}


CvUnitDefaultAI& CvUnitDefaultAI::operator=(const CvUnitDefaultAI& other)
{
	m_iBirthmark = other.m_iBirthmark;
	m_eUnitAIType = other.m_eUnitAIType;
	m_iAutomatedAbortTurn = other.m_iAutomatedAbortTurn;
	m_iGroupLeadOverride = other.m_iGroupLeadOverride;
	m_contractsLastEstablishedTurn = other.m_contractsLastEstablishedTurn;
	m_contractualState = other.m_contractualState;
	m_eIntendedConstructBuilding = other.m_eIntendedConstructBuilding;
	m_iPredictedHitPoints = other.m_iPredictedHitPoints;
	m_bHasAttacked = other.m_bHasAttacked;
	m_bWaitingOnUnitAIAny = other.m_bWaitingOnUnitAIAny;
	m_iGarrisonCity = other.m_iGarrisonCity;
	m_iGenericValue = other.m_iGenericValue;
	m_aiWaitingOnUnitAITypes = other.m_aiWaitingOnUnitAITypes;

	//static_cast<CvUnit&>(*m_unit) = static_cast<const CvUnit&>(other)

	return *this;
}


void CvUnitDefaultAI::init(UnitAITypes eUnitAI, int iBirthmark)
{
	reset(eUnitAI);

	//--------------------------------
	// Init other game data
	setBirthmark(iBirthmark);

	FAssertMsg(getUnitAIType() != NO_UNITAI, "AI_getUnitAIType() is not expected to be equal with NO_UNITAI");
	GET_PLAYER(m_unit->getOwner()).AI_changeNumAIUnits(getUnitAIType(), 1);
}


void CvUnitDefaultAI::uninit()
{
}


void CvUnitDefaultAI::reset(UnitAITypes eUnitAI, bool bConstructorCall)
{
	FAssert(bConstructorCall || eUnitAI != NO_UNITAI);

	uninit();

	m_iBirthmark = 0;

	m_eUnitAIType = eUnitAI;

	m_iAutomatedAbortTurn = -1;

	m_iGroupLeadOverride = -1;

	m_contractsLastEstablishedTurn = -1;
	m_contractualState = CONTRACTUAL_STATE_NONE;

	m_eIntendedConstructBuilding = NO_BUILDING;

	m_iPredictedHitPoints = -1;
	m_bHasAttacked = false;
	m_bWaitingOnUnitAIAny = false;

	m_iGarrisonCity = -1;
	m_iGenericValue = -1;

	m_aiWaitingOnUnitAITypes.clear();
}
void CvUnitDefaultAI::SendLog(CvWString function, CvWString message)
{
	//WIP, wrapper of the new FLB logger, to create correct payload for m_unit class
	CvWString aiType = "CvUnitAI";


	// logAIJson(aiType, m_unit->getName(), function, message);

}

// update returns true when we should abort the loop and wait until next slice
bool CvUnitDefaultAI::update()
{
	PROFILE_FUNC();

	logBBAI("AI_Update for unit %d of owner %d\n", m_unit->getID(), m_unit->getOwner());

#ifdef _DEBUG
	m_unit->getGroup()->validateLocations(true);
#endif

	//	If m_unit->canMove() is false there is not much we can do.  Empirically m_unit can happen
	//	Not sure why but suspect either:
	//	1) The loop in CvPlayer that calls Update() for the active player but only ends turn
	//	   when there are no non-busy or movable units left.  Each time around m_unit loop it's
	//	   possible for units to be left in m_unit state due to unexecutable orders, and stack splits
	//	   which causes ALL units to be asked to reconsider their orders again the next time round the loop
	//	2) When not all units in the stack have the same movement allowance and the head unit can't move after
	//	   execution of the current order
	//	Whatever the cause, the safest thing to do is just push a SKIP without the expense of considering all
	//	the other possibilities it won't be able to execute anyway due to having no movement left
	if (!m_unit->canMove())
	{
		if (!m_unit->getGroup()->isBusy())
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP);
		}
		return false;
	}

	m_unit->getGroup()->setActivityType(ACTIVITY_AWAKE);

	FAssertMsg(m_unit->isGroupHead(), "isGroupHead is expected to be true"); // XXX is m_unit a good idea???

	CvReachablePlotSet::ClearCache();

	if (m_unit->getDomainType() == DOMAIN_LAND)
	{
		if (m_unit->plot()->isWater() && !m_unit->canMoveAllTerrain() && !m_unit->plot()->isSeaTunnel())
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return false;
		}
		const CvUnit* pTransportUnit = m_unit->getTransportUnit();

		if (pTransportUnit != NULL && (pTransportUnit->getGroup()->hasMoved() || pTransportUnit->getGroup()->headMissionQueueNode() != NULL))
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return false;
		}
	}

	if (afterAttack())
	{
		return false;
	}

	if (m_unit->getGroup()->isAutomated())
	{
		switch (m_unit->getGroup()->getAutomateType())  // NOLINT(clang-diagnostic-switch-enum)
		{
		case AUTOMATE_BUILD:
		{
			if (getUnitAIType() == UNITAI_WORKER)
			{
				workerMove();
			}
			else if (getUnitAIType() == UNITAI_WORKER_SEA)
			{
				workerSeaMove();
			}
			else FErrorMsg("error");

			break;
		}
		case AUTOMATE_NETWORK:
		{
			networkAutomated();
			break;
		}
		case AUTOMATE_CITY:
		{
			cityAutomated();
			break;
		}
		case AUTOMATE_EXPLORE:
		{
			switch (m_unit->getDomainType())
			{
			case DOMAIN_SEA:
			{
				exploreSeaMove();
				break;
			}
			case DOMAIN_AIR:
			{
				// if we are cargo (on a carrier), hold if the carrier is not done moving yet
				const CvUnit* pTransportUnit = m_unit->getTransportUnit();
				if (pTransportUnit != NULL
				&& pTransportUnit->isAutomated()
				&& pTransportUnit->canMove()
				&& pTransportUnit->getGroup()->getActivityType() != ACTIVITY_HOLD)
				{
					m_unit->getGroup()->pushMission(MISSION_SKIP);
					break;
				}
				exploreAirMove(); // Have air units explore like AI units do
				break;
			}
			case DOMAIN_LAND:
			{
				exploreMove();
				break;
			}
			default: FErrorMsg("error");
			}
			// if we have air cargo (we are a carrier), and are done moving, explore with the aircraft as well
			if (m_unit->hasCargo() && m_unit->getDomainCargo() == DOMAIN_AIR
			&& (!m_unit->canMove() || m_unit->getGroup()->getActivityType() == ACTIVITY_HOLD))
			{
				std::vector<CvUnit*> aCargoUnits;
				m_unit->getCargoUnits(aCargoUnits);
				if (!aCargoUnits.empty())
				{
					m_unit->validateCargoUnits();
				}
				foreach_(const CvUnit * pCargoUnit, aCargoUnits)
				{
					FAssert(m_unit->isAutomated())

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
			if (getUnitAIType() == UNITAI_MISSIONARY)
			{
				missionaryMove();
			}
			else if (m_unit->getGroup()->hasUnitOfAI(UNITAI_MISSIONARY))
			{
				CvSelectionGroup* pGroup = m_unit->getGroup();
				m_unit->joinGroup(NULL);
				m_unit->joinGroup(pGroup);
				m_unit->getGroup()->setAutomateType(AUTOMATE_RELIGION);
			}
			else
			{
				m_unit->getGroup()->setAutomateType(NO_AUTOMATE);
			}
			break;
		}
		case AUTOMATE_PILLAGE:
		{
			AutomatedpillageMove();
			break;
		}
		case AUTOMATE_HUNT:
		{
			SearchAndDestroyMove();
			break;
		}
		case AUTOMATE_CITY_DEFENSE:
		{
			cityDefense();
			break;
		}
		case AUTOMATE_BORDER_PATROL:
		{
			borderPatrol();
			break;
		}
		case AUTOMATE_PIRATE:
		{
			pirateSeaMove();
			break;
		}
		case AUTOMATE_HURRY:
		{
			merchantMove();
			break;
		}
		//Yes, these automations do the same thing, but they act differently for different units.
		case AUTOMATE_AIRSTRIKE:
		case AUTOMATE_AIRBOMB:
		{
			autoAirStrike();
			break;
		}
		case AUTOMATE_AIR_RECON:
		{
			exploreAirMove();
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
			// not sure how m_unit can happen but empirically it's been seen,
			// then lose the automation.
			if (m_unit->getShadowUnit() == NULL)
			{
				m_unit->getGroup()->setAutomateType(NO_AUTOMATE);
			}
			else
			{
				shadowMove();
			}
			break;
		}
		default: FErrorMsg("error");
		}
		// if no longer automated, then we want to bail
		return !m_unit->isDelayedDeath() && !m_unit->getGroup()->isAutomated();
	}

	// No confirmed garrison city until we reaffirm it with another set
	m_iAffirmedGarrisonCity = -1;

	if (m_unit->isNPC())
	{
		PROFILE("CvUnitDefaultAI::Update.NPC");

		doUnitAIMove();
	}
	else
	{
		PROFILE("CvUnitDefaultAI::Update.civ");

		doUnitAIMove();
	}

	if (NULL != m_unit->getGroup() && !m_unit->isDelayedDeath() && m_iGarrisonCity != -1 && m_iAffirmedGarrisonCity == -1)
	{
		// This group has done something else (presumably of higher priority)
		//	so should no longer be considered part of the city's garrison
		setAsGarrison();
	}

#ifdef _DEBUG
	if (NULL != m_unit->getGroup() && !m_unit->isDelayedDeath())
	{
		m_unit->getGroup()->validateLocations(true);
	}
#endif

	return !m_unit->isDelayedDeath() && isAwaitingContract();
}

void CvUnitDefaultAI::doUnitAIMove()
{
	//	If a unit has a contract mission for a unit that has just finished
	//	give the unit a chance to hook up with it by force-processing contracts
	//	before doing other AI activities
	if (m_unit->getGroup()->AI_getMissionAIType() == MISSIONAI_CONTRACT_UNIT && processContracts(0))
	{
		return;
	}

	if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS)
	&& (!m_unit->isAnimal() || m_unit->isGatherHerd())
	&& m_unit->getGroup()->doMergeCheck())
	{
		return;
	}

	switch (getUnitAIType())
	{
	case UNITAI_UNKNOWN:
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		break;

	case UNITAI_ANIMAL:
		animalMove();
		break;

	case UNITAI_SETTLE:
		settleMove();
		break;

	case UNITAI_WORKER:
		workerMove();
		break;

	case UNITAI_ATTACK:
		if (m_unit->isNPC())
		{
			barbAttackMove();
		}
		else
		{
			attackMove();
		}
		break;

	case UNITAI_ATTACK_CITY:
		attackCityMove();
		break;

	case UNITAI_COLLATERAL:
		collateralMove();
		break;

	case UNITAI_PILLAGE:
		pillageMove();
		break;

	case UNITAI_RESERVE:
		reserveMove();
		break;

	case UNITAI_COUNTER:
		counterMove();
		break;

	case UNITAI_PILLAGE_COUNTER:
		pillageCounterMove();
		break;

	case UNITAI_PARADROP:
		paratrooperMove();
		break;

	case UNITAI_CITY_DEFENSE:
		cityDefenseMove();
		break;

	case UNITAI_CITY_COUNTER:
	case UNITAI_CITY_SPECIAL:
		cityDefenseExtraMove();
		break;

	case UNITAI_EXPLORE:
		exploreMove();
		break;

	case UNITAI_HUNTER:
		SearchAndDestroyMove();
		break;

	case UNITAI_HUNTER_ESCORT:
		hunterEscortMove();
		break;

	case UNITAI_MISSIONARY:
		missionaryMove();
		break;

	case UNITAI_PROPHET:
		prophetMove();
		break;

	case UNITAI_ARTIST:
		artistMove();
		break;

	case UNITAI_SCIENTIST:
		scientistMove();
		break;

	case UNITAI_GENERAL:
		generalMove();
		break;

	case UNITAI_GREAT_HUNTER:
		greatHunterMove();
		break;

	case UNITAI_GREAT_ADMIRAL:
		greatAdmiralMove();
		break;

	case UNITAI_MERCHANT:
		merchantMove();
		break;

	case UNITAI_SUBDUED_ANIMAL:
		subduedAnimalMove();
		break;

	case UNITAI_ENGINEER:
		engineerMove();
		break;

	case UNITAI_SPY:
		spyMove();
		break;

	case UNITAI_ICBM:
		ICBMMove();
		break;

	case UNITAI_WORKER_SEA:
		workerSeaMove();
		break;

	case UNITAI_ATTACK_SEA:
		if (m_unit->isNPC())
		{
			barbAttackSeaMove();
		}
		else
		{
			attackSeaMove();
		}
		break;

	case UNITAI_RESERVE_SEA:
		reserveSeaMove();
		break;

	case UNITAI_ESCORT_SEA:
		escortSeaMove();
		break;

	case UNITAI_EXPLORE_SEA:
		exploreSeaMove();
		break;

	case UNITAI_ASSAULT_SEA:
		assaultSeaMove();
		break;

	case UNITAI_SETTLER_SEA:
		settlerSeaMove();
		break;

	case UNITAI_MISSIONARY_SEA:
		missionarySeaMove();
		break;

	case UNITAI_SPY_SEA:
		spySeaMove();
		break;

	case UNITAI_CARRIER_SEA:
		carrierSeaMove();
		break;

	case UNITAI_MISSILE_CARRIER_SEA:
		missileCarrierSeaMove();
		break;

	case UNITAI_PIRATE_SEA:
		pirateSeaMove();
		break;

	case UNITAI_ATTACK_AIR:
		attackAirMove();
		break;

	case UNITAI_DEFENSE_AIR:
		defenseAirMove();
		break;

	case UNITAI_CARRIER_AIR:
		carrierAirMove();
		break;

	case UNITAI_MISSILE_AIR:
		missileAirMove();
		break;

	case UNITAI_ATTACK_CITY_LEMMING:
		attackCityLemmingMove();
		break;

	case UNITAI_HEALER:
		healerMove();
		break;

	case UNITAI_HEALER_SEA:
		healerSeaMove();
		break;

	case UNITAI_PROPERTY_CONTROL:
		propertyControlMove();
		break;

	case UNITAI_PROPERTY_CONTROL_SEA:
		propertyControlSeaMove();
		break;

	case UNITAI_BARB_CRIMINAL:
		BarbCriminalMove();
		break;

	case UNITAI_INVESTIGATOR:
		InvestigatorMove();
		break;

	case UNITAI_INFILTRATOR:
		InfiltratorMove();
		break;

	case UNITAI_SEE_INVISIBLE:
		SeeInvisibleMove();
		break;

	case UNITAI_SEE_INVISIBLE_SEA:
		SeeInvisibleSeaMove();
		break;

	case UNITAI_ESCORT:
		EscortMove();
		break;

	default:
		FErrorMsg("error");
		break;
	}
}

//	Note death (or capture) of a unit
void CvUnitDefaultAI::killed()
{
	if (gUnitLogLevel > 1)
	{
		//	Logging of death location and some mission info
		CvPlot* pMissionPlot = m_unit->getGroup()->AI_getMissionAIPlot();

		logBBAI("%S's %S (%d) died at (%d,%d), mission was %d", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getName(0).GetCString(), m_unit->getID(), m_unit->getX(), m_unit->getY(), m_unit->getGroup()->getMissionType(0));
		if (pMissionPlot != NULL)
		{
			logBBAI("	Mission m_unit->plot was (%d,%d)", pMissionPlot->getX(), pMissionPlot->getY());
		}
	}

	//	Increment the general danger count for m_unit m_unit->plot's vicinity
	if (m_unit->getGroup()->AI_getMissionAIType() != MISSIONAI_DELIBERATE_KILL)
	{
		GET_PLAYER(m_unit->getOwner()).addPlotDangerSource(m_unit->plot(), GC.getGame().AI_combatValue(m_unit->getUnitType()) + 100);

		//	Killing units may change danger evaluation so clear the m_unit->plot danger cache
#ifdef PLOT_DANGER_CACHING
		CvPlayerAI::ClearPlotDangerCache();
#endif
	}
}

// Returns true if took an action or should wait to move later...
bool CvUnitDefaultAI::follow()
{
	if (followBombard())
	{
		return true;
	}
	// Pushing MISSION_MOVE_TO missions when not all units could move resulted in stack being
	// broken up on the next turn.  Also, if we can't attack now we don't want to queue up an
	// attack for next turn, better to re-evaluate.
	const bool bCanAllMove = m_unit->getGroup()->canAllMove();

	if (bCanAllMove && cityAttack(1, 65, true))
	{
		return true;
	}
	if (m_unit->isEnemy(m_unit->plot()->getTeam()) && m_unit->getGroup()->canPillage(m_unit->plot()))
	{
		m_unit->getGroup()->pushMission(MISSION_PILLAGE);
		return true;
	}
	if (bCanAllMove && anyAttack(1, 70, 2, true, true))
	{
		return true;
	}
	if (m_unit->isFound() && m_unit->area()->getBestFoundValue(m_unit->getOwner()) > 0 && foundRange(FOUND_RANGE, true))
	{
		return true;
	}
	return false;
}


// XXX what if a unit gets stuck due to its UnitAIType???
// XXX is m_unit function costing us a lot? (it's recursive...)
bool CvUnitDefaultAI::upgrade()
{
	PROFILE_FUNC();
	FAssertMsg(getUnitAIType() != NO_UNITAI, "AI_getUnitAIType() is not expected to be equal with NO_UNITAI");

	// TB Note: New game theory being introduced here - a unitAI should NEVER change its AI when upgrading!
	// This was causing an infinite spam of units that the game couldn't get enough of a count of because
	// the AI kept upgrading their best pick for something and then upgrading it to another unit that couldn't be that AI type.
	// NOW, we should ALWAYS maintain the role a unit was designed for.
	// Watch for odd problems m_unit might introduce elsewhere though.
	const CvUnitInfo& unitInfo = GC.getUnitInfo(m_unit->getUnitType());

	std::vector<int> upgradeChain = unitInfo.getUnitUpgradeChain();
	if (gUnitLogLevel >= 2)
	{
		logBBAI("	%S at (%d,%d) have %d upgrades", m_unit->getName(0).GetCString(), m_unit->getX(), m_unit->getY(), (int)upgradeChain.size());
	}

	if (!upgradeChain.empty())
	{
		const CvPlayerAI& kPlayer = GET_PLAYER(m_unit->getOwner());
		const UnitAITypes eUnitAI = getUnitAIType();
		const CvArea* pArea = m_unit->area();
		const int iCurrentValue = kPlayer.AI_unitValue(m_unit->getUnitType(), eUnitAI, pArea);

		UnitTypes eBestUnit = NO_UNIT;
		int iBestValue = -1;

		foreach_(int iUnitX, upgradeChain)
		{
			const UnitTypes eUnitX = (UnitTypes)iUnitX;

			if (!GC.getUnitInfo(eUnitX).getNotUnitAIType(eUnitAI)
			&& m_unit->canUpgrade(eUnitX)
			&& kPlayer.AI_unitValue(eUnitX, eUnitAI, pArea) >= iCurrentValue)
			{
				if (gUnitLogLevel >= 2)
				{
					logBBAI("	%S at (%d,%d) can upgrade to %S", m_unit->getName(0).GetCString(), m_unit->getX(), m_unit->getY(), GC.getUnitInfo(eUnitX).getDescription());
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
				logBBAI("	Trying to upgrade %S at (%d,%d) to %S", m_unit->getName(0).GetCString(), m_unit->getX(), m_unit->getY(), GC.getUnitInfo(eBestUnit).getDescription());
			}
			return m_unit->upgrade(eBestUnit);
		}
	}
	return false;
}


bool CvUnitDefaultAI::promote()
{
	PROFILE_FUNC();

	if (!m_unit->isPromotionReady()) return false;

	int iBestValue = 0;
	PromotionTypes eBestPromotion = NO_PROMOTION;

	for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if (m_unit->canPromote((PromotionTypes)iI, -1))
		{
			int iValue = promotionValue((PromotionTypes)iI);

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
			CvWString szString;
			getUnitAIString(szString, getUnitAIType());
			logBBAI("	%S's %S chooses promotion %S", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getName(0).GetCString(), GC.getPromotionInfo(eBestPromotion).getDescription());
		}

		if (m_unit->promote(eBestPromotion, -1))
		{
			promote();
			return true;
		}
		FErrorMsg(CvString::format("Couldn't apply promotion %S to %S", m_unit->getName(0).GetCString(), GC.getPromotionInfo(eBestPromotion).getDescription()).c_str());
	}
	return false;
}

int CvUnitDefaultAI::groupFirstVal() const
{
	if (m_iGroupLeadOverride != -1)
	{
		return m_iGroupLeadOverride;
	}

	switch (getUnitAIType())
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


int CvUnitDefaultAI::groupSecondVal() const
{
	return ((m_unit->getDomainType() == DOMAIN_AIR) ? m_unit->airBaseCombatStr() : m_unit->baseCombatStr());
}

int CvUnitDefaultAI::getPredictedHitPoints() const
{
	return m_iPredictedHitPoints;
}

void CvUnitDefaultAI::setPredictedHitPoints(int iPredictedHitPoints)
{
	m_iPredictedHitPoints = iPredictedHitPoints;

	if (iPredictedHitPoints == -1)
	{
		m_bHasAttacked = false; // This is a reset
	}
}

bool CvUnitDefaultAI::getHasAttacked() const
{
	return m_bHasAttacked;
}

// Returns attack odds out of 100 (the higher, the better...)
// Withdrawal odds included in returned value
int CvUnitDefaultAI::attackOdds(const CvPlot* pPlot, bool bPotentialEnemy, CvUnit** ppDefender, bool bAssassinate)
{
	PROFILE_FUNC();

	CvUnit* pDefender = pPlot->getBestDefender(NO_PLAYER, m_unit->getOwner(), m_unit, !bPotentialEnemy, bPotentialEnemy, false, bAssassinate);

	if (ppDefender != NULL)
	{
		*ppDefender = pDefender;
	}
	return attackOddsAtPlot(pPlot, pDefender, (ppDefender != NULL));
}

// Returns attack odds out of 100 (the higher, the better...)
// Withdrawal odds included in returned value
int CvUnitDefaultAI::attackOddsAtPlot(const CvPlot* pPlot, CvUnit* pDefender, bool modifyPredictedResults)
{
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
		if (eCachedAttackOddsPlayer != m_unit->getOwner() || resultsCache == NULL)
		{
			if (resultsCache == NULL)
			{
				resultsCache = new stdext::hash_map<int, int>();
			}
			else
			{
				resultsCache->clear();
			}

			eCachedAttackOddsPlayer = m_unit->getOwner();
		}

		cacheKey.add(pPlot->getX());
		cacheKey.add(1024 * pPlot->getY());
		cacheKey.add(m_unit->getID());
		cacheKey.add(pDefender->getID());

		stdext::hash_map<int, int>::const_iterator itr = resultsCache->find(cacheKey.get());
		if (itr != resultsCache->end())
		{
			return itr->second;
		}
	}

	const int iResult = attackOddsAtPlotInternal(pPlot, pDefender, modifyPredictedResults);

	if (!modifyPredictedResults)
	{
		(*resultsCache)[cacheKey.get()] = iResult;
	}

	return iResult;
}

int CvUnitDefaultAI::attackOddsAtPlotInternal(const CvPlot* pPlot, CvUnit* pDefender, bool modifyPredictedResults)
{
	PROFILE_FUNC();

	int iOurStrength = ((m_unit->getDomainType() == DOMAIN_AIR) ? m_unit->airCurrCombatStr(NULL) : m_unit->currCombatStr(NULL, NULL));
	if (iOurStrength == 0)
	{
		return 1;
	}
	int iOurFirepower = ((m_unit->getDomainType() == DOMAIN_AIR) ? iOurStrength : m_unit->currFirepower(NULL, NULL));

	const bool bSamePlot = pDefender->plot() == m_unit->plot();

	int iDamageToUs;
	int iDamageToThem;
	int iTheirStrength;
	int iTheirOdds;
	m_unit->getDefenderCombatValues(*pDefender, pPlot, iOurStrength, iOurFirepower, iTheirOdds, iTheirStrength, iDamageToUs, iDamageToThem, NULL, pDefender);

	int iBaseOdds = 100 - iTheirOdds / 10; // getDefenderCombatValues returns odds based on the combat die (which is 1000 sided)

	//int iTheirFirepower;
#if 0
	iTheirStrength = pDefender->currCombatStr(pPlot, m_unit);
	iTheirFirepower = pDefender->currFirepower(pPlot, m_unit);


	FAssert((iOurStrength + iTheirStrength) > 0);
	FAssert((iOurFirepower + iTheirFirepower) > 0);

	iBaseOdds = (100 * iOurStrength) / (iOurStrength + iTheirStrength);
#endif
	if (iBaseOdds == 0)
	{
		return 1;
	}

#if 0
	const int iStrengthFactor = (iOurFirepower + iTheirFirepower + 1) / 2;

	//TB Combat Mods Begin
	const int iUnmodifiedDefenderArmor = pDefender->armorTotal() - punctureTotal();
	const int iUnmodifiedAttackerArmor = armorTotal() - pDefender->punctureTotal();

	const int iModifiedDefenderArmorZero = iUnmodifiedDefenderArmor < 0 ? 0 : iUnmodifiedDefenderArmor;
	const int iModifiedAttackerArmorZero = iUnmodifiedAttackerArmor < 0 ? 0 : iUnmodifiedAttackerArmor;

	const int iModifiedDefenderArmor = (iModifiedDefenderArmorZero < 95 ? iModifiedDefenderArmorZero : 95);
	const int iModifiedAttackerArmor = (iModifiedAttackerArmorZero < 95 ? iModifiedAttackerArmorZero : 95);

	const int iDefenderArmor = (100 - iModifiedDefenderArmor);
	const int iAttackerArmor = (100 - iModifiedAttackerArmor);

	iDamageToUs = std::max(1, ((((GC.getCOMBAT_DAMAGE() * (iTheirFirepower + iStrengthFactor)) / (iOurFirepower + iStrengthFactor)) * iAttackerArmor) / 100));
	iDamageToThem = std::max(1, ((((GC.getCOMBAT_DAMAGE() * (iOurFirepower + iStrengthFactor)) / (iTheirFirepower + iStrengthFactor)) * iDefenderArmor) / 100));
#endif

	if (m_unit->getDomainType() != DOMAIN_AIR)
	{
		int iOurFirstStrikesTimes2 = (pDefender->immuneToFirstStrikes() ? 0 : 2 * m_unit->firstStrikes() + m_unit->chanceFirstStrikes());
		int iTheirFirstStrikesTimes2 = (m_unit->immuneToFirstStrikes() ? 0 : 2 * pDefender->firstStrikes() + pDefender->chanceFirstStrikes());

		// First assess damage from excess first strike rounds
		if (iOurFirstStrikesTimes2 > iTheirFirstStrikesTimes2)
		{
			pDefender->AI()->setPredictedHitPoints(std::max(0, pDefender->getHP() - ((iOurFirstStrikesTimes2 - iTheirFirstStrikesTimes2) * iDamageToThem * iBaseOdds) / 200));

			m_unit->getDefenderCombatValues(*pDefender, pPlot, iOurStrength, iOurFirepower, iTheirOdds, iTheirStrength, iDamageToUs, iDamageToThem, NULL, pDefender);
			iBaseOdds = 100 - iTheirOdds / 10;

			if (iTheirStrength == 0)
			{
				if (!modifyPredictedResults)
				{
					pDefender->AI()->setPredictedHitPoints(-1);
				}
				return 99;
			}
		}
		else if (iOurFirstStrikesTimes2 < iTheirFirstStrikesTimes2)
		{
			setPredictedHitPoints(std::max(0, m_unit->getHP() - ((iTheirFirstStrikesTimes2 - iOurFirstStrikesTimes2) * iDamageToUs * (100 - iBaseOdds)) / 200));

			iOurStrength = ((m_unit->getDomainType() == DOMAIN_AIR) ? m_unit->airCurrCombatStr(NULL) : m_unit->currCombatStr(NULL, NULL));
			iOurFirepower = ((m_unit->getDomainType() == DOMAIN_AIR) ? iOurStrength : m_unit->currFirepower(NULL, NULL));

			if (iOurStrength == 0)
			{
				if (!modifyPredictedResults)
				{
					setPredictedHitPoints(-1);
				}
				return 1;
			}

			m_unit->getDefenderCombatValues(*pDefender, pPlot, iOurStrength, iOurFirepower, iTheirOdds, iTheirStrength, iDamageToUs, iDamageToThem, NULL, pDefender);
			iBaseOdds = 100 - iTheirOdds / 10;
		}
	}
	//TB Combat Mods End

	const int iHitLimitThem = pDefender->getMaxHP() - m_unit->combatLimit(pDefender);

	int iNeededRoundsUs = (std::max(0, pDefender->getHP() - iHitLimitThem) + iDamageToThem - 1) / iDamageToThem;
	int iNeededRoundsThem = (std::max(0, m_unit->getHP()) + iDamageToUs - 1) / iDamageToUs;

#if 0
	if (m_unit->getDomainType() != DOMAIN_AIR)
	{
		// From Mongoose SDK
		if (!pDefender->immuneToFirstStrikes())
		{
			iNeededRoundsUs -= ((iBaseOdds * m_unit->firstStrikes()) + ((iBaseOdds * m_unit->chanceFirstStrikes()) / 2)) / 100;
		}
		if (!m_unit->immuneToFirstStrikes())
		{
			iNeededRoundsThem -= (((100 - iBaseOdds) * pDefender->firstStrikes()) + (((100 - iBaseOdds) * pDefender->chanceFirstStrikes()) / 2)) / 100;
		}
		iNeededRoundsUs = std::max(1, iNeededRoundsUs);
		iNeededRoundsThem = std::max(1, iNeededRoundsThem);
	}
#endif

	const int iRoundsDiff = iNeededRoundsUs - iNeededRoundsThem;

	if (iRoundsDiff > 0)
	{
		if (modifyPredictedResults)
		{
			setPredictedHitPoints(0);

			//	Adjust the predicted damage to allow for the effect of strength reductions
			//	between the attack rounds
			//	Basically we know the loser is losing 100% of their starting strength over the coursemof the battle,
			//	so see how much the base prediction is for the attacker, then reduce the damage to the attacker by the
			//	amount they have left (as a percentage of where they started) since their attacks as rounds go on
			//	will be diluted less (down to around m_unit amount)
			int iWinnerBaseLossPercent = 0;

			if (pDefender->getHP() != 0)
			{
				iWinnerBaseLossPercent += 100 - (100 * iNeededRoundsThem * iDamageToThem) / pDefender->getHP();
			}
			iDamageToThem -= (iDamageToThem * iWinnerBaseLossPercent) / 100;

			FAssert(pDefender->getHP() > iNeededRoundsThem * iDamageToThem);

			pDefender->AI()->setPredictedHitPoints(pDefender->getHP() - iNeededRoundsThem * iDamageToThem);
		}

		iTheirStrength *= 100 + 100 * iRoundsDiff / std::max(1, iNeededRoundsThem);
		iTheirStrength /= 100;
	}
	else
	{
		if (modifyPredictedResults)
		{
			pDefender->AI()->setPredictedHitPoints(0);

			if (iRoundsDiff != 0)
			{
				//	Adjust the predicted damage to allow for the effect of strength reductions
				//	between the attack rounds
				//	Basically we know the loser is losing 100% of their starting strength over the coursemof the battle,
				//	so see how much the base prediction is for the attacker, then reduce the damage to the attacker by the
				//	amount they have left (as a percentage of where they started) since their attacks as rounds go on
				//	will be diluted less (down to around m_unit amount)
				const int iWinnerBaseLossPercent = 100 - 100 * iNeededRoundsUs * iDamageToUs / m_unit->getHP();

				iDamageToUs -= iDamageToUs * iWinnerBaseLossPercent / 100;
			}
			// If iRoundsDiff == 0 both units can wind up at a notional 0 HP so cannot assert left over HP in that case
			setPredictedHitPoints(std::max(0, m_unit->getHP() - iNeededRoundsUs * iDamageToUs));
		}

		iOurStrength *= 100 - 100 * iRoundsDiff / std::max(1, iNeededRoundsUs);
		iOurStrength /= 100;
	}

	int iOdds = iOurStrength * 100 / (iOurStrength + iTheirStrength);

	//	Koshling - modify the calculated odds to account for withdrawal chances
	//	and the AI player's rose-tinted-spectacles value - m_unit used to simply add
	//	to the odds, but that made it look like fights with hugely different strengths
	//	still had 5%ish chances of voctory which was a good expected gain so caused the
	//	AI to suicide weak units onto strong enemy ones, thinking that one in 20 or so would
	//	win againts a massively stronger opponent.  Changed it to be multiplicative instead
	//  TB Combat Mods (I'm hoping m_unit calculates out right...):
	//  Determine Withdraw odds
	const int AdjustedWithdrawalstep1 = m_unit->withdrawVSOpponentProbTotal(pDefender, pPlot) - pDefender->pursuitVSOpponentProbTotal(m_unit);
	const int AdjustedWithdrawalstep2 = AdjustedWithdrawalstep1 > 100 ? 100 : AdjustedWithdrawalstep1;

	const int AdjustedWithdrawal = AdjustedWithdrawalstep2 < 0 ? 0 : AdjustedWithdrawalstep2;

	const int expectedRndCnt = std::min(iNeededRoundsUs, iNeededRoundsThem);
	const int timePercentage = m_unit->earlyWithdrawTotal() > 100 ? 100 : m_unit->earlyWithdrawTotal(); // InitialEarlyWithdrawPercentage

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
	const int iFortRepelWithOverrun = pDefender->fortifyRepelModifier() - m_unit->overrunTotal();

	const int iFortRepelZero = iFortRepelWithOverrun < 0 ? 0 : iFortRepelWithOverrun;
	const int iFortRepelTotal = iFortRepelZero > 100 ? 100 : iFortRepelZero;

	const int iRepelWithUnyielding = pDefender->repelTotal() + iFortRepelTotal - m_unit->unyieldingTotal();

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
	const int iKnockbackVsUnyielding = m_unit->knockbackVSOpponentProbTotal(pDefender) - pDefender->unyieldingTotal();

	const int iKnockbackZero = iKnockbackVsUnyielding < 0 ? 0 : iKnockbackVsUnyielding;
	const int iKnockbackTotal = iKnockbackZero > 100 ? 100 : iKnockbackZero;

	const int iAttackerKnockbackTries = m_unit->knockbackRetriesTotal();
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
	iOdds += (iOdds * 2 * GET_PLAYER(m_unit->getOwner()).AI_getAttackOddsChange() + (100 - iOdds) * iOddsModifier / 2) / 100;

	if (!modifyPredictedResults)
	{
		setPredictedHitPoints(-1);
		pDefender->AI()->setPredictedHitPoints(-1);
	}
	return range(iOdds, 1, 99);
}


//loops through best builds already generated by a city, and then finds the best one to do just now.
bool CvUnitDefaultAI::bestCityBuild(const CvCity* pCity, CvPlot** ppBestPlot, BuildTypes* peBestBuild, const CvPlot* pIgnorePlot, const CvUnit* pUnit)
{
	PROFILE_FUNC();

	const CvPlayerAI& kOwner = GET_PLAYER(m_unit->getOwner());
	const ImprovementTypes eRuins = GC.getIMPROVEMENT_CITY_RUINS();
	const bool bSafeAutomation = kOwner.isOption(PLAYEROPTION_SAFE_AUTOMATION);

	BuildTypes eBestBuild = NO_BUILD;
	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	for (int iPass = 0; iPass < 2; iPass++)
	{
		for (int iI = 0; iI < pCity->getNumCityPlots(); iI++)
		{
			CvPlot* pLoopPlot = plotCity(pCity->getX(), pCity->getY(), iI);

			if (pLoopPlot != NULL && pLoopPlot->getWorkingCity() == pCity && plotValid(pLoopPlot) && pLoopPlot != pIgnorePlot
				&& (!bSafeAutomation || pLoopPlot->getImprovementType() == NO_IMPROVEMENT || pLoopPlot->getImprovementType() == eRuins))
			{
				int iValue = pCity->AI_getBestBuildValue(iI);

				if (iValue > iBestValue)
				{
					const BuildTypes eBuild = pCity->AI_getBestBuild(iI);

					if (eBuild == NO_BUILD || !m_unit->canBuild(pLoopPlot, eBuild))
					{
						continue;
					}
					if (iPass == 0)
					{
						iBestValue = iValue;
						pBestPlot = pLoopPlot;
						eBestBuild = eBuild;
						continue;
					}
					// Second pass
					int iPathTurns;
					if ((!pBestPlot->isVisible(m_unit->getTeam(), false) || !pBestPlot->isVisibleEnemyUnit(m_unit)) && m_unit->generatePath(pBestPlot, 0, true, &iPathTurns))
					{
						// XXX take advantage of range (warning... m_unit could lead to some units doing nothing...)
						int iMaxWorkers = 1;
						if (m_unit->getPathMovementRemaining() == 0)
						{
							iPathTurns++;
						}
						else if (iPathTurns <= 1)
						{
							iMaxWorkers = calculatePlotWorkersNeeded(pLoopPlot, eBuild);
						}
						if (pUnit != NULL && pUnit->plot()->isCity() && iPathTurns == 1 && m_unit->getPathMovementRemaining() > 0)
						{
							iMaxWorkers += 10;
						}
						if (kOwner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD, m_unit->getGroup()) < iMaxWorkers)
						{
							// XXX - This could be improved greatly by looking at the real build time
							//	and other factors when deciding whether to stack.
							iValue /= 1 + iPathTurns;
							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = pLoopPlot;
								eBestBuild = eBuild;
							}
						}
					}
				}
			}
		}
		if (0 == iPass && eBestBuild != NO_BUILD)
		{
			int iPathTurns;
			if ((!pBestPlot->isVisible(m_unit->getTeam(), false) || !pBestPlot->isVisibleEnemyUnit(m_unit)) && m_unit->generatePath(pBestPlot, 0, true, &iPathTurns))
			{
				int iMaxWorkers = 1;
				if (m_unit->getPathMovementRemaining() == 0)
				{
					iPathTurns++;
				}
				else if (iPathTurns <= 1)
				{
					iMaxWorkers = calculatePlotWorkersNeeded(pBestPlot, eBestBuild);
				}
				if (pUnit != NULL && pUnit->plot()->isCity())
				{
					iMaxWorkers += 10;
				}

				if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pBestPlot, MISSIONAI_BUILD, m_unit->getGroup()) < iMaxWorkers)
				{
					break; // Good to go.
				}
			}
			eBestBuild = NO_BUILD;
			iBestValue = 0;
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

bool CvUnitDefaultAI::isCityAIType() const
{
	return (
			getUnitAIType() == UNITAI_CITY_DEFENSE
		|| getUnitAIType() == UNITAI_CITY_COUNTER
		|| getUnitAIType() == UNITAI_CITY_SPECIAL
		|| getUnitAIType() == UNITAI_RESERVE
		|| getUnitAIType() == UNITAI_PILLAGE_COUNTER
		|| getUnitAIType() == UNITAI_PROPERTY_CONTROL
	);
}


int CvUnitDefaultAI::getBirthmark() const
{
	return m_iBirthmark;
}

void CvUnitDefaultAI::setBirthmark(int iNewValue)
{
	m_iBirthmark = iNewValue;
}


UnitAITypes CvUnitDefaultAI::getUnitAIType() const
{
	// A unit should never have no unitAI, so if that state is found to exist set it to its default AI.
	//	- Empirically it's been seen but the underlying cause has not yet been found. -
	if (m_eUnitAIType == NO_UNITAI)
	{
		FErrorMsg("Unit has no UnitAI!");
		
		((CvUnitDefaultAI*)this)->m_eUnitAIType = m_unit->getUnitInfo().getDefaultUnitAIType();

		m_unit->area()->changeNumAIUnits(m_unit->getOwner(), m_eUnitAIType, 1);
		GET_PLAYER(m_unit->getOwner()).AI_changeNumAIUnits(m_eUnitAIType, 1);
	}
	return m_eUnitAIType;
}


// XXX make sure m_unit gets called...
void CvUnitDefaultAI::setUnitAIType(UnitAITypes eNewValue)
{
	FAssertMsg(eNewValue != NO_UNITAI, "NewValue is not assigned a valid value");

	if (getUnitAIType() != eNewValue)
	{
		m_unit->area()->changeNumAIUnits(m_unit->getOwner(), getUnitAIType(), -1);
		GET_PLAYER(m_unit->getOwner()).AI_changeNumAIUnits(getUnitAIType(), -1);

		m_eUnitAIType = eNewValue;

		m_unit->area()->changeNumAIUnits(m_unit->getOwner(), getUnitAIType(), 1);
		GET_PLAYER(m_unit->getOwner()).AI_changeNumAIUnits(getUnitAIType(), 1);

		m_unit->joinGroup(NULL);
	}
}

int CvUnitDefaultAI::sacrificeValue(const CvPlot* pPlot) const
{
	if (m_unit->getDomainType() == DOMAIN_AIR)
	{
		int iValue = 128 * (100 + m_unit->currInterceptionProbability());
		if (m_unit->getUnitInfo().getNukeRange() != -1)
		{
			iValue += 25000;
		}
		iValue /= std::max(1, (1 + m_unit->getUnitInfo().getProductionCost()));
		iValue *= m_unit->getMaxHP() - m_unit->getDamage();
		return iValue / 100;
	}
	int iValue = 128 * m_unit->currEffectiveStr(pPlot, ((pPlot == NULL) ? NULL : m_unit));

	if (iValue > 0)
	{
		int iCollateralDamageValue = 0;
		if (pPlot != NULL)
		{
			const int iPossibleTargets = std::min(pPlot->getNumVisiblePotentialEnemyDefenders(m_unit) - 1, m_unit->collateralDamageMaxUnits());

			if (iPossibleTargets > 0)
			{
				iCollateralDamageValue = m_unit->collateralDamage() + std::max(0, iCollateralDamageValue - 100);
				iCollateralDamageValue *= iPossibleTargets;
				iCollateralDamageValue /= 5;
			}
		}
		//  TB Combat Mods:
		//  Determine Withdraw odds
		const int AdjustedWithdrawalstep1 = m_unit->withdrawalProbability();
		const int AdjustedWithdrawalstep2 = AdjustedWithdrawalstep1 > 100 ? 100 : AdjustedWithdrawalstep1;
		const int AdjustedWithdrawal = AdjustedWithdrawalstep2 < 0 ? 0 : AdjustedWithdrawalstep2;
		const int timePercentage = ((100 - m_unit->earlyWithdrawTotal()) > 100 ? 100 : (100 - m_unit->earlyWithdrawTotal())); // InitialEarlyWithdrawPercentage

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
			y = AdjustedWithdrawal * y / 100;
		}
		const int EvaluatedWithdrawOdds = z;

		//* figure out odds of knockback
		const int iAttackerKnockback = m_unit->knockbackTotal();
		const int iAttackerKnockbackTries = m_unit->knockbackRetriesTotal();
		const int iKnockbackZero = iAttackerKnockback < 0 ? 0 : iAttackerKnockback;
		const int iKnockbackTotal = iKnockbackZero > 100 ? 100 : iKnockbackZero;

		y = iKnockbackTotal;
		z = iKnockbackTotal;
		for (int Time = 0; Time < iAttackerKnockbackTries; ++Time)
		{
			z += iKnockbackTotal * y / 100;
			y = iKnockbackTotal * y / 100;
		}
		const int EvaluatedKnockbackOdds = z;

		iValue *= 100 + iCollateralDamageValue;
		iValue /= 100 + m_unit->cityDefenseModifier();
		iValue *= 100 + EvaluatedWithdrawOdds;
		iValue /= 100;
		iValue *= 100 + EvaluatedKnockbackOdds;
		iValue /= 100;
		//TB Combat Mods End (above EvaluationalWithdrawOdds replaces EvaluatedWithdrawOdds)

		iValue /= (10 + m_unit->getExperience());

		// Value units which can't kill units later, also combat limits mean higher survival odds
		if (m_unit->combatLimit() < 100)
		{
			iValue *= 150;
			iValue /= 100;

			iValue *= 100;
			iValue /= std::max(1, m_unit->combatLimit());
		}
		iValue /= std::max(1, 1 + m_unit->getUnitInfo().getProductionCost());
	}
	return iValue;
}

// Protected Functions...

void CvUnitDefaultAI::animalMove()
{
	PROFILE_FUNC();

	const bool bReckless = GC.getGame().isOption(GAMEOPTION_RECKLESS_ANIMALS);

	if (m_unit->canAttack())
	{
		if (bReckless)
		{
			if (anyAttack(2, 0, 0, false))
			{
				return;
			}
		}
		else
		{
			// Recklessness based on animal aggression.
			// 1% odds assessment is there to account for some small understanding of likelihood of success even in an aggressive action.
			if (GC.getGame().getSorenRandNum(10, "Animal Attack")
				<
				getMyAggression(GC.getHandicapInfo(GC.getGame().getHandicapType()).getAnimalAttackProb()))
			{
				if (anyAttack(2, 1, 0, false))
				{
					return;
				}
			}
			else // not reckless.
			{
				// Animals are pretty good at assessing their chances of taking down prey, therefore 60% odds prereq.
				if (GC.getGame().getSorenRandNum(100, "Animal Attack")
					<
					GC.getHandicapInfo(GC.getGame().getHandicapType()).getAnimalAttackProb())
				{
					if (anyAttack(2, 60, 0, false))
					{
						return;
					}
				}
			}
		}
	}
	if (!bReckless && exposedToDanger(m_unit->plot(), 60, true) && safety())
	{
		return;
	}
	if (heal() || patrol(bReckless))
	{
		return;
	}
	m_unit->getGroup()->pushMission(MISSION_SKIP);
}

bool CvUnitDefaultAI::SettleFirstCity()
{
	// Afforess & Fuyu - Check for Good City Sites Near Starting Location
	const int iGameSpeedPercent = GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
	const int iMaxFoundTurn = iGameSpeedPercent / 100;

	if (m_unit->canMove() && !GET_PLAYER(m_unit->getOwner()).AI_isPlotCitySite(m_unit->plot()) && GC.getGame().getElapsedGameTurns() <= iMaxFoundTurn)
	{
		int iBestValue = 0;
		int iBestFoundTurn = 0;
		CvPlot* pBestPlot = NULL;

		for (int iCitySite = 0; iCitySite < GET_PLAYER(m_unit->getOwner()).AI_getNumCitySites(); iCitySite++)
		{
			CvPlot* pCitySite = GET_PLAYER(m_unit->getOwner()).AI_getCitySite(iCitySite);
			if (pCitySite->getArea() == m_unit->getArea() || m_unit->canMoveAllTerrain())
			{
				int iPlotValue = pCitySite->getFoundValue(m_unit->getOwner());
				if (iPlotValue > iBestValue)
				{
					//Can m_unit unit reach the m_unit->plot m_unit turn? (getPathLastNode()->m_iData2 == 1)
					//Will m_unit unit still have movement points left to found the city the same turn? (getPathLastNode()->m_iData1 > 0))
					int iPathTurns;
					if (m_unit->generatePath(pCitySite, 0, false, &iPathTurns))
					{
						const int iFoundTurn = GC.getGame().getElapsedGameTurns() + iPathTurns - (m_unit->getPathMovementRemaining() > 0 ? 1 : 0);
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
			if (m_unit->plot()->isRiver() && !pBestPlot->isRiver())
			{
				const int iOceanMinSize = GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize();

				if (m_unit->plot()->isCoastalLand(iOceanMinSize) && !pBestPlot->isCoastalLand(iOceanMinSize)
					|| pBestPlot->getBonusType(NO_TEAM) != NO_BONUS
					&& pBestPlot->calculateNatureYield(YIELD_FOOD, m_unit->getTeam(), true) > 0)
				{
					pBestPlot = NULL;
				}
			}
		}

		if (pBestPlot != NULL)
		{
			if (gUnitLogLevel >= 2)
			{
				logBBAI("	Settler not founding in place but moving %d, %d to nearby city site at %d, %d (%d turns away) with value %d)", (pBestPlot->getX() - m_unit->getX()), (pBestPlot->getY() - m_unit->getY()), pBestPlot->getX(), pBestPlot->getY(), iBestFoundTurn, iBestValue);
			}
			m_unit->getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_SAFE_TERRITORY, false, false, MISSIONAI_FOUND, pBestPlot);
			return true;
		}
	}
	// ! Afforess & Fuyu - Check for Good City Sites Near Starting Location

	if (m_unit->canFound(m_unit->plot()))
	{
		if (gUnitLogLevel >= 2)
		{
			logBBAI("	Settler (%d) founding in place due to no cities", m_unit->getID());
		}
		m_unit->getGroup()->pushMission(MISSION_FOUND);
		return true;
	}
	return false;
}


void CvUnitDefaultAI::settleMove()
{
	PROFILE_FUNC();
	CvPlot* pEndTurnPlot = NULL;

	if (selectStatus(true))
	{
		return;
	}

	if (GET_PLAYER(m_unit->getOwner()).getNumCities() == 0 && SettleFirstCity())
	{
		return;
	}

	const int iDanger = GET_PLAYER(m_unit->getOwner()).AI_getPlotDanger(m_unit->plot(), 3);
	SendLog("SettlerAI", CvWString::format(L"iDanger check %I64d", iDanger));
	if (iDanger > 0)
	{
		if (!m_unit->getGroup()->canDefend() && (m_unit->plot()->getOwner() == m_unit->getOwner() || iDanger > 2)
		|| GET_PLAYER(m_unit->getOwner()).AI_getVisiblePlotDanger(m_unit->plot(), 2, false, m_unit->getGroup(), 70))
		{
			if (!m_unit->getGroup()->canDefend() && retreatToCity() || safety())
			{
				return;
			}
			SendLog("SettlerAI", CvWString::format(L"already in city %I64d", iDanger));
			m_unit->getGroup()->pushMission(MISSION_SKIP);
		}
	}

	// Don't found new cities if that would cause more unhappiness when we already have happiness issues
	bool bInhibitFounding = false;


	if (GET_PLAYER(m_unit->getOwner()).getCityLimit() > 0)
	{
		if (GET_PLAYER(m_unit->getOwner()).getCityOverLimitUnhappy() > 0)
		{
			// Soft limit - If we already have unhappy cities don't create settlers
			//	that will increase overall unhappiness as they found new cities
			bInhibitFounding =
				(
					GET_PLAYER(m_unit->getOwner()).getCityLimit() <= GET_PLAYER(m_unit->getOwner()).getNumCities()
					&&
					GET_PLAYER(m_unit->getOwner()).AI_getOverallHappyness(GET_PLAYER(m_unit->getOwner()).getCityOverLimitUnhappy()) < 0
				);
		}
		else // Hard limit
		{
			bInhibitFounding = (GET_PLAYER(m_unit->getOwner()).getCityLimit() <= GET_PLAYER(m_unit->getOwner()).getNumCities());
		}
	}

	if (!bInhibitFounding)
	{
		bInhibitFounding = GET_PLAYER(m_unit->getOwner()).AI_isFinancialTrouble();
		if (bInhibitFounding) SendLog("SettlerAI", CvWString::format(L"	Settler (%d) inhibited from founding due to financial difficulties", m_unit->getID()));

		if (bInhibitFounding && gUnitLogLevel >= 2)
		{
			SendLog("SettlerAI", CvWString::format(L"	Settler (%d) inhibited from founding due to financial difficulties", m_unit->getID()));
			logBBAI("	Settler (%d) inhibited from founding due to financial difficulties", m_unit->getID());
		}
	}
	else if (gUnitLogLevel >= 2)
	{
		logBBAI("	Settler (%d) inhibited from founding due to city limit unhappyness", m_unit->getID());
	}

	if (!bInhibitFounding)
	{
		int iAreaBestFoundValue = 0;
		int iOtherBestFoundValue = 0;
		SendLog("SettlerAI", CvWString::format(L"Number of City Sites %WS %I64d ID: %d X: %d Y: %d", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), GET_PLAYER(m_unit->getOwner()).AI_getNumCitySites(), m_unit->getID(), m_unit->getX(), m_unit->getY()));
		for (int iI = 0; iI < GET_PLAYER(m_unit->getOwner()).AI_getNumCitySites(); iI++)
		{
			CvPlot* pCitySitePlot = GET_PLAYER(m_unit->getOwner()).AI_getCitySite(iI);

			// Only count city sites we can get to
			if ((pCitySitePlot->getArea() == m_unit->getArea() || m_unit->canMoveAllTerrain()) && m_unit->generatePath(pCitySitePlot, MOVE_SAFE_TERRITORY, true))
			{
				if (m_unit->plot() == pCitySitePlot && m_unit->canFound(m_unit->plot()))
				{
					SendLog("SettlerAI", CvWString::format(L"Settler %S (%d) founding in place since it's at a city site %d, %d", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getID(), m_unit->getX(), m_unit->getY()));
					if (gUnitLogLevel >= 2)
					{
						logBBAI("	Settler (%d) founding in place since it's at a city site %d, %d", m_unit->getID(), m_unit->getX(), m_unit->getY());
					}
					m_unit->getGroup()->pushMission(MISSION_FOUND);
					return;
				}
				// K-Mod. If we are already heading to m_unit site, then keep going!
				// This change fixes a bug which prevented settlers from targeting the same site two turns in a row!
				else
				{
					CvPlot* pMissionPlot = m_unit->getGroup()->AI_getMissionAIPlot();
					if (pMissionPlot == pCitySitePlot && m_unit->getGroup()->AI_getMissionAIType() == MISSIONAI_FOUND)
					{
						// safety check. (cf. conditions in found)
						if (m_unit->getGroup()->canDefend() && m_unit->getGroup()->getStrength() > (minSettlerDefense() / 2))
						{
							if (gUnitLogLevel >= 2)
							{
								logBBAI("	Settler (%d) continuing mission to %d, %d, current group strength (%d)", m_unit->getID(), pCitySitePlot->getX(), pCitySitePlot->getY(), m_unit->getGroup()->getStrength());
							}
							pEndTurnPlot = m_unit->getPathEndTurnPlot();
							if (pEndTurnPlot != NULL)
							{
								SendLog("SettlerAI", CvWString::format(L"Settler (%d) from %S continuing mission to %d, %d, current group strength (%d)", m_unit->getID(), GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), pCitySitePlot->getX(), pCitySitePlot->getY(), m_unit->getGroup()->getStrength()));
								m_unit->getGroup()->pushMission(MISSION_MOVE_TO, pEndTurnPlot->getX(), pEndTurnPlot->getY(), MOVE_SAFE_TERRITORY, false, false, MISSIONAI_FOUND, pCitySitePlot);
								return;
							}
							m_unit->getGroup()->pushMission(MISSION_SKIP);
							return;
						}
					}
				}
				// K-Mod end
				iAreaBestFoundValue = std::max(iAreaBestFoundValue, pCitySitePlot->getFoundValue(m_unit->getOwner()));
			}
			else if (m_unit->canDefend()) // Might have failed due to danger when we are unstacked which doesn't imply another m_unit->area/need for transports
			{
				iOtherBestFoundValue = std::max(iOtherBestFoundValue, pCitySitePlot->getFoundValue(m_unit->getOwner()));
			}
		}

		// No new settling of colonies when AI is in financial trouble
		if (m_unit->plot()->isCity() && m_unit->plot()->getOwner() == m_unit->getOwner() && GET_PLAYER(m_unit->getOwner()).AI_isFinancialTrouble())
		{
			iOtherBestFoundValue /= 4;
		}

		if (iAreaBestFoundValue == 0 && iOtherBestFoundValue == 0
		&& GC.getGame().getGameTurn() - m_unit->getGameTurnCreated() > intSqrt(GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent()))
		{
			if (NULL != m_unit->getTransportUnit())
			{
				m_unit->getTransportUnit()->unloadAll();
			}

			if (NULL == m_unit->getTransportUnit()
			&& GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit->getGroup()->getHeadUnit(), MISSIONAI_PICKUP) == 0)
			{
				m_unit->scrap(); //may seem wasteful, but settlers confuse the AI.
				return;
			}
		}

		if (iOtherBestFoundValue * 100 > iAreaBestFoundValue * 110 && m_unit->plot()->getOwner() == m_unit->getOwner()
		&& load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, NO_UNITAI, -1, -1, -1, 0, MOVE_SAFE_TERRITORY))
		{
			return;
		}

		if (iAreaBestFoundValue > 0 && m_unit->plot()->isBestAdjacentFound(m_unit->getOwner()) && m_unit->canFound(m_unit->plot()))
		{
			if (gUnitLogLevel >= 2)
			{
				logBBAI("	Settler (%d) founding in place due to best adjacent found", m_unit->getID());
			}
			m_unit->getGroup()->pushMission(MISSION_FOUND);
			return;
		}

		if (!m_unit->getGroup()->canDefend() || m_unit->getGroup()->getStrength() <= minSettlerDefense())
		{
			//	If we're already in a city advertise for an escort
			if (m_unit->plot()->isCity() && (m_unit->plot()->getOwner() == m_unit->getOwner()) && !m_unit->isCargo())
			{
				std::ostringstream stringStream;
				stringStream << "Settler (" << m_unit->getID() << ") for player" << m_unit->getOwner() << "(" << GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(1) << ") at" << "(" << m_unit->getX() << "," << m_unit->getY() << ")" << "requesting defensive escort, current group strength (" << m_unit->getGroup()->getStrength() << ")" << minSettlerDefense() << std::endl;
				SendLog("SettlerAI", stringStream.str());
				if (m_contractsLastEstablishedTurn != GC.getGame().getGameTurn())
				{
					if (gUnitLogLevel >= 3)
					{
						logBBAI("	Settler (%d) for player %d (%S) at (%d,%d) requesting defensive escort, current group strength (%d)\n",
							m_unit->getID(), m_unit->getOwner(),
							GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0),
							m_unit->getX(), m_unit->getY(),
							m_unit->getGroup()->getStrength()
						);
					}
					//	For now advertise the work as being here since we'll be holding position
					//	while we wait for a defender.  It would be more optimal to 'meet' the defender
					//	on the way or at the target m_unit->plot, but m_unit might leave us exposed on the way so
					//	the calculations involved are somewhat harder and not attempted for now
					GET_PLAYER(m_unit->getOwner()).getContractBroker().advertiseWork(

						HIGHEST_PRIORITY_ESCORT_PRIORITY,
						OFFENSIVE_UNITCAPABILITIES,
						m_unit->getX(), m_unit->getY(), m_unit,
						UNITAI_ESCORT,
						minSettlerDefense()
					);
					m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
					m_contractualState = CONTRACTUAL_STATE_AWAITING_ANSWER;
					return;
				}
			}
		}

		if (m_unit->plot()->isCity() && m_unit->plot()->getOwner() == m_unit->getOwner() && m_unit->getGroup()->getNumUnits() < 3
		&& GC.getGame().getMaxCityElimination() > 0 && GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(m_unit->plot()))
		{
			std::ostringstream stringStream;
			stringStream << "Settler (" << m_unit->getID() << ") for player" << m_unit->getOwner() << "(" << GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0) << ") at" << "(" << m_unit->getX() << "," << m_unit->getY() << ")" << "skipping due to not enough units (" << m_unit->getGroup()->getStrength() << ")" << GC.getGame().getMaxCityElimination() << GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(m_unit->plot()) << std::endl;
			SendLog("SettlerAI", stringStream.str());
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}

		if (iAreaBestFoundValue > 0 && found())
		{
			return;
		}

		if (m_unit->plot()->getOwner() == m_unit->getOwner())
		{
			if (load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, NO_UNITAI, -1, -1, -1, 0, MOVE_NO_ENEMY_TERRITORY))
			{
				SendLog("SettlerAI", "tries to board ship");
				return;
			}
			// BBAI TODO: Go to a good city (like one with a transport) ...
		}
	}

	if (!m_unit->getGroup()->canDefend() && retreatToCity())
	{
		return;
	}

	// Settler AI
	if (m_unit->getGroup()->isStranded()
	&& load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
	{
		return;
	}

	if (safety())
	{
		return;
	}
	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


int CvUnitDefaultAI::minSettlerDefense() const
{
	return (
		GET_PLAYER(m_unit->getOwner()).strengthOfBestUnitAI(DOMAIN_LAND, UNITAI_CITY_DEFENSE)
		*
		(
			// One defender if game-start settler(s), else 2.
			1 /* + 1 * (m_unit->getGameTurnCreated() > 0)*/
			+ // Two more if at war (ignore minor civ).
			1 * GET_TEAM(m_unit->getTeam()).isAtWar()
			)
	);
}

bool CvUnitDefaultAI::IsAbroad() {
	return m_unit->plot()->getOwner() != m_unit->getOwner();
}

bool CvUnitDefaultAI::upgradeWorker() {

	const int64_t iGold = GET_PLAYER(m_unit->getOwner()).getGold();
	const int iTargetGold = GET_PLAYER(m_unit->getOwner()).AI_goldTarget();

	if (iGold > iTargetGold) {
		//	Whether we let it try to be comsumed by an upgrade depends on how much spare cash we have
		if (GC.getGame().getSorenRandNum(100, "AI upgrade worker") < 100 * (iGold - iTargetGold) / iGold) {
			return upgrade();
		}
	}
	return false;
}

int CvUnitDefaultAI::GetNumberOfUnitsInGroup() {
	return m_unit->getGroup()->getNumUnits();
}


void CvUnitDefaultAI::workerMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}
	const bool bAbroad = IsAbroad();
	// If worker (or captive) is not defended, and is outside own borders.
	if (bAbroad)
	{
		if (m_unit->getGroup()->canDefend())
		{
			if (!m_unit->isHuman() && workerReleaseDefenderIfNotNeeded())
			{
				OutputDebugString(CvString::format("%S (%d) workerReleaseDefenderIfNotNeeded 1 (%d,%d)...\n", m_unit->getDescription().c_str(), m_unit->getID(), m_unit->getX(), m_unit->getY()).c_str());

				if (reachHome())
				{
					OutputDebugString(CvString::format("%S (%d) reachHome 1 true (%d,%d)...\n", m_unit->getDescription().c_str(), m_unit->getID(), m_unit->getX(), m_unit->getY()).c_str());
					return;
				}
			}
		}
		else if (workerNeedsDefender(m_unit->plot()))
		{
			// If I can reach safety on my own m_unit turn then don't bother other units with our escape.
			if (reachHome())
			{
				OutputDebugString(CvString::format("%S (%d) reachHome 2 true (%d,%d)...\n", m_unit->getDescription().c_str(), m_unit->getID(), m_unit->getX(), m_unit->getY()).c_str());
				return;
			}
			OutputDebugString(CvString::format("%S (%d) reachHome 2 false (%d,%d)...\n", m_unit->getDescription().c_str(), m_unit->getID(), m_unit->getX(), m_unit->getY()).c_str());
			// Look for a local group we can join to be safe!
			// We do want to take control (otherwise other unit decides where m_unit worker goes, and can go further away)
			setLeaderPriority(LEADER_PRIORITY_MAX);

			if (group(GroupingParams().withUnitAI(UNITAI_ATTACK).ignoreFaster().ignoreOwnUnitType().maxPathTurns(1)))
			{
				return;
			}
			if (group(GroupingParams().withUnitAI(UNITAI_HUNTER).ignoreOwnUnitType().maxPathTurns(1)))
			{
				return;
			}
			if (group(GroupingParams().withUnitAI(UNITAI_ATTACK_CITY).ignoreFaster().ignoreOwnUnitType().maxPathTurns(1)))
			{
				return;
			}
			if (group(GroupingParams().withUnitAI(UNITAI_COUNTER).ignoreFaster().ignoreOwnUnitType().maxPathTurns(1)))
			{
				return;
			}
			setLeaderPriority(-1); // We didn't get to group so back to normal

			// Nobody can join us and we cannot join anyone else - leg it!
			if (retreatToCity())
			{
				return;
			}
		}
		// Toffer - After reaching m_unit point the worker will always start making route from here to friendly territory... May be better things to do elsewhere.
		//	Should check if improving m_unit neutal m_unit->plot is worthwhile and if not just start going to a more worthwhile m_unit->plot/job.
	}
	else if (!m_unit->isHuman() && workerReleaseDefenderIfNotNeeded())
	{
		OutputDebugString(CvString::format("%S (%d) workerReleaseDefenderIfNotNeeded 2 (%d,%d)...\n", m_unit->getDescription().c_str(), m_unit->getID(), m_unit->getX(), m_unit->getY()).c_str());
		return;
	}
	//ls612: Combat Worker Danger Evaluation
	const bool bWorkerDanger = bAbroad && GET_PLAYER(m_unit->getOwner()).AI_isPlotThreatened(m_unit->plot(), 2) || !bAbroad && exposedToDanger(m_unit->plot(), 80, false);

	if (bWorkerDanger && (!m_unit->canDefend() || GetNumberOfUnitsInGroup() == 1)) {
		// in m_unit order, either retreat to safety, or go into a city
		if (safety() || retreatToCity()) {
			return;
		}
	}

	// TODO: move out of workermove and into new unitai called UNITAI_MISSIONUNIT ?
	if (construct(MAX_INT, MAX_INT, 0, true))
	{
		OutputDebugString(CvString::format("%S (%d) chooses to head off to construct\n", m_unit->getDescription().c_str(), m_unit->getID()).c_str());
		return;
	}
	if (upgradeWorker())
	{
		return;
	}

	if (hurry(true))
	{
		return;
	}

	// TODO: move out of workermove and into new unitai called UNITAI_MISSIONUNIT ?
	if (outcomeMission())
	{
		OutputDebugString(CvString::format("%S (%d) chooses to head off to do an outcome mission\n", m_unit->getDescription().c_str(), m_unit->getID()).c_str());
		return;
	}

	if (!m_unit->getGroup()->canDefend() && (m_unit->isHuman() && GET_PLAYER(m_unit->getOwner()).AI_isPlotThreatened(m_unit->plot(), 2) || !m_unit->isHuman() && workerNeedsDefender(m_unit->plot())) && retreatToCity()) // XXX maybe not do m_unit??? could be working productively somewhere else...
	{
		return;
	}

	bool bCanRoute = m_unit->canBuildRoute();
	// Workboats don't build Sea Tunnels over Resources
	if (bCanRoute && !bAbroad && m_unit->getDomainType() != DOMAIN_SEA)
	{
		const BonusTypes eNonObsoleteBonus = m_unit->plot()->getNonObsoleteBonusType(m_unit->getTeam());

		if (NO_BONUS != eNonObsoleteBonus
		&& !m_unit->plot()->isConnectedToCapital()
		&& NO_IMPROVEMENT != m_unit->plot()->getImprovementType()
		&& GC.getImprovementInfo(m_unit->plot()->getImprovementType()).isImprovementBonusTrade(eNonObsoleteBonus)
		&& connectPlot(m_unit->plot()))
		{
			return;
		}
	}

	CvPlot* pBestBonusPlot = NULL;
	BuildTypes eBestBonusBuild = NO_BUILD;
	int iBestBonusValue = 0;

	// find bonuses within 2 moves to improve
	if (CvWorkerService::ImproveBonus(m_unit, 2))
	{
		return;
	}

	// Afforess - worker financial trouble check
	if (!m_unit->isHuman() && getUnitAIType() == UNITAI_WORKER
	&& GET_PLAYER(m_unit->getOwner()).AI_isFinancialTrouble()
	&& GET_PLAYER(m_unit->getOwner()).getUnitUpkeepNet(m_unit->isMilitaryBranch(), m_unit->getUpkeep100()) > 0)
	{
		const int iWorkers = GET_PLAYER(m_unit->getOwner()).AI_totalUnitAIs(UNITAI_WORKER);

		if (iWorkers > 3 && iWorkers > 2 * GET_PLAYER(m_unit->getOwner()).getNumCities())
		{
			if (gUnitLogLevel > 2)
			{
				logBBAI(
					"%S's %S at (%d,%d) is disbanding itself due to large number of workers available, and financial trouble.",
					GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getName(0).GetCString(), m_unit->getX(), m_unit->getY()
				);
			}
			m_unit->scrap();
			return;
		}
	}

	CvCity* pCity = NULL;

	if (!bAbroad)
	{
		pCity = m_unit->plot()->getPlotCity(); //get city on m_unit->plot

		if (pCity == NULL) //if city is not on same m_unit->plot
		{
			pCity = m_unit->plot()->getWorkingCity(); //get city that is owning m_unit m_unit->plot
		}

		if (pCity != NULL)
		{
			if (CvWorkerService::ShouldImproveCity(pCity) && improveCity(pCity))
			{
				return;
			}

			if (pCity->AI_getWorkersNeeded() > 0 && (m_unit->plot()->isCity() || pCity->AI_getWorkersNeeded() < (1 + pCity->getNumWorkers() * 2) / 3) && improveCity(pCity))
			{
				return;
			}
		}
	}

	if (improveLocalPlot(2, pCity))
	{
		return;
	}

	// find bonuses within 4 moves to improve
	if (CvWorkerService::ImproveBonus(m_unit))
	{
		return;
	}

	if (bCanRoute && m_unit->isNPC() && connectCity())
	{
		return;
	}

	{
		const bool bNextCity = pCity == NULL || pCity->AI_getWorkersNeeded() == 0 || pCity->getNumWorkers() > pCity->AI_getWorkersNeeded() + 1;

		if (bNextCity)
		{
			if (pBestBonusPlot != NULL && iBestBonusValue >= 15 && improvePlot(pBestBonusPlot, eBestBonusBuild))
			{
				return;
			}

			if (nextCityToImprove(pCity))
			{
				return;
			}
		}

		if (pBestBonusPlot != NULL && improvePlot(pBestBonusPlot, eBestBonusBuild))
		{
			return;
		}

		if (pCity != NULL && improveCity(pCity))
		{
			return;
		}

		if (!bNextCity && nextCityToImprove(pCity))
		{
			return;
		}
	}

	if (bCanRoute)
	{
		if (routeTerritory(true))
		{
			return;
		}

		if (connectBonus(false))
		{
			return;
		}

		if (routeCity())
		{
			return;
		}
	}

	if (improveLocalPlot(3, NULL))
	{
		return;
	}

	if (irrigateTerritory())
	{
		return;
	}

	if (bCanRoute && routeTerritory())
	{
		return;
	}

	if (!m_unit->isHuman() || m_unit->isAutomated() && !GET_TEAM(m_unit->getTeam()).isAtWar())
	{
		if (!m_unit->isHuman() || m_unit->getGameTurnCreated() < GC.getGame().getGameTurn())
		{
			if (nextCityToImproveAirlift())
			{
				return;
			}
		}
		if (!m_unit->isHuman())
		{
			// Fill up boats which already have workers
			if (load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_WORKER, -1, -1, -1, -1, MOVE_SAFE_TERRITORY))
			{
				return;
			}

			// Avoid filling a galley which has just a settler in it, reduce chances for other ships
			if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
			{
				if (load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, NO_UNITAI, -1, (2 * GET_PLAYER(m_unit->getOwner()).getBestUnitTypeCargoVolume(UNITAI_WORKER)), -1, -1, MOVE_SAFE_TERRITORY))
				{
					return;
				}
			}
			else if (load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, NO_UNITAI, -1, 2, -1, -1, MOVE_SAFE_TERRITORY))
			{
				return;
			}
		}
	}


	if (!m_unit->isHuman() && !m_unit->isNPC())
	{
		if (!bAbroad && load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, 2, -1, -1, 0, MOVE_SAFE_TERRITORY))
		{
			return;
		}
	}

	if (!m_unit->isHuman() && getUnitAIType() == UNITAI_WORKER)
	{
		const int iWorkers = GET_PLAYER(m_unit->getOwner()).AI_totalUnitAIs(UNITAI_WORKER);

		if (iWorkers > 5 && iWorkers > 5 * GET_PLAYER(m_unit->getOwner()).getNumCities()
		&& GET_PLAYER(m_unit->getOwner()).AI_isFinancialTrouble()
		&& GET_PLAYER(m_unit->getOwner()).getUnitUpkeepNet(m_unit->isMilitaryBranch(), m_unit->getUpkeep100()) > 0)
		{
			if (gUnitLogLevel > 2)
			{
				logBBAI("	%S's %S at (%d,%d) is disbanding itself due to large number of workers available", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getName(0).GetCString(), m_unit->getX(), m_unit->getY());
			}
			m_unit->scrap();
			return;
		}
	}

	if (retreatToCity(false, true))
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (m_unit->getGroup()->isStranded() && load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
}

void CvUnitDefaultAI::barbAttackMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	if (anyAttack(0, 60))
	{
		return;
	}

	if (anyAttack(1, 70))
	{
		return;
	}

	if (m_unit->plot()->isGoody())
	{
		if (anyAttack(1, 80))
		{
			return;
		}

		if (m_unit->plot()->plotCount(PUF_isUnitAIType, UNITAI_ATTACK, -1, NULL, m_unit->getOwner()) == 1 && m_unit->getGroup()->getNumUnits() == 1)
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	if (GC.getGame().getSorenRandNum(3, "AI Barb") == 0 && pillageRange(1))
	{
		return;
	}

	const bool bRaging = GC.getGame().isOption(GAMEOPTION_RAGING_BARBARIANS);

	if (bRaging || GC.getGame().getNumCivCities() > GC.getGame().countCivPlayersAlive() * 2)
	{
		if (anyAttack(1, 20)
		|| cityAttack(2, 15)
		|| pillageRange(4)
		|| cityAttack(3, 10))
		{
			return;
		}
	}
	else if (
		anyAttack(1, 30)
	|| pillageRange(2)
	|| cityAttack(1, 20))
	{
		return;
	}

	const AreaAITypes eAreaAI = m_unit->area()->getAreaAIType(m_unit->getTeam());

	if (eAreaAI == AREAAI_OFFENSIVE)
	{
		if (groupMergeRange(UNITAI_ATTACK, 2, true, true, true)
		|| groupMergeRange(UNITAI_ATTACK_CITY, 4, true, true, true))
		{
			return;
		}
		if (goToTargetCity(0, MAX_BARB_TARGET_CITY_RANGE)
		|| goToTargetCity(0, MAX_INT, m_unit->area()->getTargetCity(m_unit->getOwner())))
		{
			return;
		}
	}
	else if (eAreaAI == AREAAI_ASSAULT)
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK_CITY, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
		{
			return;
		}
	}

	if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 1))
	{
		return;
	}

	if (heal())
	{
		return;
	}

	if (patrol(bRaging))
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::attackMove()
{
	PROFILE_FUNC();

	if (selectStatus(true) || checkSwitchToConstruct())
	{
		return;
	}

	if (MISSIONAI_REGROUP == m_unit->getGroup()->AI_getMissionAIType())
	{
		if (group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(2).maxPathTurns(1).allowRegrouping()))
		{
			return;
		}
		if (groupMergeRange(UNITAI_ATTACK_CITY, 1, false, true))
		{
			return;
		}
	}
	bool bDanger = (GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(m_unit->plot(), 3));

	if (bDanger)
	{
		if (m_unit->plot()->isCity() && m_unit->plot()->getOwner() == m_unit->getOwner())
		{
			// Attack choking units
			const int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
			const int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

			if (iOurDefense < 3 * iEnemyOffense && guardCity(true))
			{
				return;
			}
			if (iEnemyOffense > 0 && iOurDefense > 2 * iEnemyOffense && anyAttack(2, 55))
			{
				return;
			}
			if (groupMergeRange(UNITAI_ATTACK, 1, true, true, false))
			{
				return;
			}
			if (iEnemyOffense > 0 && iOurDefense > 2 * iEnemyOffense && anyAttack(2, 30))
			{
				return;
			}
		}
	}
	else if (m_unit->getGroup()->getNumUnits() > 2 && m_unit->plot()->getOwner() == m_unit->getOwner() && m_unit->getGroup()->getHeadUnitAI() == getUnitAIType())
	{ // No danger
		// Shouldn't have groups of > 2 attack units
		if (m_unit->getGroup()->countNumUnitAIType(UNITAI_ATTACK) > 2)
		{
			m_unit->getGroup()->AI_separate(); // will change group

			FAssert(m_unit->getGroup()->getHeadUnitAI() == m_unit->getGroup()->getHeadUnitAI());
		}
		// Should never have attack city group lead by attack unit
		if (m_unit->getGroup()->countNumUnitAIType(UNITAI_ATTACK_CITY) > 0)
		{
			m_unit->getGroup()->AI_separateAI(UNITAI_ATTACK_CITY); // will change group
			// Skip to let ATTACK_CITY units move to avoid hang since ATTACK can try to join ATTACK_CITY again.
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	{
		PROFILE("CvUnitDefaultAI::attackMove() 1");

		// Guard a city we're in if it needs it
		if (guardCity(true))
		{
			return;
		}

		//	No high priority actions to take so see if anyone need help
		if (processContracts())
		{
			return;
		}

		if (!m_unit->plot()->isOwned() && groupMergeRange(UNITAI_SETTLE, 2, true, false, false))
		{
			return;
		}

		if ((!m_unit->plot()->isOwned() || m_unit->plot()->getOwner() == m_unit->getOwner())
		&& m_unit->area()->getCitiesPerPlayer(m_unit->getOwner()) > GET_PLAYER(m_unit->getOwner()).AI_totalAreaUnitAIs(m_unit->area(), UNITAI_CITY_DEFENSE)
		&& guardCity(true, true, 3)) // Defend colonies in new world
		{
			return;
		}

		if (heal(30, 1))
		{
			return;
		}

		if (!bDanger)
		{
			if (group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(1).maxPathTurns(3).allowRegrouping()))
			{
				return;
			}

			if (group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(2).maxPathTurns(3).allowRegrouping()))
			{
				return;
			}
		}

		if (guardCityAirlift())
		{
			return;
		}

		if (guardCity(false, true, 1))
		{
			return;
		}

		//join any city attacks in progress
		//	Koshling - changed m_unit to happen unconditionally (used to only happen inside
		//	enemy territory) otherwise stacks massing on the borders didn't merge and reach
		//	a sufficient stack power threshold to actually start the city attack run
		if (groupMergeRange(UNITAI_ATTACK_CITY, 1, false, true))
		{
			return;
		}

		AreaAITypes eAreaAIType = m_unit->area()->getAreaAIType(m_unit->getTeam());
		if (m_unit->plot()->isCity() && m_unit->plot()->getOwner() == m_unit->getOwner()
		&& (eAreaAIType == AREAAI_ASSAULT || eAreaAIType == AREAAI_ASSAULT_ASSIST)
		&& offensiveAirlift())
		{
			return;
		}

		if (bDanger)
		{
			if (cityAttack(1, 55) || anyAttack(1, 65))
			{
				return;
			}

			if (m_unit->getGroup()->hasCollateralDamage())
			{
				// RevolutionDCM - ranged bombardment AI
				// In theory trebs can be set to UNITAI_ATTACK
				// Dale - RB: Field Bombard START
				if (RbombardUnit(1, 50, 3, 1, 120))
				{
					return;
				}
				// RevolutionDCM - end

				if (anyAttack(1, 45, 3))
				{
					return;
				}
			}
		}

		if (!m_unit->noDefensiveBonus() && guardCity(false, false))
		{
			return;
		}

		if (!bDanger && m_unit->plot()->getOwner() == m_unit->getOwner())
		{
			// Assault AI
			if ((eAreaAIType == AREAAI_ASSAULT || eAreaAIType == AREAAI_ASSAULT_MASSING || eAreaAIType == AREAAI_ASSAULT_ASSIST)

			&& load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK_CITY, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
			{
				return;
			}

			if (load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, -1, 1, MOVE_SAFE_TERRITORY, 3))
			{
				return;
			}

			// Not land war AI
			if (!(eAreaAIType == AREAAI_OFFENSIVE || eAreaAIType == AREAAI_DEFENSIVE || eAreaAIType == AREAAI_MASSING))
			{
				// Fill transports before starting new one, but not just full of our unit ai
				if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, 1, -1, -1, 1, MOVE_SAFE_TERRITORY, 4))
				{
					return;
				}

				// Pick new transport which has space for other unit ai types to join
				if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
				{
					if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, (2 * GET_PLAYER(m_unit->getOwner()).getBestUnitTypeCargoVolume(UNITAI_ASSAULT_SEA)), -1, -1, MOVE_SAFE_TERRITORY, 4))
					{
						return;
					}
				}
				else if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, 2, -1, -1, MOVE_SAFE_TERRITORY, 4))
				{
					return;
				}
			}

			if (GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, MISSIONAI_GROUP) > 0)
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP);
				return;
			}
		}

		// Allow larger groups if inside enemy territory
		if (m_unit->getGroup()->getNumUnits() < 3 && m_unit->plot()->isOwned() && GET_TEAM(m_unit->getTeam()).isAtWar(m_unit->plot()->getTeam())
		&& groupMergeRange(UNITAI_ATTACK, 1, true, true, true))
		{
			return;
		}

		// Go for goody or an easy battle
		if (goody(3) || anyAttack(1, 70))
		{
			return;
		}
	}

	{
		PROFILE("CvUnitDefaultAI::attackMove() 2");

		if (bDanger)
		{
			if (pillageRange(1, 20)
			|| cityAttack(1, 35)
			|| anyAttack(1, 45)
			|| pillageRange(3, 20)
			|| m_unit->getGroup()->getNumUnits() < 4 && choke(1))
			{
				return;
			}
		}
	}

	if (m_unit->plot()->isCity() && paradrop(m_unit->getDropRange()))
	{
		return;
	}

	{
		PROFILE("CvUnitDefaultAI::attackMove() 2");

		if (bDanger && (cityAttack(4, 30) || anyAttack(2, 40)))
		{
			return;
		}

		if (!m_unit->isEnemy(m_unit->plot()->getTeam()) && heal())
		{
			return;
		}

		if (GET_PLAYER(m_unit->getOwner()).AI_getNumAIUnits(UNITAI_CITY_DEFENSE) > 0 || GET_TEAM(m_unit->getTeam()).isAtWar())
		{
			// BBAI TODO: If we're fast, maybe shadow an attack city stack and pillage off of it

			const bool bIgnoreFaster = GET_PLAYER(m_unit->getOwner()).AI_isDoStrategy(AI_STRATEGY_LAND_BLITZ) && m_unit->area()->getAreaAIType(m_unit->getTeam()) != AREAAI_ASSAULT;

			if (group(GroupingParams().withUnitAI(UNITAI_ATTACK_CITY).maxGroupSize(1).maxOwnUnitAI(1).ignoreFaster(bIgnoreFaster).ignoreOwnUnitType().stackOfDoom().maxPathTurns(5)))
			{
				return;
			}

			if (group(GroupingParams().withUnitAI(UNITAI_ATTACK).maxGroupSize(1).maxOwnUnitAI(1).ignoreFaster().ignoreOwnUnitType().maxPathTurns(4)))
			{
				return;
			}

			/* BBAI TODO: Need group to be fast, need to ignore slower groups

						if (GET_PLAYER(m_unit->getOwner()).AI_isDoStrategy(AI_STRATEGY_FASTMOVERS)
						&& group(GroupingParams().withUnitAI(UNITAI_ATTACK).maxGroupSize(4).maxOwnUnitAI(1).ignoreFaster().maxPathTurns(3)))
						{
							return;
						}
			*/
			if (group(GroupingParams().withUnitAI(UNITAI_ATTACK).maxGroupSize(1).maxOwnUnitAI(1).ignoreFaster().maxPathTurns(1)))
			{
				return;
			}
		}

		if (m_unit->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_OFFENSIVE)
		{
			if (m_unit->getGroup()->getNumUnits() > 1 && goToTargetCity(MOVE_AVOID_ENEMY_WEIGHT_2, MAX_BARB_TARGET_CITY_RANGE))
			{
				return;
			}
		}
		else if (m_unit->area()->getAreaAIType(m_unit->getTeam()) != AREAAI_DEFENSIVE
		&& (m_unit->area()->getCitiesPerPlayer(BARBARIAN_PLAYER) > 0 || m_unit->area()->getCitiesPerPlayer(NEANDERTHAL_PLAYER) > 0)
		&& m_unit->getGroup()->getNumUnits() >= GC.getHandicapInfo(GC.getGame().getHandicapType()).getBarbarianInitialDefenders()
		&& goToTargetBarbCity(10)) // Deal with Barbarian pest
		{
			return;
		}

		if (guardCity(false, true, 3))
		{
			return;
		}

		if (GET_PLAYER(m_unit->getOwner()).getNumCities() > 1 && m_unit->getGroup()->getNumUnits() == 1 && m_unit->area()->getAreaAIType(m_unit->getTeam()) != AREAAI_DEFENSIVE
		&& m_unit->area()->getNumUnrevealedTiles(m_unit->getTeam()) > 0
		&& GET_PLAYER(m_unit->getOwner()).AI_areaMissionAIs(m_unit->area(), MISSIONAI_EXPLORE, m_unit->getGroup()) < GET_PLAYER(m_unit->getOwner()).AI_neededExplorers(m_unit->area()) + 1
		&& (exploreRange(3) || explore()))
		{
			return;
		}

		if (protect(35, 5))
		{
			return;
		}

		if (offensiveAirlift())
		{
			return;
		}

		if (!bDanger && (m_unit->area()->getAreaAIType(m_unit->getTeam()) != AREAAI_DEFENSIVE) && m_unit->plot()->getOwner() == m_unit->getOwner())
		{
			if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, 1, -1, -1, 1, MOVE_SAFE_TERRITORY, 4))
			{
				return;
			}
			if (GET_TEAM(m_unit->getTeam()).isAtWar() && !m_unit->getGroup()->isHasPathToAreaEnemyCity()
			&& load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
			{
				return;
			}
		}

		if (defend() || travelToUpgradeCity())
		{
			return;
		}

		if (m_unit->getGroup()->isStranded()
		&& load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}

		// If no other desireable actions, wait for pickup
		if (!bDanger && !m_unit->isHuman() && m_unit->plot()->isCoastalLand() && GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, MISSIONAI_PICKUP) > 0)
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}

		if (m_unit->getGroup()->getNumUnits() < 4 && patrol())
		{
			return;
		}

		if (retreatToCity() || safety(3))
		{
			return;
		}
	}
	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::paratrooperMove()
{
	PROFILE_FUNC();
	if (selectStatus(true))
	{
		return;
	}
	bool bHostile = (m_unit->plot()->isOwned() && m_unit->isPotentialEnemy(m_unit->plot()->getTeam()));
	if (!bHostile)
	{
		if (guardCity(true))
		{
			return;
		}

		if (m_unit->plot()->getTeam() == m_unit->getTeam())
		{
			if (m_unit->plot()->isCity())
			{
				if (heal(30, 1))
				{
					return;
				}
			}

			AreaAITypes eAreaAIType = m_unit->area()->getAreaAIType(m_unit->getTeam());
			bool bLandWar = ((eAreaAIType == AREAAI_OFFENSIVE) || (eAreaAIType == AREAAI_DEFENSIVE) || (eAreaAIType == AREAAI_MASSING));
			if (!bLandWar)
			{
				if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, 0, MOVE_SAFE_TERRITORY, 4))
				{
					return;
				}
			}
		}

		if (guardCity(false, true, 1))
		{
			return;
		}
	}

	if (cityAttack(1, 45))
	{
		return;
	}

	if (anyAttack(1, 55))
	{
		return;
	}

	if (!bHostile)
	{
		if (paradrop(m_unit->getDropRange()))
		{
			return;
		}

		if (offensiveAirlift())
		{
			return;
		}

		if (moveToStagingCity())
		{
			return;
		}

		if (guardFort(true))
		{
			return;
		}

		if (guardCityAirlift())
		{
			return;
		}
	}

	if (m_unit->getGroup()->hasCollateralDamage())
	{
		if (anyAttack(1, 45, 3))
		{
			return;
		}
	}

	if (pillageRange(1, 15))
	{
		return;
	}

	if (bHostile)
	{
		if (choke(1))
		{
			return;
		}
	}

	if (heal())
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  09/18/09								jdog5000	  */
	/*																							  */
	/* Unit AI																					  */
	/************************************************************************************************/
		//if (protect(35))
	if (protect(35, 5))
	{
		return;
	}

	if (m_unit->getGroup()->isStranded())
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/


	if (safety(3))
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::attackCityMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}
	if (checkSwitchToConstruct())
	{
		return;
	}

	AreaAITypes eAreaAIType = m_unit->area()->getAreaAIType(m_unit->getTeam());
	bool bLandWar = eAreaAIType == AREAAI_OFFENSIVE || eAreaAIType == AREAAI_DEFENSIVE || eAreaAIType == AREAAI_MASSING;
	bool bAssault = eAreaAIType == AREAAI_ASSAULT || eAreaAIType == AREAAI_ASSAULT_ASSIST || eAreaAIType == AREAAI_ASSAULT_MASSING;

	bool bTurtle = GET_PLAYER(m_unit->getOwner()).AI_isDoStrategy(AI_STRATEGY_TURTLE);
	bool bAlert1 = GET_PLAYER(m_unit->getOwner()).AI_isDoStrategy(AI_STRATEGY_ALERT1);
	bool bIgnoreFaster = false;
	if (GET_PLAYER(m_unit->getOwner()).AI_isDoStrategy(AI_STRATEGY_LAND_BLITZ))
	{
		if (!bAssault && m_unit->area()->getCitiesPerPlayer(m_unit->getOwner()) > 0)
		{
			bIgnoreFaster = true;
		}
	}

	//	Super forts - allow stack to split after fort capture as well as city
	bool bInCity = m_unit->plot()->isCity(false);

	if (bInCity && m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		// force heal if we in our own city and damaged
		// can we remove m_unit or call heal here?
		if (heal())
		{
			return;
		}

		if (m_unit->plot()->getOwnershipDuration() <= 1)
		{
			CvSelectionGroup* pOldGroup = m_unit->getGroup();

			pOldGroup->AI_separateNonAI(UNITAI_ATTACK_CITY);

			if (pOldGroup != m_unit->getGroup())
			{
				return;
			}
		}

		if ((eAreaAIType == AREAAI_ASSAULT) || (eAreaAIType == AREAAI_ASSAULT_ASSIST))
		{
			if (offensiveAirlift())
			{
				return;
			}
		}
	}

	bool bAtWar = m_unit->isEnemy(m_unit->plot()->getTeam());

	bool bHuntBarbs = false;

	if (!m_unit->isNPC() && m_unit->area()->getCitiesPerPlayer(BARBARIAN_PLAYER) > 0 || m_unit->area()->getCitiesPerPlayer(NEANDERTHAL_PLAYER) > 0)
	{
		if ((eAreaAIType != AREAAI_OFFENSIVE) && (eAreaAIType != AREAAI_DEFENSIVE) && !bAlert1 && !bTurtle)
		{
			bHuntBarbs = true;
		}
	}

	bool bReadyToAttack = false;
	if (m_unit->isHominid())
	{
		bLandWar = m_unit->area()->getNumCities() - m_unit->area()->getCitiesPerPlayer(m_unit->getOwner()) > 0;
		bReadyToAttack = m_unit->getGroup()->getNumUnits() >= stackOfDoomExtra();
	}
	else if (!bTurtle)
	{
		bReadyToAttack = m_unit->getGroup()->getNumUnits() >= (bHuntBarbs ? 3 : stackOfDoomExtra());
	}

	const bool bCity = m_unit->plot()->isCity();

	if (bReadyToAttack)
	{
		// Check that stack has units which can capture cities
		bReadyToAttack = false;
		int iCityCaptureCount = 0;

		CLLNode<IDInfo>* pUnitNode = m_unit->getGroup()->headUnitNode();
		while (pUnitNode != NULL && !bReadyToAttack)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = m_unit->getGroup()->nextUnitNode(pUnitNode);

			if (!pLoopUnit->isOnlyDefensive())
			{
				if (!(pLoopUnit->isNoCapture()) && (pLoopUnit->combatLimit() >= 100))
				{
					iCityCaptureCount++;

					if (iCityCaptureCount > 5 || 3 * iCityCaptureCount > m_unit->getGroup()->getNumUnits())
					{
						bReadyToAttack = true;
					}
				}
			}
		}

		//	Special case - if we have no attackers at all advertise for one urgently
		if (iCityCaptureCount == 0 && !m_unit->isCargo())
		{
			GET_PLAYER(m_unit->getOwner()).getContractBroker().advertiseWork(
				HIGH_PRIORITY_ESCORT_PRIORITY + 1,
				NO_UNITCAPABILITIES,
				m_unit->getX(),
				m_unit->getY(),
				m_unit,
				UNITAI_ATTACK_CITY
			);
			m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
			m_contractualState = CONTRACTUAL_STATE_AWAITING_ANSWER;

			if (gUnitLogLevel > 2)
			{
				CvWString szString;
				getUnitAIString(szString, getUnitAIType());
				logBBAI("	%S's %S at (%d,%d) requests initial city-capture-capable attacker for attack stack at priority %d", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getName(0).GetCString(), m_unit->getX(), m_unit->getY(), HIGH_PRIORITY_ESCORT_PRIORITY + 1);
			}
		}
	}

	if (groupMergeRange(UNITAI_ATTACK_CITY, 0, true, true, bIgnoreFaster))
	{
		return;
	}

	CvCity* pTargetCity = NULL;
	if (m_unit->isHominid())
	{
		pTargetCity = m_unit->area()->getTargetCity(m_unit->getOwner());
	}
	else
	{
		pTargetCity = pickTargetCity(MOVE_THROUGH_ENEMY, MAX_INT, bHuntBarbs);
	}

	if (pTargetCity != NULL)
	{
		int iStepDistToTarget = stepDistance(pTargetCity->getX(), pTargetCity->getY(), m_unit->getX(), m_unit->getY());
		int iAttackRatio = std::max(100, GC.getBBAI_ATTACK_CITY_STACK_RATIO());

		//	Koshling - use the difficulty level of the opponent's settings to estimate how
		//	much more prepared we really need to be - m_unit should cause players on higher difficulty
		//	settings to get attacked by larger stacks rather than prematurely by not-quite-adequate ones
		//	Only do m_unit if we're still in preparation mode either in our own territory or some way away from
		//	the target (before we lose the benefit of surprise)
		if (m_unit->plot()->getOwner() == m_unit->getOwner() || iStepDistToTarget > 3)
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

		if (m_unit->isHominid())
		{
			iAttackRatio = std::max(80, iAttackRatio / 2);
		}

		int iComparePostBombard = 0;
		// AI gets a 1-tile sneak peak to compensate for lack of memory
		if (iStepDistToTarget <= 2 || pTargetCity->isVisible(m_unit->getTeam(), false))
		{
			iComparePostBombard = m_unit->getGroup()->AI_compareStacks(pTargetCity->plot(), StackCompare::CheckCanAttack | StackCompare::CheckCanMove | StackCompare::PotentialEnemy, std::min(2, iStepDistToTarget - 1));

			/* original BBAI code
			int iDefenseModifier = pTargetCity->getDefenseModifier(true);
			int iBombardTurns = m_unit->getGroup()->getBombardTurns(pTargetCity);
			iDefenseModifier *= std::max(0, 20 - iBombardTurns);
			iDefenseModifier /= 20;
			iComparePostBombard *= 100 + std::max(0, iDefenseModifier);
			iComparePostBombard /= 100; */

			// K-Mod, appart from the fact that they got the defence reduction backwards; the defense modifier
			// is counted in compareStacks. So if we add it again, we'd be double counting.
			// In fact, it's worse than that because it would compound.
			// I'm going to subtract defence, but unfortunately m_unit will reduce based on the total rather than the base.
			int iDefenseModifier = pTargetCity->getDefenseModifier(false);
			int iBombardTurns = m_unit->getGroup()->getBombardTurns(pTargetCity);
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
			// With m_unit, bombarding a defence bonus of 100% with reduce effective defence by 50%

			if (iComparePostBombard < iAttackRatio && !bAtWar)
			{
				//	Koshling - if we find we are not strong enough when we get target visibility
				//	don't start a war if we haven't already.
				//	Note that m_unit is at best a very partial fix.  Changes are needed to determine
				//	whether we are likely to have enough strength much sooner.  Things to do:
				//	1)	Add some sort of history for the last-seen defense of each city and use
				//		the most recent value as an estimatre for cities we don't yet have visibility on
				//	2)	Use stealth units to frequently regain visibility on at least the m_unit->area target city
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
				if (m_unit->plot()->getUnitCombatsUnsupportedByHealer(m_unit->getOwner(), eHealType, m_unit->getDomainType()) > 4)
				{
					iNeedSupportCount = -4;
					foreach_(const CvUnit * pLoopUnit, m_unit->getGroup()->units())
					{
						if (pLoopUnit->isHasUnitCombat(eHealType))
						{
							iNeedSupportCount++;
						}
						iNeedSupportCount /= 4;
						if (pLoopUnit->isHealsUnitCombat(eHealType))
						{
							iNeedSupportCount -= pLoopUnit->getNumHealSupportTotal();
							if (pLoopUnit->getNumHealSupportTotal())
							{
								iNeedSupportCount--;
							}
						}
					}
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
			if (!m_unit->isCargo() && eMostNeeded != NO_UNITCOMBAT)
			{
				CvUnitSelectionCriteria healerCriteria;
				healerCriteria.m_bIsHealer = true;
				healerCriteria.m_eHealUnitCombat = eMostNeeded;

				GET_PLAYER(m_unit->getOwner()).getContractBroker().advertiseWork(
					HIGH_PRIORITY_ESCORT_PRIORITY,
					HEALER_UNITCAPABILITIES,
					m_unit->getX(), m_unit->getY(),
					m_unit, UNITAI_HEALER,
					-1, &healerCriteria
				);
				m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
				m_contractualState = CONTRACTUAL_STATE_AWAITING_ANSWER;
				if (gUnitLogLevel > 2)
				{
					CvWString szString;
					getUnitAIString(szString, getUnitAIType());
					logBBAI("	%S's %S at (%d,%d) requests healer for attack stack", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getName(0).GetCString(), m_unit->getX(), m_unit->getY());
				}
			}

			// If we're still on home turf advertise for more attackers at a priority that is increased if we're not yet up to strength
			if (m_unit->plot()->getOwner() == m_unit->getOwner() && !m_unit->isCargo())
			{
				//First cover the see invisible bases
				if (establishStackSeeInvisibleCoverage())
				{
					return;
				}
				GET_PLAYER(m_unit->getOwner()).getContractBroker().advertiseWork(
					bReadyToAttack ? LOW_PRIORITY_ESCORT_PRIORITY : HIGH_PRIORITY_ESCORT_PRIORITY,
					NO_UNITCAPABILITIES,
					m_unit->getX(), m_unit->getY(),
					m_unit, UNITAI_ATTACK_CITY
				);
				m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
				m_contractualState = CONTRACTUAL_STATE_AWAITING_ANSWER;
				if (gUnitLogLevel > 2)
				{
					CvWString szString;
					getUnitAIString(szString, getUnitAIType());
					logBBAI("	%S's %S at (%d,%d) requests more attackers for attack stack at priority %d", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getName(0).GetCString(), m_unit->getX(), m_unit->getY(), bReadyToAttack ? LOW_PRIORITY_ESCORT_PRIORITY : HIGH_PRIORITY_ESCORT_PRIORITY);
				}

				//	Also try to get a great commander if we don't have one
				if (GC.getGame().isOption(GAMEOPTION_GREAT_COMMANDERS) && !m_unit->getGroup()->hasCommander())
				{
					GET_PLAYER(m_unit->getOwner()).getContractBroker().advertiseWork(
						bReadyToAttack ? HIGH_PRIORITY_ESCORT_PRIORITY : LOW_PRIORITY_ESCORT_PRIORITY,
						NO_UNITCAPABILITIES,
						m_unit->getX(), m_unit->getY(),
						m_unit, UNITAI_GENERAL
					);
					m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
					m_contractualState = CONTRACTUAL_STATE_AWAITING_ANSWER;
					if (gUnitLogLevel > 2)
					{
						CvWString szString;
						getUnitAIString(szString, getUnitAIType());
						logBBAI("	%S's %S at (%d,%d) requests great commander for attack stack at priority %d", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getName(0).GetCString(), m_unit->getX(), m_unit->getY(), HIGH_PRIORITY_ESCORT_PRIORITY);
					}
				}
			}
		}

		if (iStepDistToTarget <= 2)
		{
			if (iComparePostBombard < iAttackRatio)
			{
				if (groupMergeRange(UNITAI_ATTACK_CITY, 2, true, true, bIgnoreFaster))
				{
					return;
				}

				int iOurOffense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 1, false, false, true);
				int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(pTargetCity->plot(), 2, false, false);

				// If in danger, seek defensive ground
				if (4 * iOurOffense < 3 * iEnemyOffense)
				{
					if (choke(1, true))
					{
						return;
					}
				}
			}

			if (iStepDistToTarget == 1)
			{
				// If next to target city and we would attack after bombarding down defenses,
				// or if defenses have crept up past half
				if ((iComparePostBombard >= iAttackRatio) || (pTargetCity->getDefenseDamage() < ((GC.getMAX_CITY_DEFENSE_DAMAGE() * 1) / 2)))
				{
					if (iComparePostBombard < std::max(150, GC.getBBAI_SKIP_BOMBARD_MIN_STACK_RATIO()) && (pTargetCity->isDirectAttackable() || m_unit->canIgnoreNoEntryLevel()))
					{
						// Move to good tile to attack from unless we're way more powerful
						if (goToTargetCity(0, 1, pTargetCity))
						{
							return;
						}
					}

					// Bombard may skip if stack is powerful enough
					if (bombardCity())
					{
						return;
					}

					//stack attack
					if (m_unit->getGroup()->getNumUnits() > 1)
					{
						// BBAI TODO: What is right ratio?
						if (stackAttackCity(1, iAttackRatio, true))
						{
							return;
						}
					}

					// If not strong enough alone, merge if another stack is nearby
					if (groupMergeRange(UNITAI_ATTACK_CITY, 2, true, true, bIgnoreFaster))
					{
						return;
					}

					if (m_unit->getGroup()->getNumUnits() == 1)
					{
						if (cityAttack(1, 50))
						{
							return;
						}
					}
				}
			}

			if (bCity && paradrop(m_unit->getDropRange()))
			{
				return;
			}

			if (iComparePostBombard < iAttackRatio)
			{
				// If not strong enough, pillage around target city without exposing ourselves
				if (pillageRange(1))
				{
					return;
				}

				if (anyAttack(1, 60, 0, false))
				{
					return;
				}

				if (heal(((iComparePostBombard * 100) / iAttackRatio >= 80) ? 0 : 30, 1))
				{
					return;
				}

				// Pillage around enemy city
				if (pillageAroundCity(pTargetCity, 11, 3))
				{
					return;
				}

				if (pillageAroundCity(pTargetCity, 0, 5))
				{
					return;
				}

				if (choke(1))
				{
					return;
				}
			}
			else if (goToTargetCity(0, 4, pTargetCity))
			{
				return;
			}
		}
	}

	if (groupMergeRange(UNITAI_ATTACK_CITY, 2, true, true, bIgnoreFaster))
	{
		return;
	}

	if (heal(30, 1))
	{
		return;
	}

	// BBAI TODO: Stack v stack combat ... definitely want to do in own territory, but what about enemy territory?
	if (m_unit->getGroup()->hasCollateralDamage() && m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		if (RbombardUnit(1, 50, 3, 2, 130))
		{
			return;
		}

		if (anyAttack(1, 45, 3, false))
		{
			return;
		}

		if (!bReadyToAttack)
		{
			if (RbombardUnit(m_unit->getGroup()->getMinimumRBombardRange(), 30, 5, 2, 110))
			{
				return;
			}

			if (anyAttack(1, 25, 5))
			{
				return;
			}
			// K-Mod; sometimes you just need to blast them with collateral damage before they can do it to you!
			// ... and please don't let them wipe out a massive stack just because we were too scared to fight back

			// look. I don't want to spend too much time messing around with m_unit. So let me just try an experimental heuristic...
			if (m_unit->getGroup()->getNumUnits() > 1 && anyAttack(1, 60 / (m_unit->getGroup()->getNumUnits() + 1), 3 + m_unit->getGroup()->getNumUnits(), false))
			{
				return;
			}
		}
	}

	//	If we're still in our own territory attack infiltrators
	if (m_unit->plot()->getOwner() == m_unit->getOwner() && anyAttack(2, 50))
	{
		return;
	}
	if (anyAttack(1, 60, 0, false))
	{
		return;
	}

	if (bAtWar && (m_unit->getGroup()->getNumUnits() <= 2))
	{
		if (pillageRange(3, 11))
		{
			return;
		}

		if (pillageRange(1))
		{
			return;
		}
	}

	if (m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		if (!bLandWar)
		{
			if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
			{
				return;
			}
		}

		if (bReadyToAttack)
		{
			// Wait for units about to join our group
			MissionAITypes eMissionAIType = MISSIONAI_GROUP;
			int iJoiners = GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, &eMissionAIType, 1, m_unit->getGroup(), 2);

			if ((iJoiners * 5) > m_unit->getGroup()->getNumUnits())
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP);
				return;
			}
		}
		else
		{
			if (!m_unit->isNPC() && (eAreaAIType == AREAAI_DEFENSIVE))
			{
				// Use smaller attack city stacks on defense
				if (guardCity(false, true, 3))
				{
					return;
				}
			}

			if (bTurtle)
			{
				if (guardCity(false, true, 7))
				{
					return;
				}
			}

			int iTargetCount = GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, MISSIONAI_GROUP);
			if ((iTargetCount * 5) > m_unit->getGroup()->getNumUnits())
			{
				MissionAITypes eMissionAIType = MISSIONAI_GROUP;
				int iJoiners = GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, &eMissionAIType, 1, m_unit->getGroup(), 2);

				if ((iJoiners * 5) > m_unit->getGroup()->getNumUnits() || m_unit->isHominid())
				{
					m_unit->getGroup()->pushMission(MISSION_SKIP);
					return;
				}

				if (moveToStagingCity())
				{
					return;
				}
			}
		}
	}

	if (heal(50, 3))
	{
		return;
	}

	if (!bAtWar)
	{
		if (heal())
		{
			return;
		}

		if ((m_unit->getGroup()->getNumUnits() == 1) && (m_unit->getTeam() != m_unit->plot()->getTeam()))
		{
			if (retreatToCity())
			{
				return;
			}
		}
	}

	if (!bReadyToAttack && !m_unit->noDefensiveBonus())
	{
		if (guardCity(false, false))
		{
			return;
		}
	}

	const bool bAnyWarPlan = GET_TEAM(m_unit->getTeam()).hasWarPlan(true);

	if (bReadyToAttack)
	{
		if (m_unit->isHominid())
		{
			if (goToTargetCity(MOVE_AVOID_ENEMY_WEIGHT_2, MAX_BARB_TARGET_CITY_RANGE)
			|| goToTargetCity(MOVE_AVOID_ENEMY_WEIGHT_2, MAX_INT, pTargetCity))
			{
				return;
			}

			if (pillageRange(3, 11))
			{
				return;
			}

			if (pillageRange(1))
			{
				return;
			}
		}
		else if (bHuntBarbs && goToTargetBarbCity((bAnyWarPlan ? 7 : 12)))
		{
			return;
		}
		else if (bLandWar && pTargetCity != NULL)
		{
			// Before heading out, check whether to wait to allow unit upgrades
			if (bInCity && m_unit->plot()->getOwner() == m_unit->getOwner())
			{
				if (!GET_PLAYER(m_unit->getOwner()).AI_isFinancialTrouble())
				{
					// Check if stack has units which can upgrade
					int iNeedUpgradeCount = 0;

					foreach_(const CvUnit * pLoopUnit, m_unit->getGroup()->units())
					{
						if (pLoopUnit->getUpgradeCity(false) != NULL)
						{
							iNeedUpgradeCount++;

							if (8 * iNeedUpgradeCount > m_unit->getGroup()->getNumUnits())
							{
								m_unit->getGroup()->pushMission(MISSION_SKIP);
								return;
							}
						}
					}
				}
			}

			if (goToTargetCity(MOVE_AVOID_ENEMY_WEIGHT_2, 5, pTargetCity))
			{
				return;
			}

			if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 2, 2))
			{
				return;
			}

			if (goToTargetCity(MOVE_AVOID_ENEMY_WEIGHT_2, 8, pTargetCity))
			{
				return;
			}

			// Load stack if walking will take a long time
			if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4, 3))
			{
				return;
			}

			if (goToTargetCity(MOVE_AVOID_ENEMY_WEIGHT_2, 12, pTargetCity))
			{
				return;
			}

			if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4, 7))
			{
				return;
			}

			if (goToTargetCity(MOVE_AVOID_ENEMY_WEIGHT_2, MAX_INT, pTargetCity))
			{
				return;
			}

			if (bAnyWarPlan)
			{
				const CvCity* pTargetCity = m_unit->area()->getTargetCity(m_unit->getOwner());

				if (pTargetCity != NULL && solveBlockageProblem(pTargetCity->plot(), !GET_TEAM(m_unit->getTeam()).isAtWar()))
				{
					return;
				}
			}
		}
	}
	else
	{
		const int iTargetCount = GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, MISSIONAI_GROUP);
		if (((iTargetCount * 4) > m_unit->getGroup()->getNumUnits()) || ((m_unit->getGroup()->getNumUnits() + iTargetCount) >= (bHuntBarbs ? 3 : stackOfDoomExtra())))
		{
			MissionAITypes eMissionAIType = MISSIONAI_GROUP;
			const int iJoiners = GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, &eMissionAIType, 1, m_unit->getGroup(), 2);

			if ((iJoiners * 6) > m_unit->getGroup()->getNumUnits())
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP);
				return;
			}

			if (safety())
			{
				return;
			}
		}

		if (m_unit->getGroup()->hasBombardCapability() && m_unit->noDefensiveBonus())
		{
			// BBAI Notes: Add m_unit stack lead by bombard unit to stack probably not lead by a bombard unit
			// BBAI TODO: Some sense of minimum stack size?  Can have big stack moving 10 turns to merge with tiny stacks
			if (group(GroupingParams().withUnitAI(UNITAI_ATTACK_CITY)
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
		else if (group(GroupingParams().withUnitAI(UNITAI_ATTACK_CITY)
			.maxGroupSize(stackOfDoomExtra() * 2)
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

	if (m_unit->plot()->getOwner() == m_unit->getOwner() && bLandWar && GET_TEAM(m_unit->getTeam()).isAtWar())
	{
		// if no land path to enemy cities, try getting there another way
		if (offensiveAirlift())
		{
			return;
		}

		if (pTargetCity == NULL)
		{
			if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
			{
				return;
			}
		}
	}

	if (!m_unit->isNPC() && moveToStagingCity())
	{
		return;
	}

	if (offensiveAirlift())
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (m_unit->getGroup()->isStranded())
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}

		if (!m_unit->isHuman() && m_unit->plot()->isCoastalLand() && GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, MISSIONAI_PICKUP) > 0)
		{
			// If no other desireable actions, wait for pickup
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}

		if (patrol())
		{
			return;
		}
	}

	if (safety(3))
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::attackCityLemmingMove()
{
	if (selectStatus(true))
	{
		return;
	}

	if (cityAttack(1, 80))
	{
		return;
	}

	if (bombardCity())
	{
		return;
	}

	if (cityAttack(1, 40))
	{
		return;
	}

	if (goToTargetCity(MOVE_THROUGH_ENEMY))
	{
		return;
	}

	if (anyAttack(1, 70))
	{
		return;
	}

	if (anyAttack(1, 0))
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
}

void CvUnitDefaultAI::collateralMove()
{
	PROFILE_FUNC();

	if (leaveAttack(1, 20, 100))
	{
		return;
	}

	if (guardCity(false, true, 1))
	{
		return;
	}

	if (heal(30, 1))
	{
		return;
	}

	if (cityAttack(1, 35))
	{
		return;
	}

	if (RbombardUnit(1, 50, 3, 1, 100))
	{
		return;
	}

	if (anyAttack(1, 45, 3))
	{
		return;
	}

	if (anyAttack(1, 55, 2))
	{
		return;
	}

	if (RbombardUnit(1, 40, 3, 0, 100))
	{
		return;
	}

	if (anyAttack(1, 35, 3))
	{
		return;
	}

	if (anyAttack(1, 30, 4))
	{
		return;
	}

	if (RbombardUnit(1, 25, 5, 0, 80))
	{
		return;
	}

	if (anyAttack(1, 20, 5))
	{
		return;
	}

	if (heal())
	{
		return;
	}

	if (!m_unit->noDefensiveBonus())
	{
		if (guardCity(false, false))
		{
			return;
		}
	}

	if (RbombardUnit(m_unit->getGroup()->getMinimumRBombardRange(), 40, 3, 0, 100))
	{
		return;
	}

	if (anyAttack(2, 55, 3))
	{
		return;
	}

	if (cityAttack(2, 50))
	{
		return;
	}

	if (anyAttack(2, 60))
	{
		return;
	}

	if (protect(50, 8))
	{
		return;
	}

	if (RbombardPlot(m_unit->getDCMBombRange(), 20))
	{
		return;
	}

	if (guardCity(false, true, 3))
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (m_unit->getGroup()->isStranded())
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (safety(3))
	{
		return;
	}
	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::pillageMove()
{
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  03/05/10								jdog5000	  */
	/*																							  */
	/* Unit AI																					  */
	/************************************************************************************************/
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	//if (guardCity(false, true, 1))
	//{
	//	return;
	//}

	if (heal(30, 1))
	{
		return;
	}

	// BBAI TODO: Shadow ATTACK_CITY stacks and pillage

	//join any city attacks in progress
	//if ( MISSIONAI_REGROUP == m_unit->getGroup()->AI_getMissionAIType() || (m_unit->plot()->isOwned() && m_unit->plot()->getOwner() != m_unit->getOwner()))
	//{
	//	if (groupMergeRange(UNITAI_ATTACK_CITY, 1, false, true))
	//	{
	//		return;
	//	}
	//}

	if (cityAttack(1, 55))
	{
		return;
	}

	if (anyAttack(1, 65))
	{
		return;
	}

	//if (!m_unit->noDefensiveBonus())
	//{
	//	if (guardCity(false, false))
	//	{
	//		return;
	//	}
	//}

/*****************************************************************************************************/
/**  Author: TheLadiesOgre																		  **/
/**  Date: 16.09.2009																			   **/
/**  ModComp: TLOTags																			   **/
/**  Reason Added: Implement paradrop for other UNITAI's now that it is potentially available	**/
/**  Notes: 14.10.2009-- Added bCity check														  **/
/*****************************************************************************************************/
	bool bCity = m_unit->plot()->isCity();
	if (bCity)
	{
		if (paradrop(m_unit->getDropRange()))
		{
			return;
		}
	}

	/*****************************************************************************************************/
	/**  TheLadiesOgre; 16.09.2009; TLOTags															 **/
	/*****************************************************************************************************/
	if (pillageRange(3, 11))
	{
		return;
	}

	if (choke(1))
	{
		return;
	}

	if (pillageRange(1))
	{
		return;
	}

	if (m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
		{
			return;
		}
	}

	if (heal(50, 3))
	{
		return;
	}

	if (!m_unit->isEnemy(m_unit->plot()->getTeam()))
	{
		if (heal())
		{
			return;
		}
	}

	if (group(GroupingParams().withUnitAI(UNITAI_PILLAGE).maxGroupSize(1).maxOwnUnitAI(1).ignoreFaster().maxPathTurns(3)))
	{
		return;
	}

	if ((m_unit->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_OFFENSIVE) || m_unit->isEnemy(m_unit->plot()->getTeam()))
	{
		if (pillage(20))
		{
			return;
		}
	}

	if (heal())
	{
		return;
	}

	//if (guardCity(false, true, 3))
	//{
	//	return;
	//}

	if (offensiveAirlift())
	{
		return;
	}

	if (travelToUpgradeCity())
	{
		return;
	}

	if (!m_unit->isHuman() && m_unit->plot()->isCoastalLand() && GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, MISSIONAI_PICKUP) > 0)
	{
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (patrol())
	{
		return;
	}

	//if (retreatToCity())
	//{
	//	return;
	//}

	if (safety(3))
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/
}

void CvUnitDefaultAI::reserveMove()
{
	PROFILE_FUNC();

#ifdef OUTBREAKS_AND_AFFLICTIONS
	//TB Note: One big reason to split off healers into their own ai...
	for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
	{
		if (cureAffliction((PromotionLineTypes)iI))
		{
			return;
		}
	}
#endif
	//TB OOS debug note: not terribly necessary to update into AND as m_unit is not representing a fix but was adjusted to help with tracking
	bool bcheckSwitchToConstruct = checkSwitchToConstruct();
	if (bcheckSwitchToConstruct)
	{
		return;
	}

	bool bDanger = (GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(m_unit->plot(), 3));

	if (bDanger && leaveAttack(2, 55, 130))
	{
		return;
	}

	if (m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
		{
			if (load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, ((m_unit->SMCargoVolume() * 2) - 1), -1, MOVE_SAFE_TERRITORY))
			{
				return;
			}
			if (load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_WORKER, -1, -1, ((m_unit->SMCargoVolume() * 2) - 1), -1, MOVE_SAFE_TERRITORY))
			{
				return;
			}
		}
		else
		{
			if (load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, 1, -1, MOVE_SAFE_TERRITORY))
			{
				return;
			}
			if (load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_WORKER, -1, -1, 1, -1, MOVE_SAFE_TERRITORY))
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

	if (!m_unit->plot()->isOwned())
	{
		if (group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(1).maxPathTurns(1).allowRegrouping()))
		{
			return;
		}
	}

	if (guardCity(true))
	{
		return;
	}

	if (!bDanger)
	{
		if (group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(2).maxPathTurns(3).allowRegrouping()))
		{
			return;
		}
	}

	if (!m_unit->noDefensiveBonus())
	{
		if (guardFort(false))
		{
			return;
		}
	}

	if (guardCityAirlift())
	{
		return;
	}

	if (guardCity(false, true, 1))
	{
		return;
	}

	if (guardCitySite())
	{
		return;
	}

	if (!m_unit->noDefensiveBonus())
	{
		if (guardFort(true))
		{
			return;
		}

		if (guardBonus(15))
		{
			return;
		}
	}

	if (heal(30, 1))
	{
		return;
	}

	if (bDanger)
	{
		if (cityAttack(1, 55))
		{
			return;
		}

		if (anyAttack(1, 60))
		{
			return;
		}
	}

	if (!m_unit->noDefensiveBonus())
	{
		if (guardCity(false, false))
		{
			return;
		}
	}

	if (bDanger)
	{
		if (cityAttack(3, 45))
		{
			return;
		}

		if (anyAttack(3, 50))
		{
			return;
		}
	}

	if (protect(45, 8))
	{
		return;
	}

	if (guardCity(false, true, 3))
	{
		return;
	}

	if (defend())
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (m_unit->getGroup()->isStranded())
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (safety(3))
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::pillageCounterMove()
{
	PROFILE_FUNC();

	if (establishStackSeeInvisibleCoverage())
	{
		return;
	}
	//	For now
	counterMove();
}

void CvUnitDefaultAI::counterMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	if (checkSwitchToConstruct())
	{
		return;
	}

	if (MISSIONAI_REGROUP == m_unit->getGroup()->AI_getMissionAIType())
	{
		if (group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(2).maxPathTurns(1).allowRegrouping()))
		{
			return;
		}
		if (groupMergeRange(UNITAI_ATTACK_CITY, 1, false, true))
		{
			return;
		}
	}

	// Should never have group lead by counter unit
	if (m_unit->getGroup()->getNumUnits() > 1 && m_unit->getGroup()->getHeadUnitAI() == getUnitAIType() && m_unit->plot()->isCity() && m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		//FErrorMsg("error"); // just interested in when m_unit happens, not a problem
		m_unit->getGroup()->AI_separate(); // will change group
		return;
	}

	if (!m_unit->plot()->isOwned() && groupMergeRange(UNITAI_SETTLE, 2, true, false, false))
	{
		return;
	}

	if (guardCity(false, true, 1))
	{
		return;
	}

	// Super Forts begin *AI_defense*
	if (guardFortMinDefender(true))
	{
		return;
	}
	// Super Forts end

	if (m_unit->getSameTileHeal() + m_unit->getHealUnitCombatCount() > 0)
	{
		if (!m_unit->canAttack())
		{
			// Don't restrict to groups carrying cargo ... does m_unit apply to any units in standard bts anyway?
			if (shadow(UNITAI_ATTACK_CITY, -1, 21, false, false, 4))
			{
				return;
			}
		}
	}

	bool bDanger = (GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(m_unit->plot(), 3));
	AreaAITypes eAreaAIType = m_unit->area()->getAreaAIType(m_unit->getTeam());

	if (m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		if (!bDanger)
		{
			if (m_unit->plot()->isCity())
			{
				if ((eAreaAIType == AREAAI_ASSAULT) || (eAreaAIType == AREAAI_ASSAULT_ASSIST))
				{
					if (offensiveAirlift())
					{
						return;
					}
				}
			}

			if ((eAreaAIType == AREAAI_ASSAULT) || (eAreaAIType == AREAAI_ASSAULT_ASSIST) || (eAreaAIType == AREAAI_ASSAULT_MASSING))
			{
				if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK_CITY, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
				{
					return;
				}

				if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
				{
					return;
				}
			}
		}

		if (!m_unit->noDefensiveBonus())
		{
			if (guardCity(false, false))
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
	if (m_unit->plot()->getOwner() != m_unit->getOwner())
	{
		if (groupMergeRange(UNITAI_ATTACK_CITY, 1, true, true))
		{
			return;
		}
	}

	if (bDanger)
	{
		if (cityAttack(1, 35))
		{
			return;
		}

		if (anyAttack(1, 40))
		{
			return;
		}
	}

	bool bIgnoreFasterStacks = false;
	if (GET_PLAYER(m_unit->getOwner()).AI_isDoStrategy(AI_STRATEGY_LAND_BLITZ))
	{
		if (m_unit->area()->getAreaAIType(m_unit->getTeam()) != AREAAI_ASSAULT)
		{
			bIgnoreFasterStacks = true;
		}
	}

	if (group(GroupingParams().withUnitAI(UNITAI_ATTACK_CITY).maxOwnUnitAI(2).ignoreFaster(bIgnoreFasterStacks).ignoreOwnUnitType().stackOfDoom().maxPathTurns(6)))
	{
		return;
	}

	bool bFastMovers = (GET_PLAYER(m_unit->getOwner()).AI_isDoStrategy(AI_STRATEGY_FASTMOVERS));

	if (group(GroupingParams().withUnitAI(UNITAI_ATTACK).maxGroupSize(2).ignoreFaster(bFastMovers).ignoreOwnUnitType().stackOfDoom().maxPathTurns(5)))
	{
		return;
	}

	// BBAI TODO: merge with nearby pillage

	if (guardCity(false, true, 3))
	{
		return;
	}

	if (m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		if (!bDanger)
		{
			if ((eAreaAIType != AREAAI_DEFENSIVE))
			{
				if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK_CITY, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
				{
					return;
				}

				if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
				{
					return;
				}
			}
		}
	}

	if (heal())
	{
		return;
	}

	if (offensiveAirlift())
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (safety(3))
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::cityDefenseMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}
	if (checkSwitchToConstruct())
	{
		return;
	}

	if (isNegativePropertyUnit())
	{
		setUnitAIType(UNITAI_PILLAGE);
		return;
	}

	bool bDanger = (GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(m_unit->plot(), 3));

	if (MISSIONAI_REGROUP == m_unit->getGroup()->AI_getMissionAIType())
	{
		if (group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(5).maxOwnUnitAI(2).maxPathTurns(1).allowRegrouping()))
		{
			return;
		}
		if (groupMergeRange(UNITAI_ATTACK_CITY, 1, false, true))
		{
			return;
		}
	}
	else if (!m_unit->plot()->isOwned())
	{
		if (group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(5).maxOwnUnitAI(2).maxPathTurns(2).allowRegrouping()))
		{
			return;
		}
	}

	if (bDanger)
	{
		//	Never remove the last unit from the city
		if (m_unit->plot()->getPlotCity() == NULL || m_unit->plot()->plotCount(PUF_canDefend /*PUF_canDefendGroupHead*/, -1, -1, NULL, m_unit->getOwner()) != m_unit->getGroup()->getNumUnits()) // XXX check for other team's units?
		{
			if (leaveAttack(1, 70, 175))
			{
				return;
			}

			if (chokeDefend())
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
		//if (RbombardCity())
		//{
		//	return;
		//}
		// Dale - RB: Field Bombard END

	/************************************************************************************************/
	/* DCM									 END												  */
	/************************************************************************************************/
	if (guardCityBestDefender())
	{
		return;
	}

	if (!bDanger)
	{
		if (m_unit->plot()->getOwner() == m_unit->getOwner())
		{
			if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
			{
				if (load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, ((m_unit->SMCargoVolume() * 2) - 1), -1, MOVE_SAFE_TERRITORY, 1))
				{
					return;
				}
			}
			else if (load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, 1, -1, MOVE_SAFE_TERRITORY, 1))
			{
				return;
			}
		}
	}

	if (guardCityMinDefender(false))
	{
		return;
	}

	if (guardCity(true))
	{
		return;
	}

	//	No high priority actions to take so see if anyone requesting help
	if (processContracts())
	{
		return;
	}

	// Super Forts begin *AI_defense*
	if (guardFortMinDefender(true))
	{
		return;
	}
	// Super Forts end


	//TB: bolster national defenses please
	if (guardCityMinDefender(true))
	{
		return;
	}

	if (!bDanger)
	{
		if (group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(5).maxOwnUnitAI(2).maxPathTurns(2).allowRegrouping()))
		{
			return;
		}

		//if (group(UNITAI_SETTLE, /*iMaxGroup*/ 2, -1, -1, false, false, false, /*iMaxPath*/ 2, /*bAllowRegrouping*/ true))
		//{
		//	return;
		//}

		if (m_unit->plot()->getOwner() == m_unit->getOwner())
		{
			if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
			{
				if (load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, ((m_unit->SMCargoVolume() * 2) - 1), -1, MOVE_SAFE_TERRITORY))
				{
					return;
				}
			}
			else if (load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, 1, -1, MOVE_SAFE_TERRITORY))
			{
				return;
			}
		}
	}

	AreaAITypes eAreaAI = m_unit->area()->getAreaAIType(m_unit->getTeam());
	if ((eAreaAI == AREAAI_ASSAULT) || (eAreaAI == AREAAI_ASSAULT_MASSING) || (eAreaAI == AREAAI_ASSAULT_ASSIST))
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK_CITY, -1, -1, -1, 0, MOVE_SAFE_TERRITORY))
		{
			return;
		}
	}

	if ((getBirthmark() % 4) == 0)
	{
		if (guardFort())
		{
			return;
		}
	}

	if (guardCityAirlift())
	{
		return;
	}

	if (guardCity(false, true, 1))
	{
		return;
	}

	if (m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		if (load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, 3, -1, -1, -1, MOVE_SAFE_TERRITORY))
		{
			// will enter here if in danger
			return;
		}
	}

	//join any city attacks in progress
	if (m_unit->plot()->getOwner() != m_unit->getOwner())
	{
		if (groupMergeRange(UNITAI_ATTACK_CITY, 1, true, true))
		{
			return;
		}
	}

	if (guardCity(false, true))
	{
		return;
	}

	if (!m_unit->isNPC() && ((m_unit->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_OFFENSIVE) || (m_unit->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_MASSING)))
	{
		bool bIgnoreFaster = false;
		if (GET_PLAYER(m_unit->getOwner()).AI_isDoStrategy(AI_STRATEGY_LAND_BLITZ))
		{
			if (m_unit->area()->getAreaAIType(m_unit->getTeam()) != AREAAI_ASSAULT)
			{
				bIgnoreFaster = true;
			}
		}

		if (group(GroupingParams().withUnitAI(UNITAI_ATTACK_CITY).maxOwnUnitAI(2).minUnitAI(4).ignoreFaster(bIgnoreFaster)))
		{
			return;
		}
	}

	if (m_unit->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_ASSAULT)
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK_CITY, 2, -1, -1, 1, MOVE_SAFE_TERRITORY))
		{
			return;
		}
	}

	// Ok, we're not needed locally - try to group up with others nearby
	// if we're not already in a city (preparatory to us all moving to one)
	if (m_unit->plot()->getPlotCity() == NULL)
	{
		if (groupMergeRange(UNITAI_CITY_DEFENSE, 3, false, true) ||
			groupMergeRange(UNITAI_CITY_COUNTER, 3, false, true))
		{
			return;
		}

		if (retreatToCity())
		{
			return;
		}
	}

	if (m_unit->getGroup()->isStranded())
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (safety())
	{
		return;
	}

	//This should make it so that if the AI accepts that the city is going to be overwhelmed then it starts splitting its units to buy time for more production and reinforcements.
	if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS) && m_unit->plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			if (m_unit->canSplit())
			{
				m_unit->doSplit();
			}
		}
		else
		{
			m_unit->setInhibitMerge(false);
		}
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::cityDefenseExtraMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	if (isNegativePropertyUnit())
	{
		setUnitAIType(UNITAI_PILLAGE);
		return;
	}

	CvCity* pCity;
	bool bDanger = (GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(m_unit->plot(), 3));

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  09/18/09								jdog5000	  */
	/*																							  */
	/* Settler AI																				   */
	/************************************************************************************************/
	if (!(m_unit->plot()->isOwned()))
	{
		if (m_unit->getGroup()->getHeadUnitAI() == UNITAI_CITY_COUNTER)
		{
			if (group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(5).maxOwnUnitAI(2).maxPathTurns(4).allowRegrouping()))
			{
				return;
			}
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (leaveAttack(2, 55, 150))
	{
		return;
	}

	if (chokeDefend())
	{
		return;
	}
	/************************************************************************************************/
	/* DCM									 04/19/09								Johny Smith  */
	/************************************************************************************************/
		// RevolutionDCM - new field bombard ai
		// Dale - RB: Field Bombard START
		//if (RbombardCity())
		//{
		//	return;
		//}
		// Dale - RB: Field Bombard END

	/************************************************************************************************/
	/* DCM									 END												  */
	/************************************************************************************************/

	if (m_unit->getGroup()->getHeadUnitAI() == UNITAI_CITY_COUNTER)
	{
		if (group(GroupingParams().withUnitAI(UNITAI_SETTLE).maxGroupSize(5).maxOwnUnitAI(2).maxPathTurns(2).allowRegrouping()))
		{
			return;
		}
	}

	if (guardCityBestDefender())
	{
		return;
	}

	if (m_unit->getGroup()->getHeadUnitAI() == UNITAI_CITY_COUNTER)
	{
		if (!bDanger)
		{
			if (m_unit->plot()->getOwner() == m_unit->getOwner())
			{
				if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
				{
					if (load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, ((m_unit->SMCargoVolume() * 2) - 1), -1, MOVE_SAFE_TERRITORY, 1))
					{
						return;
					}
				}
				else if (load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, 1, -1, MOVE_SAFE_TERRITORY, 1))
				{
					return;
				}
			}
		}
	}

	pCity = m_unit->plot()->getPlotCity();

	if ((pCity != NULL) && (pCity->getOwner() == m_unit->getOwner())) // XXX check for other team?
	{
		int iMinLocalCounter = m_unit->plot()->plotCount(PUF_isUnitAIType, UNITAI_CITY_COUNTER, -1, NULL, m_unit->getOwner());
		if (iMinLocalCounter < 2)
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, NULL);
			m_unit->getGroup()->AI_setAsGarrison(pCity);
			return;
		}
	}

	//	No high priority actions to take so see if anyone requesting help
	if (processContracts())
	{
		return;
	}

	// Super Forts begin *AI_defense*
	if (guardFortMinDefender(true))
	{
		return;
	}

	if (guardCity(true))
	{
		return;
	}

	if ((getBirthmark() % 4) == 0)
	{
		if (guardFort(true))
		{
			return;
		}
	}
	// Super Forts end

	if (processContracts())
	{
		return;
	}

	if ((pCity != NULL) && (pCity->getOwner() == m_unit->getOwner())) // XXX check for other team?
	{
		if (m_unit->plot()->plotCount(PUF_canDefendGroupHead, -1, -1, NULL, m_unit->getOwner(), NO_TEAM, PUF_isUnitAIType, getUnitAIType()) == 1)
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	if (guardCityAirlift())
	{
		return;
	}

	if (guardCity(false, true, 1))
	{
		return;
	}

	if (m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		if (load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, 3, -1, -1, -1, MOVE_SAFE_TERRITORY, 3))
		{
			return;
		}
	}

	if (guardCity(false, true))
	{
		return;
	}

	//	Ok, we're not needed locally - try to group up with others nearby
	//	if we're not already in a city (preparatory to us all moving to one)
	if (m_unit->plot()->getPlotCity() == NULL)
	{
		if (groupMergeRange(UNITAI_CITY_COUNTER, 3, false, true) ||
			groupMergeRange(UNITAI_CITY_DEFENSE, 3, false, true))
		{
			return;
		}

		if (retreatToCity())
		{
			return;
		}
	}

	if (safety(3))
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::exploreMove()
{
	PROFILE_FUNC();

	if (groupSelectStatus())
	{
		return;
	}
	MissionAITypes eMissionAIType = MISSIONAI_GROUP;

	if (!m_unit->isHuman() && m_unit->plot()->getOwner() == m_unit->getOwner() && GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, &eMissionAIType, 1, m_unit->getGroup(), 1) > 0)
	{
		// Wait for units which are joining our group m_unit turn
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (!m_unit->isHuman() && m_unit->canAttack())
	{
		if (cityAttack(1, 60))
		{
			return;
		}

		if (anyAttack(1, 70))
		{
			OutputDebugString(CvString::format("%S (%d) chooses to attack\n", m_unit->getDescription().c_str(), m_unit->getID()).c_str());
			return;
		}
	}
	if (m_unit->plot()->getOwner() != m_unit->getOwner())
	{
		if (m_unit->getGroup()->countNumUnitAIType(UNITAI_SUBDUED_ANIMAL) > 0)
		{
			setUnitAIType(UNITAI_HUNTER_ESCORT);
			return;
		}
		foreach_(const CvUnit * unitX, m_unit->plot()->units())
		{
			if (unitX->getOwner() == m_unit->getOwner() && !unitX->getGroup()->canDefend() && unitX->getGroup()->countNumUnitAIType(UNITAI_SUBDUED_ANIMAL) > 0)
			{
				setUnitAIType(UNITAI_HUNTER_ESCORT);
				return;
			}
		}
	}

	if (m_unit->getDamage() > 0)
	{
		OutputDebugString(CvString::format("%S (%d) damaged at (%d,%d)...\n", m_unit->getDescription().c_str(), m_unit->getID(), m_unit->getX(), m_unit->getY()).c_str());
		//	If there is an adjacent enemy seek safety before we heal
		if (exposedToDanger(m_unit->plot(), 80))
		{
			OutputDebugString("	...plot is dangerous - seeking safety\n");
			if (safety())
			{
				return;
			}
		}

		if (heal())
		{
			OutputDebugString(CvString::format("	...healing at (%d,%d)\n", m_unit->getX(), m_unit->getY()).c_str());
			return;
		}
		else if (m_unit->getDamagePercent() > 25)
		{
			//	Look for somewhere safer
			if (safety(3))
			{
				return;
			}
		}
	}

	if (!m_unit->isHuman())
	{
		if (pillageRange(1))
		{
			return;
		}

		if (cityAttack(3, 80))
		{
			return;
		}
	}

	if (goody(4))
	{
		OutputDebugString(CvString::format("%S (%d) chooses to go for goody\n", m_unit->getDescription().c_str(), m_unit->getID()).c_str());
		return;
	}

	if (exploreRange(3))
	{
		return;
	}

	if (gUnitLogLevel > 2)
	{
		CvWString szString;
		getUnitAIString(szString, getUnitAIType());
		logBBAI("	%S's %S at (%d,%d) finds nothing nearby as exploration target", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getName(0).GetCString(), m_unit->getX(), m_unit->getY());
	}

	if (!m_unit->isHuman())
	{
		if (pillageRange(3))
		{
			return;
		}
	}

	if (explore())
	{
		return;
	}

	if (gUnitLogLevel > 2)
	{
		CvWString szString;
		getUnitAIString(szString, getUnitAIType());
		logBBAI("	%S's %S at (%d,%d) finds no exploration target", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getName(0).GetCString(), m_unit->getX(), m_unit->getY());
	}

	if (!m_unit->isHuman())
	{
		if (pillage())
		{
			return;
		}
	}

	if (!m_unit->isHuman())
	{
		if (travelToUpgradeCity())
		{
			return;
		}
	}

	if (refreshExploreRange(3))
	{
		return;
	}

	if (gUnitLogLevel > 2)
	{
		CvWString szString;
		getUnitAIString(szString, getUnitAIType());
		logBBAI("	%S's %S at (%d,%d) finds nothing at all to explore", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getName(0).GetCString(), m_unit->getX(), m_unit->getY());
	}

	if (!m_unit->isHuman() && getUnitAIType() == UNITAI_EXPLORE
	&& GET_PLAYER(m_unit->getOwner()).AI_totalAreaUnitAIs(m_unit->area(), UNITAI_EXPLORE) > GET_PLAYER(m_unit->getOwner()).AI_neededExplorers(m_unit->area())
	&& GET_PLAYER(m_unit->getOwner()).getFinalUnitUpkeep() > 0)
	{
		m_unit->scrap();
		return;
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  12/03/08								jdog5000	  */
	/*																							  */
	/* Unit AI																					  */
	/************************************************************************************************/
	if (!m_unit->isHuman() && m_unit->plot()->isCoastalLand() && GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, MISSIONAI_PICKUP) > 0)
	{
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return;
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (travelToUpgradeCity())
	{
		return;
	}

	if (patrol())
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  09/18/09								jdog5000	  */
	/*																							  */
	/* Unit AI																					  */
	/************************************************************************************************/
	if (m_unit->getGroup()->isStranded())
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::missionaryMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	if (spreadReligion())
	{
		return;
	}

	if (spreadCorporation())
	{
		return;
	}

	if (doInquisition())
	{
		return;
	}

	if (!m_unit->isHuman() || m_unit->isAutomated() && !GET_TEAM(m_unit->getTeam()).isAtWar())
	{
		if (!m_unit->isHuman() || (m_unit->getGameTurnCreated() < GC.getGame().getGameTurn()))
		{
			if (spreadReligionAirlift())
			{
				return;
			}
			if (spreadCorporationAirlift())
			{
				return;
			}
		}

		if (!m_unit->isHuman())
		{
			if (load(UNITAI_MISSIONARY_SEA, MISSIONAI_LOAD_SPECIAL, NO_UNITAI, -1, -1, -1, 0, MOVE_SAFE_TERRITORY))
			{
				return;
			}

			if (load(UNITAI_MISSIONARY_SEA, MISSIONAI_LOAD_SPECIAL, NO_UNITAI, -1, -1, -1, 0, MOVE_NO_ENEMY_TERRITORY))
			{
				return;
			}
		}
	}

	if (retreatToCity())
	{
		return;
	}

	if (m_unit->getGroup()->isStranded())
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (safety())
	{
		return;
	}
	m_unit->getGroup()->pushMission(MISSION_SKIP);
}

void CvUnitDefaultAI::hunterEscortMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	if (m_unit->getDamage() > 0)
	{
		OutputDebugString(CvString::format("%S (%d) damaged at (%d,%d)...\n", m_unit->getDescription().c_str(), m_unit->getID(), m_unit->getX(), m_unit->getY()).c_str());
		//	If there is an adjacent enemy seek safety before we heal
		if (exposedToDanger(m_unit->plot(), 80))
		{
			OutputDebugString("	...plot is dangerous - seeking safety\n");
			if (safety())
			{
				return;
			}
		}

		if (heal())
		{
			OutputDebugString(CvString::format("	...healing at (%d,%d)\n", m_unit->getX(), m_unit->getY()).c_str());
			return;
		}
		else if (m_unit->getDamagePercent() > 25)
		{
			//	Look for somewhere safer
			if (safety(3))
			{
				return;
			}
		}
	}

	//	If we have any animal hangers-on and are in our territory drop them off
	if (m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		if (m_unit->getGroup()->countNumUnitAIType(UNITAI_SUBDUED_ANIMAL) > 0)
		{
			m_unit->getGroup()->AI_separateAI(UNITAI_SUBDUED_ANIMAL);
		}
		if (m_unit->getGroup()->countNumUnitAIType(UNITAI_WORKER) > 0)
		{
			m_unit->getGroup()->AI_separateAI(UNITAI_WORKER);
		}
	}
	else if (!m_unit->isHuman())
	{
		if (groupMergeRange(UNITAI_SUBDUED_ANIMAL, 0, false, true, true))
		{
			return;
		}
		//	In the worker case we don't want to get sucked into a stack with an already protected
		//	worker, or one not also in non-owned territory so just search m_unit m_unit->plot
		if (group(GroupingParams().withUnitAI(UNITAI_WORKER).maxGroupSize(1).maxPathTurns(0)))
		{
			return;
		}

		if (m_unit->getGroup()->countNumUnitAIType(UNITAI_WORKER) + m_unit->getGroup()->countNumUnitAIType(UNITAI_SUBDUED_ANIMAL) > 0)
		{
			// If we have hangers-ons escort them back to our territory
			if (reachHome() || retreatToCity())
			{
				return;
			}
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	if (group(GroupingParams().withUnitAI(UNITAI_HUNTER).maxGroupSize(1).maxPathTurns(0)))
	{
		return;
	}

	if (processContracts())
	{
		return;
	}

	if (!m_unit->isHuman() && m_unit->plot()->isCoastalLand() && GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, MISSIONAI_PICKUP) > 0)
	{
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (m_contractualState == CONTRACTUAL_STATE_NO_WORK_FOUND
	&& GET_PLAYER(m_unit->getOwner()).AI_neededExplorers(m_unit->area()) > GET_PLAYER(m_unit->getOwner()).AI_totalAreaUnitAIs(m_unit->area(), UNITAI_EXPLORE))
	{
		setUnitAIType(UNITAI_EXPLORE);
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (m_unit->getGroup()->isStranded())
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (safety())
	{
		return;
	}
	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::prophetMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	if (construct(1))
	{
		return;
	}

	/*TB Prophet Mod begin*/
	if (foundReligion())
	{
		return;
	}
	/*TB Prophet Mod end*/

	if (discover(true))
	{
		return;
	}

	if (construct(3))
	{
		return;
	}

	int iGoldenAgeValue = (GET_PLAYER(m_unit->getOwner()).AI_calculateGoldenAgeValue() / (GET_PLAYER(m_unit->getOwner()).unitsRequiredForGoldenAge()));
	int iDiscoverValue = std::max(1, m_unit->getDiscoverResearch(NO_TECH));

	if (((iGoldenAgeValue * 100) / iDiscoverValue) > 60)
	{
		if (goldenAge())
		{
			return;
		}

		if (iDiscoverValue > iGoldenAgeValue)
		{
			if (discover())
			{
				return;
			}
			if (GET_PLAYER(m_unit->getOwner()).getUnitCount(m_unit->getUnitType()) > 1
			&& join())
			{
				return;
			}
		}
	}
	else if (discover() || join())
	{
		return;
	}

	if (GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(m_unit->plot(), 2) || m_unit->getGameTurnCreated() < GC.getGame().getGameTurn() - 25)
	{
		if (discover())
		{
			return;
		}
	}

	if (retreatToCity())
	{
		return;
	}

	if (m_unit->getGroup()->isStranded())
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::artistMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	if (artistCultureVictoryMove())
	{
		return;
	}

	if (construct())
	{
		return;
	}

	if (discover(true))
	{
		return;
	}

	if (greatWork())
	{
		return;
	}

	int iGoldenAgeValue = (GET_PLAYER(m_unit->getOwner()).AI_calculateGoldenAgeValue() / (GET_PLAYER(m_unit->getOwner()).unitsRequiredForGoldenAge()));
	int iDiscoverValue = std::max(1, m_unit->getDiscoverResearch(NO_TECH));

	if (((iGoldenAgeValue * 100) / iDiscoverValue) > 60)
	{
		if (goldenAge())
		{
			return;
		}

		if (iDiscoverValue > iGoldenAgeValue)
		{
			if (discover())
			{
				return;
			}
			if (GET_PLAYER(m_unit->getOwner()).getUnitCount(m_unit->getUnitType()) > 1
			&& join())
			{
				return;
			}
		}
	}
	else if (discover() || join())
	{
		return;
	}

	if (GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(m_unit->plot(), 2) || m_unit->getGameTurnCreated() < GC.getGame().getGameTurn() - 25)
	{
		if (discover())
		{
			return;
		}
	}

	if (retreatToCity())
	{
		return;
	}

	if (m_unit->getGroup()->isStranded())
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::scientistMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	if (discover(true))
	{
		return;
	}

	if (construct(MAX_INT, 1))
	{
		return;
	}
	if (GET_PLAYER(m_unit->getOwner()).getCurrentEra() < 3)
	{
		if (join(2))
		{
			return;
		}
	}

	if (GET_PLAYER(m_unit->getOwner()).getCurrentEra() <= (GC.getNumEraInfos() / 2))
	{
		if (construct())
		{
			return;
		}
	}

	int iGoldenAgeValue = (GET_PLAYER(m_unit->getOwner()).AI_calculateGoldenAgeValue() / (GET_PLAYER(m_unit->getOwner()).unitsRequiredForGoldenAge()));
	int iDiscoverValue = std::max(1, m_unit->getDiscoverResearch(NO_TECH));

	if (((iGoldenAgeValue * 100) / iDiscoverValue) > 60)
	{
		if (goldenAge())
		{
			return;
		}

		if (iDiscoverValue > iGoldenAgeValue)
		{
			if (discover())
			{
				return;
			}
			if (GET_PLAYER(m_unit->getOwner()).getUnitCount(m_unit->getUnitType()) > 1
			&& join())
			{
				return;
			}
		}
	}
	else if (discover() || join())
	{
		return;
	}


	if (GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(m_unit->plot(), 2) || m_unit->getGameTurnCreated() < GC.getGame().getGameTurn() - 25)
	{
		if (discover())
		{
			return;
		}
	}

	if (retreatToCity())
	{
		return;
	}

	if (m_unit->getGroup()->isStranded())
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::generalMove()
{
	PROFILE_FUNC();

	std::vector<UnitAITypes> aeUnitAITypes;
	int iDanger = GET_PLAYER(m_unit->getOwner()).AI_getPlotDanger(m_unit->plot(), 2);

	bool bOffenseWar = (m_unit->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_OFFENSIVE);
	bool bCanDefend = m_unit->getGroup()->canDefend();

	//	Generals always try to get an escort before moving away from a city
	if (!bCanDefend && m_contractsLastEstablishedTurn != GC.getGame().getGameTurn() && !m_unit->isCargo())
	{
		//	For now advertise the work as being here since we'll be holding position
		//	while we wait for a defender.  It would be more optimal to 'meet' the defender
		//	on the way or at the target m_unit->plot, but m_unit might leave us exposed on the way so
		//	the calculations involved are somewhat harder and not attempted for now
		if (gUnitLogLevel >= 3)
		{
			logBBAI("	Great General for player %d (%S) at (%d,%d) requesting an escort",
					m_unit->getOwner(),
					GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0),
					m_unit->getX(),
					m_unit->getY());
		}

		GET_PLAYER(m_unit->getOwner()).getContractBroker().advertiseWork(LOW_PRIORITY_ESCORT_PRIORITY, (unitCapabilities)(DEFENSIVE_UNITCAPABILITIES | OFFENSIVE_UNITCAPABILITIES), m_unit->getX(), m_unit->getY(), m_unit);

		m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
		m_contractualState = CONTRACTUAL_STATE_AWAITING_ANSWER;

		//	Make sure we stay in charge when our escort joins us
		setLeaderPriority(LEADER_PRIORITY_MAX);
	}
	/************************************************************************************************/
	// Afforess 06/5/10		Coded By: KillMePlease
	// Great Commanders
	/************************************************************************************************/
		//bool bDefensiveWar = (m_unit->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_DEFENSIVE);
	if (m_unit->isCommander())
	{
		//	Koshling - we cannot both advertise ourselves as available AND request help in the same turn, so alternate between
		//	for now (if we try to and happen to be last in the unit processing sequence out eventual request
		//	can wind up not being processed by any units so always forces a city build request - when built that unit may also
		//	not join us for exactly the same reason)
		bool bLookForWork = ((GC.getGame().getGameTurn() % 2 == 0) && m_unit->getLevel() < 10);

		//	If we're reasonably trained up then break off for any request, otherwise
		//	keep trying to train up unless there are high priority requests
		if (bLookForWork)
		{
			if (bCanDefend && processContracts((m_unit->getLevel() < 4 ? LOW_PRIORITY_ESCORT_PRIORITY + 1 : 0)))
			{
				if (gUnitLogLevel >= 3 && m_contractualState == CONTRACTUAL_STATE_FOUND_WORK)
				{
					logBBAI("	Great General for player %d (%S) at (%d,%d) found contractual work",
							m_unit->getOwner(),
							GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0),
							m_unit->getX(),
							m_unit->getY());
				}
				return;
			}

			if (gUnitLogLevel >= 3)
			{
				logBBAI("	Great General for player %d (%S) at (%d,%d) found no urgent contractual work",
						m_unit->getOwner(),
						GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0),
						m_unit->getX(),
						m_unit->getY());
			}
		}

		if (bOffenseWar && m_unit->getLevel() >= 4)
		{
			//try to join SoD (?)
			if (group(GroupingParams().withUnitAI(UNITAI_ATTACK).maxOwnUnitAI(1).minUnitAI(6).stackOfDoom().maxPathTurns(bCanDefend ? MAX_INT : 1).allowRegrouping()))
			{
				return;
			}
			if (group(GroupingParams().withUnitAI(UNITAI_ATTACK).maxOwnUnitAI(1).minUnitAI(4).stackOfDoom().maxPathTurns(bCanDefend ? MAX_INT : 1).allowRegrouping()))
			{
				return;
			}
			if (group(GroupingParams().withUnitAI(UNITAI_ATTACK_CITY).maxOwnUnitAI(1).minUnitAI(4).stackOfDoom().maxPathTurns(bCanDefend ? MAX_INT : 1).allowRegrouping()))
			{
				return;
			}
			if (group(GroupingParams().withUnitAI(UNITAI_COLLATERAL).maxOwnUnitAI(1).minUnitAI(4).stackOfDoom().maxPathTurns(bCanDefend ? MAX_INT : 1).allowRegrouping()))
			{
				return;
			}
			if (group(GroupingParams().withUnitAI(UNITAI_COUNTER).maxOwnUnitAI(1).minUnitAI(3).stackOfDoom().maxPathTurns(bCanDefend ? MAX_INT : 1).allowRegrouping()))
			{
				return;
			}
			//Koshling - remove attempts to join small stacks now that we have generals requesting escorts and
			//responding to explicit requests themselves
			//try to join attacking stack
			//if (group(UNITAI_ATTACK, -1, 1, -1, false, false, false, bCanDefend ? MAX_INT : 1))
			//{
			//	return;
			//}

			//if (group(UNITAI_ATTACK_CITY, -1, 1, -1, false))
			//{
			//	return;
			//}
			//try airlift
			if (offensiveAirlift())
			{
				return;
			}
			//try load on transport
			if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
			{
				if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, (2 * GET_PLAYER(m_unit->getOwner()).getBestUnitTypeCargoVolume(UNITAI_ASSAULT_SEA)), 0, ((bCanDefend ? MOVE_SAFE_TERRITORY : MOVE_OUR_TERRITORY) | MOVE_WITH_CAUTION)))
				{
					return;
				}
			}
			else if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, 2, -1, 0, ((bCanDefend ? MOVE_SAFE_TERRITORY : MOVE_OUR_TERRITORY) | MOVE_WITH_CAUTION)))
			{
				return;
			}
		}

		//	Try to hook up with a hunter and exp up - we always seek to have a stack of at least one
		//	general escort unit, the hunter, and the general
		bool bHasHunter = m_unit->getGroup()->hasUnitOfAI(UNITAI_HUNTER);

		if (bHasHunter)
		{
			if (m_unit->getGroup()->getNumUnits() > 2)
			{
				if (gUnitLogLevel > 2)
				{
					logBBAI("	%S's great commander (%d) at (%d,%d) [stack size %d, exp %d] executes hunter AI to "
						"exp up", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getID(), 
						m_unit->getX(), m_unit->getY(), m_unit->getGroup()->getNumUnits(), m_unit->getExperience());
				}
				// If is high enough level, stop m_unit behavior, get back to a city and release the hunter then become unavailable for m_unit sort of assignment.
				if (m_unit->getLevel() >= 10)
				{
					if (m_unit->plot()->isCity(false, m_unit->getTeam()))
					{
						m_unit->getGroup()->AI_makeForceSeparate();
					}
					else if (moveIntoNearestOwnedCity())
					{
						return;
					}
				}
				//	Already have hunter + another escort =- execute modified hunter AI
				SearchAndDestroyMove(true);
				return;
			}
			else if (!bLookForWork && !m_unit->isCargo())
			{
				//	Get another escort unit
				GET_PLAYER(m_unit->getOwner()).getContractBroker().advertiseWork(

					HIGHEST_PRIORITY_ESCORT_PRIORITY,
					NO_UNITCAPABILITIES,
					m_unit->getX(), m_unit->getY(), m_unit
				);

				if (gUnitLogLevel > 2)
				{
					logBBAI("	%S's great commander (%d) at (%d,%d) requests escort at priority %d", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getID(), m_unit->getX(), m_unit->getY(), HIGH_PRIORITY_ESCORT_PRIORITY);
				}
			}
		}
		else if (!bLookForWork && !m_unit->isCargo())
		{
			GET_PLAYER(m_unit->getOwner()).getContractBroker().advertiseWork(

				HIGHEST_PRIORITY_ESCORT_PRIORITY,
				NO_UNITCAPABILITIES,
				m_unit->getX(), m_unit->getY(), m_unit,
				UNITAI_HUNTER
			);

			if (gUnitLogLevel > 2)
			{
				logBBAI("	%S's great commander (%d) at (%d,%d) [stack size %d] requests hunter at priority %d", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getID(), m_unit->getX(), m_unit->getY(), m_unit->getGroup()->getNumUnits(), HIGH_PRIORITY_ESCORT_PRIORITY);
			}
		}

		if (retreatToCity())
		{
			return;
		}
		if (safety())
		{
			return;
		}
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return;

	}
	else
	{
		if (command())
		{
			//Does not use it's turn
			generalMove();
			return;
		}
	}
	/************************************************************************************************/
	// ! Great Commanders
	/************************************************************************************************/
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD & RevDCM					 09/03/10						jdog5000	  */
	/*																				phungus420	*/
	/* Great People AI, Unit AI																	 */
	/************************************************************************************************/
	if (leadLegend())
	{
		return;
	}

	if (iDanger > 0)
	{
		aeUnitAITypes.clear();
		aeUnitAITypes.push_back(UNITAI_ATTACK);
		aeUnitAITypes.push_back(UNITAI_COUNTER);
		if (lead(aeUnitAITypes))
		{
			return;
		}
	}

	if (joinMilitaryCity())
	{
		return;
	}


	if (construct(1))
	{
		return;
	}
	if (join(1))
	{
		return;
	}

	if (bOffenseWar && (getBirthmark() % 2 == 0))
	{
		aeUnitAITypes.clear();
		aeUnitAITypes.push_back(UNITAI_ATTACK_CITY);
		if (lead(aeUnitAITypes))
		{
			return;
		}

		aeUnitAITypes.clear();
		aeUnitAITypes.push_back(UNITAI_ATTACK);
		if (lead(aeUnitAITypes))
		{
			return;
		}
	}

	if (join(2))
	{
		return;
	}

	if (construct(2))
	{
		return;
	}
	if (join(4))
	{
		return;
	}

	if (GC.getGame().getSorenRandNum(3, "AI General Construct") == 0)
	{
		if (construct())
		{
			return;
		}
	}

	if (join())
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (m_unit->getGroup()->isStranded())
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::greatHunterMove()
{
	PROFILE_FUNC();

	if (groupSelectStatus())
	{
		return;
	}

	std::vector<UnitAITypes> aeUnitAITypes;

	int iGHrandom = GC.getGame().getSorenRandNum(5, "AI Great Hunter Decision");

	if (iGHrandom == 0 && leadLegend())
	{
		return;
	}

	if (iGHrandom < 2)
	{
		upgrade();
		if (m_unit->isDelayedDeath())
		{
			//	Upgrade of original unit was successful
			return;
		}
	}

	if (iGHrandom < 4)
	{
		if (joinMilitaryCity())
		{
			return;
		}
	}

	if (iGHrandom < 5)
	{
		aeUnitAITypes.clear();
		aeUnitAITypes.push_back(UNITAI_HUNTER);
		aeUnitAITypes.push_back(UNITAI_EXPLORE);
		if (lead(aeUnitAITypes))
		{
			return;
		}
	}

	if (construct(1))
	{
		return;
	}
	if (join(1))
	{
		return;
	}

	if (join(2))
	{
		return;
	}

	if (construct(2))
	{
		return;
	}
	if (join(4))
	{
		return;
	}

	if (construct())
	{
		return;
	}

	if (join())
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (m_unit->getGroup()->isStranded())
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitDefaultAI::greatAdmiralMove()
{
	PROFILE_FUNC();

	std::vector<UnitAITypes> aeUnitAITypes;

	int iGArandom = GC.getGame().getSorenRandNum(4, "AI Great Admiral Decision");

	if (iGArandom == 0)
	{
		aeUnitAITypes.clear();
		aeUnitAITypes.push_back(UNITAI_ATTACK_SEA);
		aeUnitAITypes.push_back(UNITAI_RESERVE_SEA);
		aeUnitAITypes.push_back(UNITAI_CARRIER_SEA);
		if (lead(aeUnitAITypes))
		{
			return;
		}
	}

	if (iGArandom < 2)
	{
		upgrade();
		if (m_unit->isDelayedDeath())
		{
			//	Upgrade of original unit was successful
			return;
		}
	}

	if (iGArandom < 4)
	{
		if (joinMilitaryCity(true))
		{
			return;
		}
	}

	if (construct(1))
	{
		return;
	}

	if (construct(2))
	{
		return;
	}

	if (construct())
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (m_unit->getGroup()->isStranded())
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::merchantMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	if (construct())
	{
		return;
	}

	if (discover(true))
	{
		return;
	}

	if (caravan(false))
	{
		return;
	}
	if (caravan(true))
	{
		return;
	}

	int iGoldenAgeValue = (GET_PLAYER(m_unit->getOwner()).AI_calculateGoldenAgeValue() / (GET_PLAYER(m_unit->getOwner()).unitsRequiredForGoldenAge()));
	int iDiscoverValue = std::max(1, m_unit->getDiscoverResearch(NO_TECH));

	if (trade(iGoldenAgeValue * 2))
	{
		return;
	}

	if (((iGoldenAgeValue * 100) / iDiscoverValue) > 60)
	{
		if (goldenAge())
		{
			return;
		}

		if (trade(iGoldenAgeValue))
		{
			return;
		}

		if (iDiscoverValue > iGoldenAgeValue)
		{
			if (discover())
			{
				return;
			}
			if (GET_PLAYER(m_unit->getOwner()).getUnitCount(m_unit->getUnitType()) > 1
			&& join())
			{
				return;
			}
		}
	}
	else if (discover() || join())
	{
		return;
	}


	if (GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(m_unit->plot(), 2) || m_unit->getGameTurnCreated() < GC.getGame().getGameTurn() - 25)
	{
		if (discover())
		{
			return;
		}
	}

	if (retreatToCity())
	{
		return;
	}

	if (m_unit->getGroup()->isStranded())
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitDefaultAI::subduedAnimalMove()
{
	PROFILE_FUNC();

	//	Don't bother healing subdued animals in our own territory at least until after we test if they can construct
	if (m_unit->plot()->getOwner() != m_unit->getOwner())
	{
		// If they can get to our territory prefer that to healing
		if (moveToOurTerritory(1))
		{
			return;
		}

		// Try to move to a nearby hunter unit if one is available to group with it
		if (groupMergeRange(UNITAI_HUNTER, 1, false, true, true)
		// Try to move to a nearby hunter escort unit if one is available to group with it
		||  groupMergeRange(UNITAI_HUNTER_ESCORT, 1, false, true, true)
		// Try to move to a nearby explorer unit if one is available to group with it
		||  groupMergeRange(UNITAI_EXPLORE, 1, false, true, true))
		{
			return;
		}

		//	If there is an adjacent enemy seek safety before we heal
		if (exposedToDanger(m_unit->plot(), 80))
		{
			OutputDebugString("	...plot is dangerous - seeking safety\n");

			if (safety())
			{
				return;
			}
		}

		//	Failing that are there other animals nearby - safety in numbers
		if (groupMergeRange(UNITAI_SUBDUED_ANIMAL, 1, false, true, true))
		{
			return;
		}

		if (m_unit->getDamage() > 0)
		{
			OutputDebugString(CvString::format("%S (%d) damaged (%d) at (%d,%d)...\n", m_unit->getDescription().c_str(), m_unit->getID(), m_unit->getDamage(), m_unit->getX(), m_unit->getY()).c_str());
			if (heal())
			{
				OutputDebugString("	...healing\n");
				return;
			}
		}

		if (safety())
		{
			return;
		}
	}
	else if (m_unit->getGroup()->getNumUnits() > 1)
	{
		//	Separate groups of subdued animals once they reach owned territory
		m_unit->getGroup()->AI_separate();

		//	Will have changed group so the previous one no longer exists to deal with
		return;
	}

	if (construct(MAX_INT, MAX_INT, 0, false, true))
	{
		OutputDebugString(CvString::format("%S (%d) chooses to head off to construct\n", m_unit->getDescription().c_str(), m_unit->getID()).c_str());
		return;
	}

	if (outcomeMission())
	{
		OutputDebugString(CvString::format("%S (%d) chooses to head off to do an outcome mission\n", m_unit->getDescription().c_str(), m_unit->getID()).c_str());
		return;
	}

	upgrade();

	if (m_unit->isDelayedDeath())
	{
		//	Upgrade of original unit was successful
		return;
	}

	if (scrapSubdued())
	{
		OutputDebugString(CvString::format("%S (%d) chooses to disband\n", m_unit->getDescription().c_str(), m_unit->getID()).c_str());
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  09/18/09								jdog5000	  */
	/*																							  */
	/* Unit AI																					  */
	/************************************************************************************************/
	if (m_unit->getGroup()->isStranded())
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitDefaultAI::engineerMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	if (construct())
	{
		OutputDebugString(CvString::format("%S (%d) chooses to head off to construct\n", m_unit->getDescription().c_str(), m_unit->getID()).c_str());
		return;
	}

	if (switchHurry())
	{
		return;
	}

	if (hurry())
	{
		return;
	}

	if (discover(true))
	{
		return;
	}

	int iGoldenAgeValue = (GET_PLAYER(m_unit->getOwner()).AI_calculateGoldenAgeValue() / (GET_PLAYER(m_unit->getOwner()).unitsRequiredForGoldenAge()));
	int iDiscoverValue = std::max(1, m_unit->getDiscoverResearch(NO_TECH));

	if (((iGoldenAgeValue * 100) / iDiscoverValue) > 60)
	{
		if (goldenAge())
		{
			return;
		}

		if (iDiscoverValue > iGoldenAgeValue)
		{
			if (discover())
			{
				return;
			}
			if (GET_PLAYER(m_unit->getOwner()).getUnitCount(m_unit->getUnitType()) > 1
			&& join())
			{
				return;
			}
		}
	}
	else if (discover() || join())
	{
		return;
	}


	if (GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(m_unit->plot(), 2) || m_unit->getGameTurnCreated() < GC.getGame().getGameTurn() - 25)
	{
		if (discover())
		{
			return;
		}
	}

	if (retreatToCity())
	{
		return;
	}

	if (m_unit->getGroup()->isStranded())
	{
		if (load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_NO_ENEMY_TERRITORY, 1))
		{
			return;
		}
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::spyMove()
{
	PROFILE_FUNC();

	CvTeamAI& kTeam = GET_TEAM(m_unit->getTeam());
	int iEspionageChance = 0;
	if (m_unit->plot()->isOwned() && m_unit->plot()->getTeam() != m_unit->getTeam())
	{
		switch (GET_PLAYER(m_unit->getOwner()).AI_getAttitude(m_unit->plot()->getOwner()))
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

		const WarPlanTypes eWarPlan = kTeam.AI_getWarPlan(m_unit->plot()->getTeam());

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

		if (m_unit->plot()->isCity() && m_unit->plot()->getTeam() != m_unit->getTeam())
		{
			bool bTargetCity = false;

			// would we have more power if enemy defenses were down?
			int iOurPower = GET_PLAYER(m_unit->getOwner()).AI_getOurPlotStrength(m_unit->plot(), 1, false, true);
			int iEnemyPower = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 0, false, false);

			if (5 * iOurPower > 6 * iEnemyPower && eWarPlan != NO_WARPLAN)
			{
				bTargetCity = true;

				if (revoltCitySpy())
				{
					return;
				}

				if (GC.getGame().getSorenRandNum(5, "AI Spy Skip Turn") > 0)
				{
					m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_ATTACK_SPY);
					return;
				}

				if (cityOffenseSpy(5, m_unit->plot()->getPlotCity()))
				{
					return;
				}
			}

			if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(m_unit->plot(), MISSIONAI_ASSAULT, m_unit->getGroup()) > 0)
			{
				bTargetCity = true;

				m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_ATTACK_SPY);
				return;
			}

			if (!bTargetCity)
			{
				// normal city handling
				if (m_unit->getFortifyTurns() >= GC.getMAX_FORTIFY_TURNS())
				{
					if (espionageSpy())
					{
						return;
					}
				}
				else if (GC.getGame().getSorenRandNum(100, "AI Spy Skip Turn") > 5)
				{
					// don't get stuck forever
					m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_ATTACK_SPY);
					return;
				}
			}
		}
		else if (iEspionageChance > 99 || GC.getGame().getSorenRandNum(100, "AI Spy Espionage") < iEspionageChance)
		{
			// This applies only when not in an enemy city, so for destroying improvements
			if (espionageSpy())
			{
				return;
			}
		}
	}

	if (m_unit->plot()->getTeam() == m_unit->getTeam())
	{
		if (!kTeam.hasWarPlan(true)
		|| GET_PLAYER(m_unit->getOwner()).AI_isDoVictoryStrategy(AI_VICTORY_SPACE4)
		|| GET_PLAYER(m_unit->getOwner()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3))
		{
			if (GC.getGame().getSorenRandNum(10, "AI Spy defense") > 0)
			{
				if (guardSpy(0))
				{
					return;
				}
			}
		}

		if (GC.getGame().getSorenRandNum(100, "AI Spy pillage improvement") < 25)
		{
			if (bonusOffenseSpy(5))
			{
				return;
			}
		}
		else if (cityOffenseSpy(10))
		{
			return;
		}
	}

	if (iEspionageChance > 0 && (m_unit->plot()->isCity() || (m_unit->plot()->getNonObsoleteBonusType(m_unit->getTeam()) != NO_BONUS)))
	{
		if (GC.getGame().getSorenRandNum(7, "AI Spy Skip Turn") > 0)
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_ATTACK_SPY);
			return;
		}
	}

	if (m_unit->area()->getNumCities() > m_unit->area()->getCitiesPerPlayer(m_unit->getOwner()))
	{
		if (GC.getGame().getSorenRandNum(4, "AI Spy Choose Movement") > 0)
		{
			if (reconSpy(3))
			{
				return;
			}
		}
		else if (cityOffenseSpy(10))
		{
			return;
		}
	}

	if (load(UNITAI_SPY_SEA, MISSIONAI_LOAD_SPECIAL, NO_UNITAI, -1, -1, -1, 0, MOVE_NO_ENEMY_TERRITORY))
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::ICBMMove()
{
	PROFILE_FUNC();

	if (m_unit->airRange() > 0)
	{
		if (nukeRange(m_unit->airRange()))
		{
			return;
		}
	}
	else if (nuke())
	{
		return;
	}

	if (m_unit->isCargo())
	{
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (m_unit->airRange() > 0)
	{
		if (m_unit->plot()->isCity(true))
		{
			const int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
			const int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

			if (4 * iEnemyOffense > iOurDefense || iOurDefense == 0)
			{
				// Too risky, pull back
				if (airOffensiveCity())
				{
					return;
				}
			}
		}

		if (missileLoad(UNITAI_MISSILE_CARRIER_SEA, 2, true)
		|| missileLoad(UNITAI_MISSILE_CARRIER_SEA, 1, false))
		{
			return;
		}
		if ((getBirthmark() % 3) == 0 && missileLoad(UNITAI_ATTACK_SEA, 0, false))
		{
			return;
		}
		if (airOffensiveCity())
		{
			return;
		}
	}
	m_unit->getGroup()->pushMission(MISSION_SKIP);
}


void CvUnitDefaultAI::workerSeaMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	if (!m_unit->getGroup()->canDefend() && GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(m_unit->plot()))
	{
		if (retreatToCity())
		{
			return;
		}
	}

	if (improveBonus(20))
	{
		return;
	}

	if (improveBonus(10))
	{
		return;
	}

	if (improveBonus())
	{
		return;
	}

	if (improveLocalPlot(2, NULL))
	{
		return;
	}

	if (!m_unit->isHuman() && getUnitAIType() == UNITAI_WORKER_SEA)
	{
		const CvCity* pCity = m_unit->plot()->getPlotCity();

		if (pCity && pCity->getOwner() == m_unit->getOwner()
		&& (pCity->AI_neededSeaWorkers() == 0
			|| GET_PLAYER(m_unit->getOwner()).AI_isFinancialTrouble()
			&& GET_PLAYER(m_unit->getOwner()).getUnitUpkeepNet(m_unit->isMilitaryBranch(), m_unit->getUpkeep100()) > 0))
		{
			//Probably icelocked since it can't perform actions.
			m_unit->scrap();
			return;
		}
	}

	if (retreatToCity())
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::barbAttackSeaMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	// Less suicide, always chase good targets
	if (anyAttack(2, 51))
	{
		return;
	}

	if (pillageRange(1))
	{
		return;
	}

	if (anyAttack(1, 34))
	{
		return;
	}

	// We're easy to take out if wounded
	if (heal())
	{
		return;
	}

	if (pillageRange(3))
	{
		return;
	}

	// Barb ships will often hang out for a little while blockading before moving on
	if ((GC.getGame().getGameTurn() + m_unit->getID()) % 12 > 5)
	{
		if (pirateBlockade())
		{
			return;
		}
	}

	if (GC.getGame().getSorenRandNum(3, "AI Check trapped") == 0)
	{
		// If trapped in small hole in ice or around tiny island, disband to allow other units to be generated
		bool bScrap = true;
		const int iMaxRange = m_unit->baseMoves() + 2;
		foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iMaxRange, iMaxRange))
		{
			if (plotValid(pLoopPlot))
			{
				int iPathTurns;
				if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns))
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
			m_unit->scrap();
		}
	}

	if (patrol())
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::pirateSeaMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	CvArea* pWaterArea;

	// heal in defended, unthreatened forts and cities
	if (m_unit->plot()->isCity(true) && (GET_PLAYER(m_unit->getOwner()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false) > 0) && !(GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(m_unit->plot(), 2, false)))
	{
		if (heal())
		{
			return;
		}
	}

	if (m_unit->plot()->isOwned() && (m_unit->plot()->getTeam() == m_unit->getTeam()))
	{
		if (anyAttack(2, 40))
		{
			return;
		}

		//if (protect(30))
		if (protect(40, 3))
		{
			return;
		}


		if (((getBirthmark() / 8) % 2) == 0)
		{
			// Previously code actually blocked grouping
			if (group(GroupingParams().withUnitAI(UNITAI_PIRATE_SEA).maxOwnUnitAI(1).ignoreFaster().maxPathTurns(8)))
			{
				return;
			}
		}
	}
	else
	{
		if (anyAttack(2, 51))
		{
			return;
		}
	}


	if (GC.getGame().getSorenRandNum(10, "AI Pirate Explore") == 0)
	{
		pWaterArea = m_unit->plot()->waterArea();

		if (pWaterArea != NULL)
		{
			if (pWaterArea->getNumUnrevealedTiles(m_unit->getTeam()) > 0)
			{
				if (GET_PLAYER(m_unit->getOwner()).AI_areaMissionAIs(pWaterArea, MISSIONAI_EXPLORE, m_unit->getGroup()) < (GET_PLAYER(m_unit->getOwner()).AI_neededExplorers(pWaterArea)))
				{
					if (exploreRange(2))
					{
						return;
					}
				}
			}
		}
	}

	if (GC.getGame().getSorenRandNum(11, "AI Pirate Pillage") == 0)
	{
		if (pillageRange(1))
		{
			return;
		}
	}

	//Includes heal and retreat to sea routines.
	if (pirateBlockade())
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::attackSeaMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	if (m_unit->plot()->isCity(true))
	{
		PROFILE("CvUnitDefaultAI::attackSeaMove.City");

		int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

		if (m_unit->getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			if (anyAttack(2, 50))
			{
				return;
			}

			if (shadow(UNITAI_ASSAULT_SEA, 4, 34, false, true, 2))
			{
				return;
			}

			if (protect(35, 3))
			{
				return;
			}

			if (retreatToCity())
			{
				return;
			}

			if (safety())
			{
				return;
			}
		}
	}

	if (heal(30, 1))
	{
		return;
	}

	// RevolutionDCM - sea bombard AI formally DCM 1.7 RbombardCity()
	// Dale - RB: Field Bombard START
	//if (RbombardCity())
	//{
	//	return;
	//}
	//if (RbombardNaval())
	//{
	//	return;
	//}
	// Dale - RB: Field Bombard END

	{
		PROFILE("CvUnitDefaultAI::attackSeaMove.BasicAttacks");

		if (anyAttack(1, 35))
		{
			return;
		}

		if (seaBombardRange(1))
		{
			return;
		}

		if (anyAttack(2, 40))
		{
			return;
		}

		if (goody(2))
		{
			return;
		}

		if (seaBombardRange(6))
		{
			return;
		}

		if (heal(50, 3))
		{
			return;
		}

		if (heal())
		{
			return;
		}
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  08/10/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
		// BBAI TODO: Turn m_unit into a function, have docked escort ships do it to

		//Fuyu: search for more attackers, and when enough are found, always try to break through
	CvCity* pCity = m_unit->plot()->getPlotCity();

	if (pCity != NULL)
	{
		if (pCity->isBlockaded())
		{
			PROFILE("CvUnitDefaultAI::attackSeaMove.Blockaded");

			const int iBlockadeRange = GC.getSHIP_BLOCKADE_RANGE();
			// City under blockade
			// Attacker has low odds since anyAttack checks above passed, try to break if sufficient numbers

			int iAttackers = m_unit->plot()->plotCount(PUF_isUnitAIType, UNITAI_ATTACK_SEA, -1, NULL, NO_PLAYER, m_unit->getTeam(), PUF_isGroupHead, -1, -1);
			int iBlockaders = GET_PLAYER(m_unit->getOwner()).AI_getWaterDanger(m_unit->plot(), (iBlockadeRange + 1));
			//bool bBreakBlockade = (iAttackers > (iBlockaders + 2) || iAttackers >= 2*iBlockaders);

			const int iMaxRange = iBlockadeRange - 1;
			if (gUnitLogLevel > 2) logBBAI("	  Not enough attack fleet found in %S, searching for more in a %d-tile radius", pCity->getName().GetCString(), iMaxRange);

			foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iMaxRange, iMaxRange))
			{
				if (pLoopPlot->isWater())
				{
					if (pLoopPlot->getBlockadedCount(m_unit->getTeam()) > 0)
					{
						iAttackers += pLoopPlot->plotCount(PUF_isUnitAIType, UNITAI_ATTACK_SEA, -1, NULL, NO_PLAYER, m_unit->getTeam(), PUF_isGroupHead, -1, -1);
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
					if (m_unit->baseMoves() >= iBlockadeRange)
					{
						if (anyAttack(1, 15))
						{
							return;
						}
					}
					else
					{
						//Fuyu: Even slow ships should attack
						//Assuming that every ship can reach a blockade with 2 moves
						if (anyAttack(2, 15))
						{
							return;
						}
					}

					//If no mission was pushed yet and we have a lot of ships, try again with even lower odds
					if (iAttackers > 2 * iBlockaders)
					{
						if (anyAttack(1, 10))
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
	if (!m_unit->isHuman() && !m_unit->isNPC())
	{
		if (establishStackSeeInvisibleCoverage())
		{
			return;
		}
	}

	if (group(GroupingParams().withUnitAI(UNITAI_CARRIER_SEA).maxGroupSize(4).maxOwnUnitAI(1).ignoreFaster().maxPathTurns(5)))
	{
		return;
	}

	if (group(GroupingParams().withUnitAI(UNITAI_ATTACK_SEA).maxGroupSize(1).ignoreFaster().maxPathTurns(3)))
	{
		return;
	}

	if (!m_unit->plot()->isOwned() || !m_unit->isEnemy(m_unit->plot()->getTeam()))
	{
		PROFILE("CvUnitDefaultAI::attackSeaMove.Neutral");

		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD					  01/11/09								jdog5000	  */
		/*																							  */
		/* Naval AI																					 */
		/************************************************************************************************/
		/* original bts code
				if (shadow(UNITAI_ASSAULT_SEA, 4, 34))
				{
					return;
				}

				if (shadow(UNITAI_CARRIER_SEA, 4, 51))
				{
					return;
				}

				if (group(UNITAI_ASSAULT_SEA, -1, 4, -1, false, false, false))
				{
					return;
				}
			}

			if (group(UNITAI_CARRIER_SEA, -1, 1, -1, false, false, false))
			{
				return;
			}
		*/
		if (shadow(UNITAI_ASSAULT_SEA, 4, 34, true, false, 4))
		{
			return;
		}

		if (shadow(UNITAI_CARRIER_SEA, 4, 51, true, false, 5))
		{
			return;
		}

		// Group with large flotillas first
		if (group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).maxOwnUnitAI(4).minUnitAI(3).maxPathTurns(3).withCargoOnly()))
		{
			return;
		}

		if (group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).maxOwnUnitAI(2).maxPathTurns(5).withCargoOnly()))
		{
			return;
		}
	}

	if (group(GroupingParams().withUnitAI(UNITAI_CARRIER_SEA).maxOwnUnitAI(1).maxPathTurns(10)))
	{
		return;
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/


	if (m_unit->plot()->isOwned() && (m_unit->isEnemy(m_unit->plot()->getTeam())))
	{
		if (blockade())
		{
			return;
		}
	}
	if (pillageRange(4))
	{
		return;
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  09/01/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
		//if (protect(35))
	if (protect(35, 3))
	{
		return;
	}

	if (protect(35, 8))
		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD					   END												  */
		/************************************************************************************************/
	{
		return;
	}

	if (travelToUpgradeCity())
	{
		return;
	}

	if (seaAreaAttack())
	{
		return;
	}

	if (patrol())
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::reserveSeaMove()
{
	PROFILE_FUNC();

	if (m_unit->plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

		if (m_unit->getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0)  //prioritize getting outta there
		{
			if (anyAttack(2, 60))
			{
				return;
			}

			//if (protect(40))
			if (protect(40, 3))
			{
				return;
			}

			if (shadow(UNITAI_SETTLER_SEA, 2, -1, false, true, 4))
			{
				return;
			}

			if (retreatToCity())
			{
				return;
			}

			if (safety())
			{
				return;
			}
		}
	}

	if (!m_unit->isHuman() && !m_unit->isNPC())
	{
		if (establishStackSeeInvisibleCoverage())
		{
			return;
		}
	}

	if (guardBonus(30))
	{
		return;
	}

	if (heal(30, 1))
	{
		return;
	}

	if (anyAttack(1, 55))
	{
		return;
	}

	if (seaBombardRange(6))
	{
		return;
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  09/01/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
		//if (protect(40))
	if (protect(40, 5))
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
		if (shadow(UNITAI_SETTLER_SEA, 1, -1, true))
		{
			return;
		}

		if (group(UNITAI_RESERVE_SEA, 1))
		{
			return;
		}

		if (bombardRate() > 0)
		{
			if (shadow(UNITAI_ASSAULT_SEA, 2, 30, true))
			{
				return;
			}
		}
	*/
	// Shadow any nearby settler sea transport out at sea
	if (shadow(UNITAI_SETTLER_SEA, 2, -1, false, true, 5))
	{
		return;
	}

	if (group(GroupingParams().withUnitAI(UNITAI_RESERVE_SEA).maxGroupSize(1).maxPathTurns(8)))
	{
		return;
	}

	if (m_unit->getGroup()->hasBombardCapability())
	{
		if (shadow(UNITAI_ASSAULT_SEA, 2, 30, true, false, 8))
		{
			return;
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (heal(50, 3))
	{
		return;
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  09/01/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
	if (protect(40))
	{
		return;
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (anyAttack(3, 45))
	{
		return;
	}

	if (heal())
	{
		return;
	}

	if (!m_unit->isNeverInvisible())
	{
		if (anyAttack(5, 35))
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
	if (shadow(UNITAI_SETTLER_SEA, 1, -1, true, false, 10))
	{
		return;
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (travelToUpgradeCity())
	{
		return;
	}

	if (patrol())
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::escortSeaMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}
	//	// if we have cargo, possibly convert to UNITAI_ASSAULT_SEA (m_unit will most often happen with galleons)
	//	// note, m_unit should not happen when we are not the group head, so escort galleons are fine joining a group, just not as head
	//	if (m_unit->hasCargo() && (m_unit->getUnitAICargo(UNITAI_ATTACK_CITY) > 0 || m_unit->getUnitAICargo(UNITAI_ATTACK) > 0))
	//	{
	//		// non-zero unitValue means that UNITAI_ASSAULT_SEA is valid for m_unit unit (that is the check used everywhere)
	//		if (GET_PLAYER(m_unit->getOwner()).AI_unitValue(m_unit->getUnitType(), UNITAI_ASSAULT_SEA, NULL) > 0)
	//		{
	//			// save old group, so we can merge it back in
	//			CvSelectionGroup* pOldGroup = m_unit->getGroup();
	//
	//			// m_unit will remove m_unit unit from the current group
	//			setUnitAIType(UNITAI_ASSAULT_SEA);
	//
	//			// merge back the rest of the group into the new group
	//			CvSelectionGroup* pNewGroup = m_unit->getGroup();
	//			if (pOldGroup != pNewGroup)
	//			{
	//				pOldGroup->mergeIntoGroup(pNewGroup);
	//			}
	//
	//			// perform assault sea action
	//			assaultSeaMove();
	//			return;
	//		}
	//	}

	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						06/14/09	Solver & jdog5000	*/
	/* 																			*/
	/* 	Naval AI																*/
	/********************************************************************************/
	if (m_unit->plot()->isCity(true)) //prioritize getting outta there
	{
		int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

		if (m_unit->getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0)
		{
			if (anyAttack(1, 60))
			{
				return;
			}

			if (group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).maxOwnUnitAI(1).ignoreFaster().maxPathTurns(1)))
			{
				return;
			}

			if (retreatToCity())
			{
				return;
			}

			if (safety())
			{
				return;
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	if (heal(30, 1))
	{
		return;
	}

	if (anyAttack(1, 55))
	{
		return;
	}

	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						9/14/08			jdog5000		*/
	/* 																			*/
	/* 	Naval AI																*/
	/********************************************************************************/
		// Galleons can get stuck with m_unit AI type since they don't upgrade to any escort unit
		// Galleon escorts are much less useful once Frigates or later are available
	if (!m_unit->isHuman() && !m_unit->isNPC())
	{
		if (establishStackSeeInvisibleCoverage())
		{
			return;
		}

		if (m_unit->hasCargo() && (GC.getUnitInfo(m_unit->getUnitType()).getSpecialCargo() == NO_SPECIALUNIT))
		{
			//Obsolete?
			int iValue = GET_PLAYER(m_unit->getOwner()).AI_unitValue(m_unit->getUnitType(), getUnitAIType(), m_unit->area());
			int iBestValue = GET_PLAYER(m_unit->getOwner()).AI_bestAreaUnitAIValue(getUnitAIType(), m_unit->area());

			if (iValue < iBestValue)
			{
				if (GET_PLAYER(m_unit->getOwner()).AI_unitValue(m_unit->getUnitType(), UNITAI_ASSAULT_SEA, m_unit->area()) > 0)
				{
					setUnitAIType(UNITAI_ASSAULT_SEA);
					return;
				}

				if (GET_PLAYER(m_unit->getOwner()).AI_unitValue(m_unit->getUnitType(), UNITAI_SETTLER_SEA, m_unit->area()) > 0)
				{
					setUnitAIType(UNITAI_SETTLER_SEA);
					return;
				}

				m_unit->scrap();
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	if (group(GroupingParams().withUnitAI(UNITAI_CARRIER_SEA).maxOwnUnitAI(0).ignoreFaster()))
	{
		return;
	}

	if (group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).maxOwnUnitAI(0).ignoreFaster().maxPathTurns(3)))
	{
		return;
	}

	if (heal(50, 3))
	{
		return;
	}

	if (pillageRange(2))
	{
		return;
	}

	if (group(GroupingParams().withUnitAI(UNITAI_MISSILE_CARRIER_SEA).maxGroupSize(1).maxOwnUnitAI(1).minUnitAI(1)))
	{
		return;
	}

	if (group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).maxGroupSize(1).maxOwnUnitAI(0).ignoreFaster()))
	{
		return;
	}

	if (group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).maxOwnUnitAI(2).ignoreFaster()))
	{
		return;
	}

	if (group(GroupingParams().withUnitAI(UNITAI_CARRIER_SEA).maxOwnUnitAI(2).ignoreFaster()))
	{
		return;
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  01/01/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					  */
	/************************************************************************************************/
	/* original bts code
		if (group(UNITAI_ASSAULT_SEA, -1, 4, -1, true))
		{
			return;
		}
	*/
	// Group only with large flotillas first
	if (group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).maxOwnUnitAI(4).minUnitAI(3).ignoreFaster()))
	{
		return;
	}

	if (shadow(UNITAI_SETTLER_SEA, 2, -1, false, true, 4))
	{
		return;
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (heal())
	{
		return;
	}

	if (travelToUpgradeCity())
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
	if (group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).maxOwnUnitAI(4).minUnitAI(3).maxPathTurns(4).withCargoOnly()))
	{
		return;
	}

	if (group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).maxOwnUnitAI(2).maxPathTurns(1).withCargoOnly()))
	{
		return;
	}

	// Pull back to primary m_unit->area if it's not too far so primary m_unit->area cities know you exist
	// and don't build more, unnecessary escorts
	if (retreatToCity(true, false, 6))
	{
		return;
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (retreatToCity())
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::exploreSeaMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						10/21/08	Solver & jdog5000	*/
	/* 																			*/
	/* 	Naval AI																*/
	/********************************************************************************/
	if (m_unit->plot()->isCity(true)) //prioritize getting outta there
	{
		int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

		if (m_unit->getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0)
		{
			if (!m_unit->isHuman())
			{
				if (anyAttack(1, 60))
				{
					return;
				}
			}

			if (retreatToCity())
			{
				return;
			}

			if (safety())
			{
				return;
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	CvArea* pWaterArea = m_unit->plot()->waterArea();

	if (!m_unit->isHuman())
	{
		if (anyAttack(1, 75))
		{
			return;
		}
		if (!m_unit->isNPC()) //XXX move some of m_unit into a function? maybe useful elsewhere
		{
			//Obsolete?
			int iValue = GET_PLAYER(m_unit->getOwner()).AI_unitValue(m_unit->getUnitType(), getUnitAIType(), m_unit->area());
			int iBestValue = GET_PLAYER(m_unit->getOwner()).AI_bestAreaUnitAIValue(getUnitAIType(), m_unit->area());

			if (iValue < iBestValue)
			{
				//Transform
				if (GET_PLAYER(m_unit->getOwner()).AI_unitValue(m_unit->getUnitType(), UNITAI_WORKER_SEA, m_unit->area()) > 0)
				{
					setUnitAIType(UNITAI_WORKER_SEA);
					return;
				}

				if (GET_PLAYER(m_unit->getOwner()).AI_unitValue(m_unit->getUnitType(), UNITAI_PIRATE_SEA, m_unit->area()) > 0)
				{
					setUnitAIType(UNITAI_PIRATE_SEA);
					return;
				}

				if (GET_PLAYER(m_unit->getOwner()).AI_unitValue(m_unit->getUnitType(), UNITAI_MISSIONARY_SEA, m_unit->area()) > 0)
				{
					setUnitAIType(UNITAI_MISSIONARY_SEA);
					return;
				}

				if (GET_PLAYER(m_unit->getOwner()).AI_unitValue(m_unit->getUnitType(), UNITAI_RESERVE_SEA, m_unit->area()) > 0)
				{
					setUnitAIType(UNITAI_RESERVE_SEA);
					return;
				}
				m_unit->scrap();
			}
		}
	}

	if (heal())
	{
		OutputDebugString("Chosen to heal\n");
		return;
	}

	if (!m_unit->isHuman())
	{
		if (pillageRange(1))
		{
			return;
		}
	}

	if (exploreRange(4))
	{
		return;
	}

	if (!m_unit->isHuman())
	{
		if (pillageRange(4))
		{
			return;
		}
	}

	if (explore())
	{
		return;
	}

	if (!m_unit->isHuman())
	{
		if (pillage())
		{
			return;
		}
	}

	if (!m_unit->isHuman())
	{
		if (travelToUpgradeCity())
		{
			return;
		}
	}

	if (!m_unit->isHuman() && (getUnitAIType() == UNITAI_EXPLORE_SEA))
	{
		pWaterArea = m_unit->plot()->waterArea();

		if (pWaterArea != NULL
		&& GET_PLAYER(m_unit->getOwner()).AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_EXPLORE_SEA) > GET_PLAYER(m_unit->getOwner()).AI_neededExplorers(pWaterArea)
		&& GET_PLAYER(m_unit->getOwner()).getFinalUnitUpkeep() > 0)
		{
			m_unit->scrap();
			return;
		}
	}

	if (patrol())
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::assaultSeaMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}
	FAssert(getUnitAIType() == UNITAI_ASSAULT_SEA);

	bool bEmpty = !m_unit->getGroup()->hasCargo();
	bool bFull = (m_unit->getGroup()->AI_isFull() && (m_unit->getGroup()->getCargo() > 0));
	//I THINK a count of cargo is sufficient as it's only a verifier statement
	//MIGHT need to make m_unit a little less demanding of being 'full' but the splitting of units to pack onto the boat will probably suffice.

	// Afforess 09/01/10
	// After a Barb Transport is done, set it to attack AI
	if (bEmpty && m_unit->getOwner() == BARBARIAN_PLAYER)
	{
		setUnitAIType(UNITAI_ATTACK_SEA);
		barbAttackSeaMove();
		return;
	}

	if (m_unit->plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

		if (m_unit->getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 8 || iOurDefense == 0)
		{
			if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
			{
				if (!bEmpty)
				{
					m_unit->getGroup()->unloadAll();
				}

				if (anyAttack(1, 65))
				{
					return;
				}

				// Retreat to primary m_unit->area first
				if (retreatToCity(true))
				{
					return;
				}

				if (retreatToCity())
				{
					return;
				}

				if (safety())
				{
					return;
				}
			}

			if (!bFull && !bEmpty)
			{
				m_unit->getGroup()->unloadAll();
				m_unit->getGroup()->pushMission(MISSION_SKIP);
				return;
			}
		}
	}

	if (bEmpty)
	{
		if (anyAttack(1, 65))
		{
			return;
		}
		if (anyAttack(1, 45))
		{
			return;
		}
	}

	bool bReinforce = false;
	bool bAttack = false;
	const bool bNoWarPlans = !GET_TEAM(m_unit->getTeam()).hasWarPlan(true);
	bool bAttackBarbarian = false;
	bool bIsBarbarian = m_unit->isNPC();

	// Count forts as cities
	bool bIsCity = m_unit->plot()->isCity(true);

	// Cargo if already at war
	int iTargetReinforcementSize = (bIsBarbarian ? stackOfDoomExtra() : 2);

	// Cargo to launch a new invasion
	int iTargetInvasionSize = 2 * iTargetReinforcementSize;

	int iCargoCount = m_unit->getGroup()->getCargo();
	int iCargoVolume = m_unit->getGroup()->getCargo(true);

	int iEscorts = m_unit->getGroup()->countNumUnitAIType(UNITAI_ESCORT_SEA) + m_unit->getGroup()->countNumUnitAIType(UNITAI_ATTACK_SEA);

	AreaAITypes eAreaAIType = m_unit->area()->getAreaAIType(m_unit->getTeam());
	bool bLandWar = !bIsBarbarian && ((eAreaAIType == AREAAI_OFFENSIVE) || (eAreaAIType == AREAAI_DEFENSIVE) || (eAreaAIType == AREAAI_MASSING));

	// Plot danger case handled above

	if (m_unit->hasCargo() && (m_unit->getUnitAICargo(UNITAI_SETTLE) > 0 || m_unit->getUnitAICargo(UNITAI_WORKER) > 0))
	{
		// Dump inappropriate load at first oppurtunity after pick up
		if (bIsCity && (m_unit->plot()->getOwner() == m_unit->getOwner()))
		{
			m_unit->getGroup()->unloadAll();
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}
		else
		{
			if (!m_unit->isFull())
			{
				if (pickupStranded(NO_UNITAI, 1))
				{
					return;
				}
			}

			if (retreatToCity(true))
			{
				return;
			}

			if (retreatToCity())
			{
				return;
			}
		}
	}

	if (bIsCity)
	{
		CvCity* pCity = m_unit->plot()->getPlotCity();

		if (pCity != NULL && (m_unit->plot()->getOwner() == m_unit->getOwner()))
		{
			if (!m_unit->isHuman() && !m_unit->isNPC())
			{
				if (establishStackSeeInvisibleCoverage())
				{
					return;
				}
			}
			// split out galleys from stack of ocean capable ships
			if (GET_PLAYER(m_unit->getOwner()).AI_unitImpassableCount(m_unit->getUnitType()) == 0 && m_unit->getGroup()->getNumUnits() > 1)
			{
				m_unit->getGroup()->AI_separateImpassable();
			}

			// galleys with upgrade available should get that ASAP
			if (GET_PLAYER(m_unit->getOwner()).AI_unitImpassableCount(m_unit->getUnitType()) > 0)
			{
				CvCity* pUpgradeCity = m_unit->getUpgradeCity(false);
				if (pUpgradeCity != NULL && pUpgradeCity == pCity)
				{
					// Wait for upgrade, m_unit unit is top upgrade priority
					m_unit->getGroup()->pushMission(MISSION_SKIP);
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
						// Just captured city, probably from naval invasion.  If m_unit->area targets, drop cargo and leave so as to not to be lost in quick counter attack
						if (GET_TEAM(m_unit->getTeam()).countEnemyPowerByArea(m_unit->plot()->area()) > 0)
						{
							m_unit->getGroup()->unloadAll();

							if (iEscorts > 2)
							{
								if (m_unit->getGroup()->countNumUnitAIType(UNITAI_ESCORT_SEA) > 1 && m_unit->getGroup()->countNumUnitAIType(UNITAI_ATTACK_SEA) > 0)
								{
									m_unit->getGroup()->AI_separateAI(UNITAI_ATTACK_SEA);
									m_unit->getGroup()->AI_separateAI(UNITAI_RESERVE_SEA);

									iEscorts = m_unit->getGroup()->countNumUnitAIType(UNITAI_ESCORT_SEA);
								}
							}
							iCargoCount = m_unit->getGroup()->getCargo();
							iCargoVolume = m_unit->getGroup()->getCargo(true);
							//TBHERE (count or volume?)
						}
					}
				}
			}
		}

		if ((iCargoCount > 0) && (iEscorts == 0))
		{
			if (group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).ignoreFaster().maxPathTurns(1).withCargoOnly().ignoreMissionAIType(MISSIONAI_ASSAULT)))
			{
				return;
			}

			if (m_unit->plot()->plotCount(PUF_isUnitAIType, UNITAI_ESCORT_SEA, -1, NULL, m_unit->getOwner(), NO_TEAM, PUF_isGroupHead, -1, -1) > 0)
			{
				// Loaded but with no escort, wait for escorts in m_unit->plot to join us
				m_unit->getGroup()->pushMission(MISSION_SKIP);
				return;
			}

			MissionAITypes eMissionAIType = MISSIONAI_GROUP;
			if ((GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, &eMissionAIType, 1, m_unit->getGroup(), 3) > 0) || (GET_PLAYER(m_unit->getOwner()).AI_getWaterDanger(m_unit->plot(), 4) > 0))
			{
				// Loaded but with no escort, wait for others joining us soon or avoid dangerous waters
				m_unit->getGroup()->pushMission(MISSION_SKIP);
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
					m_unit->getGroup()->unloadAll();
					m_unit->getGroup()->pushMission(MISSION_SKIP);
					return;
				}
			}

			if ((iCargoCount >= iTargetReinforcementSize))
			{
				m_unit->getGroup()->AI_separateEmptyTransports();

				if (!(m_unit->getGroup()->hasCargo()))
				{
					// m_unit unit was empty group leader
					m_unit->getGroup()->pushMission(MISSION_SKIP);
					return;
				}

				// Send ready transports
				if (assaultSeaReinforce(false))
				{
					return;
				}

				if (iCargoCount >= iTargetInvasionSize)
				{
					if (assaultSeaTransport(false))
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
			&& (iCargoVolume > m_unit->cargoSpace() || iCargoCount >= iTargetReinforcementSize))
			{
				bReinforce = true;
			}
		}

		if (!bAttack && !bReinforce && (m_unit->plot()->getTeam() == m_unit->getTeam()))
		{
			if (iEscorts > 3 && iEscorts > (2 * m_unit->getGroup()->countNumUnitAIType(UNITAI_ASSAULT_SEA)))
			{
				// If we have too many escorts, try freeing some for others
				m_unit->getGroup()->AI_separateAI(UNITAI_ATTACK_SEA);
				m_unit->getGroup()->AI_separateAI(UNITAI_RESERVE_SEA);

				iEscorts = m_unit->getGroup()->countNumUnitAIType(UNITAI_ESCORT_SEA);
				if (iEscorts > 3 && iEscorts > (2 * m_unit->getGroup()->countNumUnitAIType(UNITAI_ASSAULT_SEA)))
				{
					m_unit->getGroup()->AI_separateAI(UNITAI_ESCORT_SEA);
				}
			}
		}

		MissionAITypes eMissionAIType = MISSIONAI_GROUP;
		if (GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, &eMissionAIType, 1, m_unit->getGroup(), 1) > 0)
		{
			// Wait for units which are joining our group m_unit turn
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}

		if (!bFull)
		{
			if (bAttack)
			{
				eMissionAIType = MISSIONAI_LOAD_ASSAULT;
				if (GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, &eMissionAIType, 1, m_unit->getGroup(), 1) > 0)
				{
					// Wait for cargo which will load m_unit turn
					m_unit->getGroup()->pushMission(MISSION_SKIP);
					return;
				}
			}
			else if (GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, MISSIONAI_LOAD_ASSAULT) > 0)
			{
				// Wait for cargo which is on the way
				m_unit->getGroup()->pushMission(MISSION_SKIP);
				return;
			}
		}

		if (!bAttack && !bReinforce)
		{
			if (iCargoCount > 0)
			{
				if (group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).ignoreFaster().maxPathTurns(5).withCargoOnly().ignoreMissionAIType(MISSIONAI_ASSAULT)))
				{
					return;
				}
			}
			else if (m_unit->plot()->getTeam() == m_unit->getTeam() && m_unit->getGroup()->getNumUnits() > 1)
			{
				CvCity* pCity = m_unit->plot()->getPlotCity();
				if (pCity != NULL && (GC.getGame().getGameTurn() - pCity->getGameTurnAcquired()) > 10)
				{
					if (pCity->plot()->plotCount(PUF_isAvailableUnitAITypeGroupie, UNITAI_ATTACK_CITY, -1, NULL, m_unit->getOwner()) < iTargetReinforcementSize)
					{
						// Not attacking, no cargo so release any escorts, attack ships, etc and split transports
						m_unit->getGroup()->AI_makeForceSeparate();
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
		|| bFull && iCargoVolume > m_unit->cargoSpace())
		{
			bReinforce = true;
		}

		foreach_(CvPlot * pAdjacentPlot, m_unit->plot()->adjacent())
		{
			if (iCargoCount > 0)
			{
				const CvCity* pAdjacentCity = pAdjacentPlot->getPlotCity();
				if (pAdjacentCity != NULL && pAdjacentCity->getOwner() == m_unit->getOwner() && pAdjacentCity->getPreviousOwner() != NO_PLAYER)
				{
					if ((GC.getGame().getGameTurn() - pAdjacentCity->getGameTurnAcquired()) < 5)
					{
						// If just captured city and we have some cargo, dump units in city
						if (m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pAdjacentPlot->getX(), pAdjacentPlot->getY(), 0, false, false, MISSIONAI_ASSAULT, pAdjacentPlot))
						{
							return;
						}
					}
				}
			}
			else if (pAdjacentPlot->isOwned() && m_unit->isEnemy(pAdjacentPlot->getTeam()))
			{
				if (pAdjacentPlot->getNumDefenders(m_unit->getOwner()) > 2)
				{
					// if we just made a dropoff in enemy territory, release sea bombard units to support invaders
					if ((m_unit->getGroup()->countNumUnitAIType(UNITAI_ATTACK_SEA) + m_unit->getGroup()->countNumUnitAIType(UNITAI_RESERVE_SEA)) > 0)
					{
						bool bMissionPushed = false;

						//Covers city bombard and ranged assault on adjacent cities
						if (seaBombardRange(1))
						{
							bMissionPushed = true;
						}

						CvSelectionGroup* pOldGroup = m_unit->getGroup();

						//Release any Warships to finish the job.
						m_unit->getGroup()->AI_separateAI(UNITAI_ATTACK_SEA);
						m_unit->getGroup()->AI_separateAI(UNITAI_RESERVE_SEA);

						// Fixed bug in next line with checking unit type instead of unit AI
						if (pOldGroup == m_unit->getGroup() && getUnitAIType() == UNITAI_ASSAULT_SEA)
						{
							// Need to be sure all units can move
							if (m_unit->getGroup()->canAllMove())
							{
								if (retreatToCity(true))
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
			if (GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, &eMissionAIType, 1, m_unit->getGroup(), 1) > 0)
			{
				if (iEscorts < GET_PLAYER(m_unit->getOwner()).AI_getWaterDanger(m_unit->plot(), 2))
				{
					// Wait for units which are joining our group m_unit turn (hopefully escorts)
					m_unit->getGroup()->pushMission(MISSION_SKIP);
					return;
				}
			}
		}
	}
	//If all else fails
	if (RbombardNaval())
	{
		return;
	}
	// Dale - RB: Field Bombard END

	if (bIsBarbarian)
	{
		if (m_unit->getGroup()->isFull() || iCargoCount > iTargetInvasionSize)
		{
			if (assaultSeaTransport(false))
			{
				return;
			}
		}
		else
		{
			if (pickup(UNITAI_ATTACK_CITY, true, 5))
			{
				return;
			}

			if (pickup(UNITAI_ATTACK, true, 5))
			{
				return;
			}

			//TBRETURN
			if (retreatToCity())
			{
				return;
			}

			if (!(m_unit->getGroup()->getCargo()))
			{//boolean call asking if there is cargo
				barbAttackSeaMove();
				return;
			}

			if (safety())
			{
				return;
			}

			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}
	else
	{
		if (bAttack || bReinforce)
		{
			if (bIsCity)
			{
				m_unit->getGroup()->AI_separateEmptyTransports();
			}

			if (!(m_unit->getGroup()->hasCargo()))
			{
				// m_unit unit was empty group leader
				m_unit->getGroup()->pushMission(MISSION_SKIP);
				return;
			}

			FAssert(m_unit->getGroup()->hasCargo());

			//BBAI TODO: Check that group has escorts, otherwise usually wait

			if (bAttack)
			{
				if (bReinforce && (getBirthmark() % 2 == 0))
				{
					if (assaultSeaReinforce())
					{
						return;
					}
					bReinforce = false;
				}

				if (assaultSeaTransport())
				{
					return;
				}
			}

			// If not enough troops for own invasion,
			if (bReinforce)
			{
				if (assaultSeaReinforce())
				{
					return;
				}
			}
		}

		if (bNoWarPlans && (iCargoCount >= iTargetReinforcementSize))
		{
			bAttackBarbarian = true;

			m_unit->getGroup()->AI_separateEmptyTransports();

			if (!(m_unit->getGroup()->hasCargo()))
			{
				// m_unit unit was empty group leader
				m_unit->getGroup()->pushMission(MISSION_SKIP);
				return;
			}

			FAssert(m_unit->getGroup()->hasCargo());
			if (assaultSeaReinforce(bAttackBarbarian))
			{
				return;
			}

			FAssert(m_unit->getGroup()->hasCargo());
			if (assaultSeaTransport(bAttackBarbarian))
			{
				return;
			}
		}
	}

	bFull = (m_unit->getGroup()->AI_isFull() && (m_unit->getGroup()->getCargo() > 0));
	if ((bFull || bReinforce) && !bAttack)
	{
		// Group with nearby transports with units on board
		if (group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).ignoreFaster().maxPathTurns(2).withCargoOnly().ignoreMissionAIType(MISSIONAI_ASSAULT)))
		{
			return;
		}

		if (group(GroupingParams().withUnitAI(UNITAI_ASSAULT_SEA).ignoreFaster().maxPathTurns(10).withCargoOnly().ignoreMissionAIType(MISSIONAI_ASSAULT)))
		{
			return;
		}
	}
	else if (!bFull)
	{
		bool bHasOneLoad = false;
		if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
		{
			bHasOneLoad = (m_unit->getGroup()->getCargo(true) >= m_unit->cargoSpace());
		}
		else
		{
			bHasOneLoad = (m_unit->getGroup()->getCargo() >= m_unit->cargoSpace());
		}

		bool bHasCargo = m_unit->getGroup()->hasCargo();
		if (pickup(UNITAI_ATTACK_CITY, !bHasCargo, (bHasOneLoad ? 3 : 7)))
		{
			return;
		}

		if (pickup(UNITAI_ATTACK, !bHasCargo, (bHasOneLoad ? 3 : 7)))
		{
			return;
		}

		if (pickup(UNITAI_COUNTER, !bHasCargo, (bHasOneLoad ? 3 : 7)))
		{
			return;
		}

		if (pickup(UNITAI_ATTACK_CITY, !bHasCargo))
		{
			return;
		}

		if (!bHasCargo)
		{
			if (pickupStranded(UNITAI_ATTACK_CITY))
			{
				return;
			}

			if (pickupStranded(UNITAI_ATTACK))
			{
				return;
			}

			if (pickupStranded(UNITAI_COUNTER))
			{
				return;
			}

			if ((m_unit->getGroup()->countNumUnitAIType(getUnitAIType()) == 1))
			{
				// Try picking up any thing

				if (pickupStranded())
				{
					return;
				}
			}
		}
	}

	if (bIsCity && bLandWar && m_unit->getGroup()->hasCargo())
	{
		// Enemy units in m_unit player's territory
		if (GET_PLAYER(m_unit->getOwner()).AI_countNumAreaHostileUnits(m_unit->area(), true, false, false, false, m_unit->plot(), 15) > (m_unit->getGroup()->getCargo() / 2))
		{//A count is sufficient.  Just asking if it should dropoff when threatened.
			m_unit->getGroup()->unloadAll();
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	if (retreatToCity(true))
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::settlerSeaMove()
{
	PROFILE_FUNC();

	bool bEmpty = !m_unit->getGroup()->hasCargo();

	if (selectStatus(true))
	{
		return;
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						10/21/08	Solver & jdog5000	*/
	/* 																			*/
	/* 	Naval AI																*/
	/********************************************************************************/
	if (m_unit->plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

		if (m_unit->getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			if (bEmpty)
			{
				if (anyAttack(1, 65))
				{
					return;
				}
			}

			// Retreat to primary m_unit->area first
			if (retreatToCity(true))
			{
				return;
			}

			if (retreatToCity())
			{
				return;
			}

			if (safety())
			{
				return;
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	if (bEmpty)
	{
		if (anyAttack(1, 65))
		{
			return;
		}
		if (anyAttack(1, 40))
		{
			return;
		}
	}

	int iSettlerCount = m_unit->getUnitAICargo(UNITAI_SETTLE);
	int iWorkerCount = m_unit->getUnitAICargo(UNITAI_WORKER);

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  12/07/08								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
	if (m_unit->hasCargo() && (iSettlerCount == 0) && (iWorkerCount == 0))
	{
		// Dump troop load at first oppurtunity after pick up
		if (m_unit->plot()->isCity() && m_unit->plot()->getOwner() == m_unit->getOwner())
		{
			m_unit->getGroup()->unloadAll();
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}
		else
		{
			if (!(m_unit->isFull()))
			{
				if (pickupStranded(NO_UNITAI, 1))
				{
					return;
				}
			}

			if (retreatToCity(true))
			{
				return;
			}

			if (retreatToCity())
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
	if ((iSettlerCount > 0) && (iSettlerCount + iWorkerCount == m_unit->cargoSpace()))
	{
		// No defenders for settler
		if (m_unit->plot()->isCity() && m_unit->plot()->getOwner() == m_unit->getOwner())
		{
			m_unit->getGroup()->unloadAll();
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	if ((iSettlerCount > 0) && (m_unit->isFull() ||
		((m_unit->getUnitAICargo(UNITAI_CITY_DEFENSE) > 0) &&
			(GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, MISSIONAI_LOAD_SETTLER) == 0))))
		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD					   END												  */
		/************************************************************************************************/
	{
		if (settlerSeaTransport())
		{
			return;
		}
	}
	else if ((m_unit->getTeam() != m_unit->plot()->getTeam()) && bEmpty)
	{
		if (pillageRange(3))
		{
			return;
		}
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  09/18/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
	if (m_unit->plot()->isCity() && m_unit->plot()->getOwner() == m_unit->getOwner() && !m_unit->hasCargo())
		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD					   END												  */
		/************************************************************************************************/
	{
		AreaAITypes eAreaAI = m_unit->area()->getAreaAIType(m_unit->getTeam());
		if ((eAreaAI == AREAAI_ASSAULT) || (eAreaAI == AREAAI_ASSAULT_MASSING))
		{
			CvArea* pWaterArea = m_unit->plot()->waterArea();
			FAssert(pWaterArea != NULL);
			if (pWaterArea != NULL)
			{
				if (GET_PLAYER(m_unit->getOwner()).AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_SETTLER_SEA) > 1)
				{
					if (GET_PLAYER(m_unit->getOwner()).AI_unitValue(m_unit->getUnitType(), UNITAI_ASSAULT_SEA, pWaterArea) > 0)
					{
						setUnitAIType(UNITAI_ASSAULT_SEA);
						assaultSeaMove();
						return;
					}
				}
			}
		}
	}

	if ((iWorkerCount > 0)
		&& GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, MISSIONAI_LOAD_SETTLER) == 0)
	{
		if (m_unit->isFull() || (iSettlerCount == 0))
		{
			if (settlerSeaFerry())
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
		if (pickup(UNITAI_SETTLE))
		{
			return;
		}
	*/
	if (!(m_unit->getGroup()->hasCargo()))
	{
		if (pickupStranded(UNITAI_SETTLE))
		{
			return;
		}
	}

	if (!m_unit->getGroup()->isFull())
	{
		if (GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, MISSIONAI_LOAD_SETTLER) > 0)
		{
			// Wait for units on the way
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}

		if (iSettlerCount > 0)
		{
			if (pickup(UNITAI_CITY_DEFENSE))
			{
				return;
			}
		}
		else if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
		{
			if (m_unit->cargoSpace() - 2 * GET_PLAYER(m_unit->getOwner()).getBestUnitTypeCargoVolume(UNITAI_WORKER) >= m_unit->SMgetCargo() + iWorkerCount * GET_PLAYER(m_unit->getOwner()).getBestUnitTypeCargoVolume(UNITAI_WORKER))
			{
				if (pickup(UNITAI_SETTLE, true))
				{
					return;
				}
			}
		}
		else if (m_unit->cargoSpace() - 2 >= m_unit->getCargo() + iWorkerCount)
		{
			if (pickup(UNITAI_SETTLE, true))
			{
				return;
			}
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if ((GC.getGame().getGameTurn() - m_unit->getGameTurnCreated()) < 8)
	{
		if ((m_unit->plot()->getPlotCity() == NULL) || GET_PLAYER(m_unit->getOwner()).AI_totalAreaUnitAIs(m_unit->plot()->area(), UNITAI_SETTLE) == 0)
		{
			if (explore())
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
		if (pickup(UNITAI_WORKER))
		{
			return;
		}
	*/
	if (!m_unit->getGroup()->isFull())
	{
		// Rescue stranded non-settlers
		if (pickupStranded())
		{
			return;
		}
	}

	//	Koshling - old condition here was broken for transports with a max capacity of 1 (canoes),
	//	and (after readiong the old code) I think more generally anyway.  Effect was it repeatedly went
	//	through here without ever actually wanting to load a settler (which it never does without an
	//	escort unit also) and wound up giving no orders at all to m_unit unit, then looping over m_unit 100 times
	//	a ta higher level before giving up and skipping moves for m_unit unit.
	if (!GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS)
	&& m_unit->cargoSpace() > 1 && m_unit->cargoSpace() - m_unit->getCargo() < 2 && iWorkerCount > 0

	|| GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS)
	&& m_unit->cargoSpace() > GET_PLAYER(m_unit->getOwner()).getBestUnitTypeCargoVolume(UNITAI_SETTLE)
	&& m_unit->cargoSpace() - m_unit->SMgetCargo() < 2 * GET_PLAYER(m_unit->getOwner()).getBestUnitTypeCargoVolume(UNITAI_WORKER)
	&& iWorkerCount > 0)
	{
		// If full of workers and not going anywhere, dump them if a settler is available
		if (iSettlerCount == 0 && m_unit->plot()->plotCount(PUF_isAvailableUnitAITypeGroupie, UNITAI_SETTLE, -1, NULL, m_unit->getOwner(), NO_TEAM, PUF_isFiniteRange) > 0)
		{
			m_unit->getGroup()->unloadAll();

			if (!m_unit->getGroup()->isFull())
			{
				pickup(UNITAI_SETTLE, true);
			}
			return;
		}
	}

	if (!(m_unit->getGroup()->isFull()))
	{
		if (pickup(UNITAI_WORKER))
		{
			return;
		}
	}

	// Carracks cause problems for transport upgrades, galleys can't upgrade to them and they can't
	// upgrade to galleons.  Scrap galleys, switch unit AI for stuck Carracks.
	if (m_unit->plot()->isCity() && m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		UnitTypes eBestSettlerTransport = NO_UNIT;
		GET_PLAYER(m_unit->getOwner()).AI_bestCityUnitAIValue(getUnitAIType(), NULL, &eBestSettlerTransport);
		if (eBestSettlerTransport != NO_UNIT)
		{
			if (eBestSettlerTransport != m_unit->getUnitType()
			&& GET_PLAYER(m_unit->getOwner()).AI_unitImpassableCount(eBestSettlerTransport) == 0
			&& !m_unit->upgradeAvailable(m_unit->getUnitType(), eBestSettlerTransport))
			{
				m_unit->getGroup()->unloadAll();

				if (GET_PLAYER(m_unit->getOwner()).AI_unitImpassableCount(m_unit->getUnitType()) > 0)
				{
					m_unit->scrap();
					return;
				}
				else
				{
					CvArea* pWaterArea = m_unit->plot()->waterArea();
					FAssert(pWaterArea != NULL);
					if (pWaterArea != NULL)
					{
						if (GET_PLAYER(m_unit->getOwner()).AI_totalUnitAIs(UNITAI_EXPLORE_SEA) == 0)
						{
							if (GET_PLAYER(m_unit->getOwner()).AI_unitValue(m_unit->getUnitType(), UNITAI_EXPLORE_SEA, pWaterArea) > 0)
							{
								setUnitAIType(UNITAI_EXPLORE_SEA);
								exploreSeaMove();
								return;
							}
						}

						if (GET_PLAYER(m_unit->getOwner()).AI_totalUnitAIs(UNITAI_SPY_SEA) == 0)
						{
							if (GET_PLAYER(m_unit->getOwner()).AI_unitValue(m_unit->getUnitType(), UNITAI_SPY_SEA, m_unit->area()) > 0)
							{
								setUnitAIType(UNITAI_SPY_SEA);
								spySeaMove();
								return;
							}
						}

						if (GET_PLAYER(m_unit->getOwner()).AI_totalUnitAIs(UNITAI_MISSIONARY_SEA) == 0)
						{
							if (GET_PLAYER(m_unit->getOwner()).AI_unitValue(m_unit->getUnitType(), UNITAI_MISSIONARY_SEA, m_unit->area()) > 0)
							{
								setUnitAIType(UNITAI_MISSIONARY_SEA);
								missionarySeaMove();
								return;
							}
						}

						if (GET_PLAYER(m_unit->getOwner()).AI_unitValue(m_unit->getUnitType(), UNITAI_ATTACK_SEA, pWaterArea) > 0)
						{
							setUnitAIType(UNITAI_ATTACK_SEA);
							attackSeaMove();
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

	if (retreatToCity(true))
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::missionarySeaMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						10/21/08	Solver & jdog5000	*/
	/* 																			*/
	/* 	Naval AI																*/
	/********************************************************************************/
	if (m_unit->plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

		if (m_unit->getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			// Retreat to primary m_unit->area first
			if (retreatToCity(true))
			{
				return;
			}

			if (retreatToCity())
			{
				return;
			}

			if (safety())
			{
				return;
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	if (m_unit->getUnitAICargo(UNITAI_MISSIONARY) > 0)
	{
		if (specialSeaTransportMissionary())
		{
			return;
		}
	}
	else if (!(m_unit->getGroup()->hasCargo()))
	{
		if (pillageRange(4))
		{
			return;
		}
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  01/14/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
	if (!(m_unit->getGroup()->isFull()))
	{
		if (GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, MISSIONAI_LOAD_SPECIAL) > 0)
		{
			// Wait for units on the way
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}

		if (pickup(UNITAI_MISSIONARY, true))
		{
			return;
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (explore())
	{
		return;
	}

	if (retreatToCity(true))
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::spySeaMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}
	CvCity* pCity;

	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						10/21/08	Solver & jdog5000	*/
	/* 																			*/
	/* 	Naval AI																*/
	/********************************************************************************/
	if (m_unit->plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

		if (m_unit->getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			// Retreat to primary m_unit->area first
			if (retreatToCity(true))
			{
				return;
			}

			if (retreatToCity())
			{
				return;
			}

			if (safety())
			{
				return;
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	if (m_unit->getUnitAICargo(UNITAI_SPY) > 0)
	{
		if (specialSeaTransportSpy())
		{
			return;
		}

		pCity = m_unit->plot()->getPlotCity();

		if (pCity != NULL)
		{
			if (pCity->getOwner() == m_unit->getOwner())
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_ATTACK_SPY, pCity->plot());
				return;
			}
		}
	}
	else if (!(m_unit->getGroup()->hasCargo()))
	{
		if (pillageRange(5))
		{
			return;
		}
	}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  01/14/09								jdog5000	  */
	/*																							  */
	/* Naval AI																					 */
	/************************************************************************************************/
	if (!(m_unit->getGroup()->isFull()))
	{
		if (GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, MISSIONAI_LOAD_SPECIAL) > 0)
		{
			// Wait for units on the way
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}

		if (pickup(UNITAI_SPY, true))
		{
			return;
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	if (retreatToCity(true))
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::carrierSeaMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						10/21/08	Solver & jdog5000	*/
	/* 																			*/
	/* 	Naval AI																*/
	/********************************************************************************/
	if (m_unit->plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

		if (m_unit->getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			if (retreatToCity(true))
			{
				return;
			}

			if (retreatToCity())
			{
				return;
			}

			if (safety())
			{
				return;
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	if (heal(50))
	{
		return;
	}

	if (!m_unit->isHuman() && !m_unit->isNPC())
	{
		if (establishStackSeeInvisibleCoverage())
		{
			return;
		}
	}

	if (!m_unit->isEnemy(m_unit->plot()->getTeam()))
	{
		if (GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, MISSIONAI_GROUP) > 0)
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}
	else
	{
		if (seaBombardRange(1))
		{
			return;
		}
	}

	if (group(GroupingParams().withUnitAI(UNITAI_CARRIER_SEA).maxOwnUnitAI(1)))
	{
		return;
	}

	if (m_unit->getGroup()->countNumUnitAIType(UNITAI_ATTACK_SEA) + m_unit->getGroup()->countNumUnitAIType(UNITAI_ESCORT_SEA) == 0)
	{
		if (m_unit->plot()->isCity() && m_unit->plot()->getOwner() == m_unit->getOwner())
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}
		if (retreatToCity())
		{
			return;
		}
	}

	if (m_unit->hasCargo())
	{
		if (carrierSeaTransport())
		{
			return;
		}

		if (blockade())
		{
			return;
		}

		if (shadow(UNITAI_ASSAULT_SEA))
		{
			return;
		}
	}

	if (travelToUpgradeCity())
	{
		return;
	}

	if (retreatToCity(true))
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::missileCarrierSeaMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}
	bool bIsStealth = (m_unit->getInvisibleType() != NO_INVISIBLE);

	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						06/14/09	Solver & jdog5000	*/
	/* 																			*/
	/* 	Naval AI																*/
	/********************************************************************************/
	if (m_unit->plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

		if (m_unit->getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			if (shadow(UNITAI_ASSAULT_SEA, 1, 50, false, true, 1))
			{
				return;
			}

			if (retreatToCity())
			{
				return;
			}

			if (safety())
			{
				return;
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	if (m_unit->plot()->isCity() && m_unit->plot()->getTeam() == m_unit->getTeam())
	{
		if (heal())
		{
			return;
		}
	}

	if (((m_unit->plot()->getTeam() != m_unit->getTeam()) && m_unit->getGroup()->hasCargo()) || m_unit->getGroup()->AI_isFull())
	{
		if (bIsStealth)
		{
			if (carrierSeaTransport())
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
			if (shadow(UNITAI_ASSAULT_SEA, 1, 50, true, false, 12))
			{
				return;
			}
			/********************************************************************************/
			/* 	BETTER_BTS_AI_MOD						END								*/
			/********************************************************************************/

			if (carrierSeaTransport())
			{
				return;
			}
		}
	}
	//	if (pickup(UNITAI_ICBM))
	//	{
	//		return;
	//	}
	//
	//	if (pickup(UNITAI_MISSILE_AIR))
	//	{
	//		return;
	//	}
	if (retreatToCity())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
}


void CvUnitDefaultAI::attackAirMove()
{
	PROFILE_FUNC();

	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						10/21/08	Solver & jdog5000	*/
	/* 																			*/
	/* 	Air AI																	*/
	/********************************************************************************/
	CvCity* pCity = m_unit->plot()->getPlotCity();
	bool bSkiesClear = true;

	// Check for sufficient defenders to stay
	int iDefenders = m_unit->plot()->plotCount(PUF_canDefend, -1, -1, NULL, m_unit->plot()->getOwner());

	int iAttackAirCount = m_unit->plot()->plotCount(PUF_canAirAttack, -1, -1, NULL, NO_PLAYER, m_unit->getTeam());
	iAttackAirCount += 2 * m_unit->plot()->plotCount(PUF_isUnitAIType, UNITAI_ICBM, -1, NULL, NO_PLAYER, m_unit->getTeam());

	if (m_unit->plot()->isCoastalLand(GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize()))
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
		if (airOffensiveCity())
		{
			return;
		}
	}

	// Check for direct threat to current base
	if (m_unit->plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

		if (iEnemyOffense > iOurDefense || iOurDefense == 0)
		{
			// Too risky, pull back
			if (airOffensiveCity())
			{
				return;
			}

			if (m_unit->canAirDefend())
			{
				if (airDefensiveCity())
				{
					return;
				}
			}
		}
		else if (iEnemyOffense > iOurDefense / 3)
		{
			if (m_unit->getDamage() == 0)
			{
				if (!m_unit->getGroup()->hasCollateralDamage() && m_unit->canAirDefend())
				{
					if (pCity != NULL)
					{
						// Check for whether city needs m_unit unit to air defend
						if (!(pCity->AI_isAirDefended(true, -1)))
						{
							m_unit->getGroup()->pushMission(MISSION_AIRPATROL);
							return;
						}
					}
				}

				// Attack the invaders!
				if (defendBaseAirStrike())
				{
					return;
				}

				if (defensiveAirStrike())
				{
					return;
				}

				if (airStrike())
				{
					return;
				}

				// If no targets, no sense staying in risky place
				if (airOffensiveCity())
				{
					return;
				}

				if (m_unit->canAirDefend())
				{
					if (airDefensiveCity())
					{
						return;
					}
				}
			}

			if (m_unit->healTurns(m_unit->plot()) > 1)
			{
				// If very damaged, no sense staying in risky place
				if (airOffensiveCity())
				{
					return;
				}

				if (m_unit->canAirDefend())
				{
					if (airDefensiveCity())
					{
						return;
					}
				}
			}

		}
	}

	if (m_unit->getDamage() > 0)
	{
		if (100 * m_unit->getHP() / m_unit->getMaxHP() < 40)
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}
		const int iSearchRange = m_unit->airRange();
		foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
		{
			if (m_unit->bestInterceptor(pLoopPlot) != NULL)
			{
				bSkiesClear = false;
				break;
			}
		}

		if (!bSkiesClear)
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	CvPlayerAI& kPlayer = GET_PLAYER(m_unit->getOwner());
	CvArea* pArea = m_unit->area();
	int iAttackValue = kPlayer.AI_unitValue(m_unit->getUnitType(), UNITAI_ATTACK_AIR, pArea);
	int iCarrierValue = kPlayer.AI_unitValue(m_unit->getUnitType(), UNITAI_CARRIER_AIR, pArea);
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
					setUnitAIType(UNITAI_CARRIER_AIR);
					m_unit->getGroup()->pushMission(MISSION_SKIP);
					return;
				}
			}
		}
	}

	int iDefenseValue = kPlayer.AI_unitValue(m_unit->getUnitType(), UNITAI_DEFENSE_AIR, pArea);
	if (iDefenseValue > iAttackValue)
	{
		if (kPlayer.AI_bestAreaUnitAIValue(UNITAI_ATTACK_AIR, pArea) > iAttackValue)
		{
			setUnitAIType(UNITAI_DEFENSE_AIR);
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						10/6/08			jdog5000		*/
	/* 																			*/
	/* 	Air AI																	*/
	/********************************************************************************/
		/* original BTS code
		if (airBombDefenses())
		{
			return;
		}

		if (GC.getGame().getSorenRandNum(4, "AI Air Attack Move") == 0)
		{
			if (airBombPlots())
			{
				return;
			}
		}

		if (airStrike())
		{
			return;
		}

		if (m_unit->canAirAttack())
		{
			if (airOffensiveCity())
			{
				return;
			}
		}

		if (m_unit->canRecon())
		{
			if (exploreAir())
			{
				return;
			}
		}

		if (m_unit->canAirDefend())
		{
			m_unit->getGroup()->pushMission(MISSION_AIRPATROL);
			return;
		}
		*/
	bool bDefensive = false;
	if (pArea != NULL)
	{
		bDefensive = pArea->getAreaAIType(m_unit->getTeam()) == AREAAI_DEFENSIVE;
	}

	if (GC.getGame().getSorenRandNum(bDefensive ? 3 : 6, "AI Air Attack Move") == 0)
	{
		if (defensiveAirStrike())
		{
			return;
		}
	}

	if (GC.getGame().getSorenRandNum(4, "AI Air Attack Move") == 0)
	{
		// only moves unit in a fort
		if (travelToUpgradeCity())
		{
			return;
		}
	}

	// Support ground attacks
	if (airBombDefenses())
	{
		return;
	}

	if (GC.getGame().getSorenRandNum(bDefensive ? 6 : 4, "AI Air Attack Move") == 0)
	{
		if (airBombPlots())
		{
			return;
		}
	}

	if (airStrike())
	{
		return;
	}

	if (m_unit->canAirAttack())
	{
		if (airOffensiveCity())
		{
			return;
		}
	}
	else
	{
		if (m_unit->canAirDefend())
		{
			if (airDefensiveCity())
			{
				return;
			}
		}
	}

	// BBAI TODO: Support friendly attacks on common enemies, if low risk?

	if (m_unit->canAirDefend())
	{
		if (bDefensive || GC.getGame().getSorenRandNum(2, "AI Air Attack Move") == 0)
		{
			m_unit->getGroup()->pushMission(MISSION_AIRPATROL);
			return;
		}
	}

	if (m_unit->canRecon())
	{
		if (exploreAir())
		{
			return;
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/


	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::defenseAirMove()
{
	PROFILE_FUNC();

	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						10/21/08	Solver & jdog5000	*/
	/* 																			*/
	/* 	Air AI																	*/
	/********************************************************************************/
	CvCity* pCity = m_unit->plot()->getPlotCity();

	int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

	// includes forts
	if (m_unit->plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);

		if (3 * iEnemyOffense > 4 * iOurDefense || iOurDefense == 0)
		{
			// Too risky, pull out
			// airDefensiveCity will leave some air defense, pull extras out
			if (airDefensiveCity())
			{
				return;
			}
		}
		else if (iEnemyOffense > iOurDefense / 3)
		{
			if (m_unit->getDamage() > 0)
			{
				if (m_unit->healTurns(m_unit->plot()) > 1 + GC.getGame().getSorenRandNum(2, "AI Air Defense Move"))
				{
					// Can't help current situation, only risk losing unit
					if (airDefensiveCity())
					{
						return;
					}
				}

				// Stay to defend in the future
				m_unit->getGroup()->pushMission(MISSION_SKIP);
				return;
			}

			if (m_unit->canAirDefend() && pCity != NULL)
			{
				// Check for whether city needs m_unit unit to air defend
				if (!(pCity->AI_isAirDefended(true, -1)))
				{
					m_unit->getGroup()->pushMission(MISSION_AIRPATROL);
					return;
				}

				// Consider adding extra defenders
				if (!m_unit->getGroup()->hasCollateralDamage() && (!pCity->AI_isAirDefended(false, -2)))
				{
					if (GC.getGame().getSorenRandNum(3, "AI Air Defense Move") == 0)
					{
						m_unit->getGroup()->pushMission(MISSION_AIRPATROL);
						return;
					}
				}
			}

			// Attack the invaders!
			if (defendBaseAirStrike())
			{
				return;
			}

			if (defensiveAirStrike())
			{
				return;
			}

			if (airStrike())
			{
				return;
			}

			if (airDefensiveCity())
			{
				return;
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	if (m_unit->getDamage() > 0)
	{
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (GC.getGame().getSorenRandNum(4, "AI Air Defense Move") == 0)
	{
		// only moves unit in a fort
		if (travelToUpgradeCity())
		{
			return;
		}
	}

	if (m_unit->canAirDefend())
	{
		// Check for whether city needs m_unit unit for base air defenses
		int iBaseAirDefenders = 0;

		if (iEnemyOffense > 0)
		{
			iBaseAirDefenders++;
		}

		if (pCity != NULL)
		{
			iBaseAirDefenders += pCity->AI_neededAirDefenders() / 2;
		}

		if (m_unit->plot()->countAirInterceptorsActive(m_unit->getTeam()) < iBaseAirDefenders)
		{
			m_unit->getGroup()->pushMission(MISSION_AIRPATROL);
			return;
		}
	}

	CvArea* pArea = m_unit->area();
	bool bDefensive = false;
	bool bOffensive = false;

	if (pArea != NULL)
	{
		bDefensive = (pArea->getAreaAIType(m_unit->getTeam()) == AREAAI_DEFENSIVE);
		bOffensive = (pArea->getAreaAIType(m_unit->getTeam()) == AREAAI_OFFENSIVE);
	}

	if ((iEnemyOffense > 0) || bDefensive)
	{
		if (m_unit->canAirDefend())
		{
			if (pCity != NULL)
			{
				// Consider adding extra defenders
				if (!(pCity->AI_isAirDefended(false, -1)))
				{
					if ((GC.getGame().getSorenRandNum((bOffensive ? 3 : 2), "AI Air Defense Move") == 0))
					{
						m_unit->getGroup()->pushMission(MISSION_AIRPATROL);
						return;
					}
				}
			}
			else
			{
				if ((GC.getGame().getSorenRandNum((bOffensive ? 3 : 2), "AI Air Defense Move") == 0))
				{
					m_unit->getGroup()->pushMission(MISSION_AIRPATROL);
					return;
				}
			}
		}

		if ((GC.getGame().getSorenRandNum(3, "AI Air Defense Move") > 0))
		{
			if (defensiveAirStrike())
			{
				return;
			}

			if (airStrike())
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
			if (airBombDefenses())
			{
				return;
			}

			if (GC.getGame().getSorenRandNum(3, "AI Air Defense Move") == 0)
			{
				// Hit enemy land stacks near our cities
				if (defensiveAirStrike())
				{
					return;
				}
			}

			if (airStrike())
			{
				return;
			}

			if (getBirthmark() % 2 == 0 || bOffensive)
			{
				if (airBombPlots())
				{
					return;
				}
			}
		}
	}

	if (airDefensiveCity())
	{
		return;
	}

	// BBAI TODO: how valuable is recon information to AI in war time?
	if (m_unit->canRecon())
	{
		if (GC.getGame().getSorenRandNum(bDefensive ? 6 : 3, "AI defensive air recon") == 0)
		{
			if (exploreAir())
			{
				return;
			}
		}
	}
	/********************************************************************************/
	/* 	BETTER_BTS_AI_MOD						END								*/
	/********************************************************************************/

	if (m_unit->canAirDefend())
	{
		m_unit->getGroup()->pushMission(MISSION_AIRPATROL);
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::carrierAirMove()
{
	PROFILE_FUNC();

	// XXX maybe protect land troops?

	if (m_unit->getDamage() > 0)
	{
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (m_unit->isCargo())
	{
		int iRand = GC.getGame().getSorenRandNum(3, "AI Air Carrier Move");

		if (iRand == 2 && m_unit->canAirDefend())
		{
			m_unit->getGroup()->pushMission(MISSION_AIRPATROL);
			return;
		}
		else if (airBombDefenses())
		{
			return;
		}
		else if (iRand == 1)
		{
			if (airBombPlots())
			{
				return;
			}

			if (airStrike())
			{
				return;
			}
		}
		else
		{
			if (airStrike())
			{
				return;
			}

			if (airBombPlots())
			{
				return;
			}
		}

		if (travelToUpgradeCity())
		{
			return;
		}

		if (m_unit->canAirDefend())
		{
			m_unit->getGroup()->pushMission(MISSION_AIRPATROL);
			return;
		}
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (airCarrier())
	{
		return;
	}

	if (airDefensiveCity())
	{
		return;
	}

	if (m_unit->canAirDefend())
	{
		m_unit->getGroup()->pushMission(MISSION_AIRPATROL);
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitDefaultAI::missileAirMove()
{
	PROFILE_FUNC();

	// includes forts
	if (!m_unit->isCargo() && m_unit->plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

		if (iEnemyOffense > (iOurDefense / 2) || iOurDefense == 0)
		{
			if (airOffensiveCity())
			{
				return;
			}
		}
	}

	if (m_unit->isCargo())
	{
		int iRand = GC.getGame().getSorenRandNum(3, "AI Air Missile m_unit->plot bombing");
		if (iRand != 0)
		{
			if (airBombPlots())
			{
				return;
			}
		}

		iRand = GC.getGame().getSorenRandNum(3, "AI Air Missile Carrier Move");
		if (iRand == 0)
		{
			if (airBombDefenses())
			{
				return;
			}

			if (airStrike())
			{
				return;
			}
		}
		else
		{
			if (airStrike())
			{
				return;
			}

			if (airBombDefenses())
			{
				return;
			}
		}

		if (airBombPlots())
		{
			return;
		}

		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (airStrike())
	{
		return;
	}

	if (missileLoad(UNITAI_MISSILE_CARRIER_SEA))
	{
		return;
	}

	if (missileLoad(UNITAI_RESERVE_SEA, 1))
	{
		return;
	}

	if (missileLoad(UNITAI_ATTACK_SEA, 1))
	{
		return;
	}

	if (airBombDefenses())
	{
		return;
	}

	if (!m_unit->isCargo())
	{
		if (airOffensiveCity())
		{
			return;
		}
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitDefaultAI::healerMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	CvPlot* pPlot = m_unit->plot();

	if (pPlot != NULL && exposedToDanger(pPlot, 80))
	{
		OutputDebugString("	...plot is dangerous - seeking safety\n");

		if (safety())
		{
			return;
		}
	}

	upgrade();
	if (m_unit->isDelayedDeath())
	{
		//	Upgrade of original unit was successful
		return;
	}

	if (pPlot != NULL)
	{
		if (fulfillImmediateHealerNeed(pPlot))
		{
			return;
		}

		if (processContracts())
		{
			return;
		}

		if (fulfillCityHealerNeed(pPlot))
		{
			return;
		}

		if (fulfillHealerNeed(pPlot))
		{
			return;
		}
	}

	if (moveIntoNearestOwnedCity())
	{
		return;
	}

	if (pPlot != NULL)
	{
		if (m_unit->getGroup()->pushMissionInternal(MISSION_SKIP, pPlot->getX(), pPlot->getY(), 0, false, false, MISSIONAI_HEAL_SUPPORT, pPlot))
			return;
	}
	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitDefaultAI::healerSeaMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	CvPlot* pPlot = m_unit->plot();
	if (pPlot != NULL && pPlot->isCity(true))
	{
		int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

		if (m_unit->getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			if (retreatToCity(true))
			{
				return;
			}

			if (retreatToCity())
			{
				return;
			}

			if (safety())
			{
				return;
			}
		}
	}

	if (heal(50))
	{
		return;
	}

	upgrade();
	if (m_unit->isDelayedDeath())
	{
		//	Upgrade of original unit was successful
		return;
	}

	if (travelToUpgradeCity())
	{
		return;
	}

	if (pPlot != NULL)
	{
		if (fulfillImmediateHealerNeed(pPlot))
		{
			return;
		}

		if (processContracts())
		{
			return;
		}

		if (fulfillCityHealerNeed(pPlot))
		{
			return;
		}

		if (fulfillHealerNeed(pPlot))
		{
			return;
		}
	}

	if (moveIntoNearestOwnedCity())
	{
		return;
	}

	if (pPlot != NULL)
	{
		if (m_unit->getGroup()->pushMissionInternal(MISSION_SKIP, pPlot->getX(), pPlot->getY(), 0, false, false, MISSIONAI_HEAL_SUPPORT, pPlot))
		{
			return;
		}
	}
	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitDefaultAI::propertyControlMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	if (exposedToDanger(m_unit->plot(), 80))
	{
		OutputDebugString("	...plot is dangerous - seeking safety\n");

		if (m_unit->plot()->getOwner() != m_unit->getOwner())
		{
			if (safety())
			{
				return;
			}
		}
		else
		{
			if (moveIntoNearestOwnedCity())
			{
				return;
			}
		}
	}

	upgrade();
	if (m_unit->isDelayedDeath())
	{
		//	Upgrade of original unit was successful
		return;
	}

	if (artistCultureVictoryMove())
	{
		return;
	}

	if (construct())
	{
		return;
	}

	if (arrest())
	{
		return;
	}

	if (fulfillPropertyControlNeed())
	{
		return;
	}

	if (processContracts())
	{
		return;
	}

	if (moveIntoNearestOwnedCity())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitDefaultAI::propertyControlSeaMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	if (m_unit->plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

		if (m_unit->getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			if (retreatToCity(true))
			{
				return;
			}

			if (retreatToCity())
			{
				return;
			}

			if (safety())
			{
				return;
			}
		}
	}

	if (heal(50))
	{
		return;
	}

	upgrade();
	if (m_unit->isDelayedDeath())
	{
		//	Upgrade of original unit was successful
		return;
	}

	if (travelToUpgradeCity())
	{
		return;
	}

	if (fulfillPropertyControlNeed())
	{
		return;
	}

	if (moveIntoNearestOwnedCity())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitDefaultAI::BarbCriminalMove()
{
	//consider assassination
	if (anyAttack(0, 40))
	{
		return;
	}
	// Find enemy city if not already in a city
	if (!m_unit->plot()->isCity(false) && m_unit->getGroup()->isHasPathToAreaEnemyCity(MOVE_IGNORE_DANGER | MOVE_THROUGH_ENEMY, true))
	{
		return;
	}
	//consider if they are wanted to move out of sight of player who spotted them
	//consider to return to nearest non-barb city if not already in one
	//then just skip (pick a build up) and do as much crime damage as they can.
	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitDefaultAI::InvestigatorMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	if (isNegativePropertyUnit())
	{
		setUnitAIType(UNITAI_PILLAGE);
		return;
	}

	if (exposedToDanger(m_unit->plot(), 80))
	{
		OutputDebugString("	...plot is dangerous - seeking safety\n");

		if (m_unit->plot()->getOwner() != m_unit->getOwner())
		{
			if (safety())
			{
				return;
			}
		}
		else
		{
			if (moveIntoNearestOwnedCity())
			{
				return;
			}
		}
	}

	if (m_unit->plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

		if (m_unit->getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			if (retreatToCity(true))
			{
				return;
			}

			if (retreatToCity())
			{
				return;
			}

			if (safety())
			{
				return;
			}
		}
	}

	if (heal(50))
	{
		return;
	}

	upgrade();
	if (m_unit->isDelayedDeath())
	{
		//	Upgrade of original unit was successful
		return;
	}

	if (travelToUpgradeCity())
	{
		return;
	}

	//Arrest if possible HERE
	if (arrest())
	{
		return;
	}

	if (processContracts())
	{
		return;
	}

	if (InvestigatorFulfillment())
	{
		return;
	}

	if (moveIntoNearestOwnedCity())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP, m_unit->getX(), m_unit->getY(), 0, false, false, MISSIONAI_INVESTIGATOR_MAINTAIN, m_unit->plot());
	return;
}

void CvUnitDefaultAI::InfiltratorMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	upgrade();
	if (m_unit->isDelayedDeath())
	{
		// Upgrade of original unit was successful
		return;
	}

	// if the unit is solo, and not wanted, it may be good for joining with a group
	if (m_unit->getGroup()->getNumUnits() == 1 && !m_unit->isWanted() && processContracts())
	{
		return;
	}

	if (anyAttack(0, 75))
	{
		return;
	}

	if (!m_unit->isWanted() && GC.getGame().isOption(GAMEOPTION_UNLIMITED_NATIONAL_UNITS) && !m_unit->isNPC()
	&& m_unit->plot() != NULL && m_unit->plot()->isCity(false) && m_unit->plot()->getOwner() != m_unit->getOwner()
	&& !GET_PLAYER(m_unit->getOwner()).AI_isFinancialTrouble())
	{
		int iTargetGroupSize = std::max(1, getBirthmark() / 10);
		while (iTargetGroupSize > 9)
		{
			iTargetGroupSize = std::max(1, iTargetGroupSize / 10);
		}//get a number from 1-9.

		int iGroupSize = m_unit->getGroup()->getNumUnits();
		int iMaxInfiltrationForceSize = 4 * (1 + (int)GET_PLAYER(m_unit->getOwner()).getCurrentEra());
		int iExistingInfiltrationForceSize = GET_PLAYER(m_unit->getOwner()).AI_totalAreaUnitAIs(m_unit->area(), UNITAI_INFILTRATOR);
		if (iGroupSize < iTargetGroupSize && iExistingInfiltrationForceSize <= iMaxInfiltrationForceSize)
		{
			GET_PLAYER(m_unit->getOwner()).getContractBroker().advertiseWork
			(
				HIGH_PRIORITY_ESCORT_PRIORITY,
				NO_UNITCAPABILITIES,
				m_unit->getX(), m_unit->getY(),
				m_unit, UNITAI_INFILTRATOR
			);
			if (gUnitLogLevel > 2)
			{
				logBBAI("	%S's Infiltrator (%d) at (%d,%d) [stack size %d] requests another Infiltrator at priority %d", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getID(), m_unit->getX(), m_unit->getY(), m_unit->getGroup()->getNumUnits(), HIGH_PRIORITY_ESCORT_PRIORITY);
			}
		}
	}

	// Then one by one they would end up hitting m_unit point, theoretically, and possibly even the head unit immediately
	if (m_unit->isWanted() && m_unit->getGroup()->getNumUnits() == 1)
	{
		if (GET_PLAYER(m_unit->getOwner()).AI_isFinancialTrouble() && m_unit->canTrade(m_unit->plot()))
		{
			m_unit->getGroup()->pushMission(MISSION_TRADE);
			return;
		}
		else if (m_unit->canInfiltrate(m_unit->plot()))
		{
			m_unit->getGroup()->pushMission(MISSION_INFILTRATE);
			return;
		}
		else if (moveIntoNearestOwnedCity())
		{
			return;
		}
	}

	// Am I in rival land?
	const TeamTypes ePlotTeam = m_unit->plot()->getTeam();
	if (ePlotTeam != NO_TEAM && ePlotTeam < MAX_PC_PLAYERS && ePlotTeam != m_unit->getTeam()
	&& GET_TEAM(m_unit->getTeam()).AI_getAttitudeWeight(ePlotTeam) < 0)
	{
		if (m_unit->plot()->isCity(false))
		{
			// Toffer - Added gamespeed scaling to sabotage and pillage here.
			if (m_unit->getGroup()->isInvisible(ePlotTeam)
			&& (
				GC.getGame().getSorenRandNum(
					5 +
					7 * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent() / 100,
					"Leave city to pillage"
				) == 0
				) && pillageRange(1, 20))
			{
				return;
			}
			//if inside a city, check to see if there's VERY good cause to sabotage production or steal plans.
			//Apparently the AI never steals plans soooo... leave that one alone.
			const int iProduction = m_unit->plot()->getPlotCity()->getProduction();
			if (iProduction > 0 && m_unit->plot()->getPlotCity()->getProductionTurnsLeft() < 3)
			{
				// Only enact when it is a long build that has been heavily invested into already
				const int iEstTurns = m_unit->plot()->getPlotCity()->getProductionNeeded() / iProduction;
				const int iLongBuild = 1 + 6 * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent() / 100;

				if (iEstTurns >= iLongBuild)
				{
					m_unit->joinGroup(NULL); // do it alone
					if (m_unit->getGroup()->pushMissionInternal(MISSION_SABOTAGE))
					{
						return;
					}
				}
			}

			//Otherwise sit there and cause crime rot.  Simple for now.
			m_unit->getGroup()->pushMission(MISSION_SKIP, m_unit->getX(), m_unit->getY(), 0, false, false, MISSIONAI_INFILTRATOR_MAINTAIN, m_unit->plot());
			return;
		}
		else if (m_unit->getGroup()->isInvisible(ePlotTeam)
		&& (m_unit->plot()->getImprovementType() != NO_IMPROVEMENT && pillage() || pillageRange(2, 20)))
		{
			return;
		}
	}

	if (moveIntoCity(0, true))
	{
		return;
	}
	//if all m_unit has failed, go more complicated and work like a pillage AI.
	pillageMove();
}


void CvUnitDefaultAI::SeeInvisibleMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	if (isNegativePropertyUnit())
	{
		setUnitAIType(UNITAI_PILLAGE);
		return;
	}

	if (exposedToDanger(m_unit->plot(), 80))
	{
		OutputDebugString("	...plot is dangerous - seeking safety\n");

		if (m_unit->plot()->getOwner() != m_unit->getOwner())
		{
			if (safety())
			{
				return;
			}
		}
		else if (moveIntoNearestOwnedCity())
		{
			return;
		}
	}

	if (m_unit->plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
		int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

		if (m_unit->getDamage() > 0)	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if (iEnemyOffense > iOurDefense / 4 || iOurDefense == 0) //prioritize getting outta there
		{
			if (retreatToCity(true))
			{
				return;
			}

			if (retreatToCity())
			{
				return;
			}

			if (safety())
			{
				return;
			}
		}
	}

	if (heal(50))
	{
		return;
	}

	upgrade();
	if (m_unit->isDelayedDeath())
	{
		//	Upgrade of original unit was successful
		return;
	}

	if (travelToUpgradeCity())
	{
		return;
	}

	if (processContracts())
	{
		return;
	}

	if (!m_unit->plot()->isCity(true, m_unit->getTeam()))
	{
		if (moveIntoNearestOwnedCity())
		{
			return;
		}
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP, m_unit->getX(), m_unit->getY(), 0, false, false, MISSIONAI_SEE_INVISIBLE_MAINTAIN, m_unit->plot());
	return;

}

void CvUnitDefaultAI::SeeInvisibleSeaMove()
{
	PROFILE_FUNC();

	if (selectStatus(true))
	{
		return;
	}

	if (isNegativePropertyUnit())
	{
		setUnitAIType(UNITAI_PIRATE_SEA);
		return;
	}

	if (exposedToDanger(m_unit->plot(), 80))
	{
		OutputDebugString("	...plot is dangerous - seeking safety\n");

		if (m_unit->plot()->getOwner() != m_unit->getOwner())
		{
			if (safety())
			{
				return;
			}
		}
		else
		{
			if (moveIntoNearestOwnedCity())
			{
				return;
			}
		}
	}

	if (heal(50))
	{
		return;
	}

	upgrade();
	if (m_unit->isDelayedDeath())
	{
		//	Upgrade of original unit was successful
		return;
	}

	if (travelToUpgradeCity())
	{
		return;
	}

	if (processContracts())
	{
		return;
	}

	if (!m_unit->plot()->isCity(true, m_unit->getTeam()))
	{
		if (moveIntoNearestOwnedCity())
		{
			return;
		}
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;

}

void CvUnitDefaultAI::EscortMove()
{
	PROFILE_FUNC();
	//TBTask: These need to be inspired to go join workers and settlers that need escorting - the request from the workers have been removed to correct long periods of getting stuck before they could move on and do more.

	if (selectStatus(true))
	{
		return;
	}

	if (heal(50))
	{
		return;
	}

	upgrade();
	if (m_unit->isDelayedDeath())
	{
		//	Upgrade of original unit was successful
		return;
	}

	if (travelToUpgradeCity())
	{
		return;
	}

	if (processContracts())
	{
		return;
	}

	if (moveIntoNearestOwnedCity())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
}
bool CvUnitDefaultAI::retreatIfCantDefend() {
	return (!m_unit->getGroup()->canDefend() && GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(m_unit->plot()) && retreatToCity());
}

void CvUnitDefaultAI::networkAutomated()
{
	FAssertMsg(m_unit->canBuildRoute(), "canBuildRoute is expected to be true");

	if (retreatIfCantDefend())
	{
		return;
	}

	if (connectBonus() || connectCity())
	{
		return;
	}

	if (!GET_PLAYER(m_unit->getOwner()).isModderOption(MODDEROPTION_INFRASTRUCTURE_IGNORES_IMPROVEMENTS)
	&& CvWorkerService::ImproveBonus(m_unit, 2))
	{
		return;
	}

	if (routeTerritory(true) || connectBonus(false))
	{
		return;
	}

	if (routeCity() || routeTerritory())
	{
		return;
	}

	if (retreatToCity() || safety())
	{
		return;
	}
	m_unit->getGroup()->pushMission(MISSION_SKIP);
}


void CvUnitDefaultAI::cityAutomated()
{
	if (!m_unit->getGroup()->canDefend() && GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(m_unit->plot())
	// XXX maybe not do m_unit??? could be working productively somewhere else...
	&& retreatToCity())
	{
		return;
	}
	CvCity* pCity = m_unit->plot()->getOwner() == m_unit->getOwner() ? m_unit->plot()->getWorkingCity() : NULL;

	if (pCity == NULL)
	{
		pCity = GC.getMap().findCity(m_unit->getX(), m_unit->getY(), m_unit->getOwner()); // XXX do team???
	}

	if (pCity != NULL && improveCity(pCity))
	{
		return;
	}

	if (retreatToCity() || safety())
	{
		return;
	}
	m_unit->getGroup()->pushMission(MISSION_SKIP);
}


// XXX make sure we include any new UnitAITypes...
int CvUnitDefaultAI::promotionValue(PromotionTypes ePromotion) const
{
	return GET_PLAYER(m_unit->getOwner()).AI_promotionValue(ePromotion, m_unit->getUnitType(), m_unit, getUnitAIType());
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::shadow(UnitAITypes eUnitAI, int iMax, int iMaxRatio, bool bWithCargoOnly, bool bOutsideCityOnly, int iMaxPath)
{
	PROFILE_FUNC();

	const DomainTypes domain = m_unit->getDomainType();
	const int iBaseMoves = m_unit->getGroup()->baseMoves();

	CvUnit* pBestUnit = NULL;
	int iBestValue = 0;
	foreach_(CvUnit * pLoopUnit, GET_PLAYER(m_unit->getOwner()).units())
	{
		if (pLoopUnit != m_unit && pLoopUnit->isGroupHead()
		&& pLoopUnit->getDomainType() == domain && !pLoopUnit->isCargo()
		&& pLoopUnit->AI()->getUnitAIType() == eUnitAI
		&& plotValid(pLoopUnit->plot())
		&& pLoopUnit->getGroup()->baseMoves() <= iBaseMoves
		&& (!bWithCargoOnly || pLoopUnit->getGroup()->hasCargo())
		&& (!bOutsideCityOnly || !pLoopUnit->plot()->isCity()))
		{
			const int iShadowerCount = GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(pLoopUnit, MISSIONAI_SHADOW, m_unit->getGroup());
			if ((-1 == iMax || iShadowerCount < iMax)
			&& (-1 == iMaxRatio || iShadowerCount == 0 || 100 * iShadowerCount / std::max(1, pLoopUnit->getGroup()->countNumUnitAIType(eUnitAI)) <= iMaxRatio)
			&& !pLoopUnit->plot()->isVisibleEnemyUnit(m_unit))
			{
				int iPathTurns;
				if (m_unit->generatePath(pLoopUnit->plot(), 0, true, &iPathTurns) && iPathTurns <= iMaxPath)
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
		if (m_unit->atPlot(pBestUnit->plot()))
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_SHADOW, NULL, pBestUnit);
			return true;
		}
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO_UNIT, pBestUnit->getOwner(), pBestUnit->getID(), 0, false, false, MISSIONAI_SHADOW, NULL, pBestUnit);
	}
	return false;
}


namespace {
	int unitImpassableCount(const CvUnit* unit)
	{
		return GET_PLAYER(unit->getOwner()).AI_unitImpassableCount(unit->getUnitType());
	}
}

bool CvUnitDefaultAI::group(const GroupingParams& params)
{
	PROFILE_FUNC();

	// if we are on a transport, then do not regroup
	if (m_unit->isCargo()
		|| !(params.bAllowRegrouping || params.bMergeWholeGroup) && m_unit->getGroup()->getNumUnits() > 1
		|| m_unit->getDomainType() == DOMAIN_LAND && !m_unit->canMoveAllTerrain() && m_unit->area()->getNumAIUnits(m_unit->getOwner(), params.eUnitAI) == 0
		|| !canGroupWithAIType(params.eUnitAI)
		|| GET_PLAYER(m_unit->getOwner()).AI_getNumAIUnits(params.eUnitAI) == 0
		)
	{
		return false;
	}

	const int iOurImpassableCount = algo::max_element(m_unit->getGroup()->units() | transformed(unitImpassableCount)).get_value_or(0);

	int iBestValue = MAX_INT;
	CvUnit* pBestUnit = NULL;

	const bool bCanDefend = m_unit->getGroup()->canDefend();
	const int groupExtra = params.bStackOfDoom ? stackOfDoomExtra() : 0;
	const int ourGroupSizeWithoutUs = m_unit->getGroup()->getNumUnits() - 1;

	CvReachablePlotSet plotSet(m_unit->getGroup(), bCanDefend ? 0 : MOVE_OUR_TERRITORY, searchRange(params.iMaxPath));

	// Loop over groups, allowGroup blocks non-head units anyway
	foreach_(CvSelectionGroup * group, GET_PLAYER(m_unit->getOwner()).groups_non_empty())
	{
		CvUnit* unit = group->getHeadUnit();
		if (plotSet.find(unit->plot()) != plotSet.end()
			&& (params.iMaxPath > 0 || unit->plot() == m_unit->plot())
			&& !m_unit->isEnemy(unit->plot()->getTeam())
			&& (!params.bBiggerGroupOnly || group->getNumUnits() > ourGroupSizeWithoutUs)
			&& allowGroup(unit, params.eUnitAI)
			&& (params.iMaxOwnUnitAI == -1 || group->countNumUnitAIType(getUnitAIType()) <= params.iMaxOwnUnitAI + groupExtra)
			&& (params.iMinUnitAI == -1 || group->countNumUnitAIType(params.eUnitAI) >= params.iMinUnitAI)
			&& (!params.bIgnoreFaster || group->baseMoves() <= m_unit->baseMoves())
			&& (!params.bIgnoreOwnUnitType || unit->getUnitType() != m_unit->getUnitType())
			&& (!params.bWithCargoOnly || unit->getGroup()->hasCargo())
			&& (!params.bInCityOnly || unit->plot()->isCity())
			&& (params.eIgnoreMissionAIType == NO_MISSIONAI || params.eIgnoreMissionAIType != unit->getGroup()->AI_getMissionAIType())
			&& !unit->plot()->isVisibleEnemyUnit(m_unit)
			&& (
				(iOurImpassableCount == 0 && getUnitAIType() != UNITAI_ASSAULT_SEA)
				||
				iOurImpassableCount == algo::max_element(group->units() | transformed(unitImpassableCount)).get_value_or(0)
				)
			&& (
				params.iMaxGroup == -1
				||
				group->getNumUnits() + GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(unit, MISSIONAI_GROUP, m_unit->getGroup()) <= params.iMaxGroup + groupExtra
				)
			)
		{
			int iPathTurns;
			if (m_unit->generatePath(unit->plot(), bCanDefend ? 0 : MOVE_OUR_TERRITORY, true, &iPathTurns))
			{
				if (iPathTurns <= params.iMaxPath)
				{
					int iValue = 1000 * (iPathTurns + 1);
					iValue *= 4 + group->getCargo();
					// just a count
					iValue /= group->getNumUnits();

					if (iValue < iBestValue)
					{
						iBestValue = iValue;
						pBestUnit = unit;
					}
				}
			}
		}
	}

	if (pBestUnit != NULL)
	{
		if (m_unit->atPlot(pBestUnit->plot()))
		{
			if (params.bMergeWholeGroup)
			{
				m_unit->getGroup()->mergeIntoGroup(pBestUnit->getGroup());
			}
			else
			{
				m_unit->joinGroup(pBestUnit->getGroup());
			}
			return true;
		}
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO_UNIT, pBestUnit->getOwner(), pBestUnit->getID(), bCanDefend ? 0 : (MOVE_RECONSIDER_ON_LEAVING_OWNED | MOVE_OUR_TERRITORY | MOVE_WITH_CAUTION), false, false, MISSIONAI_GROUP, NULL, pBestUnit);
	}
	return false;
}


bool CvUnitDefaultAI::groupMergeRange(UnitAITypes eUnitAI, int iMaxRange, bool bBiggerOnly, bool bAllowRegrouping, bool bIgnoreFaster)
{
	PROFILE_FUNC();

	// if we are on a transport, then do not regroup
	if (m_unit->isCargo())
	{
		return false;
	}

	if (!bAllowRegrouping)
	{
		if (m_unit->getGroup()->getNumUnits() > 1)
		{
			return false;
		}
	}

	if ((m_unit->getDomainType() == DOMAIN_LAND) && !m_unit->canMoveAllTerrain())
	{
		if (m_unit->area()->getNumAIUnits(m_unit->getOwner(), eUnitAI) == 0)
		{
			return false;
		}
	}

	if (!canGroupWithAIType(eUnitAI))
	{
		return false;
	}

	const CvPlot* pPlot = m_unit->plot();
	CvSelectionGroup* pGroup = m_unit->getGroup();

	// best match
	const CvUnit* pBestUnit = NULL;
	int iBestValue = MAX_INT;
	// iterate over plots at each range
	foreach_(const CvPlot * pLoopPlot, pPlot->rect(iMaxRange, iMaxRange))
	{
		if (pLoopPlot->getArea() == pPlot->getArea() && plotValid(pLoopPlot))
		{
			foreach_(const CvUnit * pLoopUnit, pLoopPlot->units())
			{
				const CvSelectionGroup* pLoopGroup = pLoopUnit->getGroup();

				if (allowGroup(pLoopUnit, eUnitAI))
				{
					if (bIgnoreFaster || (pLoopUnit->getGroup()->baseMoves() <= m_unit->baseMoves()))
					{
						if (!bBiggerOnly || (pLoopGroup->getNumUnits() > pGroup->getNumUnits()))
						{
							int iPathTurns;
							if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns, iMaxRange + 2))
							{
								if (iPathTurns <= (iMaxRange + 2))
								{
									int iValue = 1000 * (iPathTurns + 1);
									iValue /= pLoopGroup->getNumUnits();

									if (iValue < iBestValue)
									{
										iBestValue = iValue;
										pBestUnit = pLoopUnit;
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
		if (m_unit->atPlot(pBestUnit->plot()))
		{
			pGroup->mergeIntoGroup(pBestUnit->getGroup());
			return true;
		}
		return pGroup->pushMissionInternal(MISSION_MOVE_TO_UNIT, pBestUnit->getOwner(), pBestUnit->getID(), 0, false, false, MISSIONAI_GROUP, NULL, pBestUnit);
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
			&& loadingPlot->isOwned()
			&& targetUnit->isPotentialEnemy(loadingPlot->getTeam(), loadingPlot)
			&& !targetUnit->isNPC()
			&& loadingPlot->area()->getCitiesPerPlayer(loadingPlot->getOwner()) > 0
			)
		{
			// Transport cannot enter land m_unit->plot without cargo, so generate path only works properly if
			// land units are already loaded
			return algo::any_of(
				targetUnit->plot()->adjacent() | filtered(CvPlot::fn::isWater()),
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
// it appears to me that m_unit is fully handled at m_unit point.
// still need to do a clean sweep search for all references to load but I THINK it's taken care of at m_unit point.
// the main thing to look for would be unadapted non -1 references to iMinCargo, iMinCargoSpace and iMaxCargoSpace
bool CvUnitDefaultAI::load(UnitAITypes eUnitAI, MissionAITypes eMissionAI, UnitAITypes eTransportedUnitAI, int iMinCargo, int iMinCargoSpace, int iMaxCargoSpace, int iMaxCargoOurUnitAI, int iFlags, int iMaxPath, int iMaxTransportPath)
{
	PROFILE_FUNC();


	if (m_unit->hasCargo())
	{
		return false;
	}

	if (m_unit->isCargo())
	{
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return true;
	}

	if ((m_unit->getDomainType() == DOMAIN_LAND) && !m_unit->canMoveAllTerrain())
	{
		if (m_unit->area()->getNumAIUnits(m_unit->getOwner(), eUnitAI) == 0)
		{
			return false;
		}
	}

	const int iLoadMissionAICount = 4;
	// setting up a value so that we may loop through the following predefined array

	MissionAITypes aeLoadMissionAI[iLoadMissionAICount] = { MISSIONAI_LOAD_ASSAULT, MISSIONAI_LOAD_SETTLER, MISSIONAI_LOAD_SPECIAL, MISSIONAI_ATTACK_SPY };

	int iCurrentGroupSize = m_unit->getGroup()->getNumUnits();
	// Under Size Matters we need the volume of the group
	if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		iCurrentGroupSize = m_unit->getGroup()->getNumUnitCargoVolumeTotal();
	}

	CvUnit* bestTransportUnit = nullptr;
	int bestTransportUnitScore = MAX_INT;
	{
		PROFILE("CvUnitDefaultAI::load.SearchTransport");

		CvReachablePlotSet plotSet(m_unit->getGroup(), iFlags, iMaxPath);

		foreach_(CvUnit * pLoopUnit, GET_PLAYER(m_unit->getOwner()).units())
		{
			// if the m_unit->plot the unit under evaluation is on isn't the m_unit->plot it was headed for?
			// if m_unit unit can be loaded onto the looped unit
			if (plotSet.find(pLoopUnit->plot()) != plotSet.end()
				&& m_unit->canLoadOntoUnit(pLoopUnit, pLoopUnit->plot()))
			{
				// special case ASSAULT_SEA UnitAI, so that, if a unit is marked escort, but can load units, it will load them
				// transport units might have been built as escort, m_unit most commonly happens with galleons
				UnitAITypes eLoopUnitAI = pLoopUnit->AI()->getUnitAIType();

				// eloopunitai is the ai of the unit under evaluation.

				// if the unit under evaluation is the type we were wanting to get loaded through m_unit command
				if (eLoopUnitAI == eUnitAI)// || (eUnitAI == UNITAI_ASSAULT_SEA && eLoopUnitAI == UNITAI_ESCORT_SEA))
				{
					int iCargoSpaceAvailable = pLoopUnit->cargoSpaceAvailable(m_unit->getSpecialUnitType(), m_unit->getDomainType());

					// iCargoSpaceAvailable refers to the space (in unit counts here) available to load units on the transport under eval
					if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
					{
						// here we make it refer to the space (in cargo volumes) available to load units on the transport instead
						iCargoSpaceAvailable -= GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(pLoopUnit, aeLoadMissionAI, iLoadMissionAICount, m_unit->getGroup(), -1, true);
						// here we must refer to things in a cargo volume manner rather than a unit count manner
						// This is asking to subtract out the number of units that already have planned to load onto m_unit ship
						// Adapted the structure to enable me to ask not for the number of units but for the total cargo volume OF those units
						// iCargoSpaceAvailable now refers to actual cargo space by volume rather than by unit count
					}
					else
					{
						iCargoSpaceAvailable -= GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(pLoopUnit, aeLoadMissionAI, iLoadMissionAICount, m_unit->getGroup());
					}

					// if there is cargo space available after the units that have spoken for the transport are loaded then:
					if (iCargoSpaceAvailable > 0)
					{
						// note m_unit may not be enough now because we're talking about cargo volumes that may still be greater than the amount available

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
									// And for cases where I only need a 'count' it's impossible to say m_unit generically.
									// Solved volumetrically - when option is on it takes the estimated best unit for the type it's trying to load and converts that unit's cargo volume * the iMinCargoSpace# previously defined.

									// max cargo space... what?  Don't load if there's too much space available on m_unit ship???
									// Apparently its to keep counter units escorting settlers and workers from overloading themselves
									if (iMaxCargoSpace == -1 || iCargoSpaceAvailable <= iMaxCargoSpace)
									{
										// Resolved by conditionally making m_unit value worth ((m_unit->SMCargoVolume()*2)-1) -
										// twice the cargo volume of the evaluating unit but one less so a second one can't load after it does

										// I presume iMaxCargoOurUnitAI is intended to keep more than one settler from getting on the same boat.
										// Assuming I'm correct, m_unit probably doesn't need to be addressed any differently as it's just going to be a count.
										if (iMaxCargoOurUnitAI == -1 || pLoopUnit->getUnitAICargo(getUnitAIType()) <= iMaxCargoOurUnitAI)
										{
											// Don't block city defense from getting on board
											if (!pLoopUnit->plot()->isVisibleEnemyUnit(m_unit))
											{
												CvPlot* pUnitTargetPlot = pLoopUnit->getGroup()->AI_getMissionAIPlot();
												if (pUnitTargetPlot == nullptr
													|| pUnitTargetPlot->getTeam() == m_unit->getTeam()
													|| !pUnitTargetPlot->isOwned()
													|| !m_unit->isPotentialEnemy(pUnitTargetPlot->getTeam(), pUnitTargetPlot))
												{
													int iPathTurns = 0;
													if (m_unit->atPlot(pLoopUnit->plot()) || m_unit->generatePath(pLoopUnit->plot(), iFlags, true, &iPathTurns, iMaxPath))
													{
														// prefer a transport that can hold as much of our group as possible
														const int score = std::max(0, iCurrentGroupSize - iCargoSpaceAvailable) * 5 + iPathTurns;

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
		PROFILE("CvUnitDefaultAI::load.SearchTarget");

		// Can transport reach enemy in requested time
		const int loadingRange = iMaxTransportPath * bestTransportUnit->baseMoves();
		if (!algo::any_of(
			m_unit->plot()->rect(loadingRange, loadingRange),
			bind(canTransportReachPlot, _1, m_unit, bestTransportUnit, iMaxTransportPath))
			)
		{
			bestTransportUnit = nullptr;
		}
	}

	if (bestTransportUnit != NULL)
	{
		if (m_unit->atPlot(bestTransportUnit->plot()))
		{
			CvSelectionGroup* pOtherGroup = NULL;
			// TBNOTE: fixed setTransportUnit for Size Matters (hopefully correctly)
			m_unit->getGroup()->setTransportUnit(bestTransportUnit, &pOtherGroup); // XXX is m_unit dangerous (not pushing a mission...) XXX air units?

			// If part of large group loaded, then try to keep loading the rest
			if (eUnitAI == UNITAI_ASSAULT_SEA && (eMissionAI == MISSIONAI_LOAD_ASSAULT || eMissionAI == MISSIONAI_LOAD_SETTLER))
			{
				if (pOtherGroup != NULL && pOtherGroup->getNumUnits() > 0)
				{
					if (pOtherGroup->getHeadUnitAI() == getUnitAIType())
					{
						pOtherGroup->getHeadUnit()->AI()->load(eUnitAI, eMissionAI, eTransportedUnitAI, iMinCargo, iMinCargoSpace, iMaxCargoSpace, iMaxCargoOurUnitAI, iFlags, 0, iMaxTransportPath);
					}
					else if (eTransportedUnitAI == NO_UNITAI && iMinCargo < 0 && iMinCargoSpace < 0 && iMaxCargoSpace < 0 && iMaxCargoOurUnitAI < 0)
					{
						pOtherGroup->getHeadUnit()->AI()->load(eUnitAI, eMissionAI, NO_UNITAI, -1, -1, -1, -1, iFlags, 0, iMaxTransportPath);
					}
				}
			}
			if (m_unit->isCargo())
			{
				return true;
			}
		}
		else
		{
			// BBAI TODO: To split or not to split?
			//Here is where it gets complicated again...
			const int iCargoSpaceAvailable = bestTransportUnit->cargoSpaceAvailable(m_unit->getSpecialUnitType(), m_unit->getDomainType());

			FAssertMsg(iCargoSpaceAvailable > 0, "best unit has no space");

			// split our group to fit on the transport
			CvSelectionGroup* pOtherGroup = NULL;
			//woooo... how to convert THIS???
			CvSelectionGroup* pSplitGroup = NULL;
			if (!GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
			{
				pSplitGroup = m_unit->getGroup()->splitGroup(iCargoSpaceAvailable, m_unit, &pOtherGroup);
			}
			else
			{//we'll have to just split the group into individual units and let each process from there
				//This strikes me as having a high chance of disrupting the intentions of the units... with or without SM!
				pSplitGroup = m_unit->getGroup()->splitGroup(1, m_unit, &pOtherGroup);
			}
			FAssertMsg(pSplitGroup != NULL, "splitGroup failed");
			FAssertMsg(m_unit->getGroupID() == pSplitGroup->getID(), "splitGroup failed to put unit in the new group");

			if (pSplitGroup != NULL)
			{
				CvPlot* pOldPlot = pSplitGroup->plot();
				pSplitGroup->pushMission(MISSION_MOVE_TO_UNIT, bestTransportUnit->getOwner(), bestTransportUnit->getID(), iFlags, false, false, eMissionAI, NULL, bestTransportUnit);
				bool bMoved = (pSplitGroup->plot() != pOldPlot);
				if (!bMoved && pOtherGroup != NULL)
				{
					m_unit->joinGroup(pOtherGroup);
				}
				return bMoved;
			}
		}
	}
	//TBNOTE: A rather high likelihood that I may want to make m_unit more advanced for SM - making the units that are too big to fit consider the option to split so they can get some of them onto the boats.  For now I think m_unit might not be a good idea because it'll end up with weak units getting off the boat.  Better to just need more boats.
	return false;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					   END												  */
/************************************************************************************************/

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::guardCityBestDefender()
{
	CvCity* pCity;
	CvPlot* pPlot;

	pPlot = m_unit->plot();
	pCity = pPlot->getPlotCity();

	if (pCity != NULL)
	{
		if (pCity->getOwner() == m_unit->getOwner())
		{
			if (pPlot->getBestDefender(m_unit->getOwner()) == m_unit)
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, NULL);
				m_unit->getGroup()->AI_setAsGarrison(pCity);
				return true;
			}
		}
	}

	return false;
}

bool CvUnitDefaultAI::guardCityMinDefender(bool bSearch)
{
	PROFILE_FUNC();

	CvCity* pPlotCity = m_unit->plot()->getPlotCity();
	if ((pPlotCity != NULL) && (pPlotCity->getOwner() == m_unit->getOwner()))
	{
		int iCityDefenderCount = pPlotCity->plot()->plotCount(PUF_isUnitAIType, UNITAI_CITY_DEFENSE, -1, NULL, m_unit->getOwner());
		if ((iCityDefenderCount - 1) < pPlotCity->AI_minDefenders())
		{
			//TB: Lets try it without m_unit ankle cutting check for a bit and see if they can put up a fight when invaded
			/*if ((iCityDefenderCount <= 2) || (GC.getGame().getSorenRandNum(5, "AI shuffle defender") != 0))*/
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, NULL);
				m_unit->getGroup()->AI_setAsGarrison(pPlotCity);
				return true;
			}
		}
	}

	if (bSearch)
	{
		int iBestValue = 0;
		const CvPlot* pBestPlot = NULL;
		const CvPlot* pBestGuardPlot = NULL;

		const int iCurrentTurn = GC.getGame().getGameTurn();
		foreach_(const CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
		{
			if (plotValid(pLoopCity->plot()))
			{
				/************************************************************************************************/
				/* BETTER_BTS_AI_MOD					  08/19/09								jdog5000	  */
				/*																							  */
				/* Unit AI, Efficiency																		  */
				/************************************************************************************************/
								// BBAI efficiency: check m_unit->area for land units
				if ((m_unit->getDomainType() == DOMAIN_LAND) && (pLoopCity->area() != m_unit->area()) && !(m_unit->getGroup()->canMoveAllTerrain()))
				{
					continue;
				}
				/************************************************************************************************/
				/* BETTER_BTS_AI_MOD					   END												  */
				/************************************************************************************************/

								//	Koshling - efficiency in handling babarins who have cities scattered all over
								//	the world - just search nearby ones
				if (m_unit->isNPC() && stepDistance(pLoopCity->getX(), pLoopCity->getY(), m_unit->getX(), m_unit->getY()) > 10)
				{
					continue;
				}

				int iDefendersHave = pLoopCity->plot()->plotCount(PUF_isUnitAIType, UNITAI_CITY_DEFENSE, -1, NULL, m_unit->getOwner());
				int iDefendersNeed = pLoopCity->AI_minDefenders();
				if (iDefendersHave < iDefendersNeed)
				{
					if (!(pLoopCity->plot()->isVisibleEnemyUnit(m_unit)))
					{
						iDefendersHave += GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_GUARD_CITY, m_unit->getGroup());
						if (iDefendersHave < iDefendersNeed + 1)
						{
							int iPathTurns;
							if (!m_unit->atPlot(pLoopCity->plot()) && m_unit->generatePath(pLoopCity->plot(), 0, true, &iPathTurns))
							{
								if (iPathTurns <= 10)
								{
									int iValue = (iDefendersNeed - iDefendersHave) * 20;
									iValue += 2 * std::min(15, iCurrentTurn - pLoopCity->getGameTurnAcquired());
									if (pLoopCity->isOccupation())
									{
										iValue += 5;
									}
									iValue -= iPathTurns;

									if (iValue > iBestValue)
									{
										iBestValue = iValue;
										pBestPlot = m_unit->getPathEndTurnPlot();
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
			if (m_unit->atPlot(pBestGuardPlot))
			{
				FAssert(pBestGuardPlot == pBestPlot);
				m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, NULL);
				return true;
			}
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_GUARD_CITY, pBestGuardPlot);
		}
	}

	return false;
}

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::guardCity(bool bLeave, bool bSearch, int iMaxPath)
{
	PROFILE_FUNC();
	FAssert(m_unit->getDomainType() == DOMAIN_LAND || m_unit->getDomainType() == DOMAIN_IMMOBILE);

	if (!m_unit->getGroup()->canDefend())
	{
		return false;
	}
	CvReachablePlotSet plotSet(m_unit->getGroup(), 0, 2);

	//	If we're already close to a city we may be considered part of it's defensive posture - check for those cases first
	for (int iPlotIndex = 0; iPlotIndex < NUM_CITY_PLOTS_2; iPlotIndex++)
	{
		CvPlot* pPlot = plotCity(m_unit->getX(), m_unit->getY(), iPlotIndex);

		if (pPlot != NULL && pPlot->getOwner() == m_unit->getOwner())
		{
			CvCity* pCity = pPlot->getPlotCity();

			if (pCity != NULL && plotSet.find(pPlot) != plotSet.end())
			{
				//	Check property control attributes first - they may cause us to defend in the city
				//	regardless of other conditions
				if (m_unit->getGroup()->AI_hasBeneficialPropertyEffectForCity(pCity))
				{
					//	We have at least one unit that can help the ciy's property control (aka crime usually)
					//	Split ou he best such unit and have it defend in the city
					CvSelectionGroup* pOldGroup = m_unit->getGroup();
					CvUnit* pEjectedUnit = m_unit->getGroup()->AI_ejectBestPropertyManipulator(pCity);

					FAssert(pEjectedUnit != NULL);
					pEjectedUnit->AI()->setUnitAIType(UNITAI_PROPERTY_CONTROL);

					if (m_unit->atPlot(pCity->plot()))
					{
						//	Mark the ejected unit as part of the city garrison
						pEjectedUnit->getGroup()->AI_setAsGarrison(pCity);
						pEjectedUnit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, NULL);
						return (pEjectedUnit->getGroup() == pOldGroup || pEjectedUnit == m_unit);
					}
					else if (pEjectedUnit->generatePath(pCity->plot(), 0, true))
					{
						//	Mark the ejected unit as part of the city garrison
						pEjectedUnit->getGroup()->AI_setAsGarrison(pCity);
						pEjectedUnit->getGroup()->pushMission(MISSION_MOVE_TO, pCity->getX(), pCity->getY(), 0, false, false, MISSIONAI_GUARD_CITY, NULL);
						return (pEjectedUnit->getGroup() == pOldGroup || pEjectedUnit == m_unit);
					}
					else
					{
						//	If we can't move after all regroup and continue regular defensive processing
						pEjectedUnit->joinGroup(pOldGroup);
					}
				}

				const int iPlotDanger2 = GET_PLAYER(m_unit->getOwner()).AI_getPlotDanger(pPlot, 2);

				int iExtra = 0;

				if (pCity->AI_isDanger())
				{
					iExtra -= iPlotDanger2;
				}
#if 0
				//	If THIS unit is not a city type, then count non-city types generally when evaluating defense
				//	or else any number of them will still keep them all locked up thinking more defense is needed
				if (!isCityAIType())
				{
					int iPlotAllStrength = m_unit->plot()->plotStrength(UNITVALUE_FLAGS_DEFENSIVE, PUF_canDefend, -1, -1, m_unit->getOwner(), NO_TEAM, NULL, -1, -1, 2);
					int iPlotCityAIStrength = m_unit->plot()->plotStrength(UNITVALUE_FLAGS_DEFENSIVE, PUF_canDefend, -1, -1, m_unit->getOwner(), NO_TEAM, PUF_isCityAIType, -1, -1, 2);

					iExtra += (iPlotAllStrength - iPlotCityAIStrength);
				}
#endif
				bool bDefend = false;

				//	Never remove the last unit
				if (m_unit->atPlot(pPlot) && m_unit->canDefend() && pPlot->plotCount(PUF_canDefend /*PUF_canDefendGroupHead*/, -1, -1, NULL, m_unit->getOwner()) == m_unit->getGroup()->getNumUnits()) // XXX check for other team's units?
				{
					bDefend = true;
				}
				//	Check if it is adequately defended (allowing for m_unit group leaving if it is thinking of doing so)
				//	and also that we don't need the units to maintain happiness
				else if (!(pCity->AI_isDefended(((!m_unit->canDefend() || !isCityGarrison(pCity)) ? 0 : -m_unit->getGroup()->AI_getGenericValueTimes100(UNITVALUE_FLAGS_DEFENSIVE) / 100) + iExtra)) ||
						 (m_unit->atPlot(pPlot) && m_unit->isMilitaryHappiness() && !(pCity->AI_isAdequateHappinessMilitary(-m_unit->getGroup()->getNumUnits()))))
				{
					bDefend = true;
				}
				else
				{
					//	We have enough units here even if we leave

					//	If we were asking to, leave just allow it
					if (!bLeave)
					{
						//	Otherwise leave the cityAI types defending and others can do what they wish
						if (isCityAIType())
						{
							bDefend = true;
						}
					}
				}
				if (bDefend)
				{
					//	Need to defend city vicinity but not necessarily the city itself
					bool bGuardInCity = !pCity->AI_isAdequateHappinessMilitary(-1);

					if (!bGuardInCity && m_unit->canDefend())
					{
						//	Always leave at least half the defense actually in the city, and if there is
						//	danger around keep more (to match the enemy count)
						int currentAllDefenders = pPlot->plotCount(PUF_canDefend, -1, -1, NULL, m_unit->getOwner(), NO_TEAM, NULL, -1, -1, 2);
						int currentAllCityAIDefenders = pPlot->plotCount(PUF_canDefend, -1, -1, NULL, m_unit->getOwner(), NO_TEAM, PUF_isCityAIType, -1, -1, 2);
						int currentInCityDefenders = pPlot->plotCount(PUF_canDefend, -1, -1, NULL, m_unit->getOwner(), NO_TEAM);
						int requiredInCityDefenders = std::max(iPlotDanger2, currentAllDefenders / 2);

						if (currentInCityDefenders <= (requiredInCityDefenders + (m_unit->atPlot(pPlot) ? m_unit->getGroup()->getNumUnits() : 0)))
						{
							//	Recall city AI types preferentially if there are enough
							if (currentAllCityAIDefenders < requiredInCityDefenders || isCityAIType())
							{
								bGuardInCity = true;
							}
						}
					}

					CvPlot* pBestPlot = NULL;
					CvPlot* pBestGuardPlot = NULL;

					//	If we don't have to guard in the city figure out the best spot
					if (!bGuardInCity)
					{
						//	Guard a good spot outside the city but in its vicinity
						int iBestValue = 0;

						foreach_(CvPlot * pLoopPlot, pPlot->rect(NUM_CITY_PLOTS_2, NUM_CITY_PLOTS_2))
						{
							if (plotValid(pLoopPlot) && pLoopPlot->area() == m_unit->area())
							{
								if (pLoopPlot->getOwner() == m_unit->getOwner() &&
									pLoopPlot->getNumVisibleEnemyUnits(m_unit->getOwner()) == 0)
								{
									//	Good defensive sites are way better
									int iValue = pLoopPlot->defenseModifier(m_unit->getTeam(), false);

									//	Boost plots where there is known danger
									iValue = (iValue * (100 + pLoopPlot->getDangerCount(m_unit->getTeam()))) / 100;

									// ls612: If the terrain is damaging, don't guard there
									bool	bHasTerrainDamage = (pLoopPlot->getTotalTurnDamage(m_unit->getGroup()) > 0 || pLoopPlot->getFeatureTurnDamage() > 0);
									if (bHasTerrainDamage)
									{
										iValue = 0;
									}

									//	The city itself is considered only as a poor option since we already
									//	decided it wasn't really necessary to defend there in principal
									if (pLoopPlot == pPlot)
									{
										iValue += 2;	//	Make sure m_unit is at least considered minimally viable

										//	Can guard in the city itself if no better candidates found, but down-grade it
										//	by a factor of 2
										iValue /= 2;
									}
									else
									{
										//	If there are already units guarding here reduce the value of another guard
										int iAlreadyGuarding = GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_GUARD_CITY, m_unit->getGroup(), 0);
										if (iAlreadyGuarding > 0)
										{
											iValue /= (iAlreadyGuarding + 1);
										}
									}

									if (iValue * 1000 / 3 > iBestValue)
									{
										int iPathTurns;
										if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns))
										{
											iValue *= 1000;

											iValue /= (iPathTurns + 2);

											if (iValue > iBestValue)
											{
												if (!exposedToDanger(pLoopPlot, 60, true) && !exposedToDanger(m_unit->getPathEndTurnPlot(), 60, true))
												{
													iBestValue = iValue;
													pBestPlot = m_unit->getPathEndTurnPlot();
													pBestGuardPlot = pLoopPlot;
													FAssert(m_unit->atPlot(pBestGuardPlot) || !m_unit->atPlot(pBestPlot));
												}
											}
										}
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
						CvSelectionGroup* pOldGroup = m_unit->getGroup();
						CvUnit* pEjectedUnit = m_unit->getGroup()->AI_ejectBestDefender(pBestGuardPlot);

						if (pEjectedUnit == NULL)
						{
							//	Mark m_unit group as part of the city garrison
							m_unit->getGroup()->AI_setAsGarrison(pCity);

							//	Not an ideal defensive candidate - is there a decent attack if the city
							//	really feels it needs us as part of it (active?) defense?
							if (anyAttack(2, 55))
							{
								return true;
							}

							//	Ok, no attacks so I guess we'll defend as best we can
							pEjectedUnit = m_unit->getGroup()->AI_ejectBestDefender(pBestGuardPlot, true);
						}
						if (pEjectedUnit != NULL)
						{
							CvPlot* missionPlot = (bGuardInCity ? pBestGuardPlot : pBestPlot);
							if (bGuardInCity)
							{
								if (pPlot->plotCount(PUF_isCityAIType, -1, -1, NULL, m_unit->getOwner()) == 0)
								{
									if (pEjectedUnit->cityDefenseModifier() > 0)
									{
										FAssert(pEjectedUnit->AI()->getUnitAIType() != UNITAI_HUNTER);
										pEjectedUnit->AI()->setUnitAIType(UNITAI_CITY_DEFENSE);
									}
								}
							}
							if (m_unit->atPlot(pBestGuardPlot))
							{
								//	Mark the ejected unit as part of the city garrison
								pEjectedUnit->getGroup()->AI_setAsGarrison(pCity);
								pEjectedUnit->AI()->setUnitAIType(UNITAI_CITY_DEFENSE);
								pEjectedUnit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, NULL);
								return (pEjectedUnit->getGroup() == pOldGroup || pEjectedUnit == m_unit);
							}
							else if (pEjectedUnit->generatePath(missionPlot, 0, true))
							{
								//	Mark the ejected unit as part of the city garrison
								pEjectedUnit->getGroup()->AI_setAsGarrison(pCity);
								pEjectedUnit->AI()->setUnitAIType(UNITAI_CITY_DEFENSE);
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
						//to protect m_unit city but encourage others to defend instead.
						m_unit->getGroup()->pushMission(MISSION_SKIP);
						m_unit->finishMoves();
					}
					return true;
				}
			}
		}
	}

	if (bSearch)
	{
		int iBestValue = MAX_INT;
		const CvPlot* pBestPlot = NULL;
		const CvPlot* pBestGuardPlot = NULL;

		foreach_(CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
		{
			if (plotValid(pLoopCity->plot()))
			{
				// BBAI efficiency: check m_unit->area for land units
				if ((m_unit->getDomainType() == DOMAIN_LAND) && (pLoopCity->area() != m_unit->area()) && !(m_unit->getGroup()->canMoveAllTerrain()))
				{
					continue;
				}
				if (!(pLoopCity->AI_isDefended((!isCityAIType()) ? pLoopCity->plot()->plotStrength(UNITVALUE_FLAGS_DEFENSIVE, PUF_canDefendGroupHead, -1, -1, m_unit->getOwner(), NO_TEAM, PUF_isNotCityAIType) : 0), 2) ||
					(m_unit->isMilitaryHappiness() && !(pLoopCity->AI_isAdequateHappinessMilitary())))
				{
					if (!(pLoopCity->plot()->isVisibleEnemyUnit(m_unit)))
					{
						if ((GC.getGame().getGameTurn() - pLoopCity->getGameTurnAcquired() < 10) || GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_GUARD_CITY, m_unit->getGroup()) < 2)
						{
							int iPathTurns;
							if (!m_unit->atPlot(pLoopCity->plot()) && m_unit->generatePath(pLoopCity->plot(), 0, true, &iPathTurns))
							{
								if (iPathTurns <= iMaxPath && iPathTurns < iBestValue)
								{
									iBestValue = iPathTurns;
									pBestPlot = m_unit->getPathEndTurnPlot();
									pBestGuardPlot = pLoopCity->plot();
									FAssert(!m_unit->atPlot(pBestPlot));
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
			FAssert(!m_unit->atPlot(pBestPlot));

			CvSelectionGroup* pOldGroup = m_unit->getGroup();
			CvUnit* pEjectedUnit = m_unit->getGroup()->AI_ejectBestDefender(pBestGuardPlot);

			if (pEjectedUnit != NULL)
			{
				if (pEjectedUnit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_GUARD_CITY, NULL))
				{
					return pEjectedUnit->getGroup() == pOldGroup || pEjectedUnit == m_unit;
				}
				pEjectedUnit->joinGroup(pOldGroup);
				return false;
			}
			//This unit is not suited for defense, skip the mission
			//to protect m_unit city but encourage others to defend instead.
			if (m_unit->atPlot(pBestGuardPlot))
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP);
				return true;
			}
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_GUARD_CITY, NULL);
		}
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::guardCityAirlift()
{
	PROFILE_FUNC();

	if (m_unit->getGroup()->getNumUnits() > 1)
	{
		return false;
	}

	const CvCity* pCity = m_unit->plot()->getPlotCity();

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

	foreach_(CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
	{
		if (pLoopCity != pCity)
		{
			if (m_unit->canAirliftAt(pCity->plot(), pLoopCity->getX(), pLoopCity->getY()))
			{
				if (!(pLoopCity->AI_isDefended((!isCityAIType()) ? pLoopCity->plot()->plotStrength(UNITVALUE_FLAGS_DEFENSIVE, PUF_canDefendGroupHead, -1, -1, m_unit->getOwner(), NO_TEAM, PUF_isNotCityAIType) : 0)))	// XXX check for other team's units?
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
		m_unit->getGroup()->pushMission(MISSION_AIRLIFT, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::guardBonus(int iMinValue)
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

	const std::map<int, BonusTypes>& guardableResourcePlots = GET_PLAYER(m_unit->getOwner()).getGuardableResourcePlots();
	std::multimap<int, int> sortedIndex;

	//	A valuable optimization is possible to avoid generating the paths for all candidate resource plots (which is the expensive part).
	//	If a m_unit->plot couldn't beat the best one already evaluated even with a path distance of 1 there is no need to evaluate its path.
	//	In practise m_unit happens a LOT because the AI sends units to guard resources, then re-evaluates every turn so they are mostly
	//	already in the right place.  To ensure an evaluation order that makes m_unit optimization likely we sort on simple m_unit->plot distance
	for (std::map<int, BonusTypes>::const_iterator itr = guardableResourcePlots.begin(); itr != guardableResourcePlots.end(); ++itr)
	{
		pLoopPlot = GC.getMap().plotByIndex(itr->first);

		sortedIndex.insert(std::pair<int, int>(stepDistance(m_unit->getX(), m_unit->getY(), pLoopPlot->getX(), pLoopPlot->getY()), itr->first));
	}

	for (std::multimap<int, int>::const_iterator itr = sortedIndex.begin(); itr != sortedIndex.end(); ++itr)
	{
		PROFILE("AI_guardBonus.LoopOuter");

		iI = itr->second;

		std::map<int, BonusTypes>::const_iterator searchItr = guardableResourcePlots.find(iI);
		FAssert(searchItr != guardableResourcePlots.end());
		eNonObsoleteBonus = searchItr->second;

		pLoopPlot = GC.getMap().plotByIndex(iI);

		if (plotValid(pLoopPlot))
		{
			iValue = GET_PLAYER(m_unit->getOwner()).AI_bonusVal(eNonObsoleteBonus);

			iValue += std::max(0, 200 * GC.getBonusInfo(eNonObsoleteBonus).getAIObjective());

			if (pLoopPlot->getPlotGroupConnectedBonus(m_unit->getOwner(), eNonObsoleteBonus) == 1)
			{
				iValue *= 2;
			}

			if (iValue > iMinValue)
			{
				PROFILE("AI_guardBonus.HasValue");

				if (!pLoopPlot->isVisible(m_unit->getTeam(), false) || !(pLoopPlot->isVisibleEnemyUnit(m_unit)))
				{
					PROFILE("AI_guardBonus.NoVisibleEnemy");

					// BBAI TODO: Multiple defenders for higher value resources?
					if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_GUARD_BONUS, m_unit->getGroup()) == 0)
					{
						//	Can m_unit possibly beat the best we already have?
						iValue *= 1000;

						//	Path length from a m_unit->plot to itself (empirically) returns 1, hence the '3' rather than '2'
						if (iValue / 3 > iBestValue)
						{
							PROFILE("AI_guardBonus.TestPath");

							if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns))
							{
								OutputDebugString(CvString::format("Evaluate guard path from (%d,%d) to (%d,%d), turns=%d, value=%d\n",
									m_unit->getX(), m_unit->getY(),
									pLoopPlot->getX(),
									pLoopPlot->getY(),
									iPathTurns,
									iValue).c_str());

								iValue /= (iPathTurns + 1);

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestPlot = m_unit->getPathEndTurnPlot();
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
		if (m_unit->atPlot(pBestGuardPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_BONUS, pBestGuardPlot);
			return true;
		}
		else
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_GUARD_BONUS, pBestGuardPlot);
		}
	}

	return false;
}

int CvUnitDefaultAI::getPlotDefendersNeeded(const CvPlot* pPlot, int iExtra) const
{
	int iNeeded = iExtra;
	const BonusTypes eNonObsoleteBonus = pPlot->getNonObsoleteBonusType(m_unit->getTeam());
	if (eNonObsoleteBonus != NO_BONUS)
	{
		iNeeded += (GET_PLAYER(m_unit->getOwner()).AI_bonusVal(eNonObsoleteBonus) + 10) / 19;
	}

	//	Super forts - PARTIAL merge here to encourage defense of choke points
	const CvPlayerAI& kPlayer = GET_PLAYER(m_unit->getOwner());
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

	int iDefense = pPlot->defenseModifier(m_unit->getTeam(), true);

	iNeeded += (iDefense + 25) / 50;

	if (pPlot->getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(pPlot->getImprovementType()).getCulture() > 0)
	{
		iNeeded = std::max(1 + (GC.getImprovementInfo(pPlot->getImprovementType()).isActsAsCity() ? 1 : 0), iNeeded);
	}

	if (iNeeded == 0)
	{
		return 0;
	}

	iNeeded += GET_PLAYER(m_unit->getOwner()).AI_getPlotAirbaseValue(pPlot) / 50;

	int iNumHostiles = 0;
	int iNumPlots = 0;

	foreach_(const CvPlot * pLoopPlot, pPlot->rect(2, 2))
	{
		iNumHostiles += pLoopPlot->getNumVisiblePotentialEnemyDefenders(m_unit);
		if (pLoopPlot->getTeam() != m_unit->getTeam() || pLoopPlot->isCoastalLand())
		{
			iNumPlots++;
			if (m_unit->isEnemy(pLoopPlot->getTeam()))
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

bool CvUnitDefaultAI::guardFort(bool bSearch)
{
	PROFILE_FUNC();

	if (m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		const ImprovementTypes eImprovement = m_unit->plot()->getImprovementType();
		if (eImprovement != NO_IMPROVEMENT)
		{
			if (GC.getImprovementInfo(eImprovement).isActsAsCity() || GC.getImprovementInfo(eImprovement).getCulture() > 0)
			{
				// Super Forts begin *AI_defense* - just tweaked a number here (iExtra now 1 instead of 0)
				if (m_unit->plot()->plotCount(PUF_isCityAIType, -1, -1, NULL, m_unit->getOwner()) <= getPlotDefendersNeeded(m_unit->plot(), 1))
					// Super Forts end
				{
					m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_BONUS, m_unit->plot());
					return true;
				}
			}
		}
	}

	if (!bSearch)
	{
		return false;
	}

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestGuardPlot = NULL;

	CvReachablePlotSet plotSet(m_unit->getGroup(), 0, MAX_INT);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		const CvPlot* pLoopPlot = itr.plot();
		//	Koshling - plotValid() call not needed when using a CvReachablePlotSet
		if (/*AI_plotValid(pLoopPlot) &&*/ !m_unit->atPlot(pLoopPlot) && pLoopPlot->area() == m_unit->area())
		{
			if (pLoopPlot->getOwner() == m_unit->getOwner())
			{
				const ImprovementTypes eImprovement = pLoopPlot->getImprovementType();
				if (eImprovement != NO_IMPROVEMENT)
				{
					if (GC.getImprovementInfo(eImprovement).isActsAsCity() || GC.getImprovementInfo(eImprovement).getCulture() > 0)
					{
						// Super Forts begin *AI_defense* - just tweaked a number here (iExtra now 1 instead of 0)
						int iValue = getPlotDefendersNeeded(pLoopPlot, 1);
						// Super Forts end

						if (iValue > 0)
						{
							if (!isPlotWellDefended(pLoopPlot, true, 30))
							{
								if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_GUARD_BONUS, m_unit->getGroup()) < iValue)
								{
									int iPathTurns;
									if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns))
									{
										iValue *= 1000;

										iValue /= (iPathTurns + 2);

										if (iValue > iBestValue)
										{
											iBestValue = iValue;
											pBestPlot = m_unit->getPathEndTurnPlot();
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
		if (m_unit->atPlot(pBestGuardPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_BONUS, pBestGuardPlot);
			return true;
		}
		else
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_GUARD_BONUS, pBestGuardPlot);
		}
	}

	return false;
}


// Super Forts begin *AI_defense* - m_unit is designed to ensure each fort has one defender (or improvements that require a fortified unit to upgrade)
bool CvUnitDefaultAI::guardFortMinDefender(bool bSearch)
{
	PROFILE_FUNC();

	if (m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		const ImprovementTypes eImprovement = m_unit->plot()->getImprovementType();
		if (eImprovement != NO_IMPROVEMENT)
		{
			if (GC.getImprovementInfo(eImprovement).isActsAsCity() || GC.getImprovementInfo(eImprovement).isUpgradeRequiresFortify() || GC.getImprovementInfo(eImprovement).getCulture() > 0)
			{
				if (m_unit->plot()->plotCount(PUF_isCityAIType, -1, -1, NULL, m_unit->getOwner()) <= 1)
				{
					m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_BONUS, m_unit->plot());
					return true;
				}
			}
		}
	}

	if (!bSearch)
	{
		return false;
	}

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestGuardPlot = NULL;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		const CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (plotValid(pLoopPlot) && !m_unit->atPlot(pLoopPlot))
		{
			if (pLoopPlot->getOwner() == m_unit->getOwner())
			{
				const ImprovementTypes eImprovement = pLoopPlot->getImprovementType();
				if (eImprovement != NO_IMPROVEMENT)
				{
					if (GC.getImprovementInfo(eImprovement).isActsAsCity() || GC.getImprovementInfo(eImprovement).isUpgradeRequiresFortify() || GC.getImprovementInfo(eImprovement).getCulture() > 0)
					{
						if (!(pLoopPlot->isVisibleEnemyUnit(m_unit)))
						{
							if (pLoopPlot->plotCount(PUF_isCityAIType, -1, -1, NULL, m_unit->getOwner()) == 0)
							{
								const int iMyDistance = stepDistance(m_unit->getX(), m_unit->getY(), pLoopPlot->getX(), pLoopPlot->getY());

								//	If another closer group is already headed there don't bother
								int iOtherGroupDistance;
								if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_GUARD_BONUS, m_unit->getGroup(), 0, &iOtherGroupDistance) == 0 ||
									iMyDistance < iOtherGroupDistance)
								{
									int iPathTurns;
									if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns))
									{
										int iValue = 1000;

										iValue /= (iPathTurns + 2);

										if (iValue > iBestValue)
										{
											iBestValue = iValue;
											pBestPlot = m_unit->getPathEndTurnPlot();
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
		if (m_unit->atPlot(pBestGuardPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_BONUS, pBestGuardPlot);
			return true;
		}
		else
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			return (m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_GUARD_BONUS, pBestGuardPlot));
		}
	}

	return false;
}
// Super Forts end

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::guardCitySite()
{
	PROFILE_FUNC();

	CvPlot* pBestPlot = NULL;
	CvPlot* pBestGuardPlot = NULL;
	{
		const CvPlayerAI& owner = GET_PLAYER(m_unit->getOwner());
		int iBestValue = 0;
		int iPathTurns;
		for (int iI = 0; iI < owner.AI_getNumCitySites(); iI++)
		{
			CvPlot* plotX = owner.AI_getCitySite(iI);

			if (owner.AI_plotTargetMissionAIs(plotX, MISSIONAI_GUARD_CITY, m_unit->getGroup()) == 0 && m_unit->generatePath(plotX, 0, true, &iPathTurns))
			{
				const int iValue = plotX->getFoundValue(m_unit->getOwner());
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = m_unit->getPathEndTurnPlot();
					pBestGuardPlot = plotX;
				}
			}
		}
	}

	if (pBestPlot != NULL && pBestGuardPlot != NULL)
	{
		if (m_unit->atPlot(pBestGuardPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, pBestGuardPlot);
			return true;
		}
		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_GUARD_CITY, pBestGuardPlot);
	}
	return false;
}



// Returns true if a mission was pushed...
bool CvUnitDefaultAI::guardSpy(int iRandomPercent)
{
	PROFILE_FUNC();

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestGuardPlot = NULL;

	foreach_(const CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
	{
		if (plotValid(pLoopCity->plot()))
		{
			if (!(pLoopCity->plot()->isVisibleEnemyUnit(m_unit)))
			{
				/************************************************************************************************/
				/* BETTER_BTS_AI_MOD					  08/19/09								jdog5000	  */
				/*																							  */
				/* Unit AI, Efficiency																		  */
				/************************************************************************************************/
								// BBAI efficiency: check m_unit->area for land units
				if ((m_unit->getDomainType() == DOMAIN_LAND) && (pLoopCity->area() != m_unit->area()) && !(m_unit->getGroup()->canMoveAllTerrain()))
				{
					continue;
				}

				int iValue = 0;

				if (GET_PLAYER(m_unit->getOwner()).AI_isDoVictoryStrategy(AI_VICTORY_SPACE4))
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

				if (GET_PLAYER(m_unit->getOwner()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3))
				{
					if (pLoopCity->getCultureLevel() >= (GC.getNumCultureLevelInfos() - 2))
					{
						iValue += 10;
					}
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
				/************************************************************************************************/
				/* BETTER_BTS_AI_MOD					   END												  */
				/************************************************************************************************/

				if (iValue > 0)
				{
					if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_GUARD_SPY, m_unit->getGroup()) == 0)
					{
						int iPathTurns;
						if (m_unit->generatePath(pLoopCity->plot(), 0, true, &iPathTurns))
						{
							iValue *= 100 + GC.getGame().getSorenRandNum(iRandomPercent, "AI Guard Spy");
							//iValue /= 100;
							iValue /= iPathTurns + 1;

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = m_unit->getPathEndTurnPlot();
								pBestGuardPlot = pLoopCity->plot();
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestGuardPlot != NULL))
	{
		if (m_unit->atPlot(pBestGuardPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_SPY, pBestGuardPlot);
			return true;
		}
		else
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_GUARD_SPY, pBestGuardPlot);
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::chokeDefend()
{
	FAssert(isCityAIType());

	// XXX what about amphib invasions?

	CvCity* pCity = m_unit->plot()->getPlotCity();

	if (pCity != NULL && pCity->getOwner() == m_unit->getOwner() && pCity->AI_neededDefenders() > 1
	&& pCity->AI_isDefended(pCity->plot()->plotStrength(UNITVALUE_FLAGS_DEFENSIVE, PUF_canDefendGroupHead, -1, -1, m_unit->getOwner(), NO_TEAM, PUF_isNotCityAIType)))
	{
		const int iPlotDanger = GET_PLAYER(m_unit->getOwner()).AI_getPlotDanger(m_unit->plot(), 3);

		if (iPlotDanger <= 4 && anyAttack(1, 65, std::max(0, iPlotDanger - 1)))
		{
			return true;
		}
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::heal(int iDamagePercent, int iMaxPath)
{
	PROFILE_FUNC();

	CvSelectionGroup* pGroup = m_unit->getGroup();

	if (m_unit->plot()->getTotalTurnDamage(pGroup) > 0)
	{
		OutputDebugString("AI_heal: denying heal due to terrain damage\n");
		return false;
	}
	if (iDamagePercent < 1)
	{
		iDamagePercent = 10;
	}

	if (pGroup->getNumUnits() == 1)
	{
		if (m_unit->getDamage() < 1)
		{
			return false;
		}
		OutputDebugString("AI_heal: one unit stack\n");
		if ((m_unit->plot()->isCity() || m_unit->healTurns(m_unit->plot()) == 1) && !m_unit->isAlwaysHeal())
		{
			OutputDebugString("AI_heal: city or 1 turn heal\n");
			pGroup->pushMission(MISSION_HEAL);
			return true;
		}
		if (m_unit->plot()->getNumVisibleAdjacentEnemyDefenders(m_unit) == 0 && !m_unit->isAlwaysHeal()
		&& (m_unit->plot()->getTeam() == NO_TEAM || !GET_TEAM(m_unit->plot()->getTeam()).isAtWar(m_unit->getTeam())))
		{
			if (!m_unit->plot()->isCity() && moveIntoCity(1))
			{
				OutputDebugString("AI_heal: one turn city move\n");
				return true;
			}
			OutputDebugString("AI_heal: healing\n");
			pGroup->pushMission(MISSION_HEAL);
			return true;
		}
		OutputDebugString("AI_heal: denying heal\n");
	}

	iMaxPath = std::min(iMaxPath, 2);
	std::vector<CvUnit*> aeDamagedUnits;
	int iHurtUnitCount = 0;

	foreach_(CvUnit * pLoopUnit, pGroup->units())
	{
		int iDamageThreshold = pLoopUnit->getMaxHP() * iDamagePercent / 100;

		if (NO_UNIT != m_unit->getLeaderUnitType())
		{
			iDamageThreshold /= 2;
		}

		if (pLoopUnit->getDamage() > 0)
		{
			iHurtUnitCount++;
		}

		if (pLoopUnit->getDamage() > iDamageThreshold
		&& !pLoopUnit->hasMoved() && !pLoopUnit->isAlwaysHeal()
		&& pLoopUnit->healTurns(pLoopUnit->plot()) <= iMaxPath)
		{
			aeDamagedUnits.push_back(pLoopUnit);
		}
	}
	if (iHurtUnitCount == 0)
	{
		return false;
	}

	bool bCanClaimTerritory = m_unit->canClaimTerritory(m_unit->plot());
	bool bPushedMission = false;

	if (bCanClaimTerritory || m_unit->plot()->isCity() && m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		FAssertMsg(((int)aeDamagedUnits.size()) <= iHurtUnitCount, "damaged units array is larger than our hurt unit count");

		for (unsigned int iI = 0; iI < aeDamagedUnits.size(); iI++)
		{
			CvUnit* pUnitToHeal = aeDamagedUnits[iI];
			pUnitToHeal->joinGroup(NULL);
			pUnitToHeal->getGroup()->pushMission(MISSION_HEAL);

			// note, removing the head unit from a group will force the group to be completely split if non-human
			if (pUnitToHeal == m_unit)
			{
				bPushedMission = true;

				if (bCanClaimTerritory)
				{
					pGroup->pushMission(MISSION_CLAIM_TERRITORY, -1, -1, 0, false, false, MISSIONAI_CLAIM_TERRITORY, m_unit->plot());
					pGroup->pushMission(MISSION_HEAL, -1, -1, 0, true, false);
				}
			}
			iHurtUnitCount--;
		}
	}

	if (iHurtUnitCount * 2 > pGroup->getNumUnits())
	{
		FAssertMsg(pGroup->getNumUnits() > 0, "group now has zero units");

		if (moveIntoCity(2))
		{
			return true;
		}
		if (m_unit->healRate(m_unit->plot()) > 10)
		{
			pGroup->pushMission(MISSION_HEAL);
			return true;
		}
		if (safety(iMaxPath))
		{
			return true;
		}
	}
	return bPushedMission;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::afterAttack()
{
	if (!m_unit->isMadeAttack() || !m_unit->canFight() || m_unit->isBlitz())
	{
		return false;
	}
	// Can fight, but has used up its attack for m_unit turn.

	if (m_unit->getDomainType() == DOMAIN_LAND && guardCity(false, true, 1))
	{
		return true;
	}

	if (RbombardPlot(m_unit->getDCMBombRange(), 60))
	{
		return true;
	}

	if (pillageRange(1))
	{
		return true;
	}

	if (retreatToCity(false, false, 1))
	{
		return true;
	}

	if (hide())
	{
		return true;
	}

	if (goody(1))
	{
		return true;
	}

	if (RbombardPlot(m_unit->getDCMBombRange(), 0))
	{
		return true;
	}

	if (pillageRange(2))
	{
		return true;
	}

	if (defend())
	{
		return true;
	}

	if (safety())
	{
		return true;
	}

	return false;
}

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::goldenAge()
{
	if (m_unit->canGoldenAge())
	{
		m_unit->getGroup()->pushMission(MISSION_GOLDEN_AGE);
		return true;
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::spreadReligion()
{
	PROFILE_FUNC();

	const CvPlayerAI& owner = GET_PLAYER(m_unit->getOwner());
	const ReligionTypes eStateReligion = owner.getStateReligion();

	// More often than not, it is the state religion the AI spreads.
	ReligionTypes eReligion =
		(
			(eStateReligion != NO_RELIGION && m_unit->getUnitInfo().getReligionSpreads(eStateReligion) > 0)
			?
			eStateReligion
			:
			NO_RELIGION
		);
	if (eReligion == NO_RELIGION)
	{
		for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			if (m_unit->getUnitInfo().getReligionSpreads((ReligionTypes)iI) > 0)
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

	if (m_cachedPlayer != m_unit->getOwner()
	|| m_cachedMissionaryPlotset == NULL
	|| m_cachedMissionaryPlotset->find(m_unit->plot()) == m_cachedMissionaryPlotset->end())
	{
		SAFE_DELETE(m_cachedMissionaryPlotset);

		m_cachedMissionaryPlotset = new CvReachablePlotSet(m_unit->getGroup(), MOVE_NO_ENEMY_TERRITORY, MAX_INT, false);

		m_cachedPlayer = m_unit->getOwner();
	}

	int iBaseFlags = m_unit->isHuman() ? MOVE_NO_ENEMY_TERRITORY : (MOVE_NO_ENEMY_TERRITORY | MOVE_IGNORE_DANGER);
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestSpreadPlot = NULL;

	std::multimap<int, CvCity*> targetValues;
	CvReachablePlotSet::const_iterator itr = m_cachedMissionaryPlotset->begin();

	// If we already has a chosen spread city targeted then start with a presumption we'll stick to it
	if (m_unit->getGroup()->AI_getMissionAIType() == MISSIONAI_SPREAD)
	{
		PROFILE("CvUnitDefaultAI::spreadReligion.AlreadyMissioned");

		CvPlot* pPlot = m_unit->getGroup()->AI_getMissionAIPlot();

		CvCity* pLoopCity = pPlot->getPlotCity();

		//logBBAI("Missionary %d at (%d,%d) already targeting city at (%d,%d)\n", m_unit->getID(), m_unit->getX(), m_unit->getY(), pPlot->getX(), pPlot->getY());

		if (pLoopCity != NULL
		&& m_cachedMissionaryPlotset->find(pLoopCity->plot()) != m_cachedMissionaryPlotset->end()
		&& m_unit->generatePath(pLoopCity->plot(), iBaseFlags, true))
		{
			// If human, prefer to spread to the player where automated from.
			const bool bForceMove = m_unit->isHuman() && m_unit->plot()->getOwner() == pLoopCity->getOwner() && pLoopCity->isRevealed(m_unit->getTeam(), false);

			pBestPlot = bForceMove ? pLoopCity->plot() : m_unit->getPathEndTurnPlot();
			pBestSpreadPlot = pLoopCity->plot();

			// Check end of turn point is not in immediate danger
			if (!bForceMove && owner.AI_getVisiblePlotDanger(pBestPlot, 1, false))
			{
				//logBBAI("Danger at end turn m_unit->plot - looking for alternate route...");

				// Try to find an alternate route taking danger into account in the pathing calculation
				if (m_unit->generatePath(pLoopCity->plot(), MOVE_NO_ENEMY_TERRITORY, true))
				{
					//logBBAI("...found one");
					pBestPlot = m_unit->getPathEndTurnPlot();

					iBaseFlags = MOVE_NO_ENEMY_TERRITORY;
				}
				else pBestSpreadPlot = NULL; // No way past
			}
		}
	}

	if (pBestSpreadPlot == NULL)
	{
		const bool bCultureVictory = owner.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2);
		const bool bHasHolyCity = GET_TEAM(m_unit->getTeam()).hasHolyCity(eReligion);

		// BBAI TODO: Could also use CvPlayerAI::AI_missionaryValue to determine which player to target ...
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (!GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				continue;
			}
			int iPlayerMultiplierPercent = 0;

			if (GET_PLAYER((PlayerTypes)iI).getTeam() != m_unit->getTeam() && m_unit->canEnterTerritory(GET_PLAYER((PlayerTypes)iI).getTeam()))
			{
				if (bHasHolyCity)
				{
					iPlayerMultiplierPercent = 100;
					// BBAI TODO: If going for cultural victory, don't spread to other teams? Sure, m_unit might decrease the chance of
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
			else if (iI == m_unit->getOwner())
			{
				iPlayerMultiplierPercent = 100;
			}
			else if (bHasHolyCity && GET_PLAYER((PlayerTypes)iI).getTeam() == m_unit->getTeam())
			{
				iPlayerMultiplierPercent = 80;
			}
			else continue;

			if (iPlayerMultiplierPercent > 0)
			{
				foreach_(CvCity * cityX, GET_PLAYER((PlayerTypes)iI).cities())
				{
					if (plotValid(cityX->plot())
					&& (itr = m_cachedMissionaryPlotset->find(cityX->plot())) != m_cachedMissionaryPlotset->end()
					&& m_unit->canSpread(cityX->plot(), eReligion)
					&& !owner.AI_getAnyPlotDanger(cityX->plot())
					&& owner.AI_plotTargetMissionAIs(cityX->plot(), MISSIONAI_SPREAD, m_unit->getGroup()) == 0)
					{
						int iValue = 7 + 4 * cityX->getPopulation();

						bool bOurCity = false;
						// BBAI TODO: Why not just use iPlayerMultiplier??
						if (cityX->getOwner() == m_unit->getOwner())
						{
							iValue *= bCultureVictory ? 16 : 4;
							bOurCity = true;
						}
						else if (cityX->getTeam() == m_unit->getTeam())
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
						if (m_unit->isHuman() && m_unit->plot()->getOwner() == cityX->getOwner())
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

			if (m_unit->generatePath(pLoopCity->plot(), iBaseFlags, true))
			{
				// Reconstruct bForceMove value
				const bool bForceMove = m_unit->isHuman() && m_unit->plot()->getOwner() == pLoopCity->getOwner() && pLoopCity->isRevealed(m_unit->getTeam(), false);

				pBestPlot = bForceMove ? pLoopCity->plot() : m_unit->getPathEndTurnPlot();

				pBestSpreadPlot = pLoopCity->plot();
				//logBBAI("Potential target city at (%d,%d)", pBestSpreadPlot->getX(), pBestSpreadPlot->getY());

				// Check end of turn point is not in immediate danger
				if (!bForceMove && owner.AI_getVisiblePlotDanger(pBestPlot, 1, false))
				{
					//logBBAI("Danger at end turn m_unit->plot - looking for alternate route...");

					// Try to find an alternate route taking danger into account in the pathing calculation
					if (m_unit->generatePath(pLoopCity->plot(), MOVE_NO_ENEMY_TERRITORY, true))
					{
						pBestPlot = m_unit->getPathEndTurnPlot();
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
		//logBBAI("Missionary %d at (%d,%d) targeting spread in city at (%d,%d)\n", m_unit->getID(), m_unit->getX(), m_unit->getY(), pBestSpreadPlot->getX(), pBestSpreadPlot->getY());
		if (m_unit->atPlot(pBestSpreadPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_SPREAD, eReligion);
			return true;
		}
		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), iBaseFlags, false, false, MISSIONAI_SPREAD, pBestSpreadPlot);
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::spreadCorporation()
{
	PROFILE_FUNC();

	CorporationTypes eCorporation = NO_CORPORATION;
	int iBaseFlags = (m_unit->isHuman() ? MOVE_NO_ENEMY_TERRITORY : (MOVE_NO_ENEMY_TERRITORY | MOVE_IGNORE_DANGER));

	for (int iI = 0; iI < GC.getNumCorporationInfos(); ++iI)
	{
		if (m_unit->getUnitInfo().getCorporationSpreads((CorporationTypes)iI) > 0)
		{
			eCorporation = ((CorporationTypes)iI);
			break;
		}
	}

	if (NO_CORPORATION == eCorporation)
	{
		return false;
	}

	bool bHasHQ = (GET_TEAM(m_unit->getTeam()).hasHeadquarters(eCorporation));

	CvPlot* pBestPlot = NULL;
	CvPlot* pBestSpreadPlot = NULL;

	if (m_cachedPlayer != m_unit->getOwner() ||
		 m_cachedMissionaryPlotset == NULL ||
		 m_cachedMissionaryPlotset->find(m_unit->plot()) == m_cachedMissionaryPlotset->end())
	{
		SAFE_DELETE(m_cachedMissionaryPlotset);

		m_cachedMissionaryPlotset = new CvReachablePlotSet(m_unit->getGroup(), MOVE_NO_ENEMY_TERRITORY, MAX_INT, false);

		m_cachedPlayer = m_unit->getOwner();
	}

	//CvReachablePlotSet plotSet(m_unit->getGroup(), MOVE_NO_ENEMY_TERRITORY, MAX_INT);
	std::multimap<int, CvCity*>	targetValues;
	CvReachablePlotSet::const_iterator itr = m_cachedMissionaryPlotset->begin();

	//	If we already has a chosen spread city targeted then start with a presumption
	//	we'll stick to it
	if (m_unit->getGroup()->AI_getMissionAIType() == MISSIONAI_SPREAD_CORPORATION)
	{
		PROFILE("CvUnitDefaultAI::spreadReligion.AlreadyMissioned");

		CvPlot* pPlot = m_unit->getGroup()->AI_getMissionAIPlot();

		const CvCity* pLoopCity = pPlot->getPlotCity();

		//OutputDebugString(CvString::format("Missionary at (%d,%d) already targeting city at (%d,%d)\n", m_unit->getX(), m_unit->getY(), pPlot->getX(), pPlot->getY()).c_str());

		if (pLoopCity != NULL &&
			 m_cachedMissionaryPlotset->find(pLoopCity->plot()) != m_cachedMissionaryPlotset->end() &&
			 m_unit->generatePath(pLoopCity->plot(), iBaseFlags, true))
		{
			bool bForceMove = false;
			if (m_unit->isHuman())
			{
				//If human, prefer to spread to the player where automated from.
				if (m_unit->plot()->getOwner() == pLoopCity->getOwner())
				{
					if (pLoopCity->isRevealed(m_unit->getTeam(), false))
					{
						bForceMove = true;
					}
				}
			}

			pBestPlot = bForceMove ? pLoopCity->plot() : m_unit->getPathEndTurnPlot();
			pBestSpreadPlot = pLoopCity->plot();

			//	Check end of turn point is not in immediate danger
			if (!bForceMove && GET_PLAYER(m_unit->getOwner()).AI_getVisiblePlotDanger(pBestPlot, 1, false))
			{
				//logBBAI("Danger at end turn m_unit->plot - looking for alternate route...");
				//	Try to find an alternate route taking danger into account in the
				//	apthing calculation
				if (m_unit->generatePath(pLoopCity->plot(),
					MOVE_NO_ENEMY_TERRITORY,
					true))
				{
					//logBBAI("...found one");
					pBestPlot = m_unit->getPathEndTurnPlot();

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
		const CvTeam& kTeam = GET_TEAM(m_unit->getTeam());
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
			if (kLoopPlayer.isAlive() && ((bHasHQ && m_unit->canEnterTerritory(GET_PLAYER((PlayerTypes)iI).getTeam())) || (m_unit->getTeam() == kLoopPlayer.getTeam())))
			{
				const int iLoopPlayerCorpCount = kLoopPlayer.countCorporations(eCorporation);
				const CvTeam& kLoopTeam = GET_TEAM(kLoopPlayer.getTeam());
				foreach_(CvCity * pLoopCity, kLoopPlayer.cities())
				{
					if (plotValid(pLoopCity->plot()) && (itr = m_cachedMissionaryPlotset->find(pLoopCity->plot())) != m_cachedMissionaryPlotset->end())
					{
						// BBAI efficiency: check same m_unit->area
						if ( /*pLoopCity->area() == m_unit->area() &&*/ m_unit->canSpreadCorporation(pLoopCity->plot(), eCorporation))
						{
							if (!(pLoopCity->plot()->isVisibleEnemyUnit(m_unit)))
							{
								if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_SPREAD_CORPORATION, m_unit->getGroup()) == 0)
								{
									// BBAI TODO: Serious need for more intelligent self spread, keep certain corps from
									// enemies based on their victory pursuits (culture ...)
									int iValue = (10 + pLoopCity->getPopulation() * 2);

									if (pLoopCity->getOwner() == m_unit->getOwner())
									{
										iValue *= 4;
									}
									else if (kLoopTeam.isVassal(m_unit->getTeam()))
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
									else if (pLoopCity->getTeam() == m_unit->getTeam())
									{
										iValue *= 2;
									}

									if (iLoopPlayerCorpCount == 0)
									{
										//Generally prefer to heavily target one player
										iValue /= 2;
									}

									if (m_unit->isHuman())
									{
										//If human, prefer to spread to the player where automated from.
										if (m_unit->plot()->getOwner() == pLoopCity->getOwner())
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

			if (m_unit->generatePath(pLoopCity->plot(),
				iBaseFlags,
				true))
			{
				bool bForceMove = false;
				if (m_unit->isHuman())
				{
					// Reconstruct bForceMove value
					if (m_unit->plot()->getOwner() == pLoopCity->getOwner())
					{
						if (pLoopCity->isRevealed(m_unit->getTeam(), false))
						{
							bForceMove = true;
						}
					}
				}

				pBestPlot = bForceMove ? pLoopCity->plot() : m_unit->getPathEndTurnPlot();
				pBestSpreadPlot = pLoopCity->plot();

				//	Check end of turn point is not in immediate danger
				if (!bForceMove && GET_PLAYER(m_unit->getOwner()).AI_getVisiblePlotDanger(pBestPlot, 1, false))
				{
					//logBBAI("Danger at end turn m_unit->plot - looking for alternate route...");
					//	Try to find an alternate route taking danger into account in the
					//	apthing calculation
					if (m_unit->generatePath(pLoopCity->plot(),
						MOVE_NO_ENEMY_TERRITORY,
						true))
					{
						//logBBAI("...found one");
						pBestPlot = m_unit->getPathEndTurnPlot();

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
		if (m_unit->atPlot(pBestSpreadPlot))
		{
			if (m_unit->canSpreadCorporation(pBestSpreadPlot, eCorporation))
			{
				m_unit->getGroup()->pushMission(MISSION_SPREAD_CORPORATION, eCorporation);
			}
			else
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_SPREAD_CORPORATION, pBestSpreadPlot);
			}
			return true;
		}
		else
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD					  03/09/09								jdog5000	  */
			/*																							  */
			/* Unit AI																					  */
			/************************************************************************************************/
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), iBaseFlags, false, false, MISSIONAI_SPREAD_CORPORATION, pBestSpreadPlot);
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD					   END												  */
			/************************************************************************************************/
		}
	}

	return false;
}

bool CvUnitDefaultAI::spreadReligionAirlift()
{
	PROFILE_FUNC();

	int iValue;
	int iI;

	if (m_unit->getGroup()->getNumUnits() > 1)
	{
		return false;
	}

	const CvCity* pCity = m_unit->plot()->getPlotCity();

	if (pCity == NULL)
	{
		return false;
	}

	if (pCity->getMaxAirlift() == 0)
	{
		return false;
	}

	//bool bCultureVictory = GET_PLAYER(m_unit->getOwner()).AI_isDoStrategy(AI_STRATEGY_CULTURE2);
	ReligionTypes eReligion = NO_RELIGION;

	if (eReligion == NO_RELIGION)
	{
		if (GET_PLAYER(m_unit->getOwner()).getStateReligion() != NO_RELIGION)
		{
			if (m_unit->getUnitInfo().getReligionSpreads(GET_PLAYER(m_unit->getOwner()).getStateReligion()) > 0)
			{
				eReligion = GET_PLAYER(m_unit->getOwner()).getStateReligion();
			}
		}
	}

	if (eReligion == NO_RELIGION)
	{
		for (iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			//if (bCultureVictory || GET_TEAM(m_unit->getTeam()).hasHolyCity((ReligionTypes)iI))
			{
				if (m_unit->getUnitInfo().getReligionSpreads((ReligionTypes)iI) > 0)
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
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(m_unit->getTeam()))
		{
			foreach_(const CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
			{
				if (m_unit->canAirliftAt(pCity->plot(), pLoopCity->getX(), pLoopCity->getY()))
				{
					if (m_unit->canSpread(pLoopCity->plot(), eReligion))
					{
						if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_SPREAD, m_unit->getGroup()) == 0)
						{
							// Don't airlift where there's already one of our unit types (probably just airlifted)
							if (pLoopCity->plot()->plotCount(PUF_isUnitType, m_unit->getUnitType(), -1, NULL, m_unit->getOwner()) > 0)
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
		m_unit->getGroup()->pushMission(MISSION_AIRLIFT, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_SPREAD, pBestPlot);
		return true;
	}

	return false;
}

bool CvUnitDefaultAI::spreadCorporationAirlift()
{
	PROFILE_FUNC();

	if (m_unit->getGroup()->getNumUnits() > 1)
	{
		return false;
	}

	CvCity* pCity = m_unit->plot()->getPlotCity();

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
		if (m_unit->getUnitInfo().getCorporationSpreads((CorporationTypes)iI) > 0)
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
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(m_unit->getTeam()))
		{
			foreach_(const CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
			{
				if (m_unit->canAirliftAt(pCity->plot(), pLoopCity->getX(), pLoopCity->getY()))
				{
					if (m_unit->canSpreadCorporation(pLoopCity->plot(), eCorporation))
					{
						if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_SPREAD_CORPORATION, m_unit->getGroup()) == 0)
						{
							// Don't airlift where there's already one of our unit types (probably just airlifted)
							if (pLoopCity->plot()->plotCount(PUF_isUnitType, m_unit->getUnitType(), -1, NULL, m_unit->getOwner()) > 0)
							{
								continue;
							}

							int iValue = (pLoopCity->getPopulation() * 4);

							if (pLoopCity->getOwner() == m_unit->getOwner())
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
		m_unit->getGroup()->pushMission(MISSION_AIRLIFT, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_SPREAD, pBestPlot);
		return true;
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::discover(const bool bFirstResearchOnly)
{
	if (!m_unit->canDiscover())
	{
		return false;
	}
	const TechTypes eTech = m_unit->getDiscoveryTech();
	const bool bIsFirstTech = GET_PLAYER(m_unit->getOwner()).AI_isFirstTech(eTech);

	if (bFirstResearchOnly && !bIsFirstTech)
	{
		return false;
	}
	const int iPercentWasted = 100 * GET_TEAM(m_unit->getTeam()).getResearchProgress(eTech) / GET_TEAM(m_unit->getTeam()).getResearchCost(eTech);

	FAssert(iPercentWasted >= 0 && iPercentWasted <= 100);

	if (bIsFirstTech)
	{
		if (m_unit->getDiscoverResearch(eTech) >= GET_TEAM(m_unit->getTeam()).getResearchLeft(eTech))
		{
			if (iPercentWasted <= 30 || bFirstResearchOnly && iPercentWasted <= 50)
			{
				return m_unit->discover(eTech);
			}
		}
		else if (bFirstResearchOnly)
		{
			return false;
		}
	}
	// Unit cannot finish the tech m_unit turn, so why not speed it up some?
	if (m_unit->getDiscoverResearch(eTech) <= GET_TEAM(m_unit->getTeam()).getResearchLeft(eTech))
	{
		return m_unit->discover(eTech);
	}
	// Unit can finish the tech m_unit turn.

	if (GET_PLAYER(m_unit->getOwner()).getResearchTurnsLeft(eTech, true)
	<= std::max(1, GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 100))
	{
		return false;
	}
	// Takes some time to invent, allow some wastage.
	if (iPercentWasted <= 5 || iPercentWasted <= 15 && GET_PLAYER(m_unit->getOwner()).getCurrentResearch() == eTech)
	{
		return m_unit->discover(eTech);
	}
	return false;
}


namespace {
	// Helper function to determine if a unit looks legendaryish
	bool isLegendary(const CvUnit* unit)
	{
		const CvUnitInfo& unitInfo = GC.getUnitInfo(unit->getUnitType());
		return (unitInfo.getMaxGlobalInstances() > 0 && unitInfo.getMaxGlobalInstances() <= 3)
			|| (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS) && unit->qualityRank() > 8);
	}
}

bool CvUnitDefaultAI::leadLegend()
{
	PROFILE_FUNC();

	FAssertMsg(!m_unit->isHuman(), "AI_leadLegend shouldn't be called for human players");
	FAssert(NO_PLAYER != m_unit->getOwner());

	const CvPlayer& kOwner = GET_PLAYER(m_unit->getOwner());
	CvUnit* pBestUnit = NULL;
	CvPlot* pBestPlot = NULL;
	int iBestStrength = 0;
	foreach_(CvUnit * pLoopUnit, kOwner.units())
	{
		if (isLegendary(pLoopUnit)

			&& m_unit->canLead(pLoopUnit->plot(), pLoopUnit->getID()) > 0
			&& plotValid(pLoopUnit->plot())

			&& !pLoopUnit->plot()->isVisibleEnemyUnit(m_unit)

			&& pLoopUnit->combatLimit() >= 100

			&& m_unit->generatePath(pLoopUnit->plot(), MOVE_AVOID_ENEMY_WEIGHT_3, true)
			)
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
				// generatePath called just above actually updates our own current path, so here we save the end turn m_unit->plot of that path so we can use it
				// as a move target
				pBestPlot = m_unit->getPathEndTurnPlot();
			}
		}
	}

	if (pBestUnit && pBestPlot)
	{
		if (m_unit->atPlot(pBestPlot))
		{
			if (gUnitLogLevel > 2)
			{
				CvWString szString;
				getUnitAIString(szString, pBestUnit->AI()->getUnitAIType());

				logBBAI("	  Great general %d for %S chooses to lead %S Legend Unit", m_unit->getID(), GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), pBestUnit->getName(0).GetCString());
			}

			m_unit->getGroup()->pushMission(MISSION_LEAD, pBestUnit->getID());

			return true;
		}
		else
		{
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_WEIGHT_3);
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::lead(std::vector<UnitAITypes>& aeUnitAITypes)
{
	PROFILE_FUNC();

	FAssertMsg(!m_unit->isHuman(), "isHuman did not return false as expected");
	FAssertMsg(getUnitAIType() != NO_UNITAI, "AI_getUnitAIType() is not expected to be equal with NO_UNITAI");
	FAssert(NO_PLAYER != m_unit->getOwner());

	const CvPlayer& kOwner = GET_PLAYER(m_unit->getOwner());

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
		if (m_unit->isEnemy((TeamTypes)iI))
		{
			if (GET_TEAM((TeamTypes)iI).countNumUnitsByArea(m_unit->area()) > 0)
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
				if (m_unit->canLead(pLoopUnit->plot(), pLoopUnit->getID()) > 0)
				{
					bValid = true;
					bLegend = true;
				}
			}

			if (!bValid)
			{
				for (uint iI = 0; iI < aeUnitAITypes.size(); iI++)
				{
					if (pLoopUnit->AI()->getUnitAIType() == aeUnitAITypes[iI] || NO_UNITAI == aeUnitAITypes[iI])
					{
						if (m_unit->canLead(pLoopUnit->plot(), pLoopUnit->getID()) > 0)
						{
							bValid = true;
							break;
						}
					}
				}
			}

			if (bValid)
			{
				if (plotValid(pLoopUnit->plot()))
				{
					if (!(pLoopUnit->plot()->isVisibleEnemyUnit(m_unit)))
					{
						if (pLoopUnit->combatLimit() >= 100)
						{
							if (m_unit->generatePath(pLoopUnit->plot(), MOVE_AVOID_ENEMY_WEIGHT_3, true))
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
									pBestStrPlot = m_unit->getPathEndTurnPlot();
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
									pBestHealPlot = m_unit->getPathEndTurnPlot();
								}
							}
						}
					}
				}
			}
		}
	}

	if (getBirthmark() % 3 == 0 && pBestHealUnit != NULL)
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
		if (m_unit->atPlot(pBestPlot) && pBestUnit)
		{
			if (gUnitLogLevel > 2)
			{
				CvWString szString;
				getUnitAIString(szString, pBestUnit->AI()->getUnitAIType());

				if (bBestUnitLegend)
				{
					logBBAI("	  Great general %d for %S chooses to lead %S Legend Unit", m_unit->getID(), GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), pBestUnit->getName(0).GetCString());
				}
				else
				{
					logBBAI("	  Great general %d for %S chooses to lead %S with UNITAI %S", m_unit->getID(), GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), pBestUnit->getName(0).GetCString(), szString.GetCString());
				}
			}
			m_unit->getGroup()->pushMission(MISSION_LEAD, pBestUnit->getID());
			return true;
		}
		else
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_WEIGHT_3);
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/

	return false;
}

// Returns true if a mission was pushed...
// iMaxCounts = 1 would mean join a city if there's no existing joined GP of that type.
bool CvUnitDefaultAI::join(int iMaxCount)
{
	PROFILE_FUNC();

	int iValue;
	int iI;
	int iPathTurns = 0;

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	SpecialistTypes eBestSpecialist = NO_SPECIALIST;
	int iCount = 0;

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  09/03/10								jdog5000	  */
	/*																				phungus420	*/
	/* Unit AI, Efficiency																		  */
	/************************************************************************************************/
	if (iMaxCount && (GC.getGame().getSorenRandNum(11, "Settle GG") < iMaxCount + 5))
	{
		return false;
	}

	foreach_(CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
	{
		// BBAI efficiency: check same m_unit->area
		if ((pLoopCity->area() == m_unit->area()) && plotValid(pLoopCity->plot()))
		{
			if (generateSafePathforVulnerable(pLoopCity->plot(), &iPathTurns))
			{
				for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
				{
					bool bDoesJoin = false;
					SpecialistTypes eSpecialist = (SpecialistTypes)iI;
					if (m_unit->getUnitInfo().getGreatPeoples(eSpecialist))
					{
						bDoesJoin = true;
					}
					if (bDoesJoin)
					{
						iCount += pLoopCity->getSpecialistCount(eSpecialist);
						if (iCount >= iMaxCount)
						{
							return false;
						}
					}

					if (m_unit->canJoin(pLoopCity->plot(), ((SpecialistTypes)iI)))
					{
						int iTotalThreat = std::max(1, GET_PLAYER(m_unit->getOwner()).AI_getTotalAreaCityThreat(m_unit->area(), NULL));
						int iOurThreat = pLoopCity->AI_cityThreat();

						//	Replaced local danger blocking of joins with more nuanced threat level evaluation.
						//	This reduces the chances of the join being blocked by local dnager counts from
						//	battles with animals and so forth
						if (iOurThreat < (2 * iTotalThreat) / GET_PLAYER(m_unit->getOwner()).getNumCities())
						{
							iValue = pLoopCity->AI_specialistValue(((SpecialistTypes)iI), pLoopCity->AI_avoidGrowth(), false);
							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = m_unit->getPathEndTurnPlot();
								eBestSpecialist = ((SpecialistTypes)iI);
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (eBestSpecialist != NO_SPECIALIST))
	{
		if (m_unit->atPlot(pBestPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_JOIN, eBestSpecialist);
			return true;
		}
		else
		{
			if (generateSafePathforVulnerable(pBestPlot, &iPathTurns))
			{
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_IGNORE_DANGER);
			}
			else
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP);
				return true;
			}
		}
	}

	//m_unit->getGroup()->releaseUnitAIs(UNITAI_ESCORT);
	return false;
}

/*
CvUnit* CvUnitDefaultAI::cityConstructionTargeted(const CvCity* pCity, BuildingTypes eBuilding, const CvSelectionGroup* omitGroup) const
{
	PROFILE_FUNC();

	foreach_(const CvSelectionGroup* pLoopSelectionGroup, GET_PLAYER(m_unit->getOwner()).groups())
	{
		if ( pLoopSelectionGroup != omitGroup &&
			 pLoopSelectionGroup->AI_getMissionAIPlot() == pCity->plot() &&
			 pLoopSelectionGroup->AI_getMissionAIType() == MISSIONAI_CONSTRUCT )
		{
			CvUnit* targetingUnit = (CvUnit*)pLoopSelectionGroup->getHeadUnit();

			//	Have to handle the case of a group with no units.  This can occur after a foprce group
			//	split due to things like revokation of open borders that forces units to be unserimoniously
			//	moved to another m_unit->plot (and ungroups them in doing so since it is possible that only a subset
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

//	Should we disband m_unit unit?
bool CvUnitDefaultAI::scrapSubdued()
{
	PROFILE_FUNC();

	if (GET_PLAYER(m_unit->getOwner()).AI_isFinancialTrouble())
	{
		//	Always scrap subdued animals if we're in financial trouble and they were unable to
		//	find construction targets (which is implied by having called m_unit)
		m_unit->scrap();
		return true;
	}

	//	Count how many units of m_unit type we have that couldn't find construction missions (by
	//	implication of getting here no further units of m_unit type could)
	int	iSurplass = 0;
	//	Hold a suplasss of each type of up to 2 (inside our borders) or 1 (outside), boosted by 1 if we have less then 4 cities
	int	iExtra = (m_unit->plot()->getOwner() == m_unit->getOwner() ? 2 : 1) + (GET_PLAYER(m_unit->getOwner()).getNumCities() < 4 ? 1 : 0);

	foreach_(const CvUnit * pLoopUnit, GET_PLAYER(m_unit->getOwner()).units())
	{
		if (pLoopUnit->getUnitType() == m_unit->getUnitType() && pLoopUnit->getGroup()->AI_getMissionAIType() != MISSIONAI_CONSTRUCT)
		{
			iSurplass++;
		}
	}

	if (iSurplass > iExtra)
	{
		m_unit->scrap();
		return true;
	}

	return false;
}

bool CvUnitDefaultAI::moveToOurTerritory(int maxMoves)
{
	const int iSearchRange = searchRange(maxMoves);

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		if (pLoopPlot->area() == m_unit->plot()->area())
		{
			if (pLoopPlot->getOwner() == m_unit->getOwner())
			{
				int iTurns;

				if (m_unit->generatePath(pLoopPlot, MOVE_NO_ENEMY_TERRITORY, true, &iTurns, maxMoves))
				{
					const CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();

					if (!exposedToDanger(endTurnPlot, 75))
					{
						return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY(), MOVE_NO_ENEMY_TERRITORY);
					}
				}
			}
		}
	}

	return false;
}

// To cope with units like heroes, that are constructed under various unitAIs,
//	but which can construct buildings as an option,
//	m_unit rouine determines whether a switch to doing so is appropriate, and if so performs it.
bool CvUnitDefaultAI::checkSwitchToConstruct()
{
	// Don't bail on a city for which we are the only defender
	if (m_unit->getUnitInfo().getNumBuildings() < 1 || m_unit->plot()->getPlotCity() != NULL && m_unit->plot()->getNumDefenders(m_unit->getOwner()) == 1)
	{
		return false;
	}
	CvPlot* pBestConstructPlot;
	CvPlot* pBestPlot;
	CvUnit* eBestTargetingUnit;
	BuildingTypes eBestBuilding;

	// What are the relative values of the possible constructed buildings vs our military value.
	const int iMilitaryValue = GET_PLAYER(m_unit->getOwner()).AI_unitValue(m_unit->getUnitType(), getUnitAIType(), m_unit->area());

	if (getBestConstructValue(MAX_INT, MAX_INT, 0, 0, true, pBestConstructPlot, pBestPlot, eBestTargetingUnit, eBestBuilding) > iMilitaryValue)
	{
		// If we are grouped must ungroup before enacting m_unit
		if (m_unit->getGroup()->getNumUnits() > 1)
		{
			m_unit->joinGroup(NULL);
		}
		return enactConstruct(pBestConstructPlot, pBestPlot, eBestTargetingUnit, eBestBuilding);
	}
	// If m_unit is a bit out-dated as a military unit consider joining a city as a specialist
	UnitTypes eBestUnit = GET_PLAYER(m_unit->getOwner()).bestBuildableUnitForAIType(m_unit->getDomainType(), getUnitAIType());
	CvCity* pCapitalCity = GET_PLAYER(m_unit->getOwner()).getCapitalCity();

	if (eBestUnit != NO_UNIT && pCapitalCity != NULL
	&& GET_PLAYER(m_unit->getOwner()).AI_unitValue(eBestUnit, getUnitAIType(), pCapitalCity->area()) > iMilitaryValue * 3/2)
	{
		return join();
	}
	return false;
}

bool CvUnitDefaultAI::enactConstruct(CvPlot* pBestConstructPlot, CvPlot* pBestPlot, CvUnit* eBestTargetingUnit, 
	BuildingTypes eBestBuilding)
{
	if (pBestPlot == NULL || pBestConstructPlot == NULL || eBestBuilding == NO_BUILDING)
	{
		return false;
	}
	GET_PLAYER(m_unit->getOwner()).AI_changeNumBuildingsNeeded(eBestBuilding, -1);

	if (m_unit->atPlot(pBestConstructPlot))
	{
		m_unit->getGroup()->pushMission(MISSION_CONSTRUCT, eBestBuilding);
		return true;
	}
	FAssert(!m_unit->atPlot(pBestPlot));

	//	Take over responsibility from any overridden targeting unit
	if (eBestTargetingUnit != NULL)
	{
		CvUnitDefaultAI* eBestTargetingUnitAI = dynamic_cast<CvUnitDefaultAI*>(eBestTargetingUnit->AI());
		eBestTargetingUnitAI->m_eIntendedConstructBuilding = NO_BUILDING;
	}
	m_eIntendedConstructBuilding = eBestBuilding;

	// If we have to move outside our own territory heal first
	if (m_unit->getDamage() > 0 && pBestPlot->getOwner() != m_unit->getOwner())
	{
		// May not avoid healing on a bad m_unit->plot though since it's not going through heal() - watch for m_unit
		m_unit->getGroup()->pushMission(MISSION_HEAL, -1, -1, 0, false, false, NO_MISSIONAI, pBestConstructPlot);
		return true;
	}
	return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_NO_ENEMY_TERRITORY | MOVE_WITH_CAUTION | MOVE_AVOID_ENEMY_UNITS, false, false, MISSIONAI_CONSTRUCT, pBestConstructPlot);
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
	typedef stdext::hash_map<CityConstruct, CvUnit*> CityConstructMap;

	// Get a map of buildings that units are planning on constructing (so we can ignore them when giving new construct orders)
	CityConstructMap getConstructBuildings(const CvPlayer& player, const CvSelectionGroup* ignoreGroup)
	{
		CityConstructMap constructions;
		foreach_(const CvSelectionGroup * pLoopSelectionGroup, player.groups())
		{
			if (pLoopSelectionGroup != ignoreGroup
				&& pLoopSelectionGroup->AI_getMissionAIType() == MISSIONAI_CONSTRUCT)
			{
				CvUnit* targetingUnit = pLoopSelectionGroup->getHeadUnit();
				CvUnitDefaultAI* targetingUnitAI = dynamic_cast<CvUnitDefaultAI*>(targetingUnit);

				//	Have to handle the case of a group with no units.  This can occur after a force group
				//	split due to things like revocation of open borders that forces units to be unceremoniously
				//	moved to another m_unit->plot (and ungroups them in doing so since it is possible that only a subset
				//	of a group has to move)
				if (targetingUnitAI != NULL)
				{
					constructions[CityConstruct(pLoopSelectionGroup->AI_getMissionAIPlot(), 
						targetingUnitAI->getIntendedConstructBuilding())] = targetingUnit;
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

int CvUnitDefaultAI::getBestConstructValue(int iMaxCount, int iMaxSingleBuildingCount, int iDecayProbabilityRate, 
	int iThreshold, bool assumeSameValueEverywhere, CvPlot*& pBestConstructPlot, CvPlot*& pBestPlot, 
	CvUnit*& pBestTargetingUnit, BuildingTypes& eBestBuilding)
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
	//	If we already have a chosen construction targeted then start with a presumption
	//	we'll stick to it unless something significantly better is found
	if (m_eIntendedConstructBuilding != NO_BUILDING && m_unit->getGroup()->AI_getMissionAIType() == MISSIONAI_CONSTRUCT)
	{
		CvCity* city = m_unit->getGroup()->AI_getMissionAIPlot()->getPlotCity();

		if (city != nullptr
			&& m_unit->canConstruct(city->plot(), m_eIntendedConstructBuilding)
			&& m_unit->generatePath(city->plot(), MOVE_NO_ENEMY_TERRITORY, true)
			)
		{
			iBestValue = (city->AI_buildingValue(m_eIntendedConstructBuilding) * 110) / 100;
			pBestPlot = m_unit->getPathEndTurnPlot();
			pBestConstructPlot = city->plot();
			eBestBuilding = m_eIntendedConstructBuilding;
		}
	}

	typedef stdext::hash_map<BuildingTypes, int> PossibleBuildingsMap;
	PossibleBuildingsMap possibleBuildings;

	const CvPlayerAI& player = GET_PLAYER(m_unit->getOwner());
	// Determine the list of building types we could make, based on what already exists
	for (int iI = 0; iI < m_unit->getUnitInfo().getNumBuildings(); iI++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(m_unit->getUnitInfo().getBuildings(iI));
		if (eBuilding != NO_BUILDING
			&& player.canConstruct(eBuilding, false, false, true)
			&& player.AI_getNumBuildingsNeeded(eBuilding, (m_unit->getDomainType() == DOMAIN_SEA)) > 0)
		{
			const int numBuildings = algo::count_if(player.cities(), CvCity::fn::getNumRealBuilding(eBuilding) != 0);
			const bool canBuild = numBuildings < iMaxCount &&
				(
					iDecayProbabilityRate == 0 || getDecayedConstructProbability(iConstructBelow, iDecayProbabilityRate, numBuildings) >= iContructRand
				);

			if (canBuild)
			{
				possibleBuildings[eBuilding] = 0;
			}
		}
	}

	if (possibleBuildings.size() == 0)
	{
		return 0;
	}

	const CityConstructMap constructions = getConstructBuildings(player, m_unit->getGroup());
	const CvReachablePlotSet plotSet(m_unit->getGroup(), MOVE_NO_ENEMY_TERRITORY, MAX_INT);

	int iBestWeightedValue = 0;

	foreach_(CvCity * pLoopCity, player.cities())
	{
		//if (plotValid(pLoopCity->plot()) && pLoopCity->area() == m_unit->area())
		if (plotSet.find(pLoopCity->plot()) == plotSet.end())
			continue;
		if (pLoopCity->plot()->isVisibleEnemyUnit(m_unit)) // Koshling - m_unit line is questionable
			continue;

		for (PossibleBuildingsMap::iterator itr = possibleBuildings.begin(); itr != possibleBuildings.end(); ++itr)
		{
			const BuildingTypes buildingType = itr->first;

			if (player.getBuildingCount(buildingType) >= iMaxSingleBuildingCount)
				continue;

			// Check some other unit hasn't already got m_unit city targeted to construct m_unit building
			CityConstructMap::const_iterator foundUnit = constructions.find(CityConstruct(pLoopCity->plot(), buildingType));
			CvUnit* targetingUnit = foundUnit == constructions.end() ? NULL : foundUnit->second;
			// cityConstructionTargeted(pLoopCity, buildingType, m_unit->getGroup());

			const bool betterThanCurrentUnit =
				targetingUnit == nullptr ||
				(
					targetingUnit->plot()->getOwner() != m_unit->getOwner()
					&& m_unit->plot()->getOwner() == m_unit->getOwner()
					&& m_unit->generatePath(pLoopCity->plot(), MOVE_OUR_TERRITORY, true)
				);

			// If we're a better choice due to being already inside our own territory
			// TODO:	How about we just use straight line distance instead?
			//			Although it would probably be rare for targetingUnit to not be null so m_unit check we be
			//			done infrequently.
			if (betterThanCurrentUnit
				&& m_unit->canConstruct(pLoopCity->plot(), buildingType)
				)
			{
				PROFILE("CvUnitDefaultAI::getBestConstructValue.Pathing");

				int iPathTurns;
				if (m_unit->generatePath(pLoopCity->plot(), MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns))
				{
					//	When evaluating whether a hero unit should instead go and produce a building
					//	we want to avoid expensive calculations for every city, when in practise the kinds
					//	of things heroes can build have global effects and thus basically the same value
					//	everywhere that they have any value (note that m_unit is a heuristic assumption that is true CURRENTLY,
					//	but may need to be revisited)
					//	Koshling - also extended m_unit to subdued animals for performance reasons - generally for all cities
					//	and animal buiding CAN be built in m_unit will be a good approximation
					int iValue = 0;
					if (assumeSameValueEverywhere && itr->second > 0)
					{
						iValue = itr->second;
					}
					else
					{
						iValue = pLoopCity->AI_buildingValue(buildingType);

						itr->second = iValue;
					}

					//	Weight by ease of reaching and population slightly in the case of same-everywhere asserted
					//	builds
					int iWeightedValue = (iValue * 5) / (4 + iPathTurns);

					if (assumeSameValueEverywhere)
					{
						iWeightedValue = (iWeightedValue * (5 + pLoopCity->getPopulation())) / 10;
					}
					else
					{
						iWeightedValue = iValue;
					}

					if (iValue > iThreshold && iWeightedValue > iBestWeightedValue)
					{
						iBestValue = iValue;
						iBestWeightedValue = iWeightedValue;
						pBestPlot = m_unit->getPathEndTurnPlot();
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
//   constructed by m_unit GP type.
bool CvUnitDefaultAI::construct(int iMaxCount, int iMaxSingleBuildingCount, int iThreshold, bool bDecayProbabilities, bool assumeSameValueEverywhere)
{
	PROFILE_FUNC();

	if (m_unit->getUnitInfo().getNumBuildings() < 1)
	{
		return false;
	}
	CvPlot* pBestPlot;
	CvPlot* pBestConstructPlot;
	CvUnit* eBestTargetingUnit;
	BuildingTypes eBestBuilding;

	if (getBestConstructValue(iMaxCount, iMaxSingleBuildingCount, bDecayProbabilities ? 50 : 0, iThreshold, assumeSameValueEverywhere, pBestConstructPlot, pBestPlot, eBestTargetingUnit, eBestBuilding) > 0)
	{
		if (gUnitLogLevel >= 2)
		{
			logBBAI("	%S at (%d,%d) going to construct %S at (%d,%d)", m_unit->getName(0).GetCString(), m_unit->getX(), m_unit->getY(), GC.getBuildingInfo(eBestBuilding).getDescription(), pBestConstructPlot->getX(), pBestConstructPlot->getY());
		}
		return enactConstruct(pBestConstructPlot, pBestPlot, eBestTargetingUnit, eBestBuilding);
	}
	return false;
}

// Returns true if a mission was pushed...
// Try to get the best outcome mission done
bool CvUnitDefaultAI::outcomeMission()
{
	PROFILE_FUNC();

	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestMissionPlot = NULL;
	MissionTypes eBestMission = NO_MISSION;
	int iBestValue = 0;

	// favor the closest city
	CvCity* pClosestCity = m_unit->plot()->getPlotCity();
	if (!pClosestCity)
		pClosestCity = GC.getMap().findCity(m_unit->getX(), m_unit->getY(), m_unit->getOwner(), NO_TEAM, true, false);

	std::vector<std::pair<MissionTypes, const CvOutcomeList*> > aMissions;

	const CvUnitInfo& kInfo = m_unit->getUnitInfo();

	for (int iI = 0; iI < kInfo.getNumActionOutcomes(); iI++)
	{
		aMissions.push_back(std::make_pair(kInfo.getActionOutcomeMission(iI), kInfo.getActionOutcomeList(iI)));
	}

	// check the unit combat types for outcome missions
	for (std::map<UnitCombatTypes, UnitCombatKeyedInfo>::const_iterator it = m_unit->getUnitCombatKeyedInfo().begin(), end = m_unit->getUnitCombatKeyedInfo().end(); it != end; ++it)
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

	CvReachablePlotSet	plotSet(m_unit->getGroup(), MOVE_NO_ENEMY_TERRITORY, MAX_INT);

	for (std::vector<std::pair<MissionTypes, const CvOutcomeList*> >::iterator it = aMissions.begin(); it != aMissions.end(); ++it)
	{
		const MissionTypes eMission = it->first;
		const CvOutcomeList* pOutcomeList = it->second;

		if (eMission != NO_MISSION)
		{
			if (pOutcomeList->isPossibleSomewhere(*m_unit))
			{
				if (pClosestCity)
				{
					foreach_(const CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
					{
						//if (pLoopCity->area() == m_unit->area())
						if (plotSet.find(pLoopCity->plot()) != plotSet.end())
						{
							if (pOutcomeList->isPossibleInPlot(*m_unit, *(pLoopCity->plot())))
							{
								const int iValue = pOutcomeList->AI_getValueInPlot(*m_unit, *(pLoopCity->plot()));
								if (iValue > iBestValue)
								{
									if (m_unit->generatePath(pLoopCity->plot(), MOVE_NO_ENEMY_TERRITORY, true))
									{
										iBestValue = iValue;
										pBestPlot = m_unit->getPathEndTurnPlot();
										pBestMissionPlot = pLoopCity->plot();
										eBestMission = eMission;
									}
								}
							}
						}
					}
				}
				else
				{
					// There is no city in m_unit m_unit->area, so try to execute an outcome mission at the current location
					if (pOutcomeList->isPossibleInPlot(*m_unit, *(m_unit->plot())))
					{
						const int iValue = pOutcomeList->AI_getValueInPlot(*m_unit, *(m_unit->plot()));
						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = m_unit->getPathEndTurnPlot();
							pBestMissionPlot = m_unit->plot();
							eBestMission = eMission;
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestMissionPlot != NULL) && (eBestMission != NO_MISSION))
	{
		if (gUnitLogLevel >= 2)
		{
			logBBAI("	%S at (%d,%d) going to enact mission %S at (%d,%d)", m_unit->getName(0).GetCString(), m_unit->getX(), m_unit->getY(), GC.getMissionInfo(eBestMission).getDescription(), pBestMissionPlot->getX(), pBestMissionPlot->getY());
		}

		if (m_unit->atPlot(pBestMissionPlot))
		{
			if (m_unit->getGroup()->pushMissionInternal(eBestMission))
			{
				return true;
			}
		}
		else
		{
			FAssert(!m_unit->atPlot(pBestPlot));

			//	If we have to move outside our own territory heal first
			if (m_unit->getDamage() > 0 && pBestPlot->getOwner() != m_unit->getOwner())
			{
				if (m_unit->getGroup()->pushMissionInternal(MISSION_HEAL, -1, -1, 0, false, false, NO_MISSIONAI, pBestMissionPlot))
				{
					return true;
				}
			}

			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD					  03/09/09								jdog5000	  */
			/*																							  */
			/* Unit AI																					  */
			/************************************************************************************************/
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_NO_ENEMY_TERRITORY | MOVE_WITH_CAUTION | MOVE_AVOID_ENEMY_UNITS, false, false, NO_MISSIONAI, pBestMissionPlot);
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD					   END												  */
			/************************************************************************************************/
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::switchHurry()
{
	PROFILE_FUNC();

	CvCity* pCity = m_unit->plot()->getPlotCity();

	if ((pCity == NULL) || (pCity->getOwner() != m_unit->getOwner()))
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
				if (pCity->getBuildingProduction(eBuilding) == 0)
				{
					if (m_unit->getMaxHurryProduction(pCity) >= pCity->getProductionNeeded(eBuilding))
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
			if (m_unit->canHurry(m_unit->plot()))
			{
				m_unit->getGroup()->pushMission(MISSION_HURRY);
			}
			return true;
		}
		//This should always be true because just because the city can't hurry doesn't mean it hasn't successfully made its determination for m_unit function.
		//Thus return true was moved outside of canHurry but kept within the check that ensures that the production building chosen is the next to be built confirmation.
		FErrorMsg("error");
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::hurry(bool bAny)
{
	PROFILE_FUNC();

	bool bHurry;
	int iTurnsLeft;
	int iPathTurns;
	int iValue;

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestHurryPlot = NULL;

	foreach_(const CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
	{
		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD					  08/19/09								jdog5000	  */
		/*																							  */
		/* Unit AI, Efficiency																		  */
		/************************************************************************************************/
				// BBAI efficiency: check same m_unit->area
		if ((pLoopCity->area() == m_unit->area()) && plotValid(pLoopCity->plot()))
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD					   END												  */
			/************************************************************************************************/
		{
			if (m_unit->canHurry(pLoopCity->plot()))
			{
				if (bAny || GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_HURRY, m_unit->getGroup()) == 0)
				{
					if (generateSafePathforVulnerable(pLoopCity->plot(), &iPathTurns))
					{
						bHurry = false;

						if (pLoopCity->isProductionBuilding())
						{
							if (bAny || isWorldWonder(pLoopCity->getProductionBuilding()))
							{
								bHurry = true;
							}
						}

						if (bHurry)
						{
							iTurnsLeft = pLoopCity->getProductionTurnsLeft();

							iTurnsLeft -= iPathTurns;

							if (iTurnsLeft > (bAny ? 1 : 8))
							{
								iValue = iTurnsLeft;

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestPlot = m_unit->getPathEndTurnPlot();
									pBestHurryPlot = pLoopCity->plot();
								}
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestHurryPlot != NULL))
	{
		if (!m_unit->atPlot(pBestHurryPlot))
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			if (generateSafePathforVulnerable(pBestHurryPlot, &iPathTurns))
			{
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_HURRY, pBestHurryPlot);
			}
			else
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP);
				return true;
			}
		}
		else
		{
			m_unit->getGroup()->pushMission(MISSION_HURRY);
			return true;
		}
	}
	return false;
}

namespace scoring {


	// Implementation details of the scoring
	namespace detail {
		// Traits class for items that can be scored (probably plots, cities, units, etc.)
		// It describes how to interact with the object (currently only how to get the m_unit->plot the item is on)
		// The default implementation will work for CvUnit, CvCity, CvSelectionGroup at least
		template < class ItemTy_ >
		struct ItemTraits
		{
			typedef ItemTy_ item_value;

			static const CvPlot* plot(const ItemTy_* item) { return item->plot(); }
		};

		// Specialization of the traits for CvPlot as it can just return itself as its own m_unit->plot!
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
	ScoreResult<ItemTy_> findBestScore(Itr_ begin, Itr_ end, bst::function<int(const ItemTy_*)> scoreFn, bst::function<bool(const ItemTy_*)> validationFn)
	{
		using namespace detail;

		std::vector< ItemScore<ItemTy_> > scores;

		// Calculate scores
		for (Itr_ itr = begin; itr != end; ++itr)
		{
			scores.push_back(ItemScore<ItemTy_>(*itr, scoreFn(*itr)));
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
	bool canSafePathToCity(const CvUnit* unit, const CvCity* item)
	{
		FAssert(unit != NULL);
		FAssert(item != NULL);
		return dynamic_cast<CvUnitDefaultAI*>(unit->AI())->generateSafePathforVulnerable(item->plot());
	}

	// Validation function that will ensure a unit can reach an item
	bool canPathToCity(const CvUnit* unit, const CvCity* item)
	{
		FAssert(unit != NULL);
		FAssert(item != NULL);
		return unit->generatePath(item->plot());
	}

	// Scoring helper function to attenuate score by distance, with optional boost for current m_unit->plot
	int applyDistanceScoringFactor(int score, const CvPlot* sourcePlot, const CvPlot* targetPlot, const int currentPlotBoost = 1)
	{
		FAssert(sourcePlot != NULL);
		FAssert(targetPlot != NULL);

		if (score > 0)
		{
			if (sourcePlot != targetPlot)
			{
				// Has to be > 0 because we aren't on the same m_unit->plot...
				const int dist = stepDistance(sourcePlot->getX(), sourcePlot->getY(), targetPlot->getX(), targetPlot->getY());
				score /= dist * dist;
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
	int scoreInquisitionTarget(const CvUnit* unit, const CvCity* targetCity)
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
	CvCity* getReligiousVictoryTarget(const CvUnit* unit)
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

bool CvUnitDefaultAI::doInquisition()
{
	int iPathTurns = 0;

	CvCity* pTargetCity = GET_PLAYER(m_unit->getOwner()).getInquisitionRevoltCity(m_unit, false, GC.getOC_MIN_REV_INDEX(), 0);
	if (pTargetCity == NULL)
	{
		pTargetCity = GET_PLAYER(m_unit->getOwner()).getTeamInquisitionRevoltCity(m_unit, false, GC.getOC_MIN_REV_INDEX(), 0);
		if (pTargetCity == NULL)
		{
			pTargetCity = getReligiousVictoryTarget(m_unit);
		}
	}
	if (pTargetCity != NULL)
	{
		if (m_unit->atPlot(pTargetCity->plot()))
		{
			return m_unit->performInquisition();
		}
		if (generateSafePathforVulnerable(pTargetCity->plot(), &iPathTurns))
		{
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pTargetCity->getX(), pTargetCity->getY(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_INQUISITION, pTargetCity->plot());
		}
		return false;
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::greatWork()
{
	PROFILE_FUNC();

	int iValue;
	int iPathTurns = 0;

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestGreatWorkPlot = NULL;

	foreach_(const CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
	{
		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD					  08/19/09								jdog5000	  */
		/*																							  */
		/* Unit AI, Efficiency																		  */
		/************************************************************************************************/
				// BBAI efficiency: check same m_unit->area
		if ((pLoopCity->area() == m_unit->area()) && plotValid(pLoopCity->plot()))
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD					   END												  */
			/************************************************************************************************/
		{
			if (m_unit->canGreatWork(pLoopCity->plot()))
			{
				if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_GREAT_WORK, m_unit->getGroup()) == 0)
				{
					if (generateSafePathforVulnerable(pLoopCity->plot(), &iPathTurns))
					{
						iValue = pLoopCity->AI_calculateCulturePressure(true);
						iValue -= ((100 * pLoopCity->getCulture(pLoopCity->getOwner())) / std::max(1, m_unit->getGreatWorkCulture()));
						if (iValue > 0)
						{
							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = m_unit->getPathEndTurnPlot();
								pBestGreatWorkPlot = pLoopCity->plot();
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestGreatWorkPlot != NULL))
	{
		if (m_unit->atPlot(pBestGreatWorkPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_GREAT_WORK);
			return true;
		}
		else
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			if (generateSafePathforVulnerable(pBestPlot, &iPathTurns))
			{
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_GREAT_WORK, pBestGreatWorkPlot);
			}
			else
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP);
				return true;
			}
		}
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::offensiveAirlift()
{
	PROFILE_FUNC();

	CvCity* pTargetCity;
	int iValue;

	if (m_unit->getGroup()->getNumUnits() > 1)
	{
		return false;
	}

	if (m_unit->area()->getTargetCity(m_unit->getOwner()) != NULL)
	{
		return false;
	}

	const CvCity* pCity = m_unit->plot()->getPlotCity();

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

	foreach_(const CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
	{
		if (pLoopCity->area() != pCity->area())
		{
			if (m_unit->canAirliftAt(pCity->plot(), pLoopCity->getX(), pLoopCity->getY()))
			{
				pTargetCity = pLoopCity->area()->getTargetCity(m_unit->getOwner());

				if (pTargetCity != NULL)
				{
					AreaAITypes eAreaAIType = pTargetCity->area()->getAreaAIType(m_unit->getTeam());
					if (((eAreaAIType == AREAAI_OFFENSIVE) || (eAreaAIType == AREAAI_DEFENSIVE) || (eAreaAIType == AREAAI_MASSING))
						|| pTargetCity->AI_isDanger())
					{
						iValue = 10000;

						iValue *= (GET_PLAYER(m_unit->getOwner()).AI_militaryWeight(pLoopCity->area()) + 10);
						iValue /= (GET_PLAYER(m_unit->getOwner()).AI_totalAreaUnitAIs(pLoopCity->area(), getUnitAIType()) + 10);

						iValue += std::max(1, ((GC.getMap().maxStepDistance() * 2) - GC.getMap().calculatePathDistance(pLoopCity->plot(), pTargetCity->plot())));

						if (getUnitAIType() == UNITAI_PARADROP)
						{
							CvCity* pNearestEnemyCity = GC.getMap().findCity(pLoopCity->getX(), pLoopCity->getY(), NO_PLAYER, NO_TEAM, false, false, m_unit->getTeam());

							if (pNearestEnemyCity != NULL)
							{
								int iDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pNearestEnemyCity->getX(), pNearestEnemyCity->getY());
								if (iDistance <= m_unit->getDropRange())
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
		m_unit->getGroup()->pushMission(MISSION_AIRLIFT, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::paradrop(int iRange)
{
	PROFILE_FUNC();

	if (m_unit->getGroup()->getNumUnits() > 1)
	{
		return false;
	}

	const CvPlot* pPlot = m_unit->plot();

	if (!m_unit->canParadrop(pPlot))
	{
		return false;
	}

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	const int iParatrooperCount = m_unit->plot()->plotCount(PUF_isParadrop, -1, -1, NULL, m_unit->getOwner());
	FAssert(iParatrooperCount > 0);

	const int iSearchRange = searchRange(iRange);

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		if (m_unit->isPotentialEnemy(pLoopPlot->getTeam(), pLoopPlot))
		{
			if (m_unit->canParadropAt(pPlot, pLoopPlot->getX(), pLoopPlot->getY()))
			{
				int iValue = 0;

				const PlayerTypes eTargetPlayer = pLoopPlot->getOwner();
				FAssert(NO_PLAYER != eTargetPlayer);

				// Bonus values for bonuses the AI has are less than 10 for non-strategic resources... since m_unit is
				// in the AI territory they probably have it
				if (NO_BONUS != pLoopPlot->getNonObsoleteBonusType(m_unit->getTeam()))
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
							const int iAttackerCount = GET_PLAYER(m_unit->getOwner()).AI_adjacentPotentialAttackers(pAdjacentPlot, true);
							const int iDefenderCount = pAdjacentPlot->getNumVisiblePotentialEnemyDefenders(m_unit);
							iValue += 20 * (attackOdds(pAdjacentPlot, true) - ((50 * iDefenderCount) / (iParatrooperCount + iAttackerCount)));
						}
					}
				}

				if (iValue > 0)
				{
					iValue += pLoopPlot->defenseModifier(m_unit->getTeam(), m_unit->ignoreBuildingDefense());

					const CvUnit* pInterceptor = m_unit->bestInterceptor(pLoopPlot);
					if (NULL != pInterceptor)
					{
						int iInterceptProb = m_unit->isSuicide() ? 100 : pInterceptor->currInterceptionProbability();

						iInterceptProb *= std::max(0, (100 - m_unit->evasionProbability()));
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
		m_unit->getGroup()->pushMission(MISSION_PARADROP, pBestPlot->getX(), pBestPlot->getY());
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
bool CvUnitDefaultAI::protect(int iOddsThreshold, int iMaxPathTurns)
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	CvPlot* endTurnPlot = NULL;
	int iValue;
	int iBestValue;

	iBestValue = 0;
	pBestPlot = NULL;

	CvReachablePlotSet plotSet(m_unit->getGroup(), 0, searchRange(iMaxPathTurns));

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		pLoopPlot = itr.plot();
		if (pLoopPlot->getOwner() == m_unit->getOwner())
		{
			//	Koshling - plotValid() call not needed when using a CvReachablePlotSet
			if (/*AI_plotValid(pLoopPlot) &&*/ pLoopPlot->area() == m_unit->area())
			{
				if (pLoopPlot->isVisible(m_unit->getTeam(), false) && pLoopPlot->isVisibleEnemyUnit(m_unit))
				{
					if (!m_unit->atPlot(pLoopPlot))
					{
						// BBAI efficiency: Check m_unit->area for land units
						if ((m_unit->getDomainType() != DOMAIN_LAND) || (pLoopPlot->area() == m_unit->area()) || m_unit->getGroup()->canMoveAllTerrain())
						{
							// BBAI efficiency: Most of the time, path will exist and odds will be checked anyway.  When path doesn't exist, checking path
							// takes longer.  Therefore, check odds first.
							iValue = m_unit->getGroup()->AI_attackOdds(pLoopPlot, true);

							if ((iValue >= finalOddsThreshold(pLoopPlot, iOddsThreshold)) && (iValue * 50 > iBestValue))
							{
								int iPathTurns;
								if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns, iMaxPathTurns))
								{
									// BBAI TODO: Other units targeting m_unit already (if path turns > 1 or 0)?
									iValue *= 100;

									iValue /= (2 + iPathTurns);

									if (iValue > iBestValue)
									{
										endTurnPlot = m_unit->getPathEndTurnPlot();

										if (!exposedToDanger(endTurnPlot, 100 - (100 - iOddsThreshold) / 2))
										{
											iBestValue = iValue;
											pBestPlot = endTurnPlot;
											FAssert(!m_unit->atPlot(pBestPlot));
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
		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}

	return false;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					   END												  */
/************************************************************************************************/

//	This routine effectively parallel the test in CvCityAI for building see attack/reserve units
//	in response to a sea invader in the same sea m_unit->area (but not necessarily local)
bool CvUnitDefaultAI::seaAreaAttack()
{
	PROFILE_FUNC();

	CvPlot* pBestPlot = NULL;
	int	iBestValue = 0;
	const CvPlayerAI& kPlayer = GET_PLAYER(m_unit->getOwner());

	CvReachablePlotSet plotSet(m_unit->getGroup(), 0);
	{
		PROFILE("CvUnitDefaultAI::seaAreaAttack.Populate");
		plotSet.Populate(15);
	}
	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		CvPlot* pLoopPlot = itr.plot();
		if (pLoopPlot->area() == m_unit->area() && pLoopPlot->isVisible(m_unit->getTeam(), false) && pLoopPlot->getOwner() == m_unit->getOwner())
		{
			int iCount = pLoopPlot->plotCount(PUF_isEnemy, m_unit->getOwner(), 0, NULL, NO_PLAYER, NO_TEAM, PUF_isVisible, m_unit->getOwner());

			//	Refine m_unit so that not EVERY unit heads for the same enemy incursion!
			if (iCount > 0)
			{
				int iValue = (10000 + GC.getGame().getSorenRandNum(5000, "AI sea m_unit->area attack")) / (2 + kPlayer.AI_countNumLocalNavy(pLoopPlot, 4));

				PROFILE("CvUnitDefaultAI::seaAreaAttack.Pathing");
				int iPathTurns;
				if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns, iBestValue == 0 ? MAX_INT : iValue / iBestValue))
				{
					PROFILE("CvUnitDefaultAI::seaAreaAttack.Pathed");
					iValue /= (1 + iPathTurns);

					if (iValue > iBestValue)
					{
						CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();

						if (!exposedToDanger(endTurnPlot, 60) && (!m_unit->atPlot(endTurnPlot) || m_unit->canAmbush(endTurnPlot)))
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
		if (!m_unit->atPlot(pBestPlot))
		{
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
		}
		// Sharing m_unit->plot with the enemy
		FAssertMsg(m_unit->canAmbush(pBestPlot), "Something is wrong. . .")
			return m_unit->getGroup()->pushMissionInternal(MISSION_AMBUSH, pBestPlot->getX(), pBestPlot->getY());
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::patrol(bool bIgnoreDanger)
{
	PROFILE_FUNC();

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pAdjacentPlot, m_unit->plot()->adjacent())
	{
		if (plotValid(pAdjacentPlot)
		&& (bIgnoreDanger || !pAdjacentPlot->isVisibleEnemyUnit(m_unit))
		&& m_unit->getGroup()->canEnterPlot(pAdjacentPlot, false))
		{
			int iValue = (1 + GC.getGame().getMapRandNum(10000, "AI Patrol"));

			if (m_unit->isAnimal())
			{
				if (GC.getGame().isOption(GAMEOPTION_DANGEROUS_WILDLIFE))
				{
					if (pAdjacentPlot->isVisibleEnemyUnit(m_unit))
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
			else if (m_unit->isHominid())
			{
				if (pAdjacentPlot->isVisibleEnemyUnit(m_unit))
				{
					iValue += 5000;
				}
				if (pAdjacentPlot->isRevealedGoody(m_unit->getTeam()))
				{
					iValue += 4000;
				}
				foreach_(const CvPlot * pAdjacentPlot2, pAdjacentPlot->adjacent())
				{
					if (pAdjacentPlot2->area() == pAdjacentPlot->area())
					{
						if (pAdjacentPlot2->isVisibleEnemyUnit(m_unit))
						{
							iValue += 3000;
						}
						if (pAdjacentPlot->isRevealedGoody(m_unit->getTeam()))
						{
							iValue += 2000;
						}
					}
				}
				if (pAdjacentPlot->isOwned())
				{
					iValue += 3000;

					if (pAdjacentPlot->getOwner() == m_unit->getOwner())
					{
						iValue += 1000;
					}
				}
				else if (pAdjacentPlot->isAdjacentOwned())
				{
					iValue += 2000;
				}
			}

			if (iValue > iBestValue && !bIgnoreDanger && exposedToDanger(pAdjacentPlot, 60))
			{
				iBestValue = iValue;
				pBestPlot = pAdjacentPlot;
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::defend()
{
	PROFILE_FUNC();

	int iPathTurns;

	if (defendPlot(m_unit->plot()))
	{
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return true;
	}

	const int iSearchRange = searchRange();

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		if (plotValid(pLoopPlot))
		{
			if (defendPlot(pLoopPlot))
			{
				if (!(pLoopPlot->isVisibleEnemyUnit(m_unit)))
				{
					if (!m_unit->atPlot(pLoopPlot) && m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns, 1))
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
		if (!(pBestPlot->isCity()) && (m_unit->getGroup()->getNumUnits() > 1))
		{
			m_unit->getGroup()->AI_makeForceSeparate();
		}

		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}

	return false;
}

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::safety(int iRange)
{
	PROFILE_FUNC();

	const int iSearchRange = searchRange(iRange);

	const bool bAnimalDanger = GET_PLAYER(m_unit->getOwner()).AI_getVisiblePlotDanger(m_unit->plot(), 1, true);

	//GC.getGame().logOOSSpecial(3,iValue,getID());
	const CvPlot* pBestPlot = NULL;
	int iBestValue = 0;
	int iPass = 0;
	for (; iPass < 2 && pBestPlot == NULL; ++iPass)
	{
		if (!GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
		{
			CvReachablePlotSet plotSet(m_unit->getGroup(), (iPass > 0) ? MOVE_IGNORE_DANGER : 0, iSearchRange);

			for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
			{
				const CvPlot* pLoopPlot = itr.plot();
				if (pLoopPlot == NULL)
					continue;

				int iPathTurns = 0;
				if ((!pLoopPlot->isVisible(m_unit->getTeam(), false) || !pLoopPlot->isVisibleEnemyUnit(m_unit))
					&& m_unit->generatePath(pLoopPlot, ((iPass > 0) ? MOVE_IGNORE_DANGER : 0), true, &iPathTurns, iRange)
					)
				{
					int iCount = 0;

					foreach_(const CvUnit * pLoopUnit, pLoopPlot->units())
					{
						if (pLoopUnit->getOwner() == m_unit->getOwner() && pLoopUnit->canDefend())
						{
							const CvUnit* pHeadUnit = pLoopUnit->getGroup()->getHeadUnit();
							FAssert(pHeadUnit != NULL);
							FAssert(m_unit->getGroup()->getHeadUnit() == m_unit);

							if (pHeadUnit != m_unit
								&& (pHeadUnit->isWaiting() || !pHeadUnit->canMove())
								)
							{
								FAssert(pLoopUnit != m_unit);
								FAssert(pHeadUnit != m_unit->getGroup()->getHeadUnit());
								iCount++;
							}
						}
					}

					int iValue = (iCount * 100);

					//	If the current danger includes animals then simply running to
					//	our borders is defense to at least the animal component of the current danger
					if (bAnimalDanger)
					{
						if (pLoopPlot->getOwner() == m_unit->getOwner())
						{
							iValue += 75;
						}
						else if (pLoopPlot->getOwner() != NO_PLAYER && !atWar(pLoopPlot->getTeam(), m_unit->getTeam()))
						{
							iValue += 50;
						}
					}

					iValue += pLoopPlot->defenseModifier(m_unit->getTeam(), false);


					if (m_unit->atPlot(pLoopPlot))
					{
						iValue += 50;
					}
					else
					{
						iValue += GC.getGame().getSorenRandNum(50, "AI Safety");
					}

					int iTerrainDamage = pLoopPlot->getTotalTurnDamage(m_unit->getGroup());

					if (iTerrainDamage > 0)
					{
						iValue *= std::max(1, 100 - m_unit->getDamagePercent() - iTerrainDamage);
						iValue /= 200;
					}

					iValue = (iValue * 5) / (iPathTurns + 4);

					if (iValue > iBestValue)
					{
						if (GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(pLoopPlot, 1))
						{
							iValue /= 2;
						}
						if (iValue > iBestValue)
						{
							//	If we have to pass through worse danger to get there it's not worth it
							CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();

							if (endTurnPlot == pLoopPlot ||
									!GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(endTurnPlot, 1) ||
									endTurnPlot->defenseModifier(m_unit->getTeam(), false) >= m_unit->plot()->defenseModifier(m_unit->getTeam(), false))
							{
								iBestValue = iValue;
								pBestPlot = pLoopPlot;
							}
						}
					}
				}
			}
		}
		else
		{
			foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
			{
				//GC.getGame().logOOSSpecial(24,iValue,getID());
				if (plotValid(pLoopPlot))
				{
					//GC.getGame().logOOSSpecial(25, pLoopPlot->getX(), pLoopPlot->getY(), m_unit->getID());
					//GC.getGame().logOOSSpecial(26, pLoopPlot->getX(), pLoopPlot->getY(), m_unit->getID());
					int iPathTurns = 0;
					if ((!pLoopPlot->isVisible(m_unit->getTeam(), false) || !pLoopPlot->isVisibleEnemyUnit(m_unit))
					&& m_unit->generatePath(pLoopPlot, (iPass > 0) ? MOVE_IGNORE_DANGER : 0, true, &iPathTurns, iRange))
					{
						//GC.getGame().logOOSSpecial(27, iPass, iPathTurns, iRange);
						//GC.getGame().logOOSSpecial(28, iPass, iPathTurns, iRange);
						int iCount = 0;

						foreach_(const CvUnit * pLoopUnit, pLoopPlot->units())
						{
							if (pLoopUnit->getOwner() == m_unit->getOwner() && pLoopUnit->canDefend())
							{
								const CvUnit* pHeadUnit = pLoopUnit->getGroup()->getHeadUnit();
								FAssert(pHeadUnit != NULL);
								FAssert(m_unit->getGroup()->getHeadUnit() == m_unit);

								if (pHeadUnit != m_unit
									&& (pHeadUnit->isWaiting() || !pHeadUnit->canMove())
									)
								{
									FAssert(pLoopUnit != m_unit);
									FAssert(pHeadUnit != m_unit->getGroup()->getHeadUnit());
									iCount++;
								}
							}
						}

						int iValue = (iCount * 100);

						//	If the current danger includes animals then simply running to
						//	our borders is defense to at least the animal component of the current danger
						if (bAnimalDanger)
						{
							if (pLoopPlot->getOwner() == m_unit->getOwner())
							{
								iValue += 75;
							}
							else if (pLoopPlot->getOwner() != NO_PLAYER && !atWar(pLoopPlot->getTeam(), m_unit->getTeam()))
							{
								iValue += 50;
							}
						}
						//GC.getGame().logOOSSpecial(4,iValue,getID());

						iValue += pLoopPlot->defenseModifier(m_unit->getTeam(), false);

						//GC.getGame().logOOSSpecial(5,iValue,getID());

						if (m_unit->atPlot(pLoopPlot))
						{
							iValue += 50;
						}
						else
						{
							iValue += GC.getGame().getSorenRandNum(50, "AI Safety");
							//GC.getGame().logOOSSpecial(6,iValue,getID());
						}

						int iTerrainDamage = pLoopPlot->getTotalTurnDamage(m_unit->getGroup());

						if (iTerrainDamage > 0)
						{
							iValue *= std::max(1, 100 - m_unit->getDamagePercent() - iTerrainDamage);
							iValue /= 200;
						}

						iValue = (iValue * 5) / (iPathTurns + 4);

						if (iValue > iBestValue)
						{
							if (GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(pLoopPlot, 1))
							{
								iValue /= 2;
							}
							if (iValue > iBestValue)
							{
								//	If we have to pass through worse danger to get there it's not worth it
								CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();

								if (endTurnPlot == pLoopPlot ||
									!GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(endTurnPlot, 1) ||
									endTurnPlot->defenseModifier(m_unit->getTeam(), false) >= m_unit->plot()->defenseModifier(m_unit->getTeam(), false))
								{
									iBestValue = iValue;
									pBestPlot = pLoopPlot;
								}
							}
						}
					}
				}
				//GC.getGame().logOOSSpecial(27,iDX,iDY, m_unit->getID());
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (m_unit->atPlot(pBestPlot))
		{
			OutputDebugString(CvString::format("%S (%d) seeking safety stays put at (%d,%d)...\n", m_unit->getDescription().c_str(), m_unit->getID(), m_unit->getX(), m_unit->getY()).c_str());
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
		OutputDebugString(CvString::format("%S (%d) seeking safety moves to (%d,%d)\n", m_unit->getDescription().c_str(), m_unit->getID(), pBestPlot->getX(), pBestPlot->getY()).c_str());
		// Check iPass > 1 here because we always increment it as we exit the loop, so it will be 1 higher than when pBestPlot was assigned a value
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), ((iPass > 1) ? MOVE_IGNORE_DANGER : 0));
	}
	return false;
}

// Returns true if a mission was pushed... unless it's a mock run.
bool CvUnitDefaultAI::reachHome(const bool bMockRun) const
{
	PROFILE_FUNC();

	OutputDebugString(CvString::format("%S (%d) reachHome (%d,%d)...%d\n", m_unit->getDescription().c_str(), m_unit->getID(), m_unit->getX(), m_unit->getY(), (int)bMockRun).c_str());

	if (bMockRun && m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		return true;
	}
	const CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	// m_unit->baseMoves() is enough, m_unit function is not supposed to send the unit far through hostile land.
	foreach_(const CvPlot * plotX, m_unit->plot()->rect(m_unit->baseMoves(), m_unit->baseMoves()))
	{
		if (plotX == NULL || plotX->getOwner() != m_unit->getOwner() || GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(plotX, 1) || m_unit->atPlot(plotX))
		{
			continue;
		}
		int iPathTurns;
		if (!m_unit->generatePath(plotX, 0, true, &iPathTurns, 1) || iPathTurns > 1)
		{
			continue;
		}
		int iValue = 1000;

		const CvCity* cityNear = GC.getMap().findCity(plotX->getX(), plotX->getY(), m_unit->getOwner(), NO_TEAM, true);

		if (cityNear != NULL)
		{
			iValue /= 1 + plotDistance(plotX->getX(), plotX->getY(), cityNear->getX(), cityNear->getY());
		}

		foreach_(const CvUnit * unitX, plotX->units())
		{
			if (unitX->getOwner() == m_unit->getOwner() && unitX->canDefend())
			{
				const CvUnit* pHeadUnit = unitX->getGroup()->getHeadUnit();

				if (pHeadUnit != m_unit && (pHeadUnit->isWaiting() || !pHeadUnit->canMove()))
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
	if (pBestPlot != NULL)
	{
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0);
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::hide()
{
	PROFILE_FUNC();

	if (m_unit->getInvisibleType() == NO_INVISIBLE)
	{
		return false;
	}
	int iPathTurns;
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	const int iSearchRange = searchRange();

	foreach_(const CvPlot * plotX, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		if (plotValid(plotX))
		{
			{
				bool bValid = true;
				for (int iI = 0; iI < MAX_TEAMS; iI++)
				{
					if (GET_TEAM((TeamTypes)iI).isAlive()
					&& plotX->isSpotterInSight((TeamTypes)iI, m_unit->getInvisibleType()))
					{
						bValid = false;
						break;
					}
				}
				if (!bValid) continue;
			}
			if (!plotX->isVisibleEnemyUnit(m_unit) && m_unit->generatePath(plotX, 0, true, &iPathTurns, 1))
			{
				int iCount = 1;

				foreach_(const CvUnit * unitX, plotX->units())
				{
					if (unitX->getOwner() == m_unit->getOwner() && unitX->canDefend())
					{
						const CvUnit* pHeadUnit = unitX->getGroup()->getHeadUnit();
						FAssert(pHeadUnit != NULL);
						FAssert(m_unit->getGroup()->getHeadUnit() == m_unit);

						if (pHeadUnit != m_unit && (pHeadUnit->isWaiting() || !pHeadUnit->canMove()))
						{
							FAssert(unitX != m_unit);
							FAssert(pHeadUnit != m_unit->getGroup()->getHeadUnit());
							iCount++;
						}
					}
				}

				const int iValue =
					(
						iCount * 100
						+ plotX->defenseModifier(m_unit->getTeam(), false)
						+ (m_unit->atPlot(plotX) ? 50 : GC.getGame().getSorenRandNum(50, "AI Hide"))
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
		if (m_unit->atPlot(pBestPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::goody(int iRange)
{
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

	if (m_unit->isNPC())
	{
		return false;
	}

	FAssert(m_unit->canMove());

	iSearchRange = searchRange(iRange);

	iBestValue = 0;
	pBestPlot = NULL;

	CvReachablePlotSet plotSet(m_unit->getGroup(), 0, iSearchRange);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		pLoopPlot = itr.plot();
		if (pLoopPlot->isRevealedGoody(m_unit->getTeam()))
		{
			if ((!m_unit->canAttack() && !pLoopPlot->isVisibleEnemyUnit(m_unit)) || !exposedToDanger(pLoopPlot, 60))
			{
				if (!m_unit->atPlot(pLoopPlot) && m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns, iRange))
				{
					if (iPathTurns <= iRange)
					{
						iValue = (1 + GC.getGame().getSorenRandNum(10000, "AI Goody"));

						iValue /= (iPathTurns + 1);

						if (iValue > iBestValue)
						{
							//	Does m_unit proposed move leave us exposed at the end of m_unit
							//	turn?
							if (exposedToDanger(m_unit->getPathEndTurnPlot(), 60))
							{
								//	Reject m_unit option if we have less than 60% combat odds
								continue;
							}
							iBestValue = iValue;
							pBestPlot = m_unit->getPathEndTurnPlot();
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}

	return false;
}

bool CvUnitDefaultAI::explorerJoinOffensiveStacks()
{
	//When in an offensive war, we are already sending stacks into enemy territory, so exploring
	//in addition to the stacks is counterproductive, and dangerous
	const bool bOffenseWar = m_unit->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_OFFENSIVE;
	if (bOffenseWar && !m_unit->isHuman() && getUnitAIType() != UNITAI_HUNTER && m_unit->canAttack())	//	Exempt hunters from m_unit behaviour and also defend-only explorers
	{
		//try to join SoD
		if (group(GroupingParams().withUnitAI(UNITAI_ATTACK).maxOwnUnitAI(1).minUnitAI(6).stackOfDoom().allowRegrouping()))
		{
			return true;
		}
		if (group(GroupingParams().withUnitAI(UNITAI_ATTACK).maxOwnUnitAI(1).minUnitAI(4).stackOfDoom().allowRegrouping()))
		{
			return true;
		}
		if (group(GroupingParams().withUnitAI(UNITAI_ATTACK_CITY).maxOwnUnitAI(1).minUnitAI(4).stackOfDoom().allowRegrouping()))
		{
			return true;
		}
		if (group(GroupingParams().withUnitAI(UNITAI_COLLATERAL).maxOwnUnitAI(1).minUnitAI(4).stackOfDoom().allowRegrouping()))
		{
			return true;
		}
		if (group(GroupingParams().withUnitAI(UNITAI_COUNTER).maxOwnUnitAI(1).minUnitAI(3).stackOfDoom().allowRegrouping()))
		{
			return true;
		}
		//try to join attacking stack
		if (group(GroupingParams().withUnitAI(UNITAI_ATTACK).maxOwnUnitAI(1)))
		{
			return true;
		}
		if (group(GroupingParams().withUnitAI(UNITAI_ATTACK_CITY).maxOwnUnitAI(1)))
		{
			return true;
		}
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::explore()
{
	PROFILE_FUNC();

	if (explorerJoinOffensiveStacks())
	{
		return true;
	}

	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	CvPlot* pBestExplorePlot;
	int iPathTurns;
	int iValue;
	int iBestValue;

	iBestValue = 0;
	pBestPlot = NULL;
	pBestExplorePlot = NULL;

	bool bNoContact = (GC.getGame().countCivTeamsAlive() > GET_TEAM(m_unit->getTeam()).getHasMetCivCount(true));

	//	If we had previously selected a target make sure we include it in our evaluation m_unit
	//	time around else dithering between different plots can occur
	CvPlot* pPreviouslySelectedPlot = NULL;
	if (m_unit->getGroup()->AI_getMissionAIType() == MISSIONAI_EXPLORE)
	{
		pPreviouslySelectedPlot = m_unit->getGroup()->AI_getMissionAIPlot();

		if (pPreviouslySelectedPlot != NULL && m_unit->atPlot(pPreviouslySelectedPlot))
		{
			pPreviouslySelectedPlot = NULL;
		}
	}

	std::vector<plotValue>	plotValues;

	CvReachablePlotSet plotSet(m_unit->getGroup(), MOVE_NO_ENEMY_TERRITORY, MAX_INT);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		pLoopPlot = itr.plot();
		PROFILE("AI_explore 1");

		//	Only consider plots that are revealed or adjacent to something we already revealed
		//  ls612: Only consider plots that have no enemy defenders on them
		if (/*AI_plotValid(pLoopPlot) &&*/ !pLoopPlot->isVisibleEnemyDefender(m_unit) && pLoopPlot->area() == m_unit->area() && (pLoopPlot->isRevealed(m_unit->getTeam(), false) || pLoopPlot->isAdjacentRevealed(m_unit->getTeam())))
		{
			iValue = 0;

			if (pLoopPlot->isRevealedGoody(m_unit->getTeam()))
			{
				iValue += 100000;
			}

			if (!(pLoopPlot->isRevealed(m_unit->getTeam(), false)))
			{
				iValue += 10000;
			}
			/*************************************************************************************************/
			/**	Xienwolf Tweak							12/13/08											**/
			/**																								**/
			/**					Reduction in massive Random Spam in Logger files by using Map				**/
			/*************************************************************************************************/
			/**								---- Start Original Code ----									**
						if (iValue > 0 || GC.getGame().getSorenRandNum(4, "AI make explore faster ;)") == 0)
			/**								----  End Original Code  ----									**/
			if (iValue > 0 || pLoopPlot == pPreviouslySelectedPlot || GC.getGame().getMapRandNum(4, "AI make explore faster ;)") == 0)
				/*************************************************************************************************/
				/**	Tweak									END													**/
				/*************************************************************************************************/
			{
				PROFILE("AI_explore 2");

				// XXX is m_unit too slow?
				foreach_(const CvPlot * pAdjacentPlot, pLoopPlot->adjacent())
				{
					if (!pAdjacentPlot->isRevealed(m_unit->getTeam(), false))
					{
						iValue += 1000;
					}
					else if (bNoContact && pAdjacentPlot->getRevealedTeam(m_unit->getTeam(), false) != pAdjacentPlot->getTeam())
					{
						iValue += 100;
					}
				}

				if (iValue > 0)
				{
					if (!pLoopPlot->isVisible(m_unit->getTeam(), false) || !(pLoopPlot->isVisibleEnemyUnit(m_unit)))
					{
						/*************************************************************************************************/
						/**	Xienwolf Tweak							12/13/08											**/
						/**																								**/
						/**					Reduction in massive Random Spam in Logger files by using Map				**/
						/*************************************************************************************************/
						/**								---- Start Original Code ----									**
												iValue += GC.getGame().getSorenRandNum(250 * abs(xDistance(m_unit->getX(), pLoopPlot->getX())) + abs(yDistance(m_unit->getY(), pLoopPlot->getY())), "AI explore");
						/**								----  End Original Code  ----									**/
						iValue += GC.getGame().getMapRandNum(250 * abs(xDistance(m_unit->getX(), pLoopPlot->getX())) + abs(yDistance(m_unit->getY(), pLoopPlot->getY())), "AI explore");
						/*************************************************************************************************/
						/**	Tweak									END													**/
						/*************************************************************************************************/

						if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_EXPLORE, m_unit->getGroup(), 3) == 0)
						{
							//	Add some hysteresis to prevent switching between targets too much
							if (pPreviouslySelectedPlot == pLoopPlot)
							{
								iValue = (110 * iValue) / 100;
							}

							if (pLoopPlot->isAdjacentToLand())
							{
								iValue += 10000;
							}

							if (pLoopPlot->isOwned())
							{
								iValue += 5000;
							}

							//ls612: Make exploring AI aware of terrain damage
							bool	bHasTerrainDamage = (pLoopPlot->getTotalTurnDamage(m_unit->getGroup()) > 0 || pLoopPlot->getFeatureTurnDamage() > 0);

							if (bHasTerrainDamage)
							{
								iValue /= 5;
							}

							iValue /= 3 + (std::max(1, itr.stepDistance()) / m_unit->maxMoves());

							if (!m_unit->atPlot(pLoopPlot) && iValue > 0)
							{
								plotValue m_unitPlotValue;

								m_unitPlotValue.plot = pLoopPlot;
								m_unitPlotValue.value = iValue;

								plotValues.push_back(m_unitPlotValue);
							}
						}
					}
				}
			}
		}
	}

	algo::sort(plotValues, plotValueSortPredicate);

	for (int iI = plotValues.size() - 1; iI >= 0; iI--)
	{
		PROFILE("AI_explore 1.1");

		pLoopPlot = plotValues[iI].plot;
		iValue = plotValues[iI].value;

		if (m_unit->generatePath(pLoopPlot, MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns))
		{
			PROFILE("AI_explore 1.1.1");
			//	Does m_unit proposed move leave us exposed at the end of m_unit
			//	turn?
			if (exposedToDanger(m_unit->getPathEndTurnPlot(), 60))
			{
				//	Reject m_unit option if we have less than 60% combat odds
				continue;
			}

			pBestPlot = pLoopPlot->isRevealedGoody(m_unit->getTeam()) ? m_unit->getPathEndTurnPlot() : pLoopPlot;
			pBestExplorePlot = pLoopPlot;
			break;
		}
	}

	if ((pBestPlot != NULL) && (pBestExplorePlot != NULL))
	{
		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_NO_ENEMY_TERRITORY | MOVE_AVOID_ENEMY_UNITS | MOVE_HEAL_AS_NEEDED25, false, false, MISSIONAI_EXPLORE, pBestExplorePlot);
	}

	return false;
}

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::exploreRange(int iRange)
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	CvPlot* pBestExplorePlot;
	int iPathTurns;
	int iValue;
	int iBestValue;
	int iDX, iDY;
	bool candidatesRejectedForMoveSafety = false;

	//	If we had previously selected a target bias towards move in that direction
	CvPlot* pPreviouslySelectedPlot = NULL;
	if (m_unit->getGroup()->AI_getMissionAIType() == MISSIONAI_EXPLORE)
	{
		pPreviouslySelectedPlot = m_unit->getGroup()->AI_getMissionAIPlot();

		if (pPreviouslySelectedPlot != NULL && m_unit->atPlot(pPreviouslySelectedPlot))
		{
			pPreviouslySelectedPlot = NULL;
		}
	}

	if (explorerJoinOffensiveStacks())
	{
		return true;
	}

	std::vector<plotValue>	plotValues;

	int iImpassableCount = GET_PLAYER(m_unit->getOwner()).AI_unitImpassableCount(m_unit->getUnitType());

	const int iSearchRange = searchRange(iRange);
	CvReachablePlotSet plotSet(m_unit->getGroup(), MOVE_NO_ENEMY_TERRITORY, iSearchRange);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		pLoopPlot = itr.plot();

		//	Have we already considered m_unit m_unit->plot in a previous invocation?
		int iOpaqueInfo = itr.getOpaqueInfo(ACTIVITY_ID_EXPLORE);
		if (plotOpaqueInfoMatches(iOpaqueInfo, ACTIVITY_ID_EXPLORE, 1))
		{
			continue;
		}

		itr.setOpaqueInfo(ACTIVITY_ID_EXPLORE, 1);

		iDX = abs(m_unit->getX() - pLoopPlot->getX());
		iDY = abs(m_unit->getY() - pLoopPlot->getY());

		PROFILE("AI_exploreRange 1");

		if (m_unit->atPlot(pLoopPlot))
			continue;

		//	Only consider plots that are revealed or adjacent to something we already revealed
		if (/*!plotValid(pLoopPlot) ||*/ (!pLoopPlot->isRevealed(m_unit->getTeam(), false) && !pLoopPlot->isAdjacentRevealed(m_unit->getTeam())))
			continue;

		iValue = 0;

		//	Tend to keep heading to the same spot or else dithering, and swapping of
		//	tragets between multiple explorers tends to occur
		if (pLoopPlot == pPreviouslySelectedPlot)
		{
			iValue += 10000;
		}

		if (pLoopPlot->isRevealedGoody(m_unit->getTeam()))
		{
			iValue += 100000;
		}

		if (!(pLoopPlot->isRevealed(m_unit->getTeam(), false)))
		{
			iValue += 10000;
		}

		{
			PROFILE("AI_exploreRange 2");

			foreach_(const CvPlot * pAdjacentPlot, pLoopPlot->adjacent()
			| filtered(!CvPlot::fn::isRevealed(m_unit->getTeam(), false)))
			{
				iValue += 1000;
			}

			//	plots we can't move into are not worth considering
			if (iValue > 0 && m_unit->canEnterPlot(pLoopPlot))
			{
				if (!pLoopPlot->isVisible(m_unit->getTeam(), false) || !(pLoopPlot->isVisibleEnemyUnit(m_unit)))
				{
					PROFILE("AI_exploreRange 3");

					if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_EXPLORE, m_unit->getGroup(), 3) == 0)
					{
						PROFILE("AI_exploreRange 4");

						if (pLoopPlot->isAdjacentToLand())
						{
							iValue += 10000;
						}

						if (pLoopPlot->isOwned())
						{
							iValue += 5000;
						}

						if (!m_unit->isHuman())
						{
							int iDirectionModifier = 100;

							if (getUnitAIType() == UNITAI_EXPLORE_SEA && iImpassableCount == 0)
							{
								iDirectionModifier += (50 * (abs(iDX) + abs(iDY))) / iSearchRange;
								if (GC.getGame().circumnavigationAvailable())
								{
									if (GC.getMap().isWrapX())
									{
										if ((iDX * ((getBirthmark() % 2 == 0) ? 1 : -1)) > 0)
										{
											iDirectionModifier *= 150 + ((iDX * 100) / iSearchRange);
										}
										else
										{
											iDirectionModifier /= 2;
										}
									}
									if (GC.getMap().isWrapY())
									{
										if ((iDY * (((getBirthmark() >> 1) % 2 == 0) ? 1 : -1)) > 0)
										{
											iDirectionModifier *= 150 + ((iDY * 100) / iSearchRange);
										}
										else
										{
											iDirectionModifier /= 2;
										}
									}
								}
								iValue *= iDirectionModifier;
								iValue /= 100;
							}
						}

						iValue += GC.getGame().getMapRandNum(10000, "AI Explore");

						//ls612: Make exploring AI aware of terrain damage
						const bool bHasTerrainDamage = (pLoopPlot->getTotalTurnDamage(m_unit->getGroup()) > 0 || pLoopPlot->getFeatureTurnDamage() > 0);

						if (bHasTerrainDamage)
						{
							iValue /= 5;
						}

						iValue /= (1 + stepDistance(m_unit->getX(), m_unit->getY(), pLoopPlot->getX(), pLoopPlot->getY()));

						plotValue m_unitPlotValue;

						m_unitPlotValue.plot = pLoopPlot;
						m_unitPlotValue.value = iValue;

						plotValues.push_back(m_unitPlotValue);
					}
				}
			}
		}
	}

	iBestValue = 0;
	pBestPlot = NULL;
	pBestExplorePlot = NULL;

	//	Sort the plots found on their values
	while (plotValues.size() > 0)
	{
		algo::sort(plotValues, plotValueSortPredicate);

		iValue = plotValues[plotValues.size() - 1].value;
		pLoopPlot = plotValues[plotValues.size() - 1].plot;

		if (iValue == 0)
		{
			break;
		}

		//	Avoid the cost of path generation if m_unit cannot possibly be the best result
		CvReachablePlotSet::const_iterator itr = plotSet.find(pLoopPlot);
		if (itr != plotSet.end() && m_unit->generatePath(pLoopPlot, MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns, iRange))
		{
			//	Don't limit by iPathTurns since the range param passedin is really advisory
			//	only and intended to limit the size of the search space

			//	Does m_unit proposed move leave us exposed at the end of m_unit
			//	turn?  For general exploring moves we accept danger with 60%
			//	combat odds in our favour
			if (exposedToDanger(m_unit->getPathEndTurnPlot(), 60))
			{
				//	For now just reject m_unit.  Might want to do a combat odds check ideally
				candidatesRejectedForMoveSafety = true;
			}
			else
			{
				iBestValue = iValue;
				if (m_unit->getDomainType() == DOMAIN_LAND)
				{
					pBestPlot = m_unit->getPathEndTurnPlot();
				}
				else
				{
					pBestPlot = pLoopPlot;
				}
				pBestExplorePlot = pLoopPlot;
				break;
			}
		}
		else
		{
			//	It's actually out of range for m_unit search so we have not really meaningfully checked it
			itr.setOpaqueInfo(ACTIVITY_ID_EXPLORE, 0);
			//	Redistribute the score of the top m_unit->plot (which we could not reach) to other candidates
			//	According to their distance from it.  This creates a bias to explore towards the high
			//	value m_unit->plot that we cannot yet find a path to
			for (int iI = 0; iI < (int)plotValues.size() - 1; iI++)
			{
				//	Can knock out candidates that are adjacent to the one we just tried to path to since we won't be able to reach them
				//	either (unless both are unrevealed potentially)
				if (stepDistance(pLoopPlot->getX(), pLoopPlot->getY(), plotValues[iI].plot->getX(), plotValues[iI].plot->getY()) == 1 &&
					 (pLoopPlot->isRevealed(m_unit->getTeam(), false) || plotValues[iI].plot->isRevealed(m_unit->getTeam(), false)))
				{
					plotValues[iI].value = 0;
				}
				else if (plotValues[iI].value > 0)
				{
					plotValues[iI].value += iValue / (5 * std::max(1, plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), plotValues[iI].plot->getX(), plotValues[iI].plot->getY())));
				}
			}
		}

		plotValues.resize(plotValues.size() - 1);
	}

	if ((pBestPlot != NULL) && (pBestExplorePlot != NULL))
	{
		OutputDebugString(CvString::format("%S (%d) chooses to explore move to (%d,%d)\n", m_unit->getDescription().c_str(), m_unit->getID(), pBestPlot->getX(), pBestPlot->getY()).c_str());
		PROFILE("AI_exploreRange 5");

		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_NO_ENEMY_TERRITORY | MOVE_HEAL_AS_NEEDED25, false, false, MISSIONAI_EXPLORE, pBestExplorePlot);
	}
	else if (candidatesRejectedForMoveSafety)
	{
		OutputDebugString(CvString::format("%S (%d) finds danger blocking explore move an seeks safety\n", m_unit->getDescription().c_str(), m_unit->getID()).c_str());
		//	Just stay safe for a while
		return safety();
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::refreshExploreRange(int iRange, bool bIncludeVisibilityRefresh)
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	CvPlot* pBestExplorePlot;
	int iPathTurns;
	int iValue;
	int iBestValue;
	bool candidatesRejectedForMoveSafety = false;

	const int iSearchRange = searchRange(iRange);

	//	If we had previously selected a target bias towards move in that direction
	CvPlot* pPreviouslySelectedPlot = NULL;
	if (m_unit->getGroup()->AI_getMissionAIType() == MISSIONAI_EXPLORE)
	{
		pPreviouslySelectedPlot = m_unit->getGroup()->AI_getMissionAIPlot();

		if (pPreviouslySelectedPlot != NULL && m_unit->atPlot(pPreviouslySelectedPlot))
		{
			pPreviouslySelectedPlot = NULL;
		}
	}

	if (explorerJoinOffensiveStacks())
	{
		return true;
	}

	iBestValue = 0;
	pBestPlot = NULL;
	pBestExplorePlot = NULL;

	CvReachablePlotSet plotSet(m_unit->getGroup(), MOVE_NO_ENEMY_TERRITORY, iSearchRange);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		pLoopPlot = itr.plot();
		PROFILE("AI_exploreRange 1");

		if (pLoopPlot != NULL && !m_unit->atPlot(pLoopPlot))
		{
			//if (plotValid(pLoopPlot))
			{
				int iAdjacentEnemies = 0;
				bool bValidAdjacentEnemyValue = false;

				iValue = 0;

				if (pLoopPlot->isRevealedGoody(m_unit->getTeam()))
				{
					iValue += 100000;
				}

				if (!(pLoopPlot->isRevealed(m_unit->getTeam(), false)))
				{
					iValue += 10000;
				}

				if (bIncludeVisibilityRefresh && !pLoopPlot->isVisible(m_unit->getTeam(), false))
				{
					iValue += (GC.getGame().getGameTurn() - pLoopPlot->getLastVisibleTurn(m_unit->getTeam()));
				}

				foreach_(const CvPlot * pAdjacentPlot, pLoopPlot->adjacent())
				{
					PROFILE("AI_exploreRange 2");

					if (!(pAdjacentPlot->isRevealed(m_unit->getTeam(), false)))
					{
						iValue += 1000;
					}

					//	If there is an enemy unit there add an extra value to the adjacent m_unit->plot
					//	we are currently considering so as to consider moving there to tempt the enemy
					//	into an attack more favourable to us than us attacking them (which will already
					//	have been considered).  We will only actually consider m_unit value if we can get
					//	there m_unit turn however, since units move!
					if (plotValid(pAdjacentPlot) &&
							stepDistance(pLoopPlot->getX(), pLoopPlot->getY(), m_unit->getX(), m_unit->getY()) <= searchRange())
					{
						foreach_(const CvUnit * pLoopUnit, pAdjacentPlot->units())
						{
							//	Animals won't attack into owned territory so don't count them
							if (m_unit->isEnemy(pLoopUnit->getTeam()) &&
								(pLoopPlot->getOwner() == NO_PLAYER || !pLoopUnit->isAnimal() || (pLoopUnit->isAnimal() && pLoopUnit->canAnimalIgnoresBorders())))
							{
								iAdjacentEnemies++;

								//	Don't count extra value for might-be-attacked in owned territory.  This stops units
								//	getting stuck oscillating back and forth between two defensive tiles next to foreign
								//	(esp barbarian) cities when they should be hunting/exploring
								if (pAdjacentPlot->getOwner() == NO_PLAYER || pAdjacentPlot->getOwner() == m_unit->getOwner())
								{
									bValidAdjacentEnemyValue = true;
								}
							}
						}
					}
				}

				//bValidAdjacentEnemyValue &= (iAdjacentEnemies == 1);
				if (iValue > 0 || bValidAdjacentEnemyValue)
				{
					if (!pLoopPlot->isVisible(m_unit->getTeam(), false) || !(pLoopPlot->isVisibleEnemyUnit(m_unit)))
					{
						PROFILE("AI_exploreRange 3");

						if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_EXPLORE, m_unit->getGroup(), 3) == 0)
						{
							PROFILE("AI_exploreRange 4");

							//	Avoid the cost of path generation if m_unit cannot possibly be the best result
							if ((iValue > iBestValue - 50 - (bValidAdjacentEnemyValue ? 50 : 0)) && m_unit->generatePath(pLoopPlot, MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns, iRange))
							{
								if (iPathTurns <= iRange)
								{
									iValue += GC.getGame().getMapRandNum(50, "AI Explore");

									if (bValidAdjacentEnemyValue && iPathTurns <= 1)
									{
										iValue += 50 + pLoopPlot->defenseModifier(m_unit->getTeam(), false);	//	Chance to prompt a favorable attack
									}

									//	Try to pick something that moves towards our eventual goal (if any)
									if (pPreviouslySelectedPlot != NULL)
									{
										iValue += 50 * (stepDistance(pPreviouslySelectedPlot->getX(), pPreviouslySelectedPlot->getY(), m_unit->getX(), m_unit->getY()) -
														stepDistance(pPreviouslySelectedPlot->getX(), pPreviouslySelectedPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY()));
									}

									//ls612: Make exploring AI aware of terrain damage
									bool	bHasTerrainDamage = (pLoopPlot->getTotalTurnDamage(m_unit->getGroup()) > 0 || pLoopPlot->getFeatureTurnDamage() > 0);

									if (bHasTerrainDamage)
									{
										iValue /= 5;
									}

									if (iValue > iBestValue)
									{
										//	Does m_unit proposed move leave us exposed at the end of m_unit
										//	turn?  For general exploring moves we accept danger with 60%
										//	combat odds in our favour
										if (exposedToDanger(m_unit->getPathEndTurnPlot(), 60))
										{
											//	For now just reject m_unit.  Might want to do a combat odds check ideally
											candidatesRejectedForMoveSafety = true;
											continue;
										}
										iBestValue = iValue;
										if (m_unit->getDomainType() == DOMAIN_LAND)
										{
											pBestPlot = m_unit->getPathEndTurnPlot();
										}
										else
										{
											pBestPlot = pLoopPlot;
										}
										pBestExplorePlot = pLoopPlot;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestExplorePlot != NULL))
	{
		PROFILE("AI_exploreRange 5");

		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_NO_ENEMY_TERRITORY | MOVE_HEAL_AS_NEEDED25, false, false, MISSIONAI_EXPLORE, pPreviouslySelectedPlot != NULL ? pPreviouslySelectedPlot : pBestExplorePlot);
	}
	else if (candidatesRejectedForMoveSafety)
	{
		//	Just stay safe for a while
		return safety();
	}

	return false;
}

//	Determine if there is a threatening unit at the spcified m_unit->plot.
//	Return a pointer to the unit if so (NULL with a detected threat means multiple units)
bool CvUnitDefaultAI::getThreateningUnit(const CvPlot* pPlot, CvUnit*& pThreateningUnit, const CvPlot* pAttackPlot, int& iIndex, bool bReturnWorstOfMultiple) const
{
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
		//	Don't count animals if the target m_unit->plot is owned since they cannot attack there
		if (m_unit->isEnemy(pLoopUnit->getTeam(), NULL, pLoopUnit) && pLoopUnit->canAttack() && (!pLoopUnit->isAnimal() || (pLoopUnit->isAnimal() && pLoopUnit->canAnimalIgnoresBorders()) || pAttackPlot->getOwner() == NO_PLAYER))
		{
			if (bReturnWorstOfMultiple)
			{
				if (pPlot == pAttackPlot)
				{
					iOdds = 100 - m_unit->AI()->attackOddsAtPlot(pPlot, pLoopUnit);
				}
				else
				{
					iOdds = pLoopUnit->AI()->attackOddsAtPlot(pPlot, m_unit);
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

bool CvUnitDefaultAI::exposedToDanger(const CvPlot* pPlot, int acceptableOdds, bool bConsiderOnlyWorstThreat) const
{
	if (GET_PLAYER(m_unit->getOwner()).AI_getPlotDanger(pPlot, 1, false))
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

		//	If we're not currently at the designated m_unit->plot check it first on the assumption we'll want to
		//	attack into it if we are capable of so doing
		if (pPlot != m_unit->plot() && pPlot->isVisibleEnemyUnit(m_unit))
		{
			iIndex = 0;

			//	Evaluate based on our best attacker
			int dummy;
			pOurAttacker = m_unit->getGroup()->AI_getBestGroupAttacker(pPlot, true, dummy);

			if (pOurAttacker == NULL)
			{
				//	We cannot attack here
				return true;
			}
			iOurValue += pOurAttacker->AI()->genericUnitValueTimes100(UNITVALUE_FLAGS_DEFENSIVE | UNITVALUE_FLAGS_OFFENSIVE);

			while (getThreateningUnit(pPlot, threateningUnit, pPlot, iIndex, bConsiderOnlyWorstThreat))
			{
				int iOdds = pOurAttacker->AI()->attackOddsAtPlot(pPlot, threateningUnit);

				//	attackOddsAtPlot returns a value capped artificially at 99, but for the ensuing calculation
				//	that causes nasty rounding errors so we just treat 99 as if it were certainty
				if (iOdds == 99)
				{
					iOdds = 100;
				}
				//	Survival probability is probability of surviving up to m_unit (iOurOdds
				//	up to m_unit point) * odds of surving m_unit encounter, which we normalize
				//	from the base (100-iOdds) by a factor of iOurOdds to reflect likely damage taken
				iOurOdds = (iOurOdds * iOdds * iOurOdds) / 10000;

				if (iOurOdds > 50)
				{
					int iEnemyValue = threateningUnit->AI()->genericUnitValueTimes100(UNITVALUE_FLAGS_DEFENSIVE | UNITVALUE_FLAGS_OFFENSIVE);

					//	Only count one kill - m_unit routine is mostly geared towards single unit stacks so we'll
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

		CvUnit* pOurDefender = ((CvSelectionGroupAI*)m_unit->getGroup())->AI_findBestDefender(pPlot, true);

		if (pOurDefender != NULL)
		{
			if (pOurAttacker != pOurDefender)
			{
				iOurValue += pOurDefender->AI()->genericUnitValueTimes100(UNITVALUE_FLAGS_DEFENSIVE | UNITVALUE_FLAGS_OFFENSIVE);
			}
		}

		for (int iI = 0; iI < NUM_DIRECTION_TYPES && iOurOdds >= acceptableOdds; iI++)
		{
			CvPlot* pAdjacentPlot;

			pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL)
			{
				if (plotValid(pAdjacentPlot))
				{
					CvUnit* pUnit;

					iIndex = 0;

					while (getThreateningUnit(pAdjacentPlot, pUnit, pPlot, iIndex, bConsiderOnlyWorstThreat))
					{
						FAssert(pUnit != NULL);

						if (!m_unit->isInvisible(pUnit->getTeam(), false))
						{
							int iOdds = pUnit->AI()->attackOddsAtPlot(pPlot, pOurDefender);

							//	attackOddsAtPlot returns a value capped artificially below at 1, but for the ensuing calculation
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
								//	Survival probability is probability of surviving up to m_unit (iOurOdds
								//	up to m_unit point) * odds of surving m_unit encounter, which we normalize
								//	from the base (100-iOdds) by a factor of iOurOdds to reflect likely damage taken
								iOurOdds = (iOurOdds * (100 - iOdds) * iOurOdds) / 10000;

								//	Since all enemies can attack us, count them all
								if (iOurOdds > 50)
								{
									iEnemyTotalValueExpectedLoss += pUnit->AI()->genericUnitValueTimes100(UNITVALUE_FLAGS_DEFENSIVE | UNITVALUE_FLAGS_OFFENSIVE);
								}
							}
						}
					}
				}
			}
		}

		if (iOurOdds < acceptableOdds && iEnemyTotalValueExpectedLoss >= iOurValue)
		{
			//	If we would claim m_unit is too dangerous adjust for relative losses so that
			//	things that leave us likely to die only if we take a lot of enemies with us
			//	are considered ok - note that we only apply m_unit adjustment to single unit
			//	stacks, since use of m_unit routine with  multi-unit stacks is limited to situations
			//	where escorts (of defenseless, but relatively important) units is involved
			if (m_unit->getGroup()->getNumUnits() == 1)
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
CvCity* CvUnitDefaultAI::pickTargetCity(int iFlags, int iMaxPathTurns, bool bHuntBarbs)
{
	PROFILE_FUNC();

	static std::map<CvPlot*, bool>* cachedTargets = NULL;

	/*
	CvCity* pTargetCity = m_unit->area()->getTargetCity(m_unit->getOwner());

	// Don't always go after m_unit->area target ... don't know how far away it is
	if (pTargetCity != NULL
	&& potentialEnemy(pTargetCity->getTeam(), pTargetCity->plot())
	&& !m_unit->atPlot(pTargetCity->plot())
	&& m_unit->generatePath(pTargetCity->plot(), iFlags, true))
	{
		pBestCity = pTargetCity;
	}
	*/
	if (cachedTargets == NULL)
	{
		cachedTargets = new std::map<CvPlot*, bool>();
	}

	if (eCachedTargetCityPlayer != m_unit->getOwner())
	{
		eCachedTargetCityPlayer = m_unit->getOwner();
		cachedTargets->clear();
	}

	CvCity* pBestCity = NULL;
	for (std::map<CvPlot*, bool>::const_iterator itr = cachedTargets->begin(); itr != cachedTargets->end(); ++itr)
	{
		CvCity* possibleTargetCity = itr->first->getPlotCity();

		if (possibleTargetCity != NULL
		&& itr->first->area() == m_unit->area()
		&& stepDistance(m_unit->getX(), m_unit->getY(), itr->first->getX(), itr->first->getY()) < searchRange(iMaxPathTurns)
		&& m_unit->generatePath(itr->first, iFlags, true, NULL, MAX_CLOSE_TARGET_DISTANCE))
		{
			pBestCity = possibleTargetCity;
			break;
		}
	}

	CvReachablePlotSet plotSet(m_unit->getGroup(), iFlags);

	if (iMaxPathTurns != MAX_INT)
	{
		plotSet.Populate(searchRange(iMaxPathTurns));
	}

	std::multimap<int, targetInfo> possibleTargets;

	if (pBestCity == NULL)
	{
		for (int iI = 0; iI < (bHuntBarbs ? MAX_PLAYERS : MAX_PC_PLAYERS); iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive() && ::isPotentialEnemy(m_unit->getTeam(), GET_PLAYER((PlayerTypes)iI).getTeam()))
			{
				foreach_(CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
				{
					if
						(
							(
								iMaxPathTurns == MAX_INT && stepDistance(m_unit->getX(), m_unit->getY(), pLoopCity->getX(), pLoopCity->getY()) < searchRange(iMaxPathTurns)
								||
								iMaxPathTurns != MAX_INT && plotSet.find(pLoopCity->plot()) != plotSet.end()
								)
						&&
							pLoopCity->plot()->isRevealed(m_unit->getTeam(), false)
						&&
							(m_unit->getDomainType() != DOMAIN_LAND || pLoopCity->area() == m_unit->area()
								&&
								potentialEnemy(GET_PLAYER((PlayerTypes)iI).getTeam(), pLoopCity->plot())
								&&
								!m_unit->atPlot(pLoopCity->plot()))
						)
					{
						int iValue =
							(
								getUnitAIType() == UNITAI_ATTACK_CITY
								?
								GET_PLAYER(m_unit->getOwner()).AI_targetCityValue(pLoopCity, false, false)
								:
								GET_PLAYER(m_unit->getOwner()).AI_targetCityValue(pLoopCity, true, true)
							);

						/* TODO: cleanup m_unit function - There is no target city, it was disabled
						if (pLoopCity == pTargetCity)
						{
							iValue *= 2;
						} */

						if (m_unit->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_DEFENSIVE)
						{
							iValue *= 50 + pLoopCity->calculateCulturePercent(m_unit->getOwner());
							iValue /= 50;
						}
						iValue *= 1000;

						// If city is minor civ, less interesting
						if (GET_PLAYER(pLoopCity->getOwner()).isMinorCiv() || GET_PLAYER(pLoopCity->getOwner()).isNPC())
						{
							iValue /= 2;
						}

						if (iValue > 0)
						{
							targetInfo info;

							info.iValue = iValue;
							info.pCity = pLoopCity;

							const int iDistanceEstimate = (stepDistance(m_unit->getX(), m_unit->getY(), pLoopCity->getX(), pLoopCity->getY()) + 2) / 2;
							const int iOrderingValue = -iValue / (iDistanceEstimate * iDistanceEstimate);

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

			if (iBestValue > 0)
			{
				int iLongestWinningPath = itr->second.iValue / iBestValue - 4;
				if (iLongestWinningPath > 1)
					iLongestWinningPath = intSqrt(iLongestWinningPath);

				const int iPathTurnsExtra = std::min(12, m_unit->getGroup()->getBombardTurns(pLoopCity) / 4);

				iMaxPath = std::min(iLongestWinningPath - iPathTurnsExtra, iMaxPathTurns);
			}
			else iMaxPath = iMaxPathTurns;

			if (iMaxPath > 0)
			{
				PROFILE("AI_pickTargetCity.PrePathing");
				int iPathTurns = 0;
				if (m_unit->generatePath(pLoopCity->plot(), iFlags, true, &iPathTurns, iMaxPath)
				|| stepDistance(pLoopCity->getX(), pLoopCity->getY(), m_unit->getX(), m_unit->getY()) == 1 && m_unit->getGroup()->canBombard(m_unit->plot(), true))
				{
					PROFILE("AI_pickTargetCity.PostPathing");

					// If city is visible and our force already in position is dominantly powerful or we have a huge force
					// already on the way, pick a different target
					if (iPathTurns > 2 && pLoopCity->isVisible(m_unit->getTeam(), false))
					{
						/*
						int iOurOffense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(pLoopCity->plot(),2,false,false,true);
						int iEnemyDefense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(pLoopCity->plot(),1,true,false);

						if( 100*iOurOffense >= GC.getBBAI_SKIP_BOMBARD_BASE_STACK_RATIO()*iEnemyDefense )
						{
							continue;
						}
						*/

						if (GET_PLAYER(m_unit->getOwner()).AI_cityTargetUnitsByPath(pLoopCity, m_unit->getGroup(), iPathTurns) > std::max(6, 3 * pLoopCity->plot()->getNumVisiblePotentialEnemyDefenders(m_unit)))
						{
							continue;
						}
					}

					// For barbarians, if the city is distant, build a larger stack first than we
					// would if it were close by
					if (m_unit->isNPC() && m_unit->getGroup()->getNumUnits() < 3 + iPathTurns / 5)
					{
						continue;
					}

					const int iValue = itr->second.iValue / (4 + iPathTurns * iPathTurns);

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
		if (!bVerifiedPathing && !m_unit->generatePath(pBestCity->plot(), iFlags, true))
		{
			return NULL;
		}
		logBBAI("Unit %d at (%d,%d) picks target city at (%d,%d)",
				m_unit->getID(),
				m_unit->getX(),
				m_unit->getY(),
				pBestCity->getX(),
				pBestCity->getY());

		(*cachedTargets)[pBestCity->plot()] = true;
	}
	return pBestCity;
}

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::goToTargetCity(int iFlags, int iMaxPathTurns, const CvCity* pTargetCity)
{
	PROFILE_FUNC();


	if (pTargetCity == NULL)
	{
		pTargetCity = pickTargetCity(iFlags, iMaxPathTurns);
	}

	if (pTargetCity != NULL)
	{
		PROFILE("CvUnitDefaultAI::targetCity m_unit->plot attack");
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
					if (plotValid(pAdjacentPlot) && !pAdjacentPlot->isVisibleEnemyUnit(m_unit)
					&& m_unit->generatePath(pAdjacentPlot, iFlags, true, &iPathTurns))
					{
						int iValue = std::max(0, pAdjacentPlot->defenseModifier(m_unit->getTeam(), false) + 100);

						if (!pAdjacentPlot->isRiverCrossing(directionXY(pAdjacentPlot, pTargetCity->plot())))
						{
							iValue -= 12 * GC.getRIVER_ATTACK_MODIFIER();
						}

						if (!m_unit->isEnemy(pAdjacentPlot->getTeam(), pAdjacentPlot))
						{
							iValue += 100;
						}

						if (m_unit->atPlot(pAdjacentPlot))
						{
							iValue += 50;
						}

						iValue = std::max(1, iValue);

						iValue *= 1000;

						iValue /= (iPathTurns + 1);

						if (iValue > iBestValue)
						{
							const CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();
							bool bAcceptable;

							if (pAdjacentPlot == endTurnPlot)
							{
								bAcceptable = true;
							}
							else if (m_unit->getGroup()->getNumUnits() < 3)
							{
								bAcceptable = !exposedToDanger(endTurnPlot, 60);
							}
							else
							{
								const int iTotalEnemyStrength = endTurnPlot->getVisibleEnemyStrength(m_unit->getOwner(), 1);
								const int iOurStrength = m_unit->plot()->plotStrength((UnitValueFlags)(UNITVALUE_FLAGS_OFFENSIVE | UNITVALUE_FLAGS_DEFENSIVE), NULL, -1, -1, m_unit->getOwner());

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
				const CvPath& kPath = m_unit->getGroup()->getPath();

				for (CvPath::const_iterator itr = kPath.begin(); itr != kPath.end() && itr.plot() != pTargetCity->plot(); ++itr)
				{
					const CvPlot* pPathPlot = itr.plot();

					if (pPathPlot->isVisibleEnemyUnit(m_unit->getOwner()))
					{
						bool bWin;
						int iExpectedGainOdds = m_unit->getGroup()->AI_attackOdds(pPathPlot, true, false, &bWin);

						if (iExpectedGainOdds < 50)
						{
							if (!bWin)
							{
								//	Blockade is too strong to beat
								pBestPlot = NULL;
							}
							else
							{
								const int iStackRatio = m_unit->getGroup()->AI_compareStacks(pPathPlot, StackCompare::PotentialEnemy);

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
			if (!m_unit->atPlot(pBestPlot))
			{
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), iFlags);
			}
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::goToTargetBarbCity(int iMaxPathTurns)
{
	PROFILE_FUNC();

	int iPathTurns;
	int iBestValue = 0;
	const CvCity* pBestCity = NULL;
	const PlayerTypes eOwner = m_unit->getOwner();

	for (int iI = MAX_PC_PLAYERS; iI < MAX_PLAYERS; iI++)
	{
		if (iI == eOwner)
		{
			continue;
		}
		foreach_(const CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
		{
			if (plotValid(pLoopCity->plot()))
			{
				// BBAI efficiency: check m_unit->area for land units before generating path
				if (m_unit->getDomainType() == DOMAIN_LAND && pLoopCity->area() != m_unit->area() && !m_unit->getGroup()->canMoveAllTerrain())
				{
					continue;
				}
				if (pLoopCity->isRevealed(m_unit->getTeam(), false)
				&& !m_unit->atPlot(pLoopCity->plot())
				&& m_unit->generatePath(pLoopCity->plot(), 0, true, &iPathTurns)
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
			if (plotValid(pAdjacentPlot))
			{
				if (!(pAdjacentPlot->isVisibleEnemyUnit(m_unit)))
				{
					if (m_unit->generatePath(pAdjacentPlot, 0, true, &iPathTurns, iMaxPathTurns))
					{
						int iValue = std::max(0, (pAdjacentPlot->defenseModifier(m_unit->getTeam(), false) + 100));

						if (!(pAdjacentPlot->isRiverCrossing(directionXY(pAdjacentPlot, pBestCity->plot()))))
						{
							iValue += (10 * -(GC.getRIVER_ATTACK_MODIFIER()));
						}

						iValue = std::max(1, iValue);

						iValue *= 1000;

						iValue /= (iPathTurns + 1);

						if (iValue > iBestValue)
						{
							CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();

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
			if (m_unit->atPlot(pBestPlot))
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP);
				return true;
			}
			else
			{
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
			}
		}
	}
	return false;
}

bool CvUnitDefaultAI::pillageAroundCity(const CvCity* pTargetCity, int iBonusValueThreshold, int iMaxPathTurns)
{
	PROFILE_FUNC();

	int iBestValue = 0;
	CvPlot* endTurnPlot = NULL;
	CvPlot* pBestPlot = NULL;
	const CvPlot* pBestPillagePlot = NULL;

	foreach_(const CvPlot * pLoopPlot, pTargetCity->plots(NUM_CITY_PLOTS))
	{
		if (plotValid(pLoopPlot) && !(pLoopPlot->isNPC()))
		{
			if (m_unit->potentialWarAction(pLoopPlot) && (pLoopPlot->getTeam() == pTargetCity->getTeam()))
			{
				if (m_unit->getGroup()->canPillage(pLoopPlot))
				{
					if (!(pLoopPlot->isVisibleEnemyUnit(m_unit)))
					{
						if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_PILLAGE, m_unit->getGroup()) == 0)
						{
							int iPathTurns;
							if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns, iMaxPathTurns))
							{
								int iValue = pillageValue(pLoopPlot, iBonusValueThreshold);

								iValue *= 1000 + 30 * (pLoopPlot->defenseModifier(m_unit->getTeam(), false));

								iValue /= (iPathTurns + 1);

								// if not at war with m_unit m_unit->plot owner, then devalue m_unit->plot if we already inside m_unit owner's borders
								// (because declaring war will pop us some unknown distance away)
								if (!m_unit->isEnemy(pLoopPlot->getTeam()) && m_unit->plot()->getTeam() == pLoopPlot->getTeam())
								{
									iValue /= 10;
								}

								if (iValue > iBestValue)
								{
									endTurnPlot = m_unit->getPathEndTurnPlot();

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
		if (m_unit->atPlot(pBestPillagePlot) && !m_unit->isEnemy(pBestPillagePlot->getTeam()))
		{
			//m_unit->getGroup()->groupDeclareWar(pBestPillagePlot, true);
			// rather than declare war, just find something else to do, since we may already be deep in enemy territory
			return false;
		}

		if (m_unit->atPlot(pBestPillagePlot))
		{
			if (m_unit->isEnemy(pBestPillagePlot->getTeam()))
			{
				m_unit->getGroup()->pushMission(MISSION_PILLAGE, -1, -1, 0, false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
				return true;
			}
		}
		else
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
		}
	}

	return false;
}

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::bombardCity()
{
	PROFILE_FUNC();

	//	Replaced REV implementation with K-mod simplified one
	//	[FUTURE] - may want to use contract brokerage to get bombers and ships to bombard first
	if (m_unit->getGroup()->canBombard(m_unit->plot()))
	{
		CvCity* pBombardCity = m_unit->bombardTarget(m_unit->plot());

		if (pBombardCity != NULL)
		{
			int iAttackOdds = m_unit->getGroup()->AI_attackOdds(pBombardCity->plot(), true);
			int iBase = GC.getBBAI_SKIP_BOMBARD_BASE_STACK_RATIO();
			int iMin = GC.getBBAI_SKIP_BOMBARD_MIN_STACK_RATIO();
			int iBombardTurns = m_unit->getGroup()->getBombardTurns(pBombardCity);
			int iThreshold = (iBase * (100 - iAttackOdds) + (1 + iBombardTurns / 2) * iMin * iAttackOdds) / (100 + (iBombardTurns / 2) * iAttackOdds);
			int iComparison = m_unit->getGroup()->AI_compareStacks(pBombardCity->plot(), StackCompare::CheckCanAttack | StackCompare::CheckCanMove | StackCompare::PotentialEnemy);

			if (iComparison > iThreshold && (pBombardCity->isDirectAttackable() || m_unit->canIgnoreNoEntryLevel()))
			{
				if (gUnitLogLevel > 2)
				{
					logBBAI("	  Stack skipping bombard of %S with compare %d, starting odds %d, bombard turns %d, threshold %d", pBombardCity->getName().GetCString(), iComparison, iAttackOdds, iBombardTurns, iThreshold);
				}
			}
			else
			{
				return m_unit->getGroup()->pushMissionInternal(MISSION_BOMBARD);
			}
		}
		//	Super forts - handle fort improvements
		else
		{
			CvPlot* pTargetPlot = m_unit->bombardImprovementTarget(m_unit->plot());

			if (pTargetPlot != NULL)
			{
				// do not bombard forts with no defenders
				int iDefenderStrength = pTargetPlot->AI_sumStrength(NO_PLAYER, m_unit->getOwner(), DOMAIN_LAND, StrengthFlags::DefensiveBonuses | StrengthFlags::TestAtWar);
				if (iDefenderStrength == 0)
				{
					return false;
				}

				// do not bombard cities if we have overwelming odds
				int iAttackOdds = m_unit->getGroup()->AI_attackOdds(pTargetPlot, /*bPotentialEnemy*/ true);
				if (iAttackOdds > 95)
				{
					return false;
				}

				// could also do a compare stacks call here if we wanted, the downside of that is that we may just have a lot more units
				// we may not want to suffer high casualties just to save a turn
				//m_unit->getGroup()->AI_compareStacks(pBombardCity->plot(), /*bPotentialEnemy*/ true, /*bCheckCanAttack*/ true, /*bCheckCanMove*/ true);
				//int iOurStrength = pBombardCity->plot()->AI()->sumStrength(m_unit->getOwner(), NO_PLAYER, DOMAIN_LAND, false, false, false)

				if (pTargetPlot->getDefenseDamage() < ((GC.getImprovementInfo(pTargetPlot->getImprovementType()).getDefenseModifier() * 3) / 4))
				{
					return m_unit->getGroup()->pushMissionInternal(MISSION_BOMBARD);
				}
			}
		}
		// Super Forts end
	}

	return false;

#if 0
	CvCity* pBombardCity;

	if (canBombard(m_unit->plot()))
	{
		pBombardCity = m_unit->bombardTarget(m_unit->plot());
		FAssertMsg(pBombardCity != NULL, "BombardCity is not assigned a valid value");

		// do not bombard cities with no defenders
		int iDefenderStrength = pBombardCity->plot()->AI()->sumStrength(NO_PLAYER, m_unit->getOwner(), DOMAIN_LAND, /*bDefensiveBonuses*/ true, /*bTestAtWar*/ true, false);
		if (iDefenderStrength == 0)
		{
			return false;
		}

		// do not bombard cities if we have overwelming odds
		int iAttackOdds = m_unit->getGroup()->AI_attackOdds(pBombardCity->plot(), /*bPotentialEnemy*/ true);
		if ((iAttackOdds > 95))
		{
			return false;
		}

		// If we have reasonable odds, check for attacking without waiting for bombards
		if ((iAttackOdds >= getBBAI_SKIP_BOMBARD_BEST_ATTACK_ODDS())
		{
			int iBase = GC.getBBAI_SKIP_BOMBARD_BASE_STACK_RATIO();
				int iComparison = m_unit->getGroup()->AI_compareStacks(pBombardCity->plot(), /*bPotentialEnemy*/ true, /*bCheckCanAttack*/ true, /*bCheckCanMove*/ true);

				// Big troop advantage plus pretty good starting odds, don't wait to allow reinforcements
				if (iComparison > (iBase - 4 * iAttackOdds))
				{
					if (gUnitLogLevel > 2) logBBAI("	  Stack skipping bombard of %S with compare %d and starting odds %d", pBombardCity->getName().GetCString(), iComparison, iAttackOdds);
						return false;
				}

			int iMin = GC.getBBAI_SKIP_BOMBARD_MIN_STACK_RATIO();
			bool bHasWaited = algo::any_of(m_unit->getGroup()->units(), CvUnit::fn::getFortifyTurns() > 0)

				// Bombard at least one turn to allow bombers/ships to get some shots in too
				if (bHasWaited && (pBombardCity->getDefenseDamage() > 0))
				{
					int iBombardTurns = m_unit->getGroup()->getBombardTurns(pBombardCity);
					if (iComparison > std::max(iMin, iBase - 3 * iAttackOdds - 3 * iBombardTurns))
					{
						if (gUnitLogLevel > 2) logBBAI("	  Stack skipping bombard of %S with compare %d, starting odds %d, and bombard turns %d", pBombardCity->getName().GetCString(), iComparison, iAttackOdds, iBombardTurns);
						return false;
					}
				}
		}

		//m_unit->getGroup()->pushMission(MISSION_PILLAGE);
		m_unit->getGroup()->pushMission(MISSION_BOMBARD);
		return true;
		/************************************************************************************************/
		/* REVOLUTIONDCM							05/24/08								Glider1	 */
		/*																							  */
		/*																							  */
		/************************************************************************************************/
				// RevolutionDCM - ranged bombard AI wraps standard bombard
		if (!RbombardCity(pBombardCity))
		{
			// vanilla behaviour
			m_unit->getGroup()->pushMission(MISSION_BOMBARD);
			return true;
		}
		// RevolutionDCM - end
/************************************************************************************************/
/* REVOLUTIONDCM							END									 Glider1	 */
/************************************************************************************************/
	}

	return false;
#endif
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					   END												  */
/************************************************************************************************/


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::cityAttack(int iRange, int iOddsThreshold, bool bFollow)
{
	PROFILE_FUNC();

	int iPathTurns;

	FAssert(m_unit->canMove());

	const int iSearchRange = bFollow ? 1 : searchRange(iRange);

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		if (plotValid(pLoopPlot))
		{
			// Super Forts begin *AI_offense* - modified if statement so forts will be attacked too
			if (pLoopPlot->isCity(true))
				//if (pLoopPlot->isCity() || (pLoopPlot->isCity(true, m_unit->getTeam()) && pLoopPlot->isVisibleEnemyUnit(m_unit))) - Original Code
				// Super Forts end
			{
				if (potentialEnemy(pLoopPlot->getTeam(), pLoopPlot))
				{
					if (!m_unit->atPlot(pLoopPlot) && ((bFollow) ? m_unit->canEnterOrAttackPlot(pLoopPlot, true) : (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns, iRange) && (iPathTurns <= iRange))))
					{
						const int iValue = m_unit->getGroup()->AI_attackOdds(pLoopPlot, true);

						if (iValue >= finalOddsThreshold(pLoopPlot, iOddsThreshold))
						{
							if (iValue > iBestValue)
							{
								const CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();

								if (endTurnPlot == pLoopPlot || !exposedToDanger((bFollow) ? pLoopPlot : endTurnPlot, iOddsThreshold))
								{
									iBestValue = iValue;
									pBestPlot = ((bFollow) ? pLoopPlot : endTurnPlot);
									FAssert(!m_unit->atPlot(pBestPlot));
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
		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), ((bFollow) ? MOVE_DIRECT_ATTACK : 0));
	}

	return false;
}


bool CvUnitDefaultAI::anyAttack(int iRange, int iOddsThreshold, int iMinStack, bool bAllowCities, bool bFollow)
{
	PROFILE_FUNC();

	FAssert(m_unit->canMove());

	if (ambush(iOddsThreshold))
	{
		return true;
	}
	if (iRange < 1)
	{
		return false;
	}

	if (rangeAttack(iRange))
	{
		return true;
	}

	const int iSearchRange = bFollow ? 1 : searchRange(iRange);

	CvReachablePlotSet plotSet(m_unit->getGroup(), MOVE_THROUGH_ENEMY, iSearchRange);

	if (m_unit->getDomainType() == DOMAIN_LAND && claimForts(&plotSet, 0, iRange + 1))
	{
		return true;
	}
	int iPathTurns;
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		FAssert(itr.stepDistance() <= iSearchRange);

		if (iMinStack == 0 && bAllowCities && !bFollow
		// Have we already considered m_unit m_unit->plot in a previous invocation?
		&& plotOpaqueInfoMatches(itr.getOpaqueInfo(ACTIVITY_ID_ANY_ATTACK), ACTIVITY_ID_ANY_ATTACK, iOddsThreshold))
		{
			continue;
		}

		CvPlot* pLoopPlot = itr.plot();

		if (CvSelectionGroup::getPathGenerator()->haveRouteLength(pLoopPlot, m_unit->getGroup(), 0, iPathTurns) && iPathTurns > iRange
		|| !bAllowCities && pLoopPlot->isCity(true)
		|| !m_unit->getGroup()->canEnterOrAttackPlot(pLoopPlot, true)
		|| (!pLoopPlot->isVisible(m_unit->getTeam(), false) || !pLoopPlot->isVisibleEnemyUnit(m_unit))
		&& (!pLoopPlot->isCity(true) || !potentialEnemy(pLoopPlot->getTeam()))
		|| pLoopPlot->getNumVisibleEnemyTargetUnits(m_unit) < iMinStack)
		{
			continue;
		}
		int iValue = 0;
		bool bWinLikely = false;
		int iAdjustedOddsThreshold = 0;

		PROFILE("CvUnitDefaultAI::anyAttack.FoundTarget");

		if (pLoopPlot->getNumVisiblePotentialEnemyDefenders(m_unit) > 0)
		{
			iValue += m_unit->getGroup()->AI_attackOdds(pLoopPlot, true, false, &bWinLikely, iOddsThreshold);
			iAdjustedOddsThreshold = finalOddsThreshold(pLoopPlot, iOddsThreshold);
			if (bWinLikely && pLoopPlot->isCity(false))
			{
				iValue += iOddsThreshold;
			}
		}
		else
		{
			iValue += 100 * pLoopPlot->getNumVisiblePotentialEnemyDefenderless(m_unit);
			iAdjustedOddsThreshold = 1;
			bWinLikely = true;
		}

		//	Increase value on our territory since we really want to get rid of enemies there even
		//	if it costs us a few losses
		if (pLoopPlot->getOwner() == m_unit->getOwner() && bWinLikely)
		{
			iValue *= 2;
		}

		if (iMinStack == 0 && bAllowCities && !bFollow && iAdjustedOddsThreshold != 0)
		{
			itr.setOpaqueInfo(ACTIVITY_ID_ANY_ATTACK, 1 + 100 * iValue * iOddsThreshold / iAdjustedOddsThreshold);
		}

		if (iValue > iBestValue && iValue >= iAdjustedOddsThreshold)
		{
			PROFILE("CvUnitDefaultAI::anyAttack.SearchPath");

			if (!m_unit->atPlot(pLoopPlot) && (bFollow ? m_unit->getGroup()->canEnterPlot(pLoopPlot, true) : m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns, iRange)))
			{
				PROFILE("CvUnitDefaultAI::anyAttack.SuccessfulPath");

				CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();

				if (endTurnPlot == pLoopPlot || !exposedToDanger(bFollow ? pLoopPlot : endTurnPlot, iOddsThreshold))
				{
					iBestValue = iValue;
					pBestPlot = bFollow ? pLoopPlot : endTurnPlot;
					FAssert(!m_unit->atPlot(pBestPlot));

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
				CvPlot* pLoopPlot = plotXY(m_unit->getX(), m_unit->getY(), iDX, iDY);

				if (pLoopPlot != NULL)
				{
					if (plotValid(pLoopPlot))
					{
						if (((bAllowCities) || !(pLoopPlot->isCity(false))))
						{
							if (pLoopPlot->isVisibleEnemyUnit(m_unit) || (pLoopPlot->isCity() && potentialEnemy(pLoopPlot->getTeam())))
							{
								if (pLoopPlot->getNumVisiblePotentialEnemyDefenders(m_unit) >= iMinStack)
								{
									PROFILE("CvUnitDefaultAI::anyAttack.FoundTarget");
									const int iValue = m_unit->getGroup()->AI_attackOdds(pLoopPlot, true);

									if (iValue > iBestValue && iValue >= finalOddsThreshold(pLoopPlot, iOddsThreshold))
									{
										PROFILE("CvUnitDefaultAI::anyAttack.SearchPath");
										if (!m_unit->atPlot(pLoopPlot) && ((bFollow) ? m_unit->getGroup()->canEnterPlot(pLoopPlot, true) : (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns))))
										{
											PROFILE("CvUnitDefaultAI::anyAttack.SuccessfulPath");
											if (iPathTurns <= iRange)
											{
												PROFILE("CvUnitDefaultAI::anyAttack.SuccessfulPath.InRange");

												iBestValue = iValue;
												pBestPlot = ((bFollow) ? pLoopPlot : m_unit->getPathEndTurnPlot());
												FAssert(!m_unit->atPlot(pBestPlot));
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
		FAssert((pNewAlgorithmBestPlot == NULL) == (pBestPlot == NULL) || m_unit->getDomainType() != DOMAIN_SEA);
		FAssert(iNewAlgorithmBestValue == iBestValue || m_unit->getDomainType() != DOMAIN_SEA);
	}
#endif

	if (pBestPlot != NULL)
	{
		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), ((bFollow) ? MOVE_DIRECT_ATTACK : 0));
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::rangeAttack(int iRange)
{
	PROFILE_FUNC();

	FAssert(m_unit->canMove());

	if (!m_unit->canRangeStrike())
	{
		return false;
	}

	const int iSearchRange = searchRange(iRange);

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		// Super Forts begin *AI_offense* - modified if statement so forts will be attacked too
		if ((pLoopPlot->isVisible(m_unit->getTeam(), false) && pLoopPlot->isVisibleEnemyUnit(m_unit)) || (pLoopPlot->isCity(true) && potentialEnemy(pLoopPlot->getTeam())))
			// Super Forts end
		{
			if (!m_unit->atPlot(pLoopPlot) && m_unit->canRangeStrikeAt(m_unit->plot(), pLoopPlot->getX(), pLoopPlot->getY()))
			{
				const int iValue = m_unit->getGroup()->AI_attackOdds(pLoopPlot, true);

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
		FAssert(!m_unit->atPlot(pBestPlot));
		m_unit->getGroup()->pushMission(MISSION_RANGE_ATTACK, pBestPlot->getX(), pBestPlot->getY(), 0);
		return true;
	}

	return false;
}

bool CvUnitDefaultAI::leaveAttack(int iRange, int iOddsThreshold, int iStrengthThreshold)
{
	int iPathTurns;

	FAssert(m_unit->canMove());

	const int iSearchRange = iRange;

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;


	const CvCity* pCity = m_unit->plot()->getPlotCity();

	if ((pCity != NULL) && (pCity->getOwner() == m_unit->getOwner()))
	{
		if (m_unit->canDefend() && m_unit->plot()->plotCount(PUF_canDefend, -1, -1, NULL, m_unit->getOwner()) <= 1)
		{
			return false;
		}

		int iOurStrength = GET_PLAYER(m_unit->getOwner()).AI_getOurPlotStrength(m_unit->plot(), 0, false, false);
		int iEnemyStrength = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);
		if (iEnemyStrength > 0)
		{
			if (((iOurStrength * 100) / iEnemyStrength) < iStrengthThreshold)
			{
				return false;
			}
		}
	}

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		if (plotValid(pLoopPlot))
		{
			// Super Forts begin *AI_offense* - modified if statement so forts will be attacked too
			if ((pLoopPlot->isVisible(m_unit->getTeam(), false) && pLoopPlot->isVisibleEnemyUnit(m_unit)) || (pLoopPlot->isCity(true) && potentialEnemy(pLoopPlot->getTeam(), pLoopPlot)))
				// Super Forts end
			{
				//	Koshling - removed m_unit check because it prevents defenceless stacks being attacked (why wouldn't you?)
				//if (pLoopPlot->getNumVisibleEnemyDefenders(m_unit) > 0)
				{
					if (!m_unit->atPlot(pLoopPlot) && (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns, iRange)))
					{

						const int iValue = m_unit->getGroup()->AI_attackOdds(pLoopPlot, true);

						if (iValue >= finalOddsThreshold(pLoopPlot, iOddsThreshold))
						{
							if (iValue > iBestValue)
							{
								const CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();

								if (endTurnPlot == pLoopPlot || !exposedToDanger(endTurnPlot, iOddsThreshold))
								{
									iBestValue = iValue;
									pBestPlot = endTurnPlot;
									FAssert(!m_unit->atPlot(pBestPlot));
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
		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0);
	}

	return false;
}

int CvUnitDefaultAI::blockadeValue(const CvPlot* pLoopPlot, const CvCity* pCity, CvPlot*& endTurnPlot) const
{
	int iValue = 0;
	int	iPathTurns;

	if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns))
	{
		PROFILE("CvUnitDefaultAI::blockade.PathChecked");

		iValue = 1;

		iValue += std::min(pCity->getPopulation(), pCity->countNumWaterPlots());

		iValue += GET_PLAYER(m_unit->getOwner()).AI_adjacentPotentialAttackers(pCity->plot());

		iValue += (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pCity->plot(), MISSIONAI_ASSAULT, m_unit->getGroup(), 2) * 3);

		if (m_unit->getGroup()->canBombard(pLoopPlot))
		{
			iValue *= 2;
		}

		iValue *= 1000;

		iValue /= (iPathTurns + 1);

		if (iPathTurns == 1)
		{
			//Prefer to have movement remaining to Bombard + Plunder
			iValue *= 1 + std::min(2, (m_unit->getPathMovementRemaining() + GC.getMOVE_DENOMINATOR() - 1) / GC.getMOVE_DENOMINATOR());
		}

		// if not at war with m_unit m_unit->plot owner, then devalue m_unit->plot if we already inside m_unit owner's borders
		// (because declaring war will pop us some unknown distance away)
		if (!m_unit->isEnemy(pLoopPlot->getTeam()) && m_unit->plot()->getTeam() == pLoopPlot->getTeam())
		{
			iValue /= 10;
		}

		endTurnPlot = m_unit->getPathEndTurnPlot();
	}

	return iValue;
}

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::blockade()
{
	PROFILE_FUNC();

	int iBestRange;
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	const CvPlot* pBestBlockadePlot = NULL;

	CvReachablePlotSet plotSet(m_unit->getGroup(), 0, MAX_INT);

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (!GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(m_unit->getTeam(), false))
		{
			continue;
		}
		const TeamTypes eTeamX = GET_PLAYER((PlayerTypes)iI).getTeam();

		if (!m_unit->isEnemy(eTeamX) && GET_TEAM(m_unit->getTeam()).AI_getWarPlan(eTeamX) == NO_WARPLAN)
		{
			continue;
		}

		foreach_(const CvCity * pCity, GET_PLAYER((PlayerTypes)iI).cities())
		{
			if (!pCity->isRevealed(m_unit->getTeam(), false)
			|| !pCity->isCoastal(GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize()))
			{
				continue;
			}
			foreach_(CvPlot * pLoopPlot, pCity->plot()->adjacent())
			{
				if (pLoopPlot->isRevealed(m_unit->getTeam(), false)
				&& pLoopPlot->getWorkingCity() == pCity
				&& (!pLoopPlot->isVisible(m_unit->getTeam(), false) || !pLoopPlot->isVisibleEnemyUnit(m_unit))
				&& m_unit->canPlunder(pLoopPlot) && plotSet.find(pLoopPlot) != plotSet.end()
				&& GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BLOCKADE, m_unit->getGroup(), 2) == 0)
				{
					CvReachablePlotSet::const_iterator itr = plotSet.find(pLoopPlot);

					if (iBestValue > 0 && itr.stepDistance() > 2 * iBestRange)
					{
						continue;
					}
					if (m_unit->potentialWarAction(pLoopPlot))
					{
						PROFILE("CvUnitDefaultAI::blockade.Possibility");

						CvPlot* endTurnPlot;
						const int iValue = blockadeValue(pLoopPlot, pCity, endTurnPlot);

						if (iValue > iBestValue && (endTurnPlot == pLoopPlot || !exposedToDanger(endTurnPlot, 70)))
						{
							iBestValue = iValue;
							iBestRange = itr.stepDistance();
							pBestPlot = endTurnPlot;
							pBestBlockadePlot = pLoopPlot;
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL && pBestBlockadePlot != NULL)
	{
		FAssert(m_unit->canPlunder(pBestBlockadePlot));
		if (m_unit->atPlot(pBestBlockadePlot) && !m_unit->isEnemy(pBestBlockadePlot->getTeam(), pBestBlockadePlot))
		{
			m_unit->getGroup()->groupDeclareWar(pBestBlockadePlot, true);
		}

		if (m_unit->atPlot(pBestBlockadePlot))
		{
			if (m_unit->getGroup()->canBombard(m_unit->plot()))
			{
				m_unit->getGroup()->pushMission(MISSION_BOMBARD, -1, -1, 0, false, false, MISSIONAI_BLOCKADE, pBestBlockadePlot);
			}
			m_unit->getGroup()->pushMission(MISSION_PLUNDER, -1, -1, 0, (m_unit->getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BLOCKADE, pBestBlockadePlot);

			return true;
		}
		else
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_BLOCKADE, pBestBlockadePlot);
		}
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::pirateBlockade()
{
	PROFILE_FUNC();

	std::vector<int> aiDeathZone(GC.getMap().numPlots(), 0);

	CvReachablePlotSet plotSet(m_unit->getGroup(), 0, MAX_INT);

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* plotX = GC.getMap().plotByIndex(iI);

		if (plotSet.find(plotX) != plotSet.end() && plotX->isOwned() && plotX->getTeam() != m_unit->getTeam())
		{
			int iBestHostileMoves = 0;
			foreach_(CvUnit * unitX, plotX->units())
			{
				if (m_unit->isEnemy(unitX->getTeam(), unitX->plot(), unitX)
				&&  unitX->getDomainType() == DOMAIN_SEA
				&& !unitX->isInvisible(m_unit->getTeam(), false)
				&&  unitX->canAttack()
				&&  unitX->currEffectiveStr(NULL, NULL, NULL) > m_unit->currEffectiveStr(plotX, unitX, NULL))
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

	const bool bIsInDanger = aiDeathZone[GC.getMap().plotNum(m_unit->getX(), m_unit->getY())] > 0;
	const bool bHasTerrainDamage = m_unit->plot()->getTotalTurnDamage(m_unit->getGroup()) > 0 || m_unit->plot()->getFeatureTurnDamage() > 0;
	const bool bNeedsHeal = m_unit->getDamage() > 0;
	const bool bHurtAndInWildernessOrInBadShape = bNeedsHeal && (!m_unit->plot()->isOwned() && !m_unit->plot()->isAdjacentOwned() || m_unit->getDamagePercent() > 25);

	if (!bIsInDanger && bHurtAndInWildernessOrInBadShape)
	{
		// If we only have damage explained by the current m_unit->plot for one turn
		//	don't use that as an excuse to immediately turn around and go hide in a city!
		if (!bHasTerrainDamage || m_unit->getDamage() > (m_unit->plot()->getTotalTurnDamage(m_unit->getGroup()) + m_unit->plot()->getFeatureTurnDamage()))
		{
			if (retreatToCity(false, false, 1 + m_unit->getDamagePercent() / 20))
			{
				return true;
			}
		}

		if (!bHasTerrainDamage)
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP);
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
			if (!pLoopPlot->isVisible(m_unit->getTeam(), false) && !pLoopPlot->isVisibleEnemyUnit(m_unit) && m_unit->canPlunder(pLoopPlot))
			{
				if (GC.getGame().getSorenRandNum(4, "AI Pirate Blockade") == 0)
				{
					if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BLOCKADE, m_unit->getGroup(), 3) == 0)
					{
						int iBlockadedCount = 0;
						int iPopulationValue = 0;
						int iRange = GC.getSHIP_BLOCKADE_RANGE() - 1;
						foreach_(const CvPlot * pRangePlot, pLoopPlot->rect(iRange, iRange))
						{
							bool bPlotBlockaded = false;
							if (pRangePlot->isWater() && pRangePlot->isOwned() && m_unit->isEnemy(pRangePlot->getTeam(), pLoopPlot))
							{
								bPlotBlockaded = true;
								iBlockadedCount += pRangePlot->getBlockadedCount(pRangePlot->getTeam());
							}

							if (!bPlotBlockaded)
							{
								const CvCity* pPlotCity = pRangePlot->getPlotCity();
								if (pPlotCity != NULL)
								{
									if (m_unit->isEnemy(pPlotCity->getTeam(), pLoopPlot))
									{
										int iCityValue = 3 + pPlotCity->getPopulation();
										iCityValue *= (atWar(m_unit->getTeam(), pPlotCity->getTeam()) ? 1 : 3);
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

						if (m_unit->atPlot(pLoopPlot))
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
						const int iMinTurns = itr.stepDistance() / m_unit->baseMoves() + 1;

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

						if (iBestEstimateValue > iBestValue && m_unit->generatePath(pLoopPlot, MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns))
						{
							bool bForceMove = false;

							if (!iDeath && bIsInDanger
							&& iPathTurns < 3 && 0 == iPopulationValue
							&& m_unit->getPathMovementRemaining() == 0
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
							const bool bMove = iPathTurns == 1 && m_unit->getPathMovementRemaining() > 0;

							if (bMove && !m_unit->atPlot(pLoopPlot))
							{
								iValue *= 2;
							}
							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = bForceMove ? pLoopPlot : m_unit->getPathEndTurnPlot();
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
		FAssert(m_unit->canPlunder(pBestBlockadePlot));

		if (m_unit->atPlot(pBestBlockadePlot))
		{
			m_unit->getGroup()->pushMission(MISSION_PLUNDER, -1, -1, 0, (m_unit->getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BLOCKADE, pBestBlockadePlot);
			return true;
		}
		FAssert(!m_unit->atPlot(pBestPlot));

		if (bNeedsHeal)
		{
			CvPlot* pBestStopAndHealPlot = pBestPlot;

			// Examine plots we move through in m_unit turn and see if any is suitable to stop and heal in
			m_unit->generatePath(pBestPlot, MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns);
			CvPath::const_iterator itr = m_unit->getGroup()->getPath().begin();

			while (itr != m_unit->getGroup()->getPath().end())
			{
				CvPlot* pPlot = itr.plot();

				// If an intermediary m_unit->plot is one that the heal decsion logic (near the start of m_unit method)
				//	would choose to heal in, then just stop there on our way
				if (bHurtAndInWildernessOrInBadShape
				&& aiDeathZone[GC.getMap().plotNum(pPlot->getX(), pPlot->getY())] < 1
				&& pPlot->getTotalTurnDamage(m_unit->getGroup()) < 1
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
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_WEIGHT_3);
		}

		if (m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_WEIGHT_3, false, false, MISSIONAI_BLOCKADE, pBestBlockadePlot))
		{
			if (bBestIsMove)
			{
				m_unit->getGroup()->pushMission(MISSION_PLUNDER, -1, -1, 0, (m_unit->getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BLOCKADE, pBestBlockadePlot);
			}
			return true;
		}
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::seaBombardRange(int iMaxRange)
{
	PROFILE_FUNC();

	const CvPlayerAI& kPlayer = GET_PLAYER(m_unit->getOwner());
	const CvPlot* pPlot = m_unit->plot();
	const CvSelectionGroup* pGroup = m_unit->getGroup();

	// can any unit in m_unit group bombard?
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
		return false;
	}

	// best match
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestBombardPlot = NULL;
	const CvPlot* endTurnPlot = NULL;
	int iBestValue = 0;

	CvReachablePlotSet plotSet(m_unit->getGroup(), 0, iMaxRange);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		const CvPlot* pLoopPlot = itr.plot();
		if (pLoopPlot != NULL /*&& plotValid(pLoopPlot)*/)
		{
			CvCity* pBombardCity = m_unit->bombardTarget(pLoopPlot);

			if (pBombardCity != NULL && m_unit->isEnemy(pBombardCity->getTeam(), pLoopPlot) && pBombardCity->getDefenseDamage() < GC.getMAX_CITY_DEFENSE_DAMAGE())
			{
				int iPathTurns;
				if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns))
				{
					// Loop construction doesn't guarantee we can get there anytime soon, could be on other side of narrow continent
					if (iPathTurns <= (1 + iMaxRange / std::max(1, m_unit->baseMoves())))
					{
						// Check only for supporting our own ground troops first, if none will look for another target
						int iValue = (kPlayer.AI_plotTargetMissionAIs(pBombardCity->plot(), MISSIONAI_ASSAULT, NULL, 2) * 3);
						iValue += (kPlayer.AI_adjacentPotentialAttackers(pBombardCity->plot(), true));

						if (iValue > 0)
						{
							iValue *= 1000;

							iValue /= (iPathTurns + 1);

							if (iPathTurns == 1)
							{
								//Prefer to have movement remaining to Bombard + Plunder
								iValue *= 1 + std::min(2, m_unit->getPathMovementRemaining());
							}

							if (iValue > iBestValue)
							{
								endTurnPlot = m_unit->getPathEndTurnPlot();

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
		if (getUnitAIType() != UNITAI_ASSAULT_SEA)
		{
			foreach_(const CvPlot * pLoopPlot, pPlot->rect(iMaxRange, iMaxRange))
			{
				if (plotValid(pLoopPlot))
				{
					CvCity* pBombardCity = m_unit->bombardTarget(pLoopPlot);

					// Consider city even if fully bombarded, causes ship to camp outside blockading instead of twitching between
					// cities after bombarding to 0
					if (pBombardCity != NULL && m_unit->isEnemy(pBombardCity->getTeam(), pLoopPlot) && pBombardCity->getTotalDefense(false) > 0)
					{
						int iPathTurns;
						if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns))
						{
							// Loop construction doesn't guarantee we can get there anytime soon, could be on other side of narrow continent
							if (iPathTurns <= 1 + iMaxRange / std::max(1, m_unit->baseMoves()))
							{
								int iValue = std::min(20, pBombardCity->getDefenseModifier(false) / 2);

								// Inclination to support attacks by others
								if (GET_PLAYER(pBombardCity->getOwner()).AI_getAnyPlotDanger(pBombardCity->plot(), 2, false))
								{
									iValue += 60;
								}

								// Inclination to bombard a different nearby city to extend the reach of blockade
								if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pBombardCity->plot(), MISSIONAI_BLOCKADE, m_unit->getGroup(), 3) == 0)
								{
									iValue += 35 + pBombardCity->getPopulation();
								}

								// Small inclination to bombard m_unit->area target, not too large so as not to tip our hand
								if (pBombardCity == pBombardCity->area()->getTargetCity(m_unit->getOwner()))
								{
									iValue += 10;
								}

								if (iValue > 0)
								{
									iValue *= 1000;

									iValue /= (iPathTurns + 1);

									if (iPathTurns == 1)
									{
										//Prefer to have movement remaining to Bombard + Plunder
										iValue *= 1 + std::min(2, (m_unit->getPathMovementRemaining() + GC.getMOVE_DENOMINATOR() - 1) / GC.getMOVE_DENOMINATOR());
									}

									if (iValue > iBestValue)
									{
										iBestValue = iValue;
										pBestPlot = m_unit->getPathEndTurnPlot();
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
		if (m_unit->atPlot(pBestBombardPlot))
		{
			// if we are at the m_unit->plot from which to bombard, and we have a unit that can bombard m_unit turn, do it
			if (bBombardUnitCanBombardNow && pGroup->canBombard(pBestBombardPlot))
			{
				if (RbombardNaval())
				{
					return true;
				}
				m_unit->getGroup()->pushMission(MISSION_BOMBARD, -1, -1, 0, false, false, MISSIONAI_BLOCKADE, pBestBombardPlot);

				// if city bombarded enough, wake up any units that were waiting to bombard m_unit city
				CvCity* pBombardCity = m_unit->bombardTarget(pBestBombardPlot); // is NULL if city cannot be bombarded any more
				if (pBombardCity == NULL || pBombardCity->getDefenseDamage() < ((GC.getMAX_CITY_DEFENSE_DAMAGE() * 5) / 6))
				{
					kPlayer.AI_wakePlotTargetMissionAIs(pBestBombardPlot, MISSIONAI_BLOCKADE, m_unit->getGroup());
				}
			}
			// otherwise, skip until next turn, when we will surely bombard
			else if (m_unit->canPlunder(pBestBombardPlot))
			{
				m_unit->getGroup()->pushMission(MISSION_PLUNDER, -1, -1, 0, false, false, MISSIONAI_BLOCKADE, pBestBombardPlot);
			}
			else
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP);
			}
			return true;
		}
		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_BLOCKADE, pBestBombardPlot);
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::pillage(int iBonusValueThreshold)
{
	PROFILE_FUNC();

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestPillagePlot = NULL;

	CvReachablePlotSet plotSet(m_unit->getGroup(), 0, MAX_INT);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		CvPlot* plotX = itr.plot();

		if (plotX->area() == m_unit->area()
		&& plotX->isOwned()
		&& m_unit->isEnemy(plotX->getTeam(), plotX))
		{
			CvCity* pWorkingCity = plotX->getWorkingCity();

			if (pWorkingCity != NULL
			&& pWorkingCity != m_unit->area()->getTargetCity(m_unit->getOwner())
			&& m_unit->getGroup()->canPillage(plotX)
			&& plotX->isRevealed(m_unit->getTeam(), false)
			&& (!plotX->isVisible(m_unit->getTeam(), false) || !plotX->isVisibleEnemyUnit(m_unit))
			&& GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(plotX, MISSIONAI_PILLAGE, m_unit->getGroup(), 1) == 0)
			{
				int iValue = 1000 * pillageValue(plotX, iBonusValueThreshold);

				// if not at war with m_unit m_unit->plot owner, then devalue m_unit->plot if we already inside m_unit owner's borders
				// (because declaring war will pop us some unknown distance away)
				if (m_unit->getCombatOwner(plotX->getTeam(), plotX) == m_unit->getOwner()
				&& !m_unit->isEnemy(plotX->getTeam())
				&& m_unit->plot()->getTeam() == plotX->getTeam())
				{
					iValue /= 10;
				}

				if (iValue > iBestValue)
				{
					int iPathTurns;
					if (m_unit->generatePath(plotX, 0, true, &iPathTurns))
					{
						iValue /= iPathTurns + 1;

						if (iValue > iBestValue)
						{
							CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();

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
		if (m_unit->atPlot(pBestPillagePlot) && !m_unit->isEnemy(pBestPillagePlot->getTeam()))
		{
			//m_unit->getGroup()->groupDeclareWar(pBestPillagePlot, true);
			// rather than declare war, just find something else to do, since we may already be deep in enemy territory
			return false;
		}

		if (m_unit->atPlot(pBestPillagePlot))
		{
			if (m_unit->isEnemy(pBestPillagePlot->getTeam()))
			{
				m_unit->getGroup()->pushMission(MISSION_PILLAGE, -1, -1, 0, false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
				return true;
			}
		}
		else
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
		}
	}

	return false;
}

bool CvUnitDefaultAI::canPillage(const CvPlot& kPlot) const
{
	if (m_unit->isEnemy(kPlot.getTeam(), &kPlot))
	{
		return true;
	}

	if (!kPlot.isOwned())
	{
		bool bPillageUnowned = true;

		for (int iPlayer = 0; iPlayer < MAX_PLAYERS && bPillageUnowned; ++iPlayer)
		{
			const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			if (!m_unit->isEnemy(kLoopPlayer.getTeam(), &kPlot))
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
bool CvUnitDefaultAI::pillageRange(int iRange, int iBonusValueThreshold)
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

	iSearchRange = searchRange(iRange);

	iBestValue = 0;
	pBestPlot = NULL;
	pBestPillagePlot = NULL;

	CvReachablePlotSet plotSet(m_unit->getGroup(), 0, iSearchRange);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		pLoopPlot = itr.plot();

		//	Have we already considered m_unit m_unit->plot in a previous invocation?
		int iOpaqueInfo = itr.getOpaqueInfo(ACTIVITY_ID_PILLAGE);
		if (plotOpaqueInfoMatches(iOpaqueInfo, ACTIVITY_ID_PILLAGE, iBonusValueThreshold + 1))
		{
			continue;
		}

		itr.setOpaqueInfo(ACTIVITY_ID_PILLAGE, iBonusValueThreshold + 1);

		if ((!(pLoopPlot->isNPC()) || !GET_PLAYER(m_unit->getOwner()).isModderOption(MODDEROPTION_AUTO_PILLAGE_AVOID_BARBARIAN_CITIES)))
		{
			if (m_unit->potentialWarAction(pLoopPlot))
			{
				CvCity* pWorkingCity = pLoopPlot->getWorkingCity();

				if (pWorkingCity != NULL)
				{
					if (!(pWorkingCity == m_unit->area()->getTargetCity(m_unit->getOwner())) && m_unit->getGroup()->canPillage(pLoopPlot))
					{
						if (!pLoopPlot->isVisible(m_unit->getTeam(), false) || !pLoopPlot->isVisibleEnemyUnit(m_unit))
						{
							if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_PILLAGE, m_unit->getGroup()) == 0)
							{
								iValue = 1000 * pillageValue(pLoopPlot, iBonusValueThreshold);

								// if not at war with m_unit m_unit->plot owner, then devalue m_unit->plot if we already inside m_unit owner's borders
								// (because declaring war will pop us some unknown distance away)
								if (!m_unit->isEnemy(pLoopPlot->getTeam()) && m_unit->plot()->getTeam() == pLoopPlot->getTeam())
								{
									iValue /= 10;
								}

								//	TODO - should really evaluate how dangerous the target m_unit->plot is and take acount of that
								//	in both the value and whether to even consider it.  Factors to take account of:
								//		Defense modifier
								//		Adjacency damage
								//		Enemy stacks in strenght relationship to our own

								if (iValue > iBestValue && m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns, iRange))
								{
									if (m_unit->getPathMovementRemaining() == 0)
									{
										iPathTurns++;
									}

									if (iPathTurns <= iRange)
									{
										iValue /= (iPathTurns + 1);

										if (iValue > iBestValue)
										{
											endTurnPlot = m_unit->getPathEndTurnPlot();

											if (endTurnPlot == pLoopPlot || !exposedToDanger(endTurnPlot, 70))
											{
												iBestValue = iValue;
												pBestPlot = endTurnPlot;
												pBestPillagePlot = pLoopPlot;

												FAssert(pBestPlot == pBestPillagePlot || !m_unit->atPlot(pBestPlot));
											}
										}
									}
								}
								else
								{
									//	Actually out of range so not really markable as searched (if we'd already
									//	found a pathable candidate it wont matter anyway since m_unit routine will
									//	push a mission)
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
			pLoopPlot = plotXY(m_unit->getX(), m_unit->getY(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				if (plotValid(pLoopPlot) && (!(pLoopPlot->isNPC()) || !GET_PLAYER(m_unit->getOwner()).isModderOption(MODDEROPTION_AUTO_PILLAGE_AVOID_BARBARIAN_CITIES)))
				{
					if (m_unit->potentialWarAction(pLoopPlot))
					{
						CvCity* pWorkingCity = pLoopPlot->getWorkingCity();

						if (pWorkingCity != NULL)
						{
							if (!(pWorkingCity == m_unit->area()->getTargetCity(m_unit->getOwner())) && canPillage(pLoopPlot))
							{
								if (!(pLoopPlot->isVisibleEnemyUnit(m_unit)))
								{
									if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_PILLAGE, m_unit->getGroup()) == 0)
									{
										if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns))
										{
											if (getPathLastNode()->m_iData1 == 0)
											{
												iPathTurns++;
											}

											if (iPathTurns <= iRange)
											{
												iValue = pillageValue(pLoopPlot, iBonusValueThreshold);

												iValue *= 1000;

												iValue /= (iPathTurns + 1);

												// if not at war with m_unit m_unit->plot owner, then devalue m_unit->plot if we already inside m_unit owner's borders
												// (because declaring war will pop us some unknown distance away)
												if (!m_unit->isEnemy(pLoopPlot->getTeam()) && m_unit->plot()->getTeam() == pLoopPlot->getTeam())
												{
													iValue /= 10;
												}

												if (iValue > iBestValue)
												{
													iBestValue = iValue;
													pBestPlot = m_unit->getPathEndTurnPlot();
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

	FAssert((pNewAlgorithmBestPlot == NULL) == (pBestPlot == NULL) || m_unit->getDomainType() != DOMAIN_SEA);
	FAssert(iNewAlgorithmBestValue == iBestValue || m_unit->getDomainType() != DOMAIN_SEA);
#endif

	if (pBestPlot != NULL && pBestPillagePlot != NULL)
	{
		if (m_unit->atPlot(pBestPillagePlot) && !m_unit->isEnemy(pBestPillagePlot->getTeam()))
		{
			//m_unit->getGroup()->groupDeclareWar(pBestPillagePlot, true);
			// rather than declare war, just find something else to do, since we may already be deep in enemy territory
			return false;
		}

		if (!m_unit->atPlot(pBestPillagePlot))
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
		}

		if (m_unit->isEnemy(pBestPillagePlot->getTeam()))
		{
			return m_unit->getGroup()->pushMissionInternal(MISSION_PILLAGE, pBestPillagePlot->getX(), pBestPillagePlot->getY(), 0, false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
		}
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::found()
{
	PROFILE_FUNC();

	int iPathTurns, iBestFoundValue = 0;
	const CvPlot* pBestPlot = NULL;
	CvPlot* pBestFoundPlot = NULL;

	for (int iI = 0; iI < GET_PLAYER(m_unit->getOwner()).AI_getNumCitySites(); iI++)
	{
		CvPlot* pCitySitePlot = GET_PLAYER(m_unit->getOwner()).AI_getCitySite(iI);

		if ((pCitySitePlot->getArea() == m_unit->getArea() || m_unit->canMoveAllTerrain()) && m_unit->canFound(pCitySitePlot)
		&& (!pCitySitePlot->isVisible(m_unit->getTeam(), false) || !pCitySitePlot->isVisibleEnemyUnit(m_unit))
		&& GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pCitySitePlot, MISSIONAI_FOUND, m_unit->getGroup()) == 0
		&& m_unit->getGroup()->canDefend() && m_unit->getGroup()->getStrength() >= minSettlerDefense()
		&& generateSafePathforVulnerable(pCitySitePlot, &iPathTurns))
		{
			const int iValue = 1000 * pCitySitePlot->getFoundValue(m_unit->getOwner()) / (iPathTurns + 1);

			if (iValue > iBestFoundValue)
			{
				const CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();

				if (endTurnPlot == pCitySitePlot || !exposedToDanger(endTurnPlot, 80))
				{
					iBestFoundValue = iValue;
					pBestPlot = endTurnPlot;
					pBestFoundPlot = pCitySitePlot;
				}
			}
		}
	}

	if (pBestPlot != NULL && pBestFoundPlot != NULL)
	{
		if (m_unit->atPlot(pBestFoundPlot))
		{
			if (gUnitLogLevel >= 2)
			{
				logBBAI("	Settler (%d) founding at best found m_unit->plot %d, %d", m_unit->getID(), pBestFoundPlot->getX(), pBestFoundPlot->getY());
			}
			m_unit->getGroup()->pushMission(MISSION_FOUND, -1, -1, 0, false, false, MISSIONAI_FOUND, pBestFoundPlot);
			return true;
		}

		if (gUnitLogLevel >= 2)
		{
			logBBAI("	Settler (%d) heading for best found m_unit->plot %d, %d, current group strength (%d)", m_unit->getID(), pBestFoundPlot->getX(), pBestFoundPlot->getY(), m_unit->getGroup()->getStrength());
		}

		if (generateSafePathforVulnerable(pBestPlot, &iPathTurns))
		{
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_FOUND, pBestFoundPlot);
		}
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return true;
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::foundRange(int iRange, bool bFollow)
{
	PROFILE_FUNC();

	int iPathTurns;

	const int iSearchRange = searchRange(iRange);

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestFoundPlot = NULL;

	foreach_(CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		if (plotValid(pLoopPlot) && (pLoopPlot != m_unit->plot() || GET_PLAYER(m_unit->getOwner()).AI_getPlotDanger(pLoopPlot, 1) <= pLoopPlot->plotCount(PUF_canDefend, -1, -1, NULL, m_unit->getOwner())))
		{
			if (m_unit->canFound(pLoopPlot))
			{
				const int iValue = pLoopPlot->getFoundValue(m_unit->getOwner());

				if (iValue > iBestValue)
				{
					if (!pLoopPlot->isVisible(m_unit->getTeam(), false) || !pLoopPlot->isVisibleEnemyUnit(m_unit))
					{
						if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_FOUND, m_unit->getGroup(), 3) == 0)
						{
							if (generateSafePathforVulnerable(pLoopPlot, &iPathTurns))/*(m_unit->generatePath(pLoopPlot, MOVE_SAFE_TERRITORY, true, &iPathTurns, iRange))*/
							{
								if (iPathTurns <= iRange)
								{
									const CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();

									if (endTurnPlot == pLoopPlot || !exposedToDanger(endTurnPlot, 80))
									{
										iBestValue = iValue;
										pBestPlot = endTurnPlot;
										pBestFoundPlot = pLoopPlot;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL && pBestFoundPlot != NULL)
	{
		if (m_unit->atPlot(pBestFoundPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_FOUND, -1, -1, 0, false, false, MISSIONAI_FOUND, pBestFoundPlot);
			return true;
		}
		if (!bFollow)
		{
			if (generateSafePathforVulnerable(pBestPlot, &iPathTurns))
			{
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_FOUND, pBestFoundPlot);
			}
			m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
			return true;
		}
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::assaultSeaTransport(bool bBarbarian)
{
	PROFILE_FUNC();

	bool bIsAttackCity = (m_unit->getUnitAICargo(UNITAI_ATTACK_CITY) > 0);

	FAssert(m_unit->getGroup()->hasCargo());
	//FAssert(bIsAttackCity || m_unit->getGroup()->getUnitAICargo(UNITAI_ATTACK) > 0);

	if (!m_unit->canCargoAllMove())
	{
		return false;
	}

	std::vector<CvUnit*> aGroupCargo;
	foreach_(CvUnit * pLoopUnit, m_unit->plot()->units())
	{
		const CvUnit* pTransport = pLoopUnit->getTransportUnit();
		if (pTransport != NULL && pTransport->getGroup() == m_unit->getGroup())
		{
			aGroupCargo.push_back(pLoopUnit);
		}
	}

	int iCargoCount = m_unit->getGroup()->getCargo();
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestAssaultPlot = NULL;
	CvPlot* endTurnPlot = NULL;
	CvReachablePlotSet plotSet(m_unit->getGroup(), MOVE_AVOID_ENEMY_WEIGHT_3 | MOVE_ALLOW_ADJACENT_COASTAL, MAX_INT);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		CvPlot* pLoopPlot = itr.plot();
		if (pLoopPlot->isCoastalLand() && pLoopPlot->isOwned()
		&& (bBarbarian || !pLoopPlot->isNPC() || GET_PLAYER(m_unit->getOwner()).isMinorCiv())
		&& m_unit->isPotentialEnemy(pLoopPlot->getTeam(), pLoopPlot))
		{
			PROFILE("CvUnitDefaultAI::assaultSeaTransport.PotentialPlot");

			const int iTargetCities = pLoopPlot->area()->getCitiesPerPlayer(pLoopPlot->getOwner());
			if (iTargetCities > 0)
			{
				bool bCanCargoAllUnload = true;
				const int iVisibleEnemyDefenders = pLoopPlot->getNumVisiblePotentialEnemyDefenders(m_unit);

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
						else if (pLoopPlot->isCity() && !pLoopPlot->isVisible(m_unit->getTeam(), false))
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
					PROFILE("CvUnitDefaultAI::assaultSeaTransport.CanUnload");

					int iValue = 1;

					if (!bIsAttackCity)
					{
						iValue += pillageValue(pLoopPlot, 15) * 10;
					}

					const int iAssaultsHere = GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_ASSAULT, m_unit->getGroup());

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
						PROFILE("CvUnitDefaultAI::assaultSeaTransport.EvalCity");

						FAssert(m_unit->isPotentialEnemy(pCity->getTeam(), pLoopPlot));

						if (!pLoopPlot->isRiverCrossing(directionXY(pLoopPlot, pCity->plot())))
						{
							iValue += (50 * -(GC.getRIVER_ATTACK_MODIFIER()));
						}

						iValue += 15 * (pLoopPlot->defenseModifier(m_unit->getTeam(), false));
						iValue += 1000;
						iValue += (GET_PLAYER(m_unit->getOwner()).AI_adjacentPotentialAttackers(pCity->plot()) * 200);

						// Continue attacking in m_unit->area we have already captured cities
						if (pCity->area()->getCitiesPerPlayer(m_unit->getOwner()) > 0 && pCity->AI_playerCloseness(m_unit->getOwner()) > 5)
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
					if (iValue > 0 && m_unit->generatePath(pLoopPlot, MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns, iBestValue == 0 ? MAX_INT : (iValue + iThisTurnValueIncrement) / iBestValue))
					{
						PROFILE("CvUnitDefaultAI::assaultSeaTransport.ValidPath");

						if (iPathTurns <= 1)
						{
							iValue += iThisTurnValueIncrement;
						}

						iValue /= (iPathTurns + 1);

						if (iValue > iBestValue)
						{
							endTurnPlot = m_unit->getPathEndTurnPlot();

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
		PROFILE("CvUnitDefaultAI::assaultSeaTransport.ProcessTarget");

		FAssert(!(pBestPlot->isImpassable(m_unit->getTeam())));

		// Cancel missions of all those coming to join departing transport
		const CvPlayer& kPlayer = GET_PLAYER(m_unit->getOwner());

		foreach_(CvSelectionGroup * pLoopGroup, kPlayer.groups())
		{
			if (pLoopGroup != m_unit->getGroup() && pLoopGroup->AI_getMissionAIType() == MISSIONAI_GROUP && pLoopGroup->getHeadUnitAI() == getUnitAIType())
			{
				const CvUnit* pMissionUnit = pLoopGroup->AI_getMissionAIUnit();

				if (pMissionUnit != NULL && pMissionUnit->getGroup() == m_unit->getGroup())
				{
					pLoopGroup->clearMissionQueue();
				}
			}
		}

		if (pBestPlot == pBestAssaultPlot || stepDistance(pBestPlot->getX(), pBestPlot->getY(), pBestAssaultPlot->getX(), pBestAssaultPlot->getY()) == 1)
		{
			if (m_unit->atPlot(pBestAssaultPlot))
			{
				m_unit->getGroup()->unloadAll(); // XXX is m_unit dangerous (not pushing a mission...) XXX air units?
				return true;
			}
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestAssaultPlot->getX(), pBestAssaultPlot->getY(), MOVE_AVOID_ENEMY_WEIGHT_3, false, false, MISSIONAI_ASSAULT, pBestAssaultPlot);
		}
		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_WEIGHT_3, false, false, MISSIONAI_ASSAULT, pBestAssaultPlot);
	}

	return false;
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD					  02/07/10								jdog5000	  */
/*																							  */
/* Naval AI, Efficiency																		 */
/************************************************************************************************/
// Returns true if a mission was pushed...
bool CvUnitDefaultAI::assaultSeaReinforce(bool bBarbarian)
{
	PROFILE_FUNC();

	FAssert(m_unit->getGroup()->hasCargo());

	if (!m_unit->canCargoAllMove() || !m_unit->getGroup()->canAllMove())
	{
		return false;
	}

	std::vector<CvUnit*> aGroupCargo;
	foreach_(CvUnit * pLoopUnit, m_unit->plot()->units())
	{
		const CvUnit* pTransport = pLoopUnit->getTransportUnit();
		if (pTransport != NULL && pTransport->getGroup() == m_unit->getGroup())
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
	CvArea* pWaterArea = m_unit->plot()->waterArea();
	bool bCity = m_unit->plot()->isCity(true, m_unit->getTeam());
	bool bCanMoveAllTerrain = m_unit->getGroup()->canMoveAllTerrain();

	int iValue;

	// Loop over nearby plots for groups in enemy territory to reinforce
	{
		PROFILE("AI_assaultSeaReinforce.Nearby");

		const int iRange = 2 * m_unit->baseMoves();
		foreach_(CvPlot * pLoopPlot, m_unit->plot()->rect(iRange, iRange))
		{
			if (pLoopPlot->isOwned() && m_unit->isEnemy(pLoopPlot->getTeam(), pLoopPlot))
			{
				if (bCanMoveAllTerrain || (pWaterArea != NULL && pLoopPlot->isAdjacentToArea(pWaterArea)))
				{
					const int iTargetCities = pLoopPlot->area()->getCitiesPerPlayer(pLoopPlot->getOwner());

					if (iTargetCities > 0)
					{
						const int iOurFightersHere = pLoopPlot->getNumDefenders(m_unit->getOwner());

						if (iOurFightersHere > 2)
						{
							int iPathTurns;
							if (m_unit->generatePath(pLoopPlot, MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns))
							{
								if (iPathTurns <= 2)
								{
									pEndTurnPlot = m_unit->getPathEndTurnPlot();

									iValue = 10 * iTargetCities;
									iValue += 8 * iOurFightersHere;
									iValue += 3 * GET_PLAYER(m_unit->getOwner()).AI_adjacentPotentialAttackers(pLoopPlot);

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

			foreach_(const CvSelectionGroup * pLoopSelectionGroup, GET_PLAYER(m_unit->getOwner()).groups())
			{
				if (pLoopSelectionGroup != m_unit->getGroup())
				{
					if (pLoopSelectionGroup->AI_getMissionAIType() == MISSIONAI_ASSAULT)
					{
						CvPlot* pLoopPlot = pLoopSelectionGroup->AI_getMissionAIPlot();

						if (pLoopPlot != NULL)
						{
							if (pLoopPlot->isOwned())
							{
								if (m_unit->isPotentialEnemy(pLoopPlot->getTeam(), pLoopPlot))
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
												int	iStepPathTurns = (10 * GC.getMap().calculatePathDistance(m_unit->plot(), pLoopPlot) + 5) / (10 * m_unit->baseMoves());
												int	iOtherStepPathTurns = (10 * GC.getMap().calculatePathDistance(pLoopSelectionGroup->plot(), pLoopPlot) + 5) / (10 * pLoopSelectionGroup->getHeadUnit()->baseMoves());

												if ((iStepPathTurns > iOtherStepPathTurns) && (iStepPathTurns < iOtherStepPathTurns + 6))
												{
													int iPathTurns;
													if (m_unit->generatePath(pLoopPlot, MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns))
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
															pBestPlot = m_unit->getPathEndTurnPlot();
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

			foreach_(CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
			{
				if (bCanMoveAllTerrain || (pWaterArea != NULL && (pLoopCity->waterArea(true) == pWaterArea || pLoopCity->secondWaterArea() == pWaterArea)))
				{
					iValue = 0;
					if (pLoopCity->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_DEFENSIVE)
					{
						iValue = 3;
					}
					else if (pLoopCity->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_OFFENSIVE)
					{
						iValue = 2;
					}
					else if (pLoopCity->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_MASSING)
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
						if ((bCity && pLoopCity->area() != m_unit->area()) || bCityDanger || ((GC.getGame().getGameTurn() - pLoopCity->getGameTurnAcquired()) < 10 && pLoopCity->getPreviousOwner() != NO_PLAYER))
						{
							int iOurPower = std::max(1, pLoopCity->area()->getPower(m_unit->getOwner()));
							// Enemy power includes barb power
							int iEnemyPower = GET_TEAM(m_unit->getTeam()).countEnemyPowerByArea(pLoopCity->area());

							// Don't send troops to m_unit->areas we are dominating already
							// Don't require presence of enemy cities, just a dangerous force
							if (iOurPower < (3 * iEnemyPower))
							{
								int iPathTurns;
								if (m_unit->generatePath(pLoopCity->plot(), MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns))
								{
									iValue *= 10 * pLoopCity->AI_cityThreat();

									iValue += 20 * GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_ASSAULT, m_unit->getGroup());

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
										pBestPlot = (bCityDanger ? m_unit->getPathEndTurnPlot() : pLoopCity->plot());
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
			if (GET_TEAM(m_unit->getTeam()).isAVassal())
			{
				PROFILE("AI_assaultSeaReinforce.Vassal");

				TeamTypes eMasterTeam = NO_TEAM;

				for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
				{
					if (GET_TEAM(m_unit->getTeam()).isVassal((TeamTypes)iI))
					{
						eMasterTeam = (TeamTypes)iI;
					}
				}

				if ((eMasterTeam != NO_TEAM) && GET_TEAM(m_unit->getTeam()).isOpenBorders(eMasterTeam))
				{
					for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).getTeam() == eMasterTeam)
						{
							foreach_(CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
							{
								if (pLoopCity->area() != m_unit->area())
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
											int iOurPower = std::max(1, pLoopCity->area()->getPower(m_unit->getOwner()));
											iOurPower += GET_TEAM(eMasterTeam).countPowerByArea(pLoopCity->area());
											// Enemy power includes barb power
											int iEnemyPower = GET_TEAM(eMasterTeam).countEnemyPowerByArea(pLoopCity->area());

											// Don't send troops to m_unit->areas we are dominating already
											// Don't require presence of enemy cities, just a dangerous force
											if (iOurPower < (2 * iEnemyPower))
											{
												int iPathTurns;
												if (m_unit->generatePath(pLoopCity->plot(), MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns))
												{
													iValue *= pLoopCity->AI_cityThreat();

													iValue += 10 * GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_ASSAULT, m_unit->getGroup());

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
														endTurnPlot = m_unit->getPathEndTurnPlot();

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
		FAssert(!(pBestPlot->isImpassable(m_unit->getTeam())));

		// Cancel missions of all those coming to join departing transport
		const CvPlayer& kPlayer = GET_PLAYER(m_unit->getOwner());

		foreach_(CvSelectionGroup * pLoopGroup, kPlayer.groups())
		{
			if (pLoopGroup != m_unit->getGroup())
			{
				if (pLoopGroup->AI_getMissionAIType() == MISSIONAI_GROUP && pLoopGroup->getHeadUnitAI() == getUnitAIType())
				{
					const CvUnit* pMissionUnit = pLoopGroup->AI_getMissionAIUnit();

					if (pMissionUnit != NULL && pMissionUnit->getGroup() == m_unit->getGroup())
					{
						pLoopGroup->clearMissionQueue();
					}
				}
			}
		}

		if ((pBestPlot == pBestAssaultPlot) || (stepDistance(pBestPlot->getX(), pBestPlot->getY(), pBestAssaultPlot->getX(), pBestAssaultPlot->getY()) == 1))
		{
			if (m_unit->atPlot(pBestAssaultPlot))
			{
				m_unit->getGroup()->unloadAll(); // XXX is m_unit dangerous (not pushing a mission...) XXX air units?
				return true;
			}
			else
			{
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestAssaultPlot->getX(), pBestAssaultPlot->getY(), MOVE_AVOID_ENEMY_WEIGHT_3, false, false, MISSIONAI_ASSAULT, pBestAssaultPlot);
			}
		}
		else
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_WEIGHT_3, false, false, MISSIONAI_ASSAULT, pBestAssaultPlot);
		}
	}

	return false;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					   END												  */
/************************************************************************************************/



// Returns true if a mission was pushed...
bool CvUnitDefaultAI::settlerSeaTransport()
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

	FAssert(m_unit->hasCargo());
	FAssert(m_unit->getUnitAICargo(UNITAI_SETTLE) > 0);

	if (!m_unit->canCargoAllMove())
	{
		return false;
	}

	//New logic should allow some new tricks like
	//unloading settlers when a better site opens up locally
	//and delivering settlers
	//to inland sites

	const CvArea* pWaterArea = m_unit->plot()->waterArea();
	FAssertMsg(pWaterArea != NULL, "Ship out of water?");

	CvUnit* pSettlerUnit = NULL;
	const CvPlot* pPlot = m_unit->plot();
	foreach_(CvUnit * pLoopUnit, pPlot->units())
	{
		if (pLoopUnit->getTransportUnit() == m_unit)
		{
			if (pLoopUnit->AI()->getUnitAIType() == UNITAI_SETTLE)
			{
				pSettlerUnit = pLoopUnit;
				break;
			}
		}
	}

	FAssert(pSettlerUnit != NULL);

	int iAreaBestFoundValue = 0;
	CvPlot* pAreaBestPlot = NULL;

	int iOtherAreaBestFoundValue = 0;
	CvPlot* pOtherAreaBestPlot = NULL;

	for (iI = 0; iI < GET_PLAYER(m_unit->getOwner()).AI_getNumCitySites(); iI++)
	{
		CvPlot* pCitySitePlot = GET_PLAYER(m_unit->getOwner()).AI_getCitySite(iI);
		if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pCitySitePlot, MISSIONAI_FOUND, m_unit->getGroup()) == 0)
		{
			iValue = pCitySitePlot->getFoundValue(m_unit->getOwner());
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD					  01/13/09								jdog5000	  */
			/*																							  */
			/* Settler AI																				   */
			/************************************************************************************************/
			/* original bts code
						if (pCitySitePlot->getArea() == m_unit->getArea())
						{
							if (iValue > iAreaBestFoundValue)
							{
			*/
			// Only count city sites we can get to
			if (pCitySitePlot->getArea() == m_unit->getArea() && pSettlerUnit->generatePath(pCitySitePlot, MOVE_SAFE_TERRITORY, true))
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
		m_unit->getGroup()->unloadAll();
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return true;
	}

	iBestValue = 0;
	pBestPlot = NULL;
	pBestFoundPlot = NULL;

	for (iI = 0; iI < GET_PLAYER(m_unit->getOwner()).AI_getNumCitySites(); iI++)
	{
		CvPlot* pCitySitePlot = GET_PLAYER(m_unit->getOwner()).AI_getCitySite(iI);
		if (!(pCitySitePlot->isVisibleEnemyUnit(m_unit)))
		{
			if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pCitySitePlot, MISSIONAI_FOUND, m_unit->getGroup(), 4) == 0)
			{
				int iPathTurns;
				// BBAI TODO: Nearby plots too if much shorter (settler walk from there)
				// also, if plots are in m_unit->area player already has cities, then may not be coastal ... (see Earth 1000 AD map for Inca)
				if (m_unit->generatePath(pCitySitePlot, 0, true, &iPathTurns))
				{
					iValue = pCitySitePlot->getFoundValue(m_unit->getOwner());
					iValue *= 1000;
					iValue /= (2 + iPathTurns);

					if (iValue > iBestValue)
					{
						endTurnPlot = m_unit->getPathEndTurnPlot();

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
		FAssert(!(pBestPlot->isImpassable(m_unit->getTeam()))); // added m_unit->getTeam()

		if ((pBestPlot == pBestFoundPlot) || (stepDistance(pBestPlot->getX(), pBestPlot->getY(), pBestFoundPlot->getX(), pBestFoundPlot->getY()) == 1))
		{
			if (m_unit->atPlot(pBestFoundPlot))
			{
				m_unit->unloadAll(); // XXX is m_unit dangerous (not pushing a mission...) XXX air units?
				return true;
			}
			else
			{
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestFoundPlot->getX(), pBestFoundPlot->getY(), 0, false, false, MISSIONAI_FOUND, pBestFoundPlot);
			}
		}
		else
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_FOUND, pBestFoundPlot);
		}
	}

	//Try original logic
	//(sometimes new logic breaks)
	pPlot = m_unit->plot();

	iBestValue = 0;
	pBestPlot = NULL;
	pBestFoundPlot = NULL;

	int iMinFoundValue = GET_PLAYER(m_unit->getOwner()).AI_getMinFoundValue();

	for (iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->isCoastalLand())
		{
			iValue = pLoopPlot->getFoundValue(m_unit->getOwner());

			if ((iValue > iBestValue) && (iValue >= iMinFoundValue))
			{
				bValid = false;

				foreach_(const CvUnit * pLoopUnit, pPlot->units())
				{
					if (pLoopUnit->getTransportUnit() == m_unit)
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
					if (!pLoopPlot->isVisible(m_unit->getTeam(), false) || !pLoopPlot->isVisibleEnemyUnit(m_unit))
					{
						if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_FOUND, m_unit->getGroup(), 4) == 0)
						{
							if (m_unit->generatePath(pLoopPlot, 0, true))
							{
								iBestValue = iValue;
								pBestPlot = m_unit->getPathEndTurnPlot();
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
		FAssert(!(pBestPlot->isImpassable(m_unit->getTeam()))); // added m_unit->getTeam()

		if ((pBestPlot == pBestFoundPlot) || (stepDistance(pBestPlot->getX(), pBestPlot->getY(), pBestFoundPlot->getX(), pBestFoundPlot->getY()) == 1))
		{
			if (m_unit->atPlot(pBestFoundPlot))
			{
				m_unit->unloadAll(); // XXX is m_unit dangerous (not pushing a mission...) XXX air units?
				return true;
			}
			else
			{
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestFoundPlot->getX(), pBestFoundPlot->getY(), 0, false, false, MISSIONAI_FOUND, pBestFoundPlot);
			}
		}
		else
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_FOUND, pBestFoundPlot);
		}
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::settlerSeaFerry()
{
	PROFILE_FUNC();

	FAssert(m_unit->hasCargo());
	FAssert(m_unit->getUnitAICargo(UNITAI_WORKER) > 0);

	if (!m_unit->canCargoAllMove())
	{
		return false;
	}

	CvArea* pWaterArea = m_unit->plot()->waterArea();
	FAssertMsg(pWaterArea != NULL, "Ship out of water?");

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* endTurnPlot = NULL;

	foreach_(const CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
	{
		int iValue = pLoopCity->AI_getWorkersNeeded();
		if (iValue > 0)
		{
			iValue -= GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_FOUND, m_unit->getGroup());
			if (iValue > 0)
			{
				int iPathTurns;
				if (m_unit->generatePath(pLoopCity->plot(), 0, true, &iPathTurns))
				{
					iValue += std::max(0, GET_PLAYER(m_unit->getOwner()).AI_neededWorkers(pLoopCity->area()) - GET_PLAYER(m_unit->getOwner()).AI_totalAreaUnitAIs(pLoopCity->area(), UNITAI_WORKER));
					iValue *= 1000;
					iValue /= 4 + iPathTurns;
					if (m_unit->atPlot(pLoopCity->plot()))
					{
						iValue += 100;
					}
					else
					{
						iValue += GC.getGame().getSorenRandNum(100, "AI settler sea ferry");
					}
					if (iValue > iBestValue)
					{
						endTurnPlot = m_unit->getPathEndTurnPlot();

						if (endTurnPlot == pLoopCity->plot() || !exposedToDanger(endTurnPlot, 80))
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
		if (m_unit->atPlot(pBestPlot))
		{
			m_unit->unloadAll(); // XXX is m_unit dangerous (not pushing a mission...) XXX air units?
			return true;
		}
		else
		{
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_FOUND, pBestPlot);
		}
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::specialSeaTransportMissionary()
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

	FAssert(m_unit->hasCargo());
	FAssert(m_unit->getUnitAICargo(UNITAI_MISSIONARY) > 0);

	if (!m_unit->canCargoAllMove())
	{
		return false;
	}

	pPlot = m_unit->plot();

	pMissionaryUnit = NULL;

	foreach_(CvUnit * pLoopUnit, pPlot->units())
	{
		if (pLoopUnit->getTransportUnit() == m_unit)
		{
			if (pLoopUnit->AI()->getUnitAIType() == UNITAI_MISSIONARY)
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
	CvReachablePlotSet plotSet(m_unit->getGroup(), MOVE_ALLOW_ADJACENT_COASTAL, MAX_INT);

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
						if (GET_PLAYER(m_unit->getOwner()).getStateReligion() == ((ReligionTypes)iJ))
						{
							iValue += 3;
						}

						if (GET_PLAYER(m_unit->getOwner()).hasHolyCity((ReligionTypes)iJ))
						{
							iValue++;
						}
					}
				}

				for (iJ = 0; iJ < GC.getNumCorporationInfos(); iJ++)
				{
					if (pMissionaryUnit->canSpreadCorporation(pLoopPlot, ((CorporationTypes)iJ)))
					{
						if (GET_PLAYER(m_unit->getOwner()).hasHeadquarters((CorporationTypes)iJ))
						{
							iCorpValue += 3;
						}
					}
				}

				if (iValue > 0)
				{
					if (!(pLoopPlot->isVisibleEnemyUnit(m_unit)))
					{
						if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_SPREAD, m_unit->getGroup()) == 0)
						{
							if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns))
							{
								iValue *= pCity->getPopulation();

								if (pCity->getOwner() == m_unit->getOwner())
								{
									iValue *= 4;
								}
								else if (pCity->getTeam() == m_unit->getTeam())
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
									pBestPlot = m_unit->getPathEndTurnPlot();
									pBestSpreadPlot = pLoopPlot;
									bExecutive = false;
								}
							}
						}
					}
				}

				if (iCorpValue > 0)
				{
					if (!(pLoopPlot->isVisibleEnemyUnit(m_unit)))
					{
						if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_SPREAD_CORPORATION, m_unit->getGroup()) == 0)
						{
							if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns))
							{
								iCorpValue *= pCity->getPopulation();

								FAssert(iPathTurns > 0);

								if (iPathTurns == 1)
								{
									/************************************************************************************************/
									/* UNOFFICIAL_PATCH					   02/22/10								jdog5000	  */
									/*																							  */
									/* Bugfix																					   */
									/************************************************************************************************/
									/* original bts code
																		iValue *= 2;
									*/
									iCorpValue *= 2;
									/************************************************************************************************/
									/* UNOFFICIAL_PATCH						END												  */
									/************************************************************************************************/
								}

								iCorpValue *= 1000;

								iCorpValue /= (iPathTurns + 1);

								if (iCorpValue > iBestValue)
								{
									iBestValue = iCorpValue;
									pBestPlot = m_unit->getPathEndTurnPlot();
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
		FAssert(!(pBestPlot->isImpassable(m_unit->getTeam())) || m_unit->canMoveImpassable()); // added m_unit->getTeam()

		if ((pBestPlot == pBestSpreadPlot) || (stepDistance(pBestPlot->getX(), pBestPlot->getY(), pBestSpreadPlot->getX(), pBestSpreadPlot->getY()) == 1))
		{
			if (m_unit->atPlot(pBestSpreadPlot))
			{
				m_unit->unloadAll(); // XXX is m_unit dangerous (not pushing a mission...) XXX air units?
				return true;
			}
			else
			{
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestSpreadPlot->getX(), pBestSpreadPlot->getY(), 0, false, false, bExecutive ? MISSIONAI_SPREAD_CORPORATION : MISSIONAI_SPREAD, pBestSpreadPlot);
			}
		}
		else
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, bExecutive ? MISSIONAI_SPREAD_CORPORATION : MISSIONAI_SPREAD, pBestSpreadPlot);
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::specialSeaTransportSpy()
{
	PROFILE_FUNC();

	FAssert(m_unit->hasCargo());
	FAssert(m_unit->getUnitAICargo(UNITAI_SPY) > 0);

	if (!m_unit->canCargoAllMove())
	{
		return false;
	}
	const CvPlayerAI& owner = GET_PLAYER(m_unit->getOwner());
	int iBestValue = 0;
	PlayerTypes eBestPlayer = NO_PLAYER;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(m_unit->getTeam(), false)
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

	CvReachablePlotSet plotSet(m_unit->getGroup(), MOVE_ALLOW_ADJACENT_COASTAL, MAX_INT);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		CvPlot* pLoopPlot = itr.plot();
		if (pLoopPlot->isCoastalLand() && pLoopPlot->getOwner() == eBestPlayer)
		{
			int iValue = 1000 * pLoopPlot->area()->getCitiesPerPlayer(eBestPlayer);

			if (iValue > iBestValue && owner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_ATTACK_SPY, m_unit->getGroup(), 4) == 0)
			{
				int iPathTurns;
				if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns))
				{
					iValue /= iPathTurns + 1;

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = m_unit->getPathEndTurnPlot();
						pBestSpyPlot = pLoopPlot;
					}
				}
			}
		}
	}
	if (pBestPlot != NULL && pBestSpyPlot != NULL)
	{
		FAssert(!pBestPlot->isImpassable(m_unit->getTeam()));

		if (pBestPlot == pBestSpyPlot || 1 == stepDistance(pBestPlot->getX(), pBestPlot->getY(), pBestSpyPlot->getX(), pBestSpyPlot->getY()))
		{
			if (m_unit->atPlot(pBestSpyPlot))
			{
				m_unit->unloadAll(); // XXX is m_unit dangerous (not pushing a mission...) XXX air units?
				return true;
			}
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestSpyPlot->getX(), pBestSpyPlot->getY(), 0, false, false, MISSIONAI_ATTACK_SPY, pBestSpyPlot);
		}
		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_ATTACK_SPY, pBestSpyPlot);
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::carrierSeaTransport()
{
	PROFILE_FUNC();

	int iPathTurns;

	int iMaxAirRange = 0;

	std::vector<CvUnit*> aCargoUnits;
	m_unit->getCargoUnits(aCargoUnits);
	if (!aCargoUnits.empty())
	{
		m_unit->validateCargoUnits();
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

	CvReachablePlotSet plotSet(m_unit->getGroup(), 0, MAX_INT);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		const CvPlot* pLoopPlot = itr.plot();

		//if (plotValid(pLoopPlot))
		{
			if (pLoopPlot->isWater() && pLoopPlot->isAdjacentToLand())
			{
				if (!pLoopPlot->isVisible(m_unit->getTeam(), false) || !pLoopPlot->isVisibleEnemyUnit(m_unit))
				{
					int iValue = 0;

					foreach_(const CvPlot * pLoopPlotAir, pLoopPlot->rect(iMaxAirRange, iMaxAirRange))
					{
						if (plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pLoopPlotAir->getX(), pLoopPlotAir->getY()) <= iMaxAirRange)
						{
							if (!(pLoopPlotAir->isNPC()))
							{
								if (m_unit->potentialWarAction(pLoopPlotAir))
								{
									if (pLoopPlotAir->isCity())
									{
										iValue += 3;

										// BBAI: Support invasions
										iValue += (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlotAir, MISSIONAI_ASSAULT, m_unit->getGroup(), 2) * 6);
									}

									if (pLoopPlotAir->getImprovementType() != NO_IMPROVEMENT)
									{
										iValue += 2;
									}

									if (plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pLoopPlotAir->getX(), pLoopPlotAir->getY()) <= iMaxAirRange / 2)
									{
										// BBAI: Support/air defense for land troops
										iValue += pLoopPlotAir->plotCount(PUF_canDefend, -1, -1, NULL, m_unit->getOwner());
									}
								}
							}
						}
					}

					if (iValue > 0)
					{
						iValue *= 1000;

						foreach_(const CvPlot * pDirectionPlot, pLoopPlot->adjacent())
						{
							if (pDirectionPlot->isCity() && m_unit->isEnemy(pDirectionPlot->getTeam(), pLoopPlot))
							{
								iValue /= 2;
								break;
							}
						}

						if (iValue > iBestValue)
						{
							const bool bStealth = (m_unit->getInvisibleType() != NO_INVISIBLE);
							if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_CARRIER, m_unit->getGroup(), bStealth ? 5 : 3) <= (bStealth ? 0 : 3))
							{
								if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns))
								{
									iValue /= (iPathTurns + 1);

									if (iValue > iBestValue)
									{
										const CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();

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
		if (m_unit->atPlot(pBestCarrierPlot))
		{
			if (m_unit->getGroup()->hasCargo())
			{
				const CvPlot* pPlot = m_unit->plot();

				const int iNumUnits = pPlot->getNumUnits();

				for (int i = 0; i < iNumUnits; ++i)
				{
					bool bDone = true;
					foreach_(const CvUnit * pCargoUnit, pPlot->units())
					{
						if (pCargoUnit->isCargo())
						{
							FAssert(pCargoUnit->getTransportUnit() != NULL);
							if (pCargoUnit->getOwner() == m_unit->getOwner() && (pCargoUnit->getTransportUnit()->getGroup() == m_unit->getGroup()) && (pCargoUnit->getDomainType() == DOMAIN_AIR))
							{
								if (pCargoUnit->canMove() && pCargoUnit->isGroupHead())
								{
									// careful, m_unit might kill the cargo group
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

			if (m_unit->canPlunder(pBestCarrierPlot))
			{
				m_unit->getGroup()->pushMission(MISSION_PLUNDER, -1, -1, 0, false, false, MISSIONAI_CARRIER, pBestCarrierPlot);
			}
			else
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP);
			}
			return true;
		}
		else
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_CARRIER, pBestCarrierPlot);
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::connectPlot(CvPlot* pPlot, int iRange)
{
	PROFILE_FUNC();

	FAssert(m_unit->canBuildRoute());

	if ((m_unit->getDomainType() == DOMAIN_LAND) && (pPlot->area() != m_unit->area()) && !(m_unit->getGroup()->canMoveAllTerrain()))
	{
		return false;
	}
	int iBasePathFlags = MOVE_SAFE_TERRITORY | MOVE_AVOID_ENEMY_UNITS | MOVE_IGNORE_DANGER | MOVE_RECONSIDER_ON_LEAVING_OWNED;
	int iPathTurns = 0;

	if (!pPlot->isVisible(m_unit->getTeam(), false) || !pPlot->isVisibleEnemyUnit(m_unit)
	&& GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pPlot, MISSIONAI_BUILD, m_unit->getGroup(), iRange) == 0
	&& generateSafePathforVulnerable(pPlot, &iPathTurns))
	{
		bool bHasPossibleTargetCity = false;

		foreach_(const CvCity * cityX, GET_PLAYER(m_unit->getOwner()).cities())
		{
			if (pPlot->isConnectedTo(cityX))
			{
				continue;
			}
			if (!bHasPossibleTargetCity && !cityX->plot()->isVisibleEnemyUnit(m_unit))
			{
				bHasPossibleTargetCity = true;
			}
			FAssertMsg(pPlot->getPlotCity() != cityX, "pPlot->getPlotCity() is not expected to be equal with cityX");

			if (m_unit->plot()->getPlotGroup(m_unit->getOwner()) == cityX->plot()->getPlotGroup(m_unit->getOwner()))
			{
				PROFILE("CvUnitDefaultAI::connectPlot.StrangeCase");

				if (generateSafePathforVulnerable(pPlot, &iPathTurns))
				{
					return m_unit->getGroup()->pushMissionInternal(MISSION_ROUTE_TO, pPlot->getX(), pPlot->getY(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_BUILD, cityX->plot());
				}
				m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
				return true;
			}
		}

		if (!bHasPossibleTargetCity)
		{
			return false;
		}
		CvReachablePlotSet plotSet(m_unit->getGroup(), iBasePathFlags);
		plotSet.Populate(MAX_INT);

		foreach_(const CvCity * cityX, GET_PLAYER(m_unit->getOwner()).cities())
		{
			if (pPlot->isConnectedTo(cityX) || plotSet.find(cityX->plot()) == plotSet.end())
			{
				continue;
			}
			FAssertMsg(pPlot->getPlotCity() != cityX, "pPlot->getPlotCity() is not expected to be equal with cityX");

			if (!(cityX->plot()->isVisibleEnemyUnit(m_unit)))
			{
				if (generateSafePathforVulnerable(cityX->plot(), &iPathTurns))
				{
					if (m_unit->atPlot(pPlot)) // need to test before moving...
					{
						return m_unit->getGroup()->pushMissionInternal(MISSION_ROUTE_TO, cityX->getX(), cityX->getY(), MOVE_SAFE_TERRITORY | MOVE_WITH_CAUTION, false, false, MISSIONAI_BUILD, pPlot);
					}

					if (generateSafePathforVulnerable(cityX->plot(), &iPathTurns))
					{
						return m_unit->getGroup()->pushMissionInternal(MISSION_ROUTE_TO, cityX->getX(), cityX->getY(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_BUILD, cityX->plot());
					}
					m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
					return true;
				}
			}

		}
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::improveCity(CvCity* pCity)
{
	PROFILE_FUNC();

	CvPlot* pBestPlot;
	BuildTypes eBestBuild;

	if (bestCityBuild(pCity, &pBestPlot, &eBestBuild, NULL, m_unit))
	{
		FAssertMsg(pBestPlot != NULL, "BestPlot is not assigned a valid value");
		FASSERT_BOUNDS(0, GC.getNumBuildInfos(), eBestBuild);

		MissionTypes eMission;
		if ((m_unit->plot()->getWorkingCity() != pCity) || (GC.getBuildInfo(eBestBuild).getRoute() != NO_ROUTE))
		{
			eMission = MISSION_ROUTE_TO;
		}
		else
		{
			int iPathTurns;

			eMission = MISSION_MOVE_TO;
			if (NULL != pBestPlot && m_unit->generatePath(pBestPlot, MOVE_WITH_CAUTION, false, &iPathTurns) && (iPathTurns == 1) && (m_unit->getPathMovementRemaining() == 0))
			{
				if (pBestPlot->getRouteType() != NO_ROUTE)
				{
					eMission = MISSION_ROUTE_TO;
				}
			}
			else if (m_unit->plot()->getRouteType() == NO_ROUTE)
			{
				int iPlotMoveCost = GC.getTerrainInfo(m_unit->plot()->getTerrainType()).getMovementCost();

				if (m_unit->plot()->getFeatureType() != NO_FEATURE)
				{
					iPlotMoveCost += GC.getFeatureInfo(m_unit->plot()->getFeatureType()).getMovementCost();
				}

				if (m_unit->plot()->isHills())
				{
					iPlotMoveCost += GC.getHILLS_EXTRA_MOVEMENT();
				}

				if (m_unit->plot()->isAsPeak())
				{
					if (!GET_TEAM(m_unit->getTeam()).isMoveFastPeaks())
					{
						iPlotMoveCost += GC.getPEAK_EXTRA_MOVEMENT();
					}
					else
					{
						iPlotMoveCost += 1;
					}
				}

				if (iPlotMoveCost > 1)
				{
					eMission = MISSION_ROUTE_TO;
				}
			}
			if (m_unit->getGroup()->pushMissionInternal(eMission, pBestPlot->getX(), pBestPlot->getY(), m_unit->isHuman() ? 0 : MOVE_WITH_CAUTION, false, false, MISSIONAI_BUILD, pBestPlot))
			{
				m_unit->getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, (m_unit->getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);

				return true;
			}
		}

		eBestBuild = betterPlotBuild(pBestPlot, eBestBuild);

		if (m_unit->getGroup()->pushMissionInternal(eMission, pBestPlot->getX(), pBestPlot->getY(), m_unit->isHuman() ? 0 : MOVE_WITH_CAUTION, false, false, MISSIONAI_BUILD, pBestPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, (m_unit->getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);

			return true;
		}
	}

	return false;
}

bool CvUnitDefaultAI::improveLocalPlot(int iRange, const CvCity* pIgnoreCity)
{
	const CvPlayerAI& kOwner = GET_PLAYER(m_unit->getOwner());
	const ImprovementTypes eRuins = GC.getIMPROVEMENT_CITY_RUINS();
	const bool bSafeAutomation = kOwner.isOption(PLAYEROPTION_SAFE_AUTOMATION);

	int iPathTurns;
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	bool plotIsInCity = false;
	BuildTypes eBestBuild = NO_BUILD;

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iRange, iRange))
	{
		if (pLoopPlot->isCityRadius()
		&& (!bSafeAutomation || pLoopPlot->getImprovementType() == NO_IMPROVEMENT || pLoopPlot->getImprovementType() == eRuins))
		{
			CvCity* pCity = pLoopPlot->getPlotCity();
			if (NULL != pCity && pCity->getOwner() == m_unit->getOwner() && (NULL == pIgnoreCity || pCity != pIgnoreCity) && plotValid(pLoopPlot))
			{
				plotIsInCity = true;
				const int iIndex = pCity->getCityPlotIndex(pLoopPlot);
				const BuildTypes bestBuildForCurrentCity = pCity->AI_getBestBuild(iIndex);
				const ImprovementTypes currentImprovementOnPlot = pLoopPlot->getImprovementType();

				if (iIndex != CITY_HOME_PLOT && bestBuildForCurrentCity != NO_BUILD
				&& (currentImprovementOnPlot == NO_IMPROVEMENT || GC.getBuildInfo(bestBuildForCurrentCity).getImprovement() != NO_IMPROVEMENT)
				&& (NULL == pIgnoreCity || pCity->AI_getWorkersNeeded() > 0 && pCity->getNumWorkers() < 1 + pCity->AI_getWorkersNeeded() * 2 / 3)
				&& m_unit->canBuild(pLoopPlot, bestBuildForCurrentCity)
				&& m_unit->generatePath(pLoopPlot, m_unit->isHuman() ? 0 : MOVE_IGNORE_DANGER, true, &iPathTurns))
				{
					int iValue = pCity->AI_getBestBuildValue(iIndex);
					int iMaxWorkers = 1;
					if (m_unit->plot() == pLoopPlot)
					{
						iValue *= 3;
						iValue /= 2;
					}
					else if (m_unit->getPathMovementRemaining() == 0)
					{
						iPathTurns++;
					}
					else if (iPathTurns <= 1)
					{
						iMaxWorkers = calculatePlotWorkersNeeded(pLoopPlot, bestBuildForCurrentCity);
					}

					if (kOwner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD, m_unit->getGroup()) < iMaxWorkers)
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

		if (m_unit->generatePath(pBestPlot, 0, true, &iPathTurns) && iPathTurns == 1 && m_unit->getPathMovementRemaining() == 0)
		{
			if (pBestPlot->getRouteType() != NO_ROUTE)
			{
				eMission = MISSION_ROUTE_TO;
			}
		}
		else if (m_unit->plot()->getRouteType() == NO_ROUTE)
		{
			int iPlotMoveCost = GC.getTerrainInfo(m_unit->plot()->getTerrainType()).getMovementCost();

			if (m_unit->plot()->getFeatureType() != NO_FEATURE)
			{
				iPlotMoveCost += GC.getFeatureInfo(m_unit->plot()->getFeatureType()).getMovementCost();
			}
			if (m_unit->plot()->isHills())
			{
				iPlotMoveCost += GC.getHILLS_EXTRA_MOVEMENT();
			}
			if (m_unit->plot()->isAsPeak())
			{
				if (!GET_TEAM(m_unit->getTeam()).isMoveFastPeaks())
				{
					iPlotMoveCost += GC.getPEAK_EXTRA_MOVEMENT();
				}
				else iPlotMoveCost += 1;
			}
			if (iPlotMoveCost > 1)
			{
				eMission = MISSION_ROUTE_TO;
			}
		}
		// m_unit _should_ eliminate forts within city limits...
		if (!plotIsInCity && eBestBuild == NO_BUILD)
		{
			// betterplotbuild needs to be refactored...
			eBestBuild = betterPlotBuild(pBestPlot, eBestBuild);
		}

		if (m_unit->getGroup()->pushMissionInternal(eMission, pBestPlot->getX(), pBestPlot->getY(), (m_unit->isHuman() ? 0 : MOVE_WITH_CAUTION), false, false, MISSIONAI_BUILD, pBestPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, (m_unit->getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);
			return true;
		}
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::nextCityToImprove(CvCity* pCity)
{
	PROFILE_FUNC();

	int iBasePathFlags = MOVE_SAFE_TERRITORY | MOVE_AVOID_ENEMY_UNITS | (m_unit->isHuman() ? MOVE_OUR_TERRITORY : MOVE_IGNORE_DANGER | MOVE_RECONSIDER_ON_LEAVING_OWNED);

	int iBestValue = 0;
	BuildTypes eBestBuild = NO_BUILD;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
	{
		if (pLoopCity != pCity)
		{
			int iWorkersNeeded = pLoopCity->AI_getWorkersNeeded();
			int iWorkersHave = pLoopCity->getNumWorkers();

			int iValue = 100 * std::max(0, iWorkersNeeded - iWorkersHave) + 10 * iWorkersNeeded;

			iValue *= (iWorkersNeeded + 1);
			iValue /= (iWorkersHave + 1);

			if (iValue > 0)
			{
				CvPlot* pPlot;
				BuildTypes eBuild;
				if (bestCityBuild(pLoopCity, &pPlot, &eBuild, NULL, m_unit))
				{
					FAssert(pPlot != NULL);
					FAssert(eBuild != NO_BUILD);

					if (plotValid(pPlot))
					{
						iValue *= 1000;

						if (pLoopCity->isCapital())
						{
							iValue *= 2;
						}

						if (iValue > iBestValue)
						{
							PROFILE("CvUnitDefaultAI::nextCityToImprove.Pathing");
							int iPathTurns;
							if (m_unit->generatePath(pPlot, iBasePathFlags, true, &iPathTurns))
							{
								PROFILE("CvUnitDefaultAI::nextCityToImprove.Pathed");
								iValue /= (iPathTurns + 1);

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									eBestBuild = eBuild;
									pBestPlot = pPlot;
									//CvPlot* pEndTurnPlot = m_unit->getPathEndTurnPlot();
									FAssert(!m_unit->atPlot(pBestPlot) || NULL == pCity || pCity->AI_getWorkersNeeded() == 0 || pCity->getNumWorkers() > pCity->AI_getWorkersNeeded() + 1);
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

		eBestBuild = betterPlotBuild(pBestPlot, eBestBuild);

		if (m_unit->getGroup()->pushMissionInternal(MISSION_ROUTE_TO, pBestPlot->getX(), pBestPlot->getY(), ((m_unit->isHuman() ? 0 : MOVE_WITH_CAUTION) | MOVE_SAFE_TERRITORY), false, false, MISSIONAI_BUILD, pBestPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, (m_unit->getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);
			return true;
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::nextCityToImproveAirlift()
{
	PROFILE_FUNC();

	if (m_unit->getGroup()->getNumUnits() > 1)
	{
		return false;
	}
	const CvCity* pCity = m_unit->plot()->getPlotCity();

	if (pCity == NULL || pCity->getMaxAirlift() == 0)
	{
		return false;
	}
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
	{
		if (pLoopCity != pCity && m_unit->canAirliftAt(pCity->plot(), pLoopCity->getX(), pLoopCity->getY()))
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
		m_unit->getGroup()->pushMission(MISSION_AIRLIFT, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::irrigateTerritory()
{
	PROFILE_FUNC();

	const CvPlayerAI& kOwner = GET_PLAYER(m_unit->getOwner());
	const bool bLeaveForests = kOwner.isOption(PLAYEROPTION_LEAVE_FORESTS);
	const bool bSafeAutomation = kOwner.isOption(PLAYEROPTION_SAFE_AUTOMATION);
	const ImprovementTypes eRuins = GC.getIMPROVEMENT_CITY_RUINS();

	BuildTypes eBestBuild = NO_BUILD;
	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (plotValid(pLoopPlot) && pLoopPlot->area() == m_unit->area() && pLoopPlot->getOwner() == m_unit->getOwner() && pLoopPlot->getWorkingCity() == NULL)
		{
			const ImprovementTypes eImprovement = pLoopPlot->getImprovementType();

			if (eImprovement == NO_IMPROVEMENT
			|| (!bSafeAutomation || eImprovement == eRuins) && !GC.getImprovementInfo(eImprovement).isCarriesIrrigation())
			{
				const BonusTypes eNonObsoleteBonus = pLoopPlot->getNonObsoleteBonusType(m_unit->getTeam());

				if ((eImprovement == NO_IMPROVEMENT || eNonObsoleteBonus == NO_BONUS || !GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eNonObsoleteBonus))
				&& pLoopPlot->isIrrigationAvailable(true))
				{
					int iBestTempBuildValue = MAX_INT;
					BuildTypes eBestTempBuild = NO_BUILD;

					for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
					{
						const BuildTypes eBuild = (BuildTypes)iJ;

						if (GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT
						&& GC.getImprovementInfo(GC.getBuildInfo(eBuild).getImprovement()).isCarriesIrrigation()
						&& m_unit->canBuild(pLoopPlot, eBuild))
						{
							const int iValue = 10000 / (GC.getBuildInfo(eBuild).getTime() + 1);

							if (iValue < iBestTempBuildValue)
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

						if (bValid && (!pLoopPlot->isVisible(m_unit->getTeam(), false) || !pLoopPlot->isVisibleEnemyUnit(m_unit))
						&& kOwner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD, m_unit->getGroup(), 1) == 0)
						{
							int iPathTurns;
							// XXX should m_unit actually be at the top of the loop? (with saved paths and all...)
							if (m_unit->generatePath(pLoopPlot, m_unit->isHuman() ? 0 : MOVE_IGNORE_DANGER, true, &iPathTurns))
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

		if (m_unit->getGroup()->pushMissionInternal(MISSION_ROUTE_TO, pBestPlot->getX(), pBestPlot->getY(), (m_unit->isHuman() ? 0 : MOVE_WITH_CAUTION), false, false, MISSIONAI_BUILD, pBestPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, (m_unit->getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);
			return true;
		}
	}
	return false;
}


bool CvUnitDefaultAI::fortTerritory(bool bCanal, bool bAirbase)
{
	PROFILE_FUNC();

	//if (StrategicForts())
	//{
	//	return true;
	//}
	BuildTypes eBestBuild = NO_BUILD;
	const CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	int iMaxDistFromBorder = -1;

	if (!m_unit->isHuman() || (m_unit->getGroup()->getNumUnits() > 1 && m_unit->getGroup()->canDefend()))
	{
		iMaxDistFromBorder = GC.getAI_WORKER_MAX_DISTANCE_FROM_CITY_OUT_BORDERS();
	}

	const PlayerTypes ePlayer = m_unit->getOwner();
	const CvPlayerAI& kOwner = GET_PLAYER(ePlayer);
	const bool bLeaveForests = kOwner.isOption(PLAYEROPTION_LEAVE_FORESTS);
	const int iBasePathFlags = MOVE_SAFE_TERRITORY | MOVE_AVOID_ENEMY_UNITS | (m_unit->isHuman() ? MOVE_OUR_TERRITORY : MOVE_IGNORE_DANGER | MOVE_RECONSIDER_ON_LEAVING_OWNED);

	CvReachablePlotSet plotSet(m_unit->getGroup(), m_unit->isHuman() ? 0 : MOVE_IGNORE_DANGER, MAX_INT);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		const CvPlot* pLoopPlot = itr.plot();

		if (/*AI_plotValid(pLoopPlot) &&*/ pLoopPlot->area() == m_unit->area()
		&& (pLoopPlot->getOwner() == ePlayer || pLoopPlot->getOwner() == NO_PLAYER && pLoopPlot->isRevealed(m_unit->getTeam(), false))
		&& (pLoopPlot->getImprovementType() == NO_IMPROVEMENT || !pLoopPlot->isCityRadius()))
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
				int iBestBuildValue = MAX_INT;
				BuildTypes eBestTempBuild = NO_BUILD;

				for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
				{
					const BuildTypes eBuild = ((BuildTypes)iJ);

					if (GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT)
					{
						const CvImprovementInfo& kImprovement = GC.getImprovementInfo(GC.getBuildInfo(eBuild).getImprovement());

						if ((bCanal && kImprovement.isCanMoveSeaUnits() || kImprovement.isActsAsCity() && kImprovement.getDefenseModifier() > 0)
						&& m_unit->canBuild(pLoopPlot, eBuild))
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

				if (eBestTempBuild != NO_BUILD && (!pLoopPlot->isVisible(m_unit->getTeam(), false) || !pLoopPlot->isVisibleEnemyUnit(m_unit)))
				{
					bool bValid = true;

					if (bLeaveForests && pLoopPlot->getFeatureType() != NO_FEATURE
					&& GC.getBuildInfo(eBestTempBuild).isFeatureRemove(pLoopPlot->getFeatureType())
					&& GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange(YIELD_PRODUCTION) > 0)
					{
						bValid = false;
					}
					if (bValid && kOwner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD, m_unit->getGroup(), 1/*3*/) == 0)
					{
						int iPathTurns;
						if (m_unit->generatePath(pLoopPlot, iBasePathFlags, true, &iPathTurns))
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

		if (m_unit->getGroup()->pushMissionInternal(MISSION_ROUTE_TO, pBestPlot->getX(), pBestPlot->getY(), (m_unit->isHuman() ? 0 : MOVE_WITH_CAUTION), false, false, MISSIONAI_BUILD, pBestPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, (m_unit->getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);
			return true;
		}
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::improveBonus(int iMinValue, CvPlot** ppBestPlot, BuildTypes* peBestBuild, int* piBestValue)
{
	PROFILE_FUNC();

	int iMaxDistFromBorder = -1;

	if (m_unit->getGroup()->getNumUnits() > 1 && m_unit->getGroup()->canDefend())
	{
		iMaxDistFromBorder = GC.getAI_WORKER_MAX_DISTANCE_FROM_CITY_OUT_BORDERS();
	}
	const int iBasePathFlags = MOVE_SAFE_TERRITORY | MOVE_AVOID_ENEMY_UNITS | (m_unit->isHuman() ? MOVE_OUR_TERRITORY : MOVE_IGNORE_DANGER | MOVE_RECONSIDER_ON_LEAVING_OWNED);
	const ImprovementTypes eRuins = GC.getIMPROVEMENT_CITY_RUINS();
	const PlayerTypes ePlayer = m_unit->getOwner();
	const CvPlayerAI& kOwner = GET_PLAYER(ePlayer);
	const bool bLeaveForests = kOwner.isOption(PLAYEROPTION_LEAVE_FORESTS);
	const bool bSafeAutomation = kOwner.isOption(PLAYEROPTION_SAFE_AUTOMATION);
	const bool bCanRoute = m_unit->canBuildRoute();

	int iPathTurns = 0;
	int iBestValue = 0;
	int iBestResourceValue = 0;
	bool bBestBuildIsRoute = false;
	BuildTypes eBestBuild = NO_BUILD;
	CvPlot* pBestPlot = NULL;

	CvReachablePlotSet plotSet(m_unit->getGroup(), iBasePathFlags, -1, true, iMaxDistFromBorder == -1 ? -1 : iMaxDistFromBorder / 2 + 1);
	bool bPlotSetPopulated = false;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (bPlotSetPopulated && plotSet.find(pLoopPlot) == plotSet.end())
		{
			continue;
		}
		PROFILE("CvUnitDefaultAI::improveBonus.ConsiderPlot");
		const PlayerTypes ePlotOwner = pLoopPlot->getOwner();

		if ((ePlotOwner == ePlayer || ePlotOwner == NO_PLAYER && pLoopPlot->isRevealed(m_unit->getTeam(), false)) && plotValid(pLoopPlot)
		&& (pLoopPlot->area() == m_unit->area() || DOMAIN_SEA == m_unit->getDomainType() && pLoopPlot->isWater() && m_unit->plot()->isAdjacentToArea(pLoopPlot->area())))
		{
			BonusTypes eNonObsoleteBonus = pLoopPlot->getNonObsoleteBonusType(m_unit->getTeam());

			if (eNonObsoleteBonus != NO_BONUS)
			{
				const bool bIsConnected = pLoopPlot->isConnectedToCapital(ePlayer);
				// Super Forts - This section makes sure the m_unit->plot is "close enough" to a city to be worth building on
				bool bCloseEnough = false;

				if (ePlotOwner == ePlayer)
				{
					bCloseEnough = true;
				}
				// Automated human workers will not look outside borders to build forts on bonuses the player already has
				else if (!m_unit->isHuman() || !kOwner.hasBonus(eNonObsoleteBonus))
				{
					PROFILE("CvUnitDefaultAI::improveBonus.CheckConnectivity");

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

					if (pNearestCity != NULL && m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns, iMaxDistFromBorder / 2)

					&& plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pNearestCity->getX(), pNearestCity->getY()) * (kOwner.hasBonus(eNonObsoleteBonus) ? 2 : 1) <= iMaxDistFromBorder)
					{
						bCloseEnough = true;
					}
				}

				if ((pLoopPlot->getWorkingCity() != NULL || bIsConnected || bCanRoute) && bCloseEnough && (!pLoopPlot->isVisible(m_unit->getTeam(), false) || !pLoopPlot->isVisibleEnemyUnit(m_unit)))
				{
					PROFILE("CvUnitDefaultAI::improveBonus.CheckImprovement");

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
						//TB Super Forts debug: I wanted to allow a civ to re-evaluate whether a fort should be or continue to be used on a given m_unit->plot as the best way to gather m_unit resource.
						//The way m_unit is programmed they won't remove forts when cities move in and they won't remove forts when war is over and they won't replace a plantation or whatever with a fort if war is afoot.
						bDoImprove = true;
					}
					*/
					else if (bCityRadius && (GC.getImprovementInfo(eImprovement).isActsAsCity() || GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eNonObsoleteBonus)))
					{
						//TB Super Forts debug: I wanted to allow a civ to re-evaluate whether a fort should be or continue to be used on a given m_unit->plot as the best way to gather m_unit resource.
						//The way m_unit is programmed they won't remove forts when cities move in and they won't remove forts when war is over and they won't replace a plantation or whatever with a fort if war is afoot.
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
						PROFILE("CvUnitDefaultAI::improveBonus.CheckBuild");

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

								&& m_unit->canBuild(pLoopPlot, eBuild))
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
					// Super Forts begin *AI_worker* (if not building an improvement and you don't own the m_unit->plot then continue so the AI doesn't consider building a route)
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

						PROFILE("CvUnitDefaultAI::improveBonus.Evaluate");

						//	We use MOVE_IGNORE_DANGER in here so that the worker won't be spooked before it gets a chance
						//	to find a target m_unit->plot (at which poin it will ask for an escort if necessary)
						if (m_unit->generatePath(pLoopPlot, iBasePathFlags, true, &iPathTurns))
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
										iValue += 10 * pLoopPlot->calculateNatureYield((YieldTypes)iK, m_unit->getTeam(), eFeature == NO_FEATURE ? true : GC.getBuildInfo(eBestTempBuild).isFeatureRemove(eFeature));
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
											iCountervalue += 10 * pLoopPlot->calculateNatureYield((YieldTypes)iK, m_unit->getTeam(), eFeature == NO_FEATURE ? true : GC.getBuildInfo(eBestTempBuild).isFeatureRemove(eFeature));
										}
									}
									if (GC.getImprovementInfo(eImprovement).getCulture() > 0 || GC.getImprovementInfo(eImprovement).isActsAsCity())
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
										//Compatibility check - if the game hasn't assigned a current value to the m_unit->plot yet, assign one then recheck.
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
								iMaxWorkers = calculatePlotWorkersNeeded(pLoopPlot, eBestTempBuild);
								if (m_unit->getPathMovementRemaining() == 0)
								{
									iMaxWorkers = std::min((iMaxWorkers + 1) / 2, 1 + kOwner.AI_baseBonusVal(eNonObsoleteBonus) / 20);
								}
							}

							if (kOwner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD, m_unit->getGroup()) < iMaxWorkers
							&& (!bDoImprove || pLoopPlot->getBuildTurnsLeft(eBestTempBuild, 0, 0) > 2 * iPathTurns - 1))
							{
								if (bDoImprove)
								{
									iValue *= 1000;

									if (m_unit->atPlot(pLoopPlot))
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
				if (m_unit->generatePath(pBestPlot, iBasePathFlags, false, &iPathTurns)
				&& iPathTurns >= stepDistance(m_unit->getX(), m_unit->getY(), pBestPlot->getX(), pBestPlot->getY()))
				{
					eBestMission = MISSION_ROUTE_TO;
				}
			}
			eBestBuild = betterPlotBuild(pBestPlot, eBestBuild);

			if (m_unit->getGroup()->pushMissionInternal(eBestMission, pBestPlot->getX(), pBestPlot->getY(), (m_unit->isHuman() ? 0 : MOVE_WITH_CAUTION), false, false, MISSIONAI_BUILD, pBestPlot))
			{
				m_unit->getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, (m_unit->getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);
				return true;
			}
		}
		else if (bBestBuildIsRoute)
		{
			if (connectPlot(pBestPlot))
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


bool CvUnitDefaultAI::isAwaitingContract() const
{
	return (m_contractsLastEstablishedTurn == GC.getGame().getGameTurn() &&
			(m_contractualState == CONTRACTUAL_STATE_AWAITING_ANSWER || m_contractualState == CONTRACTUAL_STATE_AWAITING_WORK));
}

bool CvUnitDefaultAI::processContracts(int iMinPriority)
{
	PROFILE_FUNC();

	//	Currently not implemented for domains other than land
	if (m_unit->getDomainType() != DOMAIN_LAND)
	{
		return false;
	}

	bool bContractAlreadyEstablished = (m_contractsLastEstablishedTurn == GC.getGame().getGameTurn());

	// Have we advertised ourselves as available yet?
	if (!bContractAlreadyEstablished)
	{
		GET_PLAYER(m_unit->getOwner()).getContractBroker().lookingForWork(m_unit, iMinPriority);

		m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
		m_contractualState = CONTRACTUAL_STATE_AWAITING_WORK;
		if (gUnitLogLevel >= 3)
		{
			logBBAI("	Unit %S (%d) for player %d (%S) at (%d,%d) advertising for work\n",
					m_unit->getUnitInfo().getDescription(),
					m_unit->getID(),
					m_unit->getOwner(),
					GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0),
					m_unit->getX(),
					m_unit->getY());
		}
	}
	int iAtX = INVALID_PLOT_COORD;
	int iAtY = INVALID_PLOT_COORD;
	CvUnit* pJoinUnit = NULL;

	if (GET_PLAYER(m_unit->getOwner()).getContractBroker().makeContract(m_unit, iAtX, iAtY, pJoinUnit, !bContractAlreadyEstablished))
	{
		// Work found
		if (gUnitLogLevel >= 3)
		{
			logBBAI("	Unit %S (%d) for player %d (%S) at (%d,%d) found work at (%d,%d) [to join %d]\n",
					m_unit->getUnitInfo().getDescription(),
					m_unit->getID(),
					m_unit->getOwner(),
					GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0),
					m_unit->getX(),
					m_unit->getY(),
					(pJoinUnit == NULL ? iAtX : pJoinUnit->getX()),
					(pJoinUnit == NULL ? iAtY : pJoinUnit->getY()),
					(pJoinUnit == NULL ? -1 : pJoinUnit->getID()));
		}

		//	In rare circumstances it is possible that the unit that advertised for us may have itself actively
		//	joined us, in which case we're done with m_unit contract
		if (pJoinUnit != NULL && pJoinUnit->getGroup() == m_unit->getGroup())
		{
			m_contractualState = CONTRACTUAL_STATE_FOUND_WORK;

			//	Remove ourselves from advertising for work and set our status back to no contract
			contractFulfilled();

			return true;
		}

		//	Must ungroup ourselves since it's just m_unit unit that is answering the work
		//	request (unless it's a no-defend unit in which case it will go with any escorts it has)
		if (m_unit->getGroup()->getNumUnits() > 1 && m_unit->canDefend())
		{
			//	Also check we are not the ONLY unit that can defend - if we are we'll drag the rest along
			bool bHasOtherDefender = false;

			foreach_(const CvUnit * pLoopUnit, m_unit->getGroup()->units())
			{
				if (pLoopUnit != m_unit && pLoopUnit->canDefend())
				{
					bHasOtherDefender = true;
					break;
				}
			}

			if (bHasOtherDefender)
			{
				m_unit->joinGroup(NULL);
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

		if (m_unit->atPlot(pTargetPlot))
		{
			m_contractualState = CONTRACTUAL_STATE_FOUND_WORK;

			if (pJoinUnit != NULL && m_unit->atPlot(pJoinUnit->plot()))
			{
				if (gUnitLogLevel >= 3)
				{
					logBBAI("	...already at target m_unit->plot - merging into requesting unit's group.");
				}
				m_unit->getGroup()->setTransportUnit(NULL);
				pJoinUnit->getGroup()->setTransportUnit(NULL);
				m_unit->getGroup()->mergeIntoGroup(pJoinUnit->getGroup());

				//	Remove ourselves from advertising for work and set our status back to no contract
				contractFulfilled();
			}
			else
			{
				if (gUnitLogLevel >= 3)
				{
					logBBAI("	...already at target m_unit->plot.");
				}
				contractFulfilled();
				m_unit->getGroup()->pushMission(MISSION_SKIP);
			}
		}
		else
		{
			if (!m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pTargetPlot->getX(), pTargetPlot->getY(), MOVE_SAFE_TERRITORY | MOVE_AVOID_ENEMY_UNITS, false, false, (pJoinUnit == NULL ? MISSIONAI_CONTRACT : MISSIONAI_CONTRACT_UNIT), pTargetPlot))
			{
				if (gUnitLogLevel >= 3)
				{
					logBBAI("	...unexpectedly unable to enact the work!");
				}
				//	Handle as if we found no work m_unit turn since we cannot safely path to it
				m_contractualState = CONTRACTUAL_STATE_NO_WORK_FOUND;
				GET_PLAYER(m_unit->getOwner()).getContractBroker().removeUnit(m_unit);
				return false;
			}
			//	We only reset the contract establishment if we actively move to
			//	fulfill it.  This is so that:
			//	1)	In the move case, if the move doesn't use up all our movement allowance we can
			//		potentially recontract
			//	2)	In the non-move case we retain the knowledge that we've already looked and not found
			//		anything in particular to do - m_unit prevents other units grouping with us and setting us back
			//		awake
			contractFulfilled();
		}
		return true;
	}

	if (bContractAlreadyEstablished)
	{
		m_contractualState = CONTRACTUAL_STATE_NO_WORK_FOUND;

		//	No work available
		if (gUnitLogLevel >= 3)
		{
			logBBAI("	Unit %S (%d) for player %d (%S) at (%d,%d) - no work available",
					m_unit->getUnitInfo().getDescription(),
					m_unit->getID(),
					m_unit->getOwner(),
					GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0),
					m_unit->getX(),
					m_unit->getY());
		}
		return false;
	}
	return true;
}

void CvUnitDefaultAI::contractFulfilled()
{
	m_contractsLastEstablishedTurn = -1;	//	Contract fulfilled
	m_contractualState = CONTRACTUAL_STATE_NONE;

	GET_PLAYER(m_unit->getOwner()).getContractBroker().removeUnit(m_unit);
}

bool CvUnitDefaultAI::workerNeedsDefender(const CvPlot* pPlot) const
{
	PROFILE_FUNC();

	if (m_unit->isNPC())
	{
		return false;
	}

	//	Check danger level both where we are now and where we are headed
	const int iDanger = std::max(pPlot->getDangerCount(m_unit->getOwner()), m_unit->plot()->getDangerCount(m_unit->getOwner()));

	//	Need to adjust m_unit threshold based on experience with AI testing - 25 is an initial good guess
	if (iDanger > 20)
	{
		// Afforess - don't just return true, check for actual danger first!
		// return true;
		return GET_PLAYER(m_unit->getOwner()).AI_isPlotThreatened(m_unit->plot(), 2);
	}

	//	Also for non-owned territory check for nearby enemies
	if (pPlot->getOwner() != m_unit->getOwner() && GET_PLAYER(m_unit->getOwner()).AI_getVisiblePlotDanger(pPlot, 2, false))
	{
		return true;
	}

	return false;
}

bool CvUnitDefaultAI::workerReleaseDefenderIfNotNeeded() const
{
	if (m_unit->isNPC() || m_unit->getGroup()->getNumUnits() < 2)
	{
		// NPC rarely has anything better to do with military unit than be a hinder against other players capturing workers from them.
		// NPC always return false on workerNeedsDefender, so may get stuck in an infinite loop of grouping/ungrouping with defenders.
		return false;
	}
	// Never release on the same turn we contracted or an infinite loop can result
	if (m_contractsLastEstablishedTurn != GC.getGame().getGameTurn())
	{
		if (m_unit->getDomainType() != DOMAIN_LAND)
		{
			return false;
		}

		if (m_unit->getGroup()->canDefend() && !workerNeedsDefender(m_unit->plot()) && reachHome(true))
		{
			if (gUnitLogLevel >= 3)
			{
				logBBAI(
					"	Worker for player %d (%S) at (%d,%d) releasing escort",
					m_unit->getOwner(),
					GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0),
					m_unit->getX(),
					m_unit->getY()
				);
			}
			m_unit->getGroup()->AI_makeForceSeparate();

			return true;
		}
	}

	return false;
}

//returns true if a mission is pushed
//if eBuild is NO_BUILD, assumes a route is desired.
bool CvUnitDefaultAI::improvePlot(CvPlot* pPlot, BuildTypes eBuild)
{
	PROFILE_FUNC();

	FASSERT_BOUNDS(NO_BUILD, GC.getNumBuildInfos(), eBuild);
	FAssert(pPlot != NULL);

	if (eBuild != NO_BUILD)
	{
		eBuild = betterPlotBuild(pPlot, eBuild);
		if (!m_unit->atPlot(pPlot))
		{
			if (!m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pPlot->getX(), pPlot->getY(), 0, false, false, MISSIONAI_BUILD, pPlot))
			{
				return false;
			}
		}
		m_unit->getGroup()->pushMission(MISSION_BUILD, eBuild, -1, 0, (m_unit->getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pPlot);

		return true;
	}
	else if (m_unit->canBuildRoute() && connectPlot(pPlot))
	{
		return true;
	}

	return false;

}

BuildTypes CvUnitDefaultAI::betterPlotBuild(const CvPlot* pPlot, BuildTypes eBuild) const
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

	int iWorkersNeeded = calculatePlotWorkersNeeded(pPlot, eBuild);

	if ((pPlot->getNonObsoleteBonusType(m_unit->getTeam()) == NO_BONUS) && (pPlot->getWorkingCity() != NULL))
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

	if (pPlot->getNonObsoleteBonusType(m_unit->getTeam()) != NO_BONUS)
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
			if (m_unit->canBuild(pPlot, eBuild))
			{
				int iValue = 10000;

				if (bBuildRoute && (eRoute != NO_ROUTE))
				{
					iValue *= (1 + GC.getRouteInfo(eRoute).getValue());
					iValue /= 2;

					if (pPlot->getNonObsoleteBonusType(m_unit->getTeam()) != NO_BONUS)
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
							const CvPlotGroup* pLoopGroup = pLoopPlot->getPlotGroup(m_unit->getOwner());
							if (pLoopGroup != NULL && pLoopGroup->getID() != -1)
							{
								if (pLoopGroup->getID() != iPlotGroupId)
								{
									//This m_unit->plot bridges m_unit->plot groups, so route it.
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
bool CvUnitDefaultAI::connectBonus(bool bTestTrade)
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	BonusTypes eNonObsoleteBonus;
	int iI;

	// XXX how do we make sure that we can build roads???

	for (iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->getOwner() == m_unit->getOwner()) // XXX team???
		{
			if (plotValid(pLoopPlot) && pLoopPlot->area() == m_unit->area())
			{
				eNonObsoleteBonus = pLoopPlot->getNonObsoleteBonusType(m_unit->getTeam());

				if (eNonObsoleteBonus != NO_BONUS)
				{
					if (!(pLoopPlot->isConnectedToCapital()))
					{
						if (!bTestTrade || ((pLoopPlot->getImprovementType() != NO_IMPROVEMENT) && (GC.getImprovementInfo(pLoopPlot->getImprovementType()).isImprovementBonusTrade(eNonObsoleteBonus))))
						{
							if (connectPlot(pLoopPlot))
							{
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::connectCity()
{
	PROFILE_FUNC();

	// XXX how do we make sure that we can build roads???

	CvCity* pLoopCity = m_unit->plot()->getWorkingCity();
	if (pLoopCity != NULL)
	{
		if (pLoopCity->getNumWorkers() <= 2)
		{
			if (plotValid(pLoopCity->plot()))
			{
				if (!(pLoopCity->isConnectedToCapital()))
				{
					if (connectPlot(pLoopCity->plot(), 1))
					{
						return true;
					}
				}
			}
		}
	}

	foreach_(const CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
	{
		if (pLoopCity->getNumWorkers() <= 2)
		{
			if (plotValid(pLoopCity->plot()))
			{
				if (!(pLoopCity->isConnectedToCapital()))
				{
					if (connectPlot(pLoopCity->plot(), 1))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::routeCity()
{
	PROFILE_FUNC();

	CvCity* pRouteToCity;
	int iBasePathFlags = MOVE_SAFE_TERRITORY | MOVE_AVOID_ENEMY_UNITS | MOVE_OUR_TERRITORY | MOVE_RECONSIDER_ON_LEAVING_OWNED;

	if (!m_unit->canBuildRoute())
	{
		return false;
	}

	FAssert(m_unit->canBuildRoute());

	foreach_(const CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
	{
		if (plotValid(pLoopCity->plot()))
		{
			/************************************************************************************************/
			/* BETTER_BTS_AI_MOD					  02/22/10								jdog5000	  */
			/*																							  */
			/* Unit AI, Efficiency																		  */
			/************************************************************************************************/
						// BBAI efficiency: check m_unit->area for land units before generating path
			if ((m_unit->getDomainType() == DOMAIN_LAND) && (pLoopCity->area() != m_unit->area()) && !(m_unit->getGroup()->canMoveAllTerrain()))
			{
				continue;
			}

			pRouteToCity = pLoopCity->AI_getRouteToCity();

			if (pRouteToCity != NULL)
			{
				if (!(pLoopCity->plot()->isVisibleEnemyUnit(m_unit)))
				{
					if (!(pRouteToCity->plot()->isVisibleEnemyUnit(m_unit)))
					{
						if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pRouteToCity->plot(), MISSIONAI_BUILD, m_unit->getGroup()) == 0)
						{
							if (m_unit->generatePath(pLoopCity->plot(), iBasePathFlags, true))
							{
								if (m_unit->generatePath(pRouteToCity->plot(), iBasePathFlags, true))
								{
									if (m_unit->getGroup()->pushMissionInternal(MISSION_ROUTE_TO, pLoopCity->getX(), pLoopCity->getY(), MOVE_SAFE_TERRITORY | MOVE_WITH_CAUTION, false, false, MISSIONAI_BUILD, pRouteToCity->plot()))
									{
										m_unit->getGroup()->pushMission(MISSION_ROUTE_TO, pRouteToCity->getX(), pRouteToCity->getY(), MOVE_SAFE_TERRITORY | MOVE_WITH_CAUTION, (m_unit->getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pRouteToCity->plot());

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

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::routeTerritory(bool bImprovementOnly)
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	ImprovementTypes eImprovement;
	RouteTypes eBestRoute;
	bool bValid;
	int iPathTurns;
	int iValue;
	int iBestValue;
	int iI, iJ;

	FAssert(m_unit->canBuildRoute());

	iBestValue = 0;
	pBestPlot = NULL;

	for (iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->getOwner() == m_unit->getOwner())
		{
			if (pLoopPlot->area() == m_unit->area() && plotValid(pLoopPlot))
			{
				// Best route depends on Tech, Plot(Terrain/Feature, and Improvement in case of eRail v Highway)
				eBestRoute = GET_PLAYER(m_unit->getOwner()).getBestRoute(pLoopPlot, false, m_unit);

				if (eBestRoute != NO_ROUTE)
				{
					if (eBestRoute != pLoopPlot->getRouteType())
					{
						if (bImprovementOnly)
						{
							bValid = false;

							eImprovement = pLoopPlot->getImprovementType();

							if (eImprovement != NO_IMPROVEMENT)
							{
								for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
								{
									if (GC.getImprovementInfo(eImprovement).getRouteYieldChanges(eBestRoute, iJ) > 0)
									{
										bValid = true;
										break;
									}
								}
							}
						}
						else
						{
							bValid = true;
						}

						if (bValid)
						{
							if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD, m_unit->getGroup(), 1) == 0)
							{
								if (generateSafePathforVulnerable(pLoopPlot, &iPathTurns))
								{
									iValue = 10000;

									iValue /= (iPathTurns + 1);

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
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (!m_unit->atPlot(pBestPlot))
		{
			if (generateSafePathforVulnerable(pBestPlot, &iPathTurns))
			{
				return m_unit->getGroup()->pushMissionInternal(MISSION_ROUTE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_BUILD, pBestPlot);
			}
			else
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
				return true;
			}
		}
		else
		{
			m_unit->getGroup()->pushMission(MISSION_ROUTE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_SAFE_TERRITORY, false, false, MISSIONAI_BUILD, pBestPlot);
			return true;
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::travelToUpgradeCity()
{
	PROFILE_FUNC();

	// Check if unit is not a dead end, that there exist a better unit in the game that it one day can upgrade into.
	if (GC.getUnitInfo(m_unit->getUnitType()).getNumUnitUpgrades() <= 0)
	{
		return false;
	}
	SendLog("AI_ExploreMove", CvWString::format(L"Unit: %S is not a dead end unit, and can potentially upgrade to something one day", m_unit->getName().c_str()));
	// is there a city which can upgrade us?
	CvCity* pUpgradeCity = m_unit->getUpgradeCity(/*bSearch*/ true);
	if (pUpgradeCity != NULL)
	{
		// cache some stuff
		CvPlot* pPlot = m_unit->plot();

		// if we at the upgrade city, stop, wait to get upgraded
		if (pUpgradeCity->plot() == pPlot)
		{
			if (m_unit->isHuman())
			{
				// Wake up and notify player!
				const CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_UNIT_UPGRADE_READY", m_unit->getNameKey(), pUpgradeCity->getNameKey());
				AddDLLMessage(m_unit->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, NULL, GC.getCOLOR_GREY(), m_unit->getX(), m_unit->getY(), true, true);

				m_unit->getGroup()->clearMissionQueue();
				m_unit->getGroup()->setActivityType(ACTIVITY_AWAKE);
				return true;
			}
			if (m_unit->getDomainType() == DOMAIN_AIR)
			{
				return false;
			}
			if (m_unit->getGroup()->pushMissionInternal(MISSION_SKIP))
			{
				return true;
			}
		}

		if (DOMAIN_AIR == m_unit->getDomainType())
		{
			FAssert(!m_unit->atPlot(pUpgradeCity->plot()));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pUpgradeCity->getX(), pUpgradeCity->getY());
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
			pClosestCity = GC.getMap().findCity(m_unit->getX(), m_unit->getY(), NO_PLAYER, m_unit->getTeam(), true, m_unit->getDomainType() == DOMAIN_SEA);
		}

		// can we path to the upgrade city?
		int iUpgradeCityPathTurns;
		CvPlot* pThisTurnPlot = NULL;
		bool bCanPathToUpgradeCity = m_unit->generatePath(pUpgradeCity->plot(), 0, true, &iUpgradeCityPathTurns);
		if (bCanPathToUpgradeCity)
		{
			pThisTurnPlot = m_unit->getPathEndTurnPlot();
		}

		// if we close to upgrade city, head there
		if (NULL != pThisTurnPlot && NULL != pClosestCity
		&& (pClosestCity == pUpgradeCity || iUpgradeCityPathTurns < 4)
		&& !exposedToDanger(pThisTurnPlot, 70))
		{
			FAssert(!m_unit->atPlot(pThisTurnPlot));
			if (m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pThisTurnPlot->getX(), pThisTurnPlot->getY()))
			{
				return true;
			}
		}

		// check for better airlift choice
		if (m_unit->getDomainType() == DOMAIN_LAND && NULL != pClosestCity && pClosestCity->getMaxAirlift() > 0)
		{
			// if we at the closest city, then do the airlift, or wait
			if (bAtClosestCity)
			{
				// can we do the airlift m_unit turn?
				if (m_unit->canAirliftAt(pClosestCity->plot(), pUpgradeCity->getX(), pUpgradeCity->getY()))
				{
					if (m_unit->getGroup()->pushMissionInternal(MISSION_AIRLIFT, pUpgradeCity->getX(), pUpgradeCity->getY()))
					{
						return true;
					}
				}
				// wait to do it next turn
				else if (m_unit->getGroup()->pushMissionInternal(MISSION_SKIP))
				{
					return true;
				}
			}
			int iClosestCityPathTurns;
			const CvPlot* pThisTurnPlotForAirlift =
				(
					m_unit->generatePath(pClosestCity->plot(), 0, true, &iClosestCityPathTurns)
					?
					m_unit->getPathEndTurnPlot()
					:
					NULL
				);
			// is the closest city closer pathing? If so, move toward closest city
			if (NULL != pThisTurnPlotForAirlift
			&& (!bCanPathToUpgradeCity || iClosestCityPathTurns < iUpgradeCityPathTurns)
			&& !exposedToDanger(pThisTurnPlotForAirlift, 70))
			{
				FAssert(!m_unit->atPlot(pThisTurnPlotForAirlift));
				if (m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pThisTurnPlotForAirlift->getX(), pThisTurnPlotForAirlift->getY()))
				{
					return true;
				}
			}
		}

		// did not have better airlift choice, go ahead and path to the upgrade city
		if (NULL != pThisTurnPlot)
		{
			if (!exposedToDanger(pThisTurnPlot, 70))
			{
				FAssert(!m_unit->atPlot(pThisTurnPlot));
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pThisTurnPlot->getX(), pThisTurnPlot->getY());
			}
		}
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::retreatToCity(bool bPrimary, bool bAirlift, int iMaxPath)
{
	PROFILE_FUNC();

	CvPlot* pBestPlot = NULL;
	CvPlot* endTurnPlot = NULL;
	int iBestValue = MAX_INT;
	const int iCurrentDanger = GET_PLAYER(m_unit->getOwner()).AI_getPlotDanger(m_unit->plot());

	CvCity* pCity = m_unit->plot()->getPlotCity();

	if (0 == iCurrentDanger && pCity && pCity->getOwner() == m_unit->getOwner()
	&& (!bPrimary || GET_PLAYER(m_unit->getOwner()).AI_isPrimaryArea(pCity->area()))
	&& (!bAirlift || pCity->getMaxAirlift() > 0)
	&& !pCity->plot()->isVisibleEnemyUnit(m_unit))
	{
		// If we've successfully retreated and have an escort release it for now,
		//	the exceptions are generals who build up multiple escorts while in cities
		if (!m_unit->canDefend() && m_unit->getGroup()->canDefend() && getUnitAIType() != UNITAI_GENERAL)
		{
			m_unit->joinGroup(NULL);
		}
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return true;
	}
	const int iSearchRange = searchRange(iMaxPath);

	int iPass = 0;
	for (; iPass < 4; iPass++)
	{
		CvReachablePlotSet plotSet(m_unit->getGroup(), iPass > 1 ? MOVE_IGNORE_DANGER : 0, iSearchRange);

		foreach_(const CvCity * cityX, GET_PLAYER(m_unit->getOwner()).cities())
		{
			if (plotSet.find(cityX->plot()) == plotSet.end()
			|| bPrimary && !GET_PLAYER(m_unit->getOwner()).AI_isPrimaryArea(cityX->area())
			|| bAirlift && cityX->getMaxAirlift() < 1
			|| cityX->plot()->isVisibleEnemyUnit(m_unit)
			|| !bAirlift && m_unit->getDomainType() == DOMAIN_LAND && cityX->area() != m_unit->area() && !m_unit->getGroup()->canMoveAllTerrain()
			|| m_unit->atPlot(cityX->plot()))
			{
				continue;
			}
			int iPathTurns;

			if (m_unit->generatePath(cityX->plot(), ((iPass > 1) ? MOVE_IGNORE_DANGER : 0), true, &iPathTurns)
			&& iPathTurns <= (iPass == 2 ? 1 : iMaxPath))
			{
				// Water units can't defend a city.
				// Any unthreatened city acceptable on 0th pass, solves problem where sea units would
				// oscillate in and out of threatened city because they had iCurrentDanger = 0 on turns outside city

				bool bCheck = iPass > 0 || m_unit->getGroup()->canDefend();
				if (!bCheck)
				{
					const int iLoopDanger = GET_PLAYER(m_unit->getOwner()).AI_getPlotDanger(cityX->plot());
					bCheck = (
						iLoopDanger == 0
						|| (
							iLoopDanger < iCurrentDanger
							&& //Fuyu: try to avoid doomed cities
							iLoopDanger < 2 * (cityX->plot()->getNumDefenders(m_unit->getOwner()))
						)
					);
				}
				if (bCheck)
				{
					int iValue = iPathTurns;

					if (getUnitAIType() == UNITAI_SETTLER_SEA)
					{
						iValue *= 1 + std::max(0, GET_PLAYER(m_unit->getOwner()).AI_totalAreaUnitAIs(cityX->area(), UNITAI_SETTLE) - GET_PLAYER(m_unit->getOwner()).AI_totalAreaUnitAIs(cityX->area(), UNITAI_SETTLER_SEA));
					}

					if (iValue < iBestValue)
					{
						endTurnPlot = m_unit->getPathEndTurnPlot();

						if (endTurnPlot == cityX->plot() || !exposedToDanger(endTurnPlot, 60))
						{
							iBestValue = iValue;
							pBestPlot = endTurnPlot;

							if (m_unit->atPlot(pBestPlot))
							{
								pBestPlot = m_unit->getGroup()->getPathFirstPlot();
								FAssert(!m_unit->atPlot(pBestPlot));
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
		if (iPass == 0 && pCity && pCity->getOwner() == m_unit->getOwner()
		&& (!bPrimary || GET_PLAYER(m_unit->getOwner()).AI_isPrimaryArea(pCity->area()))
		&& (!bAirlift || pCity->getMaxAirlift() > 0)
		&& !pCity->plot()->isVisibleEnemyUnit(m_unit))
		{
			// If we've successfully retreated and have an escort release it for now
			if (!m_unit->canDefend() && m_unit->getGroup()->canDefend())
			{
				m_unit->joinGroup(NULL);
			}
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
		if (m_unit->getGroup()->alwaysInvisible())
		{
			break;
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), iPass > 0 ? MOVE_IGNORE_DANGER : 0);
	}

	if (pCity && pCity->getTeam() == m_unit->getTeam())
	{
		// If we've successfully retreated and have an escort release it for now
		if (!m_unit->canDefend() && m_unit->getGroup()->canDefend())
		{
			m_unit->joinGroup(NULL);
		}
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return true;
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::pickup(UnitAITypes eUnitAI, bool bCountProduction, int iMaxPath)
{
	PROFILE_FUNC();

	CvCity* pCity;
	CvPlot* pBestPlot;
	CvPlot* pBestPickupPlot;
	CvPlot* endTurnPlot = NULL;
	int iPathTurns;
	int iValue;
	int iBestValue;

	bool bFull = (m_unit->getGroup()->AI_isFull() && (m_unit->getGroup()->getCargo() > 0));
	if (bFull)
	{
		FErrorMsg("Unit is not expected to be full when pickup is called");
		return false;
	}

	pCity = m_unit->plot()->getPlotCity();

	if (pCity != NULL)
	{
		if (pCity->getOwner() == m_unit->getOwner())
		{
			if ((GC.getGame().getGameTurn() - pCity->getGameTurnAcquired()) > 15 || (GET_TEAM(m_unit->getTeam()).countEnemyPowerByArea(pCity->area()) == 0))
			{
				bool bConsider = false;

				if (getUnitAIType() == UNITAI_ASSAULT_SEA)
				{
					// Improve island hopping
					if (pCity->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_DEFENSIVE)
					{
						bConsider = false;
					}
					else if (eUnitAI == UNITAI_ATTACK_CITY && !(pCity->AI_isDanger()))
					{
						bConsider = (pCity->plot()->plotCount(PUF_canDefend, -1, -1, NULL, m_unit->getOwner(), NO_TEAM, PUF_isDomainType, DOMAIN_LAND) > pCity->AI_neededDefenders());
					}
					else
					{
						bConsider = pCity->AI_isDefended(-GET_PLAYER(m_unit->getOwner()).strengthOfBestUnitAI(DOMAIN_LAND, UNITAI_CITY_DEFENSE));
					}
				}
				else if (getUnitAIType() == UNITAI_SETTLER_SEA)
				{
					if (eUnitAI == UNITAI_CITY_DEFENSE)
					{
						bConsider = (pCity->plot()->plotCount(PUF_canDefendGroupHead, -1, -1, NULL, m_unit->getOwner(), NO_TEAM, PUF_isCityAIType) > 1);
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
					// only count units which are available to load
					int iCount = pCity->plot()->plotCount(PUF_isAvailableUnitAITypeGroupie, eUnitAI, -1, NULL, m_unit->getOwner(), NO_TEAM, PUF_isFiniteRange);

					if (bCountProduction && (pCity->getProductionUnitAI() == eUnitAI))
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

					if (!GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
					{
						if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pCity->plot(), MISSIONAI_PICKUP, m_unit->getGroup()) < ((iCount + (m_unit->getGroup()->getCargoSpace() - 1)) / std::max(1, m_unit->getGroup()->getCargoSpace())))
						{
							m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_PICKUP, pCity->plot());
							return true;
						}
					}
					else
					{
						if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIsinCargoVolume(pCity->plot(), MISSIONAI_PICKUP, m_unit->getGroup()) < (((iCount * 100) + (m_unit->getGroup()->getCargoSpace() - 100)) / std::max(1, m_unit->getGroup()->getCargoSpace())))
						{
							m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_PICKUP, pCity->plot());
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

	foreach_(CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
	{
		if (plotValid(pLoopCity->plot()))
		{
			if ((GC.getGame().getGameTurn() - pLoopCity->getGameTurnAcquired()) > 15 || (GET_TEAM(m_unit->getTeam()).countEnemyPowerByArea(pLoopCity->area()) == 0))
			{
				bool bConsider = false;

				if (getUnitAIType() == UNITAI_ASSAULT_SEA)
				{
					if (pLoopCity->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_DEFENSIVE)
					{
						bConsider = false;
					}
					else if (eUnitAI == UNITAI_ATTACK_CITY && !(pLoopCity->AI_isDanger()))
					{
						// Improve island hopping
						bConsider = (pLoopCity->plot()->plotCount(PUF_canDefend, -1, -1, NULL, m_unit->getOwner(), NO_TEAM, PUF_isDomainType, DOMAIN_LAND) > pLoopCity->AI_neededDefenders());
					}
					else
					{
						bConsider = pLoopCity->AI_isDefended(GET_PLAYER(m_unit->getOwner()).strengthOfBestUnitAI(DOMAIN_LAND, UNITAI_CITY_DEFENSE));
					}
				}
				else if (getUnitAIType() == UNITAI_SETTLER_SEA)
				{
					if (eUnitAI == UNITAI_CITY_DEFENSE)
					{
						bConsider = (pLoopCity->plot()->plotCount(PUF_canDefendGroupHead, -1, -1, NULL, m_unit->getOwner(), NO_TEAM, PUF_isCityAIType) > 1);
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
					// only count units which are available to load, have had a chance to move since being built
					int iCount = pLoopCity->plot()->plotCount(PUF_isAvailableUnitAITypeGroupie, eUnitAI, -1, NULL, m_unit->getOwner(), NO_TEAM, (bCountProduction ? PUF_isFiniteRange : PUF_isFiniteRangeAndNotJustProduced));

					iValue = iCount * 10;

					if (bCountProduction && (pLoopCity->getProductionUnitAI() == eUnitAI))
					{
						const CvUnitInfo& kUnitInfo = GC.getUnitInfo(pLoopCity->getProductionUnit());
						if ((kUnitInfo.getDomainType() != DOMAIN_AIR) || kUnitInfo.getAirRange() > 0)
						{
							iValue++;
							iCount++;
						}
					}

					if (iValue > 0)
					{
						iValue += pLoopCity->getPopulation();

						if (!(pLoopCity->plot()->isVisibleEnemyUnit(m_unit)))
						{
							bool bCheck = false;
							if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
							{
								bCheck = (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIsinCargoVolume(pLoopCity->plot(), MISSIONAI_PICKUP, m_unit->getGroup()) < (((iCount * 100) + (m_unit->getGroup()->getCargoSpace() - 100)) / std::max(1, m_unit->getGroup()->getCargoSpace())));
							}
							else
							{
								bCheck = (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_PICKUP, m_unit->getGroup()) < ((iCount + (m_unit->getGroup()->getCargoSpace() - 1)) / std::max(1, m_unit->getGroup()->getCargoSpace())));
							}
							if (bCheck)
							{
								if (!(pLoopCity->AI_isDanger()))
								{
									if (!m_unit->atPlot(pLoopCity->plot()) && m_unit->generatePath(pLoopCity->plot(), 0, true, &iPathTurns, iMaxPath))
									{
										if (getUnitAIType() == UNITAI_ASSAULT_SEA)
										{
											if (pLoopCity->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_ASSAULT)
											{
												iValue *= 4;
											}
											else if (pLoopCity->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_ASSAULT_ASSIST)
											{
												iValue *= 2;
											}
										}

										iValue *= 1000;

										iValue /= (iPathTurns + 3);

										if ((iValue > iBestValue))
										{
											endTurnPlot = m_unit->getPathEndTurnPlot();

											if (endTurnPlot == pLoopCity->plot() || !exposedToDanger(endTurnPlot, 60))
											{
												iBestValue = iValue;
												// Do one turn along path, then reevaluate
												// Causes update of destination based on troop movement
												//pBestPlot = pLoopCity->plot();
												pBestPlot = endTurnPlot;
												pBestPickupPlot = pLoopCity->plot();

												if (pBestPlot == NULL || m_unit->atPlot(pBestPlot))
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
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_WEIGHT_3, false, false, MISSIONAI_PICKUP, pBestPickupPlot);
		}
		else
		{
			return m_unit->getGroup()->pushMissionInternal(MISSION_SKIP);
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::pickupStranded(UnitAITypes eUnitAI, int iMaxPath)
{
	//TB TRANSPORTNOTES: potentially big problems here with checking for the amount of cargo not being possible to check in SM perspective.
	PROFILE_FUNC();

	if (m_unit->isNPC())
	{
		return false;
	}

	if (m_unit->getGroup()->AI_isFull() && m_unit->getGroup()->getCargo() > 0)
	{
		FErrorMsg("Unit is not expected to be full when pickupStranded is called");
		return false;
	}

	const bool bSM = GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS);

	const CvPlayerAI& player = GET_PLAYER(m_unit->getOwner());

	int iBestValue = 0;
	CvUnit* pBestUnit = NULL;
	CvPlot* pBestPickupPlot = NULL;

	CvReachablePlotSet plotSet(m_unit->getGroup(), MOVE_ALLOW_ADJACENT_COASTAL, searchRange(iMaxPath));

	foreach_(CvSelectionGroup * groupX, player.groups())
	{
		if (groupX->isStranded())
		{
			CvUnit* pHeadUnit = groupX->getHeadUnit();
			if (pHeadUnit == NULL || eUnitAI != NO_UNITAI && pHeadUnit->AI()->getUnitAIType() != eUnitAI)
			{
				continue;
			}
			CvPlot* plotX = pHeadUnit->plot();
			if (plotX == NULL || !plotX->isCoastalLand() && !m_unit->canMoveAllTerrain())
			{
				continue;
			}
			// Units are stranded, attempt rescue
			// TB TRANSPORTNOTES: iCount should be shifted to getting the cargo volume of groupX.
			int iCount = bSM ? groupX->getNumUnitCargoVolumeTotal() : groupX->getNumUnits();

			if (1000 * iCount > iBestValue)
			{
				PROFILE("CvUnitDefaultAI::pickupStranded.EvaluateTargetUnit");

				CvPlot* pPickupPlot = NULL;
				if (m_unit->atPlot(plotX))
				{
					pPickupPlot = plotX;
				}
				else if (plotSet.find(plotX) != plotSet.end())
				{
					pPickupPlot = plotX;
				}
				else if (pPickupPlot == NULL)
				{
					foreach_(CvPlot * plotY, plotX->adjacent())
					{
						if (plotSet.find(plotY) != plotSet.end()
						// If we can move into the target m_unit->plot in one turn take account of enemy units there
						&& stepDistance(plotY->getX(), plotY->getY(), m_unit->getX(), m_unit->getY()) > m_unit->getMoves()
						|| !plotY->isVisibleEnemyUnit(m_unit))
						{
							pPickupPlot = plotY;
							break;
						}
					}
				}

				if (pPickupPlot != NULL)
				{
					MissionAITypes eMissionAIType = MISSIONAI_PICKUP;
					iCount -= (
						player.AI_unitTargetMissionAIs(pHeadUnit, &eMissionAIType, 1, m_unit->getGroup(), iMaxPath == MAX_INT ? -1 : iMaxPath)
						*
						m_unit->cargoSpace()
					);
					int iValue = 1000 * iCount;

					const int iMaxValuePath = (iBestValue == 0 ? MAX_INT : iValue / iBestValue);

					SpecialUnitTypes eSpecialCargo = (SpecialUnitTypes)pHeadUnit->getSpecialUnitType();
					DomainTypes eDomainCargo = (DomainTypes)pHeadUnit->getDomainType();
					if (!bSM || m_unit->getGroup()->getCargoSpaceAvailable(eSpecialCargo, eDomainCargo) >= groupX->getNumUnitCargoVolumeTotal())
					{
						int iPathTurns = 0;
						if (m_unit->generatePath(pPickupPlot, 0, true, &iPathTurns, std::min(iMaxPath, iMaxValuePath)))
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
		}
	}

	if (pBestUnit != NULL)
	{
		if (m_unit->atPlot(pBestPickupPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_PICKUP, pBestUnit->plot());
			return true;
		}
		FAssert(!m_unit->atPlot(pBestPickupPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPickupPlot->getX(), pBestPickupPlot->getY(), 0, false, false, MISSIONAI_PICKUP, pBestPickupPlot);
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::airOffensiveCity()
{
	//PROFILE_FUNC();

	FAssert(m_unit->canAirAttack() || m_unit->nukeRange() >= 0);

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		const CvPlot* plotX = GC.getMap().plotByIndex(iI);

		// Limit to cities and forts, true for any city but only m_unit team's forts
		if (plotX->isCity(true, m_unit->getTeam())
		&&
		(
			plotX->getTeam() == m_unit->getTeam()
			||
			plotX->isOwned() && GET_TEAM(plotX->getTeam()).isVassal(m_unit->getTeam())
			)
		&& (m_unit->atPlot(plotX) || m_unit->canEnterPlot(plotX)))
		{
			const int iValue = airOffenseBaseValue(plotX);

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestPlot = plotX;
			}
		}
	}

	if (pBestPlot != NULL && !m_unit->atPlot(pBestPlot))
	{
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_SAFE_TERRITORY);
	}
	return false;
}


// Function for ranking the value of a m_unit->plot as a base for offensive air units
int CvUnitDefaultAI::airOffenseBaseValue(const CvPlot* pPlot) const
{
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
	const TeamTypes eTeam = m_unit->getTeam();

	int iAttackAirCount = pPlot->plotCount(PUF_canAirAttack, -1, -1, NULL, NO_PLAYER, eTeam);
	iAttackAirCount += 2 * pPlot->plotCount(PUF_isUnitAIType, UNITAI_ICBM, -1, NULL, NO_PLAYER, eTeam);
	if (m_unit->atPlot(pPlot))
	{
		iAttackAirCount += m_unit->canAirAttack() ? -1 : 0;
		iAttackAirCount += (m_unit->nukeRange() >= 0) ? -2 : 0;
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
			// Toffer - Will iDistance ever be more than 1 in m_unit loop?
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

	// Don't put more attack air units on m_unit->plot than effective land defenders ... too large a risk
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
		const int iRange = m_unit->airRange();

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
							iTempValue += 2 * plotX->getNumVisiblePotentialEnemyDefenders(m_unit);

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
							iTempValue += 4 * plotX->getNumVisiblePotentialEnemyDefenders(m_unit);
						}
					}
					else if (plotX->getTeam() == eTeam)
					{
						iTempValue = 0;

						if (iDistance < iRange - 2)
						{
							// Target enemy troops in our territory
							iTempValue += 5 * plotX->getNumVisiblePotentialEnemyDefenders(m_unit);

							if (plotX->getOwner() == m_unit->getOwner())
							{
								if (GET_PLAYER(m_unit->getOwner()).AI_isPrimaryArea(plotX->area()))
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

							if (m_unit->canAirBomb() && cityX->isBombardable(m_unit))
							{
								iTempValue *= 2;
							}

							if (plotX->area()->getTargetCity(m_unit->getOwner()) == cityX)
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
								iTempValue += 15 * plotX->getNumDefenders(m_unit->getOwner());

								// Target enemy troops adjacent to our territory
								if (plotX->isAdjacentTeam(eTeam, true))
								{
									iTempValue += 7 * plotX->getNumVisiblePotentialEnemyDefenders(m_unit);
								}
							}

							// Weight resources
							if (m_unit->canAirBombAt(pPlot, plotX->getX(), plotX->getY())
							&& plotX->getBonusType(eTeam) != NO_BONUS)
							{
								iTempValue += 8 * std::max(2, GET_PLAYER(plotX->getOwner()).AI_bonusVal(plotX->getBonusType(eTeam)) / 10);
							}
						}

						if (plotX->area()->getAreaAIType(eTeam) == AREAAI_OFFENSIVE)
						{
							// Extra weight for enemy territory in offensive m_unit->areas
							iTempValue *= 2;
						}

						if (GET_PLAYER(m_unit->getOwner()).AI_isPrimaryArea(plotX->area()))
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
		const int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(pPlot, 2, false, false);

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
		if (pPlot->getOwner() != m_unit->getOwner())
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
			iValue = (pCity != NULL) ? 0 : GET_PLAYER(m_unit->getOwner()).AI_getPlotAirbaseValue(pPlot);
			iValue /= 6;
		}
		iValue += std::min(24, 3 * (iDefenders - iAttackAirCount));

		if (GET_PLAYER(m_unit->getOwner()).AI_isPrimaryArea(baseArea))
		{
			iValue *= 4;
			iValue /= 3;
		}

		// No real enemies, check for minor civ or barbarian cities where attacks could be supported
		const CvCity* pNearestEnemyCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY(), NO_PLAYER, NO_TEAM, false, false, eTeam);

		if (pNearestEnemyCity != NULL)
		{
			const int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pNearestEnemyCity->getX(), pNearestEnemyCity->getY());
			if (iDistance > m_unit->airRange())
			{
				iValue /= 10 * (2 + m_unit->airRange());
			}
			else iValue /= 2 + iDistance;
		}
	}

	if (pPlot->getOwner() == m_unit->getOwner())
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
bool CvUnitDefaultAI::airDefensiveCity()
{
	//PROFILE_FUNC();

	FAssert(m_unit->getDomainType() == DOMAIN_AIR);
	FAssert(m_unit->canAirDefend());

	if (m_unit->canAirDefend() && m_unit->getDamage() == 0)
	{
		CvCity* pCity = m_unit->plot()->getPlotCity();

		if (pCity != NULL && pCity->getOwner() == m_unit->getOwner())
		{
			if (!pCity->AI_isAirDefended(false, 1))
			{
				// Stay if very short on planes, regardless of situation
				m_unit->getGroup()->pushMission(MISSION_AIRPATROL);
				return true;
			}

			if (!pCity->AI_isAirDefended(true, -1))
			{
				// Stay if city is threatened but not seriously threatened
				const int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 2, false, false);

				if (iEnemyOffense > 0)
				{
					const int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), 0, true, false, true);
					if (3 * iEnemyOffense < 4 * iOurDefense)
					{
						m_unit->getGroup()->pushMission(MISSION_AIRPATROL);
						return true;
					}
				}
			}
		}
	}

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	foreach_(CvCity * cityX, GET_PLAYER(m_unit->getOwner()).cities())
	{
		CvPlot* plotX = cityX->plot();
		if (m_unit->canAirDefend(plotX) && (m_unit->atPlot(plotX) || m_unit->canEnterPlot(plotX)))
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
					- m_unit->atPlot(plotX)
				);
			const int iNeedAirDefenders = cityX->AI_neededAirDefenders();

			if (iNeedAirDefenders > iExistingAirDefenders)
			{
				int iValue = cityX->getPopulation() + cityX->AI_cityThreat();

				const int iOurDefense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(plotX, 0, true, false, true);
				const int iEnemyOffense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(plotX, 2, false, false);

				iValue *= 100;

				// Increase value of cities needing air defense more
				iValue *= std::max(1, 3 + iNeedAirDefenders - iExistingAirDefenders);

				if (GET_PLAYER(m_unit->getOwner()).AI_isPrimaryArea(cityX->area()))
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

	if (pBestPlot != NULL && !m_unit->atPlot(pBestPlot))
	{
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::airCarrier()
{
	//PROFILE_FUNC();

	if (m_unit->hasCargo())
	{
		return false;
	}

	if (m_unit->isCargo())
	{
		if (m_unit->canAirDefend())
		{
			m_unit->getGroup()->pushMission(MISSION_AIRPATROL);
			return true;
		}
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return true;
	}

	int iBestValue = 0;
	CvUnit* pBestUnit = NULL;

	foreach_(CvUnit * unitX, GET_PLAYER(m_unit->getOwner()).units())
	{
		if (m_unit->canLoadOntoUnit(unitX, unitX->plot()))
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
			else if (m_unit->isEnemy(unitX->plot()->getTeam(), unitX->plot()))
			{
				iValue += 20;
			}

			if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
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
		if (m_unit->atPlot(pBestUnit->plot()))
		{
			m_unit->setTransportUnit(pBestUnit); // XXX is m_unit dangerous (not pushing a mission...) XXX air units?
			return true;
		}
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestUnit->getX(), pBestUnit->getY());
	}
	return false;
}

bool CvUnitDefaultAI::missileLoad(UnitAITypes eTargetUnitAI, int iMaxOwnUnitAI, bool bStealthOnly)
{
	//PROFILE_FUNC();

	CvUnit* pBestUnit = NULL;
	int iBestValue = 0;

	const bool bSM = GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS);
	const int iCargoValueFactorSM =
		(
			bSM ? GC.getGame().getBaseMissileUnitIncrementsbyCargoVolume() : 0
		);
	FAssert(!bSM || iCargoValueFactorSM > 0);

	foreach_(CvUnit * unitX, GET_PLAYER(m_unit->getOwner()).units())
	{
		if ((!bStealthOnly || unitX->getInvisibleType() != NO_INVISIBLE)
		&& unitX->AI()->getUnitAIType() == eTargetUnitAI
		&& (iMaxOwnUnitAI == -1 || unitX->getUnitAICargo(getUnitAIType()) <= iMaxOwnUnitAI)
		&& m_unit->canLoadOntoUnit(unitX, unitX->plot()))
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
		if (m_unit->atPlot(pBestUnit->plot()))
		{
			m_unit->setTransportUnit(pBestUnit); // XXX is m_unit dangerous (not pushing a mission...) XXX air units?
			return true;
		}
		if (m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestUnit->getX(), pBestUnit->getY()))
		{
			m_unit->setTransportUnit(pBestUnit);
			return true;
		}
	}
	return false;

}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::airStrike()
{
	//PROFILE_FUNC();

	CvUnit* pDefender;
	CvUnit* pInterceptor;
	int iDamage;
	int iInterceptProb;

	const int iSearchRange = m_unit->airRange();

	int iBestValue = (m_unit->isSuicide() && m_unit->getUnitInfo().getProductionCost() > 0) ? (5 * m_unit->getUnitInfo().getProductionCost()) / 6 : 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		if (m_unit->canEnterPlot(pLoopPlot, MoveCheck::Attack))
		{
			int iValue = 0;
			int iPotentialAttackers = GET_PLAYER(m_unit->getOwner()).AI_adjacentPotentialAttackers(pLoopPlot);
			if (pLoopPlot->isCity())
			{
				iPotentialAttackers += GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_ASSAULT, m_unit->getGroup(), 1) * 2;
			}
			/********************************************************************************/
			/* 	BETTER_BTS_AI_MOD						10/13/08		jdog5000		*/
			/* 																			*/
			/* 	Air AI																	*/
			/********************************************************************************/
			/* original BTS code
			if (pLoopPlot->isWater() || (iPotentialAttackers > 0) || pLoopPlot->isAdjacentTeam(m_unit->getTeam()))
			*/
			// Bombers will always consider striking units adjacent to m_unit team's territory
			// to soften them up for potential attack.  This situation doesn't apply if m_unit team's adjacent
			// territory is water, land units won't be able to reach easily for attack
			if (pLoopPlot->isWater() || (iPotentialAttackers > 0) || pLoopPlot->isAdjacentTeam(m_unit->getTeam(), true))
				/********************************************************************************/
				/* 	BETTER_BTS_AI_MOD						END								*/
				/********************************************************************************/
			{
				pDefender = pLoopPlot->getBestDefender(NO_PLAYER, m_unit->getOwner(), m_unit, true);

				if (pDefender == NULL)
				{
					return false;
				}

				FAssert(pDefender != NULL && pDefender->canDefend());

				if (pDefender != NULL && pDefender->canDefend())
				{
					// XXX factor in air defenses...

					iDamage = m_unit->airCombatDamage(pDefender);

					iValue = std::max(0, (std::min((pDefender->getDamage() + iDamage), m_unit->airCombatLimit(pDefender)) - pDefender->getDamage()));

					iValue += ((((iDamage * m_unit->collateralDamage()) / 100) * std::min((pLoopPlot->getNumVisiblePotentialEnemyDefenders(m_unit) - 1), m_unit->collateralDamageMaxUnits())) / 2);

					iValue *= (3 + iPotentialAttackers);
					iValue /= 4;

					pInterceptor = m_unit->bestInterceptor(pLoopPlot);

					if (pInterceptor != NULL)
					{
						iInterceptProb = m_unit->isSuicide() ? 100 : pInterceptor->currInterceptionProbability();

						iInterceptProb *= std::max(0, (100 - m_unit->evasionProbability()));
						iInterceptProb /= 100;

						iValue *= std::max(0, 100 - iInterceptProb / 2);
						iValue /= 100;
					}

					if (pLoopPlot->isWater())
					{
						iValue *= 3;
					}

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = pLoopPlot;
						FAssert(!m_unit->atPlot(pBestPlot));
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
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
bool CvUnitDefaultAI::defensiveAirStrike()
{
	PROFILE_FUNC();

	const int iSearchRange = m_unit->airRange();

	int iBestValue = (m_unit->isSuicide() && m_unit->getUnitInfo().getProductionCost() > 0) ? (60 * m_unit->getUnitInfo().getProductionCost()) : 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		if (m_unit->canEnterPlot(pLoopPlot, MoveCheck::Attack)) // Only true of plots m_unit unit can airstrike
		{
			// Only attack enemy land units near our cities
			if (pLoopPlot->isPlayerCityRadius(m_unit->getOwner()) && !pLoopPlot->isWater())
			{
				const CvCity* pClosestCity = GC.getMap().findCity(pLoopPlot->getX(), pLoopPlot->getY(), m_unit->getOwner(), m_unit->getTeam(), true, false);

				if (pClosestCity != NULL)
				{
					// City and pLoopPlot forced to be in same m_unit->area, check they're still close
					int iStepDist = plotDistance(pClosestCity->getX(), pClosestCity->getY(), pLoopPlot->getX(), pLoopPlot->getY());

					if (iStepDist < 3)
					{
						const CvUnit* pDefender = pLoopPlot->getBestDefender(NO_PLAYER, m_unit->getOwner(), m_unit, true);

						if (pDefender != NULL && pDefender->canDefend())
						{
							const int iDamage = m_unit->airCombatDamage(pDefender);

							int iValue = std::max(0, (std::min((pDefender->getDamage() + iDamage), m_unit->airCombatLimit(pDefender)) - pDefender->getDamage()));

							iValue += ((((iDamage * m_unit->collateralDamage()) / 100) * std::min((pLoopPlot->getNumVisiblePotentialEnemyDefenders(m_unit) - 1), m_unit->collateralDamageMaxUnits())) / 2);

							iValue *= GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(pClosestCity->plot(), 2, false, false);
							iValue /= std::max(1, GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(pClosestCity->plot(), 0, true, false, true));

							if (iStepDist == 1)
							{
								iValue *= 5;
								iValue /= 4;
							}

							const CvUnit* pInterceptor = m_unit->bestInterceptor(pLoopPlot);

							if (pInterceptor != NULL)
							{
								int iInterceptProb = m_unit->isSuicide() ? 100 : pInterceptor->currInterceptionProbability();

								iInterceptProb *= std::max(0, (100 - m_unit->evasionProbability()));
								iInterceptProb /= 100;

								iValue *= std::max(0, 100 - iInterceptProb / 2);
								iValue /= 100;
							}

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = pLoopPlot;
								FAssert(!m_unit->atPlot(pBestPlot));
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}

	return false;
}

// Air strike around base city
// Returns true if a mission was pushed...
bool CvUnitDefaultAI::defendBaseAirStrike()
{
	PROFILE_FUNC();

	// Only search around base
	int iSearchRange = 2;

	int iBestValue = (m_unit->isSuicide() && m_unit->getUnitInfo().getProductionCost() > 0) ? (15 * m_unit->getUnitInfo().getProductionCost()) : 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		if (m_unit->canEnterPlot(pLoopPlot, MoveCheck::Attack) && !pLoopPlot->isWater()) // Only true of plots m_unit unit can airstrike
		{
			if (m_unit->plot()->area() == pLoopPlot->area())
			{
				const CvUnit* pDefender = pLoopPlot->getBestDefender(NO_PLAYER, m_unit->getOwner(), m_unit, true);

				FAssert(pDefender != NULL && pDefender->canDefend());

				if (pDefender != NULL && pDefender->canDefend())
				{

					const int iDamage = m_unit->airCombatDamage(pDefender);

					int iValue = std::max(0, (std::min((pDefender->getDamage() + iDamage), m_unit->airCombatLimit(pDefender)) - pDefender->getDamage()));

					iValue += ((iDamage * m_unit->collateralDamage()) * std::min((pLoopPlot->getNumVisiblePotentialEnemyDefenders(m_unit) - 1), m_unit->collateralDamageMaxUnits())) / (2 * 100);

					// Weight towards stronger units
					iValue *= (pDefender->currCombatStr(NULL, NULL, NULL) + 2000);
					iValue /= 2000;

					// Weight towards adjacent stacks
					if (plotDistance(m_unit->getX(), m_unit->getY(), pLoopPlot->getX(), pLoopPlot->getY()) == 1)
					{
						iValue *= 5;
						iValue /= 4;
					}

					const CvUnit* pInterceptor = m_unit->bestInterceptor(pLoopPlot);

					if (pInterceptor != NULL)
					{
						int iInterceptProb = m_unit->isSuicide() ? 100 : pInterceptor->currInterceptionProbability();

						iInterceptProb *= std::max(0, (100 - m_unit->evasionProbability()));
						iInterceptProb /= 100;

						iValue *= std::max(0, 100 - iInterceptProb / 2);
						iValue /= 100;
					}

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = pLoopPlot;
						FAssert(!m_unit->atPlot(pBestPlot));
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}

	return false;
}
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						END								*/
/********************************************************************************/

bool CvUnitDefaultAI::airBombPlots()
{
	//PROFILE_FUNC();

	const int iSearchRange = m_unit->airRange();

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		if (!pLoopPlot->isCity() && pLoopPlot->isOwned() && pLoopPlot != m_unit->plot())
		{
			if (m_unit->canAirBombAt(m_unit->plot(), pLoopPlot->getX(), pLoopPlot->getY()))
			{
				int iValue = 0;

				if (pLoopPlot->getBonusType(pLoopPlot->getTeam()) != NO_BONUS)
				{
					iValue += pillageValue(pLoopPlot, 15);

					iValue += GC.getGame().getSorenRandNum(10, "AI Air Bomb");
				}
				else if (m_unit->isSuicide())
				{
					//This should only be reached when the unit is desperate to die
					iValue += pillageValue(pLoopPlot);
					// Guided missiles lean towards destroying resource-producing tiles as opposed to improvements like Towns
					if (pLoopPlot->getBonusType(pLoopPlot->getTeam()) != NO_BONUS)
					{
						//and even more so if it's a resource
						iValue += GET_PLAYER(pLoopPlot->getOwner()).AI_bonusVal(pLoopPlot->getBonusType(pLoopPlot->getTeam()));
					}
				}

				if (iValue > 0)
				{
					const CvUnit* pInterceptor = m_unit->bestInterceptor(pLoopPlot);

					if (pInterceptor != NULL)
					{
						int iInterceptProb = m_unit->isSuicide() ? 100 : pInterceptor->currInterceptionProbability();

						iInterceptProb *= std::max(0, (100 - m_unit->evasionProbability()));
						iInterceptProb /= 100;

						iValue *= std::max(0, 100 - iInterceptProb / 2);
						iValue /= 100;
					}

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = pLoopPlot;
						FAssert(!m_unit->atPlot(pBestPlot));
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		m_unit->getGroup()->pushMission(MISSION_AIRBOMB, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}

	return false;
}


bool CvUnitDefaultAI::airBombDefenses()
{
	//PROFILE_FUNC();

	const int iSearchRange = m_unit->airRange();

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		const CvCity* pCity = pLoopPlot->getPlotCity();
		if (pCity != NULL)
		{
			int iValue = 0;

			if (m_unit->canAirBombAt(m_unit->plot(), pLoopPlot->getX(), pLoopPlot->getY()))
			{
				int iPotentialAttackers = GET_PLAYER(m_unit->getOwner()).AI_adjacentPotentialAttackers(pLoopPlot);
				iPotentialAttackers += std::max(0, GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pCity->plot(), NO_MISSIONAI, m_unit->getGroup(), 2) - 4);

				if (iPotentialAttackers > 1)
				{
					iValue += std::max(0, (std::min((pCity->getDefenseDamage() + m_unit->getAirBombCurrRate()), GC.getMAX_CITY_DEFENSE_DAMAGE()) - pCity->getDefenseDamage()));

					iValue *= 4 + iPotentialAttackers;

					if (pCity->AI_isDanger())
					{
						iValue *= 2;
					}

					if (pCity == pCity->area()->getTargetCity(m_unit->getOwner()))
					{
						iValue *= 2;
					}
				}

				if (iValue > 0)
				{
					const CvUnit* pInterceptor = m_unit->bestInterceptor(pLoopPlot);

					if (pInterceptor != NULL)
					{
						int iInterceptProb = m_unit->isSuicide() ? 100 : pInterceptor->currInterceptionProbability();

						iInterceptProb *= std::max(0, (100 - m_unit->evasionProbability()));
						iInterceptProb /= 100;

						iValue *= std::max(0, 100 - iInterceptProb / 2);
						iValue /= 100;
					}

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = pLoopPlot;
						FAssert(!m_unit->atPlot(pBestPlot));
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		m_unit->getGroup()->pushMission(MISSION_AIRBOMB, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}

	return false;

}

bool CvUnitDefaultAI::exploreAir()
{
	PROFILE_FUNC();

	const CvPlayer& player = GET_PLAYER(m_unit->getOwner());
	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(m_unit->getTeam(), false))
		{
			const bool bEnemy = m_unit->isEnemy(GET_PLAYER((PlayerTypes)iI).getTeam());

			foreach_(const CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
			{
				if (!pLoopCity->isVisible(m_unit->getTeam(), false)
				&& m_unit->canReconAt(m_unit->plot(), pLoopCity->getX(), pLoopCity->getY()))
				{
					int iValue = 1 + GC.getGame().getSorenRandNum(15, "AI explore air");
					if (bEnemy)
					{
						iValue += 10;
						iValue += std::min(10, pLoopCity->area()->getNumAIUnits(m_unit->getOwner(), UNITAI_ATTACK_CITY));
						iValue += 10 * player.AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_ASSAULT);
					}
					iValue *= plotDistance(m_unit->getX(), m_unit->getY(), pLoopCity->getX(), pLoopCity->getY());

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
		m_unit->getGroup()->pushMission(MISSION_RECON, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}
	return false;
}


int CvUnitDefaultAI::exploreAirPlotValue(const CvPlot* pPlot) const
{
	int iValue = 0;
	if (pPlot->isVisible(m_unit->getTeam(), false))
	{
		iValue++;

		if (!pPlot->isOwned())
		{
			iValue++;
		}

		if (!pPlot->isImpassable(m_unit->getTeam())) // added m_unit->getTeam()
		{
			iValue *= 4;

			if (pPlot->isWater() || pPlot->getArea() == m_unit->getArea())
			{
				iValue *= 2;
			}
		}
	}

	return iValue;
}

bool CvUnitDefaultAI::exploreAir2()
{
	PROFILE_FUNC();

	const CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	const int iSearchRange = m_unit->airRange();
	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange)
	| filtered(!CvPlot::fn::isVisible(m_unit->getTeam(), false)))
	{
		if (m_unit->canReconAt(m_unit->plot(), pLoopPlot->getX(), pLoopPlot->getY()))
		{
			int iValue = exploreAirPlotValue(pLoopPlot);

			foreach_(const CvPlot * pAdjacentPlot, m_unit->plot()->adjacent()
			| filtered(CvPlot::fn::isVisible(m_unit->getTeam(), false)))
			{
				iValue += exploreAirPlotValue(pAdjacentPlot);
			}

			iValue += GC.getGame().getSorenRandNum(25, "AI explore air");
			iValue *= std::min(7, plotDistance(m_unit->getX(), m_unit->getY(), pLoopPlot->getX(), pLoopPlot->getY()));

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestPlot = pLoopPlot;
			}
		}
	}

	if (pBestPlot != NULL)
	{
		m_unit->getGroup()->pushMission(MISSION_RECON, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}

	return false;
}

void CvUnitDefaultAI::exploreAirMove()
{
	if (exploreAir() || exploreAir2())
	{
		return;
	}
	if (m_unit->canAirDefend())
	{
		m_unit->getGroup()->pushMission(MISSION_AIRPATROL);
		return;
	}
	m_unit->getGroup()->pushMission(MISSION_SKIP);

	return;
}


// Returns true if a mission was pushed...
bool CvUnitDefaultAI::nuke()
{
	PROFILE_FUNC();

	const CvCity* pBestCity = NULL;
	int iBestValue = 0;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && m_unit->isEnemy(GET_PLAYER((PlayerTypes)iI).getTeam())
		&& GET_PLAYER(m_unit->getOwner()).AI_getAttitude((PlayerTypes)iI) == ATTITUDE_FURIOUS)
		{
			foreach_(const CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
			{
				if (m_unit->canNukeAt(m_unit->plot(), pLoopCity->getX(), pLoopCity->getY()))
				{
					const int iValue = nukeValue(pLoopCity);

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestCity = pLoopCity;
						FAssert(pBestCity->getTeam() != m_unit->getTeam());
					}
				}
			}
		}
	}
	if (pBestCity != NULL)
	{
		m_unit->getGroup()->pushMission(MISSION_NUKE, pBestCity->getX(), pBestCity->getY());
		return true;
	}
	return false;
}

bool CvUnitDefaultAI::nukeRange(int iRange)
{
	const CvPlot* pBestPlot = NULL;
	int iBestValue = 0;
	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iRange, iRange))
	{
		if (m_unit->canNukeAt(m_unit->plot(), pLoopPlot->getX(), pLoopPlot->getY()))
		{
			int iValue = -99;

			foreach_(const CvPlot * pLoopPlot2, pLoopPlot->rect(m_unit->nukeRange(), m_unit->nukeRange()))
			{
				int iEnemyCount = 0;
				int iTeamCount = 0;
				int iNeutralCount = 0;
				int iDamagedEnemyCount = 0;

				foreach_(const CvUnit * pLoopUnit, pLoopPlot2->units()
				| filtered(!CvUnit::fn::isNukeImmune()))
				{
					if (pLoopUnit->getTeam() == m_unit->getTeam())
					{
						iTeamCount++;
					}
					else if (!pLoopUnit->isInvisible(m_unit->getTeam(), false))
					{
						if (m_unit->isEnemy(pLoopUnit->getTeam()))
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
				if (pLoopPlot2->getTeam() == m_unit->getTeam())
				{
					iMultiplier = -2;
				}
				else if (m_unit->isEnemy(pLoopPlot2->getTeam()))
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

				if (pLoopPlot2->getNonObsoleteBonusType(m_unit->getTeam()) != NO_BONUS)
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
		m_unit->getGroup()->pushMission(MISSION_NUKE, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}

	return false;
}

bool CvUnitDefaultAI::trade(int iValueThreshold)
{
	int iPathTurns;
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(m_unit->getTeam(), false))
		{
			foreach_(const CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
			{
				if (plotValid(pLoopCity->plot()))
				{
					int iValue = m_unit->getTradeGold(pLoopCity->plot());

					if (iValue >= iValueThreshold && m_unit->canTrade(pLoopCity->plot(), true)
					&& generateSafePathforVulnerable(pLoopCity->plot(), &iPathTurns))
					{
						iValue /= 4 + iPathTurns;

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

	if (pBestPlot != NULL)
	{
		if (!m_unit->atPlot(pBestPlot))
		{
			if (generateSafePathforVulnerable(pBestPlot, &iPathTurns))
			{
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_IGNORE_DANGER);
			}
			m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
			return true;
		}
		return m_unit->getGroup()->pushMissionInternal(MISSION_TRADE, pBestPlot->getX(), pBestPlot->getY());
	}
	return false;
}


bool CvUnitDefaultAI::infiltrate()
{
	PROFILE_FUNC();

	if (m_unit->canInfiltrate(m_unit->plot()))
	{
		m_unit->getGroup()->pushMission(MISSION_INFILTRATE);
		return true;
	}
	if (m_unit->canInfiltrate())
	{
		CvPlot* pBestPlot = NULL;
		int iBestValue = 0;
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(m_unit->getTeam(), false))
			{
				foreach_(const CvCity * cityX, GET_PLAYER((PlayerTypes)iI).cities())
				{
					if (m_unit->isInvisible(cityX->getTeam(), false, true))
					{
						// BBAI efficiency: check m_unit->area for land units before generating path
						if (m_unit->getDomainType() == DOMAIN_LAND && cityX->area() != m_unit->area() && !m_unit->getGroup()->canMoveAllTerrain())
						{
							continue;
						}
						int iValue = m_unit->getEspionagePoints();
						int iPathTurns;

						if (iValue > iBestValue && m_unit->generatePath(cityX->plot(), 0, true, &iPathTurns))
						{
							FAssert(iPathTurns > 0);

							if (m_unit->getPathMovementRemaining() == 0)
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
			if (m_unit->atPlot(pBestPlot))
			{
				m_unit->getGroup()->pushMission(MISSION_INFILTRATE);
				return true;
			}
			if (m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY()))
			{
				m_unit->getGroup()->pushMission(MISSION_INFILTRATE, -1, -1, 0, (m_unit->getGroup()->getLengthMissionQueue() > 0));
				return true;
			}
		}
	}
	return false;
}


bool CvUnitDefaultAI::reconSpy(int iRange)
{
	PROFILE_FUNC();

	const CvPlot* pBestPlot = NULL;
	CvPlot* pBestTargetPlot = NULL;
	int iBestValue = 0;

	int iSearchRange = searchRange(iRange);

	for (int iX = -iSearchRange; iX <= iSearchRange; iX++)
	{
		for (int iY = -iSearchRange; iY <= iSearchRange; iY++)
		{
			const CvPlot* pLoopPlot = plotXY(m_unit->getX(), m_unit->getY(), iX, iY);
			int iDistance = stepDistance(0, 0, iX, iY);
			if ((iDistance > 0) && (pLoopPlot != NULL) && plotValid(pLoopPlot))
			{
				int iValue = 0;
				if (pLoopPlot->getPlotCity() != NULL)
				{
					iValue += GC.getGame().getSorenRandNum(4000, "AI Spy Scout City");
				}

				if (pLoopPlot->getBonusType(m_unit->getTeam()) != NO_BONUS)
				{
					iValue += GC.getGame().getSorenRandNum(1000, "AI Spy Recon Bonus");
				}

				foreach_(const CvPlot * pAdjacentPlot, pLoopPlot->adjacent())
				{
					if (!pAdjacentPlot->isRevealed(m_unit->getTeam(), false))
					{
						iValue += 500;
					}
					else if (!pAdjacentPlot->isVisible(m_unit->getTeam(), false))
					{
						iValue += 200;
					}
				}


				if (iValue > 0)
				{
					int iPathTurns;
					if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns, iRange))
					{
						if (iPathTurns <= iRange)
						{
							// don't give each and every m_unit->plot in range a value before generating the patch (performance hit)
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
								pBestTargetPlot = m_unit->getPathEndTurnPlot();
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
		if (m_unit->atPlot(pBestTargetPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
		else
		{
			if (m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestTargetPlot->getX(), pBestTargetPlot->getY()))
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP);
				return true;
			}
		}
	}

	return false;
}

/// \brief Spy decision on whether to cause revolt in besieged city
///
/// Have spy breakdown city defenses if we have troops in position to capture city m_unit turn.
bool CvUnitDefaultAI::revoltCitySpy()
{
	PROFILE_FUNC();

	CvCity* pCity = m_unit->plot()->getPlotCity();


	if (pCity == NULL)
	{
		FErrorMsg("City is not expected to be NULL");
		return false;
	}

	if (!(GET_TEAM(m_unit->getTeam()).isAtWar(pCity->getTeam())))
	{
		return false;
	}

	if (pCity->isDisorder())
	{
		return false;
	}

	int iOurPower = GET_PLAYER(m_unit->getOwner()).AI_getOurPlotStrength(m_unit->plot(), 1, false, true);
	int iEnemyDefensePower = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 0, true, false);
	int iEnemyPostPower = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), 0, false, false);

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
			if (!GET_PLAYER(m_unit->getOwner()).canDoEspionageMission((EspionageMissionTypes)iMission, pCity->getOwner(), pCity->plot(), -1, m_unit))
			{
				continue;
			}

			if (!m_unit->espionage((EspionageMissionTypes)iMission, -1))
			{
				continue;
			}

			return true;
		}
	}

	return false;
}

int CvUnitDefaultAI::getEspionageTargetValue(const CvPlot* pPlot, int iMaxPath, int iMinUsefulValue) const
{
	PROFILE_FUNC();

	const CvTeamAI& kTeam = GET_TEAM(m_unit->getTeam());
	int iValue = 0;

	if (pPlot->isOwned() && pPlot->getTeam() != m_unit->getTeam() && !GET_TEAM(m_unit->getTeam()).isVassal(pPlot->getTeam()))
	{
		if (plotValid(pPlot))
		{
			const CvCity* pCity = pPlot->getPlotCity();
			if (pCity != NULL)
			{
				iValue += pCity->getPopulation();
				iValue += pCity->plot()->calculateCulturePercent(m_unit->getOwner()) / 8;

				// BBAI TODO: Should go to cities where missions will be cheaper ...

				const int iRand = GC.getGame().getSorenRandNum(6, "AI spy choose city");
				iValue += iRand * iRand;

				if (m_unit->area()->getTargetCity(m_unit->getOwner()) == pCity)
				{
					iValue += 30;
				}

				if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pPlot, MISSIONAI_ASSAULT, m_unit->getGroup()) > 0)
				{
					iValue += 30;
				}

				// BBAI TODO: What else?  If can see production, go for wonders and space race ...
			}
			else
			{
				const BonusTypes eBonus = pPlot->getNonObsoleteBonusType(m_unit->getTeam());
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
			iPathableValue /= (3 + GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pPlot, MISSIONAI_ATTACK_SPY, m_unit->getGroup()));

			int iPathTurns;
			if (iPathableValue > iMinUsefulValue && m_unit->generatePath(pPlot, 0, true, &iPathTurns, iMaxPath))
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


bool CvUnitDefaultAI::cityOffenseSpy(const int iMaxPath, const CvCity* pSkipCity)
{
	PROFILE_FUNC();

	const TeamTypes eTeam = m_unit->getTeam();
	const CvTeamAI& team = GET_TEAM(eTeam);
	const CvPlayerAI& player = GET_PLAYER(m_unit->getOwner());
	const CvArea* pArea = m_unit->area();

	const bool bValid = team.getBestKnownTechScorePercent() < 85;

	CvReachablePlotSet plotSet(m_unit->getGroup(), 0, MAX_INT);
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
					if (cityX->area() == pArea || m_unit->canMoveAllTerrain())
					{
						CvPlot* plotX = cityX->plot();

						if (plotSet.find(plotX) != plotSet.end())
						{
							const int iValue = getEspionageTargetValue(plotX, iMaxPath, iBestValue);
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
		if (m_unit->atPlot(pBestPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_ATTACK_SPY);
			return true;
		}
		else if (m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_ATTACK_SPY))
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_ATTACK_SPY);
			return true;
		}
	}
	return false;
}

bool CvUnitDefaultAI::bonusOffenseSpy(int iRange)
{
	PROFILE_FUNC();

	const CvPlot* pBestPlot = NULL;
	int iBestValue = 10;

	const int iSearchRange = searchRange(iRange);

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		if (pLoopPlot->getBonusType(m_unit->getTeam()) != NO_BONUS
		&& pLoopPlot->isOwned() && pLoopPlot->getTeam() != m_unit->getTeam())
		{
			// Only move to plots where we will run missions
			if (GET_PLAYER(m_unit->getOwner()).AI_getAttitudeWeight(pLoopPlot->getOwner()) < 51
			|| GET_TEAM(m_unit->getTeam()).AI_getWarPlan(pLoopPlot->getTeam()) != NO_WARPLAN)
			{
				const int iValue = getEspionageTargetValue(pLoopPlot, iRange, iBestValue);
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
		if (m_unit->atPlot(pBestPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_ATTACK_SPY);
			return true;
		}
		else if (m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_ATTACK_SPY))
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_ATTACK_SPY);
			return true;
		}
	}

	return false;
}

//Returns true if the spy performs espionage.
bool CvUnitDefaultAI::espionageSpy()
{
	PROFILE_FUNC();

	if (!m_unit->canEspionage(m_unit->plot()))
	{
		return false;
	}

	CvPlot* pTargetPlot = NULL;
	PlayerTypes eTargetPlayer = NO_PLAYER;
	int iExtraData = -1;

	EspionageMissionTypes eBestMission = GET_PLAYER(m_unit->getOwner()).AI_bestPlotEspionage(m_unit->plot(), eTargetPlayer, pTargetPlot, iExtraData);
	if (NO_ESPIONAGEMISSION == eBestMission)
	{
		return false;
	}

	if (!GET_PLAYER(m_unit->getOwner()).canDoEspionageMission(eBestMission, eTargetPlayer, pTargetPlot, iExtraData, m_unit))
	{
		return false;
	}

	if (!m_unit->espionage(eBestMission, iExtraData))
	{
		return false;
	}

	return true;
}

bool CvUnitDefaultAI::moveToStagingCity()
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
	CvTeam& kTeam = GET_TEAM(m_unit->getTeam());
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != m_unit->getTeam() && GET_TEAM((TeamTypes)iI).isAlive() && kTeam.AI_isSneakAttackPreparing((TeamTypes)iI))
		{
			//OutputDebugString(CvString::format("AI_moveToStagingCity - potential target team %d\n", iI).c_str());
			eTargetTeam = (TeamTypes)iI;
			iWarCount++;
		}
	}
	if (iWarCount > 1)
	{
		PROFILE("CvUnitDefaultAI::moveToStagingCity.MultiTargetTeams");

		//OutputDebugString("AI_moveToStagingCity - no target team\n");
		eTargetTeam = NO_TEAM;
	}

	CvReachablePlotSet plotSet(m_unit->getGroup(), 0, MAX_INT);

	//	Do a rough pathing to arrange the cities into a heuristically optimal order so
	//	that the detailed pathing is most likely to get the best cities first, and hence generate lower
	//	cutoffs for max path on later ones (and hence execute faster)
	std::multimap<int, std::pair<CvCity*, int> > orderedTargets;
	foreach_(CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
	{
		PROFILE("AI_moveToStagingCity.Valuation");

		// BBAI efficiency: check same m_unit->area
		if (pLoopCity->area() == m_unit->area())
		{
			CvReachablePlotSet::const_iterator itr = plotSet.find(pLoopCity->plot());

			if (itr != plotSet.end())
			{
				//	If we're not at war with anyone and we are not inside our own territory
				//	then we're just trying to return home - consider all cities equal
				if (iWarCount == 0 && m_unit->plot()->getOwner() != m_unit->getOwner())
				{
					iValue = 100;
				}
				else
				{
					// BBAI TODO: Need some knowledge of whether m_unit is a good city to attack from ... only get that
					// indirectly from threat.
					iValue = pLoopCity->AI_cityThreat(eTargetTeam);
				}

				// Have attack stacks in assault m_unit->areas move to coastal cities for faster loading
				if ((m_unit->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_ASSAULT) || (m_unit->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_ASSAULT_MASSING))
				{
					CvArea* pWaterArea = pLoopCity->waterArea();
					if (pWaterArea != NULL && GET_TEAM(m_unit->getTeam()).AI_isWaterAreaRelevant(pWaterArea))
					{
						// BBAI TODO:  Need a better way to determine which cities should serve as invasion launch locations

						// Inertia so units don't just chase transports around the map
						iValue = iValue / 2;
						if (pLoopCity->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_ASSAULT)
						{
							// If in assault, transports may be at sea ... tend to stay where they left from
							// to speed reinforcement
							iValue += pLoopCity->plot()->plotCount(PUF_isAvailableUnitAITypeGroupie, UNITAI_ATTACK_CITY, -1, NULL, m_unit->getOwner());
						}

						// Attraction to cities which are serving as launch/pickup points
						iValue += 3 * pLoopCity->plot()->plotCount(PUF_isUnitAIType, UNITAI_ASSAULT_SEA, -1, NULL, m_unit->getOwner());
						iValue += 2 * pLoopCity->plot()->plotCount(PUF_isUnitAIType, UNITAI_ESCORT_SEA, -1, NULL, m_unit->getOwner());
						if (!GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
						{
							iValue += 5 * GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_PICKUP);
						}
						else
						{
							iValue += 5 * (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIsinCargoVolume(pLoopCity->plot(), MISSIONAI_PICKUP) / 100);
						}
					}
					else
					{
						iValue = iValue / 8;
					}
				}

				if ((pLoopCity->plot() != m_unit->plot()) && pLoopCity->isVisible(eTargetTeam, false))
				{
					iValue /= 2;
				}

				iValue *= 1000;

				//	Do a first-stage cut-off even on the rough paths to eliminate particualrly unlikely
				//	candidates.  The +4 in m_unit calculation (lack of -4 relative to the vrsion in the final
				//	detailed evaluation) is to allow a little leeway since the optimal
				//	paths may vary somewhat from the rough ones.  The 2* is because the plotSet step distance
				//	is route-blind
				int iMaxTurns = (iBestValue == 0 ? MAX_INT : 2 * iValue / iBestValue);

				//OutputDebugString(CvString::format("Generate rough path to (%d,%d) [value %d] with maxTurns=%d\n", pLoopCity->getX(), pLoopCity->getY(), iValue, iMaxTurns).c_str());
				//if ( m_unit->generatePath(pLoopCity->plot(), MOVE_IGNORE_DANGER | MOVE_THROUGH_ENEMY, true, &iPathTurns, iMaxTurns, 0) )
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

		if (m_unit->isNPC())
		{
			iMinPath = std::max(1, stepDistance(m_unit->getX(), m_unit->getY(), pLoopCity->getX(), pLoopCity->getY()) / m_unit->baseMoves());
		}
		else
		{
			iMinPath = 1;
		}

		//OutputDebugString(CvString::format("City (%d,%d), value=%d, minPath=%d\n", pLoopCity->getX(), pLoopCity->getY(), iValue).c_str());
		if (iValue / (5 + iMinPath) > iBestValue)
		{
			PROFILE("CvUnitDefaultAI::moveToStagingCity.Pathing");

			int iMaxTurns = (iBestValue == 0 ? MAX_INT : iValue / iBestValue - 4);

			//OutputDebugString(CvString::format("Attempt pathing with maxPath=%d\n", iMaxTurns).c_str());
			if (m_unit->generatePath(pLoopCity->plot(), MOVE_IGNORE_DANGER, true, &iPathTurns, iMaxTurns))
			{
				PROFILE("CvUnitDefaultAI::moveToStagingCity.DangerCheck");
				OutputDebugString(CvString::format("Pathing suceeded with path len %d\n", iPathTurns).c_str());

				iValue /= (5 + iPathTurns);

				if (iValue > iBestValue)
				{
					endTurnPlot = m_unit->getPathEndTurnPlot();

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
		if (m_unit->atPlot(pBestPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_GROUP, pBestCity->plot());
	}
	return false;
}

/*
bool CvUnitDefaultAI::seaRetreatFromCityDanger()
{
	if (m_unit->plot()->isCity(true) && GET_PLAYER(m_unit->getOwner()).AI_getPlotDanger(m_unit->plot(), 2) > 0) //prioritize getting outta there
	{
		if (anyAttack(2, 40))
		{
			return true;
		}

		if (anyAttack(4, 50))
		{
			return true;
		}

		if (retreatToCity())
		{
			return true;
		}

		if (safety())
		{
			return true;
		}
	}
	return false;
}

bool CvUnitDefaultAI::airRetreatFromCityDanger()
{
	if (m_unit->plot()->isCity(true))
	{
		CvCity* pCity = m_unit->plot()->getPlotCity();
		if (GET_PLAYER(m_unit->getOwner()).AI_getPlotDanger(m_unit->plot(), 2) > 0 || (pCity != NULL && !pCity->AI_isDefended()))
		{
			if (airOffensiveCity())
			{
				return true;
			}

			if (m_unit->canAirDefend() && airDefensiveCity())
			{
				return true;
			}
		}
	}
	return false;
}

bool CvUnitDefaultAI::airAttackDamagedSkip()
{
	if (m_unit->getDamage() == 0)
	{
		return false;
	}

	bool bSkip = (m_unit->getHP() * 100 / m_unit->getMaxHP() < 40);
	if (!bSkip)
	{
		int iSearchRange = m_unit->airRange();
		bool bSkiesClear = true;
		for (int iDX = -iSearchRange; iDX <= iSearchRange && bSkiesClear; iDX++)
		{
			for (int iDY = -iSearchRange; iDY <= iSearchRange && bSkiesClear; iDY++)
			{
				CvPlot* pLoopPlot = plotXY(m_unit->getX(), m_unit->getY(), iDX, iDY);
				if (pLoopPlot != NULL)
				{
					if (m_unit->bestInterceptor(pLoopPlot) != NULL)
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
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return true;
	}

	return false;
}
*/

// Returns true if a mission was pushed or we should wait for another unit to bombard...
bool CvUnitDefaultAI::followBombard()
{
	/************************************************************************************************/
	/* REVOLUTIONDCM							05/24/08								Glider1	 */
	/*																							  */
	/*																							  */
	/************************************************************************************************/

	if (m_unit->getGroup()->canBombard(m_unit->plot()))
	{
		// RevolutionDCM - ranged bombard AI wraps standard bombard
		// TBHERE : This is opposite.  I like the solution above to limit the units that will try m_unit
		// and keep attackers otherwise for city attacks.(sorta)
		// But it's opposite... it should check mission bombard first
		// then check rbombard
		//if (!RbombardCity(m_unit->bombardTarget(m_unit->plot())))
		//{
		//	// vanilla behaviour
		//	m_unit->getGroup()->pushMission(MISSION_BOMBARD);
		//	return true;
		//}
		if (bombardCity())
		{
			return true;
		}

		if (RbombardCity(m_unit->bombardTarget(m_unit->plot())))
		{
			return true;
		}
		// RevolutionDCM - end
	}
	/************************************************************************************************/
	/* REVOLUTIONDCM							END									 Glider1	 */
	/************************************************************************************************/
	if (m_unit->getDomainType() == DOMAIN_LAND)
	{
		foreach_(const CvPlot * pAdjacentPlot1, m_unit->plot()->adjacent()
		| filtered(CvPlot::fn::isCity(false, NO_TEAM)))
		{
			if (potentialEnemy(pAdjacentPlot1->getTeam(), pAdjacentPlot1))
			{
				foreach_(const CvPlot * pAdjacentPlot2, pAdjacentPlot1->adjacent())
				{
					foreach_(const CvUnit * pLoopUnit, pAdjacentPlot2->units())
					{
						if (pLoopUnit->getOwner() == m_unit->getOwner())
						{
							if (pLoopUnit->canBombard(pAdjacentPlot2))
							{
								if (pLoopUnit->isGroupHead())
								{
									if (pLoopUnit->getGroup() != m_unit->getGroup())
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
bool CvUnitDefaultAI::potentialEnemy(TeamTypes eTeam, const CvPlot* pPlot)
{
	PROFILE_FUNC();

	if (m_unit->getGroup()->AI_isDeclareWar(pPlot))
	{
		return m_unit->isPotentialEnemy(eTeam, pPlot);
	}
	else
	{
		return m_unit->isEnemy(eTeam, pPlot);
	}
}


// Returns true if m_unit m_unit->plot needs some defense...
bool CvUnitDefaultAI::defendPlot(const CvPlot* pPlot) const
{
	if (!m_unit->canDefend(pPlot))
	{
		return false;
	}

	const bool bHasTerrainDamage = (pPlot->getTotalTurnDamage(m_unit->getGroup()) > 0 || pPlot->getFeatureTurnDamage() > 0);

	const CvCity* pCity = pPlot->getPlotCity();

	if (bHasTerrainDamage && !pPlot->isRoute() && pCity == NULL)
	{
		return false;
	}

	if (pCity != NULL)
	{
		if (pCity->getOwner() == m_unit->getOwner() && pCity->AI_isDanger())
		{
			return true;
		}
	}
	else
	{
		if (pPlot->plotCount(PUF_canDefendGroupHead, -1, -1, NULL, m_unit->getOwner()) <= ((m_unit->atPlot(pPlot)) ? 1 : 0))
		{
			if (pPlot->plotCount(PUF_cannotDefend, -1, -1, NULL, m_unit->getOwner()) > 0)
			{
				return true;
			}

			//			if (pPlot->defenseModifier(m_unit->getTeam(), false) >= 50 && pPlot->isRoute() && pPlot->getTeam() == m_unit->getTeam())
			//			{
			//				return true;
			//			}
		}
	}

	return false;
}


int CvUnitDefaultAI::pillageValue(const CvPlot* pPlot, int iBonusValueThreshold) const
{
	FAssert(m_unit->getGroup()->canPillage(pPlot) || m_unit->canAirBombAt(m_unit->plot(), pPlot->getX(), pPlot->getY()) || (m_unit->getGroup()->getCargo() > 0));
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

		foreach_(const CvPlot * pAdjacentPlot, m_unit->plot()->adjacent() | filtered(CvPlot::fn::getTeam() == pPlot->getTeam()))
		{
			if (pAdjacentPlot->isCity())
			{
				iValue += 10;
			}

			if (!pAdjacentPlot->isRoute() && !pAdjacentPlot->isWater() && !pAdjacentPlot->isImpassable(m_unit->getTeam()))
			{
				iValue += 2;
			}
		}
	}
	const ImprovementTypes eImprovement = pPlot->getRevealedImprovementType(m_unit->getTeam(), false);

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

int CvUnitDefaultAI::nukeValue(const CvCity* pCity) const
{
	PROFILE_FUNC();
	FAssertMsg(pCity != NULL, "City is not assigned a valid value");

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && !m_unit->isEnemy((TeamTypes)iI) && m_unit->isNukeVictim(pCity->plot(), (TeamTypes)iI))
		{
			return 0; // Don't start wars with neutrals
		}
	}

	int iValue = 1;

	iValue += GC.getGame().getSorenRandNum((pCity->getPopulation() + 1), "AI Nuke City Value");
	iValue += std::max(0, pCity->getPopulation() - 10);

	iValue += ((pCity->getPopulation() * (100 + pCity->calculateCulturePercent(pCity->getOwner()))) / 100);

	iValue += -(GET_PLAYER(m_unit->getOwner()).AI_getAttitudeVal(pCity->getOwner()) / 3);

	foreach_(const CvPlot * pLoopPlot, pCity->plot()->rect(m_unit->nukeRange(), m_unit->nukeRange()))
	{
		if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			iValue++;
		}
		if (pLoopPlot->getNonObsoleteBonusType(m_unit->getTeam()) != NO_BONUS)
		{
			iValue++;
		}
	}

	if (!(pCity->isEverOwned(m_unit->getOwner())))
	{
		iValue *= 3;
		iValue /= 2;
	}

	if (!GET_TEAM(pCity->getTeam()).isAVassal())
	{
		iValue *= 2;
	}

	if (pCity->plot()->isVisible(m_unit->getTeam(), false))
	{
		iValue += 2 * pCity->plot()->getNumVisiblePotentialEnemyDefenders(m_unit);
	}
	else
	{
		iValue += 6;
	}

	return iValue;
}


int CvUnitDefaultAI::searchRange(int iRange) const
{
	if (iRange == 0)
	{
		return 0;
	}
	if (iRange >= MAX_SEARCH_RANGE)
	{
		return MAX_SEARCH_RANGE;
	}

	if (m_unit->flatMovementCost() || (m_unit->getDomainType() == DOMAIN_AIR))
	{
		return std::min(MAX_SEARCH_RANGE, iRange * m_unit->baseMoves());
	}
	return std::min(MAX_SEARCH_RANGE, (iRange + 1) * (m_unit->baseMoves() + 1));
}


// XXX at some point test the game with and without m_unit function...
bool CvUnitDefaultAI::plotValid(const CvPlot* pPlot) const
{
	PROFILE_FUNC();

	if (m_unit->getUnitInfo().isNoRevealMap() && m_unit->willRevealByMove(pPlot))
	{
		return false;
	}

	switch (m_unit->getDomainType())
	{
	case DOMAIN_SEA:

		if (pPlot->isWater() || m_unit->canMoveAllTerrain() || pPlot->isCanMoveSeaUnits())
		{
			return true;
		}
		else if (pPlot->isFriendlyCity(*m_unit, true) && pPlot->isCoastalLand())
		{
			return true;
		}
		break;

	case DOMAIN_AIR:
		FErrorMsg("error");
		break;

	case DOMAIN_LAND:

		if (!pPlot->isWater() || m_unit->canMoveAllTerrain() || pPlot->isSeaTunnel())
		{
			return true;
		}
		break;

	case DOMAIN_IMMOBILE:
		break;

	default:
		FErrorMsg("error");
		break;
	}
	return false;
}


int CvUnitDefaultAI::finalOddsThreshold(const CvPlot* pPlot, int iOddsThreshold) const
{
	PROFILE_FUNC();
	int iFinalOddsThreshold = iOddsThreshold;
	const CvCity* pCity = pPlot->getPlotCity();

	if (pCity != NULL
		&& pCity->getDefenseDamage() < ((GC.getMAX_CITY_DEFENSE_DAMAGE() * 3) / 4)
		)
	{
		iFinalOddsThreshold += std::max(0, (pCity->getDefenseDamage() - pCity->getLastDefenseDamage() - (GC.getCITY_DEFENSE_DAMAGE_HEAL_RATE() * 2)));
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					  03/29/10								jdog5000	  */
	/*																							  */
	/* War tactics AI																			   */
	/************************************************************************************************/
	/* original bts code
		if (pPlot->getNumVisiblePotentialEnemyDefenders(m_unit) == 1)
		{
			if (pCity != NULL)
			{
				iFinalOddsThreshold *= 2;
				iFinalOddsThreshold /= 3;
			}
			else
			{
				iFinalOddsThreshold *= 7;
				iFinalOddsThreshold /= 8;
			}
		}

		if ((m_unit->getDomainType() == DOMAIN_SEA) && !m_unit->getGroup()->hasCargo())
		{
			iFinalOddsThreshold *= 3;
			iFinalOddsThreshold /= 2 + m_unit->getGroup()->getNumUnits();
		}
		else
		{
			iFinalOddsThreshold *= 6;
			iFinalOddsThreshold /= (3 + GET_PLAYER(m_unit->getOwner()).AI_adjacentPotentialAttackers(pPlot, true) + ((stepDistance(m_unit->getX(), m_unit->getY(), pPlot->getX(), pPlot->getY()) > 1) ? 1 : 0) + ((isCityAIType()) ? 2 : 0));
		}
	*/

	const int iDefenders = pPlot->getNumVisiblePotentialEnemyDefenders(m_unit);

	// More aggressive if only one enemy defending city
	// More aggressive if there are units in the m_unit->plot that are special targets of the attacker (such as animals for hunters)
	if ((iDefenders == 1 && pCity != NULL)
		|| pPlot->getNumVisibleEnemyTargetUnits(m_unit) > 0)
	{
		iFinalOddsThreshold *= 2;
		iFinalOddsThreshold /= 3;
	}

#ifdef __OLD_METHOD_PRE_ACCURATE_STACK_CALCULATIONS

	if (iDefenders * 2 < m_unit->getGroup()->getNumUnits() * 3)
	{
		if (m_unit->getGroup()->getAutomateType() == AUTOMATE_HUNT)
		{
			if (GET_PLAYER(m_unit->getOwner()).isModderOption(MODDEROPTION_AUTO_HUNT_ALLOW_UNIT_SUICIDING))
			{
				int iInitialOddsThreshold = iFinalOddsThreshold;
				iFinalOddsThreshold /= m_unit->getGroup()->getNumUnits();
				iFinalOddsThreshold *= 2;
				iFinalOddsThreshold = std::min(iInitialOddsThreshold, iFinalOddsThreshold);
			}
		}
		else if (m_unit->getGroup()->getAutomateType() == AUTOMATE_BORDER_PATROL)
		{
			if (GET_PLAYER(m_unit->getOwner()).isModderOption(MODDEROPTION_AUTO_PATROL_ALLOW_UNIT_SUICIDING))
			{
				int iInitialOddsThreshold = iFinalOddsThreshold;
				iFinalOddsThreshold /= m_unit->getGroup()->getNumUnits();
				iFinalOddsThreshold *= 2;
				iFinalOddsThreshold = std::min(iInitialOddsThreshold, iFinalOddsThreshold);
			}
		}
	}

	if ((m_unit->getDomainType() == DOMAIN_SEA) && !m_unit->getGroup()->hasCargo())
	{
		iFinalOddsThreshold *= 3 + (iDefenders / 2);
		iFinalOddsThreshold /= 2 + m_unit->getGroup()->getNumUnits();
	}
	else
	{
		iFinalOddsThreshold *= 6 + (iDefenders / ((pCity != NULL) ? 1 : 2));
		int iDivisor = 3;
		iDivisor += GET_PLAYER(m_unit->getOwner()).AI_adjacentPotentialAttackers(pPlot, true);
		iDivisor += ((stepDistance(m_unit->getX(), m_unit->getY(), pPlot->getX(), pPlot->getY()) > 1) ? m_unit->getGroup()->getNumUnits() : 0);
		iDivisor += (isCityAIType() ? 2 : 0);
		iFinalOddsThreshold /= iDivisor;
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD					   END												  */
	/************************************************************************************************/
#else
	if (m_unit->getDomainType() == DOMAIN_SEA && m_unit->getGroup()->hasCargo())
	{
		iFinalOddsThreshold = iFinalOddsThreshold * 3 / 2; // don't risk cargo without better odds
	}
	else
	{
		iFinalOddsThreshold *= 6;
		int iDivisor = 5;
		//	Koshling - old code to adjust odds for potential extra attacketrs is redundant now that base
		//	odds calculate using stack odds
		//int iPotentialAttackers = GET_PLAYER(m_unit->getOwner()).AI_adjacentPotentialAttackers(pPlot, true);
		//iDivisor += iPotentialAttackers;
		iDivisor += (isCityAIType() ? 2 : 0);
		iDivisor += ((getUnitAIType() == UNITAI_ATTACK_CITY || getUnitAIType() == UNITAI_ATTACK) ? 2 : 0);
		iFinalOddsThreshold /= iDivisor;
	}
#endif

	return range(iFinalOddsThreshold, 0, 100);
}


int CvUnitDefaultAI::stackOfDoomExtra() const
{
	return ((getBirthmark() % (1 + GET_PLAYER(m_unit->getOwner()).getCurrentEra())) + 4);
}

bool CvUnitDefaultAI::stackAttackCity(int iRange, int iPowerThreshold, bool bFollow)
{
	PROFILE_FUNC();

	FAssert(m_unit->canMove());

	int iSearchRange = bFollow ? 1 : searchRange(iRange);
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		int iPathTurns;
		if (plotValid(pLoopPlot)
			// Super Forts begin *AI_offense* - modified if statement so forts are attacked too
			&& pLoopPlot->isCity(true)
			&& potentialEnemy(pLoopPlot->getTeam(), pLoopPlot)
			&& !m_unit->atPlot(pLoopPlot)
			//	Allow for empty cities (so the move in is strictly not an attack)
			&& (
				(bFollow && m_unit->getGroup()->canEnterPlot(pLoopPlot, true) || m_unit->getGroup()->canEnterPlot(pLoopPlot, false))
				||
				(!bFollow && m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns, iRange) && (iPathTurns <= iRange))
				)
			)
		{
			const int iValue = m_unit->getGroup()->AI_compareStacks(pLoopPlot, StackCompare::CheckCanAttack | StackCompare::CheckCanMove | StackCompare::PotentialEnemy);

			if (iValue >= iPowerThreshold
				&& iValue > iBestValue)
			{
				const CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();

				if (bFollow || endTurnPlot == pLoopPlot || !exposedToDanger(endTurnPlot, 60))
				{
					iBestValue = iValue;
					pBestPlot = bFollow ? pLoopPlot : endTurnPlot;
					FAssert(!m_unit->atPlot(pBestPlot));
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (gUnitLogLevel >= 1 && pBestPlot->getPlotCity() != NULL)
		{
			logBBAI("	Stack for player %d (%S) decides to attack city %S with stack ratio %d", m_unit->getOwner(), GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), pBestPlot->getPlotCity()->getName(0).GetCString(), iBestValue);
			logBBAI("	City %S has defense modifier %d, %d with ignore building", pBestPlot->getPlotCity()->getName(0).GetCString(), pBestPlot->getPlotCity()->getDefenseModifier(false), pBestPlot->getPlotCity()->getDefenseModifier(true));
		}
		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), ((bFollow) ? MOVE_DIRECT_ATTACK : 0));
	}

	return false;
}

bool CvUnitDefaultAI::moveIntoCity(int iRange, bool bOpponentOnly)
{
	PROFILE_FUNC();
	FAssert(m_unit->canMove());

	if (m_unit->plot()->isCity())
	{
		return false;
	}

	const int iSearchRange = searchRange(iRange);

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestCityPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		int iPathTurns = 0;
		if (plotValid(pLoopPlot)
			&& (!bOpponentOnly || m_unit->isEnemy(pLoopPlot->getTeam(), pLoopPlot))
			&& (pLoopPlot->isCity() || pLoopPlot->isCity(true))
			&& m_unit->canEnterPlot(pLoopPlot)
			&& m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns, 0)
			&& (!bOpponentOnly || pLoopPlot->getTeam() != m_unit->getTeam())
			)
		{
			FAssert(m_unit->getPathEndTurnPlot() == pLoopPlot);

			int iValue = 1;
			if (pLoopPlot->getPlotCity() != NULL)
			{
				iValue += pLoopPlot->getPlotCity()->getPopulation();
				if (bOpponentOnly)
				{
					iValue *= (GET_TEAM(m_unit->getTeam()).AI_getAttitudeWeight(pLoopPlot->getTeam()) * -1);
				}
				//Reduce by amount of turns to get there
				if (iPathTurns != 0)
				{
					iValue /= iPathTurns;
				}
			}
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestPlot = m_unit->getPathEndTurnPlot();
				pBestCityPlot = pLoopPlot;
				FAssert(!m_unit->atPlot(pBestPlot));
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!m_unit->atPlot(pBestCityPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}

	return false;
}

bool CvUnitDefaultAI::moveIntoNearestOwnedCity()
{
	PROFILE_FUNC();
	FAssert(m_unit->canMove());

	if (m_unit->plot()->isCity())
	{
		return false;
	}
	int iPathTurns, iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestCityPlot = NULL;

	foreach_(const CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
	{
		if ((pLoopCity->area() == m_unit->area()) && plotValid(pLoopCity->plot()))
		{
			const CvPlot* pLoopPlot = pLoopCity->plot();
			if (pLoopPlot != NULL && pLoopPlot->getArea() == m_unit->getArea()
			&& (pLoopPlot->isCity() || pLoopPlot->isCity(true))
			&& m_unit->canEnterPlot(pLoopPlot) && m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns, 0))
			{
				FAssert(m_unit->getPathEndTurnPlot() == pLoopPlot);

				int iValue = 1000;
				if (pLoopPlot->getPlotCity() != NULL)
				{
					iValue -= iPathTurns;
				}

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = m_unit->getPathEndTurnPlot();
					pBestCityPlot = pLoopPlot;
					FAssert(!m_unit->atPlot(pBestPlot));
				}
			}
		}
	}

	if (pBestPlot != NULL && pBestCityPlot != NULL)
	{
		FAssert(!m_unit->atPlot(pBestCityPlot));
		if (getUnitAIType() == UNITAI_HEALER)
		{
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_UNITS, false, false, MISSIONAI_HEAL_SUPPORT, pBestCityPlot);
		}
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_AVOID_ENEMY_UNITS);
	}
	return false;
}

//bolsters the culture of the weakest city.
//returns true if a mission is pushed.
bool CvUnitDefaultAI::artistCultureVictoryMove()
{
	if (!GET_PLAYER(m_unit->getOwner()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE1))
	{
		return false;
	}
	int iCultureCitiesNeeded = GC.getGame().culturalVictoryNumCultureCities();
	FAssertMsg(iCultureCitiesNeeded > 0, "AI_AI_VICTORY_CULTURE1 Strategy should not be true");

	bool bJoin = true;
	bool bGreatWork = GET_PLAYER(m_unit->getOwner()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3);

	int iPathTurns = 0;
	SpecialistTypes eBestSpecialist = NO_SPECIALIST;
	CvCity* pBestCity = NULL;
	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;
	int iTargetCultureRank = iCultureCitiesNeeded;

	while (iTargetCultureRank > 0 && pBestCity == NULL)
	{
		foreach_(CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
		{
			// BBAI efficiency: check same m_unit->area
			if (pLoopCity->area() == m_unit->area() && plotValid(pLoopCity->plot())
			// instead of commerce rate rank should use the culture on tile...
			&& pLoopCity->findCommerceRateRank(COMMERCE_CULTURE) == iTargetCultureRank)
			{
				// if the city is a fledgling, probably building culture, try next higher city
				if (pLoopCity->getCultureLevel() < 2
				// if we cannot path there, try the next higher culture city
				|| !generateSafePathforVulnerable(pLoopCity->plot(), &iPathTurns))
				{
					break;
				}
				pBestCity = pLoopCity;
				pBestPlot = pLoopCity->plot();

				if (bGreatWork && m_unit->canGreatWork(pBestPlot))
				{
					break;
				}
				for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
				{
					if (m_unit->canJoin(pBestPlot, (SpecialistTypes)iI))
					{
						const int iValue = pLoopCity->AI_specialistValue(((SpecialistTypes)iI), pLoopCity->AI_avoidGrowth(), false);

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
					if (m_unit->canGreatWork(pBestPlot))
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

	if (pBestCity == NULL)
	{
		return false; // should try to airlift there...
	}

	if (!m_unit->atPlot(pBestPlot))
	{
		if (generateSafePathforVulnerable(pBestPlot, &iPathTurns))
		{
			CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY(), MOVE_IGNORE_DANGER);
		}
		m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
		return true;
	}

	if (bGreatWork)
	{
		m_unit->getGroup()->pushMission(MISSION_GREAT_WORK);
		return true;
	}
	if (bJoin)
	{
		m_unit->getGroup()->pushMission(MISSION_JOIN, eBestSpecialist);
		return true;
	}
	return false;
}


bool CvUnitDefaultAI::choke(int iRange, bool bDefensive)
{
	PROFILE_FUNC();

	bool bNoDefensiveBonus = m_unit->noDefensiveBonus();

	if (m_unit->getGroup()->getNumUnits() > 1)
	{
		foreach_(const CvUnit * pLoopUnit, m_unit->getGroup()->units())
		{
			bNoDefensiveBonus = (bNoDefensiveBonus && pLoopUnit->noDefensiveBonus());
		}
	}
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pBestChokePlot = NULL;
	int iBestValue = 0;

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iRange, iRange))
	{
		if (m_unit->isEnemy(pLoopPlot->getTeam()) && (!pLoopPlot->isVisible(m_unit->getTeam(), false) || !pLoopPlot->isVisibleEnemyUnit(m_unit)))
		{
			const CvCity* pWorkingCity = pLoopPlot->getWorkingCity();

			if (pWorkingCity != NULL && pWorkingCity->getTeam() == pLoopPlot->getTeam()
			// and no terrain damage
			&& m_unit->plot()->getTotalTurnDamage(m_unit->getGroup()) <= 0 && m_unit->plot()->getFeatureTurnDamage() <= 0)
			{
				int iValue = (bDefensive ? pLoopPlot->defenseModifier(m_unit->getTeam(), false) : -15);

				if (pLoopPlot->getBonusType(m_unit->getTeam()) != NO_BONUS)
				{
					iValue += GET_PLAYER(pLoopPlot->getOwner()).AI_bonusVal(pLoopPlot->getBonusType(), 0);
				}
				iValue += pLoopPlot->getYield(YIELD_PRODUCTION) * 10;
				iValue += pLoopPlot->getYield(YIELD_FOOD) * 10;
				iValue += pLoopPlot->getYield(YIELD_COMMERCE) * 5;

				if (bNoDefensiveBonus)
				{
					iValue *= std::max(0, ((m_unit->baseCombatStrNonGranular() * 120) - GC.getGame().getBestLandUnitCombat()));
				}
				else iValue *= pLoopPlot->defenseModifier(m_unit->getTeam(), false);

				if (iValue > 0)
				{
					if (!bDefensive)
					{
						iValue *= 10;
						iValue /= std::max(1, (pLoopPlot->getNumDefenders(m_unit->getOwner()) + ((pLoopPlot == m_unit->plot()) ? 0 : m_unit->getGroup()->getNumUnits())));
					}
					if (m_unit->generatePath(pLoopPlot, 0, true, NULL, iRange))
					{
						const CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();

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
		if (m_unit->atPlot(pBestChokePlot))
		{
			//if(m_unit->getGroup()->canPillage(m_unit->plot()))
			//{
			//	m_unit->getGroup()->pushMission(MISSION_PILLAGE);
			//}
			if (!m_unit->isInGroup())//Somehow the pillage mission caused a case where a unit lost it's group definition.  I could not spot how as I walked through the event.
			{
				m_unit->joinGroup(NULL, true);
			}
			m_unit->getGroup()->pushMission(MISSION_SKIP);

			return true;
		}
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}
	return false;
}


bool CvUnitDefaultAI::solveBlockageProblem(const CvPlot* pDestPlot, bool bDeclareWar)
{
	PROFILE_FUNC();

	FAssert(pDestPlot != NULL);


	if (pDestPlot != NULL)
	{
		const CvPlot* pSourcePlot = m_unit->plot();

		if (gDLL->getFAStarIFace()->GeneratePath(&GC.getStepFinder(), pSourcePlot->getX(), pSourcePlot->getY(), pDestPlot->getX(), pDestPlot->getY(), false, -1, true))
		{
			const FAStarNode* pStepNode = gDLL->getFAStarIFace()->GetLastNode(&GC.getStepFinder());

			while (pStepNode != NULL)
			{
				int iPathTurns;
				const CvPlot* pStepPlot = GC.getMap().plotSorenINLINE(pStepNode->m_iX, pStepNode->m_iY);
				if (m_unit->canEnterOrAttackPlot(pStepPlot) && m_unit->generatePath(pStepPlot, 0, false, &iPathTurns))
				{
					if (bDeclareWar && pStepNode->m_pPrev != NULL)
					{
						const CvPlot* pPlot = GC.getMap().plotSorenINLINE(pStepNode->m_pPrev->m_iX, pStepNode->m_pPrev->m_iY);
						if (pPlot->getTeam() != NO_TEAM
						&& !m_unit->getGroup()->canEnterPlot(pPlot, true)
						&& !m_unit->isPotentialEnemy(pPlot->getTeam(), pPlot))
						{
							CvTeamAI& kTeam = GET_TEAM(m_unit->getTeam());
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
								return (goToTargetCity(MOVE_AVOID_ENEMY_WEIGHT_2));
							}
						}
					}
					if (pStepPlot->isVisible(m_unit->getTeam(), false) && pStepPlot->isVisibleEnemyUnit(m_unit))
					{
						FAssert(m_unit->canAttack());
						const CvPlot* pBestPlot = pStepPlot;
						//To prevent puppeteering attempt to barge through
						//if quite close
						if (iPathTurns > 3)
						{
							pBestPlot = m_unit->getPathEndTurnPlot();
						}

						FAssert(!m_unit->atPlot(pBestPlot));
						return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), MOVE_DIRECT_ATTACK);
					}
				}
				pStepNode = pStepNode->m_pParent;
			}
		}
	}

	return false;
}

int CvUnitDefaultAI::calculatePlotWorkersNeeded(const CvPlot* pPlot, BuildTypes eBuild) const
{
	const int iWorkRate = m_unit->workRate(true);

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

	if (pPlot->getNonObsoleteBonusType(m_unit->getTeam()) != NO_BONUS)
	{
		iNeeded *= 2;
	}

	return iNeeded;

}

bool CvUnitDefaultAI::canGroupWithAIType(UnitAITypes eUnitAI) const
{
	if (eUnitAI != getUnitAIType())
	{
		switch (eUnitAI)
		{
		case (UNITAI_ATTACK_CITY):
			if (m_unit->plot()->isCity() && (GC.getGame().getGameTurn() - m_unit->plot()->getPlotCity()->getGameTurnAcquired()) <= 1)
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



bool CvUnitDefaultAI::allowGroup(const CvUnit* pUnit, UnitAITypes eUnitAI) const
{
	CvSelectionGroup* pGroup = pUnit->getGroup();
	CvPlot* pPlot = pUnit->plot();

	if (pUnit == m_unit)
	{
		return false;
	}

	if (!pUnit->isGroupHead())
	{
		return false;
	}

	//	Don't join a unit that was itself wondering what to do m_unit turn
	if ((static_cast<const CvUnitDefaultAI*>(pUnit->AI()))->m_contractsLastEstablishedTurn == GC.getGame().getGameTurn() &&
		 (m_contractualState == CONTRACTUAL_STATE_AWAITING_WORK || m_contractualState == CONTRACTUAL_STATE_NO_WORK_FOUND))
	{
		return false;
	}

	if (pGroup == m_unit->getGroup())
	{
		return false;
	}

	if (pUnit->isCargo())
	{
		return false;
	}

	if (pUnit->AI()->getUnitAIType() != eUnitAI)
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
		// do not attempt to join groups which are healing m_unit turn
		// (healing is cleared every turn for automated groups, so we know we pushed a heal m_unit turn)
		return false;
	}

	if (!m_unit->canJoinGroup(pPlot, pGroup))
	{
		return false;
	}

	if (eUnitAI == UNITAI_SETTLE)
	{
		if (GET_PLAYER(m_unit->getOwner()).AI_getAnyPlotDanger(pPlot, 3))
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

	if ((m_unit->getGroup()->getHeadUnitAI() == UNITAI_CITY_DEFENSE))
	{
		if (m_unit->plot()->isCity() && (m_unit->plot()->getTeam() == m_unit->getTeam()) && m_unit->plot()->getBestDefender(m_unit->getOwner())->getGroup() == m_unit->getGroup())
		{
			return false;
		}
	}

	if (m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		CvPlot* pTargetPlot = pGroup->AI_getMissionAIPlot();

		if (pTargetPlot != NULL)
		{
			if (pTargetPlot->isOwned())
			{
				if (m_unit->isPotentialEnemy(pTargetPlot->getTeam(), pTargetPlot))
				{
					//Do not join groups which have debarked on an offensive mission
					return false;
				}
			}
		}
	}

	if (pUnit->getInvisibleType() != NO_INVISIBLE)
	{
		if (m_unit->getInvisibleType() == NO_INVISIBLE)
		{
			return false;
		}
	}

	return true;
}


void CvUnitDefaultAI::read(FDataStreamBase* pStream)
{
	pStream->Read(&m_iBirthmark);

	pStream->Read((int*)&m_eUnitAIType);
	pStream->Read(&m_iAutomatedAbortTurn);

	//	Translate the unit AI id - done m_unit way because unit AI types were
	//	not originally considered to be an enum type and so older saves have them
	//	as raw ints
	UnitAITypes eTranslatedUnitAI = (UnitAITypes)CvTaggedSaveFormatWrapper::getSaveFormatWrapper().getNewClassEnumValue(REMAPPED_CLASS_TYPE_UNITAIS, m_eUnitAIType, true);
	//	If the save has no translation table for unitAIs assume the ids are identical - m_unit
	//	is normally the case, and we can't do any better with an older save
	if (eTranslatedUnitAI != NO_UNITAI)
	{
		m_eUnitAIType = eTranslatedUnitAI;
	}
	//	Animals should never have a non-animal AI (old assets had a bug that allowed m_unit
	//	so correct it here as a patch up)
	// TB Note: This is potentially a little constrictive now to some concepts.
	//if ( m_unit->isAnimal() && m_eUnitAIType != UNITAI_ANIMAL )
	//{
	//	m_eUnitAIType = UNITAI_ANIMAL;

	//	GET_PLAYER(m_unit->getOwner()).AI_noteUnitRecalcNeeded();
	//}
	//	Invalid unitAIs cannot be coped with - just delete them - better than
	//	killing the entire game when m_unit corruption is found
	if (m_eUnitAIType >= NUM_UNITAI_TYPES)
	{
		m_unit->kill(true);
	}

	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);
	WRAPPER_READ(wrapper, "CvUnitAI", &m_iGarrisonCity);	// which city does m_unit unit belong to the garrison of (if any)
	WRAPPER_READ(wrapper, "CvUnitAI", &m_iGroupLeadOverride);
	WRAPPER_READ(wrapper, "CvUnitAI", &m_bWaitingOnUnitAIAny);

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


void CvUnitDefaultAI::write(FDataStreamBase* pStream)
{
	pStream->Write(m_iBirthmark);

	pStream->Write(m_eUnitAIType);
	pStream->Write(m_iAutomatedAbortTurn);

	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);
	WRAPPER_WRITE(wrapper, "CvUnitAI", m_iGarrisonCity);	// which city does m_unit unit belong to the garrison of (if any)
	WRAPPER_WRITE(wrapper, "CvUnitAI", m_iGroupLeadOverride);
	WRAPPER_WRITE(wrapper, "CvUnitAI", m_bWaitingOnUnitAIAny);

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
bool CvUnitDefaultAI::RbombardPlot(int iRange, int iBonusValueThreshold)
{
	PROFILE_FUNC();

	if (!GC.isDCM_RANGE_BOMBARD())
	{
		return false;
	}
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * plotX, m_unit->plot()->rect(iRange, iRange))
	{
		if (plotValid(plotX) && !plotX->isCity() && plotX->getTeam() != m_unit->getTeam()
		&& plotX->getImprovementType() != NO_IMPROVEMENT
		&& m_unit->getGroup()->canBombardAtRanged(m_unit->plot(), plotX->getX(), plotX->getY()))
		{
			int iValue = 0;
			if (iBonusValueThreshold > 0)
			{
				const ImprovementTypes eImprovement = plotX->getRevealedImprovementType(m_unit->getTeam(), false);

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
			else iValue = pillageValue(plotX, 0); // returns any improvement with highest pillage value


			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestPlot = plotX;
			}
		}
	}
	if (pBestPlot != NULL)
	{
		FAssertMsg(pBestPlot->getNumVisibleEnemyCombatUnits(m_unit->getOwner()) > 0, "Can't bombard without units in target m_unit->plot");
		m_unit->getGroup()->pushMission(MISSION_RBOMBARD, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::RbombardUnit(int iRange, int iHighestOddsThreshold, int iMinStack, int iSeigeDiff, int iPowerThreshold, bool bCity)
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

	const int iSearchRange = searchRange(iRange);

	int iBestValue = 999; // looking for the odds at or below iHighestOddsThreshold
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		if (plotValid(pLoopPlot) && (!pLoopPlot->isCity() || bCity))
		{
			if (m_unit->getGroup()->canBombardAtRanged(m_unit->plot(), pLoopPlot->getX(), pLoopPlot->getY()))
			{
				if ((pLoopPlot->isVisible(m_unit->getTeam(), false) && pLoopPlot->isVisibleEnemyUnit(m_unit)) || potentialEnemy(pLoopPlot->getTeam()))
				{
					if (pLoopPlot->getNumVisiblePotentialEnemyDefenders(m_unit) >= iMinStack)
					{
						for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
						{
							const TeamTypes eEnemyTeam = GET_PLAYER((PlayerTypes)iPlayer).getTeam();
							if (GET_TEAM(eEnemyTeam).isAtWar(m_unit->getTeam()))
							{
								if (pLoopPlot->getAverageEnemyDamage(eEnemyTeam) < m_unit->getGroup()->getMinimumRBombardDamageLimit())
								{
									int iEnemyCount = pLoopPlot->getNumVisibleEnemyCombatUnits((PlayerTypes)iPlayer);

									iPrelimValue = pLoopPlot->getAverageEnemyStrength(eEnemyTeam);
									iPrelimValue *= pLoopPlot->getAverageEnemyDamage(eEnemyTeam);
									iPrelimValue /= 100;
									iPrelimValue += std::min(m_unit->getGroup()->getRBombardDamageMaxUnits(), iEnemyCount);
									iPrelimValue *= iEnemyCount;
								}
							}
						}
						const CvUnit* pDefender = pLoopPlot->getBestDefender(NO_PLAYER, m_unit->getOwner(), m_unit, true);
						if (pDefender != NULL && pDefender->isRbombardable(iMinStack))
						{
							int iValue = attackOdds(pLoopPlot, true);
							if (iValue <= iHighestOddsThreshold)
							{
								int ourSeige = m_unit->getRbombardSeigeCount(m_unit->plot());
								int theirSeige = pDefender->getRbombardSeigeCount(pLoopPlot);
								if ((ourSeige - theirSeige) >= iSeigeDiff)
								{
									int ourStrength = GET_PLAYER(m_unit->getOwner()).AI_getOurPlotStrength(m_unit->plot(), iRange, false, false) * 100;
									int theirStrength = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(m_unit->plot(), iRange, false, false);
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
		FAssertMsg(pBestPlot->getNumVisibleEnemyCombatUnits(m_unit->getOwner()) > 0, "Can't bombard without units in target m_unit->plot");
		m_unit->getGroup()->pushMission(MISSION_RBOMBARD, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::RbombardCity(const CvCity* pCity)
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

	if (!m_unit->getGroup()->canBombardAtRanged(m_unit->plot(), pPlot->getX(), pPlot->getY()))
	{
		return false;
	}

	if (RbombardUnit(std::min(2, m_unit->getGroup()->getMinimumRBombardRange()), 65, 2, 0, 0, true))
	{
		return true;
	}
	/*pDefender = pPlot->getBestDefender(NO_PLAYER, m_unit->getOwner(), m_unit, true);
	if (pDefender != NULL)
	{
		if (m_unit->collateralDamageMaxUnits() > pPlot->getNumUnits())
		{
			if (pDefender->isRbombardable(3) && m_unit->getGroup()->getBombardTurns(pCity) < 3)
			{
				m_unit->getGroup()->pushMission(MISSION_RBOMBARD, pPlot->getX(),pPlot->getY());
				return true;
			}
		}
	}*/
	return false;
}

// RevolutionDCM - sea bombard AI identical to DCM 1.7 RbombardCity() AI
// Returns true if a mission was pushed...
bool CvUnitDefaultAI::RbombardNaval()
{
	PROFILE_FUNC();

	if (!m_unit->canRBombard())
	{
		return false;
	}

	const int iSearchRange = m_unit->getGroup()->getMinimumRBombardRange();
	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		if (!m_unit->atPlot(pLoopPlot) && m_unit->getGroup()->canBombardAtRanged(m_unit->plot(), pLoopPlot->getX(), pLoopPlot->getY()))
		{
			const int iEnemyCount = pLoopPlot->getNumVisibleEnemyCombatUnits(m_unit->getOwner());

			// We definitely can't do a bombard mission unless there is enemy present, and we won't do one unless the targets are worth it
			if (iEnemyCount > 0 && pLoopPlot->getAverageEnemyDamage(m_unit->getTeam()) < m_unit->getGroup()->getMinimumRBombardDamageLimit())
			{
				int iValue = pLoopPlot->getAverageEnemyStrength(m_unit->getTeam());
				iValue *= pLoopPlot->getAverageEnemyDamage(m_unit->getTeam());
				iValue /= 100;
				iValue += std::min(m_unit->getGroup()->getRBombardDamageMaxUnits(), iEnemyCount);
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
					if (pCity == pCity->area()->getTargetCity(m_unit->getOwner()))
					{
						iValue *= 3;
					}
				}

				//TBHERE
				int iPotentialAttackers = GET_PLAYER(m_unit->getOwner()).AI_adjacentPotentialAttackers(pLoopPlot);//pLoopPlot->getNumVisibleEnemyDefenders(NO_PLAYER);
				if (iPotentialAttackers > 0 || pLoopPlot->isAdjacentTeam(m_unit->getTeam()))
				{
					const CvUnit* pDefender = pLoopPlot->getBestDefender(NO_PLAYER, m_unit->getOwner(), m_unit, true);

					if (pDefender != NULL && pDefender->canDefend() && iValue > 0)
					{
						//iDamage = GC.getGame().getSorenRandNum(rBombardDamage(), "AI Bombard");
						//iValue = std::max(0, (std::min((pDefender->getDamage() + iDamage), rBombardDamage()) - pDefender->getDamage()));
						//iValue += ((((iDamage * m_unit->collateralDamage()) / 100) * std::min((pLoopPlot->getNumVisibleEnemyDefenders(m_unit) - 1), m_unit->collateralDamageMaxUnits())) / 2);
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
		FAssertMsg(pBestPlot->getNumVisibleEnemyCombatUnits(m_unit->getOwner()) > 0, "Can't bombard without units in target m_unit->plot");
		m_unit->getGroup()->pushMission(MISSION_RBOMBARD, pBestPlot->getX(), pBestPlot->getY());
		return true;
	}
	return false;
}
// Dale - RB: Field Bombard END
// RevolutionDCM - end


// Dale - FE: Fighters START
bool CvUnitDefaultAI::FEngage()
{
	PROFILE_FUNC();

	if (!m_unit->canFEngage())
	{
		return false;
	}
	if (m_unit->currFirepower(NULL, NULL) <= 0)
	{
		return false;
	}
	if (GC.getGame().getSorenRandNum(10, "Randomness") < 5)
	{
		return false;
	}
	if (GC.isDCM_FIGHTER_ENGAGE())
	{
		const int iSearchRange = m_unit->airRange();
		int iBestValue = 0;
		const CvPlot* pBestPlot = NULL;
		foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
		{
			if (m_unit->canFEngageAt(m_unit->plot(), pLoopPlot->getX(), pLoopPlot->getY()))
			{
				int iValue = 0;
				const int iCount = algo::count_if(pLoopPlot->units(), CvUnit::fn::getDomainType() == DOMAIN_AIR);
				const int iPotentialAttackers = iCount;
				iValue += iCount;
				if (iPotentialAttackers > 0 || pLoopPlot->isAdjacentTeam(m_unit->getTeam()))
				{
					const CvUnit* pDefender = pLoopPlot->getBestDefender(NO_PLAYER, m_unit->getOwner(), m_unit, true);
					const int iDamage = std::max(1, ((GC.getCOMBAT_DAMAGE() * (m_unit->currFirepower(NULL, NULL) + ((m_unit->currFirepower(NULL, NULL) + pDefender->currFirepower(NULL, NULL) + 1) / 2))) / (pDefender->currFirepower(pLoopPlot, m_unit) + ((m_unit->currFirepower(NULL, NULL) + pDefender->currFirepower(NULL, NULL) + 1) / 2))));
					iValue += (iDamage * pLoopPlot->getNumVisiblePotentialEnemyDefenders(m_unit));
				}
				iValue *= GC.getGame().getSorenRandNum(5, "AI FEngage");
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = pLoopPlot;
					FAssert(!m_unit->atPlot(pBestPlot));
				}
			}
		}
		if (pBestPlot != NULL)
		{
			m_unit->getGroup()->pushMission(MISSION_FENGAGE, pBestPlot->getX(), pBestPlot->getY());
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
bool CvUnitDefaultAI::claimForts(CvReachablePlotSet* pReachablePlots, int iMinValue, int iMaxPath)
{
	PROFILE_FUNC();

	int iBestValue = iMinValue;
	const CvPlot* endTurnPlot = NULL;
	const CvPlot* pBestPlot = NULL;
	const CvPlot* pFortPlot = NULL;


	for (CvReachablePlotSet::const_iterator itr = pReachablePlots->begin(); itr != pReachablePlots->end(); ++itr)
	{
		const CvPlot* pLoopPlot = itr.plot();
		if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(pLoopPlot->getImprovementType()).getCulture() > 0)
		{
			if (/*AI_plotValid(pLoopPlot) &&*/ pLoopPlot->area() == m_unit->area())
			{
				if (m_unit->potentialWarAction(pLoopPlot) || pLoopPlot->getOwner() == NO_PLAYER)
				{
					if (m_unit->canClaimTerritory(pLoopPlot))
					{
						if (!isPlotWellDefended(pLoopPlot, true, 35))
						{
							if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_CLAIM_TERRITORY, m_unit->getGroup(), 1) == 0)
							{
								int iPathTurns;
								if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns, iMaxPath))
								{
									int iValue = 1000;

									foreach_(const CvPlot * pAdjacentPlot, m_unit->plot()->adjacent()
									| filtered(CvPlot::fn::getBonusType(m_unit->getTeam()) != NO_BONUS))
									{
										iValue *= 2;
									}

									iValue /= (iPathTurns + 1);

									if (iValue > iBestValue)
									{
										endTurnPlot = m_unit->getPathEndTurnPlot();

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
		if (m_unit->atPlot(pFortPlot))
		{
			if (m_unit->isEnemy(pFortPlot->getTeam()))
			{
				m_unit->getGroup()->pushMission(MISSION_CLAIM_TERRITORY, -1, -1, 0, false, false, MISSIONAI_CLAIM_TERRITORY, pFortPlot);
				return true;
			}
		}
		else
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_CLAIM_TERRITORY, pFortPlot);
		}
	}

	return false;
}

BuildTypes CvUnitDefaultAI::findBestFort(const CvPlot* pPlot) const
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
			// Is fort or tower
			if ((kImprovement.isActsAsCity() || kImprovement.getVisibilityChange() > 0)
			&& m_unit->canBuild(pPlot, eBuild)
			// If not (m_unit->plot in workable radius of any city and is tower line improvement); 'plot in workable radius of owned city' might be better?
			// Toffer - The next condition seems iffy considering the above comment about it...
			&& (!pPlot->isCityRadius() || kImprovement.isActsAsCity()))
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

bool CvUnitDefaultAI::StrategicForts()
{
	PROFILE_FUNC();

	if (m_unit->isNPC())
	{
		return false;
	}
	const CvPlayerAI& kOwner = GET_PLAYER(m_unit->getOwner());
	const TeamTypes eTeam = m_unit->getTeam();
	const bool bWarPlan = GET_TEAM(eTeam).hasWarPlan(true);

	CvReachablePlotSet plotSet(m_unit->getGroup(), 0, MAX_INT);
	BuildTypes eBestBuild = NO_BUILD;
	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;
	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		CvPlot* pLoopPlot = itr.plot();

		if (/*AI_plotValid(pLoopPlot) &&*/ pLoopPlot->area() == m_unit->area()
		&& (pLoopPlot->getOwner() == NO_PLAYER || pLoopPlot->getTeam() == eTeam)
		&& pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
		{
			const BuildTypes eBestTempBuild = findBestFort(pLoopPlot);

			if (eBestTempBuild != NO_BUILD
			&& (!pLoopPlot->isVisible(eTeam, false) || !pLoopPlot->isVisibleEnemyUnit(m_unit))
			&& kOwner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD, m_unit->getGroup(), 3) == 0)
			{
				int iPathTurns = 0;
				if (generateSafePathforVulnerable(pLoopPlot, &iPathTurns))
				{
					//	Koshling - adjusted a bit to take advantage of super forts choke calculations
					const int iValue =
						(
							1000
							*
							std::max(100, pLoopPlot->getFoundValue(m_unit->getOwner()) / 2 + kOwner.AI_getPlotChokeValue(pLoopPlot))
							*
							(pLoopPlot->getBorderPlotCount() + 1)
							*
							(pLoopPlot->getEnemyBorderPlotCount(m_unit->getOwner()) + 1) * (bWarPlan ? 3 : 1)
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
	// No real value or m_unit->plot
	if (iBestValue < 250 || pBestPlot == NULL)
	{
		return false;
	}
	FASSERT_BOUNDS(0, GC.getNumBuildInfos(), eBestBuild);

	if (!m_unit->atPlot(pBestPlot))
	{
		int iPathTurns = 0;
		if (generateSafePathforVulnerable(pBestPlot, &iPathTurns))
		{
			const CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();
			if (endTurnPlot != NULL)
			{
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_BUILD, pBestPlot);
			}
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
		m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
		return true;
	}
	m_unit->getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, (m_unit->getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);
	return true;
}

bool CvUnitDefaultAI::caravan(bool bAnyCity)
{
	PROFILE_FUNC();

	if (m_unit->getUnitInfo().getProductionCost() < 0)
	{
		return false; // Avoid using Great People
	}
	const int iNumCities = GET_PLAYER(m_unit->getOwner()).getNumCities();
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	foreach_(const CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
	{
		if (pLoopCity->area() == m_unit->area() && plotValid(pLoopCity->plot()) && m_unit->canHurry(pLoopCity->plot())
		&& !pLoopCity->plot()->isVisibleEnemyUnit(m_unit))
		{
			int iPathTurns;
			if (generateSafePathforVulnerable(pLoopCity->plot(), &iPathTurns))
			{
				if (bAnyCity
				|| pLoopCity->findPopulationRank() >= iNumCities * 2 / 3
				&& pLoopCity->getPopulation() < GET_PLAYER(m_unit->getOwner()).getTotalPopulation() * 2 / (3 * iNumCities))
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

	if (pBestPlot == NULL)
	{
		return false;
	}
	if (!m_unit->atPlot(pBestPlot))
	{
		int iPathTurns;
		if (generateSafePathforVulnerable(pBestPlot, &iPathTurns))
		{
			CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();
			if (endTurnPlot != NULL)
			{
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_HURRY, pBestPlot);
			}
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
		m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
		return true;
	}
	m_unit->getGroup()->pushMission(MISSION_HURRY);
	return true;
}

bool CvUnitDefaultAI::hurryFood()
{
	PROFILE_FUNC();

	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	foreach_(const CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
	{
		if (pLoopCity->area() == m_unit->area() && plotValid(pLoopCity->plot())
			// Hurry is possible
			&& m_unit->canHurry(pLoopCity->plot())
			&& !pLoopCity->plot()->isVisibleEnemyUnit(m_unit)
			// Nobody already hurrying
			&& GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_HURRY_FOOD, m_unit->getGroup()) == 0
			&& (
				pLoopCity->foodDifference() < 0
				|| (GC.getGame().getSorenRandNum(100, "AI hurry with food") > 75
					&& pLoopCity->findPopulationRank() > GC.getWorldInfo(GC.getMap().getWorldSize()).getTargetNumCities())
				)
			)
		{
			int iPathTurns;
			// Get a safe path
			if (generateSafePathforVulnerable(pLoopCity->plot(), &iPathTurns))
			{
				int iTurnsLeft = pLoopCity->getFoodTurnsLeft();
				iTurnsLeft -= iPathTurns;

				// Worth actually going
				if (iTurnsLeft > 10)
				{
					int iValue = iTurnsLeft;

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
		if (!m_unit->atPlot(pBestPlot))
		{
			int iPathTurns = 0;
			if (generateSafePathforVulnerable(pBestPlot, &iPathTurns))
			{
				CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();
				if (endTurnPlot != NULL)
				{
					return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_HURRY_FOOD, pBestPlot);
				}
				else
				{
					m_unit->getGroup()->pushMission(MISSION_SKIP);
					return true;
				}
			}
			else
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
				return true;
			}
		}
		else
		{
			m_unit->getGroup()->pushMission(MISSION_HURRY_FOOD);
			return true;
		}
	}

	return false;
}

bool CvUnitDefaultAI::command()
{
	if (!GC.getGame().isOption(GAMEOPTION_GREAT_COMMANDERS))
	{
		return false;
	}

	bool bCommand = static_cast<int>(GET_PLAYER(m_unit->getOwner()).Commanders.size()) < 2 + GET_PLAYER(m_unit->getOwner()).getNumCities() / 5;

	if (!bCommand)
	{
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
		{
			if (GET_TEAM(GET_PLAYER((PlayerTypes)iPlayer).getTeam()).isAtWar(m_unit->getTeam())
			&& GET_PLAYER(m_unit->getOwner()).Commanders.size() < GET_PLAYER((PlayerTypes)iPlayer).Commanders.size())
			{
				bCommand = true;
				break;
			}
		}
	}

	if (!bCommand
	&& GET_TEAM(m_unit->getTeam()).getAnyWarPlanCount(true) > static_cast<int>(GET_PLAYER(m_unit->getOwner()).Commanders.size()))
	{
		bCommand = true;
	}

	if (bCommand && m_unit->plot()->getPlotCity() != NULL)
	{
		const int iTotalThreat = std::max(1, GET_PLAYER(m_unit->getOwner()).AI_getTotalAreaCityThreat(m_unit->area(), NULL));
		const int iOurThreat = m_unit->plot()->getPlotCity()->AI_cityThreat();

		// Unlikely to level the commander fast enough to be useful, leading troops will bring more immediate benefits
		const int iNumCities = std::max(1, GET_PLAYER(m_unit->getOwner()).getNumCities());
		if (iOurThreat > 2 * iTotalThreat / iNumCities)
		{
			bCommand = false;
		}
	}
	m_unit->setCommander(bCommand);
	return bCommand;
}

bool CvUnitDefaultAI::AutomatedPillage(int iBonusValueThreshold)
{
	PROFILE_FUNC();

	FAssertMsg(m_unit->isHuman(), "should not be called for AI's");

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

		if (plotValid(pLoopPlot) && m_unit->getGroup()->canPillage(pLoopPlot) && pLoopPlot->area() == m_unit->area())
		{
			if (!pLoopPlot->isNPC() || !GET_PLAYER(m_unit->getOwner()).isModderOption(MODDEROPTION_AUTO_PILLAGE_AVOID_BARBARIAN_CITIES))
			{
				if (m_unit->potentialWarAction(pLoopPlot))
				{
					if (GET_PLAYER(m_unit->getOwner()).isModderOption(MODDEROPTION_AUTO_PILLAGE_AVOID_ENEMY_UNITS) || !pLoopPlot->isVisible(m_unit->getTeam(), false) || !pLoopPlot->isVisibleEnemyUnit(m_unit))
					{
						if (GET_PLAYER(m_unit->getOwner()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_PILLAGE, m_unit->getGroup(), 1) == 0)
						{
							if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns))
							{
								iValue = pillageValue(pLoopPlot, iBonusValueThreshold);

								iValue *= 1000;

								iValue /= (iPathTurns + 1);

								// if not at war with m_unit m_unit->plot owner, then devalue m_unit->plot if we already inside m_unit owner's borders
								// (because declaring war will pop us some unknown distance away)
								if (!m_unit->isEnemy(pLoopPlot->getTeam()) && m_unit->plot()->getTeam() == pLoopPlot->getTeam())
								{
									iValue /= 10;
								}

								if (iValue > iBestValue)
								{
									endTurnPlot = m_unit->getPathEndTurnPlot();

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
		if (m_unit->atPlot(pBestPillagePlot) && !m_unit->isEnemy(pBestPillagePlot->getTeam()))
		{
			return false;
		}

		if (m_unit->atPlot(pBestPillagePlot))
		{
			if (m_unit->isEnemy(pBestPillagePlot->getTeam()))
			{
				m_unit->getGroup()->pushMission(MISSION_PILLAGE, -1, -1, 0, false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
				return true;
			}
		}
		else
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
		}
	}

	return false;
}


void CvUnitDefaultAI::SearchAndDestroyMove(bool bWithCommander)
{
	PROFILE_FUNC();

	if (groupSelectStatus())
	{
		return;
	}

	//Apparently minimum odds is a maximum odds threshold rather than minimum
	int iMinimumOdds = m_unit->isHuman() ? GET_PLAYER(m_unit->getOwner()).getModderOption(MODDEROPTION_AUTO_HUNT_MIN_COMBAT_ODDS) : (bWithCommander ? 80 : 60);

	MissionAITypes eMissionAIType = MISSIONAI_GROUP;

	if (!m_unit->isHuman() && m_unit->plot()->getOwner() == m_unit->getOwner() && GET_PLAYER(m_unit->getOwner()).AI_unitTargetMissionAIs(m_unit, &eMissionAIType, 1, m_unit->getGroup(), 1) > 0)
	{
		// Wait for units which are joining our group m_unit turn
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	//	If we have any animal hangers-on and are in our territory drop them off
	if (m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		if (m_unit->getGroup()->countNumUnitAIType(UNITAI_SUBDUED_ANIMAL) > 0)
		{
			m_unit->getGroup()->AI_separateAI(UNITAI_SUBDUED_ANIMAL);
		}
		if (m_unit->getGroup()->countNumUnitAIType(UNITAI_WORKER) > 0)
		{
			m_unit->getGroup()->AI_separateAI(UNITAI_SUBDUED_ANIMAL);
		}
	}
	else if (!m_unit->isHuman())
	{
		if (groupMergeRange(UNITAI_SUBDUED_ANIMAL, 0, false, true, true))
		{
			return;
		}
		//	In the worker case we don't want to get sucked into a stack with an already protected
		//	worker, or one not also in non-owned territory so just search m_unit m_unit->plot
		if (group(GroupingParams().withUnitAI(UNITAI_WORKER).maxGroupSize(1).maxPathTurns(0)))
		{
			return;
		}
	}


	const UnitCombatTypes eIgnoreUnitCombat = GC.getUNITCOMBAT_SUBDUED();
	if (m_unit->getGroup()->getWorstDamagePercent(eIgnoreUnitCombat) > 0)
	{
		OutputDebugString(CvString::format("%S (%d) damaged at (%d,%d)...\n", m_unit->getDescription().c_str(), m_unit->getID(), m_unit->getX(), m_unit->getY()).c_str());
		//	If there is an adjacent enemy seek safety before we heal
		if (exposedToDanger(m_unit->plot(), 75))
		{
			OutputDebugString("	...plot is dangerous - seeking safety\n");
			if (safety())
			{
				return;
			}
		}

		if (heal())
		{
			OutputDebugString(CvString::format("	...healing at (%d,%d)\n", m_unit->getX(), m_unit->getY()).c_str());
			return;
		}

		if (m_unit->getGroup()->getWorstDamagePercent() > 25)
		{
			//	Look for somewhere safer
			if (safety(3))
			{
				return;
			}
		}
	}

	//	If we have more than 3 animal hangers-on escort them back to our territory
	if (!m_unit->isHuman()
	&& m_unit->getGroup()->countNumUnitAIType(UNITAI_WORKER) + m_unit->getGroup()->countNumUnitAIType(UNITAI_SUBDUED_ANIMAL) > 3
	&& m_unit->plot()->getOwner() != m_unit->getOwner())
	{
		if (retreatToCity())
		{
			return;
		}
	}

	if (!bWithCommander && !m_unit->isHuman() && m_unit->getGroup()->getNumUnits() == 1)
	{
		// If anyone is actively asking for a hunter that takes priority
		if (processContracts(HIGHEST_PRIORITY_ESCORT_PRIORITY))
		{
			if (gUnitLogLevel >= 3 && m_contractualState == CONTRACTUAL_STATE_FOUND_WORK)
			{
				logBBAI("	Hunter for player %d (%S) at (%d,%d) found contractual work",
					m_unit->getOwner(),
					GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0),
					m_unit->getX(),
					m_unit->getY());
			}
			return;
		}
		if (gUnitLogLevel >= 3)
		{
			logBBAI("	Hunter for player %d (%S) at (%d,%d) found no urgent contractual work",
				m_unit->getOwner(),
				GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0),
				m_unit->getX(),
				m_unit->getY());
		}
		// No one wanted us as escort, so we will advertise for an escort instead later.
		contractFulfilled();
	}

	if (huntRange(1, iMinimumOdds, false))
	{
		return;
	}

	if ((!m_unit->isHuman() || GET_PLAYER(m_unit->getOwner()).isModderOption(MODDEROPTION_AUTO_HUNT_RETURN_FOR_UPGRADES)))
	{
		if (travelToUpgradeCity())
		{
			return;
		}
	}

	{
		// Get the proper accompaniment
		const bool bContractEscort =
			(
				!m_unit->isHuman() && !m_unit->isCargo() && getUnitAIType() == UNITAI_HUNTER
				&& GET_PLAYER(m_unit->getOwner()).getBestUnitType(UNITAI_HUNTER_ESCORT) != NO_UNIT
				&& m_unit->getGroup()->countNumUnitAIType(UNITAI_HUNTER_ESCORT) < 1
			);
		if (bContractEscort)
		{
			if (m_contractsLastEstablishedTurn != GC.getGame().getGameTurn())
			{
				const int priority = HIGHEST_PRIORITY_ESCORT_PRIORITY;

				GET_PLAYER(m_unit->getOwner()).getContractBroker().advertiseWork
				(
					priority, NO_UNITCAPABILITIES,
					m_unit->getX(), m_unit->getY(),
					m_unit, UNITAI_HUNTER_ESCORT
				);
				m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
				m_contractualState = CONTRACTUAL_STATE_AWAITING_ANSWER;

				if (gUnitLogLevel > 2)
				{
					logBBAI("	%S's hunter (%d) at (%d,%d) [stack size %d] requests escort at priority %d", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getID(), m_unit->getX(), m_unit->getY(), m_unit->getGroup()->getNumUnits(), priority);
				}
			}
			// Limited operations gravitating close to borders while waiting.
			if (exposedToDanger(m_unit->plot(), 90))
			{
				if (m_unit->plot()->getOwner() != m_unit->getOwner())
				{
					if (reachHome())
					{
						return;
					}
				}
				else if (moveToBorders())
				{
					return;
				}
			}
			else if (m_unit->plot()->getOwner() == m_unit->getOwner())
			{
				if (huntRange(3, iMinimumOdds, false))
				{
					return;
				}
				if (moveToBorders())
				{
					return;
				}
				if (refreshExploreRange(3, true))
				{
					return;
				}
			}
			else
			{
				if (huntRange(2, iMinimumOdds, false))
				{
					return;
				}
				if (reachHome())
				{
					return;
				}
			}
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	if (huntRange(3, iMinimumOdds, false))
	{
		return;
	}

	if (goody(4))
	{
		return;
	}

	//	If we have animal hangers-on escort them back to our territory
	if (!m_unit->isHuman() && (m_unit->getGroup()->countNumUnitAIType(UNITAI_SUBDUED_ANIMAL) > 0 || m_unit->getGroup()->countNumUnitAIType(UNITAI_WORKER) > 0) && m_unit->plot()->getOwner() != m_unit->getOwner())
	{
		if (retreatToCity())
		{
			return;
		}
	}
	if (huntRange(5, iMinimumOdds, false))
	{
		return;
	}

	if (!bWithCommander && !m_unit->isHuman()
	&& GET_PLAYER(m_unit->getOwner()).AI_isFinancialTrouble()
	&& GET_PLAYER(m_unit->getOwner()).getUnitUpkeepNet(m_unit->isMilitaryBranch(), m_unit->getUpkeep100()) > 0)
	{
		const int iNeededHunters = GET_PLAYER(m_unit->getOwner()).AI_neededHunters(m_unit->area());
		const int iHasHunters = GET_PLAYER(m_unit->getOwner()).AI_totalAreaUnitAIs(m_unit->area(), UNITAI_HUNTER);

		if (iHasHunters > iNeededHunters + 1)
		{
			if (gUnitLogLevel >= 2)
			{
				logBBAI("%S has %d hunters (%d needed) - scrapping", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), iHasHunters, iNeededHunters);
			}
			m_unit->scrap();
			return;
		}
	}

	if (refreshExploreRange(3, true))
	{
		return;
	}

	if (moveToBorders())
	{
		return;
	}

	if (patrol())
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}

bool CvUnitDefaultAI::huntRange(int iRange, int iOddsThreshold, bool bStayInBorders, int iMinValue)
{
	PROFILE_FUNC();


	int localOddsThreshold = iOddsThreshold > 99 ? 99 : iOddsThreshold;
	
	const bool bCanCaptureCities =
		m_unit->getGroup()->getAutomateType() == AUTOMATE_HUNT && !GET_PLAYER(m_unit->getOwner()).isModderOption(MODDEROPTION_AUTO_HUNT_NO_CITY_CAPTURING) ||
		m_unit->getGroup()->getAutomateType() == AUTOMATE_BORDER_PATROL && !GET_PLAYER(m_unit->getOwner()).isModderOption(MODDEROPTION_AUTO_PATROL_NO_CITY_CAPTURING);

	int bestScore = iMinValue;
	const CvPlot* bestTargetPlot = NULL;

	foreach_(const CvPlot * potentialTargetPlot, m_unit->plot()->rect(iRange, iRange))
	{
		if (!m_unit->atPlot(potentialTargetPlot)
			&& (!bStayInBorders || potentialTargetPlot->getOwner() == m_unit->getOwner())
			&& (!potentialTargetPlot->isCity() || bCanCaptureCities)
			&& plotValid(potentialTargetPlot)
			&& potentialTargetPlot->isVisible(m_unit->getTeam(), false))
		{
			const bool huntingAnimals = getUnitAIType() == UNITAI_HUNTER && algo::any_of(potentialTargetPlot->units(), CvUnit::fn::isAnimal());
			if ((huntingAnimals || potentialTargetPlot->isVisibleEnemyUnit(m_unit))
				&& m_unit->getGroup()->canEnterPlot(potentialTargetPlot, true)
				&& m_unit->generatePath(potentialTargetPlot, 0, true, nullptr, iRange)
				)
			{
				const int attackOdds = m_unit->getGroup()->AI_attackOdds(potentialTargetPlot, true);
				const int plotScore = attackOdds + (huntingAnimals ? 25 : 0);

				if (plotScore > bestScore && attackOdds >= finalOddsThreshold(potentialTargetPlot, localOddsThreshold))
				{
					bestScore = plotScore;
					bestTargetPlot = m_unit->getPathEndTurnPlot();
				}
			}
		}
	}

	if (bestTargetPlot != nullptr)
	{
		FAssert(!m_unit->atPlot(bestTargetPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, bestTargetPlot->getX(), bestTargetPlot->getY(), MOVE_DIRECT_ATTACK, false, false);
	}

	return false;
}

void CvUnitDefaultAI::cityDefense()
{
	int iMinimumOdds = GET_PLAYER(m_unit->getOwner()).getModderOption(MODDEROPTION_AUTO_DEFENSE_MIN_COMBAT_ODDS);
	bool bCanLeaveCity = GET_PLAYER(m_unit->getOwner()).isModderOption(MODDEROPTION_AUTO_DEFENSE_CAN_LEAVE_CITY);

	if (returnToBorders())
	{
		return;
	}

	if (guardCityBestDefender())
	{
		return;
	}

	if (guardCityMinDefender(false))
	{
		return;
	}

	if (leaveAttack(2, iMinimumOdds, 100))
	{
		return;
	}

	if (leaveAttack(3, iMinimumOdds + 10, 130))
	{
		return;
	}

	if (guardCity(bCanLeaveCity, bCanLeaveCity))
	{
		return;
	}

	if (heal())
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitDefaultAI::borderPatrol()
{
	PROFILE_FUNC();

	bool bStayInBorders = !GET_PLAYER(m_unit->getOwner()).isModderOption(MODDEROPTION_AUTO_PATROL_CAN_LEAVE_BORDERS);
	int iMinimumOdds = GET_PLAYER(m_unit->getOwner()).getModderOption(MODDEROPTION_AUTO_PATROL_MIN_COMBAT_ODDS);

	if (returnToBorders())
	{
		return;
	}

	if (heal())
	{
		return;
	}

	if (huntRange(1, iMinimumOdds, bStayInBorders))
	{
		return;
	}

	if (huntRange(2, iMinimumOdds, bStayInBorders))
	{
		return;
	}

	if (huntRange(3, iMinimumOdds, true))
	{
		return;
	}

	if (huntRange(5, iMinimumOdds + 5, true))
	{
		return;
	}

	if (patrolBorders())
	{
		return;
	}

	if (huntRange(7, iMinimumOdds + 10, true))
	{
		return;
	}

	if (huntRange(12, iMinimumOdds + 15, true))
	{
		return;
	}

	if (!bStayInBorders)
	{
		if (patrol())
		{
			return;
		}
	}

	if (retreatToCity())
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}

bool CvUnitDefaultAI::returnToBorders()
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	CvPlot* endTurnPlot = NULL;
	int iPathTurns;
	int iValue;

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	//Allows the unit to be a maximum of 2 tiles from our borders before ordering him back
	if (m_unit->plot()->getOwner() == m_unit->getOwner())
	{
		return false;
	}
	foreach_(const CvPlot * pAdjacentPlot, m_unit->plot()->adjacent())
	{
		if (pAdjacentPlot->getOwner() == m_unit->getOwner())
		{
			return false;
		}
		if (algo::any_of(pAdjacentPlot->adjacent(), CvPlot::fn::getOwner() == m_unit->getOwner()))
		{
			return false;
		}
	}

	CvReachablePlotSet plotSet(m_unit->getGroup(), 0, MAX_INT);

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		pLoopPlot = itr.plot();
		if (/*AI_plotValid(pLoopPlot) &&*/ pLoopPlot->area() == m_unit->area())
		{
			if (pLoopPlot->getOwner() == m_unit->getOwner())
			{
				if (!pLoopPlot->isVisible(m_unit->getTeam(), false) || !pLoopPlot->isVisibleEnemyUnit(m_unit))
				{
					if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns))
					{
						iValue = 1000;
						iValue /= (iPathTurns + 1);

						if (iValue > iBestValue)
						{
							endTurnPlot = m_unit->getPathEndTurnPlot();

							if (endTurnPlot == pLoopPlot || !exposedToDanger(endTurnPlot, 65))
							{
								iBestValue = iValue;
								pBestPlot = endTurnPlot;
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (!isWaitingOnUnitAIAny())
		{
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
		}
		return m_unit->getGroup()->pushMissionInternal(MISSION_SKIP);
	}

	return false;
}


//	headToBorder is used to cause a unit within our borders to move towards them.  It will
//	prefer borders with players we are at war with, over nborders to neutral, over borders
//	to players we are not at war with
bool CvUnitDefaultAI::moveToBorders()
{
	PROFILE_FUNC();

	const CvPlot* pBestPlot = NULL;
	const CvPlot* endTurnPlot = NULL;
	int		iDistance = 1;
	int		iBestValue = 0;
	bool	bValid;

	//	Just intended for units inside our territory to head out
	if (m_unit->plot()->getOwner() != m_unit->getOwner())
	{
		return false;
	}

	do
	{
		bValid = false;

		//	Examine the ring of plots iDistance away from us (m_unit is a square)
		for (int iPerimeter = -iDistance; iPerimeter < iDistance; iPerimeter++)
		{
			for (int iSide = 0; iSide < 4; iSide++)
			{
				int iX = m_unit->getX();
				int iY = m_unit->getY();

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
					if (plotValid(pLoopPlot) && pLoopPlot->area() == m_unit->area())
					{
						if (pLoopPlot->getOwner() == m_unit->getOwner())
						{
							//	Still within our territory somewhere on m_unit ring
							bValid = true;

							int iValue = 0;

							foreach_(const CvPlot * pAdjacentPlot, pLoopPlot->adjacent())
							{
								if (!pAdjacentPlot->isWater() && pAdjacentPlot->getOwner() != m_unit->getOwner())
								{
									iValue += GC.getGame().getSorenRandNum(300, "AI Move to Border");

									if (NO_PLAYER == pAdjacentPlot->getOwner())
									{
										iValue += 100;
									}
									else if (GET_TEAM(m_unit->getTeam()).isAtWar(pAdjacentPlot->getTeam()))
									{
										iValue += 200 + GC.getGame().getSorenRandNum(100, "AI Move to Border2");
									}
								}
							}

							if (iValue * 10 > iBestValue)
							{
								int iPathTurns;

								if (m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns))
								{
									iValue = (iValue * 50) / (iPathTurns + 5);

									if (iValue > iBestValue)
									{
										endTurnPlot = m_unit->getPathEndTurnPlot();
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

	//	If we're already at the best border m_unit->plot m_unit mission isn't useful
	if (pBestPlot != NULL && !m_unit->atPlot(pBestPlot) && endTurnPlot != NULL)
	{
		if (!isWaitingOnUnitAIAny())
		{
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY());
		}
		return m_unit->getGroup()->pushMissionInternal(MISSION_SKIP);
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
//the AI actually never use m_unit function, only automated human units do.
bool CvUnitDefaultAI::patrolBorders()
{
	PROFILE_FUNC();

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	const int iSearchRange = m_unit->baseMoves();

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		if (m_unit->canEnterPlot(pLoopPlot, MoveCheck::IgnoreLoad))
		{
			const DirectionTypes eNewDirection = estimateDirection(m_unit->plot(), pLoopPlot);
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
			if (eNewDirection == getOppositeDirection(m_unit->getFacingDirection(false)))
			{
				iValue /= 25;
			}
			else if (isAdjacentDirection(getOppositeDirection(m_unit->getFacingDirection(false)), eNewDirection))
			{
				iValue /= 10;
			}
			if (pLoopPlot->getOwner() != m_unit->getOwner())
			{
				if (GET_PLAYER(m_unit->getOwner()).isModderOption(MODDEROPTION_AUTO_PATROL_CAN_LEAVE_BORDERS))
				{
					iValue = -1;
				}
				else
				{
					iValue /= 10;
				}
			}
			if (m_unit->getDomainType() == DOMAIN_LAND && pLoopPlot->isWater() || m_unit->getDomainType() == DOMAIN_SEA && !pLoopPlot->isWater())
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
			FAssert(!m_unit->atPlot(pBestPlot));
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
		}
		return m_unit->getGroup()->pushMissionInternal(MISSION_SKIP);
	}
	return false;
}

void CvUnitDefaultAI::AutomatedpillageMove()
{
	PROFILE_FUNC();

	if (heal(30, 1))
	{
		return;
	}
	if (pillageRange(6, 20))
	{
		return;
	}

	if (pillageRange(3, 11))
	{
		return;
	}

	if (pillageRange(1))
	{
		return;
	}

	if (heal(50, 3))
	{
		return;
	}

	if (!m_unit->isEnemy(m_unit->plot()->getTeam()))
	{
		if (heal())
		{
			return;
		}
	}

	if ((m_unit->area()->getAreaAIType(m_unit->getTeam()) == AREAAI_OFFENSIVE) || m_unit->isEnemy(m_unit->plot()->getTeam()))
	{
		if (pillage(20))
		{
			return;
		}
	}

	if (heal())
	{
		return;
	}

	if (retreatToCity())
	{
		return;
	}

	if (safety())
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitDefaultAI::autoAirStrike()
{
	PROFILE_FUNC();

	CvPlayerAI& kPlayer = GET_PLAYER(m_unit->getOwner());
	CvArea* pArea = m_unit->area();

	//Heal
	if (m_unit->getDamage() > 0 && 100 * m_unit->getHP() / m_unit->getMaxHP() < 50)
	{
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	// Attack the invaders!
	if (defendBaseAirStrike())
	{
		return;
	}

	//Attack enemies in range
	if (defensiveAirStrike())
	{
		return;
	}

	//Attack anyone
	if (airStrike())
	{
		return;
	}

	if (kPlayer.isModderOption(MODDEROPTION_AUTO_AIR_CAN_REBASE))
	{
		// If no targets, no sense staying in risky place
		if (airOffensiveCity())
		{
			return;
		}

		if (m_unit->canAirDefend())
		{
			if (airDefensiveCity())
			{
				return;
			}
		}

		if (m_unit->healTurns(m_unit->plot()) > 1)
		{
			// If very damaged, no sense staying in risky place
			if (airOffensiveCity())
			{
				return;
			}

			if (m_unit->canAirDefend())
			{
				if (airDefensiveCity())
				{
					return;
				}
			}
		}
	}

	if (kPlayer.isModderOption(MODDEROPTION_AUTO_AIR_CAN_DEFEND))
	{
		int iAttackValue = kPlayer.AI_unitValue(m_unit->getUnitType(), UNITAI_ATTACK_AIR, pArea);
		int iDefenseValue = kPlayer.AI_unitValue(m_unit->getUnitType(), UNITAI_DEFENSE_AIR, pArea);
		if (iDefenseValue > iAttackValue)
		{
			if (kPlayer.AI_bestAreaUnitAIValue(UNITAI_ATTACK_AIR, pArea) > iAttackValue)
			{
				setUnitAIType(UNITAI_DEFENSE_AIR);
				m_unit->getGroup()->pushMission(MISSION_SKIP);
				return;
			}
		}
	}


	bool bDefensive = false;
	if (pArea != NULL)
	{
		bDefensive = pArea->getAreaAIType(m_unit->getTeam()) == AREAAI_DEFENSIVE;
	}
	if (kPlayer.isModderOption(MODDEROPTION_AUTO_AIR_CAN_REBASE) && kPlayer.isModderOption(MODDEROPTION_AUTO_AIR_CAN_DEFEND))
	{
		if (GC.getGame().getSorenRandNum(bDefensive ? 3 : 6, "AI Air Attack Move") == 0)
		{
			if (defensiveAirStrike())
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
			if (travelToUpgradeCity())
			{
				return;
			}
		}
	}

	// Support ground attacks
	if (m_unit->canAirBomb())
	{
		if (airBombDefenses())
		{
			return;
		}

		if (airBombPlots())
		{
			return;
		}
	}

	if (airStrike())
	{
		return;
	}

	if (airBombCities())
	{
		return;
	}

	if (m_unit->canAirDefend() && kPlayer.isModderOption(MODDEROPTION_AUTO_AIR_CAN_DEFEND))
	{
		if (bDefensive || GC.getGame().getSorenRandNum(2, "AI Air Attack Move") == 0)
		{
			m_unit->getGroup()->pushMission(MISSION_AIRPATROL);
			return;
		}
	}

	if (m_unit->canRecon())
	{
		if (exploreAir())
		{
			return;
		}
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}

// Returns true if a mission was pushed...
bool CvUnitDefaultAI::airBombCities()
{
	//PROFILE_FUNC();

	const int iSearchRange = m_unit->airRange();

	int iBestValue = (m_unit->isSuicide() && m_unit->getUnitInfo().getProductionCost() > 0) ? (5 * m_unit->getUnitInfo().getProductionCost()) / 6 : 0;
	const CvPlot* pBestPlot = NULL;

	foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
	{
		if (m_unit->canEnterPlot(pLoopPlot, MoveCheck::Attack))
		{
			int iValue = 0;
			const int iPotentialAttackers = pLoopPlot->getNumVisiblePotentialEnemyDefenders(m_unit);

			if (iPotentialAttackers > 0)
			{
				const CvUnit* pDefender = pLoopPlot->getBestDefender(NO_PLAYER, m_unit->getOwner(), m_unit, true);

				FAssert(pDefender != NULL && pDefender->canDefend());

				if (pDefender != NULL && pDefender->canDefend())
				{
					// XXX factor in air defenses...

					const int iDamage = m_unit->airCombatDamage(pDefender);

					iValue = std::max(0, (std::min((pDefender->getDamage() + iDamage), m_unit->airCombatLimit(pDefender)) - pDefender->getDamage()));

					iValue += ((((iDamage * m_unit->collateralDamage()) / 100) * std::min((pLoopPlot->getNumVisiblePotentialEnemyDefenders(m_unit) - 1), m_unit->collateralDamageMaxUnits())) / 2);

					iValue *= (3 + iPotentialAttackers);
					iValue /= 4;

					const CvUnit* pInterceptor = m_unit->bestInterceptor(pLoopPlot);

					if (pInterceptor != NULL)
					{
						int iInterceptProb = m_unit->isSuicide() ? 100 : pInterceptor->currInterceptionProbability();

						iInterceptProb *= std::max(0, (100 - m_unit->evasionProbability()));
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
						FAssert(!m_unit->atPlot(pBestPlot));
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!m_unit->atPlot(pBestPlot));
		return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pBestPlot->getX(), pBestPlot->getY());
	}

	return false;
}


void CvUnitDefaultAI::shadowMove()
{
	PROFILE_FUNC();

	CvUnit* pTarget = m_unit->getShadowUnit();
	FAssertMsg(pTarget != NULL, "Should be Shading a Unit!");

	if (protectTarget(pTarget))
	{
		return;
	}

	if (moveToTarget(pTarget))
	{
		return;
	}

	m_unit->getGroup()->pushMission(MISSION_SKIP);
	return;
}

bool CvUnitDefaultAI::moveToTarget(const CvUnit* pTarget)
{
	PROFILE_FUNC();

	if (m_unit->atPlot(pTarget->plot()))
	{
		return false;
	}

	int iPathTurns;
	if (!isWaitingOnUnitAIAny() && m_unit->generatePath(pTarget->plot(), 0, true, &iPathTurns))
	{
		const CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();
		if (endTurnPlot != NULL)
		{
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY());
		}
	}

	return false;
}

bool CvUnitDefaultAI::protectTarget(const CvUnit* pTarget)
{
	PROFILE_FUNC();

	//const CvPlot* endTurnPlot = NULL;
	int iPathTurns;

	const int iSearchRange = m_unit->baseMoves();

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;

	const int iDanger = GET_PLAYER(m_unit->getOwner()).AI_getPlotDanger(pTarget->plot(), 1, false);

	//No Danger
	if (iDanger == 0)
	{
		return false;
	}

	//Lots of Danger, Move Ontop of Target to protect it
	else if (iDanger > m_unit->getGroup()->getNumUnits())
	{
		if (m_unit->generatePath(pTarget->plot(), 0, true, &iPathTurns))
		{
			const CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();
			m_unit->getGroup()->pushMission(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY());
			return true;
		}
	}

	//Only minimal enemy targets, move to kill them if possible
	else
	{
		foreach_(const CvPlot * pLoopPlot, m_unit->plot()->rect(iSearchRange, iSearchRange))
		{
			if (plotValid(pLoopPlot))
			{
				if (pLoopPlot->isVisible(m_unit->getTeam(), false) && pLoopPlot->isVisibleEnemyUnit(m_unit))
				{
					if (!m_unit->atPlot(pLoopPlot) && m_unit->canEnterPlot(pLoopPlot, MoveCheck::Attack) && m_unit->generatePath(pLoopPlot, 0, true, &iPathTurns, 1))
					{
						if (pLoopPlot->getNumVisiblePotentialEnemyDefenders(m_unit) <= m_unit->getGroup()->getNumUnits())
						{
							if (pLoopPlot->getNumVisibleAdjacentEnemyDefenders(m_unit) <= ((m_unit->getGroup()->getNumUnits() * 3) / 2))
							{
								const int iValue = m_unit->getGroup()->AI_attackOdds(pLoopPlot, true);

								if (iValue >= finalOddsThreshold(pLoopPlot, 65))
								{
									if (iValue > iBestValue)
									{
										iBestValue = iValue;
										pBestPlot = m_unit->getPathEndTurnPlot();
										FAssert(!m_unit->atPlot(pBestPlot));
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
		if (m_unit->atPlot(pTarget->plot()))
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
		else if (!isWaitingOnUnitAIAny() && m_unit->generatePath(pTarget->plot(), 0, true, &iPathTurns))
		{
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, pTarget->getX(), pTarget->getY());
		}
		else
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
			return true;
		}
	}
	else
	{
		m_unit->getGroup()->pushMission(MISSION_SKIP);
		return true;
	}
}

bool CvUnitDefaultAI::joinMilitaryCity(bool bNaval)
{
	PROFILE_FUNC();

	int iPathTurns = 0;
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	SpecialistTypes eBestSpecialist = NO_SPECIALIST;

	foreach_(CvCity * pLoopCity, GET_PLAYER(m_unit->getOwner()).cities())
	{
		if (pLoopCity->area() == m_unit->area())
		{
			const CvPlot* plot = pLoopCity->plot();

			if (plotValid(plot)
			&& (bNaval ? pLoopCity->AI_isNavalMilitaryProductionCity() : pLoopCity->AI_isMilitaryProductionCity())
			&& !plot->isVisibleEnemyUnit(m_unit)
			&& generateSafePathforVulnerable(plot, &iPathTurns))
			{
				for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
				{
					if (m_unit->canJoin(plot, (SpecialistTypes)iI))
					{
						const int iValue = pLoopCity->AI_specialistValue((SpecialistTypes)iI, pLoopCity->AI_avoidGrowth(), false);
						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = m_unit->getPathEndTurnPlot();
							eBestSpecialist = (SpecialistTypes)iI;
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (m_unit->atPlot(pBestPlot))
		{
			m_unit->getGroup()->pushMission(MISSION_JOIN, eBestSpecialist);
			return true;
		}
		if (generateSafePathforVulnerable(pBestPlot, &iPathTurns))
		{
			const CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();
			return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY(), MOVE_IGNORE_DANGER);
		}
		m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
		return true;
	}

	return false;
}

bool CvUnitDefaultAI::isPlotWellDefended(const CvPlot* pPlot, bool bIncludeAdjacent, int iOddsOfDefeat) const
{
	PROFILE_FUNC();

	const int iOurOffense = GET_TEAM(m_unit->getTeam()).AI_getOurPlotStrength(m_unit->plot(), bIncludeAdjacent, true, false, true);
	const int iEnemyDefense = GET_PLAYER(m_unit->getOwner()).AI_getEnemyPlotStrength(pPlot, bIncludeAdjacent, true, false);

	return iOddsOfDefeat * iOurOffense < 100 * iEnemyDefense;
}


void CvUnitDefaultAI::flushValueCache()
{
	m_iGenericValue = -1;
}

//	Assess the value of a unit without the context of any specific battle/scenario
//	This is its max strength, modified by promotions
int	CvUnitDefaultAI::genericUnitValueTimes100(UnitValueFlags eFlags) const
{
	PROFILE_FUNC();

	if (m_iGenericValue == -1 || m_eGenericValueFlagsCached != eFlags)
	{
		int	iResult = 100 * m_unit->baseCombatStrNonGranular();

		foreach_(const STD_PAIR(PromotionTypes, PromotionKeyedInfo) & keyedInfo, m_unit->getPromotionKeyedInfo())
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
				for (int iJ = 0; iJ < GC.getNumUnitCombatInfos(); iJ++)
				{
					if (kPromotion.getUnitCombatModifierPercent((UnitCombatTypes)iJ) != 0)
					{
						//	Future - make m_unit adaptive to the m_unit->area's known enemy units
						//	Note - the scaling factor of 5 (implicit in the 500) is because
						//	although there are actually 40 or so unit combat types only a small
						//	fraction are in wide use or appear in promotions in practice.  Once m_unit
						//	is normalized for actual occurance of units that fudge factor won't be needed
						int	iUnitCombatWeight = 500 / GC.getNumUnitCombatInfos();

						iResult = (iResult * (100 + (iUnitCombatWeight * kPromotion.getUnitCombatModifierPercent((UnitCombatTypes)iJ)) / 100)) / 100;

						bPromotionHasAccountedValue |= (kPromotion.getUnitCombatModifierPercent((UnitCombatTypes)iJ) > 0);
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
					for (int iJ = 0; iJ < GC.getNumTerrainInfos(); iJ++)
					{
						if (kPromotion.getTerrainDefensePercent((TerrainTypes)iJ) != 0 ||
							 kPromotion.getIgnoreTerrainDamage() == iJ)
						{
							int iNumRevealedAreaTiles = std::max(1, m_unit->area()->getNumRevealedTiles(m_unit->getTeam()));
							int	iNumRevealedAreaThisTerrain = m_unit->area()->getNumRevealedTerrainTiles(m_unit->getTeam(), (TerrainTypes)iJ);

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
					//	Feature defense
					for (int iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
					{
						if (kPromotion.getFeatureDefensePercent((FeatureTypes)iJ) != 0)
						{
							int iNumRevealedAreaTiles = std::max(1, m_unit->area()->getNumRevealedTiles(m_unit->getTeam()));
							int	iNumRevealedAreaThisFeature = m_unit->area()->getNumRevealedFeatureTiles(m_unit->getTeam(), (FeatureTypes)iJ);

							//	Note - used 1000 multiplier here for better granularity because there are
							//	a lot of feature types.  We multiply the actual ratio by 2 because in practise the AI will
							//	deploy the units to appropriate terrains/features wehn it can so the the effective value is
							//	higher than the raw proportion would apply
							int	iFeatureWeight = (2 * 1000 * iNumRevealedAreaThisFeature) / iNumRevealedAreaTiles;

							iResult = (iResult * (10000 + (kPromotion.getFeatureDefensePercent((FeatureTypes)iJ) * iFeatureWeight) / 10)) / 10000;

							bPromotionHasAccountedValue |= (kPromotion.getFeatureDefensePercent((FeatureTypes)iJ) > 0);
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
						int		iFirstStrikeWeight = 120;	//	Future - make m_unit adaptive to known enemy units

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
						//may want to include Blitz into m_unit factor somehow
						int iOffensiveVictoryMoveWeight = 140;

						iResult = (iResult * iOffensiveVictoryMoveWeight) / 100;

						bPromotionHasAccountedValue = true;
					}

					//	Terrain attack
					for (int iJ = 0; iJ < GC.getNumTerrainInfos(); iJ++)
					{
						if (kPromotion.getTerrainAttackPercent((TerrainTypes)iJ) != 0 ||
							 kPromotion.getIgnoreTerrainDamage() == iJ)
						{
							int iNumRevealedAreaTiles = std::max(1, m_unit->area()->getNumRevealedTiles(m_unit->getTeam()));
							int	iNumRevealedAreaThisTerrain = m_unit->area()->getNumRevealedTerrainTiles(m_unit->getTeam(), (TerrainTypes)iJ);

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
					//	Feature attack
					for (int iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
					{
						if (kPromotion.getFeatureAttackPercent((FeatureTypes)iJ) != 0)
						{
							int iNumRevealedAreaTiles = std::max(1, m_unit->area()->getNumRevealedTiles(m_unit->getTeam()));
							int	iNumRevealedAreaThisFeature = m_unit->area()->getNumRevealedFeatureTiles(m_unit->getTeam(), (FeatureTypes)iJ);

							//	Note - used 1000 multiplier here for better granularity because there are
							//	a lot of feature types.  We multiply the actual ratio by 2 because in practise the AI will
							//	deploy the units to appropriate terrains/features wehn it can so the the effective value is
							//	higher than the raw proportion would apply
							int	iFeatureWeight = (2 * 1000 * iNumRevealedAreaThisFeature) / iNumRevealedAreaTiles;

							iResult = (iResult * (10000 + (kPromotion.getFeatureAttackPercent((FeatureTypes)iJ) * iFeatureWeight) / 10)) / 10000;

							bPromotionHasAccountedValue |= (kPromotion.getFeatureAttackPercent((FeatureTypes)iJ) > 0);
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
					//	We don't know what m_unit promotion is for but it must be worth something
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
bool CvUnitDefaultAI::approximatePath(CvPlot* pToPlot, int iFlags, int* piPathTurns) const
{
	PROFILE_FUNC();

#define CLOSE_THRESHOLD 10
	CvPlot*	start = m_unit->plot();
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
	bool bResult = m_unit->generatePath(pToPlot, iFlags, true, piPathTurns);

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

	//	Make sure we can path m_unit stack to here, else shrink it back until we can
	FAStarNode* pCandidateSubPathEndNode = pNode;

	while (pNode->m_pParent != NULL)
	{
		if (!m_unit->canEnterPlot(GC.getMap().plot(pNode->m_iX, pNode->m_iY)))
		{
			pCandidateSubPathEndNode = pNode->m_pParent;
		}

		pNode = pNode->m_pParent;
	}

	CvPlot* pSubPathTerminus = GC.getMap().plot(pCandidateSubPathEndNode->m_iX, pCandidateSubPathEndNode->m_iY);
	int iSubPathTurns;

	{
		PROFILE("AI_approximatePath.DistantSubPathCalc");
		if (!m_unit->generatePath(pSubPathTerminus, iFlags, true, &iSubPathTurns))
		{
			OutputDebugString("Unpathable sub-path found!!\n");

			//	This should never happen - if it does something has goen wrong - just evaluate the
			//	entire path
			return m_unit->generatePath(pToPlot, iFlags, true, piPathTurns);
		}

		//	Now normalise the step path length by the ratio of the subpath step length to its actual length
		*piPathTurns = (iStepPathLen * iSubPathTurns) / std::max(1, pCandidateSubPathEndNode->m_iData1);

		OutputDebugString(CvString::format("Sub path evaluation yielded length of %d vs %d, normalising total to %d\n", iSubPathTurns, pCandidateSubPathEndNode->m_iData1, *piPathTurns).c_str());
	}
}

return true;
}
*/

bool CvUnitDefaultAI::isCityGarrison(const CvCity* pCity) const
{
	return (m_iGarrisonCity != -1 && m_iGarrisonCity == pCity->getID());
}

void CvUnitDefaultAI::setAsGarrison(const CvCity* pCity)
{
	int iGarrisonCity = (pCity == NULL ? -1 : pCity->getID());

	m_iAffirmedGarrisonCity = iGarrisonCity;

	if (iGarrisonCity != m_iGarrisonCity)
	{
		if (pCity == NULL)
		{
			pCity = GET_PLAYER(m_unit->getOwner()).getCity(m_iGarrisonCity);
		}

		m_iGarrisonCity = iGarrisonCity;

		if (gUnitLogLevel >= 3)
		{
			if (iGarrisonCity == -1)
			{
				if (pCity != NULL)
				{
					logBBAI("	Unit %S (group %d with %d units) for player %d (%S) at (%d,%d) leaves garrison for city %S\n",
							m_unit->getUnitInfo().getDescription(),
							m_unit->getGroup()->getID(),
							m_unit->getGroup()->getNumUnits(),
							m_unit->getOwner(),
							GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0),
							m_unit->getX(),
							m_unit->getY(),
							pCity->getName().GetCString());
				}
			}
			else
			{
				FAssert(pCity != NULL);
				logBBAI("	Unit %S (group %d with %d units) for player %d (%S) at (%d,%d) joins garrison for city %S\n",
						m_unit->getUnitInfo().getDescription(),
						m_unit->getGroup()->getID(),
						m_unit->getGroup()->getNumUnits(),
						m_unit->getOwner(),
						GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0),
						m_unit->getX(),
						m_unit->getY(),
						pCity->getName().GetCString());
			}
		}
	}
}


/*TB Prophet Mod begin*/
bool CvUnitDefaultAI::foundReligion()
{
	PROFILE_FUNC();

	ReligionTypes eReligion;
	int iI;
	int iJ;
	int value;
	int bestValue;

	ReligionTypes eBestReligion = NO_RELIGION;
	int iProphetCount = GET_PLAYER(m_unit->getOwner()).AI_getNumAIUnits(UNITAI_PROPHET);

	const ReligionTypes eFavorite = (ReligionTypes)GC.getLeaderHeadInfo(GET_PLAYER(m_unit->getOwner()).getLeaderType()).getFavoriteReligion();
	if (GC.getGame().isOption(GAMEOPTION_DIVINE_PROPHETS))
	{
		if (eFavorite != NO_RELIGION && !GC.getGame().isReligionFounded(eFavorite))
		{
			if (m_unit->canSpread(m_unit->plot(), eFavorite))
			{
				//if favorite religion of current player was not founded:
		//	push mission 'found religion' with parameter 'favorite religion' and return true
				m_unit->getGroup()->pushMission(MISSION_SPREAD, eFavorite);
				return true;
			}
		}
		//if favorite religion was not founded and CAN'T yet be, hold on to one prophet.			   GC.getLeaderHeadInfo(GET_PLAYER(m_unit->getOwner()).getLeaderType()).getFavoriteReligion()(GC.getTechInfo((TechTypes)(GC.getBonusInfo((BonusTypes)iK).getTechCityTrade())).getEra()
		if (eFavorite != NO_RELIGION &&
			!GC.getGame().isReligionFounded(eFavorite) &&
			(((int)(GC.getTechInfo(GC.getReligionInfo(eFavorite).getTechPrereq()).getEra()) - (int)(GET_PLAYER(m_unit->getOwner()).getCurrentEra()) < 2)) &&
			(iProphetCount == 1))
		{
			m_unit->getGroup()->pushMission(MISSION_SKIP);
			return true;
		}

		value = 0;
		bestValue = 0;
		eBestReligion = NO_RELIGION;

		//go over all religions that can be founded:
		if (!GC.getGame().isOption(GAMEOPTION_LIMITED_RELIGIONS))
		{
			for (iI = 0; iI < GC.getNumReligionInfos(); iI++)
			{
				if (m_unit->canSpread(m_unit->plot(), (ReligionTypes)iI))
				{
					if (!GC.getGame().isReligionFounded((ReligionTypes)iI))
					{
						for (iJ = 0; iJ < GC.getNumFlavorTypes(); iJ++)
						{
							value += std::max(1, GET_PLAYER(m_unit->getOwner()).AI_getFlavorValue((FlavorTypes)iJ)) * std::max(1, GC.getReligionInfo((ReligionTypes)iI).getFlavorValue((FlavorTypes)iJ));
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
				m_unit->getGroup()->pushMission(MISSION_SPREAD, eReligion);
				return true;
			}
		}

		if (GC.getGame().isOption(GAMEOPTION_LIMITED_RELIGIONS) && GC.getGame().isReligionFounded(eFavorite))
		{
			for (iI = 0; iI < GC.getNumReligionInfos(); iI++)
			{
				if (m_unit->canSpread(m_unit->plot(), (ReligionTypes)iI))
				{
					if (!GC.getGame().isReligionFounded((ReligionTypes)iI))
					{
						for (iJ = 0; iJ < GC.getNumFlavorTypes(); iJ++)
						{
							value += (GET_PLAYER(m_unit->getOwner()).AI_getFlavorValue((FlavorTypes)iJ) + 1) * (GC.getReligionInfo((ReligionTypes)iI).getFlavorValue((FlavorTypes)iJ) + 1);
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
				m_unit->getGroup()->pushMission(MISSION_SPREAD, eReligion);
				return true;
			}
		}
	}
	return false;
}

#ifdef OUTBREAKS_AND_AFFLICTIONS
bool CvUnitDefaultAI::cureAffliction(PromotionLineTypes eAfflictionLine)
{
	PROFILE_FUNC();

	if (m_unit->canCure(m_unit->plot(), eAfflictionLine))
	{
		m_unit->getGroup()->pushMission(MISSION_CURE, eAfflictionLine);
		return true;
	}
	return false;
}
#endif
/*TB Prophet Mod end*/

void unitSourcesValueToCity(const CvGameObject* pObject, const CvPropertyManipulators* pMani, const CvUnit* pUnit, const CvCityAI* pCity, int* iValue, PropertyTypes eProperty)
{
	if (pCity == NULL)
	{
		pCity = static_cast<CvCityAI*>(GET_PLAYER(pUnit->getOwner()).getCapitalCity());
	}

	if (pCity != NULL)
	{
		foreach_(const CvPropertySource * pSource, pMani->getSources())
		{
			if ((eProperty == NO_PROPERTY || pSource->getProperty() == eProperty)

			// Sources that deliver to the city or the m_unit->plot are both considered since the city m_unit->plot diffuses to the city for most properties anyway
			&& pSource->getType() == PROPERTYSOURCE_CONSTANT && (pSource->getObjectType() == GAMEOBJECT_CITY || pSource->getObjectType() == GAMEOBJECT_PLOT))
			{
				const PropertyTypes eProperty = pSource->getProperty();
				const int iCurrentSourceSize = pCity->getTotalBuildingSourcedProperty(eProperty) + pCity->getTotalUnitSourcedProperty(eProperty) + pCity->getPropertyNonBuildingSource(eProperty);
				const int iNewSourceSize = iCurrentSourceSize + static_cast<const CvPropertySourceConstant*>(pSource)->getAmountPerTurn(pCity->getGameObject());
				const int iDecayPercent = pCity->getPropertyDecay(eProperty);

				//	Steady state occurs at a level where the decay removes as much per turn as the sources add
				//	Decay can be 0 if the current level is below the threshold at which decay cuts in, so for the
				//	purposes of calculation just treat m_unit as very slow decay
				const int iCurrentSteadyStateLevel = (100 * iCurrentSourceSize) / std::max(1, iDecayPercent);
				const int iNewSteadyStateLevel = (100 * iNewSourceSize) / std::max(1, iDecayPercent);

				*iValue += pCity->getPropertySourceValue(eProperty, iNewSteadyStateLevel - iCurrentSteadyStateLevel);
			}
		}
	}
}

//	Find the total beneficial bnet value to a given city of m_unit unit's property sources
int CvUnitDefaultAI::beneficialPropertyValueToCity(const CvCity* pCity, PropertyTypes eProperty) const
{
	int iValue = 0;
	//void unitSourcesValueToCity(CvGameObject * pObject, CvPropertyManipulators * pMani, const CvUnit * pUnit, const CvCityAI * pCity, int* iValue, PropertyTypes eProperty)

	m_unit->getGameObject()->foreachManipulator(
		bind(unitSourcesValueToCity, m_unit->getGameObject(), _1, m_unit, static_cast<const CvCityAI*>(pCity), &iValue, eProperty)
	);

	return iValue;
}

//	Set an override to the group head priority for m_unit unit, to allow a unit that
//	would not nomally lead a stack to do so - priority will be preserved when other groups
//	joing one lead by a unit with an override, but reset if we join another group
void CvUnitDefaultAI::setLeaderPriority(int iPriority)	//	 -1 means reset to default
{
	m_iGroupLeadOverride = iPriority;
}

bool CvUnitDefaultAI::fulfillHealerNeed(const CvPlot* pPlot)
{
	const CvPlot* pBestPlot = NULL;
	const CvPlot* endTurnPlot = NULL;
	int iBestValue = 0;
	int iPathTurns = 0;
	const UnitCombatTypes eUnitCombat = m_unit->getBestHealingType();
	if (eUnitCombat == NO_UNITCOMBAT)
	{
		return false;
	}

	const PlayerTypes ePlayer = m_unit->getOwner();
	const DomainTypes eDomain = m_unit->getDomainType();

	int iValue = 0;
	foreach_(const CvPlot * pLoopPlot, pPlot->rect(25, 25))
	{
		const int iCheck = pLoopPlot->getUnitCombatsUnsupportedByHealer(ePlayer, eUnitCombat, eDomain);

		if (!m_unit->atPlot(pLoopPlot) && iCheck > 0)
		{
			if (generateSafePathforVulnerable(pLoopPlot, &iPathTurns))
			{
				iValue = iCheck;
				iValue -= iPathTurns / 4;
			}
		}
		else
		{
			if (iCheck > 0)
			{
				iValue = iCheck;
				iValue += m_unit->getNumHealSupportTotal();
			}
			else
			{
				const int iPlotCount = pLoopPlot->plotCount(PUF_isHealUnitCombatType, eUnitCombat, eDomain, NULL, ePlayer);
				iValue = std::min(iPlotCount, m_unit->getNumHealSupportTotal());
			}
		}
		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			pBestPlot = pLoopPlot;
		}
	}

	if (pBestPlot != NULL)
	{
		if (!m_unit->atPlot(pBestPlot))
		{
			if (generateSafePathforVulnerable(pBestPlot, &iPathTurns))
			{
				endTurnPlot = m_unit->getPathEndTurnPlot();
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_HEAL_SUPPORT, pBestPlot);
			}
			else
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
				return true;
			}
		}
		else
		{
			return m_unit->getGroup()->pushMissionInternal(MISSION_SKIP, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_HEAL_SUPPORT, pBestPlot);
		}
	}

	return false;
}

bool CvUnitDefaultAI::fulfillImmediateHealerNeed(const CvPlot* pPlot)
{
	const CvPlot* pBestPlot = NULL;
	const CvPlot* endTurnPlot = NULL;
	int iBestValue = 0;
	int iPathTurns = 0;
	const UnitCombatTypes eUnitCombat = m_unit->getBestHealingType();
	if (eUnitCombat == NO_UNITCOMBAT)
	{
		return false;
	}

	const PlayerTypes ePlayer = m_unit->getOwner();
	const DomainTypes eDomain = m_unit->getDomainType();

	int iValue = 0;

	foreach_(const CvPlot * pLoopPlot, pPlot->rect(4, 4))
	{
		const int iCheck = pLoopPlot->getInjuredUnitCombatsUnsupportedByHealer(ePlayer, eUnitCombat, eDomain);

		if (!m_unit->atPlot(pLoopPlot) && iCheck > 0)
		{
			if (generateSafePathforVulnerable(pLoopPlot, &iPathTurns))
			{
				iValue = iCheck;
				iValue -= iPathTurns;
			}
		}
		else
		{
			if (iCheck > 0)
			{
				iValue = iCheck;
				iValue += m_unit->getNumHealSupportTotal();
			}
			else
			{
				const int iPlotCount = pLoopPlot->plotCount(PUF_isInjuredUnitCombatType, eUnitCombat, eDomain, NULL, ePlayer);
				iValue = std::min(iPlotCount, m_unit->getNumHealSupportTotal());
			}
		}
		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			pBestPlot = pLoopPlot;
		}
	}

	if (pBestPlot != NULL)
	{
		if (!m_unit->atPlot(pBestPlot))
		{
			if (generateSafePathforVulnerable(pBestPlot, &iPathTurns))
			{
				endTurnPlot = m_unit->getPathEndTurnPlot();
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_HEAL_SUPPORT, pBestPlot);
			}
			else
			{
				m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
				return true;
			}
		}
		else
		{
			return m_unit->getGroup()->pushMissionInternal(MISSION_SKIP, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_HEAL_SUPPORT, pBestPlot);
		}
	}

	return false;
}

int CvUnitDefaultAI::scoreCityHealerNeed(const UnitCombatTypes eUnitCombat, const DomainTypes eDomain, const CvCity* city) const
{
	if (city->area() != m_unit->area() || !plotValid(city->plot()))
		return 0;
	CvPlot* targetPlot = city->plot();
	int score = 0;
	if (targetPlot != NULL && targetPlot->getArea() == m_unit->getArea())
	{
		int unsupportedUnits = targetPlot->getUnitCombatsUnsupportedByHealer(m_unit->getOwner(), eUnitCombat, eDomain);

		if (!m_unit->atPlot(targetPlot))
		{
			score = unsupportedUnits;
		}
		else if (unsupportedUnits > 0)
		{
			score = unsupportedUnits + m_unit->getNumHealSupportTotal();
		}
		else
		{
			int healersOnPlot = targetPlot->plotCount(PUF_isHealUnitCombatType, eUnitCombat, eDomain, NULL, m_unit->getOwner());
			score = std::min(healersOnPlot, m_unit->getNumHealSupportTotal());
		}
	}
	// Scale up (fairly arbitrarily) so that the distance factor that is applied will have the
	// appropriate effect (not overwhelming the base score, nor having no effect)
	return scoring::applyDistanceScoringFactor(score * 10, m_unit->plot(), targetPlot, 2);
}

bool CvUnitDefaultAI::fulfillCityHealerNeed(const CvPlot* pPlot)
{
	const UnitCombatTypes eUnitCombat = m_unit->getBestHealingType();
	if (eUnitCombat == NO_UNITCOMBAT)
	{
		return false;
	}
	const DomainTypes eDomain = m_unit->getDomainType();
	const CvPlayer& player = GET_PLAYER(m_unit->getOwner());

	using namespace scoring;
	ScoreResult<CvCity> bestCityScore = findBestScore<CvCity, GreatestScore>(
		player.beginCities(), player.endCities(),
		bind(&CvUnitDefaultAI::scoreCityHealerNeed, this, eUnitCombat, eDomain, _1), // scoring the city for healing need
		bind(canSafePathToCity, m_unit, _1) // final validation is that we can actually path to the city
	);

	if (bestCityScore.found)
	{
		CvCity* bestCity = bestCityScore.result.item;
		if (!m_unit->atPlot(bestCity->plot()))
		{
			int iPathTurns = 0;
			if (generateSafePathforVulnerable(bestCity->plot(), &iPathTurns))
			{
				const CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_HEAL_SUPPORT, bestCity->plot());
			}
			m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
			return true;
		}
		return m_unit->getGroup()->pushMissionInternal(MISSION_SKIP, bestCity->getX(), bestCity->getY(), 0, false, false, MISSIONAI_HEAL_SUPPORT, bestCity->plot());
	}

	return false;
}

namespace {
	struct PropertyAmount
	{
		PropertyAmount(PropertyTypes prop = NO_PROPERTY, int score = 0) : prop(prop), score(score) { }

		PropertyTypes prop;
		int score;
	};

	int scorePropertyControlNeed(const std::vector<PropertyAmount>& propertyScores, const CvUnit* unit, const CvCity* city)
	{
		const CvPlayer& player = GET_PLAYER(unit->getOwner());
		const int C2C_MIN_PROP_CONTROL = GC.getC2C_MIN_PROP_CONTROL();

		int maxScore = 0;

		// loop through property types and get the difference between the target the AI wants the city to be at vs where it currently is
		foreach_(const PropertyAmount & propertyAmount, propertyScores)
		{
			//int iCurrentValue = pLoopCity->getPropertiesConst()->getValueByProperty(eProperty);
			int iValue = city->getPropertyNeed(propertyAmount.prop);
			if (iValue < 0 && !unit->atPlot(city->plot()))
			{
				continue;
			}

			int minRequired = C2C_MIN_PROP_CONTROL;
			// prop 0 == crime
			// TODO: better way to drive m_unit behavior that doesn't hard code property index.
			// First question is why m_unit extra calculation is needed at all? The property value and delta should always be enough to work out what is needed, by definition.
			if (propertyAmount.prop == 0)
			{
				minRequired += city->plot()->getNumCriminals() * 2; //local criminals demand more attention even if crime is under full control
			}

			// This gets ALL property control missions, not just for m_unit property...
			// TODO: filter by property somehow...
			int iResponders = player.AI_plotTargetMissionAIs(city->plot(), MISSIONAI_PROPERTY_CONTROL_RESPONSE, NULL, 0);
			int iExisting = player.AI_plotTargetMissionAIs(city->plot(), MISSIONAI_PROPERTY_CONTROL_MAINTAIN, NULL, 0);

			if (iResponders > 0)
			{
				iValue /= iResponders;
			}

			// generate path seems horribly bugged if an enemy exists inside the city. Cannot assume a false to that means they can't move in!
			if (unit->atPlot(city->plot()))
			{
				if (iExisting <= minRequired)
				{
					iValue += 1000;
				}
				iValue = std::max(100, iValue); //Ensure that SOME value is established to staying put.
			}

			maxScore = std::max(maxScore, iValue);
		}

		return scoring::applyDistanceScoringFactor(maxScore, unit->plot(), city->plot(), 2);
	}
};

bool CvUnitDefaultAI::fulfillPropertyControlNeed()
{
	const CvPropertyManipulators* propertyManipulators = GC.getUnitInfo(m_unit->getUnitType()).getPropertyManipulators();
	// If it doesn't change properties then it can't fulfill control needs
	if (propertyManipulators == nullptr)
	{
		return false;
	}

	std::vector<PropertyAmount> propertyScores;
	propertyScores.reserve(GC.getNumPropertyInfos());

	// loop through property types and get the difference between the target the AI wants the city to be at vs where it currently is
	for (int propIdx = 0; propIdx < GC.getNumPropertyInfos(); propIdx++)
	{
		const PropertyTypes eProperty = static_cast<PropertyTypes>(propIdx);

		int score = 0;
		//TBNote: in figuring out how much the unit provides, it's not as important as determining that it simply does provide.  Therefore, promotions and unitcombat values on the unit aren't checked (unless m_unit already by nature does but I think it's only asking for base unit info)
		foreach_(const CvPropertySource * pSource, propertyManipulators->getSources())
		{
			if (pSource->getType() == PROPERTYSOURCE_CONSTANT && pSource->getObjectType() == GAMEOBJECT_CITY && pSource->getProperty() == eProperty)
			{
				score += static_cast<const CvPropertySourceConstant*>(pSource)->getAmountPerTurn(m_unit->getGameObject());
			}
		}

		score *= GC.getPropertyInfo(eProperty).getAIWeight() / 50;
		if (score >= 1)
		{
			propertyScores.push_back(PropertyAmount(eProperty, score));
		}
	}

	const CvPlayer& player = GET_PLAYER(m_unit->getOwner());

	using namespace scoring;
	ScoreResult<CvCity> bestCityScore = findBestScore<CvCity, GreatestScore>(
		player.beginCities(), player.endCities(),
		bind(scorePropertyControlNeed, propertyScores, m_unit, _1),
		bind(canSafePathToCity, m_unit, _1)
	);

	if (bestCityScore.found)
	{
		CvCity* bestCity = bestCityScore.result.item;
		if (!m_unit->atPlot(bestCity->plot()))
		{
			int iPathTurns = 0;
			if (generateSafePathforVulnerable(bestCity->plot(), &iPathTurns))
			{
				const CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_PROPERTY_CONTROL_RESPONSE, bestCity->plot());
			}
			m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT);
			return true;
		}
		return m_unit->getGroup()->pushMissionInternal(MISSION_SKIP, bestCity->getX(), bestCity->getY(), 0, false, false, MISSIONAI_PROPERTY_CONTROL_MAINTAIN, bestCity->plot());
	}
	return false;
}

bool CvUnitDefaultAI::arrest()
{
	const int iMoves = m_unit->getMoves();
	if (m_unit->canArrest())
	{
		m_unit->doArrest();
	}
	if (m_unit->getMoves() > iMoves)
	{
		return true;
	}
	return false;
}

bool CvUnitDefaultAI::ambush(int iOddsThreshold, bool bAssassinationOnly)
{
	int iValue = 0;
	int iBestValue = 0;

	const CvPlot* pPlot = m_unit->getGroup()->plot();
	if (pPlot == NULL)
	{
		return false;
	}
	int iVisDef = pPlot->getNumVisiblePotentialEnemyDefenders(m_unit);//getting 1 here which is to say there is a target but the target doesn't validate later.
	int iVisDefless = pPlot->getNumVisiblePotentialEnemyDefenderless(m_unit);

	if (pPlot->isCity(true) && !m_unit->isAssassin())
	{
		return false;
	}

	bool bAssassin = m_unit->isAssassin();
	bool bCanAssassinate = m_unit->canAmbush(pPlot, bAssassin);
	if (bAssassinationOnly && !bCanAssassinate)
	{
		return false;
	}

	if (iVisDef > 0 || iVisDefless > 0)
	{
		bool bWinLikely = false;
		int iAdjustedOddsThreshold = 0;

		PROFILE("CvUnitDefaultAI::anyAttack.FoundTargetforAmbush");
		if (iVisDef > 0)
		{
			iValue = m_unit->getGroup()->AI_attackOdds(pPlot, true, true, &bWinLikely, iOddsThreshold);
			iAdjustedOddsThreshold = finalOddsThreshold(pPlot, iOddsThreshold);
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

		//	Increase value on our territory since we really want to get rid of enemies there even
		//	if it costs us a few losses
		if (pPlot->getOwner() == m_unit->getOwner() && bWinLikely)
		{
			iValue *= 2;
		}

		if (iValue > iBestValue && iValue >= iAdjustedOddsThreshold)
		{
			iBestValue = iValue;
		}
	}

	if (iBestValue > 0)
	{
		if (bCanAssassinate)
		{
			return m_unit->getGroup()->pushMissionInternal(MISSION_ASSASSINATE, pPlot->getX(), pPlot->getY());
		}
		return m_unit->getGroup()->pushMissionInternal(MISSION_AMBUSH, pPlot->getX(), pPlot->getY());
	}
	return false;
}

bool CvUnitDefaultAI::activateStatus(bool bStack, PromotionTypes eStatus, CvUnit* pUnit)
{
	if (eStatus == NO_PROMOTION)
	{
		return false;
	}
	int iStatus = (int)eStatus;
	//function to turn on or off status on whole group or just unit
	if (bStack)
	{
		if (m_unit->getGroup()->canDoCommand(COMMAND_STATUS, iStatus, 0, false, false, true))
		{
			m_unit->getGroup()->doCommand(COMMAND_STATUS, iStatus, 0);
			return true;
		}
	}
	else
	{
		if (pUnit == NULL)
		{
			return false;
		}
		if (pUnit->canDoCommand(COMMAND_STATUS, iStatus, 0))
		{
			pUnit->doCommand(COMMAND_STATUS, iStatus, 0);
			return true;
		}
	}
	return false;
}

bool CvUnitDefaultAI::selectStatus(bool bStack, CvUnit* pUnit)
{
	if (bStack)
	{
		if (m_unit->getGroup() == NULL)
		{
			return false;
		}
	}
	else if (pUnit == NULL)
	{
		return false;
	}
	int iBestValue = 0;
	int iWorstValue = 0;
	PromotionTypes eBestStatus = NO_PROMOTION;
	PromotionTypes eWorstStatus = NO_PROMOTION;
	const MissionAITypes eMissionAI = m_unit->getGroup() ? m_unit->getGroup()->AI_getMissionAIType() : NO_MISSIONAI;
	const UnitAITypes eUnitAI = bStack ? getUnitAIType() : pUnit->AI()->getUnitAIType();

	const PromotionLineTypes promotionLineStandout = GC.getPROMOTIONLINE_STANDOUT();

	const int iNumStatusPromotions = GC.getNumStatusPromotions();

	for (int iI = 0; iI < iNumStatusPromotions; iI++)
	{
		int iValue = 0;
		const int iStatus = GC.getStatusPromotion(iI);
		const PromotionTypes eStatus = (PromotionTypes)iStatus;
		const CvPromotionInfo& kPromotion = GC.getPromotionInfo(eStatus);
		PromotionTypes eRemoveStatus = NO_PROMOTION;

		for (int iJ = 0; iJ < iNumStatusPromotions; iJ++)
		{
			if (kPromotion.getPromotionLine() == GC.getPromotionInfo((PromotionTypes)GC.getStatusPromotion(iJ)).getPromotionLine()
			&& GC.getPromotionInfo((PromotionTypes)GC.getStatusPromotion(iJ)).getLinePriority() == 1)
			{
				eRemoveStatus = (PromotionTypes)GC.getStatusPromotion(iJ);
				break;
			}
		}
		if (kPromotion.getLinePriority() != 1)
		{
			if (bStack)
			{
				if (m_unit->getGroup()->canDoCommand(COMMAND_STATUS, iStatus, 0, false, false, true) || m_unit->isHasPromotion(eStatus))
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
					if (m_unit->hasInvisibleAbility())
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
								//if m_unit Promotion isn't on the Standout Promotionline NegatesInvisibility is a bad thing
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
						else if (m_unit->plot() != NULL && m_unit->plot()->getOwner() == m_unit->getOwner())
						{
							iTemp *= 5;
						}
						iValue += iTemp;
					}
					//combat%
					iTemp = kPromotion.getCombatPercent();
					if (m_unit->plot() != NULL && iTemp != 0)
					{
						int iRange = m_unit->visibilityRange(m_unit->plot()) + m_unit->getMoves();
						if (GET_PLAYER(m_unit->getOwner()).AI_getPlotDanger(m_unit->plot(), iRange) > 0)
						{
							iTemp *= 2;
						}
						if (m_unit->plot()->getOwner() != m_unit->getOwner())
						{
							iTemp *= 4;
						}
						iValue += iTemp;
					}
					//selfheal
					iTemp = kPromotion.getSelfHealModifier();
					iTemp *= m_unit->getGroup()->getWorstDamagePercent();
					iTemp /= 10;
					iValue += iTemp;

				}
				// If any can remove
				if (iValue < 0 && !m_unit->getGroup()->canDoCommand(COMMAND_STATUS, (int)eRemoveStatus, 0, false, false, false))
				{
					iValue = 0;
				}
			}
			else
			{
				if (pUnit->canDoCommand(COMMAND_STATUS, iStatus, 0) || pUnit->isHasPromotion(eStatus))
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
								//if m_unit Promotion isn't on the Standout Promotionline NegatesInvisibility is a bad thing
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
						int iRange = pUnit->visibilityRange(m_unit->plot()) + pUnit->getMoves();
						/*if (GET_PLAYER(m_unit->getOwner()).AI_getVisiblePlotDanger(m_unit->plot(), iRange, false, m_unit->getGroup(), 70))*/
						if (GET_PLAYER(m_unit->getOwner()).AI_getPlotDanger(m_unit->plot(), iRange) > 0)
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
					iTemp *= m_unit->getDamage();
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
	const int iCompare = iBestValue - iWorstValue;

	if (iCompare < 0 && eWorstStatus != NO_PROMOTION)
	{
		return activateStatus(bStack, eWorstStatus, pUnit) && !GC.getPromotionInfo(eWorstStatus).isQuick();
		//setWorstStatus
	}

	if (iCompare > 0 && eBestStatus != NO_PROMOTION && !m_unit->isHasPromotion(eBestStatus))
	{
		return activateStatus(bStack, eBestStatus, pUnit) && !GC.getPromotionInfo(eBestStatus).isQuick();
		//setBestStatus
	}
	return false;
}

bool CvUnitDefaultAI::groupSelectStatus()
{
	foreach_(CvUnit * pLoopUnit, m_unit->getGroup()->units())
	{
		if (selectStatus(false, pLoopUnit))
		{
			return true;
		}
	}
	return false;
}

bool CvUnitDefaultAI::InvestigatorFulfillment()
{
	const int iMinimumDedicated = 1;
	const CvPlayerAI& player = GET_PLAYER(m_unit->getOwner());

	bool bDedicate = false;

	int iBestValue = 0;
	const CvPlot* pBestPlot = NULL;
	//CvCity* pBestCity = NULL;

	foreach_(const CvCity * pLoopCity, player.cities())
	{
		const CvPlot* pLoopPlot = pLoopCity->plot();
		bool bAtPlot = m_unit->atPlot(pLoopPlot);
		int iDedicated = player.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_INVESTIGATOR_MAINTAIN, NULL, 0);
		int iDedicatedNeeded = iMinimumDedicated - iDedicated;
		if (!bDedicate && bAtPlot && iDedicatedNeeded > 0)
		{
			bDedicate = true;
			break;
		}
		int iNumCriminals = pLoopCity->plot()->getNumCriminals();
		int iResponders = player.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_INVESTIGATOR_RESPONSE, NULL, 0);
		iNumCriminals -= iResponders; //At m_unit point iNumCriminals is more of a subjective need base
		int iValue = iNumCriminals * iNumCriminals * 100; //Each criminal makes things exponentially more valuable.  The multiply by 100 step ensures some granularity when then dividing by reduction factors in the next step.

		if (iValue <= 0)
			continue;

		if (!bAtPlot)
		{
			int iPathTurns = 0;
			if (generateSafePathforVulnerable(pLoopPlot, &iPathTurns))
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
		else
		{
			iValue *= 2;
		}
		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			pBestPlot = pLoopPlot;
			//pBestCity = pLoopCity;
		}
	}

	if (bDedicate)
	{
		return m_unit->getGroup()->pushMissionInternal(MISSION_SKIP, m_unit->getX(), m_unit->getY(), 0, false, false, MISSIONAI_INVESTIGATOR_MAINTAIN, m_unit->plot());
	}
	if (pBestPlot != NULL)
	{
		if (!m_unit->atPlot(pBestPlot))
		{
			int iPathTurns = 0;
			if (generateSafePathforVulnerable(pBestPlot, &iPathTurns))
			{
				const CvPlot* endTurnPlot = m_unit->getPathEndTurnPlot();
				return m_unit->getGroup()->pushMissionInternal(MISSION_MOVE_TO, endTurnPlot->getX(), endTurnPlot->getY(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_INVESTIGATOR_RESPONSE, pBestPlot);
			}
		}
		else
		{
			return m_unit->getGroup()->pushMissionInternal(MISSION_SKIP, pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_INVESTIGATOR_MAINTAIN, pBestPlot);
		}
	}

	return false;
}

//in progress
bool CvUnitDefaultAI::establishStackSeeInvisibleCoverage()
{
	if (m_unit->isHuman())
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

			if (m_unit->getDomainType() == DOMAIN_LAND)
			{
				if (m_unit->getGroup()->countSeeInvisibleActive(UNITAI_SEE_INVISIBLE, eVisible) < 1)
				{
					iUnitValue = GET_PLAYER(m_unit->getOwner()).AI_bestAreaUnitAIValue(UNITAI_SEE_INVISIBLE, m_unit->area(), &eBestUnit, &criteria);
					if (eBestUnit != NO_UNIT)
					{
						GET_PLAYER(m_unit->getOwner()).getContractBroker().advertiseWork(HIGHEST_PRIORITY_ESCORT_PRIORITY,
							NO_UNITCAPABILITIES,
							m_unit->getX(),
							m_unit->getY(),
							m_unit,
							UNITAI_SEE_INVISIBLE,
							-1,
							&criteria);

						m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
						m_contractualState = CONTRACTUAL_STATE_AWAITING_ANSWER;
						setToWaitOnUnitAI(UNITAI_SEE_INVISIBLE, true);

						if (gUnitLogLevel > 2)
						{
							logBBAI("	%S (%d) at (%d,%d) [stack size %d] requests See Invisible Unit at priority %d", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getID(), m_unit->getX(), m_unit->getY(), m_unit->getGroup()->getNumUnits(), HIGH_PRIORITY_ESCORT_PRIORITY);
						}

						m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_SEE_INVISIBLE);
						return true;
					}
				}
			}
			else if (m_unit->getDomainType() == DOMAIN_SEA)
			{
				if (m_unit->getGroup()->countSeeInvisibleActive(UNITAI_SEE_INVISIBLE_SEA, eVisible) < 1)
				{
					iUnitValue = GET_PLAYER(m_unit->getOwner()).AI_bestAreaUnitAIValue(UNITAI_SEE_INVISIBLE_SEA, m_unit->area(), &eBestUnit, &criteria);
					if (eBestUnit != NO_UNIT)
					{
						GET_PLAYER(m_unit->getOwner()).getContractBroker().advertiseWork(HIGHEST_PRIORITY_ESCORT_PRIORITY,
							NO_UNITCAPABILITIES,
							m_unit->getX(),
							m_unit->getY(),
							m_unit,
							UNITAI_SEE_INVISIBLE_SEA,
							-1,
							&criteria);

						m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
						m_contractualState = CONTRACTUAL_STATE_AWAITING_ANSWER;
						setToWaitOnUnitAI(UNITAI_SEE_INVISIBLE_SEA, true);

						if (gUnitLogLevel > 2)
						{
							logBBAI("	%S (%d) at (%d,%d) [stack size %d] requests See Invisible Sea Unit at priority %d", GET_PLAYER(m_unit->getOwner()).getCivilizationDescription(0), m_unit->getID(), m_unit->getX(), m_unit->getY(), m_unit->getGroup()->getNumUnits(), HIGH_PRIORITY_ESCORT_PRIORITY);
						}

						m_unit->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_SEE_INVISIBLE);
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool CvUnitDefaultAI::generateSafePathforVulnerable(const CvPlot* pToPlot, int* piPathTurns) const
{
	return m_unit->generatePath(pToPlot, MOVE_IGNORE_DANGER, true, piPathTurns);
}

void CvUnitDefaultAI::setToWaitOnUnitAI(UnitAITypes eUnitAI, bool bAdd)
{
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

bool CvUnitDefaultAI::isWaitingOnUnitAI(int iIndex) const
{
	return !m_unit->isHuman() && algo::any_of_equal(m_aiWaitingOnUnitAITypes, iIndex);
}

bool CvUnitDefaultAI::isWaitingOnUnitAIAny() const
{
	return m_bWaitingOnUnitAIAny;
}

void CvUnitDefaultAI::setWaitingOnUnitAIAny()
{
	m_bWaitingOnUnitAIAny = false;
	if (m_unit->isHuman())
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

bool CvUnitDefaultAI::isNegativePropertyUnit() const
{
	const CvPropertyManipulators* propertyManipulators = GC.getUnitInfo(m_unit->getUnitType()).getPropertyManipulators();
	if (propertyManipulators != NULL)
	{
		foreach_(const CvPropertySource * pSource, propertyManipulators->getSources())
		{
			//	We have a source for a property - value is crudely just the AIweight of that property times the source size (which is expected to only depend on the player)
			if (pSource->getType() == PROPERTYSOURCE_CONSTANT)
			{
				if (GC.getPropertyInfo(pSource->getProperty()).getAIWeight() < 0)
				{
					if (static_cast<const CvPropertySourceConstant*>(pSource)->getAmountPerTurn(m_unit->getGameObject()) > 0)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

int CvUnitDefaultAI::getMyAggression(int iAttackProb) const
{
	int iAggression = m_unit->getUnitInfo().getAggression() + m_unit->getLevel() - 1;
	iAggression *= m_unit->getMaxHP() - m_unit->getDamage();
	iAggression /= 100;
	iAggression *= iAttackProb;
	return iAggression / 100;
}