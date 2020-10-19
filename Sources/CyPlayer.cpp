#include "CvGameCoreDLL.h"
#include "CvGameAI.h"
#include "CvPlayerAI.h"
#include "CyArea.h"
#include "CyCity.h"
#include "CyPlayer.h"
#include "CyPlot.h"
#include "CySelectionGroup.h"
#include "CyUnit.h"

//
// Python wrapper class for CvPlayer 
//

CyPlayer::CyPlayer() : m_pPlayer(NULL)
{
}

CyPlayer::CyPlayer(CvPlayer* pPlayer) : m_pPlayer(pPlayer)
{
}

#ifdef PARALLEL_MAPS
void CyPlayer::updateMembers()
{
	m_pPlayer->updateMembers();
}

void CyPlayer::initMembers(int iIndex)
{
	m_pPlayer->initMembers(iIndex);
}
#endif
/************************************************************************************************/
/* CHANGE_PLAYER                         08/27/08                                 jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
void CyPlayer::changeLeader(int /*LeaderHeadTypes*/ eNewLeader)
{
	m_pPlayer->changeLeader((LeaderHeadTypes)eNewLeader);
}

void CyPlayer::changeCiv(int /*CivilizationTypes*/ eNewCiv)
{
	m_pPlayer->changeCiv((CivilizationTypes)eNewCiv);
}

void CyPlayer::setIsHuman(bool bNewValue)
{
	m_pPlayer->setIsHuman(bNewValue);
}
/************************************************************************************************/
/* CHANGE_PLAYER                          END                                                   */
/************************************************************************************************/
/************************************************************************************************/
/* REVOLUTION_MOD                         01/01/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
void CyPlayer::setIsRebel(bool bNewValue)
{
	m_pPlayer->setIsRebel(bNewValue);
}

bool CyPlayer::isRebel()
{
	return m_pPlayer->isRebel();
}

int CyPlayer::getStabilityIndex()
{
	return m_pPlayer->getStabilityIndex();
}

void CyPlayer::changeStabilityIndex(int iChange)
{
	m_pPlayer->changeStabilityIndex(iChange);
}

int CyPlayer::getStabilityIndexAverage()
{
	return m_pPlayer->getStabilityIndexAverage();
}

void CyPlayer::updateStabilityIndexAverage()
{
	m_pPlayer->updateStabilityIndexAverage();
}

void CyPlayer::setColor(int /*PlayerColorTypes*/ iColor)
{
	m_pPlayer->setColor((PlayerColorTypes)iColor);
}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

bool CyPlayer::isDoNotBotherStatus(int /*PlayerTypes*/ playerID)
{
	return m_pPlayer->isDoNotBotherStatus((PlayerTypes)playerID);
}

int CyPlayer::startingPlotRange()
{
	return m_pPlayer->startingPlotRange();
}

bool CyPlayer::startingPlotWithinRange(CyPlot *pPlot, int /*PlayerTypes*/ ePlayer, int iRange, int iPass)
{
	if (pPlot != NULL && !pPlot->isNone())
	{
		CvPlot *pcvPlot = pPlot->getPlot();
		return m_pPlayer->startingPlotWithinRange(pcvPlot, (PlayerTypes)ePlayer, iRange, iPass);
	}
	return NULL;
}

CyPlot* CyPlayer::findStartingPlot(bool bRandomize) 
{
	return new CyPlot(m_pPlayer->findStartingPlot(bRandomize));
}

CyCity* CyPlayer::initCity(int x, int y) 
{
	return new CyCity(m_pPlayer->initCity(x, y, true, true));
}

void CyPlayer::acquireCity(CyCity* pCity, bool bConquest, bool bTrade)
{
	m_pPlayer->acquireCity(pCity->getCity(), bConquest, bTrade, true);
}

void CyPlayer::killCities()
{
	m_pPlayer->killCities();
}

std::wstring CyPlayer::getNewCityName()
{
	return m_pPlayer->getNewCityName();
}

CyUnit* CyPlayer::initUnit(int /*UnitTypes*/ iIndex, int iX, int iY, UnitAITypes eUnitAI, DirectionTypes eFacingDirection)
{
/************************************************************************************************/
/* Afforess	                  Start		 09/29/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if( m_pPlayer && iIndex == -1)
	{
		FErrorMsg("Initiating NO_UNIT Type!");
		return NULL;
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	return new CyUnit(m_pPlayer->initUnit((UnitTypes) iIndex, iX, iY, eUnitAI, eFacingDirection, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark")));
}

void CyPlayer::killUnits()
{
	m_pPlayer->killUnits();
}

bool CyPlayer::hasTrait(int /*TraitTypes*/ iIndex)
{
	return m_pPlayer->hasTrait((TraitTypes) iIndex);
}

bool CyPlayer::isHuman()
{
	return m_pPlayer->isHuman();
}

/************************************************************************************************/
/* REVOLUTION_MOD                                                                 lemmy101      */
/*                                                                                jdog5000      */
/*                                                                                              */
/************************************************************************************************/
bool CyPlayer::isHumanDisabled()
{
	return m_pPlayer->isHumanDisabled();
}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

bool CyPlayer::isBarbarian()
{
	return m_pPlayer->isBarbarian();
}

bool CyPlayer::isNPC()
{
	return m_pPlayer->isNPC();
}

bool CyPlayer::isHominid()
{
	return m_pPlayer->isHominid();
}

std::wstring CyPlayer::getName()
{
	return m_pPlayer->getName();
}

/************************************************************************************************/
/* REVOLUTION_MOD                         01/01/08                                jdog5000      */
/*                                                                                              */
/* For dynamic civ names                                                                        */
/************************************************************************************************/
void CyPlayer::setName(std::wstring szNewValue)
{
	m_pPlayer->setName(szNewValue);
}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
std::wstring CyPlayer::getNameForm(int iForm)
{
	return m_pPlayer->getName((uint)iForm);
}

std::wstring CyPlayer::getNameKey()
{
	return m_pPlayer->getNameKey();
}

std::wstring CyPlayer::getCivilizationDescription(int iForm)
{
	return m_pPlayer->getCivilizationDescription((uint)iForm);
}

/************************************************************************************************/
/* REVOLUTION_MOD                         01/01/08                                jdog5000      */
/*                                                                                              */
/* For dynamic civ names                                                                        */
/************************************************************************************************/
void CyPlayer::setCivName(std::wstring szNewDesc, std::wstring szNewShort, std::wstring szNewAdj)
{
	m_pPlayer->setCivName(szNewDesc, szNewShort, szNewAdj);
}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

std::wstring CyPlayer::getCivilizationDescriptionKey()
{
	return m_pPlayer->getCivilizationDescriptionKey();
}

std::wstring CyPlayer::getCivilizationShortDescription(int iForm)
{
	return m_pPlayer->getCivilizationShortDescription((uint)iForm);
}

std::wstring CyPlayer::getCivilizationShortDescriptionKey()
{
	return m_pPlayer->getCivilizationShortDescriptionKey();
}

std::wstring CyPlayer::getCivilizationAdjective(int iForm)
{
	return m_pPlayer->getCivilizationAdjective((uint)iForm);
}

std::wstring CyPlayer::getCivilizationAdjectiveKey()
{
	return m_pPlayer->getCivilizationAdjectiveKey();
}

std::wstring CyPlayer::getFlagDecal()
{
	return m_pPlayer->getFlagDecal();
}

bool CyPlayer::isWhiteFlag()
{
	return m_pPlayer->isWhiteFlag();
}

std::wstring CyPlayer::getStateReligionName(int iForm)
{
	return m_pPlayer->getStateReligionName((int)iForm);
}

std::wstring CyPlayer::getStateReligionKey()
{
	return m_pPlayer->getStateReligionKey();
}

