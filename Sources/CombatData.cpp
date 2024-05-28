#include "CvGameCoreDLL.h"
#include "CombatData.h"
#include "CvInfos.h"
#include "CvUnit.h"

CombatData::CombatData()
{
	bDefenderWithdrawn = false;
	bAttackerPursued = false;
	bDefenderPursued = false;
	bAttackerPursuedSustain = false;
	bDefenderPursuedSustain = false;
	bAttackerRepelled = false;
	bAttackerRefusedtoYield = false;
	bDefenderRefusedtoYield = false;
	bAttackerRefusedtoYieldSustain = false;
	bDefenderRefusedtoYieldSustain = false;
	bDefenderKnockedBack = false;
	bAttackerStampedes = false;
	bAttackerWithdraws = false;
	bAttackerOnslaught = false;
	bAttackerInjured = false;
	bDefenderInjured = false;
	bDefenderHitAttackerWithDistanceAttack = false;
	bAttackerHitDefenderWithDistanceAttack = false;
	bStealthDefense = false;
	bDeathMessaged = true;
	bNeverMelee = true;

	iTurnCount = 0;
	iAttacksCount = 0;

	pPlot = NULL;
}
CombatData::~CombatData() { }
