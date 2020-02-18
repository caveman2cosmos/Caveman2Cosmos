//
// Python wrapper class for CvUnit
//
//
#include "CvGameCoreDLL.h"

CyUnit::CyUnit() : m_pUnit(NULL)
{

}

CyUnit::CyUnit(CvUnit* pUnit) : m_pUnit(pUnit)
{

}

void CyUnit::convert(CyUnit* pUnit)
{
	if (m_pUnit)
		m_pUnit->convert(pUnit->getUnit());
}

void CyUnit::kill(bool bDelay, int /*PlayerTypes*/ ePlayer)
{
	if (m_pUnit)
		m_pUnit->kill(bDelay, (PlayerTypes)ePlayer);
}

void CyUnit::NotifyEntity(int /*MissionTypes*/ eEvent)
{
	if (m_pUnit)
		m_pUnit->NotifyEntity((MissionTypes)eEvent);
}

bool CyUnit::isActionRecommended(int i)
{
	if ( m_pUnit )
	{
		return m_pUnit->isActionRecommended(i);
	}

	return false;
}

bool CyUnit::isBetterDefenderThan(CyUnit* pDefender, CyUnit* pAttacker) 
{
	return m_pUnit ? m_pUnit->isBetterDefenderThan(pDefender->getUnit(), pAttacker->getUnit()) : false;
}

bool CyUnit::canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible)
{
	return m_pUnit ? m_pUnit->canDoCommand(eCommand, iData1, iData2, bTestVisible) : false;
}

void CyUnit::doCommand(CommandTypes eCommand, int iData1, int iData2)
{
	if (m_pUnit)
		return m_pUnit->doCommand(eCommand, iData1, iData2);
}

CyPlot* CyUnit::getPathEndTurnPlot()
{
	return m_pUnit ? new CyPlot(m_pUnit->getPathEndTurnPlot()) : false;
}

bool CyUnit::generatePath(CyPlot* pToPlot, int iFlags, bool bReuse, int* piPathTurns)
{
	return m_pUnit ? m_pUnit->generatePath(pToPlot->getPlot(), iFlags, bReuse, piPathTurns) : false;
}

bool CyUnit::canEnterTerritory(int /*TeamTypes*/ eTeam, bool bIgnoreRightOfPassage)
{
	return m_pUnit ? (int) m_pUnit->canEnterTerritory((TeamTypes) eTeam, bIgnoreRightOfPassage) : false;
}

bool CyUnit::canEnterArea(int /*TeamTypes*/ eTeam, CyArea* pArea, bool bIgnoreRightOfPassage)
{
	return m_pUnit ? (int) m_pUnit->canEnterArea((TeamTypes) eTeam, pArea->getArea(), bIgnoreRightOfPassage) : false;
}

int /*TeamTypes*/ CyUnit::getDeclareWarMove(CyPlot* pPlot)																					 
{
	return m_pUnit ? (int) m_pUnit->getDeclareWarMove(pPlot->getPlot()) : (int) NO_TEAM;
}

bool CyUnit::canMoveInto(CyPlot* pPlot, bool bAttack, bool bDeclareWar, bool bIgnoreLoad)
{
	return m_pUnit ? m_pUnit->canMoveInto(pPlot->getPlot(), 
		(bAttack ? MoveCheck::Attack : MoveCheck::None) |
		(bDeclareWar ? MoveCheck::DeclareWar : MoveCheck::None) |
		(bIgnoreLoad ? MoveCheck::IgnoreLoad : MoveCheck::None)
	) : false;
}

bool CyUnit::canMoveOrAttackInto(CyPlot* pPlot, bool bDeclareWar)
{
	return m_pUnit ? m_pUnit->canMoveOrAttackInto(pPlot->getPlot(), bDeclareWar) : false;
}

bool CyUnit::canMoveThrough(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canMoveThrough(pPlot->getPlot()) : false;
}

bool CyUnit::jumpToNearestValidPlot()
{
	return m_pUnit ? m_pUnit->jumpToNearestValidPlot() : false;
}

bool CyUnit::canAutomate(AutomateTypes eAutomate)
{
	return m_pUnit ? m_pUnit->canAutomate(eAutomate) : false;
}

bool CyUnit::canScrap()
{
	return m_pUnit ? m_pUnit->canScrap() : false;
}

bool CyUnit::canGift(bool bTestVisible)
{
	return m_pUnit ? m_pUnit->canGift(bTestVisible) : false;
}

/************************************************************************************************/
/* REVOLUTION_MOD                         01/01/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
void CyUnit::gift()
{
	if( m_pUnit )
		m_pUnit->gift();
}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
bool CyUnit::canLoadUnit(CyUnit* pUnit, CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canLoadOntoUnit(pUnit->getUnit(), pPlot->getPlot()) : false;
}

bool CyUnit::canLoad(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canLoad(pPlot->getPlot()) : false;
}

bool CyUnit::canUnload()
{
	return m_pUnit ? m_pUnit->canUnload() : false;
}

bool CyUnit::canUnloadAll()
{
	return m_pUnit ? m_pUnit->canUnloadAll() : false;
}

bool CyUnit::canHold(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canHold(pPlot->getPlot()) : false;
}

bool CyUnit::canSleep(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canSleep(pPlot->getPlot()) : false;
}

bool CyUnit::canFortify(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canFortify(pPlot->getPlot()) : false;
}

bool CyUnit::canEstablish(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canEstablish(pPlot->getPlot()) : false;
}

bool CyUnit::canEscape(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canEscape(pPlot->getPlot()) : false;
}

bool CyUnit::canPlunder(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canPlunder(pPlot->getPlot()) : false;
}

bool CyUnit::canAirPatrol(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canAirPatrol(pPlot->getPlot()) : false;
}

bool CyUnit::canSeaPatrol(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canSeaPatrol(pPlot->getPlot()) : false;
}

bool CyUnit::canHeal(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canHeal(pPlot->getPlot()) : false;
}

bool CyUnit::canSentry(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canSentry(pPlot->getPlot()) : false;
}

bool CyUnit::canAirlift(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canAirlift(pPlot->getPlot()) : false;
}

bool CyUnit::canAirliftAt(CyPlot* pPlot, int iX, int iY)
{
	return m_pUnit ? m_pUnit->canAirliftAt(pPlot->getPlot(), iX, iY) : false;
}

bool CyUnit::isNukeVictim(CyPlot* pPlot, int /*TeamTypes*/ eTeam)
{
	return m_pUnit ? m_pUnit->isNukeVictim(pPlot->getPlot(), (TeamTypes) eTeam) : false;
}

bool CyUnit::canNuke(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canNuke(pPlot->getPlot()) : false;
}

bool CyUnit::canNukeAt(CyPlot* pPlot, int iX, int iY)
{
	return m_pUnit ? m_pUnit->canNukeAt(pPlot->getPlot(), iX, iY) : false;
}

bool CyUnit::canRecon(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canRecon(pPlot->getPlot()) : false;
}

bool CyUnit::canReconAt(CyPlot* pPlot, int iX, int iY)
{
	return m_pUnit ? m_pUnit->canReconAt(pPlot->getPlot(), iX, iY) : false;
}

bool CyUnit::canParadrop(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canParadrop(pPlot->getPlot()) : false;
}

bool CyUnit::canParadropAt(CyPlot* pPlot, int iX, int iY)
{
	return m_pUnit ? m_pUnit->canParadropAt(pPlot->getPlot(), iX, iY) : false;
}

bool CyUnit::canAirBomb(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canAirBomb(pPlot->getPlot()) : false;
}

bool CyUnit::canAirBombAt(CyPlot* pPlot, int iX, int iY)
{
	return m_pUnit ? m_pUnit->canAirBombAt(pPlot->getPlot(), iX, iY) : false;
}

CyCity* CyUnit::bombardTarget(CyPlot* pPlot)
{
	return m_pUnit ? new CyCity(m_pUnit->bombardTarget(pPlot->getPlot())) : false;
}

bool CyUnit::canBombard(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canBombard(pPlot->getPlot()) : false;
}

bool CyUnit::canPillage(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canPillage(pPlot->getPlot()) : false;
}

/****************************************************************************************/
/* SUPER SPIES				28/05/09						TSHEEP                      */
/**																						*/
/**																						*/
/****************************************************************************************/
//TSHEEP Assassin Mission
bool CyUnit::canAssassin(CyPlot* pPlot, bool bTestVisible)
{
	return m_pUnit ? m_pUnit->canAssassin(pPlot->getPlot(), bTestVisible) : false;
}
//int CyUnit::assassinCost(CyUnit* pUnit)
//{
//	return m_pUnit ? m_pUnit->assassinCost(pUnit->getUnit()) : -1;
//}
//int CyUnit::assassinProb(CyUnit* pUnit)
//{
//	return m_pUnit ? m_pUnit->assassinProb(pUnit->getUnit()) : -1;
//}
//TSHEEP End
/****************************************************************************************/
/* SUPER SPIES				END						TSHEEP                              */
/****************************************************************************************/

