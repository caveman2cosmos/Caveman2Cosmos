// selectionGroupAI.cpp


#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvCity.h"
#include "CvGlobals.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "CvPlot.h"
#include "CvSelectionGroupAI.h"
#include "CvTeamAI.h"
#include "CvUnit.h"
#include "CvImprovementInfo.h"

// Public Functions...

CvSelectionGroupAI::CvSelectionGroupAI() { AI_reset(); }
CvSelectionGroupAI::~CvSelectionGroupAI() { }


void CvSelectionGroupAI::AI_reset()
{
	m_iMissionAIX = INVALID_PLOT_COORD;
	m_iMissionAIY = INVALID_PLOT_COORD;

	m_bForceSeparate = false;

	m_eMissionAIType = NO_MISSIONAI;

	m_missionAIUnit.reset();

	m_bGroupAttack = false;
	m_iGroupAttackX = -1;
	m_iGroupAttackY = -1;
}

namespace {
	bool matchAll(const CvUnit*)
	{
		return true;
	}
	bool matchNonAI(const CvUnit* unit, const UnitAITypes unitAI)
	{
		return unit->AI_getUnitAIType() != unitAI;
	}
	bool matchAI(const CvUnit* unit, const UnitAITypes unitAI)
	{
		return unit->AI_getUnitAIType() == unitAI;
	}
	bool matchImpassable(const CvUnit* unit, const CvPlayerAI& player)
	{
		return player.AI_unitImpassableCount(unit->getUnitType()) > 0;
	}
	bool matchEmptyTransport(const CvUnit* unit)
	{
		return unit->AI_getUnitAIType() == UNITAI_ASSAULT_SEA && !unit->hasCargo();
	}

	void separateIf(CvSelectionGroup* group, bst::function<bool(const CvUnit*)> predicateFn)
	{
		foreach_(CvUnit* unit, group->units() | filtered(predicateFn))
		{
			unit->joinGroup(NULL);
			FAssertMsg(algo::none_of_equal(group->units(), unit), "Failed to remove unit from group");
			if (unit->plot()->getTeam() == group->getTeam())
			{
				unit->getGroup()->pushMission(MISSION_SKIP);
			}
		}
	}
}


void CvSelectionGroupAI::AI_separate()
{
	separateIf(this, matchAll);
}

void CvSelectionGroupAI::AI_separateNonAI(UnitAITypes eUnitAI)
{
	separateIf(this, bind(matchNonAI, _1, eUnitAI));
}

void CvSelectionGroupAI::AI_separateAI(UnitAITypes eUnitAI)
{
	separateIf(this, bind(matchAI, _1, eUnitAI));
}

void CvSelectionGroupAI::AI_separateImpassable()
{
	separateIf(this, bind(matchImpassable, _1, bst::ref(GET_PLAYER(getOwner()))));
}

void CvSelectionGroupAI::AI_separateEmptyTransports()
{
	separateIf(this, matchEmptyTransport);
}

// Returns true if the group has become busy...
bool CvSelectionGroupAI::AI_update()
{
	PROFILE("CvSelectionGroupAI::AI_update");

	FAssert(getOwner() != NO_PLAYER);

	if (!AI_isControlled() || getNumUnits() == 0)
	{
		return false;
	}

	if (!isHuman() && !getHeadUnit()->isCargo() && getActivityType() == ACTIVITY_SLEEP)
	{
		setForceUpdate(true);
	}

	if (isForceUpdate())
	{
		
		LOG_UNIT_BLOCK(4, {
			UnitAITypes eUnitAi = this->getHeadUnit()->AI_getUnitAIType();
			MissionAITypes eMissionAI = AI_getMissionAIType();
			CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
			CvWString MissionInfos = MissionAITypeToString(eMissionAI);
			CvWString StrUnitName = this->getHeadUnit()->getNameNoDesc();
			if (StrUnitName.length() == 0)
			{
				StrUnitName = this->getHeadUnit()->getName(0).GetCString();
			}

			logBBAI("Player %d Group ID %d, mené par %d %S of Type %S, at (%d, %d), Mission %S [stack size %d], Was fortified/BuildUp, Force to Awake...", getOwner(), m_iID, this->getHeadUnit()->getID(), StrUnitName.GetCString(), StrunitAIType.GetCString(), getX(), getY(), MissionInfos.GetCString(), getNumUnits());
		});
		clearMissionQueue(); // XXX ???
		setActivityType(ACTIVITY_AWAKE);
		setForceUpdate(false);

		// if we are in the middle of attacking with a stack, cancel it
		AI_cancelGroupAttack();
	}

	setGroupToCacheFor(this);

	FAssert(!GET_PLAYER(getOwner()).isAutoMoves() || isHuman());

	int iTempHack = 0; // XXX
	bool bDead = false;
	bool bFailedAlreadyFighting = false;

	while (m_bGroupAttack && !bFailedAlreadyFighting || readyToMove())
	{
		iTempHack++;
		if (iTempHack > 45 && iTempHack < 50)
		{
			CvUnit* pHeadUnit = getHeadUnit();
			if (NULL != pHeadUnit)
			{
				int iPass = iTempHack - 45;
				char szOut[1024];
				CvWString szTempString;
				getUnitAIString(szTempString, pHeadUnit->AI_getUnitAIType());
				LOG_PLAYER_BLOCK(1, {
					logAiEvaluations(1, "Unit stuck in loop( Warning before short circuit (pass: %d) ): %S(%S)[%d, %d] (%S)\n",
					iPass, pHeadUnit->getName().GetCString(), GET_PLAYER(pHeadUnit->getOwner()).getName(),
					pHeadUnit->getX(), pHeadUnit->getY(), szTempString.GetCString());
				});
				sprintf
				(
					szOut, "Unit stuck in loop( Warning before short circuit (pass: %d) ): %S(%S)[%d, %d] (%S)\n",
					iPass, pHeadUnit->getName().GetCString(), GET_PLAYER(pHeadUnit->getOwner()).getName(),
					pHeadUnit->getX(), pHeadUnit->getY(), szTempString.GetCString()
				);
				gDLL->messageControlLog(szOut);

				FErrorMsg(szOut);
			}
			else FErrorMsg("error");
		}
		else if (iTempHack >= 50)
		{
			CvUnit* pHeadUnit = getHeadUnit();
			if (NULL != pHeadUnit)
			{
				char szOut[1024];
				CvWString szTempString;
				getUnitAIString(szTempString, pHeadUnit->AI_getUnitAIType());
				LOG_PLAYER_BLOCK(1, {
					logAiEvaluations(1, "Unit stuck in loop: %S(%S)[%d, %d] (%S)\n",
					pHeadUnit->getName().GetCString(), GET_PLAYER(pHeadUnit->getOwner()).getName(),
					pHeadUnit->getX(), pHeadUnit->getY(), szTempString.GetCString());
				});
				sprintf
				(
					szOut, "Unit stuck in loop: %S(%S)[%d, %d] (%S)\n",
					pHeadUnit->getName().GetCString(), GET_PLAYER(pHeadUnit->getOwner()).getName(),
					pHeadUnit->getX(), pHeadUnit->getY(), szTempString.GetCString()
				);
				gDLL->messageControlLog(szOut);

				FErrorMsg(szOut);

				pHeadUnit->finishMoves();
			}
			
			else if (readyToMove())
			{
				FErrorMsg("splitting group");
				LOG_PLAYER_BLOCK(1, {
					CvPlot * pPlot = this->plot();
					int XCoord = -99;
					int YCoord = -99;
					if (pPlot != NULL)
					{
						XCoord = pPlot->getX();
						YCoord = pPlot->getY();
					}
					logAiEvaluations(1, "pHeadUnit NULL, Splitting Group %d [%d, %d]",
					this->m_iID,
					XCoord, YCoord);
				});
				splitGroup(1);
				break;
			}
			else
			{
				FErrorMsg("error in CvSelectionGroupAI::AI_update()");
			}

			break;
		}
		// if we want to force the group to attack, force another attack
		if (m_bGroupAttack)
		{
			m_bGroupAttack = false;

			groupAttack(m_iGroupAttackX, m_iGroupAttackY, MOVE_DIRECT_ATTACK, bFailedAlreadyFighting);
		}
		else // pick AI action
		{
			CvUnit* pHeadUnit = getHeadUnit();

			if (!pHeadUnit || pHeadUnit->isDelayedDeath())
			{
				break;
			}
			resetPath();

			if (pHeadUnit->AI_update())
			{
				// AI_update returns true when we should abort the loop and wait until next slice
				break;
			}
		}

		if (doDelayedDeath())
		{
			bDead = true;
			break;
		}

		// if no longer group attacking, and force separate is true, then bail, decide what to do after group is split up
		// (UnitAI of head unit may have changed)
		if (!m_bGroupAttack && AI_isForceSeparate())
		{
			AI_separate();	// pointers could become invalid...
			return false;
		}
	}

	if (!bDead && !isHuman() && !AI_isAwaitingContract())
	{
		bool bFollow = false;

		// if we not group attacking, then check for follow action
		if (!m_bGroupAttack)
		{
			foreach_ (CvUnit* unit, units() | filtered(CvUnit::fn::canMove()))
			{
				if (!readyToMove(true))
					break;

				resetPath();
				if (unit->AI_follow())
				{
					bFollow = true;
					break;
				}
			}
		}

		if (doDelayedDeath())
		{
			bDead = true;
		}
		else if (!bFollow && readyToMove(true))
		{
			pushMission(MISSION_SKIP);
		}
	}
	return !bDead && (isBusy() || isCargoBusy());
}


