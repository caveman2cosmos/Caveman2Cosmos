//------------------------------------------------------------------------------------------------
//  FILE:    CvHandicapInfo.cpp
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvArtFileMgr.h"
#include "CvBuildingInfo.h"
#include "CvHeritageInfo.h"
#include "CvGameAI.h"
#include "CvGameTextMgr.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvInfoUtil.h"
#include "CvPlayerAI.h"
#include "CvPython.h"
#include "CvXMLLoadUtility.h"
#include "CvXMLLoadUtilityModTools.h"
#include "CheckSum.h"
#include "CvImprovementInfo.h"
#include "CvBonusInfo.h"
#include "CvHandicapInfo.h"


//======================================================================================================
//					CvHandicapInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvHandicapInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvHandicapInfo::CvHandicapInfo() :
m_iFreeWinsVsBarbs(0),
m_iAnimalAttackProb(0),
m_iAdvancedStartPointsMod(0),
m_iStartingGold(0),
m_iUnitUpkeepPercent(0),
m_iDistanceMaintenancePercent(0),
m_iNumCitiesMaintenancePercent(0),
m_iColonyMaintenancePercent(0),
m_iMaxColonyMaintenance(0),
m_iCorporationMaintenancePercent(0),
m_iCivicUpkeepPercent(0),
m_iInflationPercent(0),
m_iHealthBonus(0),
m_iHappyBonus(0),
m_iAttitudeChange(0),
m_iNoTechTradeModifier(0),
m_iTechTradeKnownModifier(0),
m_iUnownedWaterTilesPerBarbarianUnit(0),
m_iUnownedTilesPerBarbarianCity(0),
m_iBarbarianCityCreationTurnsElapsed(0),
m_iBarbarianCityCreationProb(0),
m_iAnimalCombatModifier(0),
m_iBarbarianCombatModifier(0),
m_iAIAnimalCombatModifier(0),
m_iSubdueAnimalBonusAI(0),
m_iAIBarbarianCombatModifier(0),
m_iStartingDefenseUnits(0),
m_iStartingWorkerUnits(0),
m_iStartingExploreUnits(0),
m_iAIStartingDefenseUnits(0),
m_iAIStartingWorkerUnits(0),
m_iAIStartingExploreUnits(0),
m_iBarbarianInitialDefenders(0),
m_iAIDeclareWarProb(0),
m_iAIWorkRateModifier(0),
m_iAIGrowthPercent(0),
m_iAITrainPercent(0),
m_iAIWorldTrainPercent(0),
m_iAIConstructPercent(0),
m_iAIWorldConstructPercent(0),
m_iAICreatePercent(0),
m_iAIResearchPercent(0),
m_iAIWorldCreatePercent(0),
m_iAICivicUpkeepPercent(0),
m_iAIUnitUpkeepPercent(0),
m_iAIUnitSupplyPercent(0),
m_iAIUnitUpgradePercent(0),
m_iAIInflationPercent(0),
m_iAIWarWearinessPercent(0),
m_iAIPerEraModifier(0),
m_iAIAdvancedStartPercent(0),
m_iRevolutionIndexPercent(0)
,m_PropertyManipulators()
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvHandicapInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvHandicapInfo::~CvHandicapInfo()
{
}


int CvHandicapInfo::getFreeWinsVsBarbs() const
{
	return m_iFreeWinsVsBarbs;
}


int CvHandicapInfo::getAnimalAttackProb() const
{
	return m_iAnimalAttackProb;
}


int CvHandicapInfo::getAdvancedStartPointsMod() const
{
	return m_iAdvancedStartPointsMod;
}


int CvHandicapInfo::getStartingGold() const
{
	return m_iStartingGold;
}


int CvHandicapInfo::getUnitUpkeepPercent() const
{
	return m_iUnitUpkeepPercent;
}


int CvHandicapInfo::getDistanceMaintenancePercent() const
{
	return m_iDistanceMaintenancePercent;
}


int CvHandicapInfo::getNumCitiesMaintenancePercent() const
{
	return m_iNumCitiesMaintenancePercent;
}


int CvHandicapInfo::getColonyMaintenancePercent() const
{
	return m_iColonyMaintenancePercent;
}


int CvHandicapInfo::getMaxColonyMaintenance() const
{
	return m_iMaxColonyMaintenance;
}


int CvHandicapInfo::getCorporationMaintenancePercent() const
{
	return m_iCorporationMaintenancePercent;
}


