//------------------------------------------------------------------------------------------------
//  FILE:    CvProjectInfo.cpp
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
#include "CvProjectInfo.h"


//======================================================================================================
//					CvProjectInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvProjectInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvProjectInfo::CvProjectInfo() :
	m_iVictoryPrereq(NO_VICTORY),
	m_iTechPrereq(NO_TECH),
	m_iAnyoneProjectPrereq(NO_PROJECT),
	m_iMaxGlobalInstances(-1),
	m_iMaxTeamInstances(-1),
	m_iProductionCost(0),
	m_iNukeInterception(0),
	m_iTechShare(0),

	m_iGlobalMaintenanceModifier(0),
	m_iDistanceMaintenanceModifier(0),
	m_iNumCitiesMaintenanceModifier(0),
	m_iConnectedCityMaintenanceModifier(0),

	m_iEveryoneSpecialUnit(NO_SPECIALUNIT),
	m_iEveryoneSpecialBuilding(NO_SPECIALBUILDING),
	m_iVictoryDelayPercent(0),
	m_iSuccessRate(0),
	m_bSpaceship(false),
	m_bAllowsNukes(false),
	m_piBonusProductionModifier(NULL),
	m_piVictoryThreshold(NULL),
	m_piVictoryMinThreshold(NULL),
	m_piProjectsNeeded(NULL)

	,m_iWorldHappiness(0)
	,m_iGlobalHappiness(0)
	,m_iWorldHealth(0)
	,m_iGlobalHealth(0)
	,m_iWorldTradeRoutes(0)
	,m_iInflationModifier(0)
	,m_bTechShareWithHalfCivs(false)
	,m_piCommerceModifier(NULL)
{}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvProjectInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvProjectInfo::~CvProjectInfo()
{
	SAFE_DELETE_ARRAY(m_piBonusProductionModifier);
	SAFE_DELETE_ARRAY(m_piVictoryThreshold);
	SAFE_DELETE_ARRAY(m_piVictoryMinThreshold);
	SAFE_DELETE_ARRAY(m_piProjectsNeeded);

}


int CvProjectInfo::getVictoryPrereq() const
{
	return m_iVictoryPrereq;
}


int CvProjectInfo::getAnyoneProjectPrereq() const
{
	return m_iAnyoneProjectPrereq;
}


void CvProjectInfo::setAnyoneProjectPrereq(int i)
{
	m_iAnyoneProjectPrereq = i;
}


int CvProjectInfo::getMaxGlobalInstances() const
{
	return m_iMaxGlobalInstances;
}


int CvProjectInfo::getMaxTeamInstances() const
{
	return m_iMaxTeamInstances;
}


int CvProjectInfo::getProductionCost() const
{
	return m_iProductionCost;
}


int CvProjectInfo::getNukeInterception() const
{
	return m_iNukeInterception;
}


int CvProjectInfo::getTechShare() const
{
	return m_iTechShare;
}


//DPII < Maintenance Modifiers >
int CvProjectInfo::getGlobalMaintenanceModifier() const
{
	return m_iGlobalMaintenanceModifier;
}


int CvProjectInfo::getDistanceMaintenanceModifier() const
{
	return m_iDistanceMaintenanceModifier;
}


int CvProjectInfo::getNumCitiesMaintenanceModifier() const
{
	return m_iNumCitiesMaintenanceModifier;

}


int CvProjectInfo::getConnectedCityMaintenanceModifier() const
{
	return m_iConnectedCityMaintenanceModifier;
}

//DPII < Maintenance Modifiers >

int CvProjectInfo::getEveryoneSpecialUnit() const
{
	return m_iEveryoneSpecialUnit;
}


int CvProjectInfo::getEveryoneSpecialBuilding() const
{
	return m_iEveryoneSpecialBuilding;
}


int CvProjectInfo::getVictoryDelayPercent() const
{
	return m_iVictoryDelayPercent;
}


int CvProjectInfo::getSuccessRate() const
{
	return m_iSuccessRate;
}


bool CvProjectInfo::isSpaceship() const
{
	return m_bSpaceship;
}


bool CvProjectInfo::isAllowsNukes() const
{
	return m_bAllowsNukes;
}


const char* CvProjectInfo::getMovieArtDef() const
{
	return m_szMovieArtDef;
}


const char* CvProjectInfo::getCreateSound() const
{
	return m_szCreateSound;
}


void CvProjectInfo::setCreateSound(const char* szVal)
{
	m_szCreateSound = szVal;
}


// Arrays

int CvProjectInfo::getBonusProductionModifier(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i);
	return m_piBonusProductionModifier ? m_piBonusProductionModifier[i] : 0;
}


