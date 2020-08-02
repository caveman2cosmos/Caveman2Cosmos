#include "CvGameCoreDLL.h"
#include "CyArea.h"
#include "CyCity.h"
#include "CyPlot.h"
#include "CyUnit.h"

//
// Python wrapper class for CvCity
//

CyCity::CyCity() : m_pCity(NULL) {}

CyCity::CyCity(CvCity* pCity) : m_pCity(pCity) {}


void CyCity::kill()
{
	if (m_pCity) m_pCity->kill(true);
}

int CyCity::getRevolutionIndex() const
{
	return m_pCity ? m_pCity->getRevolutionIndex() : 0;
}

void CyCity::setRevolutionIndex(int iNewValue)
{
	if (m_pCity) m_pCity->setRevolutionIndex(iNewValue);
}

void CyCity::changeRevolutionIndex(int iChange)
{
	if (m_pCity) m_pCity->changeRevolutionIndex(iChange);
}

int CyCity::getLocalRevIndex() const
{
	return m_pCity ? m_pCity->getLocalRevIndex() : 0;
}

void CyCity::setLocalRevIndex(int iNewValue)
{
	if (m_pCity) m_pCity->setLocalRevIndex(iNewValue);
}

int CyCity::getRevIndexAverage() const
{
	return m_pCity ? m_pCity->getRevIndexAverage() : 0;
}

void CyCity::setRevIndexAverage(int iNewValue)
{
	if (m_pCity) m_pCity->setRevIndexAverage(iNewValue);
}

void CyCity::updateRevIndexAverage()
{
	if (m_pCity) m_pCity->updateRevIndexAverage();
}

int CyCity::getReinforcementCounter() const
{
	return m_pCity ? m_pCity->getReinforcementCounter() : 0;
}

void CyCity::setReinforcementCounter(int iNewValue)
{
	if (m_pCity) m_pCity->setReinforcementCounter(iNewValue);
}

void CyCity::changeReinforcementCounter(int iChange)
{
	if (m_pCity) m_pCity->changeReinforcementCounter(iChange);
}

int CyCity::getRevolutionCounter() const
{
	return m_pCity ? m_pCity->getRevolutionCounter() : 0;
}

void CyCity::setRevolutionCounter(int iNewValue)
{
	if (m_pCity) m_pCity->setRevolutionCounter(iNewValue);
}

void CyCity::changeRevolutionCounter(int iChange)
{
	if (m_pCity) m_pCity->changeRevolutionCounter(iChange);
}

CyPlot* CyCity::getCityIndexPlot(int iIndex) const
{
	return m_pCity ? new CyPlot(m_pCity->getCityIndexPlot(iIndex)) : NULL;
}

bool CyCity::canWork(const CyPlot* pPlot) const
{
	return m_pCity ? m_pCity->canWork(pPlot ? pPlot->getPlot() : NULL) : false;
}

int CyCity::countNumImprovedPlots() const
{
	return m_pCity ? m_pCity->countNumImprovedPlots() : -1;
}

int CyCity::countNumWaterPlots() const
{
	return m_pCity ? m_pCity->countNumWaterPlots() : -1;
}

int CyCity::findBaseYieldRateRank(int /*YieldTypes*/ eYield) const
{
	return m_pCity ? m_pCity->findBaseYieldRateRank((YieldTypes) eYield) : -1;
}

int CyCity::findYieldRateRank(int /*YieldTypes*/ eYield) const
{
	return m_pCity ? m_pCity->findYieldRateRank((YieldTypes) eYield) : -1;
}

int CyCity::findCommerceRateRank(int /*CommerceTypes*/ eCommerce) const
{
	return m_pCity ? m_pCity->findCommerceRateRank((CommerceTypes) eCommerce) : -1;
}

bool CyCity::canTrain(int /*UnitTypes*/ eUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreUpgrades) const
{
	return m_pCity ? m_pCity->canTrain((UnitTypes)eUnit, bContinue, bTestVisible, bIgnoreCost, bIgnoreUpgrades) : false;
}

bool CyCity::canConstruct(int /*BuildingTypes*/ eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost) const
{
	return m_pCity ? m_pCity->canConstruct((BuildingTypes)eBuilding, bContinue, bTestVisible, bIgnoreCost) : false;
}

bool CyCity::canCreate(int /*ProjectTypes*/ eProject, bool bContinue, bool bTestVisible) const
{
	return m_pCity ? m_pCity->canCreate((ProjectTypes)eProject, bContinue, bTestVisible) : false;
}

bool CyCity::canMaintain(int /*ProcessTypes*/ eProcess, bool bContinue) const
{
	return m_pCity ? m_pCity->canMaintain((ProcessTypes)eProcess, bContinue) : false;
}

int CyCity::getFoodTurnsLeft() const
{
	return m_pCity ? m_pCity->getFoodTurnsLeft() : 0;
}

bool CyCity::isProduction() const
{
	return m_pCity ? m_pCity->isProduction() : false;
}

bool CyCity::isProductionUnit() const
{
	return m_pCity ? m_pCity->isProductionUnit() : false;
}

bool CyCity::isProductionBuilding() const
{
	return m_pCity ? m_pCity->isProductionBuilding() : false;
}

bool CyCity::isProductionProject() const
{
	return m_pCity ? m_pCity->isProductionProject() : false;
}

bool CyCity::isProductionProcess() const
{
	return m_pCity ? m_pCity->isProductionProcess() : false;
}

int CyCity::getProductionExperience(int /*UnitTypes*/ eUnit) const
{
	return m_pCity ? m_pCity->getProductionExperience((UnitTypes) eUnit) : -1;
}

void CyCity::addProductionExperience(const CyUnit& kUnit, bool bConscript)
{
	if (m_pCity) m_pCity->addProductionExperience(kUnit.getUnit(), bConscript);
}

int /*UnitTypes*/ CyCity::getProductionUnit() const
{
	return m_pCity ? m_pCity->getProductionUnit() : NO_UNIT;
}

int /*BuildingTypes*/ CyCity::getProductionBuilding() const
{
	return m_pCity ? m_pCity->getProductionBuilding() : NO_BUILDING;
}

int /*ProjectTypes*/ CyCity::getProductionProject() const
{
	return m_pCity ? m_pCity->getProductionProject() : -1;
}

int /*ProcessTypes*/ CyCity::getProductionProcess() const
{
	return m_pCity ? m_pCity->getProductionProcess() : -1;
}

std::wstring CyCity::getProductionName() const
{
	return m_pCity ? m_pCity->getProductionName() : L"";
}

int CyCity::getGeneralProductionTurnsLeft() const
{
	return m_pCity ? m_pCity->getGeneralProductionTurnsLeft() : -1;
}

std::wstring CyCity::getProductionNameKey() const
{
	return m_pCity ? m_pCity->getProductionNameKey() : L"";
}

bool CyCity::isFoodProduction() const
{
	return m_pCity ? m_pCity->isFoodProduction() : false;
}

int CyCity::getFirstUnitOrder(int /*UnitTypes*/ eUnit) const
{
	return m_pCity ? m_pCity->getFirstUnitOrder((UnitTypes)eUnit) : -1;
}

int CyCity::getFirstBuildingOrder(int /*BuildingTypes*/ eBuilding) const
{
	return m_pCity ? m_pCity->getFirstBuildingOrder((BuildingTypes)eBuilding) : -1;
}

