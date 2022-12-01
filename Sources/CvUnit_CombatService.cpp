#include "CvUnit_CombatService.h"

#include "CvCity.h"
#include "CvUnit.h"


using namespace CvUnitNS;

CombatService::CombatService(CvUnit* unit, bool bQuick, CvUnit* pSelectedDefender, bool bSamePlot, 
	bool bStealth, bool bNoCache) :

	m_unit(unit),
	m_bQuick(bQuick),
	m_selectedDefender(pSelectedDefender),
	m_bSamePlot(bSamePlot),
	m_bStealth(bStealth),
	m_bNoCache(bNoCache),

	m_bHuman(false),
	m_bHumanDefender(false),
	m_bVisible(false),
	m_eAttacker(NO_PLAYER),
	m_eDefender(NO_PLAYER),
	m_bStealthDefense(false)
{
}

void CombatService::updateCombat()
{
	PROFILE_FUNC();

	init();

	//if not finished and not fighting yet, set up combat damage and mission
	if (!m_bFinish)
	{
		bool bAbort;
		fight(bAbort);
		if (bAbort) return;
	}

	if (m_bFinish)
	{
		PROFILE("CvUnitNS::CombatService::updateCombat.Finish");

		finishingPrelude();

		if (m_unit->isDead())
			onLoss();
		else if (m_defender->isDead())
			onWin();
		else if (m_unit->m_combatResult.bDefenderWithdrawn)
			onDefenderWithraw();
		//TB Combat Mods
		//Repel
		else if (m_unit->m_combatResult.bAttackerRepelled)
			onAttackerRepelled();
		//Knockback
		else if (m_unit->m_combatResult.bDefenderKnockedBack)
			onDefenderKnockedBack();
		else if (m_unit->m_combatResult.bAttackerWithdraws)
			onAttackerWithraw();
		else
			onOtherResult();

		if (m_unit->IsSelected() && !m_unit->canMove())
		{
			GC.getGame().updateSelectionListInternal();
		}
	}
}

void CombatService::init()
{
	/*GC.getGame().logOOSSpecial(6, getID(), m_unit->getDamage());*/

	if (m_bStealth)
	{
		m_bNoCache = true;
	}

	m_bFinish = false;

	if (m_unit->getCombatTimer() > 0)
	{
		m_unit->changeCombatTimer(-1);
		if (m_unit->getCombatTimer() > 0)
		{
			/*GC.getGame().logOOSSpecial(7, getID(), m_unit->getDamage());*/
			return;
		}
		m_bFinish = true;
	}

	m_plot = m_unit->getAttackPlot();

	if (m_plot == NULL)
	{
		/*GC.getGame().logOOSSpecial(8, getID(), m_unit->getDamage());*/
		return;
	}

	if (m_unit->getDomainType() == DOMAIN_AIR)
	{
		m_unit->updateAirStrike(m_plot, m_bQuick, m_bFinish);
		/*GC.getGame().logOOSSpecial(9, getID(), m_unit->getDamage());*/
		return;
	}

	m_defender = NULL;
	if (m_bFinish)
	{
		m_defender = m_unit->getCombatUnit();
	}
	else if (m_selectedDefender == NULL)
	{
		m_defender = m_plot->getBestDefender(NO_PLAYER, m_unit->getOwner(), m_unit, true, false, false, false, m_bNoCache);
	}
	else
	{
		m_defender = m_selectedDefender;
	}

	if (m_defender == NULL)
	{
		m_unit->setAttackPlot(NULL, false);
		m_unit->setCombatUnit(NULL);

		if (!m_bSamePlot)
		{
			m_unit->getGroup()->groupMove(m_plot, true, ((m_unit->canAdvance(m_plot, 0)) ? m_unit : NULL));
		}

		m_unit->getGroup()->clearMissionQueue();

		/*GC.getGame().logOOSSpecial(10, getID(), m_unit->getDamage());*/
		return;
	}
	(bool&)m_bHuman = m_unit->isHuman();
	(bool&)m_bHumanDefender = m_defender->isHuman();

	//check if quick combat
	(bool&)m_bVisible = !m_bQuick && !m_bSamePlot && m_unit->isCombatVisible(m_defender);

	//FAssertMsg((pPlot == pDefender->plot()), "There is not expected to be a defender or the defender's plot is expected to be pPlot (the attack plot)");

	(bool&)m_eAttacker = m_unit->getVisualOwner(m_defender->getTeam());
	(bool&)m_eDefender = m_defender->getVisualOwner(m_unit->getTeam());
}