int CvProjectInfo::getVictoryThreshold(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumVictoryInfos(), i);
	return m_piVictoryThreshold ? m_piVictoryThreshold[i] : 0;
}


int CvProjectInfo::getVictoryMinThreshold(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumVictoryInfos(), i);

	if (m_piVictoryMinThreshold && m_piVictoryMinThreshold[i] != 0)
	{
		return m_piVictoryMinThreshold[i];
	}
	return getVictoryThreshold(i);
}


int CvProjectInfo::getProjectsNeeded(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), i);
	return m_piProjectsNeeded ? m_piProjectsNeeded[i] : false;
}


int CvProjectInfo::getWorldHappiness() const
{
	return m_iWorldHappiness;
}


int CvProjectInfo::getGlobalHappiness() const
{
	return m_iGlobalHappiness;
}


int CvProjectInfo::getWorldHealth() const
{
	return m_iWorldHealth;
}


int CvProjectInfo::getGlobalHealth() const
{
	return m_iGlobalHealth;
}


int CvProjectInfo::getWorldTradeRoutes() const
{
	return m_iWorldTradeRoutes;
}


int CvProjectInfo::getInflationModifier() const
{
	return m_iInflationModifier;
}


bool CvProjectInfo::isTechShareWithHalfCivs() const
{
	return m_bTechShareWithHalfCivs;
}


int CvProjectInfo::getCommerceModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_piCommerceModifier ? m_piCommerceModifier[i] : 0;
}


int* CvProjectInfo::getCommerceModifierArray() const
{
	return m_piCommerceModifier;
}


int CvProjectInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}


int CvProjectInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}


bool CvProjectInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}


int CvProjectInfo::getProjectsNeededVectorSize() const						{ return m_aszProjectsNeededforPass3.size(); }

CvString CvProjectInfo::getProjectsNeededNamesVectorElement(int i) const	{ return m_aszProjectsNeededforPass3[i]; }

int CvProjectInfo::getProjectsNeededValuesVectorElement(int i) const		{ return m_aiProjectsNeededforPass3[i]; }



bool CvProjectInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"VictoryPrereq");
	m_iVictoryPrereq = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalTypeEnum(m_iTechPrereq, L"TechPrereq");
	pXML->GetOptionalChildXmlValByName(&m_iMaxGlobalInstances, L"iMaxGlobalInstances", -1);
	pXML->GetOptionalChildXmlValByName(&m_iMaxTeamInstances, L"iMaxTeamInstances", -1);
	pXML->GetOptionalChildXmlValByName(&m_iProductionCost, L"iCost");
	pXML->GetOptionalChildXmlValByName(&m_iNukeInterception, L"iNukeInterception");
	pXML->GetOptionalChildXmlValByName(&m_iTechShare, L"iTechShare");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalMaintenanceModifier, L"iGlobalMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iDistanceMaintenanceModifier, L"iDistanceMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesMaintenanceModifier, L"iNumCitiesMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iConnectedCityMaintenanceModifier, L"iConnectedCityMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"EveryoneSpecialUnit");
	m_iEveryoneSpecialUnit = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"EveryoneSpecialBuilding");
	m_iEveryoneSpecialBuilding = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bSpaceship, L"bSpaceship");
	pXML->GetOptionalChildXmlValByName(&m_bAllowsNukes, L"bAllowsNukes");
	pXML->GetOptionalChildXmlValByName(m_szMovieArtDef, L"MovieDefineTag");

	pXML->SetVariableListTagPair(&m_piBonusProductionModifier, L"BonusProductionModifiers", GC.getNumBonusInfos());
	pXML->SetVariableListTagPair(&m_piVictoryThreshold, L"VictoryThresholds",  GC.getNumVictoryInfos());
	pXML->SetVariableListTagPair(&m_piVictoryMinThreshold, L"VictoryMinThresholds",  GC.getNumVictoryInfos());
	pXML->GetOptionalChildXmlValByName(&m_iVictoryDelayPercent, L"iVictoryDelayPercent");
	pXML->GetOptionalChildXmlValByName(&m_iSuccessRate, L"iSuccessRate");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CreateSound");
	setCreateSound(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iWorldHappiness, L"iWorldHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalHappiness, L"iGlobalHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iWorldHealth, L"iWorldHealth");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalHealth, L"iGlobalHealth");
	pXML->GetOptionalChildXmlValByName(&m_iWorldTradeRoutes, L"iWorldTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_bTechShareWithHalfCivs, L"bTechShareWithHalfCivs");
	pXML->GetOptionalChildXmlValByName(&m_iInflationModifier, L"iInflationModifier");

	if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommerceModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"PrereqProjects"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();
		int iTemp = 0;
		if (iNumSibs > 0)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int i=0;i<iNumSibs;i++)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						m_aszProjectsNeededforPass3.push_back(szTextVal);
						pXML->GetNextXmlVal(&iTemp);
						m_aiProjectsNeededforPass3.push_back(iTemp);
						pXML->MoveToXmlParent();
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}

	pXML->SetOptionalVector(&m_aiCategories, L"Categories");
	pXML->SetOptionalVector(&m_aeMapCategoryTypes, L"MapCategoryTypes");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"AnyonePrereqProject");
	m_aszExtraXMLforPass3.push_back(szTextVal);

	return true;
}