// Returns attack odds out of 100 (the higher, the better...)
int CvSelectionGroupAI::AI_attackOdds(const CvPlot* pPlot, bool bPotentialEnemy, bool bForce, bool* bWin, int iThreshold) const
{
#define	ODDS_CACHE_VALUE_MASK	1023
#define	ODDS_CACHE_WIN_BIT		1024
	static std::map<const CvPlot*, int>*	g_attackOddsCache = NULL;
	static const CvSelectionGroupAI*		g_attackOddsCacheGroup = NULL;
	int iResult;
	bool bIsWin = false;

	PROFILE_FUNC();

	if ( g_attackOddsCacheGroup != this )
	{
		g_attackOddsCacheGroup = this;

		if ( g_attackOddsCache == NULL )
		{
			g_attackOddsCache = new std::map<const CvPlot*,int>();
		}

		g_attackOddsCache->clear();
	}

	if ( bPotentialEnemy && !bForce )
	{
		std::map<const CvPlot*,int>::const_iterator itr = g_attackOddsCache->find(pPlot);

		if ( itr != g_attackOddsCache->end() )
		{
			iResult = (itr->second & ODDS_CACHE_VALUE_MASK);
			if ( bWin != NULL )
			{
				*bWin = ((itr->second & ODDS_CACHE_WIN_BIT) != 0);
			}

			return iResult;
		}
	}

	{
		PROFILE("CvSelectionGroupAI::AI_attackOdds.CacheMiss");

		CvUnit* pAttacker;

		FAssert(getOwner() != NO_PLAYER);

		if (!pPlot->hasDefender(false, NO_PLAYER, getOwner(), getHeadUnit(), !bPotentialEnemy, bPotentialEnemy))
		{
			return 100; // Disagrees with earlier count
		}

		// Initialize predicted health for all units involved
		int iStartAttackerMaxStrength = 0;
		int iNumDefenders = 0;
		bool bAttackingGroupHasAlwaysHostileUnits = false;

#ifdef _DEBUG
		char buffer[300];
#endif
		foreach_(CvUnit* pLoopUnit, units())
		{
			pLoopUnit->AI_setPredictedHitPoints(-1);

#ifdef _DEBUG
			sprintf(buffer,"Attacker id %d has start cur HP %d\n", pLoopUnit->getID(), pLoopUnit->getHP());
			OutputDebugString(buffer);
#endif
			if ( pLoopUnit->getHP() > 0 )
			{
				int iStr = pLoopUnit->maxCombatStr(pPlot, pLoopUnit);

				iStartAttackerMaxStrength += iStr; // Really want a more general valuation here that accounts
												//	for all promotions, not just those in action at this plot
#ifdef _DEBUG
				sprintf(buffer,"Attacker id %d has start str %d\n", pLoopUnit->getID(), iStr);
				OutputDebugString(buffer);
#endif
				pLoopUnit->AI_setPredictedHitPoints(pLoopUnit->getHP());

				if ( pLoopUnit->isAlwaysHostile(pPlot) )
				{
					bAttackingGroupHasAlwaysHostileUnits = true;
				}
			}
		}

		//	Similarly for the defenders
		int iStartDefenderMaxStrength = 0;
		foreach_(CvUnit* pLoopUnit, pPlot->units())
		{
			if (!(pLoopUnit->isInvisible(getTeam(), false)))
			{
				//	If we have always hostile untis attacking then any unit that isn't the same team is a target
				if ((bAttackingGroupHasAlwaysHostileUnits ? (pLoopUnit->getTeam() != getTeam()) : ::isPotentialEnemy(getTeam(),GET_PLAYER(pLoopUnit->getCombatOwner(getTeam(), pPlot)).getTeam())) &&
					pLoopUnit->getHP() > 0)
				{
					pLoopUnit->AI_setPredictedHitPoints(-1);

					int iStr = pLoopUnit->maxCombatStr(pPlot, pLoopUnit);

					iStartDefenderMaxStrength += iStr;	//	Really want a more general valuation here that accounts
														//	for all promotions, not just those in action at this plot
#ifdef _DEBUG
					sprintf(buffer,"Defender id %d has start str %d\n",pLoopUnit->getID(),iStr);
					OutputDebugString(buffer);
#endif
					pLoopUnit->AI_setPredictedHitPoints(pLoopUnit->getHP());

					//This is a routine to ensure that the attackers are not all invisible to this unit.  If they are, the unit won't have the opportunity to attack back afterwards and could sway the perception of needed strength to overcome a counterattack.
					if (plot()->getNumVisibleEnemyUnits(pLoopUnit) > 0)
					{
						iNumDefenders++;
					}
					if (iNumDefenders == 0)
					{
						iNumDefenders = 1;
					}
				}
			}
		}
		// 2*getNumUnits is not terribly effective if you have one unit.  If you're incapable of being attacked in response especially.  This is addressed above by adjusting the manner in which the defenders are counted.
		// Additionally other units could be on your tile as well that aren't part of your selection group.  This is addressed with iNumPotentialAttackers rather than previously getNumUnits() (number of units in the group).
		int iNumPotentialAttackers = plot()->getNumAttackers(getOwner());
		int iDivisor = std::max(1,2*iNumPotentialAttackers);
		int iInitialOddsThreshold = (iThreshold == -1 ? 0 : (iThreshold*iNumDefenders)/iDivisor);
		//int iLastBattleOdds = 0;
		CvUnit* pLastDefender = NULL;
		CvUnit* pLastAttacker = NULL;
		int	iLastOdds = 0;
		CvUnit* pDefender = NULL;

		do
		{
			int iOdds = 0;
			pAttacker = AI_getBestGroupAttacker(pPlot, bPotentialEnemy, iOdds, bForce, &pDefender);

			if ( pDefender != NULL && pAttacker != NULL )
			{
				pLastDefender = pDefender;
				pLastAttacker = pAttacker;
				iLastOdds = iOdds;

				//	If at any stage we don't have a minimal threshold of odds that would
				//	be likely to give us a result over the threshold asked for abort the
				//	calculation and return the apoproximation we have so far
				if ( iOdds < iInitialOddsThreshold )
				{
					pAttacker = NULL;
				}
			}
		} while( pAttacker && pDefender );

		if ( pLastAttacker == NULL )
		{
			//	There was no attacker in the first place!
			iResult = 0;
		}
		else
		{
			bIsWin = (pAttacker != NULL || iLastOdds > 50);
			if ( bWin != NULL )
			{
				*bWin = bIsWin;
			}

			FAssert( pLastDefender != NULL );

			//	Evaluate outcome
			int iEndAttackerMaxStrength = 0;
			bool bAttackerWonLastRound = (pLastDefender->currCombatStr(NULL,NULL) == 0);

			foreach_(CvUnit* pLoopUnit, units())
			{
#ifdef _DEBUG
				sprintf(buffer,"Attacker id %d has end cur HP %d, predicted %d\n",pLoopUnit->getID(),pLoopUnit->getHP(),pLoopUnit->AI_getPredictedHitPoints());
				OutputDebugString(buffer);
#endif
				if ( pLoopUnit->AI_getPredictedHitPoints() > 0 )
				{
					int iStr = pLoopUnit->maxCombatStr(pPlot, pLoopUnit);

					iEndAttackerMaxStrength += iStr;	//	Really want a more general valuation here that accounts
														//	for all promotions, not just those in action at this plot
#ifdef _DEBUG
					sprintf(buffer,"Attacker id %d has end str %d\n",pLoopUnit->getID(),iStr);
					OutputDebugString(buffer);
#endif
				}

				//	Reset predicted HP to make sure it doesn't get accidentally used from another context
				pLoopUnit->AI_setPredictedHitPoints(-1);
			}

			//	Similarly for the defenders
			int iEndDefenderMaxStrength = 0;

			foreach_(CvUnit* pLoopUnit, pPlot->units())
			{
				if (!(pLoopUnit->isInvisible(getTeam(), false)))
				{
					if ((bAttackingGroupHasAlwaysHostileUnits ? (pLoopUnit->getTeam() != getTeam()) : ::isPotentialEnemy(getTeam(),GET_PLAYER(pLoopUnit->getCombatOwner(getTeam(), pPlot)).getTeam())) )
					{
						if ( pLoopUnit->AI_getPredictedHitPoints() > 0 )
						{
							int iStr = pLoopUnit->maxCombatStr(pPlot, pLoopUnit);

							iEndDefenderMaxStrength += iStr;	//	Really want a more general valuation here that accounts
																//	for all promotions, not just those in action at this plot
#ifdef _DEBUG
							sprintf(buffer,"Defender id %d has end str %d\n",pLoopUnit->getID(),iStr);
							OutputDebugString(buffer);
#endif
						}

						pLoopUnit->AI_setPredictedHitPoints(-1);
					}
				}
			}

			//	Caller is set up to expect a probability of victory, but now we are evaluating
			//	full stack outcomes that is ill-defined.  We convert the outcomes losses on
			//	each side to a measure of the 'goodness' of the outcome, with 50 being equal
			//	losses on each side
			int iAttackerLosses = iStartAttackerMaxStrength - iEndAttackerMaxStrength;
			int iDefenderLosses = iStartDefenderMaxStrength - iEndDefenderMaxStrength;
			//	Last round result can be considered to go either way based on the final round returned odds
			if ( bAttackerWonLastRound )
			{
				//	Give the defender a proportion of their last unit value back based on final round odds
				//	unless it was an uncontested win
				if ( pLastDefender != NULL && pLastDefender->canDefend() )
				{
					iAttackerLosses += (pLastAttacker->maxCombatStr(pPlot,pLastAttacker)*(100-iLastOdds))/100;
				}
			}
			else
			{
				//	Give the attacker a proportion of their last unit value back based on final round odds
				iDefenderLosses += (pLastDefender->maxCombatStr(pPlot,pLastDefender)*iLastOdds)/100;
			}

			//	Special case for attack against a defenseless unit
			if ( iDefenderLosses == 0 && iAttackerLosses == 0 )
			{
				/*FAssert(bAttackerWonLastRound);*/

				iResult = 100;
			}
			else
			{
				iResult = (iDefenderLosses*100)/(iDefenderLosses + iAttackerLosses);
			}

			FAssert(iResult >= 0);
			FAssert(iResult <= ODDS_CACHE_VALUE_MASK);
		}

		if ( bPotentialEnemy && !bForce )
		{

			(*g_attackOddsCache)[pPlot] = (iResult & ODDS_CACHE_VALUE_MASK) + (bIsWin ? ODDS_CACHE_WIN_BIT : 0);
		}
	}

	return iResult;
}