void CombatService::fight(bool& bAbort)
{
	bAbort = false;

	if (!m_unit->isFighting())
	{
		PROFILE("CvUnitNS::CombatService::updateCombat.StartFight");

		if (!m_bStealth && (m_unit->plot()->isFighting() || m_plot->isFighting()))
		{
			/*GC.getGame().logOOSSpecial(11, getID(), m_unit->getDamage());*/
			bAbort = true;
			return;
		}
		//TB Combat Mods (Att&DefCounters)
		if (m_unit->getRoundCount() > 0)
		{
			m_unit->changeRoundCount(-m_unit->getRoundCount());
		}
		if (m_defender->getRoundCount() > 0)
		{
			m_defender->changeRoundCount(-m_defender->getRoundCount());
		}
		m_unit->changeAttackCount(1);
		//TB Combat Mods end
		if (!m_bSamePlot)
		{
			//rotate to face plot
			DirectionTypes newDirection = estimateDirection(m_unit->plot(), m_defender->plot());
			if (newDirection != NO_DIRECTION)
			{
				m_unit->setFacingDirection(newDirection);
			}

			//rotate enemy to face us
			newDirection = estimateDirection(m_defender->plot(), m_unit->plot());
			if (newDirection != NO_DIRECTION)
			{
				m_defender->setFacingDirection(newDirection);
			}
		}

		const bool bStealthAttack = m_unit->isInvisible(GET_PLAYER(m_defender->getOwner()).getTeam(), false, false) || m_defender->plot() == m_unit->plot();
		if (bStealthAttack)
		{
			if (m_bHuman)
			{
				AddDLLMessage(
					m_unit->getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_STEALTH_ATTACK_OWNER", m_unit->getNameKey(), GET_PLAYER(m_eDefender).getNameKey(), m_defender->getNameKey()),
					"AS2D_EXPOSED", MESSAGE_TYPE_MINOR_EVENT, m_unit->getButton(), GC.getCOLOR_UNIT_TEXT(), m_unit->getX(), m_unit->getY(), true, true
				);
			}
			if (m_bHumanDefender)
			{
				AddDLLMessage(
					m_defender->getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_STEALTH_ATTACK", GET_PLAYER(m_eAttacker).getNameKey(), m_unit->getNameKey(), m_defender->getNameKey()),
					"AS2D_EXPOSED", MESSAGE_TYPE_MINOR_EVENT, m_unit->getButton(), GC.getCOLOR_UNIT_TEXT(), m_unit->getX(), m_unit->getY(), true, true
				);
			}
		}
		bool bStealthDefense = false;
		if (bStealthAttack || m_bStealth)
		{
			const bool bLieInWait = !m_unit->isInvisible(GET_PLAYER(m_defender->getOwner()).getTeam(), false, false) && m_defender->plot() == m_unit->plot();

			bStealthDefense = m_bStealth || bLieInWait;
			if (bStealthDefense && bLieInWait)
			{

				if (m_bHumanDefender)
				{
					AddDLLMessage(
						m_defender->getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_STEALTH_LIE_IN_WAIT_OWNER", m_defender->getNameKey(), GET_PLAYER(m_unit->getOwner()).getNameKey(), m_unit->getNameKey()),
						"AS2D_EXPOSED", MESSAGE_TYPE_MINOR_EVENT, m_unit->getButton(), GC.getCOLOR_UNIT_TEXT(), m_unit->getX(), m_unit->getY(), true, true
					);
				}
				if (m_bHuman)
				{
					AddDLLMessage(
						m_unit->getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_STEALTH_LIE_IN_WAIT", GET_PLAYER(m_eAttacker).getNameKey(), m_unit->getNameKey(), m_defender->getNameKey()),
						"AS2D_EXPOSED", MESSAGE_TYPE_MINOR_EVENT, m_unit->getButton(), GC.getCOLOR_UNIT_TEXT(), m_unit->getX(), m_unit->getY(), true, true
					);
				}
			}
		}
		m_unit->m_combatResult.bStealthDefense = bStealthDefense && !(m_defender->plot() == m_unit->plot());//Note m_unit information is transferred to bStealthDefense during bFinish routine to help define whether units should lose movement or not.
		//TBMaybeproblem : I'd like to elminate m_unit from attackers in a stealth defense situation - they shouldn't be counted as having attacked for being ambushed.
		if (!m_unit->canStampede() && !m_unit->canOnslaught() && !bStealthDefense)
		{
			m_unit->setMadeAttack(true);
		}
		m_unit->setCombatUnit(m_defender, true, bStealthAttack, bStealthDefense);
		m_defender->setCombatUnit(m_unit, false, bStealthAttack, bStealthDefense);
		//TB Combat Mods (Att&DefCounters)
		m_defender->changeDefenseCount(1);
		//TB Combat Mods end

		m_defender->getGroup()->clearMissionQueue();

		bool bFocused = m_bVisible && m_unit->isCombatFocus() && gDLL->getInterfaceIFace()->isCombatFocus() && m_unit->plot()->isInViewport() && m_defender->isInViewport();
		if (bFocused)
		{ // TBMaybeproblem - is it possible that all m_unit should happen to setup the combat on a surprise defense?
			// It is not currently doing so, perhaps because of fear of the revealed m_unit not being visible yet?
			DirectionTypes directionType = directionXY(m_unit->plot(), m_plot);
			//								N			NE				E				SE					S				SW					W				NW
			NiPoint2 directions[8] = { NiPoint2(0, 1), NiPoint2(1, 1), NiPoint2(1, 0), NiPoint2(1, -1), NiPoint2(0, -1), NiPoint2(-1, -1), NiPoint2(-1, 0), NiPoint2(-1, 1) };
			NiPoint3 attackDirection = NiPoint3(directions[directionType].x, directions[directionType].y, 0);
			float plotSize = GC.getPLOT_SIZE();
			NiPoint3 lookAtPoint(m_unit->plot()->getPoint().x + plotSize / 2 * attackDirection.x, m_unit->plot()->getPoint().y + plotSize / 2 * attackDirection.y, (m_unit->plot()->getPoint().z + m_plot->getPoint().z) / 2);
			attackDirection.Unitize();
			gDLL->getInterfaceIFace()->lookAt(lookAtPoint, (((m_unit->getOwner() != GC.getGame().getActivePlayer()) || gDLL->getGraphicOption(GRAPHICOPTION_NO_COMBAT_ZOOM)) ? CAMERALOOKAT_BATTLE : CAMERALOOKAT_BATTLE_ZOOM_IN), attackDirection);
		}
		else if (m_bHumanDefender)
		{

			if (BARBARIAN_PLAYER != m_eAttacker)
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_YOU_UNITS_UNDER_ATTACK", GET_PLAYER(m_unit->getOwner()).getNameKey()),
					"AS2D_COMBAT", MESSAGE_TYPE_DISPLAY_ONLY, m_unit->getButton(), GC.getCOLOR_RED(), m_plot->getX(), m_plot->getY(), true
				);
			}
			else
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_YOU_UNITS_UNDER_ATTACK_UNKNOWN"),
					"AS2D_COMBAT", MESSAGE_TYPE_DISPLAY_ONLY, m_unit->getButton(), GC.getCOLOR_RED(), m_plot->getX(), m_plot->getY(), true
				);
			}
		}
		//Damage to Attacking Unit from City Defenses
		if (m_plot->isCity(false) && !m_bSamePlot)
		{
			std::vector<UnitCombatTypes> damagableUnitCombatTypes;

			CvCity* pCity = m_plot->getPlotCity();
			for (std::map<UnitCombatTypes, UnitCombatKeyedInfo>::iterator it = m_unit->getUnitCombatKeyedInfo().begin(), end = m_unit->getUnitCombatKeyedInfo().end(); it != end; ++it)
			{
				if (it->second.m_bHasUnitCombat && pCity->canDamageAttackingUnitCombat(it->first))
				{
					damagableUnitCombatTypes.push_back(it->first);
				}
			}
			if (damagableUnitCombatTypes.size() > 0)
			{
				m_unit->checkCityAttackDefensesDamage(pCity, damagableUnitCombatTypes);
			}
		}
	}
	FAssertMsg(m_defender != NULL, "Defender is not assigned a valid value");
	FAssertMsg(m_unit->plot()->isFighting(), "Current m_unit instance plot is not fighting as expected");
	FAssertMsg(m_plot->isFighting(), "pPlot is not fighting as expected");

	if (!m_defender->canDefend())
	{
		if (m_bVisible)
		{
			m_unit->addMission(CvMissionDefinition(MISSION_SURRENDER, m_plot, m_unit, m_defender, m_unit->getCombatTimer() * gDLL->getSecsPerTurn()));

			// Surrender mission
			m_unit->setCombatTimer(GC.getMissionInfo(MISSION_SURRENDER).getTime());

			GC.getGame().incrementTurnTimer(m_unit->getCombatTimer());
		}
		else m_bFinish = true;

		// Kill them!
		m_defender->setDamage(m_defender->getMaxHP());
	}
	//	Need to check the attacker has not already died in the attempt (on building defenses)
	else if (!m_unit->isDead())
	{
		PROFILE("CvUnitNS::CombatService::updateCombat.CanDefend");

		//USE commanders here (so their command points will be decreased) for attacker and defender:
		m_unit->tryUseCommander();
		m_defender->tryUseCommander();

		CvBattleDefinition kBattle(m_plot, m_unit, m_defender);

		//	Koshling - save pre-combat helath so we can use health loss as
		//	a basis for more granular war weariness
		m_unit->setupPreCombatDamage();
		m_defender->setupPreCombatDamage();

		m_unit->resolveCombat(m_defender, m_plot, kBattle, m_bSamePlot);

		if (!m_bVisible)
		{
			if (m_bHuman)
			{
				//	Hack to make quick offensive option not switch away from
				//	the stack.  It appears to be a bug in the main game engine
				//	in that it ALWAYS switches away unles you compleet the combat
				//	in a timer update call rather than directly here, so fake up
				//	a pseudo-combat round to perform delayed completion (but without
				//	animation, so no battle setup) via the m_unit timer
				m_unit->setCombatTimer(1);

				GC.getGame().incrementTurnTimer(m_unit->getCombatTimer());
				//TB: This is a fix for (standard bts) stack attack.  This 'hack' works as described above but if you have stack attack on,
				//it causes a situation where the tile is still considered to be in the middle of a battle after the battle has ended.
				//This will in effect disable stack attack and force each attack to be instructed individually as if the option was off.
				//Therefore, the bQuick definition that the groupAttack sends when the attack is one among a sequence
				//was modified to only send false IF it was the LAST attack in the stack attack sequence while
				//m_unit section was modified to ONLY turn bFinish true on quick attack if it is the last attack in the sequence, when,
				//at that point it is safe to process as the finish of the battle.
				if (m_bQuick || !GET_PLAYER(m_unit->getOwner()).isOption(PLAYEROPTION_QUICK_ATTACK))//TBFLAG
				{
					m_bFinish = true;
				}
			}
			else m_bFinish = true;
		}
		else
		{
			kBattle.setDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_END, m_unit->getDamage());
			kBattle.setDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_END, m_defender->getDamage());
			if (!m_bSamePlot)
			{
				kBattle.setAdvanceSquare(m_unit->canAdvance(m_plot, m_defender->isDead() ? 0 : 1));
			}

			if (m_unit->isRanged() && m_defender->isRanged())
			{
				kBattle.setDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_RANGED, kBattle.getDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_END));
				kBattle.setDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_RANGED, kBattle.getDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_END));
			}
			else
			{
				kBattle.addDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_RANGED, kBattle.getDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_BEGIN));
				kBattle.addDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_RANGED, kBattle.getDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_BEGIN));
			}

			const int iTurns = m_unit->planBattle(kBattle);
			kBattle.setMissionTime(iTurns * gDLL->getSecsPerTurn());
			m_unit->setCombatTimer(iTurns);
			//TB Debug: Without plot set, m_unit routine ended up causing a crash at addMission below.

			GC.getGame().incrementTurnTimer(m_unit->getCombatTimer());

			if (m_plot->isActiveVisible(false) && !m_defender->isUsingDummyEntities())
			{
				//TB sameplot?
				m_unit->ExecuteMove(0.5f, true);
				m_unit->addMission(kBattle);
			}
		}
	}
	else m_bFinish = true;	//Attacker died before it could even engage
}