void CvProjectInfo::copyNonDefaults(const CvProjectInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getVictoryPrereq() == iTextDefault) m_iVictoryPrereq = pClassInfo->getVictoryPrereq();
	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();
	if (getEveryoneSpecialUnit() == iTextDefault) m_iEveryoneSpecialUnit = pClassInfo->getEveryoneSpecialUnit();
	if (getEveryoneSpecialBuilding() == iTextDefault) m_iEveryoneSpecialBuilding = pClassInfo->getEveryoneSpecialBuilding();

	if (getMaxGlobalInstances() == -1) m_iMaxGlobalInstances = pClassInfo->getMaxGlobalInstances();
	if (getMaxTeamInstances() == -1) m_iMaxTeamInstances = pClassInfo->getMaxTeamInstances();
	if (getProductionCost() == iDefault) m_iProductionCost = pClassInfo->getProductionCost();
	if (getNukeInterception() == iDefault) m_iNukeInterception = pClassInfo->getNukeInterception();
	if (getTechShare() == iDefault) m_iTechShare = pClassInfo->getTechShare();

	if (isSpaceship() == bDefault) m_bSpaceship = pClassInfo->isSpaceship();
	if (isAllowsNukes() == bDefault) m_bAllowsNukes = pClassInfo->isAllowsNukes();

	if (getMovieArtDef() == cDefault) m_szMovieArtDef = pClassInfo->getMovieArtDef();

	for ( int i = 0; i < GC.getNumBonusInfos(); i++)
	{
		if (getBonusProductionModifier(i) == iDefault && pClassInfo->getBonusProductionModifier(i) != iDefault)
		{
			if ( NULL == m_piBonusProductionModifier )
			{
				CvXMLLoadUtility::InitList(&m_piBonusProductionModifier,GC.getNumBonusInfos(),iDefault);
			}
			m_piBonusProductionModifier[i] = pClassInfo->getBonusProductionModifier(i);
		}
	}
	for ( int i = 0; i < GC.getNumVictoryInfos(); i++)
	{
		if (getVictoryThreshold(i) == iDefault && pClassInfo->getVictoryThreshold(i) != iDefault)
		{
			if ( NULL == m_piVictoryThreshold )
			{
				CvXMLLoadUtility::InitList(&m_piVictoryThreshold,GC.getNumVictoryInfos(),iDefault);
			}
			m_piVictoryThreshold[i] = pClassInfo->getVictoryThreshold(i);
		}
		if (getVictoryMinThreshold(i) == iDefault && pClassInfo->getVictoryMinThreshold(i) != iDefault)
		{
			if ( NULL == m_piVictoryMinThreshold )
			{
				CvXMLLoadUtility::InitList(&m_piVictoryMinThreshold,GC.getNumVictoryInfos(),iDefault);
			}
			m_piVictoryMinThreshold[i] = pClassInfo->getVictoryMinThreshold(i);
		}
	}
	if (getVictoryDelayPercent() == iDefault) m_iVictoryDelayPercent = pClassInfo->getVictoryDelayPercent();
	if (getSuccessRate() == iDefault) m_iSuccessRate = pClassInfo->getSuccessRate();

	if (getCreateSound() == cDefault) setCreateSound(pClassInfo->getCreateSound());


	if (m_iAnyoneProjectPrereq == iTextDefault) m_iAnyoneProjectPrereq = pClassInfo->getAnyoneProjectPrereq();

	if (getWorldHappiness() == iDefault) m_iWorldHappiness = pClassInfo->getWorldHappiness();
	if (getGlobalHappiness() == iDefault) m_iGlobalHappiness= pClassInfo->getGlobalHappiness();
	if (getWorldHealth() == iDefault) m_iWorldHealth = pClassInfo->getWorldHealth();
	if (getGlobalHealth() == iDefault) m_iGlobalHealth = pClassInfo->getGlobalHealth();
	if (isTechShareWithHalfCivs() == bDefault) m_bTechShareWithHalfCivs = pClassInfo->isTechShareWithHalfCivs();
	if (getWorldTradeRoutes() == iDefault) m_iWorldTradeRoutes = pClassInfo->getWorldTradeRoutes();
	if (getInflationModifier() == iDefault) m_iInflationModifier = pClassInfo->getInflationModifier();

	if (!m_iGlobalMaintenanceModifier) m_iGlobalMaintenanceModifier = pClassInfo->getGlobalMaintenanceModifier();
	if (!m_iDistanceMaintenanceModifier) m_iDistanceMaintenanceModifier = pClassInfo->getDistanceMaintenanceModifier();
	if (!m_iNumCitiesMaintenanceModifier) m_iNumCitiesMaintenanceModifier = pClassInfo->getNumCitiesMaintenanceModifier();
	if (!m_iConnectedCityMaintenanceModifier) m_iConnectedCityMaintenanceModifier = pClassInfo->getConnectedCityMaintenanceModifier();

	for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
	{
		if ( m_piCommerceModifier[j] == iDefault )
		{
			m_piCommerceModifier[j] = pClassInfo->getCommerceModifier(j);
		}
	}

	for ( int i = 0; i < pClassInfo->getProjectsNeededVectorSize(); i++ )
	{
		m_aiProjectsNeededforPass3.push_back(pClassInfo->getProjectsNeededValuesVectorElement(i));
		m_aszProjectsNeededforPass3.push_back(pClassInfo->getProjectsNeededNamesVectorElement(i));
	}

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aeMapCategoryTypes, pClassInfo->getMapCategories());
}