static bool isClearlySuperior(CvUnit* pUnit, CvUnit* pOtherUnit, const CvPlot* pPlot)
{
	PROFILE_FUNC();

	if (pUnit->currEffectiveStr(pPlot, pUnit) < pOtherUnit->currEffectiveStr(pPlot, pOtherUnit)
	// First strikes
	|| pUnit->getCombatFirstStrikes() < pOtherUnit->getCombatFirstStrikes())
	{
		return false;
	}
	int	iTotalCombatMods = 0;
	int	iOtherTotalCombatMods = 0;

	//	Unit combat modifiers
	const CvUnitInfo& kUnit = pUnit->getUnitInfo();
	const CvUnitInfo& kOtherUnit = pOtherUnit->getUnitInfo();

	for (int i = GC.getNumUnitCombatInfos() - 1; i > -1; i--)
	{
		iTotalCombatMods += kUnit.getUnitCombatModifier(i);
		iOtherTotalCombatMods += kOtherUnit.getUnitCombatModifier(i);
	}

	for (std::map<UnitCombatTypes, UnitCombatKeyedInfo>::const_iterator it = pUnit->getUnitCombatKeyedInfo().begin(), end = pUnit->getUnitCombatKeyedInfo().end(); it != end; ++it)
	{
		iTotalCombatMods += it->second.m_iExtraUnitCombatModifier;
	}

	if (!pUnit->isCommander())
	{
		const CvUnit* pCommander = pUnit->getCommander();
		if (pCommander)
		{
			for (std::map<UnitCombatTypes, UnitCombatKeyedInfo>::const_iterator it = pCommander->getUnitCombatKeyedInfo().begin(), end = pCommander->getUnitCombatKeyedInfo().end(); it != end; ++it)
			{
				iTotalCombatMods += it->second.m_iExtraUnitCombatModifier;
			}
		}
	}

	if (!pUnit->isCommodore())
	{
		const CvUnit* pCommodore = pUnit->getCommodore();
		if (pCommodore)
		{
			for (std::map<UnitCombatTypes, UnitCombatKeyedInfo>::const_iterator it = pCommodore->getUnitCombatKeyedInfo().begin(), end = pCommodore->getUnitCombatKeyedInfo().end(); it != end; ++it)
			{
				iTotalCombatMods += it->second.m_iExtraUnitCombatModifier;
			}
		}
	}

	for (std::map<UnitCombatTypes, UnitCombatKeyedInfo>::const_iterator it = pOtherUnit->getUnitCombatKeyedInfo().begin(), end = pOtherUnit->getUnitCombatKeyedInfo().end(); it != end; ++it)
	{
		iOtherTotalCombatMods += it->second.m_iExtraUnitCombatModifier;
	}

	if (!pOtherUnit->isCommander())
	{
		const CvUnit* pOtherCommander = pOtherUnit->getCommander();
		if (pOtherCommander)
		{
			for (std::map<UnitCombatTypes, UnitCombatKeyedInfo>::const_iterator it = pOtherCommander->getUnitCombatKeyedInfo().begin(), end = pOtherCommander->getUnitCombatKeyedInfo().end(); it != end; ++it)
			{
				iOtherTotalCombatMods += it->second.m_iExtraUnitCombatModifier;
			}
		}
	}

    if (!pOtherUnit->isCommodore())
    	{
    		const CvUnit* pOtherCommodore = pOtherUnit->getCommodore();
    		if (pOtherCommodore)
    		{
    			for (std::map<UnitCombatTypes, UnitCombatKeyedInfo>::const_iterator it = pOtherCommodore->getUnitCombatKeyedInfo().begin(), end = pOtherCommodore->getUnitCombatKeyedInfo().end(); it != end; ++it)
    			{
    				iOtherTotalCombatMods += it->second.m_iExtraUnitCombatModifier;
    			}
    		}
    	}

	return iTotalCombatMods >= iOtherTotalCombatMods;
}

