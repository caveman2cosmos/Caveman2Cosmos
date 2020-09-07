#include "CvGameCoreDLL.h"
#include "CyArea.h"
#include "CyPlot.h"
#include "CySelectionGroup.h"
#include "CyUnit.h"

//
// Python wrapper class for CvUnit
//

CyUnit::CyUnit() : m_pUnit(NULL) {}

CyUnit::CyUnit(CvUnit* pUnit) : m_pUnit(pUnit) {}


void CyUnit::convert(const CyUnit& kUnit, bool bKillOriginal)
{
	if (m_pUnit) m_pUnit->convert(kUnit.getUnit(), bKillOriginal);
}

void CyUnit::kill(bool bDelay, int /*PlayerTypes*/ ePlayer)
{
	if (m_pUnit) m_pUnit->kill(bDelay, (PlayerTypes)ePlayer);
}

void CyUnit::NotifyEntity(int /*MissionTypes*/ eEvent)
{
	if (m_pUnit) m_pUnit->NotifyEntity((MissionTypes)eEvent);
}

bool CyUnit::isActionRecommended(int i) const
{
	return m_pUnit ? m_pUnit->isActionRecommended(i) : false;
}

void CyUnit::doCommand(CommandTypes eCommand, int iData1, int iData2)
{
	if (m_pUnit) m_pUnit->doCommand(eCommand, iData1, iData2);
}

bool CyUnit::canMoveInto(const CyPlot& kPlot, bool bAttack, bool bDeclareWar, bool bIgnoreLoad) const
{
	return m_pUnit ? m_pUnit->canMoveInto(kPlot.getPlot(), 
		(bAttack ? MoveCheck::Attack : MoveCheck::None) |
		(bDeclareWar ? MoveCheck::DeclareWar : MoveCheck::None) |
		(bIgnoreLoad ? MoveCheck::IgnoreLoad : MoveCheck::None)
	) : false;
}

bool CyUnit::canHeal(const CyPlot& kPlot) const
{
	return m_pUnit ? m_pUnit->canHeal(kPlot.getPlot()) : false;
}

bool CyUnit::canFound(const CyPlot& kPlot, bool bTestVisible) const
{
	return m_pUnit ? m_pUnit->canFound(kPlot.getPlot(), bTestVisible) : false;
}

bool CyUnit::canConstruct(const CyPlot& kPlot, int /*BuildingTypes*/ eBuilding) const
{
	return m_pUnit ? m_pUnit->canConstruct(kPlot.getPlot(), (BuildingTypes)eBuilding) : false;
}

int /*TechTypes*/ CyUnit::getDiscoveryTech() const
{
	return m_pUnit ? m_pUnit->getDiscoveryTech() : -1;
}

int CyUnit::getDiscoverResearch(int /*TechTypes*/ eTech) const
{
	return m_pUnit ? m_pUnit->getDiscoverResearch((TechTypes) eTech) : -1;
}

int CyUnit::getHurryProduction(const CyPlot& kPlot) const
{
	return m_pUnit ? m_pUnit->getHurryProduction(kPlot.getPlot()) : -1;
}

bool CyUnit::canHurry(const CyPlot& kPlot, bool bTestVisible) const
{
	return m_pUnit ? m_pUnit->canHurry(kPlot.getPlot(), bTestVisible) : false;
}

bool CyUnit::canTrade(const CyPlot& kPlot, bool bTestVisible) const
{
	return m_pUnit ? m_pUnit->canTrade(kPlot.getPlot(), bTestVisible) : false;
}

int CyUnit::getGreatWorkCulture(const CyPlot& kPlot) const
{
	return m_pUnit ? m_pUnit->getGreatWorkCulture(kPlot.getPlot()) : -1;
}

int CyUnit::getEspionagePoints(const CyPlot& kPlot) const
{
	return m_pUnit ? m_pUnit->getEspionagePoints(kPlot.getPlot()) : -1;
}

bool CyUnit::canBuild(const CyPlot& kPlot, int /*BuildTypes*/ eBuild, bool bTestVisible) const
{
	return m_pUnit ? m_pUnit->canBuild(kPlot.getPlot(), (BuildTypes) eBuild, bTestVisible) : false;
}

bool CyUnit::canUpgrade(int /*UnitTypes*/ eUnit, bool bTestVisible) const
{
	return m_pUnit ? m_pUnit->canUpgrade((UnitTypes)eUnit, bTestVisible) : false;
}