/****************************************************************************************/
/* REVOLUTIONDCM				28/05/09						Glider1                 */
/**																						*/
/**																						*/
/****************************************************************************************/
//RevolutionDCM start
bool CyUnit::canBribe(CyPlot* pPlot, bool bTestVisible)
{
	return m_pUnit ? m_pUnit->canBribe(pPlot->getPlot(), bTestVisible) : false;
}
//RevolutionDCM end
/****************************************************************************************/
/* REVOLUTIONDCM				END     						Glider1                 */
/****************************************************************************************/

int CyUnit::sabotageCost(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->sabotageCost(pPlot->getPlot()) : -1;
}

int CyUnit::sabotageProb(CyPlot* pPlot, int /*ProbabilityTypes*/ eProbStyle)
{
	return m_pUnit ? m_pUnit->sabotageProb(pPlot->getPlot(), (ProbabilityTypes)eProbStyle) : -1;
}

bool CyUnit::canSabotage(CyPlot* pPlot, bool bTestVisible)
{
	return m_pUnit ? m_pUnit->canSabotage(pPlot->getPlot(), bTestVisible) : false;
}

int CyUnit::destroyCost(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->destroyCost(pPlot->getPlot()) : -1;
}

int CyUnit::destroyProb(CyPlot* pPlot, int /*ProbabilityTypes*/ eProbStyle)
{
	return m_pUnit ? m_pUnit->destroyProb(pPlot->getPlot(), (ProbabilityTypes)eProbStyle) : -1;
}

bool CyUnit::canDestroy(CyPlot* pPlot, bool bTestVisible)
{
	return m_pUnit ? m_pUnit->canDestroy(pPlot->getPlot(), bTestVisible) : false;
}

int CyUnit::stealPlansCost(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->stealPlansCost(pPlot->getPlot()) : -1;
}

int CyUnit::stealPlansProb(CyPlot* pPlot, int /*ProbabilityTypes*/ eProbStyle)
{
	return m_pUnit ? m_pUnit->stealPlansProb(pPlot->getPlot(), (ProbabilityTypes) eProbStyle) : -1;
}

bool CyUnit::canStealPlans(CyPlot* pPlot, bool bTestVisible)
{
	return m_pUnit ? m_pUnit->canStealPlans(pPlot->getPlot(), bTestVisible) : false;
}

bool CyUnit::canFound(CyPlot* pPlot, bool bTestVisible)
{
	return m_pUnit ? m_pUnit->canFound(pPlot->getPlot(), bTestVisible) : false;
}

bool CyUnit::canSpread(CyPlot* pPlot, int /*ReligionTypes*/ eReligion, bool bTestVisible)
{
	return m_pUnit ? m_pUnit->canSpread(pPlot->getPlot(), (ReligionTypes) eReligion, bTestVisible) : false;
}

bool CyUnit::canJoin(CyPlot* pPlot, int /*SpecialistTypes*/ eSpecialist)
{
	return m_pUnit ? m_pUnit->canFound(pPlot->getPlot(), (SpecialistTypes) eSpecialist) : false;
}

bool CyUnit::canConstruct(CyPlot* pPlot, int /*BuildingTypes*/ eBuilding)
{
	return m_pUnit ? m_pUnit->canConstruct(pPlot->getPlot(), (BuildingTypes) eBuilding) : false;
}

int /*TechTypes*/ CyUnit::getDiscoveryTech()
{
	return m_pUnit ? (TechTypes) m_pUnit->getDiscoveryTech() : -1;
}

int CyUnit::getDiscoverResearch(int /*TechTypes*/ eTech)
{
	return m_pUnit ? m_pUnit->getDiscoverResearch((TechTypes) eTech) : -1;
}

bool CyUnit::canDiscover()
{
	return m_pUnit ? m_pUnit->canDiscover() : false;
}

int CyUnit::getMaxHurryProduction(CyCity* pCity)
{
	return m_pUnit ? m_pUnit->getMaxHurryProduction(pCity->getCity()) : -1;
}

int CyUnit::getHurryProduction(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->getHurryProduction(pPlot->getPlot()) : -1;
}

bool CyUnit::canHurry(CyPlot* pPlot, bool bTestVisible)
{
	return m_pUnit ? m_pUnit->canHurry(pPlot->getPlot(), bTestVisible) : false;
}

int CyUnit::getTradeGold(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->getTradeGold(pPlot->getPlot()) : -1;
}

bool CyUnit::canTrade(CyPlot* pPlot, bool bTestVisible)
{
	return m_pUnit ? m_pUnit->canTrade(pPlot->getPlot(), bTestVisible) : false;
}

int CyUnit::getGreatWorkCulture(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->getGreatWorkCulture(pPlot->getPlot()) : -1;
}

bool CyUnit::canGreatWork(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canGreatWork(pPlot->getPlot()) : false;
}

int CyUnit::getEspionagePoints(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->getEspionagePoints(pPlot->getPlot()) : -1;
}

bool CyUnit::canInfiltrate(CyPlot* pPlot, bool bTestVisible)
{
	return m_pUnit ? m_pUnit->canInfiltrate(pPlot->getPlot(), bTestVisible) : false;
}

bool CyUnit::canEspionage(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canEspionage(pPlot->getPlot()) : false;
}

bool CyUnit::canGoldenAge(CyPlot* pPlot, bool bTestVisible)
{
	return m_pUnit ? m_pUnit->canGoldenAge(pPlot->getPlot(), bTestVisible) : false;
}

bool CyUnit::canBuild(CyPlot* pPlot, int /*BuildTypes*/ eBuild, bool bTestVisible)
{
	return m_pUnit ? m_pUnit->canBuild(pPlot->getPlot(), (BuildTypes) eBuild, bTestVisible) : false;
}

int CyUnit::canLead(CyPlot* pPlot, int iUnitId) const
{
	return m_pUnit ? m_pUnit->canLead(pPlot->getPlot(), iUnitId) : 0;
}

bool CyUnit::lead(int iUnitId)
{
	return m_pUnit ? m_pUnit->lead(iUnitId) : false;
}

int CyUnit::canGiveExperience(CyPlot* pPlot) const
{
	return m_pUnit ? m_pUnit->canGiveExperience(pPlot->getPlot()) : 0;
}

bool CyUnit::giveExperience()
{
	return m_pUnit ? m_pUnit->giveExperience() : false;
}

bool CyUnit::canPromote(int /*PromotionTypes*/ ePromotion, int iLeaderUnitId)
{
	return m_pUnit ? m_pUnit->canPromote((PromotionTypes) ePromotion, iLeaderUnitId) : false;
}

void CyUnit::promote(int /*PromotionTypes*/ ePromotion, int iLeaderUnitId)
{
	if (m_pUnit)
		m_pUnit->promote((PromotionTypes) ePromotion, iLeaderUnitId);
}

int CyUnit::upgradePrice(int /*UnitTypes*/ eUnit)
{
	return m_pUnit ? m_pUnit->upgradePrice((UnitTypes) eUnit) : -1;
}

bool CyUnit::upgradeAvailable(int /*UnitTypes*/ eFromUnit, int /*UnitClassTypes*/ eToUnitClass, int iCount)
{
	FAssert(iCount == 0);
	return m_pUnit ? m_pUnit->upgradeAvailable((UnitTypes) eFromUnit, (UnitClassTypes) eToUnitClass) : false;
}

bool CyUnit::canUpgrade(int /*UnitTypes*/ eUnit, bool bTestVisible)			
{
	return m_pUnit ? m_pUnit->canUpgrade((UnitTypes)eUnit, bTestVisible) : false;
}

bool CyUnit::hasUpgrade(bool bSearch)			
{
	return m_pUnit ? m_pUnit->hasUpgrade(bSearch) : false;
}

int /*HandicapTypes*/ CyUnit::getHandicapType()
{
	return m_pUnit ? (int) m_pUnit->getHandicapType() : (int) NO_HANDICAP;
}

int /*CivilizationTypes*/ CyUnit::getCivilizationType()
{
	return m_pUnit ? (int) m_pUnit->getCivilizationType() : (int) NO_CIVILIZATION;
}

int /*SpecialUnitTypes*/ CyUnit::getSpecialUnitType()
{
	return m_pUnit ? (int) m_pUnit->getSpecialUnitType() : (int) NO_SPECIALUNIT;
}