CvUnit* CvSelectionGroupAI::AI_getBestGroupAttacker(
	const CvPlot* pPlot,
	bool bPotentialEnemy,
	int& iUnitOdds,
	bool bForce,
	CvUnit** ppDefender,
	bool bAssassinate,
	bool bSuprise,
	bool bIgnoreMadeAttack,
	const std::set<int>& ignoreUnitID) const
{
	PROFILE_FUNC();

	const bool bIsHuman = isHuman();

	const MoveCheck::flags moveCheckFlags = MoveCheck::Attack |
		(bPotentialEnemy ? MoveCheck::DeclareWar : MoveCheck::None) |
		MoveCheck::CheckForBest |
		(bAssassinate ? MoveCheck::Assassinate : MoveCheck::None) |
		(bSuprise ? MoveCheck::Suprise : MoveCheck::None);

	int iBestOdds = 0;
	int iBestValue = 0;
	CvUnit* pBestUnit = NULL;

	foreach_(CvUnit* unitX, units())
	{
		if (unitX->isDead()
		|| ignoreUnitID.find(unitX->getID()) != ignoreUnitID.end()
		|| pBestUnit && isClearlySuperior(pBestUnit, unitX, pPlot))
		{
			continue;
		}
		if (
			(
				unitX->getDomainType() == DOMAIN_AIR
				?
				unitX->canAirAttack()
				:
				unitX->canAttack()
				&&
				(
					bIgnoreMadeAttack
					||
					unitX->isBlitz()
					||
					!unitX->isMadeAttack()
				)
			)
		&& (bSuprise || !unitX->AI_getHasAttacked())
		&& (bForce || unitX->canMove()))
		{
			CvUnit* pBestDefender = NULL;
			if (bForce || unitX->canEnterPlot(pPlot, moveCheckFlags, &pBestDefender))
			{
				PROFILE("AI_getBestGroupAttacker.RegularAttackOdds");

				const int iOdds =
				(
					pBestDefender
					?
					unitX->AI_attackOddsAtPlot(pPlot, (CvUnitAI*)pBestDefender)
					:
					unitX->AI_attackOdds(pPlot, bPotentialEnemy, 0, bAssassinate)
				);
				int iValue = iOdds;
				FAssertMsg(iValue > 0, "iValue is expected to be greater than 0");

				if (unitX->collateralDamage() > 0)
				{
					const int iPossibleTargets = std::min((pPlot->getNumVisiblePotentialEnemyDefenders(unitX) - 1), unitX->collateralDamageMaxUnits());

					if (iPossibleTargets > 0)
					{
						iValue *= (100 + ((unitX->collateralDamage() * iPossibleTargets) / 5));
						iValue /= 100;
					}
				}

				// if non-human, prefer the last unit that has the best value (so as to avoid splitting the group)
				if (iValue > iBestValue || (!bIsHuman && iValue > 0 && iValue == iBestValue))
				{
					iBestValue = iValue;
					iBestOdds = iOdds;
					pBestUnit = unitX;
				}
			}
		}
	}

	if (ppDefender && pBestUnit)
	{
		PROFILE("AI_getBestGroupAttacker.FinalOdds");

		// Recalculate, this time modifying the predicted hitpoints that will result
		// if the battle occurs.  This will return a better odds estimate than we
		//	had previously, but should not change the unit choice
		iBestOdds = pBestUnit->AI_attackOdds(pPlot, bPotentialEnemy, ppDefender);
	}
	iUnitOdds = iBestOdds;
	return pBestUnit;
}