int /*HandicapTypes*/ CyUnit::getHandicapType() const
{
	return m_pUnit ? m_pUnit->getHandicapType() : NO_HANDICAP;
}

int /*CivilizationTypes*/ CyUnit::getCivilizationType() const
{
	return m_pUnit ? m_pUnit->getCivilizationType() : NO_CIVILIZATION;
}

int /*SpecialUnitTypes*/ CyUnit::getSpecialUnitType() const
{
	return m_pUnit ? m_pUnit->getSpecialUnitType() : NO_SPECIALUNIT;
}

int /*UnitTypes*/ CyUnit::getCaptureUnitType() const
{
	return m_pUnit ? m_pUnit->getCaptureUnitType() : -1;
}

int /*UnitCombatTypes*/ CyUnit::getUnitCombatType() const
{
	return m_pUnit ? m_pUnit->getUnitCombatType() : NO_UNITCOMBAT;
}

int /*DomainTypes*/ CyUnit::getDomainType() const
{
	return m_pUnit ? m_pUnit->getDomainType() : NO_DOMAIN;
}

bool CyUnit::isBarbarian() const
{
	return m_pUnit ? m_pUnit->isBarbarian() : false;
}

bool CyUnit::isNPC() const
{
	return m_pUnit ? m_pUnit->isNPC() : false;
}

bool CyUnit::isHominid() const
{
	return m_pUnit ? m_pUnit->isHominid() : false;
}

bool CyUnit::isHuman() const
{
	return m_pUnit ? m_pUnit->isHuman() : false;
}

int CyUnit::baseMoves() const
{
	return m_pUnit ? m_pUnit->baseMoves() : -1;
}

int CyUnit::maxMoves() const
{
	return m_pUnit ? m_pUnit->maxMoves() : -1;
}

int CyUnit::movesLeft() const
{
	return m_pUnit ? m_pUnit->movesLeft() : -1;
}

bool CyUnit::canMove() const
{
	return m_pUnit ? m_pUnit->canMove() : false;
}

bool CyUnit::hasMoved() const
{
	return m_pUnit ? m_pUnit->hasMoved() : false;
}

int CyUnit::nukeRange() const
{
	return m_pUnit ? m_pUnit->nukeRange() : -1;
}

bool CyUnit::isAnimal() const
{
	return m_pUnit ? m_pUnit->isAnimal() : false;
}

bool CyUnit::isOnlyDefensive() const
{
	return m_pUnit ? m_pUnit->isOnlyDefensive() : false;
}

bool CyUnit::isFound() const
{
	return m_pUnit ? m_pUnit->isFound() : false;
}

bool CyUnit::isGoldenAge() const
{
	return m_pUnit ? m_pUnit->isGoldenAge() : false;
}

bool CyUnit::isFighting() const
{
	return m_pUnit ? m_pUnit->isFighting() : false;
}

int CyUnit::maxHitPoints() const
{
	return m_pUnit ? m_pUnit->maxHitPoints() : -1;
}

int CyUnit::currHitPoints() const
{
	return m_pUnit ? m_pUnit->currHitPoints() : -1;
}

bool CyUnit::isHurt() const
{
	return m_pUnit ? m_pUnit->isHurt() : false;
}

bool CyUnit::isDead() const
{
	return m_pUnit ? m_pUnit->isDead() : false;
}

void CyUnit::setBaseCombatStr(int iCombat)
{
	if (m_pUnit) m_pUnit->setBaseCombatStr(iCombat);
}

int CyUnit::baseCombatStr() const
{
	return m_pUnit ? m_pUnit->baseCombatStr() : -1;
}

bool CyUnit::canFight() const
{
	return m_pUnit ? m_pUnit->canFight() : false;
}

int CyUnit::airBaseCombatStr() const
{
	return m_pUnit ? m_pUnit->airBaseCombatStr() : -1;
}

bool CyUnit::canAirAttack() const
{
	return m_pUnit ? m_pUnit->canAirAttack() : false;
}

bool CyUnit::isAutomated() const
{
	return m_pUnit ? m_pUnit->isAutomated() : false;
}

bool CyUnit::isAutoPromoting() const
{
	return m_pUnit ? m_pUnit->isAutoPromoting() : false;
}

bool CyUnit::isAutoUpgrading() const
{
	return m_pUnit ? m_pUnit->isAutoUpgrading() : false;
}