int /*UnitTypes*/ CyUnit::getCaptureUnitType(int /*CivilizationTypes*/ eCivilization)
{
	return m_pUnit ? m_pUnit->getCaptureUnitType((CivilizationTypes)eCivilization) : -1;
}

int /*UnitCombatTypes*/ CyUnit::getUnitCombatType()
{
	return m_pUnit ? (int) m_pUnit->getUnitCombatType() : (int) NO_UNITCOMBAT;
}

int /*DomainTypes*/ CyUnit::getDomainType()
{
	return m_pUnit ? (int) m_pUnit->getDomainType() : (int) NO_DOMAIN;
}

int /*InvisibleTypes*/ CyUnit::getInvisibleType()
{
	return m_pUnit ? (int) m_pUnit->getInvisibleType() : (int) NO_INVISIBLE;
}

int CyUnit::getNumSeeInvisibleTypes()
{
	return m_pUnit ? m_pUnit->getNumSeeInvisibleTypes() : -1;
}

int /*InvisibleTypes*/ CyUnit::getSeeInvisibleType(int i)
{
	return m_pUnit ? (int) m_pUnit->getSeeInvisibleType(i) : (int) NO_INVISIBLE;
}

int CyUnit::flavorValue(int /*FlavorTypes*/ eFlavor)
{
	return m_pUnit ? m_pUnit->flavorValue((FlavorTypes) eFlavor) : -1;
}

bool CyUnit::isBarbarian()
{
	return m_pUnit ? m_pUnit->isBarbarian() : false;
}

bool CyUnit::isNPC()
{
	return m_pUnit ? m_pUnit->isNPC() : false;
}

bool CyUnit::isHominid()
{
	return m_pUnit ? m_pUnit->isHominid() : false;
}

bool CyUnit::isHuman()
{
	return m_pUnit ? m_pUnit->isHuman() : false;
}

int CyUnit::visibilityRange()
{
	return m_pUnit ? m_pUnit->visibilityRange() : -1;
}

int CyUnit::baseMoves()
{
	return m_pUnit ? m_pUnit->baseMoves() : -1;
}

int CyUnit::maxMoves()
{
	return m_pUnit ? m_pUnit->maxMoves() : -1;
}

int CyUnit::movesLeft()
{
	return m_pUnit ? m_pUnit->movesLeft() : -1;
}

bool CyUnit::canMove()
{
	return m_pUnit ? m_pUnit->canMove() : false;
}

bool CyUnit::hasMoved()
{
	return m_pUnit ? m_pUnit->hasMoved() : false;
}

int CyUnit::airRange()
{
	return m_pUnit ? m_pUnit->airRange() : -1;
}

int CyUnit::nukeRange()
{
	return m_pUnit ? m_pUnit->nukeRange() : -1;
}

bool CyUnit::canBuildRoute()
{
	return m_pUnit ? m_pUnit->canBuildRoute() : false;
}

int /*BuildTypes*/ CyUnit::getBuildType()
{
	return (int) (m_pUnit ? m_pUnit->getBuildType() : NO_BUILD);
}

int CyUnit::workRate(bool bMax)
{
	return m_pUnit ? m_pUnit->workRate(bMax) : -1;
}

bool CyUnit::isAnimal()
{
	return m_pUnit ? m_pUnit->isAnimal() : false;
}

bool CyUnit::isNoBadGoodies()
{
	return m_pUnit ? m_pUnit->isNoBadGoodies() : false;
}

bool CyUnit::isOnlyDefensive()
{
	return m_pUnit ? m_pUnit->isOnlyDefensive() : false;
}

bool CyUnit::isNoCapture()
{
	return m_pUnit ? m_pUnit->isNoCapture() : false;
}

bool CyUnit::isRivalTerritory()
{
	return m_pUnit ? m_pUnit->isRivalTerritory() : false;
}

bool CyUnit::isMilitaryHappiness()
{
	return m_pUnit ? m_pUnit->isMilitaryHappiness() : false;
}

bool CyUnit::isInvestigate()
{
	return m_pUnit ? m_pUnit->isInvestigate() : false;
}

bool CyUnit::isCounterSpy()
{
	return m_pUnit ? m_pUnit->isCounterSpy() : false;
}

bool CyUnit::isFound()
{
	return m_pUnit ? m_pUnit->isFound() : false;
}
/********************************************************************************/
/**		REVOLUTION_MOD							1/1/08				DPII		*/
/**																				*/
/**		 																		*/
/********************************************************************************/
/*
bool CyUnit::isCanBeRebel()
{
	return m_pUnit ? m_pUnit->isCanBeRebel() : false;
}

bool CyUnit::isCanRebelCapture()
{
	return m_pUnit ? m_pUnit->isCanRebelCapture() : false;
}

bool CyUnit::isCannotDefect()
{
	return m_pUnit ? m_pUnit->isCannotDefect() : false;
}

bool CyUnit::isCanQuellRebellion()
{
	return m_pUnit ? m_pUnit->isCanQuellRebellion() : false;
}
*/
/********************************************************************************/
/**		REVOLUTION_MOD							END								*/
/********************************************************************************/

bool CyUnit::isGoldenAge()
{
	return m_pUnit ? m_pUnit->isGoldenAge() : false;
}

bool CyUnit::canCoexistWithEnemyUnit(int /*TeamTypes*/ eTeam)
{
	return m_pUnit ? m_pUnit->canCoexistWithTeamOnPlot((TeamTypes)eTeam, *m_pUnit->plot()) : false;
}

bool CyUnit::isFighting()
{
	return m_pUnit ? m_pUnit->isFighting() : false;
}

bool CyUnit::isAttacking()
{
	return m_pUnit ? m_pUnit->isAttacking() : false;
}

bool CyUnit::isDefending()
{
	return m_pUnit ? m_pUnit->isDefending() : false;
}

bool CyUnit::isCombat()
{
	return m_pUnit ? m_pUnit->isCombat() : false;
}

int CyUnit::maxHitPoints()
{
	return m_pUnit ? m_pUnit->maxHitPoints() : -1;
}

int CyUnit::currHitPoints()
{
	return m_pUnit ? m_pUnit->currHitPoints() : -1;
}

bool CyUnit::isHurt()
{
	return m_pUnit ? m_pUnit->isHurt() : false;
}

bool CyUnit::isDead()
{
	return m_pUnit ? m_pUnit->isDead() : false;
}

void CyUnit::setBaseCombatStr(int iCombat)
{
	if (m_pUnit)
	{
		m_pUnit->setBaseCombatStr(iCombat);
	}
}

int CyUnit::baseCombatStr()
{
	return m_pUnit ? m_pUnit->baseCombatStr() : -1;
}

float CyUnit::fbaseCombatStr()
{
	return m_pUnit ? m_pUnit->fbaseCombatStr() : -1;
}

int CyUnit::maxCombatStr(CyPlot* pPlot, CyUnit* pAttacker)
{
	return m_pUnit ? m_pUnit->maxCombatStr(pPlot->getPlot(), pAttacker->getUnit()) : -1;
}

int CyUnit::currCombatStr(CyPlot* pPlot, CyUnit* pAttacker)
{
	return m_pUnit ? m_pUnit->currCombatStr(pPlot->getPlot(), pAttacker->getUnit()) : -1;
}

int CyUnit::currFirepower(CyPlot* pPlot, CyUnit* pAttacker)
{
	return m_pUnit ? m_pUnit->currFirepower(pPlot->getPlot(), pAttacker->getUnit()) : -1;
}

float CyUnit::maxCombatStrFloat(CyPlot* pPlot, CyUnit* pAttacker)
{
	return m_pUnit ? m_pUnit->maxCombatStrFloat(pPlot->getPlot(), pAttacker->getUnit()) : 0.0f;
}

float CyUnit::currCombatStrFloat(CyPlot* pPlot, CyUnit* pAttacker)
{
	return m_pUnit ? m_pUnit->currCombatStrFloat(pPlot->getPlot(), pAttacker->getUnit()) : 0.0f;
}

bool CyUnit::canFight()
{
	return m_pUnit ? m_pUnit->canFight() : false;
}

bool CyUnit::canAttack()
{
	return m_pUnit ? m_pUnit->canAttack() : false;
}

bool CyUnit::canDefend(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canDefend(pPlot->getPlot()) : false;
}

bool CyUnit::canSiege(int /*TeamTypes*/ eTeam)
{
	return m_pUnit ? m_pUnit->canSiege((TeamTypes) eTeam) : false;
}

int CyUnit::airBaseCombatStr()
{
	return m_pUnit ? m_pUnit->airBaseCombatStr() : -1;
}

float CyUnit::fairBaseCombatStr()
{
	return m_pUnit ? m_pUnit->fairBaseCombatStr() : -1;
}

