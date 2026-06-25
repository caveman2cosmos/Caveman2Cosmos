//------------------------------------------------------------------------------------------------
//  FILE:    CvWorldInfo.cpp
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
#include "CvWorldInfo.h"



//======================================================================================================
//					CvWorldInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvWorldInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvWorldInfo::CvWorldInfo() :
 m_iDefaultPlayers(0)
,m_iUnitNameModifier(0)
,m_iTargetNumCities(0)
,m_iBuildingPrereqModifier(0)
,m_iMaxConscriptModifier(0)
,m_iWarWearinessModifier(0)
,m_iGridWidth(0)
,m_iGridHeight(0)
,m_iTerrainGrainChange(0)
,m_iFeatureGrainChange(0)
,m_iTradeProfitPercent(0)
,m_iDistanceMaintenancePercent(0)
,m_iNumCitiesMaintenancePercent(0)
,m_iColonyMaintenancePercent(0)
,m_iCorporationMaintenancePercent(0)
,m_iNumCitiesAnarchyPercent(0)
,m_iAdvancedStartPointsMod(0)
,m_iCommandersLevelThresholdsPercent(0)
,m_iOceanMinAreaSize(0)
{ }


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvWorldInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvWorldInfo::~CvWorldInfo()
{
}


int CvWorldInfo::getDefaultPlayers() const
{
	return m_iDefaultPlayers;
}


int CvWorldInfo::getUnitNameModifier() const
{
	return m_iUnitNameModifier;
}


int CvWorldInfo::getTargetNumCities() const
{
	return m_iTargetNumCities;
}


int CvWorldInfo::getBuildingPrereqModifier() const
{
	return m_iBuildingPrereqModifier;
}


int CvWorldInfo::getMaxConscriptModifier() const
{
	return m_iMaxConscriptModifier;
}


int CvWorldInfo::getWarWearinessModifier() const
{
	return m_iWarWearinessModifier;
}


int CvWorldInfo::getGridWidth() const
{
	return m_iGridWidth;
}


int CvWorldInfo::getGridHeight() const
{
	return m_iGridHeight;
}


int CvWorldInfo::getTerrainGrainChange() const
{
	return m_iTerrainGrainChange;
}


int CvWorldInfo::getFeatureGrainChange() const
{
	return m_iFeatureGrainChange;
}


int CvWorldInfo::getTradeProfitPercent() const
{
	return m_iTradeProfitPercent;
}


int CvWorldInfo::getDistanceMaintenancePercent() const
{
	return m_iDistanceMaintenancePercent;
}


int CvWorldInfo::getNumCitiesMaintenancePercent() const
{
	return m_iNumCitiesMaintenancePercent;
}


int CvWorldInfo::getColonyMaintenancePercent() const
{
	return m_iColonyMaintenancePercent;
}


int CvWorldInfo::getCorporationMaintenancePercent() const
{
	return m_iCorporationMaintenancePercent;
}


int CvWorldInfo::getNumCitiesAnarchyPercent() const
{
	return m_iNumCitiesAnarchyPercent;
}


int CvWorldInfo::getAdvancedStartPointsMod() const
{
	return m_iAdvancedStartPointsMod;
}


int CvWorldInfo::getCommandersLevelThresholdsPercent() const
{
	return m_iCommandersLevelThresholdsPercent;
}


int CvWorldInfo::getOceanMinAreaSize() const
{
	return m_iOceanMinAreaSize;
}


int CvWorldInfo::getPercent(int iID) const
{
	return m_Percent.getValue(iID);
}


