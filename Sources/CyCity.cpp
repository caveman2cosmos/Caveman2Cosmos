#include "CvGameCoreDLL.h"
#include "CvDefines.h"
#include "CityOutputHistory.h"
#include "CvArea.h"
#include "CvCity.h"
#include "CyArea.h"
#include "CyCity.h"
#include "CyPlot.h"
#include "CvGameAI.h"
#include "CyUnit.h"
#include "CvGame.h"
#include "CvUnitSelectionCriteria.h"

//
// Python wrapper class for CvCity
//

CyCity::CyCity(CvCity* pCity) : m_pCity(pCity)
{
	FAssert(m_pCity != NULL);
}

void CyCity::kill()
{
	m_pCity->kill(true);
}

int CyCity::getRevolutionIndex() const
{
	return m_pCity->getRevolutionIndex();
}

void CyCity::setRevolutionIndex(int iNewValue)
{
	m_pCity->setRevolutionIndex(iNewValue);
}

void CyCity::changeRevolutionIndex(int iChange)
{
	m_pCity->changeRevolutionIndex(iChange);
}

int CyCity::getLocalRevIndex() const
{
	return m_pCity->getLocalRevIndex();
}

void CyCity::setLocalRevIndex(int iNewValue)
{
	m_pCity->setLocalRevIndex(iNewValue);
}

int CyCity::getRevIndexAverage() const
{
	return m_pCity->getRevIndexAverage();
}

void CyCity::setRevIndexAverage(int iNewValue)
{
	m_pCity->setRevIndexAverage(iNewValue);
}

void CyCity::updateRevIndexAverage()
{
	m_pCity->updateRevIndexAverage();
}

int CyCity::getRevIndexDistanceMod() const
{
	return m_pCity->getRevIndexDistanceMod();
}

int CyCity::getReinforcementCounter() const
{
	return m_pCity->getReinforcementCounter();
}

void CyCity::setReinforcementCounter(int iNewValue)
{
	m_pCity->setReinforcementCounter(iNewValue);
}

void CyCity::changeReinforcementCounter(int iChange)
{
	m_pCity->changeReinforcementCounter(iChange);
}

int CyCity::getRevolutionCounter() const
{
	return m_pCity->getRevolutionCounter();
}

void CyCity::setRevolutionCounter(int iNewValue)
{
	m_pCity->setRevolutionCounter(iNewValue);
}

void CyCity::changeRevolutionCounter(int iChange)
{
	m_pCity->changeRevolutionCounter(iChange);
}

CyPlot* CyCity::getCityIndexPlot(int iIndex) const
{
	if (m_pCity->getCityIndexPlot(iIndex))
	{
		return new CyPlot(m_pCity->getCityIndexPlot(iIndex));
	}
	return NULL;
}

bool CyCity::canWork(const CyPlot* pPlot) const
{
	return m_pCity->canWork(pPlot ? pPlot->getPlot() : NULL);
}

int CyCity::countNumImprovedPlots() const
{
	return m_pCity->countNumImprovedPlots();
}

int CyCity::countNumWaterPlots() const
{
	return m_pCity->countNumWaterPlots();
}

int CyCity::findBaseYieldRateRank(YieldTypes eYield) const
{
	return m_pCity->findBaseYieldRateRank(eYield);
}

int CyCity::findYieldRateRank(YieldTypes eYield) const
{
	return m_pCity->findYieldRateRank(eYield);
}

int CyCity::findCommerceRateRank(CommerceTypes eCommerce) const
{
	return m_pCity->findCommerceRateRank(eCommerce);
}

bool CyCity::canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreUpgrades) const
{
	return m_pCity->canTrain(eUnit, bContinue, bTestVisible, bIgnoreCost, bIgnoreUpgrades);
}

bool CyCity::canConstruct(BuildingTypes eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost) const
{
	return m_pCity->canConstruct(eBuilding, bContinue, bTestVisible, bIgnoreCost);
}

bool CyCity::canCreate(ProjectTypes eProject, bool bContinue, bool bTestVisible) const
{
	return m_pCity->canCreate(eProject, bContinue, bTestVisible);
}

bool CyCity::canMaintain(ProcessTypes eProcess) const
{
	return m_pCity->canMaintain(eProcess);
}

int CyCity::getFoodTurnsLeft() const
{
	return m_pCity->getFoodTurnsLeft();
}

bool CyCity::isProduction() const
{
	return m_pCity->isProduction();
}

bool CyCity::isProductionUnit() const
{
	return m_pCity->isProductionUnit();
}

bool CyCity::isProductionBuilding() const
{
	return m_pCity->isProductionBuilding();
}

bool CyCity::isProductionProject() const
{
	return m_pCity->isProductionProject();
}

bool CyCity::isProductionProcess() const
{
	return m_pCity->isProductionProcess();
}

int CyCity::getProductionExperience(UnitTypes eUnit) const
{
	return m_pCity->getProductionExperience(eUnit);
}

void CyCity::addProductionExperience(const CyUnit& kUnit, bool bConscript)
{
	m_pCity->addProductionExperience(kUnit.getUnit(), bConscript);
}

UnitTypes CyCity::getProductionUnit() const
{
	return m_pCity->getProductionUnit();
}

BuildingTypes CyCity::getProductionBuilding() const
{
	return m_pCity->getProductionBuilding();
}

ProjectTypes CyCity::getProductionProject() const
{
	return m_pCity->getProductionProject();
}

ProcessTypes CyCity::getProductionProcess() const
{
	return m_pCity->getProductionProcess();
}

std::wstring CyCity::getProductionName() const
{
	return m_pCity->getProductionName();
}

int CyCity::getGeneralProductionTurnsLeft() const
{
	return m_pCity->getGeneralProductionTurnsLeft();
}

std::wstring CyCity::getProductionNameKey() const
{
	return m_pCity->getProductionNameKey();
}

bool CyCity::isFoodProduction() const
{
	return m_pCity->isFoodProduction();
}

int CyCity::getFirstUnitOrder(int /*UnitTypes*/ eUnit) const
{
	return m_pCity->getFirstUnitOrder((UnitTypes)eUnit);
}

int CyCity::getFirstBuildingOrder(int /*BuildingTypes*/ eBuilding) const
{
	return m_pCity->getFirstBuildingOrder((BuildingTypes)eBuilding);
}

int CyCity::getNumTrainUnitAI(int /*UnitAITypes*/ eUnitAI) const
{
	return m_pCity->getNumTrainUnitAI((UnitAITypes) eUnitAI);
}

int CyCity::getProductionProgress() const
{
	return m_pCity->getProductionProgress();
}