int CyCity::getNumTrainUnitAI(int /*UnitAITypes*/ eUnitAI) const
{
	return m_pCity ? m_pCity->getNumTrainUnitAI((UnitAITypes) eUnitAI) : -1;
}

int CyCity::getProduction() const
{
	return m_pCity ? m_pCity->getProduction() : -1;
}

int CyCity::getProductionNeeded() const
{
	return m_pCity ? m_pCity->getProductionNeeded() : -1;
}

int CyCity::getProductionTurnsLeft() const
{
	return m_pCity ? m_pCity->getProductionTurnsLeft() : -1;
}

int CyCity::getUnitProductionTurnsLeft(int /*UnitTypes*/ iUnit, int iNum) const
{
	return m_pCity ? m_pCity->getProductionTurnsLeft((UnitTypes) iUnit, iNum) : -1;
}

int CyCity::getBuildingProductionTurnsLeft(int /*BuildingTypes*/ iBuilding, int iNum) const
{
	return m_pCity ? m_pCity->getProductionTurnsLeft((BuildingTypes) iBuilding, iNum) : -1;
}

int CyCity::getProjectProductionTurnsLeft(int /*ProjectTypes*/ eProject, int iNum) const
{
	return m_pCity ? m_pCity->getProductionTurnsLeft((ProjectTypes)eProject, iNum) : -1;
}

void CyCity::setProduction(int iNewValue)
{
	if (m_pCity) m_pCity->setProduction(iNewValue);
}

void CyCity::changeProduction(int iChange)
{
	if (m_pCity) m_pCity->changeProduction(iChange);
}

int CyCity::getProductionModifier() const
{
	return m_pCity ? m_pCity->getProductionModifier() : -1;
}

int CyCity::getCurrentProductionDifference(bool bIgnoreFood, bool bOverflow) const
{
	return m_pCity ? m_pCity->getCurrentProductionDifference(
		(bIgnoreFood? ProductionCalc::None : ProductionCalc::FoodProduction) |
		(bOverflow? ProductionCalc::Overflow : ProductionCalc::None)
	) : -1;
}

bool CyCity::canHurry(int /*HurryTypes*/ iHurry, bool bTestVisible) const
{
	return m_pCity ? m_pCity->canHurry((HurryTypes)iHurry, bTestVisible) : false;
}

int /*UnitTypes*/ CyCity::getConscriptUnit() const
{
	return m_pCity ? m_pCity->getConscriptUnit() : -1;
}

int CyCity::flatConscriptAngerLength() const
{
	return m_pCity ? m_pCity->flatConscriptAngerLength() : -1;
}

bool CyCity::canConscript() const
{
	return m_pCity ? m_pCity->canConscript() : false;
}

int CyCity::getBonusHealth(int /*BonusTypes*/ iBonus) const
{
	return m_pCity ? m_pCity->getBonusHealth((BonusTypes) iBonus) : -1;
}

int CyCity::getBonusHappiness(int /*BonusTypes*/ iBonus) const
{
	return m_pCity ? m_pCity->getBonusHappiness((BonusTypes) iBonus) : -1;
}

int CyCity::getBonusPower(int /*BonusTypes*/ eBonus, bool bDirty) const
{
	return m_pCity ? m_pCity->getBonusPower((BonusTypes)eBonus, bDirty) : -1;
}

int CyCity::getBonusYieldRateModifier(int /*YieldTypes*/ eIndex, int /*BonusTypes*/ eBonus) const
{
	return m_pCity ? m_pCity->getBonusYieldRateModifier((YieldTypes)eIndex, (BonusTypes)eBonus) : -1;
}

int /*HandicapTypes*/ CyCity::getHandicapType() const
{
	return m_pCity ? m_pCity->getHandicapType() : NO_HANDICAP;
}

int /*CivilizationTypes*/ CyCity::getCivilizationType() const
{
	return m_pCity ? m_pCity->getCivilizationType() : NO_CIVILIZATION;
}

int /*LeaderHeadTypes*/ CyCity::getPersonalityType() const
{
	return m_pCity ? m_pCity->getPersonalityType() : NO_LEADER;
}

int /*ArtStyleTypes*/ CyCity::getArtStyleType() const
{
	return m_pCity ? m_pCity->getArtStyleType() : -1;
}

bool CyCity::hasTrait(int /*TraitTypes*/ iTrait) const
{
	return m_pCity ? m_pCity->hasTrait((TraitTypes) iTrait) : false;
}

bool CyCity::isBarbarian() const
{
	return m_pCity ? m_pCity->isBarbarian() : false;
}

bool CyCity::isNPC() const
{
	return m_pCity ? m_pCity->isNPC() : false;
}

bool CyCity::isHominid() const
{
	return m_pCity ? m_pCity->isHominid() : false;
}

bool CyCity::isHuman() const
{
	return m_pCity ? m_pCity->isHuman() : false;
}

bool CyCity::isVisible(int /*TeamTypes*/ eTeam, bool bDebug) const
{
	return m_pCity ? m_pCity->isVisible((TeamTypes) eTeam, bDebug) : false;
}

bool CyCity::isCapital() const
{
	return m_pCity ? m_pCity->isCapital() : false;
}

bool CyCity::isCoastal(int iMinWaterSize) const
{
	return m_pCity ? m_pCity->isCoastal(iMinWaterSize) : false;
}

bool CyCity::isDisorder() const
{
	return m_pCity ? m_pCity->isDisorder() : false;
}

bool CyCity::isHolyCityByType(int /*ReligionTypes*/ iIndex) const
{
	return m_pCity ? m_pCity->isHolyCity((ReligionTypes) iIndex) : false;
}

bool CyCity::isHolyCity() const
{
	return m_pCity ? m_pCity->isHolyCity() : false;
}

bool CyCity::isHeadquartersByType(int /*CorporationTypes*/ iIndex) const
{
	return m_pCity ? m_pCity->isHeadquarters((CorporationTypes) iIndex) : false;
}

int CyCity::getNoMilitaryPercentAnger() const
{
	return m_pCity ? m_pCity->getNoMilitaryPercentAnger() : -1;
}

int CyCity::getWarWearinessPercentAnger() const
{
	return m_pCity ? m_pCity->getWarWearinessPercentAnger() : -1;
}

int CyCity::getRevIndexPercentAnger() const
{
	return m_pCity ? m_pCity->getRevIndexPercentAnger() : -1;
}

int CyCity::unhappyLevel(int iExtra) const
{
	return m_pCity ? m_pCity->unhappyLevel(iExtra) : -1;
}

int CyCity::happyLevel() const
{
	return m_pCity ? m_pCity->happyLevel() : -1;
}

int CyCity::angryPopulation(int iExtra) const
{
	return m_pCity ? m_pCity->angryPopulation(iExtra) : -1;
}

int CyCity::totalFreeSpecialists() const
{
	return m_pCity ? m_pCity->totalFreeSpecialists() : -1;
}

int CyCity::goodHealth() const
{
	return m_pCity ? m_pCity->goodHealth() : -1;
}

int CyCity::badHealth(bool bNoAngry) const
{
	return m_pCity ? m_pCity->badHealth(bNoAngry) : -1;
}

int CyCity::healthRate(bool bNoAngry, int iExtra) const
{
	return m_pCity ? m_pCity->healthRate(bNoAngry, iExtra) : -1;
}

