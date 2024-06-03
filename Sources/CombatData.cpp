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

	bSamePlot = m_attacker->plot() == m_plot;

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
		bBreakdown = m_attacker->isBreakdownCombat(m_plot, bSamePlot) && m_defender->getCombatFirstStrikes() == 0;

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

				if  ((!m_plot->isCity(true, m_defender->getTeam()) || bSamePlot)
				&& GC.getGame().isModderGameOption(MODDERGAMEOPTION_DEFENDER_WITHDRAW)
				&& (bDefenderSkirmish || m_defender->getDamage() + iDefenderDamage >= m_attacker->withdrawalHP(m_defender->getMaxHP(), iDefenderEarlyWithdraw))
				&& !m_attacker->isSuicide()
				&& iCloseCombatRoundNum > 0
				&& m_defender->withdrawVSOpponentProbTotal(m_attacker, m_plot) > 0)	//can not to escape at close combat round 1
				{
					if (DefenderWithdrawalRollResult < AdjustedDefWithdraw)
					{
						bst::optional<CvPlot*> withdrawPlot = selectWithdrawPlot(bSamePlot, m_defender);
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
					if (!bSamePlot)
					{
						int KnockbackRollResult = GC.getGame().getSorenRandNum(100, "Knockback");

						if (m_attacker->getCombatKnockbacks() > 0)
						{
							if (KnockbackRollResult < AdjustedKnockback)
							{
								m_bDefenderKnockedBack = true;
								m_bDeathMessaged = false;
								m_plotWithdrawal = selectWithdrawPlot(bSamePlot, m_defender).get_value_or(nullptr);

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

		if (!bBreakdown && !bSamePlot
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