void CombatService::finishingPrelude()
{
	(bool&)m_bStealthDefense = m_unit->m_combatResult.bStealthDefense; // m_bStealthDefense initialization

#ifdef STRENGTH_IN_NUMBERS
	//TB Combat Mod begin
	if (GC.getGame().isOption(GAMEOPTION_STRENGTH_IN_NUMBERS))
	{
		ClearSupports();
	}
#endif // STRENGTH_IN_NUMBERS
	if (m_bVisible && m_unit->isCombatFocus() && gDLL->getInterfaceIFace()->isCombatFocus() && m_unit->getOwner() == GC.getGame().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->releaseLockedCamera();
	}
#ifdef OUTBREAKS_AND_AFFLICTIONS
	//TB Combat Mod (Afflict) begin
	if (GC.getGame().isOption(GAMEOPTION_OUTBREAKS_AND_AFFLICTIONS))
	{
		for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
		{
			if (m_unit->m_combatResult.bAttackerInjured
			&&
			(
				m_defender->isDead()
				|| m_unit->m_combatResult.bDefenderWithdrawn
				|| m_unit->m_combatResult.bDefenderKnockedBack
				|| m_unit->m_combatResult.bAttackerRepelled
				|| m_unit->m_combatResult.bAttackerWithdraws
				)
			&& GC.getPromotionLineInfo((PromotionLineTypes)iI).isAffliction()
			&& !GC.getPromotionLineInfo((PromotionLineTypes)iI).isCritical())
			{
				const PromotionLineTypes eAfflictionLine = (PromotionLineTypes)iI;

				if (m_defender->hasAfflictOnAttackType(eAfflictionLine)
				&& !m_defender->isAfflictOnAttackTypeAttempted(eAfflictionLine)
				// Distance Qualifier
				&&
				(
					m_unit->m_combatResult.bDefenderHitAttackerWithDistanceAttack
					&& m_defender->isAfflictOnAttackTypeDistance(eAfflictionLine)
					|| m_defender->isAfflictOnAttackTypeMelee(eAfflictionLine)
					&& !m_unit->m_combatResult.bNeverMelee
					))
				{
					const int iDefendersPoisonChance =
						(
							m_defender->getAfflictOnAttackTypeProbability(eAfflictionLine)
							- m_unit->fortitudeTotal() - m_unit->getUnitAfflictionTolerance(eAfflictionLine)
						);
					if (GC.getGame().getSorenRandNum(100, "DefendersPoisonRoll") < iDefendersPoisonChance)
					{
						m_unit->afflict(eAfflictionLine, true, m_defender);
					}
					m_defender->setAfflictOnAttackTypeAttemptedCount(eAfflictionLine, 1);
				}
				//Battle proximity communicability affliction spread
				if (!m_unit->m_combatResult.bNeverMelee && m_defender->hasAfflictionLine(eAfflictionLine) && GC.getPromotionLineInfo(eAfflictionLine).getCommunicability() > 0 && !GC.getPromotionLineInfo(eAfflictionLine).isNoSpreadonBattle())
				{
					const int iDefendersChancetoAfflict = m_unit->getChancetoContract(eAfflictionLine) + m_defender->worsenedProbabilitytoAfflict(eAfflictionLine);

					if (GC.getGame().getSorenRandNum(100, "DefenderCauseContractRoll") < iDefendersChancetoAfflict)
					{
						m_unit->afflict(eAfflictionLine, false, m_defender);
					}
				}
			}
			if (m_unit->m_combatResult.bDefenderInjured
			&&
			(
				m_unit->isDead()
				|| m_unit->m_combatResult.bDefenderWithdrawn
				|| m_unit->m_combatResult.bDefenderKnockedBack
				|| m_unit->m_combatResult.bAttackerRepelled
				|| m_unit->m_combatResult.bAttackerWithdraws
				)
			&& GC.getPromotionLineInfo((PromotionLineTypes)iI).isAffliction()
			&& !GC.getPromotionLineInfo((PromotionLineTypes)iI).isCritical())
			{
				const PromotionLineTypes eAfflictionLine = (PromotionLineTypes)iI;
				if (m_unit->hasAfflictOnAttackType(eAfflictionLine)
				&& !m_unit->isAfflictOnAttackTypeAttempted(eAfflictionLine)
				&&
				(
					m_unit->m_combatResult.bDefenderHitAttackerWithDistanceAttack
					&& m_unit->isAfflictOnAttackTypeDistance(eAfflictionLine)
					|| m_unit->isAfflictOnAttackTypeMelee(eAfflictionLine)
					&& !m_unit->m_combatResult.bNeverMelee
					))
				{
					const int iAttackersPoisonChance =
						(
							m_unit->getAfflictOnAttackTypeProbability(eAfflictionLine)
							- m_defender->fortitudeTotal()
							- m_defender->getUnitAfflictionTolerance(eAfflictionLine)
						);
					if (GC.getGame().getSorenRandNum(100, "AttackersPoisonRoll") < iAttackersPoisonChance)
					{
						m_defender->afflict(eAfflictionLine, true, m_unit);
					}
					m_unit->setAfflictOnAttackTypeAttemptedCount(eAfflictionLine, 1);
				}
				//Communicability exposure
				if (!m_unit->m_combatResult.bNeverMelee && m_unit->hasAfflictionLine(eAfflictionLine) && GC.getPromotionLineInfo(eAfflictionLine).getCommunicability() > 0 && !GC.getPromotionLineInfo(eAfflictionLine).isNoSpreadonBattle())
				{
					const int iAttackersChancetoAfflict = m_defender->getChancetoContract(eAfflictionLine) + m_unit->worsenedProbabilitytoAfflict(eAfflictionLine);

					if (GC.getGame().getSorenRandNum(100, "AttackerCauseContractRoll") < iAttackersChancetoAfflict)
					{
						m_defender->afflict(eAfflictionLine, false, m_unit);
					}
				}
			}
		}
	}
	//TB Combat Mod (Afflict) end
#endif // OUTBREAKS_AND_AFFLICTIONS

		//TB Combat Mod (Stampede/Onslaught)
	if (m_defender->isDead() || m_unit->m_combatResult.bDefenderWithdrawn || m_unit->m_combatResult.bDefenderKnockedBack || m_unit->m_combatResult.bAttackerRepelled || m_unit->m_combatResult.bAttackerWithdraws)
	{
		if (!m_bSamePlot && m_unit->canStampede() && m_plot->getNumVisiblePotentialEnemyDefenders(m_unit) > 1)
		{
			m_unit->m_combatResult.bAttackerStampedes = true;
		}
		if (!m_bSamePlot && m_unit->canOnslaught() && (m_unit->getDamage() == 0) && m_plot->getNumVisiblePotentialEnemyDefenders(m_unit) > 1)
		{
			m_unit->m_combatResult.bAttackerOnslaught = true;
		}
	}
	if (m_unit->isDead() || m_defender->isDead() || m_unit->m_combatResult.bDefenderWithdrawn || m_unit->m_combatResult.bDefenderKnockedBack || m_unit->m_combatResult.bAttackerRepelled || m_unit->m_combatResult.bAttackerWithdraws)
	{
		//Sustain Reports
		//Pursuit Reports
		if (m_unit->m_combatResult.bAttackerPursuedSustain)
		{
			if (m_bHuman)
			{

				if (BARBARIAN_PLAYER != m_eDefender)
				{
					AddDLLMessage(
						m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_PURSUED_SUSTAIN_HUMAN", m_unit->getNameKey(), m_defender->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_PURSUED_SUSTAIN_HUMAN_HIDDEN", m_unit->getNameKey(), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			if (m_bHumanDefender)
			{

				if (BARBARIAN_PLAYER != m_eAttacker)
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_PURSUED_SUSTAIN", m_unit->getVisualCivAdjective(m_unit->getTeam()), m_unit->getNameKey(), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_PURSUED_SUSTAIN_HIDDEN", m_unit->getNameKey(), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			m_unit->m_combatResult.bAttackerPursuedSustain = false;
		}
		if (m_unit->m_combatResult.bDefenderPursuedSustain)
		{
			if (m_bHuman)
			{

				if (BARBARIAN_PLAYER != m_eDefender)
				{
					AddDLLMessage(
						m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText(
							"TXT_KEY_MISC_DEFENDER_PURSUED_SUSTAIN_HUMAN",
							m_unit->getNameKey(), m_defender->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()
						),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_DEFENDER_PURSUED_SUSTAIN_HUMAN_HIDDEN", m_unit->getNameKey(), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			if (m_bHumanDefender)
			{

				if (BARBARIAN_PLAYER != m_eAttacker)
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_DEFENDER_PURSUED_SUSTAIN", m_defender->getNameKey(), m_unit->getVisualCivAdjective(m_unit->getTeam()), m_unit->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_DEFENDER_PURSUED_SUSTAIN_HIDDEN", m_defender->getNameKey(), m_unit->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			m_unit->m_combatResult.bDefenderPursuedSustain = false;
		}

		//Unyielding Reports
		if (m_unit->m_combatResult.bAttackerRefusedtoYieldSustain)
		{
			if (m_bHuman)
			{

				if (BARBARIAN_PLAYER != m_eAttacker)
				{
					AddDLLMessage(
						m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText(
							"TXT_KEY_MISC_ATTACKER_UNYIELDING_SUSTAIN_HUMAN",
							m_unit->getNameKey(), m_unit->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()
						),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_UNYIELDING_SUSTAIN_HUMAN_HIDDEN", m_unit->getNameKey(), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			if (m_bHumanDefender)
			{

				if (BARBARIAN_PLAYER != m_eAttacker)
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_UNYIELDING_SUSTAIN", m_unit->getVisualCivAdjective(m_unit->getTeam()), m_defender->getNameKey(), m_unit->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_UNYIELDING_SUSTAIN_HIDDEN", m_defender->getNameKey(), m_unit->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			m_unit->m_combatResult.bAttackerRefusedtoYieldSustain = false;
		}
		if (m_unit->m_combatResult.bDefenderRefusedtoYieldSustain)
		{
			if (m_bHuman)
			{

				if (BARBARIAN_PLAYER != m_eDefender)
				{
					AddDLLMessage(
						m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_DEFENDER_UNYIELDING_SUSTAIN_HUMAN", m_unit->getNameKey(), m_unit->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_DEFENDER_UNYIELDING_SUSTAIN_HUMAN_HIDDEN", m_unit->getNameKey(), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			if (m_bHumanDefender)
			{

				if (BARBARIAN_PLAYER != m_eAttacker)
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_DEFENDER_UNYIELDING_SUSTAIN", m_defender->getNameKey(), m_unit->getVisualCivAdjective(m_unit->getTeam()), m_unit->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_DEFENDER_UNYIELDING_SUSTAIN_HIDDEN", m_defender->getNameKey(), m_unit->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			m_unit->m_combatResult.bDefenderRefusedtoYieldSustain = false;
		}
	}
	//TB Combat Mods End
	//end the combat mission if m_unit code executes first
	if (!m_unit->isUsingDummyEntities() && m_unit->isInViewport())
	{
		gDLL->getEntityIFace()->RemoveUnitFromBattle(m_unit);
	}
	if (!m_defender->isUsingDummyEntities() && m_defender->isInViewport())
	{
		gDLL->getEntityIFace()->RemoveUnitFromBattle(m_defender);
	}
	m_unit->setAttackPlot(NULL, false);
	m_unit->setCombatUnit(NULL);
	m_defender->setCombatUnit(NULL);
	m_unit->NotifyEntity(MISSION_DAMAGE);
	m_defender->NotifyEntity(MISSION_DAMAGE);
}

void CombatService::onWin()
{
	if (m_defender->isNPC())
	{
		GET_PLAYER(m_unit->getOwner()).changeWinsVsBarbs(1);
	}

	if (!m_unit->isHiddenNationality() && !m_defender->isHiddenNationality())
	{
		const int defenderWarWearinessChangeTimes100 =
			(
				std::max(
					1,
					100 * GC.getDefineINT("WW_UNIT_KILLED_DEFENDING")
					* (m_defender->getMaxHP() - m_defender->getPreCombatDamage())
					/
					m_defender->getMaxHP()
				)
			);
		GET_TEAM(m_defender->getTeam()).changeWarWearinessTimes100(m_unit->getTeam(), *m_plot, defenderWarWearinessChangeTimes100);

		const int attackerWarWearinessChangeTimes100 =
			(
				100 * GC.getDefineINT("WW_KILLED_UNIT_ATTACKING")
				* (m_unit->getDamage() - m_unit->getPreCombatDamage())
				/
				m_unit->getMaxHP()
			);
		GET_TEAM(m_unit->getTeam()).changeWarWearinessTimes100(m_defender->getTeam(), *m_plot, attackerWarWearinessChangeTimes100);

		GET_TEAM(m_unit->getTeam()).AI_changeWarSuccess(m_defender->getTeam(), GC.getDefineINT("WAR_SUCCESS_ATTACKING"));
	}

	const int iInfluenceRatio = GC.isIDW_ENABLED() ? m_unit->doVictoryInfluence(m_defender, true, false) : 0;

	if (m_bHuman)
	{
		CvWString szBuffer;

		if (m_unit->m_combatResult.bDefenderPursued)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_PURSUIT_ONATTACK_SUCCESS", m_defender->getNameKey(), m_unit->getNameKey());
		}
		else if (m_unit->m_combatResult.bAttackerRefusedtoYield)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_UNYIELDING_YOU_DESTROYED_ENEMY", m_defender->getNameKey(), m_unit->getNameKey());
		}
		else if (m_unit->m_combatResult.bDefenderRefusedtoYield)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_UNYIELDING_ENEMY", m_defender->getNameKey(), m_unit->getNameKey());
		}
		else if (m_unit->m_combatResult.bAttackerStampedes)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_ENEMY_STAMPEDE", m_defender->getNameKey(), m_unit->getNameKey());
		}
		else if (m_unit->m_combatResult.bAttackerOnslaught)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_ENEMY_ONSLAUGHT", m_defender->getNameKey(), m_unit->getNameKey());
		}
		else szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_ENEMY", m_unit->getNameKey(), m_defender->getNameKey());

		if (iInfluenceRatio > 0)
		{
			szBuffer = szBuffer + CvString::format(" %s: +%.1f%%", gDLL->getText("TXT_KEY_TILE_INFLUENCE").GetCString(), ((float)iInfluenceRatio) / 10);
		}
		AddDLLMessage(
			m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer,
			GC.getEraInfo(GC.getGame().getCurrentEra()).getAudioUnitVictoryScript(),
			MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
		);
	}
	if (m_bHumanDefender)
	{
		CvWString szBuffer;

		if (m_unit->m_combatResult.bDefenderPursued)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_PURSUIT_ONATTACK_VICTIM", m_defender->getNameKey(), m_unit->getNameKey());
		}
		else if (m_unit->m_combatResult.bAttackerRefusedtoYield)
		{
			if (BARBARIAN_PLAYER != m_eDefender)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_BYUNYIELDING_UNKNOWN", m_defender->getNameKey(), m_unit->getNameKey());
			}
			else szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_BYUNYIELDING", m_defender->getNameKey(), m_unit->getVisualCivAdjective(m_defender->getTeam()), m_unit->getNameKey());
		}
		else if (m_unit->m_combatResult.bDefenderRefusedtoYield)
		{
			if (BARBARIAN_PLAYER == m_eDefender)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_WHILEUNYIELDING_UNKNOWN", m_defender->getNameKey(), m_unit->getNameKey());
			}
			else szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_WHILEUNYIELDING", m_defender->getNameKey(), m_unit->getVisualCivAdjective(m_defender->getTeam()), m_unit->getNameKey());
		}
		else if (m_unit->m_combatResult.bAttackerStampedes)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_ENEMY_STAMPEDE", m_defender->getNameKey(), m_unit->getNameKey());
		}
		else if (m_unit->m_combatResult.bAttackerOnslaught)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_ENEMY_ONSLAUGHT", m_defender->getNameKey(), m_unit->getNameKey());
		}
		else if (BARBARIAN_PLAYER == m_eAttacker)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_UNKNOWN", m_defender->getNameKey(), m_unit->getNameKey());
		}
		else szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED", m_defender->getNameKey(), m_unit->getNameKey(), m_unit->getVisualCivAdjective(m_defender->getTeam()));

		if (iInfluenceRatio > 0)
		{
			szBuffer = szBuffer + CvString::format(" %s: -%.1f%%", gDLL->getText("TXT_KEY_TILE_INFLUENCE").GetCString(), ((float)iInfluenceRatio) / 10);
		}
		AddDLLMessage(
			m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer,
			GC.getEraInfo(GC.getGame().getCurrentEra()).getAudioUnitDefeatScript(),
			MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_RED(), m_plot->getX(), m_plot->getY()
		);
	}

	// report event to Python, along with some other key state
	int iUnitsHealed = 0;
	if (m_unit->getVictoryAdjacentHeal() > 0
	&& GC.getGame().getSorenRandNum(100, "Field Hospital Die Roll") <= m_unit->getVictoryAdjacentHeal())
	{
		foreach_(const CvPlot * pLoopPlot, m_plot->adjacent() | filtered(CvPlot::fn::area() == m_plot->area()))
		{
			foreach_(CvUnit * pLoopUnit, pLoopPlot->units())
			{
				if (pLoopUnit->getTeam() == m_unit->getTeam() && pLoopUnit->isHurt())
				{
					iUnitsHealed++;
					pLoopUnit->doHeal();
				}
			}
		}
	}

	if (m_unit->getVictoryStackHeal() > 0
	&& GC.getGame().getSorenRandNum(100, "Field Surgeon Die Roll") <= m_unit->getVictoryStackHeal())
	{
		foreach_(CvUnit * pLoopUnit, m_plot->units())
		{
			if (pLoopUnit->getTeam() == m_unit->getTeam() && pLoopUnit->isHurt())
			{
				iUnitsHealed++;
				pLoopUnit->doHeal();
			}
		}
	}
	else if (m_unit->getVictoryHeal() > 0 && GC.getGame().getSorenRandNum(100, "Field Medic Die Roll") <= m_unit->getVictoryHeal())
	{
		m_unit->doHeal();
	}

	if (iUnitsHealed > 1)
	{
		if (m_bHumanDefender)
		{
			if (BARBARIAN_PLAYER != m_eAttacker)
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_ENEMY_FIELD_MEDIC_ATTACKERS", GET_PLAYER(m_unit->getOwner()).getCivilizationAdjective()),
					"AS2D_COMBAT", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_YELLOW(), m_unit->getX(), m_unit->getY(), true, true
				);
			}
			else
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_ENEMY_FIELD_MEDIC_ATTACKERS_HIDDEN"),
					"AS2D_COMBAT", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_YELLOW(), m_unit->getX(), m_unit->getY(), true, true
				);
			}
		}
		if (m_bHuman)
		{
			AddDLLMessage(
				m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
				gDLL->getText("TXT_KEY_MISC_FIELD_MEDIC_ATTACKERS", m_unit->getNameKey(), iUnitsHealed),
				"AS2D_POSITIVE_DINK", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_GREEN(), m_unit->getX(), m_unit->getY()
			);
		}
	}

	if (m_unit->getOffensiveVictoryMoveCount() > 0)
	{
		m_unit->changeMoves(-GC.getMOVE_DENOMINATOR());
	}

	if (m_defender->getSurvivorChance() > 0
	&& GC.getGame().getSorenRandNum(100, "Too Badass Check") <= m_defender->getSurvivorChance())
	{
		m_defender->setSurvivor(true);
		m_defender->jumpToNearestValidPlot();
	}
	else if (m_defender->isOneUp())
	{
		CvCity* pCapitalCity = GET_PLAYER(m_defender->getOwner()).getCapitalCity();
		if (pCapitalCity != NULL)
		{
			m_defender->setCanRespawn(true);
		}
	}

	if (m_unit->isPillageOnVictory())
	{
		CvPlot* pDefenderPlot = (m_defender->plot());
		int iPillageGold = (m_defender->getLevel() * m_defender->getExperience());
		if (NO_UNIT != m_defender->getLeaderUnitType())
		{
			iPillageGold *= m_defender->getLevel();
		}
		if (iPillageGold > 0)
		{
			iPillageGold += iPillageGold * m_unit->getPillageChange() / 100;
			GET_PLAYER(m_unit->getOwner()).changeGold(iPillageGold);

			if (m_bHuman)
			{
				if (BARBARIAN_PLAYER != m_eAttacker)
				{
					AddDLLMessage(
						m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_GOLD_LOOTED_FROM_DEAD", m_defender->getNameKey(), m_unit->getVisualCivAdjective(m_unit->getTeam())),
						"AS2D_PILLAGED", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_YELLOW(), pDefenderPlot->getX(), pDefenderPlot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_GOLD_LOOTED_FROM_DEAD_HIDDEN", m_defender->getNameKey()),
						"AS2D_PILLAGED", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_YELLOW(), pDefenderPlot->getX(), pDefenderPlot->getY()
					);
				}
			}

			for (int iI = 0; iI < NUM_COMMERCE_TYPES; ++iI)
			{
				TechTypes ePillageTech = GET_PLAYER(m_unit->getOwner()).getCurrentResearch();
				CommerceTypes eCommerce = (CommerceTypes)iI;
				switch (eCommerce)
				{
				case COMMERCE_GOLD:
					if (m_unit->isPillageMarauder())
					{
						GET_PLAYER(m_unit->getOwner()).changeGold(iPillageGold);
						if (m_bHuman)
						{
							AddDLLMessage(
								m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText("TXT_KEY_MISC_MARAUDERS_PLUNDERED_VICTIMS", iPillageGold, m_unit->getNameKey()),
								"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_GREEN(), pDefenderPlot->getX(), pDefenderPlot->getY()
							);
						}
						if (m_bHumanDefender)
						{
							if (BARBARIAN_PLAYER != m_eAttacker)
							{
								AddDLLMessage(
									m_defender->getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText("TXT_KEY_MISC_ATTACKED_BY_MARAUDERS", m_defender->getNameKey(), m_unit->getVisualCivAdjective(m_unit->getTeam())),
									"AS2D_PILLAGED", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_YELLOW(), pDefenderPlot->getX(), pDefenderPlot->getY(), true, true
								);
							}
							else
							{
								AddDLLMessage(
									m_defender->getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText("TXT_KEY_MISC_ATTACKED_BY_MARAUDERS_HIDDEN", m_defender->getNameKey()),
									"AS2D_PILLAGED", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_YELLOW(), pDefenderPlot->getX(), pDefenderPlot->getY(), true, true
								);
							}
						}
					}
					break;
				case COMMERCE_RESEARCH:
					if (m_unit->isPillageResearch())
					{
						GET_TEAM(GET_PLAYER(m_unit->getOwner()).getTeam()).changeResearchProgress(ePillageTech, iPillageGold, m_unit->getOwner());
						if (m_bHuman)
						{
							if (BARBARIAN_PLAYER != m_eDefender)
							{
								AddDLLMessage(
									m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText(
										"TXT_KEY_MISC_PLUNDERED_RESEARCH_FROM_IMP",
										iPillageGold, m_unit->getNameKey(), m_unit->getVisualCivAdjective(m_defender->getTeam()), ePillageTech
									),
									"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_GREEN(), pDefenderPlot->getX(), pDefenderPlot->getY()
								);
							}
							else
							{
								AddDLLMessage(
									m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText("TXT_KEY_MISC_PLUNDERED_RESEARCH_FROM_IMP_HIDDEN", iPillageGold, m_unit->getNameKey(), ePillageTech),
									"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_GREEN(), pDefenderPlot->getX(), pDefenderPlot->getY()
								);
							}
						}
					}
					break;
				case COMMERCE_CULTURE:
					break;
				case COMMERCE_ESPIONAGE:
					if (m_unit->isPillageEspionage() && pDefenderPlot->getTeam() != NO_TEAM)
					{
						GET_TEAM(GET_PLAYER(m_unit->getOwner()).getTeam()).changeEspionagePointsAgainstTeam(pDefenderPlot->getTeam(), iPillageGold);

						if (m_bHuman)
						{
							if (BARBARIAN_PLAYER != m_eDefender)
							{
								AddDLLMessage(
									m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText(
										"TXT_KEY_MISC_PLUNDERED_ESPIONAGE_FROM_IMP",
										iPillageGold, m_unit->getNameKey(), m_unit->getVisualCivAdjective(m_defender->getTeam()), ePillageTech
									),
									"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_GREEN(), pDefenderPlot->getX(), pDefenderPlot->getY()
								);
							}
							else
							{
								AddDLLMessage(
									m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText("TXT_KEY_MISC_PLUNDERED_ESPIONAGE_FROM_IMP_HIDDEN", iPillageGold, m_unit->getNameKey(), ePillageTech),
									"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_GREEN(), pDefenderPlot->getX(), pDefenderPlot->getY()
								);
							}
						}
					}
					break;
				}
			}
		}
	}

	CvEventReporter::getInstance().combatResult(m_unit, m_defender);
	PlayerTypes eDefenderUnitPlayer = m_defender->getOwner();
	UnitTypes eDefenderUnitType = m_defender->getUnitType();

	// generate the kill outcome list but don't execute it yet
	//std::vector<UnitCombatTypes> aDefenderCombats;
	CvOutcomeListMerged mergedList;
	mergedList.addOutcomeList(m_defender->getUnitInfo().getKillOutcomeList());
	for (std::map<UnitCombatTypes, UnitCombatKeyedInfo>::const_iterator it = m_unit->getUnitCombatKeyedInfo().begin(), end = m_unit->getUnitCombatKeyedInfo().end(); it != end; ++it)
	{
		if (it->second.m_bHasUnitCombat)
		{
			mergedList.addOutcomeList(GC.getUnitCombatInfo((UnitCombatTypes)it->first).getKillOutcomeList());
			//aDefenderCombats.push_back((UnitCombatTypes)it->first);
		}
	}

	if (m_unit->isSuicide())
	{
		m_unit->kill(true);

		m_defender->kill(false, NO_PLAYER, true);
		m_defender = NULL;
	}
	else
	{
		if (m_plot == m_unit->plot())
		{
			m_bSamePlot = true;
		}
		//TB debug attempt: added && pDefender->plot() == pPlot to try to allow units to move in when one defender on the plot withdrew.
		const bool bAdvance = !m_bSamePlot && m_unit->canAdvance(m_plot, (m_defender->canDefend() && !m_defender->isDead() && m_defender->plot() == m_plot) ? 1 : 0);

		//TBMaybeproblem - should m_unit come before the generation of the captive which takes place above at the add outcome step?
		if (bAdvance && !m_unit->isNoCapture())
		{
			m_defender->setCapturingPlayer(m_unit->getOwner());
			m_defender->setCapturingUnit(m_unit);
		}

		m_defender->killUnconditional(false, NO_PLAYER, true);
		m_defender = NULL;
		//TB Combat Mod (Stampede) begin
		if (!bAdvance && !m_unit->m_combatResult.bAttackerStampedes && !m_unit->m_combatResult.bAttackerOnslaught && !m_bStealthDefense)
		{
			m_unit->changeMoves(std::max(GC.getMOVE_DENOMINATOR(), m_plot->movementCost(m_unit, m_unit->plot())));
		}
		if (m_unit->m_combatResult.bAttackerStampedes || m_unit->m_combatResult.bAttackerOnslaught)
		{
			m_unit->getGroup()->clearMissionQueue();
			m_bFinish = false;
			m_unit->attack(m_plot, true);
		}
		if (bAdvance && m_unit->getGroup() != NULL)
		{
			PROFILE("CvUnitNS::CombatService::updateCombat.Advance");
			m_unit->getGroup()->groupMove(m_plot, true, m_unit);
		}
	}
	//TB Combat Mods End

	if (m_unit->getGroup() != NULL)
	{
		if (m_unit->getGroup()->canAnyMove()) // testing selective situation here.
		{
			m_unit->getGroup()->setMissionPaneDirty();
		}
		else m_unit->getGroup()->clearMissionQueue();
	}
	mergedList.execute(*m_unit, eDefenderUnitPlayer, eDefenderUnitType);
}

void CombatService::onLoss()
{
	if (m_unit->isNPC())
	{
		GET_PLAYER(m_defender->getOwner()).changeWinsVsBarbs(1);
	}

	if (!m_unit->isHiddenNationality() && !m_defender->isHiddenNationality())
	{
		const int attackerWarWearinessChangeTimes100 = std::max(1, 100 * GC.getDefineINT("WW_UNIT_KILLED_ATTACKING") * (m_unit->getMaxHP() - m_unit->getPreCombatDamage()) / m_unit->getMaxHP());
		GET_TEAM(m_unit->getTeam()).changeWarWearinessTimes100(m_defender->getTeam(), *m_plot, attackerWarWearinessChangeTimes100);

		const int defenderWarWearinessChangeTimes100 = 100 * GC.getDefineINT("WW_KILLED_UNIT_DEFENDING") * (m_defender->getDamage() - m_defender->getPreCombatDamage()) / m_defender->getMaxHP();
		GET_TEAM(m_defender->getTeam()).changeWarWearinessTimes100(m_unit->getTeam(), *m_plot, defenderWarWearinessChangeTimes100);

		GET_TEAM(m_defender->getTeam()).AI_changeWarSuccess(m_unit->getTeam(), GC.getDefineINT("WAR_SUCCESS_DEFENDING"));
	}

	const int iInfluenceRatio = GC.isIDW_ENABLED() ? m_defender->doVictoryInfluence(m_unit, false, false) : 0;

	if (m_bHuman)
	{
		CvWString szBuffer;

		if (m_unit->m_combatResult.bAttackerPursued)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_PURSUIT_VICTIM", m_unit->getNameKey(), m_defender->getNameKey());
		}
		else if (m_unit->m_combatResult.bAttackerRefusedtoYield)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNYIELDING_UNIT_DIED_ATTACKING", m_unit->getNameKey(), m_defender->getNameKey());
		}
		else if (m_unit->m_combatResult.bDefenderRefusedtoYield)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_DIED_ATTACKING_UNYIELDING", m_unit->getNameKey(), m_defender->getNameKey());
		}
		else szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_DIED_ATTACKING", m_unit->getNameKey(), m_defender->getNameKey());

		if (iInfluenceRatio > 0)
		{
			szBuffer = szBuffer + CvString::format(" %s: -%.1f%%", gDLL->getText("TXT_KEY_TILE_INFLUENCE").GetCString(), ((float)iInfluenceRatio) / 10);
		}
		AddDLLMessage(
			m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer,
			GC.getEraInfo(GC.getGame().getCurrentEra()).getAudioUnitDefeatScript(),
			MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_RED(), m_plot->getX(), m_plot->getY()
		);
	}
	if (m_bHumanDefender)
	{
		CvWString szBuffer;

		if (m_unit->m_combatResult.bAttackerPursued)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_PURSUIT_SUCCESS", m_unit->getNameKey(), m_defender->getNameKey());
		}
		else if (m_unit->m_combatResult.bAttackerRefusedtoYield)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_UNYIELDING_ATTACKER_YOU_KILLED", m_unit->getNameKey(), m_defender->getNameKey());
		}
		else if (m_unit->m_combatResult.bDefenderRefusedtoYield)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_KILLED_ATTACKER_WHILEUNYIELDING", m_unit->getNameKey(), m_defender->getNameKey());
		}
		else if (BARBARIAN_PLAYER != m_eAttacker)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_KILLED_ENEMY_UNIT", m_defender->getNameKey(), m_unit->getNameKey(), m_unit->getVisualCivAdjective(m_defender->getTeam()));
		}
		else szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_KILLED_ENEMY_UNIT_HIDDEN", m_defender->getNameKey(), m_unit->getNameKey());

		if (iInfluenceRatio > 0)
		{
			szBuffer = szBuffer + CvString::format(" %s: +%.1f%%", gDLL->getText("TXT_KEY_TILE_INFLUENCE").GetCString(), ((float)iInfluenceRatio) / 10);
		}
		AddDLLMessage(
			m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer,
			GC.getEraInfo(GC.getGame().getCurrentEra()).getAudioUnitVictoryScript(),
			MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
		);
	}

	int iUnitsHealed = 0;
	if (m_defender->getVictoryAdjacentHeal() > 0
	&& GC.getGame().getSorenRandNum(100, "Field Hospital Die Roll") <= m_defender->getVictoryAdjacentHeal())
	{
		foreach_(const CvPlot * pLoopPlot, m_plot->adjacent() | filtered(CvPlot::fn::area() == m_plot->area()))
		{
			foreach_(CvUnit * pLoopUnit, pLoopPlot->units())
			{
				if (pLoopUnit->getTeam() == m_defender->getTeam() && pLoopUnit->isHurt())
				{
					iUnitsHealed++;
					pLoopUnit->doHeal();
				}
			}
		}
	}

	if (m_defender->getVictoryStackHeal() > 0
	&& GC.getGame().getSorenRandNum(100, "Field Surgeon Die Roll") <= m_defender->getVictoryStackHeal())
	{
		foreach_(CvUnit * pLoopUnit, m_plot->units())
		{
			if (pLoopUnit->getTeam() == m_defender->getTeam() && pLoopUnit->isHurt())
			{
				iUnitsHealed++;
				pLoopUnit->doHeal();
			}
		}
	}
	else if (m_defender->getVictoryHeal() > 0 && m_defender->isHurt()
	&& GC.getGame().getSorenRandNum(100, "Field Medic Die Roll") <= m_defender->getVictoryHeal())
	{
		m_defender->doHeal();
	}

	if (iUnitsHealed > 1)
	{
		if (m_bHuman)
		{
			if (BARBARIAN_PLAYER != m_eAttacker)
			{
				AddDLLMessage(
					m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_ENEMY_FIELD_MEDIC_DEFENDERS", GET_PLAYER(m_defender->getOwner()).getCivilizationAdjective()),
					"AS2D_COMBAT", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_YELLOW(), m_defender->getX(), m_defender->getY()
				);
			}
			else
			{
				AddDLLMessage(
					m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_MISC_ENEMY_FIELD_MEDIC_DEFENDERS_HIDDEN"),
					"AS2D_COMBAT", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_YELLOW(), m_defender->getX(), m_defender->getY()
				);
			}
		}
		if (m_bHumanDefender)
		{
			AddDLLMessage(
				m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
				gDLL->getText("TXT_KEY_MISC_FIELD_MEDIC_DEFENDERS", m_defender->getNameKey(), iUnitsHealed),
				"AS2D_POSITIVE_DINK", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_GREEN(), m_defender->getX(), m_defender->getY()
			);
		}
	}

	if (m_defender->getDefensiveVictoryMoveCount() > 0)
	{
		m_defender->changeMoves(-GC.getMOVE_DENOMINATOR());
	}

	if (m_unit->getSurvivorChance() > 0
	&& GC.getGame().getSorenRandNum(100, "Too Badass Check") <= m_unit->getSurvivorChance())
	{
		m_unit->setSurvivor(true);
		m_unit->jumpToNearestValidPlot();
	}
	else if (m_unit->isOneUp())
	{
		m_unit->setCanRespawn(true);
	}

	if (m_defender->isPillageOnVictory())
	{
		CvPlot* pDefenderPlot = m_defender->plot();
		int iPillageGold = m_unit->getLevel() * m_unit->getExperience();
		if (NO_UNIT != m_unit->getLeaderUnitType())
		{
			iPillageGold *= m_unit->getLevel();
		}

		if (iPillageGold > 0)
		{
			iPillageGold += (iPillageGold * m_defender->getPillageChange()) / 100;
			GET_PLAYER(m_defender->getOwner()).changeGold(iPillageGold);

			if (m_bHuman)
			{
				if (BARBARIAN_PLAYER != m_eDefender)
				{
					AddDLLMessage(
						m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_GOLD_LOOTED_FROM_DEAD", m_unit->getNameKey(), m_defender->getVisualCivAdjective(m_defender->getTeam())),
						"AS2D_PILLAGED", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_YELLOW(), pDefenderPlot->getX(), pDefenderPlot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_GOLD_LOOTED_FROM_DEAD_HIDDEN", m_unit->getNameKey()),
						"AS2D_PILLAGED", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_YELLOW(), pDefenderPlot->getX(), pDefenderPlot->getY()
					);
				}
			}

			for (int iI = 0; iI < NUM_COMMERCE_TYPES; ++iI)
			{
				TechTypes ePillageTech = GET_PLAYER(m_defender->getOwner()).getCurrentResearch();
				CommerceTypes eCommerce = (CommerceTypes)iI;
				switch (eCommerce)
				{
				case COMMERCE_GOLD:
					if (m_defender->isPillageMarauder())
					{
						GET_PLAYER(m_defender->getOwner()).changeGold(iPillageGold);
						if (m_bHumanDefender)
						{
							AddDLLMessage(
								m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText("TXT_KEY_MISC_MARAUDERS_PLUNDERED_VICTIMS", iPillageGold, m_defender->getNameKey()),
								"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_GREEN(), pDefenderPlot->getX(), pDefenderPlot->getY()
							);
						}
						if (m_bHuman)
						{
							if (BARBARIAN_PLAYER != m_eDefender)
							{
								AddDLLMessage(
									m_unit->getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText("TXT_KEY_MISC_DEFENDED_BY_MARAUDERS", m_unit->getNameKey(), m_unit->getVisualCivAdjective(m_defender->getTeam())),
									"AS2D_PILLAGED", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_YELLOW(), pDefenderPlot->getX(), pDefenderPlot->getY(), true, true
								);
							}
							else
							{
								AddDLLMessage(
									m_unit->getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText("TXT_KEY_MISC_DEFENDED_BY_MARAUDERS_HIDDEN", m_unit->getNameKey()),
									"AS2D_PILLAGED", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_YELLOW(), pDefenderPlot->getX(), pDefenderPlot->getY(), true, true
								);
							}
						}
					}
					break;
				case COMMERCE_RESEARCH:
					if (m_defender->isPillageResearch())
					{
						GET_TEAM(GET_PLAYER(m_defender->getOwner()).getTeam()).changeResearchProgress(ePillageTech, iPillageGold, m_defender->getOwner());
						if (m_bHumanDefender)
						{
							if (BARBARIAN_PLAYER != m_eDefender)
							{
								AddDLLMessage(
									m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText("TXT_KEY_MISC_PLUNDERED_RESEARCH_FROM_VICTIMS", iPillageGold, m_defender->getNameKey(), m_unit->getVisualCivAdjective(m_unit->getTeam()), ePillageTech),
									"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_GREEN(), pDefenderPlot->getX(), pDefenderPlot->getY()
								);
							}
							else
							{
								AddDLLMessage(
									m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText("TXT_KEY_MISC_PLUNDERED_RESEARCH_FROM_VICTIMS_HIDDEN", iPillageGold, m_defender->getNameKey(), ePillageTech),
									"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_GREEN(), pDefenderPlot->getX(), pDefenderPlot->getY()
								);
							}
						}
					}
					break;
				case COMMERCE_CULTURE:
					break;
				case COMMERCE_ESPIONAGE:
					if (m_defender->isPillageEspionage() && pDefenderPlot->getTeam() != NO_TEAM)
					{
						GET_TEAM(GET_PLAYER(m_defender->getOwner()).getTeam()).changeEspionagePointsAgainstTeam(m_unit->getTeam(), iPillageGold);
						if (m_bHumanDefender)
						{
							if (BARBARIAN_PLAYER != m_eDefender)
							{
								AddDLLMessage(
									m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText("TXT_KEY_MISC_PLUNDERED_ESPIONAGE_FROM_VICTIMS", iPillageGold, m_unit->getVisualCivAdjective(m_unit->getTeam()), m_defender->getNameKey()),
									"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_GREEN(), pDefenderPlot->getX(), pDefenderPlot->getY()
								);
							}
							else
							{
								AddDLLMessage(
									m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText("TXT_KEY_MISC_PLUNDERED_ESPIONAGE_FROM_VICTIMS_HIDDEN", iPillageGold, m_defender->getNameKey()),
									"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_unit->getButton(), GC.getCOLOR_GREEN(), pDefenderPlot->getX(), pDefenderPlot->getY()
								);
							}
						}
					}
					break;
				}
			}
		}
	}

	// report event to Python, along with some other key state
	CvEventReporter::getInstance().combatResult(m_defender, m_unit);

	CvOutcomeListMerged list;
	list.addOutcomeList(m_unit->getUnitInfo().getKillOutcomeList());
	//getUnitInfo().getKillOutcomeList()->execute(*pDefender, m_unit->getOwner(), m_unit->getUnitType());
	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		const UnitCombatTypes eCombat = (UnitCombatTypes)iI;
		if (m_unit->isHasUnitCombat(eCombat))
		{
			list.addOutcomeList(GC.getUnitCombatInfo(eCombat).getKillOutcomeList());
			//pOutcomeList->execute(*pDefender, m_unit->getOwner(), m_unit->getUnitType());
		}
	}
	list.execute(*m_defender, m_unit->getOwner(), m_unit->getUnitType());
}