std::wstring CyPlayer::getBestAttackUnitName(int iForm)
{
	return m_pPlayer->getBestAttackUnitName((uint)iForm);
}

std::wstring CyPlayer::getWorstEnemyName()
{
	return m_pPlayer->getWorstEnemyName();
}

int /*ArtStyleTypes*/ CyPlayer::getArtStyleType()
{
	return (int) m_pPlayer->getArtStyleType();
}

std::string CyPlayer::getUnitButton(int eUnit)
{
	return m_pPlayer->getUnitButton((UnitTypes)eUnit);
}

int CyPlayer::countReligionSpreadUnits(CyArea* pArea, int /*ReligionTypes*/ eReligion)
{
	return m_pPlayer->countReligionSpreadUnits(pArea->getArea(), (ReligionTypes) eReligion);
}

int CyPlayer::countNumCoastalCities()
{
	return m_pPlayer->countNumCoastalCities();
}

int CyPlayer::countOwnedBonuses(int /*BonusTypes*/ eBonus)
{
	return m_pPlayer->countOwnedBonuses((BonusTypes)eBonus);
}

int CyPlayer::countNumBuildings(int /*BuildingTypes*/ eBuilding)
{
	return m_pPlayer->countNumBuildings((BuildingTypes) eBuilding);
}

int CyPlayer::countNumCitiesConnectedToCapital()
{
	return m_pPlayer->countNumCitiesConnectedToCapital();
}

bool CyPlayer::canContact(int /*PlayerTypes*/ ePlayer)
{
	return m_pPlayer->canContact((PlayerTypes)ePlayer);
}

void CyPlayer::contact(int /*PlayerTypes*/ ePlayer)
{
	m_pPlayer->contact((PlayerTypes)ePlayer);
}

bool CyPlayer::canTradeWith(int /*PlayerTypes*/ eWhoTo)
{
	return m_pPlayer->canTradeWith((PlayerTypes)eWhoTo);
}

bool CyPlayer::canTradeItem(int /*PlayerTypes*/ eWhoTo, TradeData item, bool bTestDenial)
{
	return m_pPlayer->canTradeItem((PlayerTypes)eWhoTo, item, bTestDenial);
}

DenialTypes CyPlayer::getTradeDenial(int /*PlayerTypes*/ eWhoTo, TradeData item)
{
	return m_pPlayer->getTradeDenial((PlayerTypes)eWhoTo, item);
}

bool CyPlayer::canTradeNetworkWith(int /*PlayerTypes*/ iPlayer)
{
	return m_pPlayer->canTradeNetworkWith((PlayerTypes)iPlayer);
}

int CyPlayer::getNumAvailableBonuses(int /*BonusTypes*/ eBonus)
{
	return m_pPlayer->getNumAvailableBonuses((BonusTypes)eBonus);
}

int CyPlayer::getNumTradeableBonuses(int /*BonusTypes*/ eBonus)
{
	return m_pPlayer->getNumTradeableBonuses((BonusTypes)eBonus);
}

bool CyPlayer::hasBonus(int /*BonusTypes*/ eBonus)
{
	return m_pPlayer->hasBonus((BonusTypes)eBonus);
}

bool CyPlayer::isTurnActive()
{
	return m_pPlayer->isTurnActive();
}

void CyPlayer::receiveGoody(CyPlot* pPlot, int /*GoodyTypes*/ iIndex, CyUnit* pUnit)
{
	m_pPlayer->receiveGoody(pPlot->getPlot(), (GoodyTypes) iIndex, pUnit->getUnit());
}

void CyPlayer::doGoody(CyPlot* pPlot, CyUnit* pUnit)
{
	m_pPlayer->doGoody(pPlot->getPlot(), pUnit->getUnit());
}

bool CyPlayer::canFound(int iX, int iY)
{
	return m_pPlayer->canFound(iX, iY);
}

void CyPlayer::found(int x, int y) 
{
	m_pPlayer->found(x,y);
}

bool CyPlayer::canTrain(int /*UnitTypes*/ eUnit, bool bContinue, bool bTestVisible)
{
	return m_pPlayer->canTrain((UnitTypes)eUnit, bContinue, bTestVisible);
}

bool CyPlayer::canConstruct(int /*BuildingTypes*/eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost)
{
	return m_pPlayer->canConstruct((BuildingTypes)eBuilding, bContinue, bTestVisible, bIgnoreCost);
}

bool CyPlayer::canCreate(int /*ProjectTypes*/ eProject, bool bContinue, bool bTestVisible)
{
	return m_pPlayer->canCreate((ProjectTypes)eProject, bContinue, bTestVisible);
}

bool CyPlayer::canMaintain(int /*ProcessTypes*/ eProcess, bool bContinue)
{
	return m_pPlayer->canMaintain((ProcessTypes)eProcess, bContinue);
}

int CyPlayer::getUnitProductionNeeded(int /*UnitTypes*/ iIndex)
{
	return m_pPlayer->getProductionNeeded((UnitTypes) iIndex);
}

int CyPlayer::getBuildingProductionNeeded(int /*BuildingTypes*/ iIndex)
{
	return m_pPlayer->getProductionNeeded((BuildingTypes) iIndex);
}

int CyPlayer::getProjectProductionNeeded(int /*ProjectTypes*/ iIndex)
{
	return m_pPlayer->getProductionNeeded((ProjectTypes)iIndex);
}

bool CyPlayer::canBuild(CyPlot* pPlot, int /*BuildTypes*/ eBuild, bool bTestEra, bool bTestVisible)
{
	return m_pPlayer->canBuild(pPlot->getPlot(), (BuildTypes)eBuild, bTestEra, bTestVisible);
}

int /*RouteTypes*/ CyPlayer::getBestRoute(CyPlot* pPlot) const
{
	return (int) m_pPlayer->getBestRoute(NULL != pPlot ? pPlot->getPlot() : NULL);
}

int CyPlayer::calculateTotalYield(int /*YieldTypes*/ eYield)
{
	return m_pPlayer->calculateTotalYield((YieldTypes)eYield);
}

int CyPlayer::calculateTotalExports(int /*YieldTypes*/ eYield)
{
	return m_pPlayer->calculateTotalExports((YieldTypes)eYield);
}

int CyPlayer::calculateTotalImports(int /*YieldTypes*/ eYield)
{
	return m_pPlayer->calculateTotalImports((YieldTypes)eYield);
}

int CyPlayer::calculateTotalCityHappiness()
{
	return m_pPlayer->calculateTotalCityHappiness();
}

int CyPlayer::calculateTotalCityUnhappiness()
{
	return m_pPlayer->calculateTotalCityUnhappiness();
}

int CyPlayer::calculateTotalCityHealthiness()
{
	return m_pPlayer->calculateTotalCityHealthiness();
}

int CyPlayer::calculateTotalCityUnhealthiness()
{
	return m_pPlayer->calculateTotalCityUnhealthiness();
}

int64_t CyPlayer::getFinalUnitUpkeep()
{
	return m_pPlayer->getFinalUnitUpkeep();
}

int CyPlayer::calculateUnitSupply()
{
	return m_pPlayer->calculateUnitSupply();
}

int64_t CyPlayer::calculatePreInflatedCosts()
{
	return m_pPlayer->calculatePreInflatedCosts();
}

int64_t CyPlayer::calculateInflatedCosts()
{
	return m_pPlayer->calculateInflatedCosts();
}

int CyPlayer::calculateGoldRate()
{
	return m_pPlayer->calculateGoldRate();
}

int CyPlayer::calculateTotalCommerce()
{
	return m_pPlayer->calculateTotalCommerce();
}

int CyPlayer::calculateResearchRate(int /*TechTypes*/ eTech)
{
	return m_pPlayer->calculateResearchRate((TechTypes)eTech);
}

int CyPlayer::calculateResearchModifier(int /*TechTypes*/ eTech)
{
	return m_pPlayer->calculateResearchModifier((TechTypes)eTech);
}