bool CyUnit::isWaiting() const
{
	return m_pUnit ? m_pUnit->isWaiting() : false;
}

bool CyUnit::isFortifyable() const
{
	return m_pUnit ? m_pUnit->isFortifyable() : false;
}

int CyUnit::experienceNeeded() const
{
	return m_pUnit ? m_pUnit->experienceNeeded() : -1;
}

bool CyUnit::isInvisible(int /*TeamTypes*/ eTeam, bool bDebug) const
{
	return m_pUnit ? m_pUnit->isInvisible((TeamTypes) eTeam, bDebug) : false;
}

bool CyUnit::isNukeImmune() const
{
	return m_pUnit ? m_pUnit->isNukeImmune() : false;
}

int CyUnit::bombardRate() const
{
	return m_pUnit ? m_pUnit->getBombardRate() : -1;
}

int /*SpecialUnitTypes*/ CyUnit::specialCargo() const
{
	return m_pUnit ? m_pUnit->specialCargo() : NO_SPECIALUNIT;
}

int /*DomainTypes*/ CyUnit::domainCargo() const
{
	return m_pUnit ? m_pUnit->domainCargo() : NO_DOMAIN;
}

int CyUnit::cargoSpace() const
{
	return m_pUnit ? m_pUnit->cargoSpace() : -1;
}

void CyUnit::changeCargoSpace(int iChange)
{
	if (m_pUnit) m_pUnit->changeCargoSpace(iChange);
}

bool CyUnit::isFull() const
{
	return m_pUnit ? m_pUnit->isFull() : false;
}

int CyUnit::getID() const
{
	return m_pUnit ? m_pUnit->getID() : -1;
}

int CyUnit::getGroupID() const
{
	return m_pUnit ? m_pUnit->getGroupID() : -1;
}

bool CyUnit::isInGroup() const
{
	return m_pUnit ? m_pUnit->isInGroup() : false;
}

bool CyUnit::isGroupHead() const
{
	return m_pUnit ? m_pUnit->isGroupHead() : false;
}

CySelectionGroup* CyUnit::getGroup() const
{
	return m_pUnit ? new CySelectionGroup(m_pUnit->getGroup()) : NULL;
}

int CyUnit::getHotKeyNumber() const
{
	return m_pUnit ? m_pUnit->getHotKeyNumber() : -1;
}

int CyUnit::getX() const
{
	return m_pUnit ? m_pUnit->getX() : -1;
}

int CyUnit::getY() const
{
	return m_pUnit ? m_pUnit->getY() : -1;
}

void CyUnit::setXY(int iX, int iY, bool bGroup, bool bUpdate, bool bShow)
{
	if (m_pUnit) m_pUnit->setXY(iX, iY, bGroup, bUpdate, bShow);
}

CyPlot* CyUnit::plot() const
{
	return m_pUnit ? new CyPlot(m_pUnit->plot()) : NULL;
}

CyArea* CyUnit::area() const
{
	return m_pUnit ? new CyArea(m_pUnit->area()) : NULL;
}

int CyUnit::getDamage() const
{
	return m_pUnit ? m_pUnit->getDamage() : -1;
}

void CyUnit::setDamage(int iNewValue, int /*PlayerTypes*/ ePlayer)
{
	if (m_pUnit) m_pUnit->setDamage(iNewValue, (PlayerTypes)ePlayer);
}

void CyUnit::changeDamage(int iChange, int /*PlayerTypes*/ ePlayer)
{
	if (m_pUnit) m_pUnit->changeDamage(iChange, (PlayerTypes)ePlayer);
}

int CyUnit::getMoves() const
{
	return m_pUnit ? m_pUnit->getMoves() : -1;
}

void CyUnit::setMoves(int iNewValue)
{
	if (m_pUnit) m_pUnit->setMoves(iNewValue);
}

void CyUnit::changeMoves(int iChange)
{
	if (m_pUnit) m_pUnit->changeMoves(iChange);
}

void CyUnit::finishMoves()
{
	if (m_pUnit) m_pUnit->finishMoves();
}

int CyUnit::getExperience() const
{
	return m_pUnit ? m_pUnit->getExperience() : -1;
}

void CyUnit::setExperience(int iNewValue, int iMax)
{
	if (m_pUnit) m_pUnit->setExperience(iNewValue, iMax);
}

