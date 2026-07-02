//------------------------------------------------------------------------------------------------
//  FILE:    CvBuildInfo.cpp
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
#include "CvBuildInfo.h"


//======================================================================================================
//					CvBuildInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvBuildInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvBuildInfo::CvBuildInfo() :
m_iTime(0),
m_iCost(0),
m_iTechPrereq(NO_TECH),
m_iImprovement(NO_IMPROVEMENT),
m_iRoute(NO_ROUTE),
m_iEntityEvent(ENTITY_EVENT_NONE),
m_iMissionType(NO_MISSION),
m_bKill(false),
m_paiFeatureTech(NULL),
m_paiFeatureTime(NULL),
m_paiFeatureProduction(NULL),
m_pabFeatureRemove(NULL)

,m_iTerrainChange(NO_TERRAIN)
,m_iFeatureChange(NO_FEATURE)
,m_iObsoleteTech(NO_TECH)
,m_bDisabled(false)
{
	CvInfoUtil(this).initDataMembers();
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvBuildInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvBuildInfo::~CvBuildInfo()
{
	PROFILE_EXTRA_FUNC();
	CvInfoUtil(this).uninitDataMembers();

	SAFE_DELETE_ARRAY(m_paiFeatureTech);
	SAFE_DELETE_ARRAY(m_paiFeatureTime);
	SAFE_DELETE_ARRAY(m_paiFeatureProduction);
	SAFE_DELETE_ARRAY(m_pabFeatureRemove);

	for (int i=0; i<(int)m_aiPrereqBonusTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aiPrereqBonusTypes[i]));
	}

	for (int i=0; i<(int)m_aTerrainStructs.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aTerrainStructs[i]));
	}

	for (int i=0; i<(int)m_aPlaceBonusTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aPlaceBonusTypes[i]));
	}
}


void CvBuildInfo::getDataMembers(CvInfoUtil& util)
{
	util
		//.addEnum(m_iObsoleteTech, L"ObsoleteTech")
		//.add(m_piBonusHealthChanges, L"BonusHealthChanges")
	;
}


int CvBuildInfo::getTime() const
{
	return m_iTime;
}


int CvBuildInfo::getCost() const
{
	return m_iCost;
}


int CvBuildInfo::getRoute() const
{
	return m_iRoute;
}


int CvBuildInfo::getTerrainChange() const
{
	return m_iTerrainChange;
}


int CvBuildInfo::getFeatureChange() const
{
	return m_iFeatureChange;
}


TechTypes CvBuildInfo::getObsoleteTech() const
{
	return m_iObsoleteTech;
}


bool CvBuildInfo::isDisabled() const
{
	return m_bDisabled;
}

void CvBuildInfo::setDisabled(bool bNewVal)
{
	m_bDisabled = bNewVal;
}


int CvBuildInfo::getEntityEvent() const
{
	return m_iEntityEvent;
}


int CvBuildInfo::getMissionType() const
{
	return m_iMissionType;
}


void CvBuildInfo::setMissionType(int iNewType)
{
	m_iMissionType = iNewType;
}


bool CvBuildInfo::isKill() const
{
	return m_bKill;
}


// Arrays

TechTypes CvBuildInfo::getFeatureTech(FeatureTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), e);
	return m_paiFeatureTech ? (TechTypes)m_paiFeatureTech[e] : NO_TECH;
}


int CvBuildInfo::getFeatureTime(FeatureTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), e);
	return m_paiFeatureTime ? m_paiFeatureTime[e] : 0;
}


int CvBuildInfo::getFeatureProduction(FeatureTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), e);
	return m_paiFeatureProduction ? m_paiFeatureProduction[e] : 0;
}


bool CvBuildInfo::isFeatureRemove(FeatureTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), e);
	return m_pabFeatureRemove ? m_pabFeatureRemove[e] : false;
}


int CvBuildInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}


int CvBuildInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}


bool CvBuildInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}


bool CvBuildInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	CvString szTextVal2;
	CvString szTextVal3;
	CvString szTextVal4;
	CvString szTextVal5;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	CvInfoUtil(this).readXml(pXML);

	pXML->GetOptionalTypeEnum(m_iTechPrereq, L"PrereqTech");
	pXML->GetOptionalChildXmlValByName(&m_iTime, L"iTime");
	pXML->GetOptionalChildXmlValByName(&m_iCost, L"iCost");
	pXML->GetOptionalChildXmlValByName(&m_bKill, L"bKill");
	pXML->GetOptionalTypeEnum(m_iObsoleteTech, L"ObsoleteTech");
	pXML->GetOptionalTypeEnum(m_iImprovement, L"ImprovementType");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"RouteType");
	m_iRoute = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TerrainChange");
	m_iTerrainChange = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"FeatureChange");
	m_iFeatureChange = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"EntityEvent");
	m_iEntityEvent = pXML->GetInfoClass(szTextVal);

	pXML->SetFeatureStruct(&m_paiFeatureTech, &m_paiFeatureTime, &m_paiFeatureProduction, &m_pabFeatureRemove);

	pXML->SetOptionalVectorWithDelayedResolution(m_aiPrereqBonusTypes, L"PrereqBonusTypes");
	pXML->SetOptionalVector(&m_aeMapCategoryTypes, L"MapCategoryTypes");

	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	if(pXML->TryMoveToXmlFirstChild(L"TerrainStructs"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"TerrainStruct" );
		m_aTerrainStructs.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"TerrainStruct"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"TerrainType");
					m_aTerrainStructs[i].eTerrain = (TerrainTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"PrereqTech");
					m_aTerrainStructs[i].ePrereqTech = (TechTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aTerrainStructs[i].iTime), L"iTime");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"TerrainStruct"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"PlaceBonusTypes"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"PlaceBonusType" );
		m_aPlaceBonusTypes.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"PlaceBonusType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"BonusType");
					m_aPlaceBonusTypes[i].eBonus = (BonusTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aPlaceBonusTypes[i].iProbability), L"iProbability");
					pXML->GetChildXmlValByName(&(m_aPlaceBonusTypes[i].bRequiresAccess), L"bRequiresAccess");
					pXML->GetChildXmlValByName(szTextVal, L"TerrainType");
					m_aPlaceBonusTypes[i].ePrereqTerrain = (TerrainTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"FeatureType");
					m_aPlaceBonusTypes[i].ePrereqFeature = (FeatureTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"MapCategoryType");
					m_aPlaceBonusTypes[i].ePrereqMapCategory = (MapCategoryTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"TechType");
					m_aPlaceBonusTypes[i].ePrereqTech = (TechTypes)pXML->GetInfoClass(szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PlaceBonusType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	return true;
}