void CombatService::onAttackerWithraw()
{
	if (!m_unit->m_combatResult.bAttackerStampedes && !m_unit->m_combatResult.bAttackerOnslaught)
	{
		if (m_bHuman)
		{
			AddDLLMessage(
				m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
				gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WITHDRAW", m_unit->getNameKey(), m_defender->getNameKey()),
				"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
			);
		}
		if (m_bHumanDefender)
		{
			AddDLLMessage(
				m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
				gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_WITHDRAW", m_unit->getNameKey(), m_defender->getNameKey()),
				"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
			);
		}
		m_unit->changeMoves(std::max(GC.getMOVE_DENOMINATOR(), m_plot->movementCost(m_unit, m_unit->plot())));
		//GC.getGame().logOOSSpecial(53, getID(), getMoves(), m_unit->getDamage());

		if (m_unit->getGroup() != NULL)
		{
			if (m_unit->getGroup()->canAnyMove())//testing selective situation here.
			{
				m_unit->getGroup()->setMissionPaneDirty();
			}
			else m_unit->getGroup()->clearMissionQueue();
		}
	}
	//TB Combat Mod (Stampede) begin
	else if (m_unit->m_combatResult.bAttackerStampedes)
	{
		if (m_bHuman)
		{
			AddDLLMessage(
				m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
				gDLL->getText("TXT_KEY_MISC_YOU_UNIT_ATTACKER_WITHDRAW_STAMPEDE", m_unit->getNameKey(), m_defender->getNameKey()),
				"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
			);
		}
		if (m_bHumanDefender)
		{
			AddDLLMessage(
				m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
				gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_ATTACKER_WITHDRAW_STAMPEDE", m_unit->getNameKey(), m_defender->getNameKey()),
				"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
			);
		}
		if (m_unit->getGroup() != NULL)
		{
			m_unit->getGroup()->clearMissionQueue();
		}
		m_bFinish = false;
		m_unit->attack(m_plot, true);
	}
	else if (m_unit->m_combatResult.bAttackerOnslaught)
	{
		if (m_bHuman)
		{
			AddDLLMessage(
				m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
				gDLL->getText("TXT_KEY_MISC_YOU_UNIT_ATTACKER_WITHDRAW_ONSLAUGHT", m_unit->getNameKey(), m_defender->getNameKey()),
				"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
			);
		}
		if (m_bHumanDefender)
		{
			AddDLLMessage(
				m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
				gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_ATTACKER_WITHDRAW_ONSLAUGHT", m_unit->getNameKey(), m_defender->getNameKey()),
				"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
			);
		}
		if (m_unit->getGroup() != NULL)
		{
			m_unit->getGroup()->clearMissionQueue();
		}
		m_bFinish = false;
		m_unit->attack(m_plot, true);
	}
}

