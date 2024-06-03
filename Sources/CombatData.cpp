#include "CvGameCoreDLL.h"
#include "CombatData.h"
#include "CvInfos.h"
#include "CvUnit.h"

CombatData::CombatData(CvUnit* attacker, CvUnit* defender, CvPlot* pPlot)
{
	m_attacker = attacker;
	m_defender = defender;

	m_plot = pPlot;
	m_plotWithdrawal = NULL;

	m_bSamePlot = attacker->plot() == pPlot;
	m_bQuick = m_bSamePlot || !attacker->isCombatVisible(defender);
	m_bCombatStarted = false;
	m_bDefenderWithdrawn = false;
	m_bAttackerPursued = false;
	m_bDefenderPursued = false;
	m_bAttackerPursuedSustain = false;
	m_bDefenderPursuedSustain = false;
	m_bAttackerRepelled = false;
	m_bAttackerRefusedtoYield = false;
	m_bDefenderRefusedtoYield = false;
	m_bAttackerRefusedtoYieldSustain = false;
	m_bDefenderRefusedtoYieldSustain = false;
	m_bDefenderKnockedBack = false;
	m_bAttackerStampedes = false;
	m_bAttackerWithdraws = false;
	m_bAttackerOnslaught = false;
	m_bAttackerInjured = false;
	m_bDefenderInjured = false;
	m_bDefenderHitAttackerWithDistanceAttack = false;
	m_bAttackerHitDefenderWithDistanceAttack = false;
	m_bStealthDefense = false;
	m_bDeathMessaged = true;
	m_bNeverMelee = true;

	m_iTurnCount = 0;
	m_iAttacksCount = 0;

}
CombatData::~CombatData() { }