int CyCity::getProductionNeeded() const
{
	return m_pCity->getProductionNeeded();
}

int CyCity::getProductionTurnsLeft() const
{
	return m_pCity->getProductionTurnsLeft();
}

int CyCity::getUnitProductionTurnsLeft(int /*UnitTypes*/ iUnit, int iNum) const
{
	return m_pCity->getProductionTurnsLeft((UnitTypes) iUnit, iNum);
}

int CyCity::getBuildingProductionTurnsLeft(int /*BuildingTypes*/ iBuilding, int iNum) const
{
	return m_pCity->getProductionTurnsLeft((BuildingTypes) iBuilding, iNum);
}

int CyCity::getProjectProductionTurnsLeft(int /*ProjectTypes*/ eProject, int iNum) const
{
	return m_pCity->getProductionTurnsLeft((ProjectTypes) eProject, iNum);
}

void CyCity::setProductionProgress(int iNewValue)
{
	m_pCity->setProductionProgress(iNewValue);
}

void CyCity::changeProduction(int iChange)
{
	m_pCity->changeProduction(iChange);
}

int CyCity::getCurrentProductionDifference(bool bIgnoreFood, bool bOverflow) const
{
	return m_pCity->getCurrentProductionDifference(
		(bIgnoreFood? ProductionCalc::None : ProductionCalc::FoodProduction) |
		(bOverflow? ProductionCalc::Overflow : ProductionCalc::None)
	);
}

bool CyCity::canHurry(int /*HurryTypes*/ iHurry, bool bTestVisible) const
{
	return m_pCity->canHurry((HurryTypes)iHurry, bTestVisible);
}

int /*UnitTypes*/ CyCity::getConscriptUnit() const
{
	return m_pCity->getConscriptUnit();
}

int CyCity::flatConscriptAngerLength() const
{
	return m_pCity->flatConscriptAngerLength();
}

bool CyCity::canConscript() const
{
	return m_pCity->canConscript();
}

int CyCity::getBonusHealth(int /*BonusTypes*/ iBonus) const
{
	return m_pCity->getBonusHealth((BonusTypes) iBonus);
}

int CyCity::getBonusHappiness(int /*BonusTypes*/ iBonus) const
{
	return m_pCity->getBonusHappiness((BonusTypes) iBonus);
}

int CyCity::getBonusPower(int /*BonusTypes*/ eBonus) const
{
	return m_pCity->getBonusPower((BonusTypes)eBonus);
}

int CyCity::getBonusYieldRateModifier(int /*YieldTypes*/ eIndex, int /*BonusTypes*/ eBonus) const
{
	return m_pCity->getBonusYieldRateModifier((YieldTypes)eIndex, (BonusTypes)eBonus);
}

int /*HandicapTypes*/ CyCity::getHandicapType() const
{
	return m_pCity->getHandicapType();
}

int /*CivilizationTypes*/ CyCity::getCivilizationType() const
{
	return m_pCity->getCivilizationType();
}

int /*LeaderHeadTypes*/ CyCity::getPersonalityType() const
{
	return m_pCity->getPersonalityType();
}

int /*ArtStyleTypes*/ CyCity::getArtStyleType() const
{
	return m_pCity->getArtStyleType();
}

bool CyCity::hasTrait(int /*TraitTypes*/ iTrait) const
{
	return m_pCity->hasTrait((TraitTypes) iTrait);
}

bool CyCity::isNPC() const
{
	return m_pCity->isNPC();
}

bool CyCity::isHominid() const
{
	return m_pCity->isHominid();
}

bool CyCity::isHuman() const
{
	return m_pCity->isHuman();
}

bool CyCity::isVisible(int /*TeamTypes*/ eTeam, bool bDebug) const
{
	return m_pCity->isVisible((TeamTypes) eTeam, bDebug);
}

bool CyCity::isCapital() const
{
	return m_pCity->isCapital();
}

bool CyCity::isCoastal(int iMinWaterSize) const
{
	return m_pCity->isCoastal(iMinWaterSize);
}

bool CyCity::isDisorder() const
{
	return m_pCity->isDisorder();
}

bool CyCity::isHolyCityByType(int /*ReligionTypes*/ iIndex) const
{
	return m_pCity->isHolyCity((ReligionTypes) iIndex);
}

bool CyCity::isHolyCity() const
{
	return m_pCity->isHolyCity();
}

bool CyCity::isHeadquartersByType(int /*CorporationTypes*/ iIndex) const
{
	return m_pCity->isHeadquarters((CorporationTypes) iIndex);
}

int CyCity::getNoMilitaryPercentAnger() const
{
	return m_pCity->getNoMilitaryPercentAnger();
}

int CyCity::getWarWearinessPercentAnger() const
{
	return m_pCity->getWarWearinessPercentAnger();
}

int CyCity::getRevIndexPercentAnger() const
{
	return m_pCity->getRevIndexPercentAnger();
}

int CyCity::unhappyLevel(int iExtra) const
{
	return m_pCity->unhappyLevel(iExtra);
}

int CyCity::happyLevel() const
{
	return m_pCity->happyLevel();
}

int CyCity::angryPopulation(int iExtra) const
{
	return m_pCity->angryPopulation(iExtra);
}

int CyCity::totalFreeSpecialists() const
{
	return m_pCity->totalFreeSpecialists();
}

int CyCity::goodHealth() const
{
	return m_pCity->goodHealth();
}

int CyCity::badHealth(bool bNoAngry) const
{
	return m_pCity->badHealth(bNoAngry);
}

int CyCity::healthRate(bool bNoAngry, int iExtra) const
{
	return m_pCity->healthRate(bNoAngry, iExtra);
}

int CyCity::foodConsumption(bool bNoAngry, int iExtra) const
{
	return m_pCity->foodConsumption(bNoAngry, iExtra);
}

int CyCity::foodDifference(bool bBottom) const
{
	return m_pCity->foodDifference(bBottom);
}

int CyCity::growthThreshold() const
{
	return m_pCity->growthThreshold();
}

int CyCity::productionLeft() const
{
	return m_pCity->productionLeft();
}

int64_t CyCity::getHurryGold(int /*HurryTypes*/ iHurry) const
{
	return m_pCity->getHurryGold((HurryTypes)iHurry);
}

int CyCity::hurryPopulation(int /*HurryTypes*/ iHurry) const
{
	return m_pCity->hurryPopulation((HurryTypes)iHurry);
}

int CyCity::hurryProduction(int /*HurryTypes*/ iHurry) const
{
	return m_pCity->hurryProduction((HurryTypes)iHurry);
}