void CyUnit::changeExperience(int iChange, int iMax, bool bFromCombat, bool bInBorders, bool bUpdateGlobal)
{
	if (m_pUnit) m_pUnit->changeExperience(iChange, iMax, bFromCombat, bInBorders, bUpdateGlobal);
}

int CyUnit::getLevel() const
{
	return m_pUnit ? m_pUnit->getLevel() : -1;
}

void CyUnit::setLevel(int iNewLevel)
{
	if (m_pUnit) m_pUnit->setLevel(iNewLevel);
}

int CyUnit::getFacingDirection() const
{
	return m_pUnit ? m_pUnit->getFacingDirection(false) : NO_DIRECTION;
}

void CyUnit::rotateFacingDirectionClockwise()
{
	if (m_pUnit) m_pUnit->rotateFacingDirectionClockwise();
}

void CyUnit::rotateFacingDirectionCounterClockwise()
{
	if (m_pUnit) m_pUnit->rotateFacingDirectionCounterClockwise();
}

int CyUnit::getCargo() const
{
	return m_pUnit ? m_pUnit->getCargo() : -1;
}

int CyUnit::getFortifyTurns() const
{
	return m_pUnit ? m_pUnit->getFortifyTurns() : -1;
}

void CyUnit::setFortifyTurns(int iNewValue)
{
	if (m_pUnit) m_pUnit->setFortifyTurns(iNewValue);
}

bool CyUnit::isRiver() const
{
	return m_pUnit ? m_pUnit->isRiver() : false;
}

int CyUnit::getExtraMoves() const
{
	return m_pUnit ? m_pUnit->getExtraMoves() : -1;
}

int CyUnit::getRevoltProtection() const
{
	return m_pUnit ? m_pUnit->revoltProtectionTotal() : -1;
}

int CyUnit::getPillageChange() const
{
	return m_pUnit ? m_pUnit->getPillageChange() : -1;
}

int CyUnit::getUpgradeDiscount() const
{
	return m_pUnit ? m_pUnit->getUpgradeDiscount() : -1;
}

int CyUnit::getImmobileTimer() const
{
	return m_pUnit ? m_pUnit->getImmobileTimer() : -1;
}

void CyUnit::setImmobileTimer(int iNewValue)
{
	if (m_pUnit) m_pUnit->setImmobileTimer(iNewValue);
}

bool CyUnit::isMadeAttack() const
{
	return m_pUnit ? m_pUnit->isMadeAttack() : false;
}

void CyUnit::setMadeAttack(bool bNewValue)
{
	if (m_pUnit) m_pUnit->setMadeAttack(bNewValue);
}

bool CyUnit::isMadeInterception() const
{
	return m_pUnit ? m_pUnit->isMadeInterception() : false;
}

void CyUnit::setMadeInterception(bool bNewValue)
{
	if (m_pUnit) m_pUnit->setMadeInterception(bNewValue);
}

bool CyUnit::isPromotionReady() const
{
	return m_pUnit ? m_pUnit->isPromotionReady() : false;
}

void CyUnit::setPromotionReady(bool bNewValue)
{
	if (m_pUnit) m_pUnit->setPromotionReady(bNewValue);
}

int CyUnit::getOwner() const
{
	return m_pUnit ? m_pUnit->getOwner() : -1;
}

int CyUnit::getVisualOwner() const
{
	return m_pUnit ? m_pUnit->getVisualOwner() : -1;
}

int CyUnit::getTeam() const
{
	return m_pUnit ? m_pUnit->getTeam() : -1;
}

int /*UnitTypes*/ CyUnit::getUnitType() const
{
	return m_pUnit ? m_pUnit->getUnitType() : -1;
}

int /*UnitTypes*/ CyUnit::getLeaderUnitType() const
{
	return m_pUnit ? m_pUnit->getLeaderUnitType() : -1;
}

void CyUnit::setLeaderUnitType(int leaderUnitType)
{
	if (m_pUnit) m_pUnit->setLeaderUnitType((UnitTypes) leaderUnitType);
}

CyUnit* CyUnit::getTransportUnit() const
{
	return m_pUnit ? new CyUnit(m_pUnit->getTransportUnit()) : NULL;
}

bool CyUnit::isCargo() const
{
	return m_pUnit ? m_pUnit->isCargo() : false;
}

void CyUnit::setTransportUnit(const CyUnit& kTransportUnit)
{
	if (m_pUnit) m_pUnit->setTransportUnit(kTransportUnit.getUnit());
}