int CyCity::foodConsumption(bool bNoAngry, int iExtra) const
{
	return m_pCity ? m_pCity->foodConsumption(bNoAngry, iExtra) : -1;
}

int CyCity::foodDifference(bool bBottom) const
{
	return m_pCity ? m_pCity->foodDifference(bBottom) : -1;
}

int CyCity::growthThreshold() const
{
	return m_pCity ? m_pCity->growthThreshold() : -1;
}

int CyCity::productionLeft() const
{
	return m_pCity ? m_pCity->productionLeft() : -1;
}

int CyCity::hurryGold(int /*HurryTypes*/ iHurry) const
{
	return m_pCity ? m_pCity->hurryGold((HurryTypes)iHurry) : -1;
}

int CyCity::hurryPopulation(int /*HurryTypes*/ iHurry) const
{
	return m_pCity ? m_pCity->hurryPopulation((HurryTypes)iHurry) : -1;
}

int CyCity::hurryProduction(int /*HurryTypes*/ iHurry) const
{
	return m_pCity ? m_pCity->hurryProduction((HurryTypes)iHurry) : -1;
}

int CyCity::flatHurryAngerLength() const
{
	return m_pCity ? m_pCity->flatHurryAngerLength() : -1;
}

int CyCity::getNumBuilding(int /*BuildingTypes*/ iIndex) const
{
	return m_pCity ? (iIndex != -1 ? m_pCity->getNumBuilding((BuildingTypes) iIndex) : 0) : -1;
}

bool CyCity::isHasBuilding(int /*BuildingTypes*/ iIndex) const
{
	return m_pCity ? m_pCity->getNumBuilding((BuildingTypes) iIndex) > 0 : false;
}

int CyCity::getNumActiveBuilding(int /*BuildingTypes*/ iIndex) const
{
	return m_pCity ? m_pCity->getNumActiveBuilding((BuildingTypes) iIndex) : -1;
}

int CyCity::getID() const
{
	return m_pCity ? m_pCity->getID() : -1;
}

int CyCity::getX() const
{
	return m_pCity ? m_pCity->getX() : -1;
}

int CyCity::getY() const
{
	return m_pCity ? m_pCity->getY() : -1;
}

CyPlot* CyCity::plot() const
{
	return	m_pCity ? new CyPlot(m_pCity->plot()) : NULL;
}

bool CyCity::isConnectedTo(const CyCity& kCity) const
{
	return m_pCity ? m_pCity->isConnectedTo(kCity.getCity()) : false;
}

bool CyCity::isConnectedToCapital(int /*PlayerTypes*/ ePlayer) const
{
	return m_pCity ? m_pCity->isConnectedToCapital((PlayerTypes)ePlayer) : false;
}

CyArea* CyCity::area() const
{
	return	m_pCity ? new CyArea(m_pCity->area()) : NULL;
}

CyArea* CyCity::waterArea() const
{
	return	m_pCity ? new CyArea(m_pCity->waterArea()) : NULL;
}

CyPlot* CyCity::getRallyPlot() const
{
	return	m_pCity ? new CyPlot(m_pCity->getRallyPlot()) : NULL;
}

int CyCity::getGameTurnFounded() const
{
	return m_pCity ? m_pCity->getGameTurnFounded() : -1;
}

int CyCity::getGameTurnAcquired() const
{
	return m_pCity ? m_pCity->getGameTurnAcquired() : -1;
}

int CyCity::getPopulation() const
{
	return m_pCity ? m_pCity->getPopulation() : -1;
}

void CyCity::setPopulation(int iNewValue)
{
	if (m_pCity) m_pCity->setPopulation(iNewValue);
}

void CyCity::changePopulation(int iChange)
{
	if (m_pCity) m_pCity->changePopulation(iChange);
}

long CyCity::getRealPopulation() const
{
	return m_pCity ? m_pCity->getRealPopulation() : -1;
}

int CyCity::getHighestPopulation() const
{
	return m_pCity ? m_pCity->getHighestPopulation() : -1;
}

void CyCity::setHighestPopulation(int iNewValue)
{
	if (m_pCity) m_pCity->setHighestPopulation(iNewValue);
}

int CyCity::getWorkingPopulation() const
{
	return m_pCity ? m_pCity->getWorkingPopulation() : -1;
}

int CyCity::getSpecialistPopulation() const
{
	return m_pCity ? m_pCity->getSpecialistPopulation() : -1;
}

int CyCity::getNumGreatPeople() const
{
	return m_pCity ? m_pCity->getNumGreatPeople() : -1;
}

int CyCity::getBaseGreatPeopleRate() const
{
	return m_pCity ? m_pCity->getBaseGreatPeopleRate() : -1;
}

int CyCity::getGreatPeopleRate() const
{
	return m_pCity ? m_pCity->getGreatPeopleRate() : -1;
}

void CyCity::changeBaseGreatPeopleRate(int iChange)
{
	if (m_pCity) m_pCity->changeBaseGreatPeopleRate(iChange);
}

int CyCity::getGreatPeopleRateModifier() const
{
	return m_pCity ? m_pCity->getGreatPeopleRateModifier() : -1;
}

int CyCity::getGreatPeopleProgress() const
{
	return m_pCity ? m_pCity->getGreatPeopleProgress() : -1;
}

void CyCity::changeGreatPeopleProgress(int iChange)
{
	if (m_pCity) m_pCity->changeGreatPeopleProgress(iChange);
}

int CyCity::getNumWorldWonders() const
{
	return m_pCity ? m_pCity->getNumWorldWonders() : -1;
}

int CyCity::getNumNationalWonders() const
{
	return m_pCity ? m_pCity->getNumNationalWonders() : -1;
}

int CyCity::getMaxNumWorldWonders() const
{
	return m_pCity ? m_pCity->getMaxNumWorldWonders() : -1;
}

int CyCity::getMaxNumNationalWonders() const
{
	return m_pCity ? m_pCity->getMaxNumNationalWonders() : -1;
}

int CyCity::getNumBuildings() const
{
	return m_pCity ? m_pCity->getNumBuildings() : -1;
}

bool CyCity::isGovernmentCenter() const
{
	return m_pCity ? m_pCity->isGovernmentCenter() : false;
}

int CyCity::getMaintenance() const
{
	return m_pCity ? m_pCity->getMaintenance() : -1;
}

int CyCity::getMaintenanceTimes100() const
{
	return m_pCity ? m_pCity->getMaintenanceTimes100() : -1;
}

int CyCity::calculateDistanceMaintenance() const
{
	return m_pCity ? m_pCity->calculateDistanceMaintenance() : -1;
}

int CyCity::calculateNumCitiesMaintenance()	const
{
	return m_pCity ? m_pCity->calculateNumCitiesMaintenance() : -1;
}

int CyCity::calculateColonyMaintenanceTimes100()	const
{
	return m_pCity ? m_pCity->calculateColonyMaintenanceTimes100() : -1;
}

int CyCity::getMaintenanceModifier() const
{
	return m_pCity ? m_pCity->getMaintenanceModifier() : -1;
}

int CyCity::getWarWearinessModifier() const
{
	return m_pCity ? m_pCity->getWarWearinessModifier() : -1;
}

int CyCity::getEspionageHealthCounter() const
{
	return m_pCity ? m_pCity->getEspionageHealthCounter() : -1;
}