int CyPlayer::calculateBaseNetResearch()
{
	return m_pPlayer->calculateBaseNetResearch();
}

bool CyPlayer::canEverResearch(int /*TechTypes*/ eTech)
{
	return m_pPlayer->canEverResearch((TechTypes)eTech);
}

bool CyPlayer::canResearch(int /*TechTypes*/ eTech)
{
	return m_pPlayer->canResearch((TechTypes)eTech);
}

int /*TechTypes*/ CyPlayer::getCurrentResearch()
{
	return (int) m_pPlayer->getCurrentResearch();
}

bool CyPlayer::isCurrentResearchRepeat()
{
	return m_pPlayer->isCurrentResearchRepeat();
}

int CyPlayer::getResearchTurnsLeft(int /*TechTypes*/ eTech, bool bOverflow)
{
	return m_pPlayer->getResearchTurnsLeft((TechTypes)eTech, bOverflow);
}

bool CyPlayer::isCivic(int /*CivicTypes*/ eCivic)
{
	return m_pPlayer->isCivic((CivicTypes)eCivic);
}

bool CyPlayer::canDoCivics(int /*CivicTypes*/ eCivic)
{
	return m_pPlayer->canDoCivics((CivicTypes)eCivic);
}

bool CyPlayer::canRevolution(int /*CivicTypes**/ paeNewCivics)
{
	return m_pPlayer->canRevolution((CivicTypes*)paeNewCivics);
}

bool CyPlayer::canChangeReligion()
{
	return m_pPlayer->canChangeReligion();
}

bool CyPlayer::canConvert(int /*ReligionTypes*/ iIndex)
{
	return m_pPlayer->canConvert((ReligionTypes)iIndex);
}

void CyPlayer::convert(int /*ReligionTypes*/ iIndex)
{
	m_pPlayer->convert((ReligionTypes)iIndex);
}

bool CyPlayer::hasHolyCity(int /*ReligionTypes*/ eReligion)
{
	return m_pPlayer->hasHolyCity((ReligionTypes)eReligion);
}

int CyPlayer::countHolyCities()
{
	return m_pPlayer->countHolyCities();
}

bool CyPlayer::hasHeadquarters(int /*CorporationTypes*/ eCorporation)
{
	return m_pPlayer->hasHeadquarters((CorporationTypes)eCorporation);
}

int CyPlayer::getCivicAnarchyLength(boost::python::list& /*CivicTypes**/ paeNewCivics)
{
	int* pCivics = NULL;
	gDLL->getPythonIFace()->putSeqInArray(paeNewCivics.ptr() /*src*/, &pCivics /*dst*/);

	int iRet = m_pPlayer->getCivicAnarchyLength((CivicTypes*)pCivics);
	delete [] pCivics;
	return iRet;
}

int CyPlayer::getReligionAnarchyLength()
{
	return m_pPlayer->getReligionAnarchyLength();
}

int CyPlayer::unitsRequiredForGoldenAge()
{
	return m_pPlayer->unitsRequiredForGoldenAge();
}

int CyPlayer::greatPeopleThresholdMilitary()
{
	return m_pPlayer->greatPeopleThresholdMilitary();
}

int CyPlayer::greatPeopleThresholdNonMilitary()
{
	return m_pPlayer->greatPeopleThresholdNonMilitary();
}

int CyPlayer::getGreatGeneralsThresholdModifier() const
{
	return m_pPlayer->getGreatGeneralsThresholdModifier();
}

int CyPlayer::specialistYield(int /*SpecialistTypes*/ eSpecialist, int /*YieldTypes*/ eCommerce)
{
	return m_pPlayer->specialistYield((SpecialistTypes)eSpecialist, (YieldTypes)eCommerce);
}

CyPlot* CyPlayer::getStartingPlot()
{
	return new CyPlot(m_pPlayer->getStartingPlot());
}

void CyPlayer::setStartingPlot(CyPlot* pPlot, bool bUpdateStartDist)
{
	m_pPlayer->setStartingPlot(NULL != pPlot ? pPlot->getPlot() : NULL, bUpdateStartDist);
}

int CyPlayer::getTotalPopulation()
{
	return m_pPlayer->getTotalPopulation();
}

int64_t CyPlayer::getRealPopulation() const
{
	return m_pPlayer->getRealPopulation();
}

int CyPlayer::getTotalLand()
{
	return m_pPlayer->getTotalLand();
}

int64_t CyPlayer::getGold() const
{
	return m_pPlayer->getGold();
}

void CyPlayer::setGold(int64_t iNewValue)
{
	m_pPlayer->setGold(iNewValue);
}

void CyPlayer::changeGold(int64_t iChange)
{
	m_pPlayer->changeGold(iChange);
}

int CyPlayer::getGoldPerTurn()
{
	return m_pPlayer->getGoldPerTurn();
}

int CyPlayer::getAdvancedStartPoints() 
{
	return m_pPlayer->getAdvancedStartPoints();
}

int CyPlayer::getAdvancedStartUnitCost(int /*UnitTypes*/ eUnit, bool bAdd, CyPlot* pPlot)
{
	return m_pPlayer->getAdvancedStartUnitCost((UnitTypes) eUnit, bAdd, NULL != pPlot ? pPlot->getPlot() : NULL);
}

int CyPlayer::getAdvancedStartCityCost(bool bAdd, CyPlot* pPlot)
{
	return m_pPlayer->getAdvancedStartCityCost(bAdd, NULL != pPlot ? pPlot->getPlot() : NULL);
}

int CyPlayer::getAdvancedStartPopCost(bool bAdd, CyCity* pCity)
{
	return m_pPlayer->getAdvancedStartPopCost(bAdd, pCity->getCity());
}

int CyPlayer::getAdvancedStartCultureCost(bool bAdd, CyCity* pCity)
{
	return m_pPlayer->getAdvancedStartCultureCost(bAdd, pCity->getCity());
}

int CyPlayer::getAdvancedStartBuildingCost(int /*BuildingTypes*/ eBuilding, bool bAdd, CyCity* pCity)
{
	return m_pPlayer->getAdvancedStartBuildingCost((BuildingTypes) eBuilding, bAdd, pCity->getCity());
}

int CyPlayer::getAdvancedStartImprovementCost(int /*ImprovementTypes*/ eImprovement, bool bAdd, CyPlot* pPlot)
{
	return m_pPlayer->getAdvancedStartImprovementCost((ImprovementTypes) eImprovement, bAdd, NULL != pPlot ? pPlot->getPlot() : NULL);
}

int CyPlayer::getAdvancedStartRouteCost(int /*RouteTypes*/ eRoute, bool bAdd, CyPlot* pPlot)
{
	return m_pPlayer->getAdvancedStartRouteCost((RouteTypes) eRoute, bAdd, NULL != pPlot ? pPlot->getPlot() : NULL);
}

int CyPlayer::getAdvancedStartTechCost(int /*TechTypes*/ eTech, bool bAdd)
{
	return m_pPlayer->getAdvancedStartTechCost((TechTypes) eTech, bAdd);
}

int CyPlayer::getAdvancedStartVisibilityCost(CyPlot* pPlot)
{
	return m_pPlayer->getAdvancedStartVisibilityCost(NULL != pPlot ? pPlot->getPlot() : NULL);
}

int CyPlayer::getEspionageSpending(int /*TeamTypes*/ eIndex)
{
	return m_pPlayer->getEspionageSpending((TeamTypes) eIndex);
}

bool CyPlayer::canDoEspionageMission(int /*EspionageMissionTypes*/ eMission, int /*PlayerTypes*/ eTargetPlayer, CyPlot* pPlot, int iExtraData)
{
	return m_pPlayer->canDoEspionageMission((EspionageMissionTypes) eMission, (PlayerTypes) eTargetPlayer, NULL != pPlot ? pPlot->getPlot() : NULL, iExtraData, NULL);
}