void CombatService::onDefenderWithraw()
{
	if (!m_unit->m_combatResult.bAttackerOnslaught)
	{
		if (!m_unit->m_combatResult.bAttackerStampedes)
		{
			if (m_bHuman)
			{
				AddDLLMessage(
					m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_WITHDRAW", m_defender->getNameKey(), m_unit->getNameKey()),
					"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
				);
			}
			if (m_bHumanDefender)
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WITHDRAW", m_defender->getNameKey(), m_unit->getNameKey()),
					"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
				);
			}
		}
		else
		{
			if (m_bHuman)
			{
				AddDLLMessage(
					m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_WITHDRAW_STAMPEDE", m_defender->getNameKey(), m_unit->getNameKey()),
					"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
				);
			}
			if (m_bHumanDefender)
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WITHDRAW_STAMPEDE", m_defender->getNameKey(), m_unit->getNameKey()),
					"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
				);
			}
		}
	}
	else
	{
		if (m_bHuman)
		{
			AddDLLMessage(
				m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
				gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_WITHDRAW_ONSLAUGHT", m_defender->getNameKey(), m_unit->getNameKey()),
				"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
			);
		}
		if (m_bHumanDefender)
		{
			AddDLLMessage(
				m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
				gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WITHDRAW_ONSLAUGHT", m_defender->getNameKey(), m_unit->getNameKey()),
				"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
			);
		}
	}

	if (m_plot->isCity())
	{
		m_unit->m_combatResult.pPlot = NULL;
		//TB Combat Mod (Stampede)
		if (m_unit->m_combatResult.bAttackerStampedes || m_unit->m_combatResult.bAttackerOnslaught)
		{
			m_unit->getGroup()->clearMissionQueue();
			m_bFinish = false;
			m_unit->attack(m_plot, true);
		}
		else
		{
			const bool bAdvance = !m_bSamePlot && m_unit->canAdvance(m_plot, (m_defender->canDefend() && !m_defender->isDead() && m_defender->plot() == m_plot) ? 1 : 0);
			if (bAdvance)
			{
				if (m_unit->getGroup() != NULL && m_plot->getNumVisiblePotentialEnemyDefenders(m_unit) == 0)
				{
					PROFILE("CvUnitNS::CombatService::updateCombat.Advance");
					m_unit->getGroup()->groupMove(m_plot, true, bAdvance ? m_unit : NULL);
				}
			}
			else if (!m_unit->m_combatResult.bAttackerStampedes && !m_unit->m_combatResult.bAttackerOnslaught)
			{
				m_unit->changeMoves(std::max(GC.getMOVE_DENOMINATOR(), m_plot->movementCost(m_unit, m_unit->plot())));
			}
			if (m_unit->getGroup() != NULL)
			{
				if (!m_unit->getGroup()->canAnyMove())//testing selective situation here.
				{
					m_unit->getGroup()->clearMissionQueue();
				}
				else
				{
					m_unit->getGroup()->setMissionPaneDirty();
				}
			}
		}
		//TB Combat Mod end
	}

	if (m_unit->m_combatResult.pPlot != NULL && !m_bSamePlot)
	{
		FAssertMsg(m_unit->m_combatResult.pPlot != m_unit->plot(), "Can't escape back to attacker plot");
		FAssertMsg(m_unit->m_combatResult.pPlot != m_defender->plot(), "Can't escape back to own plot");

		//defender escapes to a safe plot
		m_defender->move(m_unit->m_combatResult.pPlot, true);
		const bool bAdvance = m_unit->canAdvance(m_plot, (m_defender->canDefend() && !m_defender->isDead() && m_defender->plot() == m_plot) ? 1 : 0);

		//TB Combat Mod (Stampede) begin
		if (m_unit->m_combatResult.bAttackerStampedes || m_unit->m_combatResult.bAttackerOnslaught)
		{
			m_unit->getGroup()->clearMissionQueue();
			m_bFinish = false;
			m_unit->attack(m_plot, true);
		}
		else
		{
			if (m_unit->getGroup() != NULL)
			{
				if (bAdvance && m_unit->getGroup() != NULL && m_plot->getNumVisiblePotentialEnemyDefenders(m_unit) == 0)
				{
					PROFILE("CvUnitNS::CombatService::updateCombat.Advance");

					m_unit->getGroup()->groupMove(m_plot, true, ((bAdvance) ? m_unit : NULL));
				}
				else if (!m_bStealthDefense && !m_unit->m_combatResult.bAttackerStampedes && !m_unit->m_combatResult.bAttackerOnslaught)
				{
					m_unit->changeMoves(std::max(GC.getMOVE_DENOMINATOR(), m_plot->movementCost(m_unit, m_unit->plot())));
				}
			}
			else
			{
				//TB Combat Mod (Stampede) end
				if (!m_bStealthDefense)
				{
					m_unit->changeMoves(std::max(GC.getMOVE_DENOMINATOR(), m_plot->movementCost(m_unit, m_unit->plot())));
				}
			}
			if (m_unit->getGroup() != NULL)
			{
				if (m_unit->getGroup()->canAnyMove())//testing selective situation here.
				{
					m_unit->getGroup()->setMissionPaneDirty();
				}
				else m_unit->getGroup()->clearMissionQueue();
			}
		}
	}
	else
	{
		//TB Combat Mod (Stampede) end
		if (!m_bStealthDefense)
		{
			m_unit->changeMoves(std::max(GC.getMOVE_DENOMINATOR(), m_plot->movementCost(m_unit, m_unit->plot())));
		}
		if (m_unit->getGroup() != NULL)
		{
			if (m_unit->getGroup()->canAnyMove())//testing selective situation here.
			{
				m_unit->getGroup()->setMissionPaneDirty();
			}
			else m_unit->getGroup()->clearMissionQueue();
		}
	}
}