void CombatData::resolveCombat(CvBattleDefinition& kBattle)
{
	PROFILE_FUNC();

	CombatDetails cdAttackerDetails;
	CombatDetails cdDefenderDetails;

	m_attacker->AI_setPredictedHitPoints(-1);
	m_defender->AI_setPredictedHitPoints(-1);
	int iAttackerStrength = m_attacker->currCombatStr(NULL, NULL, &cdAttackerDetails);
	int iAttackerFirepower = m_attacker->currFirepower(NULL, NULL);
	int iDefenderStrength = 0;
	int iAttackerDamage = 0;
	int iDefenderDamage = 0;
	int iDefenderOdds = 0;

	bool bAttackerWithdrawn = false;

	int temporarypursuit = 0;
	const int iDefenderDodge = m_defender->dodgeVSOpponentProbTotal(m_attacker);
	int iDefenderPrecision = m_defender->precisionVSOpponentProbTotal(m_attacker);
	const int iAttackerDodge = m_attacker->dodgeVSOpponentProbTotal(m_defender);
	int iAttackerPrecision = m_attacker->precisionVSOpponentProbTotal(m_defender);
	bool bBreakdown = false;
	const int iDefenderFirstStrikes = m_defender->getCombatFirstStrikes();
	const int iAttackerFirstStrikes = m_attacker->getCombatFirstStrikes();
	//TB Combat Mods End
	bool bAttackerHasLostNoHP = true;
	int iAttackerInitialDamage = m_attacker->getDamage();
	int iDefenderInitialDamage = m_defender->getDamage();
	int iDefenderCombatRoll = 0;
	int iAttackerCombatRoll = 0;
	int WithdrawalRollResult = 0;
	int DefenderWithdrawalRollResult = 0;
	int RepelRollResult = 0;
	int PursuitRollResult = 0;
	int iAttackerHitModifier = 0;
	int iDefenderHitModifier = 0;
	int iAttackerOdds = 0;
	int	iDefenderHitChance = 0;
	int	iAttackerHitChance = 0;
	int iInitialDefXP = m_defender->getExperience100();
	int iInitialAttXP = m_attacker->getExperience100();
	int iInitialAttGGXP = GET_PLAYER(m_attacker->getOwner()).getCombatExperience();
	int iInitialDefGGXP = GET_PLAYER(m_defender->getOwner()).getCombatExperience();
	const bool bDynamicXP = GC.getGame().isModderGameOption(MODDERGAMEOPTION_IMPROVED_XP);

	getDefenderCombatValues(*m_defender, m_plot, iAttackerStrength, iAttackerFirepower, iDefenderOdds, iDefenderStrength, iAttackerDamage, iDefenderDamage, &cdDefenderDetails, m_defender);
	int iInitialAttackerStrength = iAttackerStrength;
	int iInitialDefenderStrength = iDefenderStrength;
#ifdef OUTBREAKS_AND_AFFLICTIONS
	//TB Combat Mods Begin
	for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
	{
		if (m_attacker->hasAfflictOnAttackType((PromotionLineTypes)iI))
		{
			m_attacker->setAfflictOnAttackTypeAttemptedCount((PromotionLineTypes)iI, 0);
		}
		if (m_defender->hasAfflictOnAttackType((PromotionLineTypes)iI))
		{
			m_defender->setAfflictOnAttackTypeAttemptedCount((PromotionLineTypes)iI, 0);
		}
	}
#endif
	//  Determine Attack Withdraw odds
	int iHitLimitThem = m_defender->getMaxHP() - m_attacker->combatLimit(m_defender);

	int iNeededRoundsUs = (iDefenderDamage == 0 ? MAX_INT : (std::max(0, m_defender->getHP() - iHitLimitThem) + iDefenderDamage - 1 ) / iDefenderDamage);
	int iNeededRoundsThem = (iAttackerDamage == 0 ? MAX_INT : (std::max(0, m_attacker->getHP()) + iAttackerDamage - 1 ) / iAttackerDamage);
	int iAttackerWithdraw = m_attacker->withdrawVSOpponentProbTotal(m_defender, m_plot);
	int iDefenderPursuit = m_defender->pursuitVSOpponentProbTotal(m_attacker);
	int iAttackerEarly = m_attacker->earlyWithdrawTotal();

	int AdjustedAttWithdrawalstep1 = iAttackerWithdraw - iDefenderPursuit;
	int AdjustedAttWithdrawalstep2 = std::min(100, AdjustedAttWithdrawalstep1);
	int AdjustedAttWithdrawal = std::max(0, AdjustedAttWithdrawalstep2);

	int expectedrndcnt = std::min(iNeededRoundsUs, iNeededRoundsThem);
	int expectedrnds = ((expectedrndcnt * iAttackerEarly)/100);

	int y = AdjustedAttWithdrawal;
	int z = AdjustedAttWithdrawal;
	int Time;
	for (Time = 0; Time < expectedrnds; ++Time)
	{
		z += ((AdjustedAttWithdrawal * y)/100);
		y = ((AdjustedAttWithdrawal * (100 - z))/100);	//	Prob next round is prob per round times prob you haven't already
	}

	int EvaluatedAttWithdrawOdds = z;


	int iAttackerKnockback = m_attacker->knockbackVSOpponentProbTotal(m_defender);
	int iDefenderUnyielding = m_defender->unyieldingTotal();
	int iAttackerKnockbackTries = m_attacker->knockbackRetriesTotal();

	int AdjustedKnockbackstep1 = iAttackerKnockback - iDefenderUnyielding;
	int AdjustedKnockbackstep2 = ((AdjustedKnockbackstep1 > 100) ? 100 : AdjustedKnockbackstep1);
	int AdjustedKnockback = ((AdjustedKnockbackstep2 < 0) ? 0 : AdjustedKnockbackstep2);

	y = AdjustedKnockback;
	z = AdjustedKnockback;

	for (Time = 0; Time < iAttackerKnockbackTries; ++Time)
	{
		z += ((AdjustedKnockback * y)/100);
		y = ((AdjustedKnockback * (100 - z))/100);	//	Prob next round is prob per round times prob you haven't already
	}

	int EvaluatedKnockbackOdds = z;
	//Original: int iAttackerKillOdds = iDefenderOdds * (100 - withdrawalProbability()) / 100;
	int iAttackerKillOdds = iDefenderOdds * (100 - (EvaluatedAttWithdrawOdds)) / 100;

	iAttackerKillOdds *= (100 - (EvaluatedKnockbackOdds));
	iAttackerKillOdds /= 100;
	//TB Combat Mods End

	if (m_attacker->isHuman() || m_defender->isHuman())
	{
		//Added ST
		CyArgsList pyArgsCD;
		pyArgsCD.add(gDLL->getPythonIFace()->makePythonObject(&cdAttackerDetails));
		pyArgsCD.add(gDLL->getPythonIFace()->makePythonObject(&cdDefenderDetails));
		pyArgsCD.add(getCombatOdds(m_attacker, m_defender));
		CvEventReporter::getInstance().genericEvent("combatLogCalc", pyArgsCD.makeFunctionArgs());
	}

	collateralCombat(m_plot, m_defender);

	int iCloseCombatRoundNum = -1;
	//bool bTryMobileWithdraw = false;	//if unit will be trying to withdraw from a plot it occupies
	//if (m_plot->getNumDefenders(m_defender->getOwner()) == 1 && m_defender->baseMoves() > baseMoves())	//must be faster than attacker
	//{
	//	bTryMobileWithdraw = true;
	//}
	int iWinningOdds = getCombatOdds(m_attacker, m_defender);
	bool bDefenderSkirmish = false; //iWinningOdds > 60;

	m_iTurnCount++;
	//Compile the Repel values only once as necessary
	int iDefenderRepel = m_defender->repelVSOpponentProbTotal(m_attacker);
	int iAttackerUnyielding = m_attacker->unyieldingTotal();

	int AdjustedRepelstep1 = iDefenderRepel - iAttackerUnyielding;
	int AdjustedRepelstep2 = ((AdjustedRepelstep1 > 100) ? 100 : AdjustedRepelstep1);
	int AdjustedRepel = ((AdjustedRepelstep2 < 0) ? 0 : AdjustedRepelstep2);
	bool bNoFurtherDamagetoDefender = false;


	int iDefenderWithdraw = m_defender->withdrawVSOpponentProbTotal(m_attacker, m_plot);
	int iAttackerPursuit = m_attacker->pursuitVSOpponentProbTotal(m_defender);
	int iDefenderEarlyWithdraw = m_defender->earlyWithdrawTotal();

	int AdjustedDefWithdrawstep1 = iDefenderWithdraw - iAttackerPursuit;
	int AdjustedDefWithdrawstep2 = ((AdjustedDefWithdrawstep1 > 100) ? 100 : AdjustedDefWithdrawstep1);
	int AdjustedDefWithdraw = ((AdjustedDefWithdrawstep2 < 0) ? 0 : AdjustedDefWithdrawstep2);

	bool bVanillaCombat = GC.getGame().isOption(GAMEOPTION_COMBAT_VANILLA_ENGINE);
	if (bVanillaCombat)
	{
		iAttackerStrength = m_attacker->currCombatStr(NULL, NULL, &cdAttackerDetails);
		iAttackerFirepower = m_attacker->currFirepower(NULL, NULL);
		m_attacker->getDefenderCombatValues(*m_defender, m_plot, iAttackerStrength, iAttackerFirepower, iDefenderOdds, iDefenderStrength, iAttackerDamage, iDefenderDamage, &cdDefenderDetails, m_defender);
		iDefenderHitChance = std::max(5, iDefenderOdds + ((iDefenderHitModifier * iDefenderOdds)/100));
		iAttackerHitChance = std::max(5, iAttackerOdds + ((iAttackerHitModifier * iAttackerOdds)/100));
	}

	while (true)
	{
		m_attacker->changeRoundCount(1);
		m_defender->changeRoundCount(1);
		if (!bVanillaCombat)
		{
			iAttackerStrength = m_attacker->currCombatStr(NULL, NULL, &cdAttackerDetails);
			iAttackerFirepower = m_attacker->currFirepower(NULL, NULL);
			m_attacker->getDefenderCombatValues(*m_defender, m_plot, iAttackerStrength, iAttackerFirepower, iDefenderOdds, iDefenderStrength, iAttackerDamage, iDefenderDamage, &cdDefenderDetails, m_defender);
			iDefenderHitChance = std::max(5, iDefenderOdds + ((iDefenderHitModifier * iDefenderOdds)/100));
			iAttackerHitChance = std::max(5, iAttackerOdds + ((iAttackerHitModifier * iAttackerOdds)/100));
		}

		//Check if this is a Breakdown Attack round and adjust the local bool so as to avoid reprocessing the Breakdown check multiple times per round.
		bBreakdown = m_attacker->isBreakdownCombat(m_plot, m_bSamePlot) && m_defender->getCombatFirstStrikes() == 0;

		if (m_attacker->getCombatPowerShots() > 0)
		{
			iAttackerPrecision += m_attacker->powerShotPrecisionModifierTotal();
		}
		if (m_defender->getCombatPowerShots() > 0)
		{
			iDefenderPrecision += m_defender->powerShotPrecisionModifierTotal();
		}
		iAttackerHitModifier = iAttackerPrecision - iDefenderDodge;
		iDefenderHitModifier = iDefenderPrecision - iAttackerDodge;
		iAttackerOdds = std::max((GC.getCOMBAT_DIE_SIDES() - iDefenderOdds), 0);
		iDefenderCombatRoll = GC.getGame().getSorenRandNum(GC.getCOMBAT_DIE_SIDES(), "DefenderCombatRoll");
		iAttackerCombatRoll = GC.getGame().getSorenRandNum(GC.getCOMBAT_DIE_SIDES(), "AttackerCombatRoll");
		WithdrawalRollResult = GC.getGame().getSorenRandNum(100, "Withdrawal");
		DefenderWithdrawalRollResult = GC.getGame().getSorenRandNum(100, "DefenderWithdrawal");
		RepelRollResult = GC.getGame().getSorenRandNum(100, "Repel");
		PursuitRollResult = GC.getGame().getSorenRandNum(100, "Pursuit");
		//Breakdown attack round?  If so we make the damage the defender would be dealt 0 and the chance of the attcker
		//hitting absolute so as to get through all normal checks to roll the chance for damaging the defenses while the
		//unit really does not engage in any counterattack against the defender.
		//TB Breakdown Adjustment: Finding this is probably not appropriate.  Better to allow actual combat to take place though
		//we may need to reduce the strengths on Rams some...  I'll probably end up making this more what I was looking for
		//when I get into the H2H/Distance mechanism.
		//if (bBreakdown)
		//{
		//	iDefenderDamage = 0;
		//	iAttackerHitChance = 10000;
		//}

		//TB Combat Mods (Breakdown) begin
		//Changes: No longer requires any particular combat result to make happen - previously attacker had to hit and since it had originally been setup to always hit so long as first strikes weren't taking place, the ram was rarely doing much damage.
		//I had made all rams immune to first strike though I'd prefer not to at this point... I can take that away now and allow the first strike rounds to take place as intended.
		if (bBreakdown)
		{
			m_attacker->resolveBreakdownAttack(m_plot, AdjustedRepel);
			m_attacker->changeExperience100(10, MAX_INT, false, false, true);
		}
		bool bNeitherRanged = !m_defender->isRanged() && !m_attacker->isRanged();
		bool bDefenderRangedbutOutofFS = m_defender->isRanged() && m_defender->getCombatFirstStrikes() < 1;
		bool bDefenderNotRanged = m_defender->isRanged();
		bool bAttackerRangedbutOutofFS = m_defender->isRanged() && m_defender->getCombatFirstStrikes() < 1;
		bool bAttackerNotRanged = m_attacker->isRanged();
		if (
			bNeitherRanged
		|| (
				(bDefenderRangedbutOutofFS || bDefenderNotRanged)
				&&
				(bAttackerRangedbutOutofFS || bAttackerNotRanged)
			)
		)
		{
			m_bNeverMelee = false;
		}
		//Defender's attack round
		if (m_defender->getCombatStuns() == 0 && iDefenderCombatRoll < iDefenderHitChance)
		{
			if (m_attacker->getCombatFirstStrikes() == 0)
			{
				// TB Combat Mods Begin
				//	Attacker Attempts Withdrawal
				if (m_attacker->getDamage() + iAttackerDamage >= m_attacker->withdrawalHP(m_attacker->getMaxHP(), iAttackerEarly) && iAttackerWithdraw > 0)
				{
					if (WithdrawalRollResult < AdjustedAttWithdrawal)
					{
						m_attacker->flankingStrikeCombat(m_plot, iAttackerStrength, iAttackerFirepower, iAttackerKillOdds, iDefenderDamage, m_defender);
						bAttackerWithdrawn = true;

						if (!bDynamicXP)
						{
							m_attacker->changeExperience100(m_attacker->getExperiencefromWithdrawal(AdjustedAttWithdrawal) * 10 / 100, 100 * m_attacker->maxXPValue(m_defender), true, m_plot->getOwner() == m_attacker->getOwner(), true);

							int iExperience = 100 * m_defender->defenseXPValue() * iInitialAttackerStrength / iInitialDefenderStrength;
							iExperience = range(iExperience, 100 * GC.getMIN_EXPERIENCE_PER_COMBAT(), 100 * GC.getMAX_EXPERIENCE_PER_COMBAT());
							m_defender->changeExperience100(iExperience, 100 * m_defender->maxXPValue(m_attacker), true, m_plot->getOwner() == m_defender->getOwner(), true);
						}

						CvEventReporter::getInstance().combatRetreat(m_attacker, m_defender);
						m_bAttackerWithdraws = true;
						m_bDeathMessaged = false;
						break;
					}
					else if ((WithdrawalRollResult < iAttackerWithdraw) && (WithdrawalRollResult > AdjustedAttWithdrawal))
					{
						if (m_attacker->getDamage() + iAttackerDamage < m_attacker->getMaxHP())
						{
							m_bAttackerPursuedSustain = true;
						}
						else m_bAttackerPursued = true;
					}
				}
				//TB Combat Mod (Afflict) begin
				if (iAttackerDamage > 0)
				{
					m_bAttackerInjured = true;
				}
				//TB Combat Mod (Afflict) end
				m_attacker->changeDamage(iAttackerDamage, m_defender->getOwner());
				//TB Combat Mod begin
				m_attacker->checkForStun(iAttackerDamage, m_defender);
#ifdef OUTBREAKS_AND_AFFLICTIONS
				m_attacker->checkForCritical(iAttackerDamage, m_defender);
#endif
				if (m_defender->dealsColdDamage())
				{
					m_attacker->changeColdDamage(iAttackerDamage);
				}
				//TB Combat Mod end

				bAttackerHasLostNoHP = false;

				if (m_defender->getCombatFirstStrikes() > 0 && m_defender->isRanged())
				{
					kBattle.addFirstStrikes(BATTLE_UNIT_DEFENDER, 1);
					kBattle.addDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_RANGED, iAttackerDamage);
				}

				cdAttackerDetails.iCurrHitPoints = m_attacker->getHP();

				if (m_attacker->isHuman() || m_defender->isHuman())
				{
					CyArgsList pyArgs;
					pyArgs.add(gDLL->getPythonIFace()->makePythonObject(&cdAttackerDetails));
					pyArgs.add(gDLL->getPythonIFace()->makePythonObject(&cdDefenderDetails));
					pyArgs.add(1);
					pyArgs.add(iAttackerDamage);
					CvEventReporter::getInstance().genericEvent("combatLogHit", pyArgs.makeFunctionArgs());
				}
				//TB Combat Mods Begin (Repel)
				//within the 'after attacker's first strikes segment'

				if (m_defender->getCombatRepels() > 0)
				{
					if (RepelRollResult < AdjustedRepel)
					{
						m_bAttackerRepelled = true;
						m_bDeathMessaged = false;

						if (!bDynamicXP)
						{
							m_defender->changeExperience100(getExperiencefromWithdrawal(AdjustedRepel) * 15 / 100, 100 * m_defender->maxXPValue(m_attacker), true, m_plot->getOwner() == getOwner(), true);
						}
						break;
					}
					else if (RepelRollResult < iDefenderRepel && RepelRollResult > AdjustedRepel)
					{
						if ((m_defender->getDamage() + iDefenderDamage) < m_defender->getMaxHP())
						{
							m_bAttackerRefusedtoYieldSustain = true;
						}
						else m_bAttackerRefusedtoYield = true;
					}
				}
				if (m_defender->getCombatFirstStrikes() > 0 && m_defender->isRanged())
				{
					m_bDefenderHitAttackerWithDistanceAttack = true;
				}
#ifdef OUTBREAKS_AND_AFFLICTIONS
				if (GC.getGame().isOption(GAMEOPTION_COMBAT_OUTBREAKS_AND_AFFLICTIONS))
				{
					PROFILE("CvUnit::resolveCombat.Afflictions");

					for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
					{
						if (GC.getPromotionLineInfo((PromotionLineTypes)iI).isAffliction()
						&& !GC.getPromotionLineInfo((PromotionLineTypes)iI).isCritical())
						{
							const PromotionLineTypes eAfflictionLine = static_cast<PromotionLineTypes>(iI);

							if (m_defender->hasAfflictOnAttackType(eAfflictionLine)
							&&  m_defender->isAfflictOnAttackTypeImmediate(eAfflictionLine)
							&&  m_defender->isAfflictOnAttackTypeAttempted(eAfflictionLine) == false)
							{
								if (m_defender->getCombatFirstStrikes() > 0
								&&  m_defender->isAfflictOnAttackTypeDistance(eAfflictionLine)
								&&  m_defender->isRanged()
								||
								   (m_defender->getCombatFirstStrikes() < 1 || !m_defender->isRanged())
								&&  m_defender->isAfflictOnAttackTypeMelee(eAfflictionLine)
								&& (m_attacker->getCombatFirstStrikes() < 1 || !m_attacker->isRanged()))
								{
									const int iDefendersPoisonChance = (
										m_defender->getAfflictOnAttackTypeProbability(eAfflictionLine)
										- m_attacker->fortitudeTotal()
										- m_attacker->getUnitAfflictionTolerance(eAfflictionLine)
									);
									if (GC.getGame().getSorenRandNum(100, "DefendersPoisonRoll") < iDefendersPoisonChance)
									{
										m_attacker->afflict(eAfflictionLine, true, m_defender);
									}
									m_defender->changeAfflictOnAttackTypeAttemptedCount(eAfflictionLine, 1);
								}
							}
						}
					}
				}
#endif // OUTBREAKS_AND_AFFLICTIONS
			}
		}
		//Attacker's attack round
		if (m_attacker->getCombatStuns() == 0 && ((bVanillaCombat && iDefenderCombatRoll >= iDefenderHitChance) || (iAttackerCombatRoll < iAttackerHitChance)))
		{
			if (m_defender->getCombatFirstStrikes() == 0)
			{
				if (GC.getGame().isModderGameOption(MODDERGAMEOPTION_DEFENDER_WITHDRAW))
				{
					iCloseCombatRoundNum++;
				}
				//TB Combat Mods Begin
				//Attacker attempts withdrawal due to combatlimit
				if (std::min(m_defender->getMaxHP(), m_defender->getDamage() + iDefenderDamage) > m_attacker->combatLimit(m_defender)
				&& PursuitRollResult > m_defender->pursuitVSOpponentProbTotal(m_attacker))
				{
					if (!bBreakdown || m_attacker->getDamage() > m_attacker->combatLimit(m_attacker))
					{
						if (!bDynamicXP)
						{
							m_attacker->changeExperience100(
								m_attacker->getExperiencefromWithdrawal(100 - m_defender->pursuitVSOpponentProbTotal(m_attacker)),
								100 * m_attacker->maxXPValue(m_defender),
								true, m_plot->getOwner() == m_attacker->getOwner(), true
							);
							const int iExperience = (
								range(
									100 * m_defender->defenseXPValue() * iInitialAttackerStrength / iInitialDefenderStrength,
									100 * GC.getMIN_EXPERIENCE_PER_COMBAT(),
									100 * GC.getMAX_EXPERIENCE_PER_COMBAT()
								)
							);
							m_defender->changeExperience100(iExperience, 100 * m_defender->maxXPValue(m_attacker), true, m_plot->getOwner() == m_defender->getOwner(), true);
						}

						if (temporarypursuit == 0)
						{
							m_bDeathMessaged = false;
							m_defender->setDamage(m_attacker->combatLimit(m_defender), m_attacker->getOwner());
							if (m_attacker->dealsColdDamage())
							{
								m_defender->setColdDamage(m_attacker->combatLimit(m_defender));
							}
						}
						temporarypursuit = 0;
						break;
					}
					else
					{
						bNoFurtherDamagetoDefender = true;
					}
				}
				else if (std::min(m_defender->getMaxHP(), m_defender->getDamage() + iDefenderDamage) > m_attacker->combatLimit(m_defender)
				&& PursuitRollResult < m_defender->pursuitVSOpponentProbTotal(m_attacker)
				&& m_defender->pursuitVSOpponentProbTotal(m_attacker) > 0)
				{
					if (!bBreakdown || m_attacker->getDamage() > m_attacker->combatLimit(m_attacker))
					{
						temporarypursuit += 1;
						m_bAttackerPursuedSustain = true;
					}
				}
				// Current Code (Defender Attempts Withdrawal):

				if  ((!m_plot->isCity(true, m_defender->getTeam()) || m_bSamePlot)
				&& GC.getGame().isModderGameOption(MODDERGAMEOPTION_DEFENDER_WITHDRAW)
				&& (bDefenderSkirmish || m_defender->getDamage() + iDefenderDamage >= m_attacker->withdrawalHP(m_defender->getMaxHP(), iDefenderEarlyWithdraw))
				&& !m_attacker->isSuicide()
				&& iCloseCombatRoundNum > 0
				&& m_defender->withdrawVSOpponentProbTotal(m_attacker, m_plot) > 0)	//can not to escape at close combat round 1
				{
					if (DefenderWithdrawalRollResult < AdjustedDefWithdraw)
					{
						bst::optional<CvPlot*> withdrawPlot = selectWithdrawPlot(m_bSamePlot, m_defender);
						if (withdrawPlot)
						{
							m_plotWithdrawal = *withdrawPlot;
							m_bDefenderWithdrawn = true;
							m_bDeathMessaged = false;

							if (bDynamicXP)
							{
								m_attacker->doDynamicXP(m_defender, m_plot, iAttackerInitialDamage, iWinningOdds, iDefenderInitialDamage);
							}
							else
							{
								m_defender->changeExperience100(m_attacker->getExperiencefromWithdrawal(AdjustedDefWithdraw) * 10 / 100, 100 * m_defender->maxXPValue(m_attacker), true, m_plot->getOwner() == m_defender->getOwner(), true);
								m_attacker->changeExperience100(10, 100 * m_attacker->maxXPValue(m_defender), true, m_plot->getOwner() == m_attacker->getOwner(), true);
							}
							return;
						}
					}
					else if (DefenderWithdrawalRollResult < iDefenderWithdraw && DefenderWithdrawalRollResult > AdjustedDefWithdraw)
					{
						if ((m_defender->getDamage() + iDefenderDamage) < m_defender->getMaxHP())
						{
							m_bDefenderPursuedSustain = true;
						}
						else m_bDefenderPursued = true;
					}
				}

				if (iDefenderDamage > 0)
				{
					if (!bNoFurtherDamagetoDefender)
					{
						m_bDefenderInjured = true;
						m_defender->changeDamage(iDefenderDamage, m_attacker->getOwner());
					}
				}

				if (!bBreakdown)
				{
					m_defender->checkForStun(iDefenderDamage, m_attacker);
#ifdef OUTBREAKS_AND_AFFLICTIONS
					m_defender->checkForCritical(iDefenderDamage, m_attacker);
#endif
					if (m_attacker->dealsColdDamage())
					{
						m_defender->changeColdDamage(iDefenderDamage);
					}
				}

				if (m_attacker->getCombatFirstStrikes() > 0 && m_attacker->isRanged())
				{
					kBattle.addFirstStrikes(BATTLE_UNIT_ATTACKER, 1);
					kBattle.addDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_RANGED, iDefenderDamage);
				}

				cdDefenderDetails.iCurrHitPoints=m_defender->getHP();

				if (m_attacker->isHuman() || m_defender->isHuman())
				{
					CyArgsList pyArgs;
					pyArgs.add(gDLL->getPythonIFace()->makePythonObject(&cdAttackerDetails));
					pyArgs.add(gDLL->getPythonIFace()->makePythonObject(&cdDefenderDetails));
					pyArgs.add(0);
					pyArgs.add(iDefenderDamage);
					CvEventReporter::getInstance().genericEvent("combatLogHit", pyArgs.makeFunctionArgs());
				}

				//TB Combat Mods (Knockback and attacker affliction)
				if (!bBreakdown && !m_defender->isDead())
				{
					if (!m_bSamePlot)
					{
						int KnockbackRollResult = GC.getGame().getSorenRandNum(100, "Knockback");

						if (m_attacker->getCombatKnockbacks() > 0)
						{
							if (KnockbackRollResult < AdjustedKnockback)
							{
								m_bDefenderKnockedBack = true;
								m_bDeathMessaged = false;
								m_plotWithdrawal = selectWithdrawPlot(m_bSamePlot, m_defender).get_value_or(nullptr);

								if (bDynamicXP)
								{
									m_attacker->doDynamicXP(m_defender, m_plot, iAttackerInitialDamage, iWinningOdds, iDefenderInitialDamage);
								}
								else
								{
									m_attacker->changeExperience100(
										m_attacker->getExperiencefromWithdrawal(AdjustedKnockback) * 15 / 100,
										100 * m_attacker->maxXPValue(m_defender), true,
										m_plot->getOwner() == m_attacker->getOwner(), true
									);
								}
								return;
							}
							else if ((KnockbackRollResult < iAttackerKnockback) && (KnockbackRollResult > AdjustedKnockback))
							{
								if ((m_defender->getDamage() + iDefenderDamage) < m_attacker->getMaxHP())
								{
									m_bDefenderRefusedtoYieldSustain = true;
								}
								else
								{
									m_bDefenderRefusedtoYield = true;
								}
							}
						}
					}
					if (m_attacker->getCombatFirstStrikes() > 0 && m_attacker->isRanged())
					{
						m_bAttackerHitDefenderWithDistanceAttack = true;
					}
#ifdef OUTBREAKS_AND_AFFLICTIONS
					if (GC.getGame().isOption(GAMEOPTION_COMBAT_OUTBREAKS_AND_AFFLICTIONS))
					{
						for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
						{
							if (GC.getPromotionLineInfo((PromotionLineTypes)iI).isAffliction()
							&& !GC.getPromotionLineInfo((PromotionLineTypes)iI).isCritical())
							{
								const PromotionLineTypes eAfflictionLine = static_cast<PromotionLineTypes>(iI);
								if (m_attacker->hasAfflictOnAttackType(eAfflictionLine)
								&&  m_attacker->isAfflictOnAttackTypeImmediate(eAfflictionLine)
								&&  m_attacker->isAfflictOnAttackTypeAttempted(eAfflictionLine) == false)
								{
									if (m_attacker->getCombatFirstStrikes() > 0
									&&  m_attacker->isAfflictOnAttackTypeDistance(eAfflictionLine)
									&&  m_attacker->isRanged()
									||
									   (m_attacker->getCombatFirstStrikes() < 1 || !m_attacker->isRanged())
									&&  m_attacker->isAfflictOnAttackTypeMelee(eAfflictionLine)
									&& (m_defender->getCombatFirstStrikes() < 1 || !m_defender->isRanged()))
									{
										const int iAttackersPoisonChance = (
											getAfflictOnAttackTypeProbability(eAfflictionLine)
											- m_defender->fortitudeTotal()
											- m_defender->getUnitAfflictionTolerance(eAfflictionLine)
										);
										if (GC.getGame().getSorenRandNum(100, "AttackersPoisonRoll") < iAttackersPoisonChance)
										{
											m_defender->afflict(eAfflictionLine, true, m_attacker);
										}
										m_attacker->changeAfflictOnAttackTypeAttemptedCount(eAfflictionLine, 1);
									}
								}
							}
						}
					}
#endif // OUTBREAKS_AND_AFFLICTIONS
				}
			}
		}
		//TB Combat Mods begin
		if (m_defender->getCombatFirstStrikes() == 0 && m_attacker->getCombatPowerShots() > 0)
		{
			if (!bBreakdown)
			{
				m_attacker->changeCombatPowerShots(-1);
			}
		}

		if (m_attacker->getCombatFirstStrikes() == 0 && m_defender->getCombatPowerShots() > 0)
		{
			m_defender->changeCombatPowerShots(-1);
		}

		if (!bBreakdown && !m_bSamePlot
		&& m_attacker->getCombatKnockbacks() > 0
		&& m_defender->getCombatFirstStrikes() == 0)
		{
			m_attacker->changeCombatKnockbacks(-1);
		}

		if (m_defender->getCombatRepels() > 0 && m_attacker->getCombatFirstStrikes() == 0)
		{
			m_defender->changeCombatRepels(-1);
		}

		if (!bBreakdown && m_attacker->getCombatStuns() > 0)
		{
			m_attacker->changeCombatStuns(-1);
		}

		if (m_defender->getCombatStuns() > 0)
		{
			m_defender->changeCombatStuns(-1);
		}
		//TB Combat Mods end
		if (m_defender->getCombatStuns() == 0 && m_attacker->getCombatFirstStrikes() > 0)
		{
			changeCombatFirstStrikes(-1);
		}

		if (m_attacker->getCombatStuns() == 0 && m_defender->getCombatFirstStrikes() > 0)
		{
			m_defender->changeCombatFirstStrikes(-1);
		}

		if (m_attacker->isDead() || m_defender->isDead())
		{
			if (m_attacker->isDead())
			{
				if (!bDynamicXP)
				{
					int iExperience = m_defender->defenseXPValue() * iInitialAttackerStrength / iInitialDefenderStrength;
					iExperience = range(iExperience, GC.getMIN_EXPERIENCE_PER_COMBAT(), GC.getMAX_EXPERIENCE_PER_COMBAT());
					m_defender->changeExperience(iExperience, m_defender->maxXPValue(m_attacker), true, m_plot->getOwner() == m_defender->getOwner(), true);
				}
				// Koshling - add rolling history of combat results to allow the AI to adapt to what it sees happening
				m_plot->area()->recordCombatDeath(m_attacker->getOwner(), m_attacker->getUnitType(), m_defender->getUnitType());
			}
			else
			{
				//TB Note: Place again in the successful withdrawal segment if its not already there.  This may need debugging as well based on reports.
				m_attacker->flankingStrikeCombat(m_plot, iAttackerStrength, iAttackerFirepower, iAttackerKillOdds, iDefenderDamage, m_defender);

				if (!bDynamicXP)
				{
					int iExperience = m_attacker->attackXPValue() * iInitialDefenderStrength / std::max(1, iInitialAttackerStrength);
					iExperience = range(iExperience, GC.getMIN_EXPERIENCE_PER_COMBAT(), GC.getMAX_EXPERIENCE_PER_COMBAT());
					m_attacker->changeExperience(iExperience, m_attacker->maxXPValue(m_defender), true, m_plot->getOwner() == m_attacker->getOwner(), true);
				}
				// Koshling - add rolling history of combat results to allow the AI to adapt to what it sees happening
				m_plot->area()->recordCombatDeath(m_defender->getOwner(), m_defender->getUnitType(), m_attacker->getUnitType());
			}
			break;
		}
	}

	bool bPromotion = false;
	bool bDefPromotion = false;
	//TB Note: for both doBattleFieldPromotions and doDynamicXP, the iWinningOdds needs adjusted by YOUR ability to withdraw - if you have withdrawn at least.  Check the instance there.
	int iNonLethalAttackWinChance = std::max(0, std::max(AdjustedAttWithdrawal, AdjustedKnockback));
	int iNonLethalDefenseWinChance = std::max(0, std::max(AdjustedDefWithdraw, AdjustedRepel));
	m_attacker->doBattleFieldPromotions(
		m_defender, cdDefenderDetails, m_plot,
		bAttackerHasLostNoHP, bAttackerWithdrawn,
		iAttackerInitialDamage, iWinningOdds,
		iInitialAttXP, iInitialAttGGXP, iDefenderInitialDamage,
		iInitialDefXP, iInitialDefGGXP, bPromotion, bDefPromotion,
		iNonLethalAttackWinChance, iNonLethalDefenseWinChance,
		iDefenderFirstStrikes, iAttackerFirstStrikes
	);
	if (bDynamicXP)
	{
		m_attacker->doDynamicXP(m_defender, m_plot, iAttackerInitialDamage, iWinningOdds, iDefenderInitialDamage, bPromotion, bDefPromotion);
	}
}


