// selectionGroupAI.cpp

#include "CvGameCoreDLL.h"
#include "BetterBTSAI.h"
#include <boost/bind.hpp>
// Public Functions...

CvSelectionGroupAI::CvSelectionGroupAI()
{
	AI_reset();
}


CvSelectionGroupAI::~CvSelectionGroupAI()
{
	AI_uninit();
}


void CvSelectionGroupAI::AI_init()
{
	AI_reset();

	//--------------------------------
	// Init other game data
}


void CvSelectionGroupAI::AI_uninit()
{
}


void CvSelectionGroupAI::AI_reset()
{
	AI_uninit();

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
	bool matchAll(CvUnit*)
	{
		return true;
	}
	bool matchNonAI(CvUnit* unit, UnitAITypes unitAI)
	{
		return unit->AI_getUnitAIType() != unitAI;
	}
	bool matchAI(CvUnit* unit, UnitAITypes unitAI)
	{
		return unit->AI_getUnitAIType() == unitAI;
	}
	bool matchImpassable(CvUnit* unit, const CvPlayerAI& player)
	{
		return player.AI_unitImpassableCount(unit->getUnitType()) > 0;
	}
	bool matchEmptyTransport(CvUnit* unit)
	{
		return unit->AI_getUnitAIType() == UNITAI_ASSAULT_SEA && !unit->hasCargo();
	}
}

void CvSelectionGroupAI::AI_separateIf(boost::function<bool(CvUnit*)> predicateFn)
{
	std::vector<CvUnit*> toRemove = get_if(predicateFn);

	for (std::vector<CvUnit*>::iterator itr = toRemove.begin(); itr != toRemove.end(); ++itr)
	{
		CvUnit* unit = *itr;
		unit->joinGroup(NULL);
		FAssertMsg(std::find(beginUnits(), endUnits(), unit) == endUnits(), "Failed to remove unit from group");
		if (unit->plot()->getTeam() == getTeam())
		{
			unit->getGroup()->pushMission(MISSION_SKIP);
		}
	}
}

void CvSelectionGroupAI::AI_separate()
{
	AI_separateIf(matchAll);
}

void CvSelectionGroupAI::AI_separateNonAI(UnitAITypes eUnitAI)
{
	AI_separateIf(boost::bind(matchNonAI, _1, eUnitAI));
}

void CvSelectionGroupAI::AI_separateAI(UnitAITypes eUnitAI)
{
	AI_separateIf(boost::bind(matchAI, _1, eUnitAI));
}

void CvSelectionGroupAI::AI_separateImpassable()
{
	AI_separateIf(boost::bind(matchImpassable, _1, boost::ref(GET_PLAYER(getOwner()))));
}

void CvSelectionGroupAI::AI_separateEmptyTransports()
{
	AI_separateIf(matchEmptyTransport);
}