int CvHandicapInfo::getCivicUpkeepPercent() const
{
	return m_iCivicUpkeepPercent;
}


int CvHandicapInfo::getInflationPercent() const
{
	return m_iInflationPercent;
}


int CvHandicapInfo::getHealthBonus() const
{
	return m_iHealthBonus;
}


int CvHandicapInfo::getHappyBonus() const
{
	return m_iHappyBonus;
}


int CvHandicapInfo::getAttitudeChange() const
{
	return m_iAttitudeChange;
}


int CvHandicapInfo::getNoTechTradeModifier() const
{
	return m_iNoTechTradeModifier;
}


int CvHandicapInfo::getTechTradeKnownModifier() const
{
	return m_iTechTradeKnownModifier;
}


int CvHandicapInfo::getUnownedWaterTilesPerBarbarianUnit() const
{
	return m_iUnownedWaterTilesPerBarbarianUnit;
}


int CvHandicapInfo::getUnownedTilesPerBarbarianCity() const
{
	return m_iUnownedTilesPerBarbarianCity;
}


int CvHandicapInfo::getBarbarianCityCreationTurnsElapsed() const
{
	return m_iBarbarianCityCreationTurnsElapsed;
}


int CvHandicapInfo::getBarbarianCityCreationProb() const
{
	return m_iBarbarianCityCreationProb;
}


int CvHandicapInfo::getAnimalCombatModifier() const
{
	return m_iAnimalCombatModifier;
}


int CvHandicapInfo::getBarbarianCombatModifier() const
{
	return m_iBarbarianCombatModifier;
}


int CvHandicapInfo::getAIAnimalCombatModifier() const
{
	return m_iAIAnimalCombatModifier;
}


int CvHandicapInfo::getAIBarbarianCombatModifier() const
{
	return m_iAIBarbarianCombatModifier;
}


int CvHandicapInfo::getStartingDefenseUnits() const
{
	return m_iStartingDefenseUnits;
}


int CvHandicapInfo::getStartingWorkerUnits() const
{
	return m_iStartingWorkerUnits;
}


int CvHandicapInfo::getStartingExploreUnits() const
{
	return m_iStartingExploreUnits;
}


int CvHandicapInfo::getAIStartingDefenseUnits() const
{
	return m_iAIStartingDefenseUnits;
}


int CvHandicapInfo::getAIStartingWorkerUnits() const
{
	return m_iAIStartingWorkerUnits;
}


int CvHandicapInfo::getAIStartingExploreUnits() const
{
	return m_iAIStartingExploreUnits;
}


int CvHandicapInfo::getBarbarianInitialDefenders() const
{
	return m_iBarbarianInitialDefenders;
}


int CvHandicapInfo::getAIDeclareWarProb() const
{
	return m_iAIDeclareWarProb;
}


int CvHandicapInfo::getAIWorkRateModifier() const
{
	return m_iAIWorkRateModifier;
}


int CvHandicapInfo::getAIGrowthPercent() const
{
	return m_iAIGrowthPercent;
}


int CvHandicapInfo::getAITrainPercent() const
{
	return m_iAITrainPercent;
}


int CvHandicapInfo::getAIWorldTrainPercent() const
{
	return m_iAIWorldTrainPercent;
}


int CvHandicapInfo::getAIConstructPercent() const
{
	return m_iAIConstructPercent;
}


int CvHandicapInfo::getAIWorldConstructPercent() const
{
	return m_iAIWorldConstructPercent;
}


int CvHandicapInfo::getAICreatePercent() const
{
	return m_iAICreatePercent;
}


int CvHandicapInfo::getAIResearchPercent() const
{
	return m_iAIResearchPercent;
}


int CvHandicapInfo::getAIWorldCreatePercent() const
{
	return m_iAIWorldCreatePercent;
}


int CvHandicapInfo::getAICivicUpkeepPercent() const
{
	return m_iAICivicUpkeepPercent;
}


int CvHandicapInfo::getAIUnitUpkeepPercent() const
{
	return m_iAIUnitUpkeepPercent;
}


int CvHandicapInfo::getAIUnitSupplyPercent() const
{
	return m_iAIUnitSupplyPercent;
}


int CvHandicapInfo::getAIUnitUpgradePercent() const
{
	return m_iAIUnitUpgradePercent;
}


int CvHandicapInfo::getAIInflationPercent() const
{
	return m_iAIInflationPercent;
}