void CombatData::endCombat()
{
	if (m_attacker->getDomainType() == DOMAIN_AIR)
	{
		m_attacker->endAirCombat();
		return;
	}

	if (!m_defender)
	{
		m_attacker->setAttackPlot(NULL, false);
		m_attacker->setCombatUnit(NULL);
		m_attacker->setCombatTimer(0);
		FErrorMsg("Defender is expected to be exist when it's in the middle of combat...");
		return;
	}

	if (!m_plot)
	{
		FErrorMsg("Plot to attack is expected to exist when ending combat...");
		return;
	}

	const bool bHuman = m_attacker->isHuman();
	const bool bHumanDefender = m_defender->isHuman();

	const PlayerTypes eAttacker = m_attacker->getVisualOwner(m_defender->getTeam());
	const PlayerTypes eDefender = m_defender->getVisualOwner(getTeam());

#ifdef STRENGTH_IN_NUMBERS
	//TB Combat Mod begin
	if (GC.getGame().isOption(GAMEOPTION_COMBAT_STRENGTH_IN_NUMBERS))
	{
		m_attacker->ClearSupports();
	}
#endif // STRENGTH_IN_NUMBERS

#ifdef OUTBREAKS_AND_AFFLICTIONS
	//TB Combat Mod (Afflict) begin
	if (GC.getGame().isOption(GAMEOPTION_COMBAT_OUTBREAKS_AND_AFFLICTIONS))
	{
		for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
		{
			if (m_bAttackerInjured
			&&
			(
				m_defender->isDead()
				|| m_bDefenderWithdrawn
				|| m_bDefenderKnockedBack
				|| m_bAttackerRepelled
				|| m_bAttackerWithdraws
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
					m_bDefenderHitAttackerWithDistanceAttack
					&& m_defender->isAfflictOnAttackTypeDistance(eAfflictionLine)
					|| m_defender->isAfflictOnAttackTypeMelee(eAfflictionLine)
					&& !m_bNeverMelee
				))
				{
					const int iDefendersPoisonChance =
					(
						m_defender->getAfflictOnAttackTypeProbability(eAfflictionLine)
						- m_attacker->fortitudeTotal() - m_attacker->getUnitAfflictionTolerance(eAfflictionLine)
					);
					if (GC.getGame().getSorenRandNum(100, "DefendersPoisonRoll") < iDefendersPoisonChance)
					{
						m_attacker->afflict(eAfflictionLine, true, m_defender);
					}
					m_defender->setAfflictOnAttackTypeAttemptedCount(eAfflictionLine, 1);
				}
				//Battle proximity communicability affliction spread
				if (!m_bNeverMelee
				&&  m_defender->hasAfflictionLine(eAfflictionLine)
				&&  GC.getPromotionLineInfo(eAfflictionLine).getCommunicability() > 0
				&& !GC.getPromotionLineInfo(eAfflictionLine).isNoSpreadonBattle())
				{
					const int iDefendersChancetoAfflict = m_attacker->getChancetoContract(eAfflictionLine) + m_defender->worsenedProbabilitytoAfflict(eAfflictionLine);

					if (GC.getGame().getSorenRandNum(100, "DefenderCauseContractRoll") < iDefendersChancetoAfflict)
					{
						m_attacker->afflict(eAfflictionLine, false, m_defender);
					}
				}
			}
			if (m_bDefenderInjured
			&&
			(
				m_attacker->isDead()
				|| m_bDefenderWithdrawn
				|| m_bDefenderKnockedBack
				|| m_bAttackerRepelled
				|| m_bAttackerWithdraws
			)
			&& GC.getPromotionLineInfo((PromotionLineTypes)iI).isAffliction()
			&& !GC.getPromotionLineInfo((PromotionLineTypes)iI).isCritical())
			{
				const PromotionLineTypes eAfflictionLine = (PromotionLineTypes)iI;
				if (m_attacker->hasAfflictOnAttackType(eAfflictionLine)
				&& !m_attacker->isAfflictOnAttackTypeAttempted(eAfflictionLine)
				&&
				(
					m_bDefenderHitAttackerWithDistanceAttack
					&& m_attacker->isAfflictOnAttackTypeDistance(eAfflictionLine)
					|| m_attacker->isAfflictOnAttackTypeMelee(eAfflictionLine)
					&& !m_bNeverMelee
				))
				{
					const int iAttackersPoisonChance =
					(
						m_attacker->getAfflictOnAttackTypeProbability(eAfflictionLine)
						- m_defender->fortitudeTotal()
						- m_defender->getUnitAfflictionTolerance(eAfflictionLine)
					);
					if (GC.getGame().getSorenRandNum(100, "AttackersPoisonRoll") < iAttackersPoisonChance)
					{
						m_defender->afflict(eAfflictionLine, true, m_attacker);
					}
					m_attacker->setAfflictOnAttackTypeAttemptedCount(eAfflictionLine, 1);
				}
				//Communicability exposure
				if (!m_bNeverMelee
				&&  m_attacker->hasAfflictionLine(eAfflictionLine)
				&&  GC.getPromotionLineInfo(eAfflictionLine).getCommunicability() > 0
				&& !GC.getPromotionLineInfo(eAfflictionLine).isNoSpreadonBattle())
				{
					const int iAttackersChancetoAfflict = m_defender->getChancetoContract(eAfflictionLine) + m_attacker->worsenedProbabilitytoAfflict(eAfflictionLine);

					if (GC.getGame().getSorenRandNum(100, "AttackerCauseContractRoll") < iAttackersChancetoAfflict)
					{
						m_defender->afflict(eAfflictionLine, false, m_attacker);
					}
				}
			}
		}
	}
	//TB Combat Mod (Afflict) end