CvUnit* CvSelectionGroupAI::AI_getBestGroupSacrifice(const CvPlot* pPlot, bool bForce, bool bNoBlitz) const
{
	PROFILE_EXTRA_FUNC();

	int iBestValue = 0;
	CvUnit* pBestUnit = NULL;

	foreach_(CvUnit* unitX, units())
	{
		if (!unitX->isDead()
		&& (
				unitX->getDomainType() == DOMAIN_AIR
				?
				unitX->canAirAttack()
				:
				unitX->canAttack()
				&&
				(bNoBlitz ? !unitX->isMadeAttack() : unitX->isBlitz() || !unitX->isMadeAttack())
			)
		&& (bForce || unitX->canMove() && unitX->canEnterPlot(pPlot, MoveCheck::Attack)))
		{
			const int iValue = unitX->AI_sacrificeValue(pPlot);
			FASSERT_NOT_NEGATIVE(iValue);

			// we want to pick the last unit of highest value, so pick the last unit with a good value
			if (iValue > 0 && iValue >= iBestValue)
			{
				iBestValue = iValue;
				pBestUnit = unitX;
			}
		}
	}
	return pBestUnit;
}

// Returns ratio of strengths of stacks times 100
// (so 100 is even ratio, numbers over 100 mean this group is more powerful than the stack on a plot)
int CvSelectionGroupAI::AI_compareStacks(const CvPlot* pPlot, StackCompare::flags flags /*= StackCompare::None*/, int iRange /*= 0*/) const
{
	PROFILE_FUNC();

	FAssert(pPlot != NULL);

	DomainTypes eDomainType = getDomainType();

	// if not aircraft, then choose based on the plot, not the head unit (mainly for transport carried units)
	if (eDomainType != DOMAIN_AIR)
	{
		if (pPlot->isWater())
			eDomainType = DOMAIN_SEA;
		else
			eDomainType = DOMAIN_LAND;
	}

	bool bDivideFirst = false;
	int iAttackStrength = AI_sumStrength(pPlot, eDomainType, flags);
	int compareRatio = iAttackStrength;
	if (compareRatio >= (MAX_INT/100))
	{
		compareRatio /= 100;
		bDivideFirst = true;
	}
	compareRatio *= 100;

	PlayerTypes eOwner = getOwner();
	if (eOwner == NO_PLAYER)
	{
		eOwner = getHeadOwner();
	}
	FAssert(eOwner != NO_PLAYER);

/* original bts code
	int defenderSum = pPlot->AI_sumStrength(NO_PLAYER, getOwner(), eDomainType, true, !bPotentialEnemy, bPotentialEnemy);
*/
	// Clearly meant to use eOwner here ...
	//	Koshling - changed final param from 'bPotentialEnemy' to '!bPotentialEnemy' since the param is whether
	//	to test, and the value we have here is an assertion.  This bug was causing the AI to send inadequate stacks
	//	against cities of players it was not YET at war with and causing a premature war
	const bool bPotentialEnemy = flags & StackCompare::PotentialEnemy;
	const bool bFast = flags & StackCompare::Fast;
	StrengthFlags::flags strengthFlags = bFast? StrengthFlags::None : StrengthFlags::DefensiveBonuses;
	if (!bFast && !bPotentialEnemy)
	{
		strengthFlags |= StrengthFlags::TestAtWar;
		strengthFlags |= StrengthFlags::TestPotentialEnemy;
	}
	int defenderSum = pPlot->AI_sumStrength(NO_PLAYER, eOwner, eDomainType, strengthFlags, iRange);
	if (bDivideFirst)
	{
		defenderSum /= 100;
	}
	compareRatio /= std::max(1, defenderSum);
	LOG_UNIT_BLOCK(4, {
		CvUnit* pUnit = this->getHeadUnit();
		FAssert(pUnit != NULL);
		UnitAITypes eUnitAi = pUnit->AI_getUnitAIType();
		MissionAITypes eMissionAI = this->AI_getMissionAIType();
		CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
		CvWString MissionInfos = MissionAITypeToString(eMissionAI);
		CvWString StrUnitName = pUnit->getName();
		if (StrUnitName.length() == 0)
		{
			StrUnitName = pUnit->getName(0).GetCString();
		}
		logAiEvaluations(4,"Player %d Group ID %d Head ID %d, %S of Type %S, at (%d, %d), Evaluation of Combat Odds [stack size %d], Attack %d Defense %d, Ratio %d", 
			getOwner(), m_iID, pUnit->getID(), StrUnitName.GetCString(), StrunitAIType.GetCString(), pPlot->getX(), pPlot->getY(), getNumUnits(), iAttackStrength, defenderSum, compareRatio);
	});

	return compareRatio;
}