bool CvProjectInfo::readPass3()
{
	PROFILE_EXTRA_FUNC();
	m_piProjectsNeeded = new int[GC.getNumProjectInfos()];
	for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		m_piProjectsNeeded[iI] = 0;
	}
	if (!m_aiProjectsNeededforPass3.empty() && !m_aszProjectsNeededforPass3.empty())
	{
		int iNumLoad = m_aiProjectsNeededforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			int iTempIndex = GC.getInfoTypeForString(m_aszProjectsNeededforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumEventInfos())
				m_piProjectsNeeded[iTempIndex] = m_aiProjectsNeededforPass3[iI];
		}
		m_aszProjectsNeededforPass3.clear();
		m_aiProjectsNeededforPass3.clear();
	}

	if (m_aszExtraXMLforPass3.size() < 1)
	{
		FErrorMsg("error");
		return false;
	}
	m_iAnyoneProjectPrereq = GC.getInfoTypeForString(m_aszExtraXMLforPass3[0]);

	return true;
}



void CvProjectInfo::getCheckSum(uint32_t &iSum) const
{
	CheckSum(iSum, m_iWorldHappiness);
	CheckSum(iSum, m_iGlobalHappiness);
	CheckSum(iSum, m_iWorldHealth);
	CheckSum(iSum, m_iGlobalHealth);
	CheckSum(iSum, m_iWorldTradeRoutes);
	CheckSum(iSum, m_iInflationModifier);
	CheckSum(iSum, m_bTechShareWithHalfCivs);

	CheckSum(iSum, m_piCommerceModifier, NUM_COMMERCE_TYPES);

	CheckSum(iSum, m_iVictoryPrereq);
	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iAnyoneProjectPrereq);
	CheckSum(iSum, m_iMaxGlobalInstances);
	CheckSum(iSum, m_iMaxTeamInstances);
	CheckSum(iSum, m_iProductionCost);
	CheckSum(iSum, m_iNukeInterception);
	CheckSum(iSum, m_iTechShare);

	CheckSum(iSum, m_iGlobalMaintenanceModifier);
	CheckSum(iSum, m_iDistanceMaintenanceModifier);
	CheckSum(iSum, m_iNumCitiesMaintenanceModifier);
	CheckSum(iSum, m_iConnectedCityMaintenanceModifier);

	CheckSum(iSum, m_iEveryoneSpecialUnit);
	CheckSum(iSum, m_iEveryoneSpecialBuilding);
	CheckSum(iSum, m_iVictoryDelayPercent);
	CheckSum(iSum, m_iSuccessRate);

	CheckSum(iSum, m_bSpaceship);
	CheckSum(iSum, m_bAllowsNukes);

	// Arrays

	CheckSum(iSum, m_piBonusProductionModifier, GC.getNumBonusInfos());
	CheckSum(iSum, m_piVictoryThreshold, GC.getNumVictoryInfos());
	CheckSum(iSum, m_piVictoryMinThreshold, GC.getNumVictoryInfos());
	CheckSum(iSum, m_piProjectsNeeded, GC.getNumProjectInfos());

	// Vectors

	CheckSumC(iSum, m_aiCategories);
	CheckSumC(iSum, m_aeMapCategoryTypes);
}