#endif // OUTBREAKS_AND_AFFLICTIONS

	//TB Combat Mod (Stampede/Onslaught)
	if (m_defender->isDead() || m_bDefenderWithdrawn || m_bDefenderKnockedBack || m_bAttackerRepelled || m_bAttackerWithdraws)
	{
		if (!m_bSamePlot && m_attacker->canStampede() && m_plot->getNumVisiblePotentialEnemyDefenders(m_attacker) > 1)
		{
			m_bAttackerStampedes = true;
		}
		if (!m_bSamePlot
		&& m_attacker->canOnslaught()
		&& m_attacker->getDamage() == 0
		&& m_plot->getNumVisiblePotentialEnemyDefenders(m_attacker) > 1)
		{
			m_bAttackerOnslaught = true;
		}
	}
	if (m_attacker->isDead() || m_defender->isDead() || m_bDefenderWithdrawn || m_bDefenderKnockedBack || m_bAttackerRepelled || m_bAttackerWithdraws)
	{
		//Sustain Reports
		//Pursuit Reports
		if (m_bAttackerPursuedSustain)
		{
			if (bHuman)
			{
				if (BARBARIAN_PLAYER != eDefender)
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_PURSUED_SUSTAIN_HUMAN", m_attacker->getNameKey(), m_defender->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_PURSUED_SUSTAIN_HUMAN_HIDDEN", m_attacker->getNameKey(), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			if (bHumanDefender)
			{
				if (BARBARIAN_PLAYER != eAttacker)
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_PURSUED_SUSTAIN", m_attacker->getVisualCivAdjective(m_attacker->getTeam()), m_attacker->getNameKey(), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_PURSUED_SUSTAIN_HIDDEN", m_attacker->getNameKey(), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			m_bAttackerPursuedSustain = false;
		}
		if (m_bDefenderPursuedSustain)
		{
			if (bHuman)
			{
				if (BARBARIAN_PLAYER != eDefender)
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText(
							"TXT_KEY_MISC_DEFENDER_PURSUED_SUSTAIN_HUMAN",
							getNameKey(), m_defender->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()
						),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_DEFENDER_PURSUED_SUSTAIN_HUMAN_HIDDEN", m_attacker->getNameKey(), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			if (bHumanDefender)
			{

				if (BARBARIAN_PLAYER != eAttacker)
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_DEFENDER_PURSUED_SUSTAIN",
						m_defender->getNameKey(), m_attacker->getVisualCivAdjective(getTeam()), m_attacker->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_DEFENDER_PURSUED_SUSTAIN_HIDDEN", m_defender->getNameKey(), m_attacker->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			m_bDefenderPursuedSustain = false;
		}

		//Unyielding Reports
		if (m_bAttackerRefusedtoYieldSustain)
		{
			if (bHuman)
			{
				if (BARBARIAN_PLAYER != eAttacker)
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText(
							"TXT_KEY_MISC_ATTACKER_UNYIELDING_SUSTAIN_HUMAN",
							m_attacker->getNameKey(), m_attacker->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()
						),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_UNYIELDING_SUSTAIN_HUMAN_HIDDEN", m_attacker->getNameKey(), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			if (bHumanDefender)
			{
				if (BARBARIAN_PLAYER != eAttacker)
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_UNYIELDING_SUSTAIN",
						m_attacker->getVisualCivAdjective(getTeam()), m_defender->getNameKey(), m_attacker->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_UNYIELDING_SUSTAIN_HIDDEN",
						m_defender->getNameKey(), m_attacker->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			m_bAttackerRefusedtoYieldSustain = false;
		}
		if (m_bDefenderRefusedtoYieldSustain)
		{
			if (bHuman)
			{
				if (BARBARIAN_PLAYER != eDefender)
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_DEFENDER_UNYIELDING_SUSTAIN_HUMAN",
						m_attacker->getNameKey(), m_attacker->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_DEFENDER_UNYIELDING_SUSTAIN_HUMAN_HIDDEN", m_attacker->getNameKey(), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			if (bHumanDefender)
			{
				if (BARBARIAN_PLAYER != eAttacker)
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_DEFENDER_UNYIELDING_SUSTAIN",
						m_defender->getNameKey(), m_attacker->getVisualCivAdjective(getTeam()), m_attacker->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_DEFENDER_UNYIELDING_SUSTAIN_HIDDEN", m_defender->getNameKey(), m_attacker->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			m_bDefenderRefusedtoYieldSustain = false;
		}
	}
	//TB Combat Mods End
	//end the combat mission if this code executes first
	if (!m_attacker->isUsingDummyEntities() && m_attacker->isInViewport())
	{
		gDLL->getEntityIFace()->RemoveUnitFromBattle(m_attacker);
	}
	if (!m_defender->isUsingDummyEntities() && m_defender->isInViewport())
	{
		gDLL->getEntityIFace()->RemoveUnitFromBattle(m_defender);
	}
	m_attacker->setAttackPlot(NULL, false);
	m_attacker->setCombatUnit(NULL);
	m_defender->setCombatUnit(NULL);

	m_attacker->NotifyEntity(MISSION_DAMAGE);
	m_defender->NotifyEntity(MISSION_DAMAGE);

	if (m_attacker->isDead())
	{
		if (m_attacker->isNPC())
		{
			GET_PLAYER(m_defender->getOwner()).changeWinsVsBarbs(1);
		}

		if (!m_attacker->isHiddenNationality() && !m_defender->isHiddenNationality())
		{
			const int attackerWarWearinessChangeTimes100 = std::max(1, 100 * GC.getDefineINT("WW_UNIT_KILLED_ATTACKING") * (m_attacker->getMaxHP() - m_attacker->getPreCombatDamage()) / m_attacker->getMaxHP());
			GET_TEAM(m_attacker->getTeam()).changeWarWearinessTimes100(m_defender->getTeam(), *m_plot, attackerWarWearinessChangeTimes100);

			const int defenderWarWearinessChangeTimes100 = 100*GC.getDefineINT("WW_KILLED_UNIT_DEFENDING")*(m_defender->getDamage() - m_defender->getPreCombatDamage())/m_defender->getMaxHP();
			GET_TEAM(m_defender->getTeam()).changeWarWearinessTimes100(m_attacker->getTeam(), *m_plot, defenderWarWearinessChangeTimes100);

			GET_TEAM(m_defender->getTeam()).AI_changeWarSuccess(m_attacker->getTeam(), GC.getDefineINT("WAR_SUCCESS_DEFENDING"));
		}

		const int iInfluenceRatio = GC.isIDW_ENABLED() ? m_defender->doVictoryInfluence(m_attacker, false, false) : 0;

		if (bHuman)
		{
			CvWString szBuffer;

			if (m_bAttackerPursued)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_PURSUIT_VICTIM", m_attacker->getNameKey(), m_defender->getNameKey());
			}
			else if (m_bAttackerRefusedtoYield)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNYIELDING_UNIT_DIED_ATTACKING", m_attacker->getNameKey(), m_defender->getNameKey());
			}
			else if (m_bDefenderRefusedtoYield)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_DIED_ATTACKING_UNYIELDING", m_attacker->getNameKey(), m_defender->getNameKey());
			}
			else szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_DIED_ATTACKING", m_attacker->getNameKey(), m_defender->getNameKey());

			if (iInfluenceRatio > 0)
			{
				szBuffer = szBuffer + CvString::format(" %s: -%.1f%%", gDLL->getText("TXT_KEY_TILE_INFLUENCE").GetCString(), ((float)iInfluenceRatio)/10);
			}
			AddDLLMessage(
				m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer,
				GC.getEraInfo(GC.getGame().getCurrentEra()).getAudioUnitDefeatScript(),
				MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_RED(), m_plot->getX(), m_plot->getY()
			);
		}
		if (bHumanDefender)
		{
			CvWString szBuffer;

			if (m_bAttackerPursued)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_PURSUIT_SUCCESS", m_attacker->getNameKey(), m_defender->getNameKey());
			}
			else if (m_bAttackerRefusedtoYield)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_UNYIELDING_ATTACKER_YOU_KILLED", m_attacker->getNameKey(), m_defender->getNameKey());
			}
			else if (m_bDefenderRefusedtoYield)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_KILLED_ATTACKER_WHILEUNYIELDING", m_attacker->getNameKey(), m_defender->getNameKey());
			}
			else if (BARBARIAN_PLAYER != eAttacker)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_KILLED_ENEMY_UNIT", m_defender->getNameKey(), m_attacker->getNameKey(), m_attacker->getVisualCivAdjective(m_defender->getTeam()));
			}
			else szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_KILLED_ENEMY_UNIT_HIDDEN", m_defender->getNameKey(), m_attacker->getNameKey());

			if (iInfluenceRatio > 0)
			{
				szBuffer = szBuffer + CvString::format(" %s: +%.1f%%", gDLL->getText("TXT_KEY_TILE_INFLUENCE").GetCString(), ((float)iInfluenceRatio)/10);
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
			foreach_(const CvPlot* plotX, m_plot->adjacent() | filtered(CvPlot::fn::area() == m_plot->area()))
			{
				foreach_(CvUnit* unitX, plotX->units())
				{
					if (unitX->getTeam() == m_defender->getTeam() && unitX->isHurt())
					{
						iUnitsHealed++;
						unitX->doHeal();
					}
				}
			}
		}

		if (m_defender->getVictoryStackHeal() > 0
		&& GC.getGame().getSorenRandNum(100, "Field Surgeon Die Roll") <= m_defender->getVictoryStackHeal())
		{
			foreach_(CvUnit* unitX, m_plot->units())
			{
				if (unitX->getTeam() == m_defender->getTeam() && unitX->isHurt())
				{
					iUnitsHealed++;
					unitX->doHeal();
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
			if (bHuman)
			{
				if (BARBARIAN_PLAYER != eAttacker)
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ENEMY_FIELD_MEDIC_DEFENDERS", GET_PLAYER(m_defender->getOwner()).getCivilizationAdjective()),
						"AS2D_COMBAT", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_YELLOW(), m_defender->getX(), m_defender->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_MISC_ENEMY_FIELD_MEDIC_DEFENDERS_HIDDEN"),
						"AS2D_COMBAT", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_YELLOW(), m_defender->getX(), m_defender->getY()
					);
				}
			}
			if (bHumanDefender)
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_FIELD_MEDIC_DEFENDERS", m_defender->getNameKey(), iUnitsHealed),
					"AS2D_POSITIVE_DINK", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_GREEN(), m_defender->getX(), m_defender->getY()
				);
			}
		}

		if (m_defender->getDefensiveVictoryMoveCount() > 0)
		{
			m_defender->changeMoves(-GC.getMOVE_DENOMINATOR());
		}

		if (m_attacker->getSurvivorChance() > 0
		&& GC.getGame().getSorenRandNum(100, "Too Badass Check") <= m_attacker->getSurvivorChance())
		{
			m_attacker->setSurvivor(true);
			m_attacker->jumpToNearestValidPlot();
		}
		else if (m_attacker->isOneUp())
		{
			m_attacker->setCanRespawn(true);
		}

		if (m_defender->isPillageOnVictory())
		{
			CvPlot* m_defenderPlot = m_defender->plot();
			int iPillageGold = m_attacker->getLevel() * m_attacker->getExperience();
			if (NO_UNIT != m_attacker->getLeaderUnitType())
			{
				iPillageGold *= m_attacker->getLevel();
			}

			if (iPillageGold > 0)
			{
				iPillageGold += (iPillageGold * m_defender->getPillageChange()) / 100;
				GET_PLAYER(m_defender->getOwner()).changeGold(iPillageGold);

				if (bHuman)
				{
					if (BARBARIAN_PLAYER != eDefender)
					{
						AddDLLMessage(
							m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
							gDLL->getText("TXT_KEY_MISC_GOLD_LOOTED_FROM_DEAD", m_attacker->getNameKey(), m_defender->getVisualCivAdjective(m_defender->getTeam())),
							"AS2D_PILLAGED", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_YELLOW(), m_defenderPlot->getX(), m_defenderPlot->getY()
						);
					}
					else
					{
						AddDLLMessage(
							m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
							gDLL->getText("TXT_KEY_MISC_GOLD_LOOTED_FROM_DEAD_HIDDEN", m_attacker->getNameKey()),
							"AS2D_PILLAGED", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_YELLOW(), m_defenderPlot->getX(), m_defenderPlot->getY()
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
							if (bHumanDefender)
							{
								AddDLLMessage(
									m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText("TXT_KEY_MISC_MARAUDERS_PLUNDERED_VICTIMS", iPillageGold, m_defender->getNameKey()),
									"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_GREEN(), m_defenderPlot->getX(), m_defenderPlot->getY()
								);
							}
							if (bHuman)
							{
								if (BARBARIAN_PLAYER != eDefender)
								{
									AddDLLMessage(
										m_attacker->getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
										gDLL->getText("TXT_KEY_MISC_DEFENDED_BY_MARAUDERS", m_attacker->getNameKey(), m_attacker->getVisualCivAdjective(m_defender->getTeam())),
										"AS2D_PILLAGED", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_YELLOW(), m_defenderPlot->getX(), m_defenderPlot->getY(), true, true
									);
								}
								else
								{
									AddDLLMessage(
										m_attacker->getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
										gDLL->getText("TXT_KEY_MISC_DEFENDED_BY_MARAUDERS_HIDDEN", m_attacker->getNameKey()),
										"AS2D_PILLAGED", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_YELLOW(), m_defenderPlot->getX(), m_defenderPlot->getY(), true, true
									);
								}
							}
						}
						break;
					case COMMERCE_RESEARCH:
						if (m_defender->isPillageResearch())
						{
							GET_TEAM(GET_PLAYER(m_defender->getOwner()).getTeam()).changeResearchProgress(ePillageTech, iPillageGold, m_defender->getOwner());
							if (bHumanDefender)
							{
								if (BARBARIAN_PLAYER != eDefender)
								{
									AddDLLMessage(
										m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
										gDLL->getText(
											"TXT_KEY_MISC_PLUNDERED_RESEARCH_FROM_VICTIMS",
											iPillageGold, m_defender->getNameKey(), m_attacker->getVisualCivAdjective(m_attacker->getTeam()), ePillageTech
										),
										"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_GREEN(), m_defenderPlot->getX(), m_defenderPlot->getY()
									);
								}
								else
								{
									AddDLLMessage(
										m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
										gDLL->getText("TXT_KEY_MISC_PLUNDERED_RESEARCH_FROM_VICTIMS_HIDDEN", iPillageGold, m_defender->getNameKey(), ePillageTech),
										"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_GREEN(), m_defenderPlot->getX(), m_defenderPlot->getY()
									);
								}
							}
						}
						break;
					case COMMERCE_CULTURE:
						break;
					case COMMERCE_ESPIONAGE:
						if (m_defender->isPillageEspionage() && m_defenderPlot->getTeam() != NO_TEAM)
						{
							GET_TEAM(GET_PLAYER(m_defender->getOwner()).getTeam()).changeEspionagePointsAgainstTeam(m_attacker->getTeam(), iPillageGold);
							if (bHumanDefender)
							{
								if (BARBARIAN_PLAYER != eDefender)
								{
									AddDLLMessage(
										m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
										gDLL->getText("TXT_KEY_MISC_PLUNDERED_ESPIONAGE_FROM_VICTIMS", iPillageGold, m_attacker->getVisualCivAdjective(m_attacker->getTeam()), m_defender->getNameKey()),
										"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_GREEN(), m_defenderPlot->getX(), m_defenderPlot->getY()
									);
								}
								else
								{
									AddDLLMessage(
										m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
										gDLL->getText("TXT_KEY_MISC_PLUNDERED_ESPIONAGE_FROM_VICTIMS_HIDDEN", iPillageGold, m_defender->getNameKey()),
										"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_GREEN(), m_defenderPlot->getX(), m_defenderPlot->getY()
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
		CvEventReporter::getInstance().combatResult(m_defender, m_attacker);

		CvOutcomeListMerged list;
		list.addOutcomeList(m_attacker->getUnitInfo().getKillOutcomeList());
		//getUnitInfo().getKillOutcomeList()->execute(*m_defender, getOwner(), getUnitType());
		for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		{
			const UnitCombatTypes eCombat = (UnitCombatTypes)iI;
			if (m_attacker->isHasUnitCombat(eCombat))
			{
				list.addOutcomeList(GC.getUnitCombatInfo(eCombat).getKillOutcomeList());
				//pOutcomeList->execute(*m_defender, getOwner(), getUnitType());
			}
		}
		list.execute(*m_defender, m_attacker->getOwner(), m_attacker->getUnitType());

		return;
	}

	if (m_bDefenderWithdrawn)
	{
		if (!m_bAttackerOnslaught)
		{
			if (!m_bAttackerStampedes)
			{
				if (bHuman)
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_WITHDRAW", m_defender->getNameKey(), m_attacker->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
				if (bHumanDefender)
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WITHDRAW", m_defender->getNameKey(), m_attacker->getNameKey()),
						"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			else
			{
				if (bHuman)
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_WITHDRAW_STAMPEDE", m_defender->getNameKey(), m_attacker->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
				if (bHumanDefender)
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WITHDRAW_STAMPEDE", m_defender->getNameKey(), m_attacker->getNameKey()),
						"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
			}
		}
		else
		{
			if (bHuman)
			{
				AddDLLMessage(
					m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_WITHDRAW_ONSLAUGHT", m_defender->getNameKey(), m_attacker->getNameKey()),
					"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
				);
			}
			if (bHumanDefender)
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WITHDRAW_ONSLAUGHT", m_defender->getNameKey(), m_attacker->getNameKey()),
					"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
				);
			}
		}

		if (m_plot->isCity())
		{
			m_plotWithdrawal = NULL;
			//TB Combat Mod (Stampede)
			if (m_bAttackerStampedes || m_bAttackerOnslaught)
			{
				m_attacker->attack(m_plot);
			}
			else
			{
				const bool bAdvance = !m_bSamePlot && m_attacker->canAdvance(m_plot, (m_defender->canDefend() && !m_defender->isDead() && m_defender->plot() == m_plot) ? 1 : 0);
				if (bAdvance)
				{
					if (m_attacker->getGroup() && m_plot->getNumVisiblePotentialEnemyDefenders(m_attacker) == 0)
					{
						PROFILE("CvUnit::updateCombat.Advance");
						m_attacker->getGroup()->groupMove(m_plot, true, bAdvance ? m_attacker : NULL);
					}
				}
				else if (!m_bAttackerStampedes && !m_bAttackerOnslaught)
				{
					m_attacker->changeMoves(std::max(GC.getMOVE_DENOMINATOR(), m_plot->movementCost(m_attacker, m_attacker->plot())));
				}
			}
			//TB Combat Mod end
		}

		if (m_plotWithdrawal && !m_bSamePlot)
		{
			FAssertMsg(m_plotWithdrawal != m_attacker->plot(), "Can't escape back to attacker plot");
			FAssertMsg(m_plotWithdrawal != m_defender->plot(), "Can't escape back to own plot");

			//defender escapes to a safe plot
			m_defender->move(m_plotWithdrawal, true);
			const bool bAdvance = m_attacker->canAdvance(m_plot, (m_defender->canDefend() && !m_defender->isDead() && m_defender->plot() == m_plot) ? 1 : 0);

			//TB Combat Mod (Stampede) begin
			if (m_bAttackerStampedes || m_bAttackerOnslaught)
			{
				m_attacker->attack(m_plot);
			}
			else if (m_attacker->getGroup())
			{
				if (bAdvance && m_plot->getNumVisiblePotentialEnemyDefenders(m_attacker) == 0)
				{
					PROFILE("CvUnit::updateCombat.Advance");

					m_attacker->getGroup()->groupMove(m_plot, true, bAdvance ? m_attacker : NULL);
				}
				else if (!m_bStealthDefense && !m_bAttackerStampedes && !m_bAttackerOnslaught)
				{
					m_attacker->changeMoves(std::max(GC.getMOVE_DENOMINATOR(), m_plot->movementCost(m_attacker, m_attacker->plot())));
				}
			}
			else if (!m_bStealthDefense)
			{
				m_attacker->changeMoves(std::max(GC.getMOVE_DENOMINATOR(), m_plot->movementCost(m_attacker, m_attacker->plot())));
			}
		}
		else if (!m_bStealthDefense)
		{
			m_attacker->changeMoves(std::max(GC.getMOVE_DENOMINATOR(), m_plot->movementCost(m_attacker, m_attacker->plot())));
		}
	}
	else if (m_bAttackerRepelled)
	{
		if (!m_bAttackerStampedes && !m_bAttackerOnslaught)
		{
			if (bHuman)
			{
				if (BARBARIAN_PLAYER != eDefender)
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText(
							"TXT_KEY_MISC_ATTACKER_REPELLED_HUMAN",
							m_attacker->getNameKey(), m_attacker->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()
						),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_REPELLED_HUMAN_HIDDEN", getNameKey(), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			if (bHumanDefender)
			{
				if (BARBARIAN_PLAYER != eDefender)
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_REPELLED", m_attacker->getVisualCivAdjective(m_attacker->getTeam()), m_attacker->getNameKey(), m_defender->getNameKey()),
						"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_REPELLED_HIDDEN", m_attacker->getNameKey(), m_defender->getNameKey()),
						"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
			}
		}
		else if (!m_bAttackerOnslaught)
		{
			if (bHuman)
			{
				if (BARBARIAN_PLAYER != eDefender)
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText(
							"TXT_KEY_MISC_ATTACKER_REPELLED_HUMAN_STAMPEDE",
							m_attacker->getNameKey(), m_attacker->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()
						),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_REPELLED_HUMAN_STAMPEDE_HIDDEN", m_attacker->getNameKey(), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			if (bHumanDefender)
			{

				if (BARBARIAN_PLAYER != eAttacker)
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_REPELLED_STAMPEDE", m_attacker->getVisualCivAdjective(m_attacker->getTeam()), m_attacker->getNameKey(), m_defender->getNameKey()),
						"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_REPELLED_STAMPEDE_HIDDEN", m_attacker->getNameKey(), m_defender->getNameKey()),
						"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
			}
		}
		else
		{
			if (bHuman)
			{
				if (BARBARIAN_PLAYER != eDefender)
				{
					AddDLLMessage(
						getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText(
							"TXT_KEY_MISC_ATTACKER_REPELLED_HUMAN_ONSLAUGHT",
							m_attacker->getNameKey(), m_attacker->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()
						),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_REPELLED_HUMAN_ONSLAUGHT_HIDDEN", m_attacker->getNameKey(), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			if (bHumanDefender)
			{
				if (BARBARIAN_PLAYER != eAttacker)
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText(
							"TXT_KEY_MISC_ATTACKER_REPELLED_ONSLAUGHT",
							m_attacker->getVisualCivAdjective(getTeam()), m_attacker->getNameKey(), m_defender->getNameKey()
						),
						"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ATTACKER_REPELLED_ONSLAUGHT_HIDDEN", m_attacker->getNameKey(), m_defender->getNameKey()),
						"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
			}
		}

		m_bAttackerRepelled = false;

		if (!m_bAttackerStampedes && !m_bAttackerOnslaught)
		{
			if (!m_bStealthDefense)
			{
				m_attacker->changeMoves(std::max(GC.getMOVE_DENOMINATOR(), m_plot->movementCost(m_attacker, m_attacker->plot())));
			}
		}
		else m_attacker->attack(m_plot); //TB Combat Mod (Stampede)
	}
	else if (m_bDefenderKnockedBack)
	{
		//Knockback
		if (!m_bAttackerStampedes && !m_bAttackerOnslaught)
		{
			if (bHuman)
			{
				if (BARBARIAN_PLAYER != eDefender)
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText(
							"TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_HUMAN",
							m_attacker->getNameKey(), m_attacker->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()
						),
						"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_HUMAN_HIDDEN", m_attacker->getNameKey(), m_defender->getNameKey()),
						"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			if (bHumanDefender)
			{
				if (BARBARIAN_PLAYER != eAttacker)
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText(
							"TXT_KEY_MISC_DEFENDER_KNOCKEDBACK",
							m_attacker->getVisualCivAdjective(m_attacker->getTeam()), m_attacker->getNameKey(), m_defender->getNameKey()
						),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_HIDDEN", m_attacker->getNameKey(), m_defender->getNameKey()),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
			}
		}
		else if (!m_bAttackerOnslaught)
		{
			if (bHuman)
			{
				if (BARBARIAN_PLAYER != eDefender)
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText(
							"TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_HUMAN_STAMPEDE",
							m_attacker->getNameKey(), m_attacker->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()
						),
						"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText(
							"TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_HUMAN_STAMPEDE_HIDDEN",
							m_attacker->getNameKey(), m_defender->getNameKey()
						),
						"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			if (bHumanDefender)
			{
				if (BARBARIAN_PLAYER != eAttacker)
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText(
							"TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_STAMPEDE",
							m_attacker->getVisualCivAdjective(m_attacker->getTeam()), m_attacker->getNameKey(), m_defender->getNameKey()
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
							m_attacker->getNameKey(), m_defender->getNameKey()
						),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
			}
		}
		else
		{
			if (bHuman)
			{
				if (BARBARIAN_PLAYER != eDefender)
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText(
							"TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_HUMAN_ONSLAUGHT",
							m_attacker->getNameKey(), m_attacker->getVisualCivAdjective(m_defender->getTeam()), m_defender->getNameKey()
						),
						"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
				else
				{
					AddDLLMessage(
						m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText(
							"TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_HUMAN_ONSLAUGHT_HIDDEN",
							m_attacker->getNameKey(), m_defender->getNameKey()
						),
						"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
					);
				}
			}
			if (bHumanDefender)
			{
				if (BARBARIAN_PLAYER != eAttacker)
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText(
							"TXT_KEY_MISC_DEFENDER_KNOCKEDBACK_ONSLAUGHT",
							m_attacker->getVisualCivAdjective(m_attacker->getTeam()), m_attacker->getNameKey(), m_defender->getNameKey()
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
							m_attacker->getNameKey(), m_defender->getNameKey()
						),
						"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
					);
				}
			}
		}
		m_bDefenderKnockedBack = false;

		if (m_plot->isCity())
		{
			m_plotWithdrawal = NULL;
		}
		m_defender->setFortifyTurns(0);

		if (m_plotWithdrawal)
		{
			FAssertMsg(m_plotWithdrawal != m_attacker->plot(), "Can't get knocked back to attacker plot");
			FAssertMsg(m_plotWithdrawal != m_defender->plot(), "Can't get knocked back to own plot");

			//defender escapes to a safe plot
			m_defender->move(m_plotWithdrawal, true);
			m_defender->getGroup()->clearMissionQueue();
		}

		const bool bAdvance = m_attacker->canAdvance(m_plot, (m_defender->canDefend() && !m_defender->isDead() && m_defender->plot() == m_plot) ? 1 : 0);

		if (!bAdvance && !m_bAttackerStampedes && !m_bAttackerOnslaught)
		{
			if (!m_attacker->isNoCapture())
			{
				m_defender->setCapturingPlayer(m_attacker->getOwner());
				m_defender->setCapturingUnit(m_attacker);
			}
			if (!m_bStealthDefense)
			{
				m_attacker->changeMoves(std::max(GC.getMOVE_DENOMINATOR(), m_plot->movementCost(m_attacker, m_attacker->plot())));
			}
		}
		else if (m_bAttackerStampedes || m_bAttackerOnslaught)
		{
			m_attacker->attack(m_plot);
		}

		if (m_attacker->getGroup())
		{
			if (m_plot->getNumVisiblePotentialEnemyDefenders(m_attacker) == 0)
			{
				PROFILE("CvUnit::updateCombat.Advance");

				m_attacker->getGroup()->groupMove(m_plot, true, (bAdvance ? m_attacker : NULL));
			}
			// This is before the plot advancement, the unit will always try to walk back
			// to the square that they came from, before advancing.
		}
	}
	else if (m_bAttackerWithdraws)
	{
		if (!m_bAttackerStampedes && !m_bAttackerOnslaught)
		{
			if (bHuman)
			{
				AddDLLMessage(
					m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WITHDRAW", m_attacker->getNameKey(), m_defender->getNameKey()),
					"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
				);
			}
			if (bHumanDefender)
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_WITHDRAW", m_attacker->getNameKey(), m_defender->getNameKey()),
					"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
				);
			}
			m_attacker->changeMoves(std::max(GC.getMOVE_DENOMINATOR(), m_plot->movementCost(m_attacker, m_attacker->plot())));
			//GC.getGame().logOOSSpecial(53, getID(), getMoves(), getDamage());
		}
		//TB Combat Mod (Stampede) begin
		else if (m_bAttackerStampedes)
		{
			if (bHuman)
			{
				AddDLLMessage(
					m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_YOU_UNIT_ATTACKER_WITHDRAW_STAMPEDE", m_attacker->getNameKey(), m_defender->getNameKey()),
					"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
				);
			}
			if (bHumanDefender)
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_ATTACKER_WITHDRAW_STAMPEDE", m_attacker->getNameKey(), m_defender->getNameKey()),
					"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
				);
			}
			attack(m_plot);
		}
		else if (m_bAttackerOnslaught)
		{
			if (bHuman)
			{
				AddDLLMessage(
					m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_YOU_UNIT_ATTACKER_WITHDRAW_ONSLAUGHT", m_attacker->getNameKey(), m_defender->getNameKey()),
					"AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
				);
			}
			if (bHumanDefender)
			{
				AddDLLMessage(
					m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_ATTACKER_WITHDRAW_ONSLAUGHT", m_attacker->getNameKey(), m_defender->getNameKey()),
					"AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_YELLOW(), m_plot->getX(), m_plot->getY()
				);
			}
			m_attacker->attack(m_plot);
		}
	}
	else if (m_defender->isDead())
	{
		if (m_defender->isNPC())
		{
			GET_PLAYER(m_attacker->getOwner()).changeWinsVsBarbs(1);
		}

		if (!m_attacker->isHiddenNationality() && !m_defender->isHiddenNationality())
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
			GET_TEAM(m_defender->getTeam()).changeWarWearinessTimes100(m_attacker->getTeam(), *m_plot, defenderWarWearinessChangeTimes100);

			const int attackerWarWearinessChangeTimes100 =
			(
				100 * GC.getDefineINT("WW_KILLED_UNIT_ATTACKING")
				* (m_attacker->getDamage() - m_attacker->getPreCombatDamage())
				/
				m_attacker->getMaxHP()
			);
			GET_TEAM(m_attacker->getTeam()).changeWarWearinessTimes100(m_defender->getTeam(), *m_plot, attackerWarWearinessChangeTimes100);

			GET_TEAM(m_attacker->getTeam()).AI_changeWarSuccess(m_defender->getTeam(), GC.getDefineINT("WAR_SUCCESS_ATTACKING"));
		}

		const int iInfluenceRatio = GC.isIDW_ENABLED() ? m_attacker->doVictoryInfluence(m_defender, true, false) : 0;

		if (bHuman)
		{
			CvWString szBuffer;

			if (m_bDefenderPursued)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_PURSUIT_ONATTACK_SUCCESS", m_defender->getNameKey(), m_attacker->getNameKey());
			}
			else if (m_bAttackerRefusedtoYield)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_UNYIELDING_YOU_DESTROYED_ENEMY", m_defender->getNameKey(), m_attacker->getNameKey());
			}
			else if (m_bDefenderRefusedtoYield)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_UNYIELDING_ENEMY", m_defender->getNameKey(), m_attacker->getNameKey());
			}
			else if (m_bAttackerStampedes)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_ENEMY_STAMPEDE", m_defender->getNameKey(), m_attacker->getNameKey());
			}
			else if (m_bAttackerOnslaught)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_ENEMY_ONSLAUGHT", m_defender->getNameKey(), m_attacker->getNameKey());
			}
			else szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_ENEMY", m_attacker->getNameKey(), m_defender->getNameKey());

			if (iInfluenceRatio > 0)
			{
				szBuffer = szBuffer + CvString::format(" %s: +%.1f%%", gDLL->getText("TXT_KEY_TILE_INFLUENCE").GetCString(), ((float)iInfluenceRatio)/10);
			}
			AddDLLMessage(
				m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer,
				GC.getEraInfo(GC.getGame().getCurrentEra()).getAudioUnitVictoryScript(),
				MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), m_plot->getX(), m_plot->getY()
			);
		}
		if (bHumanDefender)
		{
			CvWString szBuffer;

			if (m_bDefenderPursued)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_PURSUIT_ONATTACK_VICTIM", m_defender->getNameKey(), m_attacker->getNameKey());
			}
			else if (m_bAttackerRefusedtoYield)
			{
				if (BARBARIAN_PLAYER != eDefender)
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_BYUNYIELDING_UNKNOWN", m_defender->getNameKey(), m_attacker->getNameKey());
				}
				else szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_BYUNYIELDING", m_defender->getNameKey(), m_attacker->getVisualCivAdjective(m_defender->getTeam()), m_attacker->getNameKey());
			}
			else if (m_bDefenderRefusedtoYield)
			{
				if (BARBARIAN_PLAYER == eDefender)
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_WHILEUNYIELDING_UNKNOWN", m_defender->getNameKey(), m_attacker->getNameKey());
				}
				else szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_WHILEUNYIELDING", m_defender->getNameKey(), m_attacker->getVisualCivAdjective(m_defender->getTeam()), m_attacker->getNameKey());
			}
			else if (m_bAttackerStampedes)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_ENEMY_STAMPEDE", m_defender->getNameKey(), m_attacker->getNameKey());
			}
			else if (m_bAttackerOnslaught)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_ENEMY_ONSLAUGHT", m_defender->getNameKey(), m_attacker->getNameKey());
			}
			else if (BARBARIAN_PLAYER == eAttacker)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_UNKNOWN", m_defender->getNameKey(), m_attacker->getNameKey());
			}
			else szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED", m_defender->getNameKey(), m_attacker->getNameKey(), m_attacker->getVisualCivAdjective(m_defender->getTeam()));

			if (iInfluenceRatio > 0)
			{
				szBuffer = szBuffer + CvString::format(" %s: -%.1f%%", gDLL->getText("TXT_KEY_TILE_INFLUENCE").GetCString(), ((float)iInfluenceRatio)/10);
			}
			AddDLLMessage(
				m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer,
				GC.getEraInfo(GC.getGame().getCurrentEra()).getAudioUnitDefeatScript(),
				MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_RED(), m_plot->getX(), m_plot->getY()
			);
		}

		// report event to Python, along with some other key state
		int iUnitsHealed = 0;
		if (m_attacker->getVictoryAdjacentHeal() > 0
		&& GC.getGame().getSorenRandNum(100, "Field Hospital Die Roll") <= m_attacker->getVictoryAdjacentHeal())
		{
			foreach_(const CvPlot* plotX, m_plot->adjacent() | filtered(CvPlot::fn::area() == m_plot->area()))
			{
				foreach_(CvUnit* unitX, plotX->units())
				{
					if (unitX->getTeam() == m_attacker->getTeam() && unitX->isHurt())
					{
						iUnitsHealed++;
						unitX->doHeal();
					}
				}
			}
		}

		if (m_attacker->getVictoryStackHeal() > 0
		&& GC.getGame().getSorenRandNum(100, "Field Surgeon Die Roll") <= m_attacker->getVictoryStackHeal())
		{
			foreach_(CvUnit* pLoopUnit, m_plot->units())
			{
				if (pLoopUnit->getTeam() == m_attacker->getTeam() && pLoopUnit->isHurt())
				{
					iUnitsHealed++;
					pLoopUnit->doHeal();
				}
			}
		}
		else if (m_attacker->getVictoryHeal() > 0 && GC.getGame().getSorenRandNum(100, "Field Medic Die Roll") <= m_attacker->getVictoryHeal())
		{
			m_attacker->doHeal();
		}

		if (iUnitsHealed > 1)
		{
			if (bHumanDefender)
			{
				if (BARBARIAN_PLAYER != eAttacker)
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ENEMY_FIELD_MEDIC_ATTACKERS", GET_PLAYER(m_attacker->getOwner()).getCivilizationAdjective()),
						"AS2D_COMBAT", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_YELLOW(), getX(), getY(), true, true
					);
				}
				else
				{
					AddDLLMessage(
						m_defender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_ENEMY_FIELD_MEDIC_ATTACKERS_HIDDEN"),
						"AS2D_COMBAT", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_YELLOW(), getX(), getY(), true, true
					);
				}
			}
			if (bHuman)
			{
				AddDLLMessage(
					m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_MISC_FIELD_MEDIC_ATTACKERS", m_attacker->getNameKey(), iUnitsHealed),
					"AS2D_POSITIVE_DINK", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_GREEN(), getX(), getY()
				);
			}
		}

		if (m_attacker->getOffensiveVictoryMoveCount() > 0)
		{
			m_attacker->changeMoves(-GC.getMOVE_DENOMINATOR());
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
			if (pCapitalCity)
			{
				m_defender->setCanRespawn(true);
			}
		}

		if (m_attacker->isPillageOnVictory())
		{
			CvPlot* m_defenderPlot = m_defender->plot();
			int iPillageGold = m_defender->getLevel() * m_defender->getExperience();
			if (NO_UNIT != m_defender->getLeaderUnitType())
			{
				iPillageGold *= m_defender->getLevel();
			}
			if (iPillageGold > 0)
			{
				iPillageGold += iPillageGold * m_attacker->getPillageChange() / 100;
				GET_PLAYER(m_attacker->getOwner()).changeGold(iPillageGold);

				if (bHuman)
				{
					if (BARBARIAN_PLAYER != eAttacker)
					{
						AddDLLMessage(
							m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
							gDLL->getText("TXT_KEY_MISC_GOLD_LOOTED_FROM_DEAD", m_defender->getNameKey(), m_attacker->getVisualCivAdjective(m_attacker->getTeam())),
							"AS2D_PILLAGED", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_YELLOW(), m_defenderPlot->getX(), m_defenderPlot->getY()
						);
					}
					else
					{
						AddDLLMessage(
							m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
							gDLL->getText("TXT_KEY_MISC_GOLD_LOOTED_FROM_DEAD_HIDDEN", m_defender->getNameKey()),
							"AS2D_PILLAGED", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_YELLOW(), m_defenderPlot->getX(), m_defenderPlot->getY()
						);
					}
				}

				for (int iI = 0; iI < NUM_COMMERCE_TYPES; ++iI)
				{
					TechTypes ePillageTech = GET_PLAYER(m_attacker->getOwner()).getCurrentResearch();
					CommerceTypes eCommerce = (CommerceTypes)iI;
					switch (eCommerce)
					{
					case COMMERCE_GOLD:
						if (m_attacker->isPillageMarauder())
						{
							GET_PLAYER(m_attacker->getOwner()).changeGold(iPillageGold);
							if (bHuman)
							{
								AddDLLMessage(
									m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText("TXT_KEY_MISC_MARAUDERS_PLUNDERED_VICTIMS", iPillageGold, m_attacker->getNameKey()),
									"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_GREEN(), m_defenderPlot->getX(), m_defenderPlot->getY()
								);
							}
							if (bHumanDefender)
							{
								if (BARBARIAN_PLAYER != eAttacker)
								{
									AddDLLMessage(
										m_defender->getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
										gDLL->getText("TXT_KEY_MISC_ATTACKED_BY_MARAUDERS", m_defender->getNameKey(), m_attacker->getVisualCivAdjective(m_attacker->getTeam())),
										"AS2D_PILLAGED", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_YELLOW(), m_defenderPlot->getX(), m_defenderPlot->getY(), true, true
									);
								}
								else
								{
									AddDLLMessage(
										m_defender->getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
										gDLL->getText("TXT_KEY_MISC_ATTACKED_BY_MARAUDERS_HIDDEN", m_defender->getNameKey()),
										"AS2D_PILLAGED", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_YELLOW(), m_defenderPlot->getX(), m_defenderPlot->getY(), true, true
									);
								}
							}
						}
						break;
					case COMMERCE_RESEARCH:
						if (m_attacker->isPillageResearch())
						{
							GET_TEAM(GET_PLAYER(m_attacker->getOwner()).getTeam()).changeResearchProgress(ePillageTech, iPillageGold, m_attacker->getOwner());
							if (bHuman)
							{
								if (BARBARIAN_PLAYER != eDefender)
								{
									AddDLLMessage(
										m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
										gDLL->getText(
											"TXT_KEY_MISC_PLUNDERED_RESEARCH_FROM_IMP",
											iPillageGold, m_attacker->getNameKey(), m_attacker->getVisualCivAdjective(m_defender->getTeam()), ePillageTech
										),
										"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_GREEN(), m_defenderPlot->getX(), m_defenderPlot->getY()
									);
								}
								else
								{
									AddDLLMessage(
										m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
										gDLL->getText("TXT_KEY_MISC_PLUNDERED_RESEARCH_FROM_IMP_HIDDEN", iPillageGold, m_attacker->getNameKey(), ePillageTech),
										"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_GREEN(), m_defenderPlot->getX(), m_defenderPlot->getY()
									);
								}
							}
						}
						break;
					case COMMERCE_CULTURE:
						break;
					case COMMERCE_ESPIONAGE:
						if (m_attacker->isPillageEspionage() && m_defenderPlot->getTeam() != NO_TEAM)
						{
							GET_TEAM(GET_PLAYER(m_attacker->getOwner()).getTeam()).changeEspionagePointsAgainstTeam(m_defenderPlot->getTeam(), iPillageGold);

							if (bHuman)
							{
								if (BARBARIAN_PLAYER != eDefender)
								{
									AddDLLMessage(
										m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
										gDLL->getText(
											"TXT_KEY_MISC_PLUNDERED_ESPIONAGE_FROM_IMP",
											iPillageGold, m_attacker->getNameKey(), m_attacker->getVisualCivAdjective(m_defender->getTeam()), ePillageTech
										),
										"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_GREEN(), m_defenderPlot->getX(), m_defenderPlot->getY()
									);
								}
								else
								{
									AddDLLMessage(
										m_attacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(),
										gDLL->getText("TXT_KEY_MISC_PLUNDERED_ESPIONAGE_FROM_IMP_HIDDEN", iPillageGold, m_attacker->getNameKey(), ePillageTech),
										"AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_attacker->getButton(), GC.getCOLOR_GREEN(), m_defenderPlot->getX(), m_defenderPlot->getY()
									);
								}
							}
						}
						break;
					}
				}
			}
		}

		CvEventReporter::getInstance().combatResult(m_attacker, m_defender);
		PlayerTypes eDefenderUnitPlayer = m_defender->getOwner();
		UnitTypes eDefenderUnitType = m_defender->getUnitType();

		// generate the kill outcome list but don't execute it yet
		//std::vector<UnitCombatTypes> aDefenderCombats;
		CvOutcomeListMerged mergedList;
		mergedList.addOutcomeList(m_defender->getUnitInfo().getKillOutcomeList());
		for (std::map<UnitCombatTypes, UnitCombatKeyedInfo>::const_iterator it = m_unitCombatKeyedInfo.begin(), end = m_unitCombatKeyedInfo.end(); it != end; ++it)
		{
			if (it->second.m_bHasUnitCombat)
			{
				mergedList.addOutcomeList(GC.getUnitCombatInfo((UnitCombatTypes)it->first).getKillOutcomeList());
				//aDefenderCombats.push_back((UnitCombatTypes)it->first);
			}
		}

		if (m_attacker->isSuicide())
		{
			m_attacker->kill(true);

			m_defender->kill(true, NO_PLAYER, true);
			m_defender = NULL;
		}
		else
		{
			// Each attacker will only be ambushed once by a stealth defender per move,
			//	and stampede only needs to trigger against visible defenders.
			if (!m_bStealthDefense)
			{
				if (m_plot->hasDefender(false, NO_PLAYER, m_attacker->getOwner(), m_attacker, true, false, false, true)
				&& (m_bAttackerStampedes || m_bAttackerOnslaught))
				{
					m_attacker->attack(m_plot);
				}
				else if (m_plot->hasStealthDefender(m_attacker))  // Reveals the unit if true
				{
					// Doesn't matter if the plot has visible defenders, stealth defense is an opportunistic ambush.
					m_attacker->attack(m_plot, true); 
				}
			}
			const bool bAdvance = !m_bSamePlot && m_attacker->canAdvance(m_plot, m_defender->canDefend());

			// TBMaybeproblem - should this come before the generation of the captive which takes place above at the add outcome step?
			if (bAdvance && !m_attacker->isNoCapture())
			{
				m_defender->setCapturingPlayer(getOwner());
				m_defender->setCapturingUnit(m_attacker);
			}
			m_defender->kill(true, NO_PLAYER, true);
			m_defender = NULL;

			if (m_bSamePlot || !bAdvance)
			{
				if (bCombatFinished || !m_bAttackerStampedes && !m_bAttackerOnslaught)
				{
					m_attacker->changeMoves(std::max(GC.getMOVE_DENOMINATOR(), m_plot->movementCost(m_attacker, m_attacker->plot())));
				}
			}
			else if (bCombatFinished) m_attacker->getGroup()->groupMove(m_plot, true, m_attacker);
		}
		//TB Combat Mods End

		mergedList.execute(*m_attacker, eDefenderUnitPlayer, eDefenderUnitType);
	}

	if (m_bQuick && m_attacker->IsSelected() && !m_attacker->canMove())
	{
		gDLL->getInterfaceIFace()->removeFromSelectionList(m_attacker);
	}
}