bool CvWorldInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iDefaultPlayers, L"iDefaultPlayers");
	pXML->GetOptionalChildXmlValByName(&m_iUnitNameModifier, L"iUnitNameModifier");
	pXML->GetOptionalChildXmlValByName(&m_iTargetNumCities, L"iTargetNumCities");
	pXML->GetOptionalChildXmlValByName(&m_iBuildingPrereqModifier, L"iBuildingPrereqModifier");
	pXML->GetOptionalChildXmlValByName(&m_iMaxConscriptModifier, L"iMaxConscriptModifier");
	pXML->GetOptionalChildXmlValByName(&m_iWarWearinessModifier, L"iWarWearinessModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGridWidth, L"iGridWidth");
	pXML->GetOptionalChildXmlValByName(&m_iGridHeight, L"iGridHeight");
	pXML->GetOptionalChildXmlValByName(&m_iTerrainGrainChange, L"iTerrainGrainChange");
	pXML->GetOptionalChildXmlValByName(&m_iFeatureGrainChange, L"iFeatureGrainChange");
	pXML->GetOptionalChildXmlValByName(&m_iTradeProfitPercent, L"iTradeProfitPercent");
	pXML->GetOptionalChildXmlValByName(&m_iDistanceMaintenancePercent, L"iDistanceMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesMaintenancePercent, L"iNumCitiesMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iColonyMaintenancePercent, L"iColonyMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iCorporationMaintenancePercent, L"iCorporationMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesAnarchyPercent, L"iNumCitiesAnarchyPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartPointsMod, L"iAdvancedStartPointsMod");
	pXML->GetOptionalChildXmlValByName(&m_iCommandersLevelThresholdsPercent, L"iCommandersLevelThresholdsPercent");
	pXML->GetOptionalChildXmlValByName(&m_iOceanMinAreaSize, L"iOceanMinAreaSize");

	m_Percent.read(pXML, L"Percents");

	return true;
}


void CvWorldInfo::copyNonDefaults(const CvWorldInfo* pClassInfo)
{
	int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getDefaultPlayers() == iDefault) m_iDefaultPlayers = pClassInfo->getDefaultPlayers();
	if (getUnitNameModifier() == iDefault) m_iUnitNameModifier = pClassInfo->getUnitNameModifier();
	if (getTargetNumCities() == iDefault) m_iTargetNumCities = pClassInfo->getTargetNumCities();
	if (getBuildingPrereqModifier() == iDefault) m_iBuildingPrereqModifier = pClassInfo->getBuildingPrereqModifier();
	if (getMaxConscriptModifier() == iDefault) m_iMaxConscriptModifier = pClassInfo->getMaxConscriptModifier();
	if (getWarWearinessModifier() == iDefault) m_iWarWearinessModifier = pClassInfo->getWarWearinessModifier();
	if (getGridWidth() == iDefault) m_iGridWidth = pClassInfo->getGridWidth();
	if (getGridHeight() == iDefault) m_iGridHeight = pClassInfo->getGridHeight();
	if (getTerrainGrainChange() == iDefault) m_iTerrainGrainChange = pClassInfo->getTerrainGrainChange();
	if (getFeatureGrainChange() == iDefault) m_iFeatureGrainChange = pClassInfo->getFeatureGrainChange();
	if (getTradeProfitPercent() == iDefault) m_iTradeProfitPercent = pClassInfo->getTradeProfitPercent();
	if (getDistanceMaintenancePercent() == iDefault) m_iDistanceMaintenancePercent = pClassInfo->getDistanceMaintenancePercent();
	if (getNumCitiesMaintenancePercent() == iDefault) m_iNumCitiesMaintenancePercent = pClassInfo->getNumCitiesMaintenancePercent();
	if (getColonyMaintenancePercent() == iDefault) m_iColonyMaintenancePercent = pClassInfo->getColonyMaintenancePercent();
	if (getCorporationMaintenancePercent() == iDefault) m_iCorporationMaintenancePercent = pClassInfo->getCorporationMaintenancePercent();
	if (getNumCitiesAnarchyPercent() == iDefault) m_iNumCitiesAnarchyPercent = pClassInfo->getNumCitiesAnarchyPercent();
	if (getAdvancedStartPointsMod() == iDefault) m_iAdvancedStartPointsMod = pClassInfo->getAdvancedStartPointsMod();
	if (getCommandersLevelThresholdsPercent() == iDefault) m_iCommandersLevelThresholdsPercent = pClassInfo->getCommandersLevelThresholdsPercent();
	if (m_iOceanMinAreaSize == iDefault) m_iOceanMinAreaSize = pClassInfo->getOceanMinAreaSize();

	m_Percent.copyNonDefaults(pClassInfo->m_Percent);
}


void CvWorldInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iDefaultPlayers);
	CheckSum(iSum, m_iUnitNameModifier);
	CheckSum(iSum, m_iTargetNumCities);
	CheckSum(iSum, m_iBuildingPrereqModifier);
	CheckSum(iSum, m_iMaxConscriptModifier);
	CheckSum(iSum, m_iWarWearinessModifier);
	CheckSum(iSum, m_iGridWidth);
	CheckSum(iSum, m_iGridHeight);
	CheckSum(iSum, m_iTerrainGrainChange);
	CheckSum(iSum, m_iFeatureGrainChange);
	CheckSum(iSum, m_iTradeProfitPercent);
	CheckSum(iSum, m_iDistanceMaintenancePercent);
	CheckSum(iSum, m_iNumCitiesMaintenancePercent);
	CheckSum(iSum, m_iColonyMaintenancePercent);
	CheckSum(iSum, m_iCorporationMaintenancePercent);
	CheckSum(iSum, m_iNumCitiesAnarchyPercent);
	CheckSum(iSum, m_iAdvancedStartPointsMod);
	CheckSum(iSum, m_iCommandersLevelThresholdsPercent);
	CheckSum(iSum, m_iOceanMinAreaSize);
	CheckSumC(iSum, m_Percent);
}

