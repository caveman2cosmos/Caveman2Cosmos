#pragma once
// CombatData.h
#ifndef COMBAT_DATA_H
#define COMBAT_DATA_H

class CombatData
{
public:
	CombatData();
	~CombatData();


private:
	bool bDefenderWithdrawn;
	bool bAttackerPursued;
	bool bDefenderPursued;
	bool bAttackerPursuedSustain;
	bool bDefenderPursuedSustain;
	bool bAttackerRepelled;
	bool bAttackerRefusedtoYield;
	bool bDefenderRefusedtoYield;
	bool bAttackerRefusedtoYieldSustain;
	bool bDefenderRefusedtoYieldSustain;
	bool bDefenderKnockedBack;
	bool bAttackerStampedes;
	bool bAttackerWithdraws;
	bool bAttackerOnslaught;
	bool bAttackerInjured;
	bool bDefenderInjured;
	bool bDeathMessaged;
	bool bDefenderHitAttackerWithDistanceAttack;
	bool bAttackerHitDefenderWithDistanceAttack;
	bool bNeverMelee;
	bool bStealthDefense;

	int iAttacksCount;
	int iTurnCount;

	CvPlot* pPlot;

};


#endif