// Returns true if the group has become busy...
bool CvSelectionGroupAI::AI_update()
{
	CLLNode<IDInfo>* pEntityNode;
	CvUnit* pLoopUnit;
	bool bDead;
	bool bFollow;

	PROFILE("CvSelectionGroupAI::AI_update");

	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (!AI_isControlled())
	{
		return false;
	}

	if (getNumUnits() == 0)
	{
		return false;
	}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      04/28/10                                jdog5000      */
/*                                                                                              */
/* Unit AI                                                                                      */
/************************************************************************************************/
	if( !(isHuman()) && !(getHeadUnit()->isCargo()) && (getActivityType() == ACTIVITY_SLEEP))
	{
		setForceUpdate(true);
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	if (isForceUpdate())
	{
		clearMissionQueue(); // XXX ???
		setActivityType(ACTIVITY_AWAKE);
		setForceUpdate(false);

		// if we are in the middle of attacking with a stack, cancel it
		AI_cancelGroupAttack();
	}

	setGroupToCacheFor(this);

	FAssert(!(GET_PLAYER(getOwnerINLINE()).isAutoMoves()) || isHuman());

	int iTempHack = 0; // XXX

	bDead = false;
	
	bool bFailedAlreadyFighting = false;
	while ((m_bGroupAttack && !bFailedAlreadyFighting) || readyToMove())
	{
		iTempHack++;
		if (iTempHack > 90 && iTempHack < 100)
		{
			FAssert(false);
			CvUnit* pHeadUnit = getHeadUnit();
			if (NULL != pHeadUnit)
			{
				//if (GC.getLogging())
				//{
					int iPass = iTempHack - 90;
					TCHAR szOut[1024];
					CvWString szTempString;
					getUnitAIString(szTempString, pHeadUnit->AI_getUnitAIType());
					sprintf(szOut, "Unit stuck in loop( Warning before short circuit (pass: %d) ): %S(%S)[%d, %d] (%S)\n", iPass, pHeadUnit->getName().GetCString(), GET_PLAYER(pHeadUnit->getOwnerINLINE()).getName(),
						pHeadUnit->getX_INLINE(), pHeadUnit->getY_INLINE(), szTempString.GetCString());
					gDLL->messageControlLog(szOut);
				//}
			}
		}
		if (iTempHack >= 100)
		{
			FAssert(false);
			CvUnit* pHeadUnit = getHeadUnit();
			if (NULL != pHeadUnit)
			{
				if (GC.getLogging())
				{
					TCHAR szOut[1024];
					CvWString szTempString;
					getUnitAIString(szTempString, pHeadUnit->AI_getUnitAIType());
					sprintf(szOut, "Unit stuck in loop: %S(%S)[%d, %d] (%S)\n", pHeadUnit->getName().GetCString(), GET_PLAYER(pHeadUnit->getOwnerINLINE()).getName(),
						pHeadUnit->getX_INLINE(), pHeadUnit->getY_INLINE(), szTempString.GetCString());
					gDLL->messageControlLog(szOut);
				}
				
				pHeadUnit->finishMoves();
			}
			else if (readyToMove())
			{
				splitGroup(1);
				break;
			}
			break;
		}

		// if we want to force the group to attack, force another attack
		if (m_bGroupAttack)
		{			
			m_bGroupAttack = false;

			groupAttack(m_iGroupAttackX, m_iGroupAttackY, MOVE_DIRECT_ATTACK, bFailedAlreadyFighting);
		}
		// else pick AI action
		else
		{
			CvUnit* pHeadUnit = getHeadUnit();

			if (pHeadUnit == NULL || pHeadUnit->isDelayedDeath())
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

	if (!bDead)
	{
		if (!isHuman() && !AI_isAwaitingContract())
		{
			bFollow = false;

			// if we not group attacking, then check for follow action
			if (!m_bGroupAttack)
			{
				pEntityNode = headUnitNode();

				while ((pEntityNode != NULL) && readyToMove(true))
				{
					pLoopUnit = ::getUnit(pEntityNode->m_data);
					pEntityNode = nextUnitNode(pEntityNode);

					if (pLoopUnit->canMove())
					{
						resetPath();

						if (pLoopUnit->AI_follow())
						{
							bFollow = true;
							break;
						}
					}
				}
			}

			if (doDelayedDeath())
			{
				bDead = true;
			}

			if (!bDead)
			{
				if (!bFollow && readyToMove(true))
				{
					pushMission(MISSION_SKIP);
				}
			}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      04/28/10                                jdog5000      */
/*                                                                                              */
/* Unit AI                                                                                      */
/************************************************************************************************/
			// AI should never put units to sleep, how does this ever happen?
			//FAssert( getHeadUnit()->isCargo() || getActivityType() != ACTIVITY_SLEEP );
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
		}
	}

	if (bDead)
	{
		return false;
	}

	return (isBusy() || isCargoBusy());
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

		FAssert(getOwnerINLINE() != NO_PLAYER);

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                      02/21/10                                jdog5000      */
	/*                                                                                              */
	/* Efficiency, Lead From Behind                                                                 */
	/************************************************************************************************/
		// From Lead From Behind by UncutDragon
	/* original code
		if (pPlot->getBestDefender(NO_PLAYER, getOwnerINLINE(), NULL, !bPotentialEnemy, bPotentialEnemy) == NULL)
	*/	// modified
		if (!pPlot->hasDefender(false, NO_PLAYER, getOwnerINLINE(), getHeadUnit(), !bPotentialEnemy, bPotentialEnemy))//disagrees with earlier count
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                       END                                                  */
	/************************************************************************************************/
		{
			return 100;
		}
		
		//	Initialize predicted health for all units involved
		CLLNode<IDInfo>* pUnitNode;
		CvUnit* pLoopUnit;
		int iStartAttackerMaxStrength = 0;
		int iNumDefenders = 0;
		bool bAttackingGroupHasAlwaysHostileUnits = false;

#ifdef _DEBUG
		char buffer[300];
#endif

		pUnitNode = headUnitNode();
				
		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			pLoopUnit->AI_setPredictedHitPoints(-1);

#ifdef _DEBUG
			sprintf(buffer,"Attacker id %d has start cur HP %d\n",pLoopUnit->getID(),pLoopUnit->currHitPoints());
			OutputDebugString(buffer);
#endif

			if ( pLoopUnit->currHitPoints() > 0 )
			{
				int iStr = pLoopUnit->maxCombatStr(pPlot, pLoopUnit);

				iStartAttackerMaxStrength += iStr;	//	Really want a more general valuation here that accounts
													//	for all promotions, not just those in action at this plot

#ifdef _DEBUG
				sprintf(buffer,"Attacker id %d has start str %d\n",pLoopUnit->getID(),iStr);
				OutputDebugString(buffer);
#endif

				pLoopUnit->AI_setPredictedHitPoints(pLoopUnit->currHitPoints());

				if ( pLoopUnit->isAlwaysHostile(pPlot) )
				{
					bAttackingGroupHasAlwaysHostileUnits = true;
				}
			}
		}

		//	Similarly for the defenders
		int iStartDefenderMaxStrength = 0;
		pUnitNode = pPlot->headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if (!(pLoopUnit->isInvisible(getTeam(), false)))
			{
				//	If we have always hostile untis attacking then any unit that isn't the same team is a target
				if ((bAttackingGroupHasAlwaysHostileUnits ? (pLoopUnit->getTeam() != getTeam()) : ::isPotentialEnemy(getTeam(),GET_PLAYER(pLoopUnit->getCombatOwner(getTeam(), pPlot)).getTeam())) &&
					pLoopUnit->currHitPoints() > 0)
				{
					pLoopUnit->AI_setPredictedHitPoints(-1);

					int iStr = pLoopUnit->maxCombatStr(pPlot, pLoopUnit);

					iStartDefenderMaxStrength += iStr;	//	Really want a more general valuation here that accounts
														//	for all promotions, not just those in action at this plot

#ifdef _DEBUG
					sprintf(buffer,"Defender id %d has start str %d\n",pLoopUnit->getID(),iStr);
					OutputDebugString(buffer);
#endif

					pLoopUnit->AI_setPredictedHitPoints(pLoopUnit->currHitPoints());
					
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
		int iOdds;
		//int iLastBattleOdds = 0;
		CvUnit* pLastDefender = NULL;
		CvUnit* pLastAttacker = NULL;
		int	iLastOdds = 0;
		CvUnit* pDefender = NULL;

		do
		{
			iOdds = 0;
			pAttacker = AI_getBestGroupAttacker(pPlot, bPotentialEnemy, iOdds, bForce, false, &pDefender);

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

			pUnitNode = headUnitNode();
					
			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

#ifdef _DEBUG
				sprintf(buffer,"Attacker id %d has end cur HP %d, predicted %d\n",pLoopUnit->getID(),pLoopUnit->currHitPoints(),pLoopUnit->AI_getPredictedHitPoints());
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
			pUnitNode = pPlot->headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

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
			MEMORY_TRACK_EXEMPT();

			(*g_attackOddsCache)[pPlot] = (iResult & ODDS_CACHE_VALUE_MASK) + (bIsWin ? ODDS_CACHE_WIN_BIT : 0);
		}
	}

	return iResult;
}

static bool isClearlySuperior(CvUnit* pUnit, CvUnit* pOtherUnit, const CvPlot* pPlot)
{
	PROFILE_FUNC();

	int	iValue = pUnit->currEffectiveStr(pPlot,pUnit);
	int	iOtherValue = pOtherUnit->currEffectiveStr(pPlot,pOtherUnit);
	int	iTotalCombatMods = 0;
	int	iOtherTotalCombatMods = 0;

	if ( iValue < iOtherValue )
	{
		return false;
	}

	//	First strikes
	if ( pUnit->getCombatFirstStrikes() < pOtherUnit->getCombatFirstStrikes() )
	{
		return false;
	}

	//	Unit combat modifiers
	CvUnitInfo& kUnit = pUnit->getUnitInfo();
	CvUnitInfo& kOtherUnit = pOtherUnit->getUnitInfo();
	int numUnitCombatInfos = GC.getNumUnitCombatInfos();
	for(int iJ = 0; iJ < numUnitCombatInfos; iJ++)
	{
		iTotalCombatMods += kUnit.getUnitCombatModifier(iJ);
		iOtherTotalCombatMods += kOtherUnit.getUnitCombatModifier(iJ);
	}

	for (std::map<UnitCombatTypes, UnitCombatKeyedInfo>::const_iterator it = pUnit->getUnitCombatKeyedInfo().begin(), end = pUnit->getUnitCombatKeyedInfo().end(); it != end; ++it)
	{
		iTotalCombatMods += it->second.m_iExtraUnitCombatModifier;
	}

	if (!pUnit->isCommander())
	{
		CvUnit* pCommander = pUnit->getCommander();
		if (pCommander != NULL)
		{
			for (std::map<UnitCombatTypes, UnitCombatKeyedInfo>::const_iterator it = pCommander->getUnitCombatKeyedInfo().begin(), end = pCommander->getUnitCombatKeyedInfo().end(); it != end; ++it)
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
		CvUnit* pOtherCommander = pOtherUnit->getCommander();
		if (pOtherCommander != NULL)
		{
			for (std::map<UnitCombatTypes, UnitCombatKeyedInfo>::const_iterator it = pOtherCommander->getUnitCombatKeyedInfo().begin(), end = pOtherCommander->getUnitCombatKeyedInfo().end(); it != end; ++it)
			{
				iOtherTotalCombatMods += it->second.m_iExtraUnitCombatModifier;
			}
		}
	}

	return (iTotalCombatMods >= iOtherTotalCombatMods);
}

CvUnit* CvSelectionGroupAI::AI_getBestGroupAttacker(const CvPlot* pPlot, bool bPotentialEnemy, int& iUnitOdds, bool bForce, bool bNoBlitz, CvUnit** ppDefender, bool bAssassinate, bool bSuprise) const
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvUnit* pBestUnit;
	int iPossibleTargets;
	int iValue;
	int iBestValue;
	int iOdds;
	int iBestOdds;

	iBestValue = 0;
	iBestOdds = 0;
	pBestUnit = NULL;

	pUnitNode = headUnitNode();

	bool bIsHuman = (pUnitNode != NULL) ? GET_PLAYER(::getUnit(pUnitNode->m_data)->getOwnerINLINE()).isHuman() : true;
			
	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (!pLoopUnit->isDead())
		{
			if ( pBestUnit != NULL && isClearlySuperior(pBestUnit, pLoopUnit, pPlot) )
			{
				continue;
			}

			bool bCanAttack = false;
			if (pLoopUnit->getDomainType() == DOMAIN_AIR)
			{
				bCanAttack = pLoopUnit->canAirAttack();
			}
			else
			{
				bCanAttack = pLoopUnit->canAttack();

				if (bCanAttack && bNoBlitz && pLoopUnit->isBlitz() && pLoopUnit->isMadeAttack())
				{
					bCanAttack = false;
				}
			}

			if (bCanAttack && (!pLoopUnit->AI_getHasAttacked()|| bSuprise))
			{
				if (bForce || pLoopUnit->canMove())
				{
					CvUnit* pBestDefender = NULL;
					if ( bForce || pLoopUnit->canMoveInto(pPlot, /*bAttack*/ true, /*bDeclareWar*/ bPotentialEnemy, false, false, false, false, &pBestDefender, true, bAssassinate, bSuprise))
					{
						PROFILE("AI_getBestGroupAttacker.RegularAttackOdds");

						if (pBestDefender)
						{
							iOdds = pLoopUnit->AI_attackOddsAtPlot(pPlot, (CvUnitAI*)pBestDefender);
						}
						else
						{
							iOdds = pLoopUnit->AI_attackOdds(pPlot, bPotentialEnemy, 0, bAssassinate);
						}
						
							
						iValue = iOdds;
						FAssertMsg(iValue > 0, "iValue is expected to be greater than 0");
	
						if (pLoopUnit->collateralDamage() > 0)
						{
							iPossibleTargets = std::min((pPlot->getNumVisiblePotentialEnemyDefenders(pLoopUnit) - 1), pLoopUnit->collateralDamageMaxUnits());
	
							if (iPossibleTargets > 0)
							{
								iValue *= (100 + ((pLoopUnit->collateralDamage() * iPossibleTargets) / 5));
								iValue /= 100;
							}
						}
	
						// if non-human, prefer the last unit that has the best value (so as to avoid splitting the group)
						if (iValue > iBestValue || (!bIsHuman && iValue > 0 && iValue == iBestValue))
						{
							iBestValue = iValue;
							iBestOdds = iOdds;
							pBestUnit = pLoopUnit;
						}
					}
				}
			}
		}
	}

	if ( ppDefender != NULL && pBestUnit != NULL )
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

CvUnit* CvSelectionGroupAI::AI_getBestGroupSacrifice(const CvPlot* pPlot, bool bPotentialEnemy, bool bForce, bool bNoBlitz, bool bSuprise) const
{
	int iBestValue = 0;
	CvUnit* pBestUnit = NULL;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (!pLoopUnit->isDead())
		{
			bool bCanAttack = false;
			if (pLoopUnit->getDomainType() == DOMAIN_AIR)
			{
				bCanAttack = pLoopUnit->canAirAttack();
			}
			else
			{
				bCanAttack = pLoopUnit->canAttack();

				if (bCanAttack && bNoBlitz && pLoopUnit->isBlitz() && pLoopUnit->isMadeAttack())
				{
					bCanAttack = false;
				}
			}

			if (bCanAttack)
			{
				if (bForce || pLoopUnit->canMove())
				{
					if (bForce || pLoopUnit->canMoveInto(pPlot, true))
					{
						int iValue = pLoopUnit->AI_sacrificeValue(pPlot);
						FAssertMsg(iValue >= 0, "iValue is expected to be greater than 0");

						// we want to pick the last unit of highest value, so pick the last unit with a good value
						if (iValue > 0 && iValue >= iBestValue)
						{
							iBestValue = iValue;
							pBestUnit = pLoopUnit;
						}
					}
				}
			}
		}
	}

	return pBestUnit;
}

// Returns ratio of strengths of stacks times 100
// (so 100 is even ratio, numbers over 100 mean this group is more powerful than the stack on a plot)
int CvSelectionGroupAI::AI_compareStacks(const CvPlot* pPlot, bool bPotentialEnemy, bool bCheckCanAttack, bool bCheckCanMove, int iRange) const
{
	PROFILE_FUNC();

	FAssert(pPlot != NULL);

	int	compareRatio;
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
	compareRatio = AI_sumStrength(pPlot, eDomainType, bCheckCanAttack, bCheckCanMove);
	if (compareRatio >= (MAX_INT/100))
	{
		compareRatio /= 100;
		bDivideFirst = true;
	}
	compareRatio *= 100;

	PlayerTypes eOwner = getOwnerINLINE();
	if (eOwner == NO_PLAYER)
	{
		eOwner = getHeadOwner();
	}
	FAssert(eOwner != NO_PLAYER);
	
/************************************************************************************************/
/* UNOFFICIAL_PATCH                       03/04/10                                jdog5000      */
/*                                                                                              */
/* Bugfix                                                                                       */
/************************************************************************************************/
/* original bts code
	int defenderSum = pPlot->AI_sumStrength(NO_PLAYER, getOwnerINLINE(), eDomainType, true, !bPotentialEnemy, bPotentialEnemy);
*/
	// Clearly meant to use eOwner here ...
	//	Koshling - changed final param from 'bPotentialEnemy' to '!bPotentialEnemy' since the param is whether
	//	to test, and the value we have here is an assertion.  This bug was causing the AI to send inadequate stacks
	//	against cities of players it was not YET at war with and causing a premature war
	int defenderSum = pPlot->AI_sumStrength(NO_PLAYER, eOwner, eDomainType, true, !bPotentialEnemy, !bPotentialEnemy, iRange);
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
	if (bDivideFirst)
	{
		defenderSum /= 100;
	}
	compareRatio /= std::max(1, defenderSum);

	return compareRatio;
}

int CvSelectionGroupAI::AI_sumStrength(const CvPlot* pAttackedPlot, DomainTypes eDomainType, bool bCheckCanAttack, bool bCheckCanMove) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	unsigned long long strSum = 0;
	int iBaseCollateral = GC.getDefineINT("COLLATERAL_COMBAT_DAMAGE"); // K-Mod. (currently this number is "10")

	pUnitNode = headUnitNode();

	CvUnit* pInitialUnit = ::getUnit(pUnitNode->m_data);
	int iNumPotentialDefenders = pAttackedPlot->getNumVisiblePotentialEnemyDefenders(pInitialUnit) - 1;
	while (pUnitNode != NULL && strSum <= MAX_INT)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (!pLoopUnit->isDead())
		{
			bool bCanAttack = false;
			if (pLoopUnit->getDomainType() == DOMAIN_AIR)
				bCanAttack = pLoopUnit->canAirAttack();
			else
				bCanAttack = pLoopUnit->canAttack();

			if (!bCheckCanAttack || bCanAttack)
			{
				if (!bCheckCanMove || pLoopUnit->canMove())
				{
					//TB: canMoveInto may need simplified here somehow.
					if (!bCheckCanMove || pAttackedPlot == NULL || pLoopUnit->canMoveInto(pAttackedPlot, /*bAttack*/ true, /*bDeclareWar*/ true, false, false, true /*bIgnoreLocation*/))
					{
						if (eDomainType == NO_DOMAIN || pLoopUnit->getDomainType() == eDomainType)
						{
							//strSum += (unsigned long long)pLoopUnit->currEffectiveStr(pAttackedPlot, pLoopUnit);
							//TB Simplify for speed:
							strSum += (unsigned long long)pLoopUnit->currCombatStr(NULL,NULL);
							// K-Mod estimate the attack power of collateral units 
							if (pLoopUnit->collateralDamage() > 0 && pAttackedPlot != plot()) 
							{ 
								int iPossibleTargets = std::min(iNumPotentialDefenders, pLoopUnit->collateralDamageMaxUnits()); 

								if (iPossibleTargets > 0) 
								{ 
									// collateral damage is not trivial to calculate. This estimate is pretty rough. 
									strSum += (unsigned long long)pLoopUnit->baseCombatStrNonGranular() * iBaseCollateral * pLoopUnit->collateralDamage() * iPossibleTargets / 100;
								} 
							} 
							// K-Mod end 
						}
					}
				}
			}
		}
	}
	int iTotal = 0;
	if (strSum > MAX_INT)
	{
		iTotal = MAX_INT;
	}
	else
	{
		iTotal = (int)strSum;
	}

	return iTotal;
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

inline bool CvSelectionGroupAI::AI_isGroupAttack()
{
	return m_bGroupAttack;
}

bool CvSelectionGroupAI::AI_isControlled()
{
	return (!isHuman() || isAutomated());
}


bool CvSelectionGroupAI::AI_isDeclareWar(const CvPlot* pPlot)
{
	FAssert(getHeadUnit() != NULL);

	if (isHuman())
	{
		return false;
	}
	else
	{
		bool bLimitedWar = false;
		if (pPlot != NULL)
		{
			TeamTypes ePlotTeam = pPlot->getTeam();
			if (ePlotTeam != NO_TEAM)
			{
				WarPlanTypes eWarplan = GET_TEAM(getTeam()).AI_getWarPlan(ePlotTeam);
				if (eWarplan == WARPLAN_LIMITED)
				{
					bLimitedWar = true;
				}
			}
		}

		CvUnit* pHeadUnit = getHeadUnit();

		if (pHeadUnit != NULL)
		{
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
				FAssert(false);
				break;
			}
		}
	}

	return false;
}