int CyPlayer::getEspionageMissionCost(int /*EspionageMissionTypes*/ eMission, int /*PlayerTypes*/ eTargetPlayer, CyPlot* pPlot, int iExtraData)
{
	return m_pPlayer->getEspionageMissionCost((EspionageMissionTypes) eMission, (PlayerTypes) eTargetPlayer, NULL != pPlot ? pPlot->getPlot() : NULL, iExtraData);
}

int CyPlayer::getEspionageSpendingWeightAgainstTeam(int /*TeamTypes*/ eIndex)
{
	return m_pPlayer->getEspionageSpendingWeightAgainstTeam((TeamTypes) eIndex);
}

void CyPlayer::setEspionageSpendingWeightAgainstTeam(int /*TeamTypes*/ eIndex, int iValue)
{
	m_pPlayer->setEspionageSpendingWeightAgainstTeam((TeamTypes) eIndex, iValue);
}

void CyPlayer::changeEspionageSpendingWeightAgainstTeam(int /*TeamTypes*/ eIndex, int iChange)
{
	m_pPlayer->changeEspionageSpendingWeightAgainstTeam((TeamTypes) eIndex, iChange);
}

int CyPlayer::getGoldenAgeTurns()
{
	return m_pPlayer->getGoldenAgeTurns();
}

int CyPlayer::getGoldenAgeLength()
{
	return m_pPlayer->getGoldenAgeLength();
}

bool CyPlayer::isGoldenAge()
{
	return m_pPlayer->isGoldenAge();
}

void CyPlayer::changeGoldenAgeTurns(int iChange)
{
	m_pPlayer->changeGoldenAgeTurns(iChange);
}

void CyPlayer::changeNumUnitGoldenAges(int iChange)
{
	m_pPlayer->changeNumUnitGoldenAges(iChange);
}

int CyPlayer::getAnarchyTurns()
{
	return m_pPlayer->getAnarchyTurns();
}

bool CyPlayer::isAnarchy()
{
	return m_pPlayer->isAnarchy();
}

void CyPlayer::changeAnarchyTurns(int iChange)
{
	m_pPlayer->changeAnarchyTurns(iChange);
}

int CyPlayer::getAnarchyModifier()
{
	return m_pPlayer->getAnarchyModifier();
}

int CyPlayer::getGoldenAgeModifier()
{
	return m_pPlayer->getGoldenAgeModifier();
}

void CyPlayer::changeGreatGeneralsThresholdModifier(int iChange)
{
	m_pPlayer->changeGreatGeneralsThresholdModifier(iChange);
}

int CyPlayer::getGreatPeopleRateModifier()
{
	return m_pPlayer->getGreatPeopleRateModifier();
}

int CyPlayer::getGreatGeneralRateModifier()
{
	return m_pPlayer->getGreatGeneralRateModifier();
}

int CyPlayer::getFreeExperience()
{
	return m_pPlayer->getFreeExperience();
}

int CyPlayer::getFeatureProductionModifier()
{
	return m_pPlayer->getFeatureProductionModifier();
}

int CyPlayer::getWorkerSpeedModifier()
{
	return m_pPlayer->getWorkerSpeedModifier();
}

int CyPlayer::getMilitaryProductionModifier()
{
	return m_pPlayer->getMilitaryProductionModifier();
}

int CyPlayer::getSpaceProductionModifier()
{
	return m_pPlayer->getSpaceProductionModifier();
}

int CyPlayer::getBestUnitType(int /*UnitAITypes*/ eUnitAI) const
{
	return (int) m_pPlayer->getBestUnitType(UnitAITypes(eUnitAI));
}

bool CyPlayer::isNonStateReligionCommerce() const
{
	return m_pPlayer->isNonStateReligionCommerce();
}

int CyPlayer::getRevIdxLocal()
{
	return m_pPlayer->getRevIdxLocal();
}

int CyPlayer::getRevIdxNational()
{
	return m_pPlayer->getRevIdxNational();
}

int CyPlayer::getRevIdxDistanceModifier()
{
	return m_pPlayer->getRevIdxDistanceModifier();
}

int CyPlayer::getRevIdxHolyCityGood()
{
	return m_pPlayer->getRevIdxHolyCityGood();
}

int CyPlayer::getRevIdxHolyCityBad()
{
	return m_pPlayer->getRevIdxHolyCityBad();
}

float CyPlayer::getRevIdxNationalityMod()
{
	return m_pPlayer->getRevIdxNationalityMod();
}

float CyPlayer::getRevIdxBadReligionMod()
{
	return m_pPlayer->getRevIdxBadReligionMod();
}

float CyPlayer::getRevIdxGoodReligionMod()
{
	return m_pPlayer->getRevIdxGoodReligionMod();
}

bool CyPlayer::canFoundReligion()
{
	return m_pPlayer->canFoundReligion();
}

int CyPlayer::getNumMilitaryUnits()
{
	return m_pPlayer->getNumMilitaryUnits();
}

int CyPlayer::getOverflowResearch()
{
	return m_pPlayer->getOverflowResearch();
}

bool CyPlayer::isBuildingOnlyHealthy()
{
	return m_pPlayer->isBuildingOnlyHealthy();
}

int64_t CyPlayer::getTreasuryUpkeep()
{
	return m_pPlayer->getTreasuryUpkeep();
}

int CyPlayer::getTotalMaintenance()
{
	return m_pPlayer->getTotalMaintenance();
}

int CyPlayer::getLevelExperienceModifier() const
{
	return m_pPlayer->getLevelExperienceModifier();
}

int CyPlayer::getExtraHealth()
{
	return m_pPlayer->getExtraHealth();
}

void CyPlayer::changeExtraHealth(int iChange)
{
	m_pPlayer->changeExtraHealth(iChange);
}

int CyPlayer::getExtraHappiness()
{
	return m_pPlayer->getExtraHappiness();
}

void CyPlayer::changeExtraHappiness(int iChange)
{
	m_pPlayer->changeExtraHappiness(iChange, true);
}

int CyPlayer::getBuildingHappiness()
{
	return m_pPlayer->getBuildingHappiness();
}

int CyPlayer::getWarWearinessPercentAnger()
{
	return m_pPlayer->getWarWearinessPercentAnger();
}

int CyPlayer::getWarWearinessModifier()
{
	return m_pPlayer->getWarWearinessModifier();
}

int CyPlayer::getCoastalTradeRoutes()
{
	return m_pPlayer->getCoastalTradeRoutes();
}

void CyPlayer::changeCoastalTradeRoutes(int iChange)
{
	m_pPlayer->changeCoastalTradeRoutes(iChange);
}

int CyPlayer::getTradeRoutes()
{
	return m_pPlayer->getTradeRoutes();
}

int CyPlayer::getRevolutionTimer()
{
	return m_pPlayer->getRevolutionTimer();
}

void CyPlayer::changeRevolutionTimer(int addTime)
{
	m_pPlayer->changeRevolutionTimer(addTime);
}

bool CyPlayer::isStateReligion()
{
	return m_pPlayer->isStateReligion();
}

int CyPlayer::getStateReligionHappiness()
{
	return m_pPlayer->getStateReligionHappiness();
}

int CyPlayer::getStateReligionUnitProductionModifier()
{
	return m_pPlayer->getStateReligionUnitProductionModifier();
}

void CyPlayer::changeStateReligionUnitProductionModifier(int iChange)
{
	m_pPlayer->changeStateReligionUnitProductionModifier(iChange);
}

int CyPlayer::getStateReligionBuildingProductionModifier()
{
	return m_pPlayer->getStateReligionBuildingProductionModifier();
}

void CyPlayer::changeStateReligionBuildingProductionModifier(int iChange)
{
	m_pPlayer->changeStateReligionBuildingProductionModifier(iChange);
}