int CyUnit::airMaxCombatStr(CyUnit* pOther)
{
	return m_pUnit ? m_pUnit->airMaxCombatStr(pOther->getUnit()) : -1;
}

int CyUnit::airCurrCombatStr(CyUnit* pOther)
{
	return m_pUnit ? m_pUnit->airCurrCombatStr(pOther->getUnit()) : -1;
}

float CyUnit::airMaxCombatStrFloat(CyUnit* pOther)
{
	return m_pUnit ? m_pUnit->airMaxCombatStrFloat(pOther->getUnit()) : -1;
}

float CyUnit::airCurrCombatStrFloat(CyUnit* pOther)
{
	return m_pUnit ? m_pUnit->airCurrCombatStrFloat(pOther->getUnit()) : -1;
}

int CyUnit::combatLimit()
{
	return m_pUnit ? m_pUnit->combatLimit() : -1;
}

int CyUnit::airCombatLimit()
{
	return m_pUnit ? m_pUnit->airCombatLimit() : -1;
}

bool CyUnit::canAirAttack()
{
	return m_pUnit ? m_pUnit->canAirAttack() : false;
}

bool CyUnit::canAirDefend(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canAirDefend(pPlot->getPlot()) : false;
}

int CyUnit::airCombatDamage(CyUnit* pDefender)
{
	return m_pUnit ? m_pUnit->airCombatDamage(pDefender->getUnit()) : -1;
}

CyUnit* CyUnit::bestInterceptor(CyPlot* pPlot)
{
	return m_pUnit ? new CyUnit(m_pUnit->bestInterceptor(pPlot->getPlot())) : false;
}

bool CyUnit::isAutomated()
{
	return m_pUnit ? m_pUnit->isAutomated() : false;
}
/************************************************************************************************/
/* RevDCM	                  Start		 21/12/2010                                             */
/*                                                                                              */
/* Advanced Automations                                                                         */
/************************************************************************************************/
bool CyUnit::isAutoPromoting()
{
	return m_pUnit ? m_pUnit->isAutoPromoting() : false;
}
bool CyUnit::isAutoUpgrading()
{
	return m_pUnit ? m_pUnit->isAutoUpgrading() : false;
}
/************************************************************************************************/
/* Advanced Automations		end                                                                 */
/************************************************************************************************/

bool CyUnit::isWaiting()
{
	return m_pUnit ? m_pUnit->isWaiting() : false;
}

bool CyUnit::isFortifyable()
{
	return m_pUnit ? m_pUnit->isFortifyable() : false;
}

bool CyUnit::isEstablishable()
{
	return /*m_pUnit ? m_pUnit->isEstablishable() : */false;
}

bool CyUnit::isEscapable()
{
	return /*m_pUnit ? m_pUnit->isEscapable() : */false;
}

int CyUnit::fortifyModifier()
{
	return m_pUnit ? m_pUnit->fortifyModifier() : -1;
}
//TB Combat Mods begin
int CyUnit::fortifyRepelModifier()
{
	return m_pUnit ? m_pUnit->fortifyRepelModifier() : -1;
}
//TB Combat Mods end
int CyUnit::experienceNeeded()
{
	return m_pUnit ? m_pUnit->experienceNeeded() : -1;
}

int CyUnit::attackXPValue()
{
	return m_pUnit ? m_pUnit->attackXPValue() : -1;
}

int CyUnit::defenseXPValue()
{
	return m_pUnit ? m_pUnit->defenseXPValue() : -1;
}

int CyUnit::firstStrikes()
{
	return m_pUnit ? m_pUnit->firstStrikes() : -1;
}

int CyUnit::chanceFirstStrikes()
{
	return m_pUnit ? m_pUnit->chanceFirstStrikes() : -1;
}

int CyUnit::maxFirstStrikes()
{
	return m_pUnit ? m_pUnit->maxFirstStrikes() : -1;
}

bool CyUnit::isRanged()
{
	return m_pUnit ? m_pUnit->isRanged() : false;
}

bool CyUnit::alwaysInvisible()
{
	return m_pUnit ? m_pUnit->alwaysInvisible() : false;
}

bool CyUnit::immuneToFirstStrikes()
{
	return m_pUnit ? m_pUnit->immuneToFirstStrikes() : false;
}

bool CyUnit::noDefensiveBonus()
{
	return m_pUnit ? m_pUnit->noDefensiveBonus() : false;
}

bool CyUnit::ignoreBuildingDefense()
{
	return m_pUnit ? m_pUnit->ignoreBuildingDefense() : false;
}

bool CyUnit::canMoveImpassable()
{
	return m_pUnit ? m_pUnit->canMoveImpassable() : false;
}

bool CyUnit::canMoveAllTerrain()
{
	return m_pUnit ? m_pUnit->canMoveAllTerrain() : false;
}

bool CyUnit::flatMovementCost()
{
	return m_pUnit ? m_pUnit->flatMovementCost() : false;
}

bool CyUnit::ignoreTerrainCost()
{
	return m_pUnit ? m_pUnit->ignoreTerrainCost() : false;
}

bool CyUnit::isNeverInvisible()
{
	return m_pUnit ? m_pUnit->isNeverInvisible() : false;
}

bool CyUnit::isInvisible(int /*TeamTypes*/ eTeam, bool bDebug)
{
	return m_pUnit ? m_pUnit->isInvisible((TeamTypes) eTeam, bDebug) : false;
}

bool CyUnit::isNukeImmune()
{
	return m_pUnit ? m_pUnit->isNukeImmune() : false;
}

/************************************************************************************************/
/* REVDCM_OC                              02/16/10                                phungus420    */
/*                                                                                              */
/* Inquisitions                                                                                 */
/************************************************************************************************/
bool CyUnit::isInquisitor()
{
	return m_pUnit ? m_pUnit->isInquisitor() : false;
}
/************************************************************************************************/
/* REVDCM_OC                               END                                                  */
/************************************************************************************************/

int CyUnit::maxInterceptionProbability()
{
	return m_pUnit ? m_pUnit->maxInterceptionProbability() : -1;
}

int CyUnit::currInterceptionProbability()
{
	return m_pUnit ? m_pUnit->currInterceptionProbability() : -1;
}

int CyUnit::evasionProbability()
{
	return m_pUnit ? m_pUnit->evasionProbability() : -1;
}

int CyUnit::withdrawalProbability()
{
	return m_pUnit ? m_pUnit->withdrawalProbability() : -1;
}

//TB Combat Mods Begin
int CyUnit::attackCombatModifierTotal()
{
	return m_pUnit ? m_pUnit->attackCombatModifierTotal() : -1;
}

int CyUnit::defenseCombatModifierTotal()
{
	return m_pUnit ? m_pUnit->defenseCombatModifierTotal() : -1;
}

int CyUnit::pursuitProbability()
{
	return m_pUnit ? m_pUnit->pursuitProbability() : -1;
}

int CyUnit::earlyWithdrawTotal()
{
	return m_pUnit ? m_pUnit->earlyWithdrawTotal() : -1;
}

int CyUnit::vsBarbsModifier()
{
	return m_pUnit ? m_pUnit->vsBarbsModifier() : -1;
}

int CyUnit::armorTotal()
{
	return m_pUnit ? m_pUnit->armorTotal() : -1;
}

int CyUnit::punctureTotal()
{
	return m_pUnit ? m_pUnit->punctureTotal() : -1;
}

int CyUnit::damageModifierTotal()
{
	return m_pUnit ? m_pUnit->damageModifierTotal() : -1;
}

int CyUnit::overrunTotal()
{
	return m_pUnit ? m_pUnit->overrunTotal() : -1;
}

int CyUnit::repelTotal()
{
	return m_pUnit ? m_pUnit->repelTotal() : -1;
}

int CyUnit::fortRepelTotal()
{
	return m_pUnit ? m_pUnit->fortRepelTotal() : -1;
}

int CyUnit::repelRetriesTotal()
{
	return m_pUnit ? m_pUnit->repelRetriesTotal() : -1;
}

int CyUnit::unyieldingTotal()
{
	return m_pUnit ? m_pUnit->unyieldingTotal() : -1;
}

int CyUnit::knockbackTotal()
{
	return m_pUnit ? m_pUnit->knockbackTotal() : -1;
}

int CyUnit::knockbackRetriesTotal()
{
	return m_pUnit ? m_pUnit->knockbackRetriesTotal() : -1;
}

bool CyUnit::canStampede()
{
	return m_pUnit ? m_pUnit->canStampede() : false;
}

int CyUnit::strAdjperRndTotal()
{
	return m_pUnit ? m_pUnit->strAdjperRndTotal() : -1;
}

int CyUnit::strAdjperAttTotal()
{
	return m_pUnit ? m_pUnit->strAdjperAttTotal() : -1;
}