CvPlot* CvSelectionGroupAI::AI_getMissionAIPlot()
{
	return GC.getMapINLINE().plotSorenINLINE(m_iMissionAIX, m_iMissionAIY);
}


bool CvSelectionGroupAI::AI_isForceSeparate()
{
	return m_bForceSeparate;
}


void CvSelectionGroupAI::AI_makeForceSeparate()
{
	m_bForceSeparate = true;
}


MissionAITypes CvSelectionGroupAI::AI_getMissionAIType()
{
	return m_eMissionAIType;
}

void CvSelectionGroupAI::AI_noteSizeChange(int iChange, int iVolume)
{
	if ( m_eMissionAIType != NO_MISSIONAI )
	{
		CvPlot* pPlot = AI_getMissionAIPlot();

		if ( pPlot != NULL )
		{
			GET_PLAYER(getOwnerINLINE()).AI_noteMissionAITargetCountChange(m_eMissionAIType, pPlot, iChange, NULL, iVolume);
		}
	}
}

void CvSelectionGroupAI::AI_setMissionAI(MissionAITypes eNewMissionAI, CvPlot* pNewPlot, CvUnit* pNewUnit)
{
	PROFILE_FUNC();

	if ( m_eMissionAIType != NO_MISSIONAI )
	{
		CvPlot* pPlot = AI_getMissionAIPlot();

		if ( pPlot != NULL )
		{
			GET_PLAYER(getOwnerINLINE()).AI_noteMissionAITargetCountChange(m_eMissionAIType, pPlot, -getNumUnits(), plot(), -getNumUnitCargoVolumeTotal());
		}
	}

	m_eMissionAIType = eNewMissionAI;

	if (pNewPlot != NULL)
	{
		m_iMissionAIX = pNewPlot->getX_INLINE();
		m_iMissionAIY = pNewPlot->getY_INLINE();
	}
	else
	{
		m_iMissionAIX = INVALID_PLOT_COORD;
		m_iMissionAIY = INVALID_PLOT_COORD;
	}

	if (pNewUnit != NULL)
	{
		m_missionAIUnit = pNewUnit->getIDInfo();
	}
	else
	{
		m_missionAIUnit.reset();
	}

	if ( m_eMissionAIType != NO_MISSIONAI )
	{
		CvPlot* pPlot = AI_getMissionAIPlot();

		if ( pPlot != NULL )
		{
			GET_PLAYER(getOwnerINLINE()).AI_noteMissionAITargetCountChange(m_eMissionAIType, pPlot, getNumUnits(), plot(), getNumUnitCargoVolumeTotal());
		}
	}
}


