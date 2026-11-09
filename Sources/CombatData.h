#pragma once
// CombatData.h
#ifndef COMBAT_DATA_H
#include "CvDLLEntity.h"
#include "CvUnit.h"
#define COMBAT_DATA_H

class CombatData
{
public:
	CombatData(CvUnit *attacker, CvUnit *defender, CvPlot *pPlot);
	~CombatData();

	void resolveCombat(CvBattleDefinition &kBattle);
	void endCombat();
	void setStealthDefense(const bool bNew);
	bool getStealthDefense() const;

private:
	CvUnit* m_attacker;
	CvUnit* m_defender;
	CvPlot* m_plot;
	CvPlot* m_plotWithdrawal;

	bool m_bSamePlot;
	bool m_bQuick;
	bool m_bCombatStarted;
	bool m_bDefenderWithdrawn;
	bool m_bAttackerPursued;
	bool m_bDefenderPursued;
	bool m_bAttackerPursuedSustain;
	bool m_bDefenderPursuedSustain;
	bool m_bAttackerRepelled;
	bool m_bAttackerRefusedtoYield;
	bool m_bDefenderRefusedtoYield;
	bool m_bAttackerRefusedtoYieldSustain;
	bool m_bDefenderRefusedtoYieldSustain;
	bool m_bDefenderKnockedBack;
	bool m_bAttackerStampedes;
	bool m_bAttackerWithdraws;
	bool m_bAttackerOnslaught;
	bool m_bAttackerInjured;
	bool m_bDefenderInjured;
	bool m_bDeathMessaged;
	bool m_bDefenderHitAttackerWithDistanceAttack;
	bool m_bAttackerHitDefenderWithDistanceAttack;
	bool m_bNeverMelee;
	bool m_bStealthDefense;
	bool m_bAttackerHasLostNoHP;
	bool m_bAttackerWithdrawn;

	int m_iAttacksCount;
	int m_iTurnCount;
	int m_iAttackerInitialDamage;
	int m_iDefenderInitialDamage;
	int m_iInitialAttXP;
	int m_iInitialDefXP;
	int m_iInitialAttGGXP;
	int m_iInitialDefGGXP;
	int m_iAttackerFirstStrikes;
	int m_iDefenderFirstStrikes;
	int m_iAttackerDodge;
	int m_iDefenderDodge;
	int m_iAttackerPrecision;
	int m_iDefenderPrecision;

	void doBattleFieldPromotions(CombatDetails &cdDefenderDetails, int iWinningOdds, bool &bAttackerPromoted, bool &bDefenderPromoted, int iNonLethalAttackWinChance, int iNonLethalDefenseWinChance);
};

#endif