#include "CvGameCoreDLL.h"
#include "CyArea.h"
#include "CyPlot.h"
#include "CySelectionGroup.h"
#include "CyUnit.h"

//
// Python wrapper class for CvUnit
//

CyUnit::CyUnit(CvUnit* pUnit) : m_pUnit(pUnit)
{
	FAssert(m_pUnit != NULL);
}

void CyUnit::convert(const CyUnit& kUnit, bool bKillOriginal)
{
	m_pUnit->convert(kUnit.getUnit(), bKillOriginal);
}

void CyUnit::kill(bool bDelay, int /*PlayerTypes*/ ePlayer)
{
	m_pUnit->kill(bDelay, (PlayerTypes)ePlayer);
}

void CyUnit::NotifyEntity(int /*MissionTypes*/ eEvent)
{
	m_pUnit->NotifyEntity((MissionTypes)eEvent);
}

bool CyUnit::isActionRecommended(int i) const
{
	return m_pUnit->isActionRecommended(i);
}

void CyUnit::doCommand(CommandTypes eCommand, int iData1, int iData2)
{
	m_pUnit->doCommand(eCommand, iData1, iData2);
}

bool CyUnit::canMoveInto(const CyPlot& kPlot, bool bAttack, bool bDeclareWar, bool bIgnoreLoad) const
{
	return m_pUnit->canMoveInto(kPlot.getPlot(),
		(bAttack ? MoveCheck::Attack : MoveCheck::None) |
		(bDeclareWar ? MoveCheck::DeclareWar : MoveCheck::None) |
		(bIgnoreLoad ? MoveCheck::IgnoreLoad : MoveCheck::None)
	);
}

bool CyUnit::canHeal(const CyPlot& kPlot) const
{
	return m_pUnit->canHeal(kPlot.getPlot());
}

bool CyUnit::canFound(const CyPlot& kPlot, bool bTestVisible) const
{
	return m_pUnit->canFound(kPlot.getPlot(), bTestVisible);
}

bool CyUnit::canConstruct(const CyPlot& kPlot, int /*BuildingTypes*/ eBuilding) const
{
	return m_pUnit->canConstruct(kPlot.getPlot(), (BuildingTypes)eBuilding);
}

int /*TechTypes*/ CyUnit::getDiscoveryTech() const
{
	return m_pUnit->getDiscoveryTech();
}

int CyUnit::getDiscoverResearch(int /*TechTypes*/ eTech) const
{
	return m_pUnit->getDiscoverResearch((TechTypes) eTech);
}

int CyUnit::getHurryProduction(const CyPlot& kPlot) const
{
	return m_pUnit->getHurryProduction(kPlot.getPlot());
}

bool CyUnit::canHurry(const CyPlot& kPlot, bool bTestVisible) const
{
	return m_pUnit->canHurry(kPlot.getPlot(), bTestVisible);
}

bool CyUnit::canTrade(const CyPlot& kPlot, bool bTestVisible) const
{
	return m_pUnit->canTrade(kPlot.getPlot(), bTestVisible);
}

int CyUnit::getGreatWorkCulture(const CyPlot& kPlot) const
{
	return m_pUnit->getGreatWorkCulture();
}

int CyUnit::getEspionagePoints(const CyPlot& kPlot) const
{
	return m_pUnit->getEspionagePoints();
}

bool CyUnit::canBuild(const CyPlot& kPlot, int /*BuildTypes*/ eBuild, bool bTestVisible) const
{
	return m_pUnit->canBuild(kPlot.getPlot(), (BuildTypes) eBuild, bTestVisible);
}

bool CyUnit::canUpgrade(int /*UnitTypes*/ eUnit, bool bTestVisible) const
{
	return m_pUnit->canUpgrade((UnitTypes)eUnit, bTestVisible);
}

int /*HandicapTypes*/ CyUnit::getHandicapType() const
{
	return m_pUnit->getHandicapType();
}

int /*CivilizationTypes*/ CyUnit::getCivilizationType() const
{
	return m_pUnit->getCivilizationType();
}

int /*SpecialUnitTypes*/ CyUnit::getSpecialUnitType() const
{
	return m_pUnit->getSpecialUnitType();
}

int /*UnitTypes*/ CyUnit::getCaptureUnitType() const
{
	return m_pUnit->getCaptureUnitType();
}

int /*UnitCombatTypes*/ CyUnit::getUnitCombatType() const
{
	return m_pUnit->getUnitCombatType();
}