int CyUnit::strAdjperDefTotal()
{
	return m_pUnit ? m_pUnit->strAdjperDefTotal() : -1;
}

int CyUnit::withdrawAdjperAttTotal()
{
	return m_pUnit ? m_pUnit->withdrawAdjperAttTotal() : -1;
}

int CyUnit::currentStrAdjperRndTotal()
{
	return m_pUnit ? m_pUnit->currentStrAdjperRndTotal() : -1;
}

int CyUnit::currentStrAdjperAttTotal()
{
	return m_pUnit ? m_pUnit->currentStrAdjperAttTotal() : -1;
}

int CyUnit::currentStrAdjperDefTotal()
{
	return m_pUnit ? m_pUnit->currentStrAdjperDefTotal() : -1;
}

int CyUnit::currentWithdrawAdjperAttTotal()
{
	return m_pUnit ? m_pUnit->currentWithdrawAdjperAttTotal() : -1;
}

int CyUnit::unnerveTotal()
{
	return m_pUnit ? m_pUnit->unnerveTotal() : -1;
}

int CyUnit::encloseTotal()
{
	return m_pUnit ? m_pUnit->encloseTotal() : -1;
}

int CyUnit::lungeTotal()
{
	return m_pUnit ? m_pUnit->lungeTotal() : -1;
}

int CyUnit::dynamicDefenseTotal()
{
	return m_pUnit ? m_pUnit->dynamicDefenseTotal() : -1;
}

bool CyUnit::canAnimalIgnoresBorders()
{
	return m_pUnit ? m_pUnit->canAnimalIgnoresBorders() : false;
}

bool CyUnit::canOnslaught()
{
	return m_pUnit ? m_pUnit->canOnslaught() : false;
}

bool CyUnit::hasSubCombatType(int /*UnitCombatTypes*/ eCombatType)
{
	return m_pUnit ? m_pUnit->hasSubCombatType((UnitCombatTypes) eCombatType): false;
}

bool CyUnit::hasCureAfflictionType(int /*PromotionLineTypes*/ ePromotionLineType)
{
	return m_pUnit ? m_pUnit->hasCureAfflictionType((PromotionLineTypes) ePromotionLineType): false;
}

int CyUnit::fortitudeTotal()
{
	return m_pUnit ? m_pUnit->fortitudeTotal() : -1;
}

//int CyUnit::aidTotal()
//{
//	return m_pUnit ? m_pUnit->aidTotal() : -1;
//}
//TB Combat Mods End

int CyUnit::collateralDamage()
{
	return m_pUnit ? m_pUnit->collateralDamage() : -1;
}

int CyUnit::collateralDamageLimit()
{
	return m_pUnit ? m_pUnit->collateralDamageLimit() : -1;
}

int CyUnit::collateralDamageMaxUnits()
{
	return m_pUnit ? m_pUnit->collateralDamageMaxUnits() : -1;
}

int CyUnit::cityAttackModifier()
{
	return m_pUnit ? m_pUnit->cityAttackModifier() : -1;
}

int CyUnit::cityDefenseModifier()
{
	return m_pUnit ? m_pUnit->cityDefenseModifier() : -1;
}

int CyUnit::animalCombatModifier()
{
	return m_pUnit ? m_pUnit->animalCombatModifier() : -1;
}

int CyUnit::hillsAttackModifier()
{
	return m_pUnit ? m_pUnit->hillsAttackModifier() : -1;
}

int CyUnit::hillsDefenseModifier()
{
	return m_pUnit ? m_pUnit->hillsDefenseModifier() : -1;
}

int CyUnit::terrainAttackModifier(int /*TerrainTypes*/ eTerrain)
{
	return m_pUnit ? m_pUnit->terrainAttackModifier((TerrainTypes) eTerrain) : -1;
}

int CyUnit::terrainDefenseModifier(int /*TerrainTypes*/ eTerrain)
{
	return m_pUnit ? m_pUnit->terrainDefenseModifier((TerrainTypes) eTerrain) : -1;
}

int CyUnit::featureAttackModifier(int /*FeatureTypes*/ eFeature)
{
	return m_pUnit ? m_pUnit->featureAttackModifier((FeatureTypes) eFeature) : -1;
}

int CyUnit::featureDefenseModifier(int /*FeatureTypes*/ eFeature)
{
	return m_pUnit ? m_pUnit->featureDefenseModifier((FeatureTypes) eFeature) : -1;
}

int CyUnit::unitClassAttackModifier(int /*UnitClassTypes*/ eUnitClass)
{
	return m_pUnit ? m_pUnit->unitClassAttackModifier((UnitClassTypes) eUnitClass) : -1;
}

int CyUnit::unitClassDefenseModifier(int /*UnitClassTypes*/ eUnitClass)
{
	return m_pUnit ? m_pUnit->unitClassDefenseModifier((UnitClassTypes) eUnitClass) : -1;
}

int CyUnit::unitCombatModifier(int /*UnitCombatTypes*/ eUnitCombat)
{
	return m_pUnit ? m_pUnit->unitCombatModifier((UnitCombatTypes) eUnitCombat) : -1;
}

int CyUnit::domainModifier(int /*DomainTypes*/ eDomain)
{
	return m_pUnit ? m_pUnit->domainModifier((DomainTypes) eDomain) : -1;
}

int CyUnit::bombardRate()
{
	return m_pUnit ? m_pUnit->getBombardRate() : -1;
}

int CyUnit::airBombBaseRate()
{
	return m_pUnit ? m_pUnit->getAirBombBaseRate() : -1;
}

int CyUnit::airBombCurrRate()
{
	return m_pUnit ? m_pUnit->getAirBombCurrRate() : -1;
}

int /*SpecialUnitTypes*/ CyUnit::specialCargo()
{
	return m_pUnit ? (int) m_pUnit->specialCargo() : (int) NO_SPECIALUNIT;
}

int /*DomainTypes*/ CyUnit::domainCargo()
{
	return m_pUnit ? (int) m_pUnit->domainCargo() : (int) NO_DOMAIN;
}

int CyUnit::cargoSpace()
{
	return m_pUnit ? m_pUnit->cargoSpace() : -1;
}

void CyUnit::changeCargoSpace(int iChange)
{
	if (m_pUnit)
		m_pUnit->changeCargoSpace(iChange);
}

bool CyUnit::isFull()
{
	return m_pUnit ? m_pUnit->isFull() : false;
}

int CyUnit::cargoSpaceAvailable(int /*SpecialUnitTypes*/ eSpecialCargo, int /*DomainTypes*/ eDomainCargo)	 
{
	return m_pUnit ? m_pUnit->cargoSpaceAvailable((SpecialUnitTypes) eSpecialCargo, (DomainTypes) eDomainCargo) : -1;
}

bool CyUnit::hasCargo()
{
	return m_pUnit ? m_pUnit->hasCargo() : false;
}

bool CyUnit::canCargoAllMove()
{
	return m_pUnit ? m_pUnit->canCargoAllMove() : false;
}

int CyUnit::getUnitAICargo(UnitAITypes eUnitAI)
{
	return m_pUnit ? m_pUnit->getUnitAICargo(eUnitAI) : -1;
}

int CyUnit::getID()
{
	return m_pUnit ? m_pUnit->getID() : -1;
}

int CyUnit::getGroupID()
{
	return m_pUnit ? m_pUnit->getGroupID() : -1;
}

bool CyUnit::isInGroup()
{
	return m_pUnit ? m_pUnit->isInGroup() : false;
}

bool CyUnit::isGroupHead()
{
	return m_pUnit ? m_pUnit->isGroupHead() : false;
}

CySelectionGroup* CyUnit::getGroup()
{
	return m_pUnit ? new CySelectionGroup( m_pUnit->getGroup() ) : NULL;
}

int CyUnit::getHotKeyNumber()
{
	return m_pUnit ? m_pUnit->getHotKeyNumber() : -1;
}

void CyUnit::setHotKeyNumber(int iNewValue)
{
	if (m_pUnit)
		m_pUnit->setHotKeyNumber(iNewValue);
}

int CyUnit::getX()
{
	return m_pUnit ? m_pUnit->getX() : -1;
}

int CyUnit::getY()
{
	return m_pUnit ? m_pUnit->getY() : -1;
}

void CyUnit::setXY(int iX, int iY, bool bGroup, bool bUpdate, bool bShow)
{
	if (m_pUnit)
	{
		//GC.getGame().logOOSSpecial(23, m_pUnit->getID(), iX, iY);
		return m_pUnit->setXY(iX, iY, bGroup, bUpdate, bShow);
	}
}

bool CyUnit::at(int iX, int iY)
{
	return m_pUnit ? m_pUnit->at(iX, iY) : false;
}