int CyPlayer::getStateReligionFreeExperience()
{
	return m_pPlayer->getStateReligionFreeExperience();
}

CyCity* CyPlayer::getCapitalCity()
{
	return new CyCity(m_pPlayer->getCapitalCity());
}

int CyPlayer::getCitiesLost()
{
	return m_pPlayer->getCitiesLost();
}

int CyPlayer::getAssets()
{
	return m_pPlayer->getAssets();
}

int CyPlayer::getPower()
{
	return m_pPlayer->getPower();
}

int CyPlayer::getUnitPower()
{
	return m_pPlayer->getUnitPower();
}

int CyPlayer::getPopScore()
{
	return m_pPlayer->getPopScore();
}

int CyPlayer::getLandScore()
{
	return m_pPlayer->getLandScore();
}

int CyPlayer::getSevoWondersScore(int mode)
{
	return m_pPlayer->getSevoWondersScore(mode);
}

int CyPlayer::getWondersScore()
{
	return m_pPlayer->getWondersScore();
}

int CyPlayer::getTechScore()
{
	return m_pPlayer->getTechScore();
}

bool CyPlayer::isMinorCiv()
{
	return m_pPlayer->isMinorCiv();
}

bool CyPlayer::isAlive() 
{
	return m_pPlayer->isAlive();
}

bool CyPlayer::isEverAlive() 
{
	return m_pPlayer->isEverAlive();
}

bool CyPlayer::isFoundedFirstCity()
{
	return m_pPlayer->isFoundedFirstCity();
}

void CyPlayer::setFoundedFirstCity(bool bNewValue)
{
	m_pPlayer->setFoundedFirstCity(bNewValue);
}

void CyPlayer::setAlive(bool bNewValue)
{
	m_pPlayer->setAlive(bNewValue);
}

void CyPlayer::setNewPlayerAlive(bool bNewValue)
{
	m_pPlayer->setNewPlayerAlive(bNewValue);
}

void CyPlayer::changeTechScore(int iChange)
{
	m_pPlayer->changeTechScore(iChange);
}

bool CyPlayer::isMADNukesEnabled() const
{
	return m_pPlayer->isEnabledMAD();
}

bool CyPlayer::isStrike()
{
	return m_pPlayer->isStrike();
}

int CyPlayer::getID()
{
	return m_pPlayer->getID();
}

int /*HandicapTypes*/ CyPlayer::getHandicapType()
{
	return (int) m_pPlayer->getHandicapType();
}

int /*CivilizationTypes*/ CyPlayer::getCivilizationType() 
{
	return (int) m_pPlayer->getCivilizationType();
}

int /*LeaderHeadTypes*/ CyPlayer::getLeaderType()
{
	return (int) m_pPlayer->getLeaderType();
}

int /*LeaderHeadTypes*/ CyPlayer::getPersonalityType()
{
	return (int) m_pPlayer->getPersonalityType();
}

void CyPlayer::setPersonalityType(int /*LeaderHeadTypes*/ eNewValue)
{
	m_pPlayer->setPersonalityType((LeaderHeadTypes) eNewValue);
}

int /*ErasTypes*/ CyPlayer::getCurrentEra()
{
	return (int) m_pPlayer->getCurrentEra();
}

void CyPlayer::setCurrentEra(int /*EraTypes*/ iNewValue)
{
	m_pPlayer->setCurrentEra((EraTypes) iNewValue);
}

int /*ReligonTypes*/ CyPlayer::getStateReligion()
{
	return (int) m_pPlayer->getStateReligion();
}

void CyPlayer::setLastStateReligion(int /*ReligionTypes*/ iNewReligion)
{
	m_pPlayer->setLastStateReligion((ReligionTypes) iNewReligion);
}

int CyPlayer::getTeam()
{
	return m_pPlayer->getTeam();
}

int /*PlayerColorTypes*/ CyPlayer::getPlayerColor()
{
	return (int) m_pPlayer->getPlayerColor();
}

int CyPlayer::getPlayerTextColorR()
{
	return m_pPlayer->getPlayerTextColorR();
}

int CyPlayer::getPlayerTextColorG()
{
	return m_pPlayer->getPlayerTextColorG();
}

int CyPlayer::getPlayerTextColorB()
{
	return m_pPlayer->getPlayerTextColorB();
}

int CyPlayer::getPlayerTextColorA()
{
	return m_pPlayer->getPlayerTextColorA();
}

int CyPlayer::getSeaPlotYield(YieldTypes eIndex)
{
	return m_pPlayer->getSeaPlotYield(eIndex);
}

int CyPlayer::getYieldRateModifier(YieldTypes eIndex)
{
	return m_pPlayer->getYieldRateModifier(eIndex);
}

int CyPlayer::getCommercePercent(int /*CommerceTypes*/ eIndex)
{
	return m_pPlayer->getCommercePercent((CommerceTypes)eIndex);
}

void CyPlayer::changeCommercePercent(CommerceTypes eIndex, int iChange)
{
	m_pPlayer->changeCommercePercent(eIndex, iChange);
}

int CyPlayer::getCommerceRate(CommerceTypes eIndex)
{
	return m_pPlayer->getCommerceRate(eIndex);
}

int CyPlayer::getCommerceRateModifier(CommerceTypes eIndex)
{
	return m_pPlayer->getCommerceRateModifier(eIndex) + m_pPlayer->getCommerceRateModifierfromEvents(eIndex);
}

int CyPlayer::getCapitalCommerceRateModifier(CommerceTypes eIndex)
{
	return m_pPlayer->getCapitalCommerceRateModifier(eIndex);
}

int CyPlayer::getSpecialistExtraCommerce(CommerceTypes eIndex)
{
	return m_pPlayer->getSpecialistExtraCommerce(eIndex);
}

bool CyPlayer::isCommerceFlexible(int /*CommerceTypes*/ eIndex)
{
	return m_pPlayer->isCommerceFlexible((CommerceTypes)eIndex);
}

bool CyPlayer::isFeatAccomplished(int /*FeatTypes*/ eIndex)	
{
	//TB Something's wrong in the python and this allows me to protect against it.
	if (eIndex < 0)
	{
		FErrorMsg("eIndex is expected to be non-negative (invalid Feat Called by Python and Caught Early)");
		return false;
	}
	if (eIndex >= NUM_FEAT_TYPES)
	{
		FErrorMsg("eIndex is expected to be within maximum bounds (invalid Feat Called by Python and Caught Early)");
		return false;
	}
	return m_pPlayer->isFeatAccomplished((FeatTypes)eIndex);
}

void CyPlayer::setFeatAccomplished(int /*FeatTypes*/ eIndex, bool bNewValue)
{
	//TB Something's wrong in the python and this allows me to protect against it.
	if (eIndex < 0)
	{
		FErrorMsg("eIndex is expected to be non-negative (invalid Feat Called by Python and Caught Early)");
		return;
	}
	if(eIndex >= NUM_FEAT_TYPES)
	{
		FErrorMsg("eIndex is expected to be within maximum bounds (invalid Feat Called by Python and Caught Early)");
		return;
	}
	else if (m_pPlayer)
	{
		m_pPlayer->setFeatAccomplished((FeatTypes)eIndex, bNewValue);
	}
}

bool CyPlayer::isOption(int /*PlayerOptionTypes*/ eIndex)	
{
	return m_pPlayer->isOption((PlayerOptionTypes)eIndex);
}

void CyPlayer::setOption(int /*PlayerOptionTypes*/ eIndex, bool bNewValue)
{
	m_pPlayer->setOption((PlayerOptionTypes)eIndex, bNewValue);
}

int CyPlayer::getVotes(int /*VoteTypes*/ eVote, int /*VoteSourceTypes*/ eVoteSource)
{
	return m_pPlayer->getVotes((VoteTypes)eVote, (VoteSourceTypes)eVoteSource);
}

