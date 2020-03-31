//
// Python wrapper class for CvUnit
//
//
#include "CvGameCoreDLL.h"

CyUnit::CyUnit() : m_pUnit(NULL) {}

CyUnit::CyUnit(CvUnit* pUnit) : m_pUnit(pUnit) {}

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
	return m_pUnit ? m_pUnit->isActionRecommended(i) : false;
}

void CyUnit::doCommand(CommandTypes eCommand, int iData1, int iData2)
{
	if (m_pUnit)
		m_pUnit->doCommand(eCommand, iData1, iData2);
}

bool CyUnit::canMoveInto(CyPlot* pPlot, bool bAttack, bool bDeclareWar, bool bIgnoreLoad)
{
	return m_pUnit ? m_pUnit->canMoveInto(pPlot->getPlot(), 
		(bAttack ? MoveCheck::Attack : MoveCheck::None) |
		(bDeclareWar ? MoveCheck::DeclareWar : MoveCheck::None) |
		(bIgnoreLoad ? MoveCheck::IgnoreLoad : MoveCheck::None)
	) : false;
}

bool CyUnit::canHeal(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->canHeal(pPlot->getPlot()) : false;
}

bool CyUnit::canFound(CyPlot* pPlot, bool bTestVisible)
{
	return m_pUnit ? m_pUnit->canFound(pPlot->getPlot(), bTestVisible) : false;
}

bool CyUnit::canConstruct(CyPlot* pPlot, int /*BuildingTypes*/ eBuilding)
{
	return m_pUnit ? m_pUnit->canConstruct(pPlot->getPlot(), (BuildingTypes) eBuilding) : false;
}

bool CyUnit::canHurry(CyPlot* pPlot, bool bTestVisible)
{
	return m_pUnit ? m_pUnit->canHurry(pPlot->getPlot(), bTestVisible) : false;
}

bool CyUnit::canTrade(CyPlot* pPlot, bool bTestVisible)
{
	return m_pUnit ? m_pUnit->canTrade(pPlot->getPlot(), bTestVisible) : false;
}

int CyUnit::getEspionagePoints(CyPlot* pPlot)
{
	return m_pUnit ? m_pUnit->getEspionagePoints(pPlot->getPlot()) : -1;
}

bool CyUnit::canBuild(CyPlot* pPlot, int /*BuildTypes*/ eBuild, bool bTestVisible)
{
	return m_pUnit ? m_pUnit->canBuild(pPlot->getPlot(), (BuildTypes) eBuild, bTestVisible) : false;
}