std::wstring CyUnit::getName() const
{
	return m_pUnit ? m_pUnit->getName() : L"";
}

std::wstring CyUnit::getNameForm(int iForm) const
{
	return m_pUnit ? m_pUnit->getName((uint)iForm) : L"";
}

std::wstring CyUnit::getNameKey() const
{
	return m_pUnit ? m_pUnit->getNameKey() : L"";
}

std::wstring CyUnit::getNameNoDesc() const
{
	return m_pUnit ? m_pUnit->getNameNoDesc() : L"";
}

void CyUnit::setName(std::wstring szNewValue)
{
	if (m_pUnit) m_pUnit->setName(szNewValue);
}

std::string CyUnit::getScriptData() const
{
	return m_pUnit ? m_pUnit->getScriptData() : "";
}

void CyUnit::setScriptData(std::string szNewValue)
{
	if (m_pUnit) m_pUnit->setScriptData(szNewValue.c_str());
}

bool CyUnit::canAcquirePromotion(int /*PromotionTypes*/ ePromotion) const
{
	return m_pUnit ? m_pUnit->canAcquirePromotion((PromotionTypes) ePromotion) : false;
}

bool CyUnit::canAcquirePromotionAny() const
{
	return m_pUnit ? m_pUnit->canAcquirePromotionAny() : false;
}

bool CyUnit::isPromotionValid(int /*PromotionTypes*/ ePromotion) const
{
	return m_pUnit ? m_pUnit->isPromotionValid((PromotionTypes) ePromotion) : false;
}

bool CyUnit::isPromotionOverriden(int /*PromotionTypes*/eIndex) const
{
	return m_pUnit ? m_pUnit->isPromotionOverriden((PromotionTypes)eIndex) : false;
}

bool CyUnit::isHasPromotion(int /*PromotionTypes*/eIndex) const
{
	return m_pUnit ? m_pUnit->isHasPromotion((PromotionTypes)eIndex) : false;
}

bool CyUnit::isHasUnitCombat(int /*UnitCombatTypes*/eIndex) const
{
	return m_pUnit ? m_pUnit->isHasUnitCombat((UnitCombatTypes)eIndex) : false;
}

void CyUnit::setHasPromotion(int /*PromotionTypes*/ eIndex, bool bNewValue)
{
	if (m_pUnit) m_pUnit->setHasPromotion((PromotionTypes) eIndex, bNewValue);
}

int CyUnit::captureProbabilityTotal() const
{
	return m_pUnit ? m_pUnit->captureProbabilityTotal() : -1;
}

int CyUnit::captureResistanceTotal() const
{
	return m_pUnit ? m_pUnit->captureResistanceTotal() : -1;
}

int /*UnitAITypes*/ CyUnit::getUnitAIType() const
{
	return m_pUnit ? m_pUnit->AI_getUnitAIType() : -1;
}

void CyUnit::setUnitAIType(int /*UnitAITypes*/ iNewValue)
{
	if (m_pUnit) m_pUnit->AI_setUnitAIType((UnitAITypes)iNewValue);
}

bool CyUnit::IsSelected() const
{
	return m_pUnit ? m_pUnit->IsSelected() : false;
}

std::string CyUnit::getButton() const
{
	return m_pUnit ? m_pUnit->getButton() : "";
}

void CyUnit::setCommander(bool bNewValue)
{
	if(m_pUnit != NULL)
	{
		m_pUnit->setCommander(bNewValue);
	}
}

bool CyUnit::isCommander() const
{
	return m_pUnit ? m_pUnit->isCommander() : false;
}

int CyUnit::controlPointsLeft() const
{
	return m_pUnit ? m_pUnit->controlPointsLeft() : -1;
}

int CyUnit::controlPoints() const
{
	return m_pUnit ? m_pUnit->controlPoints() : -1;
}


float CyUnit::getRealExperience() const
{
	return m_pUnit ? (float)m_pUnit->getExperience100() / 100 : -1;
}

CyPlot* CyUnit::getMADTargetPlot() const
{
	return m_pUnit ? new CyPlot(m_pUnit->getMADTargetPlot()) : false;
}

bool CyUnit::isHiddenNationality() const
{
	return m_pUnit ? m_pUnit->isHiddenNationality() : false;
}

void CyUnit::doHNCapture()
{
	if (m_pUnit) m_pUnit->doHNCapture();
}