bool CyPlayer::isFullMember(int /*VoteSourceTypes*/ eVoteSource) const
{
	return m_pPlayer->isFullMember((VoteSourceTypes)eVoteSource);
}

bool CyPlayer::isVotingMember(int /*VoteSourceTypes*/ eVoteSource) const
{
	return m_pPlayer->isVotingMember((VoteSourceTypes)eVoteSource);
}

bool CyPlayer::isPlayable()
{
	return m_pPlayer->isPlayable();
}

int CyPlayer::getBonusExport(int /*BonusTypes*/ iIndex)
{
	return m_pPlayer->getBonusExport((BonusTypes)iIndex);
}

int CyPlayer::getBonusImport(int /*BonusTypes*/ iIndex)
{
	return m_pPlayer->getBonusImport((BonusTypes)iIndex);
}

int CyPlayer::getImprovementCount(int /*ImprovementTypes*/ iIndex)
{
	return m_pPlayer->getImprovementCount((ImprovementTypes)iIndex);
}

int CyPlayer::getExtraBuildingHappiness(int /*BuildingTypes*/ iIndex)
{
	return m_pPlayer->getExtraBuildingHappiness((BuildingTypes)iIndex);
}

int CyPlayer::getExtraBuildingHealth(int /*BuildingTypes*/ iIndex)
{
	return m_pPlayer->getExtraBuildingHealth((BuildingTypes)iIndex);
}

int CyPlayer::getUnitCount(int /*UnitTypes*/ eIndex)
{
	return eIndex >= 0 ? m_pPlayer->getUnitCount((UnitTypes) eIndex) : 0;
}

bool CyPlayer::isUnitMaxedOut(int /*UnitTypes*/ eIndex, int iExtra)
{
	return eIndex >= 0 ? m_pPlayer->isUnitMaxedOut((UnitTypes) eIndex, iExtra);
}

int CyPlayer::getUnitMaking(int /*UnitTypes*/ eIndex)
{
	return eIndex >= 0 ? m_pPlayer->getUnitMaking((UnitTypes) eIndex) : 0;
}

int CyPlayer::getUnitCountPlusMaking(int /*UnitTypes*/ eIndex)
{
	return eIndex >= 0 ? m_pPlayer->getUnitCountPlusMaking((UnitTypes) eIndex) : 0;
}

int CyPlayer::getBuildingCount(int /*BuildingTypes*/ iIndex)
{
	return iIndex >= 0 ? m_pPlayer->getBuildingCount((BuildingTypes)iIndex) : 0;
}

bool CyPlayer::isBuildingMaxedOut(int /*BuildingTypes*/ iIndex, int iExtra)
{
	return iIndex >= 0 ? m_pPlayer->isBuildingMaxedOut((BuildingTypes)iIndex, iExtra);
}

int CyPlayer::getBuildingCountPlusMaking(int /*BuildingTypes*/ iIndex)
{
	return iIndex >= 0 ? m_pPlayer->getBuildingCountPlusMaking((BuildingTypes)iIndex) : 0;
}

bool CyPlayer::canHurry(int /*HurryTypes*/ eIndex)
{
	return m_pPlayer->canHurry((HurryTypes)eIndex);
}

int CyPlayer::getHasReligionCount(int /*ReligionTypes*/ iIndex)
{
	return m_pPlayer->getHasReligionCount((ReligionTypes)iIndex);
}

int CyPlayer::getHasCorporationCount(int /*CorporationTypes*/ iIndex)
{
	return m_pPlayer->getHasCorporationCount((CorporationTypes)iIndex);
}

bool CyPlayer::isSpecialistValid(int /*SpecialistTypes*/ iIndex)
{
	return m_pPlayer->isSpecialistValid((SpecialistTypes)iIndex);
}

bool CyPlayer::isResearchingTech(int /*TechTypes*/ iIndex)
{
	return m_pPlayer->isResearchingTech((TechTypes)iIndex);
}

int /* CivicTypes */ CyPlayer::getCivics(int /*CivicOptionTypes*/ iIndex)
{
	return iIndex != -1 ? m_pPlayer->getCivics((CivicOptionTypes)iIndex);
}

int CyPlayer::getSingleCivicUpkeep(int /*CivicTypes*/ eCivic, bool bIgnoreAnarchy)
{
	return m_pPlayer->getSingleCivicUpkeep((CivicTypes) eCivic, bIgnoreAnarchy);
}

int CyPlayer::getCivicUpkeep(boost::python::list& /*CivicTypes*/ paiCivics, bool bIgnoreAnarchy)
{
	int* pCivics = NULL;
	gDLL->getPythonIFace()->putSeqInArray(paiCivics.ptr() /*src*/, &pCivics /*dst*/);
	int iRet = m_pPlayer->getCivicUpkeep((CivicTypes*)pCivics, bIgnoreAnarchy);
	delete [] pCivics;
	return iRet;
}

void CyPlayer::setCivics(int /*CivicOptionTypes*/ eIndex, int /*CivicTypes*/ eNewValue)
{
	m_pPlayer->setCivics((CivicOptionTypes) eIndex, (CivicTypes) eNewValue);
}

int CyPlayer::getCombatExperience() const
{
	return m_pPlayer->getCombatExperience();
}

void CyPlayer::changeCombatExperience(int iChange)
{
	m_pPlayer->changeCombatExperience(iChange);
}

void CyPlayer::setCombatExperience(int iExperience)
{
	m_pPlayer->setCombatExperience(iExperience);
}

int CyPlayer::getQueuePosition(int /*TechTypes*/ eTech)
{
	return m_pPlayer->getQueuePosition((TechTypes)eTech);
}

void CyPlayer::clearResearchQueue()
{
	m_pPlayer->clearResearchQueue();
}

bool CyPlayer::pushResearch(int /*TechTypes*/ iIndex, bool bClear)
{
	return m_pPlayer->pushResearch((TechTypes)iIndex, bClear);
}

int CyPlayer::getLengthResearchQueue()
{
	return m_pPlayer->getLengthResearchQueue();
}

void CyPlayer::addCityName(std::wstring szName)
{
	m_pPlayer->addCityName(szName);
}

int CyPlayer::getNumCityNames()
{
	return m_pPlayer->getNumCityNames();
}

std::wstring CyPlayer::getCityName(int iIndex)
{
	return m_pPlayer->getCityName(iIndex);
}

// returns tuple of (CyCity, iterOut)
python::tuple CyPlayer::firstCity(bool bRev)
{
	int iterIn = 0;
	CvCity* pvObj = m_pPlayer->firstCity(&iterIn, bRev);
	CyCity* pyObj = pvObj ? new CyCity(pvObj) : NULL;
	python::tuple tup = python::make_tuple(pyObj, iterIn);
	if(pyObj) delete pyObj;
	return tup;
}

// returns tuple of (CyCity, iterOut)
python::tuple CyPlayer::nextCity(int iterIn, bool bRev)
{
	CvCity* pvObj = m_pPlayer->nextCity(&iterIn, bRev);
	CyCity* pyObj = pvObj ? new CyCity(pvObj) : NULL;
	python::tuple tup = python::make_tuple(pyObj, iterIn);
	if(pyObj) delete pyObj;
	return tup;
}

int CyPlayer::getNumCities()
{
	return m_pPlayer->getNumCities();
}

CyCity* CyPlayer::getCity(int iID)
{
	return new CyCity(m_pPlayer->getCity(iID));
}

// returns tuple of (CyUnit, iterOut)
python::tuple CyPlayer::firstUnit(bool bRev)
{
	int iterIn = 0;
	CvUnit* pvUnit = m_pPlayer->firstUnit(&iterIn, bRev);
	CyUnit* pyUnit = pvUnit ? new CyUnit(pvUnit) : NULL;
	python::tuple tup = python::make_tuple(pyUnit, iterIn);
	if(pyUnit) delete pyUnit;
	return tup;
}