bool CyUnit::canUpgrade(int /*UnitTypes*/ eUnit, bool bTestVisible)			
{
	return m_pUnit ? m_pUnit->canUpgrade((UnitTypes)eUnit, bTestVisible) : false;
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

int /*UnitTypes*/ CyUnit::getCaptureUnitType()
{
	return m_pUnit ? m_pUnit->getCaptureUnitType() : -1;
}

int /*UnitCombatTypes*/ CyUnit::getUnitCombatType()
{
	return m_pUnit ? (int) m_pUnit->getUnitCombatType() : (int) NO_UNITCOMBAT;
}

int /*DomainTypes*/ CyUnit::getDomainType()
{
	return m_pUnit ? (int) m_pUnit->getDomainType() : (int) NO_DOMAIN;
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

int CyUnit::nukeRange()
{
	return m_pUnit ? m_pUnit->nukeRange() : -1;
}

bool CyUnit::isAnimal()
{
	return m_pUnit ? m_pUnit->isAnimal() : false;
}

bool CyUnit::isOnlyDefensive()
{
	return m_pUnit ? m_pUnit->isOnlyDefensive() : false;
}

bool CyUnit::isFound()
{
	return m_pUnit ? m_pUnit->isFound() : false;
}

bool CyUnit::isGoldenAge()
{
	return m_pUnit ? m_pUnit->isGoldenAge() : false;
}

bool CyUnit::isFighting()
{
	return m_pUnit ? m_pUnit->isFighting() : false;
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
		m_pUnit->setBaseCombatStr(iCombat);
}

int CyUnit::baseCombatStr()
{
	return m_pUnit ? m_pUnit->baseCombatStr() : -1;
}

bool CyUnit::canFight()
{
	return m_pUnit ? m_pUnit->canFight() : false;
}

int CyUnit::airBaseCombatStr()
{
	return m_pUnit ? m_pUnit->airBaseCombatStr() : -1;
}

bool CyUnit::canAirAttack()
{
	return m_pUnit ? m_pUnit->canAirAttack() : false;
}

bool CyUnit::isAutomated()
{
	return m_pUnit ? m_pUnit->isAutomated() : false;
}

bool CyUnit::isAutoPromoting()
{
	return m_pUnit ? m_pUnit->isAutoPromoting() : false;
}

bool CyUnit::isAutoUpgrading()
{
	return m_pUnit ? m_pUnit->isAutoUpgrading() : false;
}

bool CyUnit::isWaiting()
{
	return m_pUnit ? m_pUnit->isWaiting() : false;
}

bool CyUnit::isFortifyable()
{
	return m_pUnit ? m_pUnit->isFortifyable() : false;
}

int CyUnit::experienceNeeded()
{
	return m_pUnit ? m_pUnit->experienceNeeded() : -1;
}

bool CyUnit::isInvisible(int /*TeamTypes*/ eTeam, bool bDebug)
{
	return m_pUnit ? m_pUnit->isInvisible((TeamTypes) eTeam, bDebug) : false;
}

bool CyUnit::isNukeImmune()
{
	return m_pUnit ? m_pUnit->isNukeImmune() : false;
}

int CyUnit::bombardRate()
{
	return m_pUnit ? m_pUnit->getBombardRate() : -1;
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

int CyUnit::getID()
{
	return m_pUnit ? m_pUnit->getID() : -1;
}

CySelectionGroup* CyUnit::getGroup()
{
	return m_pUnit ? new CySelectionGroup(m_pUnit->getGroup()) : NULL;
}

int CyUnit::getHotKeyNumber()
{
	return m_pUnit ? m_pUnit->getHotKeyNumber() : -1;
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
		m_pUnit->setXY(iX, iY, bGroup, bUpdate, bShow);
}

CyPlot* CyUnit::plot()
{
	return m_pUnit ? new CyPlot(m_pUnit->plot()) : NULL;
}

CyArea* CyUnit::area()
{
	return m_pUnit ? new CyArea(m_pUnit->area()) : NULL;
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

int CyUnit::getFacingDirection()
{
	return m_pUnit ? m_pUnit->getFacingDirection(false) : NO_DIRECTION;
}

void CyUnit::rotateFacingDirectionClockwise()
{
	if (m_pUnit)
		m_pUnit->rotateFacingDirectionClockwise();
}

void CyUnit::rotateFacingDirectionCounterClockwise()
{
	if (m_pUnit)
		m_pUnit->rotateFacingDirectionCounterClockwise();
}

int CyUnit::getFortifyTurns()
{
	return m_pUnit ? m_pUnit->getFortifyTurns() : -1;
}

void CyUnit::setFortifyTurns(int iNewValue)
{
	if (m_pUnit)
		m_pUnit->setFortifyTurns(iNewValue);
}

bool CyUnit::isRiver()	 
{
	return m_pUnit ? m_pUnit->isRiver() : false;
}

int CyUnit::getExtraMoves()
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
	if (m_pUnit)
		m_pUnit->setImmobileTimer(iNewValue);
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

int CyUnit::getTeam()
{
	return m_pUnit ? m_pUnit->getTeam() : -1;
}

int /*UnitTypes*/ CyUnit::getUnitType()
{
	return m_pUnit ? (int)m_pUnit->getUnitType() : -1;
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

bool CyUnit::canAcquirePromotion(int /*PromotionTypes*/ ePromotion)
{
	return m_pUnit ? m_pUnit->canAcquirePromotion((PromotionTypes) ePromotion) : false;
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
		m_pUnit->setHasUnitCombat((UnitCombatTypes) eIndex, bNewValue);
}

void CyUnit::setHasPromotion(int /*PromotionTypes*/ eIndex, bool bNewValue)
{
	if (m_pUnit)
		m_pUnit->setHasPromotion((PromotionTypes) eIndex, bNewValue);
}

int CyUnit::captureProbabilityTotal()
{
	return m_pUnit ? m_pUnit->captureProbabilityTotal() : -1;
}

int CyUnit::captureResistanceTotal()
{
	return m_pUnit ? m_pUnit->captureResistanceTotal() : -1;
}

int /*UnitAITypes*/ CyUnit::getUnitAIType()
{
	return m_pUnit ? (int)m_pUnit->AI_getUnitAIType() : -1;
}

void CyUnit::setUnitAIType(int /*UnitAITypes*/ iNewValue)
{
	if (m_pUnit)
		m_pUnit->AI_setUnitAIType((UnitAITypes)iNewValue);
}

bool CyUnit::IsSelected()
{
	return m_pUnit ? m_pUnit->IsSelected() : false;
}

std::string CyUnit::getButton() const
{
	return m_pUnit ? m_pUnit->getButton() : "";
}

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

float CyUnit::getRealExperience()
{
	return m_pUnit ? (float)m_pUnit->getExperience100() / 100 : -1;
}

CyPlot* CyUnit::getMADTargetPlot()
{
	return m_pUnit ? new CyPlot(m_pUnit->getMADTargetPlot()) : NULL;
}

bool CyUnit::isHiddenNationality()
{
	return m_pUnit ? m_pUnit->isHiddenNationality() : false;
}

void CyUnit::doHNCapture()
{
	if (m_pUnit)
		m_pUnit->doHNCapture();
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