bool CyUnit::atPlot(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->atPlot(pPlot->getPlot()) : false;
}

CyPlot* CyUnit::plot()
{
	return m_pUnit ? new CyPlot(m_pUnit->plot()) : NULL;
}

CyArea* CyUnit::area()
{
	return m_pUnit ? new CyArea(m_pUnit->area()) : NULL;
}

CyPlot* CyUnit::getReconPlot()
{
	return m_pUnit ? new CyPlot(m_pUnit->getReconPlot()) : NULL;
}

void CyUnit::setReconPlot(CyPlot* pNewValue)
{
	if (m_pUnit)
		m_pUnit->setReconPlot(pNewValue->getPlot());
}

int CyUnit::getGameTurnCreated()
{
	return m_pUnit ? m_pUnit->getGameTurnCreated() : -1;
}

int CyUnit::getDamage()
{
	return m_pUnit ? m_pUnit->getDamage() : -1;
}

void CyUnit::setDamage(int iNewValue, int /*PlayerTypes*/ ePlayer)
{
	if (m_pUnit)
		m_pUnit->setDamage(iNewValue, (PlayerTypes)ePlayer);
}

void CyUnit::changeDamage(int iChange, int /*PlayerTypes*/ ePlayer)
{
	if (m_pUnit)
		m_pUnit->changeDamage(iChange, (PlayerTypes)ePlayer);
}

int CyUnit::getMoves()
{
	return m_pUnit ? m_pUnit->getMoves() : -1;
}

void CyUnit::setMoves(int iNewValue)
{
	if (m_pUnit)
		m_pUnit->setMoves(iNewValue);
}

void CyUnit::changeMoves(int iChange)
{
	if (m_pUnit)
		m_pUnit->changeMoves(iChange);
}

void CyUnit::finishMoves()
{
	if (m_pUnit)
		m_pUnit->finishMoves();
}

int CyUnit::getExperience()
{
	return m_pUnit ? m_pUnit->getExperience() : -1;
}

void CyUnit::setExperience(int iNewValue, int iMax)
{
	if (m_pUnit)
		m_pUnit->setExperience(iNewValue, iMax);
}

void CyUnit::changeExperience(int iChange, int iMax, bool bFromCombat, bool bInBorders, bool bUpdateGlobal)
{
	if (m_pUnit)
		m_pUnit->changeExperience(iChange, iMax, bFromCombat, bInBorders, bUpdateGlobal);
}

int CyUnit::getLevel()
{
	return m_pUnit ? m_pUnit->getLevel() : -1;
}

void CyUnit::setLevel(int iNewLevel)
{
	if (m_pUnit)
		m_pUnit->setLevel(iNewLevel);
}

void CyUnit::changeLevel(int iChange)
{
	if (m_pUnit)
		m_pUnit->changeLevel(iChange);
}

int CyUnit::getFacingDirection()
{
	if(m_pUnit)
		return m_pUnit->getFacingDirection(false);
	else
		return NO_DIRECTION;
}

void CyUnit::rotateFacingDirectionClockwise()
{
	if(m_pUnit)
		return m_pUnit->rotateFacingDirectionClockwise();
}

void CyUnit::rotateFacingDirectionCounterClockwise()
{
	if(m_pUnit)
		return m_pUnit->rotateFacingDirectionCounterClockwise();
}

int CyUnit::getCargo()
{
	return m_pUnit ? m_pUnit->getCargo() : -1;
}

int CyUnit::getFortifyTurns()
{
	return m_pUnit ? m_pUnit->getFortifyTurns() : -1;
}

/************************************************************************************************/
/* REVOLUTION_MOD                         05/27/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
void CyUnit::setFortifyTurns(int iNewValue)
{
	if( m_pUnit )
		m_pUnit->setFortifyTurns(iNewValue);
}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
int CyUnit::getBlitzCount()
{
	return m_pUnit ? m_pUnit->getBlitzCount() : -1;
}

bool CyUnit::isBlitz()	 
{
	return m_pUnit ? m_pUnit->isBlitz() : false;
}

int CyUnit::getAmphibCount()
{
	return m_pUnit ? m_pUnit->getAmphibCount() : -1;
}

bool CyUnit::isAmphib()		 
{
	return m_pUnit ? m_pUnit->isAmphib() : false;
}

int CyUnit::getRiverCount()
{
	return m_pUnit ? m_pUnit->getRiverCount() : -1;
}

bool CyUnit::isRiver()	 
{
	return m_pUnit ? m_pUnit->isRiver() : false;
}

bool CyUnit::isEnemyRoute()	
{
	return m_pUnit ? m_pUnit->isEnemyRoute(): false;
}

bool CyUnit::isAlwaysHeal()			 
{
	return m_pUnit ? m_pUnit->isAlwaysHeal(): false;
}

bool CyUnit::isHillsDoubleMove()			 
{
	return m_pUnit ? m_pUnit->isHillsDoubleMove(): false;
}

int CyUnit::getExtraVisibilityRange()
{
	return m_pUnit ? m_pUnit->getExtraVisibilityRange() : -1;
}

int CyUnit::getExtraMoves()
{
	return m_pUnit ? m_pUnit->getExtraMoves() : -1;
}

int CyUnit::getExtraMoveDiscount()
{
	return m_pUnit ? m_pUnit->getExtraMoveDiscount() : -1;
}

int CyUnit::getExtraAirRange()
{
	return m_pUnit ? m_pUnit->getExtraAirRange() : -1;
}

int CyUnit::getExtraIntercept()
{
	return m_pUnit ? m_pUnit->getExtraIntercept() : -1;
}

int CyUnit::getExtraEvasion()
{
	return m_pUnit ? m_pUnit->getExtraEvasion() : -1;
}

int CyUnit::getExtraFirstStrikes()
{
	return m_pUnit ? m_pUnit->getExtraFirstStrikes() : -1;
}

int CyUnit::getExtraChanceFirstStrikes()
{
	return m_pUnit ? m_pUnit->getExtraChanceFirstStrikes() : -1;
}

int CyUnit::getExtraWithdrawal()
{
	return m_pUnit ? m_pUnit->getExtraWithdrawal() : -1;
}
//TB Combat Mods Begin
int CyUnit::getExtraPursuit()
{
	return m_pUnit ? m_pUnit->getExtraPursuit() : -1;
}
int CyUnit::getExtraEarlyWithdraw()
{
	return m_pUnit ? m_pUnit->getExtraEarlyWithdraw() : -1;
}
int CyUnit::getExtraVSBarbs()
{
	return m_pUnit ? m_pUnit->getExtraVSBarbs() : -1;
}
int CyUnit::getExtraArmor()
{
	return m_pUnit ? m_pUnit->getExtraArmor() : -1;
}
int CyUnit::getExtraPuncture()
{
	return m_pUnit ? m_pUnit->getExtraPuncture() : -1;
}
int CyUnit::getExtraDamageModifier()
{
	return m_pUnit ? m_pUnit->getExtraDamageModifier() : -1;
}
int CyUnit::getExtraOverrun()
{
	return m_pUnit ? m_pUnit->getExtraOverrun() : -1;
}
int CyUnit::getExtraRepel()
{
	return m_pUnit ? m_pUnit->getExtraRepel() : -1;
}
int CyUnit::getExtraFortRepel()
{
	return m_pUnit ? m_pUnit->getExtraFortRepel() : -1;
}
int CyUnit::getExtraRepelRetries()
{
	return m_pUnit ? m_pUnit->getExtraRepelRetries() : -1;
}
int CyUnit::getExtraUnyielding()
{
	return m_pUnit ? m_pUnit->getExtraUnyielding() : -1;
}
int CyUnit::getExtraKnockback()
{
	return m_pUnit ? m_pUnit->getExtraKnockback() : -1;
}
int CyUnit::getExtraKnockbackRetries()
{
	return m_pUnit ? m_pUnit->getExtraKnockbackRetries() : -1;
}
bool CyUnit::mayStampede()
{
	return m_pUnit ? m_pUnit->mayStampede() : false;
}
int CyUnit::getExtraStrAdjperRnd()
{
	return m_pUnit ? m_pUnit->getExtraStrAdjperRnd() : -1;
}
int CyUnit::getExtraStrAdjperAtt()
{
	return m_pUnit ? m_pUnit->getExtraStrAdjperAtt() : -1;
}
int CyUnit::getExtraStrAdjperDef()
{
	return m_pUnit ? m_pUnit->getExtraStrAdjperDef() : -1;
}
int CyUnit::getExtraWithdrawAdjperAtt()
{
	return m_pUnit ? m_pUnit->getExtraWithdrawAdjperAtt() : -1;
}
int CyUnit::getExtraUnnerve()
{
	return m_pUnit ? m_pUnit->getExtraUnnerve() : -1;
}
int CyUnit::getExtraEnclose()
{
	return m_pUnit ? m_pUnit->getExtraEnclose() : -1;
}
int CyUnit::getExtraLunge()
{
	return m_pUnit ? m_pUnit->getExtraLunge() : -1;
}
int CyUnit::getExtraDynamicDefense()
{
	return m_pUnit ? m_pUnit->getExtraDynamicDefense() : -1;
}
int CyUnit::getExtraStrength()
{
	return m_pUnit ? m_pUnit->getExtraStrength() : -1;
}
bool CyUnit::mayAnimalIgnoresBorders()
{
	return m_pUnit ? m_pUnit->mayAnimalIgnoresBorders() : false;
}
bool CyUnit::mayOnslaught()
{
	return m_pUnit ? m_pUnit->mayOnslaught() : false;
}
bool CyUnit:: hasExtraSubCombatType(int /*UnitCombatTypes*/ eCombatType)
{
	return m_pUnit ? m_pUnit->hasExtraSubCombatType((UnitCombatTypes) eCombatType): false;
}
bool CyUnit:: hasRemovesUnitCombatType(int /*UnitCombatTypes*/ eCombatType)
{
	return m_pUnit ? m_pUnit->hasRemovesUnitCombatType((UnitCombatTypes) eCombatType): false;
}
bool CyUnit:: hasExtraCureAffliction(int /*PromotionLineTypes*/ ePromotionLineType)
{
	return m_pUnit ? m_pUnit->hasExtraCureAffliction((PromotionLineTypes) ePromotionLineType): false;
}
int CyUnit::getExtraFortitude()
{
	return m_pUnit ? m_pUnit->getExtraFortitude() : -1;
}
//TB Combat Mods end
int CyUnit::getExtraCollateralDamage()
{
	return m_pUnit ? m_pUnit->getExtraCollateralDamage() : -1;
}