void CombatService::onDefenderKnockedBack()
{
	if (!m_unit->m_combatResult.bAttackerStampedes && !m_unit->m_combatResult.bAttackerOnslaught)
	{
		if (m_bHuman)
		{
			if (BARBARIAN_PLAYER != m_eDefender)
			{
				AddDLLMessage(
					m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText(
						"TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_HUMAN",
						m_unit->getNameKey(), m_unit->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()
					),
					"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
				);
			}
			else
			{
				AddDLLMessage(
					m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_HUMAN_HIDDEN", m_unit->getNameKey(), m_defender->getNameKey()),
					"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
				);
			}
		}
		if (m_bHumanDefender)
		{
			if (BARBARIAN_PLAYER != m_eAttacker)
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText(
						"TXT_KEY_MISC_DEFENDER_KNOCKEDBACK",
						m_unit->getVisualCivAdjective(m_unit->getTeam()), m_unit->getNameKey(), m_defender->getNameKey()
					),
					"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
				);
			}
			else
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_HIDDEN", m_unit->getNameKey(), m_defender->getNameKey()),
					"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
				);
			}
		}
	}
	else if (!m_unit->m_combatResult.bAttackerOnslaught)
	{
		if (m_bHuman)
		{
			if (BARBARIAN_PLAYER != m_eDefender)
			{
				AddDLLMessage(
					m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText(
						"TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_HUMAN_STAMPEDE",
						m_unit->getNameKey(), m_unit->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()
					),
					"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
				);
			}
			else
			{
				AddDLLMessage(
					m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText(
						"TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_HUMAN_STAMPEDE_HIDDEN",
						m_unit->getNameKey(), m_defender->getNameKey()
					),
					"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
				);
			}
		}
		if (m_bHumanDefender)
		{
			if (BARBARIAN_PLAYER != m_eAttacker)
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText(
						"TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_STAMPEDE",
						m_unit->getVisualCivAdjective(m_unit->getTeam()), m_unit->getNameKey(), m_defender->getNameKey()
					),
					"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
				);
			}
			else
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText(
						"TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_STAMPEDE_HIDDEN",
						m_unit->getNameKey(), m_defender->getNameKey()
					),
					"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
				);
			}
		}
	}
	else
	{
		if (m_bHuman)
		{
			if (BARBARIAN_PLAYER != m_eDefender)
			{
				AddDLLMessage(
					m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText(
						"TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_HUMAN_ONSLAUGHT",
						m_unit->getNameKey(), m_unit->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()
					),
					"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
				);
			}
			else
			{
				AddDLLMessage(
					m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText(
						"TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_HUMAN_ONSLAUGHT_HIDDEN",
						m_unit->getNameKey(), m_defender->getNameKey()
					),
					"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
				);
			}
		}
		if (m_bHumanDefender)
		{
			if (BARBARIAN_PLAYER != m_eAttacker)
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText(
						"TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_ONSLAUGHT",
						m_unit->getVisualCivAdjective(m_unit->getTeam()), m_unit->getNameKey(), m_defender->getNameKey()
					),
					"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
				);
			}
			else
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText(
						"TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_ONSLAUGHT_HIDDEN",
						m_unit->getNameKey(), m_defender->getNameKey()
					),
					"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
				);
			}
		}
	}
	m_unit->m_combatResult.bDefenderKnockedBack = false;

	if (m_plot->isCity())
	{
		m_unit->m_combatResult.pPlot = NULL;
	}
	m_defender->setFortifyTurns(0);

	if (m_unit->m_combatResult.pPlot != NULL)
	{
		FAssertMsg(m_unit->m_combatResult.pPlot != m_unit->plot(), "Can't get knocked back to attacker plot");
		FAssertMsg(m_unit->m_combatResult.pPlot != m_defender->plot(), "Can't get knocked back to own plot");

		//defender escapes to a safe plot
		m_defender->move(m_unit->m_combatResult.pPlot, true);
		m_defender->getGroup()->clearMissionQueue();
	}

	const bool bAdvance = m_unit->canAdvance(m_plot, (m_defender->canDefend() && !m_defender->isDead() && m_defender->plot() == m_plot) ? 1 : 0);

	if (!bAdvance && !m_unit->m_combatResult.bAttackerStampedes && !m_unit->m_combatResult.bAttackerOnslaught)
	{
		if (!m_unit->isNoCapture())
		{
			m_defender->setCapturingPlayer(m_unit->getOwner());
			m_defender->setCapturingUnit(m_unit);
		}
		if (!m_bStealthDefense)
		{
			m_unit->changeMoves(std::max(GC.getMOVE_DENOMINATOR(), m_plot->movementCost(m_unit, m_unit->plot())));
		}
	}
	else if (m_unit->m_combatResult.bAttackerStampedes || m_unit->m_combatResult.bAttackerOnslaught)
	{
		//TB Combat Mod (Stampede)
		m_unit->getGroup()->clearMissionQueue();
		m_bFinish = false;
		m_unit->attack(m_plot, true);
	}

	if (m_unit->getGroup() != NULL)
	{
		if (m_plot->getNumVisiblePotentialEnemyDefenders(m_unit) == 0)
		{
			PROFILE("CvUnitNS::CombatService::updateCombat.Advance");

			m_unit->getGroup()->groupMove(m_plot, true, ((bAdvance) ? m_unit : NULL));
		}

		// This is before the plot advancement, the m_unit will always try to walk back
		// to the square that they came from, before advancing.
	}
	if (m_unit->getGroup() != NULL)
	{
		if (m_unit->getGroup()->canAnyMove())//testing selective situation here.
		{
			m_unit->getGroup()->setMissionPaneDirty();
		}
		else m_unit->getGroup()->clearMissionQueue();
	}
}