void CyCity::changeEspionageHealthCounter(int iChange)
{
	if (m_pCity) m_pCity->changeEspionageHealthCounter(iChange);
}

int CyCity::getEspionageHappinessCounter() const
{
	return m_pCity ? m_pCity->getEspionageHappinessCounter() : -1;
}

void CyCity::changeEspionageHappinessCounter(int iChange)
{
	if (m_pCity) m_pCity->changeEspionageHappinessCounter(iChange);
}

int CyCity::getBuildingHealth(int /*BuildingTypes*/ eBuilding) const
{
	return m_pCity ? m_pCity->getBuildingHealth((BuildingTypes)eBuilding) : -1;
}

int CyCity::getMilitaryHappinessUnits() const
{
	return m_pCity ? m_pCity->getMilitaryHappinessUnits() : -1;
}

int CyCity::getBuildingHappiness(int /*BuildingTypes*/ eBuilding) const
{
	return m_pCity ? m_pCity->getBuildingHappiness((BuildingTypes)eBuilding) : -1;
}

int CyCity::getReligionHappiness(int /*ReligionTypes*/ eReligion) const
{
	return m_pCity ? m_pCity->getReligionHappiness((ReligionTypes)eReligion) : -1;
}

int CyCity::getExtraHappiness() const
{
	return m_pCity ? m_pCity->getExtraHappiness() : -1;
}

void CyCity::changeExtraHappiness(int iChange)
{
	if (m_pCity) m_pCity->changeExtraHappiness(iChange);
}

int CyCity::getExtraHealth() const
{
	return m_pCity ? m_pCity->getExtraHealth() : -1;
}

void CyCity::changeExtraHealth(int iChange)
{
	if (m_pCity) m_pCity->changeExtraHealth(iChange);
}

int CyCity::getHurryAngerTimer() const
{
	return m_pCity ? m_pCity->getHurryAngerTimer() : -1;
}

void CyCity::changeHurryAngerTimer(int iChange)
{
	if (m_pCity) m_pCity->changeHurryAngerTimer(iChange);
}

int CyCity::getRevRequestAngerTimer() const
{
	return m_pCity ? m_pCity->getRevRequestAngerTimer() : -1;
}

void CyCity::changeRevRequestAngerTimer(int iChange)
{
	if (m_pCity) m_pCity->changeRevRequestAngerTimer(iChange);
}

void CyCity::changeRevSuccessTimer(int iChange)
{
	if (m_pCity) m_pCity->changeRevSuccessTimer(iChange);
}

int CyCity::getConscriptAngerTimer() const
{
	return m_pCity ? m_pCity->getConscriptAngerTimer() : -1;
}

void CyCity::changeConscriptAngerTimer(int iChange)
{
	if (m_pCity) m_pCity->changeConscriptAngerTimer(iChange);
}

int CyCity::getDefyResolutionAngerTimer() const
{
	return m_pCity ? m_pCity->getDefyResolutionAngerTimer() : -1;
}

void CyCity::changeDefyResolutionAngerTimer(int iChange)
{
	if (m_pCity) m_pCity->changeDefyResolutionAngerTimer(iChange);
}

int CyCity::flatDefyResolutionAngerLength() const
{
	return m_pCity ? m_pCity->flatDefyResolutionAngerLength() : -1;
}

int CyCity::getHappinessTimer() const
{
	return m_pCity ? m_pCity->getHappinessTimer() : -1;
}

void CyCity::changeHappinessTimer(int iChange)
{
	if (m_pCity) m_pCity->changeHappinessTimer(iChange);
}

bool CyCity::isNoUnhappiness() const
{
	return m_pCity ? m_pCity->isNoUnhappiness() : false;
}

bool CyCity::isBuildingOnlyHealthy() const
{
	return m_pCity ? m_pCity->isBuildingOnlyHealthy() : false;
}

int CyCity::getFood() const
{
	return m_pCity ? m_pCity->getFood() : -1;
}

void CyCity::setFood(int iNewValue)
{
	if (m_pCity) m_pCity->setFood(iNewValue);
}

void CyCity::changeFood(int iChange)
{
	if (m_pCity) m_pCity->changeFood(iChange);
}

int CyCity::getFoodKept() const
{
	return m_pCity ? m_pCity->getFoodKept() : -1;
}

int CyCity::getOverflowProduction() const
{
	return m_pCity ? m_pCity->getOverflowProduction() : -1;
}

void CyCity::setOverflowProduction(int iNewValue)
{
	if (m_pCity) m_pCity->setOverflowProduction(iNewValue);
}

int CyCity::getFeatureProduction() const
{
	return m_pCity ? m_pCity->getFeatureProduction() : -1;
}

void CyCity::setFeatureProduction(int iNewValue)
{
	if (m_pCity) m_pCity->setFeatureProduction(iNewValue);
}

int CyCity::getMilitaryProductionModifier() const
{
	return m_pCity ? m_pCity->getMilitaryProductionModifier() : -1;
}

int CyCity::getSpaceProductionModifier() const
{
	return m_pCity ? m_pCity->getSpaceProductionModifier() : -1;
}

int CyCity::getExtraTradeRoutes() const
{
	return m_pCity ? m_pCity->getExtraTradeRoutes() : -1;
}

void CyCity::changeExtraTradeRoutes(int iChange)
{
	if (m_pCity) m_pCity->changeExtraTradeRoutes(iChange);
}

int CyCity::getTradeRouteModifier() const
{
	return m_pCity ? m_pCity->getTradeRouteModifier() : -1;
}

int CyCity::getForeignTradeRouteModifier() const
{
	return m_pCity ? m_pCity->getForeignTradeRouteModifier() : -1;
}

int CyCity::getBuildingDefense() const
{
	return m_pCity ? m_pCity->getBuildingDefense() : -1;
}

int CyCity::getFreeExperience() const
{
	return m_pCity ? m_pCity->getFreeExperience() : -1;
}

int CyCity::getMaxAirlift() const
{
	return m_pCity ? m_pCity->getMaxAirlift() : -1;
}

int CyCity::getAirModifier() const
{
	return m_pCity ? m_pCity->getAirModifier() : -1;
}

int CyCity::getNukeModifier() const
{
	return m_pCity ? m_pCity->getNukeModifier() : -1;
}

bool CyCity::isPower() const
{
	return m_pCity ? m_pCity->isPower() : false;
}

bool CyCity::isAreaCleanPower() const
{
	return m_pCity ? m_pCity->isAreaCleanPower() : false;
}

bool CyCity::isDirtyPower() const
{
	return m_pCity ? m_pCity->isDirtyPower() : false;
}

int CyCity::getDefenseDamage() const
{
	return m_pCity ? m_pCity->getDefenseDamage() : -1;
}

void CyCity::changeDefenseDamage(int iChange)
{
	if (m_pCity) m_pCity->changeDefenseDamage(iChange);
}

int CyCity::getNaturalDefense() const
{
	return m_pCity ? m_pCity->getNaturalDefense() : -1;
}

int CyCity::getTotalDefense(bool bIgnoreBuilding) const
{
	return m_pCity ? m_pCity->getTotalDefense(bIgnoreBuilding) : -1;
}

int CyCity::getDefenseModifier(bool bIgnoreBuilding) const
{
	return m_pCity ? m_pCity->getDefenseModifier(bIgnoreBuilding) : -1;
}

int CyCity::getOccupationTimer() const
{
	return m_pCity ? m_pCity->getOccupationTimer() : -1;
}