int CvHandicapInfo::getAIWarWearinessPercent() const
{
	return m_iAIWarWearinessPercent;
}


int CvHandicapInfo::getAIPerEraModifier() const
{
	return m_iAIPerEraModifier;
}


int CvHandicapInfo::getAIAdvancedStartPercent() const
{
	return m_iAIAdvancedStartPercent;
}


int CvHandicapInfo::getNumGoodies() const
{
	return m_piGoodies.size();
}


int CvHandicapInfo::getGoodies(int i) const
{
	FASSERT_BOUNDS(0, getNumGoodies(), i);
	return m_piGoodies[i];
}


int CvHandicapInfo::getPercent(int iID) const
{
	return m_Percent.getValue(iID);
}


int CvHandicapInfo::getRevolutionIndexPercent() const
{
	return m_iRevolutionIndexPercent;
}



void CvHandicapInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iFreeWinsVsBarbs);
	CheckSum(iSum, m_iAnimalAttackProb);
	CheckSum(iSum, m_iAdvancedStartPointsMod);
	CheckSum(iSum, m_iStartingGold);
	CheckSum(iSum, m_iUnitUpkeepPercent);
	CheckSum(iSum, m_iDistanceMaintenancePercent);
	CheckSum(iSum, m_iNumCitiesMaintenancePercent);
	CheckSum(iSum, m_iColonyMaintenancePercent);
	CheckSum(iSum, m_iMaxColonyMaintenance);
	CheckSum(iSum, m_iCorporationMaintenancePercent);
	CheckSum(iSum, m_iCivicUpkeepPercent);
	CheckSum(iSum, m_iInflationPercent);
	CheckSum(iSum, m_iHealthBonus);
	CheckSum(iSum, m_iHappyBonus);
	CheckSum(iSum, m_iAttitudeChange);
	CheckSum(iSum, m_iNoTechTradeModifier);
	CheckSum(iSum, m_iTechTradeKnownModifier);
	CheckSum(iSum, m_iUnownedWaterTilesPerBarbarianUnit);
	CheckSum(iSum, m_iUnownedTilesPerBarbarianCity);
	CheckSum(iSum, m_iBarbarianCityCreationTurnsElapsed);
	CheckSum(iSum, m_iBarbarianCityCreationProb);
	CheckSum(iSum, m_iAnimalCombatModifier);
	CheckSum(iSum, m_iBarbarianCombatModifier);
	CheckSum(iSum, m_iAIAnimalCombatModifier);
	CheckSum(iSum, m_iAIBarbarianCombatModifier);

	CheckSum(iSum, m_iStartingDefenseUnits);
	CheckSum(iSum, m_iStartingWorkerUnits);
	CheckSum(iSum, m_iStartingExploreUnits);
	CheckSum(iSum, m_iAIStartingDefenseUnits);
	CheckSum(iSum, m_iAIStartingWorkerUnits);
	CheckSum(iSum, m_iAIStartingExploreUnits);
	CheckSum(iSum, m_iBarbarianInitialDefenders);
	CheckSum(iSum, m_iAIDeclareWarProb);
	CheckSum(iSum, m_iAIWorkRateModifier);
	CheckSum(iSum, m_iAIGrowthPercent);
	CheckSum(iSum, m_iAITrainPercent);
	CheckSum(iSum, m_iAIWorldTrainPercent);
	CheckSum(iSum, m_iAIConstructPercent);
	CheckSum(iSum, m_iAIWorldConstructPercent);
	CheckSum(iSum, m_iAICreatePercent);
	CheckSum(iSum, m_iAIResearchPercent);
	CheckSum(iSum, m_iAIWorldCreatePercent);
	CheckSum(iSum, m_iAICivicUpkeepPercent);
	CheckSum(iSum, m_iAIUnitUpkeepPercent);
	CheckSum(iSum, m_iAIUnitSupplyPercent);
	CheckSum(iSum, m_iAIUnitUpgradePercent);
	CheckSum(iSum, m_iAIInflationPercent);
	CheckSum(iSum, m_iAIWarWearinessPercent);
	CheckSum(iSum, m_iAIPerEraModifier);
	CheckSum(iSum, m_iAIAdvancedStartPercent);
	CheckSumC(iSum, m_Percent);

	m_PropertyManipulators.getCheckSum(iSum);

	CheckSumC(iSum, m_piGoodies);

	CheckSum(iSum, m_iRevolutionIndexPercent);
}


