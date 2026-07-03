#include "CvGameCoreDLL.h"
#include "CvHunterAI.h"

#include "FProfiler.h"

#include "BetterBTSAI.h"
#include "CvContractBroker.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvPlayerAI.h"
#include "CvPlot.h"
#include "CvSelectionGroup.h"
#include "CvSelectionGroupAI.h"
#include "CvUnitAI.h"

// ---------------------------------------------------------------------------
// Lifecycle & claim ledger.
// ---------------------------------------------------------------------------
CvHunterAI::CvHunterAI(PlayerTypes owner)
	: m_owner(owner)
	, m_lastTurn(-1)
{
}

void CvHunterAI::onTurnBegin(int gameTurn)
{
	m_lastTurn = gameTurn;
	m_claims.clear();
}

bool CvHunterAI::tryClaim(int plotIdx, int unitId)
{
	std::map<int, int>::const_iterator it = m_claims.find(plotIdx);
	if (it != m_claims.end() && it->second != unitId)
	{
		return false;
	}
	m_claims[plotIdx] = unitId;
	return true;
}

bool CvHunterAI::isClaimedByOther(int plotIdx, int unitId) const
{
	std::map<int, int>::const_iterator it = m_claims.find(plotIdx);
	return it != m_claims.end() && it->second != unitId;
}