bool CyCity::isOccupation() const
{
	return m_pCity ? m_pCity->isOccupation() : false;
}

void CyCity::setOccupationTimer(int iNewValue)
{
	if (m_pCity) m_pCity->setOccupationTimer(iNewValue);
}

void CyCity::changeOccupationTimer(int iChange)
{
	if (m_pCity) m_pCity->changeOccupationTimer(iChange);
}

bool CyCity::isNeverLost() const
{
	return m_pCity ? m_pCity->isNeverLost() : false;
}

void CyCity::setNeverLost(int iNewValue)
{
	if (m_pCity) m_pCity->setNeverLost(iNewValue);
}

int CyCity::getMADIncoming() const
{
	return m_pCity ? m_pCity->getMADIncoming() : -1;
}

bool CyCity::isBombarded() const
{
	return m_pCity ? m_pCity->isBombarded() : false;
}

void CyCity::setBombarded(int iNewValue)
{
	if (m_pCity) m_pCity->setBombarded(iNewValue);
}

bool CyCity::isDrafted() const
{
	return m_pCity ? m_pCity->isDrafted() : false;
}

void CyCity::setDrafted(int iNewValue)
{
	if (m_pCity) m_pCity->setDrafted(iNewValue);
}

bool CyCity::isAirliftTargeted() const
{
	return m_pCity ? m_pCity->isAirliftTargeted() : false;
}

void CyCity::setAirliftTargeted(int iNewValue)
{
	if (m_pCity) m_pCity->setAirliftTargeted(iNewValue);
}

bool CyCity::isCitizensAutomated() const
{
	return m_pCity ? m_pCity->isCitizensAutomated() : false;
}

void CyCity::setCitizensAutomated(bool bNewValue)
{
	if (m_pCity) m_pCity->setCitizensAutomated(bNewValue);
}

bool CyCity::isProductionAutomated() const
{
	return m_pCity ? m_pCity->isProductionAutomated() : false;
}

void CyCity::setProductionAutomated(bool bNewValue)
{
	if (m_pCity) m_pCity->setProductionAutomated(bNewValue, false);
}

bool CyCity::isWallOverride() const
{
	return m_pCity ? m_pCity->isWallOverride() : false;
}

void CyCity::setWallOverride(bool bOverride)
{
	if (m_pCity) m_pCity->setWallOverride(bOverride);
}

bool CyCity::isPlundered() const
{
	return m_pCity ? m_pCity->isPlundered() : false;
}

void CyCity::setPlundered(bool bNewValue)
{
	if (m_pCity) m_pCity->setPlundered(bNewValue);
}

int /*PlayerTypes*/ CyCity::getOwner() const
{
	return m_pCity ? m_pCity->getOwner() : NO_PLAYER;
}

int /*TeamTypes*/ CyCity::getTeam() const
{
	return m_pCity ? m_pCity->getTeam() : NO_TEAM;
}

int /*PlayerTypes*/ CyCity::getPreviousOwner() const
{
	return m_pCity ? m_pCity->getPreviousOwner() : NO_PLAYER;
}

int /*PlayerTypes*/ CyCity::getOriginalOwner() const
{
	return m_pCity ? m_pCity->getOriginalOwner() : NO_PLAYER;
}

int /*CultureLevelTypes*/ CyCity::getCultureLevel() const
{
	return m_pCity ? m_pCity->getCultureLevel() : NO_CULTURELEVEL;
}

int CyCity::getCultureThreshold() const
{
	return m_pCity ? m_pCity->getCultureThreshold() : -1;
}

int CyCity::getSeaPlotYield(int /*YieldTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getSeaPlotYield((YieldTypes) eIndex) : -1;
}

int CyCity::getBaseYieldRate(int /*YieldTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getBaseYieldRate((YieldTypes)eIndex) : -1;
}

void CyCity::changeBaseYieldRate(int /*YieldTypes*/ eIndex, int iNewValue)
{
	if (m_pCity) m_pCity->changeBaseYieldRate((YieldTypes)eIndex, iNewValue);
}

int CyCity::getBaseYieldRateModifier(int /*YieldTypes*/ eIndex, int iExtra) const
{
	return m_pCity ? m_pCity->getBaseYieldRateModifier((YieldTypes)eIndex, iExtra) : -1;
}

int CyCity::getYieldRate(int /*YieldTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getYieldRate((YieldTypes)eIndex) : -1;
}

int CyCity::getYieldRateModifier(int /*YieldTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getYieldRateModifier((YieldTypes)eIndex) : -1;
}

int CyCity::getTradeYield(int /*YieldTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getTradeYield((YieldTypes)eIndex) : -1;
}

// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
int CyCity::calculateTradeProfitTimes100(const CyCity& kCity) const
{
	return m_pCity ? m_pCity->calculateTradeProfitTimes100(kCity.getCity()) : -1;
}
#endif
// BUG - Fractional Trade Routes - end

int CyCity::calculateTradeProfit(const CyCity& kCity) const
{
	return m_pCity ? m_pCity->calculateTradeProfit(kCity.getCity()) : -1;
}

int CyCity::calculateTradeYield(int /*YieldTypes*/ eIndex, int iTradeProfit) const
{
	return m_pCity ? m_pCity->calculateTradeYield((YieldTypes)eIndex, iTradeProfit) : -1;
}

int CyCity::getCommerceRate(int /*CommerceTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getCommerceRate((CommerceTypes)eIndex) : -1;
}

int CyCity::getCommerceRateTimes100(int /*CommerceTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getCommerceRateTimes100((CommerceTypes)eIndex) : -1;
}

int CyCity::getBaseCommerceRate(int /*CommerceTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getBaseCommerceRate((CommerceTypes)eIndex) : -1;
}

int CyCity::getProductionToCommerceModifier(int /*CommerceTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getProductionToCommerceModifier((CommerceTypes)eIndex) : -1;
}

int CyCity::getBuildingCommerceByBuilding(int /*CommerceTypes*/ eIndex, int /*BuildingTypes*/ iBuilding) const
{
	return m_pCity ? m_pCity->getBuildingCommerceByBuilding((CommerceTypes)eIndex, (BuildingTypes) iBuilding) : -1;
}

int CyCity::getSpecialistCommerce(int /*CommerceTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getSpecialistCommerce((CommerceTypes)eIndex) : -1;
}

void CyCity::changeSpecialistCommerce(int /*CommerceTypes*/ eIndex, int iChange)
{
	if (m_pCity) m_pCity->changeSpecialistCommerceTimes100((CommerceTypes)eIndex, 100*iChange);
}

int CyCity::getReligionCommerceByReligion(int /*CommerceTypes*/ eIndex, int /*ReligionTypes*/ eReligion) const
{
	return m_pCity ? m_pCity->getReligionCommerceByReligion((CommerceTypes)eIndex, (ReligionTypes)eReligion) : -1;
}

int CyCity::getCorporationCommerce(int /*CommerceTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getCorporationCommerce((CommerceTypes)eIndex) : -1;
}

int CyCity::getCorporationCommerceByCorporation(int /*CommerceTypes*/ eIndex, int /*CorporationTypes*/ eCorporation) const
{
	return m_pCity ? m_pCity->getCorporationCommerceByCorporation((CommerceTypes)eIndex, (CorporationTypes)eCorporation) : -1;
}