CvUnit* CvSelectionGroupAI::AI_getMissionAIUnit()
{
	return getUnit(m_missionAIUnit);
}

bool CvSelectionGroupAI::AI_isFull()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	if (getNumUnits() > 0)
	{
		UnitAITypes eUnitAI = getHeadUnitAI();
		// do two passes, the first pass, we ignore units with speical cargo
		int iSpecialCargoCount = 0;
		int iCargoCount = 0;
		
		// first pass, count but ignore special cargo units
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);
			if (pLoopUnit->AI_getUnitAIType() == eUnitAI)
			{
				if (pLoopUnit->cargoSpace() > 0 || pLoopUnit->SMcargoSpace() > 0)
				{
					iCargoCount++;
				}

				if (pLoopUnit->specialCargo() != NO_SPECIALUNIT)
				{
					iSpecialCargoCount++;
				}
				else if (!(pLoopUnit->isFull()))
				{
					return false;
				}
			}
		}
		
		// if every unit in the group has special cargo, then check those, otherwise, consider ourselves full
		if (iSpecialCargoCount >= iCargoCount)
		{
			pUnitNode = headUnitNode();
			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);
				
				if (pLoopUnit->AI_getUnitAIType() == eUnitAI)
				{
					if (!(pLoopUnit->isFull()))
					{
						return false;
					}
				}
			}
		}

		return true;
	}

	return false;	
}