void CvBuildInfo::copyNonDefaults(const CvBuildInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default

	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	CvInfoUtil(this).copyNonDefaults(pClassInfo);

	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();
	if (getTime() == iDefault) m_iTime = pClassInfo->getTime();
	if (getCost() == iDefault) m_iCost = pClassInfo->getCost();
	if (isKill() == bDefault) m_bKill = pClassInfo->isKill();
	if (getImprovement() == iTextDefault) m_iImprovement = pClassInfo->getImprovement();
	if (getRoute() == iTextDefault) m_iRoute = pClassInfo->getRoute();
	if (getTerrainChange() == iTextDefault) m_iTerrainChange = pClassInfo->getTerrainChange();
	if (getFeatureChange() == iTextDefault) m_iFeatureChange = pClassInfo->getFeatureChange();
	if (getObsoleteTech() == iTextDefault) m_iObsoleteTech = pClassInfo->getObsoleteTech();
	if (getEntityEvent() == iTextDefault) m_iEntityEvent = pClassInfo->getEntityEvent();

	for ( int i = 0; i < GC.getNumFeatureInfos(); i++)
	{
		if ( m_paiFeatureTech[i] == iTextDefault )
		{
			m_paiFeatureTech[i] = pClassInfo->getFeatureTech((FeatureTypes)i);
			m_paiFeatureTime[i] = pClassInfo->getFeatureTime((FeatureTypes)i);
			m_paiFeatureProduction[i] = pClassInfo->getFeatureProduction((FeatureTypes)i);
			m_pabFeatureRemove[i] = pClassInfo->isFeatureRemove((FeatureTypes)i);
		}
	}
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqBonusTypes, pClassInfo->m_aiPrereqBonusTypes);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aeMapCategoryTypes, pClassInfo->getMapCategories());
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aTerrainStructs, pClassInfo->m_aTerrainStructs);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aPlaceBonusTypes, pClassInfo->m_aPlaceBonusTypes);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

}


void CvBuildInfo::getCheckSum(uint32_t &iSum) const
{
	PROFILE_EXTRA_FUNC();
	CvInfoUtil(this).checkSum(iSum);

	CheckSum(iSum, m_iTime);
	CheckSum(iSum, m_iCost);
	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iImprovement);
	CheckSum(iSum, m_iRoute);

	CheckSum(iSum, m_iTerrainChange);
	CheckSum(iSum, m_iFeatureChange);
	CheckSum(iSum, m_iObsoleteTech);
	CheckSum(iSum, m_bDisabled);

	CheckSum(iSum, m_iEntityEvent);
	CheckSum(iSum, m_iMissionType);

	CheckSum(iSum, m_bKill);

	// Arrays

	CheckSum(iSum, m_paiFeatureTech,  GC.getNumFeatureInfos());
	CheckSum(iSum, m_paiFeatureTime,  GC.getNumFeatureInfos());
	CheckSum(iSum, m_paiFeatureProduction,  GC.getNumFeatureInfos());
	CheckSum(iSum, m_pabFeatureRemove,  GC.getNumFeatureInfos());

	//Vectors

	CheckSumC(iSum, m_aiPrereqBonusTypes);
	CheckSumC(iSum, m_aeMapCategoryTypes);
	CheckSumC(iSum, m_aiCategories);

	int iNumElements = m_aTerrainStructs.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aTerrainStructs[i].eTerrain);
		CheckSum(iSum, m_aTerrainStructs[i].ePrereqTech);
		CheckSum(iSum, m_aTerrainStructs[i].iTime);
	}

	iNumElements = m_aPlaceBonusTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aPlaceBonusTypes[i].eBonus);
		CheckSum(iSum, m_aPlaceBonusTypes[i].iProbability);
		CheckSum(iSum, m_aPlaceBonusTypes[i].bRequiresAccess);
		CheckSum(iSum, m_aPlaceBonusTypes[i].ePrereqTerrain);
		CheckSum(iSum, m_aPlaceBonusTypes[i].ePrereqFeature);
		CheckSum(iSum, m_aPlaceBonusTypes[i].ePrereqMapCategory);
		CheckSum(iSum, m_aPlaceBonusTypes[i].ePrereqTech);
	}
}


void CvBuildInfo::doPostLoadCaching(uint32_t iThis)
{
}