int CyUnit::getExtraEnemyHeal()
{
	return m_pUnit ? m_pUnit->getExtraEnemyHeal() : -1;
}

int CyUnit::getExtraNeutralHeal()
{
	return m_pUnit ? m_pUnit->getExtraNeutralHeal() : -1;
}

int CyUnit::getExtraFriendlyHeal()
{
	return m_pUnit ? m_pUnit->getExtraFriendlyHeal() : -1;
}

int CyUnit::getSameTileHeal()
{
	return m_pUnit ? m_pUnit->getSameTileHeal() : -1;
}

int CyUnit::getAdjacentTileHeal()
{
	return m_pUnit ? m_pUnit->getAdjacentTileHeal() : -1;
}

int CyUnit::getExtraCombatPercent()
{
	return m_pUnit ? m_pUnit->getExtraCombatPercent() : -1;
}

int CyUnit::getExtraCityAttackPercent()
{
	return m_pUnit ? m_pUnit->getExtraCityAttackPercent() : -1;
}

int CyUnit::getExtraCityDefensePercent()
{
	return m_pUnit ? m_pUnit->getExtraCityDefensePercent() : -1;
}

int CyUnit::getExtraHillsAttackPercent()
{
	return m_pUnit ? m_pUnit->getExtraHillsAttackPercent() : -1;
}

int CyUnit::getExtraHillsDefensePercent()
{
	return m_pUnit ? m_pUnit->getExtraHillsDefensePercent() : -1;
}

int CyUnit::getRevoltProtection() const
{
	return m_pUnit ? m_pUnit->revoltProtectionTotal() : -1;
}

int CyUnit::getCollateralDamageProtection() const
{
	return m_pUnit ? m_pUnit->getCollateralDamageProtection() : -1;
}

int CyUnit::getPillageChange() const
{
	return m_pUnit ? m_pUnit->getPillageChange() : -1;
}

int CyUnit::getUpgradeDiscount() const
{
	return m_pUnit ? m_pUnit->getUpgradeDiscount() : -1;
}

int CyUnit::getExperiencePercent() const
{
	return m_pUnit ? m_pUnit->getExperiencePercent() : -1;
}

int CyUnit::getKamikazePercent() const
{
	return m_pUnit ? m_pUnit->getKamikazePercent() : -1;
}

int CyUnit::getImmobileTimer() const
{
	return m_pUnit ? m_pUnit->getImmobileTimer() : -1;
}

void CyUnit::setImmobileTimer(int iNewValue)
{
	if (m_pUnit)
	{
		m_pUnit->setImmobileTimer(iNewValue);
	}
}

bool CyUnit::isMadeAttack()	 
{
	return m_pUnit ? m_pUnit->isMadeAttack() : false;
}

void CyUnit::setMadeAttack(bool bNewValue)
{
	if (m_pUnit)
		m_pUnit->setMadeAttack(bNewValue);
}

//TB Combat Mods begin
int CyUnit::getRoundCount() const
{
	return m_pUnit ? m_pUnit->getRoundCount() : -1;
}

void CyUnit::changeRoundCount(int iChange)
{
	if (m_pUnit)
			m_pUnit->changeRoundCount(iChange);
}

int CyUnit::getAttackCount() const
{
	return m_pUnit ? m_pUnit->getAttackCount() : -1;
}

void CyUnit::changeAttackCount(int iChange)
{
	if (m_pUnit)
			m_pUnit->changeAttackCount(iChange);
}

int CyUnit::getDefenseCount() const
{
	return m_pUnit ? m_pUnit->getDefenseCount() : -1;
}

void CyUnit::changeDefenseCount(int iChange)
{
	if (m_pUnit)
			m_pUnit->changeDefenseCount(iChange);
}
//TB Combat mods end

bool CyUnit::isMadeInterception()	 
{
	return m_pUnit ? m_pUnit->isMadeInterception() : false;
}

void CyUnit::setMadeInterception(bool bNewValue)
{
	if (m_pUnit)
		m_pUnit->setMadeInterception(bNewValue);
}

bool CyUnit::isPromotionReady() 
{
	return m_pUnit ? m_pUnit->isPromotionReady() : false;
}

void CyUnit::setPromotionReady(bool bNewValue)
{
	if (m_pUnit)
		m_pUnit->setPromotionReady(bNewValue);
}

int CyUnit::getOwner()
{
	return m_pUnit ? m_pUnit->getOwner() : -1;
}

int CyUnit::getVisualOwner()
{
	return m_pUnit ? m_pUnit->getVisualOwner() : -1;
}

int CyUnit::getCombatOwner(int iForTeam)
{
	return m_pUnit ? m_pUnit->getCombatOwner((TeamTypes)iForTeam, m_pUnit->plot()) : -1;
}

int CyUnit::getTeam()
{
	return m_pUnit ? m_pUnit->getTeam() : -1;
}

int /*UnitTypes*/ CyUnit::getUnitType()
{
	return m_pUnit ? (int)m_pUnit->getUnitType() : -1;
}

int /*UnitClassTypes*/ CyUnit::getUnitClassType()
{
	return m_pUnit ? (int)m_pUnit->getUnitClassType() : -1;
}

int /*UnitTypes*/ CyUnit::getLeaderUnitType()
{
	return m_pUnit ? (int)m_pUnit->getLeaderUnitType() : -1;
}

void CyUnit::setLeaderUnitType(int leaderUnitType)
{
	if (m_pUnit)
		m_pUnit->setLeaderUnitType((UnitTypes) leaderUnitType);
}

CyUnit* CyUnit::getTransportUnit() const
{
	return m_pUnit ? new CyUnit(m_pUnit->getTransportUnit()) : NULL;
}

bool CyUnit::isCargo()
{
	return m_pUnit ? m_pUnit->isCargo() : false;
}

void CyUnit::setTransportUnit(CyUnit* pTransportUnit)
{
	if (m_pUnit)
		m_pUnit->setTransportUnit(pTransportUnit->getUnit());
}

int CyUnit::getExtraDomainModifier(int /*DomainTypes*/ eIndex)
{
	return m_pUnit ? m_pUnit->getExtraDomainModifier((DomainTypes) eIndex) : -1;
}

std::wstring CyUnit::getName()
{
	return m_pUnit ? m_pUnit->getName() : L"";
}

std::wstring CyUnit::getNameForm(int iForm)
{
	return m_pUnit ? m_pUnit->getName((uint)iForm) : L"";
}

std::wstring CyUnit::getNameKey()
{
	return m_pUnit ? m_pUnit->getNameKey() : L"";
}

std::wstring CyUnit::getNameNoDesc()
{
	return m_pUnit ? m_pUnit->getNameNoDesc() : L"";
}

void CyUnit::setName(std::wstring szNewValue)
{
	if (m_pUnit)
		m_pUnit->setName(szNewValue);
}