int CyCity::getCorporationYield(int /*YieldTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getCorporationYield((YieldTypes)eIndex) : -1;
}

int CyCity::getCorporationYieldByCorporation(int /*YieldTypes*/ eIndex, int /*CorporationTypes*/ eCorporation) const
{
	return m_pCity ? m_pCity->getCorporationYieldByCorporation((YieldTypes)eIndex, (CorporationTypes)eCorporation) : -1;
}

int CyCity::getCommerceRateModifier(int /*CommerceTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getTotalCommerceRateModifier((CommerceTypes)eIndex) : -1;
}

int CyCity::getCommerceHappinessByType(int /*CommerceTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getCommerceHappinessByType((CommerceTypes) eIndex) : -1;
}

int CyCity::getDomainFreeExperience(int /*DomainTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getDomainFreeExperience((DomainTypes)eIndex) : -1;
}

int CyCity::getDomainProductionModifier(int /*DomainTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getDomainProductionModifier((DomainTypes)eIndex) : -1;
}

int CyCity::getCulture(int /*PlayerTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getCulture((PlayerTypes)eIndex) : -1;
}

int CyCity::getCultureTimes100(int /*PlayerTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getCultureTimes100((PlayerTypes)eIndex) : -1;
}

int CyCity::countTotalCultureTimes100() const
{
	return m_pCity ? m_pCity->countTotalCultureTimes100() : -1;
}

PlayerTypes CyCity::findHighestCulture() const
{
	return m_pCity ? m_pCity->findHighestCulture() : NO_PLAYER;
}

int CyCity::calculateCulturePercent(int /*PlayerTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->calculateCulturePercent((PlayerTypes)eIndex) : -1;
}

void CyCity::setCulture(int /*PlayerTypes*/ eIndex, int iNewValue, bool bPlots)
{
	if (m_pCity) m_pCity->setCulture((PlayerTypes)eIndex, iNewValue, bPlots, true);
}

void CyCity::setCultureTimes100(int /*PlayerTypes*/ eIndex, int iNewValue, bool bPlots)
{
	if (m_pCity) m_pCity->setCultureTimes100((PlayerTypes)eIndex, iNewValue, bPlots, true);
}

void CyCity::changeCulture(int /*PlayerTypes*/ eIndex, int iChange, bool bPlots)
{
	if (m_pCity) m_pCity->changeCulture((PlayerTypes)eIndex, iChange, bPlots, true);
}

int CyCity::getNumRevolts(int playerIdx) const
{
	return m_pCity ? m_pCity->getNumRevolts((PlayerTypes)playerIdx) : -1;
}

void CyCity::changeNumRevolts(int playerIdx, int iChange)
{
	if (m_pCity) m_pCity->changeNumRevolts((PlayerTypes)playerIdx, iChange);
}

bool CyCity::isRevealed(int /*TeamTypes*/ eIndex, bool bDebug) const
{
	return m_pCity ? m_pCity->isRevealed((TeamTypes)eIndex, bDebug) : false;
}

void CyCity::setRevealed(int /*TeamTypes*/ eIndex, bool bNewValue)
{
	if (m_pCity) m_pCity->setRevealed((TeamTypes)eIndex, bNewValue);
}

bool CyCity::getEspionageVisibility(int /*TeamTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getEspionageVisibility((TeamTypes)eIndex) : false;
}

std::wstring CyCity::getName() const
{
	return m_pCity ? m_pCity->getName() : L"";
}

std::wstring CyCity::getNameForm(int iForm) const
{
	return m_pCity ? m_pCity->getName((uint)iForm) : L"";
}

std::wstring CyCity::getNameKey() const
{
	return m_pCity ? m_pCity->getNameKey() : L"";
}

void CyCity::setName(std::wstring szNewValue, bool bFound)
{
	if (m_pCity) m_pCity->setName((CvWString)szNewValue, bFound);
}

void CyCity::changeNoBonusCount(int /*BonusTypes*/ eBonus, int iChange)
{
	if (m_pCity) m_pCity->changeNoBonusCount((BonusTypes)eBonus, iChange);
}

bool CyCity::isNoBonus(int /*BonusTypes*/ eBonus) const
{
	return m_pCity ? m_pCity->isNoBonus((BonusTypes)eBonus) : false;
}

int CyCity::getFreeBonus(int /*BonusTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getFreeBonus((BonusTypes)eIndex) : -1;
}

void CyCity::changeFreeBonus(int /*BonusTypes*/ eIndex, int iChange)
{
	if (m_pCity) m_pCity->changeFreeBonus((BonusTypes)eIndex, iChange);
}

int CyCity::getNumBonuses(int /*BonusTypes*/ iBonus) const
{
	return m_pCity ? m_pCity->getNumBonuses((BonusTypes) iBonus) : -1;
}

bool CyCity::hasBonus(int /*BonusTypes*/ iBonus) const
{
	return m_pCity ? m_pCity->hasBonus((BonusTypes) iBonus) : false;
}

int CyCity::getBuildingProduction(int /*BuildingTypes*/ iIndex) const
{
	return m_pCity ? m_pCity->getBuildingProduction((BuildingTypes) iIndex) : -1;
}

void CyCity::setBuildingProduction(int /*BuildingTypes*/ iIndex, int iNewValue)
{
	if (m_pCity) m_pCity->setBuildingProduction((BuildingTypes) iIndex, std::max(0, iNewValue));
}

int CyCity::getBuildingProductionTime(int /*BuildingTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getBuildingProductionTime((BuildingTypes)eIndex) : -1;
}

bool CyCity::isBuildingProductionDecay(int /*BuildingTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->isBuildingProductionDecay((BuildingTypes)eIndex) : false;
}

int CyCity::getBuildingProductionDecayTurns(int /*BuildingTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getBuildingProductionDecayTurns((BuildingTypes)eIndex) : 0;
}

int CyCity::getBuildingOriginalOwner(int /*BuildingTypes*/ iIndex) const
{
	return m_pCity ? m_pCity->getBuildingOriginalOwner((BuildingTypes) iIndex) : -1;
}

int CyCity::getBuildingOriginalTime(int /*BuildingTypes*/ iIndex) const
{
	return m_pCity ? m_pCity->getBuildingOriginalTime((BuildingTypes) iIndex) : -1;
}

void CyCity::setBuildingOriginalTime(int iIndex, int iNewValue)
{
	if (m_pCity)
		m_pCity->setBuildingOriginalTime((BuildingTypes) iIndex, iNewValue);
}

int CyCity::getUnitProduction(int iIndex) const
{
	return m_pCity ? m_pCity->getUnitProduction((UnitTypes) iIndex) : -1;
}

void CyCity::setUnitProduction(int iIndex, int iNewValue)
{
	if (m_pCity) m_pCity->setUnitProduction((UnitTypes)iIndex, iNewValue);
}

int CyCity::getUnitProductionTime(int /*UnitTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getUnitProductionTime((UnitTypes)eIndex) : -1;
}

bool CyCity::isUnitProductionDecay(int /*UnitTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->isUnitProductionDecay((UnitTypes)eIndex) : false;
}

int CyCity::getUnitProductionDecayTurns(int /*UnitTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getUnitProductionDecayTurns((UnitTypes)eIndex) : 0;
}

int CyCity::getProjectProduction(int /*ProjectTypes*/ iIndex) const
{
	return m_pCity ? m_pCity->getProjectProduction((ProjectTypes) iIndex) : -1;
}