// returns tuple of (CyUnit, iterOut)
python::tuple CyPlayer::nextUnit(int iterIn, bool bRev)
{
	CvUnit* pvObj = m_pPlayer->nextUnit(&iterIn, bRev);
	CyUnit* pyObj = pvObj ? new CyUnit(pvObj) : NULL;
	python::tuple tup = python::make_tuple(pyObj, iterIn);
	if(pyObj) delete pyObj;
	return tup;
}

int CyPlayer::getNumUnits()
{
	return m_pPlayer->getNumUnits();
}

CyUnit* CyPlayer::getUnit(int iID)
{
	return new CyUnit(m_pPlayer->getUnit(iID));
}

int CyPlayer::getNumSelectionGroups()
{
	return m_pPlayer->getNumSelectionGroups();
}

CySelectionGroup* CyPlayer::getSelectionGroup(int iID)
{
	return new CySelectionGroup(m_pPlayer->getSelectionGroup(iID));
}

void CyPlayer::trigger(/*EventTriggerTypes*/int eEventTrigger)
{
	m_pPlayer->trigger((EventTriggerTypes)eEventTrigger);
}

const EventTriggeredData* CyPlayer::getEventOccured(int /*EventTypes*/ eEvent) const
{
	return m_pPlayer->getEventOccured((EventTypes)eEvent);
}

void CyPlayer::resetEventOccured(int /*EventTypes*/ eEvent)
{
	m_pPlayer->resetEventOccured((EventTypes)eEvent);
}

EventTriggeredData* CyPlayer::getEventTriggered(int iID) const
{
	return m_pPlayer->getEventTriggered(iID);
}

EventTriggeredData* CyPlayer::initTriggeredData(int /*EventTriggerTypes*/ eEventTrigger, bool bFire, int iCityId, int iPlotX, int iPlotY, int /*PlayerTypes*/ eOtherPlayer, int iOtherPlayerCityId, int /*ReligionTypes*/ eReligion, int /*CorporationTypes*/ eCorporation, int iUnitId, int /*BuildingTypes*/ eBuilding)
{
	return m_pPlayer->initTriggeredData((EventTriggerTypes)eEventTrigger, bFire, iCityId, iPlotX, iPlotY, (PlayerTypes)eOtherPlayer, iOtherPlayerCityId, (ReligionTypes)eReligion, (CorporationTypes)eCorporation, iUnitId, (BuildingTypes)eBuilding);
}

int CyPlayer::getEventTriggerWeight(int /*EventTriggerTypes*/ eTrigger)
{
	return m_pPlayer->getEventTriggerWeight((EventTriggerTypes)eTrigger);
}

void CyPlayer::AI_updateFoundValues(bool bStartingLoc)
{
	//	Koshling - found values are now calculated on demand, so clearing them
	//	will force recalcaultaion as needed
	m_pPlayer->AI_updateFoundValues(true);
}

bool CyPlayer::AI_isFinancialTrouble()
{
	return m_pPlayer->AI_isFinancialTrouble();
}

bool CyPlayer::AI_demandRebukedWar(int /*PlayerTypes*/ ePlayer)
{
	return m_pPlayer->AI_demandRebukedWar((PlayerTypes)ePlayer);
}

AttitudeTypes CyPlayer::AI_getAttitude(int /*PlayerTypes*/ ePlayer)
{
	//Fuyu catching AIAutoplay weirdness
	if (m_pPlayer->getID() == (PlayerTypes)ePlayer)
	{
#ifdef _DEBUG
		FErrorMsg("shouldn't call this function on ourselves (Python)");
		throw new std::exception();
#endif
		return NO_ATTITUDE;
	}
	//Fuyu end

	return m_pPlayer->AI_getAttitude((PlayerTypes)ePlayer);
}

int CyPlayer::AI_unitValue(int /*UnitTypes*/ eUnit, int /*UnitAITypes*/ eUnitAI, CyArea* pArea)
{
	return m_pPlayer->AI_unitValue((UnitTypes)eUnit, (UnitAITypes)eUnitAI, pArea->getArea());
}

int CyPlayer::AI_civicValue(int /*CivicTypes*/ eCivic)
{
	return m_pPlayer->AI_civicValue((CivicTypes)eCivic);
}

int CyPlayer::AI_totalUnitAIs(int /*UnitAITypes*/ eUnitAI)
{
	return m_pPlayer->AI_totalUnitAIs((UnitAITypes)eUnitAI);
}

int CyPlayer::AI_totalAreaUnitAIs(CyArea* pArea, int /*UnitAITypes*/ eUnitAI)
{
	return m_pPlayer->AI_totalAreaUnitAIs(pArea->getArea(), (UnitAITypes)eUnitAI);
}

int CyPlayer::AI_getNumAIUnits(int /*UnitAITypes*/ eIndex)
{
	return m_pPlayer->AI_getNumAIUnits((UnitAITypes)eIndex);
}

int CyPlayer::AI_getAttitudeExtra(int /*PlayerTypes*/ eIndex)
{
	return m_pPlayer->AI_getAttitudeExtra((PlayerTypes)eIndex);
}

void CyPlayer::AI_setAttitudeExtra(int /*PlayerTypes*/ eIndex, int iNewValue)
{
	m_pPlayer->AI_setAttitudeExtra((PlayerTypes)eIndex, iNewValue);
}

void CyPlayer::AI_changeAttitudeExtra(int /*PlayerTypes*/ eIndex, int iChange)
{
	m_pPlayer->AI_changeAttitudeExtra((PlayerTypes)eIndex, iChange);
}

int CyPlayer::AI_getMemoryCount(int /*PlayerTypes*/ eIndex1, int /*MemoryTypes*/ eIndex2)
{
	return m_pPlayer->AI_getMemoryCount((PlayerTypes)eIndex1, (MemoryTypes)eIndex2);
}

void CyPlayer::AI_changeMemoryCount(int /*PlayerTypes*/ eIndex1, int /*MemoryTypes*/ eIndex2, int iChange)
{
	m_pPlayer->AI_changeMemoryCount((PlayerTypes)eIndex1, (MemoryTypes)eIndex2, iChange);
}

bool CyPlayer::AI_isWillingToTalk(int /*PlayerTypes*/ ePlayer)
{
	return m_pPlayer->AI_isWillingToTalk((PlayerTypes)ePlayer);
}

int64_t CyPlayer::getScoreHistory(int iTurn) const
{
	return m_pPlayer->getScoreHistory(iTurn);
}

int64_t CyPlayer::getEconomyHistory(int iTurn) const
{
	return m_pPlayer->getEconomyHistory(iTurn);
}

int64_t CyPlayer::getIndustryHistory(int iTurn) const
{
	return m_pPlayer->getIndustryHistory(iTurn);
}

int64_t CyPlayer::getAgricultureHistory(int iTurn) const
{
	return m_pPlayer->getAgricultureHistory(iTurn);
}

int64_t CyPlayer::getPowerHistory(int iTurn) const
{
	return m_pPlayer->getPowerHistory(iTurn);
}

int64_t CyPlayer::getCultureHistory(int iTurn) const
{
	return m_pPlayer->getCultureHistory(iTurn);
}

int64_t CyPlayer::getEspionageHistory(int iTurn) const
{
	return m_pPlayer->getEspionageHistory(iTurn);
}

int64_t CyPlayer::getRevolutionStabilityHistory(int iTurn) const
{
	return m_pPlayer->getRevolutionStabilityHistory(iTurn);
}

std::string CyPlayer::getScriptData() const
{
	return m_pPlayer->getScriptData();
}

void CyPlayer::setScriptData(std::string szNewValue)
{
	m_pPlayer->setScriptData(szNewValue);
}