std::string CyUnit::getScriptData() const
{
	return m_pUnit ? m_pUnit->getScriptData() : "";
}

void CyUnit::setScriptData(std::string szNewValue)
{
	if (m_pUnit)
		m_pUnit->setScriptData(szNewValue.c_str());
}

bool CyUnit:: isTerrainDoubleMove(int /*TerrainTypes*/ eIndex)
{
	return m_pUnit ? m_pUnit->isTerrainDoubleMove((TerrainTypes) eIndex): false;
}

bool CyUnit:: isFeatureDoubleMove(int /*FeatureTypes*/ eIndex)
{
	return m_pUnit ? m_pUnit->isFeatureDoubleMove((FeatureTypes) eIndex): false;
}

int CyUnit::getExtraTerrainAttackPercent(int /*TerrainTypes*/ eIndex)
{
	return m_pUnit ? m_pUnit->getExtraTerrainAttackPercent((TerrainTypes) eIndex) : -1;
}

int CyUnit::getExtraTerrainDefensePercent(int /*TerrainTypes*/ eIndex)
{
	return m_pUnit ? m_pUnit->getExtraTerrainDefensePercent((TerrainTypes) eIndex) : -1;
}

int CyUnit::getExtraFeatureAttackPercent(int /*FeatureTypes*/ eIndex)
{
	return m_pUnit ? m_pUnit->getExtraFeatureAttackPercent((FeatureTypes) eIndex) : -1;
}

int CyUnit::getExtraFeatureDefensePercent(int /*FeatureTypes*/ eIndex)
{
	return m_pUnit ? m_pUnit->getExtraFeatureDefensePercent((FeatureTypes) eIndex) : -1;
}

int CyUnit::getExtraUnitCombatModifier(int /*UnitCombatTypes*/ eIndex)
{
	return m_pUnit ? m_pUnit->getExtraUnitCombatModifier((UnitCombatTypes) eIndex) : -1;
}

bool CyUnit::canAcquirePromotion(int /*PromotionTypes*/ ePromotion)
{
	return m_pUnit ? m_pUnit->canAcquirePromotion((PromotionTypes) ePromotion) : false;
}

bool CyUnit::canAcquirePromotionAny()
{
	return m_pUnit ? m_pUnit->canAcquirePromotionAny() : false;
}

bool CyUnit::isPromotionValid(int /*PromotionTypes*/ ePromotion)
{
	return m_pUnit ? m_pUnit->isPromotionValid((PromotionTypes) ePromotion) : false;
}

bool CyUnit::isPromotionOverriden(int /*PromotionTypes*/eIndex)
{
	return m_pUnit ? m_pUnit->isPromotionOverriden((PromotionTypes)eIndex) : false;
}

bool CyUnit::isHasPromotion(int /*PromotionTypes*/eIndex)
{
	return m_pUnit ? m_pUnit->isHasPromotion((PromotionTypes)eIndex) : false;
}

bool CyUnit::isHasUnitCombat(int /*UnitCombatTypes*/eIndex)
{
	return m_pUnit ? m_pUnit->isHasUnitCombat((UnitCombatTypes)eIndex) : false;
}

void CyUnit::setHasUnitCombat(int /*UnitCombatTypes*/ eIndex, bool bNewValue)
{
	if (m_pUnit)
	{
		m_pUnit->setHasUnitCombat((UnitCombatTypes) eIndex, bNewValue);
	}
}

void CyUnit::setHasPromotion(int /*PromotionTypes*/ eIndex, bool bNewValue)
{
	if (m_pUnit)
	{
		m_pUnit->setHasPromotion((PromotionTypes) eIndex, bNewValue);
	}
}

//TB Tags
//Team Project (3)
int CyUnit::captureProbabilityTotal()
{
	return m_pUnit ? m_pUnit->captureProbabilityTotal() : -1;
}

int CyUnit::captureResistanceTotal()
{
	return m_pUnit ? m_pUnit->captureResistanceTotal() : -1;
}

bool CyUnit::isUnitAtBaseGroup()
{
	return m_pUnit ? m_pUnit->isUnitAtBaseGroup() : false;
}

bool CyUnit::isUnitAboveBaseGroup()
{
	return m_pUnit ? m_pUnit->isUnitAboveBaseGroup() : false;
}

bool CyUnit::isUnitBelowBaseGroup()
{
	return m_pUnit ? m_pUnit->isUnitBelowBaseGroup() : false;
}


int /*UnitAITypes*/ CyUnit::getUnitAIType()
{
	return m_pUnit ? (int)m_pUnit->AI_getUnitAIType() : -1;
}

void CyUnit::setUnitAIType(int /*UnitAITypes*/ iNewValue)
{
	if (m_pUnit)
	{
		m_pUnit->AI_setUnitAIType((UnitAITypes)iNewValue);
	}
}

bool CyUnit::IsSelected( void )
{
	return m_pUnit ? m_pUnit->IsSelected() : false;
}

// Python Helper Functions
void CyUnit::centerCamera()
{
	if (m_pUnit)
	{
		gDLL->getInterfaceIFace()->centerCamera(m_pUnit);
	}
}

void CyUnit::attackForDamage(CyUnit *defender, int attakerDamageChange, int defenderDamageChange)
{
	if(m_pUnit != NULL)
	{
		m_pUnit->attackForDamage(defender->m_pUnit, attakerDamageChange, defenderDamageChange);
	}
}

void CyUnit::rangeStrike(int iX, int iY)
{
	if(m_pUnit != NULL)
	{
		m_pUnit->rangeStrike(iX, iY);
	}
}

const CvArtInfoUnit* CyUnit::getArtInfo(int i, EraTypes eEra) const
{
	return m_pUnit ? m_pUnit->getArtInfo(i, eEra) : NULL;
}

std::string CyUnit::getButton() const
{
	return m_pUnit ? m_pUnit->getButton() : "";
}
/************************************************************************************************/
/* Afforess	                  Start		 03/1/10                       Coded By: KillMePlease   */
/*                                                                                              */
/* Great Commanders                                                                             */
/************************************************************************************************/
bool CyUnit::isCommander() const
{
	return m_pUnit ? m_pUnit->isCommander(): false;
}

int CyUnit::controlPointsLeft() const
{
	return m_pUnit ? m_pUnit->controlPointsLeft() : -1;
}

int CyUnit::controlPoints() const
{
	return m_pUnit ? m_pUnit->controlPoints() : -1;
}

int CyUnit::commandRange() const
{
	return m_pUnit ? m_pUnit->commandRange() : -1;
}

int CyUnit::getExperience100()
{
	return m_pUnit ? m_pUnit->getExperience100() : -1;
}

float CyUnit::getRealExperience()
{
	return m_pUnit ? (float)m_pUnit->getExperience100() / 100 : -1;
}

void CyUnit::setExperience100(int iNewValue, int iMax)
{
	if (m_pUnit)
		m_pUnit->setExperience100(iNewValue, iMax);
}

void CyUnit::changeExperience100(int iChange, int iMax, bool bFromCombat, bool bInBorders, bool bUpdateGlobal)
{
	if (m_pUnit)
		m_pUnit->changeExperience100(iChange, iMax, bFromCombat, bInBorders, bUpdateGlobal);
}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

// < M.A.D. Nukes Start >
bool CyUnit::isMADEnabled()
{
	if(m_pUnit)
	{
		return m_pUnit->isMADEnabled();
	}
	return false;
}

CyPlot* CyUnit::getMADTargetPlot()
{
	return m_pUnit ? new CyPlot(m_pUnit->getMADTargetPlot()) : false;
}

int CyUnit::getMADTargetPlotX()
{
	return m_pUnit ? m_pUnit->getMADTargetPlotX() : false;
}

int CyUnit::getMADTargetPlotY()
{
	return m_pUnit ? m_pUnit->getMADTargetPlotY() : false;
}

PlayerTypes CyUnit::getMADTargetPlotOwner()
{
	if(m_pUnit)
	{
		return m_pUnit->getMADTargetPlotOwner();
	}
	return NO_PLAYER;
}
// < M.A.D. Nukes End   >

bool CyUnit::isHiddenNationality()
{
	return m_pUnit ? m_pUnit->isHiddenNationality() : false;
}

void CyUnit::doHNCapture()
{
	if (m_pUnit)
	{
		m_pUnit->doHNCapture();
	}
}

CyUnit* CyUnit::getCapturingUnit() const
{
	return m_pUnit ? new CyUnit(m_pUnit->getCapturingUnit()) : NULL;
}

void CyUnit::setCapturingUnit(CyUnit* pCapturingUnit)
{
	if (m_pUnit)
		m_pUnit->setCapturingUnit(pCapturingUnit->getUnit());
}