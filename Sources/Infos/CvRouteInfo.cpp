//------------------------------------------------------------------------------------------------
//  FILE:    CvRouteInfo.cpp
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
#include "CvRouteInfo.h"


//======================================================================================================
//					CvRouteInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvRouteInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvRouteInfo::CvRouteInfo() :
m_iAdvancedStartCost(100),
m_iValue(0),
m_iMovementCost(0),
m_iFlatMovementCost(0),
m_bSeaTunnel(false),
m_iPrereqBonus(NO_BONUS),
m_piYieldChange(NULL),
m_piTechMovementChange(NULL),
m_PropertyManipulators()
{
	m_zobristValue = GC.getGame().getSorenRand().getInt();
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvRouteInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvRouteInfo::~CvRouteInfo()
{
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piTechMovementChange);
}


int CvRouteInfo::getAdvancedStartCost() const
{
	return m_iAdvancedStartCost;
}


int CvRouteInfo::getValue() const
{
	return m_iValue;
}


int CvRouteInfo::getMovementCost() const
{
	return m_iMovementCost;
}


int CvRouteInfo::getFlatMovementCost() const
{
	return m_iFlatMovementCost;
}


bool CvRouteInfo::isSeaTunnel() const
{
	return m_bSeaTunnel;
}


int CvRouteInfo::getPrereqBonus() const
{
	return m_iPrereqBonus;
}


int CvRouteInfo::getYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}


int* CvRouteInfo::getYieldChangeArray() const
{
	return m_piYieldChange;
}


int CvRouteInfo::getTechMovementChange(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), i);
	return m_piTechMovementChange ? m_piTechMovementChange[i] : 0;
}


int CvRouteInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}


int CvRouteInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}


bool CvRouteInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}


const std::vector<BonusTypes>& CvRouteInfo::getPrereqOrBonuses() const
{
	return m_piPrereqOrBonuses;
}


bool CvRouteInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	//shouldHaveType = true;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	if (m_szType.empty())
	{
		OutputDebugStringW(pXML->GetXmlTagName());
		OutputDebugString("\n");
		FErrorMsg("error");
	}
	//shouldHaveType = false;

	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartCost, L"iAdvancedStartCost", 100);

	pXML->GetOptionalChildXmlValByName(&m_iValue, L"iValue");
	pXML->GetOptionalChildXmlValByName(&m_iMovementCost, L"iMovement");
	pXML->GetOptionalChildXmlValByName(&m_iFlatMovementCost, L"iFlatMovement");
	pXML->GetOptionalChildXmlValByName(&m_bSeaTunnel, L"bSeaTunnel");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BonusType");
	m_iPrereqBonus = pXML->GetInfoClass(szTextVal);

	// if we can set the current xml node to it's next sibling
	if (pXML->TryMoveToXmlFirstChild(L"Yields"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piYieldChange);
		// set the current xml node to it's parent node
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldChange);
	}

	pXML->SetVariableListTagPair(&m_piTechMovementChange, L"TechMovementChanges", GC.getNumTechInfos());

	pXML->SetOptionalVector(&m_piPrereqOrBonuses, L"PrereqOrBonuses");

	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	m_PropertyManipulators.read(pXML);

	return true;
}


void CvRouteInfo::copyNonDefaults(const CvRouteInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getAdvancedStartCost() == 100) m_iAdvancedStartCost = pClassInfo->getAdvancedStartCost();
	if (getValue() == iDefault) m_iValue = pClassInfo->getValue();
	if (getMovementCost() == iDefault) m_iMovementCost = pClassInfo->getMovementCost();
	if (getFlatMovementCost() == iDefault) m_iFlatMovementCost = pClassInfo->getFlatMovementCost();
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 07/07/09														*/
/*																							  */
/*																							  */
/************************************************************************************************/
	if (!isSeaTunnel()) m_bSeaTunnel = pClassInfo->isSeaTunnel();
/************************************************************************************************/
/* JOOYO_ADDON						  END													 */
/************************************************************************************************/
	if (getPrereqBonus() == iTextDefault) m_iPrereqBonus = pClassInfo->getPrereqBonus();

	for ( int i = 0;  i < NUM_YIELD_TYPES; i++)
	{
		if (getYieldChange(i) == iDefault && pClassInfo->getYieldChange(i) != iDefault)
		{
			if ( m_piYieldChange == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piYieldChange, NUM_YIELD_TYPES);
			}
			m_piYieldChange[i] = pClassInfo->getYieldChange(i);
		}
	}

	for ( int i = 0;  i < GC.getNumTechInfos(); i++)
	{
		if (getTechMovementChange(i) == iDefault && pClassInfo->getTechMovementChange(i) != iDefault)
		{
			if ( NULL == m_piTechMovementChange )
			{
				CvXMLLoadUtility::InitList(&m_piTechMovementChange,GC.getNumTechInfos(),iDefault);
			}
			m_piTechMovementChange[i] = pClassInfo->getTechMovementChange(i);
		}
	}

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_piPrereqOrBonuses, pClassInfo->getPrereqOrBonuses());

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);
}


void CvRouteInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iAdvancedStartCost);

	CheckSum(iSum, m_iValue);
	CheckSum(iSum, m_iMovementCost);
	CheckSum(iSum, m_iFlatMovementCost);

	CheckSum(iSum, m_bSeaTunnel);
	CheckSum(iSum, m_iPrereqBonus);

	// Arrays

	CheckSum(iSum, m_piYieldChange, NUM_YIELD_TYPES);
	CheckSum(iSum, m_piTechMovementChange, GC.getNumTechInfos());
	CheckSumC(iSum, m_piPrereqOrBonuses);
	CheckSumC(iSum, m_aiCategories);

	m_PropertyManipulators.getCheckSum(iSum);
}