int CyCity::getGreatPeopleUnitProgress(int /*UnitTypes*/ iIndex) const
{
	return m_pCity ? m_pCity->getGreatPeopleUnitProgress((UnitTypes) iIndex) : -1;
}

void CyCity::setGreatPeopleUnitProgress(int /*UnitTypes*/ iIndex, int iNewValue)
{
	if (m_pCity) m_pCity->setGreatPeopleUnitProgress((UnitTypes) iIndex, iNewValue);
}

void CyCity::changeGreatPeopleUnitProgress(int /*UnitTypes*/ iIndex, int iChange)
{
	if (m_pCity) m_pCity->changeGreatPeopleUnitProgress((UnitTypes) iIndex, iChange);
}

int CyCity::getSpecialistCount(int /*SpecialistTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getSpecialistCount((SpecialistTypes)eIndex) : -1;
}

int CyCity::getMaxSpecialistCount(int /*SpecialistTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getMaxSpecialistCount((SpecialistTypes)eIndex) : -1;
}

bool CyCity::isSpecialistValid(int /*SpecialistTypes*/ eIndex, int iExtra) const
{
	return m_pCity ? m_pCity->isSpecialistValid((SpecialistTypes) eIndex, iExtra) : false;
}

int CyCity::getForceSpecialistCount(int /*SpecialistTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getForceSpecialistCount((SpecialistTypes)eIndex) : -1;
}

int CyCity::getImprovementFreeSpecialists(int /*ImprovementTypes*/ iIndex) const
{
	return m_pCity ? m_pCity->getImprovementFreeSpecialists((ImprovementTypes) iIndex) : -1;
}

int CyCity::getReligionInfluence(int /*ReligionTypes*/ iIndex) const
{
	return m_pCity ? m_pCity->getReligionInfluence((ReligionTypes) iIndex) : -1;
}

void CyCity::setForceSpecialistCount(int /*SpecialistTypes*/ eIndex, int iNewValue)
{
	if (m_pCity) m_pCity->setForceSpecialistCount((SpecialistTypes)eIndex, iNewValue);
}

int CyCity::getFreeSpecialistCount(int /*SpecialistTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getFreeSpecialistCount((SpecialistTypes)eIndex) : -1;
}

void CyCity::changeFreeSpecialistCount(int /*SpecialistTypes*/ eIndex, int iChange)
{
	if (m_pCity) m_pCity->changeFreeSpecialistCount((SpecialistTypes)eIndex, iChange, true);
}

int CyCity::getAddedFreeSpecialistCount(int /*SpecialistTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getAddedFreeSpecialistCount((SpecialistTypes)eIndex) : -1;
}

void CyCity::changeImprovementFreeSpecialists(int /*ImprovementTypes*/ iIndex, int iChange)
{
	if (m_pCity) m_pCity->changeImprovementFreeSpecialists((ImprovementTypes) iIndex, iChange);
}

void CyCity::changeReligionInfluence(int /*ReligionTypes*/ iIndex, int iChange)
{
	if (m_pCity) m_pCity->changeReligionInfluence((ReligionTypes) iIndex, iChange);
}

int CyCity::getStateReligionHappiness(int /*ReligionTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getStateReligionHappiness((ReligionTypes) eIndex) : -1;
}

void CyCity::changeStateReligionHappiness(int /*ReligionTypes*/ eIndex, int iChange)
{
	if (m_pCity) m_pCity->changeStateReligionHappiness((ReligionTypes) eIndex, iChange);
}

int CyCity::getSpecialistFreeExperience() const
{
	return m_pCity ? m_pCity->getSpecialistFreeExperience() : -1;
}

int CyCity::getEspionageDefenseModifier() const
{
	return m_pCity ? m_pCity->getEspionageDefenseModifier() : 0;
}

bool CyCity::isWorkingPlot(const CyPlot& kPlot) const
{
	return m_pCity ? m_pCity->isWorkingPlot(kPlot.getPlot()) : false;
}

int CyCity::getNumRealBuilding(int /*BuildingTypes*/ iIndex) const
{
	return m_pCity ? m_pCity->getNumRealBuilding((BuildingTypes) iIndex) : -1;
}

void CyCity::setNumRealBuilding(int /*BuildingTypes*/ iIndex, int iNewValue)
{
	if (m_pCity) m_pCity->setNumRealBuilding((BuildingTypes) iIndex, iNewValue);
}

bool CyCity::isHasReligion(int /*ReligionTypes*/ iIndex) const
{
	return m_pCity ? m_pCity->isHasReligion((ReligionTypes) iIndex) : false;
}

void CyCity::setHasReligion(int /*ReligionTypes*/ iIndex, bool bNewValue, bool bAnnounce, bool bArrows)
{
	if (m_pCity) m_pCity->setHasReligion((ReligionTypes) iIndex, bNewValue, bAnnounce, bArrows);
}

bool CyCity::isHasCorporation(int /*CorporationTypes*/ iIndex) const
{
	return m_pCity ? m_pCity->isHasCorporation((CorporationTypes) iIndex) : false;
}

void CyCity::setHasCorporation(int /*CorporationTypes*/ iIndex, bool bNewValue, bool bAnnounce, bool bArrows)
{
	if (m_pCity) m_pCity->setHasCorporation((CorporationTypes) iIndex, bNewValue, bAnnounce, bArrows);
}

bool CyCity::isActiveCorporation(int /*CorporationTypes*/ eCorporation) const
{
	return m_pCity ? m_pCity->isActiveCorporation((CorporationTypes) eCorporation) : false;
}

CyCity* CyCity::getTradeCity(int iIndex) const
{
	return m_pCity ? new CyCity(m_pCity->getTradeCity(iIndex)) : NULL;
}

int CyCity::getTradeRoutes() const
{
	return m_pCity ? m_pCity->getTradeRoutes() : -1;
}

void CyCity::clearOrderQueue()
{
	if (m_pCity) m_pCity->clearOrderQueue();
}

void CyCity::pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bForce)
{
	if (m_pCity) m_pCity->pushOrder(eOrder, iData1, iData2, bSave, bPop, bAppend, bForce);
}

void CyCity::popOrder(int iNum, bool bFinish, bool bChoose)
{
	if (m_pCity) m_pCity->popOrder(iNum, bFinish, bChoose);
}

int CyCity::getOrderQueueLength() const
{
	return m_pCity ? m_pCity->getOrderQueueLength() : -1;
}

OrderData CyCity::getOrderFromQueue(int iIndex) const
{
	return m_pCity ? m_pCity->getOrderAt(iIndex) : OrderData::InvalidOrder;
}

bool CyCity::AI_isEmphasize(int iEmphasizeType) const
{
	return m_pCity ? m_pCity->AI_isEmphasize((EmphasizeTypes)iEmphasizeType) : false;
}

int CyCity::AI_countBestBuilds(const CyArea& kArea) const
{
	return m_pCity ? m_pCity->AI_countBestBuilds(const_cast<CvArea*>(kArea.getArea())) : -1;
}

int CyCity::AI_cityValue() const
{
	return m_pCity ? m_pCity->AI_cityValue() : -1;
}

std::string CyCity::getScriptData() const
{
	return m_pCity ? m_pCity->getScriptData() : "";
}

void CyCity::setScriptData(std::string szNewValue)
{
	if (m_pCity) m_pCity->setScriptData(szNewValue);
}