void CombatService::onOtherResult()
{
	if (!m_bStealthDefense)
	{
		m_unit->changeMoves(std::max(GC.getMOVE_DENOMINATOR(), m_plot->movementCost(m_unit, m_unit->plot())));
	}

	if (m_unit->getGroup() != NULL)
	{
		if (m_unit->getGroup()->canAnyMove()) // testing selective situation here.
		{
			m_unit->getGroup()->setMissionPaneDirty();
		}
		else m_unit->getGroup()->clearMissionQueue();
	}
}

void CombatService::onAttackerRepelled()
{
	if (!m_unit->m_combatResult.bAttackerStampedes && !m_unit->m_combatResult.bAttackerOnslaught)
	{
		if (m_bHuman)
		{

			if (BARBARIAN_PLAYER != m_eDefender)
			{
				AddDLLMessage(
					m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText(
						"TXT_KEY_MISC_ATTACKER_REPELLED_HUMAN",
						m_unit->getNameKey(), m_unit->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()
					),
					"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
				);
			}
			else
			{
				AddDLLMessage(
					m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_ATTACKER_REPELLED_HUMAN_HIDDEN", m_unit->getNameKey(), m_defender->getNameKey()),
					"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
				);
			}
		}
		if (m_bHumanDefender)
		{

			if (BARBARIAN_PLAYER != m_eDefender)
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_ATTACKER_REPELLED", m_unit->getVisualCivAdjective(m_unit->getTeam()), m_unit->getNameKey(), m_defender->getNameKey()),
					"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
				);
			}
			else
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_ATTACKER_REPELLED_HIDDEN", m_unit->getNameKey(), m_defender->getNameKey()),
					"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
				);
			}
		}
	}
	else if (!m_unit->m_combatResult.bAttackerOnslaught)
	{
		if (m_bHuman)
		{

			if (BARBARIAN_PLAYER != m_eDefender)
			{
				AddDLLMessage(
					m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText(
						"TXT_KEY_MISC_ATTACKER_REPELLED_HUMAN_STAMPEDE",
						m_unit->getNameKey(), m_unit->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()
					),
					"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
				);
			}
			else
			{
				AddDLLMessage(
					m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_ATTACKER_REPELLED_HUMAN_STAMPEDE_HIDDEN", m_unit->getNameKey(), m_defender->getNameKey()),
					"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
				);
			}
		}
		if (m_bHumanDefender)
		{

			if (BARBARIAN_PLAYER != m_eAttacker)
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_ATTACKER_REPELLED_STAMPEDE", m_unit->getVisualCivAdjective(m_unit->getTeam()), m_unit->getNameKey(), m_defender->getNameKey()),
					"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
				);
			}
			else
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_ATTACKER_REPELLED_STAMPEDE_HIDDEN", m_unit->getNameKey(), m_defender->getNameKey()),
					"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
				);
			}
		}
	}
	else
	{
		if (m_bHuman)
		{
			if (BARBARIAN_PLAYER != m_eDefender)
			{
				AddDLLMessage(
					m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText(
						"TXT_KEY_MISC_ATTACKER_REPELLED_HUMAN_ONSLAUGHT",
						m_unit->getNameKey(), m_unit->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()
					),
					"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
				);
			}
			else
			{
				AddDLLMessage(
					m_unit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_ATTACKER_REPELLED_HUMAN_ONSLAUGHT_HIDDEN", m_unit->getNameKey(), m_defender->getNameKey()),
					"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
				);
			}
		}
		if (m_bHumanDefender)
		{
			if (BARBARIAN_PLAYER != m_eAttacker)
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText(
						"TXT_KEY_MISC_ATTACKER_REPELLED_ONSLAUGHT",
						m_unit->getVisualCivAdjective(m_unit->getTeam()), m_unit->getNameKey(), m_defender->getNameKey()
					),
					"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
				);
			}
			else
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_ATTACKER_REPELLED_ONSLAUGHT_HIDDEN", m_unit->getNameKey(), m_defender->getNameKey()),
					"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
				);
			}
		}
	}

	m_unit->m_combatResult.bAttackerRepelled = false;

	if (m_unit->m_combatResult.bAttackerStampedes || m_unit->m_combatResult.bAttackerOnslaught)
	{
		//TB Combat Mod (Stampede)
		m_bFinish = false;
		m_unit->attack(m_plot, true);
	}
	else
	{
		if (!m_bStealthDefense)
		{
			m_unit->changeMoves(std::max(GC.getMOVE_DENOMINATOR(), m_plot->movementCost(m_unit, m_unit->plot())));
		}
		if (m_unit->getGroup() != NULL)
		{
			if (m_unit->getGroup()->canAnyMove())//testing selective situation here.
			{
				m_unit->getGroup()->setMissionPaneDirty();
			}
			else m_unit->getGroup()->clearMissionQueue();
		}
	}
}