int CyCity::flatHurryAngerLength() const
{
	return m_pCity->flatHurryAngerLength();
}

void CyCity::changeHasBuilding(int /*BuildingTypes*/ iIndex, bool bNewValue)
{
	m_pCity->changeHasBuilding((BuildingTypes) iIndex, bNewValue);
}

int CyCity::hasBuilding(int /*BuildingTypes*/ iIndex) const
{
	return m_pCity->hasBuilding((BuildingTypes) iIndex);
}

bool CyCity::isActiveBuilding(int /*BuildingTypes*/ iIndex) const
{
	return m_pCity->isActiveBuilding((BuildingTypes) iIndex);
}

bool CyCity::isFreeBuilding(int /*BuildingTypes*/ iIndex) const
{
	return m_pCity->isFreeBuilding((BuildingTypes) iIndex);
}

int CyCity::getID() const
{
	return m_pCity->getID();
}

int CyCity::getX() const
{
	return m_pCity->getX();
}

int CyCity::getY() const
{
	return m_pCity->getY();
}

CyPlot* CyCity::plot() const
{
	if (m_pCity->plot())
	{
		return new CyPlot(m_pCity->plot());
	}
	return NULL;
}

bool CyCity::isConnectedTo(const CyCity& kCity) const
{
	return m_pCity->isConnectedTo(kCity.getCity());
}

bool CyCity::isConnectedToCapital(int /*PlayerTypes*/ ePlayer) const
{
	return m_pCity->isConnectedToCapital((PlayerTypes)ePlayer);
}

CyArea* CyCity::area() const
{
	return new CyArea(m_pCity->area());
}

CyArea* CyCity::waterArea() const
{
	CvArea* waterArea = m_pCity->waterArea();
	return waterArea ? new CyArea(waterArea) : NULL;
}

int CyCity::getGameTurnFounded() const
{
	CvGame& GAME = GC.getGame();
	bool bHistoricalCalendar = GAME.isModderGameOption(MODDERGAMEOPTION_USE_HISTORICAL_ACCURATE_CALENDAR);
	return m_pCity->getGameTurnFounded(bHistoricalCalendar);
}

int CyCity::getGameDateFounded() const
{
	CvGame& GAME = GC.getGame();
	bool bHistoricalCalendar = GAME.isModderGameOption(MODDERGAMEOPTION_USE_HISTORICAL_ACCURATE_CALENDAR);
	return m_pCity->getGameDateFounded(bHistoricalCalendar);
}

int CyCity::getGameTurnAcquired() const
{
	return m_pCity->getGameTurnAcquired();
}

int CyCity::getPopulation() const
{
	return m_pCity->getPopulation();
}

void CyCity::setPopulation(int iNewValue)
{
	m_pCity->setPopulation(iNewValue);
}

void CyCity::changePopulation(int iChange)
{
	m_pCity->changePopulation(iChange);
}

int64_t CyCity::getRealPopulation() const
{
	return m_pCity->getRealPopulation();
}

int CyCity::getHighestPopulation() const
{
	return m_pCity->getHighestPopulation();
}

void CyCity::setHighestPopulation(int iNewValue)
{
	m_pCity->setHighestPopulation(iNewValue);
}

int CyCity::getWorkingPopulation() const
{
	return m_pCity->getWorkingPopulation();
}

int CyCity::getSpecialistPopulation() const
{
	return m_pCity->getSpecialistPopulation();
}

int CyCity::getNumGreatPeople() const
{
	return m_pCity->getNumGreatPeople();
}

int CyCity::getBaseGreatPeopleRate() const
{
	return m_pCity->getBaseGreatPeopleRate();
}

int CyCity::getGreatPeopleRate() const
{
	return m_pCity->getGreatPeopleRate();
}

void CyCity::changeBaseGreatPeopleRate(int iChange)
{
	m_pCity->changeBaseGreatPeopleRate(iChange);
}

int CyCity::getGreatPeopleRateModifier() const
{
	return m_pCity->getGreatPeopleRateModifier();
}

int CyCity::getGreatPeopleProgress() const
{
	return m_pCity->getGreatPeopleProgress();
}

void CyCity::changeGreatPeopleProgress(int iChange)
{
	m_pCity->changeGreatPeopleProgress(iChange);
}

int CyCity::getNumWorldWonders() const
{
	return m_pCity->getNumWorldWonders();
}

int CyCity::getNumNationalWonders() const
{
	return m_pCity->getNumNationalWonders();
}

int CyCity::getMaxNumWorldWonders() const
{
	return m_pCity->getMaxNumWorldWonders();
}

int CyCity::getMaxNumNationalWonders() const
{
	return m_pCity->getMaxNumNationalWonders();
}

int CyCity::getNumBuildings() const
{
	return m_pCity->getNumBuildings();
}

bool CyCity::isGovernmentCenter() const
{
	return m_pCity->isGovernmentCenter();
}

int CyCity::getMaintenance() const
{
	return m_pCity->getMaintenance();
}

int CyCity::getMaintenanceTimes100() const
{
	return m_pCity->getMaintenanceTimes100();
}

int CyCity::calculateDistanceMaintenance() const
{
	return m_pCity->calculateDistanceMaintenance();
}

int CyCity::calculateNumCitiesMaintenance()	const
{
	return m_pCity->calculateNumCitiesMaintenance();
}

int CyCity::calculateColonyMaintenanceTimes100() const
{
	return m_pCity->calculateColonyMaintenanceTimes100();
}

int CyCity::getMaintenanceModifier() const
{
	return m_pCity->getMaintenanceModifier();
}

int CyCity::getWarWearinessModifier() const
{
	return m_pCity->getWarWearinessModifier();
}

int CyCity::getEspionageHealthCounter() const
{
	return m_pCity->getEspionageHealthCounter();
}

void CyCity::changeEspionageHealthCounter(int iChange)
{
	m_pCity->changeEspionageHealthCounter(iChange);
}

int CyCity::getEspionageHappinessCounter() const
{
	return m_pCity->getEspionageHappinessCounter();
}

void CyCity::changeEspionageHappinessCounter(int iChange)
{
	m_pCity->changeEspionageHappinessCounter(iChange);
}

int CyCity::getBuildingHealth(int /*BuildingTypes*/ eBuilding) const
{
	return m_pCity->getBuildingHealth((BuildingTypes)eBuilding);
}

int CyCity::getMilitaryHappinessUnits() const
{
	return m_pCity->getMilitaryHappinessUnits();
}