DomainTypes CyUnit::getDomainType() const
{
	return m_pUnit->getDomainType();
}

bool CyUnit::isBarbarian() const
{
	return m_pUnit->isBarbarian();
}

bool CyUnit::isNPC() const
{
	return m_pUnit->isNPC();
}

bool CyUnit::isHominid() const
{
	return m_pUnit->isHominid();
}

bool CyUnit::isHuman() const
{
	return m_pUnit->isHuman();
}

int CyUnit::baseMoves() const
{
	return m_pUnit->baseMoves();
}

int CyUnit::maxMoves() const
{
	return m_pUnit->maxMoves();
}

int CyUnit::movesLeft() const
{
	return m_pUnit->movesLeft();
}

bool CyUnit::canMove() const
{
	return m_pUnit->canMove();
}

bool CyUnit::hasMoved() const
{
	return m_pUnit->hasMoved();
}

int CyUnit::nukeRange() const
{
	return m_pUnit->nukeRange();
}

bool CyUnit::isAnimal() const
{
	return m_pUnit->isAnimal();
}

bool CyUnit::isOnlyDefensive() const
{
	return m_pUnit->isOnlyDefensive();
}

bool CyUnit::isFound() const
{
	return m_pUnit->isFound();
}

bool CyUnit::isGoldenAge() const
{
	return m_pUnit->isGoldenAge();
}

bool CyUnit::isFighting() const
{
	return m_pUnit->isFighting();
}

int CyUnit::getMaxHP() const
{
	return m_pUnit->getMaxHP();
}

int CyUnit::getHP() const
{
	return m_pUnit->getHP();
}

bool CyUnit::isHurt() const
{
	return m_pUnit->isHurt();
}

bool CyUnit::isDead() const
{
	return m_pUnit->isDead();
}

void CyUnit::setBaseCombatStr(int iCombat)
{
	m_pUnit->setBaseCombatStr(iCombat);
}

int CyUnit::baseCombatStr() const
{
	return m_pUnit->baseCombatStr();
}

bool CyUnit::canFight() const
{
	return m_pUnit->canFight();
}

int CyUnit::airBaseCombatStr() const
{
	return m_pUnit->airBaseCombatStr();
}

bool CyUnit::canAirAttack() const
{
	return m_pUnit->canAirAttack();
}

bool CyUnit::isAutomated() const
{
	return m_pUnit->isAutomated();
}

bool CyUnit::isAutoPromoting() const
{
	return m_pUnit->isAutoPromoting();
}

bool CyUnit::isAutoUpgrading() const
{
	return m_pUnit->isAutoUpgrading();
}

bool CyUnit::isWaiting() const
{
	return m_pUnit->isWaiting();
}

bool CyUnit::isFortifyable() const
{
	return m_pUnit->isFortifyable();
}

int CyUnit::experienceNeeded() const
{
	return m_pUnit->experienceNeeded();
}

bool CyUnit::isInvisible(int /*TeamTypes*/ eTeam, bool bDebug) const
{
	return m_pUnit->isInvisible((TeamTypes) eTeam, bDebug);
}

bool CyUnit::isNukeImmune() const
{
	return m_pUnit->isNukeImmune();
}

int CyUnit::bombardRate() const
{
	return m_pUnit->getBombardRate();
}

int /*SpecialUnitTypes*/ CyUnit::specialCargo() const
{
	return m_pUnit->specialCargo();
}

int /*DomainTypes*/ CyUnit::domainCargo() const
{
	return m_pUnit->domainCargo();
}

int CyUnit::cargoSpace() const
{
	return m_pUnit->cargoSpace();
}

void CyUnit::changeCargoSpace(int iChange)
{
	m_pUnit->changeCargoSpace(iChange);
}

bool CyUnit::isFull() const
{
	return m_pUnit->isFull();
}

int CyUnit::getID() const
{
	return m_pUnit->getID();
}

int CyUnit::getGroupID() const
{
	return m_pUnit->getGroupID();
}

bool CyUnit::isInGroup() const
{
	return m_pUnit->isInGroup();
}

bool CyUnit::isGroupHead() const
{
	return m_pUnit->isGroupHead();
}

CySelectionGroup* CyUnit::getGroup() const
{
	return new CySelectionGroup(m_pUnit->getGroup());
}