void CvHunterAI::releaseAllClaimsBy(int unitId)
{
	for (std::map<int, int>::iterator it = m_claims.begin(); it != m_claims.end(); )
	{
		if (it->second == unitId)
		{
			m_claims.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

// ---------------------------------------------------------------------------
// hunterMove -- UNITAI_HUNTER units and great commanders escorting a hunter
// (bWithCommander). Moved verbatim from the legacy CvUnitAI::AI_SearchAndDestroyMove,
// with [HAI/*] logging and the favorable-adjacent-kill pass added.
// ---------------------------------------------------------------------------
bool CvHunterAI::hunterMove(CvUnitAI* unit, bool bWithCommander)
{
	PROFILE_FUNC();

	CvPlayerAI& player = GET_PLAYER(unit->getOwner());

	logHunterAI(1, "[HAI/begin] hunterMove owner=%d unit=%d aitype=%d automate=%d withCmd=%d at=(%d,%d) stack=%d",
		unit->getOwner(), unit->getID(), unit->AI_getUnitAIType(), unit->getGroup()->getAutomateType(), (int)bWithCommander, unit->getX(), unit->getY(), unit->getGroup()->getNumUnits());

	if (unit->AI_groupSelectStatus())
	{
		return false;
	}

	MissionAITypes eMissionAIType = MISSIONAI_GROUP;

	if (!unit->isHuman() && unit->plot()->getOwner() == unit->getOwner() && player.AI_unitTargetMissionAIs(unit, &eMissionAIType, 1, unit->getGroup(), 2) > 0)  //Calvitix, set to 2 turns, to facilitate escort to joint its hunter
	{
		// Wait for units which are joining our group this turn
		unit->getGroup()->pushMission(MISSION_SKIP);
		return true;
	}

	//	If we have any animal hangers-on and are in our territory drop them off
	if (unit->getGroup()->countNumUnitAIType(UNITAI_SUBDUED_ANIMAL) > 0)
	{
		if (unit->plot()->getOwner() == unit->getOwner())
		{
			unit->getGroup()->AI_separateAI(UNITAI_SUBDUED_ANIMAL);
		}
		else if (unit->getGroup()->countNumUnitAIType(UNITAI_HUNTER_ESCORT) > 0)
		{
			unit->getGroup()->AI_separateAI(UNITAI_HUNTER);
			if (unit->getGroup()->countNumUnitAIType(UNITAI_HUNTER) == 0)
			{
				return false; // The hunter wasn't the stack leader?
			}
		}
	}


	if (unit->getGroup()->getWorstDamagePercent() > 0)
	{
		OutputDebugString(CvString::format("%S (%d) damaged at (%d,%d)...\n", unit->getDescription().c_str(), unit->getID(), unit->getX(), unit->getY()).c_str());
		//	If there is an adjacent enemy seek safety before we heal
		if (unit->exposedToDanger(unit->plot(), 75))
		{
			OutputDebugString("	...plot is dangerous - seeking safety\n");
			if (unit->AI_safety())
			{
				logHunterAI(2, "[HAI/heal] unit=%d action=safety", unit->getID());
				return true;
			}
		}

		OutputDebugString("	...Try to heal\n");
		if (unit->AI_heal())
		{
			OutputDebugString(CvString::format("	...healing at (%d,%d)\n", unit->getX(), unit->getY()).c_str());
			logHunterAI(2, "[HAI/heal] unit=%d action=heal", unit->getID());
			return true;
		}

		if (unit->getGroup()->getWorstDamagePercent() > 25)
		{
			//	Look for somewhere safer
			if (unit->AI_safety(3))
			{
				logHunterAI(2, "[HAI/heal] unit=%d action=safety3", unit->getID());
				return true;
			}
		}
	}

	//	If we have more than 3 animal hangers-on escort them back to our territory
	if (unit->getGroup()->countNumUnitAIType(UNITAI_WORKER) + unit->getGroup()->countNumUnitAIType(UNITAI_SUBDUED_ANIMAL) > 3 && unit->plot()->getOwner() != unit->getOwner())
	{
		if (unit->AI_retreatToCity())
		{
			return true;
		}
	}
	const bool bLookForWork = !bWithCommander && (GC.getGame().getGameTurn() % 2 == 0);

	if (bLookForWork && !unit->isHuman() && unit->getGroup()->getNumUnits() == 1)
	{
		//If is able to merge right now with an escort
		if (unit->AI_groupMergeRange(UNITAI_HUNTER_ESCORT, 1, false, true, true))
		{
			//return;
			logHunterAI(2, "[HAI/escort] unit=%d merge with hunter escort", unit->getID());

			//if Hunter if not the group Head, trick to obtain it
			if (unit->getGroup()->getNumUnits() == 2)
			{
				if (unit != unit->getGroup()->getHeadUnit())
				{
					CvSelectionGroup* myGroup = unit->getGroup();
					unit->joinGroup(NULL);
					unit->joinGroup(myGroup);

					FAssert(unit == unit->getGroup()->getHeadUnit())
				}
			}

		}
		else
		{
			// If anyone is actively asking for a hunter that takes priority
			if (unit->processContracts(HIGHEST_PRIORITY_ESCORT_PRIORITY))
			{
				if (gUnitLogLevel >= 3 && unit->m_contractualState == CONTRACTUAL_STATE_FOUND_WORK)
				{
					logContractBroker(1, "	Hunter for player %d (%S) at (%d,%d) found contractual work",
						unit->getOwner(),
						player.getCivilizationDescription(0),
						unit->getX(),
						unit->getY());
				}
				return true;
			}
			if (gUnitLogLevel >= 3)
			{
				logContractBroker(1, "	Hunter for player %d (%S) at (%d,%d) found no urgent contractual work",
					unit->getOwner(),
					player.getCivilizationDescription(0),
					unit->getX(),
					unit->getY());
			}
		}
	}

	//Apparently minimum odds is a maximum odds threshold rather than minimum
	const int iMinimumOdds = unit->isHuman() ? player.getModderOption(MODDEROPTION_AUTO_HUNT_MIN_COMBAT_ODDS) : (bWithCommander ? 90 : 70);

	// Take an obvious adjacent kill on raw odds before the inflated final-odds
	// threshold (or the ranged passes) can let us walk past it.
	if (unit->AI_huntRange(1, iMinimumOdds, false, 0, /*bRawOdds*/true))
	{
		logHunterAI(1, "[HAI/engage] unit=%d adjacent kill", unit->getID());
		return true;
	}

	if (unit->AI_huntRange(1, iMinimumOdds, false))
	{
		return true;
	}

	// Toffer - Non-optimal hunter is temporary, phase them out when appropriate.
	if (!bWithCommander && !unit->isHuman() && unit->getUnitInfo().getDefaultUnitAIType() != UNITAI_HUNTER)
	{
		const int iOwnedHunters = player.AI_totalAreaUnitAIs(unit->area(), UNITAI_HUNTER);
		if (iOwnedHunters > 5)
		{
			logHunterAI(2, "[HAI/scrap] unit=%d revert AI (owned=%d)", unit->getID(), iOwnedHunters);
			unit->AI_setUnitAIType(unit->getUnitInfo().getDefaultUnitAIType());
			return true;
		}
		if (iOwnedHunters > 1)
		{
			const int iNeededHunters = player.AI_neededHunters(unit->area());
			const int iHunterDeficitPercent = (iNeededHunters <= iOwnedHunters) ? 0 : (iNeededHunters - iOwnedHunters) * 100 / iNeededHunters;

			if (iHunterDeficitPercent <= 80)
			{
				logHunterAI(2, "[HAI/scrap] unit=%d revert AI (deficit=%d%%)", unit->getID(), iHunterDeficitPercent);
				unit->AI_setUnitAIType(unit->getUnitInfo().getDefaultUnitAIType());
				return true;
			}
		}
	}

	if (!bWithCommander && (!unit->isHuman() || player.isModderOption(MODDEROPTION_AUTO_HUNT_RETURN_FOR_UPGRADES)))
	{
		if (unit->AI_travelToUpgradeCity())
		{
			return true;
		}
	}

	{
		// Get the proper accompaniment
		const bool bContractEscort = (
			!bLookForWork && !unit->isHuman() && !unit->isCargo()
			&& unit->getGroup()->countNumUnitAIType(UNITAI_HUNTER_ESCORT) < 1
			&& player.getBestUnitType(UNITAI_HUNTER_ESCORT) != NO_UNIT
		);
		if (bContractEscort)
		{
			player.getContractBroker().advertiseWork
			(
				HIGHEST_PRIORITY_ESCORT_PRIORITY,
				NO_UNITCAPABILITIES,
				unit->getX(), unit->getY(),
				unit, UNITAI_HUNTER_ESCORT
			);

			logHunterAI(2, "[HAI/escort] unit=%d advertise hunter-escort work", unit->getID());
			// Limited operations gravitating close to borders while waiting.
			if (unit->exposedToDanger(unit->plot(), 90))
			{
				if (unit->AI_safety())
				{
					return true;
				}
			}
			else if (unit->plot()->getOwner() == unit->getOwner())
			{
				if (unit->AI_huntRange(4, iMinimumOdds, false))
				{
					return true;
				}
				if (unit->AI_moveToBorders())
				{
					return true;
				}
			}
			else
			{
				if (unit->AI_huntRange(1, iMinimumOdds, false))
				{
					return true;
				}
				if (GC.getGame().getSorenRandNum(10, "keep close to home") < 4)
				{
					if (unit->AI_reachHome(false, 6))
					{
						return true;
					}
				}
			}
			if (unit->AI_explore())
			{
				return true;
			}
			unit->getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
	}

	//	If we have animal hangers-on escort them back to our territory if it is not too far
	if ((unit->getGroup()->countNumUnitAIType(UNITAI_SUBDUED_ANIMAL) > 0 || unit->getGroup()->countNumUnitAIType(UNITAI_WORKER) > 0) && unit->plot()->getOwner() != unit->getOwner())
	{
		if (unit->AI_reachHome(false, 4))
		{
			return true;
		}
	}

	if (unit->AI_huntRange(1, iMinimumOdds, false))
	{
		return true;
	}

	if (unit->AI_goody(4))
	{
		return true;
	}

	if (unit->AI_huntRange(5, iMinimumOdds, false))
	{
		return true;
	}

	// Toffer - Should change this to scrap the lowest level hunters first, perhaps in the player objects doTurn() routine.
	if (!bWithCommander && !unit->isHuman()
	&& player.AI_isFinancialTrouble()
	&& player.getUnitUpkeepNet(unit->isMilitaryBranch(), unit->getUpkeep100()) > 0)
	{
		const int iNeededHunters = player.AI_neededHunters(unit->area());
		const int iHasHunters = player.AI_totalAreaUnitAIs(unit->area(), UNITAI_HUNTER);

		if (iHasHunters > iNeededHunters)
		{
			logHunterAI(2, "[HAI/scrap] unit=%d scrap (has=%d needed=%d financial)", unit->getID(), iHasHunters, iNeededHunters);
			unit->scrap();
			return true;
		}
	}

	if (unit->AI_refreshExploreRange(3, true))
	{
		logHunterAI(2, "[HAI/spread] unit=%d action=refreshExplore", unit->getID());
		return true;
	}

	if (unit->AI_moveToBorders())
	{
		logHunterAI(2, "[HAI/spread] unit=%d action=borders", unit->getID());
		return true;
	}

	if (unit->AI_patrol())
	{
		logHunterAI(2, "[HAI/spread] unit=%d action=patrol", unit->getID());
		return true;
	}

	if (unit->AI_retreatToCity())
	{
		return true;
	}

	if (unit->AI_safety())
	{
		return true;
	}

	unit->getGroup()->pushMission(MISSION_SKIP);
	logHunterAI(1, "[HAI/end] hunterMove unit=%d result=skip", unit->getID());
	return true;
}

// ---------------------------------------------------------------------------
// autoHuntMove -- generic combat units a player toggled to AUTOMATE_HUNT. Lean,
// aggressive enemy-seeking; none of the hunter-only bookkeeping, no animal-only
// target restriction (AI_huntRange's wildlife filter only triggers for
// UNITAI_HUNTER), and a lower odds bar so attackers actually engage.
// ---------------------------------------------------------------------------
bool CvHunterAI::autoHuntMove(CvUnitAI* unit)
{
	PROFILE_FUNC();

	CvPlayerAI& player = GET_PLAYER(unit->getOwner());

	logHunterAI(1, "[HAI/begin] autoHuntMove owner=%d unit=%d aitype=%d automate=%d at=(%d,%d) stack=%d",
		unit->getOwner(), unit->getID(), unit->AI_getUnitAIType(), unit->getGroup()->getAutomateType(), unit->getX(), unit->getY(), unit->getGroup()->getNumUnits());

	if (unit->AI_groupSelectStatus())
	{
		return false;
	}

	// Heal / retreat from danger first.
	if (unit->getGroup()->getWorstDamagePercent() > 0)
	{
		if (unit->exposedToDanger(unit->plot(), 75) && unit->AI_safety())
		{
			logHunterAI(2, "[HAI/heal] unit=%d action=safety", unit->getID());
			return true;
		}
		if (unit->AI_heal())
		{
			logHunterAI(2, "[HAI/heal] unit=%d action=heal", unit->getID());
			return true;
		}
		if (unit->getGroup()->getWorstDamagePercent() > 25 && unit->AI_safety(3))
		{
			logHunterAI(2, "[HAI/heal] unit=%d action=safety3", unit->getID());
			return true;
		}
	}

	const int iMinimumOdds = unit->isHuman()
		? player.getModderOption(MODDEROPTION_AUTO_HUNT_MIN_COMBAT_ODDS)
		: 55;

	// Favorable adjacent kill first -- raw odds, bypassing the inflated final-odds
	// threshold so we never walk past a target we can clearly beat.
	if (unit->AI_huntRange(1, iMinimumOdds, false, 0, /*bRawOdds*/true))
	{
		logHunterAI(1, "[HAI/engage] unit=%d adjacent kill", unit->getID());
		return true;
	}

	// Expanding enemy search (normal odds gate).
	if (unit->AI_huntRange(1, iMinimumOdds, false)
	||  unit->AI_huntRange(3, iMinimumOdds, false)
	||  unit->AI_huntRange(6, iMinimumOdds + 5, false))
	{
		return true;
	}

	if (unit->AI_goody(4))
	{
		return true;
	}

	// Spread out instead of trailing one another.
	if (unit->AI_moveToBorders())
	{
		logHunterAI(2, "[HAI/spread] unit=%d action=borders", unit->getID());
		return true;
	}

	if (unit->AI_patrol())
	{
		logHunterAI(2, "[HAI/spread] unit=%d action=patrol", unit->getID());
		return true;
	}

	if (unit->AI_retreatToCity())
	{
		return true;
	}

	if (unit->AI_safety())
	{
		return true;
	}

	unit->getGroup()->pushMission(MISSION_SKIP);
	logHunterAI(1, "[HAI/end] autoHuntMove unit=%d result=skip", unit->getID());
	return true;
}