int CyPlayer::AI_maxGoldTrade(int iPlayer)
{
	return dynamic_cast<const CvPlayerAI&>(m_pPlayer).AI_maxGoldTrade((PlayerTypes)iPlayer);
}

int CyPlayer::AI_maxGoldPerTurnTrade(int iPlayer)
{
	return dynamic_cast<const CvPlayerAI&>(m_pPlayer).AI_maxGoldPerTurnTrade((PlayerTypes)iPlayer);
}

bool CyPlayer::canSplitEmpire() const
{
	return m_pPlayer->canSplitEmpire();
}

bool CyPlayer::canSplitArea(int iAreaId) const
{
	return m_pPlayer->canSplitArea(iAreaId);
}

bool CyPlayer::assimilatePlayer(int iPlayer)
{
	return m_pPlayer->assimilatePlayer((PlayerTypes) iPlayer);
}

bool CyPlayer::canHaveTradeRoutesWith(int iPlayer)
{
	return m_pPlayer->canHaveTradeRoutesWith((PlayerTypes)iPlayer);
}

void  CyPlayer::forcePeace(int iPlayer)
{
	m_pPlayer->forcePeace((PlayerTypes)iPlayer);
}

void CyPlayer::addReminder(int iGameTurn, std::wstring szMessage) const
{
	m_pPlayer->addReminder(iGameTurn, CvWString(szMessage));
}

int CyPlayer::getBuildingCountWithUpgrades(int iBuilding) const
{
	return m_pPlayer->getBuildingCountWithUpgrades((BuildingTypes)iBuilding);
}

void CyPlayer::setHandicap(int iNewVal)
{
	m_pPlayer->setHandicap(iNewVal);
}

void CyPlayer::setModderOption(int /*ModderOptionTypes*/ eIndex, int iNewValue)
{
	m_pPlayer->setModderOption((ModderOptionTypes)eIndex, iNewValue);
}

void CyPlayer::doRevolution(boost::python::list& /*CivicTypes**/ paeNewCivics, bool bForce)
{
	int* pCivics = NULL;
	gDLL->getPythonIFace()->putSeqInArray(paeNewCivics.ptr() /*src*/, &pCivics /*dst*/);
	if (m_pPlayer)
		m_pPlayer->revolution((CivicTypes*)pCivics, bForce);
	delete [] pCivics;
}

bool CyPlayer::isAutomatedCanBuild(int /*BuildTypes*/ eIndex)	
{
	return m_pPlayer->isAutomatedCanBuild((BuildTypes)eIndex);
}

void CyPlayer::setAutomatedCanBuild(int /*BuildTypes*/ eIndex, bool bNewValue)
{
	m_pPlayer->setAutomatedCanBuild((BuildTypes)eIndex, bNewValue);
}

int64_t CyPlayer::getCulture() const
{
	return m_pPlayer->getCulture();
}

void CyPlayer::setCulture(int64_t iNewValue)
{
	m_pPlayer->setCulture(iNewValue);
}

void CyPlayer::changeCulture(int64_t iAddValue)
{
	m_pPlayer->changeCulture(iAddValue);
}

CvProperties* CyPlayer::getProperties()
{
	return m_pPlayer->getProperties();
}

bool CyPlayer::getBuildingListFilterActive(int eFilter)
{
	return m_pPlayer->getBuildingListFilterActive((BuildingFilterTypes)eFilter);
}

void CyPlayer::setBuildingListFilterActive(int eFilter, bool bActive)
{
	m_pPlayer->setBuildingListFilterActive((BuildingFilterTypes)eFilter, bActive);
}

int CyPlayer::getBuildingListGrouping()
{
	return m_pPlayer->getBuildingListGrouping();
}

void CyPlayer::setBuildingListGrouping(int eGrouping)
{
	m_pPlayer->setBuildingListGrouping((BuildingGroupingTypes)eGrouping);
}

int CyPlayer::getBuildingListSorting()
{
	return m_pPlayer->getBuildingListSorting();
}

void CyPlayer::setBuildingListSorting(int eSorting)
{
	m_pPlayer->setBuildingListSorting((BuildingSortTypes)eSorting);
}

int CyPlayer::getBuildingListGroupNum()
{
	return m_pPlayer->getBuildingListGroupNum();
}

int CyPlayer::getBuildingListNumInGroup(int iGroup)
{
	return m_pPlayer->getBuildingListNumInGroup(iGroup);
}

int CyPlayer::getBuildingListType(int iGroup, int iPos)
{
	return m_pPlayer->getBuildingListType(iGroup, iPos);
}

void CyPlayer::setUnitListInvalid()
{
	m_pPlayer->setUnitListInvalid();
}

bool CyPlayer::getUnitListFilterActive(int eFilter)
{
	return m_pPlayer->getUnitListFilterActive((UnitFilterTypes)eFilter);
}

void CyPlayer::setUnitListFilterActive(int eFilter, bool bActive)
{
	m_pPlayer->setUnitListFilterActive((UnitFilterTypes)eFilter, bActive);
}

int CyPlayer::getUnitListGrouping()
{
	return m_pPlayer->getUnitListGrouping();
}

void CyPlayer::setUnitListGrouping(int eGrouping)
{
	m_pPlayer->setUnitListGrouping((UnitGroupingTypes)eGrouping);
}

int CyPlayer::getUnitListSorting()
{
	return m_pPlayer->getUnitListSorting();
}

void CyPlayer::setUnitListSorting(int eSorting)
{
	m_pPlayer->setUnitListSorting((UnitSortTypes)eSorting);
}

int CyPlayer::getUnitListGroupNum()
{
	return m_pPlayer->getUnitListGroupNum();
}

int CyPlayer::getUnitListNumInGroup(int iGroup)
{
	return m_pPlayer->getUnitListNumInGroup(iGroup);
}

int CyPlayer::getUnitListType(int iGroup, int iPos)
{
	return m_pPlayer->getUnitListType(iGroup, iPos);
}

void CyPlayer::makeNukesValid(bool bValid)
{
	m_pPlayer->makeNukesValid(bValid);
}

// AIAndy: Build Lists
int CyPlayer::getBLNumLists()
{
	return m_pPlayer->m_pBuildLists->getNumLists();
}

int CyPlayer::getBLIndexByID(int iID)
{
	return m_pPlayer->m_pBuildLists->getIndexByID(iID);
}

int CyPlayer::getBLID(int index)
{
	return m_pPlayer->m_pBuildLists->getID(index);
}

std::wstring CyPlayer::getBLListName(int index)
{
	return CvWString(m_pPlayer->m_pBuildLists->getListName(index));
}

int CyPlayer::getBLListLength(int index)
{
	return m_pPlayer->m_pBuildLists->getListLength(index);
}

const OrderData* CyPlayer::getBLOrder(int index, int iQIndex) const
{
	return m_pPlayer->m_pBuildLists->getOrder(index, iQIndex);
}

void CyPlayer::writeBLToFile()
{
	m_pPlayer->m_pBuildLists->writeToFile();
}

int CyPlayer::getBLCurrentList()
{
	return m_pPlayer->m_pBuildLists->getCurrentList();
}

void CyPlayer::addBLList()
{
	CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_NAME_LIST);
	if (NULL != pInfo)
	{
		pInfo->setData1(-1);
		gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGame().getActivePlayer(), true, true);
	}
	//CvMessageControl::getInstance().sendBuildListEdit(-1, CvString(szName));
}

void CyPlayer::renameBLList(int iID)
{
	CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_NAME_LIST);
	if (NULL != pInfo)
	{
		pInfo->setData1(iID);
		gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGame().getActivePlayer(), true, true);
	}
	//CvMessageControl::getInstance().sendBuildListEdit(iID, CvString(szName));
}

void CyPlayer::removeBLList(int iID)
{
	CvMessageControl::getInstance().sendBuildListEdit(iID, "", true);
}