int CvSelectionGroupAI::AI_getGenericValueTimes100(UnitValueFlags eFlags) const
{
	CLLNode<IDInfo>* pEntityNode;
	CvUnit* pLoopUnit;

	pEntityNode = headUnitNode();
	
	int iResult = 0;

	while (pEntityNode != NULL)
	{
		pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);

		iResult += pLoopUnit->AI_genericUnitValueTimes100(eFlags);
	}

	return iResult;
}

bool CvSelectionGroupAI::AI_hasBeneficialPropertyEffectForCity(CvCity* pCity) const
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	CvUnit* pLoopUnit;

	FAssertMsg(pUnitNode != NULL, "headUnitNode() expected to be non-NULL");

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if ( pLoopUnit->AI_beneficialPropertyValueToCity(pCity, NO_PROPERTY) > 0 )
		{
			return true;
		}
	}

	return false;
}

CvUnit* CvSelectionGroupAI::AI_ejectBestPropertyManipulator(CvCity* pTargetCity)
{
	CLLNode<IDInfo>* pEntityNode;
	CvUnit* pLoopUnit;

	pEntityNode = headUnitNode();
	
	CvUnit* pBestUnit = NULL;
	int iBestUnitValue = 0;

	while (pEntityNode != NULL)
	{
		pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);
		
		int iValue = pLoopUnit->AI_beneficialPropertyValueToCity(pTargetCity, NO_PROPERTY);

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

CvUnit* CvSelectionGroupAI::AI_findBestDefender(CvPlot* pDefendPlot, bool allowAllDefenders, bool bConsiderPropertyValues)
{
	CLLNode<IDInfo>* pEntityNode;
	CvUnit* pLoopUnit;

	pEntityNode = headUnitNode();
	
	CvUnit* pBestUnit = NULL;
	int iBestUnitValue = 0;

	while (pEntityNode != NULL)
	{
		pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);
		
		if (!pLoopUnit->noDefensiveBonus() || (allowAllDefenders && pLoopUnit->canDefend()))
		{
			int iPropertyValue = 0;
			int iValue = pLoopUnit->currEffectiveStr(pDefendPlot, NULL) * 100;

			if (pDefendPlot->isCity(true, getTeam()))
			{
				if ( bConsiderPropertyValues )
				{
					iPropertyValue = GET_PLAYER(getOwnerINLINE()).AI_unitPropertyValue(pLoopUnit->getUnitType())*200;
					iValue += iPropertyValue;
				}

				iValue *= 100 + pLoopUnit->cityDefenseModifier();

				iValue /= 100;
			}
			
			if (plot()->isCity(true, getTeam()))
			{
				if ( bConsiderPropertyValues )
				{
					iPropertyValue = GET_PLAYER(getOwnerINLINE()).AI_unitPropertyValue(pLoopUnit->getUnitType())*200;
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

CvUnit* CvSelectionGroupAI::AI_ejectBestDefender(CvPlot* pDefendPlot, bool allowAllDefenders)
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
	CvUnit* pHeadUnit = getHeadUnit();

	if ( pHeadUnit != NULL )
	{
		return pHeadUnit->AI_isCityGarrison(pCity);
	}
	else
	{
		return false;
	}
}

void CvSelectionGroupAI::AI_setAsGarrison(const CvCity* pCity)
{
	CvUnit* pHeadUnit = getHeadUnit();

	if ( pHeadUnit != NULL )
	{
		pHeadUnit->AI_setAsGarrison(pCity);
	}
}


// Protected Functions...

void CvSelectionGroupAI::read(FDataStreamBase* pStream)
{
	CvSelectionGroup::read(pStream);

	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

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

	uint uiFlag=0;
	pStream->Write(uiFlag);		// flag for expansion

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

bool	CvSelectionGroupAI::AI_isAwaitingContract() const
{
	return (getHeadUnit() != NULL && getHeadUnit()->AI_isAwaitingContract());
}

// Private Functions...