int CyCity::getBuildingHappiness(int /*BuildingTypes*/ eBuilding) const
{
	return m_pCity->getBuildingHappiness((BuildingTypes)eBuilding);
}

int CyCity::getReligionHappiness(int /*ReligionTypes*/ eReligion) const
{
	return m_pCity->getReligionHappiness((ReligionTypes)eReligion);
}

int CyCity::getExtraHappiness() const
{
	return m_pCity->getExtraHappiness();
}

void CyCity::changeExtraHappiness(int iChange)
{
	m_pCity->changeExtraHappiness(iChange);
}

int CyCity::getExtraHealth() const
{
	return m_pCity->getExtraHealth();
}

void CyCity::changeExtraHealth(int iChange)
{
	m_pCity->changeExtraHealth(iChange);
}

int CyCity::getHurryAngerTimer() const
{
	return m_pCity->getHurryAngerTimer();
}

void CyCity::changeHurryAngerTimer(int iChange)
{
	m_pCity->changeHurryAngerTimer(iChange);
}

int CyCity::getRevRequestAngerTimer() const
{
	return m_pCity->getRevRequestAngerTimer();
}

void CyCity::changeRevRequestAngerTimer(int iChange)
{
	m_pCity->changeRevRequestAngerTimer(iChange);
}

void CyCity::changeRevSuccessTimer(int iChange)
{
	m_pCity->changeRevSuccessTimer(iChange);
}

int CyCity::getConscriptAngerTimer() const
{
	return m_pCity->getConscriptAngerTimer();
}

void CyCity::changeConscriptAngerTimer(int iChange)
{
	m_pCity->changeConscriptAngerTimer(iChange);
}

int CyCity::getDefyResolutionAngerTimer() const
{
	return m_pCity->getDefyResolutionAngerTimer();
}

void CyCity::changeDefyResolutionAngerTimer(int iChange)
{
	m_pCity->changeDefyResolutionAngerTimer(iChange);
}

int CyCity::flatDefyResolutionAngerLength() const
{
	return m_pCity->flatDefyResolutionAngerLength();
}

int CyCity::getHappinessTimer() const
{
	return m_pCity->getHappinessTimer();
}

void CyCity::changeHappinessTimer(int iChange)
{
	m_pCity->changeHappinessTimer(iChange);
}

bool CyCity::isNoUnhappiness() const
{
	return m_pCity->isNoUnhappiness();
}

bool CyCity::isBuildingOnlyHealthy() const
{
	return m_pCity->isBuildingOnlyHealthy();
}

int CyCity::getFood() const
{
	return m_pCity->getFood();
}

void CyCity::setFood(int iNewValue)
{
	m_pCity->setFood(iNewValue);
}

void CyCity::changeFood(int iChange)
{
	m_pCity->changeFood(iChange);
}

int CyCity::getFoodKept() const
{
	return m_pCity->getFoodKept();
}

int CyCity::getMaxProductionOverflow() const
{
	return m_pCity->getMaxProductionOverflow();
}

int CyCity::getOverflowProduction() const
{
	return m_pCity->getOverflowProduction();
}

void CyCity::setOverflowProduction(int iNewValue)
{
	m_pCity->setOverflowProduction(iNewValue);
}

int CyCity::getFeatureProduction() const
{
	return m_pCity->getFeatureProduction();
}

void CyCity::setFeatureProduction(int iNewValue)
{
	m_pCity->setFeatureProduction(iNewValue);
}

int CyCity::getMilitaryProductionModifier() const
{
	return m_pCity->getMilitaryProductionModifier();
}

int CyCity::getSpaceProductionModifier() const
{
	return m_pCity->getSpaceProductionModifier();
}

int CyCity::getExtraTradeRoutes() const
{
	return m_pCity->getExtraTradeRoutes();
}

int CyCity::getMaxTradeRoutes() const
{
	return m_pCity->getMaxTradeRoutes();
}

void CyCity::changeExtraTradeRoutes(int iChange)
{
	m_pCity->changeExtraTradeRoutes(iChange);
}

int CyCity::getTradeRouteModifier() const
{
	return m_pCity->getTradeRouteModifier();
}

int CyCity::getForeignTradeRouteModifier() const
{
	return m_pCity->getForeignTradeRouteModifier();
}

int CyCity::getBuildingDefense() const
{
	return m_pCity->getBuildingDefense();
}

int CyCity::getFreeExperience() const
{
	return m_pCity->getFreeExperience();
}

int CyCity::getMaxAirlift() const
{
	return m_pCity->getMaxAirlift();
}

int CyCity::getAirModifier() const
{
	return m_pCity->getAirModifier();
}

int CyCity::getNukeModifier() const
{
	return m_pCity->getNukeModifier();
}

bool CyCity::isPower() const
{
	return m_pCity->isPower();
}

int CyCity::getDefenseDamage() const
{
	return m_pCity->getDefenseDamage();
}

void CyCity::changeDefenseDamage(int iChange)
{
	m_pCity->changeDefenseDamage(iChange);
}

int CyCity::getNaturalDefense() const
{
	return m_pCity->getNaturalDefense();
}

int CyCity::getTotalDefense(bool bIgnoreBuilding) const
{
	return m_pCity->getTotalDefense(bIgnoreBuilding);
}

int CyCity::getDefenseModifier(bool bIgnoreBuilding) const
{
	return m_pCity->getDefenseModifier(bIgnoreBuilding);
}

int CyCity::getOccupationTimer() const
{
	return m_pCity->getOccupationTimer();
}

bool CyCity::isOccupation() const
{
	return m_pCity->isOccupation();
}

void CyCity::setOccupationTimer(int iNewValue)
{
	m_pCity->setOccupationTimer(iNewValue);
}

void CyCity::changeOccupationTimer(int iChange)
{
	m_pCity->changeOccupationTimer(iChange);
}

bool CyCity::isNeverLost() const
{
	return m_pCity->isNeverLost();
}

void CyCity::setNeverLost(bool bNewValue)
{
	m_pCity->setNeverLost(bNewValue);
}

bool CyCity::isBombarded() const
{
	return m_pCity->isBombarded();
}

void CyCity::setBombarded(bool bNewValue)
{
	m_pCity->setBombarded(bNewValue);
}

bool CyCity::isDrafted() const
{
	return m_pCity->isDrafted();
}

void CyCity::setDrafted(bool bNewValue)
{
	m_pCity->setDrafted(bNewValue);
}

bool CyCity::isAirliftTargeted() const
{
	return m_pCity->isAirliftTargeted();
}

void CyCity::setAirliftTargeted(bool bNewValue)
{
	m_pCity->setAirliftTargeted(bNewValue);
}

bool CyCity::isCitizensAutomated() const
{
	return m_pCity->isCitizensAutomated();
}

void CyCity::setCitizensAutomated(bool bNewValue)
{
	m_pCity->setCitizensAutomated(bNewValue);
}

bool CyCity::isProductionAutomated() const
{
	return m_pCity->isProductionAutomated();
}

void CyCity::setProductionAutomated(bool bNewValue)
{
	m_pCity->setProductionAutomated(bNewValue);
}

bool CyCity::isWallOverride() const
{
	return m_pCity->isWallOverride();
}

void CyCity::setWallOverride(bool bOverride)
{
	m_pCity->setWallOverride(bOverride);
}

bool CyCity::isPlundered() const
{
	return m_pCity->isPlundered();
}

void CyCity::setPlundered(bool bNewValue)
{
	m_pCity->setPlundered(bNewValue);
}

int /*PlayerTypes*/ CyCity::getOwner() const
{
	return m_pCity->getOwner();
}

int /*TeamTypes*/ CyCity::getTeam() const
{
	return m_pCity->getTeam();
}

int /*PlayerTypes*/ CyCity::getPreviousOwner() const
{
	return m_pCity->getPreviousOwner();
}

int /*PlayerTypes*/ CyCity::getOriginalOwner() const
{
	return m_pCity->getOriginalOwner();
}

void CyCity::setOriginalOwner(int iPlayer)
{
	return m_pCity->setOriginalOwner((PlayerTypes)iPlayer);
}

int /*CultureLevelTypes*/ CyCity::getCultureLevel() const
{
	return m_pCity->getCultureLevel();
}

int CyCity::getCultureThreshold() const
{
	return m_pCity->getCultureThreshold();
}

int CyCity::getPlotYield(int /*YieldTypes*/ eIndex) const
{
	return m_pCity->getPlotYield((YieldTypes)eIndex);
}

int CyCity::getBaseYieldRateModifier(int /*YieldTypes*/ eIndex, int iExtra) const
{
	return m_pCity->getBaseYieldRateModifier((YieldTypes)eIndex, iExtra);
}

int CyCity::getYieldRate(int /*YieldTypes*/ eIndex) const
{
	return m_pCity->getYieldRate((YieldTypes)eIndex);
}

int CyCity::getYieldRateModifier(int /*YieldTypes*/ eIndex) const
{
	return m_pCity->getYieldRateModifier((YieldTypes)eIndex);
}

int CyCity::getTradeYield(int /*YieldTypes*/ eIndex) const
{
	return m_pCity->getTradeYield((YieldTypes)eIndex);
}

// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
int CyCity::calculateTradeProfitTimes100(const CyCity& kCity) const
{
	return m_pCity->calculateTradeProfitTimes100(kCity.getCity());
}
#endif
// BUG - Fractional Trade Routes - end

int CyCity::calculateTradeProfit(const CyCity& kCity) const
{
	return m_pCity->calculateTradeProfit(kCity.getCity());
}

int CyCity::calculateTradeYield(int /*YieldTypes*/ eIndex, int iTradeProfit) const
{
	return m_pCity->calculateTradeYield((YieldTypes)eIndex, iTradeProfit);
}

int CyCity::getCommerceRate(int /*CommerceTypes*/ eIndex) const
{
	return m_pCity->getCommerceRate((CommerceTypes)eIndex);
}

int CyCity::getCommerceRateTimes100(int /*CommerceTypes*/ eIndex) const
{
	return m_pCity->getCommerceRateTimes100((CommerceTypes)eIndex);
}

int CyCity::getBaseCommerceRate(int /*CommerceTypes*/ eIndex) const
{
	return m_pCity->getBaseCommerceRate((CommerceTypes)eIndex);
}

int CyCity::getProductionToCommerceModifier(int /*CommerceTypes*/ eIndex) const
{
	return m_pCity->getProductionToCommerceModifier((CommerceTypes)eIndex);
}

int CyCity::getBuildingCommerceByBuilding(int /*CommerceTypes*/ eIndex, int /*BuildingTypes*/ iBuilding) const
{
	return m_pCity->getBuildingCommerceByBuilding((CommerceTypes)eIndex, (BuildingTypes) iBuilding);
}

int CyCity::getSpecialistCommerce(int /*CommerceTypes*/ eIndex) const
{
	return m_pCity->getSpecialistCommerce((CommerceTypes)eIndex);
}

void CyCity::changeSpecialistCommerce(int /*CommerceTypes*/ eIndex, int iChange)
{
	m_pCity->changeSpecialistCommerceTimes100((CommerceTypes)eIndex, 100*iChange);
}

int CyCity::getReligionCommerceByReligion(int /*CommerceTypes*/ eIndex, int /*ReligionTypes*/ eReligion) const
{
	return m_pCity->getReligionCommerceByReligion((CommerceTypes)eIndex, (ReligionTypes)eReligion);
}

int CyCity::getCorporationCommerce(int /*CommerceTypes*/ eIndex) const
{
	return m_pCity->getCorporationCommerce((CommerceTypes)eIndex);
}

int CyCity::getCorporationCommerceByCorporation(int /*CommerceTypes*/ eIndex, int /*CorporationTypes*/ eCorporation) const
{
	return m_pCity->getCorporationCommerceByCorporation((CommerceTypes)eIndex, (CorporationTypes)eCorporation);
}

int CyCity::getCorporationYield(int /*YieldTypes*/ eIndex) const
{
	return m_pCity->getCorporationYield((YieldTypes)eIndex);
}

int CyCity::getCorporationYieldByCorporation(int /*YieldTypes*/ eIndex, int /*CorporationTypes*/ eCorporation) const
{
	return m_pCity->getCorporationYieldByCorporation((YieldTypes)eIndex, (CorporationTypes)eCorporation);
}

int CyCity::getCommerceRateModifier(int /*CommerceTypes*/ eIndex) const
{
	return m_pCity->getTotalCommerceRateModifier((CommerceTypes)eIndex);
}

int CyCity::getCommerceHappinessByType(int /*CommerceTypes*/ eIndex) const
{
	return m_pCity->getCommerceHappinessByType((CommerceTypes) eIndex);
}

int CyCity::getDomainFreeExperience(int /*DomainTypes*/ eIndex) const
{
	return m_pCity->getDomainFreeExperience((DomainTypes)eIndex);
}

int CyCity::getDomainProductionModifier(int /*DomainTypes*/ eIndex) const
{
	return m_pCity->getDomainProductionModifier((DomainTypes)eIndex);
}

int CyCity::getCulture(int /*PlayerTypes*/ eIndex) const
{
	return m_pCity->getCulture((PlayerTypes)eIndex);
}

int CyCity::getCultureTimes100(int /*PlayerTypes*/ eIndex) const
{
	return m_pCity->getCultureTimes100((PlayerTypes)eIndex);
}

int CyCity::countTotalCultureTimes100() const
{
	return m_pCity->countTotalCultureTimes100();
}

PlayerTypes CyCity::findHighestCulture() const
{
	return m_pCity->findHighestCulture();
}

int CyCity::calculateCulturePercent(int /*PlayerTypes*/ eIndex) const
{
	return m_pCity->calculateCulturePercent((PlayerTypes)eIndex);
}

void CyCity::setCulture(int /*PlayerTypes*/ eIndex, int iNewValue, bool bPlots)
{
	m_pCity->setCulture((PlayerTypes)eIndex, iNewValue, bPlots, true);
}

void CyCity::setCultureTimes100(int /*PlayerTypes*/ eIndex, int iNewValue, bool bPlots)
{
	m_pCity->setCultureTimes100((PlayerTypes)eIndex, iNewValue, bPlots, true);
}

void CyCity::changeCulture(int /*PlayerTypes*/ eIndex, int iChange, bool bPlots)
{
	m_pCity->changeCulture((PlayerTypes)eIndex, iChange, bPlots, true);
}

int CyCity::getNumRevolts(int playerIdx) const
{
	return m_pCity->getNumRevolts((PlayerTypes)playerIdx);
}

void CyCity::changeNumRevolts(int playerIdx, int iChange)
{
	m_pCity->changeNumRevolts((PlayerTypes)playerIdx, iChange);
}

bool CyCity::isRevealed(int /*TeamTypes*/ eIndex, bool bDebug) const
{
	return m_pCity->isRevealed((TeamTypes)eIndex, bDebug);
}

void CyCity::setRevealed(int /*TeamTypes*/ eIndex, bool bNewValue)
{
	m_pCity->setRevealed((TeamTypes)eIndex, bNewValue);
}

bool CyCity::getEspionageVisibility(int /*TeamTypes*/ eIndex) const
{
	return m_pCity->getEspionageVisibility((TeamTypes)eIndex);
}

std::wstring CyCity::getName() const
{
	return m_pCity->getName();
}

std::wstring CyCity::getNameForm(int iForm) const
{
	return m_pCity->getName((uint)iForm);
}

std::wstring CyCity::getNameKey() const
{
	return m_pCity->getNameKey();
}

void CyCity::setName(std::wstring szNewValue, bool bFound)
{
	m_pCity->setName((CvWString)szNewValue, bFound);
}

int CyCity::getFreeBonus(int /*BonusTypes*/ eIndex) const
{
	return m_pCity->getFreeBonus((BonusTypes)eIndex);
}

void CyCity::changeFreeBonus(int /*BonusTypes*/ eIndex, int iChange)
{
	m_pCity->changeFreeBonus((BonusTypes)eIndex, iChange);
}

int CyCity::getNumBonuses(int /*BonusTypes*/ iBonus) const
{
	return m_pCity->getNumBonuses((BonusTypes) iBonus);
}

bool CyCity::hasBonus(int /*BonusTypes*/ iBonus) const
{
	return m_pCity->hasBonus((BonusTypes) iBonus);
}

int CyCity::getProgressOnBuilding(int /*BuildingTypes*/ iIndex) const
{
	return m_pCity->getProgressOnBuilding((BuildingTypes) iIndex);
}

void CyCity::setProgressOnBuilding(int /*BuildingTypes*/ iIndex, int iNewValue)
{
	m_pCity->setProgressOnBuilding((BuildingTypes) iIndex, std::max(0, iNewValue));
}

int CyCity::getDelayOnBuilding(int /*BuildingTypes*/ eIndex) const
{
	return m_pCity->getDelayOnBuilding((BuildingTypes)eIndex);
}

bool CyCity::isBuildingProductionDecay(int /*BuildingTypes*/ eIndex) const
{
	return m_pCity->isBuildingProductionDecay((BuildingTypes)eIndex);
}

int CyCity::getBuildingProductionDecayTurns(int /*BuildingTypes*/ eIndex) const
{
	return m_pCity->getBuildingProductionDecayTurns((BuildingTypes)eIndex);
}

int CyCity::getBuildingOriginalOwner(int /*BuildingTypes*/ iType) const
{
	return m_pCity->getBuildingData((BuildingTypes) iType).eBuiltBy;
}

int CyCity::getBuildingOriginalTime(int /*BuildingTypes*/ iType) const
{
	return m_pCity->getBuildingData((BuildingTypes) iType).iTimeBuilt;
}

int CyCity::getProgressOnUnit(int iIndex) const
{
	return m_pCity->getProgressOnUnit((UnitTypes) iIndex);
}

void CyCity::setProgressOnUnit(int iIndex, int iNewValue)
{
	m_pCity->setProgressOnUnit((UnitTypes)iIndex, iNewValue);
}

int CyCity::getDelayOnUnit(int /*UnitTypes*/ eIndex) const
{
	return m_pCity->getDelayOnUnit((UnitTypes)eIndex);
}

bool CyCity::isUnitProductionDecay(int /*UnitTypes*/ eIndex) const
{
	return m_pCity->isUnitProductionDecay((UnitTypes)eIndex);
}

int CyCity::getUnitProductionDecayTurns(int /*UnitTypes*/ eIndex) const
{
	return m_pCity->getUnitProductionDecayTurns((UnitTypes)eIndex);
}

int CyCity::getProjectProduction(int /*ProjectTypes*/ iIndex) const
{
	return m_pCity->getProjectProduction((ProjectTypes) iIndex);
}

int CyCity::getGreatPeopleUnitProgress(int /*UnitTypes*/ iIndex) const
{
	return m_pCity->getGreatPeopleUnitProgress((UnitTypes) iIndex);
}

void CyCity::setGreatPeopleUnitProgress(int /*UnitTypes*/ iIndex, int iNewValue)
{
	m_pCity->setGreatPeopleUnitProgress((UnitTypes) iIndex, iNewValue);
}

void CyCity::changeGreatPeopleUnitProgress(int /*UnitTypes*/ iIndex, int iChange)
{
	m_pCity->changeGreatPeopleUnitProgress((UnitTypes) iIndex, iChange);
}

int CyCity::getSpecialistCount(int /*SpecialistTypes*/ eIndex) const
{
	return m_pCity->getSpecialistCount((SpecialistTypes)eIndex);
}

int CyCity::getMaxSpecialistCount(int /*SpecialistTypes*/ eIndex) const
{
	return m_pCity->getMaxSpecialistCount((SpecialistTypes)eIndex);
}

bool CyCity::isSpecialistValid(int /*SpecialistTypes*/ eIndex, int iExtra) const
{
	return m_pCity->isSpecialistValid((SpecialistTypes) eIndex, iExtra);
}

int CyCity::getForceSpecialistCount(int /*SpecialistTypes*/ eIndex) const
{
	return m_pCity->getForceSpecialistCount((SpecialistTypes)eIndex);
}

int CyCity::getImprovementFreeSpecialists(int /*ImprovementTypes*/ iIndex) const
{
	return m_pCity->getImprovementFreeSpecialists((ImprovementTypes) iIndex);
}

int CyCity::getReligionInfluence(int /*ReligionTypes*/ iIndex) const
{
	return m_pCity->getReligionInfluence((ReligionTypes) iIndex);
}

void CyCity::setForceSpecialistCount(int /*SpecialistTypes*/ eIndex, int iNewValue)
{
	m_pCity->setForceSpecialistCount((SpecialistTypes)eIndex, iNewValue);
}

int CyCity::getFreeSpecialistCount(int /*SpecialistTypes*/ eIndex) const
{
	return m_pCity->getFreeSpecialistCount((SpecialistTypes)eIndex);
}

void CyCity::changeFreeSpecialistCount(int /*SpecialistTypes*/ eIndex, int iChange)
{
	m_pCity->changeFreeSpecialistCount((SpecialistTypes)eIndex, iChange, true);
}

int CyCity::getAddedFreeSpecialistCount(int /*SpecialistTypes*/ eIndex) const
{
	return m_pCity->getAddedFreeSpecialistCount((SpecialistTypes)eIndex);
}

void CyCity::changeImprovementFreeSpecialists(int /*ImprovementTypes*/ iIndex, int iChange)
{
	m_pCity->changeImprovementFreeSpecialists((ImprovementTypes) iIndex, iChange);
}

void CyCity::changeReligionInfluence(int /*ReligionTypes*/ iIndex, int iChange)
{
	m_pCity->changeReligionInfluence((ReligionTypes) iIndex, iChange);
}

int CyCity::getStateReligionHappiness(int /*ReligionTypes*/ eIndex) const
{
	return m_pCity->getStateReligionHappiness((ReligionTypes) eIndex);
}

void CyCity::changeStateReligionHappiness(int /*ReligionTypes*/ eIndex, int iChange)
{
	m_pCity->changeStateReligionHappiness((ReligionTypes) eIndex, iChange);
}

int CyCity::getSpecialistFreeExperience() const
{
	return m_pCity->getSpecialistFreeExperience();
}

int CyCity::getEspionageDefenseModifier() const
{
	return m_pCity->getEspionageDefenseModifier();
}

bool CyCity::isWorkingPlot(const CyPlot& kPlot) const
{
	return m_pCity->isWorkingPlot(kPlot.getPlot());
}

bool CyCity::isHasReligion(int /*ReligionTypes*/ iIndex) const
{
	return m_pCity->isHasReligion((ReligionTypes) iIndex);
}

void CyCity::setHasReligion(int /*ReligionTypes*/ iIndex, bool bNewValue, bool bAnnounce, bool bArrows)
{
	m_pCity->setHasReligion((ReligionTypes) iIndex, bNewValue, bAnnounce, bArrows);
}

bool CyCity::isHasCorporation(int /*CorporationTypes*/ iIndex) const
{
	return m_pCity->isHasCorporation((CorporationTypes) iIndex);
}

void CyCity::setHasCorporation(int /*CorporationTypes*/ iIndex, bool bNewValue, bool bAnnounce, bool bArrows)
{
	m_pCity->setHasCorporation((CorporationTypes) iIndex, bNewValue, bAnnounce, bArrows);
}

bool CyCity::isActiveCorporation(int /*CorporationTypes*/ eCorporation) const
{
	return m_pCity->isActiveCorporation((CorporationTypes) eCorporation);
}

CyCity* CyCity::getTradeCity(int iIndex) const
{
	CvCity* city = m_pCity->getTradeCity(iIndex);
	return city ? new CyCity(city) : NULL;
}

int CyCity::getTradeRoutes() const
{
	return m_pCity->getTradeRoutes();
}

void CyCity::clearOrderQueue()
{
	m_pCity->clearOrderQueue();
}

void CyCity::pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bForce)
{
	m_pCity->pushOrder(eOrder, iData1, iData2, bSave, bPop, bAppend, bForce);
}

void CyCity::popOrder(int iNum, bool bFinish, bool bChoose)
{
	m_pCity->popOrder(iNum, bFinish, bChoose);
}

int CyCity::getOrderQueueLength() const
{
	return m_pCity->getOrderQueueLength();
}

OrderData CyCity::getOrderFromQueue(int iIndex) const
{
	return m_pCity->getOrderAt(iIndex);
}

bool CyCity::AI_isEmphasizeSpecialist(int /*SpecialistTypes*/ iIndex) const
{
	return m_pCity->AI_isEmphasizeSpecialist((SpecialistTypes)iIndex);
}

bool CyCity::AI_isEmphasize(int iEmphasizeType) const
{
	return m_pCity->AI_isEmphasize((EmphasizeTypes)iEmphasizeType);
}

int CyCity::AI_countBestBuilds(const CyArea& kArea) const
{
	return m_pCity->AI_countBestBuilds(const_cast<CvArea*>(kArea.getArea()));
}

int CyCity::AI_cityValue() const
{
	return m_pCity->AI_cityValue();
}

std::string CyCity::getScriptData() const
{
	return m_pCity->getScriptData();
}

void CyCity::setScriptData(std::string szNewValue)
{
	m_pCity->setScriptData(szNewValue);
}

int CyCity::getBuildingYieldChange(int /*BuildingTypes*/ eBuilding, int /*YieldTypes*/ eYield) const
{
	return m_pCity->getBuildingYieldChange((BuildingTypes)eBuilding, (YieldTypes)eYield);
}

void CyCity::setBuildingYieldChange(int /*BuildingTypes*/ eBuilding, int /*YieldTypes*/ eYield, int iChange)
{
	m_pCity->setBuildingYieldChange((BuildingTypes)eBuilding, (YieldTypes)eYield, iChange);
}

int CyCity::getBuildingCommerceChange(int /*BuildingTypes*/ eBuilding, int /*CommerceTypes*/ eCommerce) const
{
	return m_pCity->getBuildingCommerceChange((BuildingTypes)eBuilding, (CommerceTypes)eCommerce);
}

void CyCity::setBuildingCommerceChange(int /*BuildingTypes*/ eBuilding, int /*CommerceTypes*/ eCommerce, int iChange)
{
	m_pCity->setBuildingCommerceChange((BuildingTypes)eBuilding, (CommerceTypes)eCommerce, iChange);
}

int CyCity::getBuildingHappyChange(int /*BuildingTypes*/ eBuilding) const
{
	return m_pCity->getBuildingHappyChange((BuildingTypes)eBuilding);
}

void CyCity::setBuildingHappyChange(int /*BuildingTypes*/ eBuilding, int iChange)
{
	m_pCity->setBuildingHappyChange((BuildingTypes)eBuilding, iChange);
}

int CyCity::getBuildingHealthChange(int /*BuildingTypes*/ eBuilding) const
{
	return m_pCity->getBuildingHealthChange((BuildingTypes)eBuilding);
}

void CyCity::setBuildingHealthChange(int /*BuildingTypes*/ eBuilding, int iChange)
{
	m_pCity->setBuildingHealthChange((BuildingTypes)eBuilding, iChange);
}

int CyCity::getLiberationPlayer(bool bConquest) const
{
	return m_pCity->getLiberationPlayer(bConquest);
}

int CyCity::getBonusCommerceRateModifier(int /*CommerceTypes*/ eIndex) const
{
	return m_pCity->getBonusCommerceRateModifier((CommerceTypes)eIndex);
}

int CyCity::getArea() const
{
	return m_pCity->getArea();
}

bool CyCity::isWeLoveTheKingDay() const
{
	return m_pCity->isWeLoveTheKingDay();
}

void CyCity::setWeLoveTheKingDay(bool bWeLoveTheKingDay)
{
	m_pCity->setWeLoveTheKingDay(bWeLoveTheKingDay);
}

int64_t CyCity::calcCorporateMaintenance() const
{
	return m_pCity->calcCorporateMaintenance();
}

void CyCity::changePowerCount(int iChange)
{
	m_pCity->changePowerCount(iChange);
}

void CyCity::changeEventAnger(int iChange)
{
	m_pCity->changeEventAnger(iChange);
}

int CyCity::getNumPopulationEmployed() const
{
	return m_pCity->getNumPopulationEmployed();
}

int CyCity::getBonusCommercePercentChanges(int eIndex, int eBuilding) const
{
	return m_pCity->getBonusCommercePercentChanges((CommerceTypes)eIndex, (BuildingTypes)eBuilding);
}

int CyCity::getBaseYieldRateFromBuilding100(int iYield, int iBuilding) const
{
	return m_pCity->getBaseYieldRateFromBuilding100((YieldTypes)iYield, (BuildingTypes)iBuilding);
}

bool CyCity::isAutomatedCanBuild(int /*BuildTypes*/ eIndex) const
{
	return m_pCity->isAutomatedCanBuild((BuildTypes)eIndex);
}

void CyCity::setAutomatedCanBuild(int /*BuildTypes*/ eIndex, bool bNewValue)
{
	m_pCity->setAutomatedCanBuild((BuildTypes)eIndex, bNewValue);
}

CvProperties* CyCity::getProperties() const
{
	return m_pCity->getProperties();
}

const CityOutputHistory* CyCity::getCityOutputHistory() const
{
	return m_pCity->getCityOutputHistory();
}

bool CyCity::getBuildingListFilterActive(int eFilter)
{
	return m_pCity->getBuildingListFilterActive((BuildingFilterTypes)eFilter);
}

void CyCity::setBuildingListFilterActive(int eFilter, bool bActive)
{
	m_pCity->setBuildingListFilterActive((BuildingFilterTypes)eFilter, bActive);
}

int CyCity::getBuildingListGrouping()
{
	return m_pCity->getBuildingListGrouping();
}

void CyCity::setBuildingListGrouping(int eGrouping)
{
	m_pCity->setBuildingListGrouping((BuildingGroupingTypes)eGrouping);
}

int CyCity::getBuildingListSorting()
{
	return m_pCity->getBuildingListSorting();
}

void CyCity::setBuildingListSorting(int eSorting)
{
	m_pCity->setBuildingListSorting((BuildingSortTypes)eSorting);
}

int CyCity::getBuildingListGroupNum()
{
	return m_pCity->getBuildingListGroupNum();
}

int CyCity::getBuildingListNumInGroup(int iGroup)
{
	return m_pCity->getBuildingListNumInGroup(iGroup);
}

int CyCity::getBuildingListType(int iGroup, int iPos)
{
	return m_pCity->getBuildingListType(iGroup, iPos);
}

void CyCity::setUnitListInvalid()
{
	m_pCity->setUnitListInvalid();
}

bool CyCity::getUnitListFilterActive(int eFilter)
{
	return m_pCity->getUnitListFilterActive((UnitFilterTypes)eFilter);
}

void CyCity::setUnitListFilterActive(int eFilter, bool bActive)
{
	m_pCity->setUnitListFilterActive((UnitFilterTypes)eFilter, bActive);
}

int CyCity::getUnitListGrouping()
{
	return m_pCity->getUnitListGrouping();
}

void CyCity::setUnitListGrouping(int eGrouping)
{
	m_pCity->setUnitListGrouping((UnitGroupingTypes)eGrouping);
}

int CyCity::getUnitListSorting()
{
	return m_pCity->getUnitListSorting();
}

void CyCity::setUnitListSorting(int eSorting)
{
	m_pCity->setUnitListSorting((UnitSortTypes)eSorting);
}

int CyCity::getUnitListGroupNum()
{
	return m_pCity->getUnitListGroupNum();
}

int CyCity::getUnitListNumInGroup(int iGroup)
{
	return m_pCity->getUnitListNumInGroup(iGroup);
}

int CyCity::getUnitListType(int iGroup, int iPos)
{
	return m_pCity->getUnitListType(iGroup, iPos);
}

bool CyCity::isEventOccured(int eEvent) const
{
	return m_pCity->isEventOccured((EventTypes)eEvent);
}

int CyCity::AI_bestUnit() const
{
	int iDummyValue;
	return m_pCity->AI_bestUnit(iDummyValue, -1, NULL, true, NULL, true, false, NULL);
}

int CyCity::AI_bestUnitAI(UnitAITypes eUnitAITypes) const
{
	int iDummyValue;
	return m_pCity->AI_bestUnitAI(eUnitAITypes, iDummyValue, true, true, &CvUnitSelectionCriteria().IgnoreGrowth(true));
}
