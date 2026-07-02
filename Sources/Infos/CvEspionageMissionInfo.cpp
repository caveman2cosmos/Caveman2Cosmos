//------------------------------------------------------------------------------------------------
//  FILE:    CvEspionageMissionInfo.cpp
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
#include "CvEspionageMissionInfo.h"



//======================================================================================================
//					CvEspionageMissionInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvEspionageMissionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvEspionageMissionInfo::CvEspionageMissionInfo() :
m_bNuke(false),
m_bRevolt(false),
m_bDisablePower(false),
m_iWarWearinessCounter(0),
m_iPrereqGameOption(NO_GAMEOPTION),
m_iSabatogeResearchCostFactor(0),
m_iRemoveReligionsCostFactor(0),
m_iRemoveCorporationsCostFactor(0),
m_iCost(0),
m_bIsPassive(false),
m_bIsTwoPhases(false),
m_bTargetsCity(false),
m_bSelectPlot(false),
m_iTechPrereq(NO_TECH),
m_iVisibilityLevel(0),
m_bInvestigateCity(false),
m_bSeeDemographics(false),
m_bNoActiveMissions(false),
m_bSeeResearch(false),
m_bDestroyImprovement(false),
m_iDestroyBuildingCostFactor(0),
m_iDestroyUnitCostFactor(0),
m_iDestroyProjectCostFactor(0),
m_iDestroyProductionCostFactor(0),
m_iBuyUnitCostFactor(0),
m_iBuyCityCostFactor(0),
m_iStealTreasuryTypes(0),
m_iCityInsertCultureAmountFactor(0),
m_iCityInsertCultureCostFactor(0),
m_iCityPoisonWaterCounter(0),
m_iCityUnhappinessCounter(0),
m_iCityRevoltCounter(0),
m_iBuyTechCostFactor(0),
m_iSwitchCivicCostFactor(0),
m_iSwitchReligionCostFactor(0),
m_iPlayerAnarchyCounter(0),
m_iCounterespionageNumTurns(0),
m_iCounterespionageMod(0),
m_iDifficultyMod(0)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvEspionageMissionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvEspionageMissionInfo::~CvEspionageMissionInfo()
{
}


int CvEspionageMissionInfo::getCost() const
{
	return m_iCost;
}


bool CvEspionageMissionInfo::isPassive() const
{
	return m_bIsPassive;
}


bool CvEspionageMissionInfo::isTwoPhases() const
{
	return m_bIsTwoPhases;
}


bool CvEspionageMissionInfo::isTargetsCity() const
{
	return m_bTargetsCity;
}


bool CvEspionageMissionInfo::isSelectPlot() const
{
	return m_bSelectPlot;
}


int CvEspionageMissionInfo::getVisibilityLevel() const
{
	return m_iVisibilityLevel;
}


bool CvEspionageMissionInfo::isInvestigateCity() const
{
	return m_bInvestigateCity;
}


bool CvEspionageMissionInfo::isSeeDemographics() const
{
	return m_bSeeDemographics;
}


bool CvEspionageMissionInfo::isNoActiveMissions() const
{
	return m_bNoActiveMissions;
}


bool CvEspionageMissionInfo::isSeeResearch() const
{
	return m_bSeeResearch;
}


bool CvEspionageMissionInfo::isDestroyImprovement() const
{
	return m_bDestroyImprovement;
}


int CvEspionageMissionInfo::getDestroyBuildingCostFactor() const
{
	return m_iDestroyBuildingCostFactor;
}


int CvEspionageMissionInfo::getDestroyUnitCostFactor() const
{
	return m_iDestroyUnitCostFactor;
}


int CvEspionageMissionInfo::getDestroyProjectCostFactor() const
{
	return m_iDestroyProjectCostFactor;
}


int CvEspionageMissionInfo::getDestroyProductionCostFactor() const
{
	return m_iDestroyProductionCostFactor;
}


int CvEspionageMissionInfo::getBuyUnitCostFactor() const
{
	return m_iBuyUnitCostFactor;
}


int CvEspionageMissionInfo::getBuyCityCostFactor() const
{
	return m_iBuyCityCostFactor;
}


int CvEspionageMissionInfo::getStealTreasuryTypes() const
{
	return m_iStealTreasuryTypes;
}


int CvEspionageMissionInfo::getCityInsertCultureAmountFactor() const
{
	return m_iCityInsertCultureAmountFactor;
}