int CvSelectionGroupAI::AI_sumStrength(const CvPlot* pAttackedPlot, DomainTypes eDomainType, StackCompare::flags flags /*= StackCompare::None*/) const
{
	PROFILE_EXTRA_FUNC();
	if (getNumUnits() == 0)
		return 0;

	uint64_t strSum = 0;
	const int COLLATERAL_COMBAT_DAMAGE = GC.getCOLLATERAL_COMBAT_DAMAGE(); // K-Mod. (currently this number is "10")

	const bool bCheckCanAttack = flags & StackCompare::CheckCanAttack;
	const bool bCheckCanMove = flags & StackCompare::CheckCanMove;
	const bool bFastMode = flags & StackCompare::Fast;

	const int iNumPotentialDefenders = pAttackedPlot->getNumVisiblePotentialEnemyDefenders(getHeadUnit()) - 1;

	foreach_ (const CvUnit* unit, units() | filtered(!CvUnit::fn::isDead()))
	{
		if ((
				!bCheckCanAttack
				|| (unit->getDomainType() == DOMAIN_AIR && unit->canAirAttack())
				|| (unit->getDomainType() != DOMAIN_AIR && unit->canAttack())
			)
			&& (!bCheckCanMove || unit->canMove())
			//TB: canEnterPlot may need simplified here somehow.
			&& (!bCheckCanMove || pAttackedPlot == NULL || unit->canEnterPlot(pAttackedPlot, MoveCheck::Attack | MoveCheck::DeclareWar | MoveCheck::IgnoreLocation))
			&& (eDomainType == NO_DOMAIN || unit->getDomainType() == eDomainType)
			)
		{
			//strSum += (uint64_t)unit->currEffectiveStr(pAttackedPlot, pLoopUnit);
			//TB Simplify for speed:
			strSum += (uint64_t)unit->currCombatStr(NULL,NULL);
			// K-Mod estimate the attack power of collateral units
			if (!bFastMode && unit->collateralDamage() > 0 && pAttackedPlot != plot())
			{
				const int iPossibleTargets = std::min(iNumPotentialDefenders, unit->collateralDamageMaxUnits());

				if (iPossibleTargets > 0)
				{
					// collateral damage is not trivial to calculate. This estimate is pretty rough.
					strSum += (uint64_t)unit->baseCombatStrNonGranular() * COLLATERAL_COMBAT_DAMAGE * unit->collateralDamage() * iPossibleTargets / 100;
				}
			}

			if (strSum >= MAX_INT)
				break;
			// K-Mod end
		}
	}
	return static_cast<int>(std::min<uint64_t>(MAX_INT, strSum));
}

void CvSelectionGroupAI::AI_queueGroupAttack(int iX, int iY)
{
	m_bGroupAttack = true;

	m_iGroupAttackX = iX;
	m_iGroupAttackY = iY;
}

inline void CvSelectionGroupAI::AI_cancelGroupAttack()
{
	m_bGroupAttack = false;
}

inline bool CvSelectionGroupAI::AI_isGroupAttack() const
{
	return m_bGroupAttack;
}

bool CvSelectionGroupAI::AI_isControlled() const
{
	return (!isHuman() || isAutomated());
}


bool CvSelectionGroupAI::AI_isDeclareWar(const CvPlot* pPlot) const
{
	FAssert(getHeadUnit() != NULL);

	if (isHuman())
	{
		return false;
	}

	bool bLimitedWar = false;
	if (pPlot != NULL)
	{
		TeamTypes ePlotTeam = pPlot->getTeam();
		if (ePlotTeam != NO_TEAM && GET_TEAM(getTeam()).AI_getWarPlan(ePlotTeam) == WARPLAN_LIMITED)
		{
			bLimitedWar = true;
		}
	}

	CvUnit* pHeadUnit = getHeadUnit();

	switch (pHeadUnit->AI_getUnitAIType())
	{
	case UNITAI_UNKNOWN:
	case UNITAI_ANIMAL:
	case UNITAI_SETTLE:
	case UNITAI_WORKER:
	case UNITAI_SUBDUED_ANIMAL:
	case UNITAI_HUNTER:
	case UNITAI_HUNTER_ESCORT:
	case UNITAI_HEALER:
	case UNITAI_PROPERTY_CONTROL:
	case UNITAI_HEALER_SEA:
	case UNITAI_PROPERTY_CONTROL_SEA:
	case UNITAI_INVESTIGATOR:
	case UNITAI_INFILTRATOR:
	case UNITAI_SEE_INVISIBLE:
	case UNITAI_SEE_INVISIBLE_SEA:
	case UNITAI_ESCORT:
		break;
	case UNITAI_ATTACK_CITY:
	case UNITAI_ATTACK_CITY_LEMMING:
		return true;
		break;

	case UNITAI_ATTACK:
	case UNITAI_COLLATERAL:
	case UNITAI_PILLAGE:
		if (bLimitedWar)
		{
			return true;
		}
		break;

	case UNITAI_PARADROP:
	case UNITAI_RESERVE:
	case UNITAI_COUNTER:
	case UNITAI_PILLAGE_COUNTER:
	case UNITAI_CITY_DEFENSE:
	case UNITAI_CITY_COUNTER:
	case UNITAI_CITY_SPECIAL:
	case UNITAI_EXPLORE:
	case UNITAI_MISSIONARY:
	case UNITAI_PROPHET:
	case UNITAI_ARTIST:
	case UNITAI_SCIENTIST:
	case UNITAI_GENERAL:
	case UNITAI_GREAT_HUNTER:
	case UNITAI_GREAT_ADMIRAL:
	case UNITAI_MERCHANT:
	case UNITAI_ENGINEER:
	case UNITAI_SPY:
	case UNITAI_ICBM:
	case UNITAI_WORKER_SEA:
	case UNITAI_BARB_CRIMINAL:
		break;

	case UNITAI_ATTACK_SEA:
	case UNITAI_RESERVE_SEA:
	case UNITAI_ESCORT_SEA:
		if (bLimitedWar)
		{
			return true;
		}
		break;
	case UNITAI_EXPLORE_SEA:
		break;

	case UNITAI_ASSAULT_SEA:
		if (hasCargo())
		{
			return true;
		}
		break;

	case UNITAI_SETTLER_SEA:
	case UNITAI_MISSIONARY_SEA:
	case UNITAI_SPY_SEA:
	case UNITAI_CARRIER_SEA:
	case UNITAI_MISSILE_CARRIER_SEA:
	case UNITAI_PIRATE_SEA:
	case UNITAI_ATTACK_AIR:
	case UNITAI_DEFENSE_AIR:
	case UNITAI_CARRIER_AIR:
	case UNITAI_MISSILE_AIR:
		break;

	default:
		FErrorMsg("error");
		break;
	}

	return false;
}