int CyUnit::getHotKeyNumber() const
{
	return m_pUnit->getHotKeyNumber();
}

int CyUnit::getX() const
{
	return m_pUnit->getX();
}

int CyUnit::getY() const
{
	return m_pUnit->getY();
}

void CyUnit::setXY(int iX, int iY, bool bGroup, bool bUpdate, bool bShow)
{
	m_pUnit->setXY(iX, iY, bGroup, bUpdate, bShow);
}

CyPlot* CyUnit::plot() const
{
	return new CyPlot(m_pUnit->plot());
}

CyArea* CyUnit::area() const
{
	return new CyArea(m_pUnit->area());
}

int CyUnit::getDamage() const
{
	return m_pUnit->getDamage();
}

void CyUnit::setDamage(int iNewValue, int /*PlayerTypes*/ ePlayer)
{
	m_pUnit->setDamage(iNewValue, (PlayerTypes)ePlayer);
}

void CyUnit::changeDamage(int iChange, int /*PlayerTypes*/ ePlayer)
{
	m_pUnit->changeDamage(iChange, (PlayerTypes)ePlayer);
}

int CyUnit::getMoves() const
{
	return m_pUnit->getMoves();
}

void CyUnit::setMoves(int iNewValue)
{
	m_pUnit->setMoves(iNewValue);
}

void CyUnit::changeMoves(int iChange)
{
	m_pUnit->changeMoves(iChange);
}

void CyUnit::finishMoves()
{
	m_pUnit->finishMoves();
}

int CyUnit::getExperience() const
{
	return m_pUnit->getExperience();
}

void CyUnit::setExperience(int iNewValue, int iMax)
{
	m_pUnit->setExperience(iNewValue, iMax);
}

void CyUnit::changeExperience(int iChange, int iMax, bool bFromCombat, bool bInBorders, bool bUpdateGlobal)
{
	m_pUnit->changeExperience(iChange, iMax, bFromCombat, bInBorders, bUpdateGlobal);
}

int CyUnit::getLevel() const
{
	return m_pUnit->getLevel();
}

void CyUnit::setLevel(int iNewLevel)
{
	m_pUnit->setLevel(iNewLevel);
}

int CyUnit::getFacingDirection() const
{
	return m_pUnit->getFacingDirection(false);
}

void CyUnit::rotateFacingDirectionClockwise()
{
	m_pUnit->rotateFacingDirectionClockwise();
}

void CyUnit::rotateFacingDirectionCounterClockwise()
{
	m_pUnit->rotateFacingDirectionCounterClockwise();
}

int CyUnit::getCargo() const
{
	return m_pUnit->getCargo();
}

int CyUnit::getFortifyTurns() const
{
	return m_pUnit->getFortifyTurns();
}

void CyUnit::setFortifyTurns(int iNewValue)
{
	m_pUnit->setFortifyTurns(iNewValue);
}

bool CyUnit::isRiver() const
{
	return m_pUnit->isRiver();
}

int CyUnit::getExtraMoves() const
{
	return m_pUnit->getExtraMoves();
}

int CyUnit::getRevoltProtection() const
{
	return m_pUnit->revoltProtectionTotal();
}

int CyUnit::getPillageChange() const
{
	return m_pUnit->getPillageChange();
}

int CyUnit::getUpgradeDiscount() const
{
	return m_pUnit->getUpgradeDiscount();
}

int CyUnit::getImmobileTimer() const
{
	return m_pUnit->getImmobileTimer();
}

void CyUnit::setImmobileTimer(int iNewValue)
{
	m_pUnit->setImmobileTimer(iNewValue);
}

bool CyUnit::isMadeAttack() const
{
	return m_pUnit->isMadeAttack();
}

void CyUnit::setMadeAttack(bool bNewValue)
{
	m_pUnit->setMadeAttack(bNewValue);
}

bool CyUnit::isMadeInterception() const
{
	return m_pUnit->isMadeInterception();
}

void CyUnit::setMadeInterception(bool bNewValue)
{
	m_pUnit->setMadeInterception(bNewValue);
}

bool CyUnit::isPromotionReady() const
{
	return m_pUnit->isPromotionReady();
}

void CyUnit::setPromotionReady(bool bNewValue)
{
	m_pUnit->setPromotionReady(bNewValue);
}

int CyUnit::getOwner() const
{
	return m_pUnit->getOwner();
}

int CyUnit::getVisualOwner() const
{
	return m_pUnit->getVisualOwner();
}