int CvEspionageMissionInfo::getCityInsertCultureCostFactor() const
{
	return m_iCityInsertCultureCostFactor;
}


int CvEspionageMissionInfo::getCityPoisonWaterCounter() const
{
	return m_iCityPoisonWaterCounter;
}


int CvEspionageMissionInfo::getCityUnhappinessCounter() const
{
	return m_iCityUnhappinessCounter;
}


int CvEspionageMissionInfo::getCityRevoltCounter() const
{
	return m_iCityRevoltCounter;
}


int CvEspionageMissionInfo::getBuyTechCostFactor() const
{
	return m_iBuyTechCostFactor;
}


int CvEspionageMissionInfo::getSwitchCivicCostFactor() const
{
	return m_iSwitchCivicCostFactor;
}


int CvEspionageMissionInfo::getSwitchReligionCostFactor() const
{
	return m_iSwitchReligionCostFactor;
}


int CvEspionageMissionInfo::getPlayerAnarchyCounter() const
{
	return m_iPlayerAnarchyCounter;
}


int CvEspionageMissionInfo::getCounterespionageNumTurns() const
{
	return m_iCounterespionageNumTurns;
}


int CvEspionageMissionInfo::getCounterespionageMod() const
{
	return m_iCounterespionageMod;
}


int CvEspionageMissionInfo::getDifficultyMod() const
{
	return m_iDifficultyMod;
}


bool CvEspionageMissionInfo::isNuke() const
{
	return m_bNuke;
}


bool CvEspionageMissionInfo::isRevolt() const
{
	return m_bRevolt;
}


bool CvEspionageMissionInfo::isDisablePower() const
{
	return m_bDisablePower;
}


int CvEspionageMissionInfo::getPrereqGameOption() const
{
	return m_iPrereqGameOption;
}


int CvEspionageMissionInfo::getWarWearinessCounter() const
{
	return m_iWarWearinessCounter;
}


int CvEspionageMissionInfo::getSabatogeResearchCostFactor() const
{
	return m_iSabatogeResearchCostFactor;
}


int CvEspionageMissionInfo::getRemoveReligionsCostFactor() const
{
	return m_iRemoveReligionsCostFactor;
}


int CvEspionageMissionInfo::getRemoveCorporationsCostFactor() const
{
	return m_iRemoveCorporationsCostFactor;
}