CvPlot* CvSelectionGroupAI::AI_getMissionAIPlot() const
{
	return GC.getMap().plotSorenINLINE(m_iMissionAIX, m_iMissionAIY);
}


bool CvSelectionGroupAI::AI_isForceSeparate() const
{
	return m_bForceSeparate;
}


void CvSelectionGroupAI::AI_makeForceSeparate()
{
	m_bForceSeparate = true;
}


MissionAITypes CvSelectionGroupAI::AI_getMissionAIType() const
{
	return m_eMissionAIType;
}

void CvSelectionGroupAI::AI_noteSizeChange(int iChange, int iVolume)
{
	if ( m_eMissionAIType != NO_MISSIONAI )
	{
		const CvPlot* pPlot = AI_getMissionAIPlot();

		if ( pPlot != NULL )
		{
			GET_PLAYER(getOwner()).AI_noteMissionAITargetCountChange(m_eMissionAIType, pPlot, iChange, NULL, iVolume);
		}
	}
}

void CvSelectionGroupAI::AI_setMissionAI(MissionAITypes eNewMissionAI, const CvPlot* newPlot, const CvUnit* pNewUnit)
{
	PROFILE_FUNC();
	const CvPlot* oldPlot = AI_getMissionAIPlot();
	const MissionAITypes eOldMissionAI = m_eMissionAIType;

	if (oldPlot && eOldMissionAI != NO_MISSIONAI)
	{
		GET_PLAYER(getOwner()).AI_noteMissionAITargetCountChange(eOldMissionAI, oldPlot, -getNumUnits(), plot(), -getNumUnitCargoVolumeTotal());
	}

	// Worker city tracking
	{
		const CvPlot* plotX = newPlot ? newPlot : plot();
		CvCity* cityX = plotX ? plotX->getWorkingCity() : NULL;

		foreach_(CvUnit* unitX, units())
		{
			UnitCompWorker* workerComp = unitX->getWorkerComponent();
			if (workerComp)
			{
				const int iAssignedCityID = workerComp->getAssignedCity();

				if (cityX && cityX->getOwner() == getOwner())
				{
					if (iAssignedCityID != cityX->getID())
					{
						if (iAssignedCityID != -1)
						{
							CvCity* city = GET_PLAYER(getOwner()).getCity(iAssignedCityID);
							if (city)
							{
								if (gUnitLogLevel > 2)
								{
									logBBAI("    Worker (%d) at (%d,%d) detaching from mission for city %S", unitX->getID(), unitX->getX(), unitX->getY(), city->getName().GetCString());
								}
								city->setWorkerHave(unitX->getID(), false);
							}
							else
							{
								FErrorMsg("Worker assigned to a NULL city...")
								workerComp->setCityAssignment(-1);
							}
						}
						if (gUnitLogLevel > 2)
						{
							logBBAI("Worker (%d) at (%d,%d) attaching to mission for city %S\n", unitX->getID(), unitX->getX(), unitX->getY(), cityX->getName().GetCString());
						}
						cityX->setWorkerHave(unitX->getID(), true);
					}
				}
				else if (iAssignedCityID != -1)
				{
					CvCity* city = GET_PLAYER(getOwner()).getCity(iAssignedCityID);
					if (city)
					{
						if (gUnitLogLevel > 2)
						{
							logBBAI("    Worker (%d) at (%d,%d) detaching from mission for city %S", unitX->getID(), unitX->getX(), unitX->getY(), city->getName().GetCString());
						}
						city->setWorkerHave(unitX->getID(), false);
					}
					else
					{
						FErrorMsg("Worker assigned to a NULL city...")
						workerComp->setCityAssignment(-1);
					}
				}
			}
		}
	}
	// Set mission AI
	m_eMissionAIType = eNewMissionAI;

	if (newPlot)
	{
		m_iMissionAIX = newPlot->getX();
		m_iMissionAIY = newPlot->getY();

		if (eNewMissionAI != NO_MISSIONAI)
		{
			GET_PLAYER(getOwner()).AI_noteMissionAITargetCountChange(eNewMissionAI, newPlot, getNumUnits(), plot(), getNumUnitCargoVolumeTotal());
		}
	}
	else
	{
		m_iMissionAIX = INVALID_PLOT_COORD;
		m_iMissionAIY = INVALID_PLOT_COORD;
	}

	if (pNewUnit)
	{
		m_missionAIUnit = pNewUnit->getIDInfo();
	}
	else m_missionAIUnit.reset();
}


CvUnit* CvSelectionGroupAI::AI_getMissionAIUnit() const
{
	return getUnit(m_missionAIUnit);
}