bool CvHandicapInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iFreeWinsVsBarbs, L"iFreeWinsVsBarbs");
	pXML->GetOptionalChildXmlValByName(&m_iAnimalAttackProb, L"iAnimalAttackProb");
	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartPointsMod, L"iAdvancedStartPointsMod");
	pXML->GetOptionalChildXmlValByName(&m_iStartingGold, L"iGold");
	pXML->GetOptionalChildXmlValByName(&m_iUnitUpkeepPercent, L"iUnitUpkeepPercent");
	pXML->GetOptionalChildXmlValByName(&m_iDistanceMaintenancePercent, L"iDistanceMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesMaintenancePercent, L"iNumCitiesMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iColonyMaintenancePercent, L"iColonyMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iMaxColonyMaintenance, L"iMaxColonyMaintenance");
	pXML->GetOptionalChildXmlValByName(&m_iCorporationMaintenancePercent, L"iCorporationMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iCivicUpkeepPercent, L"iCivicUpkeepPercent");
	pXML->GetOptionalChildXmlValByName(&m_iInflationPercent, L"iInflationPercent");
	pXML->GetOptionalChildXmlValByName(&m_iHealthBonus, L"iHealthBonus");
	pXML->GetOptionalChildXmlValByName(&m_iHappyBonus, L"iHappyBonus");
	pXML->GetOptionalChildXmlValByName(&m_iAttitudeChange, L"iAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iNoTechTradeModifier, L"iNoTechTradeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iTechTradeKnownModifier, L"iTechTradeKnownModifier");
	pXML->GetOptionalChildXmlValByName(&m_iUnownedWaterTilesPerBarbarianUnit, L"iUnownedWaterTilesPerBarbarianUnit");
	pXML->GetOptionalChildXmlValByName(&m_iUnownedTilesPerBarbarianCity, L"iUnownedTilesPerBarbarianCity");
	pXML->GetOptionalChildXmlValByName(&m_iBarbarianCityCreationTurnsElapsed, L"iBarbarianCityCreationTurnsElapsed");
	pXML->GetOptionalChildXmlValByName(&m_iBarbarianCityCreationProb, L"iBarbarianCityCreationProb");
	pXML->GetOptionalChildXmlValByName(&m_iAnimalCombatModifier, L"iAnimalBonus");
	pXML->GetOptionalChildXmlValByName(&m_iBarbarianCombatModifier, L"iBarbarianBonus");
	pXML->GetOptionalChildXmlValByName(&m_iAIAnimalCombatModifier, L"iAIAnimalBonus");
	pXML->GetOptionalChildXmlValByName(&m_iSubdueAnimalBonusAI, L"iSubdueAnimalBonusAI");
	pXML->GetOptionalChildXmlValByName(&m_iAIBarbarianCombatModifier, L"iAIBarbarianBonus");
	pXML->GetOptionalChildXmlValByName(&m_iStartingDefenseUnits, L"iStartingDefenseUnits");
	pXML->GetOptionalChildXmlValByName(&m_iStartingWorkerUnits, L"iStartingWorkerUnits");
	pXML->GetOptionalChildXmlValByName(&m_iStartingExploreUnits, L"iStartingExploreUnits");
	pXML->GetOptionalChildXmlValByName(&m_iAIStartingDefenseUnits, L"iAIStartingDefenseUnits");
	pXML->GetOptionalChildXmlValByName(&m_iAIStartingWorkerUnits, L"iAIStartingWorkerUnits");
	pXML->GetOptionalChildXmlValByName(&m_iAIStartingExploreUnits, L"iAIStartingExploreUnits");
	pXML->GetOptionalChildXmlValByName(&m_iBarbarianInitialDefenders, L"iBarbarianDefenders");
	pXML->GetOptionalChildXmlValByName(&m_iAIDeclareWarProb, L"iAIDeclareWarProb");
	pXML->GetOptionalChildXmlValByName(&m_iAIWorkRateModifier, L"iAIWorkRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iAIGrowthPercent, L"iAIGrowthPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAITrainPercent, L"iAITrainPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIWorldTrainPercent, L"iAIWorldTrainPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIConstructPercent, L"iAIConstructPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIWorldConstructPercent, L"iAIWorldConstructPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAICreatePercent, L"iAICreatePercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIResearchPercent, L"iAIResearchPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIWorldCreatePercent, L"iAIWorldCreatePercent");
	pXML->GetOptionalChildXmlValByName(&m_iAICivicUpkeepPercent, L"iAICivicUpkeepPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIUnitUpkeepPercent, L"iAIUnitUpkeepPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIUnitSupplyPercent, L"iAIUnitSupplyPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIUnitUpgradePercent, L"iAIUnitUpgradePercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIInflationPercent, L"iAIInflationPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIWarWearinessPercent, L"iAIWarWearinessPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIPerEraModifier, L"iAIPerEraModifier");
	pXML->GetOptionalChildXmlValByName(&m_iAIAdvancedStartPercent, L"iAIAdvancedStartPercent");

	pXML->SetOptionalVector(&m_piGoodies, L"Goodies");

	pXML->GetOptionalChildXmlValByName(&m_iRevolutionIndexPercent, L"iRevolutionIndexPercent");

	m_Percent.read(pXML, L"Percents");

	m_PropertyManipulators.read(pXML);

	return true;
}


void CvHandicapInfo::copyNonDefaults(const CvHandicapInfo* pClassInfo)
{
	int iDefault = 0;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getFreeWinsVsBarbs() == iDefault) m_iFreeWinsVsBarbs = pClassInfo->getFreeWinsVsBarbs();
	if (getAnimalAttackProb() == iDefault) m_iAnimalAttackProb = pClassInfo->getAnimalAttackProb();
	if (getAdvancedStartPointsMod() == iDefault) m_iAdvancedStartPointsMod = pClassInfo->getAdvancedStartPointsMod();
	if (getStartingGold() == iDefault) m_iStartingGold = pClassInfo->getStartingGold();
	if (getUnitUpkeepPercent() == iDefault) m_iUnitUpkeepPercent = pClassInfo->getUnitUpkeepPercent();
	if (getDistanceMaintenancePercent() == iDefault) m_iDistanceMaintenancePercent = pClassInfo->getDistanceMaintenancePercent();
	if (getNumCitiesMaintenancePercent() == iDefault) m_iNumCitiesMaintenancePercent = pClassInfo->getNumCitiesMaintenancePercent();
	if (getColonyMaintenancePercent() == iDefault) m_iColonyMaintenancePercent = pClassInfo->getColonyMaintenancePercent();
	if (getMaxColonyMaintenance() == iDefault) m_iMaxColonyMaintenance = pClassInfo->getMaxColonyMaintenance();
	if (getCorporationMaintenancePercent() == iDefault) m_iCorporationMaintenancePercent = pClassInfo->getCorporationMaintenancePercent();
	if (getCivicUpkeepPercent() == iDefault) m_iCivicUpkeepPercent = pClassInfo->getCivicUpkeepPercent();
	if (getInflationPercent() == iDefault) m_iInflationPercent = pClassInfo->getInflationPercent();
	if (getHealthBonus() == iDefault) m_iHealthBonus = pClassInfo->getHealthBonus();
	if (getHappyBonus() == iDefault) m_iHappyBonus = pClassInfo->getHappyBonus();
	if (getAttitudeChange() == iDefault) m_iAttitudeChange = pClassInfo->getAttitudeChange();
	if (getNoTechTradeModifier() == iDefault) m_iNoTechTradeModifier = pClassInfo->getNoTechTradeModifier();
	if (getTechTradeKnownModifier() == iDefault) m_iTechTradeKnownModifier = pClassInfo->getTechTradeKnownModifier();
	if (getUnownedWaterTilesPerBarbarianUnit() == iDefault) m_iUnownedWaterTilesPerBarbarianUnit = pClassInfo->getUnownedWaterTilesPerBarbarianUnit();
	if (getUnownedTilesPerBarbarianCity() == iDefault) m_iUnownedTilesPerBarbarianCity = pClassInfo->getUnownedTilesPerBarbarianCity();
	if (getBarbarianCityCreationTurnsElapsed() == iDefault) m_iBarbarianCityCreationTurnsElapsed = pClassInfo->getBarbarianCityCreationTurnsElapsed();
	if (getBarbarianCityCreationProb() == iDefault) m_iBarbarianCityCreationProb = pClassInfo->getBarbarianCityCreationProb();
	if (getAnimalCombatModifier() == iDefault) m_iAnimalCombatModifier = pClassInfo->getAnimalCombatModifier();
	if (getBarbarianCombatModifier() == iDefault) m_iBarbarianCombatModifier = pClassInfo->getBarbarianCombatModifier();
	if (getAIAnimalCombatModifier() == iDefault) m_iAIAnimalCombatModifier = pClassInfo->getAIAnimalCombatModifier();
	if (m_iSubdueAnimalBonusAI == iDefault) m_iSubdueAnimalBonusAI = pClassInfo->getSubdueAnimalBonusAI();
	if (getAIBarbarianCombatModifier() == iDefault) m_iAIBarbarianCombatModifier = pClassInfo->getAIBarbarianCombatModifier();
	if (getStartingDefenseUnits() == iDefault) m_iStartingDefenseUnits = pClassInfo->getStartingDefenseUnits();
	if (getStartingWorkerUnits() == iDefault) m_iStartingWorkerUnits = pClassInfo->getStartingWorkerUnits();
	if (getStartingExploreUnits() == iDefault) m_iStartingExploreUnits = pClassInfo->getStartingExploreUnits();
	if (getAIStartingDefenseUnits() == iDefault) m_iAIStartingDefenseUnits = pClassInfo->getAIStartingDefenseUnits();
	if (getAIStartingWorkerUnits() == iDefault) m_iAIStartingWorkerUnits = pClassInfo->getAIStartingWorkerUnits();
	if (getAIStartingExploreUnits() == iDefault) m_iAIStartingExploreUnits = pClassInfo->getAIStartingExploreUnits();
	if (getBarbarianInitialDefenders() == iDefault) m_iBarbarianInitialDefenders = pClassInfo->getBarbarianInitialDefenders();
	if (getAIDeclareWarProb() == iDefault) m_iAIDeclareWarProb = pClassInfo->getAIDeclareWarProb();
	if (getAIWorkRateModifier() == iDefault) m_iAIWorkRateModifier = pClassInfo->getAIWorkRateModifier();
	if (getAIGrowthPercent() == iDefault) m_iAIGrowthPercent = pClassInfo->getAIGrowthPercent();
	if (getAITrainPercent() == iDefault) m_iAITrainPercent = pClassInfo->getAITrainPercent();
	if (getAIWorldTrainPercent() == iDefault) m_iAIWorldTrainPercent = pClassInfo->getAIWorldTrainPercent();
	if (getAIConstructPercent() == iDefault) m_iAIConstructPercent = pClassInfo->getAIConstructPercent();
	if (getAIWorldConstructPercent() == iDefault) m_iAIWorldConstructPercent = pClassInfo->getAIWorldConstructPercent();
	if (getAICreatePercent() == iDefault) m_iAICreatePercent = pClassInfo->getAICreatePercent();
	if (getAIResearchPercent() == iDefault) m_iAIResearchPercent = pClassInfo->getAIResearchPercent();
	if (getAIWorldCreatePercent() == iDefault) m_iAIWorldCreatePercent = pClassInfo->getAIWorldCreatePercent();
	if (getAICivicUpkeepPercent() == iDefault) m_iAICivicUpkeepPercent = pClassInfo->getAICivicUpkeepPercent();
	if (getAIUnitUpkeepPercent() == iDefault) m_iAIUnitUpkeepPercent = pClassInfo->getAIUnitUpkeepPercent();
	if (getAIUnitSupplyPercent() == iDefault) m_iAIUnitSupplyPercent = pClassInfo->getAIUnitSupplyPercent();
	if (getAIUnitUpgradePercent() == iDefault) m_iAIUnitUpgradePercent = pClassInfo->getAIUnitUpgradePercent();
	if (getAIInflationPercent() == iDefault) m_iAIInflationPercent = pClassInfo->getAIInflationPercent();
	if (getAIWarWearinessPercent() == iDefault) m_iAIWarWearinessPercent = pClassInfo->getAIWarWearinessPercent();
	if (getAIPerEraModifier() == iDefault) m_iAIPerEraModifier = pClassInfo->getAIPerEraModifier();
	if (getAIAdvancedStartPercent() == iDefault) m_iAIAdvancedStartPercent = pClassInfo->getAIAdvancedStartPercent();
	if (getRevolutionIndexPercent() == iDefault) m_iRevolutionIndexPercent = pClassInfo->getRevolutionIndexPercent();

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_piGoodies, pClassInfo->m_piGoodies);

	m_Percent.copyNonDefaults(pClassInfo->m_Percent);

	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);
}