bool CvEspionageMissionInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iCost, L"iCost");
	pXML->GetOptionalChildXmlValByName(&m_bIsPassive, L"bIsPassive");
	pXML->GetOptionalChildXmlValByName(&m_bIsTwoPhases, L"bIsTwoPhases");
	pXML->GetOptionalChildXmlValByName(&m_bTargetsCity, L"bTargetsCity");
	pXML->GetOptionalChildXmlValByName(&m_bSelectPlot, L"bSelectPlot");
	pXML->GetOptionalTypeEnum(m_iTechPrereq, L"TechPrereq");
	pXML->GetOptionalChildXmlValByName(&m_iVisibilityLevel, L"iVisibilityLevel");
	pXML->GetOptionalChildXmlValByName(&m_bInvestigateCity, L"bInvestigateCity");
	pXML->GetOptionalChildXmlValByName(&m_bSeeDemographics, L"bSeeDemographics");
	pXML->GetOptionalChildXmlValByName(&m_bNoActiveMissions, L"bNoActiveMissions");
	pXML->GetOptionalChildXmlValByName(&m_bSeeResearch, L"bSeeResearch");
	pXML->GetOptionalChildXmlValByName(&m_bDestroyImprovement, L"bDestroyImprovement");
	pXML->GetOptionalChildXmlValByName(&m_iDestroyBuildingCostFactor, L"iDestroyBuildingCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iDestroyUnitCostFactor, L"iDestroyUnitCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iDestroyProjectCostFactor, L"iDestroyProjectCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iDestroyProductionCostFactor, L"iDestroyProductionCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iBuyUnitCostFactor, L"iBuyUnitCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iBuyCityCostFactor, L"iBuyCityCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iStealTreasuryTypes, L"iStealTreasuryTypes");
	pXML->GetOptionalChildXmlValByName(&m_iCityInsertCultureAmountFactor, L"iCityInsertCultureAmountFactor");
	pXML->GetOptionalChildXmlValByName(&m_iCityInsertCultureCostFactor, L"iCityInsertCultureCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iCityPoisonWaterCounter, L"iCityPoisonWaterCounter");
	pXML->GetOptionalChildXmlValByName(&m_iCityUnhappinessCounter, L"iCityUnhappinessCounter");
	pXML->GetOptionalChildXmlValByName(&m_iCityRevoltCounter, L"iCityRevoltCounter");
	pXML->GetOptionalChildXmlValByName(&m_iBuyTechCostFactor, L"iBuyTechCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iSwitchCivicCostFactor, L"iSwitchCivicCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iSwitchReligionCostFactor, L"iSwitchReligionCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iPlayerAnarchyCounter, L"iPlayerAnarchyCounter");
	pXML->GetOptionalChildXmlValByName(&m_iCounterespionageNumTurns, L"iCounterespionageNumTurns");
	pXML->GetOptionalChildXmlValByName(&m_iCounterespionageMod, L"iCounterespionageMod");
	pXML->GetOptionalChildXmlValByName(&m_iDifficultyMod, L"iDifficultyMod");
	pXML->GetOptionalChildXmlValByName(&m_bNuke, L"bNuke");
	pXML->GetOptionalChildXmlValByName(&m_bRevolt, L"bRevolt");
	pXML->GetOptionalChildXmlValByName(&m_bDisablePower, L"bDisablePower");
	pXML->GetOptionalChildXmlValByName(&m_iWarWearinessCounter, L"iWarWearinessCounter");
	pXML->GetOptionalChildXmlValByName(&m_iSabatogeResearchCostFactor, L"iSabatogeResearchCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iRemoveReligionsCostFactor, L"iRemoveReligionsCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iRemoveCorporationsCostFactor, L"iRemoveCorporationsCostFactor");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqGameOption");
	m_iPrereqGameOption = pXML->GetInfoClass(szTextVal);

	return true;
}


void CvEspionageMissionInfo::copyNonDefaults(const CvEspionageMissionInfo* pClassInfo)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getCost() == iDefault) m_iCost = pClassInfo->getCost();
	if (isPassive() == bDefault) m_bIsPassive = pClassInfo->isPassive();
	if (isTwoPhases() == bDefault) m_bIsTwoPhases = pClassInfo->isTwoPhases();
	if (isTargetsCity() == bDefault) m_bTargetsCity = pClassInfo->isTargetsCity();
	if (isSelectPlot() == bDefault) m_bSelectPlot = pClassInfo->isSelectPlot();

	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();

	if (getVisibilityLevel() == iDefault) m_iVisibilityLevel = pClassInfo->getVisibilityLevel();
	if (isInvestigateCity() == bDefault) m_bInvestigateCity = pClassInfo->isInvestigateCity();
	if (isSeeDemographics() == bDefault) m_bSeeDemographics = pClassInfo->isSeeDemographics();
	if (isNoActiveMissions() == bDefault) m_bNoActiveMissions = pClassInfo->isNoActiveMissions();
	if (isSeeResearch() == bDefault) m_bSeeResearch = pClassInfo->isSeeResearch();

	if (isDestroyImprovement() == bDefault) m_bDestroyImprovement = pClassInfo->isDestroyImprovement();
	if (getDestroyBuildingCostFactor() == iDefault) m_iDestroyBuildingCostFactor = pClassInfo->getDestroyBuildingCostFactor();
	if (getDestroyUnitCostFactor() == iDefault) m_iDestroyUnitCostFactor = pClassInfo->getDestroyUnitCostFactor();
	if (getDestroyProjectCostFactor() == iDefault) m_iDestroyProjectCostFactor = pClassInfo->getDestroyProjectCostFactor();
	if (getDestroyProductionCostFactor() == iDefault) m_iDestroyProductionCostFactor = pClassInfo->getDestroyProductionCostFactor();
	if (getBuyUnitCostFactor() == iDefault) m_iBuyUnitCostFactor = pClassInfo->getBuyUnitCostFactor();
	if (getBuyCityCostFactor() == iDefault) m_iBuyCityCostFactor = pClassInfo->getBuyCityCostFactor();
	if (getStealTreasuryTypes() == iDefault) m_iStealTreasuryTypes = pClassInfo->getStealTreasuryTypes();
	if (getCityInsertCultureAmountFactor() == iDefault) m_iCityInsertCultureAmountFactor = pClassInfo->getCityInsertCultureAmountFactor();
	if (getCityInsertCultureCostFactor() == iDefault) m_iCityInsertCultureCostFactor = pClassInfo->getCityInsertCultureCostFactor();
	if (getCityPoisonWaterCounter() == iDefault) m_iCityPoisonWaterCounter = pClassInfo->getCityPoisonWaterCounter();
	if (getCityUnhappinessCounter() == iDefault) m_iCityUnhappinessCounter = pClassInfo->getCityUnhappinessCounter();
	if (getCityRevoltCounter() == iDefault) m_iCityRevoltCounter = pClassInfo->getCityRevoltCounter();
	if (getBuyTechCostFactor() == iDefault) m_iBuyTechCostFactor = pClassInfo->getBuyTechCostFactor();
	if (getSwitchCivicCostFactor() == iDefault) m_iSwitchCivicCostFactor = pClassInfo->getSwitchCivicCostFactor();
	if (getSwitchReligionCostFactor() == iDefault) m_iSwitchReligionCostFactor = pClassInfo->getSwitchReligionCostFactor();
	if (getPlayerAnarchyCounter() == iDefault) m_iPlayerAnarchyCounter = pClassInfo->getPlayerAnarchyCounter();
	if (getCounterespionageNumTurns() == iDefault) m_iCounterespionageNumTurns = pClassInfo->getCounterespionageNumTurns();
	if (getCounterespionageMod() == iDefault) m_iCounterespionageMod = pClassInfo->getCounterespionageMod();
	if (getDifficultyMod() == iDefault) m_iDifficultyMod = pClassInfo->getDifficultyMod();
	if (isNuke() == bDefault) m_bNuke = pClassInfo->isNuke();
	if (isRevolt() == bDefault) m_bRevolt = pClassInfo->isRevolt();
	if (isDisablePower() == bDefault) m_bDisablePower = pClassInfo->isDisablePower();
	if (getPrereqGameOption() == iTextDefault) m_iPrereqGameOption = pClassInfo->getPrereqGameOption();
	if (getWarWearinessCounter() == iDefault) m_iWarWearinessCounter = pClassInfo->getWarWearinessCounter();
	if (getSabatogeResearchCostFactor() == iDefault) m_iSabatogeResearchCostFactor = pClassInfo->getSabatogeResearchCostFactor();
	if (getRemoveReligionsCostFactor() == iDefault) m_iRemoveReligionsCostFactor = pClassInfo->getRemoveReligionsCostFactor();
	if (getRemoveCorporationsCostFactor() == iDefault) m_iRemoveCorporationsCostFactor = pClassInfo->getRemoveCorporationsCostFactor();
}


void CvEspionageMissionInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iCost);
	CheckSum(iSum, m_bIsPassive);
	CheckSum(iSum, m_bIsTwoPhases);
	CheckSum(iSum, m_bTargetsCity);
	CheckSum(iSum, m_bSelectPlot);

	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iVisibilityLevel);
	CheckSum(iSum, m_bInvestigateCity);
	CheckSum(iSum, m_bSeeDemographics);
	CheckSum(iSum, m_bNoActiveMissions);
	CheckSum(iSum, m_bSeeResearch);

	CheckSum(iSum, m_bDestroyImprovement);
	CheckSum(iSum, m_iDestroyBuildingCostFactor);
	CheckSum(iSum, m_iDestroyUnitCostFactor);
	CheckSum(iSum, m_iDestroyProjectCostFactor);
	CheckSum(iSum, m_iDestroyProductionCostFactor);
	CheckSum(iSum, m_iBuyUnitCostFactor);
	CheckSum(iSum, m_iBuyCityCostFactor);
	CheckSum(iSum, m_iStealTreasuryTypes);
	CheckSum(iSum, m_iCityInsertCultureAmountFactor);
	CheckSum(iSum, m_iCityInsertCultureCostFactor);
	CheckSum(iSum, m_iCityPoisonWaterCounter);
	CheckSum(iSum, m_iCityUnhappinessCounter);
	CheckSum(iSum, m_iCityRevoltCounter);
	CheckSum(iSum, m_iBuyTechCostFactor);
	CheckSum(iSum, m_iSwitchCivicCostFactor);
	CheckSum(iSum, m_iSwitchReligionCostFactor);
	CheckSum(iSum, m_iPlayerAnarchyCounter);
	CheckSum(iSum, m_iCounterespionageNumTurns);
	CheckSum(iSum, m_iCounterespionageMod);
	CheckSum(iSum, m_iDifficultyMod);
}

