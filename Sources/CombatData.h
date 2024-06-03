#pragma once
// CombatData.h
#ifndef COMBAT_DATA_H
#define COMBAT_DATA_H

class CombatData
{
public:
	CombatData(CvUnit* attacker, CvUnit* defender, CvPlot* pPlot);
	~CombatData();

	void resolveCombat(CvBattleDefinition& kBattle);
	void endCombat();

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

	int m_iAttacksCount;
	int m_iTurnCount;
};

#endif