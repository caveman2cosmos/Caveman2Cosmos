//
// Python wrapper class for CvCity
//
#include "CvGameCoreDLL.h"
#include "CvPlayerAI.h"
#include "CyArea.h"
#include "CyCity.h"
#include "CyPlot.h"
#include "CyUnit.h"

CyCity::CyCity() : m_pCity(NULL)
{
}

CyCity::CyCity(CvCity* pCity) : m_pCity(pCity)
{
}

void CyCity::kill()
{
	if (m_pCity)
		m_pCity->kill(true);
}

/************************************************************************************************/
/* REVOLUTION_MOD                         03/29/09                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int CyCity::getRevolutionIndex()
{
	return m_pCity ? m_pCity->getRevolutionIndex() : 0;
}

void CyCity::setRevolutionIndex( int iNewValue )
{
	if( m_pCity )
		m_pCity->setRevolutionIndex(iNewValue);
}

void CyCity::changeRevolutionIndex( int iChange )
{
	if( m_pCity )
		m_pCity->changeRevolutionIndex( iChange );
}

int CyCity::getLocalRevIndex()
{
	if( m_pCity )
		return m_pCity->getLocalRevIndex();
	else
		return 0;
}

void CyCity::setLocalRevIndex( int iNewValue )
{
	if( m_pCity )
		m_pCity->setLocalRevIndex( iNewValue );
}

void CyCity::changeLocalRevIndex( int iChange )
{
	if( m_pCity )
		m_pCity->changeLocalRevIndex( iChange );
}

int CyCity::getRevIndexAverage() const
{
	if( m_pCity )
		return m_pCity->getRevIndexAverage();
	else
		return 0;
}

void CyCity::setRevIndexAverage( int iNewValue )
{
	if( m_pCity )
		m_pCity->setRevIndexAverage( iNewValue );
}

void CyCity::updateRevIndexAverage( )
{
	if( m_pCity )
		m_pCity->updateRevIndexAverage( );
}

int CyCity::getReinforcementCounter()
{
	if( m_pCity )
		return m_pCity->getReinforcementCounter();
	else
		return 0;
}

void CyCity::setReinforcementCounter( int iNewValue )
{
	if( m_pCity )
		m_pCity->setReinforcementCounter(iNewValue);
}

void CyCity::changeReinforcementCounter( int iChange )
{
	if( m_pCity )
		m_pCity->changeReinforcementCounter( iChange );
}

int CyCity::getRevolutionCounter()
{
	if( m_pCity )
		return m_pCity->getRevolutionCounter();
	else
		return 0;
}

void CyCity::setRevolutionCounter( int iNewValue )
{
	if( m_pCity )
		m_pCity->setRevolutionCounter(iNewValue);
}

void CyCity::changeRevolutionCounter( int iChange )
{
	if( m_pCity )
		m_pCity->changeRevolutionCounter( iChange );
}

bool CyCity::isRecentlyAcquired()
{
	if( m_pCity )
		return m_pCity->isRecentlyAcquired();
	else
		return false;
}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

void CyCity::createGreatPeople(int /*UnitTypes*/ eGreatPersonUnit, bool bIncrementThreshold, bool bIncrementExperience)
{
	if (m_pCity)
		m_pCity->createGreatPeople((UnitTypes) eGreatPersonUnit, bIncrementThreshold, bIncrementExperience);
}

void CyCity::doTask(int /*TaskTypes*/ eTask, int iData1, int iData2, bool bOption)
{
	if (m_pCity)
		m_pCity->doTask((TaskTypes)eTask, iData1, iData2, bOption);
}

void CyCity::chooseProduction(int /*UnitTypes*/ eTrainUnit, int /*BuildingTypes*/ eConstructBuilding, int /*ProjectTypes*/ eCreateProject, bool bFinish, bool bFront)
{
	if (m_pCity)
		m_pCity->chooseProduction((UnitTypes) eTrainUnit, (BuildingTypes) eConstructBuilding, (ProjectTypes) eCreateProject, bFinish, bFront);
}

int CyCity::getCityPlotIndex(CyPlot* pPlot)
{
	return m_pCity ? m_pCity->getCityPlotIndex(pPlot->getPlot()) : -1;
}

CyPlot* CyCity::getCityIndexPlot(int iIndex)
{
	return m_pCity ? new CyPlot(m_pCity->getCityIndexPlot(iIndex)) : NULL;
}

bool CyCity::canWork(CyPlot* pPlot)
{
	return m_pCity ? m_pCity->canWork(pPlot ? pPlot->getPlot() : NULL) : false;
}

void CyCity::clearWorkingOverride(int iIndex)
{
	if (m_pCity)
		m_pCity->clearWorkingOverride(iIndex);
}

int CyCity::countNumImprovedPlots()
{
	return m_pCity ? m_pCity->countNumImprovedPlots() : -1;
}

int CyCity::countNumWaterPlots()
{
	return m_pCity ? m_pCity->countNumWaterPlots() : -1;
}

int CyCity::countNumRiverPlots()
{
	return m_pCity ? m_pCity->countNumRiverPlots() : -1;
}

int CyCity::findPopulationRank()
{
	return m_pCity ? m_pCity->findPopulationRank() : -1;
}

int CyCity::findBaseYieldRateRank(int /*YieldTypes*/ eYield)
{
	return m_pCity ? m_pCity->findBaseYieldRateRank((YieldTypes) eYield) : -1;
}

int CyCity::findYieldRateRank(int /*YieldTypes*/ eYield)
{
	return m_pCity ? m_pCity->findYieldRateRank((YieldTypes) eYield) : -1;
}

int CyCity::findCommerceRateRank(int /*CommerceTypes*/ eCommerce)
{
	return m_pCity ? m_pCity->findCommerceRateRank((CommerceTypes) eCommerce) : -1;
}

bool CyCity::isSupersedingUnitAvailable(int /*UnitTypes*/ eUnitType)
{
	return m_pCity ? m_pCity->isSupersedingUnitAvailable((UnitTypes) eUnitType) : false;
}

bool CyCity::isPlotTrainable(int /*UnitTypes*/ eUnit, bool bContinue, bool bTestVisible)
{
	return m_pCity ? m_pCity->isPlotTrainable((UnitTypes)eUnit, bContinue, bTestVisible) : false;
}

int /*UnitTypes*/ CyCity::allUpgradesAvailable(int /*UnitTypes*/ eUnitType, int iUpgradeCount)
{
	return m_pCity ? m_pCity->allUpgradesAvailable((UnitTypes) eUnitType, iUpgradeCount) : -1;
}

bool CyCity::isWorldWondersMaxed()
{
	return m_pCity ? m_pCity->isWorldWondersMaxed() : false;
}

bool CyCity::isTeamWondersMaxed()
{
	return m_pCity ? m_pCity->isTeamWondersMaxed() : false;
}

bool CyCity::isNationalWondersMaxed()
{
	return m_pCity ? m_pCity->isNationalWondersMaxed() : false;
}

bool CyCity::canTrain( int /*UnitTypes*/ eUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreUpgrades )
{
	return m_pCity ? m_pCity->canTrain((UnitTypes)eUnit, bContinue, bTestVisible, bIgnoreCost, bIgnoreUpgrades) : false;
}

bool CyCity::canConstruct( int /*BuildingTypes*/ eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost)
{
	return m_pCity ? m_pCity->canConstruct((BuildingTypes)eBuilding, bContinue, bTestVisible, bIgnoreCost) : false;
}

bool CyCity::canCreate( int /*ProjectTypes*/ eProject, bool bContinue, bool bTestVisible )
{
	return m_pCity ? m_pCity->canCreate((ProjectTypes)eProject, bContinue, bTestVisible) : false;
}

bool CyCity::canMaintain( int /*ProcessTypes*/ eProcess, bool bContinue )
{
	return m_pCity ? m_pCity->canMaintain((ProcessTypes)eProcess, bContinue) : false;
}

bool CyCity::canJoin()
{
	return m_pCity ? m_pCity->canJoin() : false;
}

int CyCity::getFoodTurnsLeft()
{
	return m_pCity ? m_pCity->getFoodTurnsLeft() : 0;
}

bool CyCity::isProduction()
{
	return m_pCity ? m_pCity->isProduction() : false;
}

bool CyCity::isProductionLimited()
{
	return m_pCity ? m_pCity->isProductionLimited() : false;
}

bool CyCity::isProductionUnit()
{
	return m_pCity ? m_pCity->isProductionUnit() : false;
}

bool CyCity::isProductionBuilding()
{
	return m_pCity ? m_pCity->isProductionBuilding() : false;
}

bool CyCity::isProductionProject()															
{
	return m_pCity ? m_pCity->isProductionProject() : false;
}

bool CyCity::isProductionProcess()
{
	return m_pCity ? m_pCity->isProductionProcess() : false;
}

bool CyCity::canContinueProduction(OrderData order)
{
	return m_pCity ? m_pCity->canContinueProduction(order) : false;
}

int CyCity::getProductionExperience(int /*UnitTypes*/ eUnit)
{
	return m_pCity ? m_pCity->getProductionExperience((UnitTypes) eUnit) : -1;
}

void CyCity::addProductionExperience(CyUnit* pUnit, bool bConscript)
{
	if (m_pCity)
		m_pCity->addProductionExperience(pUnit->getUnit(), bConscript);
}

int /*UnitTypes*/ CyCity::getProductionUnit()
{
	return m_pCity ? m_pCity->getProductionUnit() : NO_UNIT;
}

int /*UnitAITypes*/  CyCity::getProductionUnitAI()
{
	return m_pCity ? m_pCity->getProductionUnitAI() : NO_UNIT;
}

int /*BuildingTypes*/ CyCity::getProductionBuilding()
{
	return m_pCity ? m_pCity->getProductionBuilding() : NO_BUILDING;
}

int /*ProjectTypes*/ CyCity::getProductionProject()															
{
	return m_pCity ? m_pCity->getProductionProject() : -1;
}

int /*ProcessTypes*/ CyCity::getProductionProcess()
{
	return m_pCity ? m_pCity->getProductionProcess() : -1;
}

std::wstring CyCity::getProductionName()
{
	return m_pCity ? m_pCity->getProductionName() : L"";
}

int CyCity::getGeneralProductionTurnsLeft()
{
	return m_pCity ? m_pCity->getGeneralProductionTurnsLeft() : -1;
}

std::wstring CyCity::getProductionNameKey()
{
	return m_pCity ? m_pCity->getProductionNameKey() : L"";
}

bool CyCity::isFoodProduction()
{
	return m_pCity ? m_pCity->isFoodProduction() : false;
}

int CyCity::getFirstUnitOrder(int /*UnitTypes*/ eUnit)
{
	return m_pCity ? m_pCity->getFirstUnitOrder((UnitTypes)eUnit) : -1;
}

int CyCity::getFirstBuildingOrder(int /*BuildingTypes*/ eBuilding)
{
	return m_pCity ? m_pCity->getFirstBuildingOrder((BuildingTypes)eBuilding) : -1;
}

int CyCity::getFirstProjectOrder(int /*ProjectTypes*/ eProject)
{
	return m_pCity ? m_pCity->getFirstProjectOrder((ProjectTypes)eProject) : -1;
}

int CyCity::getNumTrainUnitAI(int /*UnitAITypes*/ eUnitAI)
{
	return m_pCity ? m_pCity->getNumTrainUnitAI((UnitAITypes) eUnitAI) : -1;
}

bool CyCity::isUnitFoodProduction(int /*UnitTypes*/ iUnit)
{
	return m_pCity ? m_pCity->isFoodProduction((UnitTypes)iUnit) : false;
}

int CyCity::getProduction()
{
	return m_pCity ? m_pCity->getProduction() : -1;
}

int CyCity::getProductionNeeded()
{
	return m_pCity ? m_pCity->getProductionNeeded() : -1;
}

int CyCity::getProductionTurnsLeft()																
{
	return m_pCity ? m_pCity->getProductionTurnsLeft() : -1;
}

int CyCity::getUnitProductionTurnsLeft(int /*UnitTypes*/ iUnit, int iNum)									
{
	return m_pCity ? m_pCity->getProductionTurnsLeft((UnitTypes) iUnit, iNum) : -1;
}

int CyCity::getBuildingProductionTurnsLeft(int /*BuildingTypes*/ iBuilding, int iNum)
{
	return m_pCity ? m_pCity->getProductionTurnsLeft((BuildingTypes) iBuilding, iNum) : -1;
}

int CyCity::getProjectProductionTurnsLeft(int /*ProjectTypes*/ eProject, int iNum)								
{
	return m_pCity ? m_pCity->getProductionTurnsLeft((ProjectTypes)eProject, iNum) : -1;
}

void CyCity::setProduction(int iNewValue)
{
	if (m_pCity)
		m_pCity->setProduction(iNewValue);
}

void CyCity::changeProduction(int iChange)
{
	if (m_pCity)
		m_pCity->changeProduction(iChange);
}

int CyCity::getProductionModifier()
{
	return m_pCity ? m_pCity->getProductionModifier() : -1;
}

int CyCity::getCurrentProductionDifference(bool bIgnoreFood, bool bOverflow)
{
	return m_pCity ? m_pCity->getCurrentProductionDifference(
		(bIgnoreFood? ProductionCalc::None : ProductionCalc::FoodProduction) | 
		(bOverflow? ProductionCalc::Overflow : ProductionCalc::None)
	) : -1;
}

int CyCity::getProjectProductionModifier(int /*ProjectTypes*/ eProject)
{
	return m_pCity ? m_pCity->getProductionModifier((ProjectTypes)eProject) : -1;
}

int CyCity::getExtraProductionDifference(int iExtra)
{
	return m_pCity ? m_pCity->getExtraProductionDifference(iExtra) : -1;
}

bool CyCity::canHurry(int /*HurryTypes*/ iHurry, bool bTestVisible)
{
	return m_pCity ? m_pCity->canHurry((HurryTypes)iHurry, bTestVisible) : false;
}

void CyCity::hurry(int /*HurryTypes*/ iHurry)
{
	if (m_pCity)
		m_pCity->hurry((HurryTypes)iHurry);
}

int /*UnitTypes*/ CyCity::getConscriptUnit()
{
	return m_pCity ? (int)m_pCity->getConscriptUnit() : -1;
}

int CyCity::getConscriptPopulation()
{
	return m_pCity ? m_pCity->getConscriptPopulation() : -1;
}

int CyCity::conscriptMinCityPopulation()
{
	return m_pCity ? m_pCity->conscriptMinCityPopulation() : -1;
}

int CyCity::flatConscriptAngerLength()
{
	return m_pCity ? m_pCity->flatConscriptAngerLength() : -1;
}

bool CyCity::canConscript()
{
	return m_pCity ? m_pCity->canConscript() : false;
}

void CyCity::conscript()
{
	if (m_pCity)
		m_pCity->conscript();
}

int CyCity::getBonusHealth(int /*BonusTypes*/ iBonus)
{
	return m_pCity ? m_pCity->getBonusHealth((BonusTypes) iBonus) : -1;
}

int CyCity::getBonusHappiness(int /*BonusTypes*/ iBonus)
{
	return m_pCity ? m_pCity->getBonusHappiness((BonusTypes) iBonus) : -1;
}

int CyCity::getBonusPower(int /*BonusTypes*/ eBonus, bool bDirty)										
{
	return m_pCity ? m_pCity->getBonusPower((BonusTypes)eBonus, bDirty) : -1;
}

int CyCity::getBonusYieldRateModifier(int /*YieldTypes*/ eIndex, int /*BonusTypes*/ eBonus)	
{
	return m_pCity ? m_pCity->getBonusYieldRateModifier((YieldTypes)eIndex, (BonusTypes)eBonus) : -1;
}

int /* HandicapTypes */ CyCity::getHandicapType()
{
	return m_pCity ? m_pCity->getHandicapType() : NO_HANDICAP;
}

int /* CivilizationTypes */ CyCity::getCivilizationType()
{
	return m_pCity ? m_pCity->getCivilizationType() : NO_CIVILIZATION;
}

int /* LeaderHeadTypes */ CyCity::getPersonalityType()
{
	return m_pCity ? m_pCity->getPersonalityType() : NO_LEADER;
}

int /* ArtStyleTypes */ CyCity::getArtStyleType()
{
	return m_pCity ? m_pCity->getArtStyleType() : -1;
}

int /* CitySizeTypes */ CyCity::getCitySizeType()
{
	return m_pCity ? m_pCity->getCitySizeType() : -1;
}

bool CyCity::hasTrait(int /*TraitTypes*/ iTrait)
{
	return m_pCity ? m_pCity->hasTrait((TraitTypes) iTrait) : false;
}

bool CyCity::isBarbarian()
{
	return m_pCity ? m_pCity->isBarbarian() : false;
}

bool CyCity::isNPC()
{
	return m_pCity ? m_pCity->isNPC() : false;
}

bool CyCity::isHominid()
{
	return m_pCity ? m_pCity->isHominid() : false;
}

bool CyCity::isHuman()
{
	return m_pCity ? m_pCity->isHuman() : false;
}

bool CyCity::isVisible(int /*TeamTypes*/ eTeam, bool bDebug)
{
	return m_pCity ? m_pCity->isVisible((TeamTypes) eTeam, bDebug) : false;
}

bool CyCity::isCapital()
{
	return m_pCity ? m_pCity->isCapital() : false;
}

bool CyCity::isCoastal(int iMinWaterSize)
{
	return m_pCity ? m_pCity->isCoastal(iMinWaterSize) : false;
}

bool CyCity::isDisorder()
{
	return m_pCity ? m_pCity->isDisorder() : false;
}

bool CyCity::isHolyCityByType(int /*ReligionTypes*/ iIndex)
{
	return m_pCity ? m_pCity->isHolyCity((ReligionTypes) iIndex) : false;
}

bool CyCity::isHolyCity()
{
	return m_pCity ? m_pCity->isHolyCity() : false;
}

bool CyCity::isHeadquartersByType(int /*CorporationTypes*/ iIndex)
{
	return m_pCity ? m_pCity->isHeadquarters((CorporationTypes) iIndex) : false;
}

bool CyCity::isHeadquarters()
{
	return m_pCity ? m_pCity->isHeadquarters() : false;
}

int CyCity::getOvercrowdingPercentAnger(int iExtra)
{
	return m_pCity ? m_pCity->getOvercrowdingPercentAnger(iExtra) : -1;
}

int CyCity::getNoMilitaryPercentAnger()
{
	return m_pCity ? m_pCity->getNoMilitaryPercentAnger() : -1;
}

int CyCity::getCulturePercentAnger()
{
	return m_pCity ? m_pCity->getCulturePercentAnger() : -1;
}

int CyCity::getReligionPercentAnger()
{
	return m_pCity ? m_pCity->getReligionPercentAnger() : -1;
}

int CyCity::getWarWearinessPercentAnger()
{
	return m_pCity ? m_pCity->getWarWearinessPercentAnger() : -1;
}

/************************************************************************************************/
/* REVOLUTION_MOD                         04/26/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int CyCity::getRevRequestPercentAnger()
{
	return m_pCity ? m_pCity->getRevRequestPercentAnger() : -1;
}

int CyCity::getRevIndexPercentAnger()
{
	return m_pCity ? m_pCity->getRevIndexPercentAnger() : -1;
}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

int CyCity::getLargestCityHappiness()
{
	return m_pCity ? m_pCity->getLargestCityHappiness() : -1;
}

int CyCity::unhappyLevel(int iExtra)
{
	return m_pCity ? m_pCity->unhappyLevel(iExtra) : -1;
}

int CyCity::happyLevel()
{
	return m_pCity ? m_pCity->happyLevel() : -1;
}

int CyCity::angryPopulation(int iExtra)
{
	return m_pCity ? m_pCity->angryPopulation(iExtra) : -1;
}

int CyCity::totalFreeSpecialists()
{
	return m_pCity ? m_pCity->totalFreeSpecialists() : -1;
}

int CyCity::extraFreeSpecialists()
{
	return m_pCity ? m_pCity->extraFreeSpecialists() : -1;
}

int CyCity::extraPopulation()
{
	return m_pCity ? m_pCity->extraPopulation() : -1;
}

int CyCity::extraSpecialists()
{
	return m_pCity ? m_pCity->extraSpecialists() : -1;
}

int CyCity::unhealthyPopulation(bool bNoAngry, int iExtra)									
{
	return m_pCity ? m_pCity->unhealthyPopulation(bNoAngry, iExtra) : -1;
}

int CyCity::totalGoodBuildingHealth()																	
{
	return m_pCity ? m_pCity->totalGoodBuildingHealth() : -1;
}

int CyCity::totalBadBuildingHealth()																			
{
	return m_pCity ? m_pCity->totalBadBuildingHealth() : -1;
}

int CyCity::goodHealth()
{
	return m_pCity ? m_pCity->goodHealth() : -1;
}

int CyCity::badHealth(bool bNoAngry)
{
	return m_pCity ? m_pCity->badHealth(bNoAngry) : -1;
}

int CyCity::healthRate(bool bNoAngry, int iExtra)														
{
	return m_pCity ? m_pCity->healthRate(bNoAngry, iExtra) : -1;
}

int CyCity::foodConsumption(bool bNoAngry, int iExtra)											
{
	return m_pCity ? m_pCity->foodConsumption(bNoAngry, iExtra) : -1;
}

int CyCity::foodDifference(bool bBottom)
{
	return m_pCity ? m_pCity->foodDifference(bBottom) : -1;
}

int CyCity::growthThreshold()
{
	return m_pCity ? m_pCity->growthThreshold() : -1;
}

int CyCity::productionLeft()
{
	return m_pCity ? m_pCity->productionLeft() : -1;
}

int CyCity::hurryCost(bool bExtra)
{
	return m_pCity ? m_pCity->hurryCost(bExtra) : -1;
}

int CyCity::hurryGold(int /*HurryTypes*/ iHurry)
{
	return m_pCity ? m_pCity->hurryGold((HurryTypes)iHurry) : -1;
}

int CyCity::hurryPopulation(int /*HurryTypes*/ iHurry)
{
	return m_pCity ? m_pCity->hurryPopulation((HurryTypes)iHurry) : -1;
}

int CyCity::hurryProduction(int /*HurryTypes*/ iHurry)
{
	return m_pCity ? m_pCity->hurryProduction((HurryTypes)iHurry) : -1;
}

int CyCity::flatHurryAngerLength()
{
	return m_pCity ? m_pCity->flatHurryAngerLength() : -1;
}

int CyCity::hurryAngerLength(int /*HurryTypes*/ iHurry)
{
	return m_pCity ? m_pCity->hurryAngerLength((HurryTypes)iHurry) : -1;
}

int CyCity::maxHurryPopulation()
{
	return m_pCity ? m_pCity->maxHurryPopulation() : -1;
}

int CyCity::cultureDistance(int iDX, int iDY)
{
	return m_pCity ? m_pCity->cultureDistance(iDX, iDY) : -1;
}

int CyCity::cultureStrength(int /*PlayerTypes*/ ePlayer)
{
	int iOriginal = 0;
	return m_pCity ? m_pCity->cultureStrength((PlayerTypes)ePlayer, iOriginal) : -1;
}

int CyCity::cultureGarrison(int /*PlayerTypes*/ ePlayer)
{
	return m_pCity ? m_pCity->cultureGarrison((PlayerTypes)ePlayer) : -1;
}

int CyCity::getNumBuilding(int /*BuildingTypes*/ iIndex)
{
	return m_pCity ? (iIndex != -1 ? m_pCity->getNumBuilding((BuildingTypes) iIndex) : 0) : -1;
}

bool CyCity::isHasBuilding(int /*BuildingTypes*/ iIndex)
{
	if (m_pCity)
	{
		if (m_pCity->getNumBuilding((BuildingTypes) iIndex) > 0)
		{
			return true;
		}
	}
	return false;
}

int CyCity::getNumActiveBuilding(int /*BuildingTypes*/ iIndex)
{
	return m_pCity ? m_pCity->getNumActiveBuilding((BuildingTypes) iIndex) : -1;
}

int CyCity::getID()
{
	return m_pCity ? m_pCity->getID() : -1;
}

int CyCity::getX()
{
	return m_pCity ? m_pCity->getX() : -1;
}

int CyCity::getY()
{
	return m_pCity ? m_pCity->getY() : -1;
}

bool CyCity::at(int iX, int iY)
{
	return m_pCity ? m_pCity->at(iX, iY) : false;
}

bool CyCity::atPlot(CyPlot* pPlot)
{
	return m_pCity ? m_pCity->at(pPlot->getPlot()) : false;
}

CyPlot* CyCity::plot()
{
	return	m_pCity ? new CyPlot(m_pCity->plot()) : NULL;
}

bool CyCity::isConnectedTo(CyCity* pCity)
{
	return m_pCity ? m_pCity->isConnectedTo(pCity->getCity()) : false;
}

bool CyCity::isConnectedToCapital(int /*PlayerTypes*/ ePlayer)
{
	return m_pCity ? m_pCity->isConnectedToCapital((PlayerTypes)ePlayer) : false;
}

CyArea* CyCity::area()
{
	return	m_pCity ? new CyArea(m_pCity->area()) : NULL;
}

CyArea* CyCity::waterArea()
{
	return	m_pCity ? new CyArea(m_pCity->waterArea()) : NULL;
}

CyPlot* CyCity::getRallyPlot()
{
	return	m_pCity ? new CyPlot(m_pCity->getRallyPlot()) : NULL;
}

int CyCity::getGameTurnFounded()
{
	return m_pCity ? m_pCity->getGameTurnFounded() : -1;
}

int CyCity::getGameTurnAcquired()
{
	return m_pCity ? m_pCity->getGameTurnAcquired() : -1;
}

int CyCity::getPopulation()
{
	return m_pCity ? m_pCity->getPopulation() : -1;
}

void CyCity::setPopulation(int iNewValue)
{
	if (m_pCity)
		m_pCity->setPopulation(iNewValue);
}

void CyCity::changePopulation(int iChange)
{
	if (m_pCity)
		m_pCity->changePopulation(iChange);
}

long CyCity::getRealPopulation()
{
	return m_pCity ? m_pCity->getRealPopulation() : -1;
}

int CyCity::getHighestPopulation() 
{
	return m_pCity ? m_pCity->getHighestPopulation() : -1;
}

void CyCity::setHighestPopulation(int iNewValue) 
{
	if (m_pCity)
		m_pCity->setHighestPopulation(iNewValue);
}

int CyCity::getWorkingPopulation()
{
	return m_pCity ? m_pCity->getWorkingPopulation() : -1;
}

int CyCity::getSpecialistPopulation()
{
	return m_pCity ? m_pCity->getSpecialistPopulation() : -1;
}

int CyCity::getNumGreatPeople()
{
	return m_pCity ? m_pCity->getNumGreatPeople() : -1;
}

int CyCity::getBaseGreatPeopleRate()
{
	return m_pCity ? m_pCity->getBaseGreatPeopleRate() : -1;
}

int CyCity::getGreatPeopleRate()
{
	return m_pCity ? m_pCity->getGreatPeopleRate() : -1;
}

int CyCity::getTotalGreatPeopleRateModifier()
{
	return m_pCity ? m_pCity->getTotalGreatPeopleRateModifier() : -1;
}

void CyCity::changeBaseGreatPeopleRate(int iChange)
{
	if (m_pCity)
		m_pCity->changeBaseGreatPeopleRate(iChange);
}
int CyCity::getGreatPeopleRateModifier()
{
	return m_pCity ? m_pCity->getGreatPeopleRateModifier() : -1;
}

// BUG - Building Additional Great People - start
int CyCity::getAdditionalGreatPeopleRateByBuilding(int /*BuildingTypes*/ iBuilding)
{
	return m_pCity ? m_pCity->getAdditionalGreatPeopleRateByBuilding((BuildingTypes)iBuilding) : -1;
}

int CyCity::getAdditionalBaseGreatPeopleRateByBuilding(int /*BuildingTypes*/ iBuilding)
{
	return m_pCity ? m_pCity->getAdditionalBaseGreatPeopleRateByBuilding((BuildingTypes)iBuilding) : -1;
}

int CyCity::getAdditionalGreatPeopleRateModifierByBuilding(int /*BuildingTypes*/ iBuilding)
{
	return m_pCity ? m_pCity->getAdditionalGreatPeopleRateModifierByBuilding((BuildingTypes)iBuilding) : -1;
}
// BUG - Building Additional Great People - end

int CyCity::getGreatPeopleProgress()
{
	return m_pCity ? m_pCity->getGreatPeopleProgress() : -1;
}

void CyCity::changeGreatPeopleProgress(int iChange)
{
	if (m_pCity)
		m_pCity->changeGreatPeopleProgress(iChange);
}

int CyCity::getNumWorldWonders()
{
	return m_pCity ? m_pCity->getNumWorldWonders() : -1;
}

int CyCity::getNumTeamWonders()
{
	return m_pCity ? m_pCity->getNumTeamWonders() : -1;
}

int CyCity::getNumNationalWonders()
{
	return m_pCity ? m_pCity->getNumNationalWonders() : -1;
}

int CyCity::getMaxNumWorldWonders()
{
	return m_pCity ? m_pCity->getMaxNumWorldWonders() : -1;
}

int CyCity::getMaxNumTeamWonders()
{
	return m_pCity ? m_pCity->getMaxNumTeamWonders() : -1;
}

int CyCity::getMaxNumNationalWonders()
{
	return m_pCity ? m_pCity->getMaxNumNationalWonders() : -1;
}

int CyCity::getNumBuildings()
{
	return m_pCity ? m_pCity->getNumBuildings() : -1;
}

bool CyCity::isGovernmentCenter()
{
	return m_pCity ? m_pCity->isGovernmentCenter() : false;
}

// BUG - Building Saved Maintenance - start
int CyCity::getSavedMaintenanceByBuilding(int /*BuildingTypes*/ iBuilding) const
{
	return m_pCity ? m_pCity->getSavedMaintenanceByBuilding((BuildingTypes)iBuilding) : -1;
}

int CyCity::getSavedMaintenanceTimes100ByBuilding(int /*BuildingTypes*/ iBuilding) const
{
	return m_pCity ? m_pCity->getSavedMaintenanceTimes100ByBuilding((BuildingTypes)iBuilding) : -1;
}
// BUG - Building Saved Maintenance - end

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


int CyCity::calculateDistanceMaintenanceTimes100() const														
{
	return m_pCity ? m_pCity->calculateDistanceMaintenanceTimes100() : -1;
}


int CyCity::calculateNumCitiesMaintenance()	const												
{
	return m_pCity ? m_pCity->calculateNumCitiesMaintenance() : -1;
}

int CyCity::calculateNumCitiesMaintenanceTimes100()	const												
{
	return m_pCity ? m_pCity->calculateNumCitiesMaintenanceTimes100() : -1;
}

int CyCity::calculateColonyMaintenance()	const												
{
	return m_pCity ? m_pCity->calculateColonyMaintenance() : -1;
}

int CyCity::calculateColonyMaintenanceTimes100()	const												
{
	return m_pCity ? m_pCity->calculateColonyMaintenanceTimes100() : -1;
}

int CyCity::calculateCorporationMaintenanceTimes100()	const												
{
	return m_pCity ? m_pCity->calculateCorporationMaintenanceTimes100() : -1;
}

int CyCity::calculateCorporationMaintenance()	const												
{
	return m_pCity ? m_pCity->calculateCorporationMaintenance() : -1;
}

int CyCity::getMaintenanceModifier()
{
	return m_pCity ? m_pCity->getMaintenanceModifier() : -1;
}

int CyCity::getWarWearinessModifier()
{
	return m_pCity ? m_pCity->getWarWearinessModifier() : -1;
}

int CyCity::getHurryAngerModifier()
{
	return m_pCity ? m_pCity->getHurryAngerModifier() : -1;
}

int CyCity::getEspionageHealthCounter()
{
	return m_pCity ? m_pCity->getEspionageHealthCounter() : -1;
}

void CyCity::changeEspionageHealthCounter(int iChange)
{
	if (m_pCity)
		m_pCity->changeEspionageHealthCounter(iChange);
}

int CyCity::getEspionageHappinessCounter()
{
	return m_pCity ? m_pCity->getEspionageHappinessCounter() : -1;
}

void CyCity::changeEspionageHappinessCounter(int iChange)
{
	if (m_pCity)
		m_pCity->changeEspionageHappinessCounter(iChange);
}

void CyCity::changeHealRate(int iChange)
{
	if (m_pCity)
		m_pCity->changeHealRate(iChange);
}

int CyCity::getFreshWaterGoodHealth()
{
	return m_pCity ? m_pCity->getFreshWaterGoodHealth() : -1;
}

int CyCity::getFreshWaterBadHealth()
{
	return m_pCity ? m_pCity->getFreshWaterBadHealth() : -1;
}

int CyCity::getBuildingGoodHealth()
{
	return m_pCity ? m_pCity->getBuildingGoodHealth() : -1;
}

int CyCity::getBuildingBadHealth()
{
	return m_pCity ? m_pCity->getBuildingBadHealth() : -1;
}

int CyCity::getFeatureGoodHealth()
{
	return m_pCity ? m_pCity->getFeatureGoodHealth() : -1;
}

int CyCity::getFeatureBadHealth()
{
	return m_pCity ? m_pCity->getFeatureBadHealth() : -1;
}

// BUG - Feature Health - start
int CyCity::getAdditionalHealthByFeature(int /*FeatureTypes*/ eFeature, int iChange)
{
	if (m_pCity)
	{
		int iGood = 0, iBad = 0;

		return m_pCity->getAdditionalHealthByFeature((FeatureTypes)eFeature, iChange, iGood, iBad);
	}
	else
	{
		return -1;
	}
}

int CyCity::getAdditionalGoodHealthByFeature(int /*FeatureTypes*/ eFeature, int iChange)
{
	if (m_pCity)
	{
		int iGood = 0, iBad = 0;

		m_pCity->getAdditionalHealthByFeature((FeatureTypes)eFeature, iChange, iGood, iBad);
		return iGood;
	}
	else
	{
		return -1;
	}
}

int CyCity::getAdditionalBadHealthByFeature(int /*FeatureTypes*/ eFeature, int iChange)
{
	if (m_pCity)
	{
		int iGood = 0, iBad = 0;

		m_pCity->getAdditionalHealthByFeature((FeatureTypes)eFeature, iChange, iGood, iBad);
		return iBad;
	}
	else
	{
		return -1;
	}
}
// BUG - Feature Health - end

int CyCity::getBuildingHealth(int /*BuildingTypes*/ eBuilding)
{
	return m_pCity ? m_pCity->getBuildingHealth((BuildingTypes)eBuilding) : -1;
}

// BUG - Building Additional Health - start
int CyCity::getAdditionalHealthByBuilding(int /*BuildingTypes*/ eBuilding)
{
	if (m_pCity)
	{
		int iGood = 0, iBad = 0, iSpoiledFood = 0, iStarvation = 0;

		return m_pCity->getAdditionalHealthByBuilding((BuildingTypes)eBuilding, iGood, iBad, iSpoiledFood, iStarvation);
	}
	else
	{
		return -1;
	}
}

int CyCity::getAdditionalGoodHealthByBuilding(int /*BuildingTypes*/ eBuilding)
{
	if (m_pCity)
	{
		int iGood = 0, iBad = 0, iSpoiledFood = 0, iStarvation = 0;

		m_pCity->getAdditionalHealthByBuilding((BuildingTypes)eBuilding, iGood, iBad, iSpoiledFood, iStarvation);
		return iGood;
	}
	else
	{
		return -1;
	}
}

int CyCity::getAdditionalBadHealthByBuilding(int /*BuildingTypes*/ eBuilding)
{
	if (m_pCity)
	{
		int iGood = 0, iBad = 0, iSpoiledFood = 0, iStarvation = 0;

		m_pCity->getAdditionalHealthByBuilding((BuildingTypes)eBuilding, iGood, iBad, iSpoiledFood, iStarvation);
		return iBad;
	}
	else
	{
		return -1;
	}
}

int CyCity::getAdditionalSpoiledFoodByBuilding(int /*BuildingTypes*/ eBuilding)
{
	if (m_pCity)
	{
		int iGood = 0, iBad = 0, iSpoiledFood = 0, iStarvation = 0;

		m_pCity->getAdditionalHealthByBuilding((BuildingTypes)eBuilding, iGood, iBad, iSpoiledFood, iStarvation);
		return iSpoiledFood;
	}
	else
	{
		return -1;
	}
}

int CyCity::getAdditionalStarvationByBuilding(int /*BuildingTypes*/ eBuilding)
{
	if (m_pCity)
	{
		int iGood = 0, iBad = 0, iSpoiledFood = 0, iStarvation = 0;

		m_pCity->getAdditionalHealthByBuilding((BuildingTypes)eBuilding, iGood, iBad, iSpoiledFood, iStarvation);
		return iStarvation;
	}
	else
	{
		return -1;
	}
}
// BUG - Building Additional Health - end

int CyCity::getPowerGoodHealth()
{
	return m_pCity ? m_pCity->getPowerGoodHealth() : -1;
}

int CyCity::getPowerBadHealth()
{
	return m_pCity ? m_pCity->getPowerBadHealth() : -1;
}

int CyCity::getBonusGoodHealth()
{
	return m_pCity ? m_pCity->getBonusGoodHealth() : -1;
}

int CyCity::getBonusBadHealth()
{
	return m_pCity ? m_pCity->getBonusBadHealth() : -1;
}

int CyCity::getMilitaryHappiness()
{
	return m_pCity ? m_pCity->getMilitaryHappiness() : -1;
}

int CyCity::getMilitaryHappinessUnits()
{
	return m_pCity ? m_pCity->getMilitaryHappinessUnits() : -1;
}

int CyCity::getBuildingGoodHappiness()
{
	return m_pCity ? m_pCity->getBuildingGoodHappiness() : -1;
}

int CyCity::getBuildingBadHappiness()
{
	return m_pCity ? m_pCity->getBuildingBadHappiness() : -1;
}

int CyCity::getBuildingHappiness(int /*BuildingTypes*/ eBuilding)
{
	return m_pCity ? m_pCity->getBuildingHappiness((BuildingTypes)eBuilding) : -1;
}

// BUG - Building Additional Happiness - start
int CyCity::getAdditionalHappinessByBuilding(int /*BuildingTypes*/ eBuilding)
{
	if (m_pCity)
	{
		int iGood = 0, iBad = 0, iAngryPop = 0;

		return m_pCity->getAdditionalHappinessByBuilding((BuildingTypes)eBuilding, iGood, iBad, iAngryPop);
	}
	else
	{
		return -1;
	}
}

int CyCity::getAdditionalGoodHappinessByBuilding(int /*BuildingTypes*/ eBuilding)
{
	if (m_pCity)
	{
		int iGood = 0, iBad = 0, iAngryPop = 0;

		m_pCity->getAdditionalHappinessByBuilding((BuildingTypes)eBuilding, iGood, iBad, iAngryPop);
		return iGood;
	}
	else
	{
		return -1;
	}
}

int CyCity::getAdditionalBadHappinessByBuilding(int /*BuildingTypes*/ eBuilding)
{
	if (m_pCity)
	{
		int iGood = 0, iBad = 0, iAngryPop = 0;

		m_pCity->getAdditionalHappinessByBuilding((BuildingTypes)eBuilding, iGood, iBad, iAngryPop);
		return iBad;
	}
	else
	{
		return -1;
	}
}

int CyCity::getAdditionalAngryPopulationByBuilding(int /*BuildingTypes*/ eBuilding)
{
	if (m_pCity)
	{
		int iGood = 0, iBad = 0, iAngryPop = 0;

		m_pCity->getAdditionalHappinessByBuilding((BuildingTypes)eBuilding, iGood, iBad, iAngryPop);
		return iAngryPop;
	}
	else
	{
		return -1;
	}
}
// BUG - Building Additional Happiness - end

int CyCity::getExtraBuildingGoodHappiness()
{
	return m_pCity ? m_pCity->getExtraBuildingGoodHappiness() : -1;
}

int CyCity::getExtraBuildingBadHappiness()
{
	return m_pCity ? m_pCity->getExtraBuildingBadHappiness() : -1;
}

int CyCity::getFeatureGoodHappiness()
{
	return m_pCity ? m_pCity->getFeatureGoodHappiness() : -1;
}

int CyCity::getFeatureBadHappiness()
{
	return m_pCity ? m_pCity->getFeatureBadHappiness() : -1;
}

int CyCity::getBonusGoodHappiness()
{
	return m_pCity ? m_pCity->getBonusGoodHappiness() : -1;
}

int CyCity::getBonusBadHappiness()
{
	return m_pCity ? m_pCity->getBonusBadHappiness() : -1;
}

int CyCity::getReligionGoodHappiness()
{
	return m_pCity ? m_pCity->getReligionGoodHappiness() : -1;
}

int CyCity::getReligionBadHappiness()
{
	return m_pCity ? m_pCity->getReligionBadHappiness() : -1;
}

int CyCity::getReligionHappiness(int /*ReligionTypes*/ eReligion)
{
	return m_pCity ? m_pCity->getReligionHappiness((ReligionTypes)eReligion) : -1;
}

int CyCity::getExtraHappiness()
{
	return m_pCity ? m_pCity->getExtraHappiness() : -1;
}

void CyCity::changeExtraHappiness(int iChange)
{
	if (m_pCity)
		m_pCity->changeExtraHappiness(iChange);
}

int CyCity::getExtraHealth()
{
	return m_pCity ? m_pCity->getExtraHealth() : -1;
}

void CyCity::changeExtraHealth(int iChange)
{
	if (m_pCity)
		m_pCity->changeExtraHealth(iChange);
}

int CyCity::getHurryAngerTimer()
{
	return m_pCity ? m_pCity->getHurryAngerTimer() : -1;
}

void CyCity::changeHurryAngerTimer(int iChange)
{
	if (m_pCity)
		m_pCity->changeHurryAngerTimer(iChange);
}
/************************************************************************************************/
/* REVOLUTION_MOD                         04/19/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int CyCity::getRevRequestAngerTimer()
{
	return m_pCity ? m_pCity->getRevRequestAngerTimer() : -1;
}

void CyCity::changeRevRequestAngerTimer(int iChange)
{
	if (m_pCity)
		m_pCity->changeRevRequestAngerTimer(iChange);
}

int CyCity::getRevSuccessTimer()
{
	return m_pCity ? m_pCity->getRevSuccessTimer() : -1;
}

void CyCity::changeRevSuccessTimer(int iChange)
{
	if (m_pCity)
		m_pCity->changeRevSuccessTimer(iChange);
}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

int CyCity::getConscriptAngerTimer()
{
	return m_pCity ? m_pCity->getConscriptAngerTimer() : -1;
}

void CyCity::changeConscriptAngerTimer(int iChange)
{
	if (m_pCity)
		m_pCity->changeConscriptAngerTimer(iChange);
}

int CyCity::getDefyResolutionAngerTimer()
{
	return m_pCity ? m_pCity->getDefyResolutionAngerTimer() : -1;
}

void CyCity::changeDefyResolutionAngerTimer(int iChange)
{
	if (m_pCity)
		m_pCity->changeDefyResolutionAngerTimer(iChange);
}

int CyCity::flatDefyResolutionAngerLength()
{
	return m_pCity ? m_pCity->flatDefyResolutionAngerLength() : -1;
}

int CyCity::getHappinessTimer()
{
	return m_pCity ? m_pCity->getHappinessTimer() : -1;
}

void CyCity::changeHappinessTimer(int iChange)
{
	if (m_pCity)
		m_pCity->changeHappinessTimer(iChange);
}

bool CyCity::isNoUnhappiness()
{
	return m_pCity ? m_pCity->isNoUnhappiness() : false;
}

bool CyCity::isNoUnhealthyPopulation()
{
	return m_pCity ? m_pCity->isNoUnhealthyPopulation() : false;
}

bool CyCity::isBuildingOnlyHealthy()
{
	return m_pCity ? m_pCity->isBuildingOnlyHealthy() : false;
}

int CyCity::getFood()
{
	return m_pCity ? m_pCity->getFood() : -1;
}

void CyCity::setFood(int iNewValue)
{
	if (m_pCity)
		m_pCity->setFood(iNewValue);
}

void CyCity::changeFood(int iChange)
{
	if (m_pCity)
		m_pCity->changeFood(iChange);
}

int CyCity::getFoodKept()
{
	return m_pCity ? m_pCity->getFoodKept() : -1;
}

int CyCity::getMaxFoodKeptPercent()
{
	return m_pCity ? m_pCity->getMaxFoodKeptPercent() : -1;
}

int CyCity::getOverflowProduction()
{
	return m_pCity ? m_pCity->getOverflowProduction() : -1;
}

void CyCity::setOverflowProduction(int iNewValue)
{
	if (m_pCity)
		m_pCity->setOverflowProduction(iNewValue);
}

int CyCity::getFeatureProduction()
{
	return m_pCity ? m_pCity->getFeatureProduction() : -1;
}

void CyCity::setFeatureProduction(int iNewValue)
{
	if (m_pCity)
		m_pCity->setFeatureProduction(iNewValue);
}

int CyCity::getMilitaryProductionModifier()
{
	return m_pCity ? m_pCity->getMilitaryProductionModifier() : -1;
}

int CyCity::getSpaceProductionModifier()								
{
	return m_pCity ? m_pCity->getSpaceProductionModifier() : -1;
}

int CyCity::getExtraTradeRoutes()
{
	return m_pCity ? m_pCity->getExtraTradeRoutes() : -1;
}

void CyCity::changeExtraTradeRoutes(int iChange)
{
	if (m_pCity)
		m_pCity->changeExtraTradeRoutes(iChange);
}

int CyCity::getTradeRouteModifier()															
{
	return m_pCity ? m_pCity->getTradeRouteModifier() : -1;
}

int CyCity::getForeignTradeRouteModifier()															
{
	return m_pCity ? m_pCity->getForeignTradeRouteModifier() : -1;
}

int CyCity::getBuildingDefense()
{
	return m_pCity ? m_pCity->getBuildingDefense() : -1;
}

// BUG - Building Additional Defense - start
int CyCity::getAdditionalDefenseByBuilding(int /*BuildingTypes*/ eBuilding)
{
	return m_pCity ? m_pCity->getAdditionalDefenseByBuilding((BuildingTypes)eBuilding) : -1;
}
// BUG - Building Additional Defense - end

int CyCity::getBuildingBombardDefense()
{
	return m_pCity ? m_pCity->getBuildingBombardDefense() : -1;
}

int CyCity::getFreeExperience()
{
	return m_pCity ? m_pCity->getFreeExperience() : -1;
}

int CyCity::getCurrAirlift()
{
	return m_pCity ? m_pCity->getCurrAirlift() : -1;
}

int CyCity::getMaxAirlift()
{
	return m_pCity ? m_pCity->getMaxAirlift() : -1;
}

int CyCity::getAirModifier()
{
	return m_pCity ? m_pCity->getAirModifier() : -1;
}

int CyCity::getAirUnitCapacity(int /*TeamTypes*/ eTeam)
{
	return m_pCity ? m_pCity->getAirUnitCapacity((TeamTypes)eTeam) : -1;
}

int CyCity::getNukeModifier()
{
	return m_pCity ? m_pCity->getNukeModifier() : -1;
}

int CyCity::getFreeSpecialist() 
{
	return m_pCity ? m_pCity->getFreeSpecialist() : -1;
}

bool CyCity::isPower()
{
	return m_pCity ? m_pCity->isPower() : false;
}

bool CyCity::isAreaCleanPower()
{
	return m_pCity ? m_pCity->isAreaCleanPower() : false;
}

bool CyCity::isDirtyPower()
{
	return m_pCity ? m_pCity->isDirtyPower() : false;
}

int CyCity::getDefenseDamage()
{
	return m_pCity ? m_pCity->getDefenseDamage() : -1;
}

void CyCity::changeDefenseDamage(int iChange)
{
	if (m_pCity)
		m_pCity->changeDefenseDamage(iChange);
}

bool CyCity::isBombardable(CyUnit* pUnit)
{
	return m_pCity ? m_pCity->isBombardable(pUnit->getUnit()) : false;
}

int CyCity::getNaturalDefense()
{
	return m_pCity ? m_pCity->getNaturalDefense() : -1;
}

int CyCity::getTotalDefense(bool bIgnoreBuilding)
{
	return m_pCity ? m_pCity->getTotalDefense(bIgnoreBuilding) : -1;
}

int CyCity::getDefenseModifier(bool bIgnoreBuilding)
{
	return m_pCity ? m_pCity->getDefenseModifier(bIgnoreBuilding) : -1;
}

int CyCity::getOccupationTimer()
{
	return m_pCity ? m_pCity->getOccupationTimer() : -1;
}

bool CyCity::isOccupation()
{
	return m_pCity ? m_pCity->isOccupation() : false;
}

void CyCity::setOccupationTimer(int iNewValue)
{
	if (m_pCity)
		m_pCity->setOccupationTimer(iNewValue);
}

void CyCity::changeOccupationTimer(int iChange)
{
	if (m_pCity)
		m_pCity->changeOccupationTimer(iChange);
}

int CyCity::getCultureUpdateTimer()
{
	return m_pCity ? m_pCity->getCultureUpdateTimer() : -1;
}

void CyCity::changeCultureUpdateTimer(int iChange)
{
	if (m_pCity)
		m_pCity->changeCultureUpdateTimer(iChange);
}

bool CyCity::isNeverLost()
{
	return m_pCity ? m_pCity->isNeverLost() : false;
}

void CyCity::setNeverLost(int iNewValue)
{
	if (m_pCity)
		m_pCity->setNeverLost(iNewValue);
}

// < M.A.D. Nukes Start >
int CyCity::getMADIncoming()
{
	return m_pCity ? m_pCity->getMADIncoming() : -1;
}
// < M.A.D. Nukes End   >

bool CyCity::isBombarded()
{
	return m_pCity ? m_pCity->isBombarded(): false;
}

void CyCity::setBombarded(int iNewValue)
{
	if (m_pCity)
		m_pCity->setBombarded(iNewValue);
}

bool CyCity::isDrafted()
{
	return m_pCity ? m_pCity->isDrafted(): false;
}

void CyCity::setDrafted(int iNewValue)
{
	if (m_pCity)
		m_pCity->setDrafted(iNewValue);
}

bool CyCity::isAirliftTargeted()
{
	return m_pCity ? m_pCity->isAirliftTargeted(): false;
}

void CyCity::setAirliftTargeted(int iNewValue)
{
	if (m_pCity)
		m_pCity->setAirliftTargeted(iNewValue);
}

bool CyCity::isCitizensAutomated()
{
	return m_pCity ? m_pCity->isCitizensAutomated() : false;
}

void CyCity::setCitizensAutomated(bool bNewValue)
{
	if (m_pCity)
		m_pCity->setCitizensAutomated(bNewValue);
}

bool CyCity::isProductionAutomated()
{
	return m_pCity ? m_pCity->isProductionAutomated() : false;
}

void CyCity::setProductionAutomated(bool bNewValue)
{
	if (m_pCity)
	{
		m_pCity->setProductionAutomated(bNewValue, false);
	}
}

bool CyCity::isWallOverride() const
{
	return m_pCity ? m_pCity->isWallOverride() : false;
}

void CyCity::setWallOverride(bool bOverride)
{
	if (m_pCity)
		m_pCity->setWallOverride(bOverride);
}

void CyCity::setCitySizeBoost(int iBoost)
{
	if (m_pCity)
		m_pCity->setCitySizeBoost(iBoost);
}

bool CyCity::isPlundered()
{
	return m_pCity ? m_pCity->isPlundered() : false;
}

void CyCity::setPlundered(bool bNewValue)
{
	if (m_pCity)
	{
		m_pCity->setPlundered(bNewValue);
	}
}

int /*PlayerTypes*/ CyCity::getOwner()
{
	return m_pCity ? m_pCity->getOwner() : NO_PLAYER;
}

int /*TeamTypes*/ CyCity::getTeam()
{
	return m_pCity ? m_pCity->getTeam() : NO_TEAM;
}

int /*PlayerTypes*/ CyCity::getPreviousOwner()
{
	return m_pCity ? m_pCity->getPreviousOwner() : NO_PLAYER;
}

int /*PlayerTypes*/ CyCity::getOriginalOwner()
{
	return m_pCity ? m_pCity->getOriginalOwner() : NO_PLAYER;
}

int /*CultureLevelTypes*/ CyCity::getCultureLevel()
{
	return m_pCity ? m_pCity->getCultureLevel() : NO_CULTURELEVEL;
}

int CyCity::getCultureThreshold()
{
	return m_pCity ? m_pCity->getCultureThreshold() : -1;
}

int CyCity::getSeaPlotYield(int /*YieldTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getSeaPlotYield((YieldTypes) eIndex) : -1;
}

int CyCity::getRiverPlotYield(int /*YieldTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getRiverPlotYield((YieldTypes) eIndex) : -1;
}

// BUG - Building Additional Yield - start
int CyCity::getAdditionalYieldByBuilding(int /*YieldTypes*/ eIndex, int /*BuildingTypes*/ iBuilding)
{
	return m_pCity ? m_pCity->getAdditionalYieldByBuilding((YieldTypes)eIndex, (BuildingTypes) iBuilding) : -1;
}

int CyCity::getAdditionalBaseYieldRateByBuilding(int /*YieldTypes*/ eIndex, int /*BuildingTypes*/ iBuilding)
{
	return m_pCity ? m_pCity->getAdditionalBaseYieldRateByBuilding((YieldTypes)eIndex, (BuildingTypes) iBuilding) : -1;
}

int CyCity::getAdditionalYieldRateModifierByBuilding(int /*YieldTypes*/ eIndex, int /*BuildingTypes*/ iBuilding)
{
	return m_pCity ? m_pCity->getAdditionalYieldRateModifierByBuilding((YieldTypes)eIndex, (BuildingTypes) iBuilding) : -1;
}
// BUG - Building Additional Yield - end

int CyCity::getBaseYieldRate(int /*YieldTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getBaseYieldRate((YieldTypes)eIndex) : -1;
}

void CyCity::setBaseYieldRate(int /*YieldTypes*/ eIndex, int iNewValue)
{
	if (m_pCity)
		m_pCity->setBaseYieldRate((YieldTypes)eIndex, iNewValue);
}

void CyCity::changeBaseYieldRate(int /*YieldTypes*/ eIndex, int iNewValue)
{
	if (m_pCity)
		m_pCity->changeBaseYieldRate((YieldTypes)eIndex, iNewValue);
}

int CyCity::getBaseYieldRateModifier(int /*YieldTypes*/ eIndex, int iExtra)
{
	return m_pCity ? m_pCity->getBaseYieldRateModifier((YieldTypes)eIndex, iExtra) : -1;
}

int CyCity::getYieldRate(int /*YieldTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getYieldRate((YieldTypes)eIndex) : -1;
}

int CyCity::getYieldRateModifier(int /*YieldTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getYieldRateModifier((YieldTypes)eIndex) : -1;
}

int CyCity::getTradeYield(int /*YieldTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getTradeYield((YieldTypes)eIndex) : -1;
}

int CyCity::totalTradeModifier()
{
	return m_pCity ? m_pCity->totalTradeModifier() : -1;
}

// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
int CyCity::calculateTradeProfitTimes100(CyCity* pCity)
{
	return m_pCity ? m_pCity->calculateTradeProfitTimes100(pCity->getCity()) : -1;
}
#endif
// BUG - Fractional Trade Routes - end

int CyCity::calculateTradeProfit(CyCity* pCity)
{
	return m_pCity ? m_pCity->calculateTradeProfit(pCity->getCity()) : -1;
}

int CyCity::calculateTradeYield(int /*YieldTypes*/ eIndex, int iTradeProfit)
{
	return m_pCity ? m_pCity->calculateTradeYield((YieldTypes)eIndex, iTradeProfit) : -1;
}

int CyCity::getExtraSpecialistYield(int /*YieldTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getExtraSpecialistYield((YieldTypes) eIndex) : -1;
}

int CyCity::getExtraSpecialistYieldOfType(int /*YieldTypes*/ eIndex, int /*SpecialistTypes*/ eSpecialist)
{
//Team Project (1)
	int iTotal = 0;
	iTotal += (m_pCity ? m_pCity->getExtraSpecialistYield((YieldTypes) eIndex, (SpecialistTypes) eSpecialist) : 0);
	iTotal += (m_pCity ? m_pCity->getLocalSpecialistExtraYield((SpecialistTypes) eSpecialist, (YieldTypes) eIndex) : 0);
	return m_pCity ? iTotal : -1;
}

int CyCity::getCommerceRate(int /*CommerceTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getCommerceRate((CommerceTypes)eIndex) : -1;
}

int CyCity::getCommerceRateTimes100(int /*CommerceTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getCommerceRateTimes100((CommerceTypes)eIndex) : -1;
}

int CyCity::getCommerceFromPercent(int /*CommerceTypes*/ eIndex, int iYieldRate)
{
	return m_pCity ? m_pCity->getCommerceFromPercent((CommerceTypes)eIndex, iYieldRate) : -1;
}

int CyCity::getBaseCommerceRate(int /*CommerceTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getBaseCommerceRate((CommerceTypes)eIndex) : -1;
}

int CyCity::getBaseCommerceRateTimes100(int /*CommerceTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getBaseCommerceRateTimes100((CommerceTypes)eIndex) : -1;
}

int CyCity::getTotalCommerceRateModifier(int /*CommerceTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getTotalCommerceRateModifier((CommerceTypes)eIndex) : -1;
}

int CyCity::getProductionToCommerceModifier(int /*CommerceTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getProductionToCommerceModifier((CommerceTypes)eIndex) : -1;
}

int CyCity::getBuildingCommerce(int /*CommerceTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getBuildingCommerce((CommerceTypes)eIndex) : -1;
}

int CyCity::getBuildingCommerceByBuilding(int /*CommerceTypes*/ eIndex, int /*BuildingTypes*/ iBuilding)
{
	return m_pCity ? m_pCity->getBuildingCommerceByBuilding((CommerceTypes)eIndex, (BuildingTypes) iBuilding) : -1;
}

// BUG - Building Additional Commerce - start
int CyCity::getAdditionalCommerceByBuilding(int /*CommerceTypes*/ eIndex, int /*BuildingTypes*/ iBuilding)
{
	return m_pCity ? m_pCity->getAdditionalCommerceByBuilding((CommerceTypes)eIndex, (BuildingTypes) iBuilding) : -1;
}

int CyCity::getAdditionalCommerceTimes100ByBuilding(int /*CommerceTypes*/ eIndex, int /*BuildingTypes*/ iBuilding)
{
	return m_pCity ? m_pCity->getAdditionalCommerceTimes100ByBuilding((CommerceTypes)eIndex, (BuildingTypes) iBuilding) : -1;
}

int CyCity::getAdditionalBaseCommerceRateByBuilding(int /*CommerceTypes*/ eIndex, int /*BuildingTypes*/ iBuilding)
{
	return m_pCity ? m_pCity->getAdditionalBaseCommerceRateByBuilding((CommerceTypes)eIndex, (BuildingTypes) iBuilding) : -1;
}

int CyCity::getAdditionalCommerceRateModifierByBuilding(int /*CommerceTypes*/ eIndex, int /*BuildingTypes*/ iBuilding)
{
	return m_pCity ? m_pCity->getAdditionalCommerceRateModifierByBuilding((CommerceTypes)eIndex, (BuildingTypes) iBuilding) : -1;
}
// BUG - Building Additional Commerce - end

int CyCity::getSpecialistCommerce(int /*CommerceTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getSpecialistCommerce((CommerceTypes)eIndex) : -1;
}

void CyCity::changeSpecialistCommerce(int /*CommerceTypes*/ eIndex, int iChange)
{
	if (m_pCity)
		m_pCity->changeSpecialistCommerceTimes100((CommerceTypes)eIndex, 100*iChange);
}

int CyCity::getReligionCommerce(int /*CommerceTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getReligionCommerce((CommerceTypes)eIndex) : -1;
}

int CyCity::getReligionCommerceByReligion(int /*CommerceTypes*/ eIndex, int /*ReligionTypes*/ eReligion)
{
	return m_pCity ? m_pCity->getReligionCommerceByReligion((CommerceTypes)eIndex, (ReligionTypes)eReligion) : -1;
}

int CyCity::getCorporationCommerce(int /*CommerceTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getCorporationCommerce((CommerceTypes)eIndex) : -1;
}

int CyCity::getCorporationCommerceByCorporation(int /*CommerceTypes*/ eIndex, int /*CorporationTypes*/ eCorporation)
{
	return m_pCity ? m_pCity->getCorporationCommerceByCorporation((CommerceTypes)eIndex, (CorporationTypes)eCorporation) : -1;
}

int CyCity::getCorporationYield(int /*YieldTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getCorporationYield((YieldTypes)eIndex) : -1;
}

int CyCity::getCorporationYieldByCorporation(int /*YieldTypes*/ eIndex, int /*CorporationTypes*/ eCorporation)
{
	return m_pCity ? m_pCity->getCorporationYieldByCorporation((YieldTypes)eIndex, (CorporationTypes)eCorporation) : -1;
}

int CyCity::getCommerceRateModifier(int /*CommerceTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getTotalCommerceRateModifier((CommerceTypes)eIndex) : -1;
}

int CyCity::getCommerceHappinessPer(int /*CommerceTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getCommerceHappinessPer((CommerceTypes)eIndex) : -1;
}

int CyCity::getCommerceHappinessByType(int /*CommerceTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getCommerceHappinessByType((CommerceTypes) eIndex) : -1;
}

int CyCity::getCommerceHappiness()
{
	return m_pCity ? m_pCity->getCommerceHappiness() : -1;
}

int CyCity::getDomainFreeExperience(int /*DomainTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getDomainFreeExperience((DomainTypes)eIndex) : -1;
}

int CyCity::getDomainProductionModifier(int /*DomainTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getDomainProductionModifier((DomainTypes)eIndex) : -1;
}

int CyCity::getUnitProductionModifier(int /*UnitTypes*/ eIndex) const
{
	return m_pCity ? m_pCity->getUnitProductionModifier((UnitTypes)eIndex) : -1;
}

int CyCity::getCulture(int /*PlayerTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getCulture((PlayerTypes)eIndex) : -1;
}

int CyCity::getCultureTimes100(int /*PlayerTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getCultureTimes100((PlayerTypes)eIndex) : -1;
}

int CyCity::countTotalCultureTimes100()
{
	return m_pCity ? m_pCity->countTotalCultureTimes100() : -1;
}

PlayerTypes CyCity::findHighestCulture()
{
	return m_pCity ? m_pCity->findHighestCulture() : NO_PLAYER;
}

int CyCity::calculateCulturePercent(int /*PlayerTypes*/ eIndex)
{
	return m_pCity ? m_pCity->calculateCulturePercent((PlayerTypes)eIndex) : -1;
}

int CyCity::calculateTeamCulturePercent(int /*TeamTypes*/ eIndex)
{
	return m_pCity ? m_pCity->calculateTeamCulturePercent((TeamTypes)eIndex) : -1;
}

void CyCity::setCulture(int /*PlayerTypes*/ eIndex, int iNewValue, bool bPlots)
{
	if (m_pCity)
		m_pCity->setCulture((PlayerTypes)eIndex, iNewValue, bPlots, true);
}

void CyCity::setCultureTimes100(int /*PlayerTypes*/ eIndex, int iNewValue, bool bPlots)
{
	if (m_pCity)
		m_pCity->setCultureTimes100((PlayerTypes)eIndex, iNewValue, bPlots, true);
}

void CyCity::changeCulture(int /*PlayerTypes*/ eIndex, int iChange, bool bPlots)
{
	if (m_pCity)
		m_pCity->changeCulture((PlayerTypes)eIndex, iChange, bPlots, true);
}

void CyCity::changeCultureTimes100(int /*PlayerTypes*/ eIndex, int iChange, bool bPlots)
{
	if (m_pCity)
		m_pCity->changeCultureTimes100((PlayerTypes)eIndex, iChange, bPlots, true);
}

/************************************************************************************************/
/* REVOLUTION_MOD                         01/01/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int CyCity::getNumRevolts(int playerIdx)
{
	return m_pCity ? m_pCity->getNumRevolts((PlayerTypes)playerIdx) : -1;
}
void CyCity::changeNumRevolts(int playerIdx, int iChange)
{
	if (m_pCity)
		m_pCity->changeNumRevolts((PlayerTypes)playerIdx,iChange);
}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

bool CyCity::isTradeRoute(int /*PlayerTypes*/ eIndex)
{
	return m_pCity ? m_pCity->isTradeRoute((PlayerTypes)eIndex) : false;
}

bool CyCity::isEverOwned(int /*PlayerTypes*/ eIndex)
{
	return m_pCity ? m_pCity->isEverOwned((PlayerTypes)eIndex) : false;
}

bool CyCity::isRevealed(int /*TeamTypes */eIndex, bool bDebug)
{
	return m_pCity ? m_pCity->isRevealed((TeamTypes)eIndex, bDebug) : false;
}

void CyCity::setRevealed(int /*TeamTypes*/ eIndex, bool bNewValue) 	
{
	if (m_pCity)
		m_pCity->setRevealed((TeamTypes)eIndex, bNewValue);
}

bool CyCity::getEspionageVisibility(int /*TeamTypes */eIndex)
{
	return m_pCity ? m_pCity->getEspionageVisibility((TeamTypes)eIndex) : false;
}

std::wstring CyCity::getName()
{
	return m_pCity ? m_pCity->getName() : L"";
}

std::wstring CyCity::getNameForm(int iForm)
{
	return m_pCity ? m_pCity->getName((uint)iForm) : L"";
}

std::wstring CyCity::getNameKey()
{
	return m_pCity ? m_pCity->getNameKey() : L"";
}

void CyCity::setName(std::wstring szNewValue, bool bFound)
{
	if (m_pCity)
		m_pCity->setName(CvWString(szNewValue), bFound);
}

void CyCity::changeNoBonusCount(int /*BonusTypes*/ eBonus, int iChange)
{
	if (m_pCity)
		m_pCity->changeNoBonusCount((BonusTypes)eBonus, iChange);
}

bool CyCity::isNoBonus(int /*BonusTypes*/ eBonus)
{
	return m_pCity ? m_pCity->isNoBonus((BonusTypes)eBonus) : false;
}

int CyCity::getFreeBonus(int /*BonusTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getFreeBonus((BonusTypes)eIndex) : -1;
}

void CyCity::changeFreeBonus(int /*BonusTypes*/ eIndex, int iChange)
{
	if (m_pCity)
		m_pCity->changeFreeBonus((BonusTypes)eIndex, iChange);
}

int CyCity::getNumBonuses(int /*BonusTypes*/ iBonus)
{
	return m_pCity ? m_pCity->getNumBonuses((BonusTypes) iBonus) : -1;
}

bool CyCity::hasBonus(int /*BonusTypes */iBonus)
{
	return m_pCity ? m_pCity->hasBonus((BonusTypes) iBonus) : false;
}

bool CyCity::hasVicinityBonus(int iBonus)
{
	return m_pCity ? m_pCity->hasVicinityBonus(static_cast<BonusTypes>(iBonus)) : false;
}

int CyCity::getBuildingProduction(int /*BuildingTypes*/ iIndex)
{
	return m_pCity ? m_pCity->getBuildingProduction((BuildingTypes) iIndex) : -1;
}

void CyCity::setBuildingProduction(int /*BuildingTypes*/ iIndex, int iNewValue)
{
	if (m_pCity)
	{
		int iAdaptedNewValue = std::max(0, iNewValue);
		m_pCity->setBuildingProduction((BuildingTypes) iIndex, iAdaptedNewValue);
	}
}

void CyCity::changeBuildingProduction(int /*BuildingTypes*/ iIndex, int iChange)
{
	if (m_pCity)
		m_pCity->changeBuildingProduction((BuildingTypes) iIndex, iChange);
}

int CyCity::getBuildingProductionTime(int /*BuildingTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getBuildingProductionTime((BuildingTypes)eIndex) : -1;
}

void CyCity::setBuildingProductionTime(int /*BuildingTypes*/ eIndex, int iNewValue)
{
	if (m_pCity)
		m_pCity->setBuildingProductionTime((BuildingTypes)eIndex, iNewValue);
}

void CyCity::changeBuildingProductionTime(int /*BuildingTypes*/ eIndex, int iChange)
{
	if (m_pCity)
		m_pCity->changeBuildingProductionTime((BuildingTypes) eIndex, iChange);
}

// BUG - Production Decay - start
bool CyCity::isBuildingProductionDecay(int /*BuildingTypes*/ eIndex)
{
	return m_pCity ? m_pCity->isBuildingProductionDecay((BuildingTypes)eIndex) : false;
}

int CyCity::getBuildingProductionDecay(int /*BuildingTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getBuildingProductionDecay((BuildingTypes)eIndex) : 0;
}

int CyCity::getBuildingProductionDecayTurns(int /*BuildingTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getBuildingProductionDecayTurns((BuildingTypes)eIndex) : 0;
}
// BUG - Production Decay - end

int CyCity::getBuildingOriginalOwner(int /*BuildingTypes*/ iIndex)
{
	return m_pCity ? m_pCity->getBuildingOriginalOwner((BuildingTypes) iIndex) : -1;
}

int CyCity::getBuildingOriginalTime(int /*BuildingTypes*/ iIndex)
{
	return m_pCity ? m_pCity->getBuildingOriginalTime((BuildingTypes) iIndex) : -1;
}

int CyCity::getUnitProduction(int iIndex)
{
	return m_pCity ? m_pCity->getUnitProduction((UnitTypes) iIndex) : -1;
}

void CyCity::setUnitProduction(int iIndex, int iNewValue)
{
	if (m_pCity)
		m_pCity->setUnitProduction((UnitTypes)iIndex, iNewValue);
}

void CyCity::changeUnitProduction(int /*UnitTypes*/ iIndex, int iChange)
{
	if (m_pCity)
		m_pCity->changeUnitProduction((UnitTypes) iIndex, iChange);
}

// BUG - Production Decay - start
int CyCity::getUnitProductionTime(int /*UnitTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getUnitProductionTime((UnitTypes)eIndex) : -1;
}

void CyCity::setUnitProductionTime(int /*UnitTypes*/ eIndex, int iNewValue)
{
	if (m_pCity)
		m_pCity->setUnitProductionTime((UnitTypes)eIndex, iNewValue);
}

void CyCity::changeUnitProductionTime(int /*UnitTypes*/ eIndex, int iChange)
{
	if (m_pCity)
		m_pCity->changeUnitProductionTime((UnitTypes) eIndex, iChange);
}

bool CyCity::isUnitProductionDecay(int /*UnitTypes*/ eIndex)
{
	return m_pCity ? m_pCity->isUnitProductionDecay((UnitTypes)eIndex) : false;
}

int CyCity::getUnitProductionDecay(int /*UnitTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getUnitProductionDecay((UnitTypes)eIndex) : 0;
}

int CyCity::getUnitProductionDecayTurns(int /*UnitTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getUnitProductionDecayTurns((UnitTypes)eIndex) : 0;
}
// BUG - Production Decay - end

// BUG - Project Production - start
int CyCity::getProjectProduction(int /*ProjectTypes*/ iIndex)
{
	return m_pCity ? m_pCity->getProjectProduction((ProjectTypes) iIndex) : -1;
}

void CyCity::setProjectProduction(int /*ProjectTypes*/ iIndex, int iNewValue)
{
	if (m_pCity)
		m_pCity->setProjectProduction((ProjectTypes) iIndex, iNewValue);
}

void CyCity::changeProjectProduction(int /*ProjectTypes*/ iIndex, int iChange)
{
	if (m_pCity)
		m_pCity->changeProjectProduction((ProjectTypes) iIndex, iChange);
}
// BUG - Project Production - end

int CyCity::getGreatPeopleUnitRate(int /*UnitTypes*/ iIndex)
{
	return m_pCity ? m_pCity->getGreatPeopleUnitRate((UnitTypes) iIndex) : -1;
}

int CyCity::getGreatPeopleUnitProgress(int /*UnitTypes*/ iIndex)
{
	return m_pCity ? m_pCity->getGreatPeopleUnitProgress((UnitTypes) iIndex) : -1;
}

void CyCity::setGreatPeopleUnitProgress(int /*UnitTypes*/ iIndex, int iNewValue)
{
	if (m_pCity)
		m_pCity->setGreatPeopleUnitProgress((UnitTypes) iIndex, iNewValue);
}

void CyCity::changeGreatPeopleUnitProgress(int /*UnitTypes*/ iIndex, int iChange)
{
	if (m_pCity)
		m_pCity->changeGreatPeopleUnitProgress((UnitTypes) iIndex, iChange);
}

int CyCity::getSpecialistCount(int /*SpecialistTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getSpecialistCount((SpecialistTypes)eIndex) : -1;
}

void CyCity::alterSpecialistCount(int /*SpecialistTypes*/ eIndex, int iChange)
{
	if (m_pCity)
		m_pCity->alterSpecialistCount((SpecialistTypes)eIndex, iChange);
}

int CyCity::getMaxSpecialistCount(int /*SpecialistTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getMaxSpecialistCount((SpecialistTypes)eIndex) : -1;
}

bool CyCity::isSpecialistValid(int /*SpecialistTypes*/ eIndex, int iExtra)
{
	return m_pCity ? m_pCity->isSpecialistValid((SpecialistTypes) eIndex, iExtra) : false;
}

int CyCity::getForceSpecialistCount(int /*SpecialistTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getForceSpecialistCount((SpecialistTypes)eIndex) : -1;
}

bool CyCity::isSpecialistForced()
{
	return m_pCity ? m_pCity->isSpecialistForced() : false;
}

int CyCity::getImprovementFreeSpecialists(int /*ImprovementTypes*/ iIndex)
{
	return m_pCity ? m_pCity->getImprovementFreeSpecialists((ImprovementTypes) iIndex) : -1;
}

int CyCity::getReligionInfluence(int /*ReligionTypes*/ iIndex)
{
	return m_pCity ? m_pCity->getReligionInfluence((ReligionTypes) iIndex) : -1;
}

void CyCity::setForceSpecialistCount(int /*SpecialistTypes*/ eIndex, int iNewValue)
{
	if (m_pCity)
		m_pCity->setForceSpecialistCount((SpecialistTypes)eIndex, iNewValue);
}

void CyCity::changeForceSpecialistCount(int /*SpecialistTypes*/ eIndex, int iChange)
{
	if (m_pCity)
		m_pCity->changeForceSpecialistCount((SpecialistTypes)eIndex, iChange);
}

int CyCity::getFreeSpecialistCount(int /*SpecialistTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getFreeSpecialistCount((SpecialistTypes)eIndex) : -1;
}

void CyCity::changeFreeSpecialistCount(int /*SpecialistTypes*/ eIndex, int iChange)
{
	if (m_pCity)
		m_pCity->changeFreeSpecialistCount((SpecialistTypes)eIndex, iChange, true);
}

int CyCity::getAddedFreeSpecialistCount(int /*SpecialistTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getAddedFreeSpecialistCount((SpecialistTypes)eIndex) : -1;
}

void CyCity::changeImprovementFreeSpecialists(int /*ImprovementTypes*/ iIndex, int iChange)
{
	if (m_pCity)
		m_pCity->changeImprovementFreeSpecialists((ImprovementTypes) iIndex, iChange);
}

void CyCity::changeReligionInfluence(int /*ReligionTypes*/ iIndex, int iChange)
{
	if (m_pCity)
		m_pCity->changeReligionInfluence((ReligionTypes) iIndex, iChange);
}

int CyCity::getCurrentStateReligionHappiness()
{
	return m_pCity ? m_pCity->getCurrentStateReligionHappiness() : -1;
}

int CyCity::getStateReligionHappiness(int /*ReligionTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getStateReligionHappiness((ReligionTypes) eIndex) : -1;
}

void CyCity::changeStateReligionHappiness(int /*ReligionTypes*/ eIndex, int iChange)
{
	if (m_pCity)
		m_pCity->changeStateReligionHappiness((ReligionTypes) eIndex, iChange);
}

int CyCity::getUnitCombatFreeExperience(int /*UnitCombatTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getUnitCombatFreeExperience((UnitCombatTypes) eIndex) : -1;
}

int CyCity::getFreePromotionCount(int /*PromotionTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getFreePromotionCount((PromotionTypes) eIndex) : -1;
}

bool CyCity::isFreePromotion(int /*PromotionTypes*/ eIndex)
{
	return m_pCity ? m_pCity->isFreePromotion((PromotionTypes) eIndex) : false;
}

int CyCity::getSpecialistFreeExperience() const
{
	return m_pCity ? m_pCity->getSpecialistFreeExperience() : -1;
}

int CyCity::getEspionageDefenseModifier() const
{
	return m_pCity ? m_pCity->getEspionageDefenseModifier() : 0;
}

bool CyCity::isWorkingPlotByIndex(int iIndex)
{
	return m_pCity ? m_pCity->isWorkingPlot(iIndex) : false;
}

bool CyCity::isWorkingPlot(CyPlot* pPlot) 
{
	return m_pCity ? m_pCity->isWorkingPlot(pPlot->getPlot()) : false;
}

void CyCity::alterWorkingPlot(int iIndex)
{
	if (m_pCity)
		m_pCity->alterWorkingPlot(iIndex);
}

int CyCity::getNumRealBuilding(int /*BuildingTypes*/ iIndex)
{
	return m_pCity ? m_pCity->getNumRealBuilding((BuildingTypes) iIndex) : -1;
}

void CyCity::setNumRealBuilding(int /*BuildingTypes*/ iIndex, int iNewValue)
{
	if (m_pCity)
		m_pCity->setNumRealBuilding((BuildingTypes) iIndex, iNewValue);
}

int CyCity::getNumFreeBuilding(int /*BuildingTypes*/ iIndex)
{
	return m_pCity ? m_pCity->getNumFreeBuilding((BuildingTypes) iIndex) : -1;
}

bool CyCity::isHasReligion(int /*ReligionTypes*/ iIndex)
{
	return m_pCity ? m_pCity->isHasReligion((ReligionTypes) iIndex) : false;
}

void CyCity::setHasReligion(int /*ReligionTypes*/ iIndex, bool bNewValue, bool bAnnounce, bool bArrows)
{
	if (m_pCity)
		m_pCity->setHasReligion((ReligionTypes) iIndex, bNewValue, bAnnounce, bArrows);
}

bool CyCity::isHasCorporation(int /*CorporationTypes*/ iIndex)
{
	return m_pCity ? m_pCity->isHasCorporation((CorporationTypes) iIndex) : false;
}

void CyCity::setHasCorporation(int /*CorporationTypes*/ iIndex, bool bNewValue, bool bAnnounce, bool bArrows)
{
	if (m_pCity)
		m_pCity->setHasCorporation((CorporationTypes) iIndex, bNewValue, bAnnounce, bArrows);
}

bool CyCity::isActiveCorporation(int /*CorporationTypes*/ eCorporation)
{
	return m_pCity ? m_pCity->isActiveCorporation((CorporationTypes) eCorporation) : false;
}

CyCity* CyCity::getTradeCity(int iIndex)
{
	return m_pCity ? new CyCity(m_pCity->getTradeCity(iIndex)) : NULL;
}

int CyCity::getTradeRoutes()
{
	return m_pCity ? m_pCity->getTradeRoutes() : -1;
}

void CyCity::clearOrderQueue()
{
	if (m_pCity)
		m_pCity->clearOrderQueue();
}

void CyCity::pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bForce)
{
	if (m_pCity)
		m_pCity->pushOrder(eOrder, iData1, iData2, bSave, bPop, bAppend, bForce);
}

void CyCity::popOrder(int iNum, bool bFinish, bool bChoose)
{
	if (m_pCity)
		m_pCity->popOrder(iNum, bFinish, bChoose);
}

int CyCity::getOrderQueueLength()
{
	return m_pCity ? m_pCity->getOrderQueueLength() : -1;
}

OrderData CyCity::getOrderFromQueue(int iIndex)
{
	return m_pCity ? m_pCity->getOrderAt(iIndex) : OrderData::InvalidOrder;
}

void CyCity::setWallOverridePoints(const python::tuple& kPoints)
{
	if (!m_pCity)
		return;

	float* pPointsData = NULL;
	int iSeqLength = gDLL->getPythonIFace()->putSeqInArray(kPoints.ptr() /*src*/, &pPointsData /*dst*/);
	
	// copy to pairs vector
	std::vector< std::pair<float, float> > pointsVec;
	pointsVec.reserve(iSeqLength/2);
	int i;
	for(i=0;i<iSeqLength;i+=2)
	{
		std::pair<float, float> pr(pPointsData[i], pPointsData[i+1]);
		pointsVec.push_back(pr);
	}

	m_pCity->setWallOverridePoints(pointsVec);

	delete [] pPointsData;
	
}

python::tuple CyCity::getWallOverridePoints() const
{
	python::tuple tup = python::make_tuple();
	if (m_pCity)
	{
		std::vector< std::pair<float, float> > pointsVec = m_pCity->getWallOverridePoints();
		uint i;
		for(i=0;i<pointsVec.size();i++)
			tup += python::make_tuple(pointsVec[i].first, pointsVec[i].second);
	}

	return tup;
}

bool CyCity::AI_avoidGrowth()
{
	return m_pCity ? m_pCity->AI_avoidGrowth() : false;
}

bool CyCity::AI_isEmphasize(int iEmphasizeType)
{
	return m_pCity ? m_pCity->AI_isEmphasize((EmphasizeTypes)iEmphasizeType) : false;
}

int CyCity::AI_countBestBuilds(CyArea* pArea)
{
	return m_pCity ? m_pCity->AI_countBestBuilds(pArea->getArea()) : -1;
}

int CyCity::AI_cityValue()
{
	return m_pCity ? m_pCity->AI_cityValue() : -1;
}

std::string CyCity::getScriptData() const
{
	return m_pCity ? m_pCity->getScriptData() : "";
}

void CyCity::setScriptData(std::string szNewValue)
{
	if (m_pCity)
		m_pCity->setScriptData(szNewValue);
}

int CyCity::visiblePopulation()
{
	if ( m_pCity )
	{
		return m_pCity->visiblePopulation();
	}

	return 0;
}

int CyCity::getBuildingYieldChange(int /*BuildingTypes*/ eBuilding, int /*YieldTypes*/ eYield) const
{
	return m_pCity ? m_pCity->getBuildingYieldChange((BuildingTypes)eBuilding, (YieldTypes)eYield) : 0;
}

void CyCity::setBuildingYieldChange(int /*BuildingTypes*/ eBuilding, int /*YieldTypes*/ eYield, int iChange)
{
	if (m_pCity)
	{
		m_pCity->setBuildingYieldChange((BuildingTypes)eBuilding, (YieldTypes)eYield, iChange);
	}
}

int CyCity::getBuildingCommerceChange(int /*BuildingTypes*/ eBuilding, int /*CommerceTypes*/ eCommerce) const
{
	return m_pCity ? m_pCity->getBuildingCommerceChange((BuildingTypes)eBuilding, (CommerceTypes)eCommerce) : 0;
}

void CyCity::setBuildingCommerceChange(int /*BuildingTypes*/ eBuilding, int /*CommerceTypes*/ eCommerce, int iChange)
{
	if (m_pCity)
	{
		m_pCity->setBuildingCommerceChange((BuildingTypes)eBuilding, (CommerceTypes)eCommerce, iChange);
	}
}

int CyCity::getBuildingHappyChange(int /*BuildingTypes*/ eBuilding) const
{
	return m_pCity ? m_pCity->getBuildingHappyChange((BuildingTypes)eBuilding) : 0;
}

void CyCity::setBuildingHappyChange(int /*BuildingTypes*/ eBuilding, int iChange)
{
	if (m_pCity)
	{
		m_pCity->setBuildingHappyChange((BuildingTypes)eBuilding, iChange);
	}
}

int CyCity::getBuildingHealthChange(int /*BuildingTypes*/ eBuilding) const
{
	return m_pCity ? m_pCity->getBuildingHealthChange((BuildingTypes)eBuilding) : 0;
}

void CyCity::setBuildingHealthChange(int /*BuildingTypes*/ eBuilding, int iChange)
{
	if (m_pCity)
	{
		m_pCity->setBuildingHealthChange((BuildingTypes)eBuilding, iChange);
	}
}

int CyCity::getLiberationPlayer(bool bConquest)
{
	return (m_pCity ? m_pCity->getLiberationPlayer(bConquest) : -1);
}

void CyCity::liberate(bool bConquest)
{
	if (m_pCity)
	{
		m_pCity->liberate(bConquest);
	}
}

/************************************************************************************************/
/* REVOLUTION_MOD                         02/01/09                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int /*UnitTypes*/ CyCity::AI_bestUnit()
{
	int iDummyValue;

	return (m_pCity ? m_pCity->AI_bestUnit(iDummyValue) : -1);
}

int /*UnitTypes*/ CyCity::AI_bestUnitAI(int iUnitAIType)
{
	int iDummyValue;

	return (m_pCity ? m_pCity->AI_bestUnitAI((UnitAITypes)iUnitAIType, iDummyValue) : -1);
}

/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

/************************************************************************************************/
/* Afforess	                  Start		 12/20/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int CyCity::getCivicHappiness() const
{
	return (m_pCity ? m_pCity->getCivicHappiness() : 0);
}

int CyCity::getNumCityPlots() const
{
	return (m_pCity ? m_pCity->getNumCityPlots() : 0);
}

int CyCity::getPopulationgrowthratepercentage() const
{
	return (m_pCity ? m_pCity->getPopulationgrowthratepercentage() : 0);
}

void CyCity::changePopulationgrowthratepercentage(int iChange)
{
	if (m_pCity)
	{
		m_pCity->changePopulationgrowthratepercentage(iChange, true);
	}
}

bool CyCity::isValidTerrainForBuildings(int /*BuildingTypes*/ eIndex) const
{
	return (m_pCity ? m_pCity->isValidTerrainForBuildings((BuildingTypes)eIndex) : false);
}

bool CyCity::hasFreshWater() const
{
	return (m_pCity ? m_pCity->hasFreshWater() : false);
}

bool CyCity::canUpgradeUnit(int /*UnitTypes*/ eUnit) const
{
	return (m_pCity ? m_pCity->canUpgradeUnit((UnitTypes)eUnit) : false);
}

void CyCity::changeUnitProductionModifier(int /*UnitTypes*/ eIndex, int iChange)
{
	if (m_pCity)
	{
		m_pCity->changeUnitProductionModifier((UnitTypes)eIndex, iChange);
	}
}

int CyCity::getBuildingProductionModifier(int /*BuildingTypes*/ eIndex) const
{
	return (m_pCity ? m_pCity->getBuildingProductionModifier((BuildingTypes)eIndex) : 0);
}

void CyCity::changeBuildingProductionModifier(int /*BuildingTypes*/ eIndex, int iChange)
{
	if (m_pCity)
	{
		m_pCity->changeBuildingProductionModifier((BuildingTypes)eIndex, iChange);
	}
}

int CyCity::getBonusDefenseChanges(int /*BonusTypes*/ eIndex) const
{
	return (m_pCity ? m_pCity->getBonusDefenseChanges((BonusTypes)eIndex) : 0);
}

void CyCity::changeBonusDefenseChanges(int /*BonusTypes*/ eIndex, int iChange)
{
	if (m_pCity)
	{
		m_pCity->changeBonusDefenseChanges((BonusTypes)eIndex, iChange);
	}
}

int CyCity::getBonusCommerceRateModifier(int /*CommerceTypes*/ eIndex) const
{
	return (m_pCity ? m_pCity->getBonusCommerceRateModifier((CommerceTypes)eIndex) : 0);
}

void CyCity::changeBonusCommerceRateModifier(int /*CommerceTypes*/ eIndex, int iChange)
{
	if (m_pCity)
	{
		m_pCity->changeBonusCommerceRateModifier((CommerceTypes)eIndex, iChange);
	}
}

bool CyCity::AI_isDefended(int iExtra)
{
	return (m_pCity ? m_pCity->AI_isDefended(iExtra*GET_PLAYER(m_pCity->getOwner()).strengthOfBestUnitAI(DOMAIN_LAND, UNITAI_CITY_DEFENSE)) : 0);
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
	if (m_pCity)
		m_pCity->setWeLoveTheKingDay(bWeLoveTheKingDay);
}  

int CyCity::calculateCorporateTaxes() const
{
	return m_pCity ? m_pCity->calculateCorporateTaxes() : 0;
}

void CyCity::changePowerCount(int iChange, bool bDirty)
{
	if (m_pCity)
		m_pCity->changePowerCount(iChange, bDirty);
} 

int CyCity::getEventAnger()
{
	return m_pCity ? m_pCity->getEventAnger() : 0;
}

void CyCity::changeEventAnger(int iChange)
{
	if (m_pCity)
		m_pCity->changeEventAnger(iChange);
} 

int CyCity::getNumPopulationEmployed()
{
	return m_pCity ? m_pCity->getNumPopulationEmployed() : 0;
}

int CyCity::getBonusCommercePercentChanges(int eIndex, int eBuilding)
{
	return m_pCity ? m_pCity->getBonusCommercePercentChanges((CommerceTypes)eIndex, (BuildingTypes)eBuilding) : 0;
}

bool CyCity::isAutomatedCanBuild(int /*BuildTypes*/ eIndex)	
{
	return m_pCity ? m_pCity->isAutomatedCanBuild((BuildTypes)eIndex) : true;
}

void CyCity::setAutomatedCanBuild(int /*BuildTypes*/ eIndex, bool bNewValue)
{
	if (m_pCity)
		m_pCity->setAutomatedCanBuild((BuildTypes)eIndex, bNewValue);
}

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

CvProperties* CyCity::getProperties()
{
	if (m_pCity)
		return m_pCity->getProperties();
	return NULL;
}


void CyCity::setBuildingListInvalid()
{
	if (m_pCity)
		m_pCity->setBuildingListInvalid();
}

bool CyCity::getBuildingListFilterActive(int eFilter)
{
	return m_pCity ? m_pCity->getBuildingListFilterActive((BuildingFilterTypes)eFilter) : true;
}

void CyCity::setBuildingListFilterActive(int eFilter, bool bActive)
{
	if (m_pCity)
		m_pCity->setBuildingListFilterActive((BuildingFilterTypes)eFilter, bActive);
}

int CyCity::getBuildingListGrouping()
{
	return m_pCity ? m_pCity->getBuildingListGrouping() : 0;
}

void CyCity::setBuildingListGrouping(int eGrouping)
{
	if (m_pCity)
		m_pCity->setBuildingListGrouping((BuildingGroupingTypes)eGrouping);
}

int CyCity::getBuildingListSorting()
{
	return m_pCity ? m_pCity->getBuildingListSorting() : 0;
}

void CyCity::setBuildingListSorting(int eSorting)
{
	if (m_pCity)
		m_pCity->setBuildingListSorting((BuildingSortTypes)eSorting);
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

int CyCity::getBuildingListSelectedBuildingRow()
{
	return m_pCity ? m_pCity->getBuildingListSelectedBuildingRow() : 0;
}

int CyCity::getBuildingListSelectedWonderRow()
{
	return m_pCity ? m_pCity->getBuildingListSelectedWonderRow() : 0;
}

void CyCity::setBuildingListSelectedBuilding(int eBuilding)
{
	if (m_pCity)
		m_pCity->setBuildingListSelectedBuilding((BuildingTypes)eBuilding);
}

void CyCity::setBuildingListSelectedWonder(int eWonder)
{
	if (m_pCity)
		m_pCity->setBuildingListSelectedWonder((BuildingTypes)eWonder);
}

int CyCity::getBuildingListSelectedBuilding()
{
	return m_pCity ? m_pCity->getBuildingListSelectedBuilding() : NO_BUILDING;
}

int CyCity::getBuildingListSelectedWonder()
{
	return m_pCity ? m_pCity->getBuildingListSelectedWonder() : NO_BUILDING;
}

void CyCity::setUnitListInvalid()
{
	if (m_pCity)
		m_pCity->setUnitListInvalid();
}

bool CyCity::getUnitListFilterActive(int eFilter)
{
	return m_pCity ? m_pCity->getUnitListFilterActive((UnitFilterTypes)eFilter) : true;
}

void CyCity::setUnitListFilterActive(int eFilter, bool bActive)
{
	if (m_pCity)
		m_pCity->setUnitListFilterActive((UnitFilterTypes)eFilter, bActive);
}

int CyCity::getUnitListGrouping()
{
	return m_pCity ? m_pCity->getUnitListGrouping() : 0;
}

void CyCity::setUnitListGrouping(int eGrouping)
{
	if (m_pCity)
		m_pCity->setUnitListGrouping((UnitGroupingTypes)eGrouping);
}

int CyCity::getUnitListSorting()
{
	return m_pCity ? m_pCity->getUnitListSorting() : 0;
}

void CyCity::setUnitListSorting(int eSorting)
{
	if (m_pCity)
		m_pCity->setUnitListSorting((UnitSortTypes)eSorting);
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

int CyCity::getUnitListSelectedRow()
{
	return m_pCity ? m_pCity->getUnitListSelectedRow() : 0;
}

void CyCity::setUnitListSelected(int eUnit)
{
	if (m_pCity)
		m_pCity->setUnitListSelected((UnitTypes)eUnit);
}

int CyCity::getUnitListSelected()
{
	return m_pCity ? m_pCity->getUnitListSelected() : NO_UNIT;
}

int CyCity::localCitizenCaptureResistance() const														
{
	return m_pCity ? m_pCity->localCitizenCaptureResistance() : 0;
}

int CyCity::getPrioritorizedSpecialist() const														
{
	return m_pCity ? m_pCity->getPrioritorizedSpecialist() : 0;
}

void CyCity::setPrioritorizedSpecialist(int eSpecialist)
{
	if (m_pCity)
		m_pCity->setPrioritorizedSpecialist((SpecialistTypes)eSpecialist);
}

bool CyCity::isSpecialistBanned(int eSpecialist)														
{
	return m_pCity ? m_pCity->isSpecialistBanned((SpecialistTypes)eSpecialist) : false;
}

void CyCity::banSpecialist(int eSpecialist)
{
	if (m_pCity)
		m_pCity->banSpecialist((SpecialistTypes)eSpecialist);
}

void CyCity::removeSpecialistBan(int eSpecialist)
{
	if (m_pCity)
		m_pCity->removeSpecialistBan((SpecialistTypes)eSpecialist);
}

bool CyCity::isEventOccured(int eEvent) const
{
	return m_pCity ? m_pCity->isEventOccured((EventTypes)eEvent) : false;
}