bool CvSelectionGroupAI::AI_isFull() const
{
	PROFILE_EXTRA_FUNC();
	FAssert(getNumUnits() > 0);

	const UnitAITypes eUnitAI = getHeadUnitAI();
	// do two passes, the first pass, we ignore units with speical cargo
	int iSpecialCargoCount = 0;
	int iCargoCount = 0;

	// first pass, count but ignore special cargo units
	foreach_(const CvUnit* pLoopUnit, units())
	{
		if (pLoopUnit->AI_getUnitAIType() == eUnitAI)
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
	}

	// if every unit in the group has special cargo, then check those, otherwise, consider ourselves full
	if (iSpecialCargoCount >= iCargoCount)
	{
		foreach_(const CvUnit* pLoopUnit, units())
		{
			if (pLoopUnit->AI_getUnitAIType() == eUnitAI && !pLoopUnit->isFull())
			{
				return false;
			}
		}
	}

	return true;
}

int CvSelectionGroupAI::AI_getGenericValueTimes100(UnitValueFlags eFlags) const
{
	PROFILE_EXTRA_FUNC();
	int iResult = 0;

	foreach_(const CvUnit* pLoopUnit, units())
	{
		iResult += pLoopUnit->AI_genericUnitValueTimes100(eFlags);
	}

	return iResult;
}

bool CvSelectionGroupAI::AI_hasBeneficialPropertyEffectForCity(const CvCity* pCity, PropertyTypes pProperty) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvUnit* pLoopUnit, units())
	{
		if (pLoopUnit->AI_beneficialPropertyValueToCity(pCity, pProperty) > 0)
		{
			return true;
		}
	}

	return false;
}

CvUnit* CvSelectionGroupAI::AI_ejectBestPropertyManipulator(const CvCity* pTargetCity)
{
	PROFILE_EXTRA_FUNC();
	CvUnit* pBestUnit = NULL;
	int iBestUnitValue = 0;

	foreach_(CvUnit* pLoopUnit, units())
	{
		const int iValue = pLoopUnit->AI_beneficialPropertyValueToCity(pTargetCity, NO_PROPERTY);

		if (iValue > iBestUnitValue)
		{
			iBestUnitValue = iValue;
			pBestUnit = pLoopUnit;
		}
	}

	if (NULL != pBestUnit && getNumUnits() > 1)
	{
		pBestUnit->joinGroup(NULL);
	}

	return pBestUnit;
}

CvUnit* CvSelectionGroupAI::AI_findBestDefender(const CvPlot* pDefendPlot, bool allowAllDefenders, bool bConsiderPropertyValues) const
{
	PROFILE_EXTRA_FUNC();
	CvUnit* pBestUnit = NULL;
	int iBestUnitValue = 0;

	foreach_(CvUnit* pLoopUnit, units())
	{
		if (!pLoopUnit->noDefensiveBonus() || (allowAllDefenders && pLoopUnit->canDefend()))
		{
			int iPropertyValue = 0;
			int iValue = pLoopUnit->currEffectiveStr(pDefendPlot, NULL) * 100;

			if (pDefendPlot->isCity(true, getTeam()))
			{
				if ( bConsiderPropertyValues )
				{
					iPropertyValue = GET_PLAYER(getOwner()).AI_unitPropertyValue(pLoopUnit->getUnitType())*200;
					iValue += iPropertyValue;
				}

				iValue *= 100 + pLoopUnit->cityDefenseModifier();

				iValue /= 100;
			}

			if (plot()->isCity(true, getTeam()))
			{
				if ( bConsiderPropertyValues )
				{
					iPropertyValue = GET_PLAYER(getOwner()).AI_unitPropertyValue(pLoopUnit->getUnitType())*200;
					iValue -= iPropertyValue;
				}

				iValue *= 100;
				iValue /= std::max(1,100 + pLoopUnit->cityDefenseModifier());
			}

			iValue *= 100;
			iValue /= std::max(1,100 + pLoopUnit->cityAttackModifier());

			iValue /= 2 + pLoopUnit->getLevel();

			if (iValue > iBestUnitValue || (pBestUnit == NULL && allowAllDefenders))
			{
				iBestUnitValue = iValue;
				pBestUnit = pLoopUnit;
			}
		}
	}

	return pBestUnit;
}

CvUnit* CvSelectionGroupAI::AI_ejectBestDefender(const CvPlot* pDefendPlot, bool allowAllDefenders)
{
	CvUnit*	pBestUnit = AI_findBestDefender(pDefendPlot, allowAllDefenders, true);

	if (NULL != pBestUnit && getNumUnits() > 1)
	{
		pBestUnit->joinGroup(NULL);
	}

	return pBestUnit;
}

bool CvSelectionGroupAI::AI_isCityGarrison(const CvCity* pCity) const
{
	const CvUnit* pHeadUnit = getHeadUnit();
	return pHeadUnit ? pHeadUnit->AI_isCityGarrison(pCity) : false;
}

void CvSelectionGroupAI::AI_setAsGarrison(const CvCity* pCity)
{
	CvUnit* pHeadUnit = getHeadUnit();
	if (pHeadUnit != NULL)
	{
		pHeadUnit->AI_setAsGarrison(pCity);
	}
}


// Protected Functions...

void CvSelectionGroupAI::read(FDataStreamBase* pStream)
{
	CvSelectionGroup::read(pStream);

	pStream->Read(&m_iMissionAIX);
	pStream->Read(&m_iMissionAIY);

	pStream->Read(&m_bForceSeparate);

	pStream->Read((int*)&m_eMissionAIType);

	pStream->Read((int*)&m_missionAIUnit.eOwner);
	pStream->Read(&m_missionAIUnit.iID);

	pStream->Read(&m_bGroupAttack);
	pStream->Read(&m_iGroupAttackX);
	pStream->Read(&m_iGroupAttackY);
}


void CvSelectionGroupAI::write(FDataStreamBase* pStream)
{
	CvSelectionGroup::write(pStream);

	pStream->Write(m_iMissionAIX);
	pStream->Write(m_iMissionAIY);

	pStream->Write(m_bForceSeparate);

	pStream->Write(m_eMissionAIType);

	pStream->Write(m_missionAIUnit.eOwner);
	pStream->Write(m_missionAIUnit.iID);

	pStream->Write(m_bGroupAttack);
	pStream->Write(m_iGroupAttackX);
	pStream->Write(m_iGroupAttackY);
}

bool CvSelectionGroupAI::AI_isAwaitingContract() const
{
	return (getHeadUnit() != NULL && getHeadUnit()->AI_isAwaitingContract());
}

// Private Functions...