int CyUnit::getTeam() const
{
	return m_pUnit->getTeam();
}

int /*UnitTypes*/ CyUnit::getUnitType() const
{
	return m_pUnit->getUnitType();
}

int /*UnitTypes*/ CyUnit::getLeaderUnitType() const
{
	return m_pUnit->getLeaderUnitType();
}

void CyUnit::setLeaderUnitType(int leaderUnitType)
{
	m_pUnit->setLeaderUnitType((UnitTypes) leaderUnitType);
}

CyUnit* CyUnit::getTransportUnit() const
{
	CvUnit* unit = m_pUnit->getTransportUnit();
	return unit ? new CyUnit(unit) : NULL;
}

bool CyUnit::isCargo() const
{
	return m_pUnit->isCargo();
}

void CyUnit::setTransportUnit(const CyUnit& kTransportUnit)
{
	m_pUnit->setTransportUnit(kTransportUnit.getUnit());
}

std::wstring CyUnit::getName() const
{
	return m_pUnit->getName();
}

std::wstring CyUnit::getNameForm(int iForm) const
{
	return m_pUnit->getName((uint)iForm);
}

std::wstring CyUnit::getNameKey() const
{
	return m_pUnit->getNameKey();
}

std::wstring CyUnit::getNameNoDesc() const
{
	return m_pUnit->getNameNoDesc();
}

void CyUnit::setName(std::wstring szNewValue)
{
	m_pUnit->setName(szNewValue);
}

std::string CyUnit::getScriptData() const
{
	return m_pUnit->getScriptData();
}

void CyUnit::setScriptData(std::string szNewValue)
{
	m_pUnit->setScriptData(szNewValue.c_str());
}

bool CyUnit::canAcquirePromotion(int /*PromotionTypes*/ ePromotion) const
{
	return m_pUnit->canAcquirePromotion((PromotionTypes) ePromotion);
}

bool CyUnit::canAcquirePromotionAny() const
{
	return m_pUnit->canAcquirePromotionAny();
}

bool CyUnit::isPromotionValid(int /*PromotionTypes*/ ePromotion) const
{
	return m_pUnit->isPromotionValid((PromotionTypes) ePromotion);
}

bool CyUnit::isPromotionOverriden(int /*PromotionTypes*/eIndex) const
{
	return m_pUnit->isPromotionOverriden((PromotionTypes)eIndex);
}

bool CyUnit::isHasPromotion(int /*PromotionTypes*/eIndex) const
{
	return m_pUnit->isHasPromotion((PromotionTypes)eIndex);
}

bool CyUnit::isHasUnitCombat(int /*UnitCombatTypes*/eIndex) const
{
	return m_pUnit->isHasUnitCombat((UnitCombatTypes)eIndex);
}

void CyUnit::setHasPromotion(int /*PromotionTypes*/ eIndex, bool bNewValue)
{
	m_pUnit->setHasPromotion((PromotionTypes) eIndex, bNewValue);
}

int CyUnit::captureProbabilityTotal() const
{
	return m_pUnit->captureProbabilityTotal();
}

int CyUnit::captureResistanceTotal() const
{
	return m_pUnit->captureResistanceTotal();
}

int /*UnitAITypes*/ CyUnit::getUnitAIType() const
{
	return m_pUnit->AI_getUnitAIType();
}

void CyUnit::setUnitAIType(int /*UnitAITypes*/ iNewValue)
{
	m_pUnit->AI_setUnitAIType((UnitAITypes)iNewValue);
}

bool CyUnit::IsSelected() const
{
	return m_pUnit->IsSelected();
}

std::string CyUnit::getButton() const
{
	return m_pUnit->getButton();
}

void CyUnit::setCommander(bool bNewValue)
{
	m_pUnit->setCommander(bNewValue);
}

bool CyUnit::isCommander() const
{
	return m_pUnit->isCommander();
}

int CyUnit::controlPointsLeft() const
{
	return m_pUnit->controlPointsLeft();
}

int CyUnit::controlPoints() const
{
	return m_pUnit->controlPoints();
}

float CyUnit::getRealExperience() const
{
	return (float)m_pUnit->getExperience100() / 100;
}

bool CyUnit::isHiddenNationality() const
{
	return m_pUnit->isHiddenNationality();
}

void CyUnit::doHNCapture()
{
	m_pUnit->doHNCapture();
}