int CyCity::getBuildingYieldChange(int /*BuildingTypes*/ eBuilding, int /*YieldTypes*/ eYield) const
{
	return m_pCity ? m_pCity->getBuildingYieldChange((BuildingTypes)eBuilding, (YieldTypes)eYield) : 0;
}

void CyCity::setBuildingYieldChange(int /*BuildingTypes*/ eBuilding, int /*YieldTypes*/ eYield, int iChange)
{
	if (m_pCity) m_pCity->setBuildingYieldChange((BuildingTypes)eBuilding, (YieldTypes)eYield, iChange);
}

int CyCity::getBuildingCommerceChange(int /*BuildingTypes*/ eBuilding, int /*CommerceTypes*/ eCommerce) const
{
	return m_pCity ? m_pCity->getBuildingCommerceChange((BuildingTypes)eBuilding, (CommerceTypes)eCommerce) : 0;
}

void CyCity::setBuildingCommerceChange(int /*BuildingTypes*/ eBuilding, int /*CommerceTypes*/ eCommerce, int iChange)
{
	if (m_pCity) m_pCity->setBuildingCommerceChange((BuildingTypes)eBuilding, (CommerceTypes)eCommerce, iChange);
}

int CyCity::getBuildingHappyChange(int /*BuildingTypes*/ eBuilding) const
{
	return m_pCity ? m_pCity->getBuildingHappyChange((BuildingTypes)eBuilding) : 0;
}

void CyCity::setBuildingHappyChange(int /*BuildingTypes*/ eBuilding, int iChange)
{
	if (m_pCity) m_pCity->setBuildingHappyChange((BuildingTypes)eBuilding, iChange);
}

int CyCity::getBuildingHealthChange(int /*BuildingTypes*/ eBuilding) const
{
	return m_pCity ? m_pCity->getBuildingHealthChange((BuildingTypes)eBuilding) : 0;
}

void CyCity::setBuildingHealthChange(int /*BuildingTypes*/ eBuilding, int iChange)
{
	if (m_pCity) m_pCity->setBuildingHealthChange((BuildingTypes)eBuilding, iChange);
}

int CyCity::getLiberationPlayer(bool bConquest) const
{
	return m_pCity ? m_pCity->getLiberationPlayer(bConquest) : -1;
}

int CyCity::getBonusCommerceRateModifier(int /*CommerceTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getBonusCommerceRateModifier((CommerceTypes)eIndex) : 0;
}

int CyCity::getArea() const
{
	return m_pCity ? m_pCity->getArea() : 0;
}

bool CyCity::isWeLoveTheKingDay() const
{
	return m_pCity ? m_pCity->isWeLoveTheKingDay() : false;
}

void CyCity::setWeLoveTheKingDay(bool bWeLoveTheKingDay)
{
	if (m_pCity) m_pCity->setWeLoveTheKingDay(bWeLoveTheKingDay);
}

int CyCity::calculateCorporateTaxes() const
{
	return m_pCity ? m_pCity->calculateCorporateTaxes() : 0;
}

void CyCity::changePowerCount(int iChange, bool bDirty)
{
	if (m_pCity) m_pCity->changePowerCount(iChange, bDirty);
}

void CyCity::changeEventAnger(int iChange)
{
	if (m_pCity) m_pCity->changeEventAnger(iChange);
}

int CyCity::getNumPopulationEmployed() const
{
	return m_pCity ? m_pCity->getNumPopulationEmployed() : 0;
}

int CyCity::getBonusCommercePercentChanges(int eIndex, int eBuilding) const
{
	return m_pCity ? m_pCity->getBonusCommercePercentChanges((CommerceTypes)eIndex, (BuildingTypes)eBuilding) : 0;
}

bool CyCity::isAutomatedCanBuild(int /*BuildTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->isAutomatedCanBuild((BuildTypes)eIndex) : true;
}

void CyCity::setAutomatedCanBuild(int /*BuildTypes*/ eIndex, bool bNewValue)
{
	if (m_pCity) m_pCity->setAutomatedCanBuild((BuildTypes)eIndex, bNewValue);
}

CvProperties* CyCity::getProperties() const
{
	return m_pCity ? m_pCity->getProperties() : NULL;
}

bool CyCity::getBuildingListFilterActive(int eFilter)
{
	return m_pCity ? m_pCity->getBuildingListFilterActive((BuildingFilterTypes)eFilter) : true;
}

void CyCity::setBuildingListFilterActive(int eFilter, bool bActive)
{
	if (m_pCity) m_pCity->setBuildingListFilterActive((BuildingFilterTypes)eFilter, bActive);
}

int CyCity::getBuildingListGrouping()
{
	return m_pCity ? m_pCity->getBuildingListGrouping() : 0;
}

void CyCity::setBuildingListGrouping(int eGrouping)
{
	if (m_pCity) m_pCity->setBuildingListGrouping((BuildingGroupingTypes)eGrouping);
}

int CyCity::getBuildingListSorting()
{
	return m_pCity ? m_pCity->getBuildingListSorting() : 0;
}

void CyCity::setBuildingListSorting(int eSorting)
{
	if (m_pCity) m_pCity->setBuildingListSorting((BuildingSortTypes)eSorting);
}

int CyCity::getBuildingListGroupNum()
{
	return m_pCity ? m_pCity->getBuildingListGroupNum() : 0;
}

int CyCity::getBuildingListNumInGroup(int iGroup)
{
	return m_pCity ? m_pCity->getBuildingListNumInGroup(iGroup) : 0;
}

int CyCity::getBuildingListType(int iGroup, int iPos)
{
	return m_pCity ? m_pCity->getBuildingListType(iGroup, iPos) : 0;
}

void CyCity::setUnitListInvalid()
{
	if (m_pCity) m_pCity->setUnitListInvalid();
}

bool CyCity::getUnitListFilterActive(int eFilter)
{
	return m_pCity ? m_pCity->getUnitListFilterActive((UnitFilterTypes)eFilter) : true;
}

void CyCity::setUnitListFilterActive(int eFilter, bool bActive)
{
	if (m_pCity) m_pCity->setUnitListFilterActive((UnitFilterTypes)eFilter, bActive);
}

int CyCity::getUnitListGrouping()
{
	return m_pCity ? m_pCity->getUnitListGrouping() : 0;
}

void CyCity::setUnitListGrouping(int eGrouping)
{
	if (m_pCity) m_pCity->setUnitListGrouping((UnitGroupingTypes)eGrouping);
}

int CyCity::getUnitListSorting()
{
	return m_pCity ? m_pCity->getUnitListSorting() : 0;
}

void CyCity::setUnitListSorting(int eSorting)
{
	if (m_pCity) m_pCity->setUnitListSorting((UnitSortTypes)eSorting);
}

int CyCity::getUnitListGroupNum()
{
	return m_pCity ? m_pCity->getUnitListGroupNum() : 0;
}

int CyCity::getUnitListNumInGroup(int iGroup)
{
	return m_pCity ? m_pCity->getUnitListNumInGroup(iGroup) : 0;
}

int CyCity::getUnitListType(int iGroup, int iPos)
{
	return m_pCity ? m_pCity->getUnitListType(iGroup, iPos) : 0;
}

bool CyCity::isEventOccured(int eEvent) const
{
	return m_pCity ? m_pCity->isEventOccured((EventTypes)eEvent) : false;